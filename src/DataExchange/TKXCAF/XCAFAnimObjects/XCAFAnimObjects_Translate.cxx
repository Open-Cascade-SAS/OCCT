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

#include "XCAFAnimObjects_Translate.hxx"

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Translate::XCAFAnimObjects_Translate(const gp_XYZ& theTranslate)
    : XCAFAnimObjects_Operation(false),
      myTranslatePresentation(1, 1)
{
  myTranslatePresentation.SetValue(1, theTranslate);
}

//=================================================================================================

XCAFAnimObjects_Translate::XCAFAnimObjects_Translate(
  const NCollection_Array1<gp_XYZ>& theTranslate,
  const NCollection_Array1<double>& theTimeStamps)
    :

      XCAFAnimObjects_Operation(theTimeStamps),
      myTranslatePresentation(theTranslate)
{
}

//=================================================================================================

XCAFAnimObjects_Translate::XCAFAnimObjects_Translate(
  const NCollection_Array2<double>& theGeneralPresentation,
  const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myTranslatePresentation(1, theGeneralPresentation.NbRows())
{
  if (theGeneralPresentation.NbColumns() != 3)
  {
    Message::SendWarning()
      << "Warning: XCAFAnimObjects_Translate: Incorrect XYZ general presentation";
    return;
  }
  for (int aRowInd = 1; aRowInd <= theGeneralPresentation.NbRows(); aRowInd++)
  {
    gp_XYZ aXYZ(theGeneralPresentation.Value(aRowInd, 1),
                theGeneralPresentation.Value(aRowInd, 2),
                theGeneralPresentation.Value(aRowInd, 3));
    myTranslatePresentation.SetValue(aRowInd, aXYZ);
  }
}

//=================================================================================================

XCAFAnimObjects_Translate::XCAFAnimObjects_Translate(
  const Handle(XCAFAnimObjects_Translate)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      myTranslatePresentation(theOperation->myTranslatePresentation)
{
}

//=================================================================================================

NCollection_Array2<double> XCAFAnimObjects_Translate::GeneralPresentation() const
{
  NCollection_Array2<double> aRes(1, myTranslatePresentation.Length(), 1, 3);
  for (int aRowInd = 1; aRowInd <= myTranslatePresentation.Length(); aRowInd++)
  {
    const gp_XYZ& aXYZ = myTranslatePresentation.Value(aRowInd);

    aRes.SetValue(aRowInd, 1, aXYZ.X());
    aRes.SetValue(aRowInd, 2, aXYZ.Y());
    aRes.SetValue(aRowInd, 3, aXYZ.Z());
  }
  return aRes;
}