#pragma once

#include "ASMContext.h"
#include "ASMOpcode.h"
#include <vector>

typedef BYTE REGISTER;

enum regName
{
	EAX = 0x0,
	ECX = 0x1,
	EDX = 0x2,
	EBX = 0x3,
	ESP = 0x4,
	EBP = 0x5,
	ESI = 0x6,
	EDI = 0x7
};

class ASMContext;
class ASMOpcode;

class ASMFunction
{
	friend class ASMContext;

protected:
	LPVOID getStartAddress();

private:
	ASMContext *context;
	LPVOID memoryStart;
	DWORD currentOffset = 0;
	static const DWORD functionSize = 512;
	std::vector<ASMOpcode*> opCodes;
	LPVOID convertLocation(LPVOID start, DWORD offSet);
	ASMOpcode *createAndAddOpcode(size_t size, LPVOID location);

	void writeToMemoryAndProtect(LPVOID mem, LPVOID *content, DWORD offSet, size_t size);

public:
	// New opcodes can be added easily by implementing them according to their specifications.
	// For reference see how the current ones are implemented.

	ASMOpcode * push(REGISTER reg);
	ASMOpcode * pop(REGISTER reg);
	ASMOpcode * mov(REGISTER srcReg, REGISTER destReg);
	ASMOpcode * mov(DWORD * address, uint8_t value);
	ASMOpcode * mov(REGISTER destReg, DWORD value);
	ASMOpcode * movMR(DWORD *dest, REGISTER srcReg);
	ASMOpcode * lea(REGISTER dest, REGISTER base, REGISTER index);
	ASMOpcode * jmpA(DWORD address);
	ASMOpcode * jmpA(ASMOpcode *code);
	ASMOpcode * call(DWORD address);
	void jmpR(ASMOpcode *code);
	void jmpR(DWORD address);
	ASMOpcode * ret();

	template <typename rType, typename ... T>
	rType run(T ... args)
	{
		rType(*fPtr)(T ... args) = (rType(*)(T ... args))memoryStart;
		return fPtr(args...);
	}

	ASMFunction(ASMContext *context);
	~ASMFunction();
};

