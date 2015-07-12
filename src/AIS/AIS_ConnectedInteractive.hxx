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

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <AIS_KindOfInteractive.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_Selection.hxx>
class AIS_InteractiveObject;
class Standard_NotImplemented;
class gp_Trsf;
class Prs3d_Presentation;
class Prs3d_Projector;
class Geom_Transformation;
class TopoDS_Shape;


class AIS_ConnectedInteractive;
DEFINE_STANDARD_HANDLE(AIS_ConnectedInteractive, AIS_InteractiveObject)

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

public:

  

  //! Disconnects the previous view and sets highlight
  //! mode to 0. This highlights the wireframe presentation
  //! aTypeOfPresentation3d.
  //! Top_AllView deactivates hidden line removal.
  Standard_EXPORT AIS_ConnectedInteractive(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);
  
  //! Returns KOI_Object
  Standard_EXPORT virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  //! Returns 1
  Standard_EXPORT virtual Standard_Integer Signature() const Standard_OVERRIDE;
  
  //! Establishes the connection between the Connected
  //! Interactive Object, anotherIobj, and its reference.
  Standard_EXPORT virtual void Connect (const Handle(AIS_InteractiveObject)& anotherIObj);
  
  //! Establishes the connection between the Connected
  //! Interactive Object, anotherIobj, and its reference.
  //! Locates instance in aLocation.
  Standard_EXPORT virtual void Connect (const Handle(AIS_InteractiveObject)& anotherIobj, const gp_Trsf& aLocation);
  

  //! Returns true if there is a connection established
  //! between the presentation and its source reference.
    Standard_Boolean HasConnection() const;
  

  //! Returns the connection with the reference Interactive Object.
    const Handle(AIS_InteractiveObject)& ConnectedTo() const;
  
  //! Clears the connection with a source reference. The
  //! presentation will no longer be displayed.
  //! Warning Must be done before deleting the presentation.
  Standard_EXPORT void Disconnect();
  
  //! Informs the graphic context that the interactive Object
  //! may be decomposed into sub-shapes for dynamic selection.
    virtual Standard_Boolean AcceptShapeDecomposition() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(AIS_ConnectedInteractive,AIS_InteractiveObject)

protected:


  Handle(AIS_InteractiveObject) myReference;


private:

  
  //! Calculates the view aPresentation and its updates.
  //! The latter are managed by aPresentationManager.
  //! The display mode aMode is 0 by default.
  //! this method is redefined virtual;
  //! when the instance is connected to another
  //! InteractiveObject,this method doesn't
  //! compute anything, but just uses the
  //! presentation of this last object, with
  //! a transformation if there's one stored.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;
  
  //! Computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  //! Computes the presentation according to a point of view
  //! given by <aProjector>.
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  //! Generates sensitive entities by copying
  //! them from myReference selection, creates and sets an entity
  //! owner for this entities and adds them to theSelection
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection, const Standard_Integer theMode) Standard_OVERRIDE;
  
  //! Generates sensitive entities by copying
  //! them from myReference sub shapes selection, creates and sets an entity
  //! owner for this entities and adds them to theSelection
  Standard_EXPORT void computeSubShapeSelection (const Handle(SelectMgr_Selection)& theSelection, const Standard_Integer theMode);
  
  Standard_EXPORT void updateShape (const Standard_Boolean WithLocation = Standard_True);
  
  //! Computes the presentation according to a point of view
  //! given by <aProjector>.
  Standard_EXPORT void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation, const TopoDS_Shape& aShape);

  TopoDS_Shape myShape;


};


#include <AIS_ConnectedInteractive.lxx>





#endif // _AIS_ConnectedInteractive_HeaderFile
