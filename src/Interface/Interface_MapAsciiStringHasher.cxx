#include <Interface_MapAsciiStringHasher.ixx>
#include <TCollection_AsciiString.hxx>
  
//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer Interface_MapAsciiStringHasher::HashCode(const TCollection_AsciiString& K,
						 const Standard_Integer Upper)
{
  return ::HashCode(K.ToCString(),Upper);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean Interface_MapAsciiStringHasher::IsEqual(const TCollection_AsciiString& K1,
						const TCollection_AsciiString& K2)
{
  if(!K1.Length() || !K2.Length()) return Standard_False;
  return K1.IsEqual(K2);
}
