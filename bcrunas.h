

class cRAS_RuntimeArguments
{
public:

	BOOL bQuestionMark;
	BOOL bHelp;
	
	BOOL bProfile;
	BOOL bNoProfile;
	
	BOOL bEnvironment;
	
	BOOL bDomainUser;
	cGEN_String sDomain;
	cGEN_String sUser;

	BOOL bPassword;
	cGEN_String sPassword;
	
	BOOL bLogonType;
	cGEN_String sLogonType;
	
	cRAS_RuntimeArguments( void );
	
	virtual ~cRAS_RuntimeArguments( void );
		
};




