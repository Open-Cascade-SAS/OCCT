// Created on: 1996-12-05
// Created by: Odile Olivier
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_DimensionOwner_HeaderFile
#define _AIS_DimensionOwner_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <AIS_DimensionSelectionMode.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Integer.hxx>
#include <PrsMgr_PresentationManager3d.hxx>
#include <Quantity_NameOfColor.hxx>
#include <Standard_Boolean.hxx>

class SelectMgr_SelectableObject;
class PrsMgr_PresentationManager;

DEFINE_STANDARD_HANDLE(AIS_DimensionOwner, SelectMgr_EntityOwner)

//! The owner is the entity which makes it possible to link
//! the sensitive primitives and the reference shapes that
//! you want to detect. It stocks the various pieces of
//! information which make it possible to find objects. An
//! owner has a priority which you can modulate, so as to
//! make one entity more selectable than another. You
//! might want to make edges more selectable than
//! faces, for example. In that case, you could attribute sa
//! higher priority to the one compared to the other. An
//! edge, could have priority 5, for example, and a face,
//! priority 4. The default priority is 5.
class AIS_DimensionOwner : public SelectMgr_EntityOwner
{
  DEFINE_STANDARD_RTTIEXT(AIS_DimensionOwner, SelectMgr_EntityOwner)
public:

  //! Initializes the dimension owner, theSO, and attributes it
  //! the priority, thePriority.
  Standard_EXPORT AIS_DimensionOwner(const Handle(SelectMgr_SelectableObject)& theSelObject, const AIS_DimensionSelectionMode theSelMode, const Standard_Integer thePriority = 0);
  
  Standard_EXPORT AIS_DimensionSelectionMode SelectionMode() const;
  
  Standard_EXPORT virtual void HilightWithColor (const Handle(PrsMgr_PresentationManager3d)& thePM,
                                                 const Handle(Prs3d_Drawer)& theStyle,
                                                 const Standard_Integer theMode = 0) Standard_OVERRIDE;
  
  //! Returns true if an object with the selection mode
  //! aMode is highlighted in the presentation manager aPM.
  Standard_EXPORT virtual Standard_Boolean IsHilighted (const Handle(PrsMgr_PresentationManager)& thePM, const Standard_Integer theMode = 0) const Standard_OVERRIDE;
  
  //! Removes highlighting from the selected part of dimension.
  Standard_EXPORT virtual void Unhilight (const Handle(PrsMgr_PresentationManager)& thePM, const Standard_Integer theMode = 0) Standard_OVERRIDE;

private:

  AIS_DimensionSelectionMode mySelectionMode;

};

#endif // _AIS_DimensionOwner_HeaderFile
