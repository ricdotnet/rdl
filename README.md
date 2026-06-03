# Ricdotlang

Ricdotlang is a simple programming language.

## Types

At the moment only a few data types are supported:

- Number
- String
    - Use `""` to start and end a string
- Boolean
    - Use `true` or `false` to create boolean variables.
- Objects
- Arrays

Other internal data types:

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

Variables are declared using the `let` keyword. By default, variables are immutable.<br>
To make a variable mutable, use the `$` prefix in its declaration.

```
let x = 10;      // Immutable variable
let $y = 0;      // Mutable variable

y = y + 1;       // Allowed for mutable variables
```

## Objects

Objects are defined using the `{}` format with key-value pairs separated by a comma.<br>
The key is always a string, and the value can be any type.<br>
Keys can be accessed using the `.` operator (dot notation).

```
let user = {
  name: "User",
  age: 25,
}

print(user.name);
```

## Arrays

Arrays are defined using the `[...]Type` format, followed by the type of the elements.<br>
The application will exit if the type of the elements is not supported.

(Only String and Number are supported currently).

```
let numbers = [1, 2, 3, 4, 5]Number;

print(numbers[0]); // Accessing the first element
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

```
let name = "User";
let $x = 10;

func add(a, b) {
  print(name);
  return a + b;
}

let result = add(x, 5);
print(result);
```

## Type methods

You can define methods on existing types using the `::` syntax. The receiver's value is accessible via the `self`
keyword.

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

While loops are also supported using the `while` keyword loops for iteration.<br>
For mutable iterator variables, make use of the `$` so that they can be properly reassigned a new iterator value.

```
let $i = 0;

while (i < 5) {
  print(i);
  i = i + 1;
}
```

## For loops

Implementing for loops is easy with using the `for` keyword and defining a range with `{start}..{end}`.<br>
For loops must use the `$` operator so that the variable is mutable.

```
for $i in 0..5 {
  println(i);
}
```

```
// For loop with a step value
for $i in 0..10, 2 {
  println(i);
}
```

For loops can also be used with arrays, just by replacing the range with an array or a variable that contains an array.

```
for $i in [1, 2, 3, 4, 5]Number {
  println(i);
}
```

```
let arr = ["Hello", "World"];
for $i in arr {
  println(i);
}
```

Iterating over an array with an index is also supported.<br>
This index can also be added to range for loops but will be ignored and trying to use it will result in a runtime error.

```
let arr = ["Hello", "World"];
for $item, $i in arr {
  println(i + 1 .. ": " .. item);
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

Use `-` to flip a number sign.

```
let a = 1;
print(-a); // -1
print(-(-a)) // 1
```

## Built-in functions

1. `print()` or `println()` for printing to the console
2. `sleep(time in ms)` for pausing the program
3. `now()` for getting the current time in milliseconds
4. `input()` for getting user input
5. `String::length()` for getting the length of a string
6. `String::upper()` for converting a string to uppercase
7. `String::lower()` for converting a string to lowercase
8. `Number::to_string()` for converting a number to a string
9. `Boolean::to_string()` for converting a boolean to a string
10. `Array::push()` for adding an element to the end of an array
11. `Array::pop()` for removing the last element from an array