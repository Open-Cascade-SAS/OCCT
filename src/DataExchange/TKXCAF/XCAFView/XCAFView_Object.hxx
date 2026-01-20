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
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFView_ProjectionType.hxx>

//! Access object for saved view
class XCAFView_Object : public Standard_Transient
{

public:
  Standard_EXPORT XCAFView_Object();

  Standard_EXPORT XCAFView_Object(const occ::handle<XCAFView_Object>& theObj);

  void SetName(occ::handle<TCollection_HAsciiString> theName) { myName = theName; }

  occ::handle<TCollection_HAsciiString> Name() { return myName; }

  void SetType(XCAFView_ProjectionType theType) { myType = theType; }

  XCAFView_ProjectionType Type() { return myType; }

  void SetProjectionPoint(const gp_Pnt& thePoint) { myProjectionPoint = thePoint; }

  gp_Pnt ProjectionPoint() { return myProjectionPoint; }

  void SetViewDirection(const gp_Dir& theDirection) { myViewDirection = theDirection; }

  gp_Dir ViewDirection() { return myViewDirection; }

  void SetUpDirection(const gp_Dir& theDirection) { myUpDirection = theDirection; }

  gp_Dir UpDirection() { return myUpDirection; }

  void SetZoomFactor(double theZoomFactor) { myZoomFactor = theZoomFactor; }

  double ZoomFactor() { return myZoomFactor; }

  void SetWindowHorizontalSize(double theSize) { myWindowHorizontalSize = theSize; }

  double WindowHorizontalSize() { return myWindowHorizontalSize; }

  void SetWindowVerticalSize(double theSize) { myWindowVerticalSize = theSize; }

  double WindowVerticalSize() { return myWindowVerticalSize; }

  void SetClippingExpression(occ::handle<TCollection_HAsciiString> theExpression)
  {
    myClippingExpression = theExpression;
  }

  occ::handle<TCollection_HAsciiString> ClippingExpression() { return myClippingExpression; }

  void UnsetFrontPlaneClipping() { myFrontPlaneClipping = false; }

  bool HasFrontPlaneClipping() { return myFrontPlaneClipping; }

  void SetFrontPlaneDistance(double theDistance)
  {
    myFrontPlaneDistance = theDistance;
    myFrontPlaneClipping = true;
  }

  double FrontPlaneDistance() { return myFrontPlaneDistance; }

  void UnsetBackPlaneClipping() { myBackPlaneClipping = false; }

  bool HasBackPlaneClipping() { return myBackPlaneClipping; }

  void SetBackPlaneDistance(double theDistance)
  {
    myBackPlaneDistance = theDistance;
    myBackPlaneClipping = true;
  }

  double BackPlaneDistance() { return myBackPlaneDistance; }

  void SetViewVolumeSidesClipping(bool theViewVolumeSidesClipping)
  {
    myViewVolumeSidesClipping = theViewVolumeSidesClipping;
  }

  bool HasViewVolumeSidesClipping() { return myViewVolumeSidesClipping; }

  void CreateGDTPoints(const int theLenght)
  {
    if (theLenght > 0)
      myGDTPoints = new NCollection_HArray1<gp_Pnt>(1, theLenght);
  }

  bool HasGDTPoints() { return (!myGDTPoints.IsNull()); }

  int NbGDTPoints()
  {
    if (myGDTPoints.IsNull())
      return 0;
    return myGDTPoints->Length();
  }

  void SetGDTPoint(const int theIndex, const gp_Pnt& thePoint)
  {
    if (myGDTPoints.IsNull())
      return;
    if (theIndex > 0 && theIndex <= myGDTPoints->Length())
      myGDTPoints->SetValue(theIndex, thePoint);
  }

  gp_Pnt GDTPoint(const int theIndex)
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
  occ::handle<TCollection_HAsciiString> myName;
  XCAFView_ProjectionType          myType;
  gp_Pnt                           myProjectionPoint;
  gp_Dir                           myViewDirection;
  gp_Dir                           myUpDirection;
  double                    myZoomFactor;
  double                    myWindowHorizontalSize;
  double                    myWindowVerticalSize;
  occ::handle<TCollection_HAsciiString> myClippingExpression;
  bool                 myFrontPlaneClipping;
  double                    myFrontPlaneDistance;
  bool                 myBackPlaneClipping;
  double                    myBackPlaneDistance;
  bool                 myViewVolumeSidesClipping;
  // clang-format off
  occ::handle<NCollection_HArray1<gp_Pnt>> myGDTPoints; // Point for each GDT to describe position of GDT frame in View.
  // clang-format on
};

#endif // _XCAFView_Object_HeaderFile
