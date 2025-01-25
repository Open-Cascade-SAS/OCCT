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

#ifndef _XCAFView_Object_HeaderFile
#define _XCAFView_Object_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFView_ProjectionType.hxx>

class XCAFView_Object;
DEFINE_STANDARD_HANDLE(XCAFView_Object, Standard_Transient)

//! Access object for saved view
class XCAFView_Object : public Standard_Transient
{

public:
  Standard_EXPORT XCAFView_Object();

  Standard_EXPORT XCAFView_Object(const Handle(XCAFView_Object)& theObj);

  void SetName(Handle(TCollection_HAsciiString) theName) { myName = theName; }

  Handle(TCollection_HAsciiString) Name() { return myName; }

  void SetType(XCAFView_ProjectionType theType) { myType = theType; }

  XCAFView_ProjectionType Type() { return myType; }

  void SetProjectionPoint(const gp_Pnt& thePoint) { myProjectionPoint = thePoint; }

  gp_Pnt ProjectionPoint() { return myProjectionPoint; }

  void SetViewDirection(const gp_Dir& theDirection) { myViewDirection = theDirection; }

  gp_Dir ViewDirection() { return myViewDirection; }

  void SetUpDirection(const gp_Dir& theDirection) { myUpDirection = theDirection; }

  gp_Dir UpDirection() { return myUpDirection; }

  void SetZoomFactor(Standard_Real theZoomFactor) { myZoomFactor = theZoomFactor; }

  Standard_Real ZoomFactor() { return myZoomFactor; }

  void SetWindowHorizontalSize(Standard_Real theSize) { myWindowHorizontalSize = theSize; }

  Standard_Real WindowHorizontalSize() { return myWindowHorizontalSize; }

  void SetWindowVerticalSize(Standard_Real theSize) { myWindowVerticalSize = theSize; }

  Standard_Real WindowVerticalSize() { return myWindowVerticalSize; }

  void SetClippingExpression(Handle(TCollection_HAsciiString) theExpression)
  {
    myClippingExpression = theExpression;
  }

  Handle(TCollection_HAsciiString) ClippingExpression() { return myClippingExpression; }

  void UnsetFrontPlaneClipping() { myFrontPlaneClipping = Standard_False; }

  Standard_Boolean HasFrontPlaneClipping() { return myFrontPlaneClipping; }

  void SetFrontPlaneDistance(Standard_Real theDistance)
  {
    myFrontPlaneDistance = theDistance;
    myFrontPlaneClipping = Standard_True;
  }

  Standard_Real FrontPlaneDistance() { return myFrontPlaneDistance; }

  void UnsetBackPlaneClipping() { myBackPlaneClipping = Standard_False; }

  Standard_Boolean HasBackPlaneClipping() { return myBackPlaneClipping; }

  void SetBackPlaneDistance(Standard_Real theDistance)
  {
    myBackPlaneDistance = theDistance;
    myBackPlaneClipping = Standard_True;
  }

  Standard_Real BackPlaneDistance() { return myBackPlaneDistance; }

  void SetViewVolumeSidesClipping(Standard_Boolean theViewVolumeSidesClipping)
  {
    myViewVolumeSidesClipping = theViewVolumeSidesClipping;
  }

  Standard_Boolean HasViewVolumeSidesClipping() { return myViewVolumeSidesClipping; }

  void CreateGDTPoints(const Standard_Integer theLenght)
  {
    if (theLenght > 0)
      myGDTPoints = new TColgp_HArray1OfPnt(1, theLenght);
  }

  Standard_Boolean HasGDTPoints() { return (!myGDTPoints.IsNull()); }

  Standard_Integer NbGDTPoints()
  {
    if (myGDTPoints.IsNull())
      return 0;
    return myGDTPoints->Length();
  }

  void SetGDTPoint(const Standard_Integer theIndex, const gp_Pnt& thePoint)
  {
    if (myGDTPoints.IsNull())
      return;
    if (theIndex > 0 && theIndex <= myGDTPoints->Length())
      myGDTPoints->SetValue(theIndex, thePoint);
  }

  gp_Pnt GDTPoint(const Standard_Integer theIndex)
  {
    if (myGDTPoints.IsNull())
      return gp_Pnt();
    if (theIndex > 0 && theIndex <= myGDTPoints->Length())
      return myGDTPoints->Value(theIndex);
    else
      return gp_Pnt();
  }

  DEFINE_STANDARD_RTTIEXT(XCAFView_Object, Standard_Transient)

private:
  Handle(TCollection_HAsciiString) myName;
  XCAFView_ProjectionType          myType;
  gp_Pnt                           myProjectionPoint;
  gp_Dir                           myViewDirection;
  gp_Dir                           myUpDirection;
  Standard_Real                    myZoomFactor;
  Standard_Real                    myWindowHorizontalSize;
  Standard_Real                    myWindowVerticalSize;
  Handle(TCollection_HAsciiString) myClippingExpression;
  Standard_Boolean                 myFrontPlaneClipping;
  Standard_Real                    myFrontPlaneDistance;
  Standard_Boolean                 myBackPlaneClipping;
  Standard_Real                    myBackPlaneDistance;
  Standard_Boolean                 myViewVolumeSidesClipping;
  // clang-format off
  Handle(TColgp_HArray1OfPnt) myGDTPoints; // Point for each GDT to describe position of GDT frame in View.
  // clang-format on
};

#endif // _XCAFView_Object_HeaderFile
