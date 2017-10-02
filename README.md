# Real-time-assembler

This project makes it possible to write assembly code in real time by using C++. The created code can be run and also deleted in real time, thus for example making it harder to analyze the code if it only exists temporarily. I made this project to learn how to generate code on runtime and do different things with it. 

Due to time constraints many of the opcodes are not implemented. Many of the important ones like mov, call etc. are however implemented and can be used as reference on how to implement additional opcodes.

Note: The project only works on windows as the windows API is used, however it would be easy to port it linux by changing the API calls to corresponding ones on linux systems.

## Example of usage
```c++
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
```

## Implementing a new opcode
```c++
ASMOpcode * ASMFunction::mov(REGISTER srcReg, REGISTER destReg)
{
	uint8_t opCodeByte = 0x8B;
	uint8_t modByte = 0x0;

	modByte |= 0x3;
	modByte = (modByte << 6);

	uint8_t modByte2 = 0x0;
	modByte2 |= destReg;
	modByte2 = (modByte2 << 3);

	modByte |= modByte2;

	uint8_t modByte3 = 0x0;
	modByte3 |= srcReg;
	modByte |= modByte3;

	uint16_t fullInstruction = 0x0;
	fullInstruction |= modByte;
	fullInstruction = (fullInstruction << 8);
	fullInstruction |= opCodeByte;
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&fullInstruction, currentOffset, sizeof(uint16_t));
	return createAndAddOpcode(sizeof(uint16_t), convertLocation(memoryStart, currentOffset));
}
```

The opcode here is the implementation for the mov instruction to move value from one register to another. 

It roughly follows a three step process:

1. Creating the full instruction by merging all the individual bytes required into a full instruction by using simple bit manipulation by shifting the bits to right by a certain amount and then using OR operator to combine them together.

2. Writing the opcode into memory by using the writeToMemoryAndProtect method. Here you have to be careful to use the right size of the instruction.

3. Return the created opcode by using the createAndAddOpcode method. The returned value can be passed to certain other functions, for example and implementation of jmp can use the opcode as argument and take it's address.
