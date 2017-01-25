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

#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFView_ProjectionType.hxx>

class XCAFView_Object;
DEFINE_STANDARD_HANDLE(XCAFView_Object, Standard_Transient)
                            
//! object to store view
class XCAFView_Object : public Standard_Transient
{

public:
  
  Standard_EXPORT XCAFView_Object();
  
  Standard_EXPORT XCAFView_Object(const Handle(XCAFView_Object)& theObj);

  Standard_EXPORT void SetName(Handle(TCollection_HAsciiString) theName)
  {
    myName = theName;
  }

  Standard_EXPORT Handle(TCollection_HAsciiString) Name()
  {
    return myName;
  }

  Standard_EXPORT void SetType(XCAFView_ProjectionType theType)
  {
    myType = theType;
  }

  Standard_EXPORT XCAFView_ProjectionType Type()
  {
    return myType;
  }

  Standard_EXPORT void SetProjectionPoint(gp_Pnt thePoint)
  {
    myProjectionPoint = thePoint;
  }

  Standard_EXPORT gp_Pnt ProjectionPoint()
  {
    return myProjectionPoint;
  }

  Standard_EXPORT void SetViewDirection(gp_Dir theDirection)
  {
    myViewDirection = theDirection;
  }

  Standard_EXPORT gp_Dir ViewDirection()
  {
    return myViewDirection;
  }

  Standard_EXPORT void SetUpDirection(gp_Dir theDirection)
  {
    myUpDirection = theDirection;
  }

  Standard_EXPORT gp_Dir UpDirection()
  {
    return myUpDirection;
  }

  Standard_EXPORT void SetZoomFactor(Standard_Real theZoomFactor)
  {
    myZoomFactor = theZoomFactor;
  }

  Standard_EXPORT Standard_Real ZoomFactor()
  {
    return myZoomFactor;
  }

  Standard_EXPORT void SetWindowHorizontalSize(Standard_Real theSize)
  {
    myWindowHorizontalSize = theSize;
  }

  Standard_EXPORT Standard_Real WindowHorizontalSize()
  {
    return myWindowHorizontalSize;
  }

  Standard_EXPORT void SetWindowVerticalSize(Standard_Real theSize)
  {
    myWindowVerticalSize = theSize;
  }

  Standard_EXPORT Standard_Real WindowVerticalSize()
  {
    return myWindowVerticalSize;
  }

  Standard_EXPORT void SetClippingExpression(Handle(TCollection_HAsciiString) theExpression)
  {
    myClippingExpression = theExpression;
  }

  Standard_EXPORT Handle(TCollection_HAsciiString) ClippingExpression()
  {
    return myClippingExpression;
  }

  Standard_EXPORT void UnsetFrontPlaneClipping()
  {
    myFrontPlaneClipping = Standard_False;
  }

  Standard_EXPORT Standard_Boolean HasFrontPlaneClipping()
  {
    return myFrontPlaneClipping;
  }

  Standard_EXPORT void SetFrontPlaneDistance(Standard_Real theDistance)
  {
    myFrontPlaneDistance = theDistance;
    myFrontPlaneClipping = Standard_True;
  }

  Standard_EXPORT Standard_Real FrontPlaneDistance()
  {
    return myFrontPlaneDistance;
  }

  Standard_EXPORT void UnsetBackPlaneClipping()
  {
    myBackPlaneClipping = Standard_False;
  }

  Standard_EXPORT Standard_Boolean HasBackPlaneClipping()
  {
    return myBackPlaneClipping;
  }

  Standard_EXPORT void SetBackPlaneDistance(Standard_Real theDistance)
  {
    myBackPlaneDistance = theDistance;
    myBackPlaneClipping = Standard_True;
  }

  Standard_EXPORT Standard_Real BackPlaneDistance()
  {
    return myBackPlaneDistance;
  }

  Standard_EXPORT void SetViewVolumeSidesClipping(Standard_Boolean theViewVolumeSidesClipping)
  {
    myViewVolumeSidesClipping = theViewVolumeSidesClipping;
  }

  Standard_EXPORT Standard_Boolean HasViewVolumeSidesClipping()
  {
    return myViewVolumeSidesClipping;
  }

  Standard_EXPORT void CreateGDTPoints(const Standard_Integer theLenght)
  {
    if (theLenght > 0)
      myGDTPoints = new TColgp_HArray1OfPnt(1, theLenght);
  }

  Standard_EXPORT Standard_Boolean HasGDTPoints()
  {
    return (!myGDTPoints.IsNull());
  }

  Standard_EXPORT Standard_Integer NbGDTPoints()
  {
    if (myGDTPoints.IsNull())
      return 0;
    return myGDTPoints->Length();
  }

  Standard_EXPORT void SetGDTPoint(const Standard_Integer theIndex, const gp_Pnt thePoint)
  {
    if (myGDTPoints.IsNull())
      return;
    if (theIndex > 0 && theIndex <= myGDTPoints->Length())
      myGDTPoints->SetValue(theIndex, thePoint);
  }

  Standard_EXPORT gp_Pnt GDTPoint(const Standard_Integer theIndex)
  {
    if (myGDTPoints.IsNull())
      return gp_Pnt();
    if (theIndex > 0 && theIndex <= myGDTPoints->Length())
      return myGDTPoints->Value(theIndex);
    else
      return gp_Pnt();
  }
  
  DEFINE_STANDARD_RTTIEXT(XCAFView_Object,Standard_Transient)

private:

  Handle(TCollection_HAsciiString) myName;
  XCAFView_ProjectionType myType;
  gp_Pnt myProjectionPoint;
  gp_Dir myViewDirection;
  gp_Dir myUpDirection;
  Standard_Real myZoomFactor;
  Standard_Real myWindowHorizontalSize;
  Standard_Real myWindowVerticalSize;
  Handle(TCollection_HAsciiString) myClippingExpression;
  Standard_Boolean myFrontPlaneClipping;
  Standard_Real myFrontPlaneDistance;
  Standard_Boolean myBackPlaneClipping;
  Standard_Real myBackPlaneDistance;
  Standard_Boolean myViewVolumeSidesClipping;
  Handle(TColgp_HArray1OfPnt) myGDTPoints; // Point for each GDT to describe position of GDT frame in View.
};

#endif // _XCAFView_Object_HeaderFile
