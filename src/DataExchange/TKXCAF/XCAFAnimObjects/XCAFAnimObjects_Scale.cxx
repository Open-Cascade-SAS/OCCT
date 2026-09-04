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

#include "XCAFAnimObjects_Scale.hxx"

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Scale::XCAFAnimObjects_Scale(const gp_XYZ& theScale)
    : XCAFAnimObjects_Operation(false),
      myScalePresentation(1, 1)
{
  myScalePresentation.SetValue(1, theScale);
}

//=================================================================================================

XCAFAnimObjects_Scale::XCAFAnimObjects_Scale(const NCollection_Array1<gp_XYZ>& theScale,
                                             const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myScalePresentation(theScale)
{
}

//=================================================================================================

XCAFAnimObjects_Scale::XCAFAnimObjects_Scale(
  const NCollection_Array2<double>& theGeneralPresentation,
  const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myScalePresentation(1, theGeneralPresentation.NbRows())
{
  if (theGeneralPresentation.NbColumns() != 3)
  {
    Message::SendWarning() << "Warning: XCAFAnimObjects_Scale: Incorrect XYZ general presentation";
    return;
  }
  for (int aRowInd = 1; aRowInd <= theGeneralPresentation.NbRows(); aRowInd++)
  {
    gp_XYZ aXYZ(theGeneralPresentation.Value(aRowInd, 1),
                theGeneralPresentation.Value(aRowInd, 2),
                theGeneralPresentation.Value(aRowInd, 3));
    myScalePresentation.SetValue(aRowInd, aXYZ);
  }
}

//=================================================================================================

XCAFAnimObjects_Scale::XCAFAnimObjects_Scale(const Handle(XCAFAnimObjects_Scale)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      myScalePresentation(theOperation->myScalePresentation)
{
}

//=================================================================================================

NCollection_Array2<double> XCAFAnimObjects_Scale::GeneralPresentation() const
{
  NCollection_Array2<double> aRes(1, myScalePresentation.Length(), 1, 3);
  for (int aRowInd = 1; aRowInd <= myScalePresentation.Length(); aRowInd++)
  {
    const gp_XYZ& aXYZ = myScalePresentation.Value(aRowInd);

    aRes.SetValue(aRowInd, 1, aXYZ.X());
    aRes.SetValue(aRowInd, 2, aXYZ.Y());
    aRes.SetValue(aRowInd, 3, aXYZ.Z());
  }
  return aRes;
}
