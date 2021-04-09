RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

cmake -B build
cd build
make
make package

if [ $? -eq 0 ]; then
   echo -e "${GREEN}PassMann build successfully${ENDCOLOR}"
else
   echo -e "${RED}There was a problem while building PassMann ${ENDCOLOR}"
fi