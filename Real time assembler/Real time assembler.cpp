#include "stdafx.h"
#include <Windows.h>
#include <stdint.h>
#include "ASMContext.h"


int main(int argc, char* argv[])
{
	// Usage example

	// Create a context for the function. Context can have more than one function.
	ASMContext context(NULL);
	// Create a new function in the context.
	ASMFunction * f = context.createFunction();
	// A reference to an opcode can be saved this way so it can be later on used in a jump for example.
	ASMOpcode * p1 = f->push(EAX);
	f->push(EBP);
	f->push(ECX);
	DWORD b = 0;
	DWORD * a = &b;
	f->mov(&b, 5);
	f->mov(EAX, (DWORD)10);

	// The system can also be used to run already existing functions.
	// Here the windows-function GetKeyState() is ran.
	// Note that you need to repeatendly press the up-key for this function to register it.
	SHORT(__stdcall*fp)(int) = GetKeyState;
	DWORD address = (DWORD)fp;
	f->mov(EAX, (DWORD)VK_UP);
	f->push(EAX);
	f->call(address);
	// According to the calling convention of this function, the result is stored in EAX and here it is moved to the variable b.
	f->movMR(&b, EAX);

	f->pop(EAX);
	f->pop(EBP);
	f->pop(ECX);
	f->ret();
	// Run the function that was made with the chosen arguments. Note: The arguments aren't really used here.
	f->run<void>(1, 2, 3);
	// Finally destroy the function that was used.
	context.destroyFunction(f);
	if (b > 0) {
		std::cout << "The up-key was pressed\n";
	}
	else {
		std::cout << "The up-key wasn't pressed\n";
	}

	return 0;
}

