# PassMann

Simple password manager created in C for Linux \
For now only .deb file is built.
Created with CLion

THIS PROJECT IS ARCHIVED DUE TO THE FACT I DON'T HAVE TIME MAINTAINING IT SINCE I HAVE FEW PROJECTS ALREADY ON THE SIDE. 

# Installation Debian based distros

Make sure to have Cmake and libsodium-dev installed as you won't be able to build.

```shell
sudo apt update
sudo apt install cmake libsodium-dev
```

Then run:

```shell
./build.sh
```

It will simply build the project, after that navigate to build folder and
simply run .deb file that is built or run:

```shell
cd build/
sudo dpkg -i PassMann-[version]-Linux.deb
```

If you get dependencies error just run:

```shell
sudo apt install -f
```

# Installation other distors(for now)

As Debian based distros make sure to install cmake and libsodium-dev.

Then run:

```shell
./install.sh
```

The script will be compiled and installed onto your system.
