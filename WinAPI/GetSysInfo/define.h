#pragma once

#ifdef __cplusplus
extern "C"
{
#endif	 

#define MYIMAPI extern "C" __declspec(dllimport)

#define MYEXAPI extern "C" __declspec(dllexport)

#ifdef __cplusplus
}
#endif