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

## Type Methods

You can define methods on existing types using the `::` syntax. The receiver is accessible via the `self` keyword.

```
func Number::greet() {
  print("hello " .. self);
}

let name = 10;
name.greet();
```

## Control Flow

### If/Else Statements

Ricdotlang supports `if`, `else if`, and `else` blocks.

```
if (x < 10) {
  print("Small");
} else if (x < 20) {
  print("Medium");
} else {
  print("Large");
}
```

### While Loops

Ricdotlang supports `while` loops for iteration.

```
let $counter = 0;

while (counter < 5) {
  print(counter);
  counter = counter + 1;
}
```

### For Loops

Ricdotlang supports `for` loops for iteration.

For loops must use the `$` operator so that the variable is mutable.

```
for $i in 0..5 {
  print(i);
}
```

```
// For loop with a step value
for $i in 0..10, 2 {
  print(i);
}
```

## Return

You can return a value from a function using the `return` keyword.

```
func add(a, b) {
  return a + b;
}
```

## String Concatenation

You can concatenate strings using the `..` operator.

```
print("Hello" .. " " .. "World");
```
