class cGEN_String : protected std::wstring
{
public:

	class eHashCompare : public std::hash_compare<
			cGEN_String,
			std::less<cGEN_String>
		>
	{
	public:

		size_t operator()( 
				const cGEN_String& p_rString
			) const;
			
		bool operator()(
				const cGEN_String& p_rString1,
				const cGEN_String& p_rString2
			) const;
		
	};
	
	friend eHashCompare;
	
	cGEN_String( void );

	cGEN_String(
			const wchar_t* p_pwch
		);

	cGEN_String& operator=(
			const wchar_t* p_pwch
		);
		
	cGEN_String& operator+=(
			const wchar_t p_wch
		);

	cGEN_String& operator+=(
			const wchar_t* p_pwch
		);

	cGEN_String& operator+=(
			const cGEN_String& p_rString
		);
		
	friend bool operator==(
			const cGEN_String& p_rString_1,
			const cGEN_String& p_rString_2
		);
		
	friend bool operator==(
			const cGEN_String& p_rString_1,
			const wchar_t* p_pwchString_2
		);
		
	friend bool operator<(
			const cGEN_String& p_rString_1,
			const cGEN_String& p_rString_2
		);

	
};

