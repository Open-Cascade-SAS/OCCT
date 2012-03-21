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

#include <IGESDimen_DimensionDisplayData.ixx>


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
      Standard_DimensionMismatch::Raise("IGESDimen_DimensionDisplayData : Init");

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
