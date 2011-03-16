#include <Vrml_Instancing.ixx>
#include <TCollection_AsciiString.hxx>

Vrml_Instancing::Vrml_Instancing(const TCollection_AsciiString& aString)
{
 TCollection_AsciiString tmpAS = aString;
 tmpAS.ChangeAll(' ', '_', Standard_True);
 myName = tmpAS;
}

 Standard_OStream& Vrml_Instancing::DEF(Standard_OStream& anOStream) const
{
 anOStream  << "DEF " << myName << endl;
 return anOStream;
}

 Standard_OStream& Vrml_Instancing::USE(Standard_OStream& anOStream) const
{
 anOStream  << "USE " << myName << endl;
 return anOStream;
}
