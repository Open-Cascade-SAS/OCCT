#include <Vrml_Info.ixx>

Vrml_Info::Vrml_Info(const TCollection_AsciiString& aString)
{
  myString = aString;
}

 void Vrml_Info::SetString(const TCollection_AsciiString& aString) 
{
  myString = aString;
}

 TCollection_AsciiString Vrml_Info::String() const
{
  return myString; 
}

 Standard_OStream& Vrml_Info::Print(Standard_OStream& anOStream) const
{
 anOStream  << "Info {" << endl;

 if ( !(myString.IsEqual( "<Undefined info>") ) )
   {
    anOStream  << "    string" << '\t';
    anOStream << '"' << myString << '"' << endl;
   }

 anOStream  << '}' << endl;
 return anOStream;
}
