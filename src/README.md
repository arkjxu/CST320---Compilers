# CST320 - Assignment 2
Wilsonville, Winter 2020

### Overview
Use Bison to parse the Turtle Logo language.
```
For grammar information please look at the Tutle Logo Grammar Document provided by the instructor.
```
### To Dos
* Define a context-free grammar for the syntax of Turtle Logo
* Print each production to demonstrate how to parse is completed
* Print appropriate error messages

### Examples
```
// Example 1

// Input:
if (color = black)
[
]

Example Input:
if (color = black)
[
        rt 45
        fd 20
]

//Output:
function->COLOR
expression->function
expression->COLOR_NAME
condition->expression = expression
expression->NUMBER
statement->RT expression
expression->NUMBER
statement->FD expression
statements-><empty>
statements->statement statements
statements->statement statements
statement->IF ( condition ) [ statements ]
statements-><empty>
statements->statement statements
```
```
// Example 2

// Input:
if (color = black) 
[
home
rt bk
]

// Output:
function->COLOR
expression->function
expression->COLOR_NAME
condition->expression = expression
statement->HOME
ERROR! Line #4: syntax error, near 'bk'
```