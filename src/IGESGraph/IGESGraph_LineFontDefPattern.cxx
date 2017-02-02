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

#include <IGESGraph_LineFontDefPattern.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_LineFontDefPattern,IGESData_LineFontEntity)

IGESGraph_LineFontDefPattern::IGESGraph_LineFontDefPattern ()    {  }


// This class inherits from IGESData_LineFontEntity

    void IGESGraph_LineFontDefPattern::Init
  (const Handle(TColStd_HArray1OfReal)&      allSegLength,
   const Handle(TCollection_HAsciiString)&   aPattern)
{
  if (allSegLength->Lower() != 1)
    throw Standard_DimensionMismatch("IGESGraph_LineFontDefPattern : Init");
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
