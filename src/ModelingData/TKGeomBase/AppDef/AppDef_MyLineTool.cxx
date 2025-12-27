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

// AppDef_MyLineTool.cxx

#include <AppDef_MultiLine.hxx>
#include <AppDef_MultiPointConstraint.hxx>
#include <AppDef_MyLineTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

int AppDef_MyLineTool::FirstPoint(const AppDef_MultiLine&)
{
  return 1;
}

int AppDef_MyLineTool::LastPoint(const AppDef_MultiLine& ML)
{
  return ML.NbMultiPoints();
}

int AppDef_MyLineTool::NbP2d(const AppDef_MultiLine& ML)
{
  return ML.Value(1).NbPoints2d();
}

int AppDef_MyLineTool::NbP3d(const AppDef_MultiLine& ML)
{
  return ML.Value(1).NbPoints();
}

void AppDef_MyLineTool::Value(const AppDef_MultiLine& ML,
                              const int  MPointIndex,
                              NCollection_Array1<gp_Pnt>&     tabPt)
{
  AppDef_MultiPointConstraint MPC   = ML.Value(MPointIndex);
  int            nbp3d = MPC.NbPoints(), low = tabPt.Lower();
  for (int i = 1; i <= nbp3d; i++)
  {
    tabPt(i + low - 1) = MPC.Point(i);
  }
}

void AppDef_MyLineTool::Value(const AppDef_MultiLine& ML,
                              const int  MPointIndex,
                              NCollection_Array1<gp_Pnt2d>&   tabPt2d)
{
  AppDef_MultiPointConstraint MPC   = ML.Value(MPointIndex);
  int            nbp3d = MPC.NbPoints();
  int            nbp2d = MPC.NbPoints2d();
  int            low   = tabPt2d.Lower();
  for (int i = 1; i <= nbp2d; i++)
  {
    tabPt2d(i + low - 1) = MPC.Point2d(nbp3d + i);
  }
}

void AppDef_MyLineTool::Value(const AppDef_MultiLine& ML,
                              const int  MPointIndex,
                              NCollection_Array1<gp_Pnt>&     tabPt,
                              NCollection_Array1<gp_Pnt2d>&   tabPt2d)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  int            i, nbp2d = MPC.NbPoints2d(), low2d = tabPt2d.Lower();
  int            nbp3d = MPC.NbPoints(), low = tabPt.Lower();
  for (i = 1; i <= nbp3d; i++)
  {
    tabPt(i + low - 1) = MPC.Point(i);
  }
  for (i = 1; i <= nbp2d; i++)
  {
    tabPt2d(i + low2d - 1) = MPC.Point2d(nbp3d + i);
  }
}

bool AppDef_MyLineTool::Tangency(const AppDef_MultiLine& ML,
                                             const int  MPointIndex,
                                             NCollection_Array1<gp_Vec>&     tabV)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsTangencyPoint())
  {
    int nbp3d = MPC.NbPoints(), low = tabV.Lower();
    for (int i = 1; i <= nbp3d; i++)
    {
      tabV(i + low - 1) = MPC.Tang(i);
    }
    return true;
  }
  else
    return false;
}

bool AppDef_MyLineTool::Tangency(const AppDef_MultiLine& ML,
                                             const int  MPointIndex,
                                             NCollection_Array1<gp_Vec2d>&   tabV2d)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsTangencyPoint())
  {
    int nbp3d = MPC.NbPoints();
    int nbp2d = MPC.NbPoints2d(), low = tabV2d.Lower();
    for (int i = 1; i <= nbp2d; i++)
    {
      tabV2d(i + low - 1) = MPC.Tang2d(nbp3d + i);
    }
    return true;
  }
  else
    return false;
}

bool AppDef_MyLineTool::Tangency(const AppDef_MultiLine& ML,
                                             const int  MPointIndex,
                                             NCollection_Array1<gp_Vec>&     tabV,
                                             NCollection_Array1<gp_Vec2d>&   tabV2d)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsTangencyPoint())
  {
    int i, nbp3d = MPC.NbPoints(), low = tabV.Lower();
    int nbp2d = MPC.NbPoints2d(), low2d = tabV2d.Lower();
    for (i = 1; i <= nbp3d; i++)
    {
      tabV(i + low - 1) = MPC.Tang(i);
    }
    for (i = 1; i <= nbp2d; i++)
    {
      tabV2d(i + low2d - 1) = MPC.Tang2d(nbp3d + i);
    }
    return true;
  }
  else
    return false;
}

AppDef_MultiLine AppDef_MyLineTool::MakeMLBetween(const AppDef_MultiLine& theML,
                                                  const int,
                                                  const int,
                                                  const int)
{
  return theML; // stub
}

bool AppDef_MyLineTool::MakeMLOneMorePoint(const AppDef_MultiLine&,
                                                       const int,
                                                       const int,
                                                       const int,
                                                       AppDef_MultiLine&)
{
  return false;
}

Approx_Status AppDef_MyLineTool::WhatStatus(const AppDef_MultiLine&,
                                            const int,
                                            const int)
{
  return Approx_NoPointsAdded;
}

bool AppDef_MyLineTool::Curvature(const AppDef_MultiLine& ML,
                                              const int  MPointIndex,
                                              NCollection_Array1<gp_Vec>&     tabV)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsCurvaturePoint())
  {
    int nbp3d = MPC.NbPoints(), low = tabV.Lower();
    for (int i = 1; i <= nbp3d; i++)
    {
      tabV(i + low - 1) = MPC.Curv(i);
    }
    return true;
  }
  else
    return false;
}

bool AppDef_MyLineTool::Curvature(const AppDef_MultiLine& ML,
                                              const int  MPointIndex,
                                              NCollection_Array1<gp_Vec2d>&   tabV2d)
{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsCurvaturePoint())
  {
    int nbp3d = MPC.NbPoints();
    int nbp2d = MPC.NbPoints2d(), low = tabV2d.Lower();
    for (int i = 1; i <= nbp2d; i++)
    {
      tabV2d(i + low - 1) = MPC.Curv2d(nbp3d + i);
    }
    return true;
  }
  else
    return false;
}

bool AppDef_MyLineTool::Curvature(const AppDef_MultiLine& ML,
                                              const int  MPointIndex,
                                              NCollection_Array1<gp_Vec>&     tabV,
                                              NCollection_Array1<gp_Vec2d>&   tabV2d)

{
  AppDef_MultiPointConstraint MPC = ML.Value(MPointIndex);
  if (MPC.IsCurvaturePoint())
  {
    int i, nbp3d = MPC.NbPoints(), low = tabV.Lower();
    int nbp2d = MPC.NbPoints2d(), low2d = tabV2d.Lower();
    for (i = 1; i <= nbp3d; i++)
    {
      tabV(i + low - 1) = MPC.Curv(i);
    }
    for (i = 1; i <= nbp2d; i++)
    {
      tabV2d(i + low2d - 1) = MPC.Curv2d(nbp3d + i);
    }
    return true;
  }
  else
    return false;
}
