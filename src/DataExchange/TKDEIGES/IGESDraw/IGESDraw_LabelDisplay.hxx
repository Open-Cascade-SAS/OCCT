// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( TCD )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESDraw_LabelDisplay_HeaderFile
#define _IGESDraw_LabelDisplay_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_ViewKindEntity.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XYZ.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <Standard_Integer.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
class IGESData_ViewKindEntity;
class gp_Pnt;
class IGESDimen_LeaderArrow;
class IGESData_IGESEntity;

//! defines IGESLabelDisplay, Type <402> Form <5>
//! in package IGESDraw
//!
//! Permits one or more displays for the
//! entity labels of an entity
class IGESDraw_LabelDisplay : public IGESData_LabelDisplayEntity
{

public:
  Standard_EXPORT IGESDraw_LabelDisplay();

  //! This method is used to set the fields of the class
  //! LabelDisplay
  //! - allViews             : Pointers to View Entities
  //! - allTextLocations     : Coordinates of text locations in the views
  //! - allLeaderEntities    : Pointers to Leader Entities in the views
  //! - allLabelLevels       : Entity label level numbers in the views
  //! - allDisplayedEntities : Pointers to the entities being displayed
  //! raises exception if Lengths of allViews, allTextLocations,
  //! allLeaderEntities, allLabelLevels and allDisplayedEntities are
  //! not same.
  Standard_EXPORT void Init(
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViews,
    const occ::handle<NCollection_HArray1<gp_XYZ>>&                               allTextLocations,
    const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>&   allLeaderEntities,
    const occ::handle<NCollection_HArray1<int>>&                                  allLabelLevels,
    const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allDisplayedEntities);

  //! returns the number of label placements in <me>
  Standard_EXPORT int NbLabels() const;

  //! returns the View entity indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT occ::handle<IGESData_ViewKindEntity> ViewItem(const int ViewIndex) const;

  //! returns the 3d-Point coordinates of the text location, in the
  //! view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT gp_Pnt TextLocation(const int ViewIndex) const;

  //! returns the Leader entity in the view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT occ::handle<IGESDimen_LeaderArrow> LeaderEntity(const int ViewIndex) const;

  //! returns the Entity label level number in the view indicated
  //! by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT int LabelLevel(const int ViewIndex) const;

  //! returns the entity indicated by EntityIndex
  //! raises an exception if EntityIndex <= 0 or EntityIndex > NbLabels().
  Standard_EXPORT occ::handle<IGESData_IGESEntity> DisplayedEntity(const int EntityIndex) const;

  //! returns the transformed 3d-Point coordinates of the text
  //! location, in the view indicated by ViewIndex
  //! raises an exception if ViewIndex <= 0 or ViewIndex > NbLabels().
  Standard_EXPORT gp_Pnt TransformedTextLocation(const int ViewIndex) const;

  DEFINE_STANDARD_RTTIEXT(IGESDraw_LabelDisplay, IGESData_LabelDisplayEntity)

private:
  occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>> theViews;
  occ::handle<NCollection_HArray1<gp_XYZ>>                               theTextLocations;
  occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>   theLeaderEntities;
  occ::handle<NCollection_HArray1<int>>                                  theLabelLevels;
  occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>     theDisplayedEntities;
};

#endif // _IGESDraw_LabelDisplay_HeaderFile
