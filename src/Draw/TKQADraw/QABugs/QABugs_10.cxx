// Created on: 2002-05-28
// Created by: QA Admin
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Standard_ErrorHandler.hxx>

#include <Geom_SurfaceOfRevolution.hxx>

//=================================================================================================

static int isPeriodic(Draw_Interpretor& di, int argc, const char** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    // 1. Verify amount of arguments of the command
    if (argc < 2)
    {
      di << "isperiodic FAULTY. Use : isperiodic surfaceOfRevolution";
      return 0;
    }
    // 2. Retrieve surface
    occ::handle<Geom_Surface> aSurf = DrawTrSurf::GetSurface(argv[1]);
    if (aSurf.IsNull())
    {
      di << "isperiodic FAULTY. argument of command is not a surface";
      return 0;
    }
    occ::handle<Geom_SurfaceOfRevolution> aRevolSurf =
      occ::down_cast<Geom_SurfaceOfRevolution>(aSurf);
    if (aRevolSurf.IsNull())
    {
      di << "isperiodic FAULTY. argument of command is not a surface of revolution";
      return 0;
    }
    // 3. Verify whether entry surface is u-periodic and v-periodic
    if (aRevolSurf->IsUPeriodic())
    {
      di << "Surface is u-periodic \n";
    }
    else
    {
      di << "Surface is not u-periodic \n";
    }
    if (aRevolSurf->IsVPeriodic())
    {
      di << "Surface is v-periodic \n";
    }
    else
    {
      di << "Surface is not v-periodic \n";
    }
  }
  catch (Standard_Failure const&)
  {
    di << "isperiodic Exception \n";
    return 0;
  }

  return 0;
}

#include <Precision.hxx>
#include <Extrema_ExtPS.hxx>
#include <GeomAdaptor_Surface.hxx>

//=================================================================================================

static int OCC486(Draw_Interpretor& di, int argc, const char** argv)
{
  try
  {
    OCC_CATCH_SIGNALS
    if (argc < 2)
    {
      di << "OCC486 FAULTY. Use : OCC486 surf x y z du dv";
      return 1;
    }

    double du = 0;
    double dv = 0;

    occ::handle<Geom_Surface> GS;
    GS = DrawTrSurf::GetSurface(argv[1]);
    if (GS.IsNull())
    {
      di << "OCC486 FAULTY. Null surface /n";
      return 1;
    }
    gp_Pnt P3D(Draw::Atof(argv[2]), Draw::Atof(argv[3]), Draw::Atof(argv[4]));

    constexpr double Tol = Precision::PConfusion();
    Extrema_ExtPS    myExtPS;
    if (argc > 5)
      du = Draw::Atof(argv[5]);
    if (argc > 6)
      dv = Draw::Atof(argv[6]);

    double uf, ul, vf, vl;
    GS->Bounds(uf, ul, vf, vl);

    GeomAdaptor_Surface aSurf(GS);
    myExtPS.Initialize(aSurf, uf - du, ul + du, vf - dv, vl + dv, Tol, Tol);
    myExtPS.Perform(P3D);
    int nPSurf = (myExtPS.IsDone() ? myExtPS.NbExt() : 0);

    if (nPSurf > 0)
    {
      // double distMin = myExtPS.Value ( 1 );
      double distMin = myExtPS.SquareDistance(1);
      int    indMin  = 1;
      for (int sol = 2; sol <= nPSurf; sol++)
      {
        // double dist = myExtPS.Value(sol);
        double dist = myExtPS.SquareDistance(sol);
        if (distMin > dist)
        {
          distMin = dist;
          indMin  = sol;
        }
      }
      distMin = sqrt(distMin);
      double S, T;
      myExtPS.Point(indMin).Parameter(S, T);
      gp_Pnt aCheckPnt  = aSurf.Value(S, T);
      double aCheckDist = P3D.Distance(aCheckPnt);
      di << "Solution is : U = " << S << "\t V = " << T << "\n";
      di << "Solution is : X = " << aCheckPnt.X() << "\t Y = " << aCheckPnt.Y()
         << "\t Z = " << aCheckPnt.Z() << "\n";
      di << "ExtremaDistance = " << distMin << "\n";
      di << "CheckDistance = " << aCheckDist << "\n";

      if (fabs(distMin - aCheckDist) < Precision::Confusion())
        return 0;
      else
        return 1;
    }
    else
      return 1;
  }
  catch (Standard_Failure const&)
  {
    di << "OCC486 Exception \n";
    return 1;
  }
}

#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS_Wire.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>

//=================================================================================================

static int OCC712(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " draftAngle slabThick\n";
    return 1;
  }
  // NOTE: Case:1 - draftAngle = 15, slabThick = 30 --> Fails
  //       Case:2   draftAngle = 10, slabThick = 30 --> Ok
  //       Case:3   draftAngle = 10, slabThick = 40 --> Ok
  //
  //       --------------------------------------------------
  double draftAngle = Draw::Atof(argv[1]);
  double slabThick  = Draw::Atof(argv[2]);

  double f1 = 75;
  double f2 = 35;

  gp_Pnt p1(-f2, f2, 0);
  gp_Pnt p2(0, f1, 0);
  gp_Pnt p3(f2, f2, 0);
  gp_Pnt p4(f1, 0, 0);
  gp_Pnt p5(f2, -f2, 0);
  gp_Pnt p6(0, -f1, 0);
  gp_Pnt p7(-f2, -f2, 0);
  gp_Pnt p8(-f1, 0, 0);

  GC_MakeArcOfCircle arc1(p1, p2, p3);
  GC_MakeArcOfCircle arc2(p3, p4, p5);
  GC_MakeArcOfCircle arc3(p5, p6, p7);
  GC_MakeArcOfCircle arc4(p7, p8, p1);

  TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(arc1.Value());
  TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(arc2.Value());
  TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(arc3.Value());
  TopoDS_Edge e4 = BRepBuilderAPI_MakeEdge(arc4.Value());

  BRepBuilderAPI_MakeWire MW;
  MW.Add(e1);
  MW.Add(e2);
  MW.Add(e3);
  MW.Add(e4);

  if (!MW.IsDone())
  {
    di << "my Wire not done\n";
    return 1;
  }
  TopoDS_Wire W = MW.Wire();

  TopoDS_Face F = BRepBuilderAPI_MakeFace(W);
  if (F.IsNull())
  {
    di << " Error in Face creation \n";
    return 1;
  }

  occ::handle<Geom_Surface> surf    = BRep_Tool::Surface(F);
  occ::handle<Geom_Plane>   P       = occ::down_cast<Geom_Plane>(surf);
  gp_Pln                    slabPln = P->Pln();

  try
  {
    OCC_CATCH_SIGNALS
    gp_Dir slabDir(gp_Dir::D::Z);
    gp_Vec slabVect(slabDir);
    slabVect *= slabThick;

    BRepPrimAPI_MakePrism slab(F, slabVect, true);
    if (!slab.IsDone())
    {
      di << " Error in Slab creation \n";
      return 1;
    }

    TopoDS_Shape slabShape = slab.Shape();
    if (fabs(draftAngle) > 0.01)
    {
      double                   angle = draftAngle * (M_PI / 180.0);
      BRepOffsetAPI_DraftAngle draftSlab(slabShape);

      TopoDS_Shape fShape = slab.FirstShape();
      TopoDS_Shape lShape = slab.LastShape();

      TopExp_Explorer ex;
      for (ex.Init(slabShape, TopAbs_FACE); ex.More(); ex.Next())
      {
        TopoDS_Face aFace = TopoDS::Face(ex.Current());
        if (aFace.IsSame(fShape) || aFace.IsSame(lShape))
          continue;
        draftSlab.Add(aFace, slabDir, angle, slabPln);
        if (!draftSlab.AddDone())
        {
          di << " Error in Add \n";
          return 1;
        }
      }

      di << "All Faces added. Building... \n";
      draftSlab.Build();
      di << "Build done...\n";
      if (!draftSlab.IsDone())
      {
        di << " Error in Build \n";
        return 1;
      }
      slabShape = draftSlab.Shape();
      DBRep::Set(argv[1], slabShape);
    }
  }
  catch (Standard_Failure const&)
  {
    di << " Error in Draft Slab \n";
    return 1;
  }
  return 0;
}

void QABugs::Commands_10(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("isperiodic",
                  "Use : isperiodic surfaceOfRevolution",
                  __FILE__,
                  isPeriodic,
                  group);
  theCommands.Add("OCC486", "Use : OCC486 surf x y z du dv ", __FILE__, OCC486, group);
  theCommands.Add("OCC712", "OCC712 draftAngle slabThick", __FILE__, OCC712, group);

  return;
}
