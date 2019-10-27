#!/bin/bash

for background_color in {40..47} {100..107} 49; do
    for text_color in {30..37} {90..97} 39; do
        for format_code in 0 1 2 4 5 7; do
            printf "\e[${background_color};${text_color};${format_code}m ^[${background_color};${text_color};${format_code}m \e[0m"
        done
        echo
    done
done