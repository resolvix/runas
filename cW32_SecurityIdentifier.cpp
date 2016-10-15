
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>

#include "cW32_SecurityIdentifier.h"

cW32_SecurityIdentifier::cW32_SecurityIdentifier(
		PSID pSId
	)
{
	m_pSId = pSId;
}

cW32_SecurityIdentifier::~cW32_SecurityIdentifier(void)
{
	 HeapFree(
			GetProcessHeap(), 
			0, 
			(LPVOID) m_pSId
		);
}


PSID cW32_SecurityIdentifier::GetPSID(void)
{
	return m_pSId;
}


