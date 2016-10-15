#include <userenv.h>
#include <lmaccess.h>

#ifndef __W32_ACCESS_TOKEN__
#define __W32_ACCESS_TOKEN__

class cW32_AccessToken
{
protected:

	HANDLE m_hUserToken;
	HANDLE m_hProfile;

	USER_INFO_4* m_pusri4UserInfo;

public:

	cW32_AccessToken(void);

	virtual ~cW32_AccessToken(void);

	BOOL GetLogonSID(
			cW32_SecurityIdentifier** ppSId
		);

	BOOL GetUserEnvironment(
			LPVOID* lpEnvironment,
			BOOL bInherit
		);
	
	BOOL LogonUser(
			LPTSTR lpszUsername,
			LPTSTR lpszDomain,
			LPTSTR lpszPassword,
			DWORD dwLogonType = LOGON32_LOGON_INTERACTIVE,
			DWORD dwLogonProvider = LOGON32_PROVIDER_DEFAULT
		);

	/* TEMPORARY */
	BOOL DestroyEnvironmentBlock(
			LPVOID* lpEnvironment
		);

	BOOL CreateProcessAsUser(
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
		);

	BOOL ImpersonateLoggedOnUser(void);

	BOOL LoadUserProfile(void);

	BOOL UnloadUserProfile(void);

};

#endif