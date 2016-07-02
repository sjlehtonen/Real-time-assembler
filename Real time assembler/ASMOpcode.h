#pragma once
#include "ASMContext.h"

class ASMFunction;

class ASMOpcode
{
private:
	size_t size;
	LPVOID location;
	ASMFunction *function;

public:
	ASMOpcode(size_t size, LPVOID location, ASMFunction *function);
	LPVOID getLocation();
	LPVOID *getLocationAddress();
	~ASMOpcode();
};

