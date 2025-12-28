// Created on: 1997-01-08
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

#ifndef _AIS_ConnectedInteractive_HeaderFile
#define _AIS_ConnectedInteractive_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <TopoDS_Shape.hxx>

//! Creates an arbitrary located instance of another Interactive Object,
//! which serves as a reference.
//! This allows you to use the Connected Interactive
//! Object without having to recalculate presentation,
//! selection or graphic structure. These are deduced
//! from your reference object.
//! The relation between the connected interactive object
//! and its source is generally one of geometric transformation.
//! AIS_ConnectedInteractive class supports selection mode 0 for any InteractiveObject and
//! all standard modes if its reference based on AIS_Shape.
//! Descendants may redefine ComputeSelection() though.
//! Also ConnectedInteractive will handle HLR if its reference based on AIS_Shape.
class AIS_ConnectedInteractive : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_ConnectedInteractive, AIS_InteractiveObject)
public:
  //! Disconnects the previous view and sets highlight
  //! mode to 0. This highlights the wireframe presentation
  //! aTypeOfPresentation3d.
  //! Top_AllView deactivates hidden line removal.
  Standard_EXPORT AIS_ConnectedInteractive(
    const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

  //! Returns KOI_Object
  AIS_KindOfInteractive Type() const override { return AIS_KindOfInteractive_Object; }

  //! Returns 0
  int Signature() const override { return 0; }

  //! Establishes the connection between the Connected
  //! Interactive Object, anotherIobj, and its reference.
  void Connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj)
  {
    connect(theAnotherObj, occ::handle<TopLoc_Datum3D>());
  }

  //! Establishes the connection between the Connected
  //! Interactive Object, anotherIobj, and its reference.
  //! Locates instance in aLocation.
  void Connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj, const gp_Trsf& theLocation)
  {
    connect(theAnotherObj, new TopLoc_Datum3D(theLocation));
  }

  //! Establishes the connection between the Connected
  //! Interactive Object, anotherIobj, and its reference.
  //! Locates instance in aLocation.
  void Connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj,
               const occ::handle<TopLoc_Datum3D>&        theLocation)
  {
    connect(theAnotherObj, theLocation);
  }

  //! Returns true if there is a connection established
  //! between the presentation and its source reference.
  bool HasConnection() const { return !myReference.IsNull(); }

  //! Returns the connection with the reference Interactive Object.
  const occ::handle<AIS_InteractiveObject>& ConnectedTo() const { return myReference; }

  //! Clears the connection with a source reference. The
  //! presentation will no longer be displayed.
  //! Warning Must be done before deleting the presentation.
  Standard_EXPORT void Disconnect();

  //! Informs the graphic context that the interactive Object
  //! may be decomposed into sub-shapes for dynamic selection.
  bool AcceptShapeDecomposition() const override
  {
    return !myReference.IsNull() && myReference->AcceptShapeDecomposition();
  }

  //! Return true if reference presentation accepts specified display mode.
  bool AcceptDisplayMode(const int theMode) const override
  {
    return myReference.IsNull() || myReference->AcceptDisplayMode(theMode);
  }

protected:
  //! Calculates the view aPresentation and its updates.
  //! The latter are managed by aPresentationManager.
  //! The display mode aMode is 0 by default.
  //! this method is redefined virtual;
  //! when the instance is connected to another
  //! InteractiveObject,this method doesn't
  //! compute anything, but just uses the
  //! presentation of this last object, with
  //! a transformation if there's one stored.
  Standard_EXPORT void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         theprs,
                                       const int theMode) override;

  //! Computes the presentation according to a point of view.
  Standard_EXPORT void computeHLR(const occ::handle<Graphic3d_Camera>&   theProjector,
                                          const occ::handle<TopLoc_Datum3D>&     theTrsf,
                                          const occ::handle<Prs3d_Presentation>& thePrs) override;

  //! Generates sensitive entities by copying
  //! them from myReference selection, creates and sets an entity
  //! owner for this entities and adds them to theSelection
  Standard_EXPORT void ComputeSelection(
    const occ::handle<SelectMgr_Selection>& theSelection,
    const int                               theMode) override;

  //! Generates sensitive entities by copying
  //! them from myReference sub shapes selection, creates and sets an entity
  //! owner for this entities and adds them to theSelection
  Standard_EXPORT void computeSubShapeSelection(
    const occ::handle<SelectMgr_Selection>& theSelection,
    const int                               theMode);

  Standard_EXPORT void updateShape(const bool WithLocation = true);

  Standard_EXPORT void connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj,
                               const occ::handle<TopLoc_Datum3D>&        theLocation);

protected:
  occ::handle<AIS_InteractiveObject> myReference;
  TopoDS_Shape                       myShape;
};

#endif // _AIS_ConnectedInteractive_HeaderFile
