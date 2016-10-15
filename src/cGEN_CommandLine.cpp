#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif


#include <stdio.h>
#include <windows.h>
#include <tchar.h>

#include <vector>
#include <hash_map>

#include "cGEN_String.h"
#include "cGEN_HashMap.h"
#include "cGEN_CommandLine.h"

cGEN_CommandLine::eclComponent::eclComponent( void )
{

}
		
cGEN_CommandLine::eclComponent::eclComponent(
		enumComponentType p_eComponentType,
		cGEN_String p_sSwitch,
		cGEN_String p_sValue
	)
{
	m_eComponentType = p_eComponentType;
	m_sSwitch = p_sSwitch;
	m_sValue = p_sValue;
	
	return;
}

cGEN_CommandLine::eclComponent::~eclComponent( void )
{
	return;
}

LONG cGEN_CommandLine::count( void )
{
	return m_clseqComponent.size();
}

BOOL cGEN_CommandLine::lookup(
		LONG p_lIndex,
		enumComponentType& p_reComponentType,
		cGEN_String& p_rsSwitch,
		cGEN_String& p_rsValue
	)
{
	eclComponent clComponent;

	try
	{
		clComponent = m_clseqComponent[p_lIndex];
		p_reComponentType = clComponent.m_eComponentType;
		p_rsSwitch = clComponent.m_sSwitch;
		p_rsValue = clComponent.m_sValue;		
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
}

BOOL cGEN_CommandLine::parse(
		int argc,
		wchar_t *argv[]
	)
{
	LONG i, j;
	
	wchar_t ch;
	wchar_t *arg;
	
	cGEN_String sSwitch;
	cGEN_String sValue;
	
	eclComponent clComponent;
	
	eclhtResultPair clhtResult;
	
	//	1.	Step through the arguments provided, determining whether the argument
	//		is a switch, a switch/value or other command line component. 
	i = 0;
	while (i < argc)
	{
		j = 0;
		sSwitch = _T(""); sValue = _T("");
		arg = argv[i++];
		ch = arg[j++];
		if (ch == _T('-') || ch == _T('/'))
		{
			ch = arg[j++];
			if (ch == _T('-')) { ch = arg[j++]; }
			while (ch != _T('\0') && ch != _T(':') && ch != _T('=')) {
				sSwitch += ch;
				ch = arg[j++];
			}
			
			if (ch == _T(':') || ch == _T('='))	{
				ch = arg[j++];
				while (ch != _T('\0')) {
					sValue += ch;
					ch = arg[j++];
				}
			}
			
			
			clComponent = eclComponent(
					CL_COMPONENT_TYPE_SWITCH_VALUE,
					sSwitch,
					sValue
				);
				
			clhtResult = m_clhtComponent.insert(
					eclhtComponentPair(
							sSwitch,
							clComponent
						)
				);
		}
		else
		{
			// not a switch
			while (ch != _T('\0')) {
				sValue += ch;
				ch = arg[j++];
			}
			
			clComponent = eclComponent(
					CL_COMPONENT_TYPE_VALUE,
					cGEN_String(_T("")),
					sValue
				);
		}
		
		m_clseqComponent.push_back(
				clComponent
			);
	}
	
	return true;
}