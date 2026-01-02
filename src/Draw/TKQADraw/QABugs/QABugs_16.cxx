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

#include <QABugs.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <AIS_InteractiveContext.hxx>
#include <ViewerTest.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepGProp.hxx>
#include <AIS_Trihedron.hxx>
#include <Geom_Axis2Placement.hxx>
#include <gp_Ax2.hxx>
#include <Geom_Circle.hxx>
#include <AIS_Circle.hxx>
#include <gp_Pln.hxx>
#include <PrsDim_AngleDimension.hxx>

#include <Aspect_Window.hxx>
#include <V3d_View.hxx>

#include <TopExp_Explorer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <GC_MakePlane.hxx>
#include <AIS_PlaneTrihedron.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_DoubleMap.hxx>

#include <BRep_Tool.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Precision.hxx>

#include <GProp_PrincipalProps.hxx>

#include <OSD_Path.hxx>
#include <Standard_ProgramError.hxx>

#include <ShapeFix_Wireframe.hxx>
#include <ShapeBuild_ReShape.hxx>

#include <ViewerTest_EventManager.hxx>

#include <DDocStd.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_ErrorHandler.hxx>

#if !defined(_WIN32)
extern NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
  GetMapOfAIS();
#else
Standard_EXPORT NCollection_DoubleMap<occ::handle<AIS_InteractiveObject>, TCollection_AsciiString>&
                GetMapOfAIS();
#endif

static int BUC60848(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " shape \n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull())
  {
    di << "Shape is empty\n";
    return 1;
  }
  GProp_GProps G;
  BRepGProp::VolumeProperties(S, G);
  double GRes;
  GRes = G.Mass();
  if (GRes < 0)
  {
    di << "Result is negative : " << GRes << "\n";
    return 1;
  }
  else
  {
    di << "Volume : " << GRes << "\n";
  }

  return 0;
}

static int BUC60814(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 1)
  {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> myAISContext = ViewerTest::GetAISContext();
  if (myAISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  // TRIHEDRON
  occ::handle<AIS_InteractiveObject> aTrihedron;
  occ::handle<Geom_Axis2Placement>   aTrihedronAxis = new Geom_Axis2Placement(gp::XOY());
  aTrihedron                                        = new AIS_Trihedron(aTrihedronAxis);
  myAISContext->Display(aTrihedron, false);

  // Circle
  gp_Pnt P(10, 10, 10);
  gp_Dir V(gp_Dir::D::X);
  gp_Ax2 aAx2(P, V);

  occ::handle<Geom_Circle>           ahCircle = new Geom_Circle(aAx2, 20);
  occ::handle<AIS_InteractiveObject> aCircle  = new AIS_Circle(ahCircle);
  myAISContext->Display(aCircle, false);

  const occ::handle<Prs3d_Drawer>& aSelStyle = myAISContext->SelectionStyle();
  aSelStyle->SetColor(Quantity_NOC_BLUE1);

  myAISContext->AddOrRemoveSelected(aTrihedron, false);
  myAISContext->AddOrRemoveSelected(aCircle, true);

  return 0;
}

//=================================================================================================

static int BUC60774(Draw_Interpretor& theDi, int theArgNb, const char** theArgv)
{
  if (theArgNb != 1)
  {
    std::cout << "Usage : " << theArgv[0] << "\n";
    return -1;
  }

  const occ::handle<AIS_InteractiveContext>& anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cout << "use 'vinit' command before " << theArgv[0] << "\n";
    return -1;
  }

  const occ::handle<V3d_View>& aV3dView = ViewerTest::CurrentView();

  int aWinWidth  = 0;
  int aWinHeight = 0;
  aV3dView->Window()->Size(aWinWidth, aWinHeight);

  int aXPixMin = 0;
  int aYPixMin = 0;
  int aXPixMax = aWinWidth;
  int aYPixMax = aWinHeight;

  AIS_StatusOfPick aPickStatus =
    anAISContext->SelectRectangle(NCollection_Vec2<int>(aXPixMin, aYPixMin),
                                  NCollection_Vec2<int>(aXPixMax, aYPixMax),
                                  aV3dView);
  theDi << (aPickStatus == AIS_SOP_NothingSelected
              ? "status = AIS_SOP_NothingSelected : OK"
              : "status = AIS_SOP_NothingSelected : bugged - Faulty ");
  theDi << "\n";

  theDi.Eval("box b 10 10 10");
  theDi.Eval(" vdisplay b");

  aPickStatus = anAISContext->SelectRectangle(NCollection_Vec2<int>(aXPixMin, aYPixMin),
                                              NCollection_Vec2<int>(aXPixMax, aYPixMax),
                                              aV3dView);
  theDi << (aPickStatus == AIS_SOP_OneSelected ? "status = AIS_SOP_OneSelected : OK"
                                               : "status = AIS_SOP_OneSelected : bugged - Faulty ");
  theDi << "\n";

  theDi.Eval("box w 20 20 20 20 20 20");
  theDi.Eval(" vdisplay w");

  aPickStatus = anAISContext->SelectRectangle(NCollection_Vec2<int>(aXPixMin, aYPixMin),
                                              NCollection_Vec2<int>(aXPixMax, aYPixMax),
                                              aV3dView);
  anAISContext->UpdateCurrentViewer();
  theDi << (aPickStatus == AIS_SOP_SeveralSelected
              ? "status = AIS_SOP_SeveralSelected : OK"
              : "status = AIS_SOP_SeveralSelected : bugged - Faulty ");
  theDi << "\n";

  return 0;
}

static int BUC60972(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  if (argc != 6)
  {
    di << "Usage : " << argv[0] << " edge edge plane val text\n";
    return 1;
  }

  TopoDS_Edge             aFirst  = TopoDS::Edge(DBRep::Get(argv[1], TopAbs_EDGE));
  TopoDS_Edge             aSecond = TopoDS::Edge(DBRep::Get(argv[2], TopAbs_EDGE));
  occ::handle<Geom_Plane> aPlane  = occ::down_cast<Geom_Plane>(DrawTrSurf::GetSurface(argv[3]));
  if (aPlane.IsNull())
    return 1;

  di << aPlane->Pln().SquareDistance(gp_Pnt(0, 0, 0)) << "\n";

  TCollection_ExtendedString aText(argv[5]);
  // const char16_t* ExtString_aText = aText.ToExtString();
  // di << ExtString_aText << " " << Draw::Atof(argv[4]) << "\n";
  di << argv[5] << " " << Draw::Atof(argv[4]) << "\n";

  occ::handle<PrsDim_AngleDimension> aDim = new PrsDim_AngleDimension(aFirst, aSecond);
  aContext->Display(aDim, true);

  return 0;
}

static int OCC218bug(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  if (argc != 5)
  {
    di << "Usage : " << argv[0] << " name plane Xlabel Ylabel\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(argv[2]);
  if (S.IsNull())
  {
    di << "Shape is empty\n";
    return 1;
  }

  TCollection_AsciiString name(argv[1]);
  TCollection_AsciiString Xlabel(argv[3]);
  TCollection_AsciiString Ylabel(argv[4]);

  // Construction de l'AIS_PlaneTrihedron
  occ::handle<AIS_PlaneTrihedron> theAISPlaneTri;

  bool IsBound = GetMapOfAIS().IsBound2(name);
  if (IsBound)
  {
    // on recupere la shape dans la map des objets displayes
    occ::handle<AIS_InteractiveObject> aShape = GetMapOfAIS().Find2(name);

    // On verifie que l'AIS InteraciveObject est bien
    // un AIS_PlaneTrihedron
    if (aShape->Type() == AIS_KindOfInteractive_Datum && aShape->Signature() == 4)
    {
      // On downcast aShape de AIS_InteractiveObject a AIS_PlaneTrihedron
      theAISPlaneTri = occ::down_cast<AIS_PlaneTrihedron>(aShape);

      theAISPlaneTri->SetXLabel(Xlabel);
      theAISPlaneTri->SetYLabel(Ylabel);

      aContext->Redisplay(theAISPlaneTri, false);
      aContext->UpdateCurrentViewer();
    }
  }
  else
  {
    TopoDS_Face FaceB = TopoDS::Face(S);

    // Construction du Plane
    // recuperation des edges des faces.
    TopExp_Explorer FaceExpB(FaceB, TopAbs_EDGE);

    TopoDS_Edge EdgeB = TopoDS::Edge(FaceExpB.Current());
    // declarations
    gp_Pnt A, B, C;

    // si il y a plusieurs edges
    if (FaceExpB.More())
    {
      FaceExpB.Next();
      TopoDS_Edge       EdgeC = TopoDS::Edge(FaceExpB.Current());
      BRepAdaptor_Curve theCurveB(EdgeB);
      BRepAdaptor_Curve theCurveC(EdgeC);
      A = theCurveC.Value(0.1);
      B = theCurveC.Value(0.9);
      C = theCurveB.Value(0.5);
    }
    else
    {
      // FaceB a 1 unique edge courbe
      BRepAdaptor_Curve theCurveB(EdgeB);
      A = theCurveB.Value(0.1);
      B = theCurveB.Value(0.9);
      C = theCurveB.Value(0.5);
    }
    // Construction du Geom_Plane
    GC_MakePlane                   MkPlane(A, B, C);
    const occ::handle<Geom_Plane>& theGeomPlane = MkPlane.Value();

    // on le display & bind
    theAISPlaneTri = new AIS_PlaneTrihedron(theGeomPlane);

    theAISPlaneTri->SetXLabel(Xlabel);
    theAISPlaneTri->SetYLabel(Ylabel);

    GetMapOfAIS().Bind(theAISPlaneTri, name);
    aContext->Display(theAISPlaneTri, true);
  }
  return 0;
}

static int OCC295(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " edge_result edge1 edge2\n";
    return 1;
  }

  TopoDS_Shape Sh1 = DBRep::Get(argv[2]);
  TopoDS_Shape Sh2 = DBRep::Get(argv[3]);
  if (Sh1.IsNull() || Sh2.IsNull())
    return 1;
  if (Sh1.ShapeType() != TopAbs_EDGE || Sh2.ShapeType() != TopAbs_EDGE)
    return 1;
  TopoDS_Edge                    e1 = TopoDS::Edge(Sh1);
  TopoDS_Edge                    e2 = TopoDS::Edge(Sh2);
  double                         f1, l1, f2, l2;
  bool                           After  = true;
  occ::handle<Geom_Curve>        ac1    = BRep_Tool::Curve(e1, f1, l1);
  occ::handle<Geom_Curve>        ac2    = BRep_Tool::Curve(e2, f2, l2);
  occ::handle<Geom_BSplineCurve> bsplc1 = occ::down_cast<Geom_BSplineCurve>(ac1);
  occ::handle<Geom_BSplineCurve> bsplc2 = occ::down_cast<Geom_BSplineCurve>(ac2);
  if (bsplc1.IsNull() || bsplc2.IsNull())
    return 1;
  gp_Pnt pmid = 0.5 * (bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ());
  bsplc1->SetPole(bsplc1->NbPoles(), pmid);
  bsplc2->SetPole(1, pmid);
  GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
  if (!connect3d.Add(bsplc2, Precision::Confusion(), After, false))
    return 1;
  BRepBuilderAPI_MakeEdge MkEdge(connect3d.BSplineCurve());
  if (MkEdge.IsDone())
  {
    TopoDS_Edge nedge = MkEdge.Edge();
    DBRep::Set(argv[1], nedge);
    return 0;
  }
  else
    return 1;
}

static int OCC49(Draw_Interpretor& di, int argc, const char** argv)
{

  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " name\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull())
    return 0;

  GProp_GProps G;
  BRepGProp::VolumeProperties(S, G);
  GProp_PrincipalProps Pr     = G.PrincipalProperties();
  bool                 Result = Pr.HasSymmetryAxis();
  if (Result)
  {
    di << "1\n";
  }
  else
  {
    di << "0\n";
  }
  return 0;
}

static int OCC405(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " edge_result edge1 edge2; merge two edges\n";
    return 1;
  }

  TopoDS_Shape Sh1 = DBRep::Get(argv[2]);
  TopoDS_Shape Sh2 = DBRep::Get(argv[3]);
  if (Sh1.IsNull() || Sh2.IsNull())
    return 1;
  if (Sh1.ShapeType() != TopAbs_EDGE || Sh2.ShapeType() != TopAbs_EDGE)
    return 1;
  TopoDS_Edge             e1 = TopoDS::Edge(Sh1);
  TopoDS_Edge             e2 = TopoDS::Edge(Sh2);
  double                  f1, l1, f2, l2;
  bool                    After = true;
  occ::handle<Geom_Curve> ac1   = BRep_Tool::Curve(e1, f1, l1);
  occ::handle<Geom_Curve> ac2   = BRep_Tool::Curve(e2, f2, l2);
  if (e1.Orientation() == TopAbs_REVERSED)
  {
    double cf = f1;
    f1        = ac1->ReversedParameter(l1);
    l1        = ac1->ReversedParameter(cf);
    ac1       = ac1->Reversed();
  }
  if (e2.Orientation() == TopAbs_REVERSED)
  {
    double cf = f2;
    f2        = ac2->ReversedParameter(l2);
    l2        = ac2->ReversedParameter(cf);
    ac2       = ac2->Reversed();
  }
  occ::handle<Geom_BSplineCurve> bsplc1 = occ::down_cast<Geom_BSplineCurve>(ac1);
  occ::handle<Geom_BSplineCurve> bsplc2 = occ::down_cast<Geom_BSplineCurve>(ac2);
  if (bsplc1.IsNull() || bsplc2.IsNull())
    return 1;
  if (bsplc1->FirstParameter() < f1 - Precision::PConfusion()
      || bsplc1->LastParameter() > l1 + Precision::PConfusion())
  {
    occ::handle<Geom_BSplineCurve> aBstmp = occ::down_cast<Geom_BSplineCurve>(bsplc1->Copy());
    aBstmp->Segment(f1, l1);
    bsplc1 = aBstmp;
  }
  if (bsplc2->FirstParameter() < f2 - Precision::PConfusion()
      || bsplc2->LastParameter() > l2 + Precision::PConfusion())
  {
    occ::handle<Geom_BSplineCurve> aBstmp = occ::down_cast<Geom_BSplineCurve>(bsplc2->Copy());
    aBstmp->Segment(f2, l2);
    bsplc2 = aBstmp;
  }
  gp_Pnt pmid = 0.5 * (bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ());
  bsplc1->SetPole(bsplc1->NbPoles(), pmid);
  bsplc2->SetPole(1, pmid);
  GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
  if (!connect3d.Add(bsplc2, Precision::Confusion(), After, false))
    return 1;
  BRepBuilderAPI_MakeEdge MkEdge(connect3d.BSplineCurve());
  if (MkEdge.IsDone())
  {
    TopoDS_Edge nedge = MkEdge.Edge();
    DBRep::Set(argv[1], nedge);
    return 0;
  }
  else
    return 1;
}

static int OCC395(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " edge_result edge1 edge2\n";
    return 1;
  }
  // TCollection_AsciiString fnom(a[1]);
  // bool modfic = XSDRAW::FileAndVar(a[1],a[2],a[3],"IGES",fnom,rnom,resnom);
  TopoDS_Shape Sh1 = DBRep::Get(argv[2]);
  TopoDS_Shape Sh2 = DBRep::Get(argv[3]);
  if (Sh1.IsNull() || Sh2.IsNull())
    return 1;
  if (Sh1.ShapeType() != TopAbs_EDGE || Sh2.ShapeType() != TopAbs_EDGE)
    return 1;
  TopoDS_Edge             e1 = TopoDS::Edge(Sh1);
  TopoDS_Edge             e2 = TopoDS::Edge(Sh2);
  double                  f1, l1, f2, l2;
  bool                    After = true;
  occ::handle<Geom_Curve> ac1   = BRep_Tool::Curve(e1, f1, l1);
  occ::handle<Geom_Curve> ac2   = BRep_Tool::Curve(e2, f2, l2);
  if (e1.Orientation() == TopAbs_REVERSED)
  {
    // double cf = cf1;
    // cf1 = ac1->ReversedParameter ( cl1 );
    // cl1 = ac1->ReversedParameter ( cf );
    ac1 = ac1->Reversed();
  }
  if (e2.Orientation() == TopAbs_REVERSED)
  {
    // double cf = cf2;
    // ac2 = ac2->ReversedParameter ( cl2 );
    // ac2 = ac2->ReversedParameter ( cf );
    ac2 = ac2->Reversed();
  }
  occ::handle<Geom_BSplineCurve> bsplc1 = occ::down_cast<Geom_BSplineCurve>(ac1);
  occ::handle<Geom_BSplineCurve> bsplc2 = occ::down_cast<Geom_BSplineCurve>(ac2);
  if (bsplc1.IsNull() || bsplc2.IsNull())
    return 1;
  gp_Pnt pmid = 0.5 * (bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ());
  bsplc1->SetPole(bsplc1->NbPoles(), pmid);
  bsplc2->SetPole(1, pmid);
  GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
  if (!connect3d.Add(bsplc2, Precision::Confusion(), After, false))
    return 1;
  BRepBuilderAPI_MakeEdge MkEdge(connect3d.BSplineCurve());
  if (MkEdge.IsDone())
  {
    TopoDS_Edge nedge = MkEdge.Edge();
    DBRep::Set(argv[1], nedge);
    return 0;
  }
  else
    return 1;
}

static int OCC394(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 3)
  {
    di << "Usage : " << argv[0] << " edge_result edge [tol [mode [tolang]]]\n";
    return 1;
  }
  TopoDS_Shape Sh = DBRep::Get(argv[2]);

  int    k      = 3;
  double tol    = 100000;
  int    mode   = 2;
  double tolang = M_PI / 2;
  if (argc > k)
    tol = Draw::Atof(argv[k++]);

  if (argc > k)
    mode = Draw::Atoi(argv[k++]);

  if (argc > k)
    tolang = Draw::Atof(argv[k++]);

  occ::handle<ShapeFix_Wireframe> aSfwr    = new ShapeFix_Wireframe();
  occ::handle<ShapeBuild_ReShape> aReShape = new ShapeBuild_ReShape;
  aSfwr->SetContext(aReShape);
  aSfwr->Load(Sh);
  aSfwr->SetPrecision(tol);
  bool aModeDrop = true;
  if (mode == 2)
    aModeDrop = false;

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theSmallEdges, theMultyEdges;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    theEdgeToFaces, theFaceWithSmall;
  aSfwr->CheckSmallEdges(theSmallEdges, theEdgeToFaces, theFaceWithSmall, theMultyEdges);
  aSfwr->MergeSmallEdges(theSmallEdges,
                         theEdgeToFaces,
                         theFaceWithSmall,
                         theMultyEdges,
                         aModeDrop,
                         tolang);
  // aSfwr->FixSmallEdges();
  TopoDS_Shape resShape = aSfwr->Shape();
  DBRep::Set(argv[1], resShape);
  return 0;
}

static int OCC301(Draw_Interpretor& di, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> context = ViewerTest::GetAISContext();
  if (context.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " ArcRadius ArrowSize\n";
    return 1;
  }

  double aRadius     = Draw::Atof(argv[1]);
  double anArrowSize = Draw::Atof(argv[2]);

  gp_Pnt p1 = gp_Pnt(10., 10., 0.);
  gp_Pnt p2 = gp_Pnt(50., 10., 0.);
  gp_Pnt p3 = gp_Pnt(50., 50., 0.);

  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(p1, p2);
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(p2, p3);

  context->Display(new AIS_Shape(E1), false);
  context->Display(new AIS_Shape(E2), true);

  gp_Pnt                  plnpt(0, 0, 0);
  gp_Dir                  plndir(gp_Dir::D::Z);
  occ::handle<Geom_Plane> pln = new Geom_Plane(plnpt, plndir);

  occ::handle<PrsDim_AngleDimension> anAngleDimension =
    new PrsDim_AngleDimension(p1.Mirrored(p2), p2, p3);

  occ::handle<Prs3d_DimensionAspect> anAspect = new Prs3d_DimensionAspect;
  anAspect->MakeArrows3d(true);
  anAspect->ArrowAspect()->SetLength(anArrowSize);
  anAspect->SetTextHorizontalPosition(Prs3d_DTHP_Right);
  anAspect->TextAspect()->SetColor(Quantity_NOC_YELLOW);
  anAngleDimension->SetDimensionAspect(anAspect);
  // Another position of dimension
  anAngleDimension->SetFlyout(aRadius);
  context->Display(anAngleDimension, false);
  return 0;
}

static int OCC261(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " Doc\n";
    return 1;
  }

  occ::handle<TDocStd_Document> Doc;
  if (DDocStd::GetDocument(argv[1], Doc))
  {
    Doc->ClearRedos();
    return 0;
  }
  else
    return 1;
}

#include <OSD_File.hxx>

static int OCC710(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage : " << argv[0] << " path\n";
  }

  TCollection_AsciiString in(argv[1]);
  OSD_File*               aFile    = new OSD_File(in);
  bool                    anExists = aFile->Exists();
  if (anExists)
    di << "1\n";
  else
    di << "0\n";
  return 0;
}

#include <ShapeFix_Shell.hxx>
#include <AIS_InteractiveObject.hxx>
class AIS_InteractiveObject;

static int OCC904(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    di << "Usage : " << argv[0] << " result shape nonmanifoldmode(0/1)\n";
  }
  TopoDS_Shape S = DBRep::Get(argv[2]);
  if (S.IsNull() || S.ShapeType() != TopAbs_SHELL)
  {
    di << " Shape is null\n";
    return 1;
  }
  bool                        nonmanifmode = (Draw::Atoi(argv[3]) != 0);
  occ::handle<ShapeFix_Shell> SFSh         = new ShapeFix_Shell;
  SFSh->FixFaceOrientation(TopoDS::Shell(S), true, nonmanifmode);
  DBRep::Set(argv[1], SFSh->Shape());
  return 0;
}

void QABugs::Commands_16(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  theCommands.Add("BUC60848", "BUC60848 shape", __FILE__, BUC60848, group);
  theCommands.Add("BUC60814", "BUC60814", __FILE__, BUC60814, group);
  theCommands.Add("BUC60774", "BUC60774", __FILE__, BUC60774, group);
  theCommands.Add("BUC60972", "BUC60972 edge edge plane val text ", __FILE__, BUC60972, group);
  theCommands.Add("OCC218", "OCC218 name plane Xlabel Ylabel", __FILE__, OCC218bug, group);
  theCommands.Add("OCC295", "OCC295 edge_result edge1 edge2", __FILE__, OCC295, group);
  theCommands.Add("OCC49", "OCC49 name", __FILE__, OCC49, group);
  theCommands.Add("OCC405",
                  "OCC405 edge_result edge1 edge2; merge two edges",
                  __FILE__,
                  OCC405,
                  group);
  theCommands.Add("OCC395", "OCC395 edge_result edge1 edge2", __FILE__, OCC395, group);
  theCommands.Add("OCC394",
                  "OCC394 edge_result edge [tol [mode [tolang]]]",
                  __FILE__,
                  OCC394,
                  group);
  theCommands.Add("OCC301", "OCC301 ArcRadius ArrowSize", __FILE__, OCC301, group);
  theCommands.Add("OCC261", "OCC261 Doc", __FILE__, OCC261, group);
  theCommands.Add("OCC710", "OCC710 path", __FILE__, OCC710, group);
  theCommands.Add("OCC904", "OCC904 result shape nonmanifoldmode(0/1)", __FILE__, OCC904, group);

  return;
}
