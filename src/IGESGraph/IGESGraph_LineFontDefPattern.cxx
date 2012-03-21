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

#include <IGESGraph_LineFontDefPattern.ixx>
#include <Standard_DimensionMismatch.hxx>


IGESGraph_LineFontDefPattern::IGESGraph_LineFontDefPattern ()    {  }


// This class inherits from IGESData_LineFontEntity

    void IGESGraph_LineFontDefPattern::Init
  (const Handle(TColStd_HArray1OfReal)&      allSegLength,
   const Handle(TCollection_HAsciiString)&   aPattern)
{
  if (allSegLength->Lower() != 1)
    Standard_DimensionMismatch::Raise("IGESGraph_LineFontDefPattern : Init");
  theSegmentLengths = allSegLength;
  theDisplayPattern = aPattern;
  InitTypeAndForm(304,2);
}

    Standard_Integer IGESGraph_LineFontDefPattern::NbSegments () const
{
  return theSegmentLengths->Length();
}

    Standard_Real IGESGraph_LineFontDefPattern::Length
  (const Standard_Integer Index) const
{
  return theSegmentLengths->Value(Index);
  // if Index is out of bound HArray1 will raise OutOfRange exception
}

    Handle(TCollection_HAsciiString) IGESGraph_LineFontDefPattern::DisplayPattern
  () const
{
  return theDisplayPattern;
}

    Standard_Boolean IGESGraph_LineFontDefPattern::IsVisible
  (const Standard_Integer Index) const
{
  Standard_Integer nbSegs = theSegmentLengths->Length();
  if (Index <= 0 || Index > nbSegs)    return Standard_False;

  // Get the Character out of String, which contains the required BIT
  char tempStr[2];
  Standard_Integer length = theDisplayPattern->Length();
  tempStr[0] = theDisplayPattern->Value(length - ((nbSegs - Index) / 4));
  tempStr[1] = 0;
  Standard_Integer tempVal =
    (Standard_Integer) strtol(tempStr, (char **)NULL, 16);
  // Now get the BIT out of tempVal
  Standard_Integer mask = 0x01;
  mask <<= ((nbSegs - Index) % 4);
  return ((tempVal & mask) != 0);
}
