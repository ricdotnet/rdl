import {
    createConnection,
    TextDocuments,
    ProposedFeatures,
    InitializeParams,
    TextDocumentSyncKind,
    InitializeResult,
    DocumentFormattingParams,
    TextEdit,
    Range
} from 'vscode-languageserver/node';

import {
    TextDocument
} from 'vscode-languageserver-textdocument';

// Create a connection for the server, using Node's IPC as a transport.
// Also include all preview / proposed LSP features.
const connection = createConnection(ProposedFeatures.all);

// Create a simple text document manager.
const documents: TextDocuments<TextDocument> = new TextDocuments(TextDocument);

connection.onInitialize((params: InitializeParams) => {
    const result: InitializeResult = {
        capabilities: {
            textDocumentSync: TextDocumentSyncKind.Incremental,
            documentFormattingProvider: true
        }
    };
    return result;
});

// Formatting rules configuration to make it extensible
const formatConfig = {
    openBraces: ['{', '[', '('],
    closeBraces: ['}', ']', ')'],
};

connection.onDocumentFormatting((params: DocumentFormattingParams): TextEdit[] => {
    const document = documents.get(params.textDocument.uri);
    if (!document) {
        return [];
    }

    const text = document.getText();
    const formattedText = formatRdl(text, {
        tabSize: params.options.tabSize,
        insertSpaces: params.options.insertSpaces
    });

    if (formattedText === text) {
        return [];
    }

    return [
        TextEdit.replace(
            Range.create(
                document.positionAt(0),
                document.positionAt(text.length)
            ),
            formattedText
        )
    ];
});

function formatRdl(text: string, options: { tabSize: number, insertSpaces: boolean }): string {
    const lines = text.split(/\r?\n/);
    let currentIndent = 0;
    const result: string[] = [];
    const indentChar = options.insertSpaces ? ' ' : '\t';
    const indentStep = options.insertSpaces ? options.tabSize : 1;

    for (let line of lines) {
        let trimmedLine = line.trim();
        if (trimmedLine.length === 0) {
            result.push('');
            continue;
        }

        // Determine line indent: if it starts with a closing brace, it should be indented one level less
        let lineIndent = currentIndent;
        if (formatConfig.closeBraces.includes(trimmedLine[0])) {
            lineIndent = Math.max(0, lineIndent - 1);
        }

        const indentStr = indentChar.repeat(Math.max(0, lineIndent) * indentStep);
        result.push(indentStr + trimmedLine);

        // Calculate net change for the NEXT line
        let netChange = 0;
        let inString = false;
        for (let i = 0; i < trimmedLine.length; i++) {
            const char = trimmedLine[i];
            if (char === '"' && (i === 0 || trimmedLine[i - 1] !== '\\')) {
                inString = !inString;
                continue;
            }
            if (inString) continue;

            if (char === '/' && trimmedLine[i + 1] === '/') {
                break; // Comment starts, ignore rest of line
            }

            if (formatConfig.openBraces.includes(char)) {
                netChange++;
            } else if (formatConfig.closeBraces.includes(char)) {
                netChange--;
            }
        }
        
        currentIndent += netChange;
        if (currentIndent < 0) currentIndent = 0;
    }

    return result.join('\n');
}

// Make the text document manager listen on the connection
// for open, change and close text document events
documents.listen(connection);

// Listen on the connection
connection.listen();
