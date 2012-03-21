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

#include <IGESSelect_SelectName.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>
#include <stdio.h>


IGESSelect_SelectName::IGESSelect_SelectName ()    {  }


    void  IGESSelect_SelectName::SetName
  (const Handle(TCollection_HAsciiString)& levnum)
      {  thename = levnum;  }

    Handle(TCollection_HAsciiString)  IGESSelect_SelectName::Name () const
      {  return thename;  }


    Standard_Boolean  IGESSelect_SelectName::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  if (!igesent->HasName()) return Standard_False;
  if (thename.IsNull()) return Standard_False;
  Handle(TCollection_HAsciiString) name = igesent->NameValue();
//  cout<<"SelectName:"<<thename->ToCString()<<",with:"<<name->ToCString()<<",IsSameString="<<thename->IsSameString (name,Standard_False)<<endl;
  Standard_Integer nb0 = thename->Length();
  Standard_Integer nb1 = name->Length();
  Standard_Integer nbf = (nb1 <= nb0 ? nb1 : nb0);
  Standard_Integer nbt = (nb1 >= nb0 ? nb1 : nb0);
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for (i = 1; i <= nbf; i ++) {
    if (name->Value(i) != thename->Value(i)) return Standard_False;
  }
  if (nb0 > nb1) name = thename;
  for (i = nbf+1; i <= nbt; i ++) {
    if (name->Value(i) != ' ') return Standard_False;
  }
  return Standard_True;
}


    TCollection_AsciiString  IGESSelect_SelectName::ExtractLabel () const
{
  char labl [50];
  if (!thename.IsNull()) {
    sprintf (labl,"IGES Entity, Name : %s",thename->ToCString());
    return TCollection_AsciiString(labl);
  }
  else  return TCollection_AsciiString ("IGES Entity, Name : (undefined)");
}
