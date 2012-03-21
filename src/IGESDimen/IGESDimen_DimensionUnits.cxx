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

#include <IGESDimen_DimensionUnits.ixx>


IGESDimen_DimensionUnits::IGESDimen_DimensionUnits ()    {  }

    void  IGESDimen_DimensionUnits::Init
  (const Standard_Integer nbPropVal,
   const Standard_Integer aSecondPos, const Standard_Integer aUnitsInd,
   const Standard_Integer aCharSet,
   const Handle(TCollection_HAsciiString)& aFormat,
   const Standard_Integer aFracFlag,  const Standard_Integer aPrecision)
{
  theNbPropertyValues = nbPropVal;
  theSecondaryDimenPosition = aSecondPos;
  theUnitsIndicator         = aUnitsInd;
  theCharacterSet           = aCharSet;
  theFormatString           = aFormat;
  theFractionFlag           = aFracFlag;
  thePrecision              = aPrecision;
  InitTypeAndForm(406,28);
}


    Standard_Integer  IGESDimen_DimensionUnits::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESDimen_DimensionUnits::SecondaryDimenPosition () const
{
  return theSecondaryDimenPosition;
}

    Standard_Integer  IGESDimen_DimensionUnits::UnitsIndicator () const
{
  return theUnitsIndicator;
}

    Standard_Integer  IGESDimen_DimensionUnits::CharacterSet () const
{
  return theCharacterSet;
}

    Handle(TCollection_HAsciiString)  IGESDimen_DimensionUnits::FormatString () const
{
  return theFormatString;
}

    Standard_Integer  IGESDimen_DimensionUnits::FractionFlag () const
{
  return theFractionFlag;
}

    Standard_Integer  IGESDimen_DimensionUnits::PrecisionOrDenominator () const
{
  return thePrecision;
}
