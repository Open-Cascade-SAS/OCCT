// Created on: 1997-01-24
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

#ifndef _AIS_GlobalStatus_HeaderFile
#define _AIS_GlobalStatus_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <AIS_DisplayStatus.hxx>
#include <Prs3d_Drawer.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Transient.hxx>

DEFINE_STANDARD_HANDLE(AIS_GlobalStatus, Standard_Transient)

//! Stores  information  about objects in graphic context:
//! - Status Of Display : in the main viewer
//! hidden in the main viewer
//! - Displayed Modes
//! - Active Selection Modes
//! - is the Interactive Object Current ?
//! - Layer Index
class AIS_GlobalStatus : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_GlobalStatus, Standard_Transient)
public:

  Standard_EXPORT AIS_GlobalStatus();
  
  Standard_EXPORT AIS_GlobalStatus(const AIS_DisplayStatus aStat, const Standard_Integer aDispMode, const Standard_Integer aSelMode, const Standard_Boolean ishilighted = Standard_False, const Standard_Integer aLayerIndex = 0);

  void SetGraphicStatus (const AIS_DisplayStatus theStatus) { myStatus = theStatus; }

  void AddSelectionMode (const Standard_Integer theMode) { if (!IsSModeIn (theMode)) mySelModes.Append (theMode); }

  //! Sets display mode.
  void SetDisplayMode (const Standard_Integer theMode) { myDispMode = theMode; }

  //! Returns the display mode.
  Standard_Integer DisplayMode() const { return myDispMode; }

  void SetLayerIndex (const Standard_Integer theIndex) { myLayerIndex = theIndex; }

  void SetHilightStatus (const Standard_Boolean theStatus) { myIsHilit = theStatus; }

  //! Changes applied highlight style for a particular object
  void SetHilightStyle (const Handle(Prs3d_Drawer)& theStyle) { myHiStyle = theStyle; }

  //! Returns applied highlight style for a particular object
  const Handle(Prs3d_Drawer)& HilightStyle() const { return myHiStyle; }

  Standard_Boolean IsSubIntensityOn() const { return mySubInt; }

  void SubIntensityOn() { mySubInt = Standard_True; }

  void SubIntensityOff() { mySubInt = Standard_False; }
  
  Standard_EXPORT void RemoveSelectionMode (const Standard_Integer aMode);
  
  Standard_EXPORT void ClearSelectionModes();
  
  AIS_DisplayStatus GraphicStatus() const { return myStatus; }
  
  //! keeps the active selection modes of the object
  //! in the main viewer.
  const TColStd_ListOfInteger& SelectionModes() const { return mySelModes; }
  
  Standard_Boolean IsHilighted() const { return myIsHilit; }

  Standard_EXPORT Standard_Boolean IsSModeIn (const Standard_Integer aMode) const;

  //! Returns layer index.
  Standard_Integer GetLayerIndex() const
  {
    return myLayerIndex;
  }

private:

  TColStd_ListOfInteger mySelModes;
  Handle(Prs3d_Drawer) myHiStyle;
  AIS_DisplayStatus myStatus;
  Standard_Integer myDispMode;
  Standard_Integer myLayerIndex;
  Standard_Boolean myIsHilit;
  Standard_Boolean mySubInt;

};

#endif // _AIS_GlobalStatus_HeaderFile
