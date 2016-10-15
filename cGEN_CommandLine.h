#ifndef __GEN_COMMAND_LINE__
#define __GEN_COMMAND_LINE__

//#define LPCTSTR const TCHAR *
//#define BOOL bool

class cGEN_CommandLine
{
public:

	typedef enum _enumComponentType
	{
		CL_COMPONENT_TYPE_SWITCH_VALUE,
		CL_COMPONENT_TYPE_VALUE	
	}
	enumComponentType;
	
protected:

	class eclComponent
	{
	protected:
	
		enumComponentType m_eComponentType;
		cGEN_String m_sSwitch;
		cGEN_String m_sValue;
		
	public:
		
		eclComponent( void );
		
		eclComponent(
				enumComponentType p_eComponentType,
				cGEN_String p_sSwitch,
				cGEN_String p_sValue
			);
			
		virtual ~eclComponent( void );
		
		friend cGEN_CommandLine;
		
	};

	typedef cGEN_HashMap<
			cGEN_String,
			eclComponent,
			cGEN_String::eHashCompare
		> eclhtComponent;
		
	typedef std::pair<
			cGEN_String,
			eclComponent
		> eclhtComponentPair;
		
	typedef std::pair<
			eclhtComponent::iterator,
			BOOL
		> eclhtResultPair;
		
	typedef std::vector<
			eclComponent
		> eclseqComponent;
	
	//	Hash table, mapping switch names to (optional) values.
	eclhtComponent m_clhtComponent;
	
	//	Sequence, mapping argument position to switch, and (optional) values.
	eclseqComponent m_clseqComponent;

public:

	LONG count( void );
		
	BOOL lookup(
			LONG p_lIndex,
			enumComponentType& p_eComponentType,
			cGEN_String& p_sSwitch,
			cGEN_String& p_sValue
		);

	BOOL parse(
			int argc,
			wchar_t *argv[]
		);

};

#endif
