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

#include "XCAFAnimObjects_Transform.hxx"

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Transform::XCAFAnimObjects_Transform(const NCollection_Mat4<double>& theTransform)
    : XCAFAnimObjects_Operation(false),
      myTransformPresentation(1, 1)
{
  myTransformPresentation.SetValue(1, theTransform);
}

//=================================================================================================

XCAFAnimObjects_Transform::XCAFAnimObjects_Transform(
  const NCollection_Array1<NCollection_Mat4<double>>& theTransform,
  const NCollection_Array1<double>&                   theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myTransformPresentation(theTransform)
{
}

//=================================================================================================

XCAFAnimObjects_Transform::XCAFAnimObjects_Transform(
  const NCollection_Array2<double>& theGeneralPresentation,
  const NCollection_Array1<double>& theTimeStamps)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myTransformPresentation(1, theGeneralPresentation.NbRows())
{
  if (theGeneralPresentation.NbColumns() != 16)
  {
    Message::SendWarning()
      << "Warning: XCAFAnimObjects_Transform: Incorrect Mat4x4 general presentation";
    return;
  }
  for (int aRowInd = 1; aRowInd <= theGeneralPresentation.NbRows(); aRowInd++)
  {
    NCollection_Mat4<double> aTransform;
    aTransform.SetRow(0,
                      NCollection_Vec4<double>(theGeneralPresentation.Value(aRowInd, 1),
                                               theGeneralPresentation.Value(aRowInd, 2),
                                               theGeneralPresentation.Value(aRowInd, 3),
                                               theGeneralPresentation.Value(aRowInd, 4)));
    aTransform.SetRow(1,
                      NCollection_Vec4<double>(theGeneralPresentation.Value(aRowInd, 5),
                                               theGeneralPresentation.Value(aRowInd, 6),
                                               theGeneralPresentation.Value(aRowInd, 7),
                                               theGeneralPresentation.Value(aRowInd, 8)));
    aTransform.SetRow(2,
                      NCollection_Vec4<double>(theGeneralPresentation.Value(aRowInd, 9),
                                               theGeneralPresentation.Value(aRowInd, 10),
                                               theGeneralPresentation.Value(aRowInd, 11),
                                               theGeneralPresentation.Value(aRowInd, 12)));
    aTransform.SetRow(3,
                      NCollection_Vec4<double>(theGeneralPresentation.Value(aRowInd, 13),
                                               theGeneralPresentation.Value(aRowInd, 14),
                                               theGeneralPresentation.Value(aRowInd, 15),
                                               theGeneralPresentation.Value(aRowInd, 16)));
    myTransformPresentation.SetValue(aRowInd, aTransform);
  }
}

//=================================================================================================

XCAFAnimObjects_Transform::XCAFAnimObjects_Transform(
  const Handle(XCAFAnimObjects_Transform)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      myTransformPresentation(theOperation->myTransformPresentation)
{
}

//=================================================================================================

NCollection_Array2<double> XCAFAnimObjects_Transform::GeneralPresentation() const
{
  NCollection_Array2<double> aRes(1, myTransformPresentation.Length(), 1, 16);
  for (int aRowInd = 1; aRowInd <= myTransformPresentation.Length(); aRowInd++)
  {
    const NCollection_Mat4<double>& aTransform = myTransformPresentation.Value(aRowInd);

    aRes.SetValue(aRowInd, 1, aTransform.GetValue(0, 0));
    aRes.SetValue(aRowInd, 2, aTransform.GetValue(0, 1));
    aRes.SetValue(aRowInd, 3, aTransform.GetValue(0, 2));
    aRes.SetValue(aRowInd, 4, aTransform.GetValue(0, 3));
    aRes.SetValue(aRowInd, 5, aTransform.GetValue(1, 0));
    aRes.SetValue(aRowInd, 6, aTransform.GetValue(1, 1));
    aRes.SetValue(aRowInd, 7, aTransform.GetValue(1, 2));
    aRes.SetValue(aRowInd, 8, aTransform.GetValue(1, 3));
    aRes.SetValue(aRowInd, 9, aTransform.GetValue(2, 0));
    aRes.SetValue(aRowInd, 10, aTransform.GetValue(2, 1));
    aRes.SetValue(aRowInd, 11, aTransform.GetValue(2, 2));
    aRes.SetValue(aRowInd, 12, aTransform.GetValue(2, 3));
    aRes.SetValue(aRowInd, 13, aTransform.GetValue(3, 0));
    aRes.SetValue(aRowInd, 14, aTransform.GetValue(3, 1));
    aRes.SetValue(aRowInd, 15, aTransform.GetValue(3, 2));
    aRes.SetValue(aRowInd, 16, aTransform.GetValue(3, 3));
  }
  return aRes;
}