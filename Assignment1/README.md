# CST320 - Assignment 1
Wilsonville, Winter 2020

### Overview
Writing a simple lexical analyzer for the Tutle Logo using Flex
```
For grammar information please look at the Tutle Logo Grammar Document provided by the instructor.
```
### To Dos
* Examine the Turtle grammar to come up with complete list 
of valid tokens
* Use Flex to write a lexical analyzer
* Start with hand out code for assignment 1
* Only recognize tokens and identify errors in tokens
* It shouldn't check for syntax errors

### Examples
```
// Example 1

// Input:
fd 3x00
bk
setc 100
int xy3 fd 10 rt 90

* The analyzer should only see the 3x00 as an illegal token
```
```
// Example 2

// Input:
fd 3x00
bk
setc 100
int xy3 fd 10 rt 90

// Output:
Keyword: fd
Illegal: 3x00
Keyword: bk
Keyword: setc
Number: 100
Keyword: int
Id: xy3
Keyword: fd
Number: 10
Keyword: rt
Number 90
```

### Input and Output
```
Read an input logo program through stdin and output tokens 
and errors to stdout.

Output should report all illegal tokens

Output should have all comments removed (C/C++ style)
```