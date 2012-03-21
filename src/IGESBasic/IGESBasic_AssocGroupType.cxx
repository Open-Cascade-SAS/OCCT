// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESBasic_AssocGroupType.ixx>


IGESBasic_AssocGroupType::IGESBasic_AssocGroupType ()    {  }


    void  IGESBasic_AssocGroupType::Init
  (const Standard_Integer nbDataFields,
   const Standard_Integer aType, const Handle(TCollection_HAsciiString)& aName)
{
  theNbData = nbDataFields;
  theType   = aType;
  theName   = aName;
  InitTypeAndForm(406,23);
}


    Standard_Integer  IGESBasic_AssocGroupType::NbData () const
{
  return theNbData;
}

    Standard_Integer  IGESBasic_AssocGroupType::AssocType () const
{
  return theType;
}

    Handle(TCollection_HAsciiString)  IGESBasic_AssocGroupType::Name () const
{
  return theName;
}
