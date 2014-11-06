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

#include <Draw_Interpretor.hxx>
#include <DBRep.hxx>
#include <DrawTrSurf.hxx>
#include <ViewerTest.hxx>
#include <V3d_View.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_TexturedShape.hxx>
#include <Image_PixMap.hxx>
#include <Image_Color.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Ax1.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <DrawTrSurf.hxx>

#include <Precision.hxx>

#include <PCollection_HAsciiString.hxx>

#include <cstdio>
#include <cmath>
#include <iostream>
#include <OSD_PerfMeter.hxx>
#include <OSD_Timer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepAlgo_Cut.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_Handle.hxx>
#include <TCollection_HAsciiString.hxx>
#include <GeomFill_Trihedron.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>

#include <Standard_Version.hxx>

#define QCOMPARE(val1, val2) \
  di << "Checking " #val1 " == " #val2 << \
        ((val1) == (val2) ? ": OK\n" : ": Error\n")

static Standard_Integer OCC230 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 4) {
    di << "ERROR OCC230: Usage : " << argv[0] << " TrimmedCurve Pnt2d Pnt2d" << "\n";
    return 1;
  }

  gp_Pnt2d P1, P2;
  if ( !DrawTrSurf::GetPoint2d(argv[2],P1)) {
    di << "ERROR OCC230: " << argv[2] << " is not Pnt2d" << "\n";
    return 1;
  }
  if ( !DrawTrSurf::GetPoint2d(argv[3],P2)) {
    di << "ERROR OCC230: " << argv[3] << " is not Pnt2d" << "\n";
    return 1;
  }

  GCE2d_MakeSegment MakeSegment(P1,P2);
  Handle(Geom2d_TrimmedCurve) TrimmedCurve = MakeSegment.Value();
  DrawTrSurf::Set(argv[1], TrimmedCurve);
  return 0;
}

static Standard_Integer OCC142 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char ** /*argv*/)
{
  for(Standard_Integer i= 0;i <= 20;i++){
    Handle(PCollection_HAsciiString) pstr = new PCollection_HAsciiString("TEST");
    pstr->Clear();
  }
  di << "OCC142: OK" << "\n";
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
    di << "ERROR OCC23361: equivalent transformations does not produce equal points" << "\n";
  else 
    di << "OCC23361: OK" << "\n";

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
    BRepAlgo_Cut aCutter (aBox.Shape(), aSphere.Shape());

    aTM.Stop();
    aPM.Stop();
  }
 
  int aNbEnters = 0;
  Standard_Real aPerfMeter_CPUtime = 0., aTimer_ElapsedTime = aTM.ElapsedTime();

  perf_get_meter("TestMeter", &aNbEnters, &aPerfMeter_CPUtime);

  Standard_Real aTimeDiff = (fabs(aTimer_ElapsedTime - aPerfMeter_CPUtime) / aTimer_ElapsedTime);

  printf("\nMeasurement results (%d cycles):\n", count);
  printf("\nOSD_PerfMeter CPU time: %lf\nOSD_Timer elapsed time: %lf\n", aPerfMeter_CPUtime, aTimer_ElapsedTime);
  printf("Time delta is: %.3lf %%\n", aTimeDiff * 100);

  if (aTimeDiff > 0.2)
    di << "OCC23237: Error: too much difference between CPU and elapsed times";
  else if (aNbEnters != count)
    di << "OCC23237: Error: counter reported by PerfMeter (" << aNbEnters << ") does not correspond to actual number of cycles";
  else
    di << "OCC23237: OK";

  return 0;
}

#ifdef HAVE_TBB

#include <Standard_Atomic.hxx>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

class IncrementerDecrementer
{
public:
    IncrementerDecrementer (Standard_Integer* theVal, Standard_Boolean thePositive) : myVal (theVal), myPositive (thePositive)
    {}
    void operator() (const tbb::blocked_range<size_t>& r) const
    {
        if (myPositive)
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Increment (myVal);
        else
            for (size_t i = r.begin(); i != r.end(); ++i)
                Standard_Atomic_Decrement (myVal);
    }
private:
    Standard_Integer*   myVal;
    Standard_Boolean   myPositive;
};
#endif

#ifdef HAVE_TBB
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
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, true));
  QCOMPARE (aSum, N);

  //decrement
  tbb::parallel_for (tbb::blocked_range<size_t> (0, N), IncrementerDecrementer (&aSum, false));
  QCOMPARE (aSum, 0);

  return 0;
}

#else /* HAVE_TBB */

static Standard_Integer OCC22980 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char **argv)
{
  di << "Test skipped: command " << argv[0] << " requires TBB library\n";
  return 0;
}

#endif /* HAVE_TBB */

#include <TDocStd_Application.hxx>
#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>

static Standard_Integer OCC23595 (Draw_Interpretor& di, Standard_Integer /*argc*/, const char** /*argv*/)
{
  const Handle(TDocStd_Application)& anApp = XCAFApp_Application::GetApplication();
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
    di << "Not Done1 " << "\n";
    return Standard_False;
  }
  const TopoDS_Shape& step1ModifiedShape = transformer1.ModifiedShape(originalWire);
  
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
  const TopoDS_Shape& step2ModifiedShape = transformer1.ModifiedShape(step1ModifiedShape);

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
static void GeomConvertTest (Draw_Interpretor& di, Standard_Integer theTargetNbUPoles, Standard_CString theFileName)
{
	Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(theFileName);
	GeomConvert_ApproxSurface aGAS (aSurf, 1e-4, GeomAbs_C1, GeomAbs_C1, 9, 9, 100, 1);
	if (!aGAS.IsDone()) {
		di << "ApproxSurface is not done!" << "\n";
		return;
	}
	const Handle(Geom_BSplineSurface)& aBSurf = aGAS.Surface();
	if (aBSurf.IsNull()) {
		di << "BSplineSurface is not created!" << "\n";
		return;
	}
	di << "Number of UPoles:" << aBSurf->NbUPoles() << "\n";
	QCOMPARE (aBSurf->NbUPoles(), theTargetNbUPoles);
}

struct aData {
	Draw_Interpretor* di;
	Standard_Integer nbupoles;
	Standard_CString filename;
};

Standard_EXPORT Standard_Address convert(Standard_Address data)
{
	aData* info = (aData*) data;
	GeomConvertTest(*(info->di),info->nbupoles,info->filename);
	return NULL;
}

static Standard_Integer OCC23952sweep (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	if (argc != 3) {
		di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
		return 1;
	}
	struct aData aStorage;
	aStorage.di = &di;
	aStorage.nbupoles = Draw::Atoi(argv[1]); 
	aStorage.filename = argv[2];

	OSD_Thread aThread1(convert);
	aThread1.Run(&aStorage);
	GeomConvertTest(di,aStorage.nbupoles,aStorage.filename);
	cout << "result of thread: " << aThread1.Wait() << endl;

	return 0;
}

#include <GeomInt_IntSS.hxx>
static void GeomIntSSTest (Draw_Interpretor& di, Standard_Integer theNbSol, Standard_CString theFileName1, Standard_CString theFileName2)
{
	Handle(Geom_Surface) aSurf1 = DrawTrSurf::GetSurface(theFileName1);
	Handle(Geom_Surface) aSurf2 = DrawTrSurf::GetSurface(theFileName2);
	GeomInt_IntSS anInter;
	anInter.Perform(aSurf1, aSurf2, Precision::Confusion(), Standard_True);
	if (!anInter.IsDone()) {
		di << "An intersection is not done!" << "\n";
		return;
	}

	di << "Number of Lines:" << anInter.NbLines() << "\n";
	QCOMPARE (anInter.NbLines(), theNbSol);
}

struct aNewData {
	Draw_Interpretor* di;
	Standard_Integer nbsol;
	Standard_CString filename1;
	Standard_CString filename2;
};
Standard_EXPORT Standard_Address convert_inter(Standard_Address data)
{
	aNewData* info = (aNewData*) data;
	GeomIntSSTest(*(info->di),info->nbsol,info->filename1,info->filename2);
	return NULL;
}

static Standard_Integer OCC23952intersect (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
	if (argc != 4) {
		di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
		return 1;
	}
	struct aNewData aStorage;
	aStorage.di = &di;
	aStorage.nbsol = Draw::Atoi(argv[1]); 
	aStorage.filename1 = argv[2];
	aStorage.filename2 = argv[3];

	OSD_Thread aThread1(convert_inter);
	aThread1.Run(&aStorage);
	GeomIntSSTest(di,aStorage.nbsol,aStorage.filename1,aStorage.filename2);
	cout << "result of thread: " << aThread1.Wait() << endl;

	return 0;
}

#include <Geom_SurfaceOfRevolution.hxx> 
static Standard_Integer OCC23683 (Draw_Interpretor& di, Standard_Integer argc,const char ** argv)
{
  if (argc < 2) {
    di<<"Usage: " << argv[0] << " invalid number of arguments"<<"\n";
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
    di << "Error: " << argv[0] << " - invalid number of arguments" << "\n";
    di << "Usage: type help " << argv[0] << "\n";
    return 1; // TCL_ERROR
  }

  gp_Ax1 RotoAx( gp::Origin(), gp::DZ() );
  gp_Ax22d Ax2( gp::Origin2d(), gp::DY2d(), gp::DX2d() );
  Handle(Geom_Surface) Plane = new Geom_Plane( gp::YOZ() );

  di << "<<<< Preparing sample surface of revolution based on trimmed curve >>>>" << "\n";
  di << "-----------------------------------------------------------------------" << "\n";

  Handle(Geom2d_Circle) C2d1 = new Geom2d_Circle(Ax2, 1.0);
  Handle(Geom2d_TrimmedCurve) C2d1Trimmed = new Geom2d_TrimmedCurve(C2d1, 0.0, M_PI/2.0);
  TopoDS_Edge E1 = BRepBuilderAPI_MakeEdge(C2d1Trimmed, Plane);

  DBRep::Set("e1", E1);

  BRepPrimAPI_MakeRevol aRevolBuilder1(E1, RotoAx);
  TopoDS_Face F1 = TopoDS::Face( aRevolBuilder1.Shape() );

  DBRep::Set("f1", F1);

  di << "Result: f1" << "\n";

  di << "<<<< Preparing sample surface of revolution based on offset curve  >>>>" << "\n";
  di << "-----------------------------------------------------------------------" << "\n";

  Handle(Geom2d_OffsetCurve) C2d2Offset = new Geom2d_OffsetCurve(C2d1Trimmed, -0.5);
  TopoDS_Edge E2 = BRepBuilderAPI_MakeEdge(C2d2Offset, Plane);

  DBRep::Set("e2", E2);

  BRepPrimAPI_MakeRevol aRevolBuilder2(E2, RotoAx);
  TopoDS_Face F2 = TopoDS::Face( aRevolBuilder2.Shape() );

  DBRep::Set("f2", F2);

  di << "Result: f2" << "\n";

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
    di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
    return 1;
  }
  Handle(Geom_Curve) aCurve = DrawTrSurf::GetCurve(argv[1]);
  Handle(Geom_Surface) aSurf = DrawTrSurf::GetSurface(argv[2]);
  if (aCurve.IsNull()) {
    di << "Curve was not read" << "\n";
	return 1;
  }
  if (aSurf.IsNull()) {
	di << "Surface was not read" << "\n";
	return 1;
  }
  ShapeConstruct_ProjectCurveOnSurface aProj;
  aProj.Init (aSurf, Precision::Confusion());
  try {
    Handle(Geom2d_Curve) aPCurve;
    aProj.Perform (aCurve, aCurve->FirstParameter(), aCurve->LastParameter(), aPCurve);
    if (aPCurve.IsNull()) {
	  di << "PCurve was not created" << "\n";
	  return 1;
    }
  } catch (...) {
    di << "Exception was caught" << "\n";
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

#include <Voxel_BoolDS.hxx>
#include <Voxel_FastConverter.hxx>
#include <Voxel_BooleanOperation.hxx>
static Standard_Integer OCC24019 (Draw_Interpretor& di, Standard_Integer argc, const char** argv) 
{
  if ( argc != 2 ) {
    di << "Error: " << argv[0] << " - invalid number of arguments" << "\n";
	return 1;
  }

  TCollection_AsciiString aFileName = argv[1];
  TopoDS_Shape aShape;
  BRep_Builder aBuilder;

  if (!BRepTools::Read(aShape, aFileName.ToCString(), aBuilder)) {
    di << "Error: Could not read a shape!" << "\n";
	return 1;
  }
  
  TopoDS_Solid aShape1 = BRepPrimAPI_MakeSphere(gp_Pnt(20,25,35), 7);

  Standard_Real deflection = 0.005;
  Standard_Integer nbThreads = 1;
  Standard_Integer nbx = 200, nby = 200, nbz = 200;
  Voxel_BoolDS theVoxels(0,0,0, 50, 50, 50, nbx, nby, nbz);
  Voxel_BoolDS theVoxels1(0,0,0, 50, 50, 50, nbx, nby, nbz);

  Standard_Integer progress = 0;
  Voxel_FastConverter fcp(aShape, theVoxels, deflection, nbx, nby, nbz, nbThreads);
  fcp.ConvertUsingSAT(progress, 1);
  fcp.FillInVolume(1);

  Voxel_FastConverter fcp1(aShape1, theVoxels1, deflection, nbx, nby, nbz, nbThreads);
  fcp1.ConvertUsingSAT(progress, 1);
  fcp1.FillInVolume(1);

  Voxel_BooleanOperation op;
  Standard_Boolean result = op.Cut(theVoxels1, theVoxels);
  if ( result != 1 ) {
    di << "Error: invalid boolean operation" << "\n";
  } else {
	di << "OK: boolean operation is ok" << "\n";
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
    theDI<<"No intersections found!"<<"\n";

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
		myAISContext->SetColor(myShape, Quantity_Color(Quantity_NOC_YELLOW));
		myAISContext->Display(myShape, Standard_True);
    }

	return 0;
}

#include <Voxel_FastConverter.hxx>
static Standard_Integer OCC24051 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
	if (argc != 1) {
		di << "Usage : " << argv[0] << " should be one argument (command name only)";
		return 1;
	}

	TopoDS_Shape shape = BRepPrimAPI_MakeBox(gp_Pnt(5, 10, 10), 10, 20, 30).Shape();
	Standard_Integer progress = 0;
	Standard_Real deflection = 0.005;
	Standard_Integer nbx = 200, nby = 200, nbz = 200;
	Voxel_BoolDS theVoxels(-50,-50,-30, 100, 100, 100, nbx, nby, nbz);
	Voxel_BoolDS theVoxels1(-50,-50,-30, 100, 100, 100, nbx, nby, nbz);
	Standard_Integer nbThreads = 5;
	Voxel_FastConverter fcp(shape, theVoxels, deflection, nbx, nby, nbz, nbThreads, Standard_True);
	
	#ifdef WNT
	#pragma omp parallel for
        for(int i = 0; i < nbThreads; i++)
			fcp.ConvertUsingSAT(progress, i+1);
	#endif
	
	fcp.ConvertUsingSAT(progress);

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
		di << "Result was checked and it is INVALID" << "\n";
	} else {
		di << "Result was checked and it is VALID" << "\n";
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
    di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
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

#include <Extrema_FuncExtPS.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_Vector.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
static Standard_Integer OCC24137 (Draw_Interpretor& theDI, Standard_Integer theNArg, const char** theArgv) 
{
  Standard_Integer anArgIter = 1;
  if (theNArg < 5)
    {
      theDI <<"Usage: " << theArgv[0] << " face vertex U V [N]"<<"\n";
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

  Extrema_FuncExtPS    anExtFunc;
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
    di<<"Usage: " << argv[0] << " invalid number of arguments"<<"\n";
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
  const T* p = aHandle.Access();
#if OCC_VERSION_HEX > 0x060700
  //QVERIFY (!p);
  //QVERIFY (p == 0);
  QCOMPARE (!p, Standard_True);
  QCOMPARE (p == 0, Standard_True);
#endif

  aHandle = new T;
  //QVERIFY (!aHandle.IsNull());
  QCOMPARE (!aHandle.IsNull(), Standard_True);
  p = aHandle.Access();
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
  DoIsNull<Standard_Persistent, Handle(Standard_Persistent)>(di);

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
  DEFINE_STANDARD_RTTI(QABugs_HandleClass) // Type definition
};
DEFINE_STANDARD_HANDLE    (QABugs_HandleClass, Standard_Transient)
IMPLEMENT_STANDARD_HANDLE (QABugs_HandleClass, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(QABugs_HandleClass, Standard_Transient)

// Dummy class to test interface for compilation issues
struct QABugs_NHandleClass
{
  Standard_Integer NHandleProc (Draw_Interpretor& , Standard_Integer  , const char** theArgVec)
  {
    std::cerr << "QABugs_NHandleClass[" << this << "] " << "" << theArgVec[0] << "\n";
    return 0;
  }
};

#include <XCAFDoc_ColorTool.hxx>
#include <STEPControl_StepModelType.hxx>
#include <STEPCAFControl_Writer.hxx>
static Standard_Integer OCC23951 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 1) {
    di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
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

  writer.Write("test_box.step");
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

  Quantity_Color yellow(1,1,0, Quantity_TOC_RGB);
  XCAFDoc_DocumentTool::ColorTool (labelA0)->SetColor (component01, yellow, XCAFDoc_ColorGen);
  XCAFDoc_DocumentTool::ColorTool (labelA0)->SetVisibility (component01, 0);

  STEPControl_StepModelType mode = STEPControl_AsIs;
  STEPCAFControl_Writer writer;
  if (! writer.Transfer (aDoc, mode))
  {
    di << "The document cannot be translated or gives no result" << "\n";
    return 1;
  }

  writer.Write (argv[1]);
  return 0;
}

//=======================================================================
//function : OCC24622
//purpose  : The command tests sourcing Image_PixMap to AIS_TexturedShape
//=======================================================================
static Standard_Integer OCC24622 (Draw_Interpretor& /*theDi*/, Standard_Integer theArgNb, const char** theArgVec)
{
  if (theArgNb != 2)
  {
    std::cout << "Usage : " << theArgVec[0] << " texture={1D|2D}";
    return 1;
  }

  const Handle(AIS_InteractiveContext)& anAISContext = ViewerTest::GetAISContext();
  if (anAISContext.IsNull())
  {
    std::cout << "Please initialize view with \"vinit\".\n";
    return 1;
  }

  Handle(Image_PixMap) anImage = new Image_PixMap();

  static const Image_ColorRGB aBitmap[8] =
  {
    {{255,   0, 0}}, {{0,  148, 255}}, {{ 0, 148, 255}}, {{255,  94, 0}},
    {{255, 121, 0}}, {{76, 255,   0}}, {{76, 255,   0}}, {{255, 202, 0}}
  };

  TCollection_AsciiString aTextureTypeArg (theArgVec[1]);
  aTextureTypeArg.UpperCase();
  if (aTextureTypeArg == "1D")
  {
    anImage->InitWrapper (Image_PixMap::ImgRGB, (Standard_Byte*)aBitmap, 8, 1);
  }
  else if (aTextureTypeArg == "2D")
  {
    anImage->InitTrash (Image_PixMap::ImgRGB, 8, 8);
    for (Standard_Integer aRow = 0; aRow < 8; ++aRow)
    {
      for (Standard_Integer aCol = 0; aCol < 8; ++aCol)
      {
        anImage->ChangeValue<Image_ColorRGB> (aRow, aCol) = aBitmap[aRow];
      }
    }
  }
  else
  {
    std::cout << "Please specify type of texture to test {1D|2D}.\n";
    return 1;
  }

  TopoDS_Shape aBlankShape = BRepPrimAPI_MakeBox (10.0, 10.0, 10.0).Shape();

  Handle(AIS_TexturedShape) aTexturedShape = new AIS_TexturedShape (aBlankShape);
  aTexturedShape->SetTexturePixMap (anImage);
  anAISContext->Display (aTexturedShape, 3, 0);

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
    di << "OCC24667 result Wire_spine Profile [Mode [Approx]]" << "\n";
    di << "Mode = 0 - CorrectedFrenet," << "\n";
    di << "     = 1 - Frenet," << "\n";
    di << "     = 2 - DiscreteTrihedron" << "\n";
    di << "Approx - force C1-approximation if result is C0" << "\n";
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

#include <IGESControl_Reader.hxx>
#include <IGESControl_Controller.hxx>
#include <IGESData_IGESEntity.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <PTColStd_TransientPersistentMap.hxx>
#include <PTopoDS_HShape.hxx>
#include <Storage_Data.hxx>
#include <TopExp_Explorer.hxx>
#include <MgtBRep.hxx>
#include <FSD_File.hxx>
#include <ShapeSchema.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <PTColStd_PersistentTransientMap.hxx>
#include <Storage_Root.hxx>

static Standard_Integer OCC24565 (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 3) {
    di << "Usage : " << argv[0] << " FileNameIGS FileNameSTOR";
    return 1;
  }

  Standard_CString sFileNameIGS = argv[1];
  Standard_CString sFileNameSTOR = argv[2];

  IGESControl_Reader ICReader;

  /* * * * * * *
   * Read the IGES file and make sure it is valid
   *
   * * * * * * */
  IGESControl_Controller::Init();

  if (!ICReader.ReadFile(sFileNameIGS)) {
    printf("%s:%d - Error reading '%s'\n",__FUNCTION__,__LINE__,sFileNameIGS);fflush(stdout);
    return -1;
  }

  int nbShapes = ICReader.NbShapes();

  printf("%s:%d - nbShapes = '%d'\n",__FUNCTION__,__LINE__,nbShapes);fflush(stdout);

  TopoDS_Shape Shape;
  if(nbShapes == 0)
    {
      Handle(TColStd_HSequenceOfTransient) faces=ICReader.GiveList("iges-faces");
      Handle(TColStd_HSequenceOfTransient) surfaceList=ICReader.GiveList("xst-transferrable-roots",faces);

      if (surfaceList.IsNull())
	{
	  printf("%s:%d - surfaceList.IsNull()\n",__FUNCTION__,__LINE__);fflush(stdout);
	  return -1;
	}
      BRep_Builder builder;
      TopoDS_Compound* pC = new TopoDS_Compound();
      builder.MakeCompound(*pC);

      for (int j=1;j<=surfaceList->Length();j++)
	{
	  Handle(IGESData_IGESEntity) igesEntity=Handle(IGESData_IGESEntity)::DownCast(surfaceList->Value(j));
	  if (igesEntity.IsNull()) continue;
	  ICReader.ClearShapes();
	  Standard_Boolean rv;
	  try {
	    rv=ICReader.TransferEntity(igesEntity);
	  }
	  catch (...) {
	    rv=Standard_False;
	  }
	  if (!rv) {
	    printf("%s:%d - Error transferring IGES entity\n",__FUNCTION__,__LINE__);fflush(stdout);
	    printf("%s:%d - FormNumber = %d, TypeNumber = %d\n",__FUNCTION__,__LINE__,igesEntity->FormNumber(),igesEntity->TypeNumber());fflush(stdout);
	    return -1;
	  }

	  TopoDS_Shape S;
	  try {
	    S=ICReader.Shape();
	  }
	  catch(...) {
	    printf("%s:%d - Error reading IGES entity\n",__FUNCTION__,__LINE__);fflush(stdout);
	    printf("%s:%d - FormNumber = %d, TypeNumber = %d\n",__FUNCTION__,__LINE__,igesEntity->FormNumber(),igesEntity->TypeNumber());fflush(stdout);
	    return -1;
	  }
	  if (S.IsNull()) {
	    printf("%s:%d - NULL Surface encountered\n",__FUNCTION__,__LINE__);
	    return -1;
	  }

	  try
	    {
	      builder.Add(*pC,S);
	    }
	  catch(...)
	    {
	      printf("%s: Exception adding face.\n",__FUNCTION__);
	    }
	}
      Shape = TopoDS_Shape(*pC);
    }
  else
    {
      Shape = ICReader.OneShape();
    }
  {
    BRepCheck_Analyzer brca(Shape);

    if(!brca.IsValid())
      {
	printf("%s: Invalid shape after reading IGES file.\n",__FUNCTION__);
      }
  }

  /* * * * * * *
   * Write the contents of the Shape to a STOR file
   *
   * * * * * * */
  PTColStd_TransientPersistentMap aMapTP;
  Handle(PTopoDS_HShape) aPShape_write;
  Handle(Storage_Data) d_write=new Storage_Data;
  char Name[32];

  TopExp_Explorer Ex;
  int i;
  int max_i = 0;

  for (i=0,Ex.Init(Shape,TopAbs_FACE);Ex.More();i++,Ex.Next())
    {

      max_i = i;
      try {
	aPShape_write=MgtBRep::Translate(Ex.Current(),aMapTP,MgtBRep_WithoutTriangle);
      }
      catch (...) {
	printf("%s: Error translating surface '%d'\n",__FUNCTION__,i);
      }
		
      sprintf(Name,"S%010d",i);
		
      {
	BRepCheck_Analyzer brca(Ex.Current());
	if(!brca.IsValid())
	  {
	    printf("INVALID face '%s' in the shape, which will be written to the STOR file.\n",Name);
	  }
      }
      try {
	d_write->AddRoot(Name,aPShape_write);
      }
      catch (...) {
	printf("%s: Error adding surface '%d', RootName = '%s'\n",__FUNCTION__,i,Name);
      }
    }
  printf("%s: Going to write %d surfaces.\n",__FUNCTION__,max_i+1);

  FSD_File f_write;
  if(f_write.Open(sFileNameSTOR, Storage_VSWrite)!=Storage_VSOk)
    {
      printf("%s: Error opening file: %s\n", __FUNCTION__,sFileNameSTOR);
      return -1;
    }
  Handle(ShapeSchema) s_write=new ShapeSchema;
  s_write->Write(f_write,d_write);
  f_write.Close();
  printf("%s: Wrote to the STOR file.\n",__FUNCTION__);

  /* * * * * * *
   * Read the contents of the Shape from a STOR file
   *
   * * * * * * */
  FSD_File f_read;
  if(f_read.Open(sFileNameSTOR, Storage_VSRead)!=Storage_VSOk)
    {
      printf("%s: Error opening file: %s\n", __FUNCTION__,sFileNameSTOR);
      return -1;
    }
  Handle(ShapeSchema) s_read=new ShapeSchema;
  Handle(Storage_Data) d_read=s_read->Read(f_read);

  Handle(Standard_Persistent) p;
  Handle(Storage_Root) r;
  Handle(PTopoDS_HShape) aPShape_read;
  PTColStd_PersistentTransientMap aMapPT;
  TopoDS_Shape S_read;

  printf("%s: Extracting %d faces from the STOR file.\n",__FUNCTION__,max_i+1);
  for(int i = 0; i <= max_i; ++i)
    {
      sprintf(Name,"S%010d",i);
      r=d_read->Find(Name);
      if(r.IsNull())
	{
	  printf("%s:%d '%s' IsNull().\n",__FUNCTION__,__LINE__,Name);fflush(stdout);
	  continue;
	}
      p=r->Object();
      aPShape_read = Handle(PTopoDS_HShape)::DownCast(p);
      try {
	MgtBRep::Translate(aPShape_read,aMapPT,S_read,MgtBRep_WithoutTriangle);
      }
      catch (Standard_Failure) {
	Handle(Standard_Failure) E=Standard_Failure::Caught();
	std::string str;
	str="Exception: ";
	str+=E->DynamicType()->Name();
	str+=" => ";
	str+=E->GetMessageString();
	printf("%s(1): %s: %s\n",__FUNCTION__,Name,str.c_str());fflush(stdout);
      }
      catch (...) {
	printf("%s(1): Unhandled exception in MgtBRep::Translate\n",__FUNCTION__);
      }

      BRepCheck_Analyzer brca(S_read);

      if(!brca.IsValid())
	{
	  printf("%s: Read INVALID face (%s)!\n",__FUNCTION__,Name);
	}
    }

  printf("Completed.\n");fflush(stdout);

  return 0;
}

#include <Handle_BRepTools_NurbsConvertModification.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
static TopoDS_Shape CreateTestShape (int& theShapeNb)
{
  TopoDS_Compound aComp;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound (aComp);
  //NURBS modifier is used to increase footprint of each shape
  Handle_BRepTools_NurbsConvertModification aNurbsModif = new BRepTools_NurbsConvertModification;
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

#include <AppStd_Application.hxx>
#include <TDataStd_Integer.hxx>
#include <TNaming_Builder.hxx>
static Standard_Integer OCC24931 (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc != 1) {
    di << "Usage: " << argv[0] << " invalid number of arguments"<<"\n";
    return 1;
  }
  TCollection_ExtendedString aFileName ("testdocument.xml");
  PCDM_StoreStatus aSStatus  = PCDM_SS_Failure;

  Handle(TDocStd_Application) anApp = new AppStd_Application;
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

#include <AppStdL_Application.hxx>
#include <TDocStd_Application.hxx>
#include <TDataStd_Integer.hxx>
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

  Handle(TDocStd_Application) anApp = new AppStdL_Application;
  Handle(TDocStd_Document) aDoc;
  anApp->NewDocument ("MDTV-Standard", aDoc);
  TDF_Label aLab = aDoc->Main();
  TDataStd_Integer::Set (aLab, 0);
  TDataStd_Name::Set (aLab, "test");

  TDF_AttributeIterator i (aLab);
  Handle(TDF_Attribute) anAttr = i.Value();
  QCOMPARE (anAttr->IsKind (STANDARD_TYPE (TDataStd_Integer)), Standard_True);
  i.Next();
  anAttr = i.Value();
  QCOMPARE (anAttr->IsKind (STANDARD_TYPE (TDataStd_Name)), Standard_True);

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
  math_MultipleVarFunction* aFunc = new BraninFunction();

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

      aFunc->Value(aCurrPnt1, aFuncValues(idx));
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

  math_GlobOptMin aFinder(aFunc, aLower, aUpper, aLipConst);
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

  static void initGlobalPluginMap (const TCollection_AsciiString& thePlugin,
                                   const TCollection_AsciiString& theSaver,
                                   const TCollection_AsciiString& theLoader)
  {
    const Handle(Resource_Manager)& aManager = Plugin::AdditionalPluginMap();
    aManager->SetResource ((theSaver  + ".Location").ToCString(), thePlugin.ToCString());
    aManager->SetResource ((theLoader + ".Location").ToCString(), thePlugin.ToCString());
  }

  Test_TDocStd_Application (const TCollection_AsciiString& thePlugin,
                            const TCollection_AsciiString& theSaver,
                            const TCollection_AsciiString& theLoader)
  {
    initGlobalPluginMap (thePlugin, theSaver, theLoader);

    // explicitly initialize resource manager
    myResources = new Resource_Manager ("");
    myResources->SetResource ("xml.FileFormat", THE_QATEST_DOC_FORMAT);
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".Description",     "Test XML Document");
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".FileExtension",   "xml");
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".StoragePlugin",   theSaver.ToCString());
    myResources->SetResource (THE_QATEST_DOC_FORMAT ".RetrievalPlugin", theLoader.ToCString());
  }

  virtual Standard_CString ResourcesName() { return ""; }
  virtual void Formats (TColStd_SequenceOfExtendedString& theFormats) { theFormats.Clear(); }
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

  Handle(TDocStd_Application) anApp = new Test_TDocStd_Application (aPlugin, aSaver, aLoader);
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
          theDI << "Info. Faulty shape if found in source shape\n";
        }
      }
    }
  }
  else 
  {
    theDI << "Error. Problems are not detected. Test is not performed.";
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
//function : OCC23010
//purpose  :
//=======================================================================
#include <STEPCAFControl_Reader.hxx>

class mOcafApplication : public TDocStd_Application
{
  void Formats(TColStd_SequenceOfExtendedString& Formats)
  {
    Formats.Append(TCollection_ExtendedString("mOcafApplication"));
  }
  Standard_CString ResourcesName()
  {
    return Standard_CString("Resources");
  }
};

static Standard_Integer OCC23010 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if (argc != 2) {
    di << "Usage: " << argv[0] << " invalid number of arguments" << "\n";
    return 1;
  }
  std::string fileName=argv[1];
  mOcafApplication *mCasApp = new mOcafApplication();
  Handle(TDocStd_Document) doc;
  mCasApp->NewDocument("MDTV-XCAF", doc);
  STEPCAFControl_Reader stepReader;
  IFSelect_ReturnStatus status = stepReader.ReadFile (fileName.c_str());
  if (status != IFSelect_RetDone)
    return false;
  stepReader.SetColorMode(Standard_True);
  stepReader.SetLayerMode(Standard_True);
  stepReader.SetNameMode(Standard_True);
  stepReader.Transfer(doc); // ERROR HERE!!!
  delete mCasApp;
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
    di<<"Usage: "<<a[0]<<" invalid number of arguments"<<"\n";
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
    di << "Shape is empty" << "\n";
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
    di << "Usage: " << a[0] << " invalid number of arguments" << "\n";
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
  iErr = aPF.ErrorStatus();
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
  iErr = pBuilder->ErrorStatus();
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

void QABugs::Commands_19(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  Handle(QABugs_HandleClass) aClassPtr = new QABugs_HandleClass();
  theCommands.Add ("OCC24202_1", "Test Handle-based procedure",
                   __FILE__, aClassPtr, &QABugs_HandleClass::HandleProc, group);
  NCollection_Handle<QABugs_NHandleClass> aNClassPtr = new QABugs_NHandleClass();
  theCommands.Add ("OCC24202_2", "Test NCollection_Handle-based procedure",
                   __FILE__, aNClassPtr, &QABugs_NHandleClass::NHandleProc, group);

  theCommands.Add ("OCC230", "OCC230 TrimmedCurve Pnt2d Pnt2d", __FILE__, OCC230, group);
  theCommands.Add ("OCC142", "OCC142", __FILE__, OCC142, group);
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
  theCommands.Add ("OCC24019", "OCC24019 aShape", __FILE__, OCC24019, group);
  theCommands.Add ("OCC11758", "OCC11758", __FILE__, OCC11758, group);
  theCommands.Add ("OCC24005", "OCC24005 result", __FILE__, OCC24005, group);
  theCommands.Add ("OCC24137", "OCC24137 face vertex U V [N]", __FILE__, OCC24137, group);
  theCommands.Add ("OCC24271", "Boolean operations on NCollection_Map", __FILE__, OCC24271, group);
  theCommands.Add ("OCC23972", "OCC23972", __FILE__, OCC23972, group);
  theCommands.Add ("OCC24370", "OCC24370 edge pcurve surface prec", __FILE__, OCC24370, group);
  theCommands.Add ("OCC24533", "OCC24533", __FILE__, OCC24533, group);
  theCommands.Add ("OCC24012", "OCC24012 face edge", __FILE__, OCC24012, group);
  theCommands.Add ("OCC24051", "OCC24051", __FILE__, OCC24051, group);
  theCommands.Add ("OCC24086", "OCC24086 face wire", __FILE__, OCC24086, group);
  theCommands.Add ("OCC24622", "OCC24622 texture={1D|2D}\n Tests sourcing of 1D/2D pixmaps for AIS_TexturedShape", __FILE__, OCC24622, group);
  theCommands.Add ("OCC24667", "OCC24667 result Wire_spine Profile [Mode [Approx]], no args to get help", __FILE__, OCC24667, group);
  theCommands.Add ("OCC24565", "OCC24565 FileNameIGS FileNameSTOR", __FILE__, OCC24565, group);
  theCommands.Add ("OCC24755", "OCC24755", __FILE__, OCC24755, group);
  theCommands.Add ("OCC24834", "OCC24834", __FILE__, OCC24834, group);
  theCommands.Add ("OCC24889", "OCC24889", __FILE__, OCC24889, group);
  theCommands.Add ("OCC23951", "OCC23951", __FILE__, OCC23951, group);
  theCommands.Add ("OCC24931", "OCC24931", __FILE__, OCC24931, group);
  theCommands.Add ("OCC24945", "OCC24945", __FILE__, OCC24945, group);
  theCommands.Add ("OCC23950", "OCC23950 step_file", __FILE__, OCC23950, group);
  theCommands.Add ("OCC25004", "OCC25004", __FILE__, OCC25004, group);
  theCommands.Add ("OCC24925",
                   "OCC24925 filename [pluginLib=TKXml storageGuid retrievalGuid]"
                   "\nOCAF persistence without setting environment variables",
                   __FILE__, OCC24925, group);
  theCommands.Add ("OCC23010", "OCC23010 STEP_file", __FILE__, OCC23010, group);
  theCommands.Add ("OCC25043", "OCC25043 shape", __FILE__, OCC25043, group);
  theCommands.Add ("OCC24606", "OCC24606 : Tests ::FitAll for V3d view ('vfit' is for NIS view)", __FILE__, OCC24606, group);
  theCommands.Add ("OCC25202", "OCC25202 res shape numF1 face1 numF2 face2", __FILE__, OCC25202, group);
  theCommands.Add ("OCC7570", "OCC7570 shape", __FILE__, OCC7570, group);
  theCommands.Add ("OCC25100", "OCC25100 shape", __FILE__, OCC25100, group);
  theCommands.Add ("OCC25340", "OCC25340", __FILE__, OCC25340, group);
  theCommands.Add ("OCC25348", "OCC25348", __FILE__, OCC25348, group);
  theCommands.Add ("OCC25413", "OCC25413 shape", __FILE__, OCC25413, group);
  theCommands.Add ("OCC25446", "OCC25446 res b1 b2 op", __FILE__, OCC25446, group);
  return;
}
