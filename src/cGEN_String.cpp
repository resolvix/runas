#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <string>
#include <hash_map>

#include "cGEN_String.h"

size_t cGEN_String::eHashCompare::operator()( 
		const cGEN_String& p_rString
	) const
{
	return 0;
}

bool cGEN_String::eHashCompare::operator()(
			const cGEN_String& p_rString1,
			const cGEN_String& p_rString2
		) const
{	// test if _Keyval1 ordered before _Keyval2
	return (p_rString1 < p_rString2);
}

cGEN_String::cGEN_String( void ) : std::wstring()
{

}

cGEN_String::cGEN_String(
		const wchar_t *p_pwch
	) : std::wstring(p_pwch)
{
	return;
}

cGEN_String& cGEN_String::operator=(
		const wchar_t *p_pwch
	)
{
	std::wstring::operator=(p_pwch);
	return *this;
}

cGEN_String& cGEN_String::operator+=(
		const wchar_t p_wch
	)
{
	std::wstring::operator+=(p_wch);
	return *this;
}

cGEN_String& cGEN_String::operator+=(
		const wchar_t *p_pwch
	)
{
	std::wstring::operator+=(p_pwch);
	return *this;
}

cGEN_String& cGEN_String::operator+=(
		const cGEN_String &p_rString
	)
{
	std::wstring::operator+=(p_rString);
	return *this;
}

bool operator==(
		const cGEN_String& p_rString_1,
		const cGEN_String& p_rString_2
	)
{
	return std::operator==(
			(std::wstring) p_rString_1,
			(std::wstring) p_rString_2
		);
}
		
bool operator==(
		const cGEN_String& p_rString_1,
		const wchar_t* p_pwchString_2
	)
{
	return std::operator==(
			(std::wstring) p_rString_1,
			(std::wstring) p_pwchString_2
		);
}

bool operator<(
		const cGEN_String& p_rString_1,
		const cGEN_String& p_rString_2
	)
{
	return std::operator<(
			(std::wstring) p_rString_1,
			(std::wstring) p_rString_2
		);
}
