sudo apt install libsodium-dev
cmake -B debug-build
cd debug-build
make
make package
