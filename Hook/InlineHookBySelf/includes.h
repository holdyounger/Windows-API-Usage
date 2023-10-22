#pragma once
#include <Windows.h>
#include <stdio.h>

BYTE* Hook(BYTE* pbyFun, BYTE* pbyHkFun, SIZE_T szHkSize);
BYTE* unHook(BYTE* pbyFun, BYTE* pbyHkFun, SIZE_T szHkSize);

