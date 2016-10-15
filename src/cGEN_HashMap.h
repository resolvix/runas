

template<
		typename _Kty,
		typename _Ty,
		typename _Tr = std::hash_compare<
				_Kty, 
				std::less<_Kty>
			>,
		typename _Alloc = std::allocator<
				std::pair<const _Kty, _Ty>
			>
	>
class cGEN_HashMap : public std::hash_map<
		_Kty,
		_Ty,
		_Tr,
		_Alloc
	>
{

};