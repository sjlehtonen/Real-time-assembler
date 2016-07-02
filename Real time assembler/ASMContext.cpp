#include "stdafx.h"
#include "ASMContext.h"


ASMContext::ASMContext(HANDLE process)
{
	if (process != NULL)
		processHandle = process;
	else
		processHandle = GetCurrentProcess();
}

ASMContext::ASMContext()
{
	processHandle = GetCurrentProcess();
}

ASMFunction * ASMContext::createFunction()
{
	ASMFunction * f = new ASMFunction(this);
	functions.push_back(f);
	return f;
}

void ASMContext::destroyFunction(ASMFunction *function)
{
	for (std::vector<ASMFunction*>::iterator it = functions.begin(); it != functions.end(); it++)
	{
		if ((*it)->getStartAddress() == function->getStartAddress())
		{
			functions.erase(it);
			break;
		}
	}

	delete function;
}

HANDLE ASMContext::getContextHandle()
{
	return processHandle;
}


ASMContext::~ASMContext()
{
}
