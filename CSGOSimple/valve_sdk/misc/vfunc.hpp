#pragma once

template <typename FuncType>
__forceinline static FuncType CallVFunction(void* ppClass, int index)
{
	int* pVTable = *(int**)ppClass; //-V206
	int dwAddress = pVTable[index]; //-V108
	return (FuncType)(dwAddress);
}
