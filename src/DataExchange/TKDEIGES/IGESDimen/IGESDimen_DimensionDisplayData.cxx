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
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DimensionDisplayData, IGESData_IGESEntity)

IGESDimen_DimensionDisplayData::IGESDimen_DimensionDisplayData() = default;

void IGESDimen_DimensionDisplayData::Init(const int                                    numProps,
                                          const int                                    aDimType,
                                          const int                                    aLabelPos,
                                          const int                                    aCharSet,
                                          const occ::handle<TCollection_HAsciiString>& aString,
                                          const int                                    aSymbol,
                                          const double                                 anAng,
                                          const int                                    anAlign,
                                          const int                                    aLevel,
                                          const int                                    aPlace,
                                          const int                                    anOrient,
                                          const double                                 initVal,
                                          const occ::handle<NCollection_HArray1<int>>& notes,
                                          const occ::handle<NCollection_HArray1<int>>& startInd,
                                          const occ::handle<NCollection_HArray1<int>>& endInd)
{
  if (!notes.IsNull())
    if (notes->Lower() != 1 || (startInd->Lower() != 1 || notes->Length() != startInd->Length())
        || (endInd->Lower() != 1 || notes->Length() != endInd->Length()))
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
  InitTypeAndForm(406, 30);
}

int IGESDimen_DimensionDisplayData::NbPropertyValues() const
{
  return theNbPropertyValues;
}

int IGESDimen_DimensionDisplayData::DimensionType() const
{
  return theDimensionType;
}

int IGESDimen_DimensionDisplayData::LabelPosition() const
{
  return theLabelPosition;
}

int IGESDimen_DimensionDisplayData::CharacterSet() const
{
  return theCharacterSet;
}

occ::handle<TCollection_HAsciiString> IGESDimen_DimensionDisplayData::LString() const
{
  return theLString;
}

int IGESDimen_DimensionDisplayData::DecimalSymbol() const
{
  return theDecimalSymbol;
}

double IGESDimen_DimensionDisplayData::WitnessLineAngle() const
{
  return theWitnessLineAngle;
}

int IGESDimen_DimensionDisplayData::TextAlignment() const
{
  return theTextAlignment;
}

int IGESDimen_DimensionDisplayData::TextLevel() const
{
  return theTextLevel;
}

int IGESDimen_DimensionDisplayData::TextPlacement() const
{
  return theTextPlacement;
}

int IGESDimen_DimensionDisplayData::ArrowHeadOrientation() const
{
  return theArrowHeadOrientation;
}

double IGESDimen_DimensionDisplayData::InitialValue() const
{
  return theInitialValue;
}

int IGESDimen_DimensionDisplayData::NbSupplementaryNotes() const
{
  return (theSupplementaryNotes.IsNull() ? 0 : theSupplementaryNotes->Length());
}

int IGESDimen_DimensionDisplayData::SupplementaryNote(const int num) const
{
  return theSupplementaryNotes->Value(num);
}

int IGESDimen_DimensionDisplayData::StartIndex(const int num) const
{
  return theStartIndex->Value(num);
}

int IGESDimen_DimensionDisplayData::EndIndex(const int num) const
{
  return theEndIndex->Value(num);
}
