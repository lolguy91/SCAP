make -C emulator clean > /dev/null
make -C assembler clean > /dev/null
rm -rf examples/*.bin


echo "Cleaned the emulator dir and assembler dir"