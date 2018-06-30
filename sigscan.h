#pragma once

#include <string>
#include <windows.h>

bool DataCompare(char* data, char* pattern)
{
	for (int i = 0; *pattern; ++data, ++pattern, i++)
	{
		if (*pattern != '?' && *data != *pattern)
			return false;
	}

	return *pattern == 0;
}


#define	chk(dst, flag)	((dst) &  (flag))

inline bool _readptrf(void* p)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (!VirtualQuery(p, &mbi, sizeof(mbi)))
		return true;

	bool good =
		chk(mbi.Protect, PAGE_READONLY) ||
		chk(mbi.Protect, PAGE_READWRITE) ||
		chk(mbi.Protect, PAGE_WRITECOPY) ||
		chk(mbi.Protect, PAGE_EXECUTE_READ) ||
		chk(mbi.Protect, PAGE_EXECUTE_READWRITE) ||
		chk(mbi.Protect, PAGE_EXECUTE_WRITECOPY);

	if (chk(mbi.Protect, PAGE_GUARD) || chk(mbi.Protect, PAGE_NOACCESS))
		good = false;

	return !good;
}
namespace utils {
	namespace memory {
		void* FindSig(void* addr, const char *sig, int range, int offset = 0);
		void* FindByAddress(void* addr, unsigned int sig, int range);
		void *FindString(void* address, const char* string, int len = 0);
		void *FindSubStart(void *address, int maxsearch = 0);
		void *FindSubEnd(void *address, int maxsearch = 0);
		int FindSubSize(void *address, int maxsearch = 0);
		void *CalcAbsAddress(void*address);
		void* FindRef(void* base, int n, ...);
	}
}
void* utils::memory::FindSig(void* addr, const char *signature, int range, int offset)
{
	if (addr)
	{
		for (int i = 0; i < range; i++)
		{

			if (DataCompare(((char*)addr + i), (char*)signature))
			{
				return (char*)addr + i + offset;
			}
		}
	}

	return nullptr;
}

void* utils::memory::FindByAddress(void* addr, unsigned int signature, int range)
{
	if (addr)
	{
		for (int i = 0; i < range; i++)
		{
			if (*(unsigned int*)addr + i == signature) {
				return (char*)addr + i;
			}
		}
	}

	return nullptr;
}

void *utils::memory::FindString(void *p, const char *string, int len)
{
	if (!p)
		return 0;

	char *start, *str = 0;
	int fag = 0;

	int stringlen = strlen(string);

	try
	{
		for (start = (char *)p; !str; start++)
		{
			fag++;

			if (len && len != 0 && fag > len)
				return nullptr;


			if (memcmp(start, string, stringlen) == 0)
			{
				str = start;
				break;
			}


		}

		for (;; start--)
		{

			if (*(char **)start == str)
				return start;
		}
	}

	catch (...)
	{
		start = 0;//start = (char*)utils::memory::FindString(p, string, len);
		return 0;
	}


	return 0;
}

void *utils::memory::FindSubStart(void *address, int maxsearch)
{
	for (register unsigned char *i = (unsigned char *)address; maxsearch != 1; i--)
	{
		maxsearch--;

		if (maxsearch == 1)
			break;

		if (*i >> 4 == 0x5 && *(i + 1) == 0x8b && (*(i + 2) & 0xf) == 0xc)
			return i;
	}

	return 0;
}

void *utils::memory::FindSubEnd(void *address, int maxsearch)
{
	for (unsigned char *i = (unsigned char *)address; maxsearch != 1; i++)
	{
		maxsearch--;

		if (maxsearch == 1)
			break;

		if (*(unsigned char*)i == 0xC3 || *(unsigned char*)i == 0xC2)
			return i;
	}

	return 0;
}


int utils::memory::FindSubSize(void *address, int maxsearch)
{

	int start = (int)utils::memory::FindSubStart(address, maxsearch);
	int end = (int)utils::memory::FindSubEnd(address, maxsearch);

	if (!start)
		return 0;

	if (!end)
		return 0;

	return end - start;
}

void *utils::memory::CalcAbsAddress(void *a)
{
	//MessageBoxA(nullptr, "lol", "CALC ABS\n", MB_OK);
	return (char *)a + *(int *)a + sizeof(int);
}


void* utils::memory::FindRef(void*base, int n, ...)
{
	va_list v;
	va_start(v, n);

	for (int i = 0; i < n; i++)
	{
		const char* sig = va_arg(v, const char*);

		if (void* ref = FindSig(base, sig, 0x3D5720))
			return (char*)ref + strlen(sig);
	}

	return 0;
}