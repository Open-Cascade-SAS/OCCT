// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDimen_DimensionUnits.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionUnits,IGESData_IGESEntity)

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
