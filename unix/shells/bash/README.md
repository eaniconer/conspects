

## Type of shell invocation

#### as interactive login shell, or with `--login`

Files read:
1. /etc/profile
2. The first readable of: `~/.bash_profile`, `~/.bash_login`, `~/.profile`
3. on logout: `~/.bash_logout`

#### as interactive non-login shell

Files read:
1. `~/.bashrc`


## Interactive shells

#### Is shell interactive?

`$ echo $-`

if content contains `i` when the shell is interactive

## Debugging

1. `bash -x script.sh`

Note: since `-x` used, each command and its arguments are printed to stdout

#### Partial script debugging

```
set -x  # activate debug mode
...
set +x  # deactivate debug mode
```

Options overview:

1. `set -f` == `set -o noglob` - disable file name generation (`ls *` doesn't work, no such directory `*`)
2. `set -v` == `set -o verbose` - prints shell input lines as they read
3. `set -x` == `set -o xtrace` - prints command traces before its execution

#### Using shebang for debugging

`#!/bin/bash -xv`

## Variables

### Global variables

`env`, `printenv`

### Local variables
_available in the current shell_

`set` - list of all variable and functions


### Creating/Deleting local variables

`VAR="value"` (_spaces around `=` are forbidden_)

`unset VAR`

### Exporting variables

_Child processes of the current shell will not be aware of local variables. In order to pass variable to subshell, we have to use `export` command._

`export VAR="value"`

__Note__: If a subshell changes VAR, it will not affect the variable in the parent shell.

### Reserved variables

 - `CDPATH` - search path for `cd` built-in command
 - `HOME`
 - `IFS` - list of characters used for separating when shell splits words

<details><summary>IFS details</summary>

```
IFS=":"
STRS="abc:bcd abc dbe"
for word in $STRS; do
  echo "Word: $word"
done
```
Output:
```
Word: abc
Word: bcd abc dbe
```
</details>

 - `PATH` - list of directories where shell looks for commands
 - `PS1`, `PS2` - primary/secondary prompt string

### Bash-reserved variables

`RANDOM`, `UID`, `PWD`, `BASH`, `BASH_ENV`

<details><summary> REPLY </summary>

```
.tmp$ read
hello
.tmp$ echo $REPLY
hello
```

</details>

<details><summary> TMOUT </summary>

`read` will be interrupted in 2 seconds,
then shell will be interrupted in 2 seconds [Process finished]

```
.tmp$ TMOUT=2
.tmp$ read
```

</details>

### Special parameters

see `$0`, `$1`, `$$`, `$@`, `$#`, `$?`

```
#!/bin/bash

# script.sh
# expected 2 positional parameters

ARG1=$1
ARG2=$2

echo "script name: $0"
echo "process ID of the shell: $$"
echo "process ID of the most recently executed background command: $!"

echo "1st arg: $1"
echo "2nd arg: $2"
echo "all args: $@"  # or $*

echo "n args: $#"

exit 113  # check it using $?
```
Terminal:
```
$ ./script.sh 23 32
script name: ./script.sh
process ID of the shell: 65866
process ID of the most recently executed background command:
1st arg: 23
2nd arg: 32
all args: 23 32
n args: 2

$ echo $?
113
```

## Quoting characters

`VAR=10`

1. `echo \$VAR` prints `$VAR`
2. Single quotes: `echo '$VAR'` prints `$VAR`
3. Double quotes:
    - `echo "$VAR"` -> `10`
    - ```echo "`date`"```

## Shell expansion

There are 8 types of expansion perormed. In the next sections they are described in the order they are expanded.

### 1. Brace expansion

```bash
$ echo ab{1,2,3}cd
ab1cd ab2cd ab3cd
```

```bash
$ echo ab{1,2}c{3,4}d
ab1c3d ab1c4d ab2c3d ab2c4d
```

```bash
$ echo {1..3}
1 2 3
```

```bash
$ echo {a..d}
a b c d
```

A well-formed brace expansion consist of unquoted opening and closing braces, and at least one unquoted comma.


### 2. Tilde expansion

word: `~[tilde-prefix]/[anything]/[anything]/...`

_Note: tilde-prefix doesn't contain backslashes_

 - If none of character in `tilde-prefix` are quoted, the prefix is treated as login name.
 - If login name is null string, tilde replaced with `HOME`.  If 'HOME' is unset, then it replaced with home directory of the user executing the shell.

 ```bash
$ echo ~+/abc  # echo $PWD/abc
$ echo ~-/abc  # echo $OLDPWD/abc
```

###### directory stack

```bash
tmp$ mkdir -p a/b/c
tmp$ pushd a
~/tmp/a ~/tmp
a$ pushd b
~/tmp/a/b ~/tmp/a ~/tmp
b$ pushd c
~/tmp/a/b/c ~/tmp/a/b ~/tmp/a ~/tmp

c$ echo ~+0  # == echo ~0
~/tmp/a/b/c  # the first in the stack (we can see order using `dirs`)

c$ echo ~-0
~/tmp  # the last in the stack

c$ echo ~+1
~/tmp/a/b  # the second in the stack
```

### 3. Shell parameter and variable expansion

 - `"${PARAM}"` - the value of `PARAM` will be substituted
 - indirect expansion

    ```
    $ val1 = 10
    $ val2 = val1
    $ echo "${!val2}"
    10
    ```

 - creating named variable `echo "${VAR:=100}"`


### 4. Command substitution

_replace `command` with its result_

`$(command)`

or with backticks:

```
`command`
```

### 5. Arithmetic expansion

`$(( EXPRESSION ))` or `$[ EXPRESSION ]`

`[BASE#]N` - usage of arithmetic base:

```bash
echo $(( 2#11 ))  # base 2, output: 3
echo $(( 3#11 ))  # base 3, output: 4
```

### 6. Process substitution

### 7. Process substitution

### 8. File name expansion

It works, unless the `-f` option has been set (`set -f`). Bash scans each word for the characters `*`, `?`, `[` and if one of them found, then the word is regarded as PATTERN.

```bash
tmp$ touch a.txt
tmp$ set -o noglob  # `set -f`
tmp$ echo *
*
tmp$ set +o noglob  # `set +f`
tmp$ echo *
a.txt

tmp$ export GLOBIGNORE="b.*:a*"  # colon-separated list
tmp$ ls
a.txt	b	b.exe	c
tmp$ echo *  # all file names that match to at least one of pattern in GLOBIGNORE
             # will be skipped
b c

```

Note: _to get the option list_: `shopt` or `set -o`


## Aliases

_Show aliases_:

```bash
$ alias
```

```bash
$ alias say_hello='echo Hello'
$ say_hello
Hello
$ unalias say_hello
$ say_hello
-bash: say_hello: command not found
```

# Regular Expression

#### Line and word anchors. `grep`

```bash
$ grep ^abc file.txt  # lines starting with `abc`

$ grep abc$ file.txt  # lines ending with `abc`

$ grep [wx] file.txt  # lines contain either `w` or `x`

$ grep '\<g...t\>' file.txt  # five-character words starting with `g` and ending with `t`

$ grep '\<g.*t\>' file.txt

# Asterisk
$ grep '*' file.txt  # lines with asterisk

$ ls
a.txt	b.txt
tmp$ cat b.txt
a.txt
a.txt
tmp$ grep -n *  # asterisk expands to `a.txt b.txt`
1:a.txt
2:a.txt

```

### Character ranges

```bash
$ mkdir {a..z}1
$ ls -d [a-cx-z]1
a1	b1	c1	x1	y1	z1
```

Exclusion

```bash
$ mkdir a{1..4}
$ ls -d a[!3-4]  # any characters are not enclosed will be matched
a1	a2
$ ls -d a[^1-2]
a3	a4
```

### Character classes

Classes: `alnum`, `alpha`, `ascii`, `blank`, `cntrl`, `digit`, `graph`, `lower`, `print`, `punct`, `space`, `upper`, `word`, `xdigit`

Syntax: `[:CLASS:]`

Example:

```bash
$ mkdir a{1..3} a{b..d}
$ ls
a1	a2	a3	ab	ac	ad
$ ls -d a[[:digit:]]
a1	a2	a3
```

# The GNU sed stream editor

_sed - Stream EDitor_

## Interactive editing

### Printing lines containing the a pattern

```bash
$ sed -n '/hello/p' file.txt  # print all lines with substring `hello`
# `/p` - command Print
# without -n sed print entire file, but matched lines printed twice
```

With pattern

```bash
$ sed -n '/^h.*o$/p' file.txt
```

### Deleting lines

```bash
$ sed -n '/hello/d' file.txt
# `/d` stands for delete-command
```

### Range of lines

```bash

$ sed -n '2,4p' a.txt  # print lines 2,3,4
$ sed '2,4d' a.txt  # delete lines 2,3,4
$ sed '4,$d' a.txt  # print only first 3 lines

bash$ cat a.txt
a1
a2
a3
a4
a5
a6
bash$
bash$ sed -n '/a2/,/a4/p' a.txt
a2
a3
a4
```

### Find and replace

```bash
$ sed 's/a1/a11/' a.txt  # replace only first occurrence in the line
$ sed 's/a1/a11/g' a.txt  # examine entire line

$ sed 's/^/> /' a.txt  # insert `> ` at the beginning of each line
$ sed 's/$/END/' a.txt  # insert `END` at the end of each line

# mutliple find and replace with -e option
$ sed -e 's/x1/y1/g' -e 's/XX/YY/g' a.txt
```











