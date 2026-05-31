# Ricdotlang

Ricdotlang is a simple programming language.

## Types

At the moment only a few types are supported:

- Number
- String
  - Use `""` to start and end a string
- Bolean
  - Use `true` or `false` to create boolean variables.

Other internal types:

- Function
- Nil
- Undefined

## Comments

For writing comments use the `//` prefix.

```
// This is a comment

func hello() {
  return "World"; // This is a comment
}
```

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

## Type methods

You can define methods on existing types using the `::` syntax. The receiver's value is accessible via the `self` keyword.

```
func String::greet() {
  print("hello " .. self);
}

let name = "User";
name.greet();
```

## If/Else statements

You can use `if`, `else if`, and `else` blocks to implement control flow paths.

```
if (x < 10) {
  print("Small");
} else if (x < 20) {
  print("Medium");
} else {
  print("Large");
}
```

## While loops

While loops are also supported using the `while` keyword loops for iteration.
For mutable counter variables, make use of the `$` so that they can be properly reassigned a new counter value.

```
let $counter = 0;

while (counter < 5) {
  print(counter);
  counter = counter + 1;
}
```

## For loops

Implementing for loops is easy with using the `for` keyword and defining a range with `{start}..{end}`.
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


## Return statements

You can return a value from a function using the `return` keyword.

```
func add(a, b) {
  return a + b;
}
```

## String concatenation

You can concatenate strings using the `..` operator.

```
print("Hello" .. " " .. "World");
```

## Unary and Binary expressions

Use `&&` or `||` to implement logical operations.

```
func test() {
  return 12 > 10 && 10 < 9;
}

test(); // false
```

Use `+ - * /` to perform binary and arithmetic operations.

```
print(1 + 1 * 2 + 3); // 6
```

Use `!` to invert the truthyness of a value.

```
print(!false); // true
```

Use `-` to flip a numbers sign.

```
let a = 1;
print(-a); // -1
print(-(-a)) // 1
```