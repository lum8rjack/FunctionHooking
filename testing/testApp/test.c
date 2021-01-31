#include <windows.h>
#include <stdio.h>

// can compile using the following command
// i686-w64-mingw32-gcc test.c -o test32.exe

//this is the test function we will be hooking
int sum(int x, int y)
{
	return x + y;
}

int main()
{
	while(1)
	{
		//this will print the result of 5 + 5
		int s = sum(5,5);
		printf("test: 5 + 5 = %d\n", s);
		Sleep(5000);
	}
}
