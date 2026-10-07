# Only qmk is working

## why?
I started to make th project with kmk while I don't had the microcontroller, so I did a code with the documentation.
The problem was KMK, this deprecated library's documentation was wrong, functionalities were trying to cycle in None and everything was not clean/ working as intended.

### I had to make a decision:
I mention that for KMK I did not used AI.
I took the Mediapad QMK source code, and I asked an AI to translate the functionalities I implemented in KMK to the QMK project, then I asked the IA to make a script to compile the code and get the firmware in only one command.

I don't like using IA but I never used c so I needed a working base to dev on, it didn't made any new functionalities, it just traduced using Mediapad Firmware basis.

For the new functionalities and what I am working for, I no longer use the integrated IA or purely generate code with AI.
I have to mention I use VScode autocompletion the time I get used to C and use Gemini / lumo to debug the errors I don't understand now.

I will reduce my usage now because I am on my way to understand this language :)

### What is what?
For you to see more clearly:
- KMK: fully me
- QMK base: traduced from KMK using mediapad code but not with any new functionality.
- QMK: Me with AI debug for the moment

## how do this project work?
The code is (mainly) in keymap.c and to compile it, I use QMK wsl (40x times faster than qmk WSYS, a dozen seconds instead of minuts), and to make it easier, I use sBuild.cmd. This one let you in the shell so it don't need to reopen each time :)

# How do I flash my code?
Double click the reset button to enter the bootloader then put the firmwarebk_kmk_port.uf2 inside.
(And it's done :D)


*This file is fully hand written if you wonder, only the QMK folder is touched by AI (eww)*