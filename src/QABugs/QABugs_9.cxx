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

#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GC_MakeTrimmedCone.hxx>

static Standard_Integer BUC60857 (Draw_Interpretor& di, Standard_Integer /*argc*/,const char ** argv)
{
  gp_Ax2  Cone_Ax;
  double R1=8, R2=16;
  gp_Pnt P0(0,0,0), P1(0,0,20), P2(0,0,45);

  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << "Use vinit command before " << argv[0] << "\n";
    return 1;
  }

  Handle(Geom_RectangularTrimmedSurface) S = GC_MakeTrimmedCone (P1, P2, R1, R2).Value();
  TopoDS_Shape myshape = BRepBuilderAPI_MakeFace(S, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_BLUE",myshape);
  Handle(AIS_Shape) ais1 = new AIS_Shape(myshape);
  aContext->Display (ais1, Standard_False);
  aContext->SetColor (ais1, Quantity_NOC_BLUE1, Standard_False);

  Handle(Geom_RectangularTrimmedSurface) S2 = GC_MakeTrimmedCone (P1, P2, R1, 0).Value();
  TopoDS_Shape myshape2 = BRepBuilderAPI_MakeFace(S2, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_RED",myshape2);
  Handle(AIS_Shape) ais2 = new AIS_Shape(myshape2);
  aContext->Display (ais2, Standard_False);
  aContext->SetColor (ais2, Quantity_NOC_RED, Standard_False);

  Handle(Geom_RectangularTrimmedSurface) S3 = GC_MakeTrimmedCone (P1, P2, R2, R1).Value();
  TopoDS_Shape myshape3 = BRepBuilderAPI_MakeFace(S3, Precision::Confusion()).Shape();
  DBRep::Set("BUC60857_GREEN",myshape3);
  Handle(AIS_Shape) ais3 = new AIS_Shape(myshape3);
  aContext->Display (ais3, Standard_False);
  aContext->SetColor (ais3, Quantity_NOC_GREEN, Standard_True);

  return 0;
}

#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <SelectMgr_Selection.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#if ! defined(_WIN32)
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#else
Standard_EXPORT ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();
#endif

static Standard_Integer OCC137 (Draw_Interpretor& di, Standard_Integer argc, const char ** argv) 
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if(aContext.IsNull()) {
    di << argv[0] << "ERROR : use 'vinit' command before \n";
    return 1;
  }
  if ( argc < 2 || argc > 3) {
    di << "ERROR : Usage : " << argv[0] << " highlight_mode [shape]\n";
    return 1;
  }
  
  ViewerTest_DoubleMapOfInteractiveAndName aMap ;
  if(argc != 3) {
    aMap.Assign(GetMapOfAIS());
  } else {
    ViewerTest_DoubleMapOfInteractiveAndName& aMap1 = GetMapOfAIS();
    TCollection_AsciiString aName(argv[2]);
    Handle(AIS_InteractiveObject) AISObj;
    if(!aMap1.IsBound2(aName)) {
      di << "Use 'vdisplay' before\n";
      return 1;
    } else {
      AISObj = Handle(AIS_InteractiveObject)::DownCast(aMap1.Find2(aName));
      if(AISObj.IsNull()){
        di << argv[2] << " : No interactive object\n";
        return 1;
      }
      aMap.Bind(AISObj,aName);
    }
  }
  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName it(GetMapOfAIS());
  while ( it.More() ) {
    Handle(AIS_InteractiveObject) AISObj = Handle(AIS_InteractiveObject)::DownCast(it.Key1());
    AISObj->SetHilightMode(Draw::Atoi(argv[1]));
    if(AISObj->HasSelection(4)) {
      //Handle(SelectMgr_Selection)& aSelection = AISObj->Selection(4);
      const Handle(SelectMgr_Selection)& aSelection = AISObj->Selection(4);
      if(!aSelection.IsNull()) {
        for(aSelection->Init();aSelection->More();aSelection->Next()) {
          Handle(StdSelect_BRepOwner) aO = Handle(StdSelect_BRepOwner)::DownCast(aSelection->Sensitive()->BaseSensitive()->OwnerId());
          aO->SetHilightMode(Draw::Atoi(argv[1]));
        }
      }
    }
    it.Next();
  }
  
  return 0;
}

#include <GccEnt_Position.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dGcc_Circ2d2TanRad.hxx>
#include <gp_Elips2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Circle.hxx>
#include <AIS_InteractiveObject.hxx>

static Standard_Integer OCC24303(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if(n < 2)
    return 1;

  const Standard_Integer SolID = Draw::Atoi(a[1]);

  //Ellipses
  Standard_Real majorRadius = 2.0;
  Standard_Real minorRadius = 1.0;
  gp_Pnt2d p0(gp::Origin2d());
  gp_Pnt2d p1(4.0,0.0);

  gp_Elips2d ellipse1 = gp_Elips2d( gp_Ax2d(p0,gp::DX2d()),majorRadius, minorRadius,true);
  gp_Elips2d ellipse2 = gp_Elips2d( gp_Ax2d(p1,gp::DX2d()),majorRadius, minorRadius,true);

  Handle(Geom2d_Curve) curve1 = new Geom2d_Ellipse(ellipse1);
  Handle(Geom2d_Curve) curve2 = new Geom2d_Ellipse(ellipse2);
  DrawTrSurf::Set("c1", curve1);
  DrawTrSurf::Set("c2", curve2);
  //Expected tangent
  gp_Pnt2d centre(5.0,0.0);
  Standard_Real radius = 3.0;
  gp_Circ2d theorical_tangent = gp_Circ2d(gp_Ax2d(centre,gp::DX2d()),radius);

  //Calculate the tangent with Geom2dGcc_Circ2dTanRan

  const Geom2dAdaptor_Curve  AdaptedCurve1 ( curve1 );
  const Geom2dAdaptor_Curve  AdaptedCurve2 ( curve2 );

  GccEnt_Position  curveQualif1 = GccEnt_unqualified;
  GccEnt_Position  curveQualif2 = GccEnt_unqualified;

  const Geom2dGcc_QualifiedCurve qualifiedCurve1 ( AdaptedCurve1, curveQualif1 );
  const Geom2dGcc_QualifiedCurve qualifiedCurve2 ( AdaptedCurve2, curveQualif2 );

  const Geom2dGcc_Circ2d2TanRad circCalc(qualifiedCurve1,qualifiedCurve2,radius,/*Precision::Approximation()*/ 1.0e-9);

  const Standard_Integer aNbSol = circCalc.NbSolutions();
  di << "Solutions " << aNbSol << "\n";

  if((SolID < 1) || (SolID > aNbSol))
  {
    di << "Wrong SolID value\n";
    return 1;
  }

  gp_Circ2d calculated_tangent = circCalc.ThisSolution(SolID);

  char Buf[10];
  for (Standard_Integer i = 1; i <= aNbSol; i++)
  {
    gp_Circ2d ct = circCalc.ThisSolution(i);
    Handle (Geom2d_Circle) GSol = new Geom2d_Circle(ct);
    Sprintf(Buf, "Sol%d",i);
    DrawTrSurf::Set(Buf, GSol);
  }

  //This distance is different in OC 6.5.4 and OC 6.6.0
  Standard_Real dist = theorical_tangent.Location().Distance(calculated_tangent.Location());
  di << "Distance = " << dist << "\n";

  return 0;
}

void QABugs::Commands_9(Draw_Interpretor& theCommands) {
  const char *group = "QABugs";

  theCommands.Add ("BUC60857", "BUC60857", __FILE__, BUC60857, group);
  theCommands.Add("OCC137","OCC137 mode [shape]",__FILE__,OCC137,group);
  theCommands.Add("OCC24303", "OCC24303 SolID ",	__FILE__,	OCC24303,group);

  return;
}
