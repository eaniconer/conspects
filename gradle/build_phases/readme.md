

```bash
egorbash-osx:build_phases egorbasharin$ gradle task1 task2

Welcome to Gradle 6.1!

Here are the highlights of this release:
 - Dependency cache is relocatable
 - Configurable compilation order between Groovy, Java & Scala
 - New sample projects in Gradle's documentation

For more details see https://docs.gradle.org/6.1/release-notes.html

Starting a Gradle Daemon (subsequent builds will be faster)
initialization phase

> Configure project :
configuration phase - 1
configuration phase - 2
configuration phase - 3

> Task :task1
execution phase - 1

> Task :task2
execution phase - 2
execution phase - 3

BUILD SUCCESSFUL in 3s
```
