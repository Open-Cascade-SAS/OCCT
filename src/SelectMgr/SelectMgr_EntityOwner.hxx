// Created on: 1995-05-23
// Created by: Robert COUBLANC
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

#ifndef _SelectMgr_EntityOwner_HeaderFile
#define _SelectMgr_EntityOwner_HeaderFile

#include <Graphic3d_ZLayerId.hxx>
#include <Prs3d_Drawer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Quantity_NameOfColor.hxx>

class V3d_Viewer;

//! A framework to define classes of owners of sensitive primitives.
//! The owner is the link between application and
//! selection data structures.
//! For the application to make its own objects selectable,
//! it must define owner classes inheriting this framework.
class SelectMgr_EntityOwner : public SelectBasics_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(SelectMgr_EntityOwner, SelectBasics_EntityOwner)
public:

  //! Initializes the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const Standard_Integer aPriority = 0);

  //! Constructs a framework with the selectable object
  //! anSO being attributed the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const Handle(SelectMgr_SelectableObject)& aSO, const Standard_Integer aPriority = 0);

  //! Constructs a framework from existing one
  //! anSO being attributed the selection priority aPriority.
  Standard_EXPORT SelectMgr_EntityOwner(const Handle(SelectMgr_EntityOwner)& theOwner, const Standard_Integer aPriority = 0);
  
  //! Returns true if there is a selectable object to serve as an owner.
  Standard_Boolean HasSelectable() const { return mySelectable != NULL; }

  //! Returns a selectable object detected in the working context.
  Standard_EXPORT virtual Handle(SelectMgr_SelectableObject) Selectable() const;

  //! Sets the selectable object.
  Standard_EXPORT virtual void SetSelectable (const Handle(SelectMgr_SelectableObject)& theSelObj);

  //! Returns true if the presentation manager aPM
  //! highlights selections corresponding to the selection mode aMode.
  Standard_EXPORT virtual Standard_Boolean IsHilighted (const Handle(PrsMgr_PresentationManager)& aPM, const Standard_Integer aMode = 0) const;
  
  //! Highlights selectable object's presentation with mode theMode in presentation manager
  //! with given highlight style. Also a check for auto-highlight is performed - if
  //! selectable object manages highlighting on its own, execution will be passed to
  //! SelectMgr_SelectableObject::HilightOwnerWithColor method
  Standard_EXPORT virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                 const Handle(Prs3d_Drawer)& theStyle,
                                                 const Standard_Integer theMode = 0);

  //! Removes highlighting from the owner of a detected selectable object in the presentation manager.
  //! This object could be the owner of a sensitive primitive.
  //! @param thePrsMgr presentation manager
  //! @param theMode   obsolete argument for compatibility, should be ignored by implementations
  Standard_EXPORT virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& thePrsMgr, const Standard_Integer theMode = 0);

  //! Clears the owners matching the value of the selection
  //! mode aMode from the presentation manager object aPM.
  Standard_EXPORT virtual void Clear (const Handle(PrsMgr_PresentationManager)& aPM, const Standard_Integer aMode = 0);
  
  Standard_EXPORT virtual Standard_Boolean HasLocation() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetLocation (const TopLoc_Location& aLoc) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ResetLocation() Standard_OVERRIDE;
  
  Standard_EXPORT virtual TopLoc_Location Location() const Standard_OVERRIDE;
  
  //! Set the state of the owner.
  //! @param theIsSelected [in] shows if owner is selected.
  void SetSelected (const Standard_Boolean theIsSelected) { myIsSelected = theIsSelected; }

  //! @return Standard_True if the owner is selected.
  Standard_Boolean IsSelected() const { return myIsSelected; }

  //! Set the state of the owner.
  //! The method is deprecated. Use SetSelected() instead.
  void State (const Standard_Integer theStatus) { myIsSelected = (theStatus == 1); }

  Standard_Integer State() const { return myIsSelected ? 1 : 0; }

  //! if owner is not auto hilighted, for group contains many such owners
  //! will be called one method HilightSelected of SelectableObject
  Standard_EXPORT virtual Standard_Boolean IsAutoHilight() const;
  
  //! if this method returns TRUE the owner will allways call method
  //! Hilight for SelectableObject when the owner is detected. By default
  //! it always return FALSE.
  Standard_EXPORT virtual Standard_Boolean IsForcedHilight() const;
  
  //! Set Z layer ID and update all presentations.
  Standard_EXPORT virtual void SetZLayer (const Graphic3d_ZLayerId theLayerId);

  //! Implements immediate application of location transformation of parent object to dynamic highlight structure
  Standard_EXPORT virtual void UpdateHighlightTrsf (const Handle(V3d_Viewer)& theViewer,
                                                    const Handle(PrsMgr_PresentationManager3d)& theManager,
                                                    const Standard_Integer theDispMode);

  //! Returns true if pointer to selectable object of this owner is equal to the given one
  Standard_Boolean IsSameSelectable (const Handle(SelectMgr_SelectableObject)& theOther) const
  {
    return mySelectable == theOther.get();
  }

  //! Returns TRUE if this owner points to a part of object and FALSE for entire object.
  Standard_Boolean ComesFromDecomposition() const { return myFromDecomposition; }

  //! Sets flag indicating this owner points to a part of object (TRUE) or to entire object (FALSE).
  void SetComesFromDecomposition (const Standard_Boolean theIsFromDecomposition) { myFromDecomposition = theIsFromDecomposition; }

public:

  //! Sets the selectable object.
  void Set (const Handle(SelectMgr_SelectableObject)& theSelObj) { SetSelectable (theSelObj); }

protected:

  SelectMgr_SelectableObject* mySelectable;        //!< raw pointer to selectable object
  Standard_Boolean            myIsSelected;        //!< flag indicating selected state
  Standard_Boolean            myFromDecomposition; //!< flag indicating this owner points to a part of object (TRUE) or to entire object (FALSE)

};

DEFINE_STANDARD_HANDLE(SelectMgr_EntityOwner, SelectBasics_EntityOwner)

#endif // _SelectMgr_EntityOwner_HeaderFile
