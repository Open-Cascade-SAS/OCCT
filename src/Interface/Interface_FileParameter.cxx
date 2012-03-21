// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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

