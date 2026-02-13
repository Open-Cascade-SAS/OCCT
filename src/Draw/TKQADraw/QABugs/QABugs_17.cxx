// Created on: 2002-03-18
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

#include <cstdio>

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <ViewerTest_EventManager.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_BSplineSurface.hxx>
#include <gp_Pln.hxx>
#include <Geom2d_Curve.hxx>
#include <GeomAPI.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Lin2d2Tan.hxx>
#include <Geom2d_Line.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dGcc_QCurve.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <Geom2dGcc_Lin2d2TanIter.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <V3d_View.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <PrsDim_LengthDimension.hxx>
#include <PrsDim_RadiusDimension.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2dGcc_Circ2d3Tan.hxx>

#include <Standard_ErrorHandler.hxx>

static int BUC60842(Draw_Interpretor& di, int /*argc*/, const char** /*argv*/)
{
  char abuf[16];
  // char * abuf = new char[16];

  const char* st = abuf;

  occ::handle<Geom_Circle>  cir = new Geom_Circle(gp_Ax2(gp_Pnt(823.687192, 502.366825, 478.960440),
                                                        gp_Dir(0.173648, 0.984808, 0.000000),
                                                        gp_Dir(-0.932169, 0.164367, -0.322560)),
                                                 50);
  occ::handle<Geom_Ellipse> ell =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                            gp_Dir(0.173648, 0.984808, 0.000000),
                            gp_Dir(-0.932169, 0.164367, -0.322560)),
                     150,
                     100);
  occ::handle<Geom_Plane> plne =
    new Geom_Plane(gp_Ax3(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                                 gp_Dir(0.173648, 0.984808, 0.000000),
                                 gp_Dir(-0.932169, 0.164367, -0.322560))));

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();

  gp_Pln                    pln     = plne->Pln();
  occ::handle<Geom2d_Curve> curve2d = GeomAPI::To2d(ell, pln);
  Sprintf(abuf, "ell");
  DrawTrSurf::Set(st, curve2d);
  if (!aContext.IsNull())
  {
    occ::handle<AIS_Shape> aisp =
      new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display(aisp, false);
  }

  occ::handle<Geom2d_Curve> fromcurve2d = GeomAPI::To2d(cir, pln);
  Sprintf(abuf, "cil");
  DrawTrSurf::Set(st, fromcurve2d);
  if (!aContext.IsNull())
  {
    occ::handle<AIS_Shape> aisp =
      new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(fromcurve2d, pln)).Edge());
    aContext->Display(aisp, false);
  }

  Geom2dAdaptor_Curve      acur(curve2d), afromcur(fromcurve2d);
  Geom2dGcc_QualifiedCurve qcur(acur, GccEnt_outside);
  Geom2dGcc_QualifiedCurve qfromcur(afromcur, GccEnt_outside);
  Geom2dGcc_Lin2d2Tan      lintan(qcur, qfromcur, 0.1, 0.0, 0.0);
  int                      i = 0;
  for (i = 0; i < lintan.NbSolutions(); i++)
  {
    Sprintf(abuf, "lintan_%d", i);
    occ::handle<Geom2d_Line> glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st, glin);
    if (!aContext.IsNull())
    {
      occ::handle<AIS_Shape> aisp =
        new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display(aisp, false);
    }
  }
  aContext->UpdateCurrentViewer();
  di << " Is Done = \n" << (int)lintan.IsDone();
  return 0;
}

static int BUC60843(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7)
  {
    di << "Usage : " << argv[0] << "result_shape name_of_circle name_of_curve [ par1 [ tol ] ]\n";
    return 1;
  }

  double par1 = 0.0, par2 = 0.0;
  double tol = Precision::Angular();
  if (argc >= 5)
    par1 = Draw::Atof(argv[4]);
  if (argc == 6)
    par2 = Draw::Atof(argv[5]);
  if (argc == 7)
    tol = Draw::Atof(argv[6]);
  occ::handle<Geom2d_Curve> aCur2d1 = DrawTrSurf::GetCurve2d(argv[2]);
  occ::handle<Geom2d_Curve> aCur2d2 = DrawTrSurf::GetCurve2d(argv[3]);
  if (aCur2d1.IsNull() || aCur2d2.IsNull())
  {
    di << "Faulty : not curves\n";
    return 1;
  }
  bool                       c1IsCircle = false;
  occ::handle<Geom2d_Circle> aCir2d     = occ::down_cast<Geom2d_Circle>(aCur2d1);
  if (!aCir2d.IsNull())
  {
    c1IsCircle = true;
    if (argc == 6)
      tol = Draw::Atof(argv[5]);
  }
  if (c1IsCircle)
  {
    Geom2dAdaptor_Curve     acur(aCur2d2);
    Geom2dGcc_QCurve        qcur(acur, GccEnt_unqualified);
    GccEnt_QualifiedCirc    qfromcur(aCir2d->Circ2d(), GccEnt_unqualified);
    Geom2dGcc_Lin2d2TanIter lintan(qfromcur, qcur, par1, tol);
    if (lintan.IsDone())
    {
      gp_Lin2d                 lin  = lintan.ThisSolution();
      occ::handle<Geom2d_Line> glin = new Geom2d_Line(lin);
      DrawTrSurf::Set(argv[1], glin);
    }
    else
    {
      di << "Faulty : tangent line is not done.\n";
    }
  }
  else
  {
    Geom2dAdaptor_Curve     acur1(aCur2d1);
    Geom2dAdaptor_Curve     acur2(aCur2d2);
    Geom2dGcc_QCurve        qcur1(acur1, GccEnt_unqualified);
    Geom2dGcc_QCurve        qcur2(acur2, GccEnt_unqualified);
    Geom2dGcc_Lin2d2TanIter lintan(qcur1, qcur2, par1, par2, tol);
    if (lintan.IsDone())
    {
      gp_Lin2d                 lin  = lintan.ThisSolution();
      occ::handle<Geom2d_Line> glin = new Geom2d_Line(lin);
      DrawTrSurf::Set(argv[1], glin);
    }
    else
    {
      di << "Faulty : tangent line is not done.\n";
    }
  }
  return 0;
}

static int BUC60970(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " shape result\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aResult = DBRep::Get(argv[1]);

  BRepBuilderAPI_MakeWire bRepSpineWire;
  TopExp_Explorer         exp_(aResult, TopAbs_WIRE);

  int i = 0;
  for (; exp_.More(); exp_.Next())
  {
    i++;
    bRepSpineWire.Add(TopoDS::Wire(exp_.Current()));
  }

  // printf("total no of wires are ............. %d\n", i);
  di << "total no of wires are ............. " << i << "\n";

  TopoDS_Wire spineWire = bRepSpineWire.Wire();
  aContext->Display(new AIS_Shape(spineWire), false);

  DBRep::Set("slineW", spineWire);

  TopExp_Explorer         spineWireExp(spineWire, TopAbs_EDGE);
  double                  first, last;
  occ::handle<Geom_Curve> curl_ =
    BRep_Tool::Curve(TopoDS::Edge(spineWireExp.Current()), first, last);
  gp_Pnt firstPnt;
  gp_Vec tanVec;
  curl_->D1(first, firstPnt, tanVec);
  tanVec.Normalize();
  gp_Dir                  tanDir(tanVec.X(), tanVec.Y(), tanVec.Z());
  gp_Ax2                  gpAx2(firstPnt, tanDir);
  gp_Circ                 gpCirc(gpAx2, 2.5);
  BRepBuilderAPI_MakeWire aMWire(BRepBuilderAPI_MakeEdge(new Geom_Circle(gpCirc)).Edge());
  TopoDS_Wire             topoWire(aMWire);
  aContext->Display(new AIS_Shape(topoWire), false);

  DBRep::Set("topoW", topoWire);

  BRepOffsetAPI_MakePipeShell bRepPipe(spineWire);
  bRepPipe.Add(topoWire);
  bRepPipe.Build();

  aContext->Display(new AIS_Shape(bRepPipe.Shape()), true);

  DBRep::Set(argv[2], bRepPipe.Shape());

  return 0;
}

static int BUC60915_1(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc > 1)
  {
    di << "Function don't has parameters\n";
    return 1;
  }

  if (ViewerTest::GetAISContext().IsNull())
  {
    di << "View was not created. Use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  occ::handle<AIS_InteractiveContext> context = ViewerTest::GetAISContext();

  // The following dimension code has problems regarding arrow_size. The desired effect is not
  // produced.
  /***************************************/
  // First view
  /***************************************/
  gp_Pnt                             p1 = gp_Pnt(602.51, 50., 0.);
  gp_Pnt                             p2 = gp_Pnt(602.51, 200., 0.);
  gp_Pnt                             p3 = gp_Pnt(102.51, 200., 0.);
  gp_Pnt                             p4 = gp_Pnt(102.51, 170., 0.);
  gp_Pnt                             p5 = gp_Pnt(502.51, 170., 0.);
  gp_Pnt                             p6 = gp_Pnt(502.51, 80., 0.);
  gp_Pnt                             p7 = gp_Pnt(102.51, 80., 0.);
  gp_Pnt                             p8 = gp_Pnt(102.51, 50., 0.);
  TopoDS_Vertex                      V1 = BRepBuilderAPI_MakeVertex(p1);
  TopoDS_Vertex                      V2 = BRepBuilderAPI_MakeVertex(p2);
  TopoDS_Vertex                      V3 = BRepBuilderAPI_MakeVertex(p3);
  TopoDS_Vertex                      V4 = BRepBuilderAPI_MakeVertex(p4);
  TopoDS_Vertex                      V5 = BRepBuilderAPI_MakeVertex(p5);
  TopoDS_Vertex                      V6 = BRepBuilderAPI_MakeVertex(p6);
  TopoDS_Vertex                      V7 = BRepBuilderAPI_MakeVertex(p7);
  TopoDS_Vertex                      V8 = BRepBuilderAPI_MakeVertex(p8);
  gp_Pnt                             plnpt(0, 0, 0);
  gp_Dir                             plndir(gp_Dir::D::Z);
  occ::handle<Geom_Plane>            pln      = new Geom_Plane(plnpt, plndir);
  occ::handle<Prs3d_DimensionAspect> anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d(true);
  anAspect->ArrowAspect()->SetAngle(M_PI / 180.0 * 10.0);
  /***************************************/
  // dimension "L 502.51"
  /***************************************/
  occ::handle<PrsDim_LengthDimension> len = new PrsDim_LengthDimension(V2, V3, pln->Pln());
  anAspect->ArrowAspect()->SetLength(30.0);
  len->SetDimensionAspect(anAspect);
  context->Display(len, false);
  /***************************************/
  // dimension "L 90"
  /***************************************/
  occ::handle<PrsDim_LengthDimension> len1 = new PrsDim_LengthDimension(V7, V4, pln->Pln());
  len1->SetDimensionAspect(anAspect);
  len1->SetFlyout(30.0);
  anAspect->ArrowAspect()->SetLength(100.0);
  context->Display(len1, false);
  /***************************************/
  // dimension "L 150"
  /***************************************/
  occ::handle<PrsDim_LengthDimension> len2 = new PrsDim_LengthDimension(V1, V2, pln->Pln());
  len2->SetDimensionAspect(anAspect);
  context->Display(len2, false);
  /***************************************/
  // dimension "R 88.58"
  /***************************************/
  gp_Circ     cir = gp_Circ(gp_Ax2(gp_Pnt(191.09, -88.58, 0), gp_Dir(gp_Dir::D::Z)), 88.58);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(cir, gp_Pnt(191.09, 0, 0.), gp_Pnt(191.09, -177.16, 0.));
  occ::handle<PrsDim_RadiusDimension> dim1 = new PrsDim_RadiusDimension(E1);
  dim1->SetDimensionAspect(anAspect);
  context->Display(dim1, false);
  /***************************************/
  // dimension "R 43.80"
  /***************************************/
  gp_Circ     cir1   = gp_Circ(gp_Ax2(gp_Pnt(191.09, -88.58, 0), gp_Dir(gp_Dir::D::Z)), 43.80);
  TopoDS_Edge E_cir1 = BRepBuilderAPI_MakeEdge(cir1);
  dim1               = new PrsDim_RadiusDimension(E_cir1);
  anAspect->ArrowAspect()->SetLength(60.0);
  dim1->SetDimensionAspect(anAspect);
  context->Display(dim1, false);
  /***************************************/
  // dimension "R 17.86"
  /***************************************/
  gp_Circ     cir2   = gp_Circ(gp_Ax2(gp_Pnt(566.11, -88.58, 0), gp_Dir(gp_Dir::D::NZ)), 17.86);
  TopoDS_Edge E_cir2 = BRepBuilderAPI_MakeEdge(cir2);
  dim1               = new PrsDim_RadiusDimension(E_cir2);
  anAspect->ArrowAspect()->SetLength(40.0);
  dim1->SetDimensionAspect(anAspect);
  context->Display(dim1, true);

  return 0;
}

static int OCC138(Draw_Interpretor& di, int /*argc*/, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeBox box1(gp_Pnt(0, 0, 0), gp_Pnt(100, 100, 100));
  BRepPrimAPI_MakeBox box2(gp_Pnt(120, 120, 120), gp_Pnt(300, 300, 300));
  BRepPrimAPI_MakeBox box3(gp_Pnt(320, 320, 320), gp_Pnt(500, 500, 500));

  occ::handle<AIS_InteractiveObject> ais1 = new AIS_Shape(box1.Shape());
  occ::handle<AIS_InteractiveObject> ais2 = new AIS_Shape(box2.Shape());
  occ::handle<AIS_InteractiveObject> ais3 = new AIS_Shape(box3.Shape());

  aContext->Display(ais1, false);
  aContext->Display(ais2, false);
  aContext->Display(ais3, false);

  aContext->AddOrRemoveSelected(ais1, false);
  aContext->AddOrRemoveSelected(ais2, false);
  aContext->AddOrRemoveSelected(ais3, false);

  di << "\n No of currents = " << aContext->NbSelected();

  aContext->InitSelected();

  int count = 1;
  while (aContext->MoreSelected())
  {
    di << "\n count is = " << count++;
    occ::handle<AIS_InteractiveObject> ais = aContext->SelectedInteractive();
    aContext->AddOrRemoveSelected(ais, false);
    aContext->InitSelected();
  }

  aContext->UpdateCurrentViewer();

  return 0;
}

static int OCC138LC(Draw_Interpretor& di, int /*argc*/, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeBox box1(gp_Pnt(0, 0, 0), gp_Pnt(100, 100, 100));
  BRepPrimAPI_MakeBox box2(gp_Pnt(120, 120, 120), gp_Pnt(300, 300, 300));
  BRepPrimAPI_MakeBox box3(gp_Pnt(320, 320, 320), gp_Pnt(500, 500, 500));

  occ::handle<AIS_InteractiveObject> ais1 = new AIS_Shape(box1.Shape());
  occ::handle<AIS_InteractiveObject> ais2 = new AIS_Shape(box2.Shape());
  occ::handle<AIS_InteractiveObject> ais3 = new AIS_Shape(box3.Shape());

  aContext->Display(ais1, false);
  aContext->Display(ais2, false);
  aContext->Display(ais3, false);

  aContext->AddOrRemoveSelected(ais1, false);
  aContext->AddOrRemoveSelected(ais2, false);
  aContext->AddOrRemoveSelected(ais3, false);

  di << "\n No of selected = " << aContext->NbSelected();

  aContext->InitSelected();

  int count = 1;
  while (aContext->MoreSelected())
  {
    di << "\n count is = %d" << count++;
    occ::handle<AIS_InteractiveObject> ais = aContext->SelectedInteractive();
    aContext->AddOrRemoveSelected(ais, false);
    aContext->InitSelected();
  }

  aContext->UpdateCurrentViewer();

  return 0;
}

#include <BRepBndLib.hxx>

//=================================================================================================

static int OCC566(Draw_Interpretor& di, int n, const char** a)
{
  if (n < 2)
  {
    di << "Usage: " << a[0] << " shape [ xmin ymin zmin xmax ymax zmax]\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);
  if (S.IsNull())
    return 1;
  Bnd_Box B;
  BRepBndLib::AddClose(S, B);
  double axmin, aymin, azmin, axmax, aymax, azmax;
  B.Get(axmin, aymin, azmin, axmax, aymax, azmax);
  di << axmin << " " << aymin << " " << azmin << " " << axmax << " " << aymax << " " << azmax;
  if (n >= 8)
  {
    Draw::Set(a[2], axmin);
    Draw::Set(a[3], aymin);
    Draw::Set(a[4], azmin);
    Draw::Set(a[5], axmax);
    Draw::Set(a[6], aymax);
    Draw::Set(a[7], azmax);
  }
  return 0;
}

#include <BRepFilletAPI_MakeFillet.hxx>

//=================================================================================================

static int OCC570(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage: " << argv[0] << " result\n";
    return 1;
  }

  BRepPrimAPI_MakeBox mkBox(100., 100., 100.);
  TopoDS_Shape        aBox = mkBox.Shape();

  TopExp_Explorer aExp;
  aExp.Init(aBox, TopAbs_WIRE);
  if (aExp.More())
  {
    TopoDS_Shape aWire = aExp.Current();

    aExp.Init(aWire, TopAbs_EDGE);
    TopoDS_Edge e1 = TopoDS::Edge(aExp.Current());
    aExp.Next();
    TopoDS_Edge e2 = TopoDS::Edge(aExp.Current());
    aExp.Next();
    TopoDS_Edge e3 = TopoDS::Edge(aExp.Current());
    aExp.Next();
    TopoDS_Edge e4 = TopoDS::Edge(aExp.Current());

    try
    {
      OCC_CATCH_SIGNALS
      BRepFilletAPI_MakeFillet mkFillet(aBox);
      mkFillet.SetContinuity(GeomAbs_C1, .001);

      // Setup variable fillet data
      NCollection_Array1<gp_Pnt2d> t_pnt(1, 4);
      t_pnt.SetValue(1, gp_Pnt2d(0.0, 5.0));
      t_pnt.SetValue(2, gp_Pnt2d(0.3, 15.0));
      t_pnt.SetValue(3, gp_Pnt2d(0.7, 15.0));
      t_pnt.SetValue(4, gp_Pnt2d(1.0, 5.0));

      // HERE:
      // It is impossible to build fillet if at least one edge
      // with variable radius is added!!! If all are constant, everything is ok.
      mkFillet.Add(t_pnt, e1);
      mkFillet.Add(5.0, e2);
      mkFillet.Add(t_pnt, e3);
      mkFillet.Add(5.0, e4);

      mkFillet.Build();
      TopoDS_Shape aFinalShape = mkFillet.Shape();

      DBRep::Set(argv[1], aFinalShape);
    }
    catch (Standard_Failure const&)
    {
      di << argv[0] << ": Exception in fillet\n";
      return 2;
    }
  }

  return 0;
}

#include <Law_Interpol.hxx>

static double        tesp       = 1.e-4;
static double        t3d        = 1.e-4;
static double        t2d        = 1.e-5;
static double        ta         = 1.e-2;
static double        fl         = 1.e-3;
static double        tapp_angle = 1.e-2;
static GeomAbs_Shape blend_cont = GeomAbs_C1;

static BRepFilletAPI_MakeFillet* Rake = nullptr;
static char                      name[100];

static void printtolblend(Draw_Interpretor& di)
{
  di << "tolerance ang : " << ta << "\n";
  di << "tolerance 3d  : " << t3d << "\n";
  di << "tolerance 2d  : " << t2d << "\n";
  di << "fleche        : " << fl << "\n";

  di << "tolblend " << ta << " " << t3d << " " << t2d << " " << fl << "\n";
}

static int MKEVOL(Draw_Interpretor& di, int narg, const char** a)
{
  delete Rake;
  Rake = nullptr;
  printtolblend(di);
  if (narg < 3)
    return 1;
  TopoDS_Shape V = DBRep::Get(a[2]);
  Rake           = new BRepFilletAPI_MakeFillet(V);
  Rake->SetParams(ta, tesp, t2d, t3d, t2d, fl);
  Rake->SetContinuity(blend_cont, tapp_angle);
  if (narg == 4)
  {
    ChFi3d_FilletShape FSh = ChFi3d_Rational;
    if (!strcasecmp(a[3], "Q"))
    {
      FSh = ChFi3d_QuasiAngular;
    }
    else if (!strcasecmp(a[3], "P"))
    {
      FSh = ChFi3d_Polynomial;
    }
    Rake->SetFilletShape(FSh);
  }
  strcpy(name, a[1]);
  return 0;
}

static int UPDATEVOL(Draw_Interpretor& di, int narg, const char** a)
{
  if (Rake == nullptr)
  {
    di << "MakeFillet non initialise\n";
    return 1;
  }
  if (narg % 2 != 0 || narg < 4)
    return 1;
  NCollection_Array1<gp_Pnt2d> uandr(1, (narg / 2) - 1);
  double                       Rad, Par;
  TopoDS_Shape                 aLocalEdge(DBRep::Get(a[1], TopAbs_EDGE));
  TopoDS_Edge                  E = TopoDS::Edge(aLocalEdge);
  for (int ii = 1; ii <= (narg / 2) - 1; ii++)
  {
    Par = Draw::Atof(a[2 * ii]);
    Rad = Draw::Atof(a[2 * ii + 1]);
    uandr.ChangeValue(ii).SetCoord(Par, Rad);
  }
  // HELPDESK: Add law creation
  const bool aLawMode = !strcmp(a[0], "OCC570evollaw");
  if (aLawMode)
  {
    di << "INFO: Using law to define variable fillet\n";
    occ::handle<Law_Interpol> law = new Law_Interpol;
    law->Set(uandr);
    Rake->Add(law, E);
  }
  else
    Rake->Add(uandr, E);

  return 0;
}

static int BUILDEVOL(Draw_Interpretor& di, int, const char**)
{
  if (Rake == nullptr)
  {
    di << "MakeFillet non initialise\n";
    return 1;
  }
  Rake->Build();
  if (Rake->IsDone())
  {
    TopoDS_Shape result = Rake->Shape();
    DBRep::Set(name, result);
    delete Rake;
    Rake = nullptr;
    return 0;
  }
  delete Rake;
  Rake = nullptr;
  return 1;
}

#include <Geom_TrimmedCurve.hxx>
#include <GeomFill_NSections.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

//=================================================================================================

static int OCC606(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3 && n != 4)
  {
    di << "Usage : " << a[0] << " result shape [-t]\n";
    return 0;
  }

  bool TrimMode = (n == 4);
  if (TrimMode)
    di << "INFO: Using trimmed curves...\n";
  else
    di << "INFO: Using non trimmed curves...\n";

  TopoDS_Shape S = DBRep::Get(a[2]);

  TopExp_Explorer                               t_exp(S, TopAbs_EDGE);
  NCollection_Sequence<occ::handle<Geom_Curve>> n_curves1;
  NCollection_Sequence<double>                  np;
  double                                        param = 5.0;

  for (; t_exp.More(); t_exp.Next())
  {
    double                  f, l;
    occ::handle<Geom_Curve> h_cur = BRep_Tool::Curve(TopoDS::Edge(t_exp.Current()), f, l);
    if (!h_cur.IsNull())
    {
      if (TrimMode)
        h_cur = new Geom_TrimmedCurve(h_cur, f, l);

      n_curves1.Append(h_cur);
      np.Append(param);
      param += 1.0;
    }
  }

  if (n_curves1.Length() > 1 && !np.IsEmpty())
  {
    try
    {
      OCC_CATCH_SIGNALS
      GeomFill_NSections               b_surface1(n_curves1, np);
      occ::handle<Geom_BSplineSurface> result_surf1 = b_surface1.BSplineSurface();
      if (!result_surf1.IsNull())
      {
        BRepBuilderAPI_MakeFace b_face1(result_surf1, Precision::Confusion());
        const TopoDS_Face&      bsp_face1 = b_face1.Face();
        DBRep::Set(a[1], bsp_face1);
      }
    }
    catch (Standard_Failure const&)
    {
      di << "ERROR: Exception in GeomFill_NSections\n";
    }
  }

  return 0;
}

//=================================================================================================

static int OCC813(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Usage : " << argv[0] << " U V\n";
    return 1;
  }

  const char* str;
  double      U = Draw::Atof(argv[1]);
  double      V = Draw::Atof(argv[2]);

  // Between ellipse and point:

  occ::handle<Geom_Ellipse> ell =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                            gp_Dir(0.173648, 0.984808, 0.000000),
                            gp_Dir(-0.932169, 0.164367, -0.322560)),
                     150,
                     100);
  occ::handle<Geom_Plane> plne =
    new Geom_Plane(gp_Ax3(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                                 gp_Dir(0.173648, 0.984808, 0.000000),
                                 gp_Dir(-0.932169, 0.164367, -0.322560))));

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();

  gp_Pnt2d pt2d(U, V);
  gp_Pln   pln = plne->Pln();

  str = "OCC813_pnt";
  DrawTrSurf::Set(str, pt2d);

  occ::handle<Geom2d_Curve> curve2d = GeomAPI::To2d(ell, pln);
  Geom2dAdaptor_Curve       acur(curve2d);
  Geom2dGcc_QualifiedCurve  qcur(acur, GccEnt_outside);

  str = "OCC813_ell";
  DrawTrSurf::Set(str, curve2d);
  if (!aContext.IsNull())
  {
    occ::handle<AIS_Shape> aisp =
      new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display(aisp, false);
  }

  // This does not give any solutions.
  Geom2dGcc_Lin2d2Tan lintan(qcur, pt2d, 0.1);
  di << "OCC813 nb of solutions = " << lintan.NbSolutions() << "\n";

  char        abuf[16];
  const char* st = abuf;

  int i;
  for (i = 1; i <= lintan.NbSolutions(); i++)
  {
    Sprintf(abuf, "lintan_%d", i);
    occ::handle<Geom2d_Line> glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st, glin);
    if (!aContext.IsNull())
    {
      occ::handle<AIS_Shape> aisp =
        new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display(aisp, false);
    }
  }

  if (!aContext.IsNull())
  {
    aContext->UpdateCurrentViewer();
  }

  return 0;
}

//=================================================================================================

static int OCC814(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc > 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  const char* str;

  // Between Ellipse and Circle:

  occ::handle<Geom_Circle>  cir = new Geom_Circle(gp_Ax2(gp_Pnt(823.687192, 502.366825, 478.960440),
                                                        gp_Dir(0.173648, 0.984808, 0.000000),
                                                        gp_Dir(-0.932169, 0.164367, -0.322560)),
                                                 50);
  occ::handle<Geom_Ellipse> ell =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                            gp_Dir(0.173648, 0.984808, 0.000000),
                            gp_Dir(-0.932169, 0.164367, -0.322560)),
                     150,
                     100);
  occ::handle<Geom_Plane> plne =
    new Geom_Plane(gp_Ax3(gp_Ax2(gp_Pnt(1262.224429, 425.040878, 363.609716),
                                 gp_Dir(0.173648, 0.984808, 0.000000),
                                 gp_Dir(-0.932169, 0.164367, -0.322560))));

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();

  gp_Pln                    pln         = plne->Pln();
  occ::handle<Geom2d_Curve> curve2d     = GeomAPI::To2d(ell, pln);
  occ::handle<Geom2d_Curve> fromcurve2d = GeomAPI::To2d(cir, pln);

  str = "OCC814_cir";
  DrawTrSurf::Set(str, curve2d);
  str = "OCC814_ell";
  DrawTrSurf::Set(str, fromcurve2d);
  if (!aContext.IsNull())
  {
    occ::handle<AIS_Shape> aisp =
      new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(curve2d, pln)).Edge());
    aContext->Display(aisp, false);
  }
  if (!aContext.IsNull())
  {
    occ::handle<AIS_Shape> aisp =
      new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(fromcurve2d, pln)).Edge());
    aContext->Display(aisp, false);
  }

  Geom2dAdaptor_Curve acur(curve2d), afromcur(fromcurve2d);

  Geom2dGcc_QualifiedCurve qcur(acur, GccEnt_outside);
  Geom2dGcc_QualifiedCurve qfromcur(afromcur, GccEnt_outside);

  // This does not give any solutions.
  Geom2dGcc_Lin2d2Tan lintan(qcur, qfromcur, 0.1);
  di << "OCC814 nb of solutions = " << lintan.NbSolutions() << "\n";

  char        abuf[16];
  const char* st = abuf;

  int i;
  for (i = 1; i <= lintan.NbSolutions(); i++)
  {
    Sprintf(abuf, "lintan_%d", i);
    occ::handle<Geom2d_Line> glin = new Geom2d_Line(lintan.ThisSolution(i));
    DrawTrSurf::Set(st, glin);
    if (!aContext.IsNull())
    {
      occ::handle<AIS_Shape> aisp =
        new AIS_Shape(BRepBuilderAPI_MakeEdge(GeomAPI::To3d(glin, pln)).Edge());
      aContext->Display(aisp, false);
    }
  }

  if (!aContext.IsNull())
  {
    aContext->UpdateCurrentViewer();
  }

  return 0;
}

#include <ShapeFix_Wire.hxx>

//=================================================================================================

static int OCC884(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3 || argc > 5)
  {
    di << "Usage : " << argv[0] << " result shape [toler [maxtoler]]\n";
    return 1;
  }

  TopoDS_Shape shape = DBRep::Get(argv[2]); // read the shape

  TopExp_Explorer exp(shape, TopAbs_EDGE);
  if (!exp.More())
  {
    di << "Error: no edge found\n";
    return 1;
  }

  BRep_Builder builder;
  TopoDS_Wire  wire;
  builder.MakeWire(wire);
  builder.Add(wire, TopoDS::Edge(exp.Current()));

  // HelpDesk: Create planar face if possible
  TopoDS_Face face = BRepBuilderAPI_MakeFace(wire, true);

  occ::handle<ShapeAnalysis_Wire> advWA = new ShapeAnalysis_Wire;
  advWA->Load(wire);
  advWA->SetFace(face);
  advWA->SetPrecision(0.001);

  NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
  NCollection_Sequence<gp_Pnt>                     points3d;

  di << "Info: CheckSelfIntersectingEdge = "
     << (int)advWA->CheckSelfIntersectingEdge(1, points2d, points3d) << "\n";

  ShapeExtend_Status status = ShapeExtend_FAIL1;
  if (advWA->StatusSelfIntersection(status))
    di << "Info: No P Curve found in the edge\n";

  status = ShapeExtend_FAIL2;
  if (advWA->StatusSelfIntersection(status))
    di << "Info: No Vertices found in the edge\n";

  status = ShapeExtend_DONE1;
  if (advWA->StatusSelfIntersection(status))
    di << "Info: Self-intersection found in the edge\n";

  int i, num = points2d.Length();
  di << "Info: No. of self-intersection points : " << num << "\n";

  char        str[80];
  const char* aName = str;
  for (i = 1; i <= num; ++i)
  {
    gp_Pnt pt = points3d(i);
    di << "Info: Intersecting pt : (" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << ")\n";
    Sprintf(str, "p_%d", i);
    DrawTrSurf::Set(aName, pt);
  }

  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire;
  sfw->Load(wire);
  sfw->SetFace(face);

  if (argc > 3)
    sfw->SetPrecision(Draw::Atof(argv[3]) /*0.1*/);
  if (argc > 4)
    sfw->SetMaxTolerance(Draw::Atof(argv[4]));
  di << "Info: Precision is set to " << sfw->Precision() << "\n";
  di << "Info: MaxTolerance is set to " << sfw->MaxTolerance() << "\n";

  sfw->ModifyTopologyMode()          = true;
  sfw->ModifyGeometryMode()          = true;
  sfw->FixSelfIntersectingEdgeMode() = 1;
  // printf("Info: FixSelfIntersection = %d\n",sfw->FixSelfIntersection());
  di << "Info: FixSelfIntersection = " << (int)sfw->FixSelfIntersection() << "\n";

  DBRep::Set(argv[1], sfw->Wire());

  status = ShapeExtend_OK;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_OK : No intersection found\n";
  // printf("Info: ShapeExtend_OK : No intersection found\n");

  status = ShapeExtend_FAIL1;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL1 : analysis failed (edge has no pcurve, or no vertices etc.)\n";

  status = ShapeExtend_FAIL2;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL2 : self-intersection was found, but not fixed because of limit "
          "of increasing tolerance (MaxTolerance)\n";

  status = ShapeExtend_FAIL3;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_FAIL3 : intercestion of non adjacent edges found, but not fixed "
          "because of limit of increasing tolerance (MaxTolerance)\n";

  status = ShapeExtend_DONE1;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE1 : tolerance of vertex was increased to fix self-intersection\n";

  status = ShapeExtend_DONE2;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE2 : vertex was moved to fix self-intersection\n";

  status = ShapeExtend_DONE3;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE3 : some edges were removed because of intersection\n";

  status = ShapeExtend_DONE4;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE4 : pcurve(s) was(were) modified\n";

  status = ShapeExtend_DONE5;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE5 : non adjacent intersection fixed by increasing tolerance of "
          "vertex(vertices)\n";

  status = ShapeExtend_DONE6;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE6 : tolerance of edge was increased to hide intersection\n";

  status = ShapeExtend_DONE7;
  if (sfw->StatusSelfIntersection(status))
    di << "Info: ShapeExtend_DONE7 : range of some edges was decreased to avoid intersection\n";

  return 0;
}

#include <TopoDS_Solid.hxx>
#include <BRepFeat_MakeDPrism.hxx>

//=======================================================================
// function : OCCN1
// purpose  : FEATURE OPERATION (BOSS and SLOT)
//=======================================================================
static int OCCN1(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " angle fuse(1 for boss / 0 for slot) length\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  double angle  = Draw::Atof(argv[1]);
  int    fuse   = Draw::Atoi(argv[2]);
  double length = Draw::Atof(argv[3]);

  BRepBuilderAPI_MakeEdge edge1(gp_Pnt(0, 0, 0), gp_Pnt(50, 0, 0));
  BRepBuilderAPI_MakeEdge edge2(gp_Pnt(50, 0, 0), gp_Pnt(50, 50, 0));
  BRepBuilderAPI_MakeEdge edge3(gp_Pnt(50, 50, 0), gp_Pnt(0, 50, 0));
  BRepBuilderAPI_MakeEdge edge4(gp_Pnt(0, 50, 0), gp_Pnt(0, 0, 0));
  TopoDS_Edge             ted1 = edge1.Edge();
  TopoDS_Edge             ted2 = edge2.Edge();
  TopoDS_Edge             ted3 = edge3.Edge();
  TopoDS_Edge             ted4 = edge4.Edge();

  BRepBuilderAPI_MakeWire wire(ted1, ted2, ted3, ted4);
  TopoDS_Wire             twire = wire.Wire();

  BRepBuilderAPI_MakeFace face(twire);
  const TopoDS_Face&      tface = face.Face();
  ////////occ::handle<AIS_Shape> face_ais = new AIS_Shape( tface );
  ////////aContext->Display(face_ais);

  TopoDS_Solid        box;
  BRepPrimAPI_MakeBox block(gp_Pnt(-50, -50, -150), 150, 150, 100);
  box = block.Solid();

  TopoDS_Face top_face;
  top_face = block.TopFace();

  BRepFeat_MakeDPrism slot(box, tface, top_face, angle, fuse, true);

  slot.Perform(length);
  TopoDS_Shape S2 = slot.Shape();

  DBRep::Set("OCCN1_tface", tface);
  DBRep::Set("OCCN1_slot", S2);

  return 0;
}

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgoAPI_Section.hxx>

//=================================================================================================

static int OCCN2(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc > 2)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  BRepPrimAPI_MakeCylinder cylinder(50, 200);
  TopoDS_Shape             cylinder_sh = cylinder.Shape();

  BRepPrimAPI_MakeSphere sphere(gp_Pnt(60, 0, 100), 50);
  TopoDS_Shape           sphere_sh = sphere.Shape();

  di << "BRepAlgoAPI_Section section(cylinder_sh, sphere_sh)\n";
  BRepAlgoAPI_Section section(cylinder_sh, sphere_sh);
  if (!section.IsDone())
  {
    di << "Error performing intersection: not done.\n";
  }
  const TopoDS_Shape& shape = section.Shape();

  DBRep::Set("OCCN2_cylinder", cylinder_sh);
  DBRep::Set("OCCN2_sphere", sphere_sh);
  DBRep::Set("OCCN2_section", shape);

  return 0;
}

#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Geom_BezierCurve.hxx>

static int OCC2569(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " nbpoles result\n";
    return 1;
  }

  int poles = Draw::Atoi(argv[1]);

  NCollection_Array1<gp_Pnt> arr(1, poles);
  for (int i = 1; i <= poles; i++)
    arr.SetValue(i, gp_Pnt(i + 10, i * 2 + 20, i * 3 + 45));

  occ::handle<Geom_BezierCurve> bez = new Geom_BezierCurve(arr);
  if (bez.IsNull())
  {
    di << "\n The curve is not created.\n";
  }
  else
  {
    di << "\n Degree = " << bez->Degree() << "\n";
  }
  TopoDS_Edge            sh  = BRepBuilderAPI_MakeEdge(bez).Edge();
  occ::handle<AIS_Shape> ais = new AIS_Shape(sh);
  aContext->Display(ais, true);
  DrawTrSurf::Set(argv[2], bez);
  return 0;
}

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>

static int OCC1642(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 10)
  {
    di << "Usage: " << argv[0]
       << " FinalWare FinalFace InitWare InitFace shape FixReorder FixDegenerated FixConnected "
          "FixSelfIntersection\n";
    return 1;
  }

  TopoDS_Shape shape = DBRep::Get(argv[5]);

  TopExp_Explorer exp(shape, TopAbs_WIRE);
  TopoDS_Shape    wire = exp.Current();

  occ::handle<ShapeAnalysis_Wire> advWA = new ShapeAnalysis_Wire;
  advWA->Load(TopoDS::Wire(wire));

  DBRep::Set(argv[3], wire);

  TopoDS_Face face = BRepBuilderAPI_MakeFace(TopoDS::Wire(wire), true);

  DBRep::Set(argv[4], face);

  advWA->SetFace(face);
  double precision_to_ana = 0.0001;
  advWA->SetPrecision(precision_to_ana);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> M;
  TopExp::MapShapes(wire, TopAbs_EDGE, M);

  int j = 1;
  for (j = 1; j <= M.Extent(); ++j)
  {
    int                                              num = 1;
    NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
    NCollection_Sequence<gp_Pnt>                     points3d;

    std::cout << "\n j =" << j << ",  CheckSelfIntersectingEdge = "
              << advWA->CheckSelfIntersectingEdge(j, points2d, points3d);

    ShapeExtend_Status status = ShapeExtend_FAIL1;
    if (advWA->StatusSelfIntersection(status))

      status = ShapeExtend_FAIL2;
    if (advWA->StatusSelfIntersection(status))
      di << "\n No Vertices found in the edge";

    status = ShapeExtend_DONE1;
    if (advWA->StatusSelfIntersection(status))
      di << "\n Self-intersection found in the edge";

    num = points2d.Length();
    di << "\n No. of self-intersecting edges : " << num;

    for (int i = 1; i <= num; ++i)
    {
      gp_Pnt pt = points3d(i);
      di << "\n Intersecting pt : (" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << ")";
    }
  }

  occ::handle<ShapeFix_Wire> sfw = new ShapeFix_Wire;
  sfw->Load(TopoDS::Wire(wire));
  sfw->SetFace(face);
  sfw->SetPrecision(Precision::Confusion());

  sfw->FixReorderMode()                      = 1;
  sfw->ClosedWireMode()                      = true;
  sfw->FixDegeneratedMode()                  = 1;
  sfw->FixConnectedMode()                    = 1;
  sfw->ModifyTopologyMode()                  = true;
  sfw->ModifyGeometryMode()                  = true;
  sfw->FixSelfIntersectionMode()             = 1;
  sfw->FixSelfIntersectingEdgeMode()         = 1;
  sfw->FixIntersectingEdgesMode()            = 1;
  sfw->FixNonAdjacentIntersectingEdgesMode() = 1;
  sfw->FixEdgeCurvesMode()                   = 1;

  sfw->ModifyRemoveLoopMode() = 1;
  sfw->SetMaxTolerance(1.0);

  di << "\n FixReorder = " << (int)sfw->FixReorder();
  di << "\n FixDegenerated = " << (int)sfw->FixDegenerated();
  di << "\n FixConnected = " << (int)sfw->FixConnected();
  di << "\n";
  di << "\n FixSelfIntersection = " << (int)sfw->FixSelfIntersection();
  di << "\n";

  Draw::Set(argv[6], sfw->FixReorder());
  Draw::Set(argv[7], sfw->FixDegenerated());
  Draw::Set(argv[8], sfw->FixConnected());
  Draw::Set(argv[9], sfw->FixSelfIntersection());

  ShapeExtend_Status status = ShapeExtend_OK;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_OK : No intersection found";

  status = ShapeExtend_FAIL1;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL1 : analysis failed (edge has no pcurve,or no vertices etc.)";

  status = ShapeExtend_FAIL2;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL2 : self-intersection was found, but not fixed because of limit of "
          "increasing tolerance (MaxTolerance)";

  status = ShapeExtend_FAIL3;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_FAIL3 : intercestion of non adjacent edges found, but not fixed because "
          "of limit of increasing tolerance (MaxTolerance)";

  status = ShapeExtend_DONE1;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE1 : tolerance of vertex was increased to fix self-intersection";

  status = ShapeExtend_DONE2;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE2 : vertex was moved to fix self-intersection";

  status = ShapeExtend_DONE3;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE3 : some edges were removed because of intersection";

  status = ShapeExtend_DONE4;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE4 : pcurve(s) was(were) modified";

  status = ShapeExtend_DONE5;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE5 : non adjacent intersection fixed by increasing tolerance of "
          "vertex(vertices)";

  status = ShapeExtend_DONE6;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE6 : tolerance of edge was increased to hide intersection";

  status = ShapeExtend_DONE7;
  if (sfw->StatusSelfIntersection(status))
    di << "\n ShapeExtend_DONE7 : range of some edges was decreased to avoid intersection";

  TopoDS_Wire finalwire = sfw->Wire();

  DBRep::Set(argv[1], finalwire);

  advWA->Load(TopoDS::Wire(finalwire));

  TopoDS_Face fface = BRepBuilderAPI_MakeFace(TopoDS::Wire(finalwire), true);

  DBRep::Set(argv[2], fface);

  advWA->SetFace(fface);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> fM;
  TopExp::MapShapes(finalwire, TopAbs_EDGE, fM);

  for (j = 1; j <= fM.Extent(); ++j)
  {
    int                                              num = 1;
    NCollection_Sequence<IntRes2d_IntersectionPoint> points2d;
    NCollection_Sequence<gp_Pnt>                     points3d;

    di << "\n j =" << j << ",  CheckSelfIntersectingEdge = "
       << (int)advWA->CheckSelfIntersectingEdge(j, points2d, points3d);

    status = ShapeExtend_FAIL1;
    if (advWA->StatusSelfIntersection(status))

      status = ShapeExtend_FAIL2;
    if (advWA->StatusSelfIntersection(status))
      di << "\n No Vertices found in the edge";

    status = ShapeExtend_DONE1;
    if (advWA->StatusSelfIntersection(status))
      di << "\n Self-intersection found in the edge";

    num = points2d.Length();
    di << "\n No. of self-intersecting edges : " << num;

    for (int i = 1; i <= num; ++i)
    {
      gp_Pnt pt = points3d(i);
      di << "\n Intersecting pt : (" << pt.X() << ", " << pt.Y() << ", " << pt.Z() << ")";
    }
  }

  di << "\n";

  return 0;
}

void QABugs::Commands_17(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("BUC60842", "BUC60842", __FILE__, BUC60842, group);
  theCommands.Add("BUC60843",
                  "BUC60843 result_shape name_of_circle name_of_curve [ par1 [ tol ] ]",
                  __FILE__,
                  BUC60843,
                  group);
  theCommands.Add("BUC60970", "BUC60970 shape result", __FILE__, BUC60970, group);
  theCommands.Add("BUC60915", "BUC60915", __FILE__, BUC60915_1, group);
  theCommands.Add("OCC138", "OCC138", __FILE__, OCC138, group);
  theCommands.Add("OCC138LC", "OCC138LC", __FILE__, OCC138LC, group);
  theCommands.Add("OCC566",
                  "OCC566 shape [ xmin ymin zmin xmax ymax zmax] ; print bounding box",
                  __FILE__,
                  OCC566,
                  group);
  theCommands.Add("OCC570", "OCC570 result", __FILE__, OCC570, group);

  theCommands.Add("OCC570mkevol",
                  "OCC570mkevol result object (then use updatevol) [R/Q/P]; mkevol",
                  __FILE__,
                  MKEVOL,
                  group);
  theCommands.Add("OCC570updatevol",
                  "OCC570updatevol edge u1 rad1 u2 rad2 ...; updatevol",
                  __FILE__,
                  UPDATEVOL,
                  group);
  theCommands.Add("OCC570updatevollaw",
                  "OCC570updatevollaw edge u1 rad1 u2 rad2 ...; updatevollaw",
                  __FILE__,
                  UPDATEVOL,
                  group);
  theCommands.Add("OCC570buildevol",
                  "OCC570buildevol; end of the evol fillet computation",
                  __FILE__,
                  BUILDEVOL,
                  group);

  theCommands.Add("OCC606", "OCC606 result shape [-t]", __FILE__, OCC606, group);

  theCommands.Add("OCC813", "OCC813 U V", __FILE__, OCC813, group);
  theCommands.Add("OCC814", "OCC814", __FILE__, OCC814, group);
  theCommands.Add("OCC884", "OCC884 result shape [toler [maxtoler]]", __FILE__, OCC884, group);

  theCommands.Add("OCCN1",
                  "OCCN1 angle fuse(1 for boss / 0 for slot) length",
                  __FILE__,
                  OCCN1,
                  group);
  theCommands.Add("OCCN2", "OCCN2", __FILE__, OCCN2, group);

  theCommands.Add("OCC2569", "OCC2569 nbpoles result", __FILE__, OCC2569, group);

  theCommands.Add("OCC1642",
                  "OCC1642 FinalWare FinalFace InitWare InitFace shape FixReorder FixDegenerated "
                  "FixConnected FixSelfIntersection",
                  __FILE__,
                  OCC1642,
                  group);

  return;
}
