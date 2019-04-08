# New Scripting Language

This is an exploration on creating a new scripting language to replace BASH/BATCH scripts. The main point of this scripting language is to "peice together" functionality from other programs, rather than containing alot of logic/functionality in the scripting language itself (if I wanted a scripting language for that I'd just use python).  Here are some goals

1. Run on many platforms
2. Concise and pleasing syntax (sometimes these 2 goals are at odds with eachother)
3. Scripts can be pre-processed before running them to check compatibility

Being an good interactive shell language is not meant to be a requirement/goal.  This language is meant to live inside files that are re-used and maintained for long periods of time.  With that note, the syntax should be optimized for "readability" vs "minimal keystrokes".  Though, sometimes fewer characters also means improved readability, this isn't always the case.

# Design Strategy

Currently I'm not trying to design the syntax, just the semantics.  The best syntax will be easier to get right if the semantics are flushed out first, that way the more common use cases can receive shorter syntax.  That being said, I will be using a very verbose syntax for now designed to understand the semantics rather than being easy to write.

# Compatibility Check

I'd like to be able to take a script and check whether or not it can run before running it. And when you invoke a scripts, I'd like it by default to first check as much as it can whether or not the script will work and tell you about problems with it before it fails in the middle of running (i.e. this script requires 'gcc' but it isn't found on the system).

With scripting langauges today like BASH, you could imagine using a BASH parser that checks if all the programs it calls exist on the system.  However, many BASH scripts run on many systems and some of the programs called won't be used or exist on the system.  So you end up having to execute *some* of the code in the script to know which code paths will be taken. The code used to determine code paths should be distinguishable from other code in that it should be executed during the "check compatibility" stage.

# Implementation

I'm currently implementing it in D since it's quick and easy to write in.  I will consder porting it to C since it will allow it to be easily ported to many platforms and I don't anticipate the implementation being too large/complex. It should be able to run on as many platforms as possible and be easy to port to other platforms.  Having a C compiler for a platform is a much lower bar than having a `C++` compiler, and this program should be simple enough that losing `C++` features isn't a big deal.

# Current Syntax

> NOTE: this syntax is not the syntax of the final scripting language, just a syntax for exploring semantics.

### Value Literals
```
false
true
null
"<chars>"
[<value> <value>...]
unquoted strings?
$<name> (access the variable 'name)
```

### Assign a symbol to a value
```
let <name> be <value>

let x be "hello"
let foo be function
```

### Get the value of a variable
```
$<name>
```


### Declare an enum type
```
enum(<name>...)
```

### Declare a function type
```
function
    [name <name>]
    [returns <type>]
    [named|anon <type> <value> [default] <value>]...

funcion name say_hello_to
    anon string name
    named string mood

# example call
call say_hello_to("fred")(mood "quizical")

# For now I'll also use the function parameter constructs to declare
# script parameters as well.  They appear at the beginning of the script.
# anon script parameters will be posisiton and named will use some other
# interface (i.e. var=value or -var value or -var=value or something)
```

### Call a function
```
call <name> (<values>...)(<name1> <value1>...)
```
### If statement
```
if <value>
{
    <code>
}
else if <value>
{
    <code>
}
else
{
    <code>
}
```

# Type System

The `string` type. One strategy would be to make most types inherit from the string type.
The `int` and `uint` types, arbitrary precision signed and unsigned integers respectively.  I think I'll have these be arbitrary precision integers, wheter internally they will be represented as:
```c
struct Integer
{
    bool is_inplace; // true if the value is 'inplace', false if the value is in a buffer pointed
                     // to by value_ptr
    union
    {
        size_t value_inplace; // the value of the integer
        unsigned uint8_t *value_ptr; // pointer to memory containing the value of the integer in base 256
    }
}
```
The `bool` type.

Add `[]` to the end of a type to make it an array (i.e. `string[]`).

The `filehandle` type.
The `path` type, represents a string that is a file path.

The `argument` abstract type.  This means the type can be passed as an argument to a program which means it can be converted to a string.

# Functions

```
function name exit
function name exists returns bool
    anon path path
function name uid returns uint
function name equal returns bool
    anon object left
    anon object right
function name not_equal returns bool
    anon object left
    anon object right
function name open returns filehandle
    anon path filename
    anon string mode
function name print
    anon object[] objs
    named filehandle out default filehandle.stdout
function name cd
    anon path dir

# TODO: a function to build a file path?
# TODO: function to normalize string to path (i.e. replace '/' with '\' on windows)?
```
> NOTE: executing an external program is such a fundamental part of the
>       language, maybe it should have its own construct?

# Run external programs
```
run (<command>...)
    [env (<values>)]
    [in <value>]
    [out <value>]
    [err <value>]
    [cd <dir>]
    [noWait]
    [exec]
# TODO: how to handle program/PATH?
```

# Idea: Scoped Operations

The language could support scoped operations such as changing directory, setting local/environment variables which get undone when the scope is exited, i.e.
```

pwd # prints /mydir
{
    cd /anotherdir
    pwd # prints /anotherdir
}
pwd # prints /mydir
```

# Ideas

### Relative Paths

Consider the following
```
let myfile be "foo"
call print("hello")(out open(myfile "w"))
call cd("..")
forkWait ("cat" $myfile)
```

In the above example, `myfile` becomes invalid once we change directories.  However, if we can indicate to the script engine that `myfile` is supposed to be a "path relative to the current-working-directory at the time it is set", then the script engine can adjust it whenever the "current-working-directory" is changed.

```
let myfile1 be call cwdPath("foo")
let myfile2 be call scriptPath("foo")
```

So in this case, `myfile1` is relative to the current-working directory, whereas `myfile2` is relative to the path of the script.

### Refernce Counting

Consider "cat-ing" a file to another file:
```
run ("cat" "myfile") out open("myfile_copy" "w")
```

In this example, it's clear we want to open "myfile_copy" for the duration of the command, and close it when the `cat` program finishes. When the file is opened and assigned to stdout, it gains a single reference, and when the command finishes it removes that reference.  Since there are no more references, the filehandle is closed.

Consider another example:
```
let myfile_copy be open("myfile_copy" "w")
run ("cat" "myfile") out myfile_copy
```

In this case we have a reference to the opened file with the `myfile_copy` variable. When the `cat` program runs, it will temporarily have 2 references, and then the file handle will be closed when `myfile_copy` goes out of scope. What about this example?

```
let combined be open("combined" "w")
run ("cat" "a") out combined
call print("hello")(stdout combined)
run ("cat" "a") out combined
let combined2 be combined
release combined
call print("another")(stdout combined2)
```

In this example we reused the `combined` filehandle in 3 commands, and then "released" it from the scope.  "releasing" means it is removed the variable from the scope and can no longer be used afterwards.  But before it was released, we assigned the filehandle to another variable `combined2` so the filehandle stays open until `combined2` goes out of scope.

### Duck Typing

Consider
```
if call is_type(foo, string)
{
    # foo's type is now a string
}
else if is_type(foo, filehandle)
{
    # foo's type is not a filehandle
}
```
