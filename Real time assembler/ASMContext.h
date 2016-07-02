#pragma once

#include <Windows.h>
#include <stdint.h>
#include <assert.h>
#include "ASMFunction.h"
#include <iostream>
#include <vector>

class ASMFunction;

class ASMContext 
{

private:
	HANDLE processHandle;
	std::vector<ASMFunction*> functions;

public:
	ASMFunction * createFunction();
	void destroyFunction(ASMFunction *function);

	ASMContext(HANDLE process);
	ASMContext();
	HANDLE getContextHandle();
	~ASMContext();
};

