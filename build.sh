RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

if [ "$1" == "deb" ]; then
   cmake -DDEB_PACK=ON -DRPM_PACK=OFF build
elif [ "$1" == "rpm" ]; then
   cmake -DRPM_PACK=ON -DDEB_PACK=OFF build
else
   cmake -B build
fi

cd build
make
make package

if [ $? -eq 0 ]; then
   echo -e "${GREEN}PassMann build successfully${ENDCOLOR}"
else
   echo -e "${RED}There was a problem while building PassMann ${ENDCOLOR}"
fi