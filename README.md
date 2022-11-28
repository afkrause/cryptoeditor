# Crypto-Editor
A text editor with minimal dependencies that saves and loads text as AES encrypted files. 
Very useful for keeping an encrypted text file that contains your website login-passwords, ssh keys etc. or just any private information that maps to ASCII code. 

Features:
* runs and compiles everywhere (Windows, Linux, Raspberry PI, MacOS))
* only two dependencies: libfltk and libgcrypt
* tiny code size - easy to verify
* strong and secure AES encrpytion

Compile for Linux:
```console
sudo apt install libfltk1.3-dev
sudo apt install libgcrypt-dev
g++ ced_main.cpp -lgcrypt -lfltk -o ced
```

TODO List: 
[x] automatic GUI adjustements for high-DPI displays
[x] change font size with Keyboard commands: Ctrl++ and Ctrl+-
[ ] change font size with mouse scroll wheel
[ ] aes.h code review
[ ] remove global variables from ced_main.cpp
[ ] add special features
