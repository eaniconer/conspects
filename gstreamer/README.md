
# GStreamer

## macos

#### [Installing](https://gstreamer.freedesktop.org/documentation/installing/on-mac-osx.html?gi-language=c)

Установить runtime и development пакеты. Можно взять последнюю версию отсюда: [link](https://gstreamer.freedesktop.org/data/pkg/osx/)

#### [Basic tutorial](https://gstreamer.freedesktop.org/documentation/tutorials/basic/hello-world.html?gi-language=c)

Для этого нужен фикс, иначе видео не будет воспроизводиться. Исправленная версия находится в `basic/hello_world.c`

Сборка: `gcc hello_world.c -F /Library/Frameworks/ -framework GStreamer -I /Library/Frameworks/GStreamer.framework/Headers`

