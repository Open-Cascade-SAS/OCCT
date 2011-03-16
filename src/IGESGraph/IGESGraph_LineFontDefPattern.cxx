//--------------------------------------------------------------------
//
//  File Name : IGESGraph_LineFontDefPattern.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
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
