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

#include <IGESGraph_DrawingUnits.ixx>


IGESGraph_DrawingUnits::IGESGraph_DrawingUnits ()    {  }


    void IGESGraph_DrawingUnits::Init
  (const Standard_Integer nbProps, const Standard_Integer aFlag,
   const Handle(TCollection_HAsciiString)& anUnit)
{
  theNbPropertyValues = nbProps;
  theFlag             = aFlag;
  theUnit             = anUnit;
  InitTypeAndForm(406,17);
}

    Standard_Integer IGESGraph_DrawingUnits::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer IGESGraph_DrawingUnits::Flag () const
{
  return theFlag;
}

    Handle(TCollection_HAsciiString) IGESGraph_DrawingUnits::Unit () const
{
  return theUnit;
}

    Standard_Real            IGESGraph_DrawingUnits::UnitValue () const
{
  switch (theFlag) {
    case  1 : return 0.0254;
    case  2 : return 0.001;
    case  3 : return 1.;
    case  4 : return 0.3048;
    case  5 : return 1609.27;
    case  6 : return 1.;
    case  7 : return 1000.;
    case  8 : return 0.0000254;
    case  9 : return 0.000001;
    case 10 : return 0.01;
    case 11 : return 0.0000000254;
    default : break;
  }
  return 1.;
}
