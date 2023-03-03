rm ../CMakeCache.txt
rm CMakeCache.txt
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make install DESTDIR=app_dir
./linuxdeploy-x86_64.AppImage --appdir=app_dir
./appimagetool-x86_64.AppImage app_dir

