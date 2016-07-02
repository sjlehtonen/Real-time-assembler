#include "stdafx.h"
#include "ASMOpcode.h"


ASMOpcode::ASMOpcode(size_t size, LPVOID location, ASMFunction *function)
{
	this->size = size;
	this->location = location;
	this->function = function;
}

LPVOID ASMOpcode::getLocation()
{
	return location;
}

LPVOID* ASMOpcode::getLocationAddress()
{
	return &location;
}



ASMOpcode::~ASMOpcode()
{
}
