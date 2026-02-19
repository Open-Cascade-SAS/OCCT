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

#include "XCAFAnimObjects_Rotate.hxx"

#include <Message.hxx>

//=================================================================================================

XCAFAnimObjects_Rotate::XCAFAnimObjects_Rotate(const NCollection_Array1<double>& theRotate,
                                               const NCollection_Array1<double>& theTimeStamps,
                                               const XCAFAnimObjects_Rotate_Type theRotateType)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myRotateType(theRotateType),
      myRotatePresentation(1, 1, theRotate.Lower(), theRotate.Upper())
{
  for (int anInd = theRotate.Lower(); anInd <= theRotate.Upper(); anInd++)
  {
    myRotatePresentation.SetValue(1, anInd, theRotate.Value(anInd));
  }
  Standard_Integer aNbDouble = 3;
  if (theRotateType >= XCAFAnimObjects_Rotate_Type_XY
      && theRotateType <= XCAFAnimObjects_Rotate_Type_ZY)
  {
    aNbDouble = 2;
  }
  if (theRotateType >= XCAFAnimObjects_Rotate_Type_X
      && theRotateType <= XCAFAnimObjects_Rotate_Type_Z)
  {
    aNbDouble = 1;
  }
  if (theRotate.Length() != aNbDouble)
  {
    Message::SendWarning() << "Warning: XCAFAnimObjects_Rotate: Incorrect Rotate presentation";
  }
}

//=================================================================================================

XCAFAnimObjects_Rotate::XCAFAnimObjects_Rotate(
  const NCollection_Array2<double>& theGeneralPresentation,
  const NCollection_Array1<double>& theTimeStamps,
  const XCAFAnimObjects_Rotate_Type theRotateType)
    : XCAFAnimObjects_Operation(theTimeStamps),
      myRotateType(theRotateType),
      myRotatePresentation(theGeneralPresentation)
{
  Standard_Integer aNbDouble = 3;
  if (theRotateType >= XCAFAnimObjects_Rotate_Type_XY
      && theRotateType <= XCAFAnimObjects_Rotate_Type_ZY)
  {
    aNbDouble = 2;
  }
  if (theRotateType >= XCAFAnimObjects_Rotate_Type_X
      && theRotateType <= XCAFAnimObjects_Rotate_Type_Z)
  {
    aNbDouble = 1;
  }
  if (theGeneralPresentation.NbColumns() != aNbDouble)
  {
    Message::SendWarning() << "Warning: XCAFAnimObjects_Rotate: Incorrect Rotate presentation";
  }
}

//=================================================================================================

XCAFAnimObjects_Rotate::XCAFAnimObjects_Rotate(const Handle(XCAFAnimObjects_Rotate)& theOperation)
    : XCAFAnimObjects_Operation(theOperation),
      myRotateType(theOperation->myRotateType),
      myRotatePresentation(theOperation->myRotatePresentation)
{
}
