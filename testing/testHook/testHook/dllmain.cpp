#include "pch.h"
#include <Windows.h>
#include <iostream>
#include "detours.h"
#include "sigscan.h"
#include <stdio.h>

DWORD AddressOfSum = 0;
typedef int(*sum)(int x, int y);

int HookSum(int x, int y)
{
	// manipulate the arguments
	printf("Adding 100 to x and y!\n");
	x += 100;
	y += 100;
	// redirect the program to original function
	sum originalSum = (sum)AddressOfSum;
	return originalSum(x, y);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		printf("Injected\n");
		// We will use signature scanning to find the function that we want to hook
		SigScan Scanner;
		char program[] = "test32.exe";
		char s[] = "\x55\x89\xE5\x8B\x55\x08\x8B\x45\x0C";
		char mask[] = "xxxxxxxxx";
		AddressOfSum = Scanner.FindPattern(program, s, mask);
		printf("Address of sum: %p\n", (LPVOID&)AddressOfSum);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		// this will hook the function
		DetourAttach(&(LPVOID&)AddressOfSum, &HookSum);
		DetourTransactionCommit();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		// unhook
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		// this will hook the function
		DetourDetach(&(LPVOID&)AddressOfSum, &HookSum);
		DetourTransactionCommit();
	}
	return TRUE;
}