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

//=======================================================================
//purpose  : Members to transfert any IGES Curves into TopoDS_Shape
//=======================================================================
// modif mjm du 23/09/97 : appel a ShapeTool en remplacement a PCurveLib
// 21.12.98 rln, gka S4054
//%11 12.01.99 pdn CTS22023 reversing direction when reading offset curves
//:q5 abv 19.03.99 unnecessary includes removed
//%14 03.03.99 pdn USA60022 do not insert lacking edge before fix missing seam
// pdn 10.03.99 S4135 Creating vertices using minimal tolerance.
//S4181 pdn 17.04.99 Inplementing of reading IGES elementary surfaces. Transform
//pcurves using gp_Trsf2d that depends on type of pair (IGES surface, resulting 
//CAS.CADE surface)
//szv#9:PRO19565:04Oct99 missing location for standalone vertices corrected

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepTools.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_XY.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_Boundary.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CompositeCurve.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_OffsetCurve.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_SplineCurve.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_AlgoContainer.hxx>
#include <IGESToBRep_BasicCurve.hxx>
#include <IGESToBRep_BasicSurface.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_IGESBoundary.hxx>
#include <IGESToBRep_ToolContainer.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <IGESToBRep_TopoSurface.hxx>
#include <Interface_Macros.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeBuild_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColGeom2d_HSequenceOfBoundedCurve.hxx>
#include <TColGeom_HSequenceOfBoundedCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TopAbs.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <stdio.h>
//added by rln 32/12/97
//#include <TColStd_Array1OfInteger.hxx>
//#include <TColStd_Array1OfReal.hxx>
//#include <TColStd_HSequenceOfTransient.hxx>
//=======================================================================
//function : IGESToBRep_TopoCurve
//purpose  : 
//=======================================================================
IGESToBRep_TopoCurve::IGESToBRep_TopoCurve()
:IGESToBRep_CurveAndSurface()
{  
}


//=======================================================================
//function : IGESToBRep_TopoCurve
//purpose  : 
//=======================================================================

IGESToBRep_TopoCurve::IGESToBRep_TopoCurve (const IGESToBRep_TopoCurve& other)
     :IGESToBRep_CurveAndSurface (other)
{  
  TheCurves.Assign(other.TheCurves);
  TheCurves2d.Assign(other.TheCurves2d);
}


//=======================================================================
//function : IGESToBRep_TopoCurve
//purpose  : 
//=======================================================================

IGESToBRep_TopoCurve::IGESToBRep_TopoCurve
  (const IGESToBRep_CurveAndSurface& CS)
     :IGESToBRep_CurveAndSurface(CS)
{  
}


//=======================================================================
//function : IGESToBRep_TopoCurve
//purpose  : 
//=======================================================================

IGESToBRep_TopoCurve::IGESToBRep_TopoCurve
  (const Standard_Real    eps,
   const Standard_Real    epsCoeff,
   const Standard_Real    epsGeom,
   const Standard_Boolean mode,
   const Standard_Boolean modeapprox,
   const Standard_Boolean optimized)
:IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, modeapprox, 
			    optimized)
{  
}


//=======================================================================
//function : TransferPoint
//purpose  : 
//=======================================================================

TopoDS_Vertex  IGESToBRep_TopoCurve::TransferPoint
       (const Handle(IGESGeom_Point)& start)
     
{
  TopoDS_Vertex  V1;
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005");
    SendFail(start, Msg1005); //"Point Transfer Error : Null IGESEntity"
    //AddFail(start, "Point Transfer Error : Null IGESEntity");
    return  V1;
  }

  BRep_Builder B;
  gp_Pnt point;
  
  if (!GetModeTransfer() && start->HasTransf())
    point = start->TransformedValue();
  else
    point = start->Value();

  point.Scale(gp_Pnt(0,0,0),GetUnitFactor());
  B.MakeVertex(V1, point, Precision::Confusion());//S4135: GetEpsGeom()*GetUnitFactor()

  //szv#9:PRO19565:04Oct99
  if (GetModeTransfer() && start->HasTransf()) {
    gp_Trsf T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),start->CompoundLocation(),T,GetUnitFactor())) { 
      TopLoc_Location L(T);
      V1.Move(L);
    }
  }
  
  return V1;
}

//=======================================================================
//function : Transfer2dPoint
//purpose  : 
//=======================================================================

TopoDS_Vertex  IGESToBRep_TopoCurve::Transfer2dPoint
       (const Handle(IGESGeom_Point)& start)
     
{
  TopoDS_Vertex V1;
  if (start.IsNull()) {
     Message_Msg Msg1005("IGES_1005");//"2D Point Transfer Error : Null IGESEntity"
     SendFail(start, Msg1005);
   // AddFail(start, "2D Point Transfer Error : Null IGESEntity");
    return  V1;
  }

  BRep_Builder B;
  gp_Pnt point;
  
  if (!GetModeTransfer() && start->HasTransf())
    point = gp_Pnt(start->TransformedValue().X(), 
		   start->TransformedValue().Y(),
		   0.);
  else
    point = gp_Pnt(start->Value().X(), 
		   start->Value().Y(),
		   0.);  

  B.MakeVertex(V1, point, Precision::Confusion());//S4135: GetEpsCoeff()
  return V1;
}

//=======================================================================
//function : TransferCompositeCurveGeneral
//purpose  : General transfer for both 2d and 3d cases
//=======================================================================

//:13 by abv 13 Nov 97: common part of two methods (see below)
TopoDS_Shape  IGESToBRep_TopoCurve::TransferCompositeCurveGeneral(const Handle(IGESGeom_CompositeCurve)& start, 
								  const Standard_Boolean is2d,
								  const TopoDS_Face& face,
								  const gp_Trsf2d& trans,
								  const Standard_Real uFact)

{
  TopoDS_Shape  res;

  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"CompositeCurve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    // AddFail(start, "CompositeCurve Transfer Error : Null IGESEntity");
    return  res;
  }

  Standard_Real precision = GetEpsGeom() * GetUnitFactor(), maxtol = GetMaxTol();
  
  Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData();
  Handle(ShapeAnalysis_Wire) saw = new ShapeAnalysis_Wire;
  saw->Load (sewd);
  saw->SetPrecision (precision);
  
  for (Standard_Integer i=1; i<= start->NbCurves(); i++) {

    Handle(IGESData_IGESEntity)  IgesEnt = start->Curve(i);
    //added by rln 26/12/97 CSR# UKI60028 entity 3117
    if (i > 1 && IgesEnt == start->Curve (i-1)) {
//      char mess[80];
      Message_Msg Msg1045("IGES_1045"); //"The entities of the CompositeCurve are the same: %d & %d"
      Msg1045.Arg(i);
      SendWarning(start,Msg1045);
      //sprintf (mess, "The entities of the CompositeCurve are the same: %d & %d", i-1, i);
      //AddWarning (start, mess);
      continue;
    }
    if (IgesEnt.IsNull()) {
      Message_Msg Msg1040("IGES_1040");//"Curve%dd is a null object : number %d"
      Msg1040.Arg(i); 
      SendFail(start,Msg1040);
      return res;
    }
    
    if (is2d && (IgesEnt->IsKind(STANDARD_TYPE(IGESGeom_Boundary)) ||
		 IgesEnt->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface)) ) ) {
      //AddWarning(start, "Entity cannot be built from a boundary.");
      Message_Msg Msg1040("IGES_1040");//"Entity cannot be built from a boundary."
      Msg1040.Arg(i); 
      SendFail(start,Msg1040);
      return res;
    }
    else if (IGESToBRep::IsTopoCurve(IgesEnt)) {
      TopoDS_Shape shape; //:13 = TransferTopoCurve(IgesEnt);
      if ( is2d ) shape = Transfer2dTopoCurve ( IgesEnt, face, trans, uFact ); //:13
      else        shape = TransferTopoCurve   ( IgesEnt ); //:13
      if (!shape.IsNull()){
	if (shape.ShapeType() == TopAbs_VERTEX) continue;

	Handle(ShapeExtend_WireData) nextsewd = new ShapeExtend_WireData;
	nextsewd->Add (shape);
	Standard_Real distmin;
	Standard_Boolean revsewd, revnextsewd;
	Standard_Boolean isConnected = ShapeAlgo::AlgoContainer()->ConnectNextWire (saw, nextsewd, maxtol, distmin,
										    revsewd, revnextsewd);
	if (isConnected) {
	  if (revsewd) {
	    Message_Msg Msg1051("IGES_1051");   //"All curvAll curves %dd before rank %d have been to be reversed."
	    Msg1051.Arg((is2d ? 2 : 3));
	    Msg1051.Arg(i);
	    SendWarning(start,Msg1051);
	  }
	  if (revnextsewd) {
	    Message_Msg Msg1050("IGES_1050");   //"Curve %dd needs to be reversed : %d"
	    Msg1050.Arg((is2d ? 2 : 3));
	    Msg1050.Arg(i);
	    SendWarning(start,Msg1050);
	    
	    //  sprintf(mess, "Curve %dd needs to be reversed : %d", ( is2d ? 2 : 3 ), i);//:13
	  //  AddWarning(start, mess);
	  }
	  if (distmin > precision) {
	    Message_Msg Msg1055("IGES_1055");   
	    Msg1055.Arg((is2d ? 2 : 3));
	    Msg1055.Arg(i);
	    SendWarning(start,Msg1055);
	  }
	}
	else {
	  Message_Msg Msg1060("IGES_1060");   //"Curves %dd are too much disconnected : %d & %d"
	  Msg1060.Arg((is2d ? 2 : 3));
	  Msg1060.Arg(i);
	  SendFail(start,Msg1060);
	  return res;
	}
      }
    }
  }

  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load (sewd);
  sfw->ClosedWireMode() = Standard_False;//closing of face boundaries will be later
  sfw->FixConnected (maxtol);
  sfw->FixConnected (1, precision);//10.12.98 CTS18953 entity 14 (Plane+CompositeCurve)
  res = sewd->Wire();
  
  SetShapeResult (start, res);
  return  res;
}

//=======================================================================
//function : TransferCompositeCurve
//purpose  : Implementation for 3d case
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferCompositeCurve
  (const Handle(IGESGeom_CompositeCurve)&  start)

{
  TopoDS_Shape res;
  TopoDS_Face f;
  gp_Trsf trans;
  res = TransferCompositeCurveGeneral ( start, Standard_False, f, trans, 1.);

  if ( ! res.IsNull() && start->HasTransf()) {
    gp_Trsf  T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),start->CompoundLocation(),T,GetUnitFactor())) { 
      TopLoc_Location  L(T);
      res.Move(L);
    }
    else {
      Message_Msg Msg1035("IGES_1035");
      SendWarning(start,Msg1035);
    }
      //  AddWarning(start, "Transformation skipped (not a similarity)");
  }
  return res;
}

//=======================================================================
//function : Transfer2dCompositeCurve
//purpose  : Implementation for 2d case
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::Transfer2dCompositeCurve
       (const Handle(IGESGeom_CompositeCurve)&  start,
	const TopoDS_Face&          face,
	const gp_Trsf2d& trans,
	const Standard_Real uFact)
     
{
  TopoDS_Shape res;
  res = TransferCompositeCurveGeneral ( start, Standard_True, face, trans, uFact);
  // Message occur if needed in TransferCompositeCurveGeneral
  //if (start->HasTransf()) {
  //  Message_Msg Msg1036("IGES_1036"); //"The Trsf cannot be applied to the entity."
  //  SendWarning(start,Msg1036);
  // AddWarning(start, "The Trsf cannot be applied to the entity.");
  //}
  return  res;
}

//=======================================================================
//function : TransferCurveOnSurface
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferCurveOnSurface
  (const Handle(IGESGeom_CurveOnSurface)& start)
{
  TopoDS_Shape  res;
  
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005");
    SendFail(start,Msg1005);
    //  AddFail(start, "CurveOnSurface Transfer Error : Null IGESEntity");
    return  res;
  }

  TopoDS_Face  face;
  Handle (IGESData_IGESEntity) igesSurface = start->Surface();
  if (igesSurface.IsNull() || !IGESToBRep::IsTopoSurface(igesSurface)) {
    Message_Msg Msg131("XSTEP_131");  //"BasicSurface Transfer Error : Null IGESEntity"
    SendFail(start,Msg131);
    //AddFail(start, "BasicSurface Transfer Error : Null IGESEntity");
    return res;
  }
  TopAbs_ShapeEnum shapeEnum;
  IGESToBRep_TopoSurface TS(*this);
  gp_Trsf2d trans;
  Standard_Real uFact;
  TopoDS_Shape myshape = TS.ParamSurface (igesSurface, trans, uFact);

  if (!myshape.IsNull()) {
    shapeEnum = myshape.ShapeType();
    switch (shapeEnum) {
    case TopAbs_FACE :
      {
	face = TopoDS::Face(myshape);
	break;
      }
    case TopAbs_SHELL :
      {
	TopoDS_Iterator IT(myshape);
	Standard_Integer nbfaces = 0;
	for (; IT.More(); IT.Next()) {
	  nbfaces++;
	  face = TopoDS::Face(IT.Value());
	}
	//szv#4:S4163:12Mar99 optimized
	if (nbfaces != 1) {
	  if (!start->Curve3D().IsNull()) {
	    if (IGESToBRep::IsTopoCurve(start->Curve3D())) {
	      TopoDS_Shape  Sh = TransferTopoCurve(start->Curve3D());
	      if (!Sh.IsNull()) {
		Message_Msg Msg1062("IGES_1062");
     //CurveOnSurface on Composite Surface case not implemented : 3D representation returned.
		SendWarning(start,Msg1062);
		res = Sh;
	      }
	    }
	  }
	  else {
	    Message_Msg Msg1061("IGES_1061"); 
	    Msg1061.Arg("CurveOnSurface");
	    SendFail(start,Msg1061);
	  }
	  return res;
	}
      }
      break;
    default:
      {
	if (!start->Curve3D().IsNull()) {
	  if (IGESToBRep::IsTopoCurve(start->Curve3D())) {
	    TopoDS_Shape  Sh = TransferTopoCurve(start->Curve3D());
	    if (!Sh.IsNull()) {
	      Message_Msg Msg1062("IGES_1062"); //"Basis Surface Error : 3dCurve returned"
	      SendWarning(start,Msg1062);
	      //   AddWarning(start,"Basis Surface Error : 3dCurve returned");
	      res = Sh;
	    }
	  }
	}
	else {
	  Message_Msg Msg1061("IGES_1061"); 
	  Msg1061.Arg("CurveOnSurface");
	  SendFail(start,Msg1061);
	}
	//  AddFail(start, "Basis Surface Error.");
	return res;
      }
    }
  }

  face.EmptyCopy();
  res = TransferCurveOnFace (face, start, trans, uFact, Standard_True);
  return res;
}


//=======================================================================
//function : TransferCurveOnFace
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoCurve::TransferCurveOnFace(TopoDS_Face&  face, 
						       const Handle(IGESGeom_CurveOnSurface)& start,
						       const gp_Trsf2d& trans,
						       const Standard_Real uFact,
						       const Standard_Boolean isCurveOnSurf)
{
  TopoDS_Shape  res;
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"CurveOnFace Transfer Error : Null IGESEntity"
    SendFail(start,Msg1005);
    return res;
  }
  
  Standard_Boolean okCurve = Standard_True, okCurve3d = Standard_True, okCurve2d = Standard_True;
  Standard_Integer filepreference = 0;
  if      (start->PreferenceMode() == 1) filepreference = 2;
  else if (start->PreferenceMode() == 2) filepreference = 3;
  Handle(IGESData_HArray1OfIGESEntity) Curves2d = new IGESData_HArray1OfIGESEntity (1, 1);
  Curves2d->SetValue (1, start->CurveUV());
  
  Handle(IGESToBRep_IGESBoundary) IB = IGESToBRep::AlgoContainer()->ToolContainer()->IGESBoundary();
  IB->Init (*this, start, face, trans, uFact, filepreference);
  Standard_Boolean Result = IB->Transfer (okCurve, okCurve3d, okCurve2d,
					  start->Curve3D(), Standard_False,
					  Curves2d, 1);
  IB->Check(Result, !isCurveOnSurf, okCurve3d, okCurve2d);
  Handle(ShapeExtend_WireData) sewd = IB->WireData();
  if (sewd->NbEdges() == 0) {
      Message_Msg Msg1095("IGES_1095");//"Both 2d and 3d representations are invalid"
      SendFail(start,Msg1095);
    return res;
  }
  
  //%14 pdn 03.03.99
  //IB.Fix (sewd, Standard_True, !isCurveOnSurf, Standard_False, Standard_False, Standard_False);
  TopoDS_Wire mywire = sewd->Wire();

  if (start->HasTransf()) {
    gp_Trsf  T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),start->CompoundLocation(),T)) {
      TopLoc_Location L(T);
      mywire.Move(L);
    }
    else {
      Message_Msg Msg1035("IGES_1035"); //"Transformation skipped (not a similarity)"
      SendWarning(start,Msg1035);
    }
  }

  BRepLib_MakeFace MF(face);
  MF.Add(mywire);
  face = MF.Face();
  SetShapeResult (start, mywire);
  return mywire;
}


//=======================================================================
//function : TransferOffsetCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferOffsetCurve
  (const Handle(IGESGeom_OffsetCurve)&  start)

{
  TopoDS_Shape  res;

  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"Offset Curve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //AddFail(start, "OffsetCurve Transfer Error : Null IGESEntity");
    return  res;
  }
  if (start->OffsetType() != 1) {
    Message_Msg Msg1100("IGES_1100"); //"Offset distance flag different from 1 not treated"
    SendFail(start,Msg1100);
    //AddFail(start, "Offset distance flag different from 1 not treated");
    return  res;
  }

  Standard_Real    Offset = start->FirstOffsetDistance();
  gp_Dir  NrmDir;
  if (start->HasTransf()) 
    NrmDir = start->TransformedNormalVector();
  else
    NrmDir = start->NormalVector();
  //%11 pdn 12.01.99 CTS22023
  NrmDir.Reverse();

  Handle(IGESData_IGESEntity)  BaseCrv = start->BaseCurve();

  while (BaseCrv->IsKind(STANDARD_TYPE(IGESGeom_OffsetCurve))) {
    DeclareAndCast(IGESGeom_OffsetCurve, OffCrv, BaseCrv);
    if (OffCrv->OffsetType() != 1) {
       Message_Msg Msg1100("IGES_1100"); //"Offset distance flag different from 1 not treated"
       SendFail(start,Msg1100);
       return  res;
    }
    BaseCrv = OffCrv->BaseCurve();
    Offset  = Offset + OffCrv->FirstOffsetDistance();
  }


  if (!IGESToBRep::IsTopoCurve(BaseCrv)) {
   Message_Msg Msg110("XSTEP_110");
   SendFail(start,Msg110);
   return res;
 }
  
  Handle(Geom_Curve)  Crv;
  Handle(Geom_OffsetCurve)  OffCrv;
  
  IGESToBRep_TopoCurve  TC(*this);
  TopoDS_Shape  Sh = TC.TransferTopoCurve(BaseCrv);
  
  if (Sh.IsNull() || ! ((Sh.ShapeType() == TopAbs_EDGE) || (Sh.ShapeType() == TopAbs_WIRE)) ) {
    Message_Msg Msg1156("IGES_1156");
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(BaseCrv);
    Msg1156.Arg("basis curve");
    Msg1156.Arg(label);
    SendFail(start,Msg1156);
    //AddFail(start, "Edge or wire expected from TransferTopoCurve");
    return res;
  }

  if (Sh.ShapeType() == TopAbs_EDGE) {
    TopLoc_Location  aLoc;  
    Standard_Real  a, b;
    Crv = BRep_Tool::Curve(TopoDS::Edge(Sh), aLoc, a, b);
    OffCrv = new Geom_OffsetCurve(Crv,Offset,NrmDir);
    BRepBuilderAPI_MakeEdge  ME(OffCrv,start->StartParameter(),start->EndParameter());
    if (!ME.IsDone()) {
      
      Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
      SendFail(start,Msg1005);
      //AddFail(start, "Edge construction error");
      return  res;
    }
    TopoDS_Edge  anEdge = ME.Edge();  
    anEdge.Move(aLoc);
    res = anEdge;
  }
    
  else if (Sh.ShapeType() == TopAbs_WIRE) {
    Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData;
    TopoDS_Wire      aWire  = TopoDS::Wire(Sh);
    Standard_Boolean begin  = Standard_True;
    Standard_Real    length = 0.0;
    Standard_Real    staPar = start->StartParameter();
    Standard_Real    endPar = start->EndParameter();
      
    for (TopoDS_Iterator Iter(aWire); Iter.More(); Iter.Next()) {
      TopoDS_Edge     anEdge = TopoDS::Edge(Iter.Value());
      TopLoc_Location aLoc;  
      Standard_Real   first, last;
      Crv = BRep_Tool::Curve(anEdge, aLoc, first, last);
      if ((length + last - first) <= staPar) continue;
      if (length >= endPar) {
	if (begin) {
	   Message_Msg Msg1105("IGES_1105"); //"Cannot build a ruled surface from these curves."
	   SendFail(start,Msg1105);
	  //AddFail(start, "Cannot build a ruled surface from these curves.");
	  return  res;
	}
	break;
      }
      OffCrv = new Geom_OffsetCurve(Crv,Offset,NrmDir);
      BRepBuilderAPI_MakeEdge  ME(OffCrv, staPar - length, endPar - length);
	
      if (!ME.IsDone()) {
	 Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
	 SendFail(start,Msg1005);
	//AddFail(start, "Edge construction error");
	return  res;
      }
      TopoDS_Edge  anotherEdge = ME.Edge();  
      anotherEdge.Move(aLoc);
      begin   = Standard_False;
      length += last - first;
      sewd->Add (anotherEdge);
    }
    Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
    sfw->Load (sewd);
    //pdn S4135 10.03.99
    //sfw.FixConnected (GetEpsGeom() * GetUnitFactor());
    sfw->FixConnected ();
    res = sfw->Wire();
  }
  
  if (start->HasTransf()) {
    gp_Trsf T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),start->CompoundLocation(),T,GetUnitFactor())) { 
      TopLoc_Location L(T);
      res.Move(L);
    }
    else {
      Message_Msg Msg1035("IGES_1035");
      SendWarning(start,Msg1035);
    }
      //AddWarning(start, "Transformation skipped (not a similarity)");
  }

  return  res;
}




//=======================================================================
//function : Transfer2dOffsetCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::Transfer2dOffsetCurve
  (const Handle(IGESGeom_OffsetCurve)&  start,
        const TopoDS_Face&          face,
	const gp_Trsf2d& trans,
        const Standard_Real uFact)

{
  TopoDS_Shape  res;
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"Offset Curve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //AddFail(start, "2D OffsetCurve Transfer Error : Null IGESEntity");
    return  res;
  }

  if (start->OffsetType() != 1) {
    Message_Msg Msg1100("IGES_1100"); //"Offset distance flag different from 1 not treated"
    SendFail(start,Msg1100);
  }
   // AddWarning(start, "Offset distance flag different from 1 not properly treated");

  Standard_Real                Offset = start->FirstOffsetDistance();
  Handle(IGESData_IGESEntity)  BaseCrv = start->BaseCurve();
  Handle(Geom2d_Curve)         Crv;
  Handle(Geom2d_OffsetCurve)   OffCrv;


  if (IGESToBRep::IsTopoCurve(BaseCrv)) {
    IGESToBRep_TopoCurve  TC(*this);
    TC.SetModeTransfer(Standard_False);
    TopoDS_Shape  Sh = TC.Transfer2dTopoCurve(BaseCrv, face, trans, uFact);
    if (Sh.IsNull() || !( (Sh.ShapeType()==TopAbs_EDGE) ||(Sh.ShapeType()==TopAbs_WIRE)) ) {
      Message_Msg Msg1156("IGES_1156"); //"Edge or wire expected from TransferTopoCurve"
      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(BaseCrv);
      Msg1156.Arg("2D basis curve");
      Msg1156.Arg(label);
      SendFail(start,Msg1156);
      return  res;
    }
    
    if (Sh.ShapeType()==TopAbs_EDGE) {
      Handle(Geom_Surface)  Srf;
      TopLoc_Location aLoc; 
      Standard_Real  a, b;
      BRep_Tool::CurveOnSurface(TopoDS::Edge(Sh), Crv, Srf, aLoc, a, b);
      OffCrv = new Geom2d_OffsetCurve(Crv,Offset * uFact);
      TopoDS_Edge  anEdge;
      ShapeBuild_Edge().MakeEdge (anEdge, OffCrv, face, start->StartParameter(), start->EndParameter());
      if (anEdge.IsNull()/*!ME.IsDone()*/) {
	Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
	SendFail(start,Msg1005);
	return  res;
      }
      res = anEdge;
    }
    else if (Sh.ShapeType()==TopAbs_WIRE) {
      TopoDS_Wire  aWire = TopoDS::Wire(Sh);
      Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData;
      for (TopoDS_Iterator Iter(aWire); Iter.More(); Iter.Next()) {
        TopoDS_Edge  anEdge = TopoDS::Edge(Iter.Value());
	Handle(Geom_Surface)  Srf;
	TopLoc_Location  aLoc;  
	Standard_Real  a, b;
	BRep_Tool::CurveOnSurface(anEdge, Crv, Srf, aLoc, a, b);
	OffCrv = new Geom2d_OffsetCurve(Crv,Offset * uFact); 
	TopoDS_Edge  anotherEdge;
	ShapeBuild_Edge().MakeEdge (anotherEdge, OffCrv, face, start->StartParameter(), start->EndParameter());
	if (anotherEdge.IsNull()/*!ME.IsDone()*/) {
	  Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
	  SendFail(start,Msg1005);
	  return  res;
	}
	sewd->Add (anotherEdge);
      }
      Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
      sfw->Load (sewd);
      //pdn 10.03.99
      //sfw.FixConnected (GetEpsGeom() * GetUnitFactor());
      sfw->FixConnected ();
      res = sfw->Wire();
    }
  }
  return  res;
}


//=======================================================================
//function : TransferTopoCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferTopoCurve
(const Handle(IGESData_IGESEntity)&  start)
{
  TopoDS_Shape  res;
  
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"TopoCurve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //AddFail(start, "TopoCurve Transfer Error : Null IGESEntity");
    return  res;
  }
  //S4054
  if (IGESToBRep::IsBasicCurve(start)) {
    res = TransferTopoBasicCurve(start);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_CompositeCurve))) {
    DeclareAndCast(IGESGeom_CompositeCurve, st102, start);
    res = TransferCompositeCurve(st102);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))) {
    DeclareAndCast(IGESGeom_CurveOnSurface, st142, start);
    res = TransferCurveOnSurface(st142);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_Boundary))) {
    DeclareAndCast(IGESGeom_Boundary, st141, start);
    res = TransferBoundary(st141);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_Point))) {
    DeclareAndCast(IGESGeom_Point, st116, start);
    res = TransferPoint(st116);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_OffsetCurve))) {
    DeclareAndCast(IGESGeom_OffsetCurve, st130, start);
    res = TransferOffsetCurve(st130);
  }
  else {
    // This message can not occur.
    //"Improper type provided to TransferTopoCurve"
  }  
  return  res;
}


//=======================================================================
//function : Transfer2dTopoCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::Transfer2dTopoCurve(const Handle(IGESData_IGESEntity)& start,
							const TopoDS_Face& face,
							const gp_Trsf2d& trans,
							const Standard_Real uFact)
{
  TopoDS_Shape  res;

  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"2d TopoCurve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //    AddFail(start, "2D TopoCurve Transfer Error : Null IGESEntity");
    return  res;
  }
  //S4054
  if (IGESToBRep::IsBasicCurve(start)) 
    res = Transfer2dTopoBasicCurve(start, face, trans, uFact);

  else if (start->IsKind(STANDARD_TYPE(IGESGeom_CompositeCurve))) {
    DeclareAndCast(IGESGeom_CompositeCurve, st102, start);
    res = Transfer2dCompositeCurve(st102, face, trans, uFact);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_Point))) {
    DeclareAndCast(IGESGeom_Point, st116, start);
    res = Transfer2dPoint(st116);
  }
  else if (start->IsKind(STANDARD_TYPE(IGESGeom_OffsetCurve))) {
    DeclareAndCast(IGESGeom_OffsetCurve, st130, start);
    res = Transfer2dOffsetCurve(st130, face, trans, uFact);
  }
  return  res;
}


//=======================================================================
//function : TransferTopoBasicCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferTopoBasicCurve
  (const Handle(IGESData_IGESEntity)&  start)
{
  TopoDS_Shape myshape;
  TopoDS_Edge myedge;
//  TopoDS_Vertex V1,V2;

  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"TopoBasicCurve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //AddFail(start, "TopoBasicCurve Transfer Error : Null IGESEntity");
    return  myshape;
  }

  IGESToBRep_BasicCurve BC(*this);
  // 14.05.2009 skl for OCC21131
  BC.SetModeTransfer(Standard_False);
  Handle(Geom_Curve) C = BC.TransferBasicCurve(start);

  if (C.IsNull()) {
    // A message has been thrown in TransferBasicCurve
    return myshape;
  }


// si la courbe est une BSpline de degre 1, et si l`utilisateur
// le souhaite, on approxime
  TheCurves.Clear();
  if ((C->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))&& GetModeApprox()) {
	Handle(Geom_BSplineCurve) BSplineC = Handle(Geom_BSplineCurve)::DownCast(C);
    if (BSplineC->Degree() == 1) 
      ApproxBSplineCurve(BSplineC);
    else
      TheCurves.Append(C);
  }
  else
    TheCurves.Append(C);

      
  // Si la courbe est une BSpline, il ne faut pas qu`elle soit C0.
  // sinon inutilisable dans les algos de topologie ...
  // on construit un wire avec des morceaux de courbes C1.
  Standard_Integer nbcurves = NbCurves();
  if ( nbcurves == 0 ) {
    Message_Msg Msg1156("IGES_1156"); //"TopoBasicCurve Transfer Error : Null Entity"
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(start);
    Msg1156.Arg("Geom_Curve");
    Msg1156.Arg(label);
    SendFail(start,Msg1156);
    return  myshape;
  }

//  Standard_Real epsgeom = GetEpsGeom()*GetUnitFactor();
  Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData;
  
  for (Standard_Integer icurve = 1; icurve <= nbcurves; icurve++) {
    Handle(Geom_Curve) mycurve = Curve(icurve);
    if ((mycurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
	&& (mycurve->Continuity() < GeomAbs_C1)
	&& GetContinuity() >= 1) {
      Handle(Geom_BSplineCurve) BSplineC = 
	Handle(Geom_BSplineCurve)::DownCast(mycurve);

      Handle(TColGeom_HSequenceOfBoundedCurve) seqBS;
      ShapeAlgo::AlgoContainer()->C0BSplineToSequenceOfC1BSplineCurve (BSplineC, seqBS);
      Standard_Integer NbC0 = seqBS->Length();
      for (Standard_Integer i = 1; i <= NbC0; i++) {
	BRepBuilderAPI_MakeEdge ME (seqBS->Value (i));
	if (!ME.IsDone()) {
	  Message_Msg Msg1005("IGES_1005");
	  SendFail(start,Msg1005);
	  //  AddFail(start, "Edge construction error");
	  return myshape;
	}
	myedge = ME.Edge();
	sewd->Add (myedge);
      }
    }
    else {
      Standard_Real a = mycurve->FirstParameter(), b = mycurve->LastParameter();
      if (mycurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
	Handle(Geom_TrimmedCurve) tmp = Handle(Geom_TrimmedCurve)::DownCast (mycurve);
	mycurve = tmp->BasisCurve();
      }
      BRepBuilderAPI_MakeEdge ME (mycurve, a, b);
      if (!ME.IsDone() || (Precision::IsInfinite(a) || Precision::IsInfinite(b))) {
	Message_Msg Msg1005("IGES_1005");
	SendFail(start,Msg1005);
	//AddFail(start, "Edge construction error");
	return  myshape;
      }
      myedge = ME.Edge();
      sewd->Add (myedge);
    } 
  }
  
  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load (sewd);
  //pdn 10.03.99 S4135 Using mintol
  //sfw.FixConnected (epsgeom);
  sfw->FixConnected ();
  myshape = sewd->Wire();
  
  //S4054 PRO11414-1.igs entities 56, 80 (self-intersection is hidden only when
  //tolerance of vertices is resolution of IGES file)
//  ShapeAnalysis::FindBounds (myshape, V1, V2);
//  BRep_Builder B;
//  B.UpdateVertex (V1, epsgeom);
//  B.UpdateVertex (V2, epsgeom);
  // 14.05.2009 skl for OCC21131
  // 15.03.2011 emv for OCC22294 begin
  Standard_Boolean bIsNeedTransf = start->IsKind(STANDARD_TYPE(IGESGeom_SplineCurve));
  if (start->HasTransf() && bIsNeedTransf) {
    gp_Trsf  T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
       (GetEpsilon(),start->CompoundLocation(),T, GetUnitFactor())) { 
      TopLoc_Location L(T);
      myshape.Move(L);
    }
    else {
      Message_Msg Msg1035("IGES_1035");
      SendWarning(start,Msg1035);
    }
    //AddWarning(start, "Transformation skipped (not a similarity)");
  }
  //15.03.2011 emv for OCC22294 end

  // debug mjm du 26/07/96 en attendant developpement meilleur
  // sur traitement des Wire et non des Edge dans les programmes appelant
  
  if (sewd->NbEdges() != 1) {
    //S4054    TheBadCase = Standard_True; //:27
    Message_Msg Msg1120("IGES_1120");// "Wire not always implemented."
    SendWarning(start,Msg1120);
    //    AddWarning(start, "Wire not always implemented.");
    return myshape;
  }
  else 
    myedge = TopoDS::Edge (TopoDS_Iterator (myshape).Value());

  //added by rln 23/12/97 CSR# UKI60155 entity 208 (CircularArc)
  //if Starting and Terminating point are the same this can be caused by either error in the file
  //(curve with null length) or normal situation (period 2*PI). It is better to look at
  // 2d representation
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc)) &&
      Handle(IGESGeom_CircularArc)::DownCast (start)->IsClosed())
    TheBadCase = Standard_True;

  return myedge;
}


//=======================================================================
//function : Transfer2dTopoBasicCurve
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::Transfer2dTopoBasicCurve
  (const Handle(IGESData_IGESEntity)& start,
		const TopoDS_Face&    face,
		const gp_Trsf2d&    trans,
                const Standard_Real uFact)
{
  TopoDS_Edge  edge, myedge;
  TopoDS_Shape myshape;

  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"2D TopoBasicCurve Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //    AddFail(start, "2D TopoBasicCurve Transfer Error : Null IGESEntity");
    return  edge;
  }
  
  TopLoc_Location L;
  Handle(Geom_Surface) mysurf = BRep_Tool::Surface(face, L);

  IGESToBRep_BasicCurve  BC(*this);
  BC.SetModeTransfer(Standard_False);
                                // The Trsf must be applied to the Curve2d.

  Handle(Geom2d_Curve)  C2d = BC.Transfer2dBasicCurve(start);
  if (C2d.IsNull()) {
    // A message has been thrown in Transfer2dBasicCurve
    //AddFail(start, "Conversion error");
    return edge;
  }

 
// si la courbe est une BSpline de degre 1, on approxime
  TheCurves2d.Clear();
  if ((C2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))&& GetModeApprox()){
    Handle(Geom2d_BSplineCurve) BSplineC2d = 
      Handle(Geom2d_BSplineCurve)::DownCast(C2d);
    if (BSplineC2d->Degree() == 1) 
      Approx2dBSplineCurve(BSplineC2d);
    else
      TheCurves2d.Append(C2d);
  }
  else
    TheCurves2d.Append(C2d);


  // Si la courbe est une BSpline, il ne faut pas qu`elle soit C0.
  // on construit un wire avec des morceaux de courbes C1.
  Standard_Integer nbcurves = NbCurves2d();
  if ( nbcurves == 0 ) {
    Message_Msg Msg1156("IGES_1156"); //"2dTopoBasicCurve Transfer Error : Null Entity"
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(start);
    Msg1156.Arg("Geom2d_Curve");
    Msg1156.Arg(label);
    SendFail(start,Msg1156);
    //AddFail(start, "2dTopoBasicCurve Transfer Error : Null Entity");
    return  myshape;
  }

//  Standard_Real epsgeom = GetEpsGeom()*GetUnitFactor();
  //S4054 Standard_Real epscoeff = GetEpsCoeff();
  Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData;

  for (Standard_Integer icurve = 1; icurve <= nbcurves; icurve++) {
    Handle(Geom2d_Curve) mycurve2d = Curve2d(icurve);
    //S4054 November 98 Transformation of pcurve in a single place
    //(taken from Compute2d3d and Compute2d)
/*    if (isrev) {
      //#30 rln 19.10.98 transformation of pcurves for IGES Surface of Revolution
      mycurve2d ->Translate (gp_Vec2d (0, -2 * M_PI));
      mycurve2d->Mirror (gp::OX2d());
      mycurve2d->Mirror (gp_Ax2d (gp::Origin2d(), gp_Dir2d (1.,1.)));
      
      gp_Trsf2d TR;
      TR.SetTranslation (gp_Pnt2d (0.,0.),gp_Pnt2d (-paramu,0.));
      mycurve2d->Transform (TR);
    }
    if (paramv != 0.) {
      gp_Trsf2d TR;
      TR.SetTranslation (gp_Pnt2d (0.,0.), gp_Pnt2d (0.,paramv));
      mycurve2d->Transform (TR);
    }*/
       
    if(trans.Form()!=gp_Identity)
      mycurve2d->Transform (trans);
    
    gp_Trsf2d ntrsf;
    if(mysurf->IsKind(STANDARD_TYPE(Geom_Plane))) 
      ntrsf.SetScale(gp_Pnt2d(0,0),GetUnitFactor());
    
    Standard_Real a = mycurve2d->FirstParameter(), b =  mycurve2d->LastParameter();
    ShapeBuild_Edge sbe;
    mycurve2d = sbe.TransformPCurve (mycurve2d, ntrsf, uFact, a, b);
    
    if ((mycurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) 
	&& (mycurve2d->Continuity() < GeomAbs_C1)
	&& GetContinuity() >= 1) {
      Handle(Geom2d_BSplineCurve) BSplineC2d = 
	Handle(Geom2d_BSplineCurve)::DownCast(mycurve2d);

      Handle(TColGeom2d_HSequenceOfBoundedCurve) seqBS;
      ShapeAlgo::AlgoContainer()->C0BSplineToSequenceOfC1BSplineCurve (BSplineC2d, seqBS);
      Standard_Integer NbC0 = seqBS->Length();
      for (Standard_Integer i = 1; i <= NbC0; i++) {
	ShapeBuild_Edge().MakeEdge (myedge, seqBS->Value (i), face);
//	BRepBuilderAPI_MakeEdge ME (seqBS->Value (i), mysurf);
	if (myedge.IsNull()/*!ME.IsDone()*/) {
	  Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
	  SendFail(start,Msg1005);
	  //AddFail(start, "Edge construction error");
	  return myshape;
	}
//	myedge = ME.Edge();
//	ShapeBuild_Edge().RemovePCurve (myedge, mysurf);
//	B.UpdateEdge (myedge, seqBS->Value (i), face, 0);
	sewd->Add (myedge);
      }
    }
    else {
      if (mycurve2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
	Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (mycurve2d);
	mycurve2d = tmp->BasisCurve();
      }
      ShapeBuild_Edge().MakeEdge (myedge, mycurve2d, face, a, b);
//      BRepBuilderAPI_MakeEdge ME (mycurve2d, mysurf);
      if (myedge.IsNull()/*!ME.IsDone()*/) {
	Message_Msg Msg1005("IGES_1005"); //"Edge construction error"
	SendFail(start,Msg1005);
	return myshape;
      }
      //      myedge = ME.Edge();
      //      ShapeBuild_Edge().RemovePCurve (myedge, mysurf);
      //      B.UpdateEdge (myedge, mycurve2d, face, 0);
      sewd->Add (myedge);
    }
  }
  
  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load (sewd);
  //pdn 10.03.99 S4135
  //sfw.FixConnected (epsgeom);
  sfw->FixConnected ();
  myshape = sewd->Wire();

//  TopoDS_Vertex V1,V2;
//  ShapeAnalysis::FindBounds (myshape, V1, V2);
//  B.UpdateVertex (V1, epsgeom);
//  B.UpdateVertex (V2, epsgeom);

  // debug mjm du 26/07/96 en attendant developpement meilleur
  // sur traitement des Wire et non des Edge dans les programmes appelant

  if (sewd->NbEdges() != 1) {
    //S4054 TheBadCase = Standard_True; //:27
    Message_Msg Msg1120("IGES_1120");// "Wire not always implemented."
    SendWarning(start,Msg1120);
//    AddWarning(start, "Wire not always implemented.");
    return myshape;
  }

  //the same modifications as in TransferBasicCurve()
  if (start->IsKind(STANDARD_TYPE(IGESGeom_CircularArc)) &&
      Handle(IGESGeom_CircularArc)::DownCast (start)->IsClosed())
    TheBadCase = Standard_True;

  return myedge;
}


//=======================================================================
//function : TransferBoundary
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoCurve::TransferBoundary
  (const Handle(IGESGeom_Boundary)& start)
{  
  TopoDS_Shape  res;
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"Boundary Transfer Error : Null IGESEntity"
    SendFail(start, Msg1005);
    //AddFail(start, "Boundary Transfer Error : Null IGESEntity");
    return  res;
  }

  if (start->BoundaryType()==0) {
    Message_Msg Msg1125("IGES_1125"); //"Model space representation not implemented"
    SendFail(start,Msg1125);
    //AddFail(start, "Model space representation not implemented");
    return res;
  }

  //  Transfer of the unbounded surface
  //  =================================

  Handle (IGESData_IGESEntity) igesSurface = start->Surface();
  if (igesSurface.IsNull() || !IGESToBRep::IsTopoSurface(igesSurface) ) {
    Message_Msg Msg124("XSTEP_124"); //"BasicSurface Transfer Error : Null IGESEntity"
    SendFail(start,Msg124);
    //    AddFail(start, "BasicSurface Transfer Error : Null IGESEntity");
    return res;
  }
 
  TopoDS_Face  face;
  TopAbs_ShapeEnum shapeEnum;
  IGESToBRep_TopoSurface TS(*this);
  gp_Trsf2d trans;
  Standard_Real uFact;
  TopoDS_Shape myshape = TS.ParamSurface(igesSurface, trans, uFact);

  if (!myshape.IsNull()) {
    shapeEnum = myshape.ShapeType();
    switch (shapeEnum) {
    case TopAbs_FACE :
      {
	face = TopoDS::Face(myshape);
	break;
      }
    case TopAbs_SHELL :
      {
	TopoDS_Iterator IT(myshape);
	Standard_Integer nbfaces = 0;
	for (; IT.More(); IT.Next()) {
	  nbfaces++;
	  face = TopoDS::Face(IT.Value());
	}
	//szv#4:S4163:12Mar99 optimized
	if (nbfaces != 1) {
	   Message_Msg Msg1061("IGES_1061"); //"Not Implemented Trimmed Composite Surface."
	    SendFail(start,Msg1061);
	  //AddFail(start,"Not Implemented Trimmed Composite Surface.");
	  return res;
	}
      }
      break;
    default:
      {
	Message_Msg Msg1156("IGES_1156"); //"Basis Surface Error." ?? Msg1066
	Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSurface);
	Msg1156.Arg("basic surface");
	Msg1156.Arg(label);
	SendFail(start,Msg1156);
	//	AddFail(start, "Basis Surface Error.");
	return res;
      }
    }
  }

  face.EmptyCopy();
  res = TransferBoundaryOnFace(face, start, trans, uFact);
  return res;
}



//=======================================================================
//function : TransferBoundaryOnFace
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoCurve::TransferBoundaryOnFace(TopoDS_Face&  face, 
							  const Handle(IGESGeom_Boundary)& start, 
							  const gp_Trsf2d& trans,
							  const Standard_Real uFact)
{
  TopoDS_Shape  res;  
  if (start.IsNull()) {
    Message_Msg Msg1005("IGES_1005"); //"BoundaryOnFace Transfer Error : Null IGESEntity"
    SendFail(start,Msg1005);
    return res;
  }

  Standard_Boolean okCurve = Standard_True, okCurve3d = Standard_True, okCurve2d = Standard_True;
  Standard_Integer filepreference = 0;
  if      (start->PreferenceType() == 2) filepreference = 2;
  else if (start->PreferenceType() == 1) filepreference = 3;
  Standard_Boolean Result = Standard_True;

  Handle(IGESToBRep_IGESBoundary) IB = IGESToBRep::AlgoContainer()->ToolContainer()->IGESBoundary();
  IB->Init (*this, start, face, trans, uFact, filepreference);
  for (Standard_Integer i = 1; i <= start->NbModelSpaceCurves(); i++) {
    Handle(IGESData_HArray1OfIGESEntity) Curves2d;
    if (start->NbParameterCurves(i) == 0 && start->BoundaryType() == 1) {
      Message_Msg Msg1135("IGES_1135");  
      Msg1135.Arg(2); 
      Msg1135.Arg(3); 
      SendWarning(start,Msg1135);
    } 
    else
      Curves2d = start->ParameterCurves(i);
    Result = Result & IB->Transfer (okCurve, okCurve3d, okCurve2d,
				    start->ModelSpaceCurve(i), start->Sense(i) == 2,
				    Curves2d, i);
  }
  IB->Check(Result, Standard_True, okCurve3d, okCurve2d);
  Handle(ShapeExtend_WireData) sewd = IB->WireData();
  if (sewd->NbEdges() == 0) {
     Message_Msg Msg1095("IGES_1095");//"Both 2d and 3d representations are invalid"
     SendFail(start,Msg1095);
    return res;
  }

  //#20 rln 14/05/98 buc40130 entity 16977
  //first wire should be outer, all other are inner
  //%14 pdn 03.03.99
  // IB.Fix (sewd, Standard_True, Standard_True, Standard_False, Standard_False, Standard_False);
  TopoDS_Wire mywire = sewd->Wire();

  if (start->HasTransf()) {
    gp_Trsf T;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(), start->CompoundLocation(),T)) { 
      TopLoc_Location L(T);
      mywire.Move(L);
    }
    else {
      Message_Msg Msg1035("IGES_1035"); //"Transformation skipped (not a similarity)"
      SendWarning(start,Msg1035);
    }
  }
  BRep_Builder B;
  B.Add(face,mywire);
  SetShapeResult (start, mywire);
  return mywire;
}

//=======================================================================
//function : ApproxBSplineCurve
//purpose  : 
//=======================================================================

void IGESToBRep_TopoCurve::ApproxBSplineCurve
       (const Handle(Geom_BSplineCurve)& start)
	 
{
  ShapeAlgo::AlgoContainer()->ApproxBSplineCurve (start, TheCurves);
}


//=======================================================================
//function : NbCurves
//purpose  : Returns the count of produced Curves
//=======================================================================
Standard_Integer  IGESToBRep_TopoCurve::NbCurves () const
{
  return TheCurves.Length();
}


//=======================================================================
//function : Curve
//purpose  : Returns a Curvee given its rank
//=======================================================================
Handle(Geom_Curve)  IGESToBRep_TopoCurve::Curve (const Standard_Integer num) const
{
  Handle(Geom_Curve) res;
  if (num > 0 && num <= TheCurves.Length()) res = TheCurves.Value(num);
  return res;
}


//=======================================================================
//function : Approx2dBSplineCurve
//purpose  : 
//=======================================================================

void IGESToBRep_TopoCurve::Approx2dBSplineCurve
       (const Handle(Geom2d_BSplineCurve)& start)
	 
{
  ShapeAlgo::AlgoContainer()->ApproxBSplineCurve (start, TheCurves2d);
}


//=======================================================================
//function : NbCurves2d
//purpose  : Returns the count of produced Curves
//=======================================================================
Standard_Integer  IGESToBRep_TopoCurve::NbCurves2d () const
{
  return TheCurves2d.Length();
}


//=======================================================================
//function : Curve2d
//purpose  : Returns a Curve given its rank
//=======================================================================
Handle(Geom2d_Curve)  IGESToBRep_TopoCurve::Curve2d (const Standard_Integer num) const
{
  Handle(Geom2d_Curve) res;
  if (num > 0 && num <= TheCurves2d.Length()) res = TheCurves2d.Value(num);
  return res;
}

//=======================================================================
//function : SetBadCase
//purpose  : 
//=======================================================================

 void IGESToBRep_TopoCurve::SetBadCase (const Standard_Boolean value)
{
  TheBadCase = value;
}

//=======================================================================
//function : BadCase
//purpose  : 
//=======================================================================

 Standard_Boolean IGESToBRep_TopoCurve::BadCase () const
{
  return TheBadCase;
}
