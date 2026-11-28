// Copyright (c) 2025 OPEN CASCADE SAS
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

#include "XCAFAnimObjects_Skew.hxx"

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Skew::XCAFAnimObjects_Skew(const gp_XYZ& theSkew)
    : XCAFAnimObjects_Operation(false),
      mySkewPresentation(1, 1)
{
  mySkewPresentation.SetValue(1, theSkew);
}

//=================================================================================================

XCAFAnimObjects_Skew::XCAFAnimObjects_Skew(const NCollection_Array1<gp_XYZ>& theSkew,
                                           const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      mySkewPresentation(theSkew)
{
}

//=================================================================================================

XCAFAnimObjects_Skew::XCAFAnimObjects_Skew(const NCollection_Array2<double>& theGeneralPresentation,
                                           const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      mySkewPresentation(1, theGeneralPresentation.NbRows())
{
  if (theGeneralPresentation.NbColumns() != 3)
  {
    Message::SendWarning() << "Warning: XCAFAnimObjects_Skew: Incorrect Skew general presentation";
    return;
  }
  for (int aRowInd = 1; aRowInd <= theGeneralPresentation.NbRows(); aRowInd++)
  {
    gp_XYZ aSkew(theGeneralPresentation.Value(aRowInd, 1),
                 theGeneralPresentation.Value(aRowInd, 2),
                 theGeneralPresentation.Value(aRowInd, 3));
    mySkewPresentation.SetValue(aRowInd, aSkew);
  }
}

//=================================================================================================

XCAFAnimObjects_Skew::XCAFAnimObjects_Skew(const Handle(XCAFAnimObjects_Skew)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      mySkewPresentation(theOperation->mySkewPresentation)
{
}

//=================================================================================================

NCollection_Array2<double> XCAFAnimObjects_Skew::GeneralPresentation() const
{
  NCollection_Array2<double> aRes(1, mySkewPresentation.Length(), 1, 3);
  for (int aRowInd = 1; aRowInd <= mySkewPresentation.Length(); aRowInd++)
  {
    const gp_XYZ& aSkew = mySkewPresentation.Value(aRowInd);

    aRes.SetValue(aRowInd, 1, aSkew.X());
    aRes.SetValue(aRowInd, 2, aSkew.Y());
    aRes.SetValue(aRowInd, 3, aSkew.Z());
  }
  return aRes;
}