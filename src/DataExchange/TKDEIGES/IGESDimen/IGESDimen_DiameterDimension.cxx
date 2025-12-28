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
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <IGESDimen_DiameterDimension.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_DiameterDimension, IGESData_IGESEntity)

IGESDimen_DiameterDimension::IGESDimen_DiameterDimension() = default;

void IGESDimen_DiameterDimension::Init(const occ::handle<IGESDimen_GeneralNote>& aNote,
                                       const occ::handle<IGESDimen_LeaderArrow>& aLeader,
                                       const occ::handle<IGESDimen_LeaderArrow>& anotherLeader,
                                       const gp_XY&                              aCenter)
{
  theNote         = aNote;
  theFirstLeader  = aLeader;
  theSecondLeader = anotherLeader;
  theCenter       = aCenter;
  InitTypeAndForm(206, 0);
}

occ::handle<IGESDimen_GeneralNote> IGESDimen_DiameterDimension::Note() const
{
  return theNote;
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_DiameterDimension::FirstLeader() const
{
  return theFirstLeader;
}

bool IGESDimen_DiameterDimension::HasSecondLeader() const
{
  return (!theSecondLeader.IsNull());
}

occ::handle<IGESDimen_LeaderArrow> IGESDimen_DiameterDimension::SecondLeader() const
{
  return theSecondLeader;
}

gp_Pnt2d IGESDimen_DiameterDimension::Center() const
{
  gp_Pnt2d center(theCenter);
  return center;
}

gp_Pnt2d IGESDimen_DiameterDimension::TransformedCenter() const
{
  gp_XYZ center(theCenter.X(), theCenter.Y(), 0);
  if (HasTransf())
    Location().Transforms(center);
  return gp_Pnt2d(center.X(), center.Y());
}
