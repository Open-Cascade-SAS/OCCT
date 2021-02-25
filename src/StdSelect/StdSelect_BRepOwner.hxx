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
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <PrsMgr_PresentationManager.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_ZLayerId.hxx>
class StdSelect_Shape;
class TopoDS_Shape;
class SelectMgr_SelectableObject;
class PrsMgr_PresentationManager;
class TopLoc_Location;

DEFINE_STANDARD_HANDLE(StdSelect_BRepOwner, SelectMgr_EntityOwner)

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
  Standard_EXPORT StdSelect_BRepOwner(const Standard_Integer aPriority);
  
  //! Constructs an owner specification framework defined
  //! by the shape aShape and the priority aPriority.
  //! aShape and aPriority are stored in this framework. If
  //! more than one owner are detected during dynamic
  //! selection, the one with the highest priority is the one stored.
  Standard_EXPORT StdSelect_BRepOwner(const TopoDS_Shape& aShape, const Standard_Integer aPriority = 0, const Standard_Boolean ComesFromDecomposition = Standard_False);
  
  //! Constructs an owner specification framework defined
  //! by the shape aShape, the selectable object theOrigin
  //! and the priority aPriority.
  //! aShape, theOrigin and aPriority are stored in this
  //! framework. If more than one owner are detected
  //! during dynamic selection, the one with the highest
  //! priority is the one stored.
  Standard_EXPORT StdSelect_BRepOwner(const TopoDS_Shape& aShape, const Handle(SelectMgr_SelectableObject)& theOrigin, const Standard_Integer aPriority = 0, const Standard_Boolean FromDecomposition = Standard_False);
  
  //! returns False if no shape was set
  Standard_Boolean HasShape() const { return !myShape.IsNull(); }

  //! Returns the shape.
  const TopoDS_Shape& Shape() const { return myShape; }

  //! Returns true if this framework has a highlight mode defined for it.
  Standard_Boolean HasHilightMode() const { return myCurMode == -1; }
  
  //! Sets the highlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  void SetHilightMode (const Standard_Integer theMode) { myCurMode = theMode; }

  //! Resets the higlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  void ResetHilightMode() { myCurMode = -1; }

  //! Returns the highlight mode for this framework.
  //! This defines the type of display used to highlight the
  //! owner of the shape when it is detected by the selector.
  //! The default type of display is wireframe, defined by the index 0.
  Standard_Integer HilightMode() const { return myCurMode; }

  //! Returns true if an object with the selection mode
  //! aMode is highlighted in the presentation manager aPM.
  Standard_EXPORT virtual Standard_Boolean IsHilighted (const Handle(PrsMgr_PresentationManager)& aPM, const Standard_Integer aMode = 0) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager)& thePM,
                                                 const Handle(Prs3d_Drawer)& theStyle,
                                                 const Standard_Integer theMode) Standard_OVERRIDE;
  
  //! Removes highlighting from the type of shape
  //! identified the selection mode aMode in the presentation manager aPM.
  Standard_EXPORT virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& aPM, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  //! Clears the presentation manager object aPM of all
  //! shapes with the selection mode aMode.
  Standard_EXPORT virtual void Clear (const Handle(PrsMgr_PresentationManager)& aPM, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetLocation (const TopLoc_Location& aLoc) Standard_OVERRIDE;

  //! Implements immediate application of location transformation of parent object to dynamic highlight structure
  Standard_EXPORT virtual void UpdateHighlightTrsf (const Handle(V3d_Viewer)& theViewer,
                                                    const Handle(PrsMgr_PresentationManager)& theManager,
                                                    const Standard_Integer theDispMode) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  TopoDS_Shape myShape;
  Handle(StdSelect_Shape) myPrsSh;
  Standard_Integer myCurMode;

};

#endif // _StdSelect_BRepOwner_HeaderFile
