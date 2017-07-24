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
#include <AIS_LocalContext.hxx>
#include <AIS_Shape.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomFill_Trihedron.hxx>
#include <Graphic3d_ArrayOfTriangles.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Quaternion.hxx>
#include <Image_Color.hxx>
#include <Image_PixMap.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_Map.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_PerfMeter.hxx>
#include <OSD_Timer.hxx>
#include <Precision.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Prs3d_Text.hxx>
#include <SelectMgr_Filter.hxx>
#include <Standard_Version.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <ViewerTest.hxx>
#include <XmlDrivers_DocumentRetrievalDriver.hxx>
#include <XmlDrivers_DocumentStorageDriver.hxx>
#include <TDataStd_Real.hxx>

#include <cstdio>
#include <cmath>
#include <iostream>

#define QCOMPARE(val1, val2) \
  di << "Checking " #val1 " == " #val2 << \
        ((val1) == (val2) ? ": OK\n" : ": Error\n")

static Standard_Integer OCC230 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 4) {
    di << "ERROR OCC230: Usage : " << argv[0] << " TrimmedCurve Pnt2d Pnt2d\n";
    return 1;
  }

  gp_Pnt2d P1, P2;
  if ( !DrawTrSurf::GetPoint2d(argv[2],P1)) {
    di << "ERROR OCC230: " << argv[2] << " is not Pnt2d\n";
    return 1;
  }
  if ( !DrawTrSurf::GetPoint2d(argv[3],P2)) {
    di << "ERROR OCC230: " << argv[3] << " is not Pnt2d\n";
    return 1;
  }

  GCE2d_MakeSegment MakeSegment(P1,P2);
  Handle(Geom2d_TrimmedCurve) TrimmedCurve = MakeSegment.Value();
  DrawTrSurf::Set(argv[1], TrimmedCurve);
  return 0;
}

static Standard_Integer OCC23361 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  gp_Pnt p(0, 0, 2);
  
  gp_Trsf t1, t2;
  t1.SetRotation(gp_Ax1(p, gp_Dir(0, 1, 0)), -0.49328285294022267);
  t2.SetRotation(gp_Ax1(p, gp_Dir(0, 0, 1)), 0.87538474718473880);

  gp_Trsf tComp = t2 * t1;

  gp_Pnt p1(10, 3, 4);
  gp_Pnt p2 = p1.Transformed(tComp);
  gp_Pnt p3 = p1.Transformed(t1);
  p3.Transform(t2);

  // points must be equal
  if ( ! p2.IsEqual(p3, Precision::Confusion()) )
    di << "ERROR OCC23361: equivalent transformations does not produce equal points\n";
  else 
    di << "OCC23361: OK\n";

  return 0;
}

static Standard_Integer OCC23237 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  OSD_PerfMeter aPM("TestMeter",0);
  OSD_Timer aTM;
  
  // run some operation in cycle for about 2 seconds to have good values of times to compare
  int count = 0;
  printf("OSD_PerfMeter test.\nRunning Boolean operation on solids in loop.\n");
  for (; aTM.ElapsedTime() < 2.; count++)
  {
    aPM.Start();
    aTM.Start();

    // do some operation that will take considerable time compared with time of starting / stopping timers
    BRepPrimAPI_MakeBox aBox (10., 10., 10.);
    BRepPrimAPI_MakeSphere aSphere (10.);
    BRepAlgoAPI_Cut aCutter (aBox.Shape(), aSphere.Shape());

    aTM.Stop();
    aPM.Stop();
  }
 
  int aNbEnters = 0;
  Standard_Real aPerfMeter_CPUtime = 0., aTimer_CPUTime = 0., aS;
  Standard_Integer aM, aH;
  aTM.Show(aS, aM, aH, aTimer_CPUTime);

  perf_get_meter("TestMeter", &aNbEnters, &aPerfMeter_CPUtime);
  perf_init_meter("TestMeter");

  Standard_Real aTimeDiff = (fabs(aTimer_CPUTime - aPerfMeter_CPUtime) / aTimer_CPUTime);

  printf("\nMeasurement results (%d cycles):\n", count);
  printf("\nOSD_PerfMeter CPU time: %lf\nOSD_Timer CPU time: %lf\n",
    aPerfMeter_CPUtime, aTimer_CPUTime);
  printf("Time delta is: %.3lf %%\n", aTimeDiff * 100);

  if (aTimeDiff > 0.2)
    di << "OCC23237: Error: too much difference between CPU and elapsed times";
  else if (aNbEnters != count)
    di << "OCC23237: Error: counter reported by PerfMeter (" << aNbEnters << ") does not correspond to actual number of cycles";
  else
    di << "OCC23237: OK";

  return 0;
}

class IncrementerDecrementer
{
public:
    IncrementerDecrementer (Standard_Integer* theVal, Standard_Boolean thePositive) : myVal (theVal), myPositive (thePositive)
    {}
    void operator() (const size_t) const
    {
      if ( myPositive )
        Standard_Atomic_Increment(myVal);
      else
        Standard_Atomic_Decrement(myVal);
    }
private:
    Standard_Integer*   myVal;
    Standard_Boolean    myPositive;
};

static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  int aSum = 0;

  //check returned value
  QCOMPARE (Standard_Atomic_Decrement (&aSum), -1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 0);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 1);
  QCOMPARE (Standard_Atomic_Increment (&aSum), 2);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 0);
//  QCOMPARE (Standard_Atomic_DecrementTest (&aSum), 1);

  //check atomicity 
  aSum = 0;
  const int N = 1 << 24; //big enough to ensure concurrency

  //increment
  OSD_Parallel::For(0, N, IncrementerDecrementer (&aSum, true));
  QCOMPARE (aSum, N);

  //decrement
  OSD_Parallel::For(0, N, IncrementerDecrementer (&aSum, false));
  QCOMPARE (aSum, 0);

  return 0;
}

#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <DDocStd.hxx>

static Standard_Integer OCC23595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  Handle(TDocStd_Document) aDoc;
  anApp->NewDocument ("XmlXCAF", aDoc);
  QCOMPARE (!aDoc.IsNull(), Standard_True);

  Handle(XCAFDoc_ShapeTool) aShTool = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main());

  //check default value
  Standard_Boolean aValue = XCAFDoc_ShapeTool::AutoNaming();
  QCOMPARE (aValue, Standard_True);

  //true
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_True);
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox (100., 200., 300.).Shape();
  TDF_Label aLabel = aShTool->AddShape (aShape);
  Handle(TDataStd_Name) anAttr;
  QCOMPARE (aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //false
  XCAFDoc_ShapeTool::SetAutoNaming (Standard_False);
  aShape = BRepPrimAPI_MakeBox (300., 200., 100.).Shape();
  aLabel = aShTool->AddShape (aShape);
  QCOMPARE (!aLabel.FindAttribute (TDataStd_Name::GetID(), anAttr), Standard_True);

  //restore
  XCAFDoc_ShapeTool::SetAutoNaming (aValue);

  return 0;
}

#include <ExprIntrp_GenExp.hxx>
Standard_Integer OCC22611 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{

  if (argc != 3) {
    di << "Usage : " << argv[0] << " string nb\n";
    return 1;
  }

  TCollection_AsciiString aToken = argv[1];
  Standard_Integer aNb = atoi(argv[2]);

  Handle(ExprIntrp_GenExp) aGen = ExprIntrp_GenExp::Create();
  for (Standard_Integer i=0; i < aNb; i++)
  {
    aGen->Process(aToken);
    Handle(Expr_GeneralExpression) aExpr = aGen->Expression();
  }

  return 0;
}

Standard_Integer OCC22595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  gp_Mat M0;
  di << "M0 = "
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}"
  << " {" << M0(2,1) << "} {" << M0(2,2) << "} {" << M0(2,3) <<"}"
  << " {" << M0(1,1) << "} {" << M0(1,2) << "} {" << M0(1,3) <<"}";
  return 0;
}

#include <TopoDS_Face.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools.hxx>

static Standard_Boolean OCC23774Test(const TopoDS_Face& grossPlateFace, const TopoDS_Shape& originalWire, Draw_Interpretor& di)
{
  BRepExtrema_DistShapeShape distShapeShape(grossPlateFace,originalWire,Extrema_ExtFlag_MIN);
  if(!distShapeShape.IsDone()) {
    di <<"Distance ShapeShape is Not Done\n";
    return Standard_False;
  }

  if(distShapeShape.Value() > 0.01) {
    di << "Wrong Dist = " <<distShapeShape.Value() << "\n";
    return Standard_False;
  } else
    di << "Dist0 = " <<distShapeShape.Value() <<"\n";

  //////////////////////////////////////////////////////////////////////////
  /// First Flip Y
  const gp_Pnt2d axis1P1(1474.8199035519228,1249.9995745636970);
  const gp_Pnt2d axis1P2(1474.8199035519228,1250.9995745636970);

  gp_Vec2d mirrorVector1(axis1P1,axis1P2);

  gp_Trsf2d mirror1;
  mirror1.SetMirror(gp_Ax2d(axis1P1,mirrorVector1));

  BRepBuilderAPI_Transform transformer1(mirror1);
  transformer1.Perform(originalWire);
  if(!transformer1.IsDone()) {
    di << "Not Done1 \n";
    return Standard_False;
  }
  TopoDS_Shape step1ModifiedShape = transformer1.ModifiedShape(originalWire);
  
  BRepExtrema_DistShapeShape distShapeShape1(grossPlateFace,step1ModifiedShape,Extrema_ExtFlag_MIN);
  if(!distShapeShape1.IsDone())
    return Standard_False;
  if(distShapeShape1.Value() > 0.01) {
    di << "Dist = " <<distShapeShape1.Value() <<"\n";
    return Standard_False;
  } else
    di << "Dist1 = " <<distShapeShape1.Value() <<"\n";

  //////////////////////////////////////////////////////////////////////////
  /// Second flip Y
  transformer1.Perform(step1ModifiedShape);
  if(!transformer1.IsDone()) {
    di << "Not Done1 \n";
    return Standard_False;
  }
  TopoDS_Shape step2ModifiedShape = transformer1.ModifiedShape(step1ModifiedShape);

  //This is identity matrix for values but for type is gp_Rotation ?!
  gp_Trsf2d mirror11 = mirror1;
  mirror11.PreMultiply(mirror1);

  BRepExtrema_DistShapeShape distShapeShape2(grossPlateFace,step2ModifiedShape);//,Extrema_ExtFlag_MIN);
  if(!distShapeShape2.IsDone())
    return Standard_False;

  //This last test case give error (the value is 1008.8822038689706)
  if(distShapeShape2.Value() > 0.01) {
    di  << "Wrong Dist2 = " <<distShapeShape2.Value() <<"\n";
    Standard_Integer N = distShapeShape2.NbSolution();
    di << "Nb = " <<N <<"\n";
    for (Standard_Integer i=1;i <= N;i++)
        di <<"Sol(" <<i<<") = " <<distShapeShape2.PointOnShape1(i).Distance(distShapeShape2.PointOnShape2(i)) <<"\n";
    return Standard_False;
  }
  di << "Distance2 = " <<distShapeShape2.Value() <<"\n";
 
  return Standard_True;
}
static Standard_Integer OCC23774(Draw_Interpretor& di, Standard_Integer n, const char** a)
{ 

  if (n != 3) {
	di <<"OCC23774: invalid number of input parameters\n";
	return 1;
  }

  const char *ns1 = (a[1]), *ns2 = (a[2]);
  TopoDS_Shape S1(DBRep::Get(ns1)), S2(DBRep::Get(ns2));
  if (S1.IsNull() || S2.IsNull()) {
	di <<"OCC23774: Null input shapes\n";
	return 1;
  }
  const TopoDS_Face& aFace  = TopoDS::Face(S1);
  if(!OCC23774Test(aFace, S2, di))
	di << "Something is wrong\n";

 return 0;
}

#include <GeomConvert_ApproxSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Draw.hxx>
#include <OSD_Thread.hxx>

struct GeomConvertTest_Data
{
  Standard_Integer nbupoles;
  Handle(Geom_Surface) surf;
};

static Standard_Address GeomConvertTest (Standard_Address data)
{
  GeomConvertTest_Data* info = (GeomConvertTest_Data*)data;

  GeomConvert_ApproxSurface aGAS (info->surf, 1e-4, GeomAbs_C1, GeomAbs_C1, 9, 9, 100, 1);
  if (!aGAS.IsDone()) {
    cout << "Error: ApproxSurface is not done!" << endl;
    return 0;
  }
  const Handle(Geom_BSplineSurface)& aBSurf = aGAS.Surface();
  if (aBSurf.IsNull()) {
    cout << "Error: BSplineSurface is not created!" << endl;
    return 0;
  }
  cout << "Number of UPoles:" << aBSurf->NbUPoles();
  if (aBSurf->NbUPoles() == info->nbupoles)
  {
    cout << ": OK" << endl;
    return data; // any non-null pointer
  }
  else
  {
    cout << ": Error, must be " << info->nbupoles << endl;
    return 0;
  }
}

static Standard_Integer OCC23952sweep (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3) {
    cout << "Error: invalid number of arguments" << endl;
    return 1;
  }

  struct GeomConvertTest_Data aStorage;
  aStorage.nbupoles = Draw::Atoi(argv[1]); 
  aStorage.surf = DrawTrSurf::GetSurface(argv[2]);
  if (aStorage.surf.IsNull())
  {
    cout << "Error: " << argv[2] << " is not a DRAW surface!" << endl;
    return 0;
  }

  // start conversion in several threads
  const int NBTHREADS = 100;
  OSD_Thread aThread[NBTHREADS];
  for (int i=0; i < NBTHREADS; i++)
  { 
    aThread[i].SetFunction (GeomConvertTest);
    if (!aThread[i].Run(&aStorage))
      di << "Error: Cannot start thread << " << i << "\n";
  }

  // check results
  for (int i=0; i < NBTHREADS; i++)
  { 
    Standard_Address aResult = 0;
    if (!aThread[i].Wait(aResult))
      di << "Error: Failed waiting for thread << " << i << "\n";
    if (!aResult) 
      di << "Error: wrong number of poles in thread " << i << "!\n";
  }

  return 0;
}

#include <GeomInt_IntSS.hxx>

struct GeomIntSSTest_Data
{
  Standard_Integer nbsol;
  Handle(Geom_Surface) surf1, surf2;
};

static Standard_Address GeomIntSSTest (Standard_Address data)
{
  GeomIntSSTest_Data* info = (GeomIntSSTest_Data*)data;
  GeomInt_IntSS anInter;
  anInter.Perform (info->surf1, info->surf2, Precision::Confusion(), Standard_True);
  if (!anInter.IsDone()) {
    cout << "An intersection is not done!" << endl;
    return 0;
  }

  cout << "Number of Lines:" << anInter.NbLines();
  if (anInter.NbLines() == info->nbsol)
  {
    cout << ": OK" << endl;
    return data; // any non-null pointer
  }
  else
  {
    cout << ": Error, must be " << info->nbsol << endl;
    return 0;
  }
}

static Standard_Integer OCC23952intersect (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 4) {
    cout << "Error: invalid number of arguments" << endl;
    return 1;
  }

  struct GeomIntSSTest_Data aStorage;
  aStorage.nbsol = Draw::Atoi(argv[1]); 
  aStorage.surf1 = DrawTrSurf::GetSurface(argv[2]);
  aStorage.surf2 = DrawTrSurf::GetSurface(argv[3]);
  if (aStorage.surf1.IsNull() || aStorage.surf2.IsNull())
  {
    cout << "Error: Either " << argv[2] << " or " << argv[3] << " is not a DRAW surface!" << endl;
    return 0;
  }

  // start conversion in several threads
  const int NBTHREADS = 100;
  OSD_Thread aThread[NBTHREADS];
  for (int i=0; i < NBTHREADS; i++)
  { 
    aThread[i].SetFunction (GeomIntSSTest);
    if (!aThread[i].Run(&aStorage))
      di << "Error: Cannot start thread << " << i << "\n";
  }

  // check results
  for (int i=0; i < NBTHREADS; i++)
  { 
    Standard_Address aResult = 0;
    if (!aThread[i].Wait(aResult))
      di << "Error: Failed waiting for thread << " << i << "\n";
    if (!aResult) 
      di << "Error: wrong number of intersections in thread " << i << "!\n"; 
  }

  return 0;
}

#include <Geom_SurfaceOfRevolution.hxx> 
static Standard_Integer OCC23683 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 2) {
    di<<"Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }

  Standard_Integer ucontinuity = 1;
  Standard_Integer vcontinuity = 1;
  Standard_Boolean iscnu = false;
  Standard_Boolean iscnv = false;
  
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(argv[1]);

  QCOMPARE (aSurf->IsCNu (ucontinuity), iscnu);
  QCOMPARE (aSurf->IsCNv (vcontinuity), iscnv);

  return 0;
}

#include <gp_Ax1.hxx>
#include <gp_Ax22d.hxx>
#include <Geom_Plane.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <Geom2d_OffsetCurve.hxx>

static int test_offset(Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  // Check the command arguments
  if ( argc != 1 )
  {
    di << "Error: " << argv[0] << " - invalid number of arguments\n";
    di << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  gp_Ax1 RotoAx( gp::Origin(), gp::DZ() );
  gp_Ax22d Ax2( gp::Origin2d(), gp::DY2d(), gp::DX2d() );
  Handle(Geom_Surface) Plane = new Geom_Plane( gp::YOZ() );

  di << "<<<< Preparing sample surface of revolution based on trimmed curve >>>>\n";
  di << "-----------------------------------------------------------------------\n";

  Handle(Geom2d_Circle) C2d1 = new Geom2d_Circle(Ax2, 1.0);
  Handle(Geom2d_TrimmedCurve) C2d1Trimmed = new Geom2d_TrimmedCurve(C2d1, 0.0, M_PI/2.0);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(C2d1Trimmed, Plane);

  DBRep::Set("e1", E1);

  BRepPrimAPI_MakeRevol aRevolBuilder1(E1, RotoAx);
  TopoDS_Face F1 = TopoDS::Face( aRevolBuilder1.Shape() );

  DBRep::Set("f1", F1);

  di << "Result: f1\n";

  di << "<<<< Preparing sample surface of revolution based on offset curve  >>>>\n";
  di << "-----------------------------------------------------------------------\n";

  Handle(Geom2d_OffsetCurve) C2d2Offset = new Geom2d_OffsetCurve(C2d1Trimmed, -0.5);
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(C2d2Offset, Plane);

  DBRep::Set("e2", E2);

  BRepPrimAPI_MakeRevol aRevolBuilder2(E2, RotoAx);
  TopoDS_Face F2 = TopoDS::Face( aRevolBuilder2.Shape() );

  DBRep::Set("f2", F2);

  di << "Result: f2\n";

  return 0;
}

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
//=======================================================================
//function : OCC24008
//purpose  : 
//=======================================================================
static Standard_Integer OCC24008 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 3) {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }
  Handle(Geom_Curve) aCurve = DrawTrSurf::GetCurve(argv[1]);
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(argv[2]);
  if (aCurve.IsNull()) {
    di << "Curve was not read\n";
	return 1;
  }
  if (aSurf.IsNull()) {
	di << "Surface was not read\n";
	return 1;
  }
  ShapeConstruct_ProjectCurveOnSurface aProj;
  aProj.Init (aSurf, Precision::Confusion());
  try {
    Handle(Geom2d_Curve) aPCurve;
    aProj.Perform (aCurve, aCurve->FirstParameter(), aCurve->LastParameter(), aPCurve);
    if (aPCurve.IsNull()) {
	  di << "PCurve was not created\n";
	  return 1;
    }
  } catch (...) {
    di << "Exception was caught\n";
  }
  return 0;
}

#include <GeomAdaptor_Surface.hxx>
#include <Draw.hxx>
//=======================================================================
//function : OCC23945
//purpose  : 
//=======================================================================

static Standard_Integer OCC23945 (Draw_Interpretor& /*di*/,Standard_Integer n, const char** a)
{
  if (n < 5) return 1;

  Handle(Geom_Surface) aS = DrawTrSurf::GetSurface(a[1]);
  if (aS.IsNull()) return 1;

  GeomAdaptor_Surface GS(aS);

  Standard_Real U = Draw::Atof(a[2]);
  Standard_Real V = Draw::Atof(a[3]);

  Standard_Boolean DrawPoint = ( n%3 == 2);
  if ( DrawPoint) n--;

  gp_Pnt P;
  if (n >= 13) {
    gp_Vec DU,DV;
    if (n >= 22) {
      gp_Vec D2U,D2V,D2UV;
      GS.D2(U,V,P,DU,DV,D2U,D2V,D2UV);
      Draw::Set(a[13],D2U.X());
      Draw::Set(a[14],D2U.Y());
      Draw::Set(a[15],D2U.Z());
      Draw::Set(a[16],D2V.X());
      Draw::Set(a[17],D2V.Y());
      Draw::Set(a[18],D2V.Z());
      Draw::Set(a[19],D2UV.X());
      Draw::Set(a[20],D2UV.Y());
      Draw::Set(a[21],D2UV.Z());
    }
    else
      GS.D1(U,V,P,DU,DV);

    Draw::Set(a[7],DU.X());
    Draw::Set(a[8],DU.Y());
    Draw::Set(a[9],DU.Z());
    Draw::Set(a[10],DV.X());
    Draw::Set(a[11],DV.Y());
    Draw::Set(a[12],DV.Z());
  }
  else 
    GS.D0(U,V,P);

  if ( n > 6) {
    Draw::Set(a[4],P.X());
    Draw::Set(a[5],P.Y());
    Draw::Set(a[6],P.Z());
  }
  if ( DrawPoint) {
    DrawTrSurf::Set(a[n],P);
  }

  return 0;
}

//=======================================================================
//function : OCC11758
//purpose  : 
//=======================================================================
static Standard_Integer OCC11758 (Draw_Interpretor& di, Standard_Integer n, const char**)
{
  if (n != 1) return 1;

  const char* theStr = "0123456789";
  Standard_Integer i, j;
  for ( i = 0; i < 5; ++i ) {
    // TCollection_AsciiString(const Standard_CString astring)
    TCollection_AsciiString a(theStr+i);
    // IsEqual (const Standard_CString other)const
    //assert( a == theStr+i );
    QCOMPARE ( a , theStr+i );

    //TCollection_AsciiString(const Standard_CString astring,const Standard_Integer aLen )
    TCollection_AsciiString b(theStr+i, 3);
    //assert( b.Length() == 3 );
    //assert( strncmp( b.ToCString(), theStr+i, 3 ) == 0 );
    //assert( strlen( b.ToCString() ) == 3 );
    QCOMPARE ( b.Length() , 3 );
    QCOMPARE ( strncmp( b.ToCString() , theStr+i, 3 ) , 0 );
    QCOMPARE ( b.Length() , 3 );

    //TCollection_AsciiString(const Standard_Integer aValue)
    TCollection_AsciiString c(i);
    //assert( c.IsIntegerValue() );
    //assert( c.IntegerValue() == i );
    QCOMPARE ( c.IsIntegerValue() , Standard_True );
    QCOMPARE ( c.IntegerValue() , i );

    //TCollection_AsciiString(const Standard_Real aValue)
    TCollection_AsciiString d( 0.1*i );
    //assert( d.IsRealValue() );
    //assert( TCollection_AsciiString(3.3) == "3.3");
    QCOMPARE ( d.IsRealValue() , Standard_True );
    QCOMPARE ( TCollection_AsciiString(3.3) , "3.3" );

    //TCollection_AsciiString(const TCollection_AsciiString& astring)
    TCollection_AsciiString e(d);
    //assert( e == d );
    //assert( e.Length() == d.Length() );
    //assert( strcmp( e.ToCString(), d.ToCString() ) == 0 );
    QCOMPARE ( e ,d  );
    QCOMPARE ( e.Length() , d.Length() );
    QCOMPARE ( strcmp( e.ToCString(), d.ToCString() ) , 0 );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const Standard_Character other )
    TCollection_AsciiString f(e,'\a');
    //assert( f.Length() == e.Length() + 1 );
    //assert( strncmp( f.ToCString(), e.ToCString(), e.Length() ) == 0 );
    //assert( f.Value( f.Length() ) == '\a');
    QCOMPARE ( f.Length() , e.Length() + 1 );
    QCOMPARE ( strncmp( f.ToCString(), e.ToCString(), e.Length() ) , 0 );
    QCOMPARE ( f.Value( f.Length() ) , '\a' );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const Standard_CString other )
    TCollection_AsciiString g(f, theStr);
    //assert( g.Length() == f.Length() + strlen( theStr ));
    //assert( strncmp( g.ToCString(), f.ToCString(), f.Length() ) == 0 );
    //assert( g.Search( theStr ) == f.Length() + 1 );
    QCOMPARE ( g.Length() , f.Length() + (Standard_Integer)strlen( theStr ) );
    QCOMPARE ( strncmp( g.ToCString(), f.ToCString(), f.Length() ) , 0 );
    QCOMPARE ( g.Search( theStr ) , f.Length() + 1 );

    // TCollection_AsciiString(const TCollection_AsciiString& astring ,
    //                         const TCollection_AsciiString& other )
    TCollection_AsciiString h(d,a);
    //assert( h.Length() == d.Length() + a.Length() );
    //assert( strncmp( h.ToCString(), d.ToCString(), d.Length() ) == 0 );
    //assert( strncmp( h.ToCString() + d.Length(), a.ToCString(), a.Length() ) == 0 );
    QCOMPARE ( h.Length() , d.Length() + a.Length() );
    QCOMPARE ( strncmp( h.ToCString(), d.ToCString(), d.Length() ) , 0 );
    QCOMPARE ( strncmp( h.ToCString() + d.Length(), a.ToCString(), a.Length() ) , 0 );

    // AssignCat(const Standard_CString other)
    c.AssignCat( a.ToCString() );
    //assert( c.Length() == 1 + a.Length() );
    //assert( c.Search( a ) == 2 );
    QCOMPARE ( c.Length() , 1 + a.Length() );
    QCOMPARE ( c.Search( a ) , 2 );

    // AssignCat(const TCollection_AsciiString& other)
    Standard_Integer dl = d.Length();
    d.AssignCat( a );
    //assert( d.Length() == dl + a.Length() );
    //assert( d.Search( a ) == dl + 1 );
    QCOMPARE ( d.Length() , dl + a.Length() );
    QCOMPARE ( d.Search( a ) , dl + 1 );

    // Capitalize()
    TCollection_AsciiString capitalize("aBC");
    capitalize.Capitalize();
    //assert( capitalize == "Abc" );
    QCOMPARE ( capitalize , "Abc" );

    // Copy(const Standard_CString fromwhere)
    d = theStr+i;
    //assert( d == theStr+i );
    QCOMPARE ( d , theStr+i );

    // Copy(const TCollection_AsciiString& fromwhere)
    d = h;
    // IsEqual (const TCollection_AsciiString& other)const
    //assert( d == h );
    QCOMPARE ( d , h );

    // Insert(const Standard_Integer where, const Standard_CString what)
    dl = d.Length();
    d.Insert( 2, theStr );
    //assert( d.Length() == dl + strlen( theStr ));
    //assert( strncmp( d.ToCString() + 1, theStr, strlen( theStr )) == 0 );
    QCOMPARE ( d.Length() , dl + (Standard_Integer)strlen( theStr ) );
    QCOMPARE ( strncmp( d.ToCString() + 1, theStr, strlen( theStr )) , 0 );

    //Insert(const Standard_Integer where,const Standard_Character what)
    d = theStr;
    d.Insert( i+1, 'i' );
    //assert( d.Length() == strlen( theStr ) + 1 );
    //assert( d.Value( i+1 ) == 'i');
    //assert( strcmp( d.ToCString() + i + 1, theStr+i ) == 0 );
    QCOMPARE ( d.Length() , (Standard_Integer)strlen( theStr ) + 1 );
    QCOMPARE ( d.Value( i+1 ) , 'i' );
    QCOMPARE ( strcmp( d.ToCString() + i + 1, theStr+i ) , 0 );

    //Insert(const Standard_Integer where,const TCollection_AsciiString& what)
    d = theStr;
    d.Insert( i+1, TCollection_AsciiString( "i" ));
    //assert( d.Length() == strlen( theStr ) + 1 );
    //assert( d.Value( i+1 ) == 'i');
    //assert( strcmp( d.ToCString() + i + 1, theStr+i ) == 0 );
    QCOMPARE ( d.Length() , (Standard_Integer)strlen( theStr ) + 1 );
    QCOMPARE ( d.Value( i+1 ) , 'i' );
    QCOMPARE ( strcmp( d.ToCString() + i + 1, theStr+i ) , 0 );

    // IsDifferent (const Standard_CString other)const
    //assert( d.IsDifferent( theStr ));
    //assert( d.IsDifferent( "theStr" ));
    //assert( d.IsDifferent( "" ));
    //assert( !d.IsDifferent( d.ToCString() ));
    QCOMPARE ( d.IsDifferent( theStr ) , Standard_True );
    QCOMPARE ( d.IsDifferent( "theStr" ) , Standard_True );
    QCOMPARE ( d.IsDifferent( "" ) , Standard_True );
    QCOMPARE ( !d.IsDifferent( d.ToCString() ) , Standard_True );

    // IsDifferent (const TCollection_AsciiString& other)const
    //assert( d.IsDifferent( TCollection_AsciiString() ));
    //assert( d.IsDifferent( a ));
    //assert( d.IsDifferent( h ));
    //assert( !d.IsDifferent( d ));
    QCOMPARE ( d.IsDifferent( TCollection_AsciiString() ) , Standard_True );
    QCOMPARE ( d.IsDifferent( a ) , Standard_True );
    QCOMPARE ( d.IsDifferent( h ) , Standard_True );
    QCOMPARE ( !d.IsDifferent( d ) , Standard_True );

    // IsLess (const Standard_CString other)const
    //assert( TCollection_AsciiString ("0"). IsLess("1"));
    //assert( TCollection_AsciiString ("0"). IsLess("00"));
    //assert( TCollection_AsciiString ("").  IsLess("0"));
    //assert( !TCollection_AsciiString("1"). IsLess("0"));
    //assert( !TCollection_AsciiString("00").IsLess("0"));
    //assert( !TCollection_AsciiString("0"). IsLess(""));
    //assert( TCollection_AsciiString (theStr+i).IsLess(theStr+i+1));
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess("1") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess("00") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("").  IsLess("0") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("1"). IsLess("0"), Standard_True );
    QCOMPARE ( !TCollection_AsciiString("00").IsLess("0") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsLess("") , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i).IsLess(theStr+i+1) , Standard_True );

    // IsLess (const TCollection_AsciiString& other)const
    //assert( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("1" )));
    //assert( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("00")));
    //assert( TCollection_AsciiString ("").  IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("1"). IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("00").IsLess(TCollection_AsciiString("0" )));
    //assert( !TCollection_AsciiString("0"). IsLess(TCollection_AsciiString(""  )));
    //assert( TCollection_AsciiString (theStr+i).IsLess(TCollection_AsciiString(theStr+i+1)));
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("1" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsLess(TCollection_AsciiString("00")) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("").  IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("1"). IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("00").IsLess(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsLess(TCollection_AsciiString(""  )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i).IsLess(TCollection_AsciiString(theStr+i+1)) , Standard_True );

    // IsGreater (const Standard_CString other)const
    //assert( !TCollection_AsciiString("0"). IsGreater("1"));
    //assert( !TCollection_AsciiString("0"). IsGreater("00"));
    //assert( !TCollection_AsciiString("").  IsGreater("0"));
    //assert( TCollection_AsciiString ("1"). IsGreater("0"));
    //assert( TCollection_AsciiString ("00").IsGreater("0"));
    //assert( TCollection_AsciiString ("0"). IsGreater(""));
    //assert( TCollection_AsciiString (theStr+i+1).IsGreater(theStr+i));
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater("1") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater("00") , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("").  IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("1"). IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("00").IsGreater("0") , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsGreater("") , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i+1).IsGreater(theStr+i) , Standard_True );

    // IsGreater (const TCollection_AsciiString& other)const
    //assert( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("1" )));
    //assert( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("00")));
    //assert( !TCollection_AsciiString("").  IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("1"). IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("00").IsGreater(TCollection_AsciiString("0" )));
    //assert( TCollection_AsciiString ("0"). IsGreater(TCollection_AsciiString(""  )));
    //assert( TCollection_AsciiString (theStr+i+1).IsGreater(TCollection_AsciiString(theStr+i)));
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("1" )) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("0"). IsGreater(TCollection_AsciiString("00")) , Standard_True );
    QCOMPARE ( !TCollection_AsciiString("").  IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("1"). IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("00").IsGreater(TCollection_AsciiString("0" )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString ("0"). IsGreater(TCollection_AsciiString(""  )) , Standard_True );
    QCOMPARE ( TCollection_AsciiString (theStr+i+1).IsGreater(TCollection_AsciiString(theStr+i)) , Standard_True );

    // void Read(Standard_IStream& astream)
    std::istringstream is( theStr );
    e.Read( is );
    //assert( e == theStr );
    QCOMPARE ( e , theStr );

    // Standard_Integer SearchFromEnd (const Standard_CString what)const
    //assert( e.SearchFromEnd( theStr + i ) == i + 1 );
    QCOMPARE ( e.SearchFromEnd( theStr + i ) , i + 1 );

    // SetValue(const Standard_Integer where, const Standard_CString what)
    e.SetValue( i+1, "what");
    //assert( e.Search( "what" ) == i+1 );
    //assert( e.Length() == strlen( theStr ));
    QCOMPARE ( e.Search( "what" ) , i+1 );
    QCOMPARE ( e.Length() , (Standard_Integer)strlen( theStr ) );

    // TCollection_AsciiString Split (const Standard_Integer where)
    e = theStr;
    d = e.Split( i+1 );
    //assert( d.Length() + e.Length() == strlen( theStr ));
    QCOMPARE ( d.Length() + e.Length() , (Standard_Integer)strlen( theStr ) );

    // TCollection_AsciiString SubString (const Standard_Integer FromIndex,
    //                                    const Standard_Integer ToIndex) const
    e = theStr;
    d = e.SubString( (unsigned int)i+1, (unsigned int)i+3 );
    //assert( d.Length() == 3 );
    //assert( d.Value(1) == theStr[ i ]);
    QCOMPARE ( d.Length() , 3 );
    QCOMPARE ( d.Value(1) , theStr[ i ] );

    // TCollection_AsciiString Token (const Standard_CString separators,
    //                                const Standard_Integer whichone) const
    e = " ";
    for ( j = 0; j < i; ++j ) {
      e += TCollection_AsciiString( theStr[j] ) + " ";
      //assert( e.Token(" ", j+1 ) == TCollection_AsciiString( theStr+j, 1 ));
      QCOMPARE ( e.Token(" ", j+1 ) , TCollection_AsciiString( theStr+j, 1 ) );
    }
  }
  for ( i = 0; i < 5; ++i )
  {
    // TCollection_ExtendedString (const Standard_CString astring, 
    //                             const Standard_Boolean isMultiByte) 
    const TCollection_ExtendedString a( theStr+i );
    //assert( TCollection_AsciiString( a ) == theStr+i );
    QCOMPARE ( TCollection_AsciiString( a ) , theStr+i );

    //TCollection_ExtendedString (const Standard_ExtString astring)
    const TCollection_ExtendedString b( a.ToExtString() );
    //assert( a == b );
    QCOMPARE ( a , b );

    // TCollection_ExtendedString (const Standard_Integer      length,
    //                             const Standard_ExtCharacter filler )
    const TCollection_ExtendedString c( i, 1 );
    //assert( c.Length() == i );
    QCOMPARE ( c.Length() , i );
    if ( c.Length() > 0 ) {
      //assert( c.Value( i ) == 1 );
      QCOMPARE ( c.Value( i ) , 1 );
    }

    // TCollection_ExtendedString (const Standard_Integer aValue)
    TCollection_ExtendedString d( i );
    const TCollection_AsciiString da( d );
    //assert( da.IsIntegerValue() );
    //assert( da.IntegerValue() == i );
    QCOMPARE ( da.IsIntegerValue() , Standard_True );
    QCOMPARE (  da.IntegerValue(), i );

    // TCollection_ExtendedString (const Standard_Real aValue)
    const TCollection_ExtendedString e( 0.1 * i );
    const TCollection_AsciiString ea( e );
    //assert( ea.IsRealValue() );
    //assert( Abs( ea.RealValue() - 0.1 * i ) < 1e-10 );
    QCOMPARE ( ea.IsRealValue() , Standard_True );
    QCOMPARE ( Abs( ea.RealValue() - 0.1 * i ) < 1e-10 , Standard_True );

    // TCollection_ExtendedString (const TCollection_ExtendedString& astring)
    const TCollection_ExtendedString f(e);
    //assert( f.Length() == e.Length());
    //assert( f == e );
    QCOMPARE ( f.Length() , e.Length() );
    QCOMPARE ( f , e );

    // TCollection_ExtendedString (const TCollection_AsciiString& astring)
    const TCollection_ExtendedString g( ea );
    //assert( g.Length() == ea.Length() );
    //assert( TCollection_AsciiString( g ) == ea );
    QCOMPARE ( g.Length() , ea.Length() );
    QCOMPARE ( TCollection_AsciiString( g ) , ea );

    // AssignCat (const TCollection_ExtendedString& other)
    const TCollection_ExtendedString sep(",");
    d.AssignCat( sep );
    d.AssignCat( g );
    //assert( d.Length() == 2 + g.Length() );
    //assert( d.Token( sep.ToExtString(), 1 ) == TCollection_ExtendedString( i ));
    //assert( d.Token( sep.ToExtString(), 2 ) == g );
    QCOMPARE ( d.Length() , 2 + g.Length() );
    QCOMPARE ( d.Token( sep.ToExtString(), 1 ) , TCollection_ExtendedString( i ) );
    QCOMPARE ( d.Token( sep.ToExtString(), 2 ) , g );

    // TCollection_ExtendedString Cat (const TCollection_ExtendedString& other) const
    const TCollection_ExtendedString cat = a.Cat( sep );
    //assert( cat.Length() == a.Length() + sep.Length() );
    //assert( cat.Search( a ) == 1 );
    //assert( cat.Search( sep ) == a.Length() + 1 );
    QCOMPARE ( cat.Length() , a.Length() + sep.Length() );
    QCOMPARE ( cat.Search( a ) , 1 );
    QCOMPARE ( cat.Search( sep ) , a.Length() + 1 );

    // Copy (const TCollection_ExtendedString& fromwhere)
    d = cat;
    //assert( d.Length() == cat.Length() );
    //assert( d == cat );
    QCOMPARE ( d.Length() , cat.Length() );
    QCOMPARE ( d , cat );

    // IsEqual (const Standard_ExtString other) const
    //assert( d.IsEqual( d.ToExtString() ));
    QCOMPARE ( d.IsEqual( d.ToExtString() ) , Standard_True );

    // IsDifferent (const Standard_ExtString other ) const
    //assert( d.IsDifferent( a.ToExtString() ));
    QCOMPARE ( d.IsDifferent( a.ToExtString() ) , Standard_True );

    // IsDifferent (const TCollection_ExtendedString& other) const
    //assert( d.IsDifferent( a ));
    QCOMPARE ( d.IsDifferent( a ) , Standard_True );

    // IsLess (const Standard_ExtString other) const
    const TCollection_ExtendedString l0("0"), l1("1"), l00("00"), l, ls(theStr+i), ls1(theStr+i+1);
    //assert( l0. IsLess( l1.ToExtString() ));
    //assert( l0. IsLess( l00.ToExtString() ));
    //assert( l.  IsLess( l0.ToExtString() ));
    //assert( ! l1. IsLess( l0.ToExtString() ));
    //assert( ! l00.IsLess( l0.ToExtString() ));
    //assert( ! l0. IsLess( l.ToExtString() ));
    //assert( ls.IsLess( ls1.ToExtString() ));
    QCOMPARE ( l0. IsLess( l1.ToExtString() ) , Standard_True );
    QCOMPARE ( l0. IsLess( l00.ToExtString() ) , Standard_True );
    QCOMPARE ( l.  IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l1. IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l00.IsLess( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l0. IsLess( l.ToExtString() ) , Standard_True );
    QCOMPARE ( ls.IsLess( ls1.ToExtString() ) , Standard_True );

    // IsLess (const TCollection_ExtendedString& other) const
    //assert( l0. IsLess( l1 ));
    //assert( l0. IsLess( l00 ));
    //assert( l.  IsLess( l0 ));
    //assert( ! l1. IsLess( l0 ));
    //assert( ! l00.IsLess( l0 ));
    //assert( ! l0. IsLess( l ));
    //assert( ls.IsLess( ls1 ));
    QCOMPARE ( l0. IsLess( l1 ) , Standard_True );
    QCOMPARE ( l0. IsLess( l00 ) , Standard_True );
    QCOMPARE ( l.  IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l1. IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l00.IsLess( l0 ) , Standard_True );
    QCOMPARE ( ! l0. IsLess( l ) , Standard_True );
    QCOMPARE ( ls.IsLess( ls1 ) , Standard_True );

    // IsGreater (const Standard_ExtString other) const
    //assert( ! l0.IsGreater( l1.ToExtString() ));
    //assert( ! l0.IsGreater( l00.ToExtString() ));
    //assert( ! l. IsGreater( l0.ToExtString() ));
    //assert(  l1. IsGreater( l0.ToExtString() ));
    //assert(  l00.IsGreater( l0.ToExtString() ));
    //assert(  l0. IsGreater( l.ToExtString() ));
    //assert(  ls1.IsGreater( ls.ToExtString() ));
    QCOMPARE ( ! l0.IsGreater( l1.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l0.IsGreater( l00.ToExtString() ) , Standard_True );
    QCOMPARE ( ! l. IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l1. IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l00.IsGreater( l0.ToExtString() ) , Standard_True );
    QCOMPARE ( l0. IsGreater( l.ToExtString() ) , Standard_True );
    QCOMPARE ( ls1.IsGreater( ls.ToExtString() ) ,Standard_True  );

    // IsGreater (const TCollection_ExtendedString& other) const
    //assert( ! l0.IsGreater( l1));
    //assert( ! l0.IsGreater( l00));
    //assert( ! l. IsGreater( l0));
    //assert(  l1. IsGreater( l0));
    //assert(  l00.IsGreater( l0));
    //assert(  l0. IsGreater( l));
    //assert(  ls1.IsGreater( ls));
    QCOMPARE ( ! l0.IsGreater( l1) , Standard_True );
    QCOMPARE ( ! l0.IsGreater( l00) , Standard_True );
    QCOMPARE ( ! l. IsGreater( l0) , Standard_True );
    QCOMPARE ( l1. IsGreater( l0) , Standard_True );
    QCOMPARE ( l00.IsGreater( l0) , Standard_True );
    QCOMPARE ( l0. IsGreater( l) , Standard_True );
    QCOMPARE ( ls1.IsGreater( ls) , Standard_True );

    // ==========================
    //TCollection_HAsciiString::
    // ==========================

    // IsDifferent(const Handle(TCollection_HAsciiString)& S)
    Handle(TCollection_HAsciiString) ha1 = new TCollection_HAsciiString( theStr+i );
    Handle(TCollection_HAsciiString) ha2 = new TCollection_HAsciiString( theStr+i+1 );
    //assert( ha1->IsDifferent( ha2 ));
    //assert( !ha1->IsDifferent( ha1 ));
    QCOMPARE ( ha1->IsDifferent( ha2 ) , Standard_True );
    QCOMPARE ( !ha1->IsDifferent( ha1 ) , Standard_True );

    // IsSameString (const Handle(TCollection_HAsciiString)& S)
    //assert( !ha1->IsSameString( ha2 ));
    //assert( ha1->IsSameString( ha1 ));
    QCOMPARE ( !ha1->IsSameString( ha2 ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1 ) , Standard_True );

    // IsSameState (const Handle(TCollection_HAsciiString)& other) const
    //assert( !ha1->IsSameState( ha2 ));
    //assert( ha1->IsSameState( ha1 ));
    QCOMPARE ( !ha1->IsSameState( ha2 ) , Standard_True );
    QCOMPARE ( ha1->IsSameState( ha1 ) , Standard_True );

    // IsSameString (const Handle(TCollection_HAsciiString)& S ,
    //               const Standard_Boolean CaseSensitive) const
    //assert( !ha1->IsSameString( ha2, true ));
    //assert( ha1->IsSameString( ha1, true ));
    //assert( !ha1->IsSameString( ha2, false ));
    //assert( ha1->IsSameString( ha1, false ));
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_True ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1, Standard_True ) , Standard_True );
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_False ) , Standard_True );
    QCOMPARE ( ha1->IsSameString( ha1, Standard_False ) , Standard_True );

    ha1->SetValue( 1, "AbC0000000");
    ha2->SetValue( 1, "aBc0000000");
    //assert( !ha1->IsSameString( ha2, true ));
    //assert( ha1->IsSameString( ha2, false ));
    QCOMPARE ( !ha1->IsSameString( ha2, Standard_True ) , Standard_True );
    QCOMPARE (  ha1->IsSameString( ha2, Standard_False ), Standard_True );
  }
  return 0;
}

#include <Geom_CylindricalSurface.hxx>
#include <IntTools_FaceFace.hxx>
#include <IntTools_Curve.hxx>
#include <IntTools_PntOn2Faces.hxx>

static Standard_Integer OCC24005 (Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv) 
{
  if(theNArg < 2)
  {
    theDI << "Wrong a number of arguments!\n";
    return 1;
  }

  Handle(Geom_Plane) plane(new Geom_Plane(
                                  gp_Ax3( gp_Pnt(-72.948737453424499, 754.30437716359393, 259.52151854671678),
                                  gp_Dir(6.2471473085930200e-007, -0.99999999999980493, 0.00000000000000000),
                                  gp_Dir(0.99999999999980493, 6.2471473085930200e-007, 0.00000000000000000))));
  Handle(Geom_CylindricalSurface) cylinder(
                  new Geom_CylindricalSurface(
                                  gp_Ax3(gp_Pnt(-6.4812490053250649, 753.39408794522092, 279.16400974257465),
                                  gp_Dir(1.0000000000000000, 0.0, 0.00000000000000000),
                                  gp_Dir(0.0, 1.0000000000000000, 0.00000000000000000)),
                                                                                          19.712534607908712));

  DrawTrSurf::Set("pln", plane);
  theDI << "pln\n";
  DrawTrSurf::Set("cyl", cylinder);
  theDI << "cyl\n";

  BRep_Builder builder;
  TopoDS_Face face1, face2;
  builder.MakeFace(face1, plane, Precision::Confusion());
  builder.MakeFace(face2, cylinder, Precision::Confusion());
  IntTools_FaceFace anInters;
  anInters.SetParameters(false, true, true, Precision::Confusion());
  anInters.Perform(face1, face2);

  if (!anInters.IsDone())
  {
    theDI<<"No intersections found!\n";

    return 1;
  }

  //Handle(Geom_Curve) aResult;
  //gp_Pnt             aPoint;

  const IntTools_SequenceOfCurves& aCvsX=anInters.Lines();
  const IntTools_SequenceOfPntOn2Faces& aPntsX=anInters.Points();

  char buf[1024];  
  Standard_Integer aNbCurves, aNbPoints;

  aNbCurves=aCvsX.Length();
  aNbPoints=aPntsX.Length();

  if (aNbCurves >= 2)
  {
    for (Standard_Integer i=1; i<=aNbCurves; ++i)
    {
      Sprintf(buf, "%s_%d",theArgv[1],i);
      theDI << buf << " ";
      
      const IntTools_Curve& aIC = aCvsX(i);
      const Handle(Geom_Curve)& aC3D= aIC.Curve();
      DrawTrSurf::Set(buf,aC3D);
    }
  }
  else if (aNbCurves == 1)
  {
    const IntTools_Curve& aIC = aCvsX(1);
    const Handle(Geom_Curve)& aC3D= aIC.Curve();
    Sprintf(buf, "%s",theArgv[1]);
    theDI << buf << " ";
    DrawTrSurf::Set(buf,aC3D);
  }

  for (Standard_Integer i = 1; i<=aNbPoints; ++i)
  {
    const IntTools_PntOn2Faces& aPi=aPntsX(i);
    const gp_Pnt& aP=aPi.P1().Pnt();
    
    Sprintf(buf,"%s_p_%d",theArgv[1],i);
    theDI << buf << " ";
    DrawTrSurf::Set(buf, aP);
  }

  return 0;
}

#include <BRepAlgo_NormalProjection.hxx>
static Standard_Integer OCC24012 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
	if (argc != 3) {
		di << "Usage : " << argv[0] << " should be 2 arguments (face and edge)";
		return 1;
	}
	
	Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
	if(myAISContext.IsNull()) {
		di << "use 'vinit' command before " << argv[0] << "\n";
		return 1;
	}

	TopoDS_Face m_Face1 = TopoDS::Face(DBRep::Get(argv[1]));
	TopoDS_Edge m_Edge = TopoDS::Edge(DBRep::Get(argv[2]));
	
	BRepAlgo_NormalProjection anormpro(m_Face1);
    anormpro.Add(m_Edge);
    anormpro.SetDefaultParams();

    //anormpro.Compute3d();
    //anormpro.SetLimit();

    anormpro.Build();

    if (anormpro.IsDone())
    {
        TopoDS_Shape rshape = anormpro.Projection();
		Handle(AIS_InteractiveObject) myShape = new AIS_Shape (rshape);
		myAISContext->SetColor (myShape, Quantity_Color(Quantity_NOC_YELLOW), Standard_False);
		myAISContext->Display (myShape, Standard_True);
    }

	return 0;
}

#include <BRepFeat_SplitShape.hxx>
#include <ShapeAnalysis_ShapeContents.hxx>
#include <BRepAlgo.hxx>
static Standard_Integer OCC24086 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
	if (argc != 3) {
		di << "Usage : " << argv[0] << " should be 2 arguments (face and wire)";
		return 1;
	}
	
	Handle(AIS_InteractiveContext) myAISContext = ViewerTest::GetAISContext();
	if(myAISContext.IsNull()) {
		di << "use 'vinit' command before " << argv[0] << "\n";
		return 1;
	}
	
	TopoDS_Shape result;
	TopoDS_Face face = TopoDS::Face(DBRep::Get(argv[1]));
	TopoDS_Wire wire = TopoDS::Wire(DBRep::Get(argv[2]));
    
	BRepFeat_SplitShape asplit(face);
	asplit.Add(wire, face);
	asplit.Build();
    result = asplit.Shape();
    ShapeAnalysis_ShapeContents ana;
    ana.Perform(result);
    ana.NbFaces();

	if (!(BRepAlgo::IsValid(result))) {
		di << "Result was checked and it is INVALID\n";
	} else {
		di << "Result was checked and it is VALID\n";
	}
	
	Handle(AIS_InteractiveObject) myShape = new AIS_Shape (result);
	myAISContext->Display(myShape, Standard_True);

	return 0;
}

#include <Geom_Circle.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Extrema_ExtPC.hxx>
#include <gp_Cylinder.hxx>
#include <ElSLib.hxx>
static Standard_Integer OCC24945 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1) {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }

  gp_Pnt aP3D( -1725.97, 843.257, -4.22741e-013 );
  gp_Ax2 aAxis( gp_Pnt( 0, 843.257, 0 ), gp_Dir( 0, -1, 0 ), gp::DX() );
  Handle(Geom_Circle) aCircle = new Geom_Circle( aAxis, 1725.9708621929999 );
  GeomAdaptor_Curve aC3D( aCircle );

  Extrema_ExtPC aExtPC( aP3D, aC3D );
  //Standard_Real aParam = (aExtPC.Point(1)).Parameter();
  gp_Pnt aProj = (aExtPC.Point(1)).Value();
  di << "Projected point: X = " << aProj.X() << "; Y = " << aProj.Y() << "; Z = " << aProj.Z() << "\n";

  // Result of deviation
  gp_Ax2 aCylAxis( gp_Pnt( 0, 2103.87, 0 ), -gp::DY(), -gp::DX() );
  gp_Cylinder aCylinder( aCylAxis, 1890. );

  Standard_Real aU = 0., aV = 0.;
  ElSLib::Parameters( aCylinder, aProj, aU, aV );
  di << "Parameters on cylinder: U = " << aU << "; V = " << aV << "\n";
  
  return 0;
}

#include <Extrema_FuncPSNorm.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
static Standard_Integer OCC24137 (Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv) 
{
  Standard_Integer anArgIter = 1;
  if (theNArg < 5)
    {
      theDI <<"Usage: " << theArgv[0] << " face vertex U V [N]\n";
      return 1;
    }

  // get target shape
  Standard_CString aFaceName = theArgv[anArgIter++];
  Standard_CString aVertName = theArgv[anArgIter++];
  const TopoDS_Shape aShapeF = DBRep::Get (aFaceName);
  const TopoDS_Shape aShapeV = DBRep::Get (aVertName);
  const Standard_Real aUFrom = Atof (theArgv[anArgIter++]);
  const Standard_Real aVFrom = Atof (theArgv[anArgIter++]);
  const Standard_Integer aNbIts = (anArgIter < theNArg) ? atol (theArgv[anArgIter++]) : 100;
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
  const TopoDS_Face   aFace = TopoDS::Face   (aShapeF);
  const TopoDS_Vertex aVert = TopoDS::Vertex (aShapeV);
  GeomAdaptor_Surface aSurf (BRep_Tool::Surface (aFace));

  gp_Pnt aPnt = BRep_Tool::Pnt (aVert), aRes;

  Extrema_FuncPSNorm    anExtFunc;
  math_FunctionSetRoot aRoot (anExtFunc, aNbIts);

  math_Vector aTolUV (1, 2), aUVinf  (1, 2), aUVsup  (1, 2), aFromUV (1, 2);
  aTolUV (1) =  Precision::Confusion(); aTolUV (2) =  Precision::Confusion();
  aUVinf (1) = -Precision::Infinite();  aUVinf (2) = -Precision::Infinite();
  aUVsup (1) =  Precision::Infinite();  aUVsup (2) =  Precision::Infinite();
  aFromUV(1) =  aUFrom; aFromUV(2) = aVFrom;

  anExtFunc.Initialize (aSurf);
  anExtFunc.SetPoint (aPnt);
  aRoot.SetTolerance (aTolUV);
  aRoot.Perform (anExtFunc, aFromUV, aUVinf, aUVsup);
  if (!aRoot.IsDone())
    {
      std::cerr << "No results!\n";
      return 1;
    }

  theDI << aRoot.Root()(1) << " " << aRoot.Root()(2) << "\n";
  
  aSurf.D0 (aRoot.Root()(1), aRoot.Root()(2), aRes);
  DBRep::Set ("result", BRepBuilderAPI_MakeVertex (aRes));
  return 0;
}

//! Check boolean operations on NCollection_Map
static Standard_Integer OCC24271 (Draw_Interpretor& di,
                                  Standard_Integer  /*theArgNb*/,
                                  const char**      /*theArgVec*/)
{
  // input data
  const Standard_Integer aLeftLower  = 1;
  const Standard_Integer aLeftUpper  = 10;
  const Standard_Integer aRightLower = 5;
  const Standard_Integer aRightUpper = 15;

  // define arguments
  NCollection_Map<Standard_Integer> aMapLeft;
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    aMapLeft.Add (aKeyIter);
  }

  NCollection_Map<Standard_Integer> aMapRight;
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    aMapRight.Add (aKeyIter);
  }

  QCOMPARE (aMapLeft .Contains (aMapRight), Standard_False);
  QCOMPARE (aMapRight.Contains (aMapLeft),  Standard_False);

  // validate Union operation
  NCollection_Map<Standard_Integer> aMapUnion;
  aMapUnion.Union (aMapLeft, aMapRight);
  QCOMPARE (aMapUnion.Extent(), aRightUpper - aLeftLower + 1);
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    QCOMPARE (aMapUnion.Contains (aKeyIter), Standard_True);
  }

  // validate Intersection operation
  NCollection_Map<Standard_Integer> aMapSect;
  aMapSect.Intersection (aMapLeft, aMapRight);
  QCOMPARE (aMapSect.Extent(), aLeftUpper - aRightLower + 1);
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    QCOMPARE (aMapSect.Contains (aKeyIter), Standard_True);
  }
  QCOMPARE (aMapLeft .Contains (aMapSect), Standard_True);
  QCOMPARE (aMapRight.Contains (aMapSect), Standard_True);

  // validate Substruction operation
  NCollection_Map<Standard_Integer> aMapSubsLR;
  aMapSubsLR.Subtraction (aMapLeft, aMapRight);
  QCOMPARE (aMapSubsLR.Extent(), aRightLower - aLeftLower);
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    QCOMPARE (aMapSubsLR.Contains (aKeyIter), Standard_True);
  }

  NCollection_Map<Standard_Integer> aMapSubsRL;
  aMapSubsRL.Subtraction (aMapRight, aMapLeft);
  QCOMPARE (aMapSubsRL.Extent(), aRightUpper - aLeftUpper);
  for (Standard_Integer aKeyIter = aLeftUpper + 1; aKeyIter < aRightUpper; ++aKeyIter)
  {
    QCOMPARE (aMapSubsRL.Contains (aKeyIter), Standard_True);
  }

  // validate Difference operation
  NCollection_Map<Standard_Integer> aMapDiff;
  aMapDiff.Difference (aMapLeft, aMapRight);
  QCOMPARE (aMapDiff.Extent(), aRightLower - aLeftLower + aRightUpper - aLeftUpper);
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    QCOMPARE (aMapDiff.Contains (aKeyIter), Standard_True);
  }
  for (Standard_Integer aKeyIter = aLeftUpper + 1; aKeyIter < aRightUpper; ++aKeyIter)
  {
    QCOMPARE (aMapDiff.Contains (aKeyIter), Standard_True);
  }

  // validate Exchange operation
  NCollection_Map<Standard_Integer> aMapSwap;
  aMapSwap.Exchange (aMapSect);
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    QCOMPARE (aMapSwap.Contains (aKeyIter), Standard_True);
  }
  QCOMPARE (aMapSect.IsEmpty(), Standard_True);
  aMapSwap.Add (34);
  aMapSect.Add (43);

  NCollection_Map<Standard_Integer> aMapCopy (aMapSwap);
  QCOMPARE (aMapCopy.IsEqual (aMapSwap), Standard_True);
  aMapCopy.Remove (34);
  aMapCopy.Add    (43);
  QCOMPARE (aMapCopy.IsEqual (aMapSwap), Standard_False);

  return 0;
}

#define QVERIFY(val1) \
  di << "Checking " #val1 " == Standard_True" << \
        ((val1) == Standard_True ? ": OK\n" : ": Error\n")

#include <GeomInt_IntSS.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Standard_ErrorHandler.hxx>
//=======================================================================
//function : OCC23972
//purpose  : 
//=======================================================================
static void DoGeomIntSSTest (const Handle(Geom_Surface)& theSurf1,
			     const Handle(Geom_Surface)& theSurf2,
			     const Standard_Integer theNbSol,
			     Draw_Interpretor& di)
{
  try {
    OCC_CATCH_SIGNALS
	 GeomInt_IntSS anInter;
	 anInter.Perform (theSurf1, theSurf2, Precision::Confusion(), Standard_True);
	 QVERIFY (anInter.IsDone());
	 QCOMPARE (anInter.NbLines(), theNbSol);
  } catch (...) {
    QVERIFY (Standard_False);
  }
}

namespace {
  static Handle(Geom_ConicalSurface) CreateCone (const gp_Pnt& theLoc,
						 const gp_Dir& theDir,
						 const gp_Dir& theXDir,
						 const Standard_Real theRad,
						 const Standard_Real theSin,
						 const Standard_Real theCos)
  {
    const Standard_Real anA = atan (theSin / theCos);
    gp_Ax3 anAxis (theLoc, theDir, theXDir);
    Handle(Geom_ConicalSurface) aSurf = new Geom_ConicalSurface (anAxis, anA, theRad);
    return aSurf;
  }
}

static Standard_Integer OCC23972 (Draw_Interpretor& di,Standard_Integer n, const char**)
{
  if (n != 1) return 1;

  //process specific cones, cannot read them from files because due to rounding the original error
  //in math_FunctionRoots gets hidden
  Handle(Geom_Surface) aS1 = CreateCone (
					 gp_Pnt (123.694345356663, 789.9, 68.15),
					 gp_Dir (-1, 3.48029791472957e-016, -8.41302743359754e-017),
					 gp_Dir (-3.48029791472957e-016, -1, -3.17572289932207e-016),
					 3.28206830417112,
					 0.780868809443031,
					 0.624695047554424);
  Handle(Geom_Surface) aS2 = CreateCone (
					 gp_Pnt (123.694345356663, 784.9, 68.15),
					 gp_Dir (-1, -2.5209507537117e-016, -1.49772808948866e-016),
					 gp_Dir (1.49772808948866e-016, 3.17572289932207e-016, -1),
					 3.28206830417112,
					 0.780868809443031,
					 0.624695047554424);
  
  DoGeomIntSSTest (aS1, aS2, 2, di);

  return 0;
}

#include <ShapeFix_EdgeProjAux.hxx>
static Standard_Integer OCC24370 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 5) {
    di<<"Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }

  TopoDS_Shape aSh = DBRep::Get(argv[1]);
  if (aSh.IsNull()) {
    di << argv[0] << " Error: Null input edge\n";
    return 1;
  }
  const TopoDS_Edge& anEdge = TopoDS::Edge (aSh);

  Handle(Geom2d_Curve) aC = DrawTrSurf::GetCurve2d(argv[2]);
  if (aC.IsNull()) {
    di << argv[0] << " Error: Null input curve\n";
    return 1;
  }

  Handle(Geom_Surface) aS = DrawTrSurf::GetSurface(argv[3]);
  if (aS.IsNull()) {
    di << argv[0] << " Error: Null input surface\n";
    return 1;
  }

  Standard_Real prec = Draw::Atof(argv[4]);
  
  //prepare data
  TopoDS_Face aFace;
  BRep_Builder aB;
  aB.MakeFace (aFace, aS, Precision::Confusion());
  aB.UpdateEdge (anEdge, aC, aFace, Precision::Confusion());
  aB.Range (anEdge, aFace, aC->FirstParameter(), aC->LastParameter());

  //call algorithm
  ShapeFix_EdgeProjAux aProj (aFace, anEdge);
  aProj.Compute (prec);
  
  Standard_Boolean isfirstdone = aProj.IsFirstDone();
  Standard_Boolean islastdone = aProj.IsLastDone();

  Standard_Real first = 0.;
  Standard_Real last = 0.;
  Standard_Integer isfirstdoneInteger = 0;
  Standard_Integer islastdoneInteger = 0;


  if (isfirstdone) {
    first = aProj.FirstParam();
    isfirstdoneInteger = 1;
  }
 
  if (islastdone) {
    last= aProj.LastParam();
    islastdoneInteger = 1;
  }

  di << isfirstdoneInteger << " "<< islastdoneInteger << " "<< first << " "<< last << " \n";

  return 0;
}

template<typename T, typename HT>
static void DoIsNull(Draw_Interpretor& di)
{
  HT aHandle;
  //    QVERIFY (aHandle.IsNull());
  QCOMPARE (aHandle.IsNull(), Standard_True);
  const T* p = aHandle.get();
#if OCC_VERSION_HEX > 0x060700
  //QVERIFY (!p);
  //QVERIFY (p == 0);
  QCOMPARE (!p, Standard_True);
  QCOMPARE (p == 0, Standard_True);
#endif

  aHandle = new T;
  //QVERIFY (!aHandle.IsNull());
  QCOMPARE (!aHandle.IsNull(), Standard_True);
  p = aHandle.get();
  //QVERIFY (p);
  //QVERIFY (p != 0);
  QCOMPARE (p != NULL, Standard_True);
  QCOMPARE (p != 0, Standard_True);
}

//=======================================================================
//function : OCC24533
//purpose  : 
//=======================================================================
static Standard_Integer OCC24533 (Draw_Interpretor& di, Standard_Integer n, const char**)
{
  if (n != 1) return 1;

  DoIsNull<Standard_Transient, Handle(Standard_Transient)>(di);

  return 0;
}

// Dummy class to test interface for compilation issues
class QABugs_HandleClass : public Standard_Transient
{
public:
  Standard_Integer HandleProc (Draw_Interpretor& , Standard_Integer  , const char** theArgVec)
  {
    std::cerr << "QABugs_HandleClass[" << this << "] " << theArgVec[0] << "\n";
    return 0;
  }
  DEFINE_STANDARD_RTTI_INLINE(QABugs_HandleClass,Standard_Transient) // Type definition
};
DEFINE_STANDARD_HANDLE    (QABugs_HandleClass, Standard_Transient)


// Dummy class to test interface for compilation issues
struct QABugs_NHandleClass
{
  Standard_Integer NHandleProc (Draw_Interpretor& , Standard_Integer  , const char** theArgVec)
  {
    std::cerr << "QABugs_NHandleClass[" << this << "] " << theArgVec[0] << "\n";
    return 0;
  }
};

#include <XCAFDoc_ColorTool.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPCAFControl_Writer.hxx>
static Standard_Integer OCC23951 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("dummy");;
  TopoDS_Shape s1 = BRepPrimAPI_MakeBox(1,1,1).Shape();
  TDF_Label lab1 = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->NewShape();
  XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->SetShape(lab1, s1);
  TDataStd_Name::Set(lab1, "Box1");
        
  Quantity_Color yellow(1,1,0, Quantity_TOC_RGB);
  XCAFDoc_DocumentTool::ColorTool (aDoc->Main())->SetColor(lab1, yellow, XCAFDoc_ColorGen);
  XCAFDoc_DocumentTool::ColorTool(aDoc->Main())->SetVisibility(lab1, 0);

  STEPControl_StepModelType mode = STEPControl_AsIs;
  STEPCAFControl_Writer writer;
  if ( ! writer.Transfer (aDoc, mode ) )
  {
    di << "The document cannot be translated or gives no result"  <<  "\n";
    return 1;
  }

  writer.Write(argv[1]);
  return 0;
}


//=======================================================================
//function : OCC23950
//purpose  :
//=======================================================================
static Standard_Integer OCC23950 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage : " << argv[0] << " step_file\n";
    return 1;
  }

  Handle(TDocStd_Document) aDoc = new TDocStd_Document ("dummy");
  TopoDS_Shape s6 = BRepBuilderAPI_MakeVertex (gp_Pnt (75, 0, 0));
  gp_Trsf t0;
  TopLoc_Location location0 (t0);

  TDF_Label lab1 = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->NewShape ();
  XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->SetShape (lab1, s6);
  TDataStd_Name::Set(lab1, "Point1");

  TDF_Label labelA0 = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->NewShape ();
  TDataStd_Name::Set(labelA0, "ASSEMBLY");

  TDF_Label component01 = XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->AddComponent (labelA0, lab1, location0);
  XCAFDoc_DocumentTool::ShapeTool (aDoc->Main ())->UpdateAssemblies();

  Quantity_Color yellow(1,1,0, Quantity_TOC_RGB);
  XCAFDoc_DocumentTool::ColorTool (labelA0)->SetColor (component01, yellow, XCAFDoc_ColorGen);
  XCAFDoc_DocumentTool::ColorTool (labelA0)->SetVisibility (component01, 0);

  STEPControl_StepModelType mode = STEPControl_AsIs;
  STEPCAFControl_Writer writer;
  if (! writer.Transfer (aDoc, mode))
  {
    di << "The document cannot be translated or gives no result\n";
    return 1;
  }

  writer.Write (argv[1]);
  return 0;
}

//=======================================================================
//function : OCC24667
//purpose  : 
//=======================================================================
static Standard_Integer OCC24667 (Draw_Interpretor& di, Standard_Integer n, const char** a)
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

  if (n > 1 && n < 4) return 1;

  TopoDS_Shape Spine = DBRep::Get(a[2],TopAbs_WIRE);
  if ( Spine.IsNull()) return 1;

  TopoDS_Shape Profile = DBRep::Get(a[3]);
  if ( Profile.IsNull()) return 1;

  GeomFill_Trihedron Mode = GeomFill_IsCorrectedFrenet;
  if (n >= 5)
  {
    Standard_Integer iMode = atoi(a[4]);
    if (iMode == 1)
      Mode = GeomFill_IsFrenet;
    else if (iMode == 2)
      Mode = GeomFill_IsDiscreteTrihedron;
  }

  Standard_Boolean ForceApproxC1 = Standard_False;
  if (n >= 6)
    ForceApproxC1 = Standard_True;

  BRepOffsetAPI_MakePipe aPipe(TopoDS::Wire(Spine),
                                          Profile,
                                          Mode,
                                          ForceApproxC1);

  TopoDS_Shape S = aPipe.Shape();
  TopoDS_Shape aSF = aPipe.FirstShape();
  TopoDS_Shape aSL = aPipe.LastShape();

  DBRep::Set(a[1],S);

  TCollection_AsciiString aStrF(a[1], "_f");
  TCollection_AsciiString aStrL(a[1], "_l");

  DBRep::Set(aStrF.ToCString(), aSF);
  DBRep::Set(aStrL.ToCString(), aSL);

  return 0;
}

#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
static TopoDS_Shape CreateTestShape (int& theShapeNb)
{
  TopoDS_Compound aComp;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aComp);
  //NURBS modifier is used to increase footprint of each shape
  Handle(BRepTools_NurbsConvertModification) aNurbsModif = new BRepTools_NurbsConvertModification;
  TopoDS_Shape aRefShape = BRepPrimAPI_MakeCylinder (50., 100.).Solid();
  BRepTools_Modifier aModifier (aRefShape, aNurbsModif);
  if (aModifier.IsDone()) {
    aRefShape = aModifier.ModifiedShape (aRefShape);
  }
  int aSiblingNb = 0;
  for (; theShapeNb > 0; --theShapeNb) {
    TopoDS_Shape aShape;
    if (++aSiblingNb <= 100) { //number of siblings is limited to avoid long lists
		aShape = BRepBuilderAPI_Copy (aRefShape, Standard_True /*CopyGeom*/).Shape();
    } else {
      aShape = CreateTestShape (theShapeNb);
    }
    aBuilder.Add (aComp, aShape);
  }
  return aComp;
}

#include <TDataStd_Integer.hxx>
#include <TNaming_Builder.hxx>
static Standard_Integer OCC24931 (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 2) {
    di << "Usage: " << argv[0] << " invalid number of arguments\n";
    return 1;
  }
  TCollection_ExtendedString aFileName (argv[1]);
  PCDM_StoreStatus aSStatus  = PCDM_SS_Failure;

  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  {
    Handle(TDocStd_Document) aDoc;
    anApp->NewDocument ("XmlOcaf", aDoc);
    TDF_Label aLab = aDoc->Main();
    TDataStd_Integer::Set (aLab, 0);
    int n = 10000; //must be big enough
    TopoDS_Shape aShape = CreateTestShape (n);
    TNaming_Builder aBuilder (aLab);
    aBuilder.Generated (aShape);

    aSStatus = anApp->SaveAs (aDoc, aFileName);
    anApp->Close (aDoc);
  }
  QCOMPARE (aSStatus, PCDM_SS_OK);
  return 0;
}

#include <TDF_AttributeIterator.hxx>
//=======================================================================
//function : OCC24755
//purpose  : 
//=======================================================================
static Standard_Integer OCC24755 (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 1)
  {
    std::cout << "Usage : " << a[0] << "\n";
    return 1;
  }

  Handle(TDocStd_Application) anApp = DDocStd::GetApplication();
  Handle(TDocStd_Document) aDoc;
  anApp->NewDocument ("BinOcaf", aDoc);
  TDF_Label aLab = aDoc->Main();
  // Prepend an int value.
  TDataStd_Integer::Set (aLab, 0);
  // Prepend a name.
  TDataStd_Name::Set (aLab, "test");
  // Append a double value.
  aLab.AddAttribute(new TDataStd_Real(), true/*append*/);

  TDF_AttributeIterator i (aLab);
  Handle(TDF_Attribute) anAttr = i.Value();
  QCOMPARE (anAttr->IsKind (STANDARD_TYPE (TDataStd_Name)), Standard_True);
  i.Next();
  anAttr = i.Value();
  QCOMPARE (anAttr->IsKind (STANDARD_TYPE (TDataStd_Integer)), Standard_True);
  i.Next();
  anAttr = i.Value();
  QCOMPARE (anAttr->IsKind (STANDARD_TYPE (TDataStd_Real)), Standard_True);

  return 0;
}

struct MyStubObject
{
  MyStubObject() : ptr(0L) {}
  MyStubObject(void* thePtr) : ptr(thePtr) {}
  char overhead[40];
  void* ptr;
};

//=======================================================================
//function : OCC24834
//purpose  : 
//=======================================================================
static Standard_Integer OCC24834 (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 1)
  {
    std::cout << "Usage : " << a[0] << "\n";
    return 1;
  }

  int i = sizeof (char*);  
  if (i > 4) {
    std::cout << "64-bit architecture is not supported.\n";
    return 0;
  }

  NCollection_List<MyStubObject> aList;
  const Standard_Integer aSmallBlockSize = 40;
  const Standard_Integer aLargeBlockSize = 1500000;

  // quick populate memory with large blocks
  try
  {
    for (;;)
    {
      aList.Append(MyStubObject(Standard::Allocate(aLargeBlockSize)));
    }
  }
  catch (Standard_Failure)
  {
    di << "caught out of memory for large blocks: OK\n";
  }
  catch (...)
  {
    di << "skept out of memory for large blocks: Error\n";
  }

  // allocate small blocks
  try
  {
    for (;;)
    {
      aList.Append(MyStubObject(Standard::Allocate(aSmallBlockSize)));
    }
  }
  catch (Standard_Failure)
  {
    di << "caught out of memory for small blocks: OK\n";
  }
  catch (...)
  {
    di << "skept out of memory for small blocks: Error\n";
  }

  // release all allocated blocks
  for (NCollection_List<MyStubObject>::Iterator it(aList); it.More(); it.Next())
  {
    Standard::Free(it.Value().ptr);
  }
  return 0;
}


#include <Geom2dAPI_InterCurveCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
//=======================================================================
//function : OCC24889
//purpose  : 
//=======================================================================
static Standard_Integer OCC24889 (Draw_Interpretor& theDI,
                                  Standard_Integer /*theNArg*/,
                                  const char** /*theArgs*/)
{
 // Curves
  Handle( Geom2d_Circle ) aCircle1 = new Geom2d_Circle(
    gp_Ax22d( gp_Pnt2d( 25, -25 ), gp_Dir2d( 1, 0 ), gp_Dir2d( -0, 1 ) ), 155 );

  Handle( Geom2d_Circle ) aCircle2 = new Geom2d_Circle(
    gp_Ax22d( gp_Pnt2d( 25, 25 ), gp_Dir2d( 1, 0 ), gp_Dir2d( -0, 1 ) ), 155 );

  Handle( Geom2d_TrimmedCurve ) aTrim[2] = {
    new Geom2d_TrimmedCurve( aCircle1, 1.57079632679490, 2.97959469729228 ),
    new Geom2d_TrimmedCurve( aCircle2, 3.30359060633978, 4.71238898038469 )
  };

  DrawTrSurf::Set("c_1", aTrim[0]);
  DrawTrSurf::Set("c_2", aTrim[1]);

  // Intersection
  const Standard_Real aTol = Precision::Confusion();
  Geom2dAPI_InterCurveCurve aIntTool( aTrim[0], aTrim[1], aTol );

  const IntRes2d_IntersectionPoint& aIntPnt =
    aIntTool.Intersector().Point( 1 );

  gp_Pnt2d aIntRes = aIntTool.Point( 1 );
  Standard_Real aPar[2] = {
    aIntPnt.ParamOnFirst(),
    aIntPnt.ParamOnSecond()
  };

  //theDI.precision( 5 );
  theDI << "Int point: X = " << aIntRes.X() << "; Y = " << aIntRes.Y() << "\n";
  for (int i = 0; i < 2; ++i)
  {
    theDI << "Curve " << i << ": FirstParam = " << aTrim[i]->FirstParameter() <<
      "; LastParam = " << aTrim[i]->LastParameter() <<
      "; IntParameter = " << aPar[i] << "\n";
  }

  return 0;
}

#include <math_GlobOptMin.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>
//=======================================================================
//function : OCC25004
//purpose  : Check extremaCC on Branin function.
//=======================================================================
// Function is:
// f(u,v) = a*(v - b*u^2 + c*u-r)^2+s(1-t)*cos(u)+s
// Standard borders are:
// -5 <= u <= 10
//  0 <= v <= 15
class BraninFunction : public math_MultipleVarFunctionWithHessian
{
public:
  BraninFunction()
  {
    a = 1.0;
    b = 5.1 / (4.0 * M_PI * M_PI);
    c = 5.0 / M_PI;
    r = 6.0;
    s = 10.0;
    t = 1.0 / (8.0 *  M_PI);
  }
  virtual Standard_Integer NbVariables() const
  {
    return 2;
  }
  virtual Standard_Boolean Value(const math_Vector& X,Standard_Real& F)
  {
    Standard_Real u = X(1);
    Standard_Real v = X(2);

    Standard_Real aSqPt = (v - b * u * u + c * u - r); // Square Part of function.
    Standard_Real aLnPt = s * (1 - t) * cos(u); // Linear part of funcrtion.
    F = a * aSqPt * aSqPt + aLnPt + s;
    return Standard_True;
  }
  virtual Standard_Boolean Gradient(const math_Vector& X,math_Vector& G)
  {
    Standard_Real u = X(1);
    Standard_Real v = X(2);

    Standard_Real aSqPt = (v - b * u * u + c * u - r); // Square Part of function.
    G(1) = 2 * a * aSqPt * (c - 2 * b * u) - s * (1 - t) * sin(u);
    G(2) = 2 * a * aSqPt;

    return Standard_True;
  }
  virtual Standard_Boolean Values(const math_Vector& X,Standard_Real& F,math_Vector& G)
  {
    Value(X,F);
    Gradient(X,G);

    return Standard_True;
  }
  virtual Standard_Boolean Values(const math_Vector& X,Standard_Real& F,math_Vector& G,math_Matrix& H)
  {
    Value(X,F);
    Gradient(X,G);

    Standard_Real u = X(1);
    Standard_Real v = X(2);

    Standard_Real aSqPt = (v - b * u * u + c * u - r); // Square Part of function.
    Standard_Real aTmpPt = c - 2 * b *u; // Tmp part.
    H(1,1) = 2 * a * aTmpPt * aTmpPt - 4 * a * b * aSqPt - s * (1 - t) * cos(u);
    H(1,2) = 2 * a * aTmpPt;
    H(2,1) = H(1,2);
    H(2,2) = 2 * a;

    return Standard_True;
  }

private:
  // Standard parameters.
  Standard_Real a, b, c, r, s, t;
};

static Standard_Integer OCC25004 (Draw_Interpretor& theDI,
                                  Standard_Integer /*theNArg*/,
                                  const char** /*theArgs*/)
{
  BraninFunction aFunc;

  math_Vector aLower(1,2), aUpper(1,2);
  aLower(1) = -5;
  aLower(2) =  0;
  aUpper(1) = 10;
  aUpper(2) = 15;

  Standard_Integer aGridOrder = 16;
  math_Vector aFuncValues(1, aGridOrder * aGridOrder);

  Standard_Real aLipConst = 0;
  math_Vector aCurrPnt1(1, 2), aCurrPnt2(1, 2);

  // Get Lipshitz constant estimation on regular grid.
  Standard_Integer i, j, idx = 1;
  for(i = 1; i <= aGridOrder; i++)
  {
    for(j = 1; j <= aGridOrder; j++)
    {
      aCurrPnt1(1) = aLower(1) + (aUpper(1) - aLower(1)) * (i - 1) / (aGridOrder - 1.0);
      aCurrPnt1(2) = aLower(2) + (aUpper(2) - aLower(2)) * (j - 1) / (aGridOrder - 1.0);

      aFunc.Value(aCurrPnt1, aFuncValues(idx));
      idx++;
    }
  }

  Standard_Integer k, l;
  Standard_Integer idx1, idx2;
  for(i = 1; i <= aGridOrder; i++)
  for(j = 1; j <= aGridOrder; j++)
  for(k = 1; k <= aGridOrder; k++)
  for(l = 1; l <= aGridOrder; l++)
    {
      if (i == k && j == l) 
        continue;

      aCurrPnt1(1) = aLower(1) + (aUpper(1) - aLower(1)) * (i - 1) / (aGridOrder - 1.0);
      aCurrPnt1(2) = aLower(2) + (aUpper(2) - aLower(2)) * (j - 1) / (aGridOrder - 1.0);
      idx1 = (i - 1) * aGridOrder + j;

      aCurrPnt2(1) = aLower(1) + (aUpper(1) - aLower(1)) * (k - 1) / (aGridOrder - 1.0);
      aCurrPnt2(2) = aLower(2) + (aUpper(2) - aLower(2)) * (l - 1) / (aGridOrder - 1.0);
      idx2 = (k - 1) * aGridOrder + l;

      aCurrPnt1.Add(-aCurrPnt2);
      Standard_Real dist = aCurrPnt1.Norm();

      Standard_Real C = Abs(aFuncValues(idx1) - aFuncValues(idx2)) / dist;
      if (C > aLipConst)
        aLipConst = C;
    }

  math_GlobOptMin aFinder(&aFunc, aLower, aUpper, aLipConst);
  aFinder.Perform();
  //(-pi , 12.275), (pi , 2.275), (9.42478, 2.475)

  Standard_Real anExtValue = aFinder.GetF();
  theDI << "F = " << anExtValue << "\n";

  Standard_Integer aNbExt = aFinder.NbExtrema();
  theDI << "NbExtrema = " << aNbExt << "\n";

  return 0;
}

#include <OSD_Environment.hxx>
#include <Plugin.hxx>
#include <Plugin_Macro.hxx>
#include <Resource_Manager.hxx>

#define THE_QATEST_DOC_FORMAT       "My Proprietary Format"

#define QA_CHECK(theDesc, theExpr, theValue) \
{\
  const bool isTrue = !!(theExpr); \
  std::cout << theDesc << (isTrue ? " TRUE  " : " FALSE ") << (isTrue == theValue ? " is OK\n" : " is FAIL\n"); \
}

class Test_TDocStd_Application : public TDocStd_Application
{
public:

  Test_TDocStd_Application ()
  {
    // explicitly initialize resource manager
    myResources = new Resource_Manager ("");
    myResources->SetResource ("xml.FileFormat", THE_QATEST_DOC_FORMAT);
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".Description",     "Test XML Document");
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".FileExtension",   "xml");
  }

  virtual Handle(PCDM_Reader) ReaderFromFormat (const TCollection_ExtendedString&) Standard_OVERRIDE
  {
    return new XmlDrivers_DocumentRetrievalDriver ();
  }
  virtual Handle(PCDM_StorageDriver) WriterFromFormat (const TCollection_ExtendedString&) Standard_OVERRIDE
  {
    return new XmlDrivers_DocumentStorageDriver ("Test");
  }
  virtual Standard_CString ResourcesName() Standard_OVERRIDE { return ""; }
};

//=======================================================================
//function : OCC24925
//purpose  :
//=======================================================================
static Standard_Integer OCC24925 (Draw_Interpretor& theDI,
                                  Standard_Integer  theArgNb,
                                  const char**      theArgVec)
{
  if (theArgNb != 2
   && theArgNb != 5)
  {
    std::cout << "Error: wrong syntax! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Standard_Integer anArgIter = 1;
  TCollection_ExtendedString aFileName = theArgVec[anArgIter++];
  TCollection_AsciiString    aPlugin   = "TKXml";
  TCollection_AsciiString    aSaver    = "03a56820-8269-11d5-aab2-0050044b1af1"; // XmlStorageDriver   in XmlDrivers.cxx
  TCollection_AsciiString    aLoader   = "03a56822-8269-11d5-aab2-0050044b1af1"; // XmlRetrievalDriver in XmlDrivers.cxx
  if (anArgIter < theArgNb)
  {
    aPlugin = theArgVec[anArgIter++];
    aSaver  = theArgVec[anArgIter++];
    aLoader = theArgVec[anArgIter++];
  }

  PCDM_StoreStatus  aSStatus = PCDM_SS_Failure;
  PCDM_ReaderStatus aRStatus = PCDM_RS_OpenError;

  Handle(TDocStd_Application) anApp = new Test_TDocStd_Application ();
  {
    Handle(TDocStd_Document) aDoc;
    anApp->NewDocument (THE_QATEST_DOC_FORMAT, aDoc);
    TDF_Label aLab = aDoc->Main();
    TDataStd_Integer::Set (aLab, 0);
    TDataStd_Name::Set (aLab, "QABugs_19.cxx");

    aSStatus = anApp->SaveAs (aDoc, aFileName);
    anApp->Close (aDoc);
  }
  QA_CHECK ("SaveAs()", aSStatus == PCDM_SS_OK, true);

  {
    Handle(TDocStd_Document) aDoc;
    aRStatus = anApp->Open (aFileName, aDoc);
    anApp->Close (aDoc);
  }
  QA_CHECK ("Open()  ", aRStatus == PCDM_RS_OK, true);
  return 0;
}

//=======================================================================
//function : OCC25043
//purpose  :
//=======================================================================
#include <BRepAlgoAPI_Check.hxx>
static Standard_Integer OCC25043 (Draw_Interpretor& theDI,
                                  Standard_Integer  theArgNb,
                                  const char**      theArgVec)
{
  if (theArgNb != 2) {
    theDI << "Usage: " << theArgVec[0] << " shape\n";
    return 1;
  }
  
  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull()) 
  {
    theDI << theArgVec[1] << " shape is NULL\n";
    return 1;
  }
  
  BRepAlgoAPI_Check  anAlgoApiCheck(aShape, Standard_True, Standard_True);

  if (!anAlgoApiCheck.IsValid())
  {
    BOPAlgo_ListIteratorOfListOfCheckResult anCheckIter(anAlgoApiCheck.Result());
    for (; anCheckIter.More(); anCheckIter.Next())
    {
      const BOPAlgo_CheckResult& aCurCheckRes = anCheckIter.Value();
      const BOPCol_ListOfShape& aCurFaultyShapes = aCurCheckRes.GetFaultyShapes1();
      BOPCol_ListIteratorOfListOfShape aFaultyIter(aCurFaultyShapes);
      for (; aFaultyIter.More(); aFaultyIter.Next())
      {
        const TopoDS_Shape& aFaultyShape = aFaultyIter.Value();
        
        Standard_Boolean anIsFaultyShapeFound = Standard_False;
        TopExp_Explorer anExp(aShape, aFaultyShape.ShapeType());
        for (; anExp.More() && !anIsFaultyShapeFound; anExp.Next())
        {
          if (anExp.Current().IsEqual(aFaultyShape))
            anIsFaultyShapeFound = Standard_True;
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

//=======================================================================
//function : OCC24606
//purpose  :
//=======================================================================
static Standard_Integer OCC24606 (Draw_Interpretor& theDI,
                                  Standard_Integer  theArgNb,
                                  const char**      theArgVec)
{
  if (theArgNb > 1)
  {
    std::cerr << "Error: incorrect number of arguments.\n";
    theDI << "Usage : " << theArgVec[0] << "\n";
    return 1;
  }

  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "Errro: no active view, please call 'vinit'.\n";
    return 1;
  }

  aView->DepthFitAll();
  aView->FitAll();

  return 0;
}

//=======================================================================
//function : OCC25202
//purpose  :
//=======================================================================
#include <ShapeBuild_ReShape.hxx>
static Standard_Integer OCC25202 ( Draw_Interpretor& theDI,
				   Standard_Integer theArgN,
				   const char** theArgVal)
{
  //  0      1    2     3     4     5     6 
  //reshape res shape numF1 face1 numF2 face2
  if(theArgN < 7)
    {
      theDI << "Use: reshape res shape numF1 face1 numF2 face2\n";
      return 1;
    }

  TopoDS_Shape aShape = DBRep::Get(theArgVal[2]);
  const Standard_Integer  aNumOfRE1 = Draw::Atoi(theArgVal[3]),
                          aNumOfRE2 = Draw::Atoi(theArgVal[5]);
  TopoDS_Face aShapeForRepl1 = TopoDS::Face(DBRep::Get(theArgVal[4])),
              aShapeForRepl2 = TopoDS::Face(DBRep::Get(theArgVal[6]));

  if(aShape.IsNull())
  {
    theDI << theArgVal[2] << " is null shape\n";
    return 1;
  }

  if(aShapeForRepl1.IsNull())
  {
    theDI << theArgVal[4] << " is not a replaced type\n";
    return 1;
  }

  if(aShapeForRepl2.IsNull())
  {
    theDI << theArgVal[6] << " is not a replaced type\n";
    return 1;
  }


  TopoDS_Shape aReplacedShape;
  ShapeBuild_ReShape aReshape;

  //////////////////// explode (begin)
  TopTools_MapOfShape M;
  M.Add(aShape);
  Standard_Integer aNbShapes = 0;
  for (TopExp_Explorer ex(aShape,TopAbs_FACE); ex.More(); ex.Next())
    {
      const TopoDS_Shape& Sx = ex.Current();
      Standard_Boolean added = M.Add(Sx);
      if (added)
	{
	  aNbShapes++;
	  if(aNbShapes == aNumOfRE1)
	    {
	      aReplacedShape = Sx;

	      aReshape.Replace(aReplacedShape, aShapeForRepl1);
	    }

	  if(aNbShapes == aNumOfRE2)
	    {
	      aReplacedShape = Sx;

	      aReshape.Replace(aReplacedShape, aShapeForRepl2);
	    }
	}
    }
  //////////////////// explode (end)

  if(aReplacedShape.IsNull())
    {
      theDI << "There is not any shape for replacing.\n";
    }

  DBRep::Set (theArgVal[1],aReshape.Apply (aShape,TopAbs_WIRE,2));

  return 0;
}

#include <ShapeFix_Wireframe.hxx>
//=======================================================================
//function : OCC7570
//purpose  : 
//=======================================================================
static Standard_Integer OCC7570 (Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n != 2) {
    di<<"Usage: "<<a[0]<<" invalid number of arguments\n";
    return 1;
  }
  TopoDS_Shape in_shape (DBRep::Get (a[1]));
  ShapeFix_Wireframe fix_tool (in_shape);
  fix_tool.ModeDropSmallEdges () = Standard_True;
  fix_tool.SetPrecision (1.e+6);
  fix_tool.SetLimitAngle (0.01);
  fix_tool.FixSmallEdges ();
  TopoDS_Shape new_shape = fix_tool.Shape ();
  return 0;
}

#include <AIS_TypeFilter.hxx>
//=======================================================================
//function : OCC25340
//purpose  : 
//=======================================================================
static Standard_Integer OCC25340 (Draw_Interpretor& /*theDI*/,
                                 Standard_Integer  /*theArgNb*/,
                                 const char** /*theArgVec*/)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cerr << "Error: No opened viewer!\n";
    return 1;
  }
  Handle(AIS_TypeFilter) aFilter = new AIS_TypeFilter (AIS_KOI_Shape);
  aCtx->AddFilter (aFilter);
  return 0;
}

//=======================================================================
//function : OCC24826
//purpose  :
//=======================================================================
class ParallelTest_Saxpy
{
public:
  typedef NCollection_Array1<Standard_Real> Vector;

  //! Constructor
  ParallelTest_Saxpy(const Vector& theX, Vector& theY, Standard_Real theScalar)
  : myX(theX),
    myY(theY),
    myScalar(theScalar)
  {
  }

  //! Dummy calculation
  void operator() (const Standard_Integer theIndex) const
  {
    myY(theIndex) = myScalar * myX(theIndex) + myY(theIndex);
  }

private:
  ParallelTest_Saxpy( const ParallelTest_Saxpy& );
  ParallelTest_Saxpy& operator =( ParallelTest_Saxpy& );

private:
  const Vector&       myX;
  Vector&             myY;
  const Standard_Real myScalar;
};

//---------------------------------------------------------------------
static Standard_Integer OCC24826(Draw_Interpretor& theDI,
                                 Standard_Integer  trheArgc,
                                 const char**      theArgv)
{
  if ( trheArgc != 2 )
  {
    theDI << "Usage: "
          << theArgv[0]
          << " vec_length\n";
    return 1;
  }

  // Generate data;
  Standard_Integer aLength = Draw::Atoi(theArgv[1]);

  NCollection_Array1<Standard_Real> aX (0, aLength - 1);
  NCollection_Array1<Standard_Real> anY(0, aLength - 1);

  for ( Standard_Integer i = 0; i < aLength; ++i )
  {
    aX(i) = anY(i) = (Standard_Real) i;
  }

  OSD_Timer aTimer;

  aTimer.Start();

  //! Serial proccesing
  for ( Standard_Integer i = 0; i < aLength; ++i )
  {
    anY(i) = 1e-6 * aX(i) + anY(i);
  }

  aTimer.Stop();
  cout << "Processing time (sequential mode):\n";
  aTimer.Show();

  const ParallelTest_Saxpy aFunctor(aX, anY, 1e-6);

  aTimer.Reset();
  aTimer.Start();

  // Parallel processing
  OSD_Parallel::For(0, aLength, aFunctor);

  aTimer.Stop();
  cout << "Processing time (parallel mode):\n";
  aTimer.Show();

  return 0;
}

/*****************************************************************************/

#include <GeomAPI_IntSS.hxx>
//=======================================================================
//function : OCC25100
//purpose  :
//=======================================================================
static Standard_Integer OCC25100 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc < 2)
  {
    di << "the method requires a shape name\n";
    return 1;
  }

  TopoDS_Shape S = DBRep::Get(argv[1]);
  if ( S.IsNull() )
  {
    di << "Shape is empty\n";
    return 1;
  }
  
  TopExp_Explorer aFaceExp(S, TopAbs_FACE);
  const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface(TopoDS::Face(aFaceExp.Current()));

  GeomAPI_IntSS anIntersector(aSurf, aSurf, Precision::Confusion());

  if (!anIntersector.IsDone())
  {
    di << "Error. Intersection is not done\n";
    return 1;
  }

  di << "Test complete\n";

  return 0;
}

//=======================================================================
//function : OCC25348
//purpose  : 
//=======================================================================
static Standard_Integer OCC25348 (Draw_Interpretor& theDI,
                                 Standard_Integer  /*theArgNb*/,
                                 const char** /*theArgVec*/)
{
  Handle(NCollection_IncAllocator) anAlloc1;
  NCollection_List<int> aList1(anAlloc1);
  for (int i=0; i < 10; i++)
  {
    Handle(NCollection_IncAllocator) anAlloc2;
    NCollection_List<int> aList2(anAlloc2);
    aList2.Append(i);
    aList1.Assign(aList2);
  }
  theDI << "Test complete\n";
  return 0;
}

#include <IntCurvesFace_ShapeIntersector.hxx>
#include <BRepBndLib.hxx>
//=======================================================================
//function : OCC25413
//purpose  : 
//=======================================================================
static Standard_Integer OCC25413 (Draw_Interpretor& di, Standard_Integer narg , const char** a)
{
  if (narg != 2) {
    di << "Usage: " << a[0] << " invalid number of arguments\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get (a[1]);

  IntCurvesFace_ShapeIntersector Inter;
  Inter.Load(aShape, Precision::Confusion());

  Bnd_Box aBndBox;
  BRepBndLib::Add(aShape, aBndBox);

  gp_Dir aDir(0., 1., 0.);
  const int N = 250;
  Standard_Real xMin = aBndBox.CornerMin().X();
  Standard_Real zMin = aBndBox.CornerMin().Z();
  Standard_Real xMax = aBndBox.CornerMax().X();
  Standard_Real zMax = aBndBox.CornerMax().Z();
  Standard_Real xStep = (xMax - xMin) / N;
  Standard_Real zStep = (zMax - zMin) / N;

  for (Standard_Real x = xMin; x <= xMax; x += xStep)
    for (Standard_Real z = zMin; z <= zMax; z += zStep)
    {
      gp_Pnt aPoint(x, 0.0, z);
      gp_Lin aLine(aPoint, aDir);
      Inter.PerformNearest(aLine, -100., 100.);
    }
  return 0;
}


#include <BOPAlgo_PaveFiller.hxx>
//
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Section.hxx>
//
#include <BOPTools.hxx>
//
#include <BOPCol_MapOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
//=======================================================================
//function : OCC25446
//purpose  :
//=======================================================================
static Standard_Integer OCC25446 (Draw_Interpretor& theDI, 
                                  Standard_Integer argc, 
                                  const char ** argv)
{
  if (argc != 5) {
    theDI << "Usage: OCC25446 res b1 b2 op\n";
    return 1;
  }
  //
  TopoDS_Shape aS1 = DBRep::Get(argv[2]);
  if (aS1.IsNull()) {
    theDI << argv[2] << " shape is NULL\n";
    return 1;
  }
  //
  TopoDS_Shape aS2 = DBRep::Get(argv[3]);
  if (aS2.IsNull()) {
    theDI << argv[3] << " shape is NULL\n";
    return 1;
  }
  //
  Standard_Integer iOp;
  BOPAlgo_Operation aOp;
  //
  iOp = Draw::Atoi(argv[4]);
  if (iOp < 0 || iOp > 4) {
    theDI << "Invalid operation type\n";
    return 1;
  }
  aOp = (BOPAlgo_Operation)iOp;
  //
  Standard_Integer iErr;
  BOPCol_ListOfShape aLS;
  BOPAlgo_PaveFiller aPF;
  //
  aLS.Append(aS1);
  aLS.Append(aS2);
  aPF.SetArguments(aLS);
  //
  aPF.Perform();
  iErr = aPF.HasErrors();
  if (iErr) {
    theDI << "Intersection failed with error status: " << iErr << "\n";
    return 1;
  }
  //
  BRepAlgoAPI_BooleanOperation* pBuilder = NULL;
  // 
  switch (aOp) {
  case BOPAlgo_COMMON:
    pBuilder = new BRepAlgoAPI_Common(aS1, aS2, aPF);
    break;
  case BOPAlgo_FUSE:
    pBuilder = new BRepAlgoAPI_Fuse(aS1, aS2, aPF);
    break;
  case BOPAlgo_CUT:
    pBuilder = new BRepAlgoAPI_Cut (aS1, aS2, aPF);
    break;
  case BOPAlgo_CUT21:
    pBuilder = new BRepAlgoAPI_Cut(aS1, aS2, aPF, Standard_False);
    break;
  case BOPAlgo_SECTION:
    pBuilder = new BRepAlgoAPI_Section(aS1, aS2, aPF);
    break;
  default:
    break;
  }
  //
  iErr = pBuilder->HasErrors();
  if (!pBuilder->IsDone()) {
    theDI << "BOP failed with error status: " << iErr << "\n";
    return 1;
  }
  //
  const TopoDS_Shape& aRes = pBuilder->Shape();
  DBRep::Set(argv[1], aRes);
  //
  BOPCol_MapOfShape aMapArgs, aMapShape;
  BOPCol_MapIteratorOfMapOfShape aIt;
  Standard_Boolean bIsDeletedHist, bIsDeletedMap;
  TopAbs_ShapeEnum aType;
  //
  BOPTools::MapShapes(aS1, aMapArgs);
  BOPTools::MapShapes(aS2, aMapArgs);
  BOPTools::MapShapes(aRes, aMapShape);
  //
  aIt.Initialize(aMapArgs);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aS = aIt.Value();
    aType = aS.ShapeType();
    if (!(aType==TopAbs_EDGE || aType==TopAbs_FACE || 
          aType==TopAbs_VERTEX || aType==TopAbs_SOLID)) {
      continue;
    }
    //
    bIsDeletedHist = pBuilder->IsDeleted(aS);
    bIsDeletedMap = !aMapShape.Contains(aS) &&
      (pBuilder->Modified(aS).Extent() == 0);
    //
    if (bIsDeletedHist != bIsDeletedMap) {
      theDI << "Error. Wrong value of IsDeleted flag.\n";
      return 1;
    }
  }
  //
  theDI << "Test complete\n";
  return 0;
}

//====================================================
// Auxiliary functor class for the command OCC25545;
// it gets access to a vertex with the given index and
// checks that X coordinate of the point is equal to index;
// if it is not so then a data race is reported.
//====================================================
struct OCC25545_Functor
{
  OCC25545_Functor(const std::vector<TopoDS_Shape>& theShapeVec)
    : myShapeVec(&theShapeVec),
      myIsRaceDetected(0)
  {}

  void operator()(size_t i) const
  {
    if (!myIsRaceDetected) {
      const TopoDS_Vertex& aV = TopoDS::Vertex (myShapeVec->at(i));
      gp_Pnt aP = BRep_Tool::Pnt (aV);
      if (aP.X () != static_cast<double> (i)) {
        Standard_Atomic_Increment(&myIsRaceDetected);
      }
    }
  }

  const std::vector<TopoDS_Shape>* myShapeVec;
  mutable volatile int myIsRaceDetected;
};

//=======================================================================
//function : OCC25545
//purpose  : Tests data race when concurrently accessing TopLoc_Location::Transformation()
//=======================================================================
#ifdef HAVE_TBB
static Standard_Integer OCC25545 (Draw_Interpretor& di, 
                                  Standard_Integer, 
                                  const char **)
{
  // Place vertices in a vector, giving the i-th vertex the
  // transformation that translates it on the vector (i,0,0) from the origin.
  size_t n = 1000;
  std::vector<TopoDS_Shape> aShapeVec (n);
  std::vector<TopLoc_Location> aLocVec (n);
  TopoDS_Shape aShape = BRepBuilderAPI_MakeVertex (gp::Origin ());
  aShapeVec[0] = aShape;
  for (size_t i = 1; i < n; ++i) {
    gp_Trsf aT;
    aT.SetTranslation (gp_Vec (1, 0, 0));
    aLocVec[i] = aLocVec[i - 1] * aT;
    aShapeVec[i] = aShape.Moved (aLocVec[i]);
  }

  // Evaluator function will access vertices geometry
  // concurrently
  OCC25545_Functor aFunc(aShapeVec);

  //concurrently process
  tbb::parallel_for (size_t (0), n, aFunc, tbb::simple_partitioner ());
  QVERIFY (!aFunc.myIsRaceDetected);
  return 0;
}
#else
static Standard_Integer OCC25545 (Draw_Interpretor&, 
                                  Standard_Integer, 
                                  const char **argv)
{
  cout << "Test skipped: command " << argv[0] << " requires TBB library" << endl;
  return 0;
}
#endif

//=======================================================================
//function : OCC25547
//purpose  :
//=======================================================================
#include <BRepMesh_GeomTool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
static Standard_Integer OCC25547(
  Draw_Interpretor& theDI, 
  Standard_Integer  /*argc*/, 
  const char **     /*argv*/)
{
  // The general aim of this test is to prevent linkage errors due to missed
  // Standard_EXPORT attribute for static methods.

  // However, start checking the main functionality at first.
  const Standard_Real aFirstP = 0., aLastP = M_PI;
  Handle(Geom_Circle) aCircle = new Geom_Circle(gp_Ax2(gp::Origin(), gp::DZ()), 10);
  Handle(Geom_TrimmedCurve) aHalf = new Geom_TrimmedCurve(aCircle, aFirstP, aLastP);
  TopoDS_Edge aEdge = BRepBuilderAPI_MakeEdge(aHalf);
  BRepAdaptor_Curve aAdaptor(aEdge);
  BRepMesh_GeomTool aGeomTool(aAdaptor, aFirstP, aLastP, 0.1, 0.5);

  if (aGeomTool.NbPoints() == 0)
  {
    theDI << "Error. BRepMesh_GeomTool failed to discretize an arc.\n";
    return 1;
  }

  // Test static methods.
  TopoDS_Face aFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::Origin(), gp::DZ()));
  BRepAdaptor_Surface aSurf(aFace);
  Handle(BRepAdaptor_HSurface) aHSurf = new BRepAdaptor_HSurface(aSurf);

  gp_Pnt aPnt;
  gp_Dir aNormal;
  if (!BRepMesh_GeomTool::Normal(aHSurf, 10., 10., aPnt, aNormal))
  {
    theDI << "Error. BRepMesh_GeomTool failed to take a normal of surface.\n";
    return 1;
  }

  gp_XY aRefPnts[4] = {
    gp_XY(-10., -10.), gp_XY(10., 10.), 
    gp_XY(-10., 10.), gp_XY(10., -10.)
  };

  gp_Pnt2d aIntPnt;
  Standard_Real aParams[2];
  BRepMesh_GeomTool::IntFlag aIntFlag = BRepMesh_GeomTool::IntLinLin(
    aRefPnts[0], aRefPnts[1], aRefPnts[2], aRefPnts[3], 
    aIntPnt.ChangeCoord(), aParams);

  Standard_Real aDiff = aIntPnt.Distance(gp::Origin2d());
  if (aIntFlag != BRepMesh_GeomTool::Cross || aDiff > Precision::PConfusion())
  {
    theDI << "Error. BRepMesh_GeomTool failed to intersect two lines.\n";
    return 1;
  }

  aIntFlag = BRepMesh_GeomTool::IntSegSeg(
    aRefPnts[0], aRefPnts[1], aRefPnts[2], aRefPnts[3], 
    Standard_False, Standard_False, aIntPnt);

  aDiff = aIntPnt.Distance(gp::Origin2d());
  if (aIntFlag != BRepMesh_GeomTool::Cross || aDiff > Precision::PConfusion())
  {
    theDI << "Error. BRepMesh_GeomTool failed to intersect two segments.\n";
    return 1;
  }


  theDI << "Test complete\n";
  return 0;
}

static Standard_Integer OCC26139 (Draw_Interpretor& theDI,
                                  Standard_Integer  argc,
                                  const char **     argv)
{

  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    theDI << "Use 'vinit' command before " << argv[0] << "\n";
    return 1;
  }

  Standard_Integer aBoxGridSize = 100;
  Standard_Integer aCompGridSize = 3;
  Standard_Real aBoxSize = 5.0;

  if (argc > 1)
  {
    for (Standard_Integer anArgIdx = 1; anArgIdx < argc; ++anArgIdx)
    {
      TCollection_AsciiString anArg (argv[anArgIdx]);
      anArg.LowerCase();
      if (anArg == "-boxgrid")
      {
        aBoxGridSize = Draw::Atoi (argv[++anArgIdx]);
      }
      else if (anArg == "-compgrid")
      {
        aCompGridSize = Draw::Atoi (argv[++anArgIdx]);
      }
      else if (anArg == "-boxsize")
      {
        aBoxSize = Draw::Atof (argv[++anArgIdx]);
      }
    }
  }

  NCollection_List<Handle(AIS_Shape)> aCompounds;
  for (Standard_Integer aCompGridX = 0; aCompGridX < aCompGridSize; ++aCompGridX)
  {
    for (Standard_Integer aCompGridY = 0; aCompGridY < aCompGridSize; ++aCompGridY)
    {
      BRep_Builder aBuilder;
      TopoDS_Compound aComp;
      aBuilder.MakeCompound (aComp);
      for (Standard_Integer aBoxGridX = 0; aBoxGridX < aBoxGridSize; ++aBoxGridX)
      {
        for (Standard_Integer aBoxGridY = 0; aBoxGridY < aBoxGridSize; ++aBoxGridY)
        {
          BRepPrimAPI_MakeBox aBox (gp_Pnt (aBoxGridX * aBoxSize, aBoxGridY * aBoxSize, 0.0),
                                    aBoxSize, aBoxSize, aBoxSize);
          aBuilder.Add (aComp, aBox.Shape());
        }
      }
      gp_Trsf aTrsf;
      aTrsf.SetTranslation (gp_Vec (aBoxGridSize * aBoxSize * aCompGridX,
                                    aBoxGridSize * aBoxSize * aCompGridY,
                                    0.0));
      TopLoc_Location aLoc (aTrsf);
      aComp.Located (aLoc);
      aCompounds.Append (new AIS_Shape (aComp));
    }
  }

  OSD_Timer aTimer;
  for (NCollection_List<Handle(AIS_Shape)>::Iterator aCompIter (aCompounds); aCompIter.More(); aCompIter.Next())
  {
    aTimer.Start();
    aCtx->Display (aCompIter.Value(), Standard_False);
    aTimer.Stop();
    theDI << "Display time: " << aTimer.ElapsedTime() << "\n";
    aTimer.Reset();
  }

  aTimer.Reset();
  aTimer.Start();
  for (NCollection_List<Handle(AIS_Shape)>::Iterator aCompIter (aCompounds); aCompIter.More(); aCompIter.Next())
  {
    aCtx->Remove (aCompIter.Value(), Standard_False);
  }
  aTimer.Stop();
  theDI << "Remove time: " << aTimer.ElapsedTime() << "\n";

  return 0;
}

#include <TColStd_DataMapIteratorOfDataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <OSD.hxx>
#include <ShapeFix_Wire.hxx>
#include <ShapeExtend_Status.hxx>
#ifdef _WIN32
#define EXCEPTION ...
#else
#define EXCEPTION Standard_Failure
#endif

static ShapeExtend_Status getStatusGap(const Handle(ShapeFix_Wire)&   theFix,
                                       const Standard_Boolean theIs3d)
{
	for (Standard_Integer i=ShapeExtend_OK; i<=ShapeExtend_FAIL; i++)
	{
		Standard_Boolean isFound;
		if (theIs3d)
			isFound = theFix->StatusGaps3d( (ShapeExtend_Status) i );
		else
			isFound = theFix->StatusGaps2d( (ShapeExtend_Status) i );
		if (isFound) return ShapeExtend_Status(i);
	}
	return ShapeExtend_OK;
}

//===================
//function : OCC24881
//purpose  : 
//===================
static Standard_Integer OCC24881 (Draw_Interpretor& di, Standard_Integer narg , const char** a)
{
  if (narg < 2) {
    di<<"Usage: "<<a[0]<<" invalid number of arguments\n";
    return 1;
  }
//    cout <<"FileName1: " << argv[1] <<endl;

  TopoDS_Shape aShape = DBRep::Get (a[1]);

    OSD::SetSignal();
    Handle(ShapeFix_Wire) aWireFix = new ShapeFix_Wire;

    // map FixStatus - NbSuchStatuses
    TColStd_DataMapOfIntegerInteger aStatusNbDMap;
    Standard_Integer nbFixed=0, nbOk=0;

//Begin: STEP 7
    ShapeExtend_Status aStatus=ShapeExtend_OK;
    try {
	TopExp_Explorer aFaceExplorer(aShape, TopAbs_FACE);
	for (; aFaceExplorer.More(); aFaceExplorer.Next())
	{
		TopoDS_Shape aFace = aFaceExplorer.Current();
		// loop on wires
		TopoDS_Iterator aWireItr(aFace);
		for (; aWireItr.More(); aWireItr.Next() )
		{
			Standard_Boolean wasOk = Standard_False;
			TopoDS_Wire aSrcWire = TopoDS::Wire(aWireItr.Value());

			aWireFix->Load (aSrcWire);
			aWireFix->SetFace (TopoDS::Face(aFace));
			aWireFix->FixReorder(); //correct order is a prerequisite
			// fix 3d
			if (!aWireFix->FixGaps3d())
			{
				// not fixed, why?
				aStatus = getStatusGap(aWireFix, Standard_True);
				if (aStatus == ShapeExtend_OK)
					wasOk = Standard_True;
				else
				{
					// keep 3d fail status
					if (aStatusNbDMap.IsBound (aStatus))
						aStatusNbDMap(aStatus)++;
					else
						aStatusNbDMap.Bind(aStatus,1);
					continue;
				}
			}

			// fix 2d
			if (aWireFix->FixGaps2d())
				nbFixed++;
			else
			{
				aStatus = getStatusGap(aWireFix, Standard_False);
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
					Standard_Integer aStatus2d = aStatus + ShapeExtend_FAIL;
					if (aStatusNbDMap.IsBound (aStatus2d))
						aStatusNbDMap(aStatus2d)++;
					else
						aStatusNbDMap.Bind(aStatus2d,1);
					continue;
				}
			}
		}
	}
//End: STEP 7
     } catch (EXCEPTION) {
       di << "Exception is raised = " <<aStatus << "\n";
       return 1;

     }
// report what is done

	if (nbFixed)
	{
		di <<"Fix_FillGaps_Fixed: nbFixed = "<<nbFixed <<"\n";

	}    
	if (nbOk)
	{
		di << "Fix_FillGaps_NothingToDo\n";

	}
	TColStd_DataMapIteratorOfDataMapOfIntegerInteger aStatusItr(aStatusNbDMap);
	for (; aStatusItr.More(); aStatusItr.Next()) 
	{
		switch ((ShapeExtend_Status) aStatusItr.Key()) 
		{
			// treat 3d status
			case ShapeExtend_FAIL1:
			di <<"Fix_FillGaps_3dNoCurveFail, nb failed = ";
			break;
			case ShapeExtend_FAIL2:
			di <<"Fix_FillGaps_3dSomeGapsFail, nb failed = ";
			break;
			default:
			// treat 2d status
			switch ((ShapeExtend_Status) (aStatusItr.Key() - ShapeExtend_FAIL)) 
			{
				case ShapeExtend_FAIL1:
				di <<"Fix_FillGaps_2dNoPCurveFail, nb failed = ";
				break;
				case ShapeExtend_FAIL2:
				di <<"Fix_FillGaps_2dSomeGapsFail, nb failed = ";
				break;
				default:
				break;
			}
		}
		di <<aStatusItr.Value()<< "\n";
	}
	di << ("__________________________________") <<"\n";

  return 0;
}

//=======================================================================
//function : OCC26172
//purpose  :
//=======================================================================
static Standard_Integer OCC26172 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 1)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if(anAISContext.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  gp_Pnt aStart (100, 100, 100);
  gp_Pnt anEnd (300, 400, 600);
  BRepBuilderAPI_MakeEdge anEdgeBuilder (aStart, anEnd);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  Handle(AIS_Shape) aTestAISShape = new AIS_Shape (anEdge);
  anAISContext->Display (aTestAISShape, Standard_True);

  // 2. activate it in selection modes
  TColStd_SequenceOfInteger aModes;
  aModes.Append (AIS_Shape::SelectionMode ((TopAbs_ShapeEnum) TopAbs_VERTEX));
  aModes.Append (AIS_Shape::SelectionMode ((TopAbs_ShapeEnum) TopAbs_EDGE));

  Standard_DISABLE_DEPRECATION_WARNINGS
  anAISContext->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS
  anAISContext->Deactivate (aTestAISShape);
  anAISContext->Load (aTestAISShape, -1, true);
  for (Standard_Integer anIt = 1; anIt <= aModes.Length(); ++anIt)
  {
    anAISContext->Activate (aTestAISShape, aModes (anIt));
  }

  // select entities in vertex selection mode
  Handle(SelectMgr_Selection) aSelection = aTestAISShape->Selection (aModes (1));
  Standard_DISABLE_DEPRECATION_WARNINGS
  for (aSelection->Init(); aSelection->More(); aSelection->Next())
  {
    Handle(SelectBasics_SensitiveEntity) anEntity = aSelection->Sensitive()->BaseSensitive();
    if (anEntity.IsNull())
    {
      continue;
    }

    Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (anEntity->OwnerId());

    if (anOwner.IsNull())
    {
      continue;
    }

    anAISContext->LocalContext()->AddOrRemoveSelected (anOwner);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  // select entities in edge selection mode
  aSelection = aTestAISShape->Selection (aModes (2));
  Standard_DISABLE_DEPRECATION_WARNINGS
  for (aSelection->Init(); aSelection->More(); aSelection->Next())
  {
    Handle(SelectBasics_SensitiveEntity) anEntity = aSelection->Sensitive()->BaseSensitive();
    if (anEntity.IsNull())
    {
      continue;
    }

    Handle(SelectMgr_EntityOwner) anOwner =
      Handle(SelectMgr_EntityOwner)::DownCast (anEntity->OwnerId());

    if (anOwner.IsNull())
    {
      continue;
    }

    anAISContext->LocalContext()->AddOrRemoveSelected (anOwner);
  }
  Standard_ENABLE_DEPRECATION_WARNINGS

  // deactivate vertex mode and check clearing of outdated selection
  anAISContext->Deactivate (aTestAISShape, aModes (1));
  Standard_DISABLE_DEPRECATION_WARNINGS
  anAISContext->LocalContext()->ClearOutdatedSelection (aTestAISShape, true);
  Standard_ENABLE_DEPRECATION_WARNINGS

  return 0;
}

//=======================================================================
//function : OCC26284
//purpose  :
//=======================================================================
static Standard_Integer OCC26284 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 1)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  BRepPrimAPI_MakeSphere aSphereBuilder (gp_Pnt (0.0, 0.0, 0.0), 1.0);
  Handle(AIS_Shape) aSphere = new AIS_Shape (aSphereBuilder.Shape());
  anAISContext->Display (aSphere, Standard_False);
  for (Standard_Integer aChildIdx = 0; aChildIdx < 5; ++aChildIdx)
  {
    BRepPrimAPI_MakeSphere aBuilder (gp_Pnt (1.0 + aChildIdx, 1.0 + aChildIdx, 1.0 + aChildIdx), 1.0);
    Handle(AIS_Shape) aChild = new AIS_Shape (aBuilder.Shape());
    aSphere->AddChild (aChild);
    anAISContext->Display (aChild, Standard_False);
  }

  anAISContext->RecomputeSelectionOnly (aSphere);
  anAISContext->UpdateCurrentViewer();

  return 0;
}

#include <IntTools_Context.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>

//=======================================================================
//function : xprojponf
//purpose  : 
//=======================================================================
Standard_Integer xprojponf (Draw_Interpretor& di, 
                            Standard_Integer n, 
                            const char** a)
{
  if (n!=3) {
    di<<" use xprojponf p f \n";
    return 0;
  }
  // 
  gp_Pnt aP, aPS;
  TopoDS_Shape aS;
  TopoDS_Face aF;
  Handle(IntTools_Context) aCtx;
  //
  DrawTrSurf::GetPoint(a[1], aP);
  aS=DBRep::Get(a[2]);
  //
  if (aS.IsNull()) {
    di<<" null shape is not allowed\n";
    return 0;
  }
  //
  if (aS.ShapeType()!=TopAbs_FACE) {
    di << a[2] << " not a face\n";
    return 0;
  }
  //
  aCtx=new IntTools_Context;
  //
  aF=TopoDS::Face(aS);
  GeomAPI_ProjectPointOnSurf& aPPS=aCtx->ProjPS(aF);
  //
  aPPS.Perform(aP);
  if (!aPPS.IsDone()) {
    di<<" projection failed\n";
    return 0;
  }
  //
  aPS=aPPS.NearestPoint();
  di<< " point px " << aPS.X() << " " << aPS.Y() << " " <<  aPS.Z() << "\n";
  //
  return 0;
}

//=======================================================================
//function : OCC25547
//purpose  :
//=======================================================================
#include <BRepMesh_CircleTool.hxx>
#include <SelectMgr_EntityOwner.hxx>

static Standard_Boolean inspect_point(const gp_XY&        thePoint,
                                      const gp_XY&        theCenter,
                                      const Standard_Real theRadius)
{
  static Standard_Real aPrecision   = Precision::PConfusion();
  static Standard_Real aSqPrecision = aPrecision * aPrecision;
  const gp_XY aDistVec = thePoint - theCenter;
  if (aDistVec.SquareModulus() - (theRadius * theRadius) < aSqPrecision)
    return Standard_True;
  else
    return Standard_False;
}

static Standard_Integer OCC24923(
  Draw_Interpretor& theDI, 
  Standard_Integer  argc, 
  const char **     argv)
{
  srand(static_cast<unsigned int>(time(NULL)));

  const Standard_Real    aMaxDeviation = (argc > 1) ? Draw::Atof(argv[1]) : 0.01;
  const Standard_Integer aPointsNb     = 10000000;
  const Standard_Real    aMinAngle     = 5 * M_PI / 180.;
  static Standard_Real   aSqPrecision  = Precision::PConfusion() * Precision::PConfusion();

  Standard_Integer aFailedNb = 0;
  for (Standard_Integer i = 0; i < aPointsNb; ++i)
  {
    gp_XY p[3];
    for (Standard_Integer j = 0; j < 3; ++j)
      p[j].SetCoord(((Standard_Real)rand())/RAND_MAX, ((Standard_Real)rand())/RAND_MAX);

    // Check that points do not compose degenerated triangle.
    gp_XY aVec1 = p[1] - p[0];
    gp_XY aVec2 = p[2] - p[0];
    if (aVec1.SquareModulus() > aSqPrecision && 
        aVec2.SquareModulus() > aSqPrecision &&
        (aVec1 ^ aVec2) > aMinAngle)
    {
      gp_XY aCenter;
      Standard_Real aRadius;
      if (BRepMesh_CircleTool::MakeCircle(p[0], p[1], p[2], aCenter, aRadius))
      {
        if (!inspect_point(p[0], aCenter, aRadius) || 
            !inspect_point(p[1], aCenter, aRadius) || 
            !inspect_point(p[2], aCenter, aRadius))
        {
         /* theDI << "Missed: " <<
            "p1=(" << p1.X() << ", " << p1.Y() << "), " <<
            "p2=(" << p2.X() << ", " << p2.Y() << "), " <<
            "p3=(" << p3.X() << ", " << p3.Y() << "), " <<
            "c=(" << aCenter.X() << ", " << aCenter.Y() << "), " <<
            "r=" << aRadius << "\n";*/
            
          ++aFailedNb;
        }

        continue;
      }
    }

    // Ensure that aPointsNb suitable for tests are generated
    --i;
  }

  const Standard_Real aDeviation = 
    1. - (Standard_Real)(aPointsNb - aFailedNb) / (Standard_Real)aPointsNb;

  theDI << "Number of failed cases: " << aFailedNb << " (Total " << aPointsNb << ")\n";
  if (aDeviation > aMaxDeviation)
  {
    theDI << "Failed. Number of incorrect results is too huge: " << 
      aDeviation * 100 << "% (Max " << aMaxDeviation * 100 << "%)\n";
    return 1;
  }

  theDI << "Deviation of incorrect results is: " <<
    aDeviation * 100 << "% (Max " << aMaxDeviation * 100 << "%)\n";
  theDI << "Test completed\n";
  return 0;
}

//=======================================================================
//function : OCC25574
//purpose  : check implementation of Euler angles in gp_Quaternion
//=======================================================================

static Standard_Integer OCC25574 (Draw_Interpretor& theDI, Standard_Integer /*argc*/, const char** /*argv*/)
{
  Standard_Boolean isTestOk = Standard_True;

  // Check consistency of Get and Set operations for Euler angles
  gp_Quaternion aQuat;
  aQuat.Set(0.06766916507860499, 0.21848101129786085, 0.11994599260380681,0.9660744746954637);
  Standard_Real alpha,beta,gamma;
  gp_Mat aRinv = aQuat.GetMatrix().Inverted();
  gp_Mat aI;
  aI.SetIdentity();
  const char* names[] = { "Extrinsic_XYZ", "Extrinsic_XZY", "Extrinsic_YZX", "Extrinsic_YXZ", "Extrinsic_ZXY", "Extrinsic_ZYX", 
                          "Intrinsic_XYZ", "Intrinsic_XZY", "Intrinsic_YZX", "Intrinsic_YXZ", "Intrinsic_ZXY", "Intrinsic_ZYX", 
                          "Extrinsic_XYX", "Extrinsic_XZX", "Extrinsic_YZY", "Extrinsic_YXY", "Extrinsic_ZYZ", "Extrinsic_ZXZ",
                          "Intrinsic_XYX", "Intrinsic_XZX", "Intrinsic_YZY", "Intrinsic_YXY", "Intrinsic_ZXZ", "Intrinsic_ZYZ" };
  for (int i = gp_Extrinsic_XYZ; i <= gp_Intrinsic_ZYZ; i++)
  {
    aQuat.GetEulerAngles (gp_EulerSequence(i), alpha, beta, gamma);

    gp_Quaternion aQuat2;
    aQuat2.SetEulerAngles (gp_EulerSequence(i), alpha, beta, gamma);

    gp_Mat aR = aQuat2.GetMatrix();
    gp_Mat aDiff = aR * aRinv - aI;
    if (aDiff.Determinant() > 1e-5)
    {
      theDI << "Error: Euler angles conversion incorrect for sequence " << names[i - gp_Extrinsic_XYZ] << "\n";
      isTestOk = Standard_False;
    }
  }

  // Check conversion between intrinsic and extrinsic rotations
  // Any extrinsic rotation is equivalent to an intrinsic rotation
  // by the same angles but with inverted order of elemental rotations, and vice versa
  // For instance:
  //    Extrinsic_XZY = Incrinsic_XZY
  //    R = X(A)Z(B)Y(G) --> R = Y(G)Z(B)X(A)
  alpha = 0.1517461713131;
  beta = 1.5162198410141;
  gamma = 1.9313156236541;
  Standard_Real alpha2, beta2, gamma2;
  gp_EulerSequence pairs[][2] = { {gp_Extrinsic_XYZ, gp_Intrinsic_ZYX},
                                  {gp_Extrinsic_XZY, gp_Intrinsic_YZX},
                                  {gp_Extrinsic_YZX, gp_Intrinsic_XZY},
                                  {gp_Extrinsic_YXZ, gp_Intrinsic_ZXY},
                                  {gp_Extrinsic_ZXY, gp_Intrinsic_YXZ},
                                  {gp_Extrinsic_ZYX, gp_Intrinsic_XYZ} };
  for (int i = 0; i < 6; i++)
  {
    aQuat.SetEulerAngles(pairs[i][0],  alpha,  beta,  gamma);
    aQuat.GetEulerAngles(pairs[i][1], gamma2, beta2, alpha2);

    if (Abs(alpha - alpha2) > 1e-5 || Abs(beta - beta2) > 1e-5 || Abs(gamma - gamma2) > 1e-5)
    {
      theDI << "Error: intrinsic and extrinsic conversion incorrect for sequence " << names[i] << "\n";
      isTestOk = Standard_False;
    }
  }

  // Check correspondence of enumeration and actual rotation it defines, 
  // by rotation by one axis and checking that it does not change a point on that axis
  for (int i = gp_Extrinsic_XYZ; i <= gp_Intrinsic_ZYZ; i++)
  {
    // Iterate over rotations R(A)R(B)R(G) for each Euler angle Alpha, Beta, Gamma
    // There are three ordered axes corresponding to three rotations.
    // Each rotation applyed with current angle around current axis.
    for (int j=0; j < 3; j++)
    {
      // note that current axis index is obtained by parsing of enumeration name!
      int anAxis = names[i - gp_Extrinsic_XYZ][10 + j] - 'X';
      Standard_ASSERT_RETURN (anAxis >=0 && anAxis <= 2, "Incorrect parsing of enumeration name", 1);

      // Set 90 degrees to current Euler angle
      double anAngles[3] = {0., 0., 0.};
      anAngles[j] = 0.5 * M_PI;

      gp_Quaternion q2;
      q2.SetEulerAngles (gp_EulerSequence(i), anAngles[0], anAngles[1], anAngles[2]);

      // Set point on axis corresponding to current rotation
      // We will apply rotation around this axis
      gp_XYZ v (0., 0., 0.);
      v.SetCoord (anAxis + 1, 1.);

      // Apply rotation to point
      gp_Trsf aT;
      aT.SetRotation (q2);
      gp_XYZ v2 = v;
      aT.Transforms (v2);

      // Check that point is still on origin position
      if ((v - v2).SquareModulus() > Precision::SquareConfusion())
      {
        // avoid reporting small coordinates
        for (int k=1; k <= 3; k++) 
          if (Abs (v2.Coord(k)) < Precision::Confusion())
            v2.SetCoord (k, 0.);

        isTestOk = Standard_False;
        theDI << "Error: Euler sequence " << names[i - gp_Extrinsic_XYZ] << " is incorrect:\n";
        theDI << "rotating by angle 90 deg around " << (anAxis == 0 ? "X" : anAxis == 1 ? "Y" : "Z") <<
                      " converts vector (" << v.X() << ", " << v.Y() << ", " << v.Z() << ") to ("
                        << v2.X() << ", " << v2.Y() << ", " << v2.Z() << ")\n";
      }
    }
  }

  // Check correspondence of enumeration and actual rotation it defines, 
  // by comparing cumulative rotation matrix with sequence of rotations by axes
  const Standard_Real anAngle[3] = { 0.1, 0.2, 0.3 };
  for (int i = gp_Extrinsic_XYZ; i <= gp_Intrinsic_ZYZ; i++)
  {
    // Sequence of rotations
    gp_Mat aR[3];
    for (int j=0; j < 3; j++)
    {
      // note that current axis index is obtained by parsing of enumeration name!
      int anAxis = names[i - gp_Extrinsic_XYZ][10 + j] - 'X';
      Standard_ASSERT_RETURN (anAxis >=0 && anAxis <= 2, "Incorrect parsing of enumeration name", 1);

      // Set point on axis corresponding to current rotation
      // We will apply rotation around this axis
      gp_XYZ v (0., 0., 0.);
      v.SetCoord (anAxis + 1, 1.);
      aR[j].SetRotation (v, anAngle[j]);
    }

    // construct cumulative transformation (differently for extrinsic and intrinsic rotations);
    // note that we parse first symbol of the enum name to identify its type
    gp_Mat aRot;
    if (names[i - gp_Extrinsic_XYZ][0] == 'E') // extrinsic
    {
      aRot = aR[2] * aR[1] * aR[0];
    }
    else // intrinsic
    {
      aRot = aR[0] * aR[1] * aR[2];
    }

    // set the same angles in quaternion
    aQuat.SetEulerAngles (gp_EulerSequence(i), anAngle[0], anAngle[1], anAngle[2]);

    gp_Mat aRQ = aQuat.GetMatrix();
    gp_Mat aDiff = aRQ * aRot.Inverted() - aI;
    if (aDiff.Determinant() > 1e-5)
    {
      theDI << "Error: Euler angles conversion does not correspond to sequential rotations for " << names[i - gp_Extrinsic_XYZ] << "\n";
      isTestOk = Standard_False;
    }
  }

  // similar checkfor YawPitchRoll sequence as defined in description of #25574
  {
    // Start with world coordinate system
    gp_Ax2 world;

    // Perform three rotations using the yaw-pitch-roll convention.
    // This means: rotate around the original z axis with angle alpha,
    // then rotate around the new y axis with angle beta,
    // then rotate around the new x axis with angle gamma.
    alpha = 0.0 / 180.0 * M_PI;
    beta = -35.0 / 180.0 * M_PI;
    gamma = 90.0 / 180.0 * M_PI;

    const gp_Quaternion rotationZ(world.Direction(), alpha);
    const gp_Vec rotY = rotationZ.Multiply(world.YDirection());
    const gp_Vec rotX = rotationZ.Multiply(world.XDirection());

    const gp_Quaternion rotationY(rotY, beta);
    const gp_Vec rotZ = rotationY.Multiply(world.Direction());
    const gp_Vec rotRotX = rotationY.Multiply(rotX);

    const gp_Quaternion rotationX(rotRotX, gamma);
    const gp_Vec rotRotZ = rotationX.Multiply(rotZ);

    gp_Ax2 result(gp_Pnt(0.0, 0.0, 0.0), rotRotZ, rotRotX);

    // Now compute the Euler angles
    gp_Trsf transformation;
    transformation.SetDisplacement(gp_Ax2(), result);

    Standard_Real computedAlpha;
    Standard_Real computedBeta;
    Standard_Real computedGamma;

    transformation.GetRotation().GetEulerAngles(gp_YawPitchRoll, computedAlpha, computedBeta, computedGamma);

    // We expect now to get the same angles as we have used for our rotations
    if (Abs(alpha - computedAlpha) > 1e-5 || Abs(beta - computedBeta) > 1e-5 || Abs(gamma - computedGamma) > 1e-5)
    {
      theDI << "Error: unexpected values of Euler angles for YawPitchRoll sequence:\n";
      theDI << "alpha: " << alpha / M_PI * 180.0 << " and computed alpha: "
            << computedAlpha / M_PI * 180.0 << "\n";
      theDI << "beta: " << beta / M_PI * 180.0 << " and computed beta: "
            << computedBeta / M_PI * 180.0 << "\n";
      theDI << "gamma: " << gamma / M_PI * 180.0 << " and computed gamma: "
            << computedGamma / M_PI * 180.0 << "\n";
      isTestOk = Standard_False;
    }
  }

  // test from #25946
  {
    gp_Quaternion q;
    q.Set(0.06766916507860499, 0.21848101129786085, 0.11994599260380681,0.9660744746954637);

    q.GetEulerAngles(gp_Intrinsic_ZYX, alpha,beta, gamma);
    q.GetEulerAngles(gp_Extrinsic_XYZ, alpha2,beta2,gamma2);

    // gp_Intrinsic_ZYX and gp_Extrinsic_XYZ should produce the same values of angles but in opposite order
    if (Abs(alpha - gamma2) > 1e-5 || Abs(beta - beta2) > 1e-5 || Abs(gamma - alpha2) > 1e-5)
    {
      theDI << "Error: Euler angles computed for gp_Intrinsic_ZYX and gp_Extrinsic_XYZ do not match:\n";
      theDI << "alpha: " << alpha / M_PI * 180.0 << " and " << alpha2 / M_PI * 180.0 << "\n";
      theDI << "beta: " << beta / M_PI * 180.0 << " and " << beta2 / M_PI * 180.0 << "\n";
      theDI << "gamma: " << gamma / M_PI * 180.0 << " and " << gamma2 / M_PI * 180.0 << "\n";
      isTestOk = Standard_False;
    }
  }

  theDI << (isTestOk ? "Test completed" : "Test failed") << "\n";
  return 0;
}

#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>
#include <GeomConvert.hxx>

//=======================================================================
//function : OCC26446
//purpose  : 
//=======================================================================
Standard_Integer OCC26446 (Draw_Interpretor& di, 
                           Standard_Integer n, 
                           const char** a)
{
  if (n != 4) {
    di << "Usage: OCC26446 r c1 c2\n";
    return 1;
  }

  Handle(Geom_BSplineCurve) aCurve1 =
    Handle(Geom_BSplineCurve)::DownCast(DrawTrSurf::GetCurve(a[2]));
  Handle(Geom_BSplineCurve) aCurve2 = 
    Handle(Geom_BSplineCurve)::DownCast(DrawTrSurf::GetCurve(a[3]));

  if (aCurve1.IsNull()) {
    di << a[2] << " is not a BSpline curve\n";
	return 1;
  }

  if (aCurve2.IsNull()) {
    di << a[3] << " is not a BSpline curve\n";
	return 1;
  }

  TColGeom_Array1OfBSplineCurve          aCurves     (0, 1);
  TColStd_Array1OfReal                   aTolerances (0, 0);
  Standard_Real                          aTolConf    = 1.e-3;
  Standard_Real                          aTolClosure = Precision::Confusion();
  Handle(TColGeom_HArray1OfBSplineCurve) aConcatCurves;
  Handle(TColStd_HArray1OfInteger)       anIndices;

  aCurves.SetValue(0, aCurve1);
  aCurves.SetValue(1, aCurve2);
  aTolerances.SetValue(0, aTolConf);

  GeomConvert::ConcatC1(aCurves,
                        aTolerances,
                        anIndices,
                        aConcatCurves,
                        Standard_False,
                        aTolClosure);

  Handle(Geom_BSplineCurve) aResult =
    aConcatCurves->Value(aConcatCurves->Lower());

  DrawTrSurf::Set(a[1], aResult);
  return 0;
}

static Standard_Integer OCC26448 (Draw_Interpretor& theDI, Standard_Integer, const char **)
{
  TColStd_SequenceOfReal aSeq1, aSeq2;
  aSeq1.Append(11.);
  aSeq1.Prepend (aSeq2);
  theDI << "TCollection: 11 -> " << aSeq1.First() << "\n";

  NCollection_Sequence<Standard_Real> nSeq1, nSeq2;
  nSeq1.Append(11.);
  nSeq1.Prepend (nSeq2);
  theDI << "NCollection: 11 -> " << nSeq1.First() << "\n";

  theDI << "OK";
  return 0;
}

//=======================================================================
//function : OCC26407
//purpose  :
//=======================================================================
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <TCollection_AsciiString.hxx>
static Standard_Integer OCC26407 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 2)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  // Construct vertices.
  std::vector<TopoDS_Vertex> wire_vertices;
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(587.90000000000009094947, 40.6758179230516248026106, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(807.824182076948432040808, 260.599999999999965893949, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(644.174182076948454778176, 424.249999999999943156581, 88.5000000000000142108547)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(629.978025792618950617907, 424.25, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(793.628025792618700506864, 260.599999999999852207111, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(587.900000000000204636308, 54.8719742073813492311274, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(218.521974207381418864315, 424.250000000000056843419, 88.5)));
  wire_vertices.push_back(BRepBuilderAPI_MakeVertex(gp_Pnt(204.325817923051886282337, 424.249999999999943156581, 88.5)));

  // Construct wire.
  BRepBuilderAPI_MakeWire wire_builder;
  for (size_t i = 0; i < wire_vertices.size(); i++)
  {
    const TopoDS_Vertex &v = wire_vertices[i];
    const TopoDS_Vertex &w = wire_vertices[(i+1) % wire_vertices.size()];

    wire_builder.Add(BRepBuilderAPI_MakeEdge(v, w));
  }

  // Create face and triangulate it.
  // Construct face.
  gp_Pnt v0 = BRep_Tool::Pnt(wire_vertices[0]);
  gp_Pnt v1 = BRep_Tool::Pnt(wire_vertices[1]);
  gp_Pnt v2 = BRep_Tool::Pnt(wire_vertices[wire_vertices.size() - 1]);

  gp_Vec face_normal = gp_Vec(v0, v1).Crossed(gp_Vec(v0, v2));

  TopoDS_Face face = BRepBuilderAPI_MakeFace(gp_Pln(v0, face_normal), wire_builder);
  BRepMesh_IncrementalMesh m(face, 1e-7);

  if (m.GetStatusFlags() != 0)
  {
    theDI << "Failed. Status for face constructed from vertices: " << m.GetStatusFlags() << "\n";
    return 1;
  }
  DBRep::Set(theArgVec[1], face);
  char buf[256];
  sprintf(buf, "isos %s 0", theArgVec[1]);
  theDI.Eval(buf);

  sprintf(buf, "triangles %s", theArgVec[1]);
  theDI.Eval(buf);

  theDI.Eval("smallview; fit");

  theDI << "Test completed\n";
  return 0;
}

//=======================================================================
//function : OCC26485
//purpose  :
//=======================================================================
#include <Poly.hxx>
static Standard_Integer OCC26485 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 2)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(theArgVec[1]);
  if (aShape.IsNull())
  {
    theDI << "Failed. Null shape\n";
    return 1;
  }

  Standard_Boolean isFailed = Standard_False;
  TopExp_Explorer aExplorer(aShape, TopAbs_FACE);
  for (; aExplorer.More(); aExplorer.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face( aExplorer.Current() );
    TopLoc_Location L = TopLoc_Location();
    const Handle(Poly_Triangulation)& aT = BRep_Tool::Triangulation( aFace , L );

    if(aT.IsNull())
      continue;

    Poly::ComputeNormals(aT);
    const TColgp_Array1OfPnt&       aVertices = aT->Nodes();
    const TShort_Array1OfShortReal& aNormals  = aT->Normals();

    // Number of nodes in the triangulation
    int aVertexNb = aT->Nodes().Length();
    if (aVertexNb*3 != aNormals.Length())
    {
      theDI << "Failed. Different number of normals vs. vertices\n";
      return 1;
    }

    // Get each vertex index, checking common vertexes between shapes
    for( int i=0; i < aVertexNb; i++ )
    {
      gp_Pnt aPoint = aVertices.Value( i+1 );
      gp_Vec aNormal = gp_Vec(
        aNormals.Value( i*3 + 1 ),
        aNormals.Value( i*3 + 2 ),
        aNormals.Value( i*3 + 3 ) );

      if (aNormal.X() == 0 && aNormal.Y() == 0 && aNormal.Z() == 1)
      {
        char buf[256];
        sprintf(buf, "fail_%d", i+1);
        theDI << "Failed. Point " << buf << ": "
              << aPoint.X() << " "
              << aPoint.Y() << " "
              << aPoint.Z() << "\n";

        DrawTrSurf::Set (buf, aPoint);
      }
    }
  }

  theDI << (isFailed ? "Test failed" : "Test completed") << "\n";
  return 0;
}

//=======================================================================
//function : OCC26553
//purpose  :
//=======================================================================
#include <BRepBuilderAPI_MakeWire.hxx>

static Standard_Integer OCC26553 (Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
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

  TopoDS_Edge aPipeEdge = BRepBuilderAPI_MakeEdge (gp_Pnt (0, 0, 0), gp_Pnt (0, 0, 10));
  TopoDS_Wire aPipeWire = BRepBuilderAPI_MakeWire(aPipeEdge).Wire();

  BRepOffsetAPI_MakePipe aPipeBuilder(aPipeWire, aShell);
  if (!aPipeBuilder.IsDone())
  {
    theDI << "Error: failed to create pipe\n";
    return 1;
  }

  for (TopExp_Explorer aShapeExplorer(aShell, TopAbs_EDGE); aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aGeneratedShape = aPipeBuilder.Generated(aPipeEdge, aShapeExplorer.Current());
    if (aGeneratedShape.IsNull())
    {
      theDI << "Error: null shape\n";
      return 1;
    }
  }

  theDI << "History returned successfully\n";
  return 0;
}

//=======================================================================
//function : OCC26195
//purpose  :
//=======================================================================
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <Geom_CartesianPoint.hxx>
#include <AIS_Line.hxx>
#include <Aspect_Window.hxx>
static Standard_Integer OCC26195 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb < 3)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  if (ViewerTest::GetAISContext().IsNull())
  {
    std::cerr << "Error: No opened context!\n";
    return 1;
  }

  gp_Pnt2d aPxPnt1, aPxPnt2;
  aPxPnt1.SetX (Draw::Atof (theArgVec[1]));
  aPxPnt1.SetY (Draw::Atof (theArgVec[2]));
  if (theArgNb > 4)
  {
    aPxPnt2.SetX (Draw::Atof (theArgVec[3]));
    aPxPnt2.SetY (Draw::Atof (theArgVec[4]));
  }
  Standard_Boolean toPrint = Standard_False;
  if (theArgNb % 2 == 0)
  {
    toPrint = Draw::Atoi (theArgVec[theArgNb - 1]) != 0;
  }

  SelectMgr_SelectingVolumeManager* aMgr = new SelectMgr_SelectingVolumeManager();
  aMgr->SetActiveSelectionType (theArgNb > 4 ?
    SelectMgr_SelectingVolumeManager::Box : SelectMgr_SelectingVolumeManager::Point);
  aMgr->SetCamera (ViewerTest::CurrentView()->Camera());
  aMgr->SetPixelTolerance (ViewerTest::GetAISContext()->PixelTolerance());
  Standard_Integer aWidth, aHeight;
  ViewerTest::CurrentView()->View()->Window()->Size (aWidth, aHeight);
  aMgr->SetWindowSize (aWidth, aHeight);
  if (theArgNb > 4)
  {
    aMgr->BuildSelectingVolume (aPxPnt1, aPxPnt2);
  }
  else
  {
    aMgr->BuildSelectingVolume (aPxPnt1);
  }
  const gp_Pnt* aVerts = aMgr->GetVertices();
  gp_Pnt aNearPnt = aMgr->GetNearPickedPnt();
  gp_Pnt aFarPnt  = aMgr->GetFarPickedPnt();
  BRepBuilderAPI_MakePolygon aWireBldrs[4];

  aWireBldrs[0].Add (gp_Pnt (aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));
  aWireBldrs[0].Add (gp_Pnt (aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));
  aWireBldrs[0].Add (gp_Pnt (aVerts[6].X(), aVerts[6].Y(), aVerts[6].Z()));
  aWireBldrs[0].Add (gp_Pnt (aVerts[2].X(), aVerts[2].Y(), aVerts[2].Z()));
  aWireBldrs[0].Add (gp_Pnt (aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));

  aWireBldrs[1].Add (gp_Pnt (aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));
  aWireBldrs[1].Add (gp_Pnt (aVerts[5].X(), aVerts[5].Y(), aVerts[5].Z()));
  aWireBldrs[1].Add (gp_Pnt (aVerts[7].X(), aVerts[7].Y(), aVerts[7].Z()));
  aWireBldrs[1].Add (gp_Pnt (aVerts[6].X(), aVerts[6].Y(), aVerts[6].Z()));
  aWireBldrs[1].Add (gp_Pnt (aVerts[4].X(), aVerts[4].Y(), aVerts[4].Z()));

  aWireBldrs[2].Add (gp_Pnt (aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));
  aWireBldrs[2].Add (gp_Pnt (aVerts[5].X(), aVerts[5].Y(), aVerts[5].Z()));
  aWireBldrs[2].Add (gp_Pnt (aVerts[7].X(), aVerts[7].Y(), aVerts[7].Z()));
  aWireBldrs[2].Add (gp_Pnt (aVerts[3].X(), aVerts[3].Y(), aVerts[3].Z()));
  aWireBldrs[2].Add (gp_Pnt (aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));

  aWireBldrs[3].Add (gp_Pnt (aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));
  aWireBldrs[3].Add (gp_Pnt (aVerts[1].X(), aVerts[1].Y(), aVerts[1].Z()));
  aWireBldrs[3].Add (gp_Pnt (aVerts[3].X(), aVerts[3].Y(), aVerts[3].Z()));
  aWireBldrs[3].Add (gp_Pnt (aVerts[2].X(), aVerts[2].Y(), aVerts[2].Z()));
  aWireBldrs[3].Add (gp_Pnt (aVerts[0].X(), aVerts[0].Y(), aVerts[0].Z()));

  TopoDS_Compound aComp;
  BRep_Builder    aCompBuilder;
  aCompBuilder.MakeCompound (aComp);
  for (Standard_Integer aWireIdx = 0; aWireIdx < 4; ++aWireIdx)
  {
    aCompBuilder.Add (aComp, aWireBldrs[aWireIdx].Shape());
  }
  DBRep::Set ("c", aComp);

  Handle(AIS_InteractiveObject) aCmp = new AIS_Shape (aComp);
  aCmp->SetColor (Quantity_NOC_GREEN);
  ViewerTest::Display ("c", aCmp, Standard_True, Standard_True);

  Handle(Geom_CartesianPoint) aPnt1 = new Geom_CartesianPoint (aNearPnt);
  Handle(Geom_CartesianPoint) aPnt2 = new Geom_CartesianPoint (aFarPnt);

  Handle(AIS_Line) aLine = new AIS_Line (aPnt1, aPnt2);
  ViewerTest::Display ("l", aLine, Standard_True, Standard_True);

  if (toPrint)
  {
    theDI << "Near: " << aNearPnt.X() << " " << aNearPnt.Y() << " " << aNearPnt.Z() << "\n";
    theDI << "Far: " << aFarPnt.X() << " " << aFarPnt.Y() << " " << aFarPnt.Z() << "\n";
  }

  return 0;
}

//=======================================================================
//function : OCC26462
//purpose  :
//=======================================================================
static Standard_Integer OCC26462 (Draw_Interpretor& theDI, Standard_Integer /*theArgNb*/, const char** /*theArgVec*/)
{
  if (ViewerTest::GetAISContext().IsNull())
  {
    std::cerr << "Error: No opened context!\n";
    return 1;
  }

  BRepPrimAPI_MakeBox aBuilder1 (gp_Pnt (10.0, 10.0, 0.0), 10.0, 10.0, 10.0);
  BRepPrimAPI_MakeBox aBuilder2 (10.0, 10.0, 10.0);
  Handle(AIS_InteractiveObject) aBox1 = new AIS_Shape (aBuilder1.Shape());
  Handle(AIS_InteractiveObject) aBox2 = new AIS_Shape (aBuilder2.Shape());

  const Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  aCtx->Display (aBox1, 0, 2, Standard_False);
  aCtx->Display (aBox2, 0, 2, Standard_False);
  ViewerTest::CurrentView()->FitAll();
  aCtx->SetWidth (aBox1, 3, Standard_False);
  aCtx->SetWidth (aBox2, 3, Standard_False);

  aCtx->MoveTo (305, 322, ViewerTest::CurrentView(), Standard_False);
  aCtx->ShiftSelect (Standard_False);
  aCtx->MoveTo (103, 322, ViewerTest::CurrentView(), Standard_False);
  aCtx->ShiftSelect (Standard_False);
  if (aCtx->NbSelected() != 0)
  {
    theDI << "ERROR: no boxes must be selected!\n";
    return 1;
  }

  aCtx->SetSelectionSensitivity (aBox1, 2, 5);

  aCtx->MoveTo (305, 322, ViewerTest::CurrentView(), Standard_False);
  aCtx->ShiftSelect (Standard_False);
  if (aCtx->NbSelected() != 1)
  {
    theDI << "ERROR: b1 was not selected\n";
    return 1;
  }
  aCtx->MoveTo (103, 322, ViewerTest::CurrentView(), Standard_False);
  aCtx->ShiftSelect (Standard_True);
  if (aCtx->NbSelected() != 1)
  {
    theDI << "ERROR: b2 is selected after b1's tolerance increased\n";
    return 1;
  }

  return 0;
}


#include <BRepBuilderAPI_GTransform.hxx>
static Standard_Integer OCC26313(Draw_Interpretor& di,Standard_Integer n,const char** a)
{
  if (n <= 1) return 1;
  
  gp_Trsf T;
  gp_GTrsf GT(T);
  
  gp_Mat rot( 1.0, 0.0, 0.0,
              0.0, 2.0, 0.0,
              0.0, 0.0, 3.0);
  
  GT.SetVectorialPart(rot);
  BRepBuilderAPI_GTransform gtrf(GT);

  TopoDS_Shape aSrcShape = DBRep::Get(a[2]);
  if (aSrcShape.IsNull()) {
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
    catch(Standard_Failure)
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
//function : OCC26525
//purpose  : check number of intersection points
//=======================================================================
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <IntCurveSurface_HInter.hxx>
Standard_Integer OCC26525 (Draw_Interpretor& di, 
                           Standard_Integer n, 
                           const char** a)
{
  TopoDS_Shape aS1, aS2;
  TopoDS_Edge aE; 
  TopoDS_Face aF;

  if (n<4)
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
  if (aS1.ShapeType()!=TopAbs_EDGE)
  {
    di << " Shape" << a[2] << " should be of type EDGE\n";
    return 0;
  }
  if (aS2.ShapeType()!=TopAbs_FACE)
  {
    di << " Shape" << a[3] << " should be of type FACE\n";
    return 0;
  }

  aE=TopoDS::Edge(aS1);
  aF=TopoDS::Face(aS2);

  char buf[128];
  Standard_Boolean bIsDone;
  Standard_Integer i, aNbPoints;
  Standard_Real aU, aV, aT;
  gp_Pnt aP;
  BRepAdaptor_Curve aBAC;
  BRepAdaptor_Surface aBAS;
  IntCurveSurface_TransitionOnCurve aTC;
  IntCurveSurface_HInter aHInter;

  aBAC.Initialize(aE);
  aBAS.Initialize(aF);

  Handle(BRepAdaptor_HCurve) aHBAC=new BRepAdaptor_HCurve(aBAC);
  Handle(BRepAdaptor_HSurface) aHBAS = new BRepAdaptor_HSurface(aBAS);

  aHInter.Perform(aHBAC, aHBAS);
  bIsDone=aHInter.IsDone();
  if (!bIsDone)
  {
    di << " intersection is not done\n";
    return 0;
  }

  aNbPoints=aHInter.NbPoints();
  sprintf (buf, " Number of intersection points found: %d", aNbPoints);
  di <<  buf << "\n";
  for (i=1; i<=aNbPoints; ++i)
  {
    const IntCurveSurface_IntersectionPoint& aIP=aHInter.Point(i);
    aIP.Values(aP, aU, aV, aT, aTC);
    //
    sprintf (buf, "point %s_%d %lg %lg %lg  ", a[1], i, aP.X(), aP.Y(), aP.Z());
    di << buf << "\n";
  }

  return 0;
}

//=======================================================================
//function : OCC24537
//purpose  : Puts inverted numbers (in the sense of little/big endian inversion)
//           from predefined arrays.
//=======================================================================
#include <FSD_BinaryFile.hxx>

template<int size>
inline const unsigned char* SizeRef ();

template<>
inline const unsigned char* SizeRef <8> ()
{
  static const unsigned char aSizeRef[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x09,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  return aSizeRef;
}

template<>
inline const unsigned char* SizeRef <4> ()
{
  static const unsigned char aSizeRef[] = {
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,
    0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x06,
    0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x08,
    0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x00};
  return aSizeRef;
}

static Standard_Integer OCC24537(
  Draw_Interpretor& theDI, 
  Standard_Integer  argc, 
  const char **     argv)
{
  std::ofstream aF;
  if (argc > 1)
  {
    aF.open(argv[1]);
    if (!aF.is_open())
    {
      cout << "cannot create file " << argv[1] << endl;
      return 1;
    }
  }
  Standard_Boolean isErr = Standard_False;
  // 1. InverseInt
  const unsigned char anIntRef[] = {
    0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,
    0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x04,
    0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x06,
    0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x08,
    0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x00};
  Standard_Integer anIntArr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_Integer anInv = FSD_BinaryFile::InverseInt(anIntArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_Integer anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseInt(anIntArr[i]);
    if (memcmp(anInv, anIntRef, sizeof(anIntRef)) != 0)
    {
      theDI << "Error: incorrect conversion of an integer value\n";
      isErr = Standard_True;
    }
  }
  
  // 1a. Random InverseInt
  const unsigned char aRndIntRef[] = {
    0xFF,0xC2,0xF7,0x00,0xFF,0xFF,0xFB,0x2E,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x00,0x00,0x04,0xD2,0x00,0x00,0x04,0xD3,
    0xFF,0xFF,0xFD,0x1E,0xFF,0xFF,0xFF,0xFB,
    0x00,0x00,0x03,0x8D,0x00,0x3D,0x09,0x00};
  Standard_Integer aRndIntArr[] = {-4000000, -1234, 0, 1, 1234, 1235, -738, -5, 909, 4000000};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_Integer anInv = FSD_BinaryFile::InverseInt(aRndIntArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_Integer anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseInt(aRndIntArr[i]);
    if (memcmp(anInv, aRndIntRef, sizeof(aRndIntRef)) != 0)
    {
      theDI << "Error: incorrect conversion of a dispersed integer value\n";
      isErr = Standard_True;
    }
  }
  
  // 2. InverseReal
  const unsigned char aRealRef[] = {
    0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x08,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x14,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x20,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x22,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
  const Standard_Real aRealArr[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 0.0};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_Real anInv = FSD_BinaryFile::InverseReal(aRealArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_Real anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseReal(aRealArr[i]);
    if (memcmp(anInv, aRealRef, sizeof(aRealRef)) != 0)
    {
      theDI << "Error: incorrect conversion of a real value\n";
      isErr = Standard_True;
    }
  }

  // 2a. Random InverseReal
  const unsigned char aRndRealRef[] = {
    0xFE,0x37,0xE4,0x3C,0x88,0x00,0x75,0x9C,
    0xBE,0x11,0x2E,0x0B,0xE8,0x26,0xD6,0x95,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x3E,0x11,0x2E,0x0B,0xE8,0x26,0xD6,0x95,
    0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
    0x40,0x09,0x21,0xDA,0x45,0x5B,0x53,0xE4,
    0x54,0xB2,0x49,0xAD,0x25,0x94,0xC3,0x7D,
    0x40,0x20,0x00,0x00,0x00,0x00,0x00,0x00,
    0xC0,0x23,0xCC,0xCC,0xCC,0xCC,0xCC,0xCD,
    0x40,0x23,0xCC,0xCC,0xCC,0xCC,0xCC,0xCD};
  const Standard_Real aRndRealArr[] = {-1e300, -1.e-9, 0., 1.e-9, 1., 3.1415296, 1.e100, 8.0, -9.9, 9.9};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_Real anInv = FSD_BinaryFile::InverseReal(aRndRealArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_Real anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseReal(aRndRealArr[i]);
    if (memcmp(anInv, aRndRealRef, sizeof(aRndRealRef)) != 0)
    {
      theDI << "Error: incorrect conversion of a dispersed real value\n";
      isErr = Standard_True;
    }
  }

  // 3. InverseShortReal
  const unsigned char aShortRealRef[] = {
    0x3F,0x80,0x00,0x00,0x40,0x00,0x00,0x00,
    0x40,0x40,0x00,0x00,0x40,0x80,0x00,0x00,
    0x40,0xA0,0x00,0x00,0x40,0xC0,0x00,0x00,
    0x40,0xE0,0x00,0x00,0x41,0x00,0x00,0x00,
    0x41,0x10,0x00,0x00,0x00,0x00,0x00,0x00};
  const Standard_ShortReal aShortRealArr[] = {
    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 0.0f};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_ShortReal anInv = FSD_BinaryFile::InverseShortReal(aShortRealArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_ShortReal anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseShortReal(aShortRealArr[i]);
    if (memcmp(anInv, aShortRealRef, sizeof(aShortRealRef)) != 0)
    {
      theDI << "Error: incorrect conversion of a short real value\n";
      isErr = Standard_True;
    }
  }

  // 3a. Random InverseShortReal
  const unsigned char aRndShortRealRef[] = {
    0xB0,0x89,0x70,0x5F,0x00,0x00,0x00,0x00,
    0x30,0x89,0x70,0x5F,0x3F,0x80,0x00,0x00,
    0x40,0x49,0x0E,0x56,0xC0,0xD6,0x66,0x66,
    0x40,0xD6,0x66,0x66,0x42,0xC5,0xCC,0xCD,
    0xC2,0xC7,0xCC,0xCD,0x42,0xC7,0xCC,0xCD};
  const Standard_ShortReal aRndShortRealArr[] = {
    -1.e-9f, 0.f, 1.e-9f, 1.f, 3.1415f, -6.7f, 6.7f, 98.9f, -99.9f, 99.9f};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_ShortReal anInv = FSD_BinaryFile::InverseShortReal(aRndShortRealArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_ShortReal anInv[10];
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseShortReal(aRndShortRealArr[i]);
    if (memcmp(anInv, aRndShortRealRef, sizeof(aRndShortRealRef)) != 0)
    {
      theDI << "Error: incorrect conversion of a dispersed short real value\n";
      isErr = Standard_True;
    }
  }

  // 4. InverseSize
  const Standard_Size aSizeArr[] = {1ul, 2ul, 3ul, 4ul, 5ul, 6ul, 7ul, 8ul, 9ul, 0ul};
  if (aF.is_open())
  {
    for(int i = 0; i < 10; ++i)
    {
      Standard_Size anInv = FSD_BinaryFile::InverseSize(aSizeArr[i]);
      aF.write(reinterpret_cast<char*>(&anInv), sizeof(anInv));
    }
  }
  else
  {
    Standard_Size anInv[10];
    const unsigned char* aSizeRef = SizeRef<sizeof(Standard_Size)>();
    for(int i = 0; i < 10; ++i)
      anInv[i] = FSD_BinaryFile::InverseSize(aSizeArr[i]);
    if (memcmp(anInv, aSizeRef, sizeof(Standard_Size)*10) != 0)
    {
      theDI << "Error: incorrect conversion of a size value\n";
      isErr = Standard_True;
    }
  }

  if (!aF.is_open() && !isErr)
    theDI << "Conversion was done OK";
  if (aF.is_open())
  {
    cout << "the file " << argv[1] << " has been created" << endl;
    aF.close();
  }
  return 0;
}


#include <TopExp.hxx>
#include <BRepOffsetAPI_DraftAngle.hxx>
#include <vector>
static TopoDS_Shape taper(const TopoDS_Shape &shape, const TopoDS_Face &face_a, const TopoDS_Face &face_b, Standard_Real angle)
{
  // Use maximum face-to-taper z-offset.
  const gp_Pln neutral_plane(gp_Ax3(gp_Pnt(0.0, 0.0, 140.0), gp_Dir(0.0, 0.0, 1.0)));

  // Draft angle needs to be in radians, and flipped to adhere to our own (arbitrary) draft
  // angle definition.
  const Standard_Real draft_angle = -(angle / 180.0) * M_PI;

  // Add face to draft. The first argument indicates that all material added/removed during
  // drafting is located below the neutral plane
  BRepOffsetAPI_DraftAngle drafter(shape);
  drafter.Add(face_a, gp_Dir(0.0, 0.0, -1.0), draft_angle, neutral_plane);
  drafter.Add(face_b, gp_Dir(0.0, 0.0, -1.0), draft_angle, neutral_plane);
  drafter.Build();

  return drafter.Shape();
}

static void dumpShapeVertices(const TopoDS_Shape &shape, std::vector<Standard_Real>& coords)
{
  TopTools_IndexedMapOfShape shape_vertices;
  TopExp::MapShapes(shape, TopAbs_VERTEX, shape_vertices);

  for (Standard_Integer i = 1; i <= shape_vertices.Extent(); i++)
  {
    gp_Pnt p = BRep_Tool::Pnt(TopoDS::Vertex(shape_vertices(i)));
    coords.push_back(p.X());
    coords.push_back(p.Y());
    coords.push_back(p.Z());
  }
}

static void GetCoords(const Standard_CString& path_to_file, std::vector<Standard_Real>& coords)
{
  TopoDS_Shape shape;
  BRep_Builder builder;
  BRepTools::Read(shape, path_to_file, builder);
  TopTools_IndexedMapOfShape shape_faces;
  TopExp::MapShapes(shape, TopAbs_FACE, shape_faces);
  TopoDS_Face face_a = TopoDS::Face(shape_faces(1));
  TopoDS_Face face_b = TopoDS::Face(shape_faces(5));
  dumpShapeVertices(taper(shape, face_a, face_b, 5.0), coords);
}

static Standard_Integer OCC26396 (Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
{
  if (theArgc < 2)
  {
    theDI << "Error: path to file is missing\n";
    return 1;
  }

  const int maxInd = 50;

  std::vector<Standard_Real> ref_coords;
  ref_coords.reserve(100);
  Standard_Boolean Stat = Standard_True;

  GetCoords(theArgv[1], ref_coords);

  std::vector<Standard_Real> coords;
  coords.reserve(100);
  for (int i = 1; i < maxInd; i++)
  {
    GetCoords(theArgv[1], coords);
    if (coords.size() != ref_coords.size())
    {
      Stat = Standard_False;
      break;
    }
    for (size_t j = 0; j < coords.size(); j++)
      if (Abs(ref_coords[j] - coords[j]) > RealEpsilon())
      {
        Stat = Standard_False;
        break;
      }
    coords.clear();
  }
  if (!Stat)
    theDI << "Error: unstable results";
  else
    theDI << "test OK";

  return 0;
}

//=======================================================================
//function : OCC26750 
//purpose  : 
//=======================================================================
static Standard_Integer OCC26750( Draw_Interpretor& theDI,
                                  Standard_Integer  /*theNArg*/,
                                  const char ** /*theArgVal*/)
{
  const gp_Vec2d aVec1(1.0, 0.0);
  const gp_Vec2d aVec2(0.0, -1.0);

  if(aVec1.IsNormal(aVec2, Precision::Angular()))
  {
    theDI << "gp_Vec2d OK. Vectors are normal.\n";
  }
  else
  {
    theDI << "Error in gp_Vec2d. Vectors should be normal.\n";
  }

  const gp_Dir2d aD1(1.0, 0.0);
  const gp_Dir2d aD2(0.0, -1.0);

  if(aD1.IsNormal(aD2, Precision::Angular()))
  {
    theDI << "gp_Dir2d OK. Vectors are normal.\n";
  }
  else
  {
    theDI << "Error in gp_Dir2d. Vectors should be normal.\n";
  }
  
  return 0;
}

//=======================================================================
//function : OCC26746 
//purpose  : Checks if coefficients of the torus are computed properly.
//=======================================================================
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BSplineCurve.hxx>
static Standard_Integer OCC26746(
  Draw_Interpretor& theDI, 
  Standard_Integer  theNArg, 
  const char **     theArgVal)
{
  if(theNArg < 2)
  {
    theDI << "Use: OCC26746 torus [toler NbCheckedPoints]\n";
    return 1;
  }

  const Handle(Geom_ToroidalSurface) aGtor = 
    Handle(Geom_ToroidalSurface)::DownCast(DrawTrSurf::GetSurface(theArgVal[1]));

  const Standard_Real aToler = (theNArg >= 3)? Draw::Atof(theArgVal[2]) : 1.0e-7;
  const Standard_Integer aNbPntsMax = (theNArg >= 4)? Draw::Atoi(theArgVal[3]) : 5;

  const Standard_Integer aLowIndex = 5;
  const Standard_Real aStep = 2.0*M_PI/aNbPntsMax;

  TColStd_Array1OfReal anArrCoeffs(aLowIndex, aLowIndex+34);
  aGtor->Torus().Coefficients(anArrCoeffs);

  Standard_Real aUpar = 0.0, aVpar = 0.0;
  for(Standard_Integer aUind = 0; aUind <= aNbPntsMax; aUind++)
  {
    for(Standard_Integer aVind = 0; aVind <= aNbPntsMax; aVind++)
    {
      const gp_Pnt aPt(aGtor->Value(aUpar, aVpar));
      const Standard_Real aX1 = aPt.X();
      const Standard_Real aX2 = aX1*aX1;
      const Standard_Real aX3 = aX2*aX1;
      const Standard_Real aX4 = aX2*aX2;
      const Standard_Real aY1 = aPt.Y();
      const Standard_Real aY2 = aY1*aY1;
      const Standard_Real aY3 = aY2*aY1;
      const Standard_Real aY4 = aY2*aY2;
      const Standard_Real aZ1 = aPt.Z();
      const Standard_Real aZ2 = aZ1*aZ1;
      const Standard_Real aZ3 = aZ2*aZ1;
      const Standard_Real aZ4 = aZ2*aZ2;

      Standard_Integer i = aLowIndex;

      Standard_Real aDelta =  anArrCoeffs(i++) * aX4;  //1
      aDelta+= anArrCoeffs(i++) * aY4;                 //2
      aDelta+= anArrCoeffs(i++) * aZ4;                 //3
      aDelta+= anArrCoeffs(i++) * aX3 * aY1;           //4
      aDelta+= anArrCoeffs(i++) * aX3 * aZ1;           //5
      aDelta+= anArrCoeffs(i++) * aY3 * aX1;           //6
      aDelta+= anArrCoeffs(i++) * aY3 * aZ1;           //7
      aDelta+= anArrCoeffs(i++) * aZ3 * aX1;           //8
      aDelta+= anArrCoeffs(i++) * aZ3 * aY1;           //9
      aDelta+= anArrCoeffs(i++) * aX2 * aY2;           //10
      aDelta+= anArrCoeffs(i++) * aX2 * aZ2;           //11
      aDelta+= anArrCoeffs(i++) * aY2 * aZ2;           //12
      aDelta+= anArrCoeffs(i++) * aX2 * aY1 * aZ1;     //13
      aDelta+= anArrCoeffs(i++) * aX1 * aY2 * aZ1;     //14
      aDelta+= anArrCoeffs(i++) * aX1 * aY1 * aZ2;     //15
      aDelta+= anArrCoeffs(i++) * aX3;                 //16
      aDelta+= anArrCoeffs(i++) * aY3;                 //17
      aDelta+= anArrCoeffs(i++) * aZ3;                 //18
      aDelta+= anArrCoeffs(i++) * aX2 * aY1;           //19
      aDelta+= anArrCoeffs(i++) * aX2 * aZ1;           //20
      aDelta+= anArrCoeffs(i++) * aY2 * aX1;           //21
      aDelta+= anArrCoeffs(i++) * aY2 * aZ1;           //22
      aDelta+= anArrCoeffs(i++) * aZ2 * aX1;           //23
      aDelta+= anArrCoeffs(i++) * aZ2 * aY1;           //24
      aDelta+= anArrCoeffs(i++) * aX1 * aY1 * aZ1;     //25
      aDelta+= anArrCoeffs(i++) * aX2;                 //26
      aDelta+= anArrCoeffs(i++) * aY2;                 //27
      aDelta+= anArrCoeffs(i++) * aZ2;                 //28
      aDelta+= anArrCoeffs(i++) * aX1 * aY1;           //29
      aDelta+= anArrCoeffs(i++) * aX1 * aZ1;           //30
      aDelta+= anArrCoeffs(i++) * aY1 * aZ1;           //31
      aDelta+= anArrCoeffs(i++) * aX1;                 //32
      aDelta+= anArrCoeffs(i++) * aY1;                 //33
      aDelta+= anArrCoeffs(i++) * aZ1;                 //34
      aDelta+= anArrCoeffs(i++);                       //35

      if(Abs(aDelta) > aToler)
      {
        theDI << "(" << aUpar << ", " << aVpar << "): Error in torus coefficients computation (Delta = " << aDelta << ").\n";
      }
      else
      {
        theDI << "(" << aUpar << ", " << aVpar << "): OK (Delta = " << aDelta << ").\n";
      }

      aVpar = (aVind == aNbPntsMax)? 2.0*M_PI : aVpar + aStep;
    }

    aVpar = 0.0;
    aUpar = (aUind == aNbPntsMax)? 2.0*M_PI : aUpar + aStep;
  }

  return 0;     
}

DEFINE_STANDARD_HANDLE(QABugs_VertexFilter, SelectMgr_Filter)
class QABugs_VertexFilter: public SelectMgr_Filter
{
public:
  Standard_EXPORT QABugs_VertexFilter() : SelectMgr_Filter() {}

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)&) const
  {
    return Standard_False;
  }
};

//=======================================================================
//function : BUC26658 
//purpose  : Checks selection in the context after using a selection filter
//=======================================================================
static Standard_Integer BUC26658 (Draw_Interpretor& theDI,
                                  Standard_Integer  /*theNArg*/,
                                  const char ** theArgVal)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    theDI << "use 'vinit' command before " << theArgVal[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBoxShape = BRepPrimAPI_MakeBox(20,20,20).Shape();
  Handle(AIS_Shape) anAISIO = new AIS_Shape(aBoxShape);

  // visualization of the box in the local mode with possibility to
  // select box vertices
  Standard_DISABLE_DEPRECATION_WARNINGS
  aContext->OpenLocalContext();
  Standard_ENABLE_DEPRECATION_WARNINGS

  int aDispMode = 0;// wireframe
  anAISIO->SetDisplayMode(aDispMode);
  aContext->Display(anAISIO, aDispMode, 0, false, true, AIS_DS_Displayed); 
  theDI.Eval(" vfit");

  aContext->Load(anAISIO, -1, true); /// load allowing decomposition
  aContext->Deactivate(anAISIO);
  aContext->Activate(anAISIO, AIS_Shape::SelectionMode(TopAbs_VERTEX), false);
  aContext->UpdateCurrentViewer();

  // select a point on the box
  Handle(V3d_View) myV3dView = ViewerTest::CurrentView();
  double Xv,Yv;
  myV3dView->Project(20,20,0,Xv,Yv);
  Standard_Integer Xp,Yp;
  myV3dView->Convert(Xv,Yv,Xp,Yp);

  aContext->MoveTo (Xp, Yp, myV3dView, Standard_False);
  aContext->Select (Standard_False);
  bool aHasSelected = false;
  for (aContext->InitSelected(); aContext->MoreSelected() && !aHasSelected; aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    if (!anIO.IsNull()) {
      const TopoDS_Shape aShape = aContext->SelectedShape();
      if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX)
        aHasSelected = true;
    }
  }
  if (aHasSelected)
     cout << "has selected vertex : OK"   << endl;
  else {
    theDI << "has selected vertex : bugged - Faulty\n";
    return 1;
  }
  // filter to deny any selection in the viewer
  Handle(QABugs_VertexFilter) aFilter = new QABugs_VertexFilter();
  aContext->AddFilter(aFilter);

  // update previous selection by hand
  Standard_DISABLE_DEPRECATION_WARNINGS
  aContext->LocalContext()->ClearOutdatedSelection(anAISIO, true);
  Standard_ENABLE_DEPRECATION_WARNINGS

  // check that there are no selected vertices
  aContext->Select (Standard_True);
  aHasSelected = false;
  for (aContext->InitSelected(); aContext->MoreSelected() && !aHasSelected; aContext->NextSelected()) {
    Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
    if (!anIO.IsNull()) {
      const TopoDS_Shape aShape = aContext->SelectedShape();
      if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX)
        aHasSelected = true;
    }
  }
  if (!aHasSelected) cout << "has no selected vertex after filter : OK"   << endl;
  else {
    theDI << "has no selected vertex after filter : bugged - Faulty\n";
    return 1;
  }

  return 0;
}

//=======================================================================
//function : OCC26945_open
//purpose  : Opens local context and activates given standard selection mode
//=======================================================================
static Standard_Integer OCC26945_open (Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  if (theArgc < 2)
  {
    std::cout << "Not enough arguments. See usage:\n";
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  const TopAbs_ShapeEnum aSelType = AIS_Shape::SelectionType (Draw::Atoi (theArgv[1]));
  Standard_DISABLE_DEPRECATION_WARNINGS
  Standard_Integer aLocalCtxIdx = aCtx->OpenLocalContext();
  aCtx->ActivateStandardMode (aSelType);
  Standard_ENABLE_DEPRECATION_WARNINGS
  theDI << aLocalCtxIdx;

  return 0;
}

//=======================================================================
//function : OCC26945_close
//purpose  : Closes local context with the id given
//=======================================================================
static Standard_Integer OCC26945_close (Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  if (theArgc < 2)
  {
    std::cout << "Not enough arguments. See usage:\n";
    theDI.PrintHelp (theArgv[0]);
    return 1;
  }

  const Standard_Integer aCtxToClose = Draw::Atoi (theArgv[1]);
  Standard_DISABLE_DEPRECATION_WARNINGS
  aCtx->CloseLocalContext (aCtxToClose);
  Standard_ENABLE_DEPRECATION_WARNINGS

  return 0;
}

//=======================================================================
//function : OCC27048
//purpose  : Calculate value of B-spline surface N times
//=======================================================================
static Standard_Integer OCC27048(Draw_Interpretor& theDI, Standard_Integer theArgc, const char** theArgv)
{
  if (theArgc != 5)
  {
    std::cout << "Incorrect number of arguments. See usage:" << std::endl;
    theDI.PrintHelp(theArgv[0]);
    return 1;
  }

  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(theArgv[1]);
  GeomAdaptor_Surface anAdaptor(aSurf);

  Standard_Real aU = Draw::Atof(theArgv[2]);
  Standard_Real aV = Draw::Atof(theArgv[3]);
  Standard_Integer aN = Draw::Atoi(theArgv[4]);

  for (; aN > 0; --aN)
    anAdaptor.Value(aU, aV);

  return 0;
}

//========================================================================
//function : OCC27065
//purpose  : Tests overloaded method "Generated" of BRepOffsetAPI_MakePipe
//========================================================================
static Standard_Integer OCC27065(Draw_Interpretor& di,
                                 Standard_Integer n, const char** a)
{
  if (n < 3) return 1;
  BRep_Builder BB;

  TopoDS_Shape SpineShape = DBRep::Get(a[1],TopAbs_WIRE);
  if ( SpineShape.IsNull()) return 1;
  TopoDS_Wire Spine = TopoDS::Wire(SpineShape);

  TopoDS_Shape Profile = DBRep::Get(a[2]);
  if ( Profile.IsNull()) return 1;

  BRepOffsetAPI_MakePipe aPipeBuilder(Spine, Profile);
  if (!aPipeBuilder.IsDone())
  {
    di << "Error: failed to create pipe\n";
    return 1;
  }

  TopExp_Explorer Explo(Profile, TopAbs_SHELL);
  TopoDS_Shape aShape;
  TopTools_ListIteratorOfListOfShape itl;
  if (Explo.More())
  {
    aShape = Explo.Current();
    TopoDS_Compound res1;
    BB.MakeCompound(res1);
    itl.Initialize(aPipeBuilder.Generated(aShape));
    for (; itl.More(); itl.Next())
      BB.Add(res1, itl.Value());
    DBRep::Set("res_shell", res1);
  }

  Explo.Init(Profile, TopAbs_FACE);
  if (Explo.More())
  {
    aShape = Explo.Current();
    TopoDS_Compound res2;
    BB.MakeCompound(res2);
    itl.Initialize(aPipeBuilder.Generated(aShape));
    for (; itl.More(); itl.Next())
      BB.Add(res2, itl.Value());
    DBRep::Set("res_face", res2);
  }
  
  Explo.Init(Profile, TopAbs_WIRE);
  if (Explo.More())
  {
    aShape = Explo.Current();
    TopoDS_Compound res3;
    BB.MakeCompound(res3);
    itl.Initialize(aPipeBuilder.Generated(aShape));
    for (; itl.More(); itl.Next())
      BB.Add(res3, itl.Value());
    DBRep::Set("res_wire", res3);
  }
  
  Explo.Init(Profile, TopAbs_EDGE);
  if (Explo.More())
  {
    aShape = Explo.Current();
    TopoDS_Compound res4;
    BB.MakeCompound(res4);
    itl.Initialize(aPipeBuilder.Generated(aShape));
    for (; itl.More(); itl.Next())
      BB.Add(res4, itl.Value());
    DBRep::Set("res_edge", res4);
  }
  
  Explo.Init(Profile, TopAbs_VERTEX);
  if (Explo.More())
  {
    aShape = Explo.Current();
    TopoDS_Compound res5;
    BB.MakeCompound(res5);
    itl.Initialize(aPipeBuilder.Generated(aShape));
    for (; itl.More(); itl.Next())
      BB.Add(res5, itl.Value());
    DBRep::Set("res_vertex", res5);
  }
  
  return 0;
}

//========================================================================
//function : OCC27318
//purpose  : Creates a box that is not listed in map of AIS objects of ViewerTest
//========================================================================
static Standard_Integer OCC27318 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgc*/, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox (20, 20, 20).Shape();
  Handle(AIS_Shape) aBoxObj = new AIS_Shape (aBox);
  aCtx->Display (aBoxObj, Standard_True);

  return 0;
}

//========================================================================
//function : OCC27523
//purpose  : Checks recomputation of deactivated selection mode after object's redisplaying
//========================================================================
static Standard_Integer OCC27523 (Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 1)
  {
    std::cerr << "Error: wrong number of arguments! See usage:\n";
    theDI.PrintHelp (theArgVec[0]);
    return 1;
  }

  Handle(AIS_InteractiveContext) anAISContext = ViewerTest::GetAISContext();
  if(anAISContext.IsNull())
  {
    std::cerr << "Error: no active view. Please call vinit.\n";
    return 1;
  }

  gp_Pnt aStart (100, 100, 100);
  gp_Pnt anEnd (300, 400, 600);
  BRepBuilderAPI_MakeEdge anEdgeBuilder (aStart, anEnd);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  Handle(AIS_InteractiveObject) aTestAISShape = new AIS_Shape (anEdge);
  anAISContext->Display (aTestAISShape, Standard_False);

  // activate it in selection modes
  TColStd_SequenceOfInteger aModes;
  aModes.Append (AIS_Shape::SelectionMode ((TopAbs_ShapeEnum) TopAbs_VERTEX));

  anAISContext->Deactivate (aTestAISShape);
  anAISContext->Load (aTestAISShape, -1, true);
  anAISContext->Activate (aTestAISShape, 0);
  anAISContext->Deactivate (aTestAISShape, 0);

  // activate in vertices mode
  for (Standard_Integer anIt = 1; anIt <= aModes.Length(); ++anIt)
  {
    anAISContext->Activate (aTestAISShape, aModes (anIt));
  }

  TopoDS_Shape aVertexShape = BRepBuilderAPI_MakeVertex (gp_Pnt (75, 0, 0));
  TopAbs_ShapeEnum aVertexShapeType = aVertexShape.ShapeType();
  Handle(AIS_Shape)::DownCast (aTestAISShape)->Set (aVertexShape);
  aTestAISShape->Redisplay();

  anAISContext->AddOrRemoveSelected (aTestAISShape, Standard_True);

  bool aValidShapeType = false;
  for (anAISContext->InitSelected(); anAISContext->MoreSelected(); anAISContext->NextSelected())
  {
    Handle(SelectMgr_EntityOwner) anOwner = anAISContext->SelectedOwner();
    Handle(StdSelect_BRepOwner) aBRO = Handle(StdSelect_BRepOwner)::DownCast (anOwner);
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
//function : OCC27700
//purpose  : glPolygonMode() used for frame drawing affects label text shading
//========================================================================

class OCC27700_Text : public AIS_InteractiveObject
{
public:

  DEFINE_STANDARD_RTTI_INLINE (OCC27700_Text, AIS_InteractiveObject)

  virtual void Compute (const Handle(PrsMgr_PresentationManager3d)& /*thePresentationManager*/,
                        const Handle(Prs3d_Presentation)& thePresentation,
                        const Standard_Integer /*theMode*/) Standard_OVERRIDE
  {
    Handle(Graphic3d_ArrayOfTriangles) aFrame = new Graphic3d_ArrayOfTriangles (6, 6);
    aFrame->AddVertex (gp_Pnt (-1, 0, 0));
    aFrame->AddVertex (gp_Pnt (-1, 1, 0));
    aFrame->AddVertex (gp_Pnt ( 3, 1, 0));
    aFrame->AddVertex (gp_Pnt ( 3, 0, 0));

    aFrame->AddEdge (1);
    aFrame->AddEdge (2);
    aFrame->AddEdge (3);

    aFrame->AddEdge (2);
    aFrame->AddEdge (3);
    aFrame->AddEdge (4);

    Handle(Graphic3d_AspectFillArea3d) aFillAspect =
      new Graphic3d_AspectFillArea3d (*myDrawer->ShadingAspect()->Aspect().get());
    aFillAspect->SetInteriorStyle (Aspect_IS_POINT);

    // create separate group for frame elements
    Handle(Graphic3d_Group) aFrameGroup = Prs3d_Root::NewGroup (thePresentation);
    aFrameGroup->AddPrimitiveArray (aFrame);
    aFrameGroup->SetGroupPrimitivesAspect (aFillAspect);

    // create separate group for text elements
    Handle(Graphic3d_Group) aTextGroup = Prs3d_Root::NewGroup (thePresentation);
    TCollection_ExtendedString aString ("YOU SHOULD SEE THIS TEXT", Standard_True);
    Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (thePresentation), myDrawer->TextAspect(), aString, gp_Ax2 (gp::Origin(), gp::DZ()));
  }

  virtual void ComputeSelection (const Handle(SelectMgr_Selection)& /*theSelection*/,
                                 const Standard_Integer /*theMode*/) Standard_OVERRIDE {}
};

static Standard_Integer OCC27700 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgNb*/, const char** /*theArgVec*/)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    std::cout << "Error: no view available, call 'vinit' before!" << std::endl;
    return 1;
  }
  Handle(OCC27700_Text) aPresentation = new OCC27700_Text();
  aContext->Display (aPresentation, Standard_True);
  return 0;
}

//========================================================================
//function : OCC27757
//purpose  : Creates a box that has a sphere as child object and displays it
//========================================================================
static Standard_Integer OCC27757 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgc*/, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox (20.0, 20.0, 20.0).Shape();
  TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere (10.0).Shape();
  gp_Trsf aTrsf;
  aTrsf.SetTranslationPart (gp_Vec (20.0, 20.0, 0.0));
  aSphere.Located (TopLoc_Location (aTrsf));


  Handle(AIS_Shape) aBoxObj = new AIS_Shape (aBox);
  Handle(AIS_Shape) aSphereObj = new AIS_Shape (aSphere);
  aBoxObj->AddChild (aSphereObj);
  aCtx->Display (aBoxObj, 1, 0, Standard_False);
  aCtx->UpdateCurrentViewer();

  return 0;
}

//========================================================================
//function : OCC27818
//purpose  : Creates three boxes and highlights one of them with own style
//========================================================================
static Standard_Integer OCC27818 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgc*/, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  Handle(AIS_Shape) aBoxObjs[3];
  for (Standard_Integer aBoxIdx = 0; aBoxIdx < 3; ++aBoxIdx)
  {
    TopoDS_Shape aBox = BRepPrimAPI_MakeBox (20.0, 20.0, 20.0).Shape();
    aBoxObjs[aBoxIdx] = new AIS_Shape (aBox);
    gp_Trsf aTrsf;
    aTrsf.SetTranslationPart (gp_Vec (30.0 * aBoxIdx, 30.0 * aBoxIdx, 0.0));
    aBoxObjs[aBoxIdx]->SetLocalTransformation (aTrsf);
    aBoxObjs[aBoxIdx]->SetHilightMode (AIS_Shaded);
  }

  {
    Handle(Prs3d_Drawer) aHiStyle = new Prs3d_Drawer();
    aBoxObjs[1]->SetDynamicHilightAttributes (aHiStyle);
    aHiStyle->SetDisplayMode (AIS_Shaded);
    aHiStyle->SetColor (Quantity_NOC_RED);
    aHiStyle->SetTransparency (0.8f);
  }
  {
    Handle(Prs3d_Drawer) aSelStyle = new Prs3d_Drawer();
    aBoxObjs[2]->SetHilightAttributes (aSelStyle);
    aSelStyle->SetDisplayMode (AIS_Shaded);
    aSelStyle->SetColor (Quantity_NOC_RED);
    aSelStyle->SetTransparency (0.0f);
    aSelStyle->SetZLayer (Graphic3d_ZLayerId_Topmost);
  }

  for (Standard_Integer aBoxIdx = 0; aBoxIdx < 3; ++aBoxIdx)
  {
    aCtx->Display (aBoxObjs[aBoxIdx], AIS_Shaded, 0, Standard_False);
  }

  aCtx->UpdateCurrentViewer();

  return 0;
}

//========================================================================
//function : OCC27893
//purpose  : Creates a box and selects it via AIS_InteractiveContext API
//========================================================================
static Standard_Integer OCC27893 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgc*/, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox (10.0, 10.0, 10.0).Shape();
  Handle(AIS_InteractiveObject) aBoxObj = new AIS_Shape (aBox);
  aCtx->Display (aBoxObj, AIS_Shaded, 0, Standard_False);
  aCtx->SetSelected (aBoxObj, Standard_True);

  return 0;
}

//========================================================================
//function : OCC28310
//purpose  : Tests validness of iterator in AIS_InteractiveContext after
// an removing object from it
//========================================================================
static Standard_Integer OCC28310 (Draw_Interpretor& /*theDI*/, Standard_Integer /*theArgc*/, const char** theArgv)
{
  const Handle(AIS_InteractiveContext)& aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "No interactive context. Use 'vinit' command before " << theArgv[0] << "\n";
    return 1;
  }

  TopoDS_Shape aBox = BRepPrimAPI_MakeBox (10.0, 10.0, 10.0).Shape();
  Handle(AIS_InteractiveObject) aBoxObj = new AIS_Shape (aBox);
  aCtx->Display (aBoxObj, AIS_Shaded, 0, Standard_False);
  ViewerTest::CurrentView()->FitAll();
  aCtx->MoveTo (200, 200, ViewerTest::CurrentView(), Standard_True);
  aCtx->Select(Standard_True);

  aCtx->Remove (aBoxObj, Standard_True);
  // nullify the object explicitly to simulate situation in project,
  // when ::Remove is called from another method and the object is destroyed
  // before ::DetectedInteractive is called
  aBoxObj.Nullify();

  for (aCtx->InitDetected(); aCtx->MoreDetected(); aCtx->NextDetected())
  {
    Handle(AIS_InteractiveObject) anObj = aCtx->DetectedInteractive();
  }

  return 0;
}

//========================================================================
//function : Commands_19
//purpose  :
//========================================================================

void QABugs::Commands_19(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  Handle(QABugs_HandleClass) aClassPtr = new QABugs_HandleClass();
  theCommands.Add ("OCC24202_1", "Test Handle-based procedure",
                   __FILE__, aClassPtr, &QABugs_HandleClass::HandleProc, group);
  NCollection_Handle<QABugs_NHandleClass> aNClassPtr = new QABugs_NHandleClass();
  theCommands.Add ("OCC24202_2", "Test NCollection_Handle-based procedure",
                   __FILE__, aNClassPtr, &QABugs_NHandleClass::NHandleProc, group);

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC23361", "OCC23361", __FILE__, OCC23361, group);
  theCommands.Add ("OCC23237", "OCC23237", __FILE__, OCC23237, group); 
  theCommands.Add ("OCC22980", "OCC22980", __FILE__, OCC22980, group);
  theCommands.Add ("OCC23595", "OCC23595", __FILE__, OCC23595, group);
  theCommands.Add ("OCC22611", "OCC22611 string nb", __FILE__, OCC22611, group);
  theCommands.Add ("OCC22595", "OCC22595", __FILE__, OCC22595, group);
  theCommands.Add ("OCC23774", "OCC23774 shape1 shape2", __FILE__, OCC23774, group);
  theCommands.Add ("OCC23683", "OCC23683 shape", __FILE__, OCC23683, group);
  theCommands.Add ("OCC23952sweep", "OCC23952sweep nbupoles shape", __FILE__, OCC23952sweep, group);
  theCommands.Add ("OCC23952intersect", "OCC23952intersect nbsol shape1 shape2", __FILE__, OCC23952intersect, group);
  theCommands.Add ("test_offset", "test_offset", __FILE__, test_offset, group);
  theCommands.Add ("OCC23945", "OCC23945 surfname U V X Y Z [DUX DUY DUZ DVX DVY DVZ [D2UX D2UY D2UZ D2VX D2VY D2VZ D2UVX D2UVY D2UVZ]]", __FILE__, OCC23945,group);
  theCommands.Add ("OCC24008", "OCC24008 curve surface", __FILE__, OCC24008, group);
  theCommands.Add ("OCC11758", "OCC11758", __FILE__, OCC11758, group);
  theCommands.Add ("OCC24005", "OCC24005 result", __FILE__, OCC24005, group);
  theCommands.Add ("OCC24137", "OCC24137 face vertex U V [N]", __FILE__, OCC24137, group);
  theCommands.Add ("OCC24271", "Boolean operations on NCollection_Map", __FILE__, OCC24271, group);
  theCommands.Add ("OCC23972", "OCC23972", __FILE__, OCC23972, group);
  theCommands.Add ("OCC24370", "OCC24370 edge pcurve surface prec", __FILE__, OCC24370, group);
  theCommands.Add ("OCC24533", "OCC24533", __FILE__, OCC24533, group);
  theCommands.Add ("OCC24012", "OCC24012 face edge", __FILE__, OCC24012, group);
  theCommands.Add ("OCC24086", "OCC24086 face wire", __FILE__, OCC24086, group);
  theCommands.Add ("OCC24667", "OCC24667 result Wire_spine Profile [Mode [Approx]], no args to get help", __FILE__, OCC24667, group);
  theCommands.Add ("OCC24755", "OCC24755", __FILE__, OCC24755, group);
  theCommands.Add ("OCC24834", "OCC24834", __FILE__, OCC24834, group);
  theCommands.Add ("OCC24889", "OCC24889", __FILE__, OCC24889, group);
  theCommands.Add ("OCC23951", "OCC23951 path to saved step file", __FILE__, OCC23951, group);
  theCommands.Add ("OCC24931", "OCC24931 path to saved xml file", __FILE__, OCC24931, group);
  theCommands.Add ("OCC24945", "OCC24945", __FILE__, OCC24945, group);
  theCommands.Add ("OCC23950", "OCC23950 step_file", __FILE__, OCC23950, group);
  theCommands.Add ("OCC25004", "OCC25004", __FILE__, OCC25004, group);
  theCommands.Add ("OCC24925",
                   "OCC24925 filename [pluginLib=TKXml storageGuid retrievalGuid]"
                   "\nOCAF persistence without setting environment variables",
                   __FILE__, OCC24925, group);
  theCommands.Add ("OCC25043", "OCC25043 shape", __FILE__, OCC25043, group);
  theCommands.Add ("OCC24826,", "This test performs simple saxpy test.\n Usage: OCC24826 length", __FILE__, OCC24826, group);
  theCommands.Add ("OCC24606", "OCC24606 : Tests ::FitAll for V3d view ('vfit' is for NIS view)", __FILE__, OCC24606, group);
  theCommands.Add ("OCC25202", "OCC25202 res shape numF1 face1 numF2 face2", __FILE__, OCC25202, group);
  theCommands.Add ("OCC7570", "OCC7570 shape", __FILE__, OCC7570, group);
  theCommands.Add ("OCC25100", "OCC25100 shape", __FILE__, OCC25100, group);
  theCommands.Add ("OCC25340", "OCC25340", __FILE__, OCC25340, group);
  theCommands.Add ("OCC25348", "OCC25348", __FILE__, OCC25348, group);
  theCommands.Add ("OCC25413", "OCC25413 shape", __FILE__, OCC25413, group);
  theCommands.Add ("OCC25446", "OCC25446 res b1 b2 op", __FILE__, OCC25446, group);
  theCommands.Add ("OCC25545", 
                   "no args; tests data race when concurrently accessing \n"
                   "\t\tTopLoc_Location::Transformation()",
                   __FILE__, OCC25545, group);
  theCommands.Add ("OCC25547", "OCC25547", __FILE__, OCC25547, group);
  theCommands.Add ("OCC24881", "OCC24881 shape", __FILE__, OCC24881, group);
  theCommands.Add ("OCC26172", "OCC26172", __FILE__, OCC26172, group);
  theCommands.Add ("xprojponf", "xprojponf p f", __FILE__, xprojponf, group);
  theCommands.Add ("OCC24923", "OCC24923", __FILE__, OCC24923, group);
  theCommands.Add ("OCC26139", "OCC26139 [-boxsize value] [-boxgrid value] [-compgrid value]", __FILE__, OCC26139, group);
  theCommands.Add ("OCC26284", "OCC26284", __FILE__, OCC26284, group);
  theCommands.Add ("OCC26446", "OCC26446 r c1 c2", __FILE__, OCC26446, group);
  theCommands.Add ("OCC26448", "OCC26448: check method Prepend() of sequence", __FILE__, OCC26448, group);
  theCommands.Add ("OCC26407", "OCC26407 result_name", __FILE__, OCC26407, group);
  theCommands.Add ("OCC26485", "OCC26485 shape", __FILE__, OCC26485, group);
  theCommands.Add ("OCC26553", "OCC26553 file_path", __FILE__, OCC26553, group);
  theCommands.Add ("OCC26195",
                   "OCC26195: x1_pix y1_pix [x2_pix y2_pix] [toPrintPixelCoord 0|1]"
                   "\n\t\t: Draws rectangular selecting frustum defined by point selection in pixel coordinates"
                   "\n\t\t: [x1_pix, y1_pix] or rectangular selection in pixel coordinates [x1_pix, y1_pix,"
                   "\n\t\t: x2_pix, y2_pix]."
                   "\n\t\t: [toPrintPixelCoord 0|1] - prints 3d projection of pixel coordinate or center of"
                   "\n\t\t: selecting rectangle onto near and far view frustum planes",
                   __FILE__, OCC26195, group);
  theCommands.Add ("OCC26462",
                   "OCC26462: Checks the ability to manage sensitivity of a particular selection mode in local context",
                   __FILE__, OCC26462, group);

  theCommands.Add ("OCC26313", "OCC26313 result shape", __FILE__, OCC26313, group);
  theCommands.Add ("OCC26396", "OCC26396 shape_file_path", __FILE__, OCC26396, group);
  theCommands.Add ("OCC26525", "OCC26525 result edge face ", __FILE__, OCC26525, group);

  theCommands.Add ("OCC24537", "OCC24537 [file]", __FILE__, OCC24537, group);
  theCommands.Add ("OCC26750", "OCC26750", __FILE__, OCC26750, group);
  theCommands.Add ("OCC25574", "OCC25574", __FILE__, OCC25574, group);
  theCommands.Add ("OCC26746", "OCC26746 torus [toler NbCheckedPoints] ", __FILE__, OCC26746, group);

  theCommands.Add ("BUC26658", "BUC26658 unexpected selection in the context using a selection filter", __FILE__, BUC26658, group);
  theCommands.Add ("OCC26945_open",
                   "OCC26945 selectionModeToActivate"
                   "\n\t\t: Opens a new local context with selectionModeToActivate activated."
                   "\n\t\t: Prints the ID of newely opened local context in case of success.",
                   __FILE__, OCC26945_open, group);
  theCommands.Add ("OCC26945_close",
                   "OCC26945 localCtxToClose"
                   "\n\t\t: Closes local context with the ID localCtxToClose",
                   __FILE__, OCC26945_close, group);

  theCommands.Add ("OCC27048",
                   "OCC27048 surf U V N\nCalculate value of surface N times in the point (U, V)",
                   __FILE__, OCC27048, group);
  
  theCommands.Add ("OCC27065",
                   "OCC27065 spine profile",
                   __FILE__, OCC27065, group);

  theCommands.Add ("OCC27318",
                   "OCC27318: Creates a box that is not listed in map of AIS objects of ViewerTest",
                   __FILE__, OCC27318, group);
  theCommands.Add ("OCC27523",
                   "OCC27523: Checks recomputation of deactivated selection mode after object's redisplaying",
                   __FILE__, OCC27523, group);
  theCommands.Add ("OCC27700",
                   "OCC27700: Checks drawing text after setting interior style",
                   __FILE__, OCC27700, group);
  theCommands.Add ("OCC27757",
                   "OCC27757: Creates a box that has a sphere as child object and displays it",
                   __FILE__, OCC27757, group);
  theCommands.Add ("OCC27818",
                   "OCC27818: Creates three boxes and highlights one of them with own style",
                   __FILE__, OCC27818, group);
  theCommands.Add ("OCC27893",
                   "OCC27893: Creates a box and selects it via AIS_InteractiveContext API",
                   __FILE__, OCC27893, group);
  theCommands.Add("OCC28310",
                  "OCC28310: Tests validness of iterator in AIS_InteractiveContext after an removing object from it",
                  __FILE__, OCC28310, group);
  return;
}
