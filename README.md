# FunctionHooking
Function hooking examples

## injector
Simple C++ Visual Studio project that will perform DLL injection.

```
injector.exe <program to inject into> <dll to inject>
```

## Part1 - Test Program
Testing function hooking using Microsoft Detours and signature scanning. The testApp directory contains a basic C application that calculates the sum of two numbers. The testHook application contains the DLL to inject into the application. This DLL will find and hook the sum function and allow the user to intercept or change the arguments made to the function.

More details can be found here: https://lum8rjack.github.io/posts/function-hooking-part-1/

## Part2 - PasswordSafe
This directory contains the code that is used to hook the [Password Safe](https://pwsafe.org/) application. It uses the same techniques as the code from the testing directory. The DLL will hook the "CheckPasskey" function in order to capture the password that unlocks the database. 

More details can be found here: https://lum8rjack.github.io/posts/function-hooking-part-2/

## Part3 - Frida
This directory contains 

More details can be found here: https://lum8rjack.github.io/posts/function-hooking-part-3/


