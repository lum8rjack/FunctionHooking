NAME := test
BUILD := i686-w64-mingw32-gcc

default: windows

clean:
	rm -f haystack*.bin
	rm -f haystack*.exe

windows:
	echo "Compiling for Windows x32"
	$(BUILD) $(NAME).c -o $(NAME)32.exe


