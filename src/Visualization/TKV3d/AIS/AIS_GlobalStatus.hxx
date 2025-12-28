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

#include <Prs3d_Drawer.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <Standard_Transient.hxx>

//! Stores information about objects in graphic context:
class AIS_GlobalStatus : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(AIS_GlobalStatus, Standard_Transient)
public:
  //! Default constructor.
  Standard_EXPORT AIS_GlobalStatus();

  //! Returns the display mode.
  int DisplayMode() const { return myDispMode; }

  //! Sets display mode.
  void SetDisplayMode(const int theMode) { myDispMode = theMode; }

  //! Returns TRUE if object is highlighted
  bool IsHilighted() const { return myIsHilit; }

  //! Sets highlighted state.
  void SetHilightStatus(const bool theStatus) { myIsHilit = theStatus; }

  //! Changes applied highlight style for a particular object
  void SetHilightStyle(const occ::handle<Prs3d_Drawer>& theStyle) { myHiStyle = theStyle; }

  //! Returns applied highlight style for a particular object
  const occ::handle<Prs3d_Drawer>& HilightStyle() const { return myHiStyle; }

  //! Returns active selection modes of the object.
  const NCollection_List<int>& SelectionModes() const { return mySelModes; }

  //! Return TRUE if selection mode was registered.
  bool IsSModeIn(int theMode) const { return mySelModes.Contains(theMode); }

  //! Add selection mode.
  bool AddSelectionMode(const int theMode)
  {
    if (!mySelModes.Contains(theMode))
    {
      mySelModes.Append(theMode);
      return true;
    }
    return false;
  }

  //! Remove selection mode.
  bool RemoveSelectionMode(const int theMode) { return mySelModes.Remove(theMode); }

  //! Remove all selection modes.
  void ClearSelectionModes() { mySelModes.Clear(); }

  bool IsSubIntensityOn() const { return mySubInt; }

  void SetSubIntensity(bool theIsOn) { mySubInt = theIsOn; }

private:
  NCollection_List<int>     mySelModes;
  occ::handle<Prs3d_Drawer> myHiStyle;
  int                       myDispMode;
  bool                      myIsHilit;
  bool                      mySubInt;
};

#endif // _AIS_GlobalStatus_HeaderFile
