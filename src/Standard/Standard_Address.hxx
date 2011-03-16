//============================================================================
//==== Titre: Standard_Address.hxx
//==== Role : The headr file of primitve type "Address" from package "Standard"
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef char* Address;
//============================================================================

#ifndef _Standard_Address_HeaderFile
#define _Standard_Address_HeaderFile

# include <string.h>
//typedef void* Standard_Address;


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif


class Handle_Standard_Type;

__Standard_API Handle_Standard_Type& Standard_Address_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_Address, Standard_OStream& );
//============================================================================
//==== ShallowCopy: Returns a CString
//============================================================================
inline Standard_Address ShallowCopy (const Standard_Address Value)
{
  return Value;
}

//============================================================================
//==== HashCode : Returns a HashCode CString
//============================================================================
inline Standard_Integer HashCode (const Standard_Address Value, 
				  const Standard_Integer Upper)
{
  union {Standard_Address L ;
         Standard_Integer I[2] ;} U ;
  U.I[0] = 0 ;
  U.I[1] = 0 ;
  U.L = Value;
  return HashCode( ( ( U.I[0] ^ U.I[1] ) & 0x7fffffff )  , Upper ) ;
}


//============================================================================
//==== IsSimilar : Returns Standard_True if two booleans have the same value
//============================================================================
inline Standard_Boolean IsSimilar(const Standard_Address One
				 ,const Standard_Address Two)
{ return One == Two; }

//============================================================================
// IsEqual : Returns Standard_True if two CString have the same value
//============================================================================

inline Standard_Boolean IsEqual(const Standard_Address One
			       ,const Standard_Address Two)
{ return One == Two; }

#endif
