
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#define WINVER 0x0500
#define _WIN32_WINNT 0x0500

#include <stdio.h>
#include <windows.h> 
#include <lmaccess.h>
#include <aclapi.h>
#include <sddl.h>
#include <tchar.h>

#include <string>
#include <hash_map>
#include <vector>

#include "cGEN_String.h"
#include "cGEN_HashMap.h"
#include "cGEN_CommandLine.h"

#include "cW32_SecurityIdentifier.h"
#include "cW32_AccessToken.h"


#include "bcRunAs.h"

#define DESKTOP_ALL (DESKTOP_READOBJECTS | DESKTOP_CREATEWINDOW | \
DESKTOP_CREATEMENU | DESKTOP_HOOKCONTROL | DESKTOP_JOURNALRECORD | \
DESKTOP_JOURNALPLAYBACK | DESKTOP_ENUMERATE | DESKTOP_WRITEOBJECTS | \
DESKTOP_SWITCHDESKTOP | STANDARD_RIGHTS_REQUIRED)

#define WINSTA_ALL (WINSTA_ENUMDESKTOPS | WINSTA_READATTRIBUTES | \
WINSTA_ACCESSCLIPBOARD | WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES | \
WINSTA_ACCESSGLOBALATOMS | WINSTA_EXITWINDOWS | WINSTA_ENUMERATE | \
WINSTA_READSCREEN | STANDARD_RIGHTS_REQUIRED)

#define GENERIC_ACCESS (GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | \
GENERIC_ALL)

cRAS_RuntimeArguments::cRAS_RuntimeArguments( void )
{
	return;
}
	
cRAS_RuntimeArguments::~cRAS_RuntimeArguments( void )
{
	return;
}

BOOL AddAceToWindowStation(HWINSTA hWinsta, PSID pSid)
{
	BOOL bResult;
	BOOL bDaclExist;
	BOOL bDaclPresent;
	DWORD dwError;

	ACCESS_ALLOWED_ACE *pACE;
	ACL_SIZE_INFORMATION asiAclSizeInfo;

	DWORD dwNewAclSize;
	DWORD dwSidSize = 0;
	DWORD dwSdSizeNeeded;
	
	PACL pAcl;
	PACL pAcl_new;
	
	PSECURITY_DESCRIPTOR pSecDesc = NULL;
	PSECURITY_DESCRIPTOR pSecDesc_new = NULL;
	PVOID pTempAce;
	SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
	
	unsigned int i;

	__try
	{
		// Obtain the DACL for the window station.
		bResult = GetUserObjectSecurity(
				hWinsta,
				&si,
				pSecDesc,
				dwSidSize,
				&dwSdSizeNeeded
			);
			
		if (!bResult) {
			dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				pSecDesc = (PSECURITY_DESCRIPTOR) HeapAlloc(
						GetProcessHeap(),
						HEAP_ZERO_MEMORY,
						dwSdSizeNeeded
					);

				if (pSecDesc == NULL)
					__leave;

				pSecDesc_new = (PSECURITY_DESCRIPTOR) HeapAlloc(
						GetProcessHeap(),
						HEAP_ZERO_MEMORY,
						dwSdSizeNeeded
					);

				if (pSecDesc_new == NULL)
					__leave;

				dwSidSize = dwSdSizeNeeded;

				bResult = GetUserObjectSecurity(
						hWinsta,
						&si,
						pSecDesc,
						dwSidSize,
						&dwSdSizeNeeded
					);
					
				if (!bResult)
					__leave;
			}
			else
				__leave;
		}

		// Create a new DACL.

		if (!InitializeSecurityDescriptor(
			pSecDesc_new,
			SECURITY_DESCRIPTOR_REVISION)
		)
			__leave;

		// Get the DACL from the security descriptor.

		if (!GetSecurityDescriptorDacl(
			pSecDesc,
			&bDaclPresent,
			&pAcl,
			&bDaclExist)
		)
			__leave;

		// Initialize the ACL.

		ZeroMemory(&asiAclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
		asiAclSizeInfo.AclBytesInUse = sizeof(ACL);

		// Call only if the DACL is not NULL.

		if (pAcl != NULL)
		{
			// get the file ACL size info
			bResult = GetAclInformation(
					pAcl,
					(LPVOID)&asiAclSizeInfo,
					sizeof(ACL_SIZE_INFORMATION),
					AclSizeInformation
				);
				
			if (!bResult)
				__leave;
		}

		// Compute the size of the new ACL.

		dwNewAclSize = asiAclSizeInfo.AclBytesInUse + (2*sizeof(ACCESS_ALLOWED_ACE)) + 
	(2*GetLengthSid(pSid)) - (2*sizeof(DWORD));

		// Allocate memory for the new ACL.

		pAcl_new = (PACL) HeapAlloc(
				GetProcessHeap(),
				HEAP_ZERO_MEMORY,
				dwNewAclSize
			);

		if (pAcl_new == NULL)
			__leave;

		// Initialize the new DACL.

		bResult = InitializeAcl(pAcl_new, dwNewAclSize, ACL_REVISION);
		if (!bResult)
			__leave;

		// If DACL is present, copy it to a new DACL.

		if (bDaclPresent)
		{
			// Copy the ACEs to the new ACL.
			if (asiAclSizeInfo.AceCount)
			{
			for (i=0; i < asiAclSizeInfo.AceCount; i++)
			{
				// Get an ACE.
				bResult = GetAce(pAcl, i, &pTempAce);
				if (!bResult)
					__leave;

				// Add the ACE to the new ACL.
				if (!AddAce(
						pAcl_new,
						ACL_REVISION,
						MAXDWORD,
						pTempAce,
					((PACE_HEADER)pTempAce)->AceSize)
				)
					__leave;
			}
			}
		}

		// Add the first ACE to the window station.

		pACE = (ACCESS_ALLOWED_ACE *)HeapAlloc(
			GetProcessHeap(),
			HEAP_ZERO_MEMORY,
			sizeof(ACCESS_ALLOWED_ACE) + GetLengthSid(pSid) -
					sizeof(DWORD));

		if (pACE == NULL)
			__leave;

		pACE->Header.AceType = ACCESS_ALLOWED_ACE_TYPE;
		pACE->Header.AceFlags = CONTAINER_INHERIT_ACE |
					INHERIT_ONLY_ACE | OBJECT_INHERIT_ACE;
		pACE->Header.AceSize = (WORD) (sizeof(ACCESS_ALLOWED_ACE) +
					GetLengthSid(pSid) - sizeof(DWORD));
		pACE->Mask = GENERIC_ACCESS;

		bResult = CopySid(GetLengthSid(pSid), &pACE->SidStart, pSid);
		if (!bResult)
			__leave;

		bResult = AddAce(
				pAcl_new,
				ACL_REVISION,
				MAXDWORD,
				(LPVOID) pACE,
				pACE->Header.AceSize
			);
		if (!bResult)
			__leave;

		// Add the second ACE to the window station.

		pACE->Header.AceFlags = NO_PROPAGATE_INHERIT_ACE;
		pACE->Mask            = WINSTA_ALL;

		bResult = AddAce(
				pAcl_new,
				ACL_REVISION,
				MAXDWORD,
				(LPVOID) pACE,
				pACE->Header.AceSize
			);
		if (!bResult)
			__leave;

		// Set a new DACL for the security descriptor.

		bResult = SetSecurityDescriptorDacl(
				pSecDesc_new,
				TRUE,
				pAcl_new,
				FALSE
			);
		if (!bResult) 
			__leave;

		// Set the new security descriptor for the window station.

		bResult = SetUserObjectSecurity(hWinsta, &si, pSecDesc_new);
		if (!bResult) 
			__leave;
		
	}
	__finally
	{
		// Free the allocated buffers.

		if (pACE != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID) pACE);

		if (pAcl_new != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID) pAcl_new);

		if (pSecDesc != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID) pSecDesc);

		if (pSecDesc_new != NULL)
			HeapFree(GetProcessHeap(), 0, (LPVOID) pSecDesc_new);
	}

	return bResult;
}

BOOL AddAceToDesktop(HDESK hdesk, PSID psid)
{
   ACL_SIZE_INFORMATION aclSizeInfo;
   BOOL                 bDaclExist;
   BOOL                 bDaclPresent;
   BOOL                 bSuccess = FALSE;
   DWORD                dwNewAclSize;
   DWORD                dwSidSize = 0;
   DWORD                dwSdSizeNeeded;
   PACL                 pacl;
   PACL                 pNewAcl;
   PSECURITY_DESCRIPTOR psd = NULL;
   PSECURITY_DESCRIPTOR psdNew = NULL;
   PVOID                pTempAce;
   SECURITY_INFORMATION si = DACL_SECURITY_INFORMATION;
   unsigned int         i;

   __try
   {
      // Obtain the security descriptor for the desktop object.

      if (!GetUserObjectSecurity(
            hdesk,
            &si,
            psd,
            dwSidSize,
            &dwSdSizeNeeded))
      {
         if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
         {
            psd = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded );

            if (psd == NULL)
               __leave;

            psdNew = (PSECURITY_DESCRIPTOR)HeapAlloc(
                  GetProcessHeap(),
                  HEAP_ZERO_MEMORY,
                  dwSdSizeNeeded);

            if (psdNew == NULL)
               __leave;

            dwSidSize = dwSdSizeNeeded;

            if (!GetUserObjectSecurity(
                  hdesk,
                  &si,
                  psd,
                  dwSidSize,
                  &dwSdSizeNeeded)
            )
               __leave;
         }
         else
            __leave;
      }

      // Create a new security descriptor.

      if (!InitializeSecurityDescriptor(
            psdNew,
            SECURITY_DESCRIPTOR_REVISION)
      )
         __leave;

      // Obtain the DACL from the security descriptor.

      if (!GetSecurityDescriptorDacl(
            psd,
            &bDaclPresent,
            &pacl,
            &bDaclExist)
      )
         __leave;

      // Initialize.

      ZeroMemory(&aclSizeInfo, sizeof(ACL_SIZE_INFORMATION));
      aclSizeInfo.AclBytesInUse = sizeof(ACL);

      // Call only if NULL DACL.

      if (pacl != NULL)
      {
         // Determine the size of the ACL information.

         if (!GetAclInformation(
               pacl,
               (LPVOID)&aclSizeInfo,
               sizeof(ACL_SIZE_INFORMATION),
               AclSizeInformation)
         )
            __leave;
      }

      // Compute the size of the new ACL.

      dwNewAclSize = aclSizeInfo.AclBytesInUse +
            sizeof(ACCESS_ALLOWED_ACE) +
            GetLengthSid(psid) - sizeof(DWORD);

      // Allocate buffer for the new ACL.

      pNewAcl = (PACL)HeapAlloc(
            GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            dwNewAclSize);

      if (pNewAcl == NULL)
         __leave;

      // Initialize the new ACL.

      if (!InitializeAcl(pNewAcl, dwNewAclSize, ACL_REVISION))
         __leave;

      // If DACL is present, copy it to a new DACL.

      if (bDaclPresent)
      {
         // Copy the ACEs to the new ACL.
         if (aclSizeInfo.AceCount)
         {
            for (i=0; i < aclSizeInfo.AceCount; i++)
            {
               // Get an ACE.
               if (!GetAce(pacl, i, &pTempAce))
                  __leave;

               // Add the ACE to the new ACL.
               if (!AddAce(
                  pNewAcl,
                  ACL_REVISION,
                  MAXDWORD,
                  pTempAce,
                  ((PACE_HEADER)pTempAce)->AceSize)
               )
                  __leave;
            }
         }
      }

      // Add ACE to the DACL.

      if (!AddAccessAllowedAce(
            pNewAcl,
            ACL_REVISION,
            DESKTOP_ALL,
            psid)
      )
         __leave;

      // Set new DACL to the new security descriptor.

      if (!SetSecurityDescriptorDacl(
            psdNew,
            TRUE,
            pNewAcl,
            FALSE)
      )
         __leave;

      // Set the new security descriptor for the desktop object.

      if (!SetUserObjectSecurity(hdesk, &si, psdNew))
         __leave;

      // Indicate success.

      bSuccess = TRUE;
   }
   __finally
   {
      // Free buffers.

      if (pNewAcl != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)pNewAcl);

      if (psd != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psd);

      if (psdNew != NULL)
         HeapFree(GetProcessHeap(), 0, (LPVOID)psdNew);
   }

   return bSuccess;
}

int RunAs(
		cGEN_CommandLine& p_rclCommandLine
	)
{
	BOOL bResult;

	//PSID pSid;
	
	HDESK hDesktop;
	HWINSTA hWinsta_this, hWinsta_child;

	LPVOID	lpvEnvironment;
	STARTUPINFO siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;

	cW32_SecurityIdentifier* pSId;
	cW32_AccessToken atUserToken;
	
	
	PSID psidOwner;
	PSID psidGroup;
	PACL pDAcl;
	PACL pSAcl;

	PSECURITY_DESCRIPTOR pSecurityDescriptor;
	
	LPTSTR lptstrSecDesc;
		ULONG ulLen;
	

	DWORD dw;


	bResult = atUserToken.LogonUser(
			(LPTSTR) _T("icafe"),
			(LPTSTR) _T("."),
			(LPTSTR) _T(""),
			LOGON32_LOGON_NEW_CREDENTIALS
		);

	bResult = atUserToken.LoadUserProfile();

	bResult = atUserToken.GetUserEnvironment(
			&lpvEnvironment,
			FALSE
		);

	hWinsta_this = GetProcessWindowStation();

	hWinsta_child = OpenWindowStation(
			_T("winsta0"),
			FALSE,
			READ_CONTROL | WRITE_DAC
		);
		
  	bResult = SetProcessWindowStation(
			hWinsta_child
		);
		
	hDesktop = OpenDesktop(
			_T("default"),
			0,
			FALSE,
			READ_CONTROL | WRITE_DAC | DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS
		);

	bResult = SetProcessWindowStation(
			hWinsta_this
		);

	atUserToken.GetLogonSID(&pSId);
	
	AddAceToWindowStation(
			hWinsta_child,
			pSId->GetPSID()
		);
		
	AddAceToDesktop(
			hDesktop,
			pSId->GetPSID()
		);
		
	bResult = atUserToken.ImpersonateLoggedOnUser();
	
	dw = GetNamedSecurityInfo(
			(LPWSTR) _T("CURRENT_USER\\"),
			SE_REGISTRY_KEY,
			DACL_SECURITY_INFORMATION | 
				GROUP_SECURITY_INFORMATION | 
				OWNER_SECURITY_INFORMATION,
			&psidOwner,
			&psidGroup,
			&pDAcl,
			&pSAcl,
			&pSecurityDescriptor
		);
		
	if (dw == ERROR_SUCCESS)
	{
		
	             // ConvertStringSecurityDescriptorToSecurityDescriptor
	             // ConvertSecurityDescriptorToStringSecurityDescriptor;
	
		ConvertSecurityDescriptorToStringSecurityDescriptor(
				pSecurityDescriptor,
				SDDL_REVISION,
				(SECURITY_INFORMATION) OWNER_SECURITY_INFORMATION |
					GROUP_SECURITY_INFORMATION |
					DACL_SECURITY_INFORMATION |
					SACL_SECURITY_INFORMATION,
				&lptstrSecDesc,
				&ulLen
			);
		/*	 */
	}
	
	HKEY hKey;
	
	#define POLICY_KEY _T("Software\\Policies\\Microsoft\\Windows")
	
	long lResult = RegOpenCurrentUser(KEY_ALL_ACCESS, &hKey);
    if (lResult == ERROR_SUCCESS)
    {
        //lResult = RegQueryValueEx (hKey, lpValueName, 0, &dwType,                          (LPBYTE) &dwValue, &dwSize);
        RegCloseKey (hKey);
        
      

        
    }
    else
    {
		dw = GetLastError();
    }

    bResult = RefreshPolicy(FALSE);
    if (!bResult)
    {
		dw = GetLastError();
    
    }

/*DWORD GetSecurityInfo(
  HANDLE handle,
  SE_OBJECT_TYPE ObjectType,
  SECURITY_INFORMATION SecurityInfo,
  PSID* ppsidOwner,
  PSID* ppsidGroup,
  PACL* ppDacl,
  PACL* ppSacl,
  PSECURITY_DESCRIPTOR* ppSecurityDescriptor
);*/

	ZeroMemory(&siStartupInfo, sizeof(STARTUPINFO));

	siStartupInfo.cb = sizeof(STARTUPINFO);
	siStartupInfo.lpDesktop = _T("winsta0\\default");
	siStartupInfo.lpReserved = NULL;
	siStartupInfo.lpDesktop = NULL;
	siStartupInfo.lpTitle = NULL;
	siStartupInfo.dwX = CW_USEDEFAULT;
	siStartupInfo.dwY = CW_USEDEFAULT;
	siStartupInfo.dwFlags = NULL;
	siStartupInfo.cbReserved2 = NULL;
	siStartupInfo.lpReserved2 = NULL;

	bResult = atUserToken.CreateProcessAsUser(
			NULL,
			(LPTSTR) _T("C:\\WINDOWS\\system32\\cmd.exe /C \"gpupdate >> c:\\usr\\tmp.xx\""),
			NULL,
			NULL,
			TRUE,
			CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP | CREATE_UNICODE_ENVIRONMENT,
			lpvEnvironment,
			NULL,
			&siStartupInfo,
			&piProcessInfo
		);
		
	bResult = atUserToken.DestroyEnvironmentBlock(
			&lpvEnvironment
		);

	//FreeLogonSID()
	
	CloseWindowStation(hWinsta_child);
	
	CloseDesktop(hDesktop);

	return 0;	
}




int determineRuntimeArguments(
		cGEN_CommandLine& p_rclCommandLine,
		cRAS_RuntimeArguments& p_rRuntimeArguments
	)
{
	BOOL bResult;
	LONG i, i_max;

	cGEN_CommandLine::enumComponentType eComponentType;
	cGEN_String sSwitch;
	cGEN_String sValue;
	
	i_max = p_rclCommandLine.count();
	for (i = 0; i < i_max; i++) {
		bResult = p_rclCommandLine.lookup(
				i,
				eComponentType,
				sSwitch,
				sValue
			);
			
		if (bResult)
		{		
			switch (eComponentType)
			{
			case cGEN_CommandLine::CL_COMPONENT_TYPE_SWITCH_VALUE:
				
				if (sSwitch == _T("?")) {
					p_rRuntimeArguments.bQuestionMark = true;
				} else if (sSwitch == _T("help")) {
					p_rRuntimeArguments.bHelp = false;
				}
			
				break;
				
			case cGEN_CommandLine::CL_COMPONENT_TYPE_VALUE:
			
				break;
				
			default:
			
				break;
			}
		}
	}


	return true;
}

int wmain(int argc, wchar_t *argv[])
{
	BOOL bResult;

	cGEN_CommandLine clCommandLine;
	
	bResult = clCommandLine.parse(
			argc,
			argv
		);

	// refresh policy works with logon new credentials
		
	
	// determine the switches, and execution settings.


	if (false) {
	
	} else {
		return RunAs(
				clCommandLine
			);
	}

	return 0x00;
}
