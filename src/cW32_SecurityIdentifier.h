
#ifndef __W32_SECURITY_IDENTIFIER__
#define __W32_SECURITY_IDENTIFIER__

class cW32_SecurityIdentifier
{
protected:

	PSID m_pSId;

public:

	cW32_SecurityIdentifier(
			PSID pSId
		);

	virtual ~cW32_SecurityIdentifier(void);

	PSID GetPSID(void);

};

#endif