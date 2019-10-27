
### Shells

- [ ] __sh__ (Bourne shell)
- [ ] __bash__ (Bourne again shell)
- [ ] __csh__ (C Shell)
- [ ] __tcsh__ (TENEX C Shell) superset of csh
- [ ] __ksh__ (Korn shell) superset of sh (nightmare for beginners)
- [ ] __zsh__ extended sh (improvements from bash, ksh, tsch)


##### Getting list of shells

```
$ cat /etc/shells
```


##### Getting default user shell

```
$ dscl . -read /Users/<username>/
```

##### Change default user shell

```
$ chsh -s /bin/zsh
```

##### Show current shell

```
$ echo $0
```

```
$ ps -p $(echo $$)
```

