/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "globals.h"
#include <io.h>
#include <fcntl.h>

#define _MYDEBUG

#ifdef _MYDEBUG
#define kprintf	kprintf_internal
#else
#define kprintf __noop
#endif

FILE * logfile;
//#ifdef _WINDLL
wchar_t * outputBuffer;
size_t outputBufferElements, outputBufferElementsPosition;
//#endif

void kprintf_internal(PCWCHAR format, ...);
void kprintf_inputline(PCWCHAR format, ...);

BOOL kull_m_output_file(PCWCHAR file);

void kull_m_output_init();
void kull_m_output_clean();