// Created on: 1993-12-15
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

//pmn 26/09/97 Add parameters of approximation in BuildCurve3d
//  Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898

#include <Adaptor3d_CurveOnSurface.hxx>
#include <AdvApprox_ApproxAFunction.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <AppParCurves_MultiCurve.hxx>
#include <Approx_CurvilinearParameter.hxx>
#include <Approx_SameParameter.hxx>
#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_ListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <BRepAdaptor_HCurve2d.hxx>
#include <BRepAdaptor_HSurface.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BSplCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pln.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <TColgp_Array1OfXY.hxx>

#include <algorithm>

// TODO - not thread-safe static variables
static Standard_Real thePrecision = Precision::Confusion();     
static Handle(Geom_Plane) thePlane;

//=======================================================================
// function: BRepLib_ComparePoints
// purpose:  implementation of IsLess() function for two points
//=======================================================================
struct BRepLib_ComparePoints {
  bool operator()(const gp_Pnt& theP1, const gp_Pnt& theP2)
  {
    for (Standard_Integer i = 1; i <= 3; ++i) {
      if (theP1.Coord(i) < theP2.Coord(i)) {
        return Standard_True;
      }
      else if (theP1.Coord(i) > theP2.Coord(i)) {
        return Standard_False;
      }
    }
    return Standard_False;
  }
};


//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

void BRepLib::Precision(const Standard_Real P)
{
  thePrecision = P;
}

//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

Standard_Real  BRepLib::Precision()
{
  return thePrecision;
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

void  BRepLib::Plane(const Handle(Geom_Plane)& P)
{
  thePlane = P;
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

const Handle(Geom_Plane)&  BRepLib::Plane()
{
  if (thePlane.IsNull()) thePlane = new Geom_Plane(gp::XOY());
  return thePlane;
}
//=======================================================================
//function : CheckSameRange
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib::CheckSameRange(const TopoDS_Edge& AnEdge,
  const Standard_Real Tolerance) 
{
  Standard_Boolean  IsSameRange = Standard_True,
    first_time_in = Standard_True ;

  BRep_ListIteratorOfListOfCurveRepresentation an_Iterator
    ((*((Handle(BRep_TEdge)*)&AnEdge.TShape()))->ChangeCurves());

  Standard_Real first, last;
  Standard_Real current_first =0., current_last =0. ;
  Handle(BRep_GCurve) geometric_representation_ptr ;

  while (IsSameRange && an_Iterator.More()) {
    geometric_representation_ptr =
      Handle(BRep_GCurve)::DownCast(an_Iterator.Value());
    if (!geometric_representation_ptr.IsNull()) {

      first = geometric_representation_ptr->First();
      last =  geometric_representation_ptr->Last();
      if (first_time_in ) {
        current_first = first ;
        current_last = last   ;
        first_time_in = Standard_False ;
      }
      else {
        IsSameRange = (Abs(current_first - first) <= Tolerance) 
          && (Abs(current_last -last) <= Tolerance ) ;
      }
    }
    an_Iterator.Next() ;
  }
  return IsSameRange ;
}

//=======================================================================
//function : SameRange
//purpose  : 
//=======================================================================

void BRepLib::SameRange(const TopoDS_Edge& AnEdge,
  const Standard_Real Tolerance) 
{
  BRep_ListIteratorOfListOfCurveRepresentation an_Iterator
    ((*((Handle(BRep_TEdge)*)&AnEdge.TShape()))->ChangeCurves());

  Handle(Geom2d_Curve) Curve2dPtr, Curve2dPtr2, NewCurve2dPtr, NewCurve2dPtr2;
  TopLoc_Location LocalLoc ;

  Standard_Boolean first_time_in = Standard_True,
    has_curve,
    has_closed_curve ;
  Handle(BRep_GCurve) geometric_representation_ptr ;
  Standard_Real first,
    current_first,
    last,
    current_last ;

  const Handle(Geom_Curve) C = BRep_Tool::Curve(AnEdge,
    LocalLoc,
    current_first,
    current_last);
  if (!C.IsNull()) {
    first_time_in = Standard_False ;
  }

  while (an_Iterator.More()) {
    geometric_representation_ptr =
      Handle(BRep_GCurve)::DownCast(an_Iterator.Value());
    if (! geometric_representation_ptr.IsNull()) {
      has_closed_curve =
        has_curve = Standard_False ;
      first = geometric_representation_ptr->First();
      last =  geometric_representation_ptr->Last();
      if (geometric_representation_ptr->IsCurveOnSurface()) {
        Curve2dPtr = geometric_representation_ptr->PCurve() ; 
        has_curve = Standard_True ;
      }
      if (geometric_representation_ptr->IsCurveOnClosedSurface()) {
        Curve2dPtr2 = geometric_representation_ptr->PCurve2() ;
        has_closed_curve = Standard_True ;
      }
      if (has_curve || has_closed_curve) {
        if (first_time_in) {
          current_first = first ;
          current_last = last ;
          first_time_in = Standard_False ;
        }

        if (Abs(first - current_first) > Precision::Confusion() ||
          Abs(last - current_last) > Precision::Confusion() )
        {
          if (has_curve)
          {
            GeomLib::SameRange(Tolerance,
              Curve2dPtr,
              geometric_representation_ptr->First(),
              geometric_representation_ptr->Last(),
              current_first,
              current_last,
              NewCurve2dPtr);
            geometric_representation_ptr->PCurve(NewCurve2dPtr) ;
          }
          if (has_closed_curve)
          {
            GeomLib::SameRange(Tolerance,
              Curve2dPtr2,
              geometric_representation_ptr->First(),
              geometric_representation_ptr->Last(),
              current_first,
              current_last,
              NewCurve2dPtr2);
            geometric_representation_ptr->PCurve2(NewCurve2dPtr2) ;
          }
        }
      }
    }
    an_Iterator.Next() ;
  }
  BRep_Builder B;
  B.Range(TopoDS::Edge(AnEdge),
    current_first,
    current_last) ;

  B.SameRange(AnEdge,
    Standard_True) ;
}

//=======================================================================
//function : EvaluateMaxSegment
//purpose  : return MaxSegment to pass in approximation, if MaxSegment==0 provided
//=======================================================================

static Standard_Integer evaluateMaxSegment(const Standard_Integer aMaxSegment,
  const Adaptor3d_CurveOnSurface& aCurveOnSurface)
{
  if (aMaxSegment != 0) return aMaxSegment;

  Handle(Adaptor3d_HSurface) aSurf   = aCurveOnSurface.GetSurface();
  Handle(Adaptor2d_HCurve2d) aCurv2d = aCurveOnSurface.GetCurve();

  Standard_Real aNbSKnots = 0, aNbC2dKnots = 0;

  if (aSurf->GetType() == GeomAbs_BSplineSurface) {
    Handle(Geom_BSplineSurface) aBSpline = aSurf->BSpline();
    aNbSKnots = Max(aBSpline->NbUKnots(), aBSpline->NbVKnots());
  }
  if (aCurv2d->GetType() == GeomAbs_BSplineCurve) {
    aNbC2dKnots = aCurv2d->NbKnots();
  }
  Standard_Integer aReturn = (Standard_Integer) (  30 + Max(aNbSKnots, aNbC2dKnots) ) ;
  return aReturn;
}

//=======================================================================
//function : BuildCurve3d
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib::BuildCurve3d(const TopoDS_Edge& AnEdge,
  const Standard_Real Tolerance,
  const GeomAbs_Shape Continuity,
  const Standard_Integer MaxDegree,
  const Standard_Integer MaxSegment)
{
  Standard_Integer //ErrorCode,
    //                   ReturnCode = 0,
    ii,
    //                   num_knots,
    jj;

  TopLoc_Location LocalLoc,L[2],LC;
  Standard_Real f,l,fc,lc, first[2], last[2],
    tolerance,
    max_deviation,
    average_deviation ;
  Handle(Geom2d_Curve) Curve2dPtr, Curve2dArray[2]  ;
  Handle(Geom_Surface) SurfacePtr, SurfaceArray[2]  ;

  Standard_Integer not_done ;
  // if the edge has a 3d curve returns true


  const Handle(Geom_Curve) C = BRep_Tool::Curve(AnEdge,LocalLoc,f,l);
  if (!C.IsNull()) 
    return Standard_True;
  //
  // this should not exists but UpdateEdge makes funny things 
  // if the edge is not same range 
  //
  if (! CheckSameRange(AnEdge,
    Precision::Confusion())) {
      SameRange(AnEdge,
        Tolerance) ;
  }



  // search a curve on a plane
  Handle(Geom_Surface) S;
  Handle(Geom2d_Curve) PC;
  Standard_Integer i = 0;
  Handle(Geom_Plane) P;
  not_done = 1 ;

  while (not_done) {
    i++;
    BRep_Tool::CurveOnSurface(AnEdge,PC,S,LocalLoc,f,l,i);
    Handle(Geom_RectangularTrimmedSurface) RT = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
    if ( RT.IsNull()) {
      P = Handle(Geom_Plane)::DownCast(S);
    }
    else {
      P = Handle(Geom_Plane)::DownCast(RT->BasisSurface());
    }
    not_done = ! (S.IsNull() || !P.IsNull()) ;
  }
  if (! P.IsNull()) {
    // compute the 3d curve
    gp_Ax2 axes = P->Position().Ax2();
    Handle(Geom_Curve) C3d = GeomLib::To3d(axes,PC);
    if (C3d.IsNull())
      return Standard_False;
    // update the edge
    Standard_Real First, Last;

    BRep_Builder B;
    B.UpdateEdge(AnEdge,C3d,LocalLoc,0.0e0);
    BRep_Tool::Range(AnEdge, S, LC, First, Last);
    B.Range(AnEdge, First, Last); //Do not forget 3D range.(PRO6412)
  }
  else {
    //
    // compute the 3d curve using existing surface
    //
    fc = f ;
    lc = l ;
    if (!BRep_Tool::Degenerated(AnEdge)) {
      jj = 0 ;
      for (ii = 0 ; ii < 3 ; ii++ ) {
        BRep_Tool::CurveOnSurface(TopoDS::Edge(AnEdge),
          Curve2dPtr,
          SurfacePtr,
          LocalLoc,
          fc,
          lc,
          ii) ;

        if (!Curve2dPtr.IsNull() && jj < 2){
          Curve2dArray[jj] = Curve2dPtr ;
          SurfaceArray[jj] = SurfacePtr ;
          L[jj] = LocalLoc ;
          first[jj] = fc ;
          last[jj] = lc ;
          jj += 1 ;
        }
      }
      f = first[0] ;
      l = last[0] ;
      Curve2dPtr = Curve2dArray[0] ;
      SurfacePtr = SurfaceArray[0] ;

      Geom2dAdaptor_Curve     AnAdaptor3dCurve2d (Curve2dPtr, f, l) ;
      GeomAdaptor_Surface     AnAdaptor3dSurface (SurfacePtr) ;
      Handle(Geom2dAdaptor_HCurve) AnAdaptor3dCurve2dPtr =
        new Geom2dAdaptor_HCurve(AnAdaptor3dCurve2d) ;
      Handle(GeomAdaptor_HSurface) AnAdaptor3dSurfacePtr =
        new GeomAdaptor_HSurface (AnAdaptor3dSurface) ;
      Adaptor3d_CurveOnSurface  CurveOnSurface( AnAdaptor3dCurve2dPtr,
        AnAdaptor3dSurfacePtr) ;

      Handle(Geom_Curve) NewCurvePtr ;

      GeomLib::BuildCurve3d(Tolerance,
        CurveOnSurface,
        f,
        l,
        NewCurvePtr,
        max_deviation,
        average_deviation,
        Continuity,
        MaxDegree,
        evaluateMaxSegment(MaxSegment,CurveOnSurface)) ;
      BRep_Builder B;	
      tolerance = BRep_Tool::Tolerance(AnEdge) ;
      //Patch
      //max_deviation = Max(tolerance, max_deviation) ;
      max_deviation = Max( tolerance, Tolerance );
      if (NewCurvePtr.IsNull())
        return Standard_False;
      B.UpdateEdge(TopoDS::Edge(AnEdge),
        NewCurvePtr,
        L[0],
        max_deviation) ;
      if (jj == 1 ) {
        //
        // if there is only one curve on surface attached to the edge
        // than it can be qualified sameparameter
        //
        B.SameParameter(TopoDS::Edge(AnEdge),
          Standard_True) ;
      }
    }
    else {
      return Standard_False ;
    }

  }         
  return Standard_True;
}
//=======================================================================
//function : BuildCurves3d
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib::BuildCurves3d(const TopoDS_Shape& S) 

{
  return BRepLib::BuildCurves3d(S,
    1.0e-5) ;
}

//=======================================================================
//function : BuildCurves3d
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib::BuildCurves3d(const TopoDS_Shape& S,
  const Standard_Real Tolerance,
  const GeomAbs_Shape Continuity,
  const Standard_Integer MaxDegree,
  const Standard_Integer MaxSegment)
{
  Standard_Boolean boolean_value,
    ok = Standard_True;
  TopTools_MapOfShape a_counter ;
  TopExp_Explorer ex(S,TopAbs_EDGE);

  while (ex.More()) {
    if (a_counter.Add(ex.Current())) {
      boolean_value = 
        BuildCurve3d(TopoDS::Edge(ex.Current()),
        Tolerance, Continuity,
        MaxDegree, MaxSegment);
      ok = ok && boolean_value ;
    }
    ex.Next();
  }
  return ok;
}
//=======================================================================
//function : UpdateEdgeTolerance
//purpose  : 
//=======================================================================

Standard_Boolean  BRepLib::UpdateEdgeTol(const TopoDS_Edge& AnEdge,
  const Standard_Real MinToleranceRequested,
  const Standard_Real MaxToleranceToCheck)
{

  Standard_Integer curve_on_surface_index,
    curve_index,
    not_done,
    has_closed_curve,
    has_curve,
    jj,
    ii,
    geom_reference_curve_flag = 0,
    max_sampling_points = 90,
    min_sampling_points = 30 ;

  Standard_Real factor = 100.0e0,
    //     sampling_array[2],
    safe_factor = 1.4e0,
    current_last,
    current_first,
    max_distance,
    coded_edge_tolerance,
    edge_tolerance = 0.0e0 ;
  Handle(TColStd_HArray1OfReal) parameters_ptr ;
  Handle(BRep_GCurve) geometric_representation_ptr ;

  if (BRep_Tool::Degenerated(AnEdge)) return Standard_False ;
  coded_edge_tolerance = BRep_Tool::Tolerance(AnEdge) ;
  if (coded_edge_tolerance > MaxToleranceToCheck) return Standard_False ;

  const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&AnEdge.TShape());
  BRep_ListOfCurveRepresentation& list_curve_rep = TE->ChangeCurves() ;
  BRep_ListIteratorOfListOfCurveRepresentation an_iterator(list_curve_rep),
    second_iterator(list_curve_rep) ;
  Handle(Geom2d_Curve) curve2d_ptr, new_curve2d_ptr;
  Handle(Geom_Surface) surface_ptr ;
  TopLoc_Location local_location ;
  GCPnts_QuasiUniformDeflection  a_sampler ;
  GeomAdaptor_Curve  geom_reference_curve ;
  Adaptor3d_CurveOnSurface  curve_on_surface_reference ; 
  Handle(Geom_Curve) C = BRep_Tool::Curve(AnEdge,
    local_location,
    current_first,
    current_last);
  curve_on_surface_index = -1 ;
  if (!C.IsNull()) {
    if (! local_location.IsIdentity()) {
      C = Handle(Geom_Curve)::
        DownCast(C-> Transformed(local_location.Transformation()) ) ;
    }
    geom_reference_curve.Load(C) ;
    geom_reference_curve_flag = 1 ;
    a_sampler.Initialize(geom_reference_curve,
      MinToleranceRequested * factor,
      current_first,
      current_last) ;
  }
  else {
    not_done = 1 ;
    curve_on_surface_index = 0 ;  

    while (not_done && an_iterator.More()) {
      geometric_representation_ptr =
        Handle(BRep_GCurve)::DownCast(second_iterator.Value());
      if (!geometric_representation_ptr.IsNull() 
        && geometric_representation_ptr->IsCurveOnSurface()) {
          curve2d_ptr = geometric_representation_ptr->PCurve() ;
          local_location = geometric_representation_ptr->Location() ;
          current_first = geometric_representation_ptr->First();
          //first = geometric_representation_ptr->First();
          current_last =  geometric_representation_ptr->Last();
          // must be inverted 
          //
          if (! local_location.IsIdentity() ) {
            surface_ptr = Handle(Geom_Surface)::
              DownCast( geometric_representation_ptr->Surface()->
              Transformed(local_location.Transformation()) ) ;
          }
          else {
            surface_ptr = 
              geometric_representation_ptr->Surface() ;
          }
          not_done = 0 ;
      }
      curve_on_surface_index += 1 ;
    }
    Geom2dAdaptor_Curve     AnAdaptor3dCurve2d (curve2d_ptr) ;
    GeomAdaptor_Surface     AnAdaptor3dSurface (surface_ptr) ;
    Handle(Geom2dAdaptor_HCurve) AnAdaptor3dCurve2dPtr =
      new Geom2dAdaptor_HCurve(AnAdaptor3dCurve2d) ;
    Handle(GeomAdaptor_HSurface) AnAdaptor3dSurfacePtr =
      new GeomAdaptor_HSurface (AnAdaptor3dSurface) ;
    curve_on_surface_reference.Load (AnAdaptor3dCurve2dPtr, AnAdaptor3dSurfacePtr);
    a_sampler.Initialize(curve_on_surface_reference,
      MinToleranceRequested * factor,
      current_first,
      current_last) ;
  }
  TColStd_Array1OfReal   sampling_parameters(1,a_sampler.NbPoints()) ;
  for (ii = 1 ; ii <= a_sampler.NbPoints() ; ii++) {
    sampling_parameters(ii) = a_sampler.Parameter(ii) ;
  }
  if (a_sampler.NbPoints() < min_sampling_points) {
    GeomLib::DensifyArray1OfReal(min_sampling_points,
      sampling_parameters,
      parameters_ptr) ;
  }
  else if (a_sampler.NbPoints() > max_sampling_points) {
    GeomLib::RemovePointsFromArray(max_sampling_points,
      sampling_parameters,
      parameters_ptr) ; 
  }
  else {
    jj = 1 ;
    parameters_ptr =
      new TColStd_HArray1OfReal(1,sampling_parameters.Length()) ;
    for (ii = sampling_parameters.Lower() ; ii <= sampling_parameters.Upper() ; ii++) {
      parameters_ptr->ChangeArray1()(jj) =
        sampling_parameters(ii) ;
      jj +=1 ;
    }
  }

  curve_index = 0 ;

  while (second_iterator.More()) {
    geometric_representation_ptr =
      Handle(BRep_GCurve)::DownCast(second_iterator.Value());
    if (! geometric_representation_ptr.IsNull() && 
      curve_index != curve_on_surface_index) {
        has_closed_curve =
          has_curve = Standard_False ;
        //	first = geometric_representation_ptr->First();
        //	last =  geometric_representation_ptr->Last();
        local_location = geometric_representation_ptr->Location() ;
        if (geometric_representation_ptr->IsCurveOnSurface()) {
          curve2d_ptr = geometric_representation_ptr->PCurve() ; 
          has_curve = Standard_True ;
        }
        if (geometric_representation_ptr->IsCurveOnClosedSurface()) {
          curve2d_ptr = geometric_representation_ptr->PCurve2() ;
          has_closed_curve = Standard_True ;
        }

        if (has_curve ||
          has_closed_curve) {
            if (! local_location.IsIdentity() ) {
              surface_ptr = Handle(Geom_Surface)::
                DownCast( geometric_representation_ptr->Surface()->
                Transformed(local_location.Transformation()) ) ;
            }
            else {
              surface_ptr = 
                geometric_representation_ptr->Surface() ;
            }
            Geom2dAdaptor_Curve     an_adaptor_curve2d (curve2d_ptr) ;
            GeomAdaptor_Surface     an_adaptor_surface(surface_ptr) ;
            Handle(Geom2dAdaptor_HCurve) an_adaptor_curve2d_ptr =
              new Geom2dAdaptor_HCurve(an_adaptor_curve2d) ;
            Handle(GeomAdaptor_HSurface) an_adaptor_surface_ptr =
              new GeomAdaptor_HSurface (an_adaptor_surface) ;
            Adaptor3d_CurveOnSurface a_curve_on_surface(an_adaptor_curve2d_ptr,
              an_adaptor_surface_ptr) ;

            if (BRep_Tool::SameParameter(AnEdge)) {

              GeomLib::EvalMaxParametricDistance(a_curve_on_surface,
                geom_reference_curve,
                MinToleranceRequested,
                parameters_ptr->Array1(),
                max_distance) ;
            }
            else if (geom_reference_curve_flag) {
              GeomLib::EvalMaxDistanceAlongParameter(a_curve_on_surface,
                geom_reference_curve,
                MinToleranceRequested,
                parameters_ptr->Array1(),
                max_distance) ;
            }
            else {

              GeomLib::EvalMaxDistanceAlongParameter(a_curve_on_surface,
                curve_on_surface_reference,
                MinToleranceRequested,
                parameters_ptr->Array1(),
                max_distance) ;
            }
            max_distance *= safe_factor ;
            edge_tolerance = Max(max_distance, edge_tolerance) ;
        }


    }
    curve_index += 1 ;
    second_iterator.Next() ; 
  }

  TE->Tolerance(edge_tolerance);
  return Standard_True ;

}
//=======================================================================
//function : UpdateEdgeTolerance
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib::UpdateEdgeTolerance(const TopoDS_Shape& S,
  const Standard_Real MinToleranceRequested,
  const Standard_Real MaxToleranceToCheck) 
{
  TopExp_Explorer ex(S,TopAbs_EDGE);
  TopTools_MapOfShape  a_counter ;

  Standard_Boolean     return_status = Standard_False,
    local_flag ;

  while (ex.More()) {
    if (a_counter.Add(ex.Current())) {
      local_flag =
        BRepLib::UpdateEdgeTol(TopoDS::Edge(ex.Current()),
        MinToleranceRequested,
        MaxToleranceToCheck) ;
      if (local_flag && ! return_status) {
        return_status = Standard_True ;
      }
    }
    ex.Next();
  }
  return return_status ;
}

//=======================================================================
//function : SetEdgeTol
//purpose  : 
//=======================================================================

static void SetEdgeTol(const TopoDS_Edge& E,
  const TopoDS_Face& F)
{
  TopLoc_Location L;
  const Handle(Geom_Surface)& S = BRep_Tool::Surface(F,L);
  TopLoc_Location l = L.Predivided(E.Location());

  const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->ChangeCurves());

  while (itcr.More()) {
    const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
    if(cr->IsCurveOnSurface(S,l)) return;
    itcr.Next();
  }

  Handle(Geom_Plane) GP;
  Handle(Geom_RectangularTrimmedSurface) GRTS;
  GRTS = Handle(Geom_RectangularTrimmedSurface)::DownCast(S);
  if(!GRTS.IsNull())
    GP = Handle(Geom_Plane)::DownCast(GRTS->BasisSurface());
  else
    GP = Handle(Geom_Plane)::DownCast(S);

  Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();

  TopLoc_Location LC;
  Standard_Real First, Last;
  GeomAdaptor_Curve& GAC = HC->ChangeCurve();
  GAC.Load(BRep_Tool::Curve(E,LC,First,Last));
  LC = L.Predivided(LC);

  if (!LC.IsIdentity()) {
    GP = Handle(Geom_Plane)::DownCast(
      GP->Transformed(LC.Transformation()));
  }
  GeomAdaptor_Surface& GAS = HS->ChangeSurface();
  GAS.Load(GP);

  ProjLib_ProjectedCurve Proj(HS,HC);
  Handle(Geom2d_Curve) pc = Geom2dAdaptor::MakeCurve(Proj);

  gp_Pln pln = GAS.Plane();
  Standard_Real d2 = 0.;
  Standard_Integer nn = 23;
  Standard_Real unsurnn = 1./nn;
  for(Standard_Integer i = 0; i <= nn; i++){
    Standard_Real t = unsurnn*i;
    Standard_Real u = First*(1.-t) + Last*t;
    gp_Pnt Pc3d = HC->Value(u);
    gp_Pnt2d p2d = pc->Value(u);
    gp_Pnt Pcons = ElSLib::Value(p2d.X(),p2d.Y(),pln);
    Standard_Real eps = Max(Pc3d.XYZ().SquareModulus(), Pcons.XYZ().SquareModulus());
    eps = Epsilon(eps);
    Standard_Real temp = Pc3d.SquareDistance(Pcons);
    if(temp <= eps)
    {
      temp = 0.;
    }
    if(temp > d2) d2 = temp;
  }
  d2 = 1.5*sqrt(d2);
  TE->UpdateTolerance(d2);
}

//=======================================================================
//function : SameParameter
//purpose  : 
//=======================================================================
void  BRepLib::SameParameter(const TopoDS_Shape& S,
  const Standard_Real Tolerance,
  const Standard_Boolean forced) 
{
  TopExp_Explorer ex(S,TopAbs_EDGE);
  TopTools_MapOfShape  Done;
  BRep_Builder brB;

  while (ex.More()) {
    if (Done.Add(ex.Current())) {
      if (forced) {
        brB.SameRange(TopoDS::Edge(ex.Current()), Standard_False);
        brB.SameParameter(TopoDS::Edge(ex.Current()), Standard_False);
      }
      BRepLib::SameParameter(TopoDS::Edge(ex.Current()),Tolerance);
    }
    ex.Next();
  }

  Done.Clear();
  BRepAdaptor_Surface BS;
  for(ex.Init(S,TopAbs_FACE); ex.More(); ex.Next()){
    const TopoDS_Face& curface = TopoDS::Face(ex.Current());
    if(!Done.Add(curface)) continue;
    BS.Initialize(curface);
    if(BS.GetType() != GeomAbs_Plane) continue;
    TopExp_Explorer ex2;
    for(ex2.Init(curface,TopAbs_EDGE); ex2.More(); ex2.Next()){
      const TopoDS_Edge& E = TopoDS::Edge(ex2.Current());
      SetEdgeTol(E,curface);
    }
  }
  BRepLib::UpdateTolerances(S);
}

//================================================================
//function : SameParameter
//WARNING  : New spec DUB LBO 9/9/97.
//  Recode in the edge the best tolerance found, 
//  for vertex extremities it is required to find something else
//================================================================
static Standard_Boolean EvalTol(const Handle(Geom2d_Curve)& pc,
  const Handle(Geom_Surface)& s,
  const GeomAdaptor_Curve&    gac,
  const Standard_Real         tol,
  Standard_Real&              tolbail)
{
  Standard_Integer ok = 0;
  Standard_Real f = gac.FirstParameter();
  Standard_Real l = gac.LastParameter();
  Extrema_LocateExtPC Projector;
  Projector.Initialize(gac,f,l,tol);
  Standard_Real u,v;
  gp_Pnt p;
  tolbail = tol;
  for(Standard_Integer i = 1; i <= 5; i++){
    Standard_Real t = i/6.;
    t = (1.-t) * f + t * l;
    pc->Value(t).Coord(u,v);
    p = s->Value(u,v);
    Projector.Perform(p,t);
    if (Projector.IsDone()) {
      Standard_Real dist2 = Projector.SquareDistance();
      if(dist2 > tolbail * tolbail) tolbail = sqrt (dist2);
      ok++;
    }
  }
  return (ok > 2);
}

static Standard_Real ComputeTol(const Handle(Adaptor3d_HCurve)& c3d,
  const Handle(Adaptor2d_HCurve2d)& c2d,
  const Handle(Adaptor3d_HSurface)& surf,
  const Standard_Integer        nbp)

{

  TColStd_Array1OfReal dist(1,nbp+10);
  dist.Init(-1.);

  //Adaptor3d_CurveOnSurface  cons(c2d,surf);
  Standard_Real uf = surf->FirstUParameter(), ul = surf->LastUParameter(),
                vf = surf->FirstVParameter(), vl = surf->LastVParameter();
  Standard_Real du = 0.01 * (ul - uf), dv = 0.01 * (vl - vf);
  Standard_Boolean isUPeriodic = surf->IsUPeriodic(), isVPeriodic = surf->IsVPeriodic();
  Standard_Real DSdu = 1./surf->UResolution(1.), DSdv = 1./surf->VResolution(1.);
  Standard_Real d2 = 0.;
  Standard_Real first = c3d->FirstParameter();
  Standard_Real last  = c3d->LastParameter();
  Standard_Real dapp = -1.;
  Standard_Integer i = 0;
  for(i = 0; i <= nbp; i++){
    const Standard_Real t = IntToReal(i)/IntToReal(nbp);
    const Standard_Real u = first*(1.-t) + last*t;
    gp_Pnt Pc3d = c3d->Value(u);
    gp_Pnt2d Puv = c2d->Value(u);
    if(!isUPeriodic)
    {
      if(Puv.X() < uf - du)
      {
        dapp = Max(dapp, DSdu * (uf - Puv.X()));
        continue;
      }
      else if(Puv.X() > ul + du)
      {
        dapp = Max(dapp, DSdu * (Puv.X() - ul));
        continue;
      }
    }
    if(!isVPeriodic)
    {
      if(Puv.Y() < vf - dv)
      {
        dapp = Max(dapp, DSdv * (vf - Puv.Y()));
        continue;
      }
      else if(Puv.Y() > vl + dv)
      {
        dapp = Max(dapp, DSdv * (Puv.Y() - vl));
        continue;
      }
    }
    gp_Pnt Pcons = surf->Value(Puv.X(), Puv.Y());
    if (Precision::IsInfinite(Pcons.X()) ||
        Precision::IsInfinite(Pcons.Y()) ||
        Precision::IsInfinite(Pcons.Z()))
    {
        d2=Precision::Infinite();
        break;
    }
    Standard_Real temp = Pc3d.SquareDistance(Pcons);


    dist(i+1) = temp;


    if(temp > d2) d2 = temp;
  }

  if(Precision::IsInfinite(d2))
  {
    return d2;
  }

  d2 = Sqrt(d2);
  if(dapp > d2)
  {
    return dapp;
  }

  Standard_Boolean ana = Standard_False;
  Standard_Real D2 = 0;
  Standard_Integer N1 = 0;
  Standard_Integer N2 = 0;
  Standard_Integer N3 = 0;

  for( i = 1; i<= nbp+10; i++)
    if( dist(i) > 0 ) {
      if( dist(i) < 1.0 ) N1++;
      else N2++;
    }

    if( N1 > N2 && N2 != 0 ) N3 = 100*N2/(N1+N2);
    if( N3 < 10 && N3 != 0 ) {
      ana = Standard_True;
      for( i = 1; i<= nbp+10; i++)
        if( dist(i) > 0 && dist(i) < 1.0 )
          if( dist(i) > D2 ) D2 = dist(i);
    }

    //d2 = 1.5*sqrt(d2);
    d2 = (!ana) ? 1.5 * d2 : 1.5*sqrt(D2);
    if(d2<1.e-7) d2 = 1.e-7;

    return d2;
}

void BRepLib::SameParameter(const TopoDS_Edge&  AnEdge,
  const Standard_Real Tolerance)
{
  if (BRep_Tool::SameParameter(AnEdge)) return;

  const Standard_Integer NCONTROL = 22;

  Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve();
  Handle(Geom2dAdaptor_HCurve) HC2d = new Geom2dAdaptor_HCurve();
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  GeomAdaptor_Curve& GAC = HC->ChangeCurve();
  Geom2dAdaptor_Curve& GAC2d = HC2d->ChangeCurve2d();
  GeomAdaptor_Surface& GAS = HS->ChangeSurface();
  Standard_Real f3d =0.,l3d =0.;
  TopLoc_Location L3d;
  Handle(Geom_Curve) C3d;

  const Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*) &AnEdge.TShape());
  BRep_ListOfCurveRepresentation& CList = TE->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation It(CList);

  Standard_Boolean NotDone = Standard_True;

  while (NotDone && It.More()) {
    Handle(BRep_GCurve) GCurve = Handle(BRep_GCurve)::DownCast(It.Value());
    if (!GCurve.IsNull() && GCurve->IsCurve3D()) {
      C3d = GCurve->Curve3D() ;
      f3d = GCurve->First();
      l3d = GCurve->Last();
      L3d = GCurve->Location() ;
      NotDone = Standard_False;
    } 
    It.Next() ;
  }

  if(C3d.IsNull()) return;

  // modified by NIZHNY-OCC486  Tue Aug 27 17:15:13 2002 :
  Standard_Boolean m_TrimmedPeriodical = Standard_False;
  Handle(Standard_Type) TheType = C3d->DynamicType();
  if( TheType == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    Handle(Geom_Curve) gtC (Handle(Geom_TrimmedCurve)::DownCast (C3d)->BasisCurve());
    m_TrimmedPeriodical = gtC->IsPeriodic();
  }
  // modified by NIZHNY-OCC486  Tue Aug 27 17:15:17 2002 .

  BRep_Builder B;
  if(!C3d->IsPeriodic()) {
    Standard_Real Udeb = C3d->FirstParameter();
    Standard_Real Ufin = C3d->LastParameter();
    // modified by NIZHNY-OCC486  Tue Aug 27 17:17:14 2002 :
    //if (Udeb > f3d) f3d = Udeb;
    //if (l3d > Ufin) l3d = Ufin;
    if(!m_TrimmedPeriodical)
    {
      if (Udeb > f3d) f3d = Udeb;
      if (l3d > Ufin) l3d = Ufin;
    }
    // modified by NIZHNY-OCC486  Tue Aug 27 17:17:55 2002 .
  }
  if(!L3d.IsIdentity()){
    C3d = Handle(Geom_Curve)::DownCast(C3d->Transformed(L3d.Transformation()));
  }
  GAC.Load(C3d,f3d,l3d);

  Standard_Boolean IsSameP = 1;
  Standard_Real maxdist = 0.;

  //  Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898 Begin
  Standard_Real anEdgeTol = BRep_Tool::Tolerance(AnEdge);
  //  Modified by skv - Thu Jun  3 12:39:20 2004 OCC5898 End
  Standard_Boolean SameRange = BRep_Tool::SameRange(AnEdge);
  Standard_Boolean YaPCu = Standard_False;
  const Standard_Real BigError = 1.e10;
  It.Initialize(CList);

  while (It.More()) {
    Standard_Boolean isANA = Standard_False;
    Standard_Boolean isBSP = Standard_False;
    Handle(BRep_GCurve) GCurve = Handle(BRep_GCurve)::DownCast(It.Value());
    Handle(Geom2d_Curve) PC[2];
    Handle(Geom_Surface) S;
    if (!GCurve.IsNull() && GCurve->IsCurveOnSurface()) {
      YaPCu = Standard_True;
      PC[0] = GCurve->PCurve();
      TopLoc_Location PCLoc = GCurve->Location();
      S = GCurve->Surface();
      if (!PCLoc.IsIdentity() ) {
        S = Handle(Geom_Surface)::DownCast(S->Transformed(PCLoc.Transformation()));
      }

      GAS.Load(S);
      if (GCurve->IsCurveOnClosedSurface()) {
        PC[1] = GCurve->PCurve2();
      }

      // Eval tol2d to compute SameRange
      Standard_Real UResol = Max(GAS.UResolution(Tolerance), Precision::PConfusion());
      Standard_Real VResol = Max(GAS.VResolution(Tolerance), Precision::PConfusion());
      Standard_Real Tol2d  = Min(UResol, VResol);
      for(Standard_Integer i = 0; i < 2; i++){
        Handle(Geom2d_Curve) curPC = PC[i];
        Standard_Boolean updatepc = 0;
        if(curPC.IsNull()) break;
        if(!SameRange){
          GeomLib::SameRange(Tol2d,
            PC[i],GCurve->First(),GCurve->Last(),
            f3d,l3d,curPC);

          updatepc = (curPC != PC[i]);

        }
        Standard_Boolean goodpc = 1;
        GAC2d.Load(curPC,f3d,l3d);

        Standard_Real error = ComputeTol(HC, HC2d, HS, NCONTROL);

        if(error > BigError)
        {
          maxdist = error;
          break;
        }

        if(GAC2d.GetType() == GeomAbs_BSplineCurve && 
          GAC2d.Continuity() == GeomAbs_C0) {
            Handle(Geom2d_BSplineCurve) bs2d = GAC2d.BSpline();
            Handle(Geom2d_BSplineCurve) bs2dsov = bs2d;
            Standard_Real fC0 = bs2d->FirstParameter(), lC0 = bs2d->LastParameter();
            Standard_Boolean repar = Standard_True;
            gp_Pnt2d OriginPoint;
            bs2d->D0(fC0, OriginPoint);
            Geom2dConvert::C0BSplineToC1BSplineCurve(bs2d, Tol2d);
            isBSP = Standard_True; 

            if(bs2d->IsPeriodic()) { // -------- IFV, Jan 2000
              gp_Pnt2d NewOriginPoint;
              bs2d->D0(bs2d->FirstParameter(), NewOriginPoint);
              if(Abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion() ||
                Abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion()    ) {

                  TColStd_Array1OfReal Knotbs2d (1, bs2d->NbKnots());
                  bs2d->Knots(Knotbs2d);

                  for(Standard_Integer Index = 1; Index <= bs2d->NbKnots(); Index++) {
                    bs2d->D0(Knotbs2d(Index), NewOriginPoint);
                    if(Abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion() ||
                      Abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion()    ) continue;

                    bs2d->SetOrigin(Index);
                    break;
                  }
              }
            }

            if(bs2d->Continuity() == GeomAbs_C0) {
              Standard_Real tolbail;
              if(EvalTol(curPC,S,GAC,Tolerance,tolbail)){
                bs2d = bs2dsov;
                Standard_Real UResbail = GAS.UResolution(tolbail);
                Standard_Real VResbail = GAS.VResolution(tolbail);
                Standard_Real Tol2dbail  = Min(UResbail,VResbail);
                bs2d->D0(bs2d->FirstParameter(), OriginPoint); 

                Standard_Integer nbp = bs2d->NbPoles();
                TColgp_Array1OfPnt2d poles(1,nbp);
                bs2d->Poles(poles);
                gp_Pnt2d p = poles(1), p1;
                Standard_Real d = Precision::Infinite();
                for(Standard_Integer ip = 2; ip <= nbp; ip++) {
                  p1 = poles(ip);
                  d = Min(d,p.SquareDistance(p1));
                  p = p1;
                }
                d = sqrt(d)*.1;

                Tol2dbail = Max(Min(Tol2dbail,d),Tol2d);

                Geom2dConvert::C0BSplineToC1BSplineCurve(bs2d,Tol2dbail);

                if(bs2d->IsPeriodic()) { // -------- IFV, Jan 2000
                  gp_Pnt2d NewOriginPoint;
                  bs2d->D0(bs2d->FirstParameter(), NewOriginPoint);
                  if(Abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion() ||
                    Abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion()    ) {

                      TColStd_Array1OfReal Knotbs2d (1, bs2d->NbKnots());
                      bs2d->Knots(Knotbs2d);

                      for(Standard_Integer Index = 1; Index <= bs2d->NbKnots(); Index++) {
                        bs2d->D0(Knotbs2d(Index), NewOriginPoint);
                        if(Abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion() ||
                          Abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion()    ) continue;

                        bs2d->SetOrigin(Index);
                        break;
                      }
                  }
                }


                if(bs2d->Continuity() == GeomAbs_C0) {
                  goodpc = 1;
                  bs2d = bs2dsov;
                  repar = Standard_False;
                }
              }
              else goodpc = 0;
            }

            if(goodpc){
              if(repar) {
                Standard_Integer NbKnots = bs2d->NbKnots();
                TColStd_Array1OfReal Knots(1,NbKnots);
                bs2d->Knots(Knots);
                //	    BSplCLib::Reparametrize(f3d,l3d,Knots);
                BSplCLib::Reparametrize(fC0,lC0,Knots);
                bs2d->SetKnots(Knots);
                GAC2d.Load(bs2d,f3d,l3d);
                curPC = bs2d;
                Standard_Boolean updatepcsov = updatepc;
                updatepc = Standard_True;

                Standard_Real error1 = ComputeTol(HC, HC2d, HS, NCONTROL);
                if(error1 > error) {
                  bs2d = bs2dsov;
                  GAC2d.Load(bs2d,f3d,l3d);
                  curPC = bs2d;
                  updatepc = updatepcsov;
                  isANA = Standard_True;
                }
                else {
                  error = error1;
                }
              }

              //check, if new BSpline "good" or not --------- IFV, Jan of 2000
              GeomAbs_Shape cont = bs2d->Continuity();
              Standard_Boolean IsBad = Standard_False;

              if(cont > GeomAbs_C0 && error > Max(1.e-3,Tolerance)) {
                Standard_Integer NbKnots = bs2d->NbKnots();
                TColStd_Array1OfReal Knots(1,NbKnots);
                bs2d->Knots(Knots);
                Standard_Real critratio = 10.; 
                Standard_Real dtprev = Knots(2) - Knots(1), dtratio = 1.;
                Standard_Real dtmin = dtprev;
                Standard_Real dtcur;
                for(Standard_Integer j = 2; j < NbKnots; j++) {
                  dtcur = Knots(j+1) - Knots(j);
                  dtmin = Min(dtmin, dtcur);

                  if(IsBad) continue;

                  if(dtcur > dtprev) dtratio = dtcur/dtprev;
                  else dtratio = dtprev/dtcur;
                  if(dtratio > critratio) {IsBad = Standard_True;}
                  dtprev = dtcur;

                }
                if(IsBad) {
                  // To avoid failures in Approx_CurvilinearParameter 
                  bs2d->Resolution(Max(1.e-3,Tolerance), dtcur);
                  if(dtmin < dtcur) IsBad = Standard_False;
                }
              }


              if(IsBad ) { //if BSpline "bad", try to reparametrize it
                // by its curve length

                //	      GeomAbs_Shape cont = bs2d->Continuity();
                if(cont > GeomAbs_C2) cont = GeomAbs_C2;
                Standard_Integer maxdeg = bs2d->Degree();
                if(maxdeg == 1) maxdeg = 14;
                Approx_CurvilinearParameter AppCurPar(HC2d, HS, Max(1.e-3,Tolerance),
                  cont, maxdeg, 10);
                if(AppCurPar.IsDone() || AppCurPar.HasResult()) {
                  bs2d = AppCurPar.Curve2d1();
                  GAC2d.Load(bs2d,f3d,l3d);
                  curPC = bs2d;

                  if(Abs(bs2d->FirstParameter() - fC0) > Tol2d ||
                    Abs(bs2d->LastParameter() - lC0) > Tol2d    ) {
                      Standard_Integer NbKnots = bs2d->NbKnots();
                      TColStd_Array1OfReal Knots(1,NbKnots);
                      bs2d->Knots(Knots);
                      //		  BSplCLib::Reparametrize(f3d,l3d,Knots);
                      BSplCLib::Reparametrize(fC0,lC0,Knots);
                      bs2d->SetKnots(Knots);
                      GAC2d.Load(bs2d,f3d,l3d);
                      curPC = bs2d;

                  }
                }
              }


            }
        }


        if(goodpc){
          //	  Approx_SameParameter SameP(HC,HC2d,HS,Tolerance);
          Standard_Real aTol = (isANA && isBSP) ? 1.e-7 : Tolerance;
          const Handle(Adaptor3d_HCurve)& aHCurv = HC; // to avoid ambiguity
          const Handle(Adaptor2d_HCurve2d)& aHCurv2d = HC2d; // to avoid ambiguity
          Approx_SameParameter SameP(aHCurv,aHCurv2d,HS,aTol);

          if (SameP.IsSameParameter()) {
            maxdist = Max(maxdist,SameP.TolReached());
            if(updatepc){
              if (i == 0) GCurve->PCurve(curPC);
              else GCurve->PCurve2(curPC);
            }
          }
          else if (SameP.IsDone()) {
            Standard_Real tolreached = SameP.TolReached();
            if(tolreached <= error) {
              curPC = SameP.Curve2d();
              updatepc = Standard_True;
              maxdist = Max(maxdist,tolreached);
            }
            else {
              maxdist = Max(maxdist, error);
            }
            if(updatepc){
              if (i == 0) GCurve->PCurve(curPC);
              else GCurve->PCurve2(curPC);
            }
          }
          else
          {
            //Approx_SameParameter has failed.
            //Consequently, the situation might be,
            //when 3D and 2D-curve do not have same-range.
            GeomLib::SameRange( Tol2d, PC[i], 
                                GCurve->First(), GCurve->Last(),
                                f3d,l3d,curPC);

            if (i == 0) GCurve->PCurve(curPC);
            else GCurve->PCurve2(curPC);

            IsSameP = 0;
          }

        }
        else IsSameP = 0;

        //  Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898 Begin
        if (!IsSameP) {
          if (anEdgeTol >= error) {
            maxdist = Max(maxdist, anEdgeTol);
            IsSameP = Standard_True;
          }
        }
        //  Modified by skv - Thu Jun  3 12:39:20 2004 OCC5898 End
      }
    }
    It.Next() ;
  }
  B.Range(AnEdge,f3d,l3d);
  B.SameRange(AnEdge,Standard_True);
  if ( IsSameP) {
    // Reduce eventually the tolerance of the edge, as
    // all its representations are processed (except for some associated
    // to planes and not stored in the edge !) 
    // The same cannot be done with vertices that cannot be enlarged 
    // or left as is.
    if (YaPCu) {
      // Avoid setting too small tolerances.
      maxdist = Max(maxdist,Precision::Confusion());
      TopoDS_Vertex V1,V2;
      TopExp::Vertices(AnEdge,V1,V2);
      if (!V1.IsNull())
        B.UpdateVertex(V1,maxdist);
      if (!V2.IsNull())
        B.UpdateVertex(V2,maxdist);
      TE->Modified(Standard_True);
      TE->Tolerance(maxdist);
    }
    B.SameParameter(AnEdge,Standard_True);
  }
}

//=======================================================================
//function : UpdateTolerances
//purpose  : 
//=======================================================================
void  BRepLib::UpdateTolerances(const TopoDS_Shape& aShape,
  const Standard_Boolean verifyTolerance) 
{

  // Harmonize tolerances
  // with rule Tolerance(VERTEX)>=Tolerance(EDGE)>=Tolerance(FACE)
  BRep_Builder B;
  Standard_Real tol=0;
  if (verifyTolerance) {
    // Set tolerance to its minimum value
    Handle(Geom_Surface) S;
    TopLoc_Location l;
    TopExp_Explorer ex;
    Bnd_Box aB;
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dMax;
    for (ex.Init(aShape, TopAbs_FACE); ex.More(); ex.Next()) {
      const TopoDS_Face& curf=TopoDS::Face(ex.Current());
      S = BRep_Tool::Surface(curf, l);
      if (!S.IsNull()) {
        aB.SetVoid();
        BRepBndLib::Add(curf,aB);
        if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
          S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
        }
        GeomAdaptor_Surface AS(S);
        switch (AS.GetType()) {
        case GeomAbs_Plane: 
        case GeomAbs_Cylinder: 
        case GeomAbs_Cone: 
          {
            tol=Precision::Confusion();
            break;
          }
        case GeomAbs_Sphere: 
        case GeomAbs_Torus: 
          {
            tol=Precision::Confusion()*2;
            break;
          }
        default:
          tol=Precision::Confusion()*4;
        }
        if (!aB.IsWhole()) {
          aB.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          dMax=1.;
          if (!aB.IsOpenXmin() && !aB.IsOpenXmax()) dMax=aXmax-aXmin;
          if (!aB.IsOpenYmin() && !aB.IsOpenYmax()) aYmin=aYmax-aYmin;
          if (!aB.IsOpenZmin() && !aB.IsOpenZmax()) aZmin=aZmax-aZmin;
          if (aYmin>dMax) dMax=aYmin;
          if (aZmin>dMax) dMax=aZmin;
          tol=tol*dMax;
          // Do not process tolerances > 1.
          if (tol>1.) tol=0.99;
        }
        const Handle(BRep_TFace)& Tf = *((Handle(BRep_TFace)*)&curf.TShape());
        Tf->Tolerance(tol);
      }
    }
  }

  //Process edges
  TopTools_IndexedDataMapOfShapeListOfShape parents;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, parents);
  TopTools_ListIteratorOfListOfShape lConx;
  Standard_Integer iCur;
  for (iCur=1; iCur<=parents.Extent(); iCur++) {
    tol=0;
    for (lConx.Initialize(parents(iCur)); lConx.More(); lConx.Next()) {
      tol=Max(tol, BRep_Tool::Tolerance(TopoDS::Face(lConx.Value())));
    }
    // Update can only increase tolerance, so if the edge has a greater
    //  tolerance than its faces it is not concerned
    B.UpdateEdge(TopoDS::Edge(parents.FindKey(iCur)), tol);
  }

  //Vertices are processed
  const Standard_Real BigTol = 1.e10;
  parents.Clear();
  TopExp::MapShapesAndAncestors(aShape, TopAbs_VERTEX, TopAbs_EDGE, parents);
  TColStd_MapOfTransient Initialized;
  TopTools_MapOfShape Done;
  Standard_Integer nbV = parents.Extent();
  for (iCur=1; iCur<=nbV; iCur++) {
    tol=0;
    Done.Clear();
    const TopoDS_Vertex& V = TopoDS::Vertex(parents.FindKey(iCur));
    Bnd_Box box;
    box.Add(BRep_Tool::Pnt(V));
    gp_Pnt p3d;
    for (lConx.Initialize(parents(iCur)); lConx.More(); lConx.Next()) {
      const TopoDS_Edge& E = TopoDS::Edge(lConx.Value());
      if(!Done.Add(E)) continue;
      tol=Max(tol, BRep_Tool::Tolerance(E));
      if(tol > BigTol) continue;
      if(!BRep_Tool::SameRange(E)) continue;
      Standard_Real par = BRep_Tool::Parameter(V,E);
      Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
      BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
      const TopLoc_Location& Eloc = E.Location();
      while (itcr.More()) {
        // For each CurveRepresentation, check the provided parameter
        const Handle(BRep_CurveRepresentation)& cr = itcr.Value();
        const TopLoc_Location& loc = cr->Location();
        TopLoc_Location L = (Eloc * loc);
        if (cr->IsCurve3D()) {
          const Handle(Geom_Curve)& C = cr->Curve3D();
          if (!C.IsNull()) { // edge non degenerated
            p3d = C->Value(par);
            p3d.Transform(L.Transformation());
            box.Add(p3d);
          }
        }
        else if (cr->IsCurveOnSurface()) {
          const Handle(Geom_Surface)& Su = cr->Surface();
          const Handle(Geom2d_Curve)& PC = cr->PCurve();
          Handle(Geom2d_Curve) PC2;
          if (cr->IsCurveOnClosedSurface()) {
            PC2 = cr->PCurve2();
          }
          gp_Pnt2d p2d = PC->Value(par);
          p3d = Su->Value(p2d.X(),p2d.Y());
          p3d.Transform(L.Transformation());
          box.Add(p3d);
          if (!PC2.IsNull()) {
            p2d = PC2->Value(par);
            p3d = Su->Value(p2d.X(),p2d.Y());
            p3d.Transform(L.Transformation());
            box.Add(p3d);
          }
        }
        itcr.Next();
      }
    }
    Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
    box.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    aXmax -= aXmin; aYmax -= aYmin; aZmax -= aZmin;
    tol = Max(tol,sqrt(aXmax*aXmax+aYmax*aYmax+aZmax*aZmax));
    tol += 2.*Epsilon(tol);
    if (verifyTolerance) {
      // ASet minimum value of the tolerance 
      // Attention to sharing of the vertex by other shapes
      const Handle(BRep_TVertex)& TV = *((Handle(BRep_TVertex)*)&V.TShape());
      if (Initialized.Add(TV)) 
        TV->Tolerance(tol);
      else 
        B.UpdateVertex(V, tol);
    }
    else {
      // Update can only increase tolerance, so if the edge has a greater
      //  tolerance than its faces it is not concerned
      B.UpdateVertex(V, tol);
    }
  }
}

//=======================================================================
//function : UpdateInnerTolerances
//purpose  : 
//=======================================================================
void  BRepLib::UpdateInnerTolerances(const TopoDS_Shape& aShape)
{
  TopTools_IndexedDataMapOfShapeListOfShape EFmap;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, EFmap);
  BRep_Builder BB;
  for (Standard_Integer i = 1; i <= EFmap.Extent(); i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(EFmap.FindKey(i));
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2);
    Standard_Real fpar, lpar;
    BRep_Tool::Range(anEdge, fpar, lpar);
    Standard_Real TolEdge = BRep_Tool::Tolerance(anEdge);
    gp_Pnt Pnt1, Pnt2;
    Handle(BRepAdaptor_HCurve) anHCurve = new BRepAdaptor_HCurve();
    anHCurve->ChangeCurve().Initialize(anEdge);
    if (!V1.IsNull())
      Pnt1 = BRep_Tool::Pnt(V1);
    if (!V2.IsNull())
      Pnt2 = BRep_Tool::Pnt(V2);
    
    if (!BRep_Tool::Degenerated(anEdge) &&
        EFmap(i).Extent() > 0)
    {
      NCollection_Sequence<Handle(Adaptor3d_HCurve)> theRep;
      theRep.Append(anHCurve);
      TopTools_ListIteratorOfListOfShape itl(EFmap(i));
      for (; itl.More(); itl.Next())
      {
        const TopoDS_Face& aFace = TopoDS::Face(itl.Value());
        Handle(BRepAdaptor_HCurve) anHCurvOnSurf = new BRepAdaptor_HCurve();
        anHCurvOnSurf->ChangeCurve().Initialize(anEdge, aFace);
        theRep.Append(anHCurvOnSurf);
      }
      
      const Standard_Integer NbSamples = (BRep_Tool::SameParameter(anEdge))? 23 : 2;
      Standard_Real delta = (lpar - fpar)/(NbSamples-1);
      Standard_Real MaxDist = 0.;
      for (Standard_Integer j = 2; j <= theRep.Length(); j++)
      {
        for (Standard_Integer k = 0; k <= NbSamples; k++)
        {
          Standard_Real ParamOnCenter = (k == NbSamples)? lpar :
            fpar + k*delta;
          gp_Pnt Center = theRep(1)->Value(ParamOnCenter);
          Standard_Real ParamOnCurve = (BRep_Tool::SameParameter(anEdge))? ParamOnCenter
            : ((k == 0)? theRep(j)->FirstParameter() : theRep(j)->LastParameter());
          gp_Pnt aPoint = theRep(j)->Value(ParamOnCurve);
          Standard_Real aDist = Center.Distance(aPoint);
          //aDist *= 1.1;
          aDist += 2.*Epsilon(aDist);
          if (aDist > MaxDist)
            MaxDist = aDist;

          //Update tolerances of vertices
          if (k == 0 && !V1.IsNull())
          {
            Standard_Real aDist1 = Pnt1.Distance(aPoint);
            aDist1 += 2.*Epsilon(aDist1);
            BB.UpdateVertex(V1, aDist1);
          }
          if (k == NbSamples && !V2.IsNull())
          {
            Standard_Real aDist2 = Pnt2.Distance(aPoint);
            aDist2 += 2.*Epsilon(aDist2);
            BB.UpdateVertex(V2, aDist2);
          }
        }
      }
      BB.UpdateEdge(anEdge, MaxDist);
    }
    TolEdge = BRep_Tool::Tolerance(anEdge);
    if (!V1.IsNull())
    {
      gp_Pnt End1 = anHCurve->Value(fpar);
      Standard_Real dist1 = Pnt1.Distance(End1);
      dist1 += 2.*Epsilon(dist1);
      BB.UpdateVertex(V1, Max(dist1, TolEdge));
    }
    if (!V2.IsNull())
    {
      gp_Pnt End2 = anHCurve->Value(lpar);
      Standard_Real dist2 = Pnt2.Distance(End2);
      dist2 += 2.*Epsilon(dist2);
      BB.UpdateVertex(V2, Max(dist2, TolEdge));
    }
  }
}

//=======================================================================
//function : OrientClosedSolid
//purpose  : 
//=======================================================================
Standard_Boolean BRepLib::OrientClosedSolid(TopoDS_Solid& solid) 
{
  // Set material inside the solid
  BRepClass3d_SolidClassifier where(solid);
  where.PerformInfinitePoint(Precision::Confusion());
  if (where.State()==TopAbs_IN) {
    solid.Reverse();
  }
  else if (where.State()==TopAbs_ON || where.State()==TopAbs_UNKNOWN) 
    return Standard_False;

  return Standard_True;
}
//=======================================================================
//function : tgtfaces
//purpose  : check the angle at the border between two squares.
//           Two shares should have a shared front edge.
//=======================================================================
static GeomAbs_Shape tgtfaces(const TopoDS_Edge& Ed,
  const TopoDS_Face& F1,
  const TopoDS_Face& F2,
  const Standard_Real theAngleTol,
  const Standard_Boolean couture)
{
  // Check if pcurves exist on both faces of edge
  Standard_Real aFirst,aLast;
  Handle(Geom2d_Curve) aCurve;
  aCurve = BRep_Tool::CurveOnSurface(Ed,F1,aFirst,aLast);
  if(aCurve.IsNull())
    return GeomAbs_C0;
  aCurve = BRep_Tool::CurveOnSurface(Ed,F2,aFirst,aLast);
  if(aCurve.IsNull())
    return GeomAbs_C0;
  
  Standard_Real u;
  TopoDS_Edge E = Ed;
  BRepAdaptor_Surface aBAS1(F1,Standard_False);
  BRepAdaptor_Surface aBAS2(F2,Standard_False);
  
  // seam edge on elementary surface is always CN
  Standard_Boolean isElementary =
    (aBAS1.Surface().Surface()->IsKind(STANDARD_TYPE(Geom_ElementarySurface)) &&
     aBAS1.Surface().Surface()->IsKind(STANDARD_TYPE(Geom_ElementarySurface)));
  if (couture && isElementary)
  {
    return GeomAbs_CN;
  }
  
  Handle(BRepAdaptor_HSurface) HS1 = new BRepAdaptor_HSurface (aBAS1);
  Handle(BRepAdaptor_HSurface) HS2;
  if(couture) HS2 = HS1;
  else HS2 = new BRepAdaptor_HSurface(aBAS2);
  //case when edge lies on the one face
  
  E.Orientation(TopAbs_FORWARD);
  Handle(BRepAdaptor_HCurve2d) HC2d1 = new BRepAdaptor_HCurve2d();
  HC2d1->ChangeCurve2d().Initialize(E,F1);
  if(couture) E.Orientation(TopAbs_REVERSED);
  Handle(BRepAdaptor_HCurve2d) HC2d2 = new BRepAdaptor_HCurve2d();
  HC2d2->ChangeCurve2d().Initialize(E,F2);
  Adaptor3d_CurveOnSurface C1(HC2d1,HS1);
  Adaptor3d_CurveOnSurface C2(HC2d2,HS2);

  Standard_Boolean rev1 = (F1.Orientation() == TopAbs_REVERSED);
  Standard_Boolean rev2 = (F2.Orientation() == TopAbs_REVERSED);
  Standard_Real f,l,eps;
  BRep_Tool::Range(E,f,l);
  Extrema_LocateExtPC ext;
  Standard_Boolean IsInitialized = Standard_False;

  eps = (l - f)/100.;
  f += eps; // to avoid calculations on  
  l -= eps; // points of pointed squares.
  gp_Pnt2d p;
  gp_Pnt pp1,pp2;//,PP;
  gp_Vec du1, dv1, d2u1, d2v1, d2uv1;
  gp_Vec du2, dv2, d2u2, d2v2, d2uv2;
  gp_Vec d1,d2;
  Standard_Real uu, vv, norm;

  Standard_Integer i;
  GeomAbs_Shape aCont = (isElementary ? GeomAbs_CN : GeomAbs_C2);
  for(i = 0; i<= 20 && aCont > GeomAbs_C0; i++)
  {
    // First suppose that this is sameParameter
    u = f + (l-f)*i/20;

    // take derivatives of surfaces at the same u, and compute normals
    HC2d1->D0(u,p);
    HS1->D2 (p.X(), p.Y(), pp1, du1, dv1, d2u1, d2v1, d2uv1);
    d1 = (du1.Crossed(dv1));
    norm = d1.Magnitude(); 
    if (norm > 1.e-12) d1 /= norm;
    else continue; // skip degenerated point
    if(rev1) d1.Reverse();

    HC2d2->D0(u,p);
    HS2->D2 (p.X(), p.Y(), pp2, du2, dv2, d2u2, d2v2, d2uv2);
    d2 = (du2.Crossed(dv2));
    norm = d2.Magnitude();
    if (norm > 1.e-12) d2 /= norm;
    else continue; // skip degenerated point
    if(rev2) d2.Reverse();

    // check 
    Standard_Real ang = d1.Angle(d2);

    // check special case of precise equality of derivatives,
    // occurring when edge connects two faces built on equally 
    // defined surfaces (e.g. seam-like edges on periodic surfaces, 
    // or planar faces on the same plane)
    if (aCont >= GeomAbs_C2 && ang < Precision::Angular() &&
        d2u1 .IsEqual (d2u2,  Precision::PConfusion(), Precision::Angular()) &&
        d2v1 .IsEqual (d2v2,  Precision::PConfusion(), Precision::Angular()) &&
        d2uv1.IsEqual (d2uv2, Precision::PConfusion(), Precision::Angular()))
    {
      continue;
    }

    aCont = GeomAbs_G1;

    // Refine by projection
    if (ang > theAngleTol)
    {
      if (! IsInitialized ) {
        ext.Initialize(C2,f,l,Precision::PConfusion());
        IsInitialized = Standard_True;
      }      
      ext.Perform(pp1,u);
      if(ext.IsDone() && ext.IsMin()){
        Extrema_POnCurv poc = ext.Point();
        Standard_Real v = poc.Parameter();

        HC2d2->D0(v,p);
        p.Coord(uu,vv);
        HS2->D1(p.X(), p.Y(), pp2, du2, dv2);
        d2 = (du2.Crossed(dv2));
        norm = d2.Magnitude();
        if (norm> 1.e-12) d2 /= norm;
        else continue; // degenerated point
        if(rev2) d2.Reverse();
        ang = d1.Angle(d2);
      }
      if (ang > theAngleTol)
        return GeomAbs_C0;
    }
  }     

  return aCont;
}


//=======================================================================
// function : EncodeRegularity
// purpose  : code the regularities on all edges of the shape, boundary of 
//            two faces that do not have it.
//=======================================================================

void BRepLib::EncodeRegularity(const TopoDS_Shape& S,
  const Standard_Real TolAng)
{
  BRep_Builder B;
  TopTools_IndexedDataMapOfShapeListOfShape M;
  TopExp::MapShapesAndAncestors(S,TopAbs_EDGE,TopAbs_FACE,M);
  TopTools_ListIteratorOfListOfShape It;
  TopExp_Explorer Ex;
  TopoDS_Face F1,F2;
  Standard_Boolean found, couture;
  for(Standard_Integer i = 1; i <= M.Extent(); i++){
    TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
    found = Standard_False; couture = Standard_False;
    F1.Nullify();
    for(It.Initialize(M.FindFromIndex(i));It.More() && !found;It.Next()){
      if(F1.IsNull()) { F1 = TopoDS::Face(It.Value()); }
      else {
        if(!F1.IsSame(TopoDS::Face(It.Value()))){
          found = Standard_True;
          F2 = TopoDS::Face(It.Value());
        }
      }
    }
    if (!found && !F1.IsNull()){//is it a sewing edge?
      TopAbs_Orientation orE = E.Orientation();
      TopoDS_Edge curE;
      for(Ex.Init(F1,TopAbs_EDGE);Ex.More() && !found;Ex.Next()){
        curE= TopoDS::Edge(Ex.Current());
        if(E.IsSame(curE) && orE != curE.Orientation()) {
          found = Standard_True;
          couture = Standard_True;
          F2 = F1;
        }
      }
    }
    if(found){
      if(BRep_Tool::Continuity(E,F1,F2)<=GeomAbs_C0){

        try {
          GeomAbs_Shape aCont = tgtfaces(E, F1, F2, TolAng, couture);
          B.Continuity(E,F1,F2,aCont);
        }
        catch(Standard_Failure)
        {
        }
      }
    }
  }
}

//=======================================================================
// function : EncodeRegularity
// purpose  : code the regularity between 2 faces on an edge 
//=======================================================================

void BRepLib::EncodeRegularity(TopoDS_Edge& E,
  const TopoDS_Face& F1,
  const TopoDS_Face& F2,
  const Standard_Real TolAng)
{
  BRep_Builder B;
  if(BRep_Tool::Continuity(E,F1,F2)<=GeomAbs_C0){
    try {
      GeomAbs_Shape aCont = tgtfaces(E, F1, F2, TolAng, F1.IsEqual(F2));
      B.Continuity(E,F1,F2,aCont);
      
    }
    catch(Standard_Failure)
    {
    }
  }
}

//=======================================================================
// function : EnsureNormalConsistency
// purpose  : Corrects the normals in Poly_Triangulation of faces.
//              Returns TRUE if any correction is done.
//=======================================================================
Standard_Boolean BRepLib::
            EnsureNormalConsistency(const TopoDS_Shape& theShape,
                                    const Standard_Real theAngTol,
                                    const Standard_Boolean theForceComputeNormals)
{
  const Standard_Real aThresDot = cos(theAngTol);

  Standard_Boolean aRetVal = Standard_False, isNormalsFound = Standard_False;

  // compute normals if they are absent
  TopExp_Explorer anExpFace(theShape,TopAbs_FACE);
  for (; anExpFace.More(); anExpFace.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExpFace.Current());
    const Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
    if(aSurf.IsNull())
      continue;
    TopLoc_Location aLoc;
    const Handle(Poly_Triangulation)& aPT = BRep_Tool::Triangulation(aFace, aLoc);
    if(aPT.IsNull())
      continue;
    if (!theForceComputeNormals && aPT->HasNormals())
    {
      isNormalsFound = Standard_True;
      continue;
    }

    GeomLProp_SLProps aSLP(aSurf, 2, Precision::Confusion());
    const Standard_Integer anArrDim = 3*aPT->NbNodes();
    Handle(TShort_HArray1OfShortReal) aNormArr = new TShort_HArray1OfShortReal(1, anArrDim);
    Standard_Integer anNormInd = aNormArr->Lower();
    for(Standard_Integer i = aPT->UVNodes().Lower(); i <= aPT->UVNodes().Upper(); i++)
    {
      const gp_Pnt2d &aP2d = aPT->UVNodes().Value(i);
      aSLP.SetParameters(aP2d.X(), aP2d.Y());

      gp_XYZ aNorm(0.,0.,0.);
      if(!aSLP.IsNormalDefined())
      {
#ifdef OCCT_DEBUG
        cout << "BRepLib::EnsureNormalConsistency(): Cannot find normal!" << endl;
#endif
      }
      else
      {
        aNorm = aSLP.Normal().XYZ();
        if (aFace.Orientation() == TopAbs_REVERSED)
          aNorm.Reverse();
      }
      aNormArr->ChangeValue(anNormInd++) = static_cast<Standard_ShortReal>(aNorm.X());
      aNormArr->ChangeValue(anNormInd++) = static_cast<Standard_ShortReal>(aNorm.Y());
      aNormArr->ChangeValue(anNormInd++) = static_cast<Standard_ShortReal>(aNorm.Z());
    }

    aRetVal = Standard_True;
    isNormalsFound = Standard_True;
    aPT->SetNormals(aNormArr);
  }

  if(!isNormalsFound)
  {
    return aRetVal;
  }

  // loop by edges
  TopTools_IndexedDataMapOfShapeListOfShape aMapEF;
  TopExp::MapShapesAndAncestors(theShape,TopAbs_EDGE,TopAbs_FACE,aMapEF);
  for(Standard_Integer anInd = 1; anInd <= aMapEF.Extent(); anInd++)
  {
    const TopoDS_Edge& anEdg = TopoDS::Edge(aMapEF.FindKey(anInd));
    const TopTools_ListOfShape& anEdgList = aMapEF.FindFromIndex(anInd);
    if (anEdgList.Extent() != 2)
      continue;
    TopTools_ListIteratorOfListOfShape anItF(anEdgList);
    const TopoDS_Face aFace1 = TopoDS::Face(anItF.Value());
    anItF.Next();
    const TopoDS_Face aFace2 = TopoDS::Face(anItF.Value());
    TopLoc_Location aLoc1, aLoc2;
    const Handle(Poly_Triangulation)& aPT1 = BRep_Tool::Triangulation(aFace1, aLoc1);
    const Handle(Poly_Triangulation)& aPT2 = BRep_Tool::Triangulation(aFace2, aLoc2);

    if(aPT1.IsNull() || aPT2.IsNull())
      continue;

    if(!aPT1->HasNormals() || !aPT2->HasNormals())
      continue;

    const Handle(Poly_PolygonOnTriangulation)& aPTEF1 = 
                                BRep_Tool::PolygonOnTriangulation(anEdg, aPT1, aLoc1);
    const Handle(Poly_PolygonOnTriangulation)& aPTEF2 = 
                                BRep_Tool::PolygonOnTriangulation(anEdg, aPT2, aLoc2);

    TShort_Array1OfShortReal& aNormArr1 = aPT1->ChangeNormals();
    TShort_Array1OfShortReal& aNormArr2 = aPT2->ChangeNormals();

    if (aPTEF1->Nodes().Lower() != aPTEF2->Nodes().Lower() || 
        aPTEF1->Nodes().Upper() != aPTEF2->Nodes().Upper()) 
      continue; 

    for(Standard_Integer anEdgNode = aPTEF1->Nodes().Lower();
                         anEdgNode <= aPTEF1->Nodes().Upper(); anEdgNode++)
    {
      //Number of node
      const Standard_Integer aFNodF1 = aPTEF1->Nodes().Value(anEdgNode);
      const Standard_Integer aFNodF2 = aPTEF2->Nodes().Value(anEdgNode);

      const Standard_Integer aFNorm1FirstIndex = aNormArr1.Lower() + 3*
                                                    (aFNodF1 - aPT1->Nodes().Lower());
      const Standard_Integer aFNorm2FirstIndex = aNormArr2.Lower() + 3*
                                                    (aFNodF2 - aPT2->Nodes().Lower());

      gp_XYZ aNorm1(aNormArr1.Value(aFNorm1FirstIndex),
                    aNormArr1.Value(aFNorm1FirstIndex+1),
                    aNormArr1.Value(aFNorm1FirstIndex+2));
      gp_XYZ aNorm2(aNormArr2.Value(aFNorm2FirstIndex),
                    aNormArr2.Value(aFNorm2FirstIndex+1),
                    aNormArr2.Value(aFNorm2FirstIndex+2));
      const Standard_Real aDot = aNorm1 * aNorm2;

      if(aDot > aThresDot)
      {
        gp_XYZ aNewNorm = (aNorm1 + aNorm2).Normalized();
        aNormArr1.ChangeValue(aFNorm1FirstIndex) =
          aNormArr2.ChangeValue(aFNorm2FirstIndex) =
          static_cast<Standard_ShortReal>(aNewNorm.X());
        aNormArr1.ChangeValue(aFNorm1FirstIndex+1) =
          aNormArr2.ChangeValue(aFNorm2FirstIndex+1) =
          static_cast<Standard_ShortReal>(aNewNorm.Y());
        aNormArr1.ChangeValue(aFNorm1FirstIndex+2) =
          aNormArr2.ChangeValue(aFNorm2FirstIndex+2) =
          static_cast<Standard_ShortReal>(aNewNorm.Z());
        aRetVal = Standard_True;
      }
    }
  }

  return aRetVal;
}

//=======================================================================
//function : SortFaces
//purpose  : 
//=======================================================================

void  BRepLib::SortFaces (const TopoDS_Shape& Sh,
  TopTools_ListOfShape& LF)
{
  LF.Clear();
  TopTools_ListOfShape LTri,LPlan,LCyl,LCon,LSphere,LTor,LOther;
  TopExp_Explorer exp(Sh,TopAbs_FACE);
  TopLoc_Location l;
  Handle(Geom_Surface) S;

  for (; exp.More(); exp.Next()) {
    const TopoDS_Face&   F = TopoDS::Face(exp.Current());
    S = BRep_Tool::Surface(F, l);
    if (!S.IsNull()) {
      if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        S = Handle(Geom_RectangularTrimmedSurface)::DownCast (S)->BasisSurface();
      }
      GeomAdaptor_Surface AS(S);
      switch (AS.GetType()) {
      case GeomAbs_Plane: 
        {
          LPlan.Append(F);
          break;
        }
      case GeomAbs_Cylinder: 
        {
          LCyl.Append(F);
          break;
        }
      case GeomAbs_Cone: 
        {
          LCon.Append(F);
          break;
        }
      case GeomAbs_Sphere: 
        {
          LSphere.Append(F);
          break;
        }
      case GeomAbs_Torus: 
        {
          LTor.Append(F);
          break;
        }
      default:
        LOther.Append(F);
      }
    }
    else LTri.Append(F);
  }
  LF.Append(LPlan); LF.Append(LCyl  ); LF.Append(LCon); LF.Append(LSphere);
  LF.Append(LTor ); LF.Append(LOther); LF.Append(LTri); 
}

//=======================================================================
//function : ReverseSortFaces
//purpose  : 
//=======================================================================

void  BRepLib::ReverseSortFaces (const TopoDS_Shape& Sh,
  TopTools_ListOfShape& LF)
{
  LF.Clear();
  // Use the allocator of the result LF for intermediate results
  TopTools_ListOfShape LTri(LF.Allocator()), LPlan(LF.Allocator()),
    LCyl(LF.Allocator()), LCon(LF.Allocator()), LSphere(LF.Allocator()),
    LTor(LF.Allocator()), LOther(LF.Allocator());
  TopExp_Explorer exp(Sh,TopAbs_FACE);
  TopLoc_Location l;

  for (; exp.More(); exp.Next()) {
    const TopoDS_Face&   F = TopoDS::Face(exp.Current());
    const Handle(Geom_Surface)& S = BRep_Tool::Surface(F, l);
    if (!S.IsNull()) {
      GeomAdaptor_Surface AS(S);
      switch (AS.GetType()) {
      case GeomAbs_Plane: 
        {
          LPlan.Append(F);
          break;
        }
      case GeomAbs_Cylinder: 
        {
          LCyl.Append(F);
          break;
        }
      case GeomAbs_Cone: 
        {
          LCon.Append(F);
          break;
        }
      case GeomAbs_Sphere: 
        {
          LSphere.Append(F);
          break;
        }
      case GeomAbs_Torus: 
        {
          LTor.Append(F);
          break;
        }
      default:
        LOther.Append(F);
      }
    }
    else LTri.Append(F);
  }
  LF.Append(LTri); LF.Append(LOther); LF.Append(LTor ); LF.Append(LSphere);
  LF.Append(LCon); LF.Append(LCyl  ); LF.Append(LPlan);

}

//=======================================================================
// function: BoundingVertex
// purpose : 
//=======================================================================
void BRepLib::BoundingVertex(const NCollection_List<TopoDS_Shape>& theLV,
                             gp_Pnt& theNewCenter, Standard_Real& theNewTol)
{
  Standard_Integer aNb;
  //
  aNb=theLV.Extent();
  if (aNb < 2) {
    return;
  }
  //
  else if (aNb==2) {
    Standard_Integer m, n;
    Standard_Real aR[2], dR, aD, aEps;
    TopoDS_Vertex aV[2];
    gp_Pnt aP[2];
    //
    aEps=RealEpsilon();
    for (m=0; m<aNb; ++m) {
      aV[m]=(!m)? 
        *((TopoDS_Vertex*)(&theLV.First())):
        *((TopoDS_Vertex*)(&theLV.Last()));
      aP[m]=BRep_Tool::Pnt(aV[m]);
      aR[m]=BRep_Tool::Tolerance(aV[m]);
    }  
    //
    m=0; // max R
    n=1; // min R
    if (aR[0]<aR[1]) {
      m=1;
      n=0;
    }
    //
    dR=aR[m]-aR[n]; // dR >= 0.
    gp_Vec aVD(aP[m], aP[n]);
    aD=aVD.Magnitude();
    //
    if (aD<=dR || aD<aEps) { 
      theNewCenter = aP[m];
      theNewTol = aR[m];
    }
    else {
      Standard_Real aRr;
      gp_XYZ aXYZr;
      gp_Pnt aPr;
      //
      aRr=0.5*(aR[m]+aR[n]+aD);
      aXYZr=0.5*(aP[m].XYZ()+aP[n].XYZ()-aVD.XYZ()*(dR/aD));
      aPr.SetXYZ(aXYZr);
      //
      theNewCenter = aPr;
      theNewTol = aRr;
      //aBB.MakeVertex (aVnew, aPr, aRr);
    }
    return;
  }// else if (aNb==2) {
  //
  else { // if (aNb>2)
    // compute the point
    //
    // issue 0027540 - sum of doubles may depend on the order
    // of addition, thus sort the coordinates for stable result
    Standard_Integer i;
    NCollection_Array1<gp_Pnt> aPoints(0, aNb-1);
    NCollection_List<TopoDS_Edge>::Iterator aIt(theLV);
    for (i = 0; aIt.More(); aIt.Next(), ++i) {
      const TopoDS_Vertex& aVi = *((TopoDS_Vertex*)(&aIt.Value()));
      gp_Pnt aPi = BRep_Tool::Pnt(aVi);
      aPoints(i) = aPi;
    }
    //
    std::sort(aPoints.begin(), aPoints.end(), BRepLib_ComparePoints());
    //
    gp_XYZ aXYZ(0., 0., 0.);
    for (i = 0; i < aNb; ++i) {
      aXYZ += aPoints(i).XYZ();
    }
    aXYZ.Divide((Standard_Real)aNb);
    //
    gp_Pnt aP(aXYZ);
    //
    // compute the tolerance for the new vertex
    Standard_Real aTi, aDi, aDmax;
    //
    aDmax=-1.;
    aIt.Initialize(theLV);
    for (; aIt.More(); aIt.Next()) {
      TopoDS_Vertex& aVi=*((TopoDS_Vertex*)(&aIt.Value()));
      gp_Pnt aPi=BRep_Tool::Pnt(aVi);
      aTi=BRep_Tool::Tolerance(aVi);
      aDi=aP.SquareDistance(aPi);
      aDi=sqrt(aDi);
      aDi=aDi+aTi;
      if (aDi > aDmax) {
        aDmax=aDi;
      }
    }
    //
    theNewCenter = aP;
    theNewTol = aDmax;
  }
}
