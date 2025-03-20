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

#ifndef _AIS_ColoredDrawer_HeaderFile
#define _AIS_ColoredDrawer_HeaderFile

#include <Prs3d_Drawer.hxx>
#include <Quantity_Color.hxx>

//! Customizable properties.
class AIS_ColoredDrawer : public Prs3d_Drawer
{
  DEFINE_STANDARD_RTTIEXT(AIS_ColoredDrawer, Prs3d_Drawer)
public:
  //! Default constructor.
  AIS_ColoredDrawer(const Handle(Prs3d_Drawer)& theLink)
      : myIsHidden(false),
        myHasOwnMaterial(false),
        myHasOwnColor(false),
        myHasOwnTransp(false),
        myHasOwnWidth(false)
  {
    Link(theLink);
  }

  bool IsHidden() const { return myIsHidden; }

  void SetHidden(const bool theToHide) { myIsHidden = theToHide; }

  bool HasOwnMaterial() const { return myHasOwnMaterial; }

  void UnsetOwnMaterial() { myHasOwnMaterial = false; }

  void SetOwnMaterial() { myHasOwnMaterial = true; }

  bool HasOwnColor() const { return myHasOwnColor; }

  void UnsetOwnColor() { myHasOwnColor = false; }

  void SetOwnColor(const Quantity_Color& /*theColor*/) { myHasOwnColor = true; }

  bool HasOwnTransparency() const { return myHasOwnTransp; }

  void UnsetOwnTransparency() { myHasOwnTransp = false; }

  void SetOwnTransparency(Standard_Real /*theTransp*/) { myHasOwnTransp = true; }

  bool HasOwnWidth() const { return myHasOwnWidth; }

  void UnsetOwnWidth() { myHasOwnWidth = false; }

  void SetOwnWidth(const Standard_Real /*theWidth*/) { myHasOwnWidth = true; }

public: //! @name list of overridden properties
  bool myIsHidden;
  bool myHasOwnMaterial;
  bool myHasOwnColor;
  bool myHasOwnTransp;
  bool myHasOwnWidth;
};

DEFINE_STANDARD_HANDLE(AIS_ColoredDrawer, Prs3d_Drawer)

#endif // _AIS_ColoredDrawer_HeaderFile
