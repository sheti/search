#include <tchar.h>

struct linezap
{
	TCHAR left[1024];
	TCHAR right[1024];
	unsigned int left_lenght;

	linezap *pref;
	linezap *next;
};