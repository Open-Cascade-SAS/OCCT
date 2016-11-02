// Created on: 1997-01-20
// Created by: Robert COUBLANC
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

#ifndef _AIS_LocalStatus_HeaderFile
#define _AIS_LocalStatus_HeaderFile

#include <Prs3d_Drawer.hxx>
#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>

DEFINE_STANDARD_HANDLE(AIS_LocalStatus, Standard_Transient)

//! Stored Info about temporary objects.
class AIS_LocalStatus : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_LocalStatus, Standard_Transient)
public:

  Standard_EXPORT AIS_LocalStatus (const Standard_Boolean theIsTemporary = Standard_True,
                                   const Standard_Boolean theIsToDecompose = Standard_False,
                                   const Standard_Integer theDisplayMode = -1,
                                   const Standard_Integer theSelectionMode = -1,
                                   const Standard_Integer theHilightMode = 0,
                                   const Standard_Boolean theIsSubIntensity = 0,
                                   const Handle(Prs3d_Drawer)& theStyle = Handle(Prs3d_Drawer)());
  
    Standard_Boolean Decomposed() const;
  
    Standard_Boolean IsTemporary() const;
  
    Standard_Integer DisplayMode() const;
  
    const TColStd_ListOfInteger& SelectionModes() const;
  
  Standard_EXPORT Standard_Boolean IsActivated (const Standard_Integer aSelMode) const;
  
    Standard_Integer HilightMode() const;
  
    Standard_Boolean IsSubIntensityOn() const;
  
  const Handle(Prs3d_Drawer)& HilightStyle() const
  {
    return myHiStyle;
  }
  
    void SetDecomposition (const Standard_Boolean astatus);
  
    void SetTemporary (const Standard_Boolean astatus);
  
    void SetDisplayMode (const Standard_Integer aMode);
  
    void SetFirstDisplay (const Standard_Boolean aStatus);
  
    Standard_Boolean IsFirstDisplay() const;
  
  Standard_EXPORT void AddSelectionMode (const Standard_Integer aMode);
  
  Standard_EXPORT void RemoveSelectionMode (const Standard_Integer aMode);
  
  Standard_EXPORT void ClearSelectionModes();
  
  Standard_EXPORT Standard_Boolean IsSelModeIn (const Standard_Integer aMode) const;
  
    void SetHilightMode (const Standard_Integer aMode);
  
  void SetHilightStyle (const Handle(Prs3d_Drawer)& theStyle)
  {
    myHiStyle = theStyle;
  }
  
    void SubIntensityOn();
  
    void SubIntensityOff();
  
    void SetPreviousState (const Handle(Standard_Transient)& aStatus);
  
    const Handle(Standard_Transient)& PreviousState() const;

private:
  Standard_Boolean myDecomposition;
  Standard_Boolean myIsTemporary;
  Standard_Integer myDMode;
  Standard_Boolean myFirstDisplay;
  Standard_Integer myHMode;
  TColStd_ListOfInteger mySModes;
  Standard_Boolean mySubIntensity;
  Handle(Standard_Transient) myPreviousState;
  Handle(Prs3d_Drawer) myHiStyle;
};

#include <AIS_LocalStatus.lxx>

#endif // _AIS_LocalStatus_HeaderFile
