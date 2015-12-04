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
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_XY.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_RadiusDimension.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_RadiusDimension,IGESData_IGESEntity)

IGESDimen_RadiusDimension::IGESDimen_RadiusDimension ()    {  }


    void IGESDimen_RadiusDimension::Init
  (const Handle(IGESDimen_GeneralNote)& aNote,
   const Handle(IGESDimen_LeaderArrow)& anArrow,
   const gp_XY& arcCenter,
   const Handle(IGESDimen_LeaderArrow)& anotherArrow)
{
  theNote        = aNote;
  theLeaderArrow = anArrow;
  theCenter      = arcCenter;
  theLeader2     = anotherArrow;
  if (!anotherArrow.IsNull()) InitTypeAndForm(222, 1);  // 1 admet aussi Null
  else InitTypeAndForm(222,FormNumber());
}

    void IGESDimen_RadiusDimension::InitForm (const Standard_Integer form)
{
  InitTypeAndForm(222,form);
}

    Handle(IGESDimen_GeneralNote) IGESDimen_RadiusDimension::Note () const
{
  return theNote;
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader () const
{
  return theLeaderArrow;
}

    Standard_Boolean IGESDimen_RadiusDimension::HasLeader2 () const
{
  return (!theLeader2.IsNull());
}

    gp_Pnt2d IGESDimen_RadiusDimension::Center () const
{
  gp_Pnt2d g(theCenter);
  return g;
}

    gp_Pnt IGESDimen_RadiusDimension::TransformedCenter () const
{
  gp_XYZ tmpXYZ(theCenter.X(), theCenter.Y(), theLeaderArrow->ZDepth());
  if (HasTransf()) Location().Transforms(tmpXYZ);
  return gp_Pnt(tmpXYZ);
}

    Handle(IGESDimen_LeaderArrow) IGESDimen_RadiusDimension::Leader2 () const
{
  return theLeader2;
}
