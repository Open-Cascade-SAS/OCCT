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

#include <IGESDimen_DimensionDisplayData.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionDisplayData,IGESData_IGESEntity)

IGESDimen_DimensionDisplayData::IGESDimen_DimensionDisplayData ()    {  }


    void  IGESDimen_DimensionDisplayData::Init
  (const Standard_Integer numProps,
   const Standard_Integer aDimType, const Standard_Integer aLabelPos,
   const Standard_Integer aCharSet,
   const Handle(TCollection_HAsciiString)& aString,
   const Standard_Integer aSymbol, const Standard_Real anAng,
   const Standard_Integer anAlign, const Standard_Integer aLevel,
   const Standard_Integer aPlace, const Standard_Integer anOrient,
   const Standard_Real initVal,
   const Handle(TColStd_HArray1OfInteger)& notes,
   const Handle(TColStd_HArray1OfInteger)& startInd,
   const Handle(TColStd_HArray1OfInteger)& endInd)
{
  if (!notes.IsNull())
    if (notes->Lower() != 1 ||
	(startInd->Lower() != 1 || notes->Length() != startInd->Length()) || 
	(endInd->Lower()   != 1 || notes->Length() != endInd->Length())   )
      throw Standard_DimensionMismatch("IGESDimen_DimensionDisplayData : Init");

  theNbPropertyValues     = numProps;
  theDimensionType        = aDimType;
  theLabelPosition        = aLabelPos;
  theCharacterSet         = aCharSet;
  theLString              = aString;
  theDecimalSymbol        = aSymbol;
  theWitnessLineAngle     = anAng;
  theTextAlignment        = anAlign;
  theTextLevel            = aLevel;
  theTextPlacement        = aPlace;
  theArrowHeadOrientation = anOrient;
  theInitialValue         = initVal;
  theSupplementaryNotes   = notes;
  theStartIndex           = startInd;
  theEndIndex             = endInd;
  InitTypeAndForm(406,30);
}


    Standard_Integer  IGESDimen_DimensionDisplayData::NbPropertyValues () const
{
  return theNbPropertyValues;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::DimensionType () const
{
  return theDimensionType;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::LabelPosition () const
{
  return theLabelPosition;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::CharacterSet () const
{
  return theCharacterSet;
}

    Handle(TCollection_HAsciiString)  IGESDimen_DimensionDisplayData::LString () const
{
  return theLString;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::DecimalSymbol () const
{
  return theDecimalSymbol;
}

    Standard_Real  IGESDimen_DimensionDisplayData::WitnessLineAngle () const
{
  return theWitnessLineAngle;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::TextAlignment () const
{
  return theTextAlignment;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::TextLevel () const
{
  return theTextLevel;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::TextPlacement () const
{
  return theTextPlacement;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::ArrowHeadOrientation () const
{
  return theArrowHeadOrientation;
}

    Standard_Real  IGESDimen_DimensionDisplayData::InitialValue () const
{
  return theInitialValue;
}

    Standard_Integer  IGESDimen_DimensionDisplayData::NbSupplementaryNotes () const
{
  return (theSupplementaryNotes.IsNull() ? 0 : theSupplementaryNotes->Length());
}

    Standard_Integer  IGESDimen_DimensionDisplayData::SupplementaryNote
  (const Standard_Integer num) const
{
  return theSupplementaryNotes->Value(num);
}

    Standard_Integer  IGESDimen_DimensionDisplayData::StartIndex
  (const Standard_Integer num) const
{
  return theStartIndex->Value(num);
}

    Standard_Integer  IGESDimen_DimensionDisplayData::EndIndex
  (const Standard_Integer num) const
{
  return theEndIndex->Value(num);
}
