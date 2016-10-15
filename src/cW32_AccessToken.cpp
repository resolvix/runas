#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <userenv.h>
#include <lmaccess.h>
#include <lm.h>

#include "cW32_SecurityIdentifier.h"
#include "cW32_AccessToken.h"

cW32_AccessToken::cW32_AccessToken(void)
{

}

cW32_AccessToken::~cW32_AccessToken(void)
{
	BOOL bResult;

	bResult = ::UnloadUserProfile(
			m_hUserToken,
			m_hProfile
		);

	bResult = CloseHandle(
			m_hUserToken
		);


}

BOOL cW32_AccessToken::DestroyEnvironmentBlock(
		LPVOID* lpEnvironment
	)
{
	BOOL bResult;
		
	bResult = ::DestroyEnvironmentBlock(
			lpEnvironment
		);

	return bResult;
}

BOOL cW32_AccessToken::GetLogonSID(
		cW32_SecurityIdentifier** ppSId
	)
{
	BOOL bResult;

	DWORD dwIndex;
	DWORD dwLength = 0;
	
	PSID pSId;
	
	PTOKEN_GROUPS ptg = NULL;

	// Verify the parameter passed in is not NULL.
	if (NULL == ppSId)
		goto Cleanup;

	// Get required buffer size and allocate the TOKEN_GROUPS buffer.
	bResult = GetTokenInformation(
			m_hUserToken,
			TokenGroups,
			(LPVOID) ptg,
			0,
			&dwLength
		);

	if (!bResult) 
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
			goto Cleanup;

		ptg = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(),
			HEAP_ZERO_MEMORY, dwLength);

		if (ptg == NULL)
			goto Cleanup;
	}

	// Get the token group information from the access token.
	bResult = GetTokenInformation(
			m_hUserToken,
			TokenGroups,
			(LPVOID) ptg,
			dwLength,
			&dwLength
		);	
	
	if (!bResult) 
	{
		goto Cleanup;
	}

	// Loop through the groups to find the logon SID.

	for (dwIndex = 0; dwIndex < ptg->GroupCount; dwIndex++) 
		if ((ptg->Groups[dwIndex].Attributes & SE_GROUP_LOGON_ID)
				==  SE_GROUP_LOGON_ID) 
		{
		// Found the logon SID; make a copy of it.

			dwLength = GetLengthSid(ptg->Groups[dwIndex].Sid);
			pSId = (PSID) HeapAlloc(GetProcessHeap(),
						HEAP_ZERO_MEMORY, dwLength);
						
			if (pSId == NULL)
				goto Cleanup;
				
			if (!CopySid(dwLength, pSId, ptg->Groups[dwIndex].Sid)) 
			{
				HeapFree(GetProcessHeap(), 0, (LPVOID) pSId);
				goto Cleanup;
			}
			break;
		}

	bResult = TRUE;
	
	*ppSId = new cW32_SecurityIdentifier(pSId);

	Cleanup: 

	// Free the buffer for the token groups.

	if (ptg != NULL)
		HeapFree(GetProcessHeap(), 0, (LPVOID)ptg);

	return bResult;
}


BOOL cW32_AccessToken::GetUserEnvironment(
		LPVOID* lpEnvironment,
		BOOL bInherit
	)
{
	BOOL bResult;

	bResult = ::CreateEnvironmentBlock(
			lpEnvironment,
			m_hUserToken,
			bInherit
		);

    return bResult;
}

BOOL cW32_AccessToken::CreateProcessAsUser(
		LPCTSTR lpApplicationName,
		LPTSTR lpCommandLine,
		LPSECURITY_ATTRIBUTES lpProcessAttributes,
		LPSECURITY_ATTRIBUTES lpThreadAttributes,
		BOOL bInheritHandles,
		DWORD dwCreationFlags,
		LPVOID lpEnvironment,
		LPCTSTR lpCurrentDirectory,
		LPSTARTUPINFO lpStartupInfo,
		LPPROCESS_INFORMATION lpProcessInformation
	)
{
	BOOL bResult;
	DWORD dw;

	bResult = ::CreateProcessAsUser(
			m_hUserToken,
			lpApplicationName,
			lpCommandLine,
			lpProcessAttributes,
			lpThreadAttributes,
			bInheritHandles,
			dwCreationFlags,
			lpEnvironment,
			lpCurrentDirectory,
			lpStartupInfo,
			lpProcessInformation
		);

	if (!bResult) {
	
		dw = GetLastError();

	}

	return bResult;
}


BOOL cW32_AccessToken::ImpersonateLoggedOnUser(void)
{
	BOOL bResult;

	bResult = ::ImpersonateLoggedOnUser(
			m_hUserToken
		);

	return bResult;
}


BOOL cW32_AccessToken::LoadUserProfile(void)
{
	BOOL bResult;

	PROFILEINFO stProfileInfo;

/*
		typedef struct _PROFILEINFO { 
			DWORD dwSize;  
			DWORD dwFlags;  
			LPTSTR lpUserName;  
			LPTSTR lpProfilePath;  
			LPTSTR lpDefaultPath; 
			LPTSTR lpServerName;  
			LPTSTR lpPolicyPath;  
			HANDLE hProfile;
		} PROFILEINFO, *LPPROFILEINFO;*/

	stProfileInfo.dwSize = sizeof(PROFILEINFO);
	stProfileInfo.lpUserName = (LPTSTR) m_pusri4UserInfo->usri4_name;
	stProfileInfo.lpProfilePath = (LPTSTR) m_pusri4UserInfo->usri4_profile;

	bResult = ::LoadUserProfile(
			m_hUserToken,
			&stProfileInfo
		);

	if (bResult) {
		m_hProfile = stProfileInfo.hProfile;
	}

	return bResult;
}


BOOL cW32_AccessToken::LogonUser(
		LPTSTR lpszUsername,
		LPTSTR lpszDomain,
		LPTSTR lpszPassword,
		DWORD dwLogonType,
		DWORD dwLogonProvider
	)
{
	BOOL bResult;

	DWORD dw;

	NET_API_STATUS naStatus;

	//ZeroMemory(&m_usri4UserInfo, sizeof(USER_INFO_4));

	//		Obtain user information.
	naStatus = ::NetUserGetInfo(
			(LPWSTR) lpszDomain,
			(LPWSTR) lpszUsername,
			3,
			(LPBYTE *) &m_pusri4UserInfo
		);

	if (naStatus == NERR_Success) {

	}

	bResult = ::LogonUser(
			lpszUsername,
			lpszDomain,
			lpszPassword,
			dwLogonType,
			dwLogonProvider,
			&m_hUserToken
		);

	if (!bResult) {
		
		dw = GetLastError();

	}

	return bResult;
}


BOOL cW32_AccessToken::UnloadUserProfile(void)
{
	BOOL bResult;

	bResult = ::UnloadUserProfile(
			m_hUserToken,
			m_hProfile
		);

	return bResult;
}
