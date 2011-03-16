
//============================================================================
//==== Titre: Standard_ExtCharacter.cxx
//==== Role : the methodes which raise the exceptions are implemented in 
//====        this file.
//============================================================================
#include <stdlib.h>
#include <Standard_ExtCharacter.hxx>
#include <Standard_Integer.hxx>
#include <Standard_RangeError.hxx>

#include <Standard_Type.hxx>
#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

Handle_Standard_Type& Standard_ExtCharacter_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_ExtCharacter",sizeof(Standard_ExtCharacter),
		      0,NULL);
  
  return _aType;
}


// ------------------------------------------------------------------
// Hascode : Computes a hascoding value for a given ExtCharacter
// ------------------------------------------------------------------
Standard_Integer HashCode(const Standard_ExtCharacter me, 
			  const Standard_Integer Upper)
{
  if (Upper < 1){
    Standard_RangeError::
      Raise("Try to apply HashCode method with negative or null argument.");
  }
  Standard_Integer aCode = me; 
  return ( aCode % Upper) + 1;
}

// ------------------------------------------------------------------
// ShallowDump : Writes a character
// ------------------------------------------------------------------
Standard_EXPORT void ShallowDump (const Standard_ExtCharacter Value 
				  ,Standard_OStream& s)
{   
  s << hex << Value << " Standard_ExtCharacter" << endl;
}
