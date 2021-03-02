cd include/libsodium
make && make check
sudo make install
cd ../..
cmake -B debug-build
cd debug-build
make
make package
