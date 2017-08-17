// Created on: 1995-02-20
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

#ifndef _SelectMgr_SelectableObject_HeaderFile
#define _SelectMgr_SelectableObject_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <SelectMgr_SequenceOfSelection.hxx>
#include <Prs3d_Drawer.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_TypeOfPresentation3d.hxx>
#include <SelectMgr_Selection.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectMgr_SequenceOfOwner.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Graphic3d_ZLayerId.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>
class SelectMgr_EntityOwner;
class Prs3d_Presentation;
class Standard_NotImplemented;
class SelectMgr_SelectionManager;
class Bnd_Box;


class SelectMgr_SelectableObject;
DEFINE_STANDARD_HANDLE(SelectMgr_SelectableObject, PrsMgr_PresentableObject)

//! A framework to supply the structure of the object to be
//! selected. At the first pick, this structure is created by
//! calling the appropriate algorithm and retaining this
//! framework for further picking.
//! This abstract framework is inherited in Application
//! Interactive Services (AIS), notably in AIS_InteractiveObject.
//! Consequently, 3D selection should be handled by the
//! relevant daughter classes and their member functions
//! in AIS. This is particularly true in the creation of new interactive objects.
class SelectMgr_SelectableObject : public PrsMgr_PresentableObject
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_SelectableObject, PrsMgr_PresentableObject)
  friend class SelectMgr_SelectionManager;
public:

  //! Clears all selections of the object
  Standard_EXPORT virtual ~SelectMgr_SelectableObject();
  
  //! Recovers and calculates any sensitive primitive,
  //! aSelection, available in Shape mode, specified by
  //! aMode. As a rule, these are sensitive faces.
  //! This method is defined as virtual. This enables you to
  //! implement it in the creation of a new class of AIS
  //! Interactive Object. You need to do this and in so
  //! doing, redefine this method, if you create a class
  //! which enriches the list of signatures and types.
  Standard_EXPORT virtual void ComputeSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode) = 0;
  
  //! Re-computes the sensitive primitives for all modes. IMPORTANT: Do not use
  //! this method to update selection primitives except implementing custom selection manager!
  //! This method does not take into account necessary BVH updates, but may invalidate the pointers
  //! it refers to. TO UPDATE SELECTION properly from outside classes, use method UpdateSelection.
  Standard_EXPORT void RecomputePrimitives();
  
  //! Re-computes the sensitive primitives which correspond to the <theMode>th selection mode.
  //! IMPORTANT: Do not use this method to update selection primitives except implementing custom selection manager!
  //! selection manager! This method does not take into account necessary BVH updates, but may invalidate
  //! the pointers it refers to. TO UPDATE SELECTION properly from outside classes, use method UpdateSelection.
  Standard_EXPORT void RecomputePrimitives (const Standard_Integer theMode);
  
  //! Adds the selection aSelection with the selection mode
  //! index aMode to this framework.
  Standard_EXPORT void AddSelection (const Handle(SelectMgr_Selection)& aSelection, const Standard_Integer aMode);
  
  //! Empties all the selections in the SelectableObject
  //! <update> parameter defines whether all object's
  //! selections should be flagged for further update or not.
  //! This improved method can be used to recompute an
  //! object's selection (without redisplaying the object
  //! completely) when some selection mode is activated not for the first time.
  Standard_EXPORT void ClearSelections (const Standard_Boolean update = Standard_False);
  
  //! Returns the selection Selection having the selection mode aMode.
  Standard_EXPORT const Handle(SelectMgr_Selection)& Selection (const Standard_Integer aMode) const;
  
  //! Returns true if a selection corresponding to the
  //! selection mode theMode was computed for this object.
  Standard_EXPORT virtual Standard_Boolean HasSelection (const Standard_Integer theMode) const;
  
  //! Begins the iteration scanning for sensitive primitives.
  void Init()
  {
    mycurrent = 1;
  }

  //! Continues the iteration scanning for sensitive primitives.
  Standard_Boolean More() const
  {
    return mycurrent <= myselections.Length();
  }

  //! Continues the iteration scanning for sensitive primitives.
  void Next()
  {
    mycurrent++;
  }

  //! Returns the current selection in this framework.
  const Handle(SelectMgr_Selection)& CurrentSelection() const
  {
    return myselections (mycurrent);
  }

  Standard_EXPORT void ResetTransformation() Standard_OVERRIDE;
  
  //! Recomputes the location of the selection aSelection.
  Standard_EXPORT virtual void UpdateTransformation() Standard_OVERRIDE;
  
  //! Updates locations in all sensitive entities from <aSelection>
  //! and in corresponding entity owners.
  Standard_EXPORT virtual void UpdateTransformations (const Handle(SelectMgr_Selection)& aSelection);
  
  //! Method which draws selected owners ( for fast presentation draw )
  Standard_EXPORT virtual void HilightSelected (const Handle(PrsMgr_PresentationManager3d)& PM, const SelectMgr_SequenceOfOwner& Seq);
  
  //! Method which clear all selected owners belonging
  //! to this selectable object ( for fast presentation draw )
  Standard_EXPORT virtual void ClearSelected();

  //! Method that needs to be implemented when the object
  //! manages selection and dynamic highlighting on its own.
  //! Clears or invalidates dynamic highlight presentation.
  //! By default it clears immediate draw of given presentation
  //! manager.
  Standard_EXPORT virtual void ClearDynamicHighlight (const Handle(PrsMgr_PresentationManager3d)& theMgr);

  //! Method which hilight an owner belonging to
  //! this selectable object  ( for fast presentation draw )
  Standard_EXPORT virtual void HilightOwnerWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                      const Handle(Prs3d_Drawer)& theStyle,
                                                      const Handle(SelectMgr_EntityOwner)& theOwner);
  
  //! If returns True, the old mechanism for highlighting
  //! selected objects is used (HilightSelected Method may be empty).
  //! If returns False, the HilightSelected method will be
  //! fully responsible for highlighting selected entity
  //! owners belonging to this selectable object.
  Standard_EXPORT virtual Standard_Boolean IsAutoHilight() const;
  
  //! Set AutoHilight property to true or false
  //! Sets  up  Transform  Persistence Mode  for  this  object
  Standard_EXPORT virtual void SetAutoHilight (const Standard_Boolean newAutoHilight);
  
  Standard_EXPORT Handle(Prs3d_Presentation) GetHilightPresentation (const Handle(PrsMgr_PresentationManager3d)& TheMgr);
  
  Standard_EXPORT Handle(Prs3d_Presentation) GetSelectPresentation (const Handle(PrsMgr_PresentationManager3d)& TheMgr);

  //! Removes presentations returned by GetHilightPresentation() and GetSelectPresentation().
  Standard_EXPORT virtual void ErasePresentations (Standard_Boolean theToRemove);

  //! Set Z layer ID and update all presentations of the selectable object.
  //! The layers mechanism allows drawing objects in higher layers in overlay of objects in lower layers.
  Standard_EXPORT virtual void SetZLayer (const Graphic3d_ZLayerId theLayerId) Standard_OVERRIDE;
  
  //! Sets update status FULL to selections of the object. Must be used as the only method of UpdateSelection
  //! from outer classes to prevent BVH structures from being outdated.
  void UpdateSelection (const Standard_Integer theMode = -1)
  {
    updateSelection (theMode);
  }

  //! Returns bounding box of selectable object
  //! by storing its minimum and maximum 3d coordinates
  //! to output parameters
  Standard_EXPORT virtual void BoundingBox (Bnd_Box& theBndBox) = 0;

  //! Sets common entity owner for assembly sensitive object entities
  Standard_EXPORT void SetAssemblyOwner (const Handle(SelectMgr_EntityOwner)& theOwner, const Standard_Integer theMode = -1);
  
  //! Returns common entity owner if the object is an assembly
  Standard_EXPORT const Handle(SelectMgr_EntityOwner)& GetAssemblyOwner() const;
  
  //! Returns a bounding box of sensitive entities with the owners given
  //! if they are a part of activated selection
  Standard_EXPORT Bnd_Box BndBoxOfSelected (const Handle(SelectMgr_IndexedMapOfOwner)& theOwners);

  //! Returns the mode for selection of object as a whole
  Standard_Integer GlobalSelectionMode() const
  {
    return myGlobalSelMode;
  }

  //! Returns the owner of mode for selection of object as a whole
  Standard_EXPORT virtual Handle(SelectMgr_EntityOwner) GlobalSelOwner() const;

protected:

  Standard_EXPORT SelectMgr_SelectableObject(const PrsMgr_TypeOfPresentation3d aTypeOfPresentation3d = PrsMgr_TOP_AllView);

  void setGlobalSelMode (const Standard_Integer theMode)
  {
    myGlobalSelMode = theMode > 0 ? theMode : 0;
  }

  Standard_EXPORT virtual void updateSelection (const Standard_Integer theMode);

protected:

  SelectMgr_SequenceOfSelection myselections;
  Handle(SelectMgr_EntityOwner) myAssemblyOwner;
  Handle(Prs3d_Presentation) mySelectionPrs;
  Handle(Prs3d_Presentation) myHilightPrs;
  Standard_Boolean myAutoHilight;

private:

  Standard_Integer mycurrent;
  Standard_Integer myGlobalSelMode;

};

#endif // _SelectMgr_SelectableObject_HeaderFile
