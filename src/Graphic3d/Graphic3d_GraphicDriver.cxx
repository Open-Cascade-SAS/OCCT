// Created on: 1997-01-28
// Created by: CAL
// Copyright (c) 1997-1999 Matra Datavision
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

//              11/97 ; CAL : retrait de la dependance avec math


//-Version  

//-Design Declaration des variables specifiques aux Drivers

//-Warning  Un driver encapsule les Pex, Phigs et OpenGl drivers

//-References 

//-Language C++ 2.0

//-Declarations

// for the class
#include <Graphic3d_GraphicDriver.ixx>

#include <Aspect_DriverDefinitionError.hxx>

//-Aliases

//-Global data definitions

//-Constructors

Graphic3d_GraphicDriver::Graphic3d_GraphicDriver (const Standard_CString AShrName) {

  SetTrace (0);
  MySharedLibrary.SetName (AShrName);

  //if (! MySharedLibrary.DlOpen (OSD_RTLD_LAZY))
  //Aspect_DriverDefinitionError::Raise (MySharedLibrary.DlError ());

}

//-Methods, in order

Standard_Integer Graphic3d_GraphicDriver::Light (const Graphic3d_CLight& ACLight, const Standard_Boolean Update) {

  static Standard_Integer NbLights = 1;
  Standard_Boolean Result;

  Result = Update ? ACLight.LightId : NbLights++;
  return Result;

}

Standard_Integer Graphic3d_GraphicDriver::Plane (const Graphic3d_CPlane& ACPlane, const Standard_Boolean Update) {

  static Standard_Integer NbPlanes = 1;
  Standard_Boolean Result;

  Result = Update ? ACPlane.PlaneId : NbPlanes++;
  return Result;

}

//-Internal methods, in order

void Graphic3d_GraphicDriver::PrintBoolean (const Standard_CString AComment, const Standard_Boolean AValue) const {

  cout << "\t" << AComment << " : "
    << (AValue ? "True" : "False") << "\n";
  cout << flush;

}

void Graphic3d_GraphicDriver::PrintCGroup (const Graphic3d_CGroup& ACGroup, const Standard_Integer AField) const {

  if (AField) {
    cout << "\tstruct id " << ACGroup.Struct->Id << "\n";
    cout << "\tgroup\n";
    cout << flush;
  }

}

void Graphic3d_GraphicDriver::PrintCLight (const Graphic3d_CLight& ACLight, const Standard_Integer AField) const {

  if (AField) {
    cout << "\tws id " << ACLight.WsId << ", "
      << "view id " << ACLight.ViewId << "\n";
    switch (ACLight.LightType) {
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

void Graphic3d_GraphicDriver::PrintCPick (const Graphic3d_CPick& ACPick, const Standard_Integer AField) const {

  if (AField) {
    cout << "\tws id " << ACPick.WsId
      << " view id " << ACPick.ViewId << "\n";
    cout << "\twindow id " << ACPick.DefWindow.XWindow << "\n";
    cout << "\tposition " << ACPick.x << " , " << ACPick.y << "\n";
    cout << flush;
  }

}

void Graphic3d_GraphicDriver::PrintCPlane (const Graphic3d_CPlane& ACPlane, const Standard_Integer AField) const {

  if (AField) {
    cout << "\tws id " << ACPlane.WsId << ", "
      << "view id " << ACPlane.ViewId << "\n";
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
    cout << "\tws id " << ACView.WsId << ", "
      << "view id " << ACView.ViewId << "\n";
    cout << "\tXwindow id " << ACView.DefWindow.XWindow << ", "
      << "activity " << ACView.Active << "\n";
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
