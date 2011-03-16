//============================================================================
//==== Titre: Standard_Boolean.hxx
//==== Role : The headr file of primitve type "Boolean" from package "Standard"
//==== 
//==== Implementation:  This is a primitive type implementadef with typedef
//====        typedef int Standard_Boolean
//====        #define Standard_False 0
//====        #define Standard_True  1
//============================================================================

#ifndef _Standard_Boolean_HeaderFile
#define _Standard_Boolean_HeaderFile

//typedef unsigned int Standard_Boolean;


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API Handle_Standard_Type& Standard_Boolean_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_Boolean, Standard_OStream& );
// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - Hascode
//    - IsEqual
//    - IsSimilar
//    - Shallowcopy
//    - ShallowDump
// ===============================================


// ==================================
// Method implemented in Standard_Boolean.cxx
// ==================================
__Standard_API Standard_Boolean ShallowCopy (const Standard_Boolean); 
//__Standard_API Standard_Integer HashCode    (const Standard_Boolean, const Standard_Integer);

// ===============
// inline methods 
// ===============

// ------------------------------------------------------------------
// IsEqual : Returns Standard_True if two booleans have the same value
// ------------------------------------------------------------------
//inline Standard_Boolean IsEqual(const Standard_Boolean One
//			       ,const Standard_Boolean Two)
//{ return One == Two; }

// ------------------------------------------------------------------
// IsSimilar : Returns Standard_True if two booleans have the same value
// ------------------------------------------------------------------
inline Standard_Boolean IsSimilar(const Standard_Boolean One
				 ,const Standard_Boolean Two)
{ return One == Two; }


#endif
