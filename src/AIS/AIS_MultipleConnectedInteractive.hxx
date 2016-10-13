// Created on: 1997-04-22
// Created by: Guest Design
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

#ifndef _AIS_MultipleConnectedInteractive_HeaderFile
#define _AIS_MultipleConnectedInteractive_HeaderFile

#include <AIS_InteractiveObject.hxx>
#include <AIS_KindOfInteractive.hxx>

//! Defines an Interactive Object by gathering together
//! several object presentations. This is done through a
//! list of interactive objects. These can also be
//! Connected objects. That way memory-costly
//! calculations of presentation are avoided.
class AIS_MultipleConnectedInteractive : public AIS_InteractiveObject
{
  DEFINE_STANDARD_RTTIEXT(AIS_MultipleConnectedInteractive, AIS_InteractiveObject)
public:

  //! Initializes the Interactive Object with multiple
  //! connections to AIS_Interactive objects.
  Standard_EXPORT AIS_MultipleConnectedInteractive();

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its reference.
  //! Locates instance in theLocation and applies specified transformation persistence mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Handle(AIS_InteractiveObject) Connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                         const Handle(Geom_Transformation)& theLocation,
                                         const Handle(Graphic3d_TransformPers)& theTrsfPers)
  {
    return connect (theAnotherObj, theLocation, theTrsfPers);
  }

  Standard_EXPORT virtual AIS_KindOfInteractive Type() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Integer Signature() const Standard_OVERRIDE;
  
  //! Returns true if the object is connected to others.
  Standard_EXPORT Standard_Boolean HasConnection() const;
  
  //! Removes the connection with theInteractive.
  Standard_EXPORT void Disconnect (const Handle(AIS_InteractiveObject)& theInteractive);
  
  //! Clears all the connections to objects.
  Standard_EXPORT void DisconnectAll();
  
  //! computes the presentation according to a point of view
  //! given by <aProjector>.
  //! To be Used when the associated degenerated Presentations
  //! have been transformed by <aTrsf> which is not a Pure
  //! Translation. The HLR Prs can't be deducted automatically
  //! WARNING :<aTrsf> must be applied
  //! to the object to display before computation  !!!
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Geom_Transformation)& aTrsf, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void Compute (const Handle(Prs3d_Projector)& aProjector, const Handle(Prs3d_Presentation)& aPresentation) Standard_OVERRIDE;
  
  //! Informs the graphic context that the interactive Object
  //! may be decomposed into sub-shapes for dynamic selection.
  Standard_EXPORT virtual Standard_Boolean AcceptShapeDecomposition() const Standard_OVERRIDE;

  //! Returns the owner of mode for selection of object as a whole
  Standard_EXPORT virtual Handle(SelectMgr_EntityOwner) GlobalSelOwner() const Standard_OVERRIDE;

  //! Returns true if a selection corresponding to the selection mode theMode was computed for all
  //! children of multiple connected interactive object.
  Standard_EXPORT virtual Standard_Boolean HasSelection (const Standard_Integer theMode) const Standard_OVERRIDE;

  //! Assigns interactive context.
  Standard_EXPORT virtual void SetContext (const Handle(AIS_InteractiveContext)& theCtx) Standard_OVERRIDE;

public: // short aliases to Connect() method

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its reference.
  //! Copies local transformation and transformation persistence mode from theInteractive.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Handle(AIS_InteractiveObject) Connect (const Handle(AIS_InteractiveObject)& theAnotherObj)
  {
    return connect (theAnotherObj, theAnotherObj->LocalTransformationGeom(), theAnotherObj->TransformPersistence());
  }

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its reference.
  //! Locates instance in theLocation and copies transformation persistence mode from theInteractive.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Handle(AIS_InteractiveObject) Connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                         const gp_Trsf& theLocation)
  {
    return connect (theAnotherObj, new Geom_Transformation (theLocation), theAnotherObj->TransformPersistence());
  }

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its reference.
  //! Locates instance in theLocation and applies specified transformation persistence mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Handle(AIS_InteractiveObject) Connect (const Handle(AIS_InteractiveObject)& theAnotherObj,
                                         const gp_Trsf& theLocation,
                                         const Handle(Graphic3d_TransformPers)& theTrsfPers)
  {
    return connect (theAnotherObj, new Geom_Transformation (theLocation), theTrsfPers);
  }

  Standard_DEPRECATED("This method is deprecated - Connect() taking Graphic3d_TransformPers should be called instead")
  Handle(AIS_InteractiveObject) Connect (const Handle(AIS_InteractiveObject)& theInteractive,
                                         const gp_Trsf& theLocation,
                                         const Graphic3d_TransModeFlags& theTrsfPersFlag,
                                         const gp_Pnt& theTrsfPersPoint)
  {
    return connect (theInteractive, new Geom_Transformation (theLocation), Graphic3d_TransformPers::FromDeprecatedParams (theTrsfPersFlag, theTrsfPersPoint));
  }

protected:
  
  //! this method is redefined virtual;
  //! when the instance is connected to another
  //! InteractiveObject,this method doesn't
  //! compute anything, but just uses the
  //! presentation of this last object, with
  //! a transformation if there's one stored.
  Standard_EXPORT virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& aPresentationManager, const Handle(Prs3d_Presentation)& aPresentation, const Standard_Integer aMode = 0) Standard_OVERRIDE;

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its reference.
  //! Locates instance in theLocation and applies specified transformation persistence mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Standard_EXPORT virtual Handle(AIS_InteractiveObject) connect (const Handle(AIS_InteractiveObject)& theInteractive,
                                                                 const Handle(Geom_Transformation)& theLocation,
                                                                 const Handle(Graphic3d_TransformPers)& theTrsfPers);

private:
  
  //! Computes the selection for whole subtree in scene hierarchy.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) Standard_OVERRIDE;

};

DEFINE_STANDARD_HANDLE(AIS_MultipleConnectedInteractive, AIS_InteractiveObject)

#endif // _AIS_MultipleConnectedInteractive_HeaderFile
