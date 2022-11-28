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

The initial release is fully functional. 
On the todo-list are some planned improvements:
- [x] automatic GUI adjustements for high-DPI displays
- [x] change font size with Keyboard commands: Ctrl++ and Ctrl+-
- [ ] change font size with mouse scroll wheel
- [ ] independent aes.h code review
- [ ] remove global variables from ced_main.cpp
- [ ] add special feature: for quickly copying passwords
- [ ] add special feature: visually hide passwords (using syntax highlighting: white-on-white)


## Screenshots
![](http://coreloop.de/images/ced_screenshot.png)

## Credits
[Original editor code](https://github.com/NicoSchumann/fltk_text_editor) by Nico Schumann.
No license was given, but because FLTK is GPL, this code must be GPL, too.
The Code by Nico in turn is based on fltk texteditor tutorial: https://www.fltk.org/doc-1.3/editor.html

The code in aes.h provides a very thin wrapper around the [gnu cryptography library](https://www.gnupg.org/documentation/manuals/gcrypt/).
The code follows advice given in an [AES file encryption tutorial by Tyler Nichols](https://www.tnichols.org/2015/09/27/Encrypting-and-Signing-Using-libgcrypt/), a security engineer and pentester. 

