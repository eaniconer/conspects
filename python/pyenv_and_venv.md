
## Использование разных версий питона на одной машине

[documentation](https://github.com/pyenv/pyenv)

```bash
$ brew update
$ brew install pyenv
$ eval "$(pyenv init -)"
```

#### Посмотреть доступные версии питона

```bash
$ pyenv versions
  system
* 3.7.3
  3.8.0
```

*Активная версия помечена `*`*

#### Установка определенной версии

```bash
$ pyenv install 3.8.0
```

#### Локальное переключение между версиями

```bash

$ mkdir py7 && cd py7
$ pyenv local 3.7.3  # будет создан файл .python-version
$ cd ../

$ mkdir py8 && cd py8
$ pyenv local 3.8.0
$ cd ../

$ cd py7
$ python3 --version
Python 3.7.3
$ cd ../py8
$ python3 --version
Python 3.8.0

```

## Создание виртуальной среды с помощью venv

*python 3.3+*

```bash
$ python3 -m venv example_project
$ . example_project/bin/activate
```
