// Created on: 1999-03-09
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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

//gka,rln 30.04.99 S4137: new commands for testing ShapeDivide added, some removed
//abv,pdn 05.05.99 S4174: new commands for testing ShapeDivide added, some removed
//pdn,gka 10.06.99 S4189: command DT_ShapeConvertRev added

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepTools.hxx>
#include <BRepTools_ReShape.hxx>
#include <DBRep.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <DrawTrSurf.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Precision.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeCustom.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeFix.hxx>
#include <ShapeFix_ComposeShell.hxx>
#include <ShapeUpgrade.hxx>
#include <ShapeUpgrade_RemoveInternalWires.hxx>
#include <ShapeUpgrade_RemoveLocations.hxx>
#include <ShapeUpgrade_ShapeConvertToBezier.hxx>
#include <ShapeUpgrade_ShapeDivideAngle.hxx>
#include <ShapeUpgrade_ShapeDivideArea.hxx>
#include <ShapeUpgrade_ShapeDivideClosed.hxx>
#include <ShapeUpgrade_ShapeDivideContinuity.hxx>
#include <ShapeUpgrade_SplitCurve2dContinuity.hxx>
#include <ShapeUpgrade_SplitCurve3dContinuity.hxx>
#include <ShapeUpgrade_SplitSurfaceContinuity.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <SWDRAW.hxx>
#include <SWDRAW_ShapeUpgrade.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TColGeom_HArray2OfSurface.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>

#include <stdio.h> 
//#include <SWDRAW_ShapeUpgrade.hxx>
//#include <ShapeUpgrade_SupportModification.hxx>
//#include <ShapeExtend_WireData.hxx>
//#include <ShapeAnalysis_Shell.hxx>
//#include <ShapeAnalysis_WireOrder.hxx>
//#include <ShapeAnalysis_Wire.hxx>
//#include <ShapeUpgrade_ShellSewing.hxx>
// the plane (equation z=0) shared by PlaneDividedFaceContinuity and PlaneGridShell
//static Handle(Geom_Plane) ThePlane= new Geom_Plane(0,0,1,0);
//=======================================================================
//function : DT_ShapeDivide 
//purpose  : 
//=======================================================================
static Standard_Integer DT_ShapeDivide (Draw_Interpretor& di,
					Standard_Integer n, const char** a)
{
  // DT_ShapeDivide result Shape Tol
  // a[1]= result
  // a[2]= input Face/Surface
  // a[3] si n>3= Wire/Face
  // a[n-1]= Tolerance
   
  if (n<3) {
    di << "bad number of arguments\n";
    return 1;
  } 
  
  // try to read a shape:
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  // a[2] is a shape. managing:
  // DT_ShapeDivide result Face Tol
  
  // giving a face is available only in the constructor:
  // we make the whole and quit.
  ShapeUpgrade_ShapeDivideContinuity tool(inputShape);
    
  // tolerance is optional
  if (n==4) {
    Standard_Real Tol=Draw::Atof(a[3]);
    tool.SetTolerance(Tol);
  }
  
  //  theTool.SetGlobalCriterion(GeomAbs_C1);
  tool.Perform();
  TopoDS_Shape res = tool.Result();

  if ( tool.Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( tool.Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( tool.Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( tool.Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( tool.Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( tool.Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( tool.Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( tool.Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( tool.Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( tool.Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( tool.Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( tool.Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( tool.Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( tool.Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( tool.Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( tool.Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( tool.Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  // fixes
  
  ShapeFix::SameParameter ( res, Standard_False );

  DBRep::Set(a[1],res);
  return 0;
}

static Standard_Integer DT_ShapeConvertRev (Draw_Interpretor& di,
					 Standard_Integer n, const char** a)
{
  if (n<5) {
    di << "bad number of arguments\n";
    return 1;
  } 
  
  // try to read a shape:
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  
  Standard_Integer c2d = Draw::Atoi(a[3]);
  Standard_Integer c3d = Draw::Atoi(a[4]);
  TopoDS_Shape revsh = ShapeCustom::ConvertToRevolution (inputShape);
  if (revsh.IsNull()) { di<<"NO RESULT\n"; return 1; }
  else if (revsh == inputShape) { di<<"No modif\n";}
  else di<<"ConvertToRevolution -> Result : \n";
  
  ShapeUpgrade_ShapeConvertToBezier tool(revsh);
  tool.SetSurfaceConversion(Standard_True);
  if(c2d)
    tool.Set2dConversion(Standard_True);
  if(c3d) {
    tool.Set3dConversion(Standard_True);
    if(n > 5)
      tool.Set3dLineConversion(Standard_False);
    if(n > 6)
      tool.Set3dCircleConversion(Standard_False);
    if(n > 7)
      tool.Set3dConicConversion(Standard_False);
  }
  tool.Perform();
  TopoDS_Shape res = tool.Result();
  
  if ( tool.Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( tool.Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( tool.Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( tool.Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( tool.Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( tool.Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( tool.Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( tool.Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( tool.Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( tool.Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( tool.Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( tool.Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( tool.Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( tool.Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( tool.Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( tool.Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( tool.Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  // fixes
  
  ShapeFix::SameParameter ( res, Standard_False );

  DBRep::Set(a[1],res);
  return 0;
}


/*
  if (!inputShape.IsNull()) {
    // a[2] is a shape. managing:
    // DT_ShapeDivide result Face Tol

    TopoDS_Face  inputFace = TopoDS::Face(inputShape);
    if (inputFace.IsNull()) {
      di << a[2] << " is not a face\n";
      return 1;
    }

    // giving a face is available only in the constructor:
    // we make the whole and quit.
    ShapeUpgrade_ShapeDivideContinuity theTool(inputFace);
    
    // tolerance is optional
    if (n==4) {
      Standard_Real Tol=Draw::Atof(a[n-1]);
      theTool.SetTolerance(Tol);
    }

    theTool.SetGlobalCriterion(GeomAbs_C1);
    theTool.Build();
    if (!theTool.IsDone()) {
      ShapeUpgrade_Error theError=theTool.Error();
      di << "Not done: error=";
      if (theError==ShapeUpgrade_Done) 
	di << "Done\n";
      else if (theError==ShapeUpgrade_NotDone) 
	di << "NotDone\n";
      else if (theError==ShapeUpgrade_EmptyShell) 
	di << "EmptyShell\n";
      else if (theError==ShapeUpgrade_InvalidCriterion) 
	di << "InvalidCriterion\n";
      else if (theError==ShapeUpgrade_InvalidGridSurface) 
	di << "InvalidGridSurface\n";
      else if (theError==ShapeUpgrade_DegeneratedEdge) 
	di << "DegeneratedEdge\n";
      else if (theError==ShapeUpgrade_NoSurface) 
	di << "NoSurface\n";
      else if (theError==ShapeUpgrade_NoTolerance) 
	di << "NoTolerance\n";
      return 1;
    }   
    TopoDS_Shell res = theTool.Shell();
    DBRep::Set(a[1],res);
    
    return 0;
  }
  else {
    // not a face: we can use the empty consturctor.
    ShapeUpgrade_ShapeDivideContinuity theTool;
    Standard_Real Tol=Draw::Atof(a[n-1]);
    theTool.SetTolerance(Tol);
    theTool.SetGlobalCriterion(GeomAbs_C1);

    // try to read a surface:
    Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[2]);
    if (! GS.IsNull()) {
      // a[2] is a surface. managing the configurations:
      // DT_ShapeDivide result Surface Tol
      // DT_ShapeDivide result Surface Face Tol
      // DT_ShapeDivide result Surface Wire Surf Tol
      
      theTool.SetSupport(GS);

      // try to read a Wire or a Face:
      if (n>=5) {
	TopoDS_Shape inputBoundary=DBRep::Get(a[3]);
	if (inputBoundary.IsNull()) {
	  di << "Invalid Boundary\n";
	  return 1;
	}
	TopoDS_Wire WireBoundary = TopoDS::Wire(inputBoundary);
	if (!WireBoundary.IsNull()) {
	  // DT_ShapeDivide result Surface Wire Surf Tol
	  Handle(Geom_Surface) WireSupport = DrawTrSurf::GetSurface(a[4]);
	  if (WireSupport.IsNull()) {
	    di << "Invalid Surface supporting the Wire\n";
	    return 1;
	  }
	  theTool.SetBoundary(WireBoundary, WireSupport);
	}
	else {
	  TopoDS_Face  FaceBoundary = TopoDS::Face(inputBoundary);
	  // DT_ShapeDivide result Surface Face Tol
	  theTool.SetBoundary(FaceBoundary);
	}
      }
    }
    else {
      // it must be a grid: managing the configurations:
      // DT_ShapeDivide result NbU NbV {Surf_u_v...} Tol
      // DT_ShapeDivide result NbU NbV {Surf_u_v...} Face Tol
      // DT_ShapeDivide result NbU NbV {Surf_u_v...} Wire Surf Tol
      if (n<6) {
	di << "bad number of arguments for grid input\n";
	return 1;
      }
      // number of surf:
      Standard_Integer NbU=Draw::Atoi(a[2]);
      Standard_Integer NbV=Draw::Atoi(a[3]);
      if (n < 4+NbU*NbV+1) {
	di << "bad number of arguments\n";
	return 1;
      }
      
      Handle(TColGeom_HArray2OfSurface) 
	TheGridSurf= new TColGeom_HArray2OfSurface(1,NbU,1,NbV);
      
      for (Standard_Integer iu=1; iu<=NbU; iu++) {
	for (Standard_Integer jv=1; jv<=NbV; jv++) {
	  Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[4+(iu-1)*NbV+jv-1]);
	  TheGridSurf->SetValue(iu,jv,GS);
	}
      }
      theTool.SetSupport(TheGridSurf,Tol);   

      // try to read a Wire or a Face:
      if (n>=6+NbU*NbV) {
	TopoDS_Shape inputBoundary=DBRep::Get(a[4+NbU*NbV]);
	if (inputBoundary.IsNull()) {
	  di << "Invalid Boundary\n";
	  return 1;
	}
	TopoDS_Wire  WireBoundary = TopoDS::Wire(inputBoundary);
	if (!WireBoundary.IsNull()) {
	  // DT_ShapeDivide result Surface Wire Surf Tol
	  Handle(Geom_Surface) WireSupport = DrawTrSurf::GetSurface(a[4+NbU*NbV+1]);
	  if (WireSupport.IsNull()) {
	    di << "Invalid Surface supporting the Wire\n";
	    return 1;
	  }
	  theTool.SetBoundary(WireBoundary, WireSupport);
	}
	else {
	  TopoDS_Face  FaceBoundary = TopoDS::Face(inputBoundary);
	  // DT_ShapeDivide result Surface Face Tol
	  theTool.SetBoundary(FaceBoundary);
	}
      }     
    } 

    theTool.Build();
    if (!theTool.IsDone()) {
      ShapeUpgrade_Error theError=theTool.Error();
      di << "Not done: error=";
      if (theError==ShapeUpgrade_Done) 
	di << "Done\n";
      else if (theError==ShapeUpgrade_NotDone) 
	di << "NotDone\n";
      else if (theError==ShapeUpgrade_EmptyShell) 
	di << "EmptyShell\n";
      else if (theError==ShapeUpgrade_InvalidCriterion) 
	di << "InvalidCriterion\n";
      else if (theError==ShapeUpgrade_InvalidGridSurface) 
	di << "InvalidGridSurface\n";
      else if (theError==ShapeUpgrade_DegeneratedEdge) 
	di << "DegeneratedEdge\n";
      else if (theError==ShapeUpgrade_NoSurface) 
	di << "NoSurface\n";
      else if (theError==ShapeUpgrade_NoTolerance) 
	di << "NoTolerance\n";
      return 1;
    }   
    
    TopoDS_Shell res = theTool.Shell();
    DBRep::Set(a[1],res);
    
    return 0;
  }
}
*/
static Standard_Integer DT_ShapeConvert (Draw_Interpretor& di,
					 Standard_Integer n, const char** a)
{
  if (n<5) {
    di << "bad number of arguments\n";
    return 1;
  } 
  
  // try to read a shape:
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  
  Standard_Integer c2d = Draw::Atoi(a[3]);
  Standard_Integer c3d = Draw::Atoi(a[4]);
  
  ShapeUpgrade_ShapeConvertToBezier tool(inputShape);
  tool.SetSurfaceConversion(Standard_True);
  if(c2d)
    tool.Set2dConversion(Standard_True);
  if(c3d)
    tool.Set3dConversion(Standard_True);
  tool.Perform();
  TopoDS_Shape res = tool.Result();
  
  if ( tool.Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( tool.Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( tool.Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( tool.Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( tool.Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( tool.Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( tool.Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( tool.Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( tool.Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( tool.Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( tool.Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( tool.Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( tool.Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( tool.Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( tool.Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( tool.Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( tool.Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  // fixes
  
  ShapeFix::SameParameter ( res, Standard_False );

  DBRep::Set(a[1],res);
  return 0;
}
static Standard_Integer DT_SplitAngle(Draw_Interpretor& di,
				      Standard_Integer n, const char** a)
{
  if (n<3) {
    di << "bad number of arguments\n";
    return 1;
  }
  
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  
  Standard_Real maxangle = 95;
  if ( n >3 ) {
    maxangle = Draw::Atof ( a[3] );
    if ( maxangle <1 ) maxangle = 1;
  }
  
  ShapeUpgrade_ShapeDivideAngle tool(maxangle * M_PI/180,inputShape);
  tool.Perform();
  TopoDS_Shape res = tool.Result();

  if ( tool.Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( tool.Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( tool.Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( tool.Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( tool.Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( tool.Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( tool.Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( tool.Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( tool.Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( tool.Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( tool.Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( tool.Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( tool.Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( tool.Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( tool.Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( tool.Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( tool.Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  // fixes
  
  ShapeFix::SameParameter ( res, Standard_False );

  DBRep::Set(a[1],res);
  return 0;
}
  
/*  
//=======================================================================
//function : DT_PlaneDividedFace 
//purpose  : Transfer into a plane with boundary divided
//           
//
//=======================================================================
static Standard_Integer DT_PlaneDividedFace (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{
  // a[1]= result
  // a[2]= input Face
  // a[3]= Tolerance

  if (n !=4) {
    di << "bad number of arguments\n";
    return 1;
  }

  Standard_Real      Tol=Draw::Atof(a[3]);
  TopoDS_Shape inputShape=DBRep::Get(a[2]);
  TopoDS_Face  inputFace = TopoDS::Face(inputShape);
  if (inputFace.IsNull()) {
    di << a[2] << " is not a face\n";
    return 1;
  }

  ShapeUpgrade_PlaneDividedFace theTool(ThePlane);
  theTool.Init(inputFace);
  //theTool.SetBoundaryCriterion(GeomAbs_C1);
  //theTool.SetTolerance(Tol);
  theTool.Build();
  if (!theTool.IsDone()) {
    di << "Not done\n";
    return 1;
  }    

  TopoDS_Face res = theTool.Face();
  DBRep::Set(a[1],res);

  Standard_Real the2d3dFactor=theTool.Get2d3dFactor();
  di << "2d3dFactor="<<the2d3dFactor<< "\n";
  return 0;
}

//=======================================================================
//function : DT_PlaneGridShell 
//purpose  : Create a Plane Grid Shell from U and V knots
//           
//
//=======================================================================
static Standard_Integer DT_PlaneGridShell (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{

  if (n < 4) return 1;
  // a[1]= result
  // a[2]= NbU >=2
  // a[3]= NbV >=2
  // a[4..]= {UKnots}
  // a[4+NbU...] = {VKnots}
  // a[4+NbU+NbV+1] = Tol

  // number of knots:
  Standard_Integer NbU=Draw::Atoi(a[2]);
  Standard_Integer NbV=Draw::Atoi(a[3]);
  if (n != 4+NbU+NbV+1) {
    di << "bad number of arguments\n";
    return 1;
  }

  TColStd_Array1OfReal TheUKnots(1,NbU);
  TColStd_Array1OfReal TheVKnots(1,NbV);

  for (Standard_Integer ii=1; ii<=NbU; ii++) {
    TheUKnots(ii)=Draw::Atof(a[4+ii-1]);
  }
  for (ii=1; ii<=NbV; ii++) {
    TheVKnots(ii)=Draw::Atof(a[4+NbU+ii-1]);
  }

  Standard_Real Tol=Draw::Atof(a[4+NbU+NbV]);

  ShapeUpgrade_PlaneGridShell TheGrid(ThePlane,TheUKnots,TheVKnots,Tol);

  TopoDS_Shell res = TheGrid.Shell();
  DBRep::Set(a[1],res);

  return 0;
}

//=======================================================================
//function : DT_PlaneFaceCommon 
//purpose  : Common between a plane Face and a Shell whose all Faces are 
//           laying in the same plane
//           
//
//=======================================================================
static Standard_Integer DT_PlaneFaceCommon (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{
  // a[1]= result
  // a[2]= input Face
  // a[3]= input Shell

  if (n !=4) {
    di << "bad number of arguments\n";
    return 1;
  }

  TopoDS_Shape inputShape= DBRep::Get(a[2]);
  TopoDS_Face  inputFace = TopoDS::Face(inputShape);
  if (inputFace.IsNull()) {
    di << a[2] << " is not a face\n";
    return 1;
  }

  inputShape = DBRep::Get(a[3]);
  TopoDS_Shell inputShell = TopoDS::Shell(inputShape);
  if (inputShell.IsNull()) {
    di << a[3] << " is not a shell\n";
    return 1;
  }

  ShapeUpgrade_PlaneFaceCommon theTool;
  theTool.Init(inputFace,inputShell);

  TopoDS_Shell res = theTool.Shell();
  DBRep::Set(a[1],res);

  return 0;
}*/

//=======================================================================
//function : DT_SplitCurve 
//purpose  :  Splits the curve with C1 criterion
//           
//
//=======================================================================
static Standard_Integer DT_SplitCurve (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{
  // a[1]= input curve. This name is used with a suffix to name the output curves
  // a[2]= Tolerance

  if (n < 3) {
    di << "bad number of arguments\n";
    return 1;
  }

  Standard_Real      Tol=Draw::Atof(a[2]);
  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(a[1]);
  if ( GC.IsNull()) return 1;
  Standard_Integer Split = Draw::Atoi(a[3]);
  Handle(ShapeUpgrade_SplitCurve3dContinuity) theTool = new ShapeUpgrade_SplitCurve3dContinuity;
  theTool->Init(GC);
  theTool->SetTolerance (Tol);
  theTool->SetCriterion (GeomAbs_C1);
  if(Split == 1) {
    Handle(TColStd_HSequenceOfReal) spval = new TColStd_HSequenceOfReal;
    for(Standard_Integer i = 1; i<=5; i++) spval->Append(i);
    theTool->SetSplitValues(spval);
  }
  theTool->Perform (Standard_True);
  Handle(TColGeom_HArray1OfCurve) theCurves= theTool->GetCurves();
  Standard_Integer NbC=theCurves->Length();
  for (Standard_Integer icurv=1; icurv<=NbC; icurv++) {
    char name[100];
    Sprintf(name,"%s%s%d",a[1],"_",icurv);
    char* newname = name;
    DrawTrSurf::Set(newname, theCurves->Value(icurv));
    di.AppendElement(newname);
  }
  return 0;
}


//=======================================================================
//function : DT_SplitCurve2d 
//purpose  :  Splits the curve with C1 criterion
//           
//
//=======================================================================
static Standard_Integer DT_SplitCurve2d (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{
  // a[1]= input 2d curve. This name is used with a suffix to name the output curves
  // a[2]= Tolerance

  if (n < 3) {
    di << "bad number of arguments\n";
    return 1;
  }

  Standard_Real      Tol=Draw::Atof(a[2]);
  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(a[1]);
  if ( GC.IsNull()) return 1;
  Standard_Integer Split = Draw::Atoi(a[3]);
  Handle(ShapeUpgrade_SplitCurve2dContinuity) theTool = new ShapeUpgrade_SplitCurve2dContinuity;
  theTool->Init(GC);
  theTool->SetTolerance (Tol);
  theTool->SetCriterion (GeomAbs_C1);
  if(Split == 1) {
    Handle(TColStd_HSequenceOfReal) spval = new TColStd_HSequenceOfReal;
    for(Standard_Integer i = 1; i<=5; i++) spval->Append(i);
    theTool->SetSplitValues(spval);
  }
  theTool->Perform (Standard_True);
  Handle(TColGeom2d_HArray1OfCurve) theCurves= theTool->GetCurves();
  Standard_Integer NbC=theCurves->Length();
  for (Standard_Integer icurv=1; icurv<=NbC; icurv++) {
    char name[100];
        Sprintf(name,"%s%s%d",a[1],"_",icurv);
    char* newname = name;
    DrawTrSurf::Set(newname, theCurves->Value(icurv));
    di.AppendElement(newname);
  }
  return 0;
}


//=======================================================================
//function : DT_SplitSurface 
//purpose  :  Splits the surface with C1 criterion
//           
//
//=======================================================================
/*
static Standard_Integer DT_SplitWire (Draw_Interpretor& di,
				      Standard_Integer n, const char** a)
{

  if (n <3) {
    di << "bad number of arguments\n";
    return 1;
  }

  TopoDS_Face source = TopoDS::Face(DBRep::Get(a[2]));
  if(source.IsNull()) {
    di <<"Shape is not face\n";
    return 1;
  }
  TopoDS_Iterator wi(source);
  if(!wi.More()) {
    di <<"Shape is face without wire\n";
    return 1;
  }
  
  TopoDS_Wire wire = TopoDS::Wire(wi.Value());
  Handle(ShapeUpgrade_WireDivideContinuity) tool = new ShapeUpgrade_WireDivideContinuity;
  tool->Init(wire,source);
  if(n >=4 ) {
    Standard_Real      Tol=Draw::Atof(a[3]);
  }
  Handle(ShapeBuild_ReShape) context = new ShapeBuild_ReShape;
  tool->Perform(context);
  TopoDS_Wire result = tool->Wire();
  DBRep::Set(a[1],result);
  return 0;
}
*/
/*
static Standard_Integer DT_SplitFace (Draw_Interpretor& di,
				      Standard_Integer n, const char** a)
{

  if (n <3) {
    di << "bad number of arguments\n";
    return 1;
  }

  TopoDS_Face source = TopoDS::Face(DBRep::Get(a[2]));
  if(source.IsNull()) {
    di <<"Shape is not face\n";
    return 1;
  } 
  Handle(ShapeUpgrade_ShapeDivideContinuity) tool = new ShapeUpgrade_FaceDivideContinuity;
  tool->Init(source);
  if(n >=4 ) {
    Standard_Real      Tol=Draw::Atof(a[3]);
    tool->SetPrecision(Tol);
  }
  
  Handle(ShapeBuild_ReShape) context = new ShapeBuild_ReShape;
  tool->Perform(context);
  TopoDS_Shape result = tool->Result();
  
  
  if ( tool->Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( tool->Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( tool->Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( tool->Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( tool->Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( tool->Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( tool->Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( tool->Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( tool->Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( tool->Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( tool->Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( tool->Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( tool->Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( tool->Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( tool->Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( tool->Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( tool->Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  // fixes
  
  ShapeFix::SameParameter ( result, Standard_False );
  
  DBRep::Set(a[1],result);
  return 0;
}
*/

static Standard_Integer DT_SplitSurface (Draw_Interpretor& di,
				   Standard_Integer n, const char** a)

{
  // a[1]= result (used with a suffix to name the output surfaces)
  // a[2]= input surface. 
  // a[3]= Tolerance

  // a[1]= result
  // a[2]= nbU
  // a[3]= nbV
  // a[3+1]..a[3+nbU*nbV] = Input Surfaces
  // a[4+nbU*nbV]= Tolerance

  if (n <4) {
    di << "bad number of arguments\n";
    return 1;
  }

  Handle(ShapeUpgrade_SplitSurfaceContinuity) theTool = new ShapeUpgrade_SplitSurfaceContinuity;//S4137
  
  Standard_Real      Tol=Draw::Atof(a[3]);
  Standard_Integer Split = Draw::Atoi(a[4]);
  theTool->SetTolerance(Tol);
  theTool->SetCriterion(GeomAbs_C1);
  Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[2]);
/* 
  if ( GS.IsNull()) {
    // Case of composite grid surface
    di << "composite surf\n";
    Standard_Integer      nbU=Draw::Atoi(a[2]);
    Standard_Integer      nbV=Draw::Atoi(a[3]);
    if (nbU==0 || nbV==0) return 1;
    Handle(TColGeom_HArray2OfSurface) 
      theGrid= new TColGeom_HArray2OfSurface(1,nbU,1,nbV);
    for (Standard_Integer iu=1; iu<=nbU; iu++) {
      for (Standard_Integer iv=1; iv<=nbV; iv++) {
	Handle(Geom_Surface) GS = DrawTrSurf::GetSurface(a[3+(iu-1)*nbV+iv]);
	theGrid->SetValue(iu,iv,GS);
      }
    }
    di << "appel a SplitSurface::Init\n";
    theTool->Init(theGrid);
  }
  else {*/
    // Case of single surface
  di << "single surf\n";
  
  di << "appel a SplitSurface::Init\n";
  theTool->Init(GS);
  if(Split ==1) {
    Handle(TColStd_HSequenceOfReal) spval = new TColStd_HSequenceOfReal;
    for(Standard_Integer i = 1; i<=5; i++) spval->Append(i);
    theTool->SetUSplitValues(spval);
    theTool->SetVSplitValues(spval);
  }

  di << "appel a SplitSurface::Build\n";
  theTool->Build(Standard_True);

  di << "appel a SplitSurface::GlobalU/VKnots\n";
  Handle(ShapeExtend_CompositeSurface) Grid = theTool->ResSurfaces();
  Handle(TColStd_HArray1OfReal) GlobalU=Grid->UJointValues();
  Handle(TColStd_HArray1OfReal) GlobalV=Grid->VJointValues();
  Standard_Integer nbGlU=GlobalU->Length();
  Standard_Integer nbGlV=GlobalV->Length();
  di << "nb GlobalU ; nb GlobalV="<<nbGlU<<" "<<nbGlV;
  for (Standard_Integer iu=1; iu<=nbGlU; iu++)
    di  <<" "<< GlobalU->Value(iu);
//  di <<"\n";
//  di << "nb GlobalV="<<nbGlV;
  for (Standard_Integer iv=1; iv<=nbGlV; iv++)
    di  <<" "<< GlobalV->Value(iv);
  di <<"\n";

di << "appel a Surfaces\n";
  Handle(TColGeom_HArray2OfSurface) theSurfaces= Grid->Patches();

di << "transfert resultat\n";
  Standard_Integer NbRow=theSurfaces->ColLength();
  Standard_Integer NbCol=theSurfaces->RowLength();
  for (Standard_Integer irow=1; irow<=NbRow; irow++) {
    for (Standard_Integer icol=1; icol<=NbCol; icol++) {
      char name[100];
      Sprintf(name,"%s%s%d%s%d",a[1],"_",irow,"_",icol);
      char* newname = name;
      DrawTrSurf::Set(newname, theSurfaces->Value(irow, icol));
      di.AppendElement(newname);
    }
  }
  return 0;
}

//---------------gka
//=======================================================================
//function : offset2dcurve
//purpose  : 
//
//=======================================================================
static Standard_Integer offset2dcurve
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 4) {
    di<<"result + curve + offset\n";
    
    return 1 /* Error */;    
  }
//  Standard_CString arg1 = argv[1];
//  Standard_CString arg2 = argv[2];
  Standard_Real Offset = Draw::Atof(argv[3]);
  Handle(Geom2d_Curve) GC = DrawTrSurf::GetCurve2d(argv[2]);
  if ( GC.IsNull()) return 1;
  Handle(Geom2d_OffsetCurve) offcrv = new Geom2d_OffsetCurve(GC,Offset);
  DrawTrSurf::Set(argv[1], offcrv);
  return 0;
}

//=======================================================================
//function : offsetcurve
//purpose  : 
//
//=======================================================================
static Standard_Integer offsetcurve
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di<<"result + curve + offset + Dir\n";
    
    return 1 /* Error */;    
  }
//  Standard_CString arg1 = argv[1];  
//  Standard_CString arg2 = argv[2];
  Standard_Real Offset = Draw::Atof(argv[3]);
  Handle(Geom_Curve) GC = DrawTrSurf::GetCurve(argv[2]);
  if ( GC.IsNull()) return 1;
  gp_Pnt point;
  DrawTrSurf::GetPoint(argv[4],point);
  gp_Dir dir(point.XYZ()); 
  Handle(Geom_OffsetCurve) offcrv = new Geom_OffsetCurve(GC,Offset,dir);
  DrawTrSurf::Set(argv[1], offcrv);
  return 0;
}

//=======================================================================
//function : compose shell
//purpose  : 
//=======================================================================
static Standard_Integer splitface
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc < 5) {
    di << "Split face: splitface result face [u usplit1 usplit2...] [v vsplit1 vsplit2 ...]\n";
    return 1;    
  }
  
  TopoDS_Shape aLocalShape = DBRep::Get(argv[2]) ;
  TopoDS_Face face = TopoDS::Face ( aLocalShape );
  if ( face.IsNull() ) {
    di << argv[2] << " is not Face\n";
    return 1;
  }
  
  Handle(Geom_Surface) S = BRep_Tool::Surface ( face );
  Standard_Real Uf, Ul, Vf, Vl;
  BRepTools::UVBounds ( face, Uf, Ul, Vf, Vl );
  Standard_Real Umin, Umax, Vmin, Vmax;
  S->Bounds ( Umin, Umax, Vmin, Vmax );
  if ( Uf < Umin && ! S->IsUPeriodic() ) Uf = Umin;
  else if ( Uf > Umin ) {
    if ( Precision::IsInfinite(Umin) ) Uf -= 100;
    else Uf = Umin;
  }
  if ( Vf < Vmin && ! S->IsVPeriodic() ) Vf = Vmin;
  else if ( Vf > Vmin ) {
    if ( Precision::IsInfinite(Vmin) ) Vf -= 100;
    else Vf = Vmin;
  }
  if ( Ul > Umax && ! S->IsUPeriodic() ) Ul = Umax;
  else if ( Ul < Umax ) {
    if ( Precision::IsInfinite(Umax) ) Ul += 100;
    else Ul = Umax;
  }
  if ( Vl > Vmax && ! S->IsVPeriodic() ) Vl = Vmax;
  else if ( Vl < Vmax ) {
    if ( Precision::IsInfinite(Vmax) ) Vl += 100;
    else Vl = Vmax;
  }
  
  TColStd_SequenceOfReal uval;
  TColStd_SequenceOfReal vval;

  Standard_Boolean byV = Standard_False;
  Standard_Integer i; // svv Jan11 2000 : porting on DEC
  for ( i=3; i < argc; i++ ) {
    if ( argv[i][0] == 'u' ) byV = Standard_False;
    else if ( argv[i][0] == 'v' ) byV = Standard_True;
    else {
      Standard_Real val = Draw::Atof ( argv[i] );
      TColStd_SequenceOfReal &vals = ( byV ? vval : uval );
      if ( vals.Length() >0 && val - vals.Last() < Precision::PConfusion() ) {
	di << "Values should be sorted in increasing order; skipped\n";
	continue;
      }
      if ( ( byV && ( val < Vf+Precision::PConfusion() || 
		      val > Vl-Precision::PConfusion() ) ) ||
           (!byV && ( val < Uf+Precision::PConfusion() || 
		      val > Ul-Precision::PConfusion() ) ) ) {
	di << "Values should be inside range of surface; skipped\n";
	continue; 
      }
      vals.Append ( val );
    }
  }
  if ( uval.Length() <1 && vval.Length() <1 ) {
    di << "No splitting defined\n";
    return 1;
  }
  if ( uval.Length() >0 ) {
    di << "Splitting by U: ";
    for ( Standard_Integer j=1; j <= uval.Length(); j++ ) {
      //cout << ( i >j ? ", " : "" ) << uval(j);
      if (i >j) {
	di << ", ";
      } else {
	di << "";
      }
      di << uval(j);
    }
    di << "\n";
  }
  if ( vval.Length() >0 ) {
    di << "Splitting by V: ";
    for ( Standard_Integer j=1; j <= vval.Length(); j++ ) {
      //cout << ( j >1 ? ", " : "" ) << vval(j);
      if (j >1) {
	di << ", ";
      } else {
	di << "";
      }
      di << vval(j);
    }
    di << "\n";
  }
  
  Handle(TColGeom_HArray2OfSurface) AS = new TColGeom_HArray2OfSurface ( 1, uval.Length()+1, 
									 1, vval.Length()+1 );
  for ( i=0; i <= uval.Length(); i++ ) {
    Standard_Real umin = ( i ? uval(i) : Uf );
    Standard_Real umax = ( i < uval.Length() ? uval(i+1) : Ul );
    for ( Standard_Integer j=0; j <= vval.Length(); j++ ) {
      Standard_Real vmin = ( j ? vval(j) : Vf );
      Standard_Real vmax = ( j < vval.Length() ? vval(j+1) : Vl );
      Handle(Geom_RectangularTrimmedSurface) rect = 
	new Geom_RectangularTrimmedSurface ( S, umin, umax, vmin, vmax );
      AS->SetValue ( i+1, j+1, rect );
    }
  }

  Handle(ShapeExtend_CompositeSurface) Grid = new ShapeExtend_CompositeSurface;
  if ( ! Grid->Init ( AS ) ) di << "Grid badly connected!\n";

  ShapeFix_ComposeShell SUCS;
  TopLoc_Location l;
  SUCS.Init ( Grid, l, face, Precision::Confusion() );
  Handle(ShapeBuild_ReShape) RS = new ShapeBuild_ReShape;
  SUCS.SetContext( RS );
  SUCS.Perform ();
  
  if ( SUCS.Status ( ShapeExtend_OK ) ) di << "Status: OK\n";
  if ( SUCS.Status ( ShapeExtend_DONE1 ) ) di << "Status: DONE1\n";
  if ( SUCS.Status ( ShapeExtend_DONE2 ) ) di << "Status: DONE2\n";
  if ( SUCS.Status ( ShapeExtend_DONE3 ) ) di << "Status: DONE3\n";
  if ( SUCS.Status ( ShapeExtend_DONE4 ) ) di << "Status: DONE4\n";
  if ( SUCS.Status ( ShapeExtend_DONE5 ) ) di << "Status: DONE5\n";
  if ( SUCS.Status ( ShapeExtend_DONE6 ) ) di << "Status: DONE6\n";
  if ( SUCS.Status ( ShapeExtend_DONE7 ) ) di << "Status: DONE7\n";
  if ( SUCS.Status ( ShapeExtend_DONE8 ) ) di << "Status: DONE8\n";
  if ( SUCS.Status ( ShapeExtend_FAIL1 ) ) di << "Status: FAIL1\n";
  if ( SUCS.Status ( ShapeExtend_FAIL2 ) ) di << "Status: FAIL2\n";
  if ( SUCS.Status ( ShapeExtend_FAIL3 ) ) di << "Status: FAIL3\n";
  if ( SUCS.Status ( ShapeExtend_FAIL4 ) ) di << "Status: FAIL4\n";
  if ( SUCS.Status ( ShapeExtend_FAIL5 ) ) di << "Status: FAIL5\n";
  if ( SUCS.Status ( ShapeExtend_FAIL6 ) ) di << "Status: FAIL6\n";
  if ( SUCS.Status ( ShapeExtend_FAIL7 ) ) di << "Status: FAIL7\n";
  if ( SUCS.Status ( ShapeExtend_FAIL8 ) ) di << "Status: FAIL8\n";

  TopoDS_Shape sh = SUCS.Result();
  ShapeFix::SameParameter ( sh, Standard_False );
  DBRep::Set ( argv[1], sh );
  return 0;
}

static Standard_Integer converttobspline
  (Draw_Interpretor& di, Standard_Integer argc, const char** argv)
{
  if (argc<3) {
    di << "Use: " << argv[0] << " result shape [options=ero]\n";
    di << "where options is combination of letters indicating kinds of\n";
    di << "surfaces to be converted:\n";
    di << "e - extrusion\n";
    di << "r - revolution\n";
    di << "o - offset\n";
    di << "p - plane";
    return 1;
  }
  const char *options = ( argc > 3 ? argv[3] : "ero" );
  
  TopoDS_Shape inputShape=DBRep::Get(argv[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  TopoDS_Shape revsh = ShapeCustom::ConvertToRevolution (inputShape);
  TopoDS_Shape res = 
    ShapeCustom::ConvertToBSpline (revsh, strchr (options, 'e') != 0,
                                          strchr (options, 'r') != 0,
                                          strchr (options, 'o') != 0,
                                          strchr (options, 'p') != 0);
  ShapeFix::SameParameter ( res, Standard_False );
  DBRep::Set ( argv[1], res );
  return 0;
}


static Standard_Integer splitclosed (Draw_Interpretor& di, 
				     Standard_Integer argc, 
				     const char** argv)
{
  if (argc<3) {
    di << "bad number of arguments\n";
    return 1;
  }
  
  TopoDS_Shape inputShape=DBRep::Get(argv[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  
  ShapeUpgrade_ShapeDivideClosed tool (inputShape);
  tool.Perform();
  TopoDS_Shape res = tool.Result();
  
  ShapeFix::SameParameter ( res, Standard_False );
  DBRep::Set ( argv[1], res );
  return 0;
}

static Standard_Integer splitarea (Draw_Interpretor& di, 
				     Standard_Integer argc, 
				     const char** argv)
{
  if (argc<4) {
    di << "bad number of arguments\n";
    return 1;
  }
  
  TopoDS_Shape inputShape=DBRep::Get(argv[2]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  Standard_Real aMaxArea = Draw::Atof(argv[3]);
  
    
  ShapeUpgrade_ShapeDivideArea tool (inputShape);
  if(argc >4) {
    Standard_Real prec = Draw::Atof(argv[4]);
    tool.SetPrecision(prec);
  }
  tool.MaxArea() = aMaxArea;
  tool.Perform();
  TopoDS_Shape res = tool.Result();
  
  ShapeFix::SameParameter ( res, Standard_False );
  DBRep::Set ( argv[1], res );
  return 0;
}

static Standard_Integer removeinternalwires (Draw_Interpretor& di, 
                                             Standard_Integer argc, 
                                             const char** argv)
{
  if (argc<4) {
    di << "bad number of arguments\n";
    return 1;
  }
  Standard_Real aMinArea = Draw::Atof(argv[2]);
  TopoDS_Shape inputShape=DBRep::Get(argv[3]);
  if (inputShape.IsNull()) {
    di << "Unknown shape\n";
    return 1;
  }
  Handle(ShapeUpgrade_RemoveInternalWires) aTool;
  TopTools_SequenceOfShape aSeqShapes;
  if(inputShape.ShapeType() < TopAbs_WIRE)
     aTool = new ShapeUpgrade_RemoveInternalWires(inputShape);
  else {
   di<<"Invalid type of first shape: should be FACE,SHELL,SOLID or COMPOUND\n";
   return 1;
  }
  
  Standard_Integer k = 4;
  Standard_Boolean isShape = Standard_True;
  Standard_Boolean aModeRemoveFaces =Standard_True;
  
 
  for( ; k < argc; k++) {
    if(isShape) {
      TopoDS_Shape aShape=DBRep::Get(argv[k]);
      isShape = !aShape.IsNull();
      if(isShape) {
        if(aShape.ShapeType() == TopAbs_FACE || aShape.ShapeType() == TopAbs_WIRE)
          aSeqShapes.Append(aShape);
      }
    }
    if(!isShape) 
      aModeRemoveFaces = (Draw::Atoi(argv[k]) == 1);
  }
  
  aTool->MinArea() = aMinArea;
  aTool->RemoveFaceMode() = aModeRemoveFaces;
  if(aSeqShapes.Length())
    aTool->Perform(aSeqShapes);
  else
    aTool->Perform();
  if(aTool->Status(ShapeExtend_FAIL1))
     di<<"Initial shape has invalid type\n";
  else if(aTool->Status(ShapeExtend_FAIL2))
     di<<"Specified sub-shape is not belonged to whole shape\n";   
  if(aTool->Status(ShapeExtend_DONE1)) {
    const TopTools_SequenceOfShape& aRemovedWires =aTool->RemovedWires(); 
     di<<aRemovedWires.Length()<<" internal wires were removed\n";
    
  }
  if(aTool->Status(ShapeExtend_DONE2)) {
    const TopTools_SequenceOfShape& aRemovedFaces =aTool->RemovedFaces(); 
     di<<aRemovedFaces.Length()<<" small faces were removed\n";
    
  }   
  TopoDS_Shape res = aTool->GetResult();
  
  
  DBRep::Set ( argv[1], res );
  return 0;
}

static Standard_Integer removeloc (Draw_Interpretor& di, 
                                   Standard_Integer argc, 
                                   const char** argv)
{
  if (argc<3) {
    di << "bad number of arguments. Should be:  removeloc res shape [remove_level(see ShapeEnum)]\n";
    return 1;
  }
  
  TopoDS_Shape aShape = DBRep::Get(argv[2]);
  if(aShape.IsNull())
    return 1;
  ShapeUpgrade_RemoveLocations aRemLoc;
  if (argc > 3)
    aRemLoc.SetRemoveLevel((TopAbs_ShapeEnum)Draw::Atoi(argv[3]));
  aRemLoc.Remove(aShape);
  TopoDS_Shape aNewShape = aRemLoc.GetResult();
  
  DBRep::Set(argv[1],aNewShape);
  return 0;
}

static ShapeUpgrade_UnifySameDomain& Unifier() {
  static ShapeUpgrade_UnifySameDomain sUnifier;
  return sUnifier;
}

//=======================================================================
// unifysamedom
//=======================================================================
static Standard_Integer unifysamedom(Draw_Interpretor& di, Standard_Integer n, const char** a)
{
  if (n < 3)
  {
    di << "Use unifysamedom result shape [s1 s2 ...] [-f] [-e] [-nosafe] [+b] [+i] [-t val] [-a val]\n";
    di << "options:\n";
    di << "s1 s2 ... to keep the given edges during unification of faces\n";
    di << "-f to switch off 'unify-faces' mode \n";
    di << "-e to switch off 'unify-edges' mode\n";
    di << "-nosafe to switch off 'safe input shape' mode\n";
    di << "+b to switch on 'concat bspline' mode\n";
    di << "+i to switch on 'allow internal edges' mode\n";
    di << "-t val to set linear tolerance\n";
    di << "-a val to set angular tolerance\n";
    di << "'unify-faces' and 'unify-edges' modes are switched on by default";
    return 1;
  }

  TopoDS_Shape aShape = DBRep::Get(a[2]);
  if (aShape.IsNull())
    return 1;

  // default values
  Standard_Boolean anUFaces = Standard_True;
  Standard_Boolean anUEdges = Standard_True;
  Standard_Boolean anConBS = Standard_False;
  Standard_Boolean isAllowInternal = Standard_False;
  Standard_Boolean isSafeInputMode = Standard_True;
  Standard_Real aLinTol = Precision::Confusion();
  Standard_Real aAngTol = Precision::Angular();
  TopoDS_Shape aKeepShape;
  TopTools_MapOfShape aMapOfShapes;

  if (n > 3)
    for ( int i = 3; i < n; i++ ) 
    {
      aKeepShape = DBRep::Get(a[i]);
      if (!aKeepShape.IsNull()) {
        aMapOfShapes.Add(aKeepShape);
      }
      else {
        if ( !strcmp(a[i], "-f")) 
          anUFaces = Standard_False;
        else if (!strcmp(a[i], "-e"))
          anUEdges = Standard_False;
        else if (!strcmp(a[i], "-nosafe"))
          isSafeInputMode = Standard_False;
        else if (!strcmp(a[i], "+b"))
          anConBS = Standard_True;
        else if (!strcmp(a[i], "+i"))
          isAllowInternal = Standard_True;
        else if (!strcmp(a[i], "-t") || !strcmp(a[i], "-a"))
        {
          if (++i < n)
          {
            (a[i-1][1] == 't' ? aLinTol : aAngTol) = Draw::Atof(a[i]);
          }
          else
          {
            di << "value expected after " << a[i-1];
            return 1;
          }
        }
      }
    }

  Unifier().Initialize(aShape, anUEdges, anUFaces, anConBS);
  Unifier().KeepShapes(aMapOfShapes);
  Unifier().SetSafeInputMode(isSafeInputMode);
  Unifier().AllowInternalEdges(isAllowInternal);
  Unifier().SetLinearTolerance(aLinTol);
  Unifier().SetAngularTolerance(aAngTol);
  Unifier().Build();
  TopoDS_Shape Result = Unifier().Shape();

  DBRep::Set(a[1], Result);
  return 0;
}

Standard_Integer unifysamedommod(Draw_Interpretor& di,
                                 Standard_Integer n,
                                 const char** a)
{
  if (n != 3) {
    di << "use unifysamedommod newshape oldshape\n";
    return 0;
  }
  TopoDS_Shape aShape;
  aShape = DBRep::Get(a[2]);
  if (aShape.IsNull()) {
    di << "Null shape is not allowed here\n";
    return 1;
  }

  const TopTools_ListOfShape& aLS = Unifier().History()->Modified(aShape);

  if (aLS.Extent() > 1) {
    BRep_Builder aBB;
    TopoDS_Compound aRes;
    aBB.MakeCompound(aRes);
    TopTools_ListIteratorOfListOfShape aIt(aLS);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aCurrentShape = aIt.Value();
      aBB.Add(aRes, aCurrentShape);
    }
    DBRep::Set(a[1], aRes);
  }
  else if (aLS.Extent() == 1) {
    DBRep::Set(a[1], aLS.First());
  }
  else {
    di << "The shape has not been modified\n";
  }
  return 0;
}

Standard_Integer unifysamedomisdel(Draw_Interpretor& di,
                                   Standard_Integer n,
                                   const char** a)
{
  if (n < 2) {
    di << "Use: unifysamedomisdel shape\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(a[1]);
  if (aShape.IsNull()) {
    di << "Null shape is not allowed here\n";
    return 1;
  }
  Standard_Boolean IsDeleted = Unifier().History()->IsRemoved(aShape);
  di << "The shape has" << (IsDeleted ? " " : " not ") << "been deleted" << "\n";
  return 0;
}

static Standard_Integer copytranslate(Draw_Interpretor& di, 
                                   Standard_Integer argc, 
                                   const char** argv)
{
  if (argc<6) {
    di << "bad number of arguments. Should be:  copytranslate res shape dx dy dz\n";
    return 1;
  }
  TopoDS_Shape aShape = DBRep::Get(argv[2]);
  if(aShape.IsNull())
    return 1;
  Standard_Real aDx = Draw::Atof(argv[3]);
  Standard_Real aDy = Draw::Atof(argv[4]);
  Standard_Real aDz = Draw::Atof(argv[5]);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(aDx, aDy, aDz));
  BRepBuilderAPI_Transform builderTransform(aTrsf);
  builderTransform.Perform (aShape, true); 
  TopoDS_Shape aNewShape = builderTransform.Shape();
  DBRep::Set(argv[1],aNewShape);
  return 0;
  
}

Standard_Integer reshape(Draw_Interpretor& di,
                         Standard_Integer n,
                         const char** a)
{
  if ( n < 3 )
  {
    di << "Error: wrong number of arguments. Type 'help " << a[0] << "'\n";
    return 1;
  }

  TopoDS_Shape source = DBRep::Get(a[2]);
  if ( source.IsNull() )
  {
    di << "Error: source shape ('" << a[2] << "') is null\n";
    return 1;
  }

  Handle(BRepTools_ReShape) ReShaper = new BRepTools_ReShape;

  // Record the requested modifications
  for ( Standard_Integer i = 1; i < n; ++i )
  {
    Standard_CString        arg = a[i];
    TCollection_AsciiString opt(arg);
    opt.LowerCase();

    if ( opt == "-replace" )
    {
      if ( n - i < 3 )
      {
        di << "Error: not enough arguments for replacement\n";
        return 1;
      }

      TopoDS_Shape what = DBRep::Get(a[++i]);
      if ( what.IsNull() )
      {
        di << "Error: argument shape ('" << a[i] << "') is null\n";
        return 1;
      }

      TopoDS_Shape with = DBRep::Get(a[++i]);
      if ( with.IsNull() )
      {
        di << "Error: replacement shape ('" << a[i] << "') is null\n";
        return 1;
      }

      ReShaper->Replace(what, with);
    }
    else if ( opt == "-remove" )
    {
      if ( n - i < 2 )
      {
        di << "Error: not enough arguments for removal\n";
        return 1;
      }

      TopoDS_Shape what = DBRep::Get(a[++i]);
      if ( what.IsNull() )
      {
        di << "Error: shape to remove ('" << a[i] << "') is null\n";
        return 1;
      }

      ReShaper->Remove(what);
    }
  }

  // Apply all the recorded modifications
  TopoDS_Shape result = ReShaper->Apply(source);
  if ( result.IsNull() )
  {
    di << "Error: result shape is null\n";
    return 1;
  }

  DBRep::Set(a[1], result);
  return 0;
}

//=======================================================================
//function : InitCommands
//purpose  : 
//=======================================================================

 void SWDRAW_ShapeUpgrade::InitCommands(Draw_Interpretor& theCommands) 
{
  static Standard_Integer initactor = 0;
  if (initactor)
  {
    return;
  }
  initactor = 1;
  
  Standard_CString g = SWDRAW::GroupName(); // "Tests of DivideTool";
 
  theCommands.Add("DT_ShapeDivide",
		  "DT_ShapeDivide Result Shape Tol: Divides shape with C1 Criterion",
		  __FILE__,
		  DT_ShapeDivide,g);
  
  theCommands.Add("DT_SplitAngle",
		  "DT_SplitAngle Result Shape [MaxAngle=95]: Divides revolved surfaces on segments less MaxAngle deg",
		  __FILE__,
		  DT_SplitAngle,g);

  theCommands.Add("DT_ShapeConvert",
		  "DT_ShapeConvert Result Shape convert2d convert3d: Converts curves to beziers",
		  __FILE__,
		  DT_ShapeConvert,g);
  
  theCommands.Add("DT_ShapeConvertRev",
		  "DT_ShapeConvert Result Shape convert2d convert3d: Converts curves to beziers",
		  __FILE__,
		  DT_ShapeConvertRev,g);
/*  theCommands.Add("DT_PlaneDividedFace",
		  "DT_PlaneDividedFace Result Face Tol: Transfer into a plane with boundary divided",
		  __FILE__,
		  DT_PlaneDividedFace,g);

  theCommands.Add("DT_PlaneGridShell",
		  "DT_PlaneGridShell Result NbU NbV {UKnots} {VKnots} Tol : Create a plane grid Shell",
		  __FILE__,
		  DT_PlaneGridShell,g);

  theCommands.Add("DT_PlaneFaceCommon",
		  "DT_PlaneFaceCommon Result Face Shell: Common between a plane Face and a Shell",
		  __FILE__,
		  DT_PlaneFaceCommon,g);*/

  theCommands.Add("DT_SplitCurve2d",
		  "DT_SplitCurve2d Curve Tol: Splits the curve with C1 criterion",
		  __FILE__,
		  DT_SplitCurve2d,g);

  theCommands.Add("DT_SplitCurve",
		  "DT_SplitCurve Curve Tol: Splits the curve with C1 criterion",
		  __FILE__,
		  DT_SplitCurve,g);

  theCommands.Add("DT_SplitSurface",
		  "DT_SplitSurface Result Surface/GridSurf Tol: Splits the surface with C1 criterion",
		  __FILE__,
		  DT_SplitSurface,g);

  /*theCommands.Add("DT_SupportModification",
		  "DT_SupportModification Result Shell Surface 2d3dFactor: Surface will support all the faces",
		  __FILE__,
		  DT_SupportModification,g);*/

//  theCommands.Add("DT_SpltWire","DT_SpltWire Result Wire Tol",
//		  __FILE__,DT_SplitWire,g);
  
//  theCommands.Add("DT_SplitFace", "DT_SplitFace Result Face Tol",
//		  __FILE__, DT_SplitFace,g);
  
//  theCommands.Add("DT_Debug", "DT_Debug 0/1 : activation/desactivation of the debug messages",
//		  __FILE__, DT_Debug,g);
//  theCommands.Add ("shellsolid","option[a-b-c-f] shape result",
//		   __FILE__,shellsolid,g);
  theCommands.Add ("offset2dcurve","result curve offset",
		   __FILE__,offset2dcurve,g);
  
  theCommands.Add ("offsetcurve","result curve offset dir",
		   __FILE__,offsetcurve,g);

  theCommands.Add ("splitface","result face [u usplit1 usplit2...] [v vsplit1 vsplit2 ...]",
		   __FILE__,splitface,g);
  
  theCommands.Add ("DT_ToBspl","result shape [options=erop]",
		   __FILE__,converttobspline,g);
  theCommands.Add ("DT_ClosedSplit","result shape",
		   __FILE__,splitclosed,g);
  theCommands.Add ("DT_SplitByArea","result shape maxarea [preci]",
		   __FILE__,splitarea,g);
  
  theCommands.Add ("RemoveIntWires","result minarea wholeshape [faces or wires] [moderemoveface ]",
                   __FILE__,removeinternalwires,g);
  
  theCommands.Add ("removeloc","result shape [remove_level(see ShapeEnum)]",__FILE__,removeloc,g);
  
  theCommands.Add ("unifysamedom",
                   "unifysamedom result shape [s1 s2 ...] [-f] [-e] [-nosafe] [+b] [+i] [-t val] [-a val]",
                    __FILE__,unifysamedom,g);

  theCommands.Add("unifysamedommod",
                  "unifysamedommod newshape oldshape : get new shape modified "
                  "by unifysamedom command from the old one",
                  __FILE__, unifysamedommod, g);

  theCommands.Add("unifysamedomisdel",
                  "unifysamedomisdel shape : shape is deleted ",
                  __FILE__, unifysamedomisdel, g);
  
  theCommands.Add ("copytranslate","result shape dx dy dz",__FILE__,copytranslate,g);

  theCommands.Add ("reshape",
    "\n    Basic utility for topological modification: "
    "\n      '-replace what with'   Replaces 'what' sub-shape with 'with' sub-shape"
    "\n      '-remove what'         Removes 'what' sub-shape"
    "\n    Requests '-replace' and '-remove' can be repeated many times.",
    __FILE__, reshape, g);
}
