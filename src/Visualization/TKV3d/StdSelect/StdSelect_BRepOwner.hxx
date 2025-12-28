// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StdSelect_BRepOwner_HeaderFile
#define _StdSelect_BRepOwner_HeaderFile

#include <Standard.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <PrsMgr_PresentationManager.hxx>
class StdSelect_Shape;
class SelectMgr_SelectableObject;
class PrsMgr_PresentationManager;
class TopLoc_Location;

//! Defines Specific Owners for Sensitive Primitives
//! (Sensitive Segments,Circles...).
//! Used in Dynamic Selection Mechanism.
//! A BRepOwner has an Owner (the shape it represents)
//! and Users (One or More Transient entities).
//! The highlight-unhighlight methods are empty and
//! must be redefined by each User.
class StdSelect_BRepOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(StdSelect_BRepOwner, SelectMgr_EntityOwner)
public:
  //! Constructs an owner specification framework defined
  //! by the priority aPriority.
  Standard_EXPORT StdSelect_BRepOwner(const int aPriority);

  //! Constructs an owner specification framework defined
  //! by the shape aShape and the priority aPriority.
  //! aShape and aPriority are stored in this framework. If
  //! more than one owner are detected during dynamic
  //! selection, the one with the highest priority is the one stored.
  Standard_EXPORT StdSelect_BRepOwner(const TopoDS_Shape& aShape,
                                      const int           aPriority              = 0,
                                      const bool          ComesFromDecomposition = false);

  //! Constructs an owner specification framework defined
  //! by the shape aShape, the selectable object theOrigin
  //! and the priority aPriority.
  //! aShape, theOrigin and aPriority are stored in this
  //! framework. If more than one owner are detected
  //! during dynamic selection, the one with the highest
  //! priority is the one stored.
  Standard_EXPORT StdSelect_BRepOwner(const TopoDS_Shape&                            aShape,
                                      const occ::handle<SelectMgr_SelectableObject>& theOrigin,
                                      const int                                      aPriority = 0,
                                      const bool FromDecomposition = false);

  //! returns False if no shape was set
  bool HasShape() const { return !myShape.IsNull(); }

  //! Returns the shape.
  const TopoDS_Shape& Shape() const { return myShape; }

  //! Returns true if this framework has a highlight mode defined for it.
  bool HasHilightMode() const { return myCurMode == -1; }

  //! Sets the highlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  void SetHilightMode(const int theMode) { myCurMode = theMode; }

  //! Resets the higlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  void ResetHilightMode() { myCurMode = -1; }

  //! Returns the highlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  int HilightMode() const { return myCurMode; }

  //! Returns true if an object with the selection mode
  //! aMode is highlighted in the presentation manager aPM.
  Standard_EXPORT bool IsHilighted(const occ::handle<PrsMgr_PresentationManager>& aPM,
                                           const int aMode = 0) const override;

  Standard_EXPORT void HilightWithColor(
    const occ::handle<PrsMgr_PresentationManager>& thePM,
    const occ::handle<Prs3d_Drawer>&               theStyle,
    const int                                      theMode) override;

  //! Removes highlighting from the type of shape
  //! identified the selection mode aMode in the presentation manager aPM.
  Standard_EXPORT void Unhilight(const occ::handle<PrsMgr_PresentationManager>& aPM,
                                         const int aMode = 0) override;

  //! Clears the presentation manager object aPM of all
  //! shapes with the selection mode aMode.
  Standard_EXPORT void Clear(const occ::handle<PrsMgr_PresentationManager>& aPM,
                                     const int aMode = 0) override;

  Standard_EXPORT void SetLocation(const TopLoc_Location& aLoc) override;

  //! Implements immediate application of location transformation of parent object to dynamic
  //! highlight structure
  Standard_EXPORT void UpdateHighlightTrsf(
    const occ::handle<V3d_Viewer>&                 theViewer,
    const occ::handle<PrsMgr_PresentationManager>& theManager,
    const int                                      theDispMode) override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

protected:
  TopoDS_Shape                 myShape;
  occ::handle<StdSelect_Shape> myPrsSh;
  int                          myCurMode;
};

#endif // _StdSelect_BRepOwner_HeaderFile
