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

#include <XCAFAnimObjects_Orient.hxx>

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Orient::XCAFAnimObjects_Orient(const gp_Quaternion& theOrient)
    : XCAFAnimObjects_Operation(false),
      myOrientPresentation(1, 1)
{
  myOrientPresentation.SetValue(1, theOrient);
}

//=================================================================================================

XCAFAnimObjects_Orient::XCAFAnimObjects_Orient(const NCollection_Array1<gp_Quaternion>& theOrient,
                                               const NCollection_Array1<double>& theTimeStamps)
    :

      XCAFAnimObjects_Operation(theTimeStamps),
      myOrientPresentation(theOrient)
{
}

//=================================================================================================

XCAFAnimObjects_Orient::XCAFAnimObjects_Orient(
  const NCollection_Array2<double>& theGeneralPresentation,
  const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myOrientPresentation(1, theGeneralPresentation.NbRows())
{
  if (theGeneralPresentation.NbColumns() != 4)
  {
    Message::SendWarning()
      << "Warning: XCAFAnimObjects_Orient: Incorrect Quaternion general presentation";
    return;
  }
  for (int aRowInd = 1; aRowInd <= theGeneralPresentation.NbRows(); aRowInd++)
  {
    gp_Quaternion aQuat(theGeneralPresentation.Value(aRowInd, 1),
                        theGeneralPresentation.Value(aRowInd, 2),
                        theGeneralPresentation.Value(aRowInd, 3),
                        theGeneralPresentation.Value(aRowInd, 4));
    myOrientPresentation.SetValue(aRowInd, aQuat);
  }
}

//=================================================================================================

XCAFAnimObjects_Orient::XCAFAnimObjects_Orient(const Handle(XCAFAnimObjects_Orient)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      myOrientPresentation(theOperation->myOrientPresentation)
{
}

//=================================================================================================

NCollection_Array2<double> XCAFAnimObjects_Orient::GeneralPresentation() const
{
  NCollection_Array2<double> aRes(1, myOrientPresentation.Length(), 1, 4);
  for (int aRowInd = 1; aRowInd <= myOrientPresentation.Length(); aRowInd++)
  {
    const gp_Quaternion& aQuat = myOrientPresentation.Value(aRowInd);

    aRes.SetValue(aRowInd, 1, aQuat.X());
    aRes.SetValue(aRowInd, 2, aQuat.Y());
    aRes.SetValue(aRowInd, 3, aQuat.Z());
    aRes.SetValue(aRowInd, 4, aQuat.W());
  }
  return aRes;
}