// Created on: 2002-05-21
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

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Printer.hxx>
#include <DrawTrSurf.hxx>
#include <GC_MakeSegment2d.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomFill_Trihedron.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Quaternion.hxx>
#include <Message_Messenger.hxx>
#include <Message_PrinterOStream.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_LinearVector.hxx>
#include <OSD_PerfMeter.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Prs3d_Text.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <ViewerTest.hxx>
#include <XmlDrivers_DocumentRetrievalDriver.hxx>
#include <XmlDrivers_DocumentStorageDriver.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <DDocStd.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <Draw.hxx>
#include <GeomInt_IntSS.hxx>
#include <NCollection_Sequence.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Extrema_FuncPSNorm.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#ifdef HAVE_TBB
Standard_DISABLE_DEPRECATION_WARNINGS
  #include <tbb/parallel_for.h>
  #include <tbb/parallel_for_each.h>
  #include <tbb/blocked_range.h>
  Standard_ENABLE_DEPRECATION_WARNINGS
#endif

#include <atomic>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <random>

#define QCOMPARE(val1, val2)                                                                       \
  di << "Checking " #val1 " == " #val2 << ((val1) == (val2) ? ": OK\n" : ": Error\n")

#define QVERIFY(val)                                                                               \
  if (!(val))                                                                                      \
  {                                                                                                \
    std::cout << "Error: " #val " is false\n";                                                     \
  }

#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>

  static bool
  OCC23774Test(const TopoDS_Face&  grossPlateFace,
               const TopoDS_Shape& originalWire,
               Draw_Interpretor&   di)
{
  BRepExtrema_DistShapeShape distShapeShape(grossPlateFace, originalWire, Extrema_ExtFlag_MIN);
  if (!distShapeShape.IsDone())
  {
    di << "Distance ShapeShape is Not Done\n";
    return false;
  }

  if (distShapeShape.Value() > 0.01)
  {
    di << "Wrong Dist = " << distShapeShape.Value() << "\n";
    return false;
  }
  else
    di << "Dist0 = " << distShapeShape.Value() << "\n";

  //////////////////////////////////////////////////////////////////////////
  /// First Flip Y
  const gp_Pnt2d axis1P1(1474.8199035519228, 1249.9995745636970);
  const gp_Pnt2d axis1P2(1474.8199035519228, 1250.9995745636970);

  gp_Vec2d mirrorVector1(axis1P1, axis1P2);

  gp_Trsf2d mirror1;
  mirror1.SetMirror(gp_Ax2d(axis1P1, mirrorVector1));

  BRepBuilderAPI_Transform transformer1(mirror1);
  transformer1.Perform(originalWire);
  if (!transformer1.IsDone())
  {
    di << "Not Done1 \n";
    return false;
  }
  TopoDS_Shape step1ModifiedShape = transformer1.ModifiedShape(originalWire);

  BRepExtrema_DistShapeShape distShapeShape1(grossPlateFace,
                                             step1ModifiedShape,
                                             Extrema_ExtFlag_MIN);
  if (!distShapeShape1.IsDone())
    return false;
  if (distShapeShape1.Value() > 0.01)
  {
    di << "Dist = " << distShapeShape1.Value() << "\n";
    return false;
  }
  else
    di << "Dist1 = " << distShapeShape1.Value() << "\n";

  //////////////////////////////////////////////////////////////////////////
  /// Second flip Y
  transformer1.Perform(step1ModifiedShape);
  if (!transformer1.IsDone())
  {
    di << "Not Done1 \n";
    return false;
  }
  TopoDS_Shape step2ModifiedShape = transformer1.ModifiedShape(step1ModifiedShape);

  // This is identity matrix for values but for type is gp_Rotation ?!
  gp_Trsf2d mirror11 = mirror1;
  mirror11.PreMultiply(mirror1);

  // clang-format off
  BRepExtrema_DistShapeShape distShapeShape2(grossPlateFace,step2ModifiedShape);//,Extrema_ExtFlag_MIN);
  // clang-format on
  if (!distShapeShape2.IsDone())
    return false;

  // This last test case give error (the value is 1008.8822038689706)
  if (distShapeShape2.Value() > 0.01)
  {
    di << "Wrong Dist2 = " << distShapeShape2.Value() << "\n";
    int N = distShapeShape2.NbSolution();
    di << "Nb = " << N << "\n";
    for (int i = 1; i <= N; i++)
      di << "Sol(" << i
         << ") = " << distShapeShape2.PointOnShape1(i).Distance(distShapeShape2.PointOnShape2(i))
         << "\n";
    return false;
  }
  di << "Distance2 = " << distShapeShape2.Value() << "\n";

  return true;
}

static int OCC23774(Draw_Interpretor& di, int n, const char** a)
{

  if (n != 3)
  {
    di << "OCC23774: invalid number of input parameters\n";
    return 1;
  }

  const char * ns1 = (a[1]), *ns2 = (a[2]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2));
  if (S1.IsNull() || S2.IsNull())
  {
    di << "OCC23774: Null input shapes\n";
    return 1;
  }
  const TopoDS_Face& aFace = TopoDS::Face(S1);
  if (!OCC23774Test(aFace, S2, di))
    di << "Something is wrong\n";

  return 0;
}

#include <GeomConvert_ApproxSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <OSD_Thread.hxx>

struct GeomConvertTest_Data
{
  GeomConvertTest_Data()
      : nbupoles(0)
  {
  }

  int                       nbupoles;
  occ::handle<Geom_Surface> surf;
};

static void* GeomConvertTest(void* data)
{
  GeomConvertTest_Data* info = (GeomConvertTest_Data*)data;

  GeomConvert_ApproxSurface aGAS(info->surf, 1e-4, GeomAbs_C1, GeomAbs_C1, 9, 9, 100, 1);
  if (!aGAS.IsDone())
  {
    std::cout << "Error: ApproxSurface is not done!" << std::endl;
    return nullptr;
  }
  const occ::handle<Geom_BSplineSurface>& aBSurf = aGAS.Surface();
  if (aBSurf.IsNull())
  {
    std::cout << "Error: BSplineSurface is not created!" << std::endl;
    return nullptr;
  }
  std::cout << "Number of UPoles:" << aBSurf->NbUPoles();
  if (aBSurf->NbUPoles() == info->nbupoles)
  {
    std::cout << ": OK" << std::endl;
    return data; // any non-null pointer
  }
  else
  {
    std::cout << ": Error, must be " << info->nbupoles << std::endl;
    return nullptr;
  }
}

static int OCC23952sweep(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    std::cout << "Error: invalid number of arguments" << std::endl;
    return 1;
  }

  struct GeomConvertTest_Data aStorage;
  aStorage.nbupoles = Draw::Atoi(argv[1]);
  aStorage.surf     = DrawTrSurf::GetSurface(argv[2]);
  if (aStorage.surf.IsNull())
  {
    std::cout << "Error: " << argv[2] << " is not a DRAW surface!" << std::endl;
    return 0;
  }

  // start conversion in several threads
  const int  NBTHREADS = 100;
  OSD_Thread aThread[NBTHREADS];
  for (int i = 0; i < NBTHREADS; i++)
  {
    aThread[i].SetFunction(GeomConvertTest);
    if (!aThread[i].Run(&aStorage))
      di << "Error: Cannot start thread << " << i << "\n";
  }

  // check results
  for (int i = 0; i < NBTHREADS; i++)
  {
    void* aResult = nullptr;
    if (!aThread[i].Wait(aResult))
      di << "Error: Failed waiting for thread << " << i << "\n";
    if (!aResult)
      di << "Error: wrong number of poles in thread " << i << "!\n";
  }

  return 0;
}

struct GeomIntSSTest_Data
{
  GeomIntSSTest_Data()
      : nbsol(0)
  {
  }

  int                       nbsol;
  occ::handle<Geom_Surface> surf1, surf2;
};

static void* GeomIntSSTest(void* data)
{
  GeomIntSSTest_Data* info = (GeomIntSSTest_Data*)data;
  GeomInt_IntSS       anInter;
  anInter.Perform(info->surf1, info->surf2, Precision::Confusion(), true);
  if (!anInter.IsDone())
  {
    std::cout << "An intersection is not done!" << std::endl;
    return nullptr;
  }

  std::cout << "Number of Lines:" << anInter.NbLines();
  if (anInter.NbLines() == info->nbsol)
  {
    std::cout << ": OK" << std::endl;
    return data; // any non-null pointer
  }
  else
  {
    std::cout << ": Error, must be " << info->nbsol << std::endl;
    return nullptr;
  }
}

static int OCC23952intersect(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 4)
  {
    std::cout << "Error: invalid number of arguments" << std::endl;
    return 1;
  }

  struct GeomIntSSTest_Data aStorage;
  aStorage.nbsol = Draw::Atoi(argv[1]);
  aStorage.surf1 = DrawTrSurf::GetSurface(argv[2]);
  aStorage.surf2 = DrawTrSurf::GetSurface(argv[3]);
  if (aStorage.surf1.IsNull() || aStorage.surf2.IsNull())
  {
    std::cout << "Error: Either " << argv[2] << " or " << argv[3] << " is not a DRAW surface!"
              << std::endl;
    return 0;
  }

  // start conversion in several threads
  const int  NBTHREADS = 100;
  OSD_Thread aThread[NBTHREADS];
  for (int i = 0; i < NBTHREADS; i++)
  {
    aThread[i].SetFunction(GeomIntSSTest);
    if (!aThread[i].Run(&aStorage))
      di << "Error: Cannot start thread << " << i << "\n";
  }

  // check results
  for (int i = 0; i < NBTHREADS; i++)
  {
    void* aResult = nullptr;
    if (!aThread[i].Wait(aResult))
      di << "Error: Failed waiting for thread << " << i << "\n";
    if (!aResult)
      di << "Error: wrong number of intersections in thread " << i << "!\n";
  }

  return 0;
}

#include <Geom_SurfaceOfRevolution.hxx>

static int OCC23683(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }

  int  ucontinuity = 1;
  int  vcontinuity = 1;
  bool iscnu       = false;
  bool iscnv       = false;

  occ::handle<Geom_Surface> aSurf = DrawTrSurf::GetSurface(argv[1]);

  QCOMPARE(aSurf->IsCNu(ucontinuity), iscnu);
  QCOMPARE(aSurf->IsCNv(vcontinuity), iscnv);

  return 0;
}

#include <Geom_Plane.hxx>
#include <Geom2d_Circle.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <Geom2d_OffsetCurve.hxx>

static int test_offset(Draw_Interpretor& di, int argc, const char** argv)
{
  // Check the command arguments
  if (argc != 1)
  {
    di << "Error: " << argv[0] << " - invalid number of arguments\n";
    di << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  gp_Ax1                    RotoAx(gp::Origin(), gp::DZ());
  gp_Ax22d                  Ax2(gp::Origin2d(), gp::DY2d(), gp::DX2d());
  occ::handle<Geom_Surface> Plane = new Geom_Plane(gp::YOZ());

  di << "<<<< Preparing sample surface of revolution based on trimmed curve >>>>\n";
  di << "-----------------------------------------------------------------------\n";

  occ::handle<Geom2d_Circle>       C2d1        = new Geom2d_Circle(Ax2, 1.0);
  occ::handle<Geom2d_TrimmedCurve> C2d1Trimmed = new Geom2d_TrimmedCurve(C2d1, 0.0, M_PI / 2.0);
  TopoDS_Edge                      E1          = BRepBuilderAPI_MakeEdge(C2d1Trimmed, Plane);

  DBRep::Set("e1", E1);

  BRepPrimAPI_MakeRevol aRevolBuilder1(E1, RotoAx);
  TopoDS_Face           F1 = TopoDS::Face(aRevolBuilder1.Shape());

  DBRep::Set("f1", F1);

  di << "Result: f1\n";

  di << "<<<< Preparing sample surface of revolution based on offset curve  >>>>\n";
  di << "-----------------------------------------------------------------------\n";

  occ::handle<Geom2d_OffsetCurve> C2d2Offset = new Geom2d_OffsetCurve(C2d1Trimmed, -0.5);
  TopoDS_Edge                     E2         = BRepBuilderAPI_MakeEdge(C2d2Offset, Plane);

  DBRep::Set("e2", E2);

  BRepPrimAPI_MakeRevol aRevolBuilder2(E2, RotoAx);
  TopoDS_Face           F2 = TopoDS::Face(aRevolBuilder2.Shape());

  DBRep::Set("f2", F2);

  di << "Result: f2\n";

  return 0;
}

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>

//=================================================================================================

static int OCC24008(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }
  occ::handle<Geom_Curve>   aCurve = DrawTrSurf::GetCurve(argv[1]);
  occ::handle<Geom_Surface> aSurf  = DrawTrSurf::GetSurface(argv[2]);
  if (aCurve.IsNull())
  {
    di << "Curve was not read\n";
    return 1;
  }
  if (aSurf.IsNull())
  {
    di << "Surface was not read\n";
    return 1;
  }
  ShapeConstruct_ProjectCurveOnSurface aProj;
  aProj.Init(aSurf, Precision::Confusion());
  try
  {
    occ::handle<Geom2d_Curve> aPCurve;
    aProj.Perform(aCurve, aCurve->FirstParameter(), aCurve->LastParameter(), aPCurve);
    if (aPCurve.IsNull())
    {
      di << "PCurve was not created\n";
      return 1;
    }
  }
  catch (...)
  {
    di << "Exception was caught\n";
  }
  return 0;
}

//=================================================================================================

static int OCC23945(Draw_Interpretor& /*di*/, int n, const char** a)
{
  if (n < 5)
    return 1;

  occ::handle<Geom_Surface> aS = DrawTrSurf::GetSurface(a[1]);
  if (aS.IsNull())
    return 1;

  GeomAdaptor_Surface GS(aS);

  double U = Draw::Atof(a[2]);
  double V = Draw::Atof(a[3]);

  bool DrawPoint = (n % 3 == 2);
  if (DrawPoint)
    n--;

  gp_Pnt P;
  if (n >= 13)
  {
    gp_Vec DU, DV;
    if (n >= 22)
    {
      gp_Vec D2U, D2V, D2UV;
      GS.D2(U, V, P, DU, DV, D2U, D2V, D2UV);
      Draw::Set(a[13], D2U.X());
      Draw::Set(a[14], D2U.Y());
      Draw::Set(a[15], D2U.Z());
      Draw::Set(a[16], D2V.X());
      Draw::Set(a[17], D2V.Y());
      Draw::Set(a[18], D2V.Z());
      Draw::Set(a[19], D2UV.X());
      Draw::Set(a[20], D2UV.Y());
      Draw::Set(a[21], D2UV.Z());
    }
    else
      GS.D1(U, V, P, DU, DV);

    Draw::Set(a[7], DU.X());
    Draw::Set(a[8], DU.Y());
    Draw::Set(a[9], DU.Z());
    Draw::Set(a[10], DV.X());
    Draw::Set(a[11], DV.Y());
    Draw::Set(a[12], DV.Z());
  }
  else
    GS.D0(U, V, P);

  if (n > 6)
  {
    Draw::Set(a[4], P.X());
    Draw::Set(a[5], P.Y());
    Draw::Set(a[6], P.Z());
  }
  if (DrawPoint)
  {
    DrawTrSurf::Set(a[n], P);
  }

  return 0;
}

#include <BRepFeat_SplitShape.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <BRepAlgo.hxx>

static int OCC24086(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 3)
  {
    di << "Usage : " << argv[0] << " should be 2 arguments (face and wire)";
    return 1;
  }

  occ::handle<AIS_InteractiveContext> myAISContext = ViewerTest::GetAISContext();
  if (myAISContext.IsNull())
  {
    di << "use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  TopoDS_Shape result;
  TopoDS_Face  face = TopoDS::Face(DBRep::Get(argv[1]));
  TopoDS_Wire  wire = TopoDS::Wire(DBRep::Get(argv[2]));

  BRepFeat_SplitShape asplit(face);
  asplit.Add(wire, face);
  asplit.Build();
  result = asplit.Shape();
  ShapeAnalysis_ShapeContents ana;
  ana.Perform(result);
  ana.NbFaces();

  if (!(BRepAlgo::IsValid(result)))
  {
    di << "Result was checked and it is INVALID\n";
  }
  else
  {
    di << "Result was checked and it is VALID\n";
  }

  occ::handle<AIS_InteractiveObject> myShape = new AIS_Shape(result);
  myAISContext->Display(myShape, true);

  return 0;
}

#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>

static int OCC24137(Draw_Interpretor& theDI, int theNArg, const char** theArgv)
{
  int anArgIter = 1;
  if (theNArg < 5)
  {
    theDI << "Usage: " << theArgv[0] << " face vertex U V [N]\n";
    return 1;
  }

  // get target shape
  const char*        aFaceName = theArgv[anArgIter++];
  const char*        aVertName = theArgv[anArgIter++];
  const TopoDS_Shape aShapeF   = DBRep::Get(aFaceName);
  const TopoDS_Shape aShapeV   = DBRep::Get(aVertName);
  const double       aUFrom    = Atof(theArgv[anArgIter++]);
  const double       aVFrom    = Atof(theArgv[anArgIter++]);
  const int          aNbIts    = (anArgIter < theNArg) ? Draw::Atoi(theArgv[anArgIter++]) : 100;
  if (aShapeF.IsNull() || aShapeF.ShapeType() != TopAbs_FACE)
  {
    std::cout << "Error: " << aFaceName << " shape is null / not a face" << std::endl;
    return 1;
  }
  if (aShapeV.IsNull() || aShapeV.ShapeType() != TopAbs_VERTEX)
  {
    std::cout << "Error: " << aVertName << " shape is null / not a vertex" << std::endl;
    return 1;
  }
  const TopoDS_Face   aFace = TopoDS::Face(aShapeF);
  const TopoDS_Vertex aVert = TopoDS::Vertex(aShapeV);
  GeomAdaptor_Surface aSurf(BRep_Tool::Surface(aFace));

  gp_Pnt aPnt = BRep_Tool::Pnt(aVert), aRes;

  Extrema_FuncPSNorm   anExtFunc;
  math_FunctionSetRoot aRoot(anExtFunc, aNbIts);

  math_Vector aTolUV(1, 2), aUVinf(1, 2), aUVsup(1, 2), aFromUV(1, 2);
  aTolUV(1)  = Precision::Confusion();
  aTolUV(2)  = Precision::Confusion();
  aUVinf(1)  = -Precision::Infinite();
  aUVinf(2)  = -Precision::Infinite();
  aUVsup(1)  = Precision::Infinite();
  aUVsup(2)  = Precision::Infinite();
  aFromUV(1) = aUFrom;
  aFromUV(2) = aVFrom;

  anExtFunc.Initialize(aSurf);
  anExtFunc.SetPoint(aPnt);
  aRoot.SetTolerance(aTolUV);
  aRoot.Perform(anExtFunc, aFromUV, aUVinf, aUVsup);
  if (!aRoot.IsDone())
  {
    std::cerr << "No results!\n";
    return 1;
  }

  theDI << aRoot.Root()(1) << " " << aRoot.Root()(2) << "\n";

  aSurf.D0(aRoot.Root()(1), aRoot.Root()(2), aRes);
  DBRep::Set("result", BRepBuilderAPI_MakeVertex(aRes));
  return 0;
}

#include <ShapeFix_EdgeProjAux.hxx>

static int OCC24370(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 5)
  {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }

  TopoDS_Shape aSh = DBRep::Get(argv[1]);
  if (aSh.IsNull())
  {
    di << argv[0] << " Error: Null input edge\n";
    return 1;
  }
  const TopoDS_Edge& anEdge = TopoDS::Edge(aSh);

  occ::handle<Geom2d_Curve> aC = DrawTrSurf::GetCurve2d(argv[2]);
  if (aC.IsNull())
  {
    di << argv[0] << " Error: Null input curve\n";
    return 1;
  }

  occ::handle<Geom_Surface> aS = DrawTrSurf::GetSurface(argv[3]);
  if (aS.IsNull())
  {
    di << argv[0] << " Error: Null input surface\n";
    return 1;
  }

  double prec = Draw::Atof(argv[4]);

  // prepare data
  TopoDS_Face  aFace;
  BRep_Builder aB;
  aB.MakeFace(aFace, aS, Precision::Confusion());
  aB.UpdateEdge(anEdge, aC, aFace, Precision::Confusion());
  aB.Range(anEdge, aFace, aC->FirstParameter(), aC->LastParameter());

  // call algorithm
  ShapeFix_EdgeProjAux aProj(aFace, anEdge);
  aProj.Compute(prec);

  bool isfirstdone = aProj.IsFirstDone();
  bool islastdone  = aProj.IsLastDone();

  double first              = 0.;
  double last               = 0.;
  int    isfirstdoneInteger = 0;
  int    islastdoneInteger  = 0;

  if (isfirstdone)
  {
    first              = aProj.FirstParam();
    isfirstdoneInteger = 1;
  }

  if (islastdone)
  {
    last              = aProj.LastParam();
    islastdoneInteger = 1;
  }

  di << isfirstdoneInteger << " " << islastdoneInteger << " " << first << " " << last << " \n";

  return 0;
}

// Dummy class to test interface for compilation issues
class QABugs_HandleClass : public Standard_Transient
{
public:
  int HandleProc(Draw_Interpretor&, int, const char** theArgVec)
  {
    std::cerr << "QABugs_HandleClass[" << this << "] " << theArgVec[0] << "\n";
    return 0;
  }
  DEFINE_STANDARD_RTTI_INLINE(QABugs_HandleClass, Standard_Transient) // Type definition
};

// Dummy class to test interface for compilation issues
struct QABugs_NHandleClass
{
  int NHandleProc(Draw_Interpretor&, int, const char** theArgVec)
  {
    std::cerr << "QABugs_NHandleClass[" << this << "] " << theArgVec[0] << "\n";
    return 0;
  }
};

static int OCC24667(Draw_Interpretor& di, int n, const char** a)
{
  if (n == 1)
  {
    di << "OCC24667 result Wire_spine Profile [Mode [Approx]]\n";
    di << "Mode = 0 - CorrectedFrenet,\n";
    di << "     = 1 - Frenet,\n";
    di << "     = 2 - DiscreteTrihedron\n";
    di << "Approx - force C1-approximation if result is C0\n";
    return 0;
  }

  if (n > 1 && n < 4)
    return 1;

  TopoDS_Shape Spine = DBRep::Get(a[2], TopAbs_WIRE);
  if (Spine.IsNull())
    return 1;

  TopoDS_Shape Profile = DBRep::Get(a[3]);
  if (Profile.IsNull())
    return 1;

  GeomFill_Trihedron Mode = GeomFill_IsCorrectedFrenet;
  if (n >= 5)
  {
    int iMode = atoi(a[4]);
    if (iMode == 1)
      Mode = GeomFill_IsFrenet;
    else if (iMode == 2)
      Mode = GeomFill_IsDiscreteTrihedron;
  }

  bool ForceApproxC1 = false;
  if (n >= 6)
    ForceApproxC1 = true;

  BRepOffsetAPI_MakePipe aPipe(TopoDS::Wire(Spine), Profile, Mode, ForceApproxC1);

  TopoDS_Shape S   = aPipe.Shape();
  TopoDS_Shape aSF = aPipe.FirstShape();
  TopoDS_Shape aSL = aPipe.LastShape();

  DBRep::Set(a[1], S);

  TCollection_AsciiString aStrF(a[1], "_f");
  TCollection_AsciiString aStrL(a[1], "_l");

  DBRep::Set(aStrF.ToCString(), aSF);
  DBRep::Set(aStrL.ToCString(), aSL);

  return 0;
}

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_NurbsConvertModification.hxx>

static TopoDS_Shape CreateTestShape(int& theShapeNb)
{
  TopoDS_Compound aComp;
  BRep_Builder    aBuilder;
  aBuilder.MakeCompound(aComp);
  // NURBS modifier is used to increase footprint of each shape
  occ::handle<BRepTools_NurbsConvertModification> aNurbsModif =
    new BRepTools_NurbsConvertModification;
  TopoDS_Shape       aRefShape = BRepPrimAPI_MakeCylinder(50., 100.).Solid();
  BRepTools_Modifier aModifier(aRefShape, aNurbsModif);
  if (aModifier.IsDone())
  {
    aRefShape = aModifier.ModifiedShape(aRefShape);
  }
  int aSiblingNb = 0;
  for (; theShapeNb > 0; --theShapeNb)
  {
    TopoDS_Shape aShape;
    if (++aSiblingNb <= 100)
    { // number of siblings is limited to avoid long lists
      aShape = BRepBuilderAPI_Copy(aRefShape, true /*CopyGeom*/).Shape();
    }
    else
    {
      aShape = CreateTestShape(theShapeNb);
    }
    aBuilder.Add(aComp, aShape);
  }
  return aComp;
}

#include <TDataStd_Integer.hxx>
#include <TNaming_Builder.hxx>

static int OCC24931(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc != 2)
  {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }
  TCollection_ExtendedString aFileName(argv[1]);
  PCDM_StoreStatus           aSStatus = PCDM_SS_Failure;

  occ::handle<TDocStd_Application> anApp = DDocStd::GetApplication();
  {
    occ::handle<TDocStd_Document> aDoc;
    anApp->NewDocument("XmlOcaf", aDoc);
    TDF_Label aLab = aDoc->Main();
    TDataStd_Integer::Set(aLab, 0);
    int             n      = 10000; // must be big enough
    TopoDS_Shape    aShape = CreateTestShape(n);
    TNaming_Builder aBuilder(aLab);
    aBuilder.Generated(aShape);

    aSStatus = anApp->SaveAs(aDoc, aFileName);
    anApp->Close(aDoc);
  }
  QCOMPARE(aSStatus, PCDM_SS_OK);
  return 0;
}

struct MyStubObject
{
  MyStubObject()
      : ptr(nullptr)
  {
  }

  MyStubObject(void* thePtr)
      : ptr(thePtr)
  {
  }

  char  overhead[40];
  void* ptr;
};

//=================================================================================================

static int OCC24834(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 1)
  {
    std::cout << "Usage : " << a[0] << "\n";
    return 1;
  }

  int i = sizeof(char*);
  if (i > 4)
  {
    std::cout << "64-bit architecture is not supported.\n";
    return 0;
  }

  NCollection_List<MyStubObject> aList;
  const int                      aSmallBlockSize = 40;
  const int                      aLargeBlockSize = 1500000;

  // quick populate memory with large blocks
  try
  {
    for (;;)
    {
      aList.Append(MyStubObject(Standard::Allocate(aLargeBlockSize)));
    }
  }
  catch (Standard_Failure const&)
  {
    di << "caught out of memory for large blocks: OK\n";
  }
  catch (...)
  {
    di << "skipped out of memory for large blocks: Error\n";
  }

  // allocate small blocks
  try
  {
    for (;;)
    {
      aList.Append(MyStubObject(Standard::Allocate(aSmallBlockSize)));
    }
  }
  catch (Standard_Failure const&)
  {
    di << "caught out of memory for small blocks: OK\n";
  }
  catch (...)
  {
    di << "skipped out of memory for small blocks: Error\n";
  }

  // release all allocated blocks
  for (NCollection_List<MyStubObject>::Iterator it(aList); it.More(); it.Next())
  {
    Standard::Free(it.Value().ptr);
  }
  return 0;
}

#include <OSD_Environment.hxx>
#include <Resource_Manager.hxx>

#define THE_QATEST_DOC_FORMAT "My Proprietary Format"

#define QA_CHECK(theDesc, theExpr, theValue)                                                       \
  {                                                                                                \
    const bool isTrue = !!(theExpr);                                                               \
    std::cout << theDesc << (isTrue ? " TRUE  " : " FALSE ")                                       \
              << (isTrue == theValue ? " is OK\n" : " is FAIL\n");                                 \
  }

class Test_TDocStd_Application : public TDocStd_Application
{
public:
  Test_TDocStd_Application()
  {
    // explicitly initialize resource manager
    myResources = new Resource_Manager("");
    myResources->SetResource("xml.FileFormat", THE_QATEST_DOC_FORMAT);
    myResources->SetResource(THE_QATEST_DOC_FORMAT ".Description", "Test XML Document");
    myResources->SetResource(THE_QATEST_DOC_FORMAT ".FileExtension", "xml");
  }

  occ::handle<PCDM_Reader> ReaderFromFormat(const TCollection_ExtendedString&) override
  {
    return new XmlDrivers_DocumentRetrievalDriver();
  }

  occ::handle<PCDM_StorageDriver> WriterFromFormat(const TCollection_ExtendedString&) override
  {
    return new XmlDrivers_DocumentStorageDriver("Test");
  }

  const char* ResourcesName() override { return ""; }

  //! Dumps the content of me into the stream
  void DumpJson(Standard_OStream& theOStream, int theDepth) const
  {
    OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
    OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDocStd_Application)
  }
};

//=================================================================================================

static int OCC24925(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 2 && theArgNb != 5)
  {
    std::cout << "Error: wrong syntax! See usage:\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  int                        anArgIter = 1;
  TCollection_ExtendedString aFileName = theArgVec[anArgIter++];
  TCollection_AsciiString    aPlugin   = "TKXml";
  // clang-format off
  TCollection_AsciiString    aSaver    = "03a56820-8269-11d5-aab2-0050044b1af1"; // XmlStorageDriver   in XmlDrivers.cxx
  TCollection_AsciiString    aLoader   = "03a56822-8269-11d5-aab2-0050044b1af1"; // XmlRetrievalDriver in XmlDrivers.cxx
  // clang-format on
  if (anArgIter < theArgNb)
  {
    aPlugin = theArgVec[anArgIter++];
    aSaver  = theArgVec[anArgIter++];
    aLoader = theArgVec[anArgIter++];
  }

  PCDM_StoreStatus  aSStatus = PCDM_SS_Failure;
  PCDM_ReaderStatus aRStatus = PCDM_RS_OpenError;

  occ::handle<TDocStd_Application> anApp = new Test_TDocStd_Application();
  {
    occ::handle<TDocStd_Document> aDoc;
    anApp->NewDocument(THE_QATEST_DOC_FORMAT, aDoc);
    TDF_Label aLab = aDoc->Main();
    TDataStd_Integer::Set(aLab, 0);
    TDataStd_Name::Set(aLab, "QABugs_19.cxx");

    aSStatus = anApp->SaveAs(aDoc, aFileName);
    anApp->Close(aDoc);
  }
  QA_CHECK("SaveAs()", aSStatus == PCDM_SS_OK, true);

  {
    occ::handle<TDocStd_Document> aDoc;
    aRStatus = anApp->Open(aFileName, aDoc);
    anApp->Close(aDoc);
  }
  QA_CHECK("Open()  ", aRStatus == PCDM_RS_OK, true);
  return 0;
}

//=================================================================================================

#include <BRepAlgoAPI_Check.hxx>

static int OCC25043(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 2)
  {
    theDI << "Usage: " << theArgVec[0] << " shape\n";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << theArgVec[1] << " shape is NULL\n";
    return 1;
  }

  BRepAlgoAPI_Check anAlgoApiCheck(aShape, true, true);

  if (!anAlgoApiCheck.IsValid())
  {
    NCollection_List<BOPAlgo_CheckResult>::Iterator anCheckIter(anAlgoApiCheck.Result());
    for (; anCheckIter.More(); anCheckIter.Next())
    {
      const BOPAlgo_CheckResult&               aCurCheckRes     = anCheckIter.Value();
      const NCollection_List<TopoDS_Shape>&    aCurFaultyShapes = aCurCheckRes.GetFaultyShapes1();
      NCollection_List<TopoDS_Shape>::Iterator aFaultyIter(aCurFaultyShapes);
      for (; aFaultyIter.More(); aFaultyIter.Next())
      {
        const TopoDS_Shape& aFaultyShape = aFaultyIter.Value();

        bool            anIsFaultyShapeFound = false;
        TopExp_Explorer anExp(aShape, aFaultyShape.ShapeType());
        for (; anExp.More() && !anIsFaultyShapeFound; anExp.Next())
        {
          if (anExp.Current().IsEqual(aFaultyShape))
            anIsFaultyShapeFound = true;
        }

        if (!anIsFaultyShapeFound)
        {
          theDI << "Error. Faulty Shape is NOT found in source shape.\n";
          return 0;
        }
        else
        {
          theDI << "Info. Faulty shape is found in source shape\n";
        }
      }
    }
  }
  else
  {
    theDI << "Problems are not detected. Test is not performed.";
  }

  return 0;
}

//=================================================================================================

static int OCC24606(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb > 1)
  {
    std::cerr << "Error: incorrect number of arguments.\n";
    theDI << "Usage : " << theArgVec[0] << "\n";
    return 1;
  }

  occ::handle<V3d_View> aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Error: no active view, please call 'vinit'.\n";
    return 1;
  }

  aView->DepthFitAll();
  aView->FitAll();

  return 0;
}

//=================================================================================================

#include <ShapeBuild_ReShape.hxx>

static int OCC25202(Draw_Interpretor& theDI, int theArgN, const char** theArgVal)
{
  //  0      1    2     3     4     5     6
  // reshape res shape numF1 face1 numF2 face2
  if (theArgN < 7)
  {
    theDI << "Use: reshape res shape numF1 face1 numF2 face2\n";
    return 1;
  }

  TopoDS_Shape aShape    = DBRep::Get(theArgVal[2]);
  const int    aNumOfRE1 = Draw::Atoi(theArgVal[3]), aNumOfRE2 = Draw::Atoi(theArgVal[5]);
  TopoDS_Face  aShapeForRepl1 = TopoDS::Face(DBRep::Get(theArgVal[4])),
              aShapeForRepl2  = TopoDS::Face(DBRep::Get(theArgVal[6]));

  if (aShape.IsNull())
  {
    theDI << theArgVal[2] << " is null shape\n";
    return 1;
  }

  if (aShapeForRepl1.IsNull())
  {
    theDI << theArgVal[4] << " is not a replaced type\n";
    return 1;
  }

  if (aShapeForRepl2.IsNull())
  {
    theDI << theArgVal[6] << " is not a replaced type\n";
    return 1;
  }

  TopoDS_Shape       aReplacedShape;
  ShapeBuild_ReShape aReshape;

  //////////////////// explode (begin)
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
  M.Add(aShape);
  int aNbShapes = 0;
  for (TopExp_Explorer ex(aShape, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Shape& Sx    = ex.Current();
    bool                added = M.Add(Sx);
    if (added)
    {
      aNbShapes++;
      if (aNbShapes == aNumOfRE1)
      {
        aReplacedShape = Sx;

        aReshape.Replace(aReplacedShape, aShapeForRepl1);
      }

      if (aNbShapes == aNumOfRE2)
      {
        aReplacedShape = Sx;

        aReshape.Replace(aReplacedShape, aShapeForRepl2);
      }
    }
  }
  //////////////////// explode (end)

  if (aReplacedShape.IsNull())
  {
    theDI << "There is not any shape for replacing.\n";
  }

  DBRep::Set(theArgVal[1], aReshape.Apply(aShape, TopAbs_WIRE, 2));

  return 0;
}

#include <ShapeFix_Wireframe.hxx>

//=================================================================================================

static int OCC7570(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "Usage: " << a[0] << " invalid number of arguments\n";
    return 1;
  }
  TopoDS_Shape       in_shape(DBRep::Get(a[1]));
  ShapeFix_Wireframe fix_tool(in_shape);
  fix_tool.ModeDropSmallEdges() = true;
  fix_tool.SetPrecision(1.e+6);
  fix_tool.SetLimitAngle(0.01);
  fix_tool.FixSmallEdges();
  TopoDS_Shape new_shape = fix_tool.Shape();
  return 0;
}

#include <AIS_TypeFilter.hxx>

//=================================================================================================

static int OCC25340(Draw_Interpretor& /*theDI*/, int /*theArgNb*/, const char** /*theArgVec*/)
{
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cerr << "Error: No opened viewer!\n";
    return 1;
  }
  occ::handle<AIS_TypeFilter> aFilter = new AIS_TypeFilter(AIS_KindOfInteractive_Shape);
  aCtx->AddFilter(aFilter);
  return 0;
}

#include <GeomAPI_IntSS.hxx>

//=================================================================================================

static int OCC25100(Draw_Interpretor& di, int argc, const char** argv)
{
  if (argc < 2)
  {
    di << "the method requires a shape name\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if (S.IsNull())
  {
    di << "Shape is empty\n";
    return 1;
  }

  TopExp_Explorer                  aFaceExp(S, TopAbs_FACE);
  const occ::handle<Geom_Surface>& aSurf = BRep_Tool::Surface(TopoDS::Face(aFaceExp.Current()));

  GeomAPI_IntSS anIntersector(aSurf, aSurf, Precision::Confusion());

  if (!anIntersector.IsDone())
  {
    di << "Error. Intersection is not done\n";
    return 1;
  }

  di << "Test complete\n";

  return 0;
}

#include <IntCurvesFace_ShapeIntersector.hxx>
#include <BRepBndLib.hxx>

//=================================================================================================

static int OCC25413(Draw_Interpretor& di, int narg, const char** a)
{
  if (narg != 2)
  {
    di << "Usage: " << a[0] << " invalid number of arguments\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(a[1]);

  IntCurvesFace_ShapeIntersector Inter;
  Inter.Load(aShape, Precision::Confusion());

  Bnd_Box aBndBox;
  BRepBndLib::Add(aShape, aBndBox);

  gp_Dir    aDir(gp_Dir::D::Y);
  const int N     = 250;
  double    xMin  = aBndBox.CornerMin().X();
  double    zMin  = aBndBox.CornerMin().Z();
  double    xMax  = aBndBox.CornerMax().X();
  double    zMax  = aBndBox.CornerMax().Z();
  double    xStep = (xMax - xMin) / N;
  double    zStep = (zMax - zMin) / N;

  for (double x = xMin; x <= xMax; x += xStep)
    for (double z = zMin; z <= zMax; z += zStep)
    {
      gp_Pnt aPoint(x, 0.0, z);
      gp_Lin aLine(aPoint, aDir);
      Inter.PerformNearest(aLine, -100., 100.);
    }
  return 0;
}

#include <BOPAlgo_PaveFiller.hxx>
//
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
//
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>

//=================================================================================================

static int OCC25446(Draw_Interpretor& theDI, int argc, const char** argv)
{
  if (argc != 5)
  {
    theDI << "Usage: OCC25446 res b1 b2 op\n";
    return 1;
  }
  //
  TopoDS_Shape aS1 = DBRep::Get(argv[2]);
  if (aS1.IsNull())
  {
    theDI << argv[2] << " shape is NULL\n";
    return 1;
  }
  //
  TopoDS_Shape aS2 = DBRep::Get(argv[3]);
  if (aS2.IsNull())
  {
    theDI << argv[3] << " shape is NULL\n";
    return 1;
  }
  //
  int               iOp;
  BOPAlgo_Operation aOp;
  //
  iOp = Draw::Atoi(argv[4]);
  if (iOp < 0 || iOp > 4)
  {
    theDI << "Invalid operation type\n";
    return 1;
  }
  aOp = (BOPAlgo_Operation)iOp;
  //
  int                            iErr;
  NCollection_List<TopoDS_Shape> aLS;
  BOPAlgo_PaveFiller             aPF;
  //
  aLS.Append(aS1);
  aLS.Append(aS2);
  aPF.SetArguments(aLS);
  //
  aPF.Perform();
  iErr = aPF.HasErrors();
  if (iErr)
  {
    theDI << "Intersection failed with error status: " << iErr << "\n";
    return 1;
  }
  //
  BRepAlgoAPI_BooleanOperation* pBuilder = nullptr;
  //
  switch (aOp)
  {
    case BOPAlgo_COMMON:
      pBuilder = new BRepAlgoAPI_Common(aS1, aS2, aPF);
      break;
    case BOPAlgo_FUSE:
      pBuilder = new BRepAlgoAPI_Fuse(aS1, aS2, aPF);
      break;
    case BOPAlgo_CUT:
      pBuilder = new BRepAlgoAPI_Cut(aS1, aS2, aPF);
      break;
    case BOPAlgo_CUT21:
      pBuilder = new BRepAlgoAPI_Cut(aS1, aS2, aPF, false);
      break;
    case BOPAlgo_SECTION:
      pBuilder = new BRepAlgoAPI_Section(aS1, aS2, aPF);
      break;
    default:
      break;
  }
  //
  iErr = pBuilder->HasErrors();
  if (!pBuilder->IsDone())
  {
    theDI << "BOP failed with error status: " << iErr << "\n";
    return 1;
  }
  //
  const TopoDS_Shape& aRes = pBuilder->Shape();
  DBRep::Set(argv[1], aRes);
  //
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>           aMapArgs, aMapShape;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIt;
  bool                                                             bIsDeletedHist, bIsDeletedMap;
  TopAbs_ShapeEnum                                                 aType;
  //
  TopExp::MapShapes(aS1, aMapArgs);
  TopExp::MapShapes(aS2, aMapArgs);
  TopExp::MapShapes(aRes, aMapShape);
  //
  aIt.Initialize(aMapArgs);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aS = aIt.Value();
    aType                  = aS.ShapeType();
    if (aType != TopAbs_EDGE && aType != TopAbs_FACE && aType != TopAbs_VERTEX
        && aType != TopAbs_SOLID)
    {
      continue;
    }
    //
    bIsDeletedHist = pBuilder->IsDeleted(aS);
    bIsDeletedMap  = !aMapShape.Contains(aS) && (pBuilder->Modified(aS).Extent() == 0);
    //
    if (bIsDeletedHist != bIsDeletedMap)
    {
      theDI << "Error. Wrong value of IsDeleted flag.\n";
      return 1;
    }
  }
  //
  theDI << "Test complete\n";
  return 0;
}

static int OCC26139(Draw_Interpretor& theDI, int argc, const char** argv)
{

  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    theDI << "Use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  int    aBoxGridSize  = 100;
  int    aCompGridSize = 3;
  double aBoxSize      = 5.0;

  if (argc > 1)
  {
    for (int anArgIdx = 1; anArgIdx < argc; ++anArgIdx)
    {
      TCollection_AsciiString anArg(argv[anArgIdx]);
      anArg.LowerCase();
      if (anArg == "-boxgrid")
      {
        aBoxGridSize = Draw::Atoi(argv[++anArgIdx]);
      }
      else if (anArg == "-compgrid")
      {
        aCompGridSize = Draw::Atoi(argv[++anArgIdx]);
      }
      else if (anArg == "-boxsize")
      {
        aBoxSize = Draw::Atof(argv[++anArgIdx]);
      }
    }
  }

  NCollection_List<occ::handle<AIS_Shape>> aCompounds;
  for (int aCompGridX = 0; aCompGridX < aCompGridSize; ++aCompGridX)
  {
    for (int aCompGridY = 0; aCompGridY < aCompGridSize; ++aCompGridY)
    {
      BRep_Builder    aBuilder;
      TopoDS_Compound aComp;
      aBuilder.MakeCompound(aComp);
      for (int aBoxGridX = 0; aBoxGridX < aBoxGridSize; ++aBoxGridX)
      {
        for (int aBoxGridY = 0; aBoxGridY < aBoxGridSize; ++aBoxGridY)
        {
          BRepPrimAPI_MakeBox aBox(gp_Pnt(aBoxGridX * aBoxSize, aBoxGridY * aBoxSize, 0.0),
                                   aBoxSize,
                                   aBoxSize,
                                   aBoxSize);
          aBuilder.Add(aComp, aBox.Shape());
        }
      }
      gp_Trsf aTrsf;
      aTrsf.SetTranslation(
        gp_Vec(aBoxGridSize * aBoxSize * aCompGridX, aBoxGridSize * aBoxSize * aCompGridY, 0.0));
      TopLoc_Location aLoc(aTrsf);
      aComp.Located(aLoc);
      aCompounds.Append(new AIS_Shape(aComp));
    }
  }

  OSD_Timer aTimer;
  for (NCollection_List<occ::handle<AIS_Shape>>::Iterator aCompIter(aCompounds); aCompIter.More();
       aCompIter.Next())
  {
    aTimer.Start();
    aCtx->Display(aCompIter.Value(), false);
    aTimer.Stop();
    theDI << "Display time: " << aTimer.ElapsedTime() << "\n";
    aTimer.Reset();
  }

  aTimer.Reset();
  aTimer.Start();
  for (NCollection_List<occ::handle<AIS_Shape>>::Iterator aCompIter(aCompounds); aCompIter.More();
       aCompIter.Next())
  {
    aCtx->Remove(aCompIter.Value(), false);
  }
  aTimer.Stop();
  theDI << "Remove time: " << aTimer.ElapsedTime() << "\n";

  return 0;
}

#include <Standard_Integer.hxx>

#include <NCollection_DataMap.hxx>
#include <OSD.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeExtend_Status.hxx>
#ifdef _WIN32
  #define EXCEPTION ...
#else
  #define EXCEPTION Standard_Failure const&
#endif

static ShapeExtend_Status getStatusGap(const occ::handle<ShapeFix_Wire>& theFix, const bool theIs3d)
{
  for (int i = ShapeExtend_OK; i <= ShapeExtend_FAIL; i++)
  {
    bool isFound;
    if (theIs3d)
      isFound = theFix->StatusGaps3d((ShapeExtend_Status)i);
    else
      isFound = theFix->StatusGaps2d((ShapeExtend_Status)i);
    if (isFound)
      return ShapeExtend_Status(i);
  }
  return ShapeExtend_OK;
}

//=================================================================================================

static int OCC24881(Draw_Interpretor& di, int narg, const char** a)
{
  if (narg < 2)
  {
    di << "Usage: " << a[0] << " invalid number of arguments\n";
    return 1;
  }
  //    std::cout <<"FileName1: " << argv[1] <<std::endl;

  TopoDS_Shape aShape = DBRep::Get(a[1]);

  OSD::SetSignal();
  occ::handle<ShapeFix_Wire> aWireFix = new ShapeFix_Wire;

  // map FixStatus - NbSuchStatuses
  NCollection_DataMap<int, int> aStatusNbDMap;
  int                           nbFixed = 0, nbOk = 0;

  // Begin: STEP 7
  ShapeExtend_Status aStatus = ShapeExtend_OK;
  try
  {
    TopExp_Explorer aFaceExplorer(aShape, TopAbs_FACE);
    for (; aFaceExplorer.More(); aFaceExplorer.Next())
    {
      TopoDS_Shape aFace = aFaceExplorer.Current();
      // loop on wires
      TopoDS_Iterator aWireItr(aFace);
      for (; aWireItr.More(); aWireItr.Next())
      {
        bool        wasOk    = false;
        TopoDS_Wire aSrcWire = TopoDS::Wire(aWireItr.Value());

        aWireFix->Load(aSrcWire);
        aWireFix->SetFace(TopoDS::Face(aFace));
        aWireFix->FixReorder(); // correct order is a prerequisite
        // fix 3d
        if (!aWireFix->FixGaps3d())
        {
          // not fixed, why?
          aStatus = getStatusGap(aWireFix, true);
          if (aStatus == ShapeExtend_OK)
            wasOk = true;
          else
          {
            // keep 3d fail status
            if (aStatusNbDMap.IsBound(aStatus))
              aStatusNbDMap(aStatus)++;
            else
              aStatusNbDMap.Bind(aStatus, 1);
            continue;
          }
        }

        // fix 2d
        if (aWireFix->FixGaps2d())
          nbFixed++;
        else
        {
          aStatus = getStatusGap(aWireFix, false);
          if (aStatus == ShapeExtend_OK)
          {
            if (wasOk)
            {
              nbOk++;
              continue;
            }
            else
              nbFixed++;
          }
          else
          {
            // keep 2d fail status
            int aStatus2d = aStatus + ShapeExtend_FAIL;
            if (aStatusNbDMap.IsBound(aStatus2d))
              aStatusNbDMap(aStatus2d)++;
            else
              aStatusNbDMap.Bind(aStatus2d, 1);
            continue;
          }
        }
      }
    }
    // End: STEP 7
  }
  catch (EXCEPTION)
  {
    di << "Exception is raised = " << aStatus << "\n";
    return 1;
  }
  // report what is done

  if (nbFixed)
  {
    di << "Fix_FillGaps_Fixed: nbFixed = " << nbFixed << "\n";
  }
  if (nbOk)
  {
    di << "Fix_FillGaps_NothingToDo\n";
  }
  NCollection_DataMap<int, int>::Iterator aStatusItr(aStatusNbDMap);
  for (; aStatusItr.More(); aStatusItr.Next())
  {
    switch ((ShapeExtend_Status)aStatusItr.Key())
    {
      // treat 3d status
      case ShapeExtend_FAIL1:
        di << "Fix_FillGaps_3dNoCurveFail, nb failed = ";
        break;
      case ShapeExtend_FAIL2:
        di << "Fix_FillGaps_3dSomeGapsFail, nb failed = ";
        break;
      default:
        // treat 2d status
        switch ((ShapeExtend_Status)(aStatusItr.Key() - ShapeExtend_FAIL))
        {
          case ShapeExtend_FAIL1:
            di << "Fix_FillGaps_2dNoPCurveFail, nb failed = ";
            break;
          case ShapeExtend_FAIL2:
            di << "Fix_FillGaps_2dSomeGapsFail, nb failed = ";
            break;
          default:
            break;
        }
    }
    di << aStatusItr.Value() << "\n";
  }
  di << ("__________________________________") << "\n";

  return 0;
}

//=================================================================================================

static int OCC26284(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 1)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  occ::handle<AIS_InteractiveContext> anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  BRepPrimAPI_MakeSphere aSphereBuilder(gp_Pnt(0.0, 0.0, 0.0), 1.0);
  occ::handle<AIS_Shape> aSphere = new AIS_Shape(aSphereBuilder.Shape());
  anAISContext->Display(aSphere, false);
  for (int aChildIdx = 0; aChildIdx < 5; ++aChildIdx)
  {
    BRepPrimAPI_MakeSphere aBuilder(gp_Pnt(1.0 + aChildIdx, 1.0 + aChildIdx, 1.0 + aChildIdx), 1.0);
    occ::handle<AIS_Shape> aChild = new AIS_Shape(aBuilder.Shape());
    aSphere->AddChild(aChild);
    anAISContext->Display(aChild, false);
  }

  anAISContext->RecomputeSelectionOnly(aSphere);
  anAISContext->UpdateCurrentViewer();

  return 0;
}

#include <IntTools_Context.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

//=================================================================================================

int xprojponf(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 3)
  {
    di << " use xprojponf p f \n";
    return 0;
  }
  //
  gp_Pnt                        aP, aPS;
  TopoDS_Shape                  aS;
  TopoDS_Face                   aF;
  occ::handle<IntTools_Context> aCtx;
  //
  DrawTrSurf::GetPoint(a[1], aP);
  aS = DBRep::Get(a[2]);
  //
  if (aS.IsNull())
  {
    di << " null shape is not allowed\n";
    return 0;
  }
  //
  if (aS.ShapeType() != TopAbs_FACE)
  {
    di << a[2] << " not a face\n";
    return 0;
  }
  //
  aCtx = new IntTools_Context;
  //
  aF                               = TopoDS::Face(aS);
  GeomAPI_ProjectPointOnSurf& aPPS = aCtx->ProjPS(aF);
  //
  aPPS.Perform(aP);
  if (!aPPS.IsDone())
  {
    di << " projection failed\n";
    return 0;
  }
  //
  aPS = aPPS.NearestPoint();
  di << " point px " << aPS.X() << " " << aPS.Y() << " " << aPS.Z() << "\n";
  //
  return 0;
}

#include <NCollection_Array1.hxx>
#include <GeomConvert.hxx>

//=================================================================================================

int OCC26446(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 4)
  {
    di << "Usage: OCC26446 r c1 c2\n";
    return 1;
  }

  occ::handle<Geom_BSplineCurve> aCurve1 =
    occ::down_cast<Geom_BSplineCurve>(DrawTrSurf::GetCurve(a[2]));
  occ::handle<Geom_BSplineCurve> aCurve2 =
    occ::down_cast<Geom_BSplineCurve>(DrawTrSurf::GetCurve(a[3]));

  if (aCurve1.IsNull())
  {
    di << a[2] << " is not a BSpline curve\n";
    return 1;
  }

  if (aCurve2.IsNull())
  {
    di << a[3] << " is not a BSpline curve\n";
    return 1;
  }

  NCollection_Array1<occ::handle<Geom_BSplineCurve>> aCurves(0, 1);
  NCollection_Array1<double>                         aTolerances(0, 0);
  double                                             aTolConf    = 1.e-3;
  constexpr double                                   aTolClosure = Precision::Confusion();
  occ::handle<NCollection_HArray1<occ::handle<Geom_BSplineCurve>>> aConcatCurves;
  occ::handle<NCollection_HArray1<int>>                            anIndices;

  aCurves.SetValue(0, aCurve1);
  aCurves.SetValue(1, aCurve2);
  aTolerances.SetValue(0, aTolConf);

  bool closed_flag = false;
  GeomConvert::ConcatC1(aCurves, aTolerances, anIndices, aConcatCurves, closed_flag, aTolClosure);

  occ::handle<Geom_BSplineCurve> aResult = aConcatCurves->Value(aConcatCurves->Lower());

  DrawTrSurf::Set(a[1], aResult);
  return 0;
}

//=================================================================================================

#include <Poly.hxx>

static int OCC26485(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 2)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << "Failed. Null shape\n";
    return 1;
  }

  bool            isFailed = false;
  TopExp_Explorer aExplorer(aShape, TopAbs_FACE);
  for (; aExplorer.More(); aExplorer.Next())
  {
    const TopoDS_Face&                     aFace = TopoDS::Face(aExplorer.Current());
    TopLoc_Location                        L     = TopLoc_Location();
    const occ::handle<Poly_Triangulation>& aT    = BRep_Tool::Triangulation(aFace, L);

    if (aT.IsNull())
      continue;

    Poly::ComputeNormals(aT);

    // Number of nodes in the triangulation
    int aVertexNb = aT->NbNodes();

    // Get each vertex index, checking common vertexes between shapes
    for (int i = 0; i < aVertexNb; i++)
    {
      gp_Pnt       aPoint  = aT->Node(i + 1);
      const gp_Dir aNormal = aT->Normal(i + 1);

      if (aNormal.X() == 0 && aNormal.Y() == 0 && aNormal.Z() == 1)
      {
        char buf[256];
        Sprintf(buf, "fail_%d", i + 1);
        theDI << "Failed. Point " << buf << ": " << aPoint.X() << " " << aPoint.Y() << " "
              << aPoint.Z() << "\n";

        DrawTrSurf::Set(buf, aPoint);
      }
    }
  }

  theDI << (isFailed ? "Test failed" : "Test completed") << "\n";
  return 0;
}

//=================================================================================================

static int OCC26553(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc < 2)
  {
    theDI << "Error: path to file with shell is missing\n";
    return 1;
  }

  BRep_Builder aBuilder;
  TopoDS_Shape aShell;
  BRepTools::Read(aShell, theArgv[1], aBuilder);

  if (aShell.IsNull())
  {
    theDI << "Error: shell not loaded\n";
    return 1;
  }

  TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(0, 0, 10));
  TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

  BRepOffsetAPI_MakePipe aPipeBuilder(aPipeWire, aShell);
  if (!aPipeBuilder.IsDone())
  {
    theDI << "Error: failed to create pipe\n";
    return 1;
  }

  for (TopExp_Explorer aShapeExplorer(aShell, TopAbs_EDGE); aShapeExplorer.More();
       aShapeExplorer.Next())
  {
    const TopoDS_Shape& aGeneratedShape =
      aPipeBuilder.Generated(aPipeEdge, aShapeExplorer.Current());
    if (aGeneratedShape.IsNull())
    {
      theDI << "Error: null shape\n";
      return 1;
    }
  }

  theDI << "History returned successfully\n";
  return 0;
}

//=================================================================================================

#include <SelectMgr_SelectingVolumeManager.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Line.hxx>
#include <Aspect_Window.hxx>

static int OCC26195(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb < 3)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  if (ViewerTest::GetAISContext().IsNull())
  {
    std::cerr << "Error: No opened context!\n";
    return 1;
  }

  gp_Pnt2d aPxPnt1, aPxPnt2;
  aPxPnt1.SetX(Draw::Atof(theArgVec[1]));
  aPxPnt1.SetY(Draw::Atof(theArgVec[2]));
  if (theArgNb > 4)
  {
    aPxPnt2.SetX(Draw::Atof(theArgVec[3]));
    aPxPnt2.SetY(Draw::Atof(theArgVec[4]));
  }
  bool toPrint = false;
  if (theArgNb % 2 == 0)
  {
    toPrint = Draw::Atoi(theArgVec[theArgNb - 1]) != 0;
  }

  SelectMgr_SelectingVolumeManager* aMgr = new SelectMgr_SelectingVolumeManager();
  if (theArgNb > 4)
  {
    aMgr->InitBoxSelectingVolume(aPxPnt1, aPxPnt2);
  }
  else
  {
    aMgr->InitPointSelectingVolume(aPxPnt1);
  }
  aMgr->SetCamera(ViewerTest::CurrentView()->Camera());
  aMgr->SetPixelTolerance(ViewerTest::GetAISContext()->PixelTolerance());
  int aWidth, aHeight;
  ViewerTest::CurrentView()->View()->Window()->Size(aWidth, aHeight);
  aMgr->SetWindowSize(aWidth, aHeight);
  aMgr->BuildSelectingVolume();

  const gp_Pnt*              aVerts = aMgr->GetVertices();
  BRepBuilderAPI_MakePolygon aWireBldrs[4];

  aWireBldrs[0].Add(gp_Pnt(aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));
  aWireBldrs[0].Add(gp_Pnt(aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));
  aWireBldrs[0].Add(gp_Pnt(aVerts[6].X(), aVerts[6].Y(), aVerts[6].Z()));
  aWireBldrs[0].Add(gp_Pnt(aVerts[2].X(), aVerts[2].Y(), aVerts[2].Z()));
  aWireBldrs[0].Add(gp_Pnt(aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));

  aWireBldrs[1].Add(gp_Pnt(aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));
  aWireBldrs[1].Add(gp_Pnt(aVerts[5].X(), aVerts[5].Y(), aVerts[5].Z()));
  aWireBldrs[1].Add(gp_Pnt(aVerts[7].X(), aVerts[7].Y(), aVerts[7].Z()));
  aWireBldrs[1].Add(gp_Pnt(aVerts[6].X(), aVerts[6].Y(), aVerts[6].Z()));
  aWireBldrs[1].Add(gp_Pnt(aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));

  aWireBldrs[2].Add(gp_Pnt(aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));
  aWireBldrs[2].Add(gp_Pnt(aVerts[5].X(), aVerts[5].Y(), aVerts[5].Z()));
  aWireBldrs[2].Add(gp_Pnt(aVerts[7].X(), aVerts[7].Y(), aVerts[7].Z()));
  aWireBldrs[2].Add(gp_Pnt(aVerts[3].X(), aVerts[3].Y(), aVerts[3].Z()));
  aWireBldrs[2].Add(gp_Pnt(aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));

  aWireBldrs[3].Add(gp_Pnt(aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));
  aWireBldrs[3].Add(gp_Pnt(aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));
  aWireBldrs[3].Add(gp_Pnt(aVerts[3].X(), aVerts[3].Y(), aVerts[3].Z()));
  aWireBldrs[3].Add(gp_Pnt(aVerts[2].X(), aVerts[2].Y(), aVerts[2].Z()));
  aWireBldrs[3].Add(gp_Pnt(aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));

  TopoDS_Compound aComp;
  BRep_Builder    aCompBuilder;
  aCompBuilder.MakeCompound(aComp);
  for (int aWireIdx = 0; aWireIdx < 4; ++aWireIdx)
  {
    aCompBuilder.Add(aComp, aWireBldrs[aWireIdx].Shape());
  }
  DBRep::Set("c", aComp);

  occ::handle<AIS_InteractiveObject> aCmp = new AIS_Shape(aComp);
  aCmp->SetColor(Quantity_NOC_GREEN);
  ViewerTest::Display("c", aCmp, true, true);

  gp_Pnt aNearPnt = aMgr->GetNearPickedPnt();
  gp_Pnt aFarPnt  = aMgr->GetFarPickedPnt();
  if (Precision::IsInfinite(aFarPnt.X()) || Precision::IsInfinite(aFarPnt.Y())
      || Precision::IsInfinite(aFarPnt.Z()))
  {
    theDI << "Near: " << aNearPnt.X() << " " << aNearPnt.Y() << " " << aNearPnt.Z() << "\n";
    theDI << "Far: infinite point " << "\n";
    return 0;
  }

  occ::handle<Geom_CartesianPoint> aPnt1 = new Geom_CartesianPoint(aNearPnt);
  occ::handle<Geom_CartesianPoint> aPnt2 = new Geom_CartesianPoint(aFarPnt);

  occ::handle<AIS_Line> aLine = new AIS_Line(aPnt1, aPnt2);
  ViewerTest::Display("l", aLine, true, true);

  if (toPrint)
  {
    theDI << "Near: " << aNearPnt.X() << " " << aNearPnt.Y() << " " << aNearPnt.Z() << "\n";
    theDI << "Far: " << aFarPnt.X() << " " << aFarPnt.Y() << " " << aFarPnt.Z() << "\n";
  }

  return 0;
}

//=================================================================================================

static int OCC26462(Draw_Interpretor& theDI, int /*theArgNb*/, const char** /*theArgVec*/)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    std::cerr << "Error: No opened context!\n";
    return 1;
  }

  BRepPrimAPI_MakeBox                aBuilder1(gp_Pnt(10.0, 10.0, 0.0), 10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox                aBuilder2(10.0, 10.0, 10.0);
  occ::handle<AIS_InteractiveObject> aBox1 = new AIS_Shape(aBuilder1.Shape());
  occ::handle<AIS_InteractiveObject> aBox2 = new AIS_Shape(aBuilder2.Shape());

  const occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  aCtx->Display(aBox1, 0, 2, false);
  aCtx->Display(aBox2, 0, 2, false);
  ViewerTest::CurrentView()->FitAll();
  aCtx->SetWidth(aBox1, 3, false);
  aCtx->SetWidth(aBox2, 3, false);

  aCtx->MoveTo(305, 322, ViewerTest::CurrentView(), false);
  aCtx->SelectDetected(AIS_SelectionScheme_XOR);
  aCtx->MoveTo(103, 322, ViewerTest::CurrentView(), false);
  aCtx->SelectDetected(AIS_SelectionScheme_XOR);
  if (aCtx->NbSelected() != 0)
  {
    theDI << "ERROR: no boxes must be selected!\n";
    return 1;
  }

  aCtx->SetSelectionSensitivity(aBox1, 2, 5);

  aCtx->MoveTo(305, 322, ViewerTest::CurrentView(), false);
  aCtx->SelectDetected(AIS_SelectionScheme_XOR);
  if (aCtx->NbSelected() != 1)
  {
    theDI << "ERROR: b1 was not selected\n";
    return 1;
  }
  aCtx->MoveTo(103, 322, ViewerTest::CurrentView(), false);
  aCtx->SelectDetected(AIS_SelectionScheme_XOR);
  if (aCtx->NbSelected() != 1)
  {
    theDI << "ERROR: b2 is selected after b1's tolerance increased\n";
    return 1;
  }

  return 0;
}

#include <BRepBuilderAPI_GTransform.hxx>

static int OCC26313(Draw_Interpretor& di, int n, const char** a)
{
  if (n <= 1)
    return 1;

  gp_Trsf  T;
  gp_GTrsf GT(T);

  gp_Mat rot(1.0, 0.0, 0.0, 0.0, 2.0, 0.0, 0.0, 0.0, 3.0);

  GT.SetVectorialPart(rot);
  BRepBuilderAPI_GTransform gtrf(GT);

  TopoDS_Shape aSrcShape = DBRep::Get(a[2]);
  if (aSrcShape.IsNull())
  {
    di << a[2] << " is not a valid shape\n";
    return 1;
  }

  gtrf.Perform(aSrcShape);
  if (gtrf.IsDone())
  {
    try
    {
      DBRep::Set(a[1], gtrf.ModifiedShape(aSrcShape));
    }
    catch (Standard_Failure const&)
    {
      di << "Error: Exception is thrown\n";
    }
  }
  else
  {
    di << "Error: Result is not done\n";
    return 1;
  }

  return 0;
}

//=======================================================================
// function : OCC26525
// purpose  : check number of intersection points
//=======================================================================
#include <IntCurveSurface_HInter.hxx>

int OCC26525(Draw_Interpretor& di, int n, const char** a)
{
  TopoDS_Shape aS1, aS2;
  TopoDS_Edge  aE;
  TopoDS_Face  aF;

  if (n < 4)
  {
    di << " use OCC26525 r edge face \n";
    return 1;
  }

  aS1 = DBRep::Get(a[2]);
  aS2 = DBRep::Get(a[3]);

  if (aS1.IsNull() || aS2.IsNull())
  {
    di << " Null shapes are not allowed \n";
    return 0;
  }
  if (aS1.ShapeType() != TopAbs_EDGE)
  {
    di << " Shape" << a[2] << " should be of type EDGE\n";
    return 0;
  }
  if (aS2.ShapeType() != TopAbs_FACE)
  {
    di << " Shape" << a[3] << " should be of type FACE\n";
    return 0;
  }

  aE = TopoDS::Edge(aS1);
  aF = TopoDS::Face(aS2);

  char                              buf[128];
  bool                              bIsDone;
  int                               i, aNbPoints;
  double                            aU, aV, aT;
  gp_Pnt                            aP;
  BRepAdaptor_Curve                 aBAC;
  BRepAdaptor_Surface               aBAS;
  IntCurveSurface_TransitionOnCurve aTC;
  IntCurveSurface_HInter            aHInter;

  aBAC.Initialize(aE);
  aBAS.Initialize(aF);

  occ::handle<BRepAdaptor_Curve>   aHBAC = new BRepAdaptor_Curve(aBAC);
  occ::handle<BRepAdaptor_Surface> aHBAS = new BRepAdaptor_Surface(aBAS);

  aHInter.Perform(aHBAC, aHBAS);
  bIsDone = aHInter.IsDone();
  if (!bIsDone)
  {
    di << " intersection is not done\n";
    return 0;
  }

  aNbPoints = aHInter.NbPoints();
  Sprintf(buf, " Number of intersection points found: %d", aNbPoints);
  di << buf << "\n";
  for (i = 1; i <= aNbPoints; ++i)
  {
    const IntCurveSurface_IntersectionPoint& aIP = aHInter.Point(i);
    aIP.Values(aP, aU, aV, aT, aTC);
    //
    Sprintf(buf, "point %s_%d %lg %lg %lg  ", a[1], i, aP.X(), aP.Y(), aP.Z());
    di << buf << "\n";
  }

  return 0;
}

#include <BRepOffsetAPI_DraftAngle.hxx>

static TopoDS_Shape taper(const TopoDS_Shape& shape,
                          const TopoDS_Face&  face_a,
                          const TopoDS_Face&  face_b,
                          double              angle)
{
  // Use maximum face-to-taper z-offset.
  const gp_Pln neutral_plane(gp_Ax3(gp_Pnt(0.0, 0.0, 140.0), gp_Dir(gp_Dir::D::Z)));

  // Draft angle needs to be in radians, and flipped to adhere to our own (arbitrary) draft
  // angle definition.
  const double draft_angle = -(angle / 180.0) * M_PI;

  // Add face to draft. The first argument indicates that all material added/removed during
  // drafting is located below the neutral plane
  BRepOffsetAPI_DraftAngle drafter(shape);
  drafter.Add(face_a, gp_Dir(gp_Dir::D::NZ), draft_angle, neutral_plane);
  drafter.Add(face_b, gp_Dir(gp_Dir::D::NZ), draft_angle, neutral_plane);
  drafter.Build();

  return drafter.Shape();
}

static void dumpShapeVertices(const TopoDS_Shape& shape, NCollection_LinearVector<double>& coords)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> shape_vertices;
  TopExp::MapShapes(shape, TopAbs_VERTEX, shape_vertices);

  for (int i = 1; i <= shape_vertices.Extent(); i++)
  {
    gp_Pnt p = BRep_Tool::Pnt(TopoDS::Vertex(shape_vertices(i)));
    coords.Append(p.X());
    coords.Append(p.Y());
    coords.Append(p.Z());
  }
}

static void GetCoords(const char* const path_to_file, NCollection_LinearVector<double>& coords)
{
  TopoDS_Shape shape;
  BRep_Builder builder;
  BRepTools::Read(shape, path_to_file, builder);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> shape_faces;
  TopExp::MapShapes(shape, TopAbs_FACE, shape_faces);
  TopoDS_Face face_a = TopoDS::Face(shape_faces(1));
  TopoDS_Face face_b = TopoDS::Face(shape_faces(5));
  dumpShapeVertices(taper(shape, face_a, face_b, 5.0), coords);
}

static int OCC26396(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc < 2)
  {
    theDI << "Error: path to file is missing\n";
    return 1;
  }

  const int maxInd = 50;

  NCollection_LinearVector<double> ref_coords;
  ref_coords.Reserve(100);
  bool Stat = true;

  GetCoords(theArgv[1], ref_coords);

  NCollection_LinearVector<double> coords;
  coords.Reserve(100);
  for (int i = 1; i < maxInd; i++)
  {
    GetCoords(theArgv[1], coords);
    if (coords.Size() != ref_coords.Size())
    {
      Stat = false;
      break;
    }
    for (size_t j = 0; j < coords.Size(); j++)
      if (std::abs(ref_coords[j] - coords[j]) > RealEpsilon())
      {
        Stat = false;
        break;
      }
    coords.Clear();
  }
  if (!Stat)
    theDI << "Error: unstable results";
  else
    theDI << "test OK";

  return 0;
}

//=======================================================================
// function : OCC27048
// purpose  : Calculate value of B-spline surface N times
//=======================================================================
static int OCC27048(Draw_Interpretor& theDI, int theArgc, const char** theArgv)
{
  if (theArgc != 5)
  {
    std::cout << "Incorrect number of arguments. See usage:" << std::endl;
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  occ::handle<Geom_Surface> aSurf = DrawTrSurf::GetSurface(theArgv[1]);
  GeomAdaptor_Surface       anAdaptor(aSurf);

  double aU = Draw::Atof(theArgv[2]);
  double aV = Draw::Atof(theArgv[3]);
  int    aN = Draw::Atoi(theArgv[4]);

  for (; aN > 0; --aN)
    anAdaptor.Value(aU, aV);

  return 0;
}

//========================================================================
// function : OCC27318
// purpose  : Creates a box that is not listed in map of AIS objects of ViewerTest
//========================================================================
static int OCC27318(Draw_Interpretor& /*theDI*/, int /*theArgc*/, const char** theArgv)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape           aBox    = BRepPrimAPI_MakeBox(20, 20, 20).Shape();
  occ::handle<AIS_Shape> aBoxObj = new AIS_Shape(aBox);
  aCtx->Display(aBoxObj, true);

  return 0;
}

//========================================================================
// function : OCC27523
// purpose  : Checks recomputation of deactivated selection mode after object's redisplaying
//========================================================================
static int OCC27523(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  if (theArgNb != 1)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp(theArgVec[0]);
    return 1;
  }

  occ::handle<AIS_InteractiveContext> anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  gp_Pnt                             aStart(100, 100, 100);
  gp_Pnt                             anEnd(300, 400, 600);
  BRepBuilderAPI_MakeEdge            anEdgeBuilder(aStart, anEnd);
  TopoDS_Edge                        anEdge        = anEdgeBuilder.Edge();
  occ::handle<AIS_InteractiveObject> aTestAISShape = new AIS_Shape(anEdge);
  anAISContext->Display(aTestAISShape, false);

  // activate it in selection modes
  NCollection_Sequence<int> aModes;
  aModes.Append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum)TopAbs_VERTEX));

  anAISContext->Deactivate(aTestAISShape);
  anAISContext->Load(aTestAISShape, -1);
  anAISContext->Activate(aTestAISShape, 0);
  anAISContext->Deactivate(aTestAISShape, 0);

  // activate in vertices mode
  for (int anIt = 1; anIt <= aModes.Length(); ++anIt)
  {
    anAISContext->Activate(aTestAISShape, aModes(anIt));
  }

  TopoDS_Shape     aVertexShape     = BRepBuilderAPI_MakeVertex(gp_Pnt(75, 0, 0));
  TopAbs_ShapeEnum aVertexShapeType = aVertexShape.ShapeType();
  occ::down_cast<AIS_Shape>(aTestAISShape)->Set(aVertexShape);
  aTestAISShape->Redisplay();

  anAISContext->AddOrRemoveSelected(aTestAISShape, true);

  bool aValidShapeType = false;
  for (anAISContext->InitSelected(); anAISContext->MoreSelected(); anAISContext->NextSelected())
  {
    occ::handle<SelectMgr_EntityOwner> anOwner = anAISContext->SelectedOwner();
    occ::handle<StdSelect_BRepOwner>   aBRO    = occ::down_cast<StdSelect_BRepOwner>(anOwner);
    if (!aBRO.IsNull() && aBRO->HasShape())
    {
      TopoDS_Shape aShape = aBRO->Shape();

      aValidShapeType = aShape.ShapeType() == aVertexShapeType;
    }
  }

  if (!aValidShapeType)
  {
    std::cerr << "Error: shape type is invalid.\n";
    return 1;
  }

  return 0;
}

//========================================================================
// function : OCC27700
// purpose  : glPolygonMode() used for frame drawing affects label text shading
//========================================================================

class OCC27700_Text : public AIS_InteractiveObject
{
public:
  DEFINE_STANDARD_RTTI_INLINE(OCC27700_Text, AIS_InteractiveObject)

  void Compute(const occ::handle<PrsMgr_PresentationManager>&,
               const occ::handle<Prs3d_Presentation>& thePresentation,
               const int) override
  {
    occ::handle<Graphic3d_ArrayOfTriangles> aFrame = new Graphic3d_ArrayOfTriangles(6, 6);
    aFrame->AddVertex(gp_Pnt(-1, 0, 0));
    aFrame->AddVertex(gp_Pnt(-1, 1, 0));
    aFrame->AddVertex(gp_Pnt(3, 1, 0));
    aFrame->AddVertex(gp_Pnt(3, 0, 0));

    aFrame->AddEdge(1);
    aFrame->AddEdge(2);
    aFrame->AddEdge(3);

    aFrame->AddEdge(2);
    aFrame->AddEdge(3);
    aFrame->AddEdge(4);

    occ::handle<Graphic3d_AspectFillArea3d> aFillAspect =
      new Graphic3d_AspectFillArea3d(*myDrawer->ShadingAspect()->Aspect().get());
    aFillAspect->SetInteriorStyle(Aspect_IS_POINT);

    // create separate group for frame elements
    occ::handle<Graphic3d_Group> aFrameGroup = thePresentation->NewGroup();
    aFrameGroup->AddPrimitiveArray(aFrame);
    aFrameGroup->SetGroupPrimitivesAspect(aFillAspect);

    // create separate group for text elements
    occ::handle<Graphic3d_Group> aTextGroup = thePresentation->NewGroup();
    TCollection_ExtendedString   aString("YOU SHOULD SEE THIS TEXT", true);
    Prs3d_Text::Draw(aTextGroup, myDrawer->TextAspect(), aString, gp_Ax2(gp::Origin(), gp::DZ()));
  }

  void ComputeSelection(const occ::handle<SelectMgr_Selection>& /*theSelection*/,
                        const int /*theMode*/) override
  {
  }
};

static int OCC27700(Draw_Interpretor& /*theDI*/, int /*theArgNb*/, const char** /*theArgVec*/)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << "Error: no view available, call 'vinit' before!" << std::endl;
    return 1;
  }
  occ::handle<OCC27700_Text> aPresentation = new OCC27700_Text();
  aContext->Display(aPresentation, true);
  return 0;
}

//========================================================================
// function : OCC27757
// purpose  : Creates a box that has a sphere as child object and displays it
//========================================================================
static int OCC27757(Draw_Interpretor& /*theDI*/, int /*theArgc*/, const char** theArgv)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBox    = BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape();
  TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();
  gp_Trsf      aTrsf;
  aTrsf.SetTranslationPart(gp_Vec(20.0, 20.0, 0.0));
  aSphere.Located(TopLoc_Location(aTrsf));

  occ::handle<AIS_Shape> aBoxObj    = new AIS_Shape(aBox);
  occ::handle<AIS_Shape> aSphereObj = new AIS_Shape(aSphere);
  aBoxObj->AddChild(aSphereObj);
  aCtx->Display(aBoxObj, 1, 0, false);
  aCtx->UpdateCurrentViewer();

  return 0;
}

//========================================================================
// function : OCC27818
// purpose  : Creates three boxes and highlights one of them with own style
//========================================================================
static int OCC27818(Draw_Interpretor& /*theDI*/, int /*theArgc*/, const char** theArgv)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  occ::handle<AIS_Shape> aBoxObjs[3];
  for (int aBoxIdx = 0; aBoxIdx < 3; ++aBoxIdx)
  {
    TopoDS_Shape aBox = BRepPrimAPI_MakeBox(20.0, 20.0, 20.0).Shape();
    aBoxObjs[aBoxIdx] = new AIS_Shape(aBox);
    gp_Trsf aTrsf;
    aTrsf.SetTranslationPart(gp_Vec(30.0 * aBoxIdx, 30.0 * aBoxIdx, 0.0));
    aBoxObjs[aBoxIdx]->SetLocalTransformation(aTrsf);
    aBoxObjs[aBoxIdx]->SetHilightMode(AIS_Shaded);
  }

  {
    occ::handle<Prs3d_Drawer> aHiStyle = new Prs3d_Drawer();
    aBoxObjs[1]->SetDynamicHilightAttributes(aHiStyle);
    aHiStyle->SetDisplayMode(AIS_Shaded);
    aHiStyle->SetColor(Quantity_NOC_RED);
    aHiStyle->SetTransparency(0.8f);
  }
  {
    occ::handle<Prs3d_Drawer> aSelStyle = new Prs3d_Drawer();
    aBoxObjs[2]->SetHilightAttributes(aSelStyle);
    aSelStyle->SetDisplayMode(AIS_Shaded);
    aSelStyle->SetColor(Quantity_NOC_RED);
    aSelStyle->SetTransparency(0.0f);
    aSelStyle->SetZLayer(Graphic3d_ZLayerId_Topmost);
  }

  for (int aBoxIdx = 0; aBoxIdx < 3; ++aBoxIdx)
  {
    aCtx->Display(aBoxObjs[aBoxIdx], AIS_Shaded, 0, false);
  }

  aCtx->UpdateCurrentViewer();

  return 0;
}

//========================================================================
// function : OCC27893
// purpose  : Creates a box and selects it via AIS_InteractiveContext API
//========================================================================
static int OCC27893(Draw_Interpretor& /*theDI*/, int /*theArgc*/, const char** theArgv)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape                       aBox    = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  occ::handle<AIS_InteractiveObject> aBoxObj = new AIS_Shape(aBox);
  aCtx->Display(aBoxObj, AIS_Shaded, 0, false);
  aCtx->SetSelected(aBoxObj, true);

  return 0;
}

//========================================================================
// function : OCC28310
// purpose  : Tests validness of iterator in AIS_InteractiveContext after
// an removing object from it
//========================================================================
static int OCC28310(Draw_Interpretor& /*theDI*/, int /*theArgc*/, const char** theArgv)
{
  const occ::handle<AIS_InteractiveContext>& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape                       aBox    = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  occ::handle<AIS_InteractiveObject> aBoxObj = new AIS_Shape(aBox);
  aCtx->Display(aBoxObj, AIS_Shaded, 0, false);
  ViewerTest::CurrentView()->FitAll();
  aCtx->MoveTo(200, 200, ViewerTest::CurrentView(), true);
  aCtx->SelectDetected();
  aCtx->UpdateCurrentViewer();

  aCtx->Remove(aBoxObj, true);
  // nullify the object explicitly to simulate situation in project,
  // when ::Remove is called from another method and the object is destroyed
  // before ::DetectedInteractive is called
  aBoxObj.Nullify();

  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    occ::handle<AIS_InteractiveObject> anObj = aCtx->DetectedInteractive();
  }

  return 0;
}

// repetitive display and removal of multiple small objects in the viewer for
// test of memory leak in visualization (OCCT 6.9.0 - 7.0.0)
static int OCC29412(Draw_Interpretor& /*theDI*/, int theArgNb, const char** theArgVec)
{
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view.\n";
    return 1;
  }

  const int aNbIters      = (theArgNb <= 1 ? 10000 : Draw::Atoi(theArgVec[1]));
  int       aProgressPrev = -1;
  for (int m_loopIndex = 0; m_loopIndex < aNbIters; m_loopIndex++)
  {
    gp_Pnt pos;
    gp_Vec dir(0, 0, 1);

    gp_Ax2                 center(pos, dir);
    gp_Circ                circle(center, 1);
    occ::handle<AIS_Shape> feature;

    BRepBuilderAPI_MakeEdge builder(circle);

    if (builder.Error() == BRepBuilderAPI_EdgeDone)
    {
      TopoDS_Edge  E1 = builder.Edge();
      TopoDS_Shape W2 = BRepBuilderAPI_MakeWire(E1).Wire();
      feature         = new AIS_Shape(W2);
      aCtx->Display(feature, true);
    }

    aCtx->CurrentViewer()->Update();
    ViewerTest::CurrentView()->FitAll();
    aCtx->Remove(feature, true);

    const int aProgress = (m_loopIndex * 100) / aNbIters;
    if (aProgress != aProgressPrev)
    {
      std::cerr << aProgress << "%\r";
      aProgressPrev = aProgress;
    }
  }
  return 0;
}

//=================================================================================================

void QABugs::Commands_19(Draw_Interpretor& theCommands)
{
  const char* group = "QABugs";

  occ::handle<QABugs_HandleClass> aClassPtr = new QABugs_HandleClass();
  theCommands.Add("OCC24202_1",
                  "Test Handle-based procedure",
                  __FILE__,
                  aClassPtr,
                  &QABugs_HandleClass::HandleProc,
                  group);
  NCollection_Handle<QABugs_NHandleClass> aNClassPtr = new QABugs_NHandleClass();
  theCommands.Add("OCC24202_2",
                  "Test NCollection_Handle-based procedure",
                  __FILE__,
                  aNClassPtr,
                  &QABugs_NHandleClass::NHandleProc,
                  group);

  theCommands.Add("OCC23774", "OCC23774 shape1 shape2", __FILE__, OCC23774, group);
  theCommands.Add("OCC23683", "OCC23683 shape", __FILE__, OCC23683, group);
  theCommands.Add("OCC23952sweep", "OCC23952sweep nbupoles shape", __FILE__, OCC23952sweep, group);
  theCommands.Add("OCC23952intersect",
                  "OCC23952intersect nbsol shape1 shape2",
                  __FILE__,
                  OCC23952intersect,
                  group);
  theCommands.Add("test_offset", "test_offset", __FILE__, test_offset, group);
  theCommands.Add("OCC23945",
                  "OCC23945 surfname U V X Y Z [DUX DUY DUZ DVX DVY DVZ [D2UX D2UY D2UZ D2VX D2VY "
                  "D2VZ D2UVX D2UVY D2UVZ]]",
                  __FILE__,
                  OCC23945,
                  group);
  theCommands.Add("OCC24008", "OCC24008 curve surface", __FILE__, OCC24008, group);
  theCommands.Add("OCC24137", "OCC24137 face vertex U V [N]", __FILE__, OCC24137, group);
  theCommands.Add("OCC24370", "OCC24370 edge pcurve surface prec", __FILE__, OCC24370, group);
  theCommands.Add("OCC24086", "OCC24086 face wire", __FILE__, OCC24086, group);
  theCommands.Add("OCC24667",
                  "OCC24667 result Wire_spine Profile [Mode [Approx]], no args to get help",
                  __FILE__,
                  OCC24667,
                  group);
  theCommands.Add("OCC24834", "OCC24834", __FILE__, OCC24834, group);
  theCommands.Add("OCC24931", "OCC24931 path to saved xml file", __FILE__, OCC24931, group);
  theCommands.Add("OCC24925",
                  "OCC24925 filename [pluginLib=TKXml storageGuid retrievalGuid]"
                  "\nOCAF persistence without setting environment variables",
                  __FILE__,
                  OCC24925,
                  group);
  theCommands.Add("OCC25043", "OCC25043 shape", __FILE__, OCC25043, group);
  theCommands.Add("OCC24606",
                  "OCC24606 : Tests ::FitAll for V3d view ('vfit' is for NIS view)",
                  __FILE__,
                  OCC24606,
                  group);
  theCommands.Add("OCC25202",
                  "OCC25202 res shape numF1 face1 numF2 face2",
                  __FILE__,
                  OCC25202,
                  group);
  theCommands.Add("OCC7570", "OCC7570 shape", __FILE__, OCC7570, group);
  theCommands.Add("OCC25100", "OCC25100 shape", __FILE__, OCC25100, group);
  theCommands.Add("OCC25340", "OCC25340", __FILE__, OCC25340, group);
  theCommands.Add("OCC25413", "OCC25413 shape", __FILE__, OCC25413, group);
  theCommands.Add("OCC25446", "OCC25446 res b1 b2 op", __FILE__, OCC25446, group);
  theCommands.Add("OCC24881", "OCC24881 shape", __FILE__, OCC24881, group);
  theCommands.Add("xprojponf", "xprojponf p f", __FILE__, xprojponf, group);
  theCommands.Add("OCC26139",
                  "OCC26139 [-boxsize value] [-boxgrid value] [-compgrid value]",
                  __FILE__,
                  OCC26139,
                  group);
  theCommands.Add("OCC26284", "OCC26284", __FILE__, OCC26284, group);
  theCommands.Add("OCC26446", "OCC26446 r c1 c2", __FILE__, OCC26446, group);
  theCommands.Add("OCC26485", "OCC26485 shape", __FILE__, OCC26485, group);
  theCommands.Add("OCC26553", "OCC26553 file_path", __FILE__, OCC26553, group);
  theCommands.Add(
    "OCC26195",
    "OCC26195: x1_pix y1_pix [x2_pix y2_pix] [toPrintPixelCoord 0|1]"
    "\n\t\t: Draws rectangular selecting frustum defined by point selection in pixel coordinates"
    "\n\t\t: [x1_pix, y1_pix] or rectangular selection in pixel coordinates [x1_pix, y1_pix,"
    "\n\t\t: x2_pix, y2_pix]."
    "\n\t\t: [toPrintPixelCoord 0|1] - prints 3d projection of pixel coordinate or center of"
    "\n\t\t: selecting rectangle onto near and far view frustum planes",
    __FILE__,
    OCC26195,
    group);
  theCommands.Add("OCC26462",
                  "OCC26462: Checks the ability to manage sensitivity of a particular selection "
                  "mode in local context",
                  __FILE__,
                  OCC26462,
                  group);

  theCommands.Add("OCC26313", "OCC26313 result shape", __FILE__, OCC26313, group);
  theCommands.Add("OCC26396", "OCC26396 shape_file_path", __FILE__, OCC26396, group);
  theCommands.Add("OCC26525", "OCC26525 result edge face ", __FILE__, OCC26525, group);

  theCommands.Add("OCC27048",
                  "OCC27048 surf U V N\nCalculate value of surface N times in the point (U, V)",
                  __FILE__,
                  OCC27048,
                  group);

  theCommands.Add("OCC27318",
                  "OCC27318: Creates a box that is not listed in map of AIS objects of ViewerTest",
                  __FILE__,
                  OCC27318,
                  group);
  theCommands.Add(
    "OCC27523",
    "OCC27523: Checks recomputation of deactivated selection mode after object's redisplaying",
    __FILE__,
    OCC27523,
    group);
  theCommands.Add("OCC27700",
                  "OCC27700: Checks drawing text after setting interior style",
                  __FILE__,
                  OCC27700,
                  group);
  theCommands.Add("OCC27757",
                  "OCC27757: Creates a box that has a sphere as child object and displays it",
                  __FILE__,
                  OCC27757,
                  group);
  theCommands.Add("OCC27818",
                  "OCC27818: Creates three boxes and highlights one of them with own style",
                  __FILE__,
                  OCC27818,
                  group);
  theCommands.Add("OCC27893",
                  "OCC27893: Creates a box and selects it via AIS_InteractiveContext API",
                  __FILE__,
                  OCC27893,
                  group);
  theCommands.Add("OCC28310",
                  "OCC28310: Tests validness of iterator in AIS_InteractiveContext after an "
                  "removing object from it",
                  __FILE__,
                  OCC28310,
                  group);
  theCommands.Add("OCC29412",
                  "OCC29412 [nb cycles]: test display / remove of many small objects",
                  __FILE__,
                  OCC29412,
                  group);
  return;
}
