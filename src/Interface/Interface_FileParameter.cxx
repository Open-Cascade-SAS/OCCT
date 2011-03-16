#include <Interface_FileParameter.ixx>

//=======================================================================
//function : Interface_FileParameter
//purpose  : 
//=======================================================================
Interface_FileParameter::Interface_FileParameter ()
{  
thetype = Interface_ParamMisc;  thenum = 0;  
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void Interface_FileParameter::Init(const TCollection_AsciiString& val, 
				   const Interface_ParamType typ)
{
  theval  = new char[val.Length()+1];
  strcpy(theval,val.ToCString());
  thetype = typ;
  thenum  = 0;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void Interface_FileParameter::Init (const Standard_CString val, 
				    const Interface_ParamType typ)
{
  theval  = (Standard_PCharacter)val;  // Principe : Allocation geree par contenant (ParamSet)
  thetype = typ;
  thenum  = 0;
}
//=======================================================================
//function : CValue
//purpose  : 
//=======================================================================
Standard_CString  Interface_FileParameter::CValue () const
{
  return theval;  
}
//=======================================================================
//function : ParamType
//purpose  : 
//=======================================================================
Interface_ParamType Interface_FileParameter::ParamType () const
{
  return thetype;  
}
//=======================================================================
//function : SetEntityNumber
//purpose  : 
//=======================================================================
void Interface_FileParameter::SetEntityNumber (const Standard_Integer num)
{
  thenum = num;  
}
//=======================================================================
//function : EntityNumber
//purpose  : 
//=======================================================================
Standard_Integer Interface_FileParameter::EntityNumber () const
{
  return thenum;  
}
//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void Interface_FileParameter::Clear ()
{
  theval = NULL; 
}  // delete theval;  pas si gere par ParamSet
//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================
void Interface_FileParameter::Destroy ()  
{
}

