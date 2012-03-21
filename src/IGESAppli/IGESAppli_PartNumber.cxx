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

#include <IGESAppli_PartNumber.ixx>


IGESAppli_PartNumber::IGESAppli_PartNumber ()    {  }


    void  IGESAppli_PartNumber::Init
  (const Standard_Integer nbPropVal,
   const Handle(TCollection_HAsciiString)& aGenName,
   const Handle(TCollection_HAsciiString)& aMilName,
   const Handle(TCollection_HAsciiString)& aVendName,
   const Handle(TCollection_HAsciiString)& anIntName)
{
  theNbPropertyValues = nbPropVal;
  theGenericNumber    = aGenName;
  theMilitaryNumber   = aMilName;
  theVendorNumber     = aVendName;
  theInternalNumber   = anIntName;
  InitTypeAndForm(406,9);
}


    Standard_Integer  IGESAppli_PartNumber::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::GenericNumber () const
{
  return theGenericNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::MilitaryNumber () const
{
  return theMilitaryNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::VendorNumber () const
{
  return theVendorNumber;
}

    Handle(TCollection_HAsciiString)  IGESAppli_PartNumber::InternalNumber () const
{
  return theInternalNumber;
}
