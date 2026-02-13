// Created on: 1998-11-20
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <ViewerTest.hxx>
#include <BRepTest.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>

#include <ChFi3d_FilletShape.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>

#ifdef _WIN32
  #include <stdio.h>
#endif

static double        tesp       = 1.e-4;
static double        t3d        = 1.e-4;
static double        t2d        = 1.e-5;
static double        ta         = 1.e-2;
static double        fl         = 1.e-3;
static double        tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;

static BRepFilletAPI_MakeFillet* Rakk = nullptr;

static void printtolblend(Draw_Interpretor& di)
{
  di << "tolerance ang : " << ta << "\n";
  di << "tolerance 3d  : " << t3d << "\n";
  di << "tolerance 2d  : " << t2d << "\n";
  di << "fleche        : " << fl << "\n";

  di << "tolblend " << ta << " " << t3d << " " << t2d << " " << fl << "\n";
}

static int VBLEND(Draw_Interpretor& di, int narg, const char** a)
{
  delete Rakk;
  Rakk = nullptr;
  printtolblend(di);
  if (narg < 5)
    return 1;

  int                                            NbToPick = (narg - 4) / 2;
  occ::handle<NCollection_HArray1<TopoDS_Shape>> arr =
    new NCollection_HArray1<TopoDS_Shape>(1, NbToPick);
  if (ViewerTest::PickShapes(TopAbs_EDGE, arr))
  {
    for (int i = 1; i <= NbToPick; i++)
    {
      TopoDS_Shape PickSh = arr->Value(i);
      if (!PickSh.IsNull())
      {
        DBRep::Set(a[(2 * i + 2)], PickSh);
      }
    }
  }

  TopoDS_Shape V = DBRep::Get(a[2]);
  if (V.IsNull())
    return 1;
  ChFi3d_FilletShape FSh = ChFi3d_Rational;
  if (narg % 2 == 0)
  {
    if (!strcasecmp(a[narg - 1], "Q"))
    {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[narg - 1], "P"))
    {
      FSh = ChFi3d_Polynomial;
    }
  }
  Rakk = new BRepFilletAPI_MakeFillet(V, FSh);
  Rakk->SetParams(ta, tesp, t2d, t3d, t2d, fl);
  Rakk->SetContinuity(blend_cont, tapp_angle);
  double      Rad;
  TopoDS_Edge E;
  int         nbedge = 0;
  for (int ii = 1; ii < (narg - 1) / 2; ii++)
  {
    Rad                      = Draw::Atof(a[2 * ii + 1]);
    TopoDS_Shape aLocalShape = DBRep::Get(a[(2 * ii + 2)], TopAbs_EDGE);
    E                        = TopoDS::Edge(aLocalShape);
    //    E = TopoDS::Edge(DBRep::Get(a[(2*ii+2)],TopAbs_EDGE));
    if (!E.IsNull())
    {
      Rakk->Add(Rad, E);
      nbedge++;
    }
  }
  if (!nbedge)
    return 1;
  Rakk->Build();
  if (!Rakk->IsDone())
    return 1;
  TopoDS_Shape res = Rakk->Shape();
  DBRep::Set(a[1], res);

  // visu resultat...
  ViewerTest::Display(a[2], occ::handle<AIS_InteractiveObject>(), false);
  ViewerTest::Display(a[1], new AIS_Shape(res), true);
  return 0;
}

void ViewerTest::FilletCommands(Draw_Interpretor& theCommands)
{
  const char* g = "Viewer Fillet construction commands";
  theCommands.Add("vblend",
                  "vblend result object rad1 ed1 rad2 ed2 ... [R/Q/P]",
                  __FILE__,
                  VBLEND,
                  g);
}
