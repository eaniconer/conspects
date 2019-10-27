

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
...













