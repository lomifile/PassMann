
cmake -B build
cd build
make 
cd debug
sudo cp PassMann /usr/bin/
if [ $? -eq 0 ]; then
	echo Passman installed successfully
else
	echo I see you have it already
	sudo rm /usr/bin/PassMann
	sudo cp PassMann /usr/bin
fi
