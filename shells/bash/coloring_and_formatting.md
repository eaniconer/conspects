
### Intro

`<esc-character>[<FormatCode>m`

Examples:
 - `echo -e "\e[31mHello\e[0m"` (-e for parsing escape sequences)
 - `printf "\e[31mHello\e[0m"`

 `\e[0m` removes color and formatting


### Formatting

Set/reset

- `printf "\e[1mBold\e[0m"`
- `printf "\e[2mDim\e[0m"`
- `printf "\e[3mItalic\e[0m"`
- `printf "\e[4mUnderlined\e[0m"`
- `printf "\e[5mBlink\e[0m"`
- `printf "\e[7mInverted\e[0m"`
- `printf "\e[8mHidden\e[0m"` (is not visible, but copyable)

###  Color

Foreground color (text)

- `printf "\e[39mDefault\e[0m"`
- `printf "\e[31mRed\e[0m"`, `printf "\e[31mGreen\e[0m"`, ..., `printf "\e[37mCyan\e[0m"`
- `printf "Default \e[90mDark gray\e[0m"`  (till 97 inclusively)

Background works the same way, but you have to replace 3 by 4, 9 by 10 in format code.

### Code combination

- `printf "\e[1;4mBold and Underlined \e[0m"`
- `printf "\e[1;31;42mBold.RedForeground.GreenBackground \e[0m"`

### Other ways

- `tput`
	- `echo "$(tput setaf 1)Text$(tput sgr0)"`


_Note 1_: `tput setaf {CODE}` - foreground, `tput setab {CODE}` - background, `tput sgr0` - reset all
_Note 2_: `tput rev` - inverse colors, `tput bold` - bold text

#### References
1. [Tip colors and formatting](https://misc.flogisoft.com/bash/tip_colors_and_formatting)
2. [BASH Shell Change The Color of Shell Prompt on Linux or UNIX](https://www.cyberciti.biz/faq/bash-shell-change-the-color-of-my-shell-prompt-under-linux-or-unix/)