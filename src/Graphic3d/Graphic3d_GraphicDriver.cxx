// Created on: 1997-01-28
// Created by: CAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_Structure.hxx>
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TransformError.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

Graphic3d_GraphicDriver::Graphic3d_GraphicDriver (const Handle(Aspect_DisplayConnection)& theDisp)
: MyTraceLevel        (0),
  myDisplayConnection (theDisp),
  myDeviceLostFlag    (Standard_False)
{
  //
}

//-Internal methods, in order

void Graphic3d_GraphicDriver::PrintBoolean (const Standard_CString AComment, const Standard_Boolean AValue) const {

  cout << "\t" << AComment << " : "
    << (AValue ? "True" : "False") << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintCLight (const Graphic3d_CLight& theCLight,
                                           const Standard_Integer  theField) const
{
  if (theField)
  {
    switch (theCLight.Type) {
    case 0 :
      cout << "\tlight type : ambient\n";
      break;
    case 1 :
      cout << "\tlight type : directional\n";
      break;
    case 2 :
      cout << "\tlight type : positional\n";
      break;
    case 3 :
      cout << "\tlight type : spot\n";
      break;
    default :
      cout << "\tlight type : undefined, error\n";
      break;
    }
    cout << flush;
  }
}

void Graphic3d_GraphicDriver::PrintCStructure (const Graphic3d_CStructure& ACStructure, const Standard_Integer AField) const {

  if (AField) {
    cout << "\tstruct id " << ACStructure.Id << "\n";
    cout << flush;
  }

}

void Graphic3d_GraphicDriver::PrintCView (const Graphic3d_CView& ACView, const Standard_Integer AField) const {

  if (AField) {
    cout << "view id " << ACView.Identification() << "\n";
    cout << "\tXwindow id " << (ACView.Window().IsNull() ? 0 : ACView.Window()->NativeHandle()) << ", "
      << "activity " << ACView.IsActive() << "\n";
    cout << flush;
  }

}

void Graphic3d_GraphicDriver::PrintFunction (const Standard_CString AFunc) const {

  cout << AFunc << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintInteger (const Standard_CString AComment, const Standard_Integer AValue) const {

  cout << "\t" << AComment << " : " << AValue << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintIResult (const Standard_CString AFunc, const Standard_Integer AResult) const {

  cout << AFunc << " = " << AResult << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintMatrix (const Standard_CString AComment, const TColStd_Array2OfReal& AMatrix) const {

  Standard_Integer lr, ur, lc, uc;
  Standard_Integer i, j;

  lr = AMatrix.LowerRow ();
  ur = AMatrix.UpperRow ();
  lc = AMatrix.LowerCol ();
  uc = AMatrix.UpperCol ();

  if ( (ur - lr + 1 != 4) || (uc - lc + 1 != 4) )
    Graphic3d_TransformError::Raise
    ("PrintMatrix : not a 4x4 matrix");

  cout << "\t" << AComment << " :\n";
  for (i=lr; i<=ur; i++) {
    for (j=lc; j<=uc; j++) {
      cout << AMatrix (i, j) << " ";
    }
    cout << "\n";
  }
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintShortReal (const Standard_CString AComment, const Standard_ShortReal AValue) const {

  cout << "\t" << AComment << " : " << AValue << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintString (const Standard_CString AComment, const Standard_CString AString) const {

  cout << "\t" << AComment << " : " << AString << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::SetTrace (const Standard_Integer ALevel) {

  MyTraceLevel = ALevel;

}

Standard_Integer Graphic3d_GraphicDriver::Trace () const {

  return MyTraceLevel;

}

const Handle(Aspect_DisplayConnection)& Graphic3d_GraphicDriver::GetDisplayConnection() const
{
  return myDisplayConnection;
}

Standard_Boolean Graphic3d_GraphicDriver::IsDeviceLost() const
{
  return myDeviceLostFlag;
}

void Graphic3d_GraphicDriver::ResetDeviceLostFlag()
{
  myDeviceLostFlag = Standard_False;
}
