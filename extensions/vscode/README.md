# RDL VSCode Extension

Language support for the RDL programming language.

## Features

- **Syntax Highlighting**: Comprehensive highlighting for keywords, operators, builtins, and types.
- **Code Formatting**: Automatic indentation and formatting via LSP.

## Extension Structure

- `package.json`: Main extension manifest.
- `language-configuration.json`: Editor settings for RDL (brackets, comments).
- `syntaxes/rdl.tmLanguage.json`: TextMate grammar for syntax highlighting.
- `src/extension.ts`: Client-side extension entry point.
- `src/server.ts`: Language Server implementation (LSP).

## Extending the Extension

### Adding New Keywords or Tokens
To add new keywords, update both:
1. `syntaxes/rdl.tmLanguage.json`: Add the keyword to the `keywords` or `builtins` pattern.
2. `src/server.ts`: If it affects formatting, add it to the `formatConfig.keywords` array.

### Adding New Formatting Rules
Formatting logic is located in `src/server.ts` within the `formatRdl` function. You can adjust:
- `formatConfig.openBraces` and `formatConfig.closeBraces`: To change which characters trigger indentation changes.
- `formatConfig.indentSize`: Default indentation size.

## Development

1. Run `npm install` in this directory.
2. Run `npm run compile` to build the extension.
3. Open this folder in VS Code and press `F5` to test.
