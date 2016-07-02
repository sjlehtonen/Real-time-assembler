#include "stdafx.h"
#include "ASMFunction.h"


ASMFunction::ASMFunction(ASMContext *context)
{
	this->context = context;

	// Allocate region for the function
	memoryStart = VirtualAllocEx(context->getContextHandle(), NULL, functionSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	assert(memoryStart != NULL);
}

LPVOID ASMFunction::getStartAddress()
{
	return memoryStart;
}

void ASMFunction::writeToMemoryAndProtect(LPVOID mem, LPVOID *content, DWORD offSet, size_t size)
{
	VirtualProtectEx(context->getContextHandle(), (LPVOID)((DWORD)mem + offSet), size, PAGE_READWRITE, NULL);
	WriteProcessMemory(GetCurrentProcess(), (LPVOID)((DWORD)mem + offSet), content, size, NULL);
	VirtualProtectEx(context->getContextHandle(), (LPVOID)((DWORD)mem + offSet), size, PAGE_EXECUTE, NULL);
	currentOffset += size;
}

LPVOID ASMFunction::convertLocation(LPVOID start, DWORD offSet)
{
	return (LPVOID)((DWORD)start + offSet);
}

ASMOpcode * ASMFunction::createAndAddOpcode(size_t size, LPVOID location)
{
	ASMOpcode * op = new ASMOpcode(size, location, this);
	this->opCodes.push_back(op);
	return op;
}


ASMOpcode * ASMFunction::push(REGISTER reg)
{
	BYTE instruction = 0x50;

	uint8_t fullInstruction = 0x00;
	fullInstruction |= instruction;
	fullInstruction |= reg;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&fullInstruction, currentOffset, sizeof(uint8_t));
	return createAndAddOpcode(sizeof(uint8_t), convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::call(DWORD address)
{
	this->mov(EAX, (DWORD)address);
	uint8_t opCodeByte = 0xFF;
	uint8_t sCode = 0xD0;
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&sCode, currentOffset, sizeof(uint8_t));
	return createAndAddOpcode((sizeof(uint8_t) * 2), convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::pop(REGISTER reg)
{
	BYTE instruction = 0x58;

	uint8_t fullInstruction = 0x00;
	fullInstruction |= instruction;
	fullInstruction |= reg;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&fullInstruction, currentOffset, sizeof(uint8_t));
	return createAndAddOpcode(sizeof(uint8_t), convertLocation(memoryStart, currentOffset));
}


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

ASMOpcode * ASMFunction::mov(REGISTER destReg, DWORD value)
{
	uint8_t opCodeByte = 0xB8;
	opCodeByte |= destReg;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&value, currentOffset, sizeof(DWORD));
	return createAndAddOpcode((sizeof(uint8_t)) + sizeof(DWORD), convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::movMR(DWORD *dest, REGISTER srcReg)
{
	uint8_t opCodeByte = 0x89;
	uint8_t modRMByte = 0x00;
	modRMByte |= 0x0;
	modRMByte = (modRMByte << 6);
	modRMByte |= srcReg;
	modRMByte = (modRMByte << 3);
	modRMByte |= 0x5;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&modRMByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&dest, currentOffset, sizeof(DWORD));

	return createAndAddOpcode((sizeof(uint8_t)*2) + sizeof(DWORD), convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::mov(DWORD * address, uint8_t value)
{
	uint8_t opCodeByte = 0xC6;
	uint8_t modByte = 0x05;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&modByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&address, currentOffset, sizeof(DWORD));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&value, currentOffset, sizeof(uint8_t));
	return createAndAddOpcode((sizeof(uint8_t) * 3) + sizeof(DWORD), convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::jmpA(DWORD address)
{
	uint8_t opCodeByte = 0xFF;
	uint8_t sCode = 0x25;
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&sCode, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&address, currentOffset, sizeof(DWORD));
	return createAndAddOpcode((sizeof(uint8_t) * 2) + sizeof(DWORD), convertLocation(memoryStart, currentOffset));

}

ASMOpcode * ASMFunction::jmpA(ASMOpcode *code)
{
	return this->jmpA((DWORD)code->getLocationAddress());
}

// TODO
void ASMFunction::jmpR(DWORD address)
{

}

// TODO: Add support for scale part and mod part
ASMOpcode * ASMFunction::lea(REGISTER dest, REGISTER base, REGISTER index)
{
	uint8_t opCodeByte = 0x8D;
	uint8_t modByte = 0x0;

	modByte |= dest;
	modByte = (modByte << 3);

	// Set the addressing mode
	modByte |= 0x4;

	uint8_t sibByte = 0x0;
	sibByte |= index;
	sibByte = (sibByte << 3);
	sibByte |= base;

	writeToMemoryAndProtect(memoryStart, (LPVOID*)&opCodeByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&modByte, currentOffset, sizeof(uint8_t));
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&sibByte, currentOffset, sizeof(uint8_t));
	return createAndAddOpcode(sizeof(uint8_t) * 3, convertLocation(memoryStart, currentOffset));
}

ASMOpcode * ASMFunction::ret()
{
	BYTE instruction = 0xC3;
	writeToMemoryAndProtect(memoryStart, (LPVOID*)&instruction, currentOffset, sizeof(BYTE));
	return createAndAddOpcode(sizeof(BYTE), convertLocation(memoryStart, currentOffset));
}


ASMFunction::~ASMFunction()
{
	VirtualFreeEx(context->getContextHandle(), memoryStart, 0, MEM_RELEASE);
	memoryStart = NULL;
}
