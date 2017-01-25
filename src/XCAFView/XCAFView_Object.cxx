// Created on: 2016-10-20
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <XCAFView_Object.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFView_Object,Standard_Transient)

//=======================================================================
//function : XCAFView_Object
//purpose  : 
//=======================================================================
XCAFView_Object::XCAFView_Object()
{
  myClippingExpression = new TCollection_HAsciiString();
  myFrontPlaneClipping = Standard_False;
  myBackPlaneClipping = Standard_False;
  myViewVolumeSidesClipping = Standard_False;
  myGDTPoints = NULL;
}

//=======================================================================
//function : XCAFView_Object
//purpose  : 
//=======================================================================
XCAFView_Object::XCAFView_Object(const Handle(XCAFView_Object)& theObj)
{
  myType = theObj->myType;
  myProjectionPoint = theObj->myProjectionPoint;
  myViewDirection = theObj->myViewDirection;
  myUpDirection = theObj->myUpDirection;
  myZoomFactor = theObj->myZoomFactor;
  myWindowHorizontalSize = theObj->myWindowHorizontalSize;
  myWindowVerticalSize = theObj->myWindowVerticalSize;
  myClippingExpression = theObj->myClippingExpression;
  myFrontPlaneClipping = theObj->myFrontPlaneClipping;
  myFrontPlaneDistance = theObj->myFrontPlaneDistance;
  myBackPlaneClipping = theObj->myBackPlaneClipping;
  myBackPlaneDistance = theObj->myBackPlaneDistance;
  myViewVolumeSidesClipping = theObj->myViewVolumeSidesClipping;
  myGDTPoints = NULL;
}

