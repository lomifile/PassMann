RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

if ! [ -x "$(command -v cmake)" ]; then
  echo -e "${RED}Error${ENDCOLOR}: cmake is not installed." >&2
  exit 1
fi

cmake -B build
cd build
make
cd src

echo 'Requesting sudo rights now'
sudo cp PassMann /usr/bin/

if [ $? -eq 0 ]; then
	echo -e "${GREEN}Passman installed successfully${ENDCOLOR}"
else
	echo I see you have it already
	sudo rm /usr/bin/PassMann
	sudo cp PassMann /usr/bin
    echo -e "${GREEN}Passmann installed succesfully${ENDCOLOR}"
fi
