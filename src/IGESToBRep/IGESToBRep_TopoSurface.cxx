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
//modified: 
// Integration to ensure SCCS base integrity
// 21.12.98 rln, gka S4054
// 28.12.98 dce S3767 New messaging system
//#55,#56 rln 25.12.98 UKI60878
//:l1 abv 10.01.99: USA60022 7289: fix missing seam
//#63 rln 19.01.99 UKI60878 no offset if C0 surface is converted into the grid of C1 surfaces
//%13 pdn 15.02.99 USA60293 entities 792, 8604 .. handling of C0 ruled surfaces, tabulated cylindres,
//                 and surfaces of revolution.
//:p4 abv, pdn 23.02.99: PRO9234 #15720: call BRepTools::Update() for faces
//%14 pdn 24.02.99 implementing of ShapeFix_Face on IGES  
//    pdn 17.04.99 S4181: Implementing of reading IGES elementary surfaces.
//    pdn 10.05.99 S4137: Using modified ShapeDivide tools
//#11 smh 22.12.99 BUC60625 Transform axis.
//#12 smh 12.12.99 FRA62468 - Using conversion to B-Spline for Offset surface
//=======================================================================

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepFill.hxx>
#include <BRepGProp.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepTools.hxx>
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomConvert.hxx>
#include <GeomLib.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec.hxx>
#include <GProp.hxx>
#include <GProp_GProps.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESData_ToolLocation.hxx>
#include <IGESGeom_BoundedSurface.hxx>
#include <IGESGeom_BSplineSurface.hxx>
#include <IGESGeom_CircularArc.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_Direction.hxx>
#include <IGESGeom_Line.hxx>
#include <IGESGeom_OffsetSurface.hxx>
#include <IGESGeom_Plane.hxx>
#include <IGESGeom_Point.hxx>
#include <IGESGeom_RuledSurface.hxx>
#include <IGESGeom_SurfaceOfRevolution.hxx>
#include <IGESGeom_TabulatedCylinder.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <IGESSolid_ConicalSurface.hxx>
#include <IGESSolid_CylindricalSurface.hxx>
#include <IGESSolid_PlaneSurface.hxx>
#include <IGESSolid_SphericalSurface.hxx>
#include <IGESSolid_ToroidalSurface.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_BasicCurve.hxx>
#include <IGESToBRep_BasicSurface.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <IGESToBRep_TopoCurve.hxx>
#include <IGESToBRep_TopoSurface.hxx>
#include <Interface_Macros.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAlgo.hxx>
#include <ShapeAlgo_AlgoContainer.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <stdio.h>
//S4054: ShapeTool_MakeWire -> ShapeExtend_WireData //:g8: BRepLib_MakeWire -> ShapeTool_MakeWire
//:e3
//#16
//S4054
//S3767
//=======================================================================
//function : IGESToBRep_TopoSurface
//purpose  : 
//=======================================================================
IGESToBRep_TopoSurface::IGESToBRep_TopoSurface()
     :IGESToBRep_CurveAndSurface()
{  
}


//=======================================================================
//function : IGESToBRep_TopoSurface
//purpose  : 
//=======================================================================

IGESToBRep_TopoSurface::IGESToBRep_TopoSurface
  (const IGESToBRep_CurveAndSurface& CS)
     :IGESToBRep_CurveAndSurface(CS)
{  
}


//=======================================================================
//function : IGESToBRep_TopoSurface
//purpose  : 
//=======================================================================

IGESToBRep_TopoSurface::IGESToBRep_TopoSurface
  (const Standard_Real    eps,
   const Standard_Real    epsCoeff,
   const Standard_Real    epsGeom,
   const Standard_Boolean mode,
   const Standard_Boolean modeapprox,
   const Standard_Boolean optimized)
     :IGESToBRep_CurveAndSurface(eps, epsCoeff, epsGeom, mode, 
				 modeapprox, optimized)
{  
}

static Standard_Boolean extractCurve3d (const TopoDS_Shape& theEdges,
                                        Handle(Geom_Curve)& theCurve)
{
  TopExp_Explorer anExp(theEdges, TopAbs_EDGE);
  Standard_Integer howMuch = 0;
  Standard_Real f = 0., l = 0.;
  for (; anExp.More(); anExp.Next()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anExp.Current());
    if (anEdge.IsNull())
      continue;
    howMuch++;
    theCurve = BRep_Tool::Curve(anEdge, f, l);
  }
  if ( howMuch != 1 || theCurve.IsNull() )
    return Standard_False;
  
  if ( f != theCurve->FirstParameter() || l != theCurve->LastParameter() )
    theCurve = new Geom_TrimmedCurve ( theCurve, f, l );
  return Standard_True;
}


//=======================================================================
//function : TransferTopoSurface
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferTopoSurface
       (const Handle(IGESData_IGESEntity)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////

  TopoDS_Shape res;
  TheULength = 1.;
  //S4054
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
  }
  ////modified by jgv, 20.11.2009 for OCC21487///
  else if (HasShapeResult(st))
    {
      res = GetShapeResult(st);
      return res;
    }
  ///////////////////////////////////////////////
  else if (IGESToBRep::IsBasicSurface(st)) {
    res = TransferTopoBasicSurface(st);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_TrimmedSurface))) {
    DeclareAndCast(IGESGeom_TrimmedSurface, st144, st);
    res = TransferTrimmedSurface(st144);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_SurfaceOfRevolution))) {
    DeclareAndCast(IGESGeom_SurfaceOfRevolution, st120, st);
    res = TransferSurfaceOfRevolution(st120);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_TabulatedCylinder))) {
    DeclareAndCast(IGESGeom_TabulatedCylinder, st122, st);
    res = TransferTabulatedCylinder(st122);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_RuledSurface))) {
    DeclareAndCast(IGESGeom_RuledSurface, st118, st);
    res = TransferRuledSurface(st118);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_Plane))) {
    DeclareAndCast(IGESGeom_Plane, st108, st);
    res = TransferPlane(st108);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_BoundedSurface))) {
    DeclareAndCast(IGESGeom_BoundedSurface, st143, st);
    res = TransferBoundedSurface(st143);
  }
  else if (st->IsKind(STANDARD_TYPE(IGESGeom_OffsetSurface))) {
    DeclareAndCast(IGESGeom_OffsetSurface, st140, st);
    res = TransferOffsetSurface(st140);
  }
  //S4181 pdn IGESSolid_PlaneSurface recognized as basic surface
  else if (st->IsKind(STANDARD_TYPE(IGESBasic_SingleParent)))  {
    DeclareAndCast(IGESBasic_SingleParent,st402_9,st);
    res = TransferPerforate(st402_9);    // limite : Planes seulement
  }
  else {
   //  AddFail(st, "The IGESEntity is not a Topologic Surface.");
  }
  SetShapeResult (st, res);
  return res;
}

//=======================================================================
//function : TransferTopoBasicSurface
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferTopoBasicSurface
       (const Handle(IGESData_IGESEntity)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////

  TopoDS_Shape  res;

  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }
  if (!IGESToBRep::IsBasicSurface(st)) {
  //  AddFail(st, "BasicSurface Transfer Error : Not Allowed IGESEntity");  This message can not occur.
    return res;
  }

  IGESToBRep_BasicSurface BS(*this);

  Handle(Geom_Surface) surf = BS.TransferBasicSurface(st);
  if (surf.IsNull()) {
    // AddFail(st, "Surface Conversion Error"); Messages have ever been Added in the called function.
    return res;
  }

  //#9 rln 26/02/98 UKI60106
  if (surf->Continuity() < GeomAbs_C1) {
    Message_Msg msg1250("IGES_1250");
    SendWarning(st, msg1250);
  }
  if(surf->IsKind(STANDARD_TYPE(Geom_Plane))){
    BRep_Builder B;
    TopoDS_Face plane;
    B.MakeFace(plane);
    B.UpdateFace(plane, surf, TopLoc_Location(), Precision::Confusion());
    res = plane; 
  }
  else {
    BRepLib_MakeFace makeFace(surf, Precision::Confusion());
    res = makeFace.Face();
  }

  if (st->HasTransf()) {
    gp_Trsf trsf;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),st->CompoundLocation(),trsf,GetUnitFactor())) { 
      TopLoc_Location locFace(trsf);
      res.Move(locFace);
    }
    else {
      Message_Msg msg1035("IGES_1035");
      SendWarning(st, msg1035);
    } 
  }
  return res;
}


//=======================================================================
//function : TransferRuledSurface
//purpose  : 
//=======================================================================
static void reparamBSpline(Handle(Geom_Curve)& curve,
                           const Standard_Real First,
                           const Standard_Real Last)
{
  Handle (Geom_BSplineCurve) bscurve;
  if (!curve->IsKind (STANDARD_TYPE (Geom_BSplineCurve))) {
    if (curve->FirstParameter() < First || curve->LastParameter() > Last)
      curve = new Geom_TrimmedCurve (curve, First, Last);
    bscurve = GeomConvert::CurveToBSplineCurve (curve, Convert_RationalC1);
  }
  else {
    bscurve = Handle (Geom_BSplineCurve)::DownCast (curve);
    bscurve->Segment (First, Last);
  }
  
  if (bscurve.IsNull())
    return;
  
  TColStd_Array1OfReal Knots(1, bscurve->NbKnots());
  bscurve->Knots(Knots);
  BSplCLib::Reparametrize (0., 1., Knots);
  bscurve->SetKnots(Knots);
  curve = bscurve;
}

static void ReparamCurve(TopoDS_Edge& edge)
{
  TopLoc_Location L;
  Standard_Real First, Last;
  
  Handle (Geom_Curve) curve = Handle (Geom_Curve)::DownCast (BRep_Tool::Curve ( edge, L, First, Last )->Copy());
  //if ( Abs (First) <= Precision::PConfusion() && Abs (Last - 1.) <= Precision::PConfusion() ) return;
  if(!curve->IsKind(STANDARD_TYPE(Geom_Line))) return;
  
  reparamBSpline( curve, First, Last );
  
  BRep_Builder B;
  B.UpdateEdge ( edge, curve, L, Precision::Confusion() );
  B.Range ( edge, 0., 1 );
}


//=======================================================================
//function : TransferRuledSurface
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferRuledSurface
  (const Handle(IGESGeom_RuledSurface)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;

  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  IGESToBRep_TopoCurve TC(*this);
  //%13 pdn 12.02.99 
  TC.SetContinuity (0);
  Handle(IGESData_IGESEntity) igesCurve1 = st->FirstCurve();
  Handle(IGESData_IGESEntity) igesCurve2 = st->SecondCurve();

  if (igesCurve1.IsNull()) {
    Message_Msg msg148("XSTEP_148");
    SendFail(st, msg148); // Curve Reading Error : Null IGESEntity
    return res;
  }
  if (igesCurve2.IsNull()) {
    Message_Msg msg149("XSTEP_149");
    SendFail(st, msg149); // Curve Reading Error : Null IGESEntity
    return res;
  }

  Standard_Integer nbEdges1,   nbEdges2;
  TopoDS_Shape     shape1,     shape2;
  TopoDS_Wire      wire1,      wire2;
  TopoDS_Wire      newWire1,   newWire2;
  //TopoDS_Edge      edge1,      edge2; // skl

  if (IGESToBRep::IsTopoCurve(igesCurve1)) { 
    shape1 = TC.TransferTopoCurve(igesCurve1);
    if (shape1.IsNull()) {
      Message_Msg msg1156("IGES_1156");
      const Standard_CString typeName(igesCurve1->DynamicType()->Name());
      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesCurve1);
      msg1156.Arg(typeName);
      msg1156.Arg(label);
      SendFail(st, msg1156);
      return res;
    }

    //%13 pdn 15.02.99
    //added by rln on 03/12/97
    //if shape1 is a wire it means that the curve1 in file was of continuity C0
    //in order to get a face instead of shell when to BRepFill shape1
    //should be retransfered with contionuity C0 (to get an edge). Once shape1
    //has been built with C0, it is useless to require C1 from shape2 because
    //anyway resulting surface was of continuity C0. Thus shape2 is built with C0
//    if (shape1.ShapeType() != TopAbs_EDGE) {
//      TC.SetContinuity (0);
//      shape1  = TC.TransferTopoCurve(igesCurve1);
//      if (shape1.IsNull()) {
//	Message_Msg msg1156("IGES_1156");
//	const Standard_CString typeName(igesCurve1->DynamicType()->Name());
//	Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesCurve1);
//	msg1156.Arg(typeName);
//	msg1156.Arg(label);
//	SendFail(st, msg1156);  
//	return res;
//      }
//      else {  
//        Message_Msg msg1250("IGES_1250");
//        SendWarning (st, msg1250); // RuledSurface was built with continuity C0
//      }
//    }
    
    TopAbs_ShapeEnum shapeEnum1 = shape1.ShapeType();
    switch (shapeEnum1) {
    case TopAbs_EDGE :
      {
	TopoDS_Edge edge1 = TopoDS::Edge(shape1);
	ReparamCurve(edge1);
	nbEdges1 = 1;
      }
      break;
    case TopAbs_WIRE : 
      {
	wire1    = TopoDS::Wire(shape1);
	nbEdges1 = 0;
	for (TopoDS_Iterator hulot(wire1); hulot.More(); hulot.Next()) {
	 TopoDS_Edge edge1 = TopoDS::Edge(hulot.Value());
	 ReparamCurve(edge1);
	 nbEdges1++;
	}
      }
      break;
    default: 
      {
	// AddFail(st, "Curve Conversion Error."); This message can not occur.
	return res;
      }
      //break; //szv#4:S4163:12Mar99 unreachable
    }
  }
  else { 
    Message_Msg msg148("XSTEP_148");
    SendFail(st, msg148);
    // Curve Type not Allowed.
    return res;
  }
    
  if (IGESToBRep::IsTopoCurve(igesCurve2)) { 
    shape2 = TC.TransferTopoCurve(igesCurve2);
    // dirflg = 0 join first to first, last to last
    // dirflg = 1 join first to last, last to first
    
    if (shape2.IsNull()) {
      Message_Msg msg1156("IGES_1156");
      const Standard_CString typeName(igesCurve2->DynamicType()->Name());
      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesCurve2);
      msg1156.Arg(typeName);
      msg1156.Arg(label);
      SendFail(st, msg1156);
      // Curve Conversion Error.
      return res;
    }
   Standard_Integer dirflag = st->DirectionFlag ();
   // if (dirflag == 1){ // gka BUC60685
      
    //  shape2.Reverse();
    //}

    TopAbs_ShapeEnum shapeEnum2 = shape2.ShapeType();
    switch (shapeEnum2) {
    case TopAbs_EDGE : 
      {
	TopoDS_Edge edge2 = TopoDS::Edge(shape2);
	ReparamCurve(edge2);
	if (dirflag == 1)
	  shape2.Reverse();
	nbEdges2 = 1;
      }
      break;
    case TopAbs_WIRE :
      {
	wire2 = TopoDS::Wire(shape2);
	nbEdges2 = 0;
	for (TopoDS_Iterator cousto(wire2); cousto.More(); cousto.Next())  {
	  TopoDS_Edge edge2 = TopoDS::Edge(cousto.Value());
	  ReparamCurve(edge2);
	  nbEdges2++;
	}
	if (dirflag == 1) {   //gka BUC60685
	  Handle(ShapeExtend_WireData) sewd2 = new ShapeExtend_WireData;
	  sewd2->Add(shape2);
	  sewd2->Reverse();
	  wire2 = sewd2->Wire();
	}
      }
      break;
    default:
      { 
	// AddFail(st, "Curve Conversion Error.");
	return res;
      }
      //break; //szv#4:S4163:12Mar99 unreachable
    }
  }
  else { 
    Message_Msg msg149("XSTEP_149");
    SendFail(st, msg149);
    // Curve Type not Allowed
    return res;
  }
  

  if (nbEdges1 != nbEdges2) {
    if (nbEdges1 == 1) {
      Handle(ShapeExtend_WireData) sewd1 = new ShapeExtend_WireData;
      sewd1->Add(shape1);
      wire1  = sewd1->Wire();
    }
    else if (nbEdges2 == 1) {
      Handle(ShapeExtend_WireData) sewd2 = new ShapeExtend_WireData;
      sewd2->Add(shape2);
      wire2  = sewd2->Wire();
    }

    if (!st->IsRuledByParameter()) {
      // AddWarning (st,"Compute by parametric constant ratio");
    }
    if (!ShapeAlgo::AlgoContainer()->HomoWires
	(wire1, wire2, newWire1, newWire2, st->IsRuledByParameter())) {
      Message_Msg msg1255("IGES_1255");// "Ruled Surface Construction Error");
      SendFail(st, msg1255);
      return res;
    }
    nbEdges1 = 2;           // a number > 1
  }

  else if (nbEdges1 != 1) {
    newWire1 = wire1;
    newWire2 = wire2;
  }


  if (nbEdges1 == 1) {
    
    //:e3 abv 31 Mar 98: UK4.igs 3170: ruled surface with directixes - line
    // In IGES, line is parametrised [0;1] - this should have been kept !
    // Let us detect the case and remake curve as bspline [0;1]
    for ( Standard_Integer i=1; i <=2; i++ ) {
      //#43 rln 20.11.98 S4054 BUC50047 entity D463 (circles as generatrices [0, 2*PI])
      //reparameterisation should be for all curves not with range [0, 1] (see IGES)
      TopoDS_Edge edge = TopoDS::Edge ( i==1 ? shape1 : shape2 );
      //ReparamCurve(edge);
      TopLoc_Location L;
      Standard_Real First, Last;
      Handle (Geom_Curve) curve = Handle (Geom_Curve)::DownCast (BRep_Tool::Curve ( edge, L, First, Last )->Copy());
      if ( Abs (First) <= Precision::PConfusion() && Abs (Last - 1.) <= Precision::PConfusion() ) continue;
      
      Handle (Geom_BSplineCurve) bscurve;
      if (!curve->IsKind (STANDARD_TYPE (Geom_BSplineCurve))) {
	if (curve->FirstParameter() < First || curve->LastParameter() > Last)
	  curve = new Geom_TrimmedCurve (curve, First, Last);
	bscurve = GeomConvert::CurveToBSplineCurve (curve, Convert_RationalC1);
      }
      else {
	bscurve = Handle (Geom_BSplineCurve)::DownCast (curve);
	bscurve->Segment (First, Last);
      }
      TColStd_Array1OfReal Knots(1, bscurve->NbKnots());
      bscurve->Knots(Knots);
      BSplCLib::Reparametrize (0., 1., Knots);
      bscurve->SetKnots(Knots);
      
      BRep_Builder B;
      B.UpdateEdge ( edge, bscurve, L, Precision::Confusion() );
      B.Range ( edge, 0., 1 );
      if ( i ==1 ) shape1 = edge;
      else shape2 = edge;
    }
    
    res = BRepFill::Face(TopoDS::Edge(shape1), TopoDS::Edge(shape2));
    Handle(Geom_Surface) surf = BRep_Tool::Surface(TopoDS::Face(res));
    if(surf->Continuity()==GeomAbs_C0) {
      Message_Msg msg1250("IGES_1250");
      SendWarning (st, msg1250);
    }
  }
  else {
    res = BRepFill::Shell(newWire1, newWire2);
  }
  if (res.IsNull()) {
    Message_Msg msg1255("IGES_1255");// "Ruled Surface Construction Error");
    SendFail(st, msg1255);
    return res;
  }


  if (st->HasTransf()) {
    gp_Trsf trsf;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),st->CompoundLocation(), trsf,GetUnitFactor())) { 
      TopLoc_Location shapeLoc(trsf);
      res.Move(shapeLoc);
    }
    else {
      Message_Msg msg1035("IGES_1035");
      SendWarning(st,msg1035); // Transformation : not a similarity
    }
  }
  return res;
}


//=======================================================================
//function : TransferSurfaceOfRevolution
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferSurfaceOfRevolution
  (const Handle(IGESGeom_SurfaceOfRevolution)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  IGESToBRep_TopoCurve  TC(*this);
  IGESToBRep_BasicCurve BC(*this);
  Handle(IGESData_IGESEntity) igesGeneratrix = st->Generatrix();
  Handle(IGESGeom_Line)       igesAxis       = st->AxisOfRevolution();

  if (igesGeneratrix.IsNull() || !IGESToBRep::IsTopoCurve(igesGeneratrix) ) {
    Message_Msg msg153("XSTEP_153");
    SendFail(st, msg153);
    // Generatrix Reading Error : Null IGESEntity
    // Generatrix : Not Allowed IGESEntity.
    return res;
  }
  
  DeclareAndCast(IGESGeom_Line,srgen,st->Generatrix());
  if (!srgen.IsNull()) {
    gp_Pnt gen1 = srgen->StartPoint();
    gp_Pnt gen2 = srgen->EndPoint();
    TheULength = gen1.Distance(gen2)*GetUnitFactor();
  }

  if (igesAxis.IsNull()) {
    Message_Msg msg152("XSTEP_152");
    SendFail(st, msg152);
    return res;
  }

  //%13 pdn 15.02.99 
  TC.SetContinuity(0);
  TopoDS_Shape generatrix  = TC.TransferTopoCurve(igesGeneratrix);
  if (generatrix.IsNull()) {
    Message_Msg msg1156("IGES_1156");
    const Standard_CString typeName("generatrix");
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesGeneratrix);
    msg1156.Arg(typeName);
    msg1156.Arg(label);
    SendFail(st, msg1156);
    // Generatrix Conversion Error.
    return res;
  }

  gp_Trsf startLoc;
  gp_Pnt pt1 = igesAxis->TransformedStartPoint(); //smh#11
  gp_Pnt pt2 = igesAxis->TransformedEndPoint(); //smh#11
  pt1.Scale(gp_Pnt(0,0,0),GetUnitFactor());
  pt2.Scale(gp_Pnt(0,0,0),GetUnitFactor());
  //#30 rln 19.10.98 To keep IGES surface normal CAS.CADE axis = reversed IGES axis
  //CAS.CADE SA = 2*PI - IGES TA
  //CAS.CADE TA = 2*PI - IGES SA
  //gp_Ax1 revolAxis(pt1, gp_Dir(gp_Vec(pt1, pt2)));
  //Standard_Real startAngle = st->StartAngle();
  //Standard_Real endAngle = st->EndAngle();
  gp_Ax1 revolAxis(pt1, gp_Dir( gp_Vec (pt2, pt1)));
  Standard_Real startAngle = 2 * M_PI - st->EndAngle();
  Standard_Real endAngle = 2 * M_PI - st->StartAngle();
  Standard_Real deltaAngle = endAngle - startAngle;
  Standard_Boolean IsFullAngle = ( deltaAngle > 2.*M_PI-Precision::PConfusion() );
  if (IsFullAngle) deltaAngle = 2.*M_PI;  // ** CKY 18-SEP-1996
  // il faudra translater les courbes 2d de startAngle pour 
  // etre en phase IGES et BRep
  startLoc.SetRotation(revolAxis, startAngle);
  generatrix.Move(startLoc);
  
  // PTV file D44-11325-6.igs OCC660 depends on OCC450
  // PTV 29.05.2002 OCC450 create Surface of Revolution by native API
  // file NIC_file5.igs 
  // (BRepPrimAPI_MakeRevol replace surface of revolution by plane then 3D and 2D curves are inconsistent;
  // and 3D is ignored. As result shape is rectangle instead circle shape.
  Handle(Geom_Curve) aBasisCurve;
  
  {
    try
    {
      OCC_CATCH_SIGNALS
      if (extractCurve3d(generatrix, aBasisCurve))
      {
        BRepBuilderAPI_MakeFace aMakeF;
        Handle(Geom_Surface) aResultSurf = 
                new Geom_SurfaceOfRevolution(aBasisCurve, revolAxis);

        if ( !aResultSurf.IsNull())
        {
          if (!IsFullAngle)
          {
            const Standard_Real VF = aBasisCurve->FirstParameter();
            const Standard_Real VL = aBasisCurve->LastParameter();
            
            // PTV 29.08.2002  begin of OCC663 Trim surface by correct parameters
            const Standard_Real UF = 0;
            const Standard_Real UL = endAngle - startAngle;
            // PTV 29.08.2002  end of OCC663

            aMakeF.Init(aResultSurf, UF, UL, VF, VL, Precision::Confusion());
          }//if (!IsFullAngle)
          else
          {
            aMakeF.Init(aResultSurf, Standard_True, Precision::Confusion());
          }

          if (aMakeF.IsDone())
            res = aMakeF.Face();
        }//if ( !aResultSurf.IsNull())
      }//if (extractCurve3d(generatrix, aBasisCurve))
    }
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: IgesToBRep_TopoSurface::"
                    "TransferSurfaceOfRevolution(): exception by Geom: ";
      anException.Print ( cout ); cout << endl;
#endif
      (void)anException;
    }//catch (Standard_Failure)
  }
  
  if ( res.IsNull() ) {
    // do as usual.
  
    BRepPrimAPI_MakeRevol revol(generatrix, revolAxis, deltaAngle);
//mjm: si debug IsDone()est fait : 
//  if (!revol.IsDone()) {
//    AddFail(st, "Revol Construction Error.");
//    return res;
//  }
    res = revol.Shape();
  }
  //%13 pdn 15.02.99
  if (res.ShapeType() == TopAbs_FACE) {
    Handle(Geom_Surface) surf = BRep_Tool::Surface(TopoDS::Face(res));
    if(surf->Continuity()==GeomAbs_C0) {
      Message_Msg msg1250("IGES_1250");
      SendWarning (st, msg1250);
    } 
  }

  if (st->HasTransf()) {
    gp_Trsf trsf;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(), st->CompoundLocation(), trsf, GetUnitFactor())) { 
      TopLoc_Location shapeLoc(trsf);
      res.Move(shapeLoc);
    }
    else {
      Message_Msg msg1035("IGES_1035");
      SendWarning(st,msg1035); // Transformation : not a similarity
    }
  }
    
  return res;
}


//=======================================================================
//function : TransferTabulatedCylinder
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferTabulatedCylinder
       (const Handle(IGESGeom_TabulatedCylinder)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  IGESToBRep_TopoCurve TC(*this);
//  TopoDS_Edge  firstEdge;//commented by rln on 02/12/97

  Handle(IGESData_IGESEntity) igesDirectrix = st->Directrix();
  if (igesDirectrix.IsNull() || !IGESToBRep::IsTopoCurve(igesDirectrix) ) {
    Message_Msg msg153("XSTEP_153");
    SendFail(st, msg153); 
    // Directrix Reading Error : Null IGESEntity
    //Directrix, not allowed IGESEntity
    return res;
  }

  //%13 pdn 15.02.99
  TC.SetContinuity(0);
  TopoDS_Shape directrix = TC.TransferTopoCurve(igesDirectrix);
  if (directrix.IsNull()) {
    Message_Msg msg1156("IGES_1156");
    const Standard_CString typeName("directrix");
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesDirectrix);
    msg1156.Arg(typeName);
    msg1156.Arg(label);
    SendFail(st, msg1156);
    // Directrix Conversion Error.
    return res;
  }

  //modified by rln on 03/12/97
  //TopoDS_Vertex firstVertex = TopExp::FirstVertex(firstEdge);
  TopoDS_Vertex firstVertex, lastVertex;
  ShapeAnalysis::FindBounds (directrix, firstVertex, lastVertex);
  gp_Pnt pt1  = BRep_Tool::Pnt(firstVertex);
  gp_Pnt pt2  = st->EndPoint();
  pt2.Scale(gp_Pnt(0,0,0),GetUnitFactor());

  TheULength = pt1.Distance(pt2);
  if(TheULength < Precision::Confusion()) {
    Message_Msg msg("Tabulated cylinder with zero length");
    SendFail (st, msg); // TabulatedCylinder was built with continuity C0
    return res;
  }

  // PTV file D44-11325-6.igs OCC660 depends on OCC450
  // PTV 29.05.2002 OCC450 create Surface of LinearExtrusion by native API
  // see description about problem in Surface of Revolution
  Handle(Geom_Curve) aBasisCurve;
  {
  try {
    OCC_CATCH_SIGNALS
    if (extractCurve3d(directrix, aBasisCurve)) {
      gp_Vec dir (pt1, pt2);
      Handle(Geom_Surface) aResultSurf = 
        new Geom_SurfaceOfLinearExtrusion(aBasisCurve, dir);
      if (!aResultSurf.IsNull()) {
        //aResultSurf = 
        //  new Geom_RectangularTrimmedSurface(aResultSurf, 
        //                                     aBasisCurve->FirstParameter(),
        //                                     aBasisCurve->LastParameter(),
        //                                     0., dir.Magnitude() );
        BRepBuilderAPI_MakeFace aMakeF(aResultSurf, aBasisCurve->FirstParameter(),
                                             aBasisCurve->LastParameter(),
                                             0., dir.Magnitude(),
                                             Precision::Confusion());
        if (aMakeF.IsDone())
          res = aMakeF.Face();
      }
    }
  }
  catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout << "Warning: IgesToBRep_TopoSurface::TransferTabulatedCylinder(): exception by Geom: ";
    anException.Print ( cout ); cout << endl;
#endif
    (void)anException;
  }
  }
  
  if ( res.IsNull() ) {
    // do as usual.
    BRepPrimAPI_MakePrism prism(directrix, gp_Vec(pt1, pt2));
//mjm: si debug IsDone() est fait
//  if (!prism.IsDone()) {
//    AddFail(st, "Prism Construction Error.");
//    return res;
//  }
    res = prism.Shape();
  }
  //#16 rln 08/04/98 coq-inf-support.igs entity 2105
  //CAS.CADE can parametrize SurfaceOfLinearExtrusion with generatrix opposite to Vec(pt1, pt2)
  //and with parametrization V > 0, while in IGES TabulatedCylinder is parametrized with positive V
  //direction exactly in the direction Vec(pt1, pt2)
  if (res.ShapeType() == TopAbs_FACE) {
    Standard_Real UMin, UMax, VMin, VMax;
    BRepTools::UVBounds (TopoDS::Face (res), UMin, UMax, VMin, VMax);
    if (VMax <= Precision::PConfusion() && VMin < -Precision::PConfusion()) {
      TheULength *= -1;
      res.Reverse();
    }
    Handle(Geom_Surface) surf = BRep_Tool::Surface(TopoDS::Face(res));
    if(surf->Continuity()==GeomAbs_C0) {
      Message_Msg msg1250("IGES_1250");
      SendWarning (st, msg1250);
    }
  }
    
  if (st->HasTransf()) {
    gp_Trsf trsf;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),st->CompoundLocation(), trsf, GetUnitFactor())) { 
      TopLoc_Location shapeLoc(trsf);
      res.Move(shapeLoc);
    }
    else {
      Message_Msg msg1035("IGES_1035");
      SendWarning(st,msg1035); // Transformation : not a similarity
    }
  }
  return res;
}


//=======================================================================
//function : TransferOffsetSurface
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferOffsetSurface
  (const Handle(IGESGeom_OffsetSurface)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape    res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  TopoDS_Shape    igesShape;
  TopoDS_Face     face;
  TopLoc_Location basisLoc;

  Handle (IGESData_IGESEntity) igesSrf = st->Surface();
  if (igesSrf.IsNull() || !IGESToBRep::IsTopoSurface(igesSrf) ) {
    Message_Msg msg164("XSTEP_164");
    SendFail(st, msg164); 
    // Basis Surface Reading Error : Null IGESEntity
    // Basis Surface Transfer Error : Not Allowed IGESEntity 
    return res;
  }

  igesShape = TransferTopoSurface(igesSrf);
  if (igesShape.IsNull()) {
    Message_Msg msg1156("IGES_1156");
    const Standard_CString typeName("basis surface");
    Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSrf);
    msg1156.Arg(typeName);
    msg1156.Arg(label);
    SendFail(st, msg1156);  // Basis Surface Conversion Error.
    return res;
  }
  
  TopAbs_ShapeEnum shapeEnum = igesShape.ShapeType();
  switch (shapeEnum) {
  case TopAbs_FACE :
    {
      face = TopoDS::Face(igesShape);
      break;
    }
  case TopAbs_SHELL :
    {
      SendWarning(st, "The First Surface only will be transfered.");
      TopoDS_Iterator dabovil(igesShape);
      if (dabovil.More()) {
	face = TopoDS::Face(dabovil.Value());
	break;
      }
      /* else  AddF("... */
    }
  default:
    {
      Message_Msg msg1156("IGES_1156");
      const Standard_CString typeName("basis surface");
      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSrf);
      msg1156.Arg(typeName);
      msg1156.Arg(label);
      SendFail(st, msg1156);  // Basis Surface Conversion Error.
      return res;
    }
  }
  
  
  //Handle(Geom_Surface) geomSupport = BRep_Tool::Surface(face, basisLoc);
  // attention on ne peut construire une Geom_OffsetSurface que
  // si la surface de base est au moins C1, sinon on plante !
  //#56 rln 25.12.98 UKI60878 entity D593 (Offset surface on C0 B-Spline)
  //Trying to eliminate previous limitation on processing only C1 surfaces
  Handle(Geom_Surface) geomSupport = BRep_Tool::Surface(face);
  Handle(Geom_OffsetSurface) basisSrf;
  
  if (geomSupport->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    DeclareAndCast(Geom_OffsetSurface, geom140, geomSupport);
    geom140->SetOffsetValue(basisSrf->Offset() + 
 			    st->Distance()*GetUnitFactor());
    basisSrf = geom140;
  }
  else {
    if (geomSupport->Continuity() == GeomAbs_C0) {
      res = ShapeAlgo::AlgoContainer()->C0ShapeToC1Shape (face, Abs (st->Distance()) * GetUnitFactor());
      if(res.ShapeType()!=TopAbs_FACE) {
	Message_Msg msg1266("IGES_1266");
	SendFail(st, msg1266);//Basis surface is C0-continuous and cannot be corrected to C1-continuous.
	return res;
      }
      else {
	geomSupport = BRep_Tool::Surface (TopoDS::Face(res));
	if (geomSupport->Continuity() == GeomAbs_C0) {
	  Message_Msg msg1266("IGES_1266");
	  SendFail(st, msg1266);//Basis surface is C0-continuous and cannot be corrected to C1-continuous.
	  res.Nullify();
	  return res;
	}
      }
      Message_Msg msg1267("IGES_1267");
      SendWarning(st, msg1267);//Basis surface is C0-continuous but was corrected to C1-continuous
    }
    //smh#12
    if (res.IsNull()) res = face;
    geomSupport = BRep_Tool::Surface (TopoDS::Face(res));
    Standard_Real umin, umax, vmin, vmax;
    geomSupport->Bounds (umin, umax, vmin, vmax);
    if (Precision::IsInfinite (umin) || Precision::IsInfinite (umax) ||
	Precision::IsInfinite (vmin) || Precision::IsInfinite (vmax)) {
      // convert to C1 B-Spline
      BRepTools::UVBounds (face, umin, umax, vmin, vmax);
      Handle(Geom_RectangularTrimmedSurface) TS = new Geom_RectangularTrimmedSurface (geomSupport, umin, umax, vmin, vmax);
      Handle (Geom_BSplineSurface) BS = ShapeAlgo::AlgoContainer()->ConvertSurfaceToBSpline(TS, umin, umax, vmin, vmax);
      if (BS.IsNull() || BS->Continuity() == GeomAbs_C0) {
	Message_Msg msg1265("IGES_1265");
	SendFail(st, msg1265); // OffsetSurface Construction Error.
	return res;
      }
      else {
	geomSupport = BS;
      }
    }
    basisSrf = new Geom_OffsetSurface(geomSupport, st->Distance()*GetUnitFactor());
  }
  
  BRepLib_MakeFace MF(basisSrf, Precision::Confusion());
  if(!MF.IsDone()) {
    Message_Msg msg1265("IGES_1265");
    SendFail(st, msg1265); // OffsetSurface Construction Error.
    return res;
  }

  res = MF.Face();
  
  if (st->HasTransf()) {
    gp_Trsf trsf;
    SetEpsilon(1.E-04);
    if (IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(),st->CompoundLocation(),trsf, GetUnitFactor())) { 
      TopLoc_Location loc2(trsf);
      res.Move(loc2);
    }
    else {
      Message_Msg msg1035("IGES_1035");
      SendWarning(st,msg1035); // Transformation : not a similarity
    }
  }
  return res;
}


//=======================================================================
//function : TransferTrimmedSurface
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferTrimmedSurface
       (const Handle(IGESGeom_TrimmedSurface)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }
  
  TopAbs_ShapeEnum shapeEnum;
  IGESToBRep_TopoCurve TC(*this);

  Handle (IGESData_IGESEntity) igesSurface = st->Surface();
  if (igesSurface.IsNull() || !IGESToBRep::IsTopoSurface(igesSurface) ) {
    Message_Msg msg169("XSTEP_169");   
    SendFail(st, msg169);
    // BasicSurface Transfer Error : Null IGESEntity
    // Basis Surface, not Allowed IGESEntity.
    return res;
  }
  gp_Trsf2d trans;
  Standard_Real uFact;
  TopoDS_Face  face, faceres;
  
  TopoDS_Shape myshape = ParamSurface(igesSurface, trans, uFact);

  if (!myshape.IsNull()) {
    shapeEnum = myshape.ShapeType();
    switch (shapeEnum) {
    case TopAbs_FACE :
      {
	face = TopoDS::Face(myshape);
	faceres = face;
	break;
      }
    case TopAbs_SHELL :
      {
	TopoDS_Iterator IT(myshape);
	Standard_Integer nbfaces = 0;
	for (; IT.More(); IT.Next()) {
	  nbfaces++;
	  face = TopoDS::Face(IT.Value());
	  faceres = face;
	}
	//szv#4:S4163:12Mar99 optimized
	if (nbfaces != 1) {
	  Message_Msg msg1156("IGES_1156");
	  const Standard_CString typeName("basis surface");
          Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSurface);
	  msg1156.Arg(typeName);
	  msg1156.Arg(label);
	  SendFail(st, msg1156);  // Not Implemented Trimmed Composite Surface.
	  return myshape;
	}
      }
      break;
    default:
      {
	Message_Msg msg1156("IGES_1156");
	const Standard_CString typeName("basis surface");
	Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSurface);
	msg1156.Arg(typeName);
	msg1156.Arg(label);
	SendFail(st, msg1156);  // Basis Surface Conversion Error.
	return res;
      }
    }
  }
  else {
    return res;
  }
  
  //obtaining a surface
  TopLoc_Location L;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(face, L);
  TC.SetSurface(aSurf);
  
  if (st->HasOuterContour()) {
    face.EmptyCopy();
    TopoDS_Shape myshape1 = TC.TransferCurveOnFace (face, st->OuterContour(), trans, uFact, Standard_False);
    // si ca se passe mal , on recupere au moins la face avec NaturalRestriction
    if (myshape1 .IsNull()) {
      face = faceres;
      BRep_Builder B;
      B.NaturalRestriction(face,Standard_False);
    }
  }
  for (Standard_Integer i = 1; i <= st->NbInnerContours(); i++) {
    TopoDS_Shape myshape2 = TC.TransferCurveOnFace (face, st->InnerContour(i), trans, uFact, Standard_False);
  }
  BRepTools::Update ( face ); //:p4
  //%16 pdn 08.04.99
  return face;
}


//=======================================================================
//function : TransferBoundedSurface
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoSurface::TransferBoundedSurface
  (const Handle(IGESGeom_BoundedSurface)&  st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  if (st->RepresentationType()==0) {
    Message_Msg msg1275("IGES_1275");
    SendWarning(st, msg1275);
    // Model Space Representation Not Implemented : the result will be the basis surface
  }
  
  TopAbs_ShapeEnum shapeEnum;
  IGESToBRep_TopoCurve TC(*this);
  Handle (IGESData_IGESEntity) igesSrf = st->Surface();
  if (igesSrf.IsNull() || !IGESToBRep::IsTopoSurface(igesSrf) ) {
    Message_Msg msg166("XSTEP_166");
    SendFail( st, msg166);
    // Basis Surface Transfer Error : Null IGESEntity.
    // Basis Surface Transfer Error : Not Allowed IGESEntity.
    return res;
  }
  gp_Trsf2d trans;
  Standard_Real uFact;
  TopoDS_Face  face;

  TopoDS_Shape myshape = ParamSurface(igesSrf, trans, uFact);

  if (myshape.IsNull()) {
    //#55 rln 24.12.98 UKI60878 entity D593
#ifdef OCCT_DEBUG
    cout << "Fail: IGESToBRep_TopoSurface::TransferBoundedSurface  UntrimmedSurface is translated into Null" << endl;
#endif
    return res;
  }
  else {
    shapeEnum = myshape.ShapeType();
    switch (shapeEnum) {
    case TopAbs_FACE :
      {
	face = TopoDS::Face(myshape);
      }
      break;
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
	  Message_Msg msg1156("IGES_1156");
	  const Standard_CString typeName("basis surface");
	  Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSrf);
	  msg1156.Arg(typeName);
	  msg1156.Arg(label);
	  SendFail(st, msg1156);
	  // Not Implemented Trimmed Composite Surface.
	  return myshape;
	}
      }
      break;
    default:
      { 
	Message_Msg msg1156("IGES_1156");
	const Standard_CString typeName("basis surface");
	Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(igesSrf);
	msg1156.Arg(typeName);
	msg1156.Arg(label);
	SendFail(st, msg1156);
	return res;
      }
    }
  }
  
  face.EmptyCopy();
  for (Standard_Integer i = 1; i <= st->NbBoundaries(); i++) 
    TC.TransferBoundaryOnFace(face, st->Boundary(i), trans, uFact);
  
  BRepTools::Update ( face ); //:p4
  //#22 rln 01.06.98 UK3.igs entity 1279
//  ShapeFix_Face sff ( face );
//  sff.FixMissingSeam(); //:l1 abv 10 Jan 99: USA60022 7289: fix missing seam
//  if(sff.FixSmallAreaWire()) { //%14 pdn 24.02,99: USA60293: fix small area wires.
//    AddFail(st, "Small area wire detected, dropped"); 
//  }
//  sff.FixOrientation();
//  face = sff.Face();
  //%16 pdn 08.04.99
  return face;
}


//=======================================================================
//function : TransferPlane
//purpose  : 
//=======================================================================

TopoDS_Shape IGESToBRep_TopoSurface::TransferPlane
  (const Handle(IGESGeom_Plane)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape     res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  gp_Pln  pln;
  gp_Trsf trsf;
  res = TransferPlaneParts (st, pln,trsf,Standard_True);
//   res contient (en principe ...) une Face avec eventuellement un Wire
//   il reste a la mettre en position
  if (trsf.Form() != gp_Identity) {
    TopLoc_Location loc(trsf);
    res.Location(loc);
  }
  return res;
}


//=======================================================================
//function : TransferPlaneSurface
//purpose  : this function transferred into IGESToBRep_BasicSurface
//=======================================================================


//=======================================================================
//function : TransferPerforate
//purpose  : 
//=======================================================================

TopoDS_Shape  IGESToBRep_TopoSurface::TransferPerforate
  (const Handle(IGESBasic_SingleParent)& st)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape     res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  //char mess[100];
  gp_Pln  pln;
  gp_Trsf trsf;
  DeclareAndCast(IGESGeom_Plane,p0,st->SingleParent());
  BRep_Builder B;
  if (p0.IsNull()) {
    Message_Msg msg206("XSTEP_206");   
    SendFail(st, msg206);
    // SingleParent does not describe a holed face
    return res;
  }
  res = TransferPlaneParts (p0,pln,trsf,Standard_True);
//res demarre avec la face et son contour externe
  Standard_Integer nb = st->NbChildren();
  for (Standard_Integer i = 1; i <= nb; i ++) {
    DeclareAndCast(IGESGeom_Plane,pi,st->Child(i));
    if (pi.IsNull()) {
      Message_Msg msg1285("IGES_1285");
      msg1285.Arg(i);
      // A child is not a Plane, skipped, n0 %d
      SendWarning(st,msg1285);
      continue;
    }
    gp_Pln  pli;
    gp_Trsf trsi;
    TopoDS_Shape wire = TransferPlaneParts (pi,pli,trsi,Standard_False);
//    si ce n est pas un Wire, sauter
    if (wire.ShapeType() != TopAbs_WIRE) {
      Message_Msg msg1156("IGES_1156");
      const Standard_CString typeName("hole");
      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(pi);
      msg1156.Arg(typeName);
      msg1156.Arg(label);
      SendWarning(st, msg1156);
      // A hole could not be transferred, skipped, n0 %d
      continue;
    }
//    coplanaires ? verifier
    if (!pln.Position().IsCoplanar(pli.Position(),GetEpsGeom(),GetEpsilon())) {
      Message_Msg msg1295("IGES_1295");
      msg1295.Arg(i);
      SendWarning(st,msg1295);
      // "A hole is not well coplanar to the face, n0 %d",i);
    }
//    Ne pas oublier de composer la transformation locale a ce Wire
    if (trsi.Form() != gp_Identity) {
      TopLoc_Location locw(trsi);
      wire.Location(locw);
    }
    B.Add (res,wire);
  }
//    Enfin, appliquer la trsf globale
  if (trsf.Form() != gp_Identity) {
    TopLoc_Location loc(trsf);
    res.Location(loc);
  }
  return res;
}


//=======================================================================
//function : TransferPlaneParts
//purpose  : 
//=======================================================================
TopoDS_Shape IGESToBRep_TopoSurface::TransferPlaneParts(const Handle(IGESGeom_Plane)& st,
                                                        gp_Pln&  pln,  
                                                        gp_Trsf&  trsf,
                                                        const Standard_Boolean first)
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////
  TopoDS_Shape res;
  if (st.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  Standard_Real a, b, c, d;
  // equation de Geom : ax + by + cz + d = 0.0;
  // equation de IGES : ax + by + cz = d;
  st->Equation(a, b, c, d);
  pln = gp_Pln(a, b, c, -d);
  
  TopoDS_Face plane;
  TopoDS_Wire wire;
  BRep_Builder B;
  if (first) {
    B.MakeFace(plane);         // Just to create a empty Plane with a Tshape.
    Handle (Geom_Plane) geomPln = new Geom_Plane(pln);
    geomPln->Scale(gp_Pnt(0,0,0),GetUnitFactor());
//   ATTENTION, ici on CALCULE la trsf, on ne l`applique pas ...
    //S4054: B.UpdateFace (plane, geomPln, TopLoc_Location(), 
			   //GetEpsGeom()*GetUnitFactor());
    B.UpdateFace (plane, geomPln, TopLoc_Location(), Precision::Confusion());
    //:3 by ABV 5 Nov 97: set Infinite() flag (see below for unsetting)
    plane.Infinite ( Standard_True );  //:3
  }

//   ATTENTION, ici on CALCULE la trsf, on ne l'appliquera qu'a la fin !
  if (st->HasTransf()) {
    SetEpsilon(1.E-04);
    if (!IGESData_ToolLocation::ConvertLocation
	(GetEpsilon(), st->CompoundLocation(),trsf,GetUnitFactor())) { 
      Message_Msg msg1035("IGES_1035");
      SendWarning(st,msg1035); // Transformation : not a similarity
    }
  }

  if (st->HasBoundingCurve()) {
    IGESToBRep_TopoCurve TC(*this);
    Handle(IGESData_IGESEntity) crv = st->BoundingCurve();

    if (crv.IsNull()) {
      Message_Msg msg1300("IGES_1300");
      SendWarning(st,msg1300);
      //:4 by ABV 5 Nov 97: plane cannot be trimmed - let it be infinite
      //:4 NOTE: NB "else"
      //:4      return res;
    }
    else //:4
      
      if (IGESToBRep::IsTopoCurve(crv)) {
	gp_Trsf trans;
	if (crv->IsKind(STANDARD_TYPE(IGESGeom_CurveOnSurface))) {
	  DeclareAndCast(IGESGeom_CurveOnSurface, crv142, crv);
	  TopoDS_Shape myshape = TC.TransferCurveOnFace (plane, crv142, trans, TheULength, Standard_False);
	  
	  //:3 by ABV 5 Nov 97: set plane to be finite
	  if ( first ) {
	    TopExp_Explorer ws ( plane, TopAbs_WIRE ); 
	    if ( ws.More() ) plane.Infinite ( Standard_False ); 
	  }
	}
	else {
	  TopoDS_Shape     shape = TC.TransferTopoCurve(crv);
	  TopAbs_ShapeEnum shapeEnum = shape.ShapeType();
	  switch (shapeEnum) {
	  case TopAbs_EDGE :
	    {
	      TopoDS_Edge edge = TopoDS::Edge(shape);
	      Handle(ShapeExtend_WireData) sewd = new ShapeExtend_WireData;
	      sewd->Add(edge);
	      wire = sewd->Wire();
	    }
	    break;
	  case TopAbs_WIRE :
	    {
	      wire = TopoDS::Wire(shape);	    
	    }
	  break;
	  default:
	    { 
	      Message_Msg msg1156("IGES_1156");
	      const Standard_CString typeName("Bounding curve");
	      Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(crv);
	      msg1156.Arg(typeName);
	      msg1156.Arg(label);
	      SendWarning(st, msg1156);
	      if (first) res = plane;
	      else       res = wire;
	      return res;
	    }
	  }
	  //S4054 CTS18953 entity 14
	  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
	  sfw->Load (wire);
	  sfw->FixConnected (GetMaxTol());
	  wire = sfw->Wire();
	  BRepLib_MakeFace MF(pln, wire, Standard_False);
	  if (!MF.IsDone()) {
	    // AddFail(st, "Plane Construction Error.");
	    return res;
	  }
	  
	  TopoDS_Face F = MF.Face();
	  GProp_GProps G;
	  BRepGProp::SurfaceProperties(F,G);
	  if (G.Mass() < 0) {
	    if(!st->HasBoundingCurveHole())    
	      wire.Reverse();
	  }
	  else
	    if( st->HasBoundingCurveHole())
	      wire.Reverse();
	  //:3 by ABV 5 Nov 97: set plane to be finite
	  //:3        if (first) B.Add (plane,wire);
	  if ( first ) { 
	    B.Add ( plane, wire ); 
	    plane.Infinite ( Standard_False ); 
	  } 
	  //BRepLib_MakeFace MP(pln, wire);
	  //plane = MP.Face();
	}
      }
      else {
	Message_Msg msg1156("IGES_1156");
	const Standard_CString typeName("Bounding curve");
	Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(crv);
	msg1156.Arg(typeName);
	msg1156.Arg(label);
	SendWarning(st, msg1156);
	// Plane Cannot Be Trimmed.
      }
  }
  
  if (first) res = plane;
  else       res = wire;
  return res;
}


//=======================================================================
//function : ParamSurface
//purpose  : 
//=======================================================================
TopoDS_Shape IGESToBRep_TopoSurface::ParamSurface(const Handle(IGESData_IGESEntity)& st,
						  gp_Trsf2d& trans,
						  Standard_Real& uFact) 
{ // Declaration of messages// 
  // DCE 22/12/98
  //Message_Msg msg1005("IGES_1005");
  ////////////////////////////////

  TopoDS_Shape res;

  TopoDS_Shape basisSurface = TransferTopoSurface(st);
  Standard_Real uscale = 1.;
  Standard_Real cscale = TheULength;
  if (basisSurface.IsNull()) {
    Message_Msg msg1005("IGES_1005");
    SendFail(st, msg1005);
    return res;
  }

  TopAbs_ShapeEnum shapeEnum;
  shapeEnum = basisSurface.ShapeType();
  TopoDS_Face  face;
  switch (shapeEnum) {
  case TopAbs_FACE :
    {
      face = TopoDS::Face(basisSurface);
      break;
    }
  case TopAbs_SHELL :
    {
      TopoDS_Iterator IT(basisSurface);
      Standard_Integer nbfaces = 0;
      for (; IT.More(); IT.Next()) {
	nbfaces++;
	face = TopoDS::Face(IT.Value());
      }
      //szv#4:S4163:12Mar99 optimized
      if (nbfaces != 1) {
	 Message_Msg msg1156("IGES_1156");
	 const Standard_CString typeName("basis surface");
	 Handle(TCollection_HAsciiString) label = GetModel()->StringLabel(st);
	 msg1156.Arg(typeName);
	 msg1156.Arg(label);
	 SendWarning(st, msg1156);
	 return basisSurface;
      }
    }
    break;
  default:
    {
      //AddFail(st, "Basis Surface Transfer Error.");
      return res;
    }
  }
  
  //S4181 pdn 19.04.99 defining shift of parametric space on base 
  // of CAS.CADE type of surface  
  Standard_Real paramu = 0., paramv = 0.;
  TopLoc_Location L;
  TopoDS_Edge theedge;
  Handle(Geom_Surface) Surf = BRep_Tool::Surface(face);

  if (Surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) { 
    DeclareAndCast(Geom_RectangularTrimmedSurface, rectang, Surf);
    Surf = rectang->BasisSurface();
  }

  if ((Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) ||
      (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))     ||
      (Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))    ||
      (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))) {
    TopExp_Explorer TE;
    for ( TE.Init(face,TopAbs_EDGE); TE.More(); TE.Next()){
      TopoDS_Edge myedge = TopoDS::Edge(TE.Current());
      Standard_Real First, Last;
      Handle(Geom2d_Curve) Curve2d = BRep_Tool::CurveOnSurface
	(myedge, face, First, Last);
      if ( Curve2d->IsKind(STANDARD_TYPE(Geom2d_Line))) {
	DeclareAndCast(Geom2d_Line, Line2d, Curve2d);	
	if (Line2d->Direction().IsParallel(gp::DY2d(),Precision::Angular())){
	  theedge = myedge;
	  break;
	}
      }
    }

    Standard_Real First, Last;
    Handle(Geom_Curve) Curve3d = BRep_Tool::Curve(theedge, First, Last); 
    if (Precision::IsNegativeInfinite(First)) First = 0.;  
    
    if (Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))){
      DeclareAndCast(Geom_CylindricalSurface, Cyl, Surf);
      gp_Cylinder TheCyl = Cyl->Cylinder();
      ElSLib::CylinderParameters(TheCyl.Position(), 
				 TheCyl.Radius(), 
				 Curve3d->Value(First), paramu , paramv );
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))){
      DeclareAndCast(Geom_ConicalSurface, Cone, Surf);
      gp_Cone TheCone = Cone->Cone();
      ElSLib::ConeParameters(TheCone.Position(), 
			     TheCone.RefRadius(), 
			     TheCone.SemiAngle(), 
			     Curve3d->Value(First), paramu , paramv );
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))){
      DeclareAndCast(Geom_ToroidalSurface, Tore, Surf);
      gp_Torus TheTore = Tore->Torus();
      ElSLib::TorusParameters(TheTore.Position(), 
			      TheTore.MajorRadius(), 
			      TheTore.MinorRadius(), 
			      Curve3d->Value(First), paramu , paramv );
    }
    else if (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) &&
             //: abv 18.06.02: loopback on s1.brep BRep mode, face 32 - the spherical surface (192)
             //: in IGES is from [-90,90] by V, i.e. similar to OCC, thus only scaling is enough
             ! st->IsKind (STANDARD_TYPE(IGESSolid_SphericalSurface)) ) {
      DeclareAndCast(Geom_SphericalSurface, Sphere, Surf);
      gp_Sphere TheSphere = Sphere->Sphere();
      ElSLib::SphereParameters(TheSphere.Position(), 
 			       TheSphere.Radius(), 
 			       Curve3d->Value(First), paramu , paramv );
    }
        
    //#88 rln 06.04.99 CTS60168, BLEND.IGS entity 68
    //Generatrix is Transformed Circular Arc. When creating CAS.CADE surface parameterization
    //has changed.
    if (st->IsKind (STANDARD_TYPE (IGESGeom_SurfaceOfRevolution))) {
      DeclareAndCast (IGESGeom_SurfaceOfRevolution, revol, st);
      Handle(IGESData_IGESEntity) generatrix = revol->Generatrix();
      if (generatrix->IsKind (STANDARD_TYPE (IGESGeom_CircularArc))) {
	DeclareAndCast (IGESGeom_CircularArc, circ, generatrix);
	gp_Pnt2d startpoint = circ->StartPoint();
	paramv -= ElCLib::Parameter (gp_Circ2d (gp_Ax2d (circ->Center(), gp_Dir2d(1,0)), circ->Radius()), startpoint);
	if (Surf->IsKind (STANDARD_TYPE(Geom_SphericalSurface)))
	  paramv += ShapeAnalysis::AdjustToPeriod(paramv, - M_PI, M_PI);
	else if (Surf->IsKind (STANDARD_TYPE(Geom_ToroidalSurface)))
	  paramv += ShapeAnalysis::AdjustToPeriod(paramv, 0, M_PI * 2);
      }
    }
    else if (st->IsKind (STANDARD_TYPE (IGESGeom_TabulatedCylinder))) {
      DeclareAndCast (IGESGeom_TabulatedCylinder, cylinder, st);
      Handle(IGESData_IGESEntity) directrix = cylinder->Directrix();
      if (directrix->IsKind (STANDARD_TYPE (IGESGeom_CircularArc))) {
	DeclareAndCast (IGESGeom_CircularArc, circ, directrix);
	gp_Pnt2d startpoint = circ->StartPoint();
	paramu -= ElCLib::Parameter (gp_Circ2d (gp_Ax2d (circ->Center(), gp_Dir2d(1,0)), circ->Radius()), startpoint);
	paramu += ShapeAnalysis::AdjustToPeriod(paramu, 0, M_PI * 2);
      }
    }

  }

  if ( Abs(paramu) <= Precision::Confusion()) paramu = 0.;
  if ( Abs(paramv) <= Precision::Confusion()) paramv = 0.;
  
  //S4181 pdn 16.04.99 computation of transformation depending on 
  //IGES Type of surface
  Handle(IGESData_IGESEntity) isrf = st;
  if (isrf->IsKind(STANDARD_TYPE(IGESGeom_OffsetSurface))){
    DeclareAndCast(IGESGeom_OffsetSurface, offsurf, isrf);
    isrf = offsurf->Surface();
  }
  if (isrf->IsKind(STANDARD_TYPE(IGESGeom_SurfaceOfRevolution))) {
    DeclareAndCast(IGESGeom_SurfaceOfRevolution, st120, isrf);
    //S4181 pdn 19.04.99 defining transformation matrix
    gp_Trsf2d tmp;
    tmp.SetTranslation(gp_Vec2d (0, -2 * M_PI));
    trans.PreMultiply(tmp);
    tmp.SetMirror(gp::OX2d());
    trans.PreMultiply(tmp);
    tmp.SetMirror(gp_Ax2d (gp::Origin2d(), gp_Dir2d (1.,1.)));
    trans.PreMultiply(tmp);
    uscale = 1./cscale;
    //#30 rln 19.10.98
    //CAS.CADE SA = 2*PI - IGES TA
    //paramu = st120->StartAngle();
    paramu = -(2 * M_PI - st120->EndAngle());
  }
  else
    paramu = 0.;
  
  if (isrf->IsKind(STANDARD_TYPE(IGESGeom_RuledSurface))) {
    uscale = 1./cscale;
  }
  
  // corrected skl 13.11.2001 for BUC61054
  if (isrf->IsKind(STANDARD_TYPE(IGESGeom_TabulatedCylinder))) {
    Handle(IGESGeom_TabulatedCylinder) igtc = Handle(IGESGeom_TabulatedCylinder)::DownCast(isrf);
    Handle(IGESData_IGESEntity) idie = igtc->Directrix();
    Standard_Real uln=1;
    Standard_Real Umin,Umax,Vmin,Vmax;
    //scaling parameterization from [0,1]
    Surf->Bounds(Umin,Umax,Vmin,Vmax);
    uln = Abs(Umax-Umin);
    //computing shift of pcurves
    uscale = uln/cscale;
    paramu = Umin/uln;
  }
  
  if (isrf->IsKind(STANDARD_TYPE(IGESSolid_CylindricalSurface))||
      isrf->IsKind(STANDARD_TYPE(IGESSolid_ConicalSurface))) {
    uscale = M_PI/180.;
  }
  
  if (isrf->IsKind(STANDARD_TYPE(IGESSolid_SphericalSurface))) {
    cscale = M_PI/180.;
    uscale = 1.;
  }
      
  if (isrf->IsKind(STANDARD_TYPE(IGESSolid_ToroidalSurface))) {
    gp_Trsf2d tmp;
    tmp.SetTranslation(gp_Vec2d (0, -360.)); // in IGES terms
    trans.PreMultiply(tmp);
    tmp.SetMirror(gp::OX2d());
    trans.PreMultiply(tmp);
    tmp.SetMirror(gp_Ax2d (gp::Origin2d(), gp_Dir2d (1.,1.)));
    trans.PreMultiply(tmp);
    if(paramv > 0)
      paramv = paramv*180./M_PI;
    cscale = M_PI/180.;
    uscale = 1.;
  } 
    
  gp_Trsf2d tmp;
  tmp.SetTranslation(gp_Pnt2d(0.,0.), gp_Pnt2d(paramu,paramv));
  trans.PreMultiply(tmp);
  
  tmp.SetScale(gp_Pnt2d(0,0),cscale);
  trans.PreMultiply(tmp);
  uFact = uscale;
  return face;
}

