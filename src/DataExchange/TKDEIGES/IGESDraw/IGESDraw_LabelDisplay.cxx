// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <gp_Pnt.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <MoniTool_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_LabelDisplay, IGESData_LabelDisplayEntity)

IGESDraw_LabelDisplay::IGESDraw_LabelDisplay() = default;

void IGESDraw_LabelDisplay::Init(
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_ViewKindEntity>>>& allViews,
  const occ::handle<NCollection_HArray1<gp_XYZ>>&                               allTextLocations,
  const occ::handle<NCollection_HArray1<occ::handle<IGESDimen_LeaderArrow>>>&   allLeaderEntities,
  const occ::handle<NCollection_HArray1<int>>&                                  allLabelLevels,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allDisplayedEntities)
{
  int Ln = allViews->Length();
  if (allViews->Lower() != 1 || (allTextLocations->Lower() != 1 || allTextLocations->Length() != Ln)
      || (allLeaderEntities->Lower() != 1 || allLeaderEntities->Length() != Ln)
      || (allLabelLevels->Lower() != 1 || allLabelLevels->Length() != Ln)
      || (allDisplayedEntities->Lower() != 1 || allDisplayedEntities->Length() != Ln))
    throw Standard_DimensionMismatch("IGESDraw_LabelDisplay : Init");

  theViews             = allViews;
  theTextLocations     = allTextLocations;
  theLeaderEntities    = allLeaderEntities;
  theLabelLevels       = allLabelLevels;
  theDisplayedEntities = allDisplayedEntities;
  InitTypeAndForm(402, 5);
}

int IGESDraw_LabelDisplay::NbLabels() const
{
  return (theViews->Length());
}

occ::handle<IGESData_ViewKindEntity> IGESDraw_LabelDisplay::ViewItem(const int ViewIndex) const
{
  return (theViews->Value(ViewIndex));
}

gp_Pnt IGESDraw_LabelDisplay::TextLocation(const int ViewIndex) const
{
  return (gp_Pnt(theTextLocations->Value(ViewIndex)));
}

occ::handle<IGESDimen_LeaderArrow> IGESDraw_LabelDisplay::LeaderEntity(const int ViewIndex) const
{
  return (theLeaderEntities->Value(ViewIndex));
}

int IGESDraw_LabelDisplay::LabelLevel(const int ViewIndex) const
{
  return (theLabelLevels->Value(ViewIndex));
}

occ::handle<IGESData_IGESEntity> IGESDraw_LabelDisplay::DisplayedEntity(const int EntityIndex) const
{
  return (theDisplayedEntities->Value(EntityIndex));
}

gp_Pnt IGESDraw_LabelDisplay::TransformedTextLocation(const int ViewIndex) const
{
  gp_XYZ retXYZ;
  gp_XYZ tempXYZ = theTextLocations->Value(ViewIndex);

  occ::handle<IGESData_ViewKindEntity> tempView = theViews->Value(ViewIndex);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
  {
    DeclareAndCast(IGESDraw_View, thisView, tempView);
    retXYZ = thisView->ModelToView(tempXYZ);
  }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
  {
    DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
    retXYZ = thisView->ModelToView(tempXYZ);
  }
  return (gp_Pnt(retXYZ));
}
