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
#include <IGESData_IGESEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_View.hxx>
#include <Interface_Macros.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_LabelDisplay,IGESData_LabelDisplayEntity)

IGESDraw_LabelDisplay::IGESDraw_LabelDisplay ()    {  }

    void IGESDraw_LabelDisplay::Init
  (const Handle(IGESDraw_HArray1OfViewKindEntity)& allViews,
   const Handle(TColgp_HArray1OfXYZ)&              allTextLocations,
   const Handle(IGESDimen_HArray1OfLeaderArrow)&   allLeaderEntities,
   const Handle(TColStd_HArray1OfInteger)&         allLabelLevels,
   const Handle(IGESData_HArray1OfIGESEntity)&     allDisplayedEntities)
{
  Standard_Integer Ln  = allViews->Length();
  if ( allViews->Lower()          != 1 ||
      (allTextLocations->Lower()  != 1 || allTextLocations->Length()  != Ln) ||
      (allLeaderEntities->Lower() != 1 || allLeaderEntities->Length() != Ln) ||
      (allLabelLevels->Lower()    != 1 || allLabelLevels->Length()    != Ln) ||
      (allDisplayedEntities->Lower() != 1 || allDisplayedEntities->Length() != Ln))
    throw Standard_DimensionMismatch("IGESDraw_LabelDisplay : Init");

  theViews             = allViews; 
  theTextLocations     = allTextLocations; 
  theLeaderEntities    = allLeaderEntities; 
  theLabelLevels       = allLabelLevels; 
  theDisplayedEntities = allDisplayedEntities; 
  InitTypeAndForm(402,5);
}

    Standard_Integer IGESDraw_LabelDisplay::NbLabels () const
{
  return (theViews->Length());
}

    Handle(IGESData_ViewKindEntity) IGESDraw_LabelDisplay::ViewItem
  (const Standard_Integer ViewIndex) const
{
  return (theViews->Value(ViewIndex));
}

    gp_Pnt IGESDraw_LabelDisplay::TextLocation
  (const Standard_Integer ViewIndex) const
{
  return ( gp_Pnt (theTextLocations->Value(ViewIndex)) );
}

    Handle(IGESDimen_LeaderArrow) IGESDraw_LabelDisplay::LeaderEntity
  (const Standard_Integer ViewIndex) const
{
  return (theLeaderEntities->Value(ViewIndex));
}

    Standard_Integer IGESDraw_LabelDisplay::LabelLevel
  (const Standard_Integer ViewIndex) const
{
  return (theLabelLevels->Value(ViewIndex));
}

    Handle(IGESData_IGESEntity) IGESDraw_LabelDisplay::DisplayedEntity
  (const Standard_Integer EntityIndex) const
{
  return (theDisplayedEntities->Value(EntityIndex));
}

    gp_Pnt IGESDraw_LabelDisplay::TransformedTextLocation
  (const Standard_Integer ViewIndex) const
{
  gp_XYZ retXYZ;
  gp_XYZ tempXYZ = theTextLocations->Value(ViewIndex);

  Handle(IGESData_ViewKindEntity) tempView = theViews->Value(ViewIndex);
  if (tempView->IsKind(STANDARD_TYPE(IGESDraw_View)))
    {
      DeclareAndCast(IGESDraw_View, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  else if (tempView->IsKind(STANDARD_TYPE(IGESDraw_PerspectiveView)))
    {
      DeclareAndCast(IGESDraw_PerspectiveView, thisView, tempView);
      retXYZ = thisView->ModelToView( tempXYZ );
    }
  return ( gp_Pnt(retXYZ) );
}
