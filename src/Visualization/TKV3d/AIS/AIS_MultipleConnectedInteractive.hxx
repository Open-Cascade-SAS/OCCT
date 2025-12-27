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

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its
  //! reference. Locates instance in theLocation and applies specified transformation persistence
  //! mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  occ::handle<AIS_InteractiveObject> Connect(const occ::handle<AIS_InteractiveObject>&   theAnotherObj,
                                        const occ::handle<TopLoc_Datum3D>&          theLocation,
                                        const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
  {
    return connect(theAnotherObj, theLocation, theTrsfPers);
  }

  virtual AIS_KindOfInteractive Type() const override
  {
    return AIS_KindOfInteractive_Object;
  }

  virtual int Signature() const override { return 1; }

  //! Returns true if the object is connected to others.
  Standard_EXPORT bool HasConnection() const;

  //! Removes the connection with theInteractive.
  Standard_EXPORT void Disconnect(const occ::handle<AIS_InteractiveObject>& theInteractive);

  //! Clears all the connections to objects.
  Standard_EXPORT void DisconnectAll();

  //! Informs the graphic context that the interactive Object
  //! may be decomposed into sub-shapes for dynamic selection.
  Standard_EXPORT virtual bool AcceptShapeDecomposition() const override;

  //! Returns common entity owner if the object is an assembly
  virtual const occ::handle<SelectMgr_EntityOwner>& GetAssemblyOwner() const override
  {
    return myAssemblyOwner;
  }

  //! Returns the owner of mode for selection of object as a whole
  virtual occ::handle<SelectMgr_EntityOwner> GlobalSelOwner() const override
  {
    return myAssemblyOwner;
  }

  //! Assigns interactive context.
  Standard_EXPORT virtual void SetContext(const occ::handle<AIS_InteractiveContext>& theCtx)
    override;

public: // short aliases to Connect() method
  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its
  //! reference. Copies local transformation and transformation persistence mode from
  //! theInteractive.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  occ::handle<AIS_InteractiveObject> Connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj)
  {
    return connect(theAnotherObj,
                   theAnotherObj->LocalTransformationGeom(),
                   theAnotherObj->TransformPersistence());
  }

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its
  //! reference. Locates instance in theLocation and copies transformation persistence mode from
  //! theInteractive.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  occ::handle<AIS_InteractiveObject> Connect(const occ::handle<AIS_InteractiveObject>& theAnotherObj,
                                        const gp_Trsf&                       theLocation)
  {
    return connect(theAnotherObj,
                   new TopLoc_Datum3D(theLocation),
                   theAnotherObj->TransformPersistence());
  }

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its
  //! reference. Locates instance in theLocation and applies specified transformation persistence
  //! mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  occ::handle<AIS_InteractiveObject> Connect(const occ::handle<AIS_InteractiveObject>&   theAnotherObj,
                                        const gp_Trsf&                         theLocation,
                                        const occ::handle<Graphic3d_TransformPers>& theTrsfPers)
  {
    return connect(theAnotherObj, new TopLoc_Datum3D(theLocation), theTrsfPers);
  }

protected:
  //! this method is redefined virtual;
  //! when the instance is connected to another
  //! InteractiveObject,this method doesn't
  //! compute anything, but just uses the
  //! presentation of this last object, with
  //! a transformation if there's one stored.
  Standard_EXPORT virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                                       const occ::handle<Prs3d_Presentation>&         thePrs,
                                       const int theMode) override;

  //! Establishes the connection between the Connected Interactive Object, theInteractive, and its
  //! reference. Locates instance in theLocation and applies specified transformation persistence
  //! mode.
  //! @return created instance object (AIS_ConnectedInteractive or AIS_MultipleConnectedInteractive)
  Standard_EXPORT virtual occ::handle<AIS_InteractiveObject> connect(
    const occ::handle<AIS_InteractiveObject>&   theInteractive,
    const occ::handle<TopLoc_Datum3D>&          theLocation,
    const occ::handle<Graphic3d_TransformPers>& theTrsfPers);

private:
  //! Computes the selection for whole subtree in scene hierarchy.
  Standard_EXPORT virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& aSelection,
                                                const int aMode) override;

protected:
  occ::handle<SelectMgr_EntityOwner> myAssemblyOwner;
};

#endif // _AIS_MultipleConnectedInteractive_HeaderFile
