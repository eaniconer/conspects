

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






