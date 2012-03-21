// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <AIS.ixx>

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRep_Tool.hxx>
#include <Bnd_Box.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>

#include <GccEnt_QualifiedLin.hxx>
#include <Geom2d_Circle.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomLib.hxx>
#include <GeomProjLib.hxx>
#include <GeomProjLib.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <StdPrs_Point.hxx>
#include <StdPrs_WFDeflectionShape.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopoDS.hxx>
#include <gce_MakeLin.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>

const Standard_Real SquareTolerance = Precision::Confusion()*Precision::Confusion();

//=======================================================================
//function : Nearest
//purpose  :
//=======================================================================

gp_Pnt AIS::Nearest(const TopoDS_Shape& ashape, const gp_Pnt& apoint)
{
  Standard_Real dist2 = RealLast();
  Standard_Real curdist2;
  gp_Pnt result(0.0,0.0,0.0);
  gp_Pnt curpnt(0.0,0.0,0.0);
  TopExp_Explorer explo(ashape,TopAbs_VERTEX);
  while (explo.More())
    {
      curpnt = BRep_Tool::Pnt(TopoDS::Vertex(explo.Current()));
      curdist2 = apoint.SquareDistance(curpnt);
      if (curdist2 < dist2)
        {
          result = curpnt;
          dist2 = curdist2;
        }
      explo.Next();
    }
  return result;
}

//=======================================================================
//function : Farest
//purpose  :
//=======================================================================

gp_Pnt AIS::Farest( const TopoDS_Shape& aShape, const gp_Pnt& aPoint )
{
  Standard_Real MaxDist2 = 0.0e0, curdist2;
  gp_Pnt Result(0.0,0.0,0.0);
  gp_Pnt curpnt(0.0,0.0,0.0);
  TopExp_Explorer Explo( aShape, TopAbs_VERTEX );
  for (; Explo.More(); Explo.Next())
    {
      curpnt = BRep_Tool::Pnt( TopoDS::Vertex( Explo.Current() ) );
      curdist2 = aPoint.SquareDistance( curpnt );
      if (curdist2 > MaxDist2)
        {
          MaxDist2 = curdist2;
          Result = curpnt;
        }
    }
  return Result;
}


//=======================================================================
//function : ComputeGeometry
//purpose  : for line, circle, ellipse 
//=======================================================================

Standard_Boolean AIS::ComputeGeometry(const TopoDS_Edge& anEdge,
				      Handle(Geom_Curve)& aCurve,
				      gp_Pnt& FirstPnt,
				      gp_Pnt& LastPnt)
{
  TopLoc_Location loc_edge;
  Standard_Real first,last;
  aCurve = BRep_Tool::Curve(anEdge,loc_edge,first,last);
  if (aCurve.IsNull()) return Standard_False;
  if (!loc_edge.IsIdentity()) {
//#ifndef DEB
    Handle(Geom_Geometry) aGeomGeometry = aCurve->Transformed(loc_edge.Transformation());
    aCurve = (Handle(Geom_Curve)&) aGeomGeometry ;
//#else
//    aCurve = (Handle(Geom_Curve)&) aCurve->Transformed(loc_edge.Transformation());
//#endif
  }
  if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    aCurve = ((Handle(Geom_TrimmedCurve)&) aCurve)->BasisCurve();
  }

  if (aCurve->IsInstance(STANDARD_TYPE(Geom_Line))) {
    // CLE
    // const Handle(Geom_Line)& line = (Handle(Geom_Line)&) aCurve;
    Handle(Geom_Line) line = (Handle(Geom_Line)&) aCurve;
    // ENDCLE

    FirstPnt = ElCLib::Value(first,line->Lin());
    LastPnt = ElCLib::Value(last,line->Lin());
  }
  else if (aCurve->IsInstance(STANDARD_TYPE(Geom_Circle))) {
    // CLE
    // const Handle(Geom_Circle)& circ = (Handle(Geom_Circle)&) aCurve;
    Handle(Geom_Circle) circ = (Handle(Geom_Circle)&) aCurve;
    // ENDCLE
    FirstPnt = ElCLib::Value(first,circ->Circ());
    LastPnt = ElCLib::Value(last,circ->Circ());
  }
  else if (aCurve->IsInstance(STANDARD_TYPE(Geom_Ellipse))) {
    // CLE
    // const Handle(Geom_Ellipse)& elips = (Handle(Geom_Ellipse)&) aCurve;
    Handle(Geom_Ellipse) elips = (Handle(Geom_Ellipse)&) aCurve;
    // ENDCLE
    FirstPnt = ElCLib::Value(first, elips->Elips());
    LastPnt = ElCLib::Value(last, elips->Elips());
  }
  else return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  :
//=======================================================================

Standard_Boolean AIS::ComputeGeometry(const TopoDS_Edge& anEdge,
                                      Handle(Geom_Curve)& aCurve,
                                      gp_Pnt& FirstPnt,
                                      gp_Pnt& LastPnt,
                                      Handle(Geom_Curve)& extCurve,
                                      Standard_Boolean& isInfinite,
                                      Standard_Boolean& isOnPlane,
                                      const Handle(Geom_Plane)& aPlane)
{
  if (aPlane.IsNull()) return Standard_False;

  Standard_Real first,last;
  BRepAdaptor_Curve brepCurv(anEdge);
  aCurve = Handle(Geom_Curve)::DownCast(brepCurv.Curve().Curve()->Transformed(brepCurv.Trsf()));
  first = brepCurv.FirstParameter();
  last = brepCurv.LastParameter();

  if (aCurve.IsNull()) return Standard_False;
  
  extCurve = aCurve;
  isInfinite = (Precision::IsInfinite(first) || Precision::IsInfinite(last));

  // Checks that the projcurve is not in the plane
  isOnPlane = Standard_True;
  if (extCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    extCurve = ((Handle(Geom_TrimmedCurve)&) extCurve)->BasisCurve();
  }

  if ( extCurve->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
    // CLE
    // const Handle(Geom_Line) & gl = (Handle(Geom_Line)&) extCurve;
    Handle(Geom_Line) gl = (Handle(Geom_Line)&) extCurve;
    // ENDCLE
    isOnPlane = aPlane->Pln().Contains( gl->Lin(), Precision::Confusion(), Precision::Angular() );
  }
  else if (extCurve->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
    // CLE
    // const Handle(Geom_Circle) & gc = (Handle(Geom_Circle)&) extCurve;
    Handle(Geom_Circle) gc = (Handle(Geom_Circle)&) extCurve;
    // ENDCLE
    gp_Ax3 ax(gc->Position());
    isOnPlane = ax.IsCoplanar(aPlane->Pln().Position(),
                              Precision::Confusion(),
                              Precision::Angular());
  }
  if ( isOnPlane ) {
    extCurve.Nullify();
  }
  
//#ifndef DEB
  Handle(Geom_Curve) aGeomCurve = GeomProjLib::ProjectOnPlane(aCurve,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
  aCurve = aGeomCurve ;
//#else
//  aCurve = (Handle(Geom_Curve)&) GeomProjLib::ProjectOnPlane(aCurve,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
//#endif
  
  if (aCurve->IsInstance(STANDARD_TYPE(Geom_Line))) {
    // CLE
    // const Handle(Geom_Line)& line = (Handle(Geom_Line)&) aCurve;
    Handle(Geom_Line) line = (Handle(Geom_Line)&) aCurve;
    // EDNCLE
    if (!isInfinite) {
      FirstPnt = ElCLib::Value(first,line->Lin());
      LastPnt = ElCLib::Value(last,line->Lin());
    }
  }
  else if (aCurve->IsInstance(STANDARD_TYPE(Geom_Circle))) {
    // CLE
    // const Handle(Geom_Circle)& circ = (Handle(Geom_Circle)&) aCurve;
    Handle(Geom_Circle) circ = (Handle(Geom_Circle)&) aCurve;
    // ENDCLE
    FirstPnt = ElCLib::Value(first,circ->Circ());
    LastPnt = ElCLib::Value(last,circ->Circ());
  }
  // jfa 10/10/2000
  else if (aCurve->IsInstance(STANDARD_TYPE(Geom_Ellipse)))
    {
      Handle(Geom_Ellipse) ell = (Handle(Geom_Ellipse)&) aCurve;
      FirstPnt = ElCLib::Value(first,ell->Elips());
      LastPnt = ElCLib::Value(last,ell->Elips());
    }
  // jfa 10/10/2000 end
  else return Standard_False;
  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  :
//=======================================================================

Standard_Boolean AIS::ComputeGeometry(const TopoDS_Edge& anEdge1,
				      const TopoDS_Edge& anEdge2,
				      Handle(Geom_Curve)& aCurve1,
				      Handle(Geom_Curve)& aCurve2,
				      gp_Pnt& FirstPnt1,
				      gp_Pnt& LastPnt1,
				      gp_Pnt& FirstPnt2,
				      gp_Pnt& LastPnt2,
				      const Handle(Geom_Plane)& aPlane)
{
  if (aPlane.IsNull()) return Standard_False;

  TopLoc_Location loc_edge1,loc_edge2;
  Standard_Real first1,last1,first2,last2;
  
  aCurve1 = BRep_Tool::Curve(anEdge1,loc_edge1,first1,last1);
  aCurve2 = BRep_Tool::Curve(anEdge2,loc_edge2,first2,last2);

  if (aCurve1.IsNull()) return Standard_False;
  if (aCurve2.IsNull()) return Standard_False;
  
  if (!loc_edge1.IsIdentity()) {
//#ifndef DEB
    Handle(Geom_Geometry) aGeomGeometry = aCurve1->Transformed(loc_edge1.Transformation());
    aCurve1 = (Handle(Geom_Curve)&) aGeomGeometry ;
//#else
//    aCurve1 = (Handle(Geom_Curve)&) aCurve1->Transformed(loc_edge1.Transformation());
//#endif
  }
  if (!loc_edge2.IsIdentity()) {
//#ifndef DEB
    Handle(Geom_Geometry) aGeomGeometry = aCurve2->Transformed(loc_edge2.Transformation());
    aCurve2 = (Handle(Geom_Curve)&) aGeomGeometry ;
//#else
//    aCurve2 = (Handle(Geom_Curve)&) aCurve2->Transformed(loc_edge2.Transformation());
//#endif
  }

//#ifndef DEB
  Handle(Geom_Curve) aGeomCurve = GeomProjLib::ProjectOnPlane(aCurve1,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
  aCurve1 = aGeomCurve ;
//#else
//  aCurve1 =  (Handle(Geom_Curve)&) GeomProjLib::ProjectOnPlane(aCurve1,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
//#endif
//#ifndef DEB
  aGeomCurve = GeomProjLib::ProjectOnPlane(aCurve2,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
  aCurve2 = aGeomCurve;
//#else
//  aCurve2 =  (Handle(Geom_Curve)&) GeomProjLib::ProjectOnPlane(aCurve2,aPlane,aPlane->Pln().Axis().Direction(),Standard_False);
//#endif

  if (aCurve1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    aCurve1 = ((Handle(Geom_TrimmedCurve)&) aCurve1)->BasisCurve();
  }
  if (aCurve2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    aCurve2 = ((Handle(Geom_TrimmedCurve)&) aCurve2)->BasisCurve();
  }

  if (aCurve1->IsInstance(STANDARD_TYPE(Geom_Line))) {
    // CLE
    // const Handle(Geom_Line)& line = (Handle(Geom_Line)&) aCurve1;
    Handle(Geom_Line) line = (Handle(Geom_Line)&) aCurve1;
    // ENDCLE
    FirstPnt1 = ElCLib::Value(first1,line->Lin());
    LastPnt1 = ElCLib::Value(last1,line->Lin());
  }
  else if (aCurve1->IsInstance(STANDARD_TYPE(Geom_Circle))) {
    // CLE
    // const Handle(Geom_Circle)& circ = (Handle(Geom_Circle)&) aCurve1;
    Handle(Geom_Circle) circ = (Handle(Geom_Circle)&) aCurve1;
    // ENDCLE
    FirstPnt1 = ElCLib::Value(first1,circ->Circ());
    LastPnt1 = ElCLib::Value(last1,circ->Circ());
  }
  else return Standard_False;

  if (aCurve2->IsInstance(STANDARD_TYPE(Geom_Line))) {
    // CLE
    // const Handle(Geom_Line)& line = (Handle(Geom_Line)&) aCurve2;
    Handle(Geom_Line) line = (Handle(Geom_Line)&) aCurve2;
    // ENDCLE
    FirstPnt2 = ElCLib::Value(first2,line->Lin());
    LastPnt2 = ElCLib::Value(last2,line->Lin());
  }
  else if (aCurve2->IsInstance(STANDARD_TYPE(Geom_Circle))) {
    // CLE
    // const Handle(Geom_Circle)& circ = (Handle(Geom_Circle)&) aCurve2;
    Handle(Geom_Circle) circ = (Handle(Geom_Circle)&) aCurve2;
    // ENDCLE
    FirstPnt2 = ElCLib::Value(first2,circ->Circ());
    LastPnt2 = ElCLib::Value(last2,circ->Circ());
  }
  else return Standard_False;

  return Standard_True;
}

//=======================================================================
//function : ComputeGeometry
//purpose  : Computes the geometry of the 2 edges in the current wp
//           and the 'rigth' geometry of the edges if one doesn't
//           belong to the currentworkingplane.
//           There may be only one curve that can't belong to the
//           current workingplane ( attachement constraint)
//           if the 2 edges belong to the current WP, <WhatProj> = 0
//
//           indexExt = 0 2 edges are in the current wp
//           indexExt = 1 first edge is not in the current wp
//           indexExt = 2 second edge is not in the current wp
//           if none of the two edges is in the current wp ,
//           it returns Standard_False
//=======================================================================

Standard_Boolean AIS::ComputeGeometry(const TopoDS_Edge& anEdge1, 
				      const TopoDS_Edge& anEdge2, 
				      Standard_Integer& indexExt, 
				      Handle(Geom_Curve)& aCurve1, 
				      Handle(Geom_Curve)& aCurve2, 
				      gp_Pnt& FirstPnt1, 
				      gp_Pnt& LastPnt1, 
				      gp_Pnt& FirstPnt2, 
				      gp_Pnt& LastPnt2, 
				      Handle(Geom_Curve)& extCurve,
				      Standard_Boolean& isInfinite1,
				      Standard_Boolean& isInfinite2,				      
				      const Handle(Geom_Plane)& aPlane)
{
  if (aPlane.IsNull()) return Standard_False;
  extCurve.Nullify();
  indexExt = 0;

  Standard_Real first1,last1,first2,last2;
  isInfinite1 = isInfinite2 = Standard_False;

  BRepAdaptor_Curve brepCurv1(anEdge1);
  BRepAdaptor_Curve brepCurv2(anEdge2);
  aCurve1 = Handle(Geom_Curve)::DownCast(brepCurv1.Curve().Curve()->Transformed(brepCurv1.Trsf()));
  aCurve2 = Handle(Geom_Curve)::DownCast(brepCurv2.Curve().Curve()->Transformed(brepCurv2.Trsf()));
  if (aCurve1->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    aCurve1 = ((Handle(Geom_TrimmedCurve)&) aCurve1)->BasisCurve();
  }
  if (aCurve2->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
    aCurve2 = ((Handle(Geom_TrimmedCurve)&) aCurve2)->BasisCurve();
  }

  first1 = brepCurv1.FirstParameter();
  last1 = brepCurv1.LastParameter();
  first2 = brepCurv2.FirstParameter();
  last2 = brepCurv2.LastParameter();

  if (aCurve1.IsNull()) return Standard_False;
  if (aCurve2.IsNull()) return Standard_False;
  
  Handle(Geom_Curve) aSov1 = aCurve1;
  Handle(Geom_Curve) aSov2 = aCurve2;
 
  // Checks that the projcurve is not in the plane
  Standard_Boolean isOnPlanC1,isOnPlanC2;
  if ((!ComputeGeomCurve(aCurve1,first1,last1,FirstPnt1,LastPnt1,aPlane,isOnPlanC1))
      || (!ComputeGeomCurve(aCurve2,first2,last2,FirstPnt2,LastPnt2,aPlane,isOnPlanC2)))
    return Standard_False;
  
  if (Precision::IsInfinite(first1) || Precision::IsInfinite(last1)) {
    isInfinite1 = Standard_True;
    indexExt = 1;
  }
  if (Precision::IsInfinite(first2) || Precision::IsInfinite(last2)) {
    isInfinite2 = Standard_True;    
    indexExt = 2;
  }
  if (isInfinite1 && isInfinite2) indexExt = 0;  //New

  if (isInfinite1 || isInfinite2) {
    if (aCurve1->DynamicType() == aCurve2->DynamicType()) {
      // CLE
      // const gp_Lin& lin1 = ((Handle(Geom_Line)&) aCurve1)->Lin();
      // const gp_Lin& lin2 = ((Handle(Geom_Line)&) aCurve2)->Lin();
      gp_Lin lin1 = ((Handle(Geom_Line)&) aCurve1)->Lin();
      gp_Lin lin2 = ((Handle(Geom_Line)&) aCurve2)->Lin();
      // ENDCLE
      if (indexExt == 1) {
        FirstPnt1 = ElCLib::Value(ElCLib::Parameter(lin2,FirstPnt2),lin1);
        LastPnt1 = ElCLib::Value(ElCLib::Parameter(lin2,LastPnt2),lin1);
      }
      else if (indexExt == 2) {
	FirstPnt2 = ElCLib::Value(ElCLib::Parameter(lin1,FirstPnt1),lin2);
	LastPnt2 = ElCLib::Value(ElCLib::Parameter(lin1,LastPnt1),lin2);
      }
    }
  }

  if (isOnPlanC1 && isOnPlanC2) return Standard_True;

  if (!isOnPlanC1 && isOnPlanC2) {// curve 2 only in the plane
    indexExt = 1;
    extCurve = aSov1;
  }
  else if (isOnPlanC1 && !isOnPlanC2) {// curve 1 only in the plane
    indexExt = 2;
    extCurve = aSov2;
  }
  else
    return Standard_False;
  
  return Standard_True;
}


// it is patch!

//=======================================================================
//function : ComputeGeomCurve
//purpose  : Checks if aCurve belongs to aPlane; if not, projects aCurve in aPlane
//           and returns aCurveproj;
//           Return TRUE if ok
//=======================================================================

Standard_Boolean AIS::ComputeGeomCurve(Handle(Geom_Curve)& aCurve,
                                          const Standard_Real first1,
                                          const Standard_Real last1,
                                          gp_Pnt& FirstPnt1,
                                          gp_Pnt& LastPnt1,
                                          const Handle(Geom_Plane)& aPlane,
                                          Standard_Boolean& isOnPlane)
{
  isOnPlane = Standard_True;

  const Standard_Integer NodeNumber = 20;
  Standard_Real Delta = (last1 - first1) / (NodeNumber - 1);
  if (Delta <= Precision::PConfusion())
    Delta = last1 - first1;
  gp_Pnt CurPnt(0.0,0.0,0.0);
  Standard_Real CurPar = first1;
  for (Standard_Integer i = 1; i <= NodeNumber; i++)
    {
      CurPnt = aCurve->Value( CurPar );
      if (aPlane->Pln().SquareDistance( CurPnt ) > SquareTolerance)
	{
	  isOnPlane = Standard_False;
	  break;
	}
      CurPar += Delta;
    }
  
  if (! Precision::IsInfinite(first1) && ! Precision::IsInfinite(last1))
    {
      FirstPnt1 = aCurve->Value( first1 );
      LastPnt1  = aCurve->Value( last1 );
    }
  if (!isOnPlane) {
//#ifndef DEB
    Handle( Geom_Curve ) aGeomCurve = GeomProjLib::ProjectOnPlane( aCurve,
								   aPlane,
								   aPlane->Pln().Axis().Direction(),
								   Standard_False);
    aCurve = aGeomCurve ;
//#else
//    aCurve =  (Handle( Geom_Curve )&) GeomProjLib::ProjectOnPlane( aCurve,
//								   aPlane,
//								   aPlane->Pln().Axis().Direction(),
//								   Standard_False);
//#endif
    if (aCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve))) {
      aCurve = ((Handle(Geom_TrimmedCurve)&) aCurve)->BasisCurve();
    }
    if (! Precision::IsInfinite(first1) && ! Precision::IsInfinite(last1)) {
      FirstPnt1 = AIS::ProjectPointOnPlane( FirstPnt1, aPlane->Pln() );
      LastPnt1 = AIS::ProjectPointOnPlane( LastPnt1, aPlane->Pln() );
    }
  }
  return Standard_True;
}


//=======================================================================
//function : ComputeGeometry
//purpose  : computes the point corresponding to the vertex <aVertex>
//           in the plane <aPlane>. If the vertex is already in the plane
//           <isOnPlane>, <isOnPlane> = true.
//           <point> is the projected vertex in the plane.
//=======================================================================

Standard_Boolean  AIS::ComputeGeometry(const TopoDS_Vertex& aVertex,
                                       gp_Pnt& point,
                                       const Handle(Geom_Plane)& aPlane,
                                       Standard_Boolean& isOnPlane)
{
  point = BRep_Tool::Pnt(aVertex);
  isOnPlane = aPlane->Pln().Contains(point,  Precision::Confusion());
  if ( !isOnPlane) {
    point = AIS::ProjectPointOnPlane( point, aPlane->Pln() );
  }
  return Standard_True;
}

//=======================================================================
//function : GetPlaneFromFace
//purpose  :
//           Returns type of surface which can be Plane or OtherSurface
//=======================================================================

Standard_Boolean AIS::GetPlaneFromFace( const TopoDS_Face&            aFace,
                                        gp_Pln &                     aPlane,
                                        Handle( Geom_Surface )&      aSurf,
                                        AIS_KindOfSurface &          aSurfType,
                                        Standard_Real &              Offset )

{
  Standard_Boolean Result = Standard_False;
  BRepAdaptor_Surface surf1( aFace );
  Handle( Adaptor3d_HSurface ) surf2;
  //gp_Vec OffsetVec( 1.0e0, 0.0e0, 0.0e0 );
  Standard_Boolean isOffset = Standard_False;

  if (surf1.GetType() == GeomAbs_OffsetSurface)
    {
      // Extracting Basis Surface
      surf2 = surf1.BasisSurface();
      isOffset = Standard_True;
    }
  else
    surf2 = new BRepAdaptor_HSurface( surf1 );

  aSurf = surf1.Surface().Surface();
  //  aSurf->Transform(surf1.Trsf()) ;
  aSurf = Handle( Geom_Surface )::DownCast( aSurf->Transformed( surf1.Trsf() ) );

  if (surf2->GetType() == GeomAbs_Plane)
    {
      aPlane = surf2->Plane();
      aSurfType = AIS_KOS_Plane;
      Offset = 0.;
      Result = Standard_True;
    }

  else if (surf2->GetType() == GeomAbs_SurfaceOfExtrusion)
    {
      Handle( Adaptor3d_HCurve ) BasisCurve = surf2->BasisCurve();
      gp_Dir ExtrusionDir = surf2->Direction();
      if (BasisCurve->GetType() == GeomAbs_Line)
        {
          gp_Lin BasisLine = BasisCurve->Line();
          gp_Dir LineDir = BasisLine.Direction();
          gp_Pnt LinePos = BasisLine.Location();
          gp_Pln thePlane( LinePos, LineDir ^ ExtrusionDir);
          aPlane = thePlane;
          aSurfType = AIS_KOS_Plane;
          Offset = 0.;
          Result = Standard_True;
        }
    }

  if (Result == Standard_True && isOffset)
    {
      aSurf = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Surface();
      aPlane = (Handle( Geom_Plane )::DownCast( aSurf ))->Pln();
      /*
      Handle( Geom_OffsetSurface ) OffsetSurf = Handle( Geom_OffsetSurface )::DownCast( aSurf );
      gp_Pnt PointOnPlane;
      gp_Vec D1u, D1v;
      OffsetSurf->D1( 0, 0, PointOnPlane, D1u, D1v );
      D1u.Normalize();
      D1v.Normalize();
      OffsetVec = D1u ^ D1v;
      aPlane.Translate( OffsetValue * OffsetVec );
      */
      Offset = 0.0e0;
    }
  if (Result == Standard_False)
    {
      if (isOffset)
	{
	  Handle( Standard_Type ) TheType = aSurf->DynamicType();
	  if (TheType == STANDARD_TYPE(Geom_CylindricalSurface) ||
	      TheType == STANDARD_TYPE(Geom_ConicalSurface)     ||
	      TheType == STANDARD_TYPE(Geom_SphericalSurface)   ||
	      TheType == STANDARD_TYPE(Geom_ToroidalSurface))
	    {
	      aSurf = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Surface();
	      Offset = 0.0e0;
	    }
	  else
	    {
	      Offset = (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->Offset();
	      aSurf =  (Handle( Geom_OffsetSurface )::DownCast( aSurf ))->BasisSurface();
	    }
	}
      Handle( Standard_Type ) TheType = aSurf->DynamicType();
      if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
        aSurfType = AIS_KOS_Cylinder;
      else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
        aSurfType = AIS_KOS_Cone;
      else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
        aSurfType = AIS_KOS_Sphere;
      else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
        aSurfType = AIS_KOS_Torus;
      else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
        aSurfType = AIS_KOS_Revolution;
      else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
        aSurfType = AIS_KOS_Extrusion;
      else
        aSurfType = AIS_KOS_OtherSurface;
    }
  return Result;
}


//=======================================================================
//function : ProjectPointOnPlane
//purpose  :
//=======================================================================

gp_Pnt AIS::ProjectPointOnPlane( const gp_Pnt & aPoint, const gp_Pln & aPlane )
{
  gp_Vec aVec( aPlane.Location(), aPoint );
  gp_Vec Normal = aPlane.Axis().Direction();
  Normal = (aVec * Normal) * Normal;

  return ( aPoint.Translated( -Normal ) );
}

//=======================================================================
//function : ProjectPointOnLine
//purpose  :
//=======================================================================

gp_Pnt AIS::ProjectPointOnLine( const gp_Pnt & aPoint, const gp_Lin & aLine )
{
  gp_XYZ LinLoc = aLine.Location().XYZ();
  gp_XYZ LinDir = aLine.Direction().XYZ();
  Standard_Real Parameter = (aPoint.XYZ() - LinLoc) * LinDir;
  gp_Pnt Result( LinLoc + Parameter * LinDir );
  return Result;
}


//=======================================================================
//function : InitFaceLength
//purpose  : 
//=======================================================================

void AIS::InitFaceLength( const TopoDS_Face& aFace, 
			 gp_Pln               & aPlane,
			 Handle(Geom_Surface) & aSurface,
			 AIS_KindOfSurface    & aSurfaceType,
			 Standard_Real        & anOffset) 
{
  AIS::GetPlaneFromFace( aFace, aPlane, aSurface, aSurfaceType, anOffset );
  
  if (Abs( anOffset ) > Precision::Confusion())
    {
      aSurface = new Geom_OffsetSurface( aSurface, anOffset );
      anOffset = 0.0e0;
    }
  
}

//=======================================================================
//function : ComputeLengthBetweenPlanarFaces
//purpose  :
//=======================================================================

void AIS::ComputeLengthBetweenPlanarFaces( const TopoDS_Face &      FirstFace,
                                           const TopoDS_Face &      SecondFace,
                                           const gp_Pln &           Plane1,
                                           const gp_Pln &           Plane2,
                                           Standard_Real &          Value,
                                           gp_Pnt &                 FirstAttach,
                                           gp_Pnt &                 SecondAttach,
                                           gp_Dir       &           DirAttach,
                                           const Standard_Boolean   AutomaticPos,
                                           gp_Pnt &                 Position )
{
  TopExp_Explorer aExp( FirstFace, TopAbs_VERTEX );
  // case of infinite planes. SMO.
  if (!aExp.More())
    FirstAttach = Plane1.Location();
  else  
    FirstAttach = BRep_Tool::Pnt( TopoDS::Vertex( aExp.Current() ) );
  SecondAttach = AIS::ProjectPointOnPlane( FirstAttach, Plane2 );

  Value = FirstAttach.Distance( SecondAttach );

  gp_Dir LengthDir = Plane1.Axis().Direction();
  /*
  if (Value > Precision::Confusion())
    LengthDir = gp_Dir( gp_Vec( FirstAttach, SecondAttach ) );
    */
  DirAttach = Plane1.Position().XDirection();

  if (AutomaticPos)
    Position.SetXYZ((FirstAttach.XYZ() + SecondAttach.XYZ())/2.0e0) ;
 
  else // position is given
    {
      FirstAttach = AIS::Nearest( FirstFace, Position );
      SecondAttach = AIS::ProjectPointOnPlane( FirstAttach, Plane2 );
    }

  Quantity_Parameter U, V ;
  ElSLib::Parameters( Plane2, SecondAttach, U, V );
  BRepTopAdaptor_FClass2d aClassifier( SecondFace, Precision::Confusion() );
  TopAbs_State State = aClassifier.Perform( gp_Pnt2d( U, V ), Standard_False );
  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
    {
      SecondAttach = AIS::Nearest( SecondFace, Position );
      if (Value > Precision::Confusion())
        {
          gp_Vec aVector = gp_Vec( FirstAttach, SecondAttach ) ^ LengthDir;
          if (aVector.SquareMagnitude() > SquareTolerance)
            DirAttach = aVector ^ LengthDir;
        }
      else
        {
          gp_Vec DirVec( FirstAttach, SecondAttach );
          if (DirVec.SquareMagnitude() > SquareTolerance)
            DirAttach = gp_Dir( DirVec );
        }
    }
  if (!AutomaticPos)
    {
      gp_Pln PlaneOfDim( FirstAttach, DirAttach ^ LengthDir );
      Position = AIS::ProjectPointOnPlane( Position, PlaneOfDim );
    }
}


// Purpose: Return the point from <aFace> wich is the more distant
//          from <anAxis>
static gp_Pnt FindFarPoint (const gp_Ax1 &           anAxis,
                            const TopoDS_Face &      aFace )
{
  gp_Pnt ResPnt(0.0,0.0,0.0);
  Standard_Real MaxDist = 0.e0, curdist2;

  gp_Lin Line (anAxis);

  TopExp_Explorer Explo (aFace, TopAbs_VERTEX);

  if (!Explo.More()) {
    // Case of infinite planes (no Vertex, no edge)
    gp_Pln plane;
    Handle( Geom_Surface ) aSurf;
    AIS_KindOfSurface KOS;
    Standard_Real offset;
    AIS::GetPlaneFromFace (aFace,plane,aSurf,KOS,offset );
    gp_Pnt aPoint = plane.Location();
    MaxDist = Line.SquareDistance (aPoint);
    if (MaxDist <= SquareTolerance)  {
      MaxDist = 100;
      gp_Dir DMove = plane.Axis().Direction()^anAxis.Direction();
      gp_Vec VMove (DMove.XYZ()*100);
      ResPnt = aPoint.Translated (VMove);
    }
  }

  for (; Explo.More(); Explo.Next())
    {
      // CLE
      // const TopoDS_Vertex & aVertex = TopoDS::Vertex( Explo.Current() );
      TopoDS_Vertex aVertex = TopoDS::Vertex( Explo.Current() );
      // ENDCLE
      gp_Pnt aPoint = BRep_Tool::Pnt( aVertex );
      curdist2 = Line.SquareDistance( aPoint );
      if (curdist2 > MaxDist)
        {
          MaxDist = curdist2;
          ResPnt = aPoint;
        }
    }

  if (MaxDist <= SquareTolerance)
    {
      const Standard_Integer NodeNumber = 20;
      Explo.Init( aFace, TopAbs_EDGE );
      for (; Explo.More(); Explo.Next())
        {
	  // CLE
          // const TopoDS_Edge & anEdge = TopoDS::Edge( Explo.Current() );
	  TopoDS_Edge anEdge = TopoDS::Edge( Explo.Current() );
	  // ENDCLE
          BRepAdaptor_Curve aCurve( anEdge );
          Standard_Real FirstPar = aCurve.FirstParameter();
          Standard_Real LastPar = aCurve.LastParameter();
          Standard_Real Delta = (LastPar - FirstPar) / (NodeNumber - 1);
          for (Standard_Integer i = 0; i < NodeNumber; i++)
            {
              gp_Pnt aPoint(0.0,0.0,0.0);
              aCurve.D0( FirstPar, aPoint );
              curdist2 = Line.SquareDistance( aPoint );
              if (curdist2 > MaxDist)
                {
                  MaxDist = curdist2;
                  ResPnt = aPoint;
                }
              FirstPar += Delta;
            }
          if (MaxDist > SquareTolerance) break;
        }
    }
  if (MaxDist <= SquareTolerance)
    Standard_ConstructionError::Raise("AIS:: problem attach point") ;
  return ResPnt;
}

void AIS::ComputeAngleBetweenPlanarFaces( const TopoDS_Face &      FirstFace,
                                          const TopoDS_Face &      SecondFace,
                                          const Handle( Geom_Surface )& Surf2,
                                          const gp_Ax1 &           Axis,
                                          const Standard_Real      Value,
                                          const Standard_Boolean   AutomaticPos,
                                          gp_Pnt &                 Position,
                                          gp_Pnt &                 Center,
                                          gp_Pnt &                 FirstAttach,
                                          gp_Pnt &                 SecondAttach,
                                          gp_Dir &                 FirstDir,
                                          gp_Dir &                 SecondDir )
{
  FirstAttach = FindFarPoint( Axis, FirstFace );
  Center = AIS::ProjectPointOnLine( FirstAttach, gp_Lin( Axis ) );
  gp_Dir aDir1( gp_Vec( Center, FirstAttach ) );
  FirstDir = aDir1;
  SecondAttach = FirstAttach.Rotated( Axis, Value );
  gp_Dir aDir2( gp_Vec( Center, SecondAttach ) );
  SecondDir = aDir2;

  GeomAPI_ProjectPointOnSurf   aProjPnt( SecondAttach,
                                         Surf2 ) ;
  //SecondAttach = aProjPnt.Point(1) ;
  Quantity_Parameter U,V ;
  aProjPnt.Parameters(1,U,V) ;
  BRepTopAdaptor_FClass2d aClassifier(SecondFace,
                                      Precision::Confusion());
  if (aClassifier.Perform(gp_Pnt2d(U,V),Standard_False) == TopAbs_OUT ||
      aClassifier.Perform(gp_Pnt2d(U,V),Standard_False) == TopAbs_UNKNOWN)
    SecondAttach = FindFarPoint( Axis, SecondFace );

  if (AutomaticPos)
    Position = FirstAttach.Rotated( Axis, Value*0.5 );
  else
    {
      gp_Pln PlaneOfDim( Center, Axis.Direction() );
      Position = AIS::ProjectPointOnPlane( Position, PlaneOfDim );
    }
}
                                     
                                     
void AIS::ComputeAngleBetweenCurvilinearFaces( const TopoDS_Face &      FirstFace,
                                               const TopoDS_Face &      SecondFace,
                                               const Handle( Geom_Surface )& FirstSurf,
                                               const Handle( Geom_Surface )& SecondSurf,
                                               const AIS_KindOfSurface  FirstSurfType,
                                               const AIS_KindOfSurface  SecondSurfType,
                                               const gp_Ax1 &           Axis,
                                               const Standard_Real      Value,
                                               const Standard_Boolean   AutomaticPos,
                                               gp_Pnt &                 Position,
                                               gp_Pnt &                 Center,
                                               gp_Pnt &                 FirstAttach,
                                               gp_Pnt &                 SecondAttach,
                                               gp_Dir &                 FirstDir,
                                               gp_Dir &                 SecondDir,
                                               Handle( Geom_Plane ) &   Plane )
{
  //
  // even if it is not AutomaticPosition do not assume the Automatic
  // case has saved the values in the AIS_AngleDimension class : this
  // is not always the case
  //
  gp_Pnt SavedPosition = Position ;
  FirstAttach = FindFarPoint( Axis, FirstFace );
  Plane = new Geom_Plane( Axis.Location(),
                          gp_Dir( gp_Vec( Axis.Location(), FirstAttach ) ^
                                  gp_Vec( Axis.Direction() ) ) );
  
  Handle( Geom_Line ) FirstLine, SecondLine;
  Standard_Real FirstU, FirstV;
  if (FirstSurfType == AIS_KOS_Cylinder)
    ElSLib::Parameters( Handle( Geom_CylindricalSurface )::DownCast( FirstSurf )->Cylinder(),
                        FirstAttach,
                        FirstU, FirstV );
  else // it is Cone
    ElSLib::Parameters( Handle( Geom_ConicalSurface )::DownCast( FirstSurf )->Cone(),
                        FirstAttach,
                        FirstU, FirstV );
  FirstLine = Handle( Geom_Line )::DownCast( FirstSurf->UIso( FirstU ) );
  
  if (SecondSurfType == AIS_KOS_Cylinder)
    {
      Handle( Geom_CylindricalSurface ) Cylinder2 =
        Handle( Geom_CylindricalSurface )::DownCast( SecondSurf );
      Standard_Real SecondU = Cylinder2->Cylinder().XAxis().Direction().Angle(
                                                                              gp_Dir( gp_Vec( ProjectPointOnLine( FirstAttach, gp_Lin( Cylinder2->Cylinder().Axis() ) ),
                                                                                              FirstAttach ) ) );
      
      SecondLine = Handle( Geom_Line )::DownCast( Cylinder2->UIso( SecondU ) );
    }
  
  else // it is Cone
    {
      Handle( Geom_ConicalSurface ) Cone2 = Handle( Geom_ConicalSurface )::DownCast( SecondSurf );
      gp_Dir Xdirection = Cone2->Cone().XAxis().Direction() ;
      gp_Dir ToFirstAttach = gp_Dir( gp_Vec(
                                            ProjectPointOnLine( FirstAttach, gp_Lin( Cone2->Cone().Axis() )),
                                            FirstAttach ) );
      
      Standard_Real SecondU = Xdirection.Angle( ToFirstAttach );
      // check sign
      if (! Xdirection.IsEqual( ToFirstAttach, Precision::Angular() ) &&
          ! Xdirection.IsOpposite( ToFirstAttach, Precision::Angular() ) &&
          (Xdirection ^ ToFirstAttach) * Cone2->Cone().Axis().Direction() < 0.0e0)
        SecondU = 2*M_PI - SecondU ;
      
      SecondLine = Handle( Geom_Line )::DownCast( Cone2->UIso( SecondU ) );
      
    }
  if (! (FirstLine->Lin().Direction().IsEqual( SecondLine->Lin().Direction(), Precision::Angular() )) &&
      ! (FirstLine->Lin().Direction().IsOpposite( SecondLine->Lin().Direction(), Precision::Angular() )))
    {
      GeomAPI_ExtremaCurveCurve Intersection( FirstLine, SecondLine );
      Intersection.Points( 1, Center, Center );
      
      if (Center.SquareDistance( FirstAttach ) <= SquareTolerance)
        {
          FirstAttach = AIS::Farest( FirstFace, Center );
          Standard_Real U, V;
          if (FirstSurfType == AIS_KOS_Cylinder)
            {
              ElSLib::Parameters ( (Handle( Geom_CylindricalSurface )::DownCast( FirstSurf ))->Cylinder(),
                                   FirstAttach,
                                   U, V );
              FirstAttach = ElSLib::Value( FirstU, V,
                                           (Handle( Geom_CylindricalSurface )::DownCast( FirstSurf ))
                                           ->Cylinder() );
            }
          else // it is Cone
            {
              ElSLib::Parameters ( (Handle( Geom_ConicalSurface )::DownCast( FirstSurf ))->Cone(),
                                   FirstAttach,
                                   U, V );
              FirstAttach = ElSLib::Value( FirstU, V,
                                           (Handle( Geom_ConicalSurface )::DownCast( FirstSurf ))
                                           ->Cone() );
            }
        }
      gp_Vec FirstVec( Center, FirstAttach );
      FirstDir = gp_Dir( FirstVec );
      
      gp_Ax1 AxisOfRotation( Center, Plane->Pln().Axis().Direction() );
      SecondAttach = FirstAttach.Rotated( AxisOfRotation, Value );
      if (! SecondLine->Lin().Contains( SecondAttach, Precision::Confusion() ))
        {
          AxisOfRotation.Reverse();
          SecondAttach = FirstAttach.Rotated( AxisOfRotation, Value );
        }
      
      Position = FirstAttach.Rotated( AxisOfRotation, Value/2 );

      gp_Vec SecondVec( Center, SecondAttach );
      SecondDir = gp_Dir( SecondVec );
      
    }
  else // FirstLine and SecondLine are coincident
    {
      Position = SecondAttach = FirstAttach;
      FirstDir = FirstLine->Lin().Direction();
      SecondDir = FirstDir;
      Center = Position.Translated( gp_Vec( -FirstDir ) );
      //Position.Translate( gp_Vec( FirstDir ) );
    }
  
  GeomAPI_ProjectPointOnSurf   aProjPnt( SecondAttach, SecondSurf ) ;
  Quantity_Parameter U, V;
  aProjPnt.LowerDistanceParameters( U, V ) ;
  BRepTopAdaptor_FClass2d aClassifier2( SecondFace, Precision::Confusion());
  TopAbs_State State = aClassifier2.Perform( gp_Pnt2d( U, V ), Standard_True );
  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
    {
      Standard_Real MinDist2 = RealLast();
      Standard_Real curdist2;
      gp_Pnt curpnt(0.0,0.0,0.0);
      gp_Pnt Result(0.0,0.0,0.0);
      TopExp_Explorer Explo( SecondFace, TopAbs_VERTEX );
      for (; Explo.More(); Explo.Next())
        {
          curpnt = BRep_Tool::Pnt( TopoDS::Vertex( Explo.Current() ) );
          curdist2 = SecondAttach.SquareDistance( curpnt ) ;
          if (curpnt.SquareDistance( Center ) > SquareTolerance && curdist2 < MinDist2)
            {
              Result = curpnt;
              MinDist2 = curdist2;
            }
        }
      SecondAttach = Result;
    }

  if (! AutomaticPos)
    {  // protection in case this is created using the manual position
      
      
      Position = AIS::ProjectPointOnPlane( SavedPosition, Plane->Pln() );
    }
}


void AIS::ComputeLengthBetweenCurvilinearFaces( const TopoDS_Face &      FirstFace,
                                                const TopoDS_Face &      SecondFace,
                                                Handle( Geom_Surface )&  FirstSurf,
                                                Handle( Geom_Surface )&  SecondSurf,
                                                const Standard_Boolean   AutomaticPos,
                                                Standard_Real &          Value,
                                                gp_Pnt &                 Position,
                                                gp_Pnt &                 FirstAttach,
                                                gp_Pnt &                 SecondAttach,
                                                gp_Dir &                 DirAttach )
{
  GeomAPI_ProjectPointOnSurf aProjector;
  Quantity_Parameter U, V;
#ifndef DEB
  TopAbs_State State = TopAbs_UNKNOWN;
#else
  TopAbs_State State;
#endif
  if (AutomaticPos)
    {
      TopExp_Explorer Explo( FirstFace, TopAbs_VERTEX );
      FirstAttach = BRep_Tool::Pnt( TopoDS::Vertex( Explo.Current() ) );
      aProjector.Init(FirstAttach , FirstSurf );
      FirstAttach = aProjector.NearestPoint();
      aProjector.LowerDistanceParameters( U, V );
    }
  else // posiiton is given
    {
      aProjector.Init( Position, FirstSurf );
      FirstAttach = aProjector.NearestPoint();

      aProjector.LowerDistanceParameters( U, V );
      BRepTopAdaptor_FClass2d aClassifier( FirstFace, Precision::Confusion() );
      State = aClassifier.Perform( gp_Pnt2d( U, V ), ( (FirstSurf->IsUPeriodic() || FirstSurf->IsVPeriodic())?
                                                       Standard_True
                                                       : Standard_False ) );
      if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
        {
          FirstAttach = AIS::Nearest( FirstFace, Position );
          aProjector.Init( FirstAttach, FirstSurf );
          aProjector.LowerDistanceParameters( U, V );
        }
    }

  gp_Vec D1U, D1V;
  FirstSurf->D1( U, V, FirstAttach, D1U, D1V );
  if (D1U.SquareMagnitude() <= SquareTolerance || D1V.SquareMagnitude() <= SquareTolerance)
    {
      FirstAttach = AIS::Farest( FirstFace, FirstAttach );
      aProjector.Init( FirstAttach, FirstSurf );
      aProjector.LowerDistanceParameters( U, V );
      FirstSurf->D1( U, V, FirstAttach, D1U, D1V );
    }
  D1U.Normalize();
  D1V.Normalize();
  DirAttach = gp_Dir( D1U ^ D1V );

  aProjector.Init( FirstAttach, SecondSurf );
#ifndef DEB
  Standard_Integer Index = 0;
#else
  Standard_Integer Index;
#endif
  Quantity_Length MinDist = RealLast();
  gp_Dir LocalDir;
  for (Standard_Integer i = 1; i <= aProjector.NbPoints(); i++)
    {
      aProjector.Parameters( i, U, V );

      SecondSurf->D1( U, V, SecondAttach, D1U, D1V );
      if (D1U.SquareMagnitude() <= SquareTolerance || D1V.SquareMagnitude() <= SquareTolerance)
        LocalDir = gp_Dir( gp_Vec( FirstAttach, aProjector.Point( i ) ) );
      else
        LocalDir = gp_Dir( D1U ^ D1V );
      if (DirAttach.IsParallel( LocalDir, Precision::Angular() ) && aProjector.Distance( i ) < MinDist)
        {
          Index = i;
          MinDist = aProjector.Distance( i );
        }
    }
if (Index == 0)  { 
  SecondAttach = FirstAttach;
} else {
  SecondAttach = aProjector.Point( Index );
  aProjector.Parameters( Index, U, V );

  Value = FirstAttach.Distance( SecondAttach );
  if (Value > Precision::Confusion())
    DirAttach = gp_Dir( gp_Vec( FirstAttach, SecondAttach ) );

  if (AutomaticPos)
    Position.SetXYZ( (FirstAttach.XYZ() + SecondAttach.XYZ()) / 2 );
  else if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
    Position = AIS::ProjectPointOnLine( Position, gp_Lin( FirstAttach, DirAttach ) );

  // Now there is projection of FirstAttach onto SecondSurf in aProjector
  BRepTopAdaptor_FClass2d aClassifier( SecondFace, Precision::Confusion() );
  State = aClassifier.Perform( gp_Pnt2d( U, V ), ( (SecondSurf->IsUPeriodic() || SecondSurf->IsVPeriodic())?
                                                   Standard_True
                                                   : Standard_False ) );
  if (State == TopAbs_OUT || State == TopAbs_UNKNOWN)
    SecondAttach = AIS::Nearest( SecondFace, SecondAttach );
}
}
gp_Pnt AIS::TranslatePointToBound( const gp_Pnt & aPoint, const gp_Dir & aDir, const Bnd_Box & aBndBox )
{
  if (aBndBox.IsOut( aPoint ))
    return aPoint;
  else
    {
      gp_Pnt Result(0.0,0.0,0.0);
      TColStd_Array2OfReal Bound( 1, 3, 1, 2 );
      TColStd_Array1OfReal Origin( 1, 3 );
      TColStd_Array1OfReal Dir( 1, 3 );
      Standard_Real t;
      
      aBndBox.Get( Bound(1,1), Bound(2,1), Bound(3,1), Bound(1,2),  Bound(2,2), Bound(3,2) );
      aPoint.Coord( Origin(1), Origin(2), Origin(3) );
      aDir.Coord( Dir(1), Dir(2), Dir(3) );

      Bnd_Box EnlargedBox = aBndBox;
      EnlargedBox.Enlarge( aBndBox.GetGap() + Precision::Confusion() );

      Standard_Boolean IsFound = Standard_False;
      for (Standard_Integer i = 1; i <= 3; i++)
	{
	  if (Abs( Dir( i ) ) <= gp::Resolution())
	    continue;
	  for (Standard_Integer j = 1; j <= 2; j++)
	    {
	      t = (Bound( i, j ) - Origin( i )) / Dir( i );
	      if (t < 0.0e0)
		continue;
	      Result = aPoint.Translated( gp_Vec( aDir ) * t );
	      if (! EnlargedBox.IsOut( Result ))
		{
		  IsFound = Standard_True;
		  break;
		}
	    }
	  if (IsFound) break;
	}
      return Result;
    }
}


//=======================================================================
//function : InDomain
//purpose  : 
//=======================================================================

Standard_Boolean AIS::InDomain(const Standard_Real fpar,
			       const Standard_Real lpar,
			       const Standard_Real para) 
{
  if (fpar >= 0.) {
    if(lpar > fpar)
      return ((para >= fpar) && (para <= lpar));
    else { // fpar > lpar
      Standard_Real delta = 2*M_PI-fpar;
      Standard_Real lp, par, fp;
      lp = lpar + delta;
      par = para + delta;
      while(lp > 2*M_PI) lp-=2*M_PI;
      while(par > 2*M_PI) par-=2*M_PI;
      fp = 0.;
      return ((par >= fp) && (par <= lp));
    }
      
  }
  if (para >= (fpar+2*M_PI)) return Standard_True;
  if (para <= lpar) return Standard_True;
  return Standard_False;
}

//=======================================================================
//function : DistanceFromApex
//purpose  : calculates parametric length arc of ellipse
//=======================================================================

Standard_Real AIS::DistanceFromApex(const gp_Elips & elips,
				    const gp_Pnt   & Apex,
				    const Standard_Real par)
{
  Standard_Real dist;
  Standard_Real parApex = ElCLib::Parameter ( elips, Apex );
  if(parApex == 0.0 || parApex == M_PI) 
    {//Major case
      if(parApex == 0.0) //pos Apex
	dist = (par < M_PI) ? par : (2*M_PI - par);
      else //neg Apex
	dist = (par < M_PI) ? ( M_PI - par) : ( par - M_PI );
    }
  else 
    {// Minor case
      if(parApex == M_PI / 2) //pos Apex
	{
	  if(par <= parApex + M_PI && par > parApex) // 3/2*M_PI < par < M_PI/2
	    dist = par - parApex;
	  else 
	    { 
	      if(par >  parApex + M_PI) // 3/2*M_PI < par < 2*M_PI
		dist = 2*M_PI - par + parApex;
	      else
		dist = parApex - par; 
	    }
	  }
      else //neg Apex == 3/2*M_PI
	{
	  if(par <= parApex && par >= M_PI/2) // M_PI/2 < par < 3/2*M_PI
	    dist = parApex - par;
	  else
	    {
	      if(par >  parApex) // 3/2*M_PI < par < 2*M_PI
		dist = par - parApex;
	      else
		dist = par + M_PI/2; // 0 < par < M_PI/2
	    }
	}
    }
  return dist;
}

//=======================================================================
//function : NearestApex
//purpose  : 
//=======================================================================

gp_Pnt AIS::NearestApex(const gp_Elips & elips,
			const gp_Pnt   & pApex,
			const gp_Pnt   & nApex,
			const Standard_Real fpara,
			const Standard_Real lpara,
			      Standard_Boolean & IsInDomain)
{
  Standard_Real parP, parN;
  gp_Pnt EndOfArrow(0.0,0.0,0.0);
  IsInDomain = Standard_True;
  parP = ElCLib::Parameter ( elips, pApex );
  if(InDomain(fpara, lpara, parP)) EndOfArrow = pApex;
  else 
    {
      parN = ElCLib::Parameter ( elips, nApex );
      if(InDomain(fpara, lpara, parN)) EndOfArrow = nApex;
      else {
	IsInDomain = Standard_False;
	Standard_Real posd = Min(DistanceFromApex (elips,pApex, fpara), 
				 DistanceFromApex (elips,pApex, lpara));
	Standard_Real negd = Min(DistanceFromApex (elips,nApex, fpara), 
				 DistanceFromApex (elips,nApex, lpara));
	if( posd < negd ) 
	  EndOfArrow = pApex;
	else
	  EndOfArrow = nApex;
      }
    }
  return EndOfArrow;
}

//=======================================================================
//function : ComputeProjEdgePresentation
//purpose  : 
//=======================================================================

void AIS::ComputeProjEdgePresentation( const Handle( Prs3d_Presentation )& aPresentation,
				       const Handle( AIS_Drawer )& aDrawer,
				       const TopoDS_Edge& anEdge,
				       const Handle( Geom_Curve )& ProjCurve,
				       const gp_Pnt& FirstP,
				       const gp_Pnt& LastP,
				       const Quantity_NameOfColor aColor,
				       const Standard_Real aWidth,
				       const Aspect_TypeOfLine aProjTOL,
				       const Aspect_TypeOfLine aCallTOL )
{
  if (!aDrawer->HasWireAspect()){
    aDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aProjTOL,2.));}
  else {
    // CLE
    // const Handle(Prs3d_LineAspect)& li = aDrawer->WireAspect();
    Handle(Prs3d_LineAspect) li = aDrawer->WireAspect();
    // ENDCLE
    li->SetColor(aColor);
    li->SetTypeOfLine(aProjTOL);
    li->SetWidth(aWidth);
  }

  Standard_Real pf, pl;
  TopLoc_Location loc;
  Handle(Geom_Curve) curve;
  Standard_Boolean isInfinite;
  curve = BRep_Tool::Curve(anEdge,loc,pf,pl);
  isInfinite = (Precision::IsInfinite(pf) || Precision::IsInfinite(pl));

  TopoDS_Edge E;

  // Calculate  presentation of the edge
  if (ProjCurve->IsInstance(STANDARD_TYPE(Geom_Line)) ) {
    // CLE
    // const Handle(Geom_Line) & gl = (Handle(Geom_Line)&) ProjCurve;
    Handle(Geom_Line) gl = (Handle(Geom_Line)&) ProjCurve;
    // ENDCLE
    if ( !isInfinite) {
      pf = ElCLib::Parameter(gl->Lin(),FirstP);
      pl = ElCLib::Parameter(gl->Lin(),LastP);
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin(), pf, pl);
      E = MakEd.Edge();
    }
    else {
      BRepBuilderAPI_MakeEdge MakEd(gl->Lin());
      E = MakEd.Edge();
    }
  }
  else if (ProjCurve->IsInstance(STANDARD_TYPE(Geom_Circle)) ) {
    // CLE
    // const Handle(Geom_Circle) & gc = (Handle(Geom_Circle)&) ProjCurve;
    Handle(Geom_Circle) gc = (Handle(Geom_Circle)&) ProjCurve;
    // ENDCLE
    pf = ElCLib::Parameter(gc->Circ(),FirstP);
    pl = ElCLib::Parameter(gc->Circ(),LastP);
    BRepBuilderAPI_MakeEdge MakEd(gc->Circ(),pf, pl);
    E = MakEd.Edge();
  }
  StdPrs_WFDeflectionShape::Add(aPresentation, E, aDrawer);

  //Calculate the presentation of line connections
  aDrawer->WireAspect()->SetTypeOfLine(aCallTOL);
  if (!isInfinite) {
    gp_Pnt ppf(0.0,0.0,0.0), ppl(0.0,0.0,0.0);
    ppf = BRep_Tool::Pnt( TopExp::FirstVertex(TopoDS::Edge(anEdge)));
    ppl = BRep_Tool::Pnt( TopExp::LastVertex(TopoDS::Edge(anEdge)));

    // it is patch!
    if (FirstP.SquareDistance( ppf ) > SquareTolerance)
      {
	BRepBuilderAPI_MakeEdge MakEd1(FirstP, ppf);
	StdPrs_WFDeflectionShape::Add(aPresentation, MakEd1.Edge(), aDrawer);
      }
    else
      {
	BRepBuilderAPI_MakeVertex MakVert1( FirstP );
	StdPrs_WFDeflectionShape::Add(aPresentation, MakVert1.Vertex(), aDrawer);
      }
    if (LastP.SquareDistance( ppl ) > SquareTolerance)
      {
	BRepBuilderAPI_MakeEdge MakEd2(LastP, ppl);
	StdPrs_WFDeflectionShape::Add(aPresentation, MakEd2.Edge(), aDrawer);
      }
    else
      {
	BRepBuilderAPI_MakeVertex MakVert2( LastP );
	StdPrs_WFDeflectionShape::Add(aPresentation, MakVert2.Vertex(), aDrawer);
      }
/*
    BRepBuilderAPI_MakeEdge MakEd1(FirstP, ppf);
    StdPrs_WFDeflectionShape::Add(aPresentation, MakEd1.Edge(), aDrawer);
    BRepBuilderAPI_MakeEdge MakEd2(LastP, ppl);
    StdPrs_WFDeflectionShape::Add(aPresentation, MakEd2.Edge(), aDrawer);
*/
  }
}

//=======================================================================
//function : ComputeProjVertexPresentation
//purpose  : 
//=======================================================================

void AIS::ComputeProjVertexPresentation( const Handle( Prs3d_Presentation )& aPresentation,
				        const Handle( AIS_Drawer )& aDrawer,
				        const TopoDS_Vertex& aVertex,
				        const gp_Pnt& ProjPoint,
				        const Quantity_NameOfColor aColor,
				        const Standard_Real aWidth,
				        const Aspect_TypeOfMarker aProjTOM,
				        const Aspect_TypeOfLine aCallTOL ) 
{
  if (!aDrawer->HasPointAspect()){
    aDrawer->SetPointAspect(new Prs3d_PointAspect(aProjTOM, aColor,1));}
  else {
    // CLE
    // const Handle(Prs3d_PointAspect)& pa = aDrawer->PointAspect();
    Handle(Prs3d_PointAspect) pa = aDrawer->PointAspect();
    // ENDCLE
    pa->SetColor(aColor);
    pa->SetTypeOfMarker(aProjTOM);
  }
  
  // calculate the projection
  StdPrs_Point::Add(aPresentation, new Geom_CartesianPoint(ProjPoint), aDrawer);

  if (!aDrawer->HasWireAspect()){
    aDrawer->SetWireAspect(new Prs3d_LineAspect(aColor,aCallTOL,2.));}
  else {
    // CLE
    // const Handle(Prs3d_LineAspect)& li = aDrawer->WireAspect();
    Handle(Prs3d_LineAspect) li = aDrawer->WireAspect();
    // ENDCLE
    li->SetColor(aColor);
    li->SetTypeOfLine(aCallTOL);
    li->SetWidth(aWidth);
  }
  
  // If the points are not mixed...
  if (!ProjPoint.IsEqual (BRep_Tool::Pnt(aVertex),Precision::Confusion())) {
    // calculate the lines of recall
    BRepBuilderAPI_MakeEdge MakEd(ProjPoint,BRep_Tool::Pnt(aVertex));
    StdPrs_WFDeflectionShape::Add(aPresentation, MakEd.Edge(), aDrawer);
  }
}
