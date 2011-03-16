
#include <Standard_Address.hxx>
#include <Standard_Type.hxx> 
#ifndef _Standard_OStream_HeaderFile
#include <Standard_OStream.hxx>
#endif

Handle_Standard_Type& Standard_Address_Type_() 
{
  static Handle_Standard_Type _aType = 
    new Standard_Type("Standard_Address", sizeof(Standard_Address),0,NULL);

  return _aType;
}


//============================================================================
//==== ShallowDump : Writes a CString value.
//============================================================================
Standard_EXPORT void ShallowDump (const Standard_Address Value, Standard_OStream& s)
{ s << Value << " Standard_Address " << "\n"; }
