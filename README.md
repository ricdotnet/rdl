# Ricdotlang

Ricdotlang is a simple programming language.

## Variables

Variables are declared using the `let` keyword. By default, variables are immutable. 
To make a variable mutable, use the `$` prefix in its declaration.

```
let x = 10;      // Immutable variable
let $y = 0;      // Mutable variable

y = y + 1;       // Allowed for mutable variables
```

## Functions

Functions are defined using the `func` keyword followed by the function name and parentheses.

```
func greet() {
  print("Hello, world!");
}

greet();
```

You can also use global variables inside functions.

## String Concatenation

You can concatenate strings using the `..` operator.

```
print("Hello" .. " " .. "World");
```

## While Loops

Ricdotlang supports `while` loops for iteration.

```
let $counter = 0;

while (counter < 5) {
  print(counter);
  counter = counter + 1;
}
```
