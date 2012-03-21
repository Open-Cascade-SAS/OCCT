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

#include <IGESSelect_SelectVisibleStatus.ixx>
#include <IGESData_IGESEntity.hxx>
#include <Interface_Macros.hxx>



IGESSelect_SelectVisibleStatus::IGESSelect_SelectVisibleStatus ()    {  }


    Standard_Boolean  IGESSelect_SelectVisibleStatus::Sort
  (const Standard_Integer rank, const Handle(Standard_Transient)& ent,
   const Handle(Interface_InterfaceModel)& model) const
{
  DeclareAndCast(IGESData_IGESEntity,igesent,ent);
  if (igesent.IsNull()) return Standard_False;
  return (igesent->BlankStatus() == 0);
}

    TCollection_AsciiString  IGESSelect_SelectVisibleStatus::ExtractLabel
  () const
      {  return TCollection_AsciiString ("IGES Entity, Status Visible");  }
