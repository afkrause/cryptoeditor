# Crypto-Editor
A text editor with minimal dependencies that saves and loads text as AES encrypted files. 

Features:
* runs and compiles everywhere (Windows, Linux, Raspberry PI, MacOS))
* only two dependencies: libfltk and libgcrypt
* tiny code size - easy to verify
* strong and secure AES encrpytion

Compile for Linux:
'''
sudo apt install libfltk1.3-dev
sudo apt install libgcrypt-dev
g++ ced_main.cpp -lgcrypt -lfltk -o ced
'''
