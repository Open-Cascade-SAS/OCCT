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

#include <IGESGraph_NominalSize.ixx>


IGESGraph_NominalSize::IGESGraph_NominalSize ()    {  }


    void IGESGraph_NominalSize::Init
  (const Standard_Integer          nbProps,
   const Standard_Real             aNominalSizeValue,
   const Handle(TCollection_HAsciiString)& aNominalSizeName,
   const Handle(TCollection_HAsciiString)& aStandardName)
{
  theNbPropertyValues = nbProps;
  theNominalSizeValue = aNominalSizeValue;
  theNominalSizeName  = aNominalSizeName;
  theStandardName     = aStandardName;
  InitTypeAndForm(406,13);
}

    Standard_Integer IGESGraph_NominalSize::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Real IGESGraph_NominalSize::NominalSizeValue () const
{
  return theNominalSizeValue;
}

    Handle(TCollection_HAsciiString) IGESGraph_NominalSize::NominalSizeName () const
{
  return theNominalSizeName;
}

    Standard_Boolean IGESGraph_NominalSize::HasStandardName () const
{
  return (! theStandardName.IsNull() );
}

    Handle(TCollection_HAsciiString) IGESGraph_NominalSize::StandardName () const
{
  return theStandardName;
}
