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

#include <gp_GTrsf.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_TransfEntity.hxx>
#include <IGESDraw_View.hxx>
#include <IGESGeom_Plane.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_View, IGESData_ViewKindEntity)

IGESDraw_View::IGESDraw_View() {}

// This class inherits from IGESData_ViewKindEntity

void IGESDraw_View::Init(const int                          aViewNum,
                         const double                       aScale,
                         const occ::handle<IGESGeom_Plane>& aLeftPlane,
                         const occ::handle<IGESGeom_Plane>& aTopPlane,
                         const occ::handle<IGESGeom_Plane>& aRightPlane,
                         const occ::handle<IGESGeom_Plane>& aBottomPlane,
                         const occ::handle<IGESGeom_Plane>& aBackPlane,
                         const occ::handle<IGESGeom_Plane>& aFrontPlane)
{
  theViewNumber  = aViewNum;
  theScaleFactor = aScale;
  theLeftPlane   = aLeftPlane;
  theTopPlane    = aTopPlane;
  theRightPlane  = aRightPlane;
  theBottomPlane = aBottomPlane;
  theBackPlane   = aBackPlane;
  theFrontPlane  = aFrontPlane;
  InitTypeAndForm(410, 0);
}

bool IGESDraw_View::IsSingle() const
{
  return true;
  // Redefined to return TRUE
}

int IGESDraw_View::NbViews() const
{
  return 1;
}

occ::handle<IGESData_ViewKindEntity> IGESDraw_View::ViewItem(const int) const
{
  return occ::down_cast<IGESData_ViewKindEntity>(This());
}

int IGESDraw_View::ViewNumber() const
{
  return theViewNumber;
}

double IGESDraw_View::ScaleFactor() const
{
  return theScaleFactor;
}

bool IGESDraw_View::HasLeftPlane() const
{
  return (!theLeftPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::LeftPlane() const
{
  return theLeftPlane;
}

bool IGESDraw_View::HasTopPlane() const
{
  return (!theTopPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::TopPlane() const
{
  return theTopPlane;
}

bool IGESDraw_View::HasRightPlane() const
{
  return (!theRightPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::RightPlane() const
{
  return theRightPlane;
}

bool IGESDraw_View::HasBottomPlane() const
{
  return (!theBottomPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::BottomPlane() const
{
  return theBottomPlane;
}

bool IGESDraw_View::HasBackPlane() const
{
  return (!theBackPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::BackPlane() const
{
  return theBackPlane;
}

bool IGESDraw_View::HasFrontPlane() const
{
  return (!theFrontPlane.IsNull());
}

occ::handle<IGESGeom_Plane> IGESDraw_View::FrontPlane() const
{
  return theFrontPlane;
}

occ::handle<IGESData_TransfEntity> IGESDraw_View::ViewMatrix() const
{
  return (Transf());
}

gp_XYZ IGESDraw_View::ModelToView(const gp_XYZ& coords) const
{
  gp_XYZ tempCoords = coords;
  Location().Transforms(tempCoords);
  return (tempCoords);
}
