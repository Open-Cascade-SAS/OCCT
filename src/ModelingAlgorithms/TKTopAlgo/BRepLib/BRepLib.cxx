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

// pmn 26/09/97 Add parameters of approximation in BuildCurve3d
//   Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898

#include <BRepLib.hxx>

#include <AdvApprox_ApproxAFunction.hxx>
#include <AppParCurves_MultiBSpCurve.hxx>
#include <Approx_CurvilinearParameter.hxx>
#include <Approx_SameParameter.hxx>
#include <BRep_Builder.hxx>
#include <BRep_GCurve.hxx>
#include <BRepCheck.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRep_TVertex.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib_MakeFace.hxx>
#include <BSplCLib.hxx>
#include <ElSLib.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <GCPnts_QuasiUniformDeflection.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
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
#include <Standard_ErrorHandler.hxx>
#include <Standard_Real.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Map.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <gp_XY.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_LockedShape.hxx>
#include <Standard_HashUtils.hxx>

#include <algorithm>

// TODO - not thread-safe static variables
static double                  thePrecision = Precision::Confusion();
static occ::handle<Geom_Plane> thePlane;

static void InternalUpdateTolerances(const TopoDS_Shape& theOldShape,
                                     const bool          IsVerifyTolerance,
                                     const bool          IsMutableInput,
                                     BRepTools_ReShape&  theReshaper);

//=======================================================================
// function: BRepLib_ComparePoints
// purpose:  implementation of IsLess() function for two points
//=======================================================================
struct BRepLib_ComparePoints
{
  bool operator()(const gp_Pnt& theP1, const gp_Pnt& theP2)
  {
    for (int i = 1; i <= 3; ++i)
    {
      if (theP1.Coord(i) < theP2.Coord(i))
      {
        return true;
      }
      else if (theP1.Coord(i) > theP2.Coord(i))
      {
        return false;
      }
    }
    return false;
  }
};

//=================================================================================================

void BRepLib::Precision(const double P)
{
  thePrecision = P;
}

//=================================================================================================

double BRepLib::Precision()
{
  return thePrecision;
}

//=================================================================================================

void BRepLib::Plane(const occ::handle<Geom_Plane>& P)
{
  thePlane = P;
}

//=================================================================================================

const occ::handle<Geom_Plane>& BRepLib::Plane()
{
  if (thePlane.IsNull())
    thePlane = new Geom_Plane(gp::XOY());
  return thePlane;
}

//=================================================================================================

bool BRepLib::CheckSameRange(const TopoDS_Edge& AnEdge, const double Tolerance)
{
  bool IsSameRange = true, first_time_in = true;

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator an_Iterator(
    (*((occ::handle<BRep_TEdge>*)&AnEdge.TShape()))->ChangeCurves());

  double                   first, last;
  double                   current_first = 0., current_last = 0.;
  occ::handle<BRep_GCurve> geometric_representation_ptr;

  while (IsSameRange && an_Iterator.More())
  {
    geometric_representation_ptr = occ::down_cast<BRep_GCurve>(an_Iterator.Value());
    if (!geometric_representation_ptr.IsNull())
    {

      first = geometric_representation_ptr->First();
      last  = geometric_representation_ptr->Last();
      if (first_time_in)
      {
        current_first = first;
        current_last  = last;
        first_time_in = false;
      }
      else
      {
        IsSameRange = (std::abs(current_first - first) <= Tolerance)
                      && (std::abs(current_last - last) <= Tolerance);
      }
    }
    an_Iterator.Next();
  }
  return IsSameRange;
}

//=================================================================================================

void BRepLib::SameRange(const TopoDS_Edge& AnEdge, const double Tolerance)
{
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator an_Iterator(
    (*((occ::handle<BRep_TEdge>*)&AnEdge.TShape()))->ChangeCurves());

  occ::handle<Geom2d_Curve> Curve2dPtr, Curve2dPtr2, NewCurve2dPtr, NewCurve2dPtr2;
  TopLoc_Location           LocalLoc;

  bool                     first_time_in = true, has_curve, has_closed_curve;
  occ::handle<BRep_GCurve> geometric_representation_ptr;
  double                   first, current_first, last, current_last;

  const occ::handle<Geom_Curve> C = BRep_Tool::Curve(AnEdge, LocalLoc, current_first, current_last);
  if (!C.IsNull())
  {
    first_time_in = false;
  }

  while (an_Iterator.More())
  {
    geometric_representation_ptr = occ::down_cast<BRep_GCurve>(an_Iterator.Value());
    if (!geometric_representation_ptr.IsNull())
    {
      has_closed_curve = has_curve = false;
      first                        = geometric_representation_ptr->First();
      last                         = geometric_representation_ptr->Last();
      if (geometric_representation_ptr->IsCurveOnSurface())
      {
        Curve2dPtr = geometric_representation_ptr->PCurve();
        has_curve  = true;
      }
      if (geometric_representation_ptr->IsCurveOnClosedSurface())
      {
        Curve2dPtr2      = geometric_representation_ptr->PCurve2();
        has_closed_curve = true;
      }
      if (has_curve || has_closed_curve)
      {
        if (first_time_in)
        {
          current_first = first;
          current_last  = last;
          first_time_in = false;
        }

        if (std::abs(first - current_first) > Precision::Confusion()
            || std::abs(last - current_last) > Precision::Confusion())
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
            geometric_representation_ptr->PCurve(NewCurve2dPtr);
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
            geometric_representation_ptr->PCurve2(NewCurve2dPtr2);
          }
        }
      }
    }
    an_Iterator.Next();
  }
  BRep_Builder B;
  B.Range(TopoDS::Edge(AnEdge), current_first, current_last);

  B.SameRange(AnEdge, true);
}

//=======================================================================
// function : EvaluateMaxSegment
// purpose  : return MaxSegment to pass in approximation, if MaxSegment==0 provided
//=======================================================================

static int evaluateMaxSegment(const int                       aMaxSegment,
                              const Adaptor3d_CurveOnSurface& aCurveOnSurface)
{
  if (aMaxSegment != 0)
    return aMaxSegment;

  const occ::handle<Adaptor3d_Surface>& aSurf   = aCurveOnSurface.GetSurface();
  const occ::handle<Adaptor2d_Curve2d>& aCurv2d = aCurveOnSurface.GetCurve();

  double aNbSKnots = 0, aNbC2dKnots = 0;

  if (aSurf->GetType() == GeomAbs_BSplineSurface)
  {
    occ::handle<Geom_BSplineSurface> aBSpline = aSurf->BSpline();
    aNbSKnots = std::max(aBSpline->NbUKnots(), aBSpline->NbVKnots());
  }
  if (aCurv2d->GetType() == GeomAbs_BSplineCurve)
  {
    aNbC2dKnots = aCurv2d->NbKnots();
  }
  int aReturn = (int)(30 + std::max(aNbSKnots, aNbC2dKnots));
  return aReturn;
}

//=================================================================================================

bool BRepLib::BuildCurve3d(const TopoDS_Edge&  AnEdge,
                           const double        Tolerance,
                           const GeomAbs_Shape Continuity,
                           const int           MaxDegree,
                           const int           MaxSegment)
{
  int // ErrorCode,
      //                    ReturnCode = 0,
    ii,
    //                   num_knots,
    jj;

  TopLoc_Location LocalLoc, L[2], LC;
  double          f, l, fc, lc, first[2], last[2], tolerance, max_deviation, average_deviation;
  occ::handle<Geom2d_Curve> Curve2dPtr, Curve2dArray[2];
  occ::handle<Geom_Surface> SurfacePtr, SurfaceArray[2];

  int not_done;
  // if the edge has a 3d curve returns true

  const occ::handle<Geom_Curve> C = BRep_Tool::Curve(AnEdge, LocalLoc, f, l);
  if (!C.IsNull())
    return true;
  //
  // this should not exists but UpdateEdge makes funny things
  // if the edge is not same range
  //
  if (!CheckSameRange(AnEdge, Precision::Confusion()))
  {
    SameRange(AnEdge, Tolerance);
  }

  // search a curve on a plane
  occ::handle<Geom_Surface> S;
  occ::handle<Geom2d_Curve> PC;
  int                       i = 0;
  occ::handle<Geom_Plane>   P;
  not_done = 1;

  while (not_done)
  {
    i++;
    BRep_Tool::CurveOnSurface(AnEdge, PC, S, LocalLoc, f, l, i);
    occ::handle<Geom_RectangularTrimmedSurface> RT =
      occ::down_cast<Geom_RectangularTrimmedSurface>(S);
    if (RT.IsNull())
    {
      P = occ::down_cast<Geom_Plane>(S);
    }
    else
    {
      P = occ::down_cast<Geom_Plane>(RT->BasisSurface());
    }
    not_done = !S.IsNull() && P.IsNull();
  }
  if (!P.IsNull())
  {
    // compute the 3d curve
    gp_Ax2                  axes = P->Position().Ax2();
    occ::handle<Geom_Curve> C3d  = GeomLib::To3d(axes, PC);
    if (C3d.IsNull())
      return false;
    // update the edge
    double First, Last;

    BRep_Builder B;
    B.UpdateEdge(AnEdge, C3d, LocalLoc, 0.0e0);
    BRep_Tool::Range(AnEdge, S, LC, First, Last);
    B.Range(AnEdge, First, Last); // Do not forget 3D range.(PRO6412)
  }
  else
  {
    //
    // compute the 3d curve using existing surface
    //
    fc = f;
    lc = l;
    if (!BRep_Tool::Degenerated(AnEdge))
    {
      jj = 0;
      // BRep_Tool::CurveOnSurface expects Index >= 1, so start from 1.
      for (ii = 1; ii < 3; ii++)
      {
        BRep_Tool::CurveOnSurface(TopoDS::Edge(AnEdge),
                                  Curve2dPtr,
                                  SurfacePtr,
                                  LocalLoc,
                                  fc,
                                  lc,
                                  ii);

        if (!Curve2dPtr.IsNull() && jj < 2)
        {
          Curve2dArray[jj] = Curve2dPtr;
          SurfaceArray[jj] = SurfacePtr;
          L[jj]            = LocalLoc;
          first[jj]        = fc;
          last[jj]         = lc;
          jj += 1;
        }
      }
      f          = first[0];
      l          = last[0];
      Curve2dPtr = Curve2dArray[0];
      SurfacePtr = SurfaceArray[0];

      Geom2dAdaptor_Curve              AnAdaptor3dCurve2d(Curve2dPtr, f, l);
      GeomAdaptor_Surface              AnAdaptor3dSurface(SurfacePtr);
      occ::handle<Geom2dAdaptor_Curve> AnAdaptor3dCurve2dPtr =
        new Geom2dAdaptor_Curve(AnAdaptor3dCurve2d);
      occ::handle<GeomAdaptor_Surface> AnAdaptor3dSurfacePtr =
        new GeomAdaptor_Surface(AnAdaptor3dSurface);
      Adaptor3d_CurveOnSurface CurveOnSurface(AnAdaptor3dCurve2dPtr, AnAdaptor3dSurfacePtr);

      occ::handle<Geom_Curve> NewCurvePtr;

      GeomLib::BuildCurve3d(Tolerance,
                            CurveOnSurface,
                            f,
                            l,
                            NewCurvePtr,
                            max_deviation,
                            average_deviation,
                            Continuity,
                            MaxDegree,
                            evaluateMaxSegment(MaxSegment, CurveOnSurface));
      BRep_Builder B;
      tolerance = BRep_Tool::Tolerance(AnEdge);
      // Patch
      // max_deviation = std::max(tolerance, max_deviation) ;
      max_deviation = std::max(tolerance, Tolerance);
      if (NewCurvePtr.IsNull())
        return false;
      B.UpdateEdge(TopoDS::Edge(AnEdge), NewCurvePtr, L[0], max_deviation);
      if (jj == 1)
      {
        //
        // if there is only one curve on surface attached to the edge
        // than it can be qualified sameparameter
        //
        B.SameParameter(TopoDS::Edge(AnEdge), true);
      }
    }
    else
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

bool BRepLib::BuildCurves3d(const TopoDS_Shape& S)

{
  return BRepLib::BuildCurves3d(S, 1.0e-5);
}

//=================================================================================================

bool BRepLib::BuildCurves3d(const TopoDS_Shape& S,
                            const double        Tolerance,
                            const GeomAbs_Shape Continuity,
                            const int           MaxDegree,
                            const int           MaxSegment)
{
  bool                                                   boolean_value, ok = true;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> a_counter;
  TopExp_Explorer                                        ex(S, TopAbs_EDGE);

  while (ex.More())
  {
    if (a_counter.Add(ex.Current()))
    {
      boolean_value =
        BuildCurve3d(TopoDS::Edge(ex.Current()), Tolerance, Continuity, MaxDegree, MaxSegment);
      ok = ok && boolean_value;
    }
    ex.Next();
  }
  return ok;
}

//=================================================================================================

bool BRepLib::UpdateEdgeTol(const TopoDS_Edge& AnEdge,
                            const double       MinToleranceRequested,
                            const double       MaxToleranceToCheck)
{

  int curve_on_surface_index, curve_index, not_done, has_closed_curve, has_curve, jj, ii,
    geom_reference_curve_flag = 0, max_sampling_points = 90, min_sampling_points = 30;

  double factor = 100.0e0,
         //     sampling_array[2],
    safe_factor         = 1.4e0, current_last, current_first, max_distance, coded_edge_tolerance,
         edge_tolerance = 0.0e0;
  occ::handle<NCollection_HArray1<double>> parameters_ptr;
  occ::handle<BRep_GCurve>                 geometric_representation_ptr;

  if (BRep_Tool::Degenerated(AnEdge))
    return false;
  coded_edge_tolerance = BRep_Tool::Tolerance(AnEdge);
  if (coded_edge_tolerance > MaxToleranceToCheck)
    return false;

  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&AnEdge.TShape());
  NCollection_List<occ::handle<BRep_CurveRepresentation>>& list_curve_rep = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator an_iterator(list_curve_rep),
    second_iterator(list_curve_rep);
  occ::handle<Geom2d_Curve>     curve2d_ptr, new_curve2d_ptr;
  occ::handle<Geom_Surface>     surface_ptr;
  TopLoc_Location               local_location;
  GCPnts_QuasiUniformDeflection a_sampler;
  GeomAdaptor_Curve             geom_reference_curve;
  Adaptor3d_CurveOnSurface      curve_on_surface_reference;
  occ::handle<Geom_Curve> C = BRep_Tool::Curve(AnEdge, local_location, current_first, current_last);
  curve_on_surface_index    = -1;
  if (!C.IsNull())
  {
    if (!local_location.IsIdentity())
    {
      C = occ::down_cast<Geom_Curve>(C->Transformed(local_location.Transformation()));
    }
    geom_reference_curve.Load(C);
    geom_reference_curve_flag = 1;
    a_sampler.Initialize(geom_reference_curve,
                         MinToleranceRequested * factor,
                         current_first,
                         current_last);
  }
  else
  {
    not_done               = 1;
    curve_on_surface_index = 0;

    while (not_done && an_iterator.More())
    {
      geometric_representation_ptr = occ::down_cast<BRep_GCurve>(second_iterator.Value());
      if (!geometric_representation_ptr.IsNull()
          && geometric_representation_ptr->IsCurveOnSurface())
      {
        curve2d_ptr    = geometric_representation_ptr->PCurve();
        local_location = geometric_representation_ptr->Location();
        current_first  = geometric_representation_ptr->First();
        // first = geometric_representation_ptr->First();
        current_last = geometric_representation_ptr->Last();
        // must be inverted
        //
        if (!local_location.IsIdentity())
        {
          surface_ptr = occ::down_cast<Geom_Surface>(
            geometric_representation_ptr->Surface()->Transformed(local_location.Transformation()));
        }
        else
        {
          surface_ptr = geometric_representation_ptr->Surface();
        }
        not_done = 0;
      }
      curve_on_surface_index += 1;
    }
    Geom2dAdaptor_Curve              AnAdaptor3dCurve2d(curve2d_ptr);
    GeomAdaptor_Surface              AnAdaptor3dSurface(surface_ptr);
    occ::handle<Geom2dAdaptor_Curve> AnAdaptor3dCurve2dPtr =
      new Geom2dAdaptor_Curve(AnAdaptor3dCurve2d);
    occ::handle<GeomAdaptor_Surface> AnAdaptor3dSurfacePtr =
      new GeomAdaptor_Surface(AnAdaptor3dSurface);
    curve_on_surface_reference.Load(AnAdaptor3dCurve2dPtr, AnAdaptor3dSurfacePtr);
    a_sampler.Initialize(curve_on_surface_reference,
                         MinToleranceRequested * factor,
                         current_first,
                         current_last);
  }
  NCollection_Array1<double> sampling_parameters(1, a_sampler.NbPoints());
  for (ii = 1; ii <= a_sampler.NbPoints(); ii++)
  {
    sampling_parameters(ii) = a_sampler.Parameter(ii);
  }
  if (a_sampler.NbPoints() < min_sampling_points)
  {
    GeomLib::DensifyArray1OfReal(min_sampling_points, sampling_parameters, parameters_ptr);
  }
  else if (a_sampler.NbPoints() > max_sampling_points)
  {
    GeomLib::RemovePointsFromArray(max_sampling_points, sampling_parameters, parameters_ptr);
  }
  else
  {
    jj             = 1;
    parameters_ptr = new NCollection_HArray1<double>(1, sampling_parameters.Length());
    for (ii = sampling_parameters.Lower(); ii <= sampling_parameters.Upper(); ii++)
    {
      parameters_ptr->ChangeArray1()(jj) = sampling_parameters(ii);
      jj += 1;
    }
  }

  curve_index = 0;

  while (second_iterator.More())
  {
    geometric_representation_ptr = occ::down_cast<BRep_GCurve>(second_iterator.Value());
    if (!geometric_representation_ptr.IsNull() && curve_index != curve_on_surface_index)
    {
      has_closed_curve = has_curve = false;
      //	first = geometric_representation_ptr->First();
      //	last =  geometric_representation_ptr->Last();
      local_location = geometric_representation_ptr->Location();
      if (geometric_representation_ptr->IsCurveOnSurface())
      {
        curve2d_ptr = geometric_representation_ptr->PCurve();
        has_curve   = true;
      }
      if (geometric_representation_ptr->IsCurveOnClosedSurface())
      {
        curve2d_ptr      = geometric_representation_ptr->PCurve2();
        has_closed_curve = true;
      }

      if (has_curve || has_closed_curve)
      {
        if (!local_location.IsIdentity())
        {
          surface_ptr = occ::down_cast<Geom_Surface>(
            geometric_representation_ptr->Surface()->Transformed(local_location.Transformation()));
        }
        else
        {
          surface_ptr = geometric_representation_ptr->Surface();
        }
        Geom2dAdaptor_Curve              an_adaptor_curve2d(curve2d_ptr);
        GeomAdaptor_Surface              an_adaptor_surface(surface_ptr);
        occ::handle<Geom2dAdaptor_Curve> an_adaptor_curve2d_ptr =
          new Geom2dAdaptor_Curve(an_adaptor_curve2d);
        occ::handle<GeomAdaptor_Surface> an_adaptor_surface_ptr =
          new GeomAdaptor_Surface(an_adaptor_surface);
        Adaptor3d_CurveOnSurface a_curve_on_surface(an_adaptor_curve2d_ptr, an_adaptor_surface_ptr);

        if (BRep_Tool::SameParameter(AnEdge))
        {

          GeomLib::EvalMaxParametricDistance(a_curve_on_surface,
                                             geom_reference_curve,
                                             MinToleranceRequested,
                                             parameters_ptr->Array1(),
                                             max_distance);
        }
        else if (geom_reference_curve_flag)
        {
          GeomLib::EvalMaxDistanceAlongParameter(a_curve_on_surface,
                                                 geom_reference_curve,
                                                 MinToleranceRequested,
                                                 parameters_ptr->Array1(),
                                                 max_distance);
        }
        else
        {

          GeomLib::EvalMaxDistanceAlongParameter(a_curve_on_surface,
                                                 curve_on_surface_reference,
                                                 MinToleranceRequested,
                                                 parameters_ptr->Array1(),
                                                 max_distance);
        }
        max_distance *= safe_factor;
        edge_tolerance = std::max(max_distance, edge_tolerance);
      }
    }
    curve_index += 1;
    second_iterator.Next();
  }

  TE->Tolerance(edge_tolerance);
  return true;
}

//=================================================================================================

bool BRepLib::UpdateEdgeTolerance(const TopoDS_Shape& S,
                                  const double        MinToleranceRequested,
                                  const double        MaxToleranceToCheck)
{
  TopExp_Explorer                                        ex(S, TopAbs_EDGE);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> a_counter;

  bool return_status = false, local_flag;

  while (ex.More())
  {
    if (a_counter.Add(ex.Current()))
    {
      local_flag = BRepLib::UpdateEdgeTol(TopoDS::Edge(ex.Current()),
                                          MinToleranceRequested,
                                          MaxToleranceToCheck);
      if (local_flag && !return_status)
      {
        return_status = true;
      }
    }
    ex.Next();
  }
  return return_status;
}

//=================================================================================================

static void GetEdgeTol(const TopoDS_Edge& theEdge, const TopoDS_Face& theFace, double& theEdTol)
{
  TopLoc_Location                  L;
  const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(theFace, L);
  TopLoc_Location                  l = L.Predivided(theEdge.Location());

  const occ::handle<BRep_TEdge>& TE = *((occ::handle<BRep_TEdge>*)&theEdge.TShape());
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());

  while (itcr.More())
  {
    const occ::handle<BRep_CurveRepresentation>& cr = itcr.Value();
    if (cr->IsCurveOnSurface(S, l))
      return;
    itcr.Next();
  }

  occ::handle<Geom_Plane>                     GP;
  occ::handle<Geom_RectangularTrimmedSurface> GRTS;
  GRTS = occ::down_cast<Geom_RectangularTrimmedSurface>(S);
  if (!GRTS.IsNull())
    GP = occ::down_cast<Geom_Plane>(GRTS->BasisSurface());
  else
    GP = occ::down_cast<Geom_Plane>(S);

  occ::handle<GeomAdaptor_Curve>   HC = new GeomAdaptor_Curve();
  occ::handle<GeomAdaptor_Surface> HS = new GeomAdaptor_Surface();

  TopLoc_Location LC;
  double          First, Last;
  HC->Load(BRep_Tool::Curve(theEdge, LC, First, Last));
  LC = L.Predivided(LC);

  if (!LC.IsIdentity())
  {
    GP = occ::down_cast<Geom_Plane>(GP->Transformed(LC.Transformation()));
  }
  GeomAdaptor_Surface& GAS = *HS;
  GAS.Load(GP);

  ProjLib_ProjectedCurve    Proj(HS, HC);
  occ::handle<Geom2d_Curve> pc = Geom2dAdaptor::MakeCurve(Proj);

  gp_Pln pln     = GAS.Plane();
  double d2      = 0.;
  int    nn      = 23;
  double unsurnn = 1. / nn;
  for (int i = 0; i <= nn; i++)
  {
    double   t     = unsurnn * i;
    double   u     = First * (1. - t) + Last * t;
    gp_Pnt   Pc3d  = HC->Value(u);
    gp_Pnt2d p2d   = pc->Value(u);
    gp_Pnt   Pcons = ElSLib::Value(p2d.X(), p2d.Y(), pln);
    double   eps   = std::max(Pc3d.XYZ().SquareModulus(), Pcons.XYZ().SquareModulus());
    eps            = Epsilon(eps);
    double temp    = Pc3d.SquareDistance(Pcons);
    if (temp <= eps)
    {
      temp = 0.;
    }
    if (temp > d2)
      d2 = temp;
  }
  d2       = 1.05 * sqrt(d2);
  theEdTol = d2;
}

//=======================================================================
// function : UpdTolMap
// purpose  : Update map ShToTol (shape to tolerance)
//=======================================================================
static void UpdTolMap(
  const TopoDS_Shape&                                                 theSh,
  double                                                              theNewTol,
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>& theShToTol)
{
  TopAbs_ShapeEnum aSt = theSh.ShapeType();
  double           aShTol;
  if (aSt == TopAbs_VERTEX)
    aShTol = BRep_Tool::Tolerance(TopoDS::Vertex(theSh));
  else if (aSt == TopAbs_EDGE)
    aShTol = BRep_Tool::Tolerance(TopoDS::Edge(theSh));
  else
    return;
  //
  if (theNewTol > aShTol)
  {
    const double* anOldtol = theShToTol.Seek(theSh);
    if (!anOldtol)
      theShToTol.Bind(theSh, theNewTol);
    else
      theShToTol(theSh) = std::max(*anOldtol, theNewTol);
  }
}

//=======================================================================
// function : UpdShTol
// purpose  : Update vertices/edges/faces according to ShToTol map (create copies of necessary)
//=======================================================================
static void UpdShTol(
  const NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>& theShToTol,
  const bool                                                                IsMutableInput,
  BRepTools_ReShape&                                                        theReshaper,
  bool                                                                      theVForceUpdate)
{
  BRep_Builder                                                                 aB;
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher>::Iterator SHToTolit(
    theShToTol);
  for (; SHToTolit.More(); SHToTolit.Next())
  {
    const TopoDS_Shape& aSh  = SHToTolit.Key();
    double              aTol = SHToTolit.Value();
    //
    TopoDS_Shape        aNsh;
    const TopoDS_Shape& aVsh = theReshaper.Value(aSh);
    bool UseOldSh            = IsMutableInput || theReshaper.IsNewShape(aSh) || !aVsh.IsSame(aSh);
    if (UseOldSh)
      aNsh = aVsh;
    else
    {
      aNsh = aSh.EmptyCopied();
      // add subshapes from the original shape
      TopoDS_Iterator sit(aSh);
      for (; sit.More(); sit.Next())
        aB.Add(aNsh, sit.Value());
      //
      aNsh.Free(aSh.Free());
      aNsh.Checked(aSh.Checked());
      aNsh.Orientable(aSh.Orientable());
      aNsh.Closed(aSh.Closed());
      aNsh.Infinite(aSh.Infinite());
      aNsh.Convex(aSh.Convex());
      //
    }
    //
    switch (aSh.ShapeType())
    {
      case TopAbs_FACE: {
        aB.UpdateFace(TopoDS::Face(aNsh), aTol);
        break;
      }
      case TopAbs_EDGE: {
        aB.UpdateEdge(TopoDS::Edge(aNsh), aTol);
        break;
      }
      case TopAbs_VERTEX: {
        const occ::handle<BRep_TVertex>& aTV = *((occ::handle<BRep_TVertex>*)&aNsh.TShape());
        if (theVForceUpdate)
          aTV->Tolerance(aTol);
        else
          aTV->UpdateTolerance(aTol);
        aTV->Modified(true);
        break;
      }
      default:
        break;
    }
    //
    if (!UseOldSh)
      theReshaper.Replace(aSh, aNsh);
  }
}

//=================================================================================================

static void InternalSameParameter(const TopoDS_Shape& theSh,
                                  BRepTools_ReShape&  theReshaper,
                                  const double        theTol,
                                  const bool          IsForced,
                                  const bool          IsMutableInput)
{
  TopExp_Explorer                                                    ex(theSh, TopAbs_EDGE);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>             Done;
  BRep_Builder                                                       aB;
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> aShToTol;

  while (ex.More())
  {
    const TopoDS_Edge& aCE = TopoDS::Edge(ex.Current());
    if (Done.Add(aCE))
    {
      TopoDS_Edge aNE        = TopoDS::Edge(theReshaper.Value(aCE));
      bool        UseOldEdge = IsMutableInput || theReshaper.IsNewShape(aCE) || !aNE.IsSame(aCE);
      if (IsForced && (BRep_Tool::SameRange(aCE) || BRep_Tool::SameParameter(aCE)))
      {
        if (!UseOldEdge)
        {
          aNE = TopoDS::Edge(aCE.EmptyCopied());
          TopoDS_Iterator sit(aCE);
          for (; sit.More(); sit.Next())
            aB.Add(aNE, sit.Value());
          theReshaper.Replace(aCE, aNE);
          UseOldEdge = true;
        }
        aB.SameRange(aNE, false);
        aB.SameParameter(aNE, false);
      }
      double      aNewTol  = -1;
      TopoDS_Edge aResEdge = BRepLib::SameParameter(aNE, theTol, aNewTol, UseOldEdge);
      if (!UseOldEdge && !aResEdge.IsNull())
        // NE have been empty-copied
        theReshaper.Replace(aNE, aResEdge);
      if (aNewTol > 0)
      {
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(aCE, aV1, aV2);
        if (!aV1.IsNull())
          UpdTolMap(aV1, aNewTol, aShToTol);
        if (!aV2.IsNull())
          UpdTolMap(aV2, aNewTol, aShToTol);
      }
    }
    ex.Next();
  }

  Done.Clear();
  BRepAdaptor_Surface BS;
  for (ex.Init(theSh, TopAbs_FACE); ex.More(); ex.Next())
  {
    const TopoDS_Face& curface = TopoDS::Face(ex.Current());
    if (!Done.Add(curface))
      continue;
    BS.Initialize(curface);
    if (BS.GetType() != GeomAbs_Plane)
      continue;
    TopExp_Explorer ex2;
    for (ex2.Init(curface, TopAbs_EDGE); ex2.More(); ex2.Next())
    {
      const TopoDS_Edge& E = TopoDS::Edge(ex2.Current());
      if (BRep_Tool::Degenerated(E))
        continue;

      TopoDS_Shape aNe      = theReshaper.Value(E);
      double       aNewEtol = -1;
      GetEdgeTol(TopoDS::Edge(aNe), curface, aNewEtol);
      if (aNewEtol >= 0) // not equal to -1
        UpdTolMap(E, aNewEtol, aShToTol);
    }
  }

  //
  UpdShTol(aShToTol, IsMutableInput, theReshaper, false);

  InternalUpdateTolerances(theSh, false, IsMutableInput, theReshaper);
}

//================================================================
// function : SameParameter
// WARNING  : New spec DUB LBO 9/9/97.
//  Recode in the edge the best tolerance found,
//  for vertex extremities it is required to find something else
//================================================================
void BRepLib::SameParameter(const TopoDS_Shape& S, const double Tolerance, const bool forced)
{
  BRepTools_ReShape reshaper;
  InternalSameParameter(S, reshaper, Tolerance, forced, true);
}

//=================================================================================================

void BRepLib::SameParameter(const TopoDS_Shape& S,
                            BRepTools_ReShape&  theReshaper,
                            const double        Tolerance,
                            const bool          forced)
{
  InternalSameParameter(S, theReshaper, Tolerance, forced, false);
}

//=================================================================================================

static bool EvalTol(const occ::handle<Geom2d_Curve>& pc,
                    const occ::handle<Geom_Surface>& s,
                    const GeomAdaptor_Curve&         gac,
                    const double                     tol,
                    double&                          tolbail)
{
  int                 ok = 0;
  double              f  = gac.FirstParameter();
  double              l  = gac.LastParameter();
  Extrema_LocateExtPC Projector;
  Projector.Initialize(gac, f, l, tol);
  double u, v;
  gp_Pnt p;
  tolbail = tol;
  for (int i = 1; i <= 5; i++)
  {
    double t = i / 6.;
    t        = (1. - t) * f + t * l;
    pc->Value(t).Coord(u, v);
    p = s->Value(u, v);
    Projector.Perform(p, t);
    if (Projector.IsDone())
    {
      double dist2 = Projector.SquareDistance();
      if (dist2 > tolbail * tolbail)
        tolbail = sqrt(dist2);
      ok++;
    }
  }
  return (ok > 2);
}

//=================================================================================================

static double ComputeTol(const occ::handle<Adaptor3d_Curve>&   c3d,
                         const occ::handle<Adaptor2d_Curve2d>& c2d,
                         const occ::handle<Adaptor3d_Surface>& surf,
                         const int                             nbp)

{

  NCollection_Array1<double> dist(1, nbp + 10);
  dist.Init(-1.);

  // Adaptor3d_CurveOnSurface  cons(c2d,surf);
  double uf = surf->FirstUParameter(), ul = surf->LastUParameter(), vf = surf->FirstVParameter(),
         vl = surf->LastVParameter();
  double du = 0.01 * (ul - uf), dv = 0.01 * (vl - vf);
  bool   isUPeriodic = surf->IsUPeriodic(), isVPeriodic = surf->IsVPeriodic();
  double DSdu = 1. / surf->UResolution(1.), DSdv = 1. / surf->VResolution(1.);
  double d2    = 0.;
  double first = c3d->FirstParameter();
  double last  = c3d->LastParameter();
  double dapp  = -1.;
  for (int i = 0; i <= nbp; ++i)
  {
    const double t    = IntToReal(i) / IntToReal(nbp);
    const double u    = first * (1. - t) + last * t;
    gp_Pnt       Pc3d = c3d->Value(u);
    gp_Pnt2d     Puv  = c2d->Value(u);
    if (!isUPeriodic)
    {
      if (Puv.X() < uf - du)
      {
        dapp = std::max(dapp, DSdu * (uf - Puv.X()));
        continue;
      }
      else if (Puv.X() > ul + du)
      {
        dapp = std::max(dapp, DSdu * (Puv.X() - ul));
        continue;
      }
    }
    if (!isVPeriodic)
    {
      if (Puv.Y() < vf - dv)
      {
        dapp = std::max(dapp, DSdv * (vf - Puv.Y()));
        continue;
      }
      else if (Puv.Y() > vl + dv)
      {
        dapp = std::max(dapp, DSdv * (Puv.Y() - vl));
        continue;
      }
    }
    gp_Pnt Pcons = surf->Value(Puv.X(), Puv.Y());
    if (Precision::IsInfinite(Pcons.X()) || Precision::IsInfinite(Pcons.Y())
        || Precision::IsInfinite(Pcons.Z()))
    {
      d2 = Precision::Infinite();
      break;
    }
    double temp = Pc3d.SquareDistance(Pcons);

    dist(i + 1) = temp;

    d2 = std::max(d2, temp);
  }

  if (Precision::IsInfinite(d2))
  {
    return d2;
  }

  d2 = std::sqrt(d2);
  if (dapp > d2)
  {
    return dapp;
  }

  bool   ana = false;
  double D2  = 0;
  int    N1  = 0;
  int    N2  = 0;
  int    N3  = 0;

  for (int i = 1; i <= nbp + 10; ++i)
  {
    if (dist(i) > 0)
    {
      if (dist(i) < 1.0)
      {
        ++N1;
      }
      else
      {
        ++N2;
      }
    }
  }

  if (N1 > N2 && N2 != 0)
  {
    N3 = 100 * N2 / (N1 + N2);
  }
  if (N3 < 10 && N3 != 0)
  {
    ana = true;
    for (int i = 1; i <= nbp + 10; ++i)
    {
      if (dist(i) > 0 && dist(i) < 1.0)
      {
        D2 = std::max(D2, dist(i));
      }
    }
  }

  // d2 = 1.5*sqrt(d2);
  d2 = (!ana) ? 1.5 * d2 : 1.5 * sqrt(D2);
  d2 = std::max(d2, 1.e-7);
  return d2;
}

//=================================================================================================

static void GetCurve3d(const TopoDS_Edge&                                       theEdge,
                       occ::handle<Geom_Curve>&                                 theC3d,
                       double&                                                  theF3d,
                       double&                                                  theL3d,
                       TopLoc_Location&                                         theLoc3d,
                       NCollection_List<occ::handle<BRep_CurveRepresentation>>& theCList)
{
  const occ::handle<BRep_TEdge>& aTE = *((occ::handle<BRep_TEdge>*)&theEdge.TShape());
  // clang-format off
  theCList = aTE->ChangeCurves(); // current function (i.e. GetCurve3d()) will not change any of this curves
  // clang-format on
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator anIt(theCList);
  bool                                                              NotDone = true;
  while (NotDone && anIt.More())
  {
    occ::handle<BRep_GCurve> GCurve = occ::down_cast<BRep_GCurve>(anIt.Value());
    if (!GCurve.IsNull() && GCurve->IsCurve3D())
    {
      theC3d   = GCurve->Curve3D();
      theF3d   = GCurve->First();
      theL3d   = GCurve->Last();
      theLoc3d = GCurve->Location();
      NotDone  = false;
    }
    anIt.Next();
  }
}

//=================================================================================================

void UpdateVTol(const TopoDS_Vertex& theV1, const TopoDS_Vertex& theV2, double theTol)
{
  BRep_Builder aB;
  if (!theV1.IsNull())
    aB.UpdateVertex(theV1, theTol);
  if (!theV2.IsNull())
    aB.UpdateVertex(theV2, theTol);
}

//=================================================================================================

void BRepLib::SameParameter(const TopoDS_Edge& theEdge, const double theTolerance)
{
  double aNewTol = -1;
  SameParameter(theEdge, theTolerance, aNewTol, true);
  if (aNewTol > 0)
  {
    TopoDS_Vertex aV1, aV2;
    TopExp::Vertices(theEdge, aV1, aV2);
    UpdateVTol(aV1, aV2, aNewTol);
  }
}

//=================================================================================================

TopoDS_Edge BRepLib::SameParameter(const TopoDS_Edge& theEdge,
                                   const double       theTolerance,
                                   double&            theNewTol,
                                   bool               IsUseOldEdge)
{
  if (BRep_Tool::SameParameter(theEdge))
    return TopoDS_Edge();
  double                                                  f3d = 0., l3d = 0.;
  TopLoc_Location                                         L3d;
  occ::handle<Geom_Curve>                                 C3d;
  NCollection_List<occ::handle<BRep_CurveRepresentation>> CList;
  GetCurve3d(theEdge, C3d, f3d, l3d, L3d, CList);
  if (C3d.IsNull())
    return TopoDS_Edge();

  BRep_Builder            B;
  TopoDS_Edge             aNE;
  occ::handle<BRep_TEdge> aNTE;
  if (IsUseOldEdge)
  {
    aNE  = theEdge;
    aNTE = *((occ::handle<BRep_TEdge>*)&theEdge.TShape());
  }
  else
  {
    // clang-format off
    aNE = TopoDS::Edge(theEdge.EmptyCopied()); //will be modified a little bit later, so copy anyway  
    GetCurve3d(aNE, C3d, f3d, l3d, L3d, CList); //C3d pointer and CList will be differ after copying
    // clang-format on
    aNTE = *((occ::handle<BRep_TEdge>*)&aNE.TShape());
    TopoDS_Iterator sit(theEdge);
    for (; sit.More(); sit.Next()) // add vertices from old edge to the new ones
      B.Add(aNE, sit.Value());
  }

  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator It(CList);

  const int NCONTROL = 22;

  occ::handle<GeomAdaptor_Curve>   HC    = new GeomAdaptor_Curve();
  occ::handle<Geom2dAdaptor_Curve> HC2d  = new Geom2dAdaptor_Curve();
  occ::handle<GeomAdaptor_Surface> HS    = new GeomAdaptor_Surface();
  GeomAdaptor_Curve&               GAC   = *HC;
  Geom2dAdaptor_Curve&             GAC2d = *HC2d;
  GeomAdaptor_Surface&             GAS   = *HS;

  // modified by NIZHNY-OCC486  Tue Aug 27 17:15:13 2002 :
  bool                       m_TrimmedPeriodical = false;
  occ::handle<Standard_Type> TheType             = C3d->DynamicType();
  if (TheType == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    occ::handle<Geom_Curve> gtC(occ::down_cast<Geom_TrimmedCurve>(C3d)->BasisCurve());
    m_TrimmedPeriodical = gtC->IsPeriodic();
  }
  // modified by NIZHNY-OCC486  Tue Aug 27 17:15:17 2002 .

  if (!C3d->IsPeriodic())
  {
    double Udeb = C3d->FirstParameter();
    double Ufin = C3d->LastParameter();
    // modified by NIZHNY-OCC486  Tue Aug 27 17:17:14 2002 :
    // if (Udeb > f3d) f3d = Udeb;
    // if (l3d > Ufin) l3d = Ufin;
    if (!m_TrimmedPeriodical)
    {
      if (Udeb > f3d)
        f3d = Udeb;
      if (l3d > Ufin)
        l3d = Ufin;
    }
    // modified by NIZHNY-OCC486  Tue Aug 27 17:17:55 2002 .
  }
  if (!L3d.IsIdentity())
  {
    C3d = occ::down_cast<Geom_Curve>(C3d->Transformed(L3d.Transformation()));
  }
  GAC.Load(C3d, f3d, l3d);

  double Prec_C3d = BRepCheck::PrecCurve(GAC);

  bool   IsSameP = true;
  double maxdist = 0.;

  //  Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898 Begin
  double anEdgeTol = BRep_Tool::Tolerance(aNE);
  //  Modified by skv - Thu Jun  3 12:39:20 2004 OCC5898 End
  bool         SameRange = BRep_Tool::SameRange(aNE);
  bool         YaPCu     = false;
  const double BigError  = 1.e10;
  It.Initialize(CList);

  while (It.More())
  {

    bool                      isANA  = false;
    bool                      isBSP  = false;
    occ::handle<BRep_GCurve>  GCurve = occ::down_cast<BRep_GCurve>(It.Value());
    occ::handle<Geom2d_Curve> PC[2];
    occ::handle<Geom_Surface> S;
    if (!GCurve.IsNull() && GCurve->IsCurveOnSurface())
    {
      YaPCu                 = true;
      PC[0]                 = GCurve->PCurve();
      TopLoc_Location PCLoc = GCurve->Location();
      S                     = GCurve->Surface();
      if (!PCLoc.IsIdentity())
      {
        S = occ::down_cast<Geom_Surface>(S->Transformed(PCLoc.Transformation()));
      }

      GAS.Load(S);
      if (GCurve->IsCurveOnClosedSurface())
      {
        PC[1] = GCurve->PCurve2();
      }

      // Eval tol2d to compute SameRange
      double TolSameRange = std::max(GAC.Resolution(theTolerance), Precision::PConfusion());
      for (int i = 0; i < 2; i++)
      {
        occ::handle<Geom2d_Curve> curPC    = PC[i];
        bool                      updatepc = false;
        if (curPC.IsNull())
          break;
        if (!SameRange)
        {
          GeomLib::SameRange(TolSameRange, PC[i], GCurve->First(), GCurve->Last(), f3d, l3d, curPC);

          updatepc = (curPC != PC[i]);
        }
        bool goodpc = true;
        GAC2d.Load(curPC, f3d, l3d);

        double error = ComputeTol(HC, HC2d, HS, NCONTROL);

        if (error > BigError)
        {
          maxdist = error;
          break;
        }

        if (GAC2d.GetType() == GeomAbs_BSplineCurve && GAC2d.Continuity() == GeomAbs_C0)
        {
          double UResol                            = GAS.UResolution(theTolerance);
          double VResol                            = GAS.VResolution(theTolerance);
          double TolConf2d                         = std::min(UResol, VResol);
          TolConf2d                                = std::max(TolConf2d, Precision::PConfusion());
          occ::handle<Geom2d_BSplineCurve> bs2d    = GAC2d.BSpline();
          occ::handle<Geom2d_BSplineCurve> bs2dsov = bs2d;
          double   fC0 = bs2d->FirstParameter(), lC0 = bs2d->LastParameter();
          bool     repar = true;
          gp_Pnt2d OriginPoint;
          bs2d->D0(fC0, OriginPoint);
          Geom2dConvert::C0BSplineToC1BSplineCurve(bs2d, TolConf2d);
          isBSP = true;

          if (bs2d->IsPeriodic())
          { // -------- IFV, Jan 2000
            gp_Pnt2d NewOriginPoint;
            bs2d->D0(bs2d->FirstParameter(), NewOriginPoint);
            if (std::abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion()
                || std::abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion())
            {

              NCollection_Array1<double> Knotbs2d(bs2d->Knots());

              for (int Index = 1; Index <= bs2d->NbKnots(); Index++)
              {
                bs2d->D0(Knotbs2d(Index), NewOriginPoint);
                if (std::abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion()
                    || std::abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion())
                  continue;

                bs2d->SetOrigin(Index);
                break;
              }
            }
          }

          if (bs2d->Continuity() == GeomAbs_C0)
          {
            double tolbail;
            if (EvalTol(curPC, S, GAC, theTolerance, tolbail))
            {
              bs2d             = bs2dsov;
              double UResbail  = GAS.UResolution(tolbail);
              double VResbail  = GAS.VResolution(tolbail);
              double Tol2dbail = std::min(UResbail, VResbail);
              bs2d->D0(bs2d->FirstParameter(), OriginPoint);

              int                                 nbp   = bs2d->NbPoles();
              const NCollection_Array1<gp_Pnt2d>& poles = bs2d->Poles();
              gp_Pnt2d                            p     = poles(1), p1;
              double                              d     = Precision::Infinite();
              for (int ip = 2; ip <= nbp; ip++)
              {
                p1 = poles(ip);
                d  = std::min(d, p.SquareDistance(p1));
                p  = p1;
              }
              d = sqrt(d) * .1;

              Tol2dbail = std::max(std::min(Tol2dbail, d), TolConf2d);

              Geom2dConvert::C0BSplineToC1BSplineCurve(bs2d, Tol2dbail);

              if (bs2d->IsPeriodic())
              { // -------- IFV, Jan 2000
                gp_Pnt2d NewOriginPoint;
                bs2d->D0(bs2d->FirstParameter(), NewOriginPoint);
                if (std::abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion()
                    || std::abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion())
                {

                  const NCollection_Array1<double>& Knotbs2d = bs2d->Knots();

                  for (int Index = 1; Index <= bs2d->NbKnots(); Index++)
                  {
                    bs2d->D0(Knotbs2d(Index), NewOriginPoint);
                    if (std::abs(OriginPoint.X() - NewOriginPoint.X()) > Precision::PConfusion()
                        || std::abs(OriginPoint.Y() - NewOriginPoint.Y()) > Precision::PConfusion())
                      continue;

                    bs2d->SetOrigin(Index);
                    break;
                  }
                }
              }

              if (bs2d->Continuity() == GeomAbs_C0)
              {
                goodpc = true;
                bs2d   = bs2dsov;
                repar  = false;
              }
            }
            else
              goodpc = false;
          }

          if (goodpc)
          {
            if (repar)
            {
              NCollection_Array1<double> Knots(bs2d->Knots());
              //	    BSplCLib::Reparametrize(f3d,l3d,Knots);
              BSplCLib::Reparametrize(fC0, lC0, Knots);
              bs2d->SetKnots(Knots);
              GAC2d.Load(bs2d, f3d, l3d);
              curPC            = bs2d;
              bool updatepcsov = updatepc;
              updatepc         = true;

              double error1 = ComputeTol(HC, HC2d, HS, NCONTROL);
              if (error1 > error)
              {
                bs2d = bs2dsov;
                GAC2d.Load(bs2d, f3d, l3d);
                curPC    = bs2d;
                updatepc = updatepcsov;
                isANA    = true;
              }
              else
              {
                error = error1;
              }
            }

            // check, if new BSpline "good" or not --------- IFV, Jan of 2000
            GeomAbs_Shape cont  = bs2d->Continuity();
            bool          IsBad = false;

            if (cont > GeomAbs_C0 && error > std::max(1.e-3, theTolerance))
            {
              int                               NbKnots   = bs2d->NbKnots();
              const NCollection_Array1<double>& Knots     = bs2d->Knots();
              double                            critratio = 10.;
              double                            dtprev = Knots(2) - Knots(1), dtratio = 1.;
              double                            dtmin = dtprev;
              double                            dtcur;
              for (int j = 2; j < NbKnots; j++)
              {
                dtcur = Knots(j + 1) - Knots(j);
                dtmin = std::min(dtmin, dtcur);

                if (IsBad)
                  continue;

                if (dtcur > dtprev)
                  dtratio = dtcur / dtprev;
                else
                  dtratio = dtprev / dtcur;
                if (dtratio > critratio)
                {
                  IsBad = true;
                }
                dtprev = dtcur;
              }
              if (IsBad)
              {
                // To avoid failures in Approx_CurvilinearParameter
                bs2d->Resolution(std::max(1.e-3, theTolerance), dtcur);
                if (dtmin < dtcur)
                  IsBad = false;
              }
            }

            if (IsBad)
            { // if BSpline "bad", try to reparametrize it
              // by its curve length

              //	      GeomAbs_Shape cont = bs2d->Continuity();
              if (cont > GeomAbs_C2)
                cont = GeomAbs_C2;
              int maxdeg = bs2d->Degree();
              if (maxdeg == 1)
                maxdeg = 14;
              Approx_CurvilinearParameter AppCurPar(HC2d,
                                                    HS,
                                                    std::max(1.e-3, theTolerance),
                                                    cont,
                                                    maxdeg,
                                                    10);
              if (AppCurPar.IsDone() || AppCurPar.HasResult())
              {
                bs2d = AppCurPar.Curve2d1();
                GAC2d.Load(bs2d, f3d, l3d);
                curPC = bs2d;

                if (std::abs(bs2d->FirstParameter() - fC0) > TolSameRange
                    || std::abs(bs2d->LastParameter() - lC0) > TolSameRange)
                {
                  NCollection_Array1<double> Knots(bs2d->Knots());
                  //		  BSplCLib::Reparametrize(f3d,l3d,Knots);
                  BSplCLib::Reparametrize(fC0, lC0, Knots);
                  bs2d->SetKnots(Knots);
                  GAC2d.Load(bs2d, f3d, l3d);
                  curPC = bs2d;
                }
              }
            }
          }
        }

        if (goodpc)
        {
          //	  Approx_SameParameter SameP(HC,HC2d,HS,Tolerance);
          double                                aTol     = (isANA && isBSP) ? 1.e-7 : theTolerance;
          const occ::handle<Adaptor3d_Curve>&   aHCurv   = HC;   // to avoid ambiguity
          const occ::handle<Adaptor2d_Curve2d>& aHCurv2d = HC2d; // to avoid ambiguity
          Approx_SameParameter                  SameP(aHCurv, aHCurv2d, HS, aTol);

          if (SameP.IsSameParameter())
          {
            maxdist = std::max(maxdist, SameP.TolReached());
            if (updatepc)
            {
              if (i == 0)
                GCurve->PCurve(curPC);
              else
                GCurve->PCurve2(curPC);
            }
          }
          else if (SameP.IsDone())
          {
            double tolreached = SameP.TolReached();
            if (tolreached <= error)
            {
              curPC    = SameP.Curve2d();
              updatepc = true;
              maxdist  = std::max(maxdist, tolreached);
            }
            else
            {
              maxdist = std::max(maxdist, error);
            }
            if (updatepc)
            {
              if (i == 0)
                GCurve->PCurve(curPC);
              else
                GCurve->PCurve2(curPC);
            }
          }
          else
          {
            // Approx_SameParameter has failed.
            // Consequently, the situation might be,
            // when 3D and 2D-curve do not have same-range.
            GeomLib::SameRange(TolSameRange,
                               PC[i],
                               GCurve->First(),
                               GCurve->Last(),
                               f3d,
                               l3d,
                               curPC);

            if (i == 0)
              GCurve->PCurve(curPC);
            else
              GCurve->PCurve2(curPC);

            IsSameP = false;
          }
        }
        else
          IsSameP = false;

        //  Modified by skv - Thu Jun  3 12:39:19 2004 OCC5898 Begin
        if (!IsSameP)
        {
          double Prec_Surf = BRepCheck::PrecSurface(HS);
          double CurTol    = anEdgeTol + std::max(Prec_C3d, Prec_Surf);
          if (CurTol >= error)
          {
            maxdist = std::max(maxdist, anEdgeTol);
            IsSameP = true;
          }
        }
        //  Modified by skv - Thu Jun  3 12:39:20 2004 OCC5898 End
      }
    }
    It.Next();
  }
  B.Range(aNE, f3d, l3d);
  B.SameRange(aNE, true);
  if (IsSameP)
  {
    // Reduce eventually the tolerance of the edge, as
    // all its representations are processed (except for some associated
    // to planes and not stored in the edge !)
    // The same cannot be done with vertices that cannot be enlarged
    // or left as is.
    if (YaPCu)
    {
      // Avoid setting too small tolerances.
      maxdist   = std::max(maxdist, Precision::Confusion());
      theNewTol = maxdist;
      aNTE->Modified(true);
      aNTE->Tolerance(maxdist);
    }
    B.SameParameter(aNE, true);
  }

  return aNE;
}

//=================================================================================================

static void InternalUpdateTolerances(const TopoDS_Shape& theOldShape,
                                     const bool          IsVerifyTolerance,
                                     const bool          IsMutableInput,
                                     BRepTools_ReShape&  theReshaper)
{
  NCollection_DataMap<TopoDS_Shape, double, TopTools_ShapeMapHasher> aShToTol;
  // Harmonize tolerances
  // with rule Tolerance(VERTEX)>=Tolerance(EDGE)>=Tolerance(FACE)
  double tol = 0;
  if (IsVerifyTolerance)
  {
    // Set tolerance to its minimum value
    occ::handle<Geom_Surface> S;
    TopLoc_Location           l;
    TopExp_Explorer           ex;
    Bnd_Box                   aB;
    double                    aXmin, aYmin, aZmin, aXmax, aYmax, aZmax, dMax;
    for (ex.Init(theOldShape, TopAbs_FACE); ex.More(); ex.Next())
    {
      const TopoDS_Face& curf = TopoDS::Face(ex.Current());
      S                       = BRep_Tool::Surface(curf, l);
      if (!S.IsNull())
      {
        aB.SetVoid();
        BRepBndLib::Add(curf, aB);
        if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
        {
          S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
        }
        GeomAdaptor_Surface AS(S);
        switch (AS.GetType())
        {
          case GeomAbs_Plane:
          case GeomAbs_Cylinder:
          case GeomAbs_Cone: {
            tol = Precision::Confusion();
            break;
          }
          case GeomAbs_Sphere:
          case GeomAbs_Torus: {
            tol = Precision::Confusion() * 2;
            break;
          }
          default:
            tol = Precision::Confusion() * 4;
        }
        if (!aB.IsWhole())
        {
          aB.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
          dMax = 1.;
          if (!aB.IsOpenXmin() && !aB.IsOpenXmax())
            dMax = aXmax - aXmin;
          if (!aB.IsOpenYmin() && !aB.IsOpenYmax())
            aYmin = aYmax - aYmin;
          if (!aB.IsOpenZmin() && !aB.IsOpenZmax())
            aZmin = aZmax - aZmin;
          if (aYmin > dMax)
            dMax = aYmin;
          if (aZmin > dMax)
            dMax = aZmin;
          tol = tol * dMax;
          // Do not process tolerances > 1.
          if (tol > 1.)
            tol = 0.99;
        }
        aShToTol.Bind(curf, tol);
      }
    }
  }

  // Process edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    parents;
  TopExp::MapShapesAndAncestors(theOldShape, TopAbs_EDGE, TopAbs_FACE, parents);
  NCollection_List<TopoDS_Shape>::Iterator lConx;
  int                                      iCur;
  for (iCur = 1; iCur <= parents.Extent(); iCur++)
  {
    tol = 0;
    for (lConx.Initialize(parents(iCur)); lConx.More(); lConx.Next())
    {
      const TopoDS_Face& FF = TopoDS::Face(lConx.Value());
      double             Ftol;
      if (IsVerifyTolerance && aShToTol.IsBound(FF)) // first condition for speed-up
        Ftol = aShToTol(FF);
      else
        Ftol = BRep_Tool::Tolerance(FF); // tolerance have not been updated
      tol = std::max(tol, Ftol);
    }
    // Update can only increase tolerance, so if the edge has a greater
    //  tolerance than its faces it is not concerned
    const TopoDS_Edge& EK = TopoDS::Edge(parents.FindKey(iCur));
    if (tol > BRep_Tool::Tolerance(EK))
      aShToTol.Bind(EK, tol);
  }

  // Vertices are processed
  const double BigTol = 1.e10;
  parents.Clear();

  TopExp::MapShapesAndUniqueAncestors(theOldShape, TopAbs_VERTEX, TopAbs_EDGE, parents);
  NCollection_Map<occ::handle<Standard_Transient>> Initialized;
  int                                              nbV = parents.Extent();
  for (iCur = 1; iCur <= nbV; iCur++)
  {
    tol                           = 0;
    const TopoDS_Vertex& V        = TopoDS::Vertex(parents.FindKey(iCur));
    gp_Pnt               aPV      = BRep_Tool::Pnt(V);
    double               aMaxDist = 0.;
    gp_Pnt               p3d;
    for (lConx.Initialize(parents(iCur)); lConx.More(); lConx.Next())
    {
      const TopoDS_Edge& E     = TopoDS::Edge(lConx.Value());
      const double*      aNtol = aShToTol.Seek(E);
      tol                      = std::max(tol, aNtol ? *aNtol : BRep_Tool::Tolerance(E));
      if (tol > BigTol)
        continue;
      if (!BRep_Tool::SameRange(E))
        continue;
      double                   par = BRep_Tool::Parameter(V, E);
      occ::handle<BRep_TEdge>& TE  = *((occ::handle<BRep_TEdge>*)&E.TShape());
      NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itcr(TE->Curves());
      const TopLoc_Location&                                            Eloc = E.Location();
      while (itcr.More())
      {
        // For each CurveRepresentation, check the provided parameter
        const occ::handle<BRep_CurveRepresentation>& cr  = itcr.Value();
        const TopLoc_Location&                       loc = cr->Location();
        TopLoc_Location                              L   = (Eloc * loc);
        if (cr->IsCurve3D())
        {
          const occ::handle<Geom_Curve>& C = cr->Curve3D();
          if (!C.IsNull())
          { // edge non degenerated
            p3d = C->Value(par);
            p3d.Transform(L.Transformation());
            double aDist = p3d.SquareDistance(aPV);
            if (aDist > aMaxDist)
              aMaxDist = aDist;
          }
        }
        else if (cr->IsCurveOnSurface())
        {
          const occ::handle<Geom_Surface>& Su = cr->Surface();
          const occ::handle<Geom2d_Curve>& PC = cr->PCurve();
          occ::handle<Geom2d_Curve>        PC2;
          if (cr->IsCurveOnClosedSurface())
          {
            PC2 = cr->PCurve2();
          }
          gp_Pnt2d p2d = PC->Value(par);
          p3d          = Su->Value(p2d.X(), p2d.Y());
          p3d.Transform(L.Transformation());
          double aDist = p3d.SquareDistance(aPV);
          if (aDist > aMaxDist)
            aMaxDist = aDist;
          if (!PC2.IsNull())
          {
            p2d = PC2->Value(par);
            p3d = Su->Value(p2d.X(), p2d.Y());
            p3d.Transform(L.Transformation());
            aDist = p3d.SquareDistance(aPV);
            if (aDist > aMaxDist)
              aMaxDist = aDist;
          }
        }
        itcr.Next();
      }
    }
    tol = std::max(tol, sqrt(aMaxDist));
    tol += 2. * Epsilon(tol);
    //
    double                           aVTol    = BRep_Tool::Tolerance(V);
    bool                             anUpdTol = tol > aVTol;
    const occ::handle<BRep_TVertex>& aTV      = *((occ::handle<BRep_TVertex>*)&V.TShape());
    bool                             toAdd    = false;
    if (IsVerifyTolerance)
    {
      // ASet minimum value of the tolerance
      // Attention to sharing of the vertex by other shapes
      toAdd = Initialized.Add(aTV) && aVTol != tol; // if Vtol == tol => no need to update toler
    }
    //'Initialized' map is not used anywhere outside this block
    if (anUpdTol || toAdd)
      aShToTol.Bind(V, tol);
  }

  UpdShTol(aShToTol, IsMutableInput, theReshaper, true);
}

//=================================================================================================

void BRepLib::UpdateTolerances(const TopoDS_Shape& S, const bool verifyFaceTolerance)
{
  BRepTools_ReShape aReshaper;
  InternalUpdateTolerances(S, verifyFaceTolerance, true, aReshaper);
}

//=================================================================================================

void BRepLib::UpdateTolerances(const TopoDS_Shape& S,
                               BRepTools_ReShape&  theReshaper,
                               const bool          verifyFaceTolerance)
{
  InternalUpdateTolerances(S, verifyFaceTolerance, false, theReshaper);
}

//=================================================================================================

void BRepLib::UpdateInnerTolerances(const TopoDS_Shape& aShape)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    EFmap;
  TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, EFmap);
  BRep_Builder BB;
  for (int i = 1; i <= EFmap.Extent(); i++)
  {
    TopoDS_Edge anEdge = TopoDS::Edge(EFmap.FindKey(i));

    if (!BRep_Tool::IsGeometric(anEdge))
    {
      continue;
    }

    TopoDS_Vertex V1, V2;
    TopExp::Vertices(anEdge, V1, V2);
    double fpar, lpar;
    BRep_Tool::Range(anEdge, fpar, lpar);
    double                         TolEdge = BRep_Tool::Tolerance(anEdge);
    gp_Pnt                         Pnt1, Pnt2;
    occ::handle<BRepAdaptor_Curve> anHCurve = new BRepAdaptor_Curve();
    anHCurve->Initialize(anEdge);
    if (!V1.IsNull())
      Pnt1 = BRep_Tool::Pnt(V1);
    if (!V2.IsNull())
      Pnt2 = BRep_Tool::Pnt(V2);

    if (!BRep_Tool::Degenerated(anEdge) && EFmap(i).Extent() > 0)
    {
      NCollection_Sequence<occ::handle<Adaptor3d_Curve>> theRep;
      theRep.Append(anHCurve);
      NCollection_List<TopoDS_Shape>::Iterator itl(EFmap(i));
      for (; itl.More(); itl.Next())
      {
        const TopoDS_Face&             aFace         = TopoDS::Face(itl.Value());
        occ::handle<BRepAdaptor_Curve> anHCurvOnSurf = new BRepAdaptor_Curve();
        anHCurvOnSurf->Initialize(anEdge, aFace);
        theRep.Append(anHCurvOnSurf);
      }

      const int NbSamples = (BRep_Tool::SameParameter(anEdge)) ? 23 : 2;
      double    delta     = (lpar - fpar) / (NbSamples - 1);
      double    MaxDist   = 0.;
      for (int j = 2; j <= theRep.Length(); j++)
      {
        for (int k = 0; k <= NbSamples; k++)
        {
          double ParamOnCenter = (k == NbSamples) ? lpar : fpar + k * delta;
          gp_Pnt Center        = theRep(1)->Value(ParamOnCenter);
          double ParamOnCurve =
            (BRep_Tool::SameParameter(anEdge))
              ? ParamOnCenter
              : ((k == 0) ? theRep(j)->FirstParameter() : theRep(j)->LastParameter());
          gp_Pnt aPoint = theRep(j)->Value(ParamOnCurve);
          double aDist  = Center.Distance(aPoint);
          // aDist *= 1.1;
          aDist += 2. * Epsilon(aDist);
          if (aDist > MaxDist)
            MaxDist = aDist;

          // Update tolerances of vertices
          if (k == 0 && !V1.IsNull())
          {
            double aDist1 = Pnt1.Distance(aPoint);
            aDist1 += 2. * Epsilon(aDist1);
            BB.UpdateVertex(V1, aDist1);
          }
          if (k == NbSamples && !V2.IsNull())
          {
            double aDist2 = Pnt2.Distance(aPoint);
            aDist2 += 2. * Epsilon(aDist2);
            BB.UpdateVertex(V2, aDist2);
          }
        }
      }
      BB.UpdateEdge(anEdge, MaxDist);
    }
    TolEdge = BRep_Tool::Tolerance(anEdge);
    if (!V1.IsNull())
    {
      gp_Pnt End1  = anHCurve->Value(fpar);
      double dist1 = Pnt1.Distance(End1);
      dist1 += 2. * Epsilon(dist1);
      BB.UpdateVertex(V1, std::max(dist1, TolEdge));
    }
    if (!V2.IsNull())
    {
      gp_Pnt End2  = anHCurve->Value(lpar);
      double dist2 = Pnt2.Distance(End2);
      dist2 += 2. * Epsilon(dist2);
      BB.UpdateVertex(V2, std::max(dist2, TolEdge));
    }
  }
}

//=================================================================================================

bool BRepLib::OrientClosedSolid(TopoDS_Solid& solid)
{
  // Set material inside the solid
  BRepClass3d_SolidClassifier where(solid);
  where.PerformInfinitePoint(Precision::Confusion());
  if (where.State() == TopAbs_IN)
  {
    solid.Reverse();
  }
  else if (where.State() == TopAbs_ON || where.State() == TopAbs_UNKNOWN)
    return false;

  return true;
}

// Structure for calculation of properties, necessary for decision about continuity
class SurfaceProperties
{
public:
  SurfaceProperties(const occ::handle<Geom_Surface>& theSurface,
                    const gp_Trsf&                   theSurfaceTrsf,
                    const occ::handle<Geom2d_Curve>& theCurve2D,
                    const bool                       theReversed)
      : mySurfaceProps(theSurface, 2, Precision::Confusion()),
        mySurfaceTrsf(theSurfaceTrsf),
        myCurve2d(theCurve2D),
        myIsReversed(theReversed)
  {
  }

  // Calculate derivatives on surface related to the point on curve
  void Calculate(const double theParamOnCurve)
  {
    gp_Pnt2d aUV;
    myCurve2d->D1(theParamOnCurve, aUV, myCurveTangent);
    mySurfaceProps.SetParameters(aUV.X(), aUV.Y());
  }

  // Returns point just calculated
  gp_Pnt Value() { return mySurfaceProps.Value().Transformed(mySurfaceTrsf); }

  // Calculate a derivative orthogonal to curve's tangent vector
  gp_Vec Derivative()
  {
    gp_Vec aDeriv;
    // direction orthogonal to tangent vector of the curve
    gp_Vec2d anOrtho(-myCurveTangent.Y(), myCurveTangent.X());
    double   aLen = anOrtho.Magnitude();
    if (aLen < Precision::Confusion())
      return aDeriv;
    anOrtho /= aLen;
    if (myIsReversed)
      anOrtho.Reverse();

    aDeriv.SetLinearForm(anOrtho.X(), mySurfaceProps.D1U(), anOrtho.Y(), mySurfaceProps.D1V());
    return aDeriv.Transformed(mySurfaceTrsf);
  }

  gp_Dir Normal()
  {
    gp_Dir aNormal = mySurfaceProps.Normal();
    return aNormal.Transformed(mySurfaceTrsf);
  }

  // Calculate principal curvatures, which consist of minimal and maximal normal curvatures and
  // the directions on the tangent plane (principal direction) where the extremums are reached
  void Curvature(gp_Dir& thePrincipalDir1,
                 double& theCurvature1,
                 gp_Dir& thePrincipalDir2,
                 double& theCurvature2)
  {
    mySurfaceProps.CurvatureDirections(thePrincipalDir1, thePrincipalDir2);
    theCurvature1 = mySurfaceProps.MaxCurvature();
    theCurvature2 = mySurfaceProps.MinCurvature();
    if (myIsReversed)
    {
      theCurvature1 = -theCurvature1;
      theCurvature2 = -theCurvature2;
    }
    if (mySurfaceTrsf.IsNegative())
    {
      theCurvature1 = -theCurvature1;
      theCurvature2 = -theCurvature2;
    }

    thePrincipalDir1.Transform(mySurfaceTrsf);
    thePrincipalDir2.Transform(mySurfaceTrsf);
  }

private:
  GeomLProp_SLProps         mySurfaceProps; // properties calculator
  gp_Trsf                   mySurfaceTrsf;
  occ::handle<Geom2d_Curve> myCurve2d;
  bool                      myIsReversed; // the face based on the surface is reversed

  // tangent vector to Pcurve in UV
  gp_Vec2d myCurveTangent;
};

//=======================================================================
// function : tgtfaces
// purpose  : check the angle at the border between two squares.
//           Two shares should have a shared front edge.
//=======================================================================
GeomAbs_Shape BRepLib::ContinuityOfFaces(const TopoDS_Edge& theEdge,
                                         const TopoDS_Face& theFace1,
                                         const TopoDS_Face& theFace2,
                                         const double       theAngleTol)
{
  bool isSeam = theFace1.IsEqual(theFace2);

  TopoDS_Edge               anEdgeInFace1, anEdgeInFace2;
  occ::handle<Geom2d_Curve> aCurve1, aCurve2;

  double aFirst, aLast;

  if (!theFace1.IsSame(theFace2) && BRep_Tool::IsClosed(theEdge, theFace1)
      && BRep_Tool::IsClosed(theEdge, theFace2))
  {
    // Find the edge in the face 1: this edge will have correct orientation
    TopoDS_Face aFace1 = theFace1;
    aFace1.Orientation(TopAbs_FORWARD);
    TopExp_Explorer anExplo(aFace1, TopAbs_EDGE);
    for (; anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge(anExplo.Current());
      if (anEdge.IsSame(theEdge))
      {
        anEdgeInFace1 = anEdge;
        break;
      }
    }
    if (anEdgeInFace1.IsNull())
      return GeomAbs_C0;

    aCurve1            = BRep_Tool::CurveOnSurface(anEdgeInFace1, aFace1, aFirst, aLast);
    TopoDS_Face aFace2 = theFace2;
    aFace2.Orientation(TopAbs_FORWARD);
    anEdgeInFace2 = anEdgeInFace1;
    anEdgeInFace2.Reverse();
    aCurve2 = BRep_Tool::CurveOnSurface(anEdgeInFace2, aFace2, aFirst, aLast);
  }
  else
  {
    // Obtaining of pcurves of edge on two faces.
    anEdgeInFace1 = anEdgeInFace2 = theEdge;
    aCurve1 = BRep_Tool::CurveOnSurface(anEdgeInFace1, theFace1, aFirst, aLast);
    // For the case of seam edge
    if (theFace1.IsSame(theFace2))
      anEdgeInFace2.Reverse();
    aCurve2 = BRep_Tool::CurveOnSurface(anEdgeInFace2, theFace2, aFirst, aLast);
  }

  if (aCurve1.IsNull() || aCurve2.IsNull())
    return GeomAbs_C0;

  TopLoc_Location           aLoc1, aLoc2;
  occ::handle<Geom_Surface> aSurface1  = BRep_Tool::Surface(theFace1, aLoc1);
  const gp_Trsf&            aSurf1Trsf = aLoc1.Transformation();
  occ::handle<Geom_Surface> aSurface2  = BRep_Tool::Surface(theFace2, aLoc2);
  const gp_Trsf&            aSurf2Trsf = aLoc2.Transformation();

  if (aSurface1->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurface1 = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface1)->BasisSurface();
  if (aSurface2->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    aSurface2 = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurface2)->BasisSurface();

  // seam edge on elementary surface is always CN
  bool isElementary = (aSurface1->IsKind(STANDARD_TYPE(Geom_ElementarySurface))
                       && aSurface2->IsKind(STANDARD_TYPE(Geom_ElementarySurface)));
  if (isSeam && isElementary)
  {
    return GeomAbs_CN;
  }

  SurfaceProperties aSP1(aSurface1, aSurf1Trsf, aCurve1, theFace1.Orientation() == TopAbs_REVERSED);
  SurfaceProperties aSP2(aSurface2, aSurf2Trsf, aCurve2, theFace2.Orientation() == TopAbs_REVERSED);

  double f, l, eps;
  BRep_Tool::Range(theEdge, f, l);
  Extrema_LocateExtPC            ext;
  occ::handle<BRepAdaptor_Curve> aHC2;

  eps = (l - f) / 100.;
  f += eps; // to avoid calculations on
  l -= eps; // points of pointed squares.

  const double anAngleTol2 = theAngleTol * theAngleTol;

  gp_Vec aDer1, aDer2;
  double aSqLen1, aSqLen2;
  gp_Dir aCrvDir1[2], aCrvDir2[2];
  double aCrvLen1[2], aCrvLen2[2];

  GeomAbs_Shape aCont = (isElementary ? GeomAbs_CN : GeomAbs_C2);
  GeomAbs_Shape aCurCont;
  double        u;
  for (int i = 0; i <= 20 && aCont > GeomAbs_C0; i++)
  {
    // First suppose that this is sameParameter
    u = f + (l - f) * i / 20;

    // Check conditions for G1 and C1 continuity:
    // * calculate a derivative in tangent plane of each surface
    //   orthogonal to curve's tangent vector
    // * continuity is C1 if the vectors are equal
    // * continuity is G1 if the vectors are just parallel
    aCurCont = GeomAbs_C0;

    aSP1.Calculate(u);
    aSP2.Calculate(u);

    aDer1                = aSP1.Derivative();
    aSqLen1              = aDer1.SquareMagnitude();
    aDer2                = aSP2.Derivative();
    aSqLen2              = aDer2.SquareMagnitude();
    bool isSmoothSuspect = (aDer1.CrossSquareMagnitude(aDer2) <= anAngleTol2 * aSqLen1 * aSqLen2);
    if (isSmoothSuspect)
    {
      gp_Dir aNormal1 = aSP1.Normal();
      if (theFace1.Orientation() == TopAbs_REVERSED)
        aNormal1.Reverse();
      gp_Dir aNormal2 = aSP2.Normal();
      if (theFace2.Orientation() == TopAbs_REVERSED)
        aNormal2.Reverse();

      if (aNormal1 * aNormal2 < 0.)
        return GeomAbs_C0;
    }

    if (!isSmoothSuspect)
    {
      // Refine by projection
      if (aHC2.IsNull())
      {
        // adaptor for pcurve on the second surface
        aHC2 = new BRepAdaptor_Curve(anEdgeInFace2, theFace2);
        ext.Initialize(*aHC2, f, l, Precision::PConfusion());
      }
      ext.Perform(aSP1.Value(), u);
      if (ext.IsDone() && ext.IsMin())
      {
        const Extrema_POnCurv& poc = ext.Point();
        aSP2.Calculate(poc.Parameter());
        aDer2   = aSP2.Derivative();
        aSqLen2 = aDer2.SquareMagnitude();
      }
      isSmoothSuspect = (aDer1.CrossSquareMagnitude(aDer2) <= anAngleTol2 * aSqLen1 * aSqLen2);
    }
    if (isSmoothSuspect)
    {
      aCurCont = GeomAbs_G1;
      if (std::abs(std::sqrt(aSqLen1) - std::sqrt(aSqLen2)) < Precision::Confusion()
          && aDer1.Dot(aDer2) > Precision::SquareConfusion()) // <= check vectors are codirectional
        aCurCont = GeomAbs_C1;
    }
    else
      return GeomAbs_C0;

    if (aCont < GeomAbs_G2)
      continue; // no need further processing, because maximal continuity is less than G2

    // Check conditions for G2 and C2 continuity:
    // * calculate principal curvatures on each surface
    // * continuity is C2 if directions of principal curvatures are equal on different surfaces
    // * continuity is G2 if directions of principal curvatures are just parallel
    //   and values of curvatures are the same
    aSP1.Curvature(aCrvDir1[0], aCrvLen1[0], aCrvDir1[1], aCrvLen1[1]);
    aSP2.Curvature(aCrvDir2[0], aCrvLen2[0], aCrvDir2[1], aCrvLen2[1]);
    for (int aStep = 0; aStep <= 1; ++aStep)
    {
      if (aCrvDir1[0].XYZ().CrossSquareMagnitude(aCrvDir2[aStep].XYZ())
            <= Precision::SquareConfusion()
          && std::abs(aCrvLen1[0] - aCrvLen2[aStep]) < Precision::Confusion()
          && aCrvDir1[1].XYZ().CrossSquareMagnitude(aCrvDir2[1 - aStep].XYZ())
               <= Precision::SquareConfusion()
          && std::abs(aCrvLen1[1] - aCrvLen2[1 - aStep]) < Precision::Confusion())
      {
        if (aCurCont == GeomAbs_C1 && aCrvDir1[0].Dot(aCrvDir2[aStep]) > Precision::Confusion()
            && aCrvDir1[1].Dot(aCrvDir2[1 - aStep]) > Precision::Confusion())
          aCurCont = GeomAbs_C2;
        else
          aCurCont = GeomAbs_G2;
        break;
      }
    }

    if (aCurCont < aCont)
      aCont = aCurCont;
  }

  // according to the list of supported elementary surfaces,
  // if the continuity is C2, than it is totally CN
  if (isElementary && aCont == GeomAbs_C2)
    aCont = GeomAbs_CN;
  return aCont;
}

//=======================================================================
// function : EncodeRegularity
// purpose  : Code the regularities on all edges of the shape, boundary of
//            two faces that do not have it.
//            Takes into account that compound may consists of same solid
//            placed with different transformations
//=======================================================================
static void EncodeRegularity(
  const TopoDS_Shape&                                           theShape,
  const double                                                  theTolAng,
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&       theMap,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& theEdgesToEncode =
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>())
{
  TopoDS_Shape    aShape = theShape;
  TopLoc_Location aNullLoc;
  aShape.Location(aNullLoc); // nullify location
  if (!theMap.Add(aShape))
    return; // do not need to process shape twice

  if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_COMPSOLID)
  {
    for (TopoDS_Iterator it(aShape); it.More(); it.Next())
      EncodeRegularity(it.Value(), theTolAng, theMap, theEdgesToEncode);
    return;
  }

  try
  {
    OCC_CATCH_SIGNALS

    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      M;
    TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, M);
    NCollection_List<TopoDS_Shape>::Iterator It;
    TopExp_Explorer                          Ex;
    TopoDS_Face                              F1, F2;
    for (int i = 1; i <= M.Extent(); i++)
    {
      TopoDS_Edge E = TopoDS::Edge(M.FindKey(i));
      if (!theEdgesToEncode.IsEmpty())
      {
        // process only the edges from the list to update their regularity
        TopoDS_Shape aPureEdge = E.Located(aNullLoc);
        aPureEdge.Orientation(TopAbs_FORWARD);
        if (!theEdgesToEncode.Contains(aPureEdge))
          continue;
      }

      bool found = false;
      F1.Nullify();
      for (It.Initialize(M.FindFromIndex(i)); It.More() && !found; It.Next())
      {
        if (F1.IsNull())
        {
          F1 = TopoDS::Face(It.Value());
        }
        else
        {
          const TopoDS_Face& aTmpF2 = TopoDS::Face(It.Value());
          if (!F1.IsSame(aTmpF2))
          {
            found = true;
            F2    = aTmpF2;
          }
        }
      }
      if (!found && !F1.IsNull())
      { // is it a sewing edge?
        TopAbs_Orientation orE = E.Orientation();
        TopoDS_Edge        curE;
        for (Ex.Init(F1, TopAbs_EDGE); Ex.More() && !found; Ex.Next())
        {
          curE = TopoDS::Edge(Ex.Current());
          if (E.IsSame(curE) && orE != curE.Orientation())
          {
            found = true;
            F2    = F1;
          }
        }
      }
      if (found)
        BRepLib::EncodeRegularity(E, F1, F2, theTolAng);
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: Exception in BRepLib::EncodeRegularity(): ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
  }
}

//=======================================================================
// function : EncodeRegularity
// purpose  : code the regularities on all edges of the shape, boundary of
//            two faces that do not have it.
//=======================================================================

void BRepLib::EncodeRegularity(const TopoDS_Shape& S, const double TolAng)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  ::EncodeRegularity(S, TolAng, aMap);
}

//=======================================================================
// function : EncodeRegularity
// purpose  : code the regularities on all edges in the list that do not
//            have it, and which are boundary of two faces on the shape.
//=======================================================================

void BRepLib::EncodeRegularity(const TopoDS_Shape&                   S,
                               const NCollection_List<TopoDS_Shape>& LE,
                               const double                          TolAng)
{
  // Collect edges without location and orientation
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aPureEdges;
  TopLoc_Location                                        aNullLoc;
  NCollection_List<TopoDS_Shape>::Iterator               anEdgeIt(LE);
  for (; anEdgeIt.More(); anEdgeIt.Next())
  {
    TopoDS_Shape anEdge = anEdgeIt.Value();
    anEdge.Location(aNullLoc);
    anEdge.Orientation(TopAbs_FORWARD);
    aPureEdges.Add(anEdge);
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
  ::EncodeRegularity(S, TolAng, aMap, aPureEdges);
}

//=======================================================================
// function : EncodeRegularity
// purpose  : code the regularity between 2 faces connected by edge
//=======================================================================

void BRepLib::EncodeRegularity(TopoDS_Edge&       E,
                               const TopoDS_Face& F1,
                               const TopoDS_Face& F2,
                               const double       TolAng)
{
  BRep_Builder B;
  if (BRep_Tool::Continuity(E, F1, F2) <= GeomAbs_C0)
  {
    try
    {
      GeomAbs_Shape aCont = ContinuityOfFaces(E, F1, F2, TolAng);
      B.Continuity(E, F1, F2, aCont);
    }
    catch (Standard_Failure const&)
    {
#ifdef OCCT_DEBUG
      std::cout << "Failure: Exception in BRepLib::EncodeRegularity" << std::endl;
#endif
    }
  }
}

//=======================================================================
// function : EnsureNormalConsistency
// purpose  : Corrects the normals in Poly_Triangulation of faces.
//              Returns TRUE if any correction is done.
//=======================================================================
bool BRepLib::EnsureNormalConsistency(const TopoDS_Shape& theShape,
                                      const double        theAngTol,
                                      const bool          theForceComputeNormals)
{
  const double aThresDot = cos(theAngTol);

  bool aRetVal = false, isNormalsFound = false;

  // compute normals if they are absent
  TopExp_Explorer anExpFace(theShape, TopAbs_FACE);
  for (; anExpFace.More(); anExpFace.Next())
  {
    const TopoDS_Face&              aFace = TopoDS::Face(anExpFace.Current());
    const occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace);
    if (aSurf.IsNull())
      continue;
    TopLoc_Location                        aLoc;
    const occ::handle<Poly_Triangulation>& aPT = BRep_Tool::Triangulation(aFace, aLoc);
    if (aPT.IsNull())
      continue;
    if (!theForceComputeNormals && aPT->HasNormals())
    {
      isNormalsFound = true;
      continue;
    }

    aPT->AddNormals();
    GeomLProp_SLProps aSLP(aSurf, 2, Precision::Confusion());
    for (int i = 1; i <= aPT->NbNodes(); i++)
    {
      const gp_Pnt2d aP2d = aPT->UVNode(i);
      aSLP.SetParameters(aP2d.X(), aP2d.Y());

      if (!aSLP.IsNormalDefined())
      {
#ifdef OCCT_DEBUG
        std::cout << "BRepLib::EnsureNormalConsistency(): Cannot find normal!" << std::endl;
#endif
        aPT->SetNormal(i, NCollection_Vec3<float>(0.0f));
      }
      else
      {
        gp_Dir aNorm = aSLP.Normal();
        if (aFace.Orientation() == TopAbs_REVERSED)
        {
          aNorm.Reverse();
        }
        aPT->SetNormal(i, aNorm);
      }
    }

    aRetVal        = true;
    isNormalsFound = true;
  }

  if (!isNormalsFound)
  {
    return aRetVal;
  }

  // loop by edges
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapEF;
  TopExp::MapShapesAndAncestors(theShape, TopAbs_EDGE, TopAbs_FACE, aMapEF);
  for (int anInd = 1; anInd <= aMapEF.Extent(); anInd++)
  {
    const TopoDS_Edge&                    anEdg     = TopoDS::Edge(aMapEF.FindKey(anInd));
    const NCollection_List<TopoDS_Shape>& anEdgList = aMapEF.FindFromIndex(anInd);
    if (anEdgList.Extent() != 2)
      continue;
    NCollection_List<TopoDS_Shape>::Iterator anItF(anEdgList);
    const TopoDS_Face                        aFace1 = TopoDS::Face(anItF.Value());
    anItF.Next();
    const TopoDS_Face                      aFace2 = TopoDS::Face(anItF.Value());
    TopLoc_Location                        aLoc1, aLoc2;
    const occ::handle<Poly_Triangulation>& aPT1 = BRep_Tool::Triangulation(aFace1, aLoc1);
    const occ::handle<Poly_Triangulation>& aPT2 = BRep_Tool::Triangulation(aFace2, aLoc2);

    if (aPT1.IsNull() || aPT2.IsNull())
      continue;

    if (!aPT1->HasNormals() || !aPT2->HasNormals())
      continue;

    const occ::handle<Poly_PolygonOnTriangulation>& aPTEF1 =
      BRep_Tool::PolygonOnTriangulation(anEdg, aPT1, aLoc1);
    const occ::handle<Poly_PolygonOnTriangulation>& aPTEF2 =
      BRep_Tool::PolygonOnTriangulation(anEdg, aPT2, aLoc2);

    if (aPTEF1->Nodes().Lower() != aPTEF2->Nodes().Lower()
        || aPTEF1->Nodes().Upper() != aPTEF2->Nodes().Upper())
      continue;

    for (int anEdgNode = aPTEF1->Nodes().Lower(); anEdgNode <= aPTEF1->Nodes().Upper(); anEdgNode++)
    {
      // Number of node
      const int aFNodF1 = aPTEF1->Nodes().Value(anEdgNode);
      const int aFNodF2 = aPTEF2->Nodes().Value(anEdgNode);

      NCollection_Vec3<float> aNorm1f, aNorm2f;
      aPT1->Normal(aFNodF1, aNorm1f);
      aPT2->Normal(aFNodF2, aNorm2f);
      const gp_XYZ aNorm1(aNorm1f.x(), aNorm1f.y(), aNorm1f.z());
      const gp_XYZ aNorm2(aNorm2f.x(), aNorm2f.y(), aNorm2f.z());
      const double aDot = aNorm1 * aNorm2;
      if (aDot > aThresDot)
      {
        gp_XYZ aNewNorm = (aNorm1 + aNorm2).Normalized();
        aPT1->SetNormal(aFNodF1, aNewNorm);
        aPT2->SetNormal(aFNodF2, aNewNorm);
        aRetVal = true;
      }
    }
  }

  return aRetVal;
}

//=================================================================================================

namespace
{
//! Tool to estimate deflection of the given UV point
//! with regard to its representation in 3D space.
struct EvalDeflection
{
  BRepAdaptor_Surface Surface;

  //! Initializes tool with the given face.
  EvalDeflection(const TopoDS_Face& theFace)
      : Surface(theFace)
  {
  }

  //! Evaluates deflection of the given 2d point from its 3d representation.
  double Eval(const gp_Pnt2d& thePoint2d, const gp_Pnt& thePoint3d)
  {
    gp_Pnt aPnt;
    Surface.D0(thePoint2d.X(), thePoint2d.Y(), aPnt);
    return (thePoint3d.XYZ() - aPnt.XYZ()).SquareModulus();
  }
};

//! Represents link of triangulation.
struct Link
{
  int Node[2];

  //! Constructor
  Link(const int theNode1, const int theNode2)
  {
    Node[0] = theNode1;
    Node[1] = theNode2;
  }

  //! Returns true if this link has the same nodes as the other.
  bool IsEqual(const Link& theOther) const
  {
    return ((Node[0] == theOther.Node[0] && Node[1] == theOther.Node[1])
            || (Node[0] == theOther.Node[1] && Node[1] == theOther.Node[0]));
  }

  //! Alias for IsEqual.
  bool operator==(const Link& theOther) const { return IsEqual(theOther); }
};
} // namespace

namespace std
{
template <>
struct hash<Link>
{
  size_t operator()(const Link& theLink) const noexcept
  {
    int aCombination[2]{theLink.Node[0], theLink.Node[1]};
    if (aCombination[0] > aCombination[1])
    {
      std::swap(aCombination[0], aCombination[1]);
    }
    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }
};
} // namespace std

void BRepLib::UpdateDeflection(const TopoDS_Shape& theShape)
{
  TopExp_Explorer anExpFace(theShape, TopAbs_FACE);
  for (; anExpFace.More(); anExpFace.Next())
  {
    const TopoDS_Face&              aFace = TopoDS::Face(anExpFace.Current());
    const occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(aFace);
    if (aSurf.IsNull())
    {
      continue;
    }

    TopLoc_Location                        aLoc;
    const occ::handle<Poly_Triangulation>& aPT = BRep_Tool::Triangulation(aFace, aLoc);
    if (aPT.IsNull() || !aPT->HasUVNodes())
    {
      continue;
    }

    // Collect all nodes of degenerative edges and skip elements
    // build upon them due to huge distortions introduced by passage
    // from UV space to 3D.
    NCollection_Map<int> aDegNodes;
    TopExp_Explorer      anExpEdge(aFace, TopAbs_EDGE);
    for (; anExpEdge.More(); anExpEdge.Next())
    {
      const TopoDS_Edge& aEdge = TopoDS::Edge(anExpEdge.Current());
      if (BRep_Tool::Degenerated(aEdge))
      {
        const occ::handle<Poly_PolygonOnTriangulation>& aPolygon =
          BRep_Tool::PolygonOnTriangulation(aEdge, aPT, aLoc);
        if (aPolygon.IsNull())
        {
          continue;
        }

        for (int aNodeIt = aPolygon->Nodes().Lower(); aNodeIt <= aPolygon->Nodes().Upper();
             ++aNodeIt)
        {
          aDegNodes.Add(aPolygon->Node(aNodeIt));
        }
      }
    }

    EvalDeflection        aTool(aFace);
    NCollection_Map<Link> aLinks;
    double                aSqDeflection = 0.;
    const gp_Trsf&        aTrsf         = aLoc.Transformation();
    for (int aTriIt = 1; aTriIt <= aPT->NbTriangles(); ++aTriIt)
    {
      const Poly_Triangle& aTriangle = aPT->Triangle(aTriIt);

      int aNode[3];
      aTriangle.Get(aNode[0], aNode[1], aNode[2]);
      if (aDegNodes.Contains(aNode[0]) || aDegNodes.Contains(aNode[1])
          || aDegNodes.Contains(aNode[2]))
      {
        continue;
      }

      const gp_Pnt aP3d[3] = {aPT->Node(aNode[0]).Transformed(aTrsf),
                              aPT->Node(aNode[1]).Transformed(aTrsf),
                              aPT->Node(aNode[2]).Transformed(aTrsf)};

      const gp_Pnt2d aP2d[3] = {aPT->UVNode(aNode[0]),
                                aPT->UVNode(aNode[1]),
                                aPT->UVNode(aNode[2])};

      // Check midpoint of triangle.
      const gp_Pnt   aMid3d_t = (aP3d[0].XYZ() + aP3d[1].XYZ() + aP3d[2].XYZ()) / 3.;
      const gp_Pnt2d aMid2d_t = (aP2d[0].XY() + aP2d[1].XY() + aP2d[2].XY()) / 3.;

      aSqDeflection = std::max(aSqDeflection, aTool.Eval(aMid2d_t, aMid3d_t));

      for (int i = 0; i < 3; ++i)
      {
        const int  j = (i + 1) % 3;
        const Link aLink(aNode[i], aNode[j]);
        if (!aLinks.Add(aLink))
        {
          // Do not estimate boundary links due to high distortions at the edge.
          const gp_Pnt& aP3d1 = aP3d[i];
          const gp_Pnt& aP3d2 = aP3d[j];

          const gp_Pnt2d& aP2d1 = aP2d[i];
          const gp_Pnt2d& aP2d2 = aP2d[j];

          const gp_Pnt   aMid3d_l = (aP3d1.XYZ() + aP3d2.XYZ()) / 2.;
          const gp_Pnt2d aMid2d_l = (aP2d1.XY() + aP2d2.XY()) / 2.;

          aSqDeflection = std::max(aSqDeflection, aTool.Eval(aMid2d_l, aMid3d_l));
        }
      }
    }

    aPT->Deflection(std::sqrt(aSqDeflection));
  }
}

//=================================================================================================

void BRepLib::SortFaces(const TopoDS_Shape& Sh, NCollection_List<TopoDS_Shape>& LF)
{
  LF.Clear();
  NCollection_List<TopoDS_Shape> LTri, LPlan, LCyl, LCon, LSphere, LTor, LOther;
  TopExp_Explorer                exp(Sh, TopAbs_FACE);
  TopLoc_Location                l;
  occ::handle<Geom_Surface>      S;

  for (; exp.More(); exp.Next())
  {
    const TopoDS_Face& F = TopoDS::Face(exp.Current());
    S                    = BRep_Tool::Surface(F, l);
    if (!S.IsNull())
    {
      if (S->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      {
        S = occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface();
      }
      GeomAdaptor_Surface AS(S);
      switch (AS.GetType())
      {
        case GeomAbs_Plane: {
          LPlan.Append(F);
          break;
        }
        case GeomAbs_Cylinder: {
          LCyl.Append(F);
          break;
        }
        case GeomAbs_Cone: {
          LCon.Append(F);
          break;
        }
        case GeomAbs_Sphere: {
          LSphere.Append(F);
          break;
        }
        case GeomAbs_Torus: {
          LTor.Append(F);
          break;
        }
        default:
          LOther.Append(F);
      }
    }
    else
      LTri.Append(F);
  }
  LF.Append(LPlan);
  LF.Append(LCyl);
  LF.Append(LCon);
  LF.Append(LSphere);
  LF.Append(LTor);
  LF.Append(LOther);
  LF.Append(LTri);
}

//=================================================================================================

void BRepLib::ReverseSortFaces(const TopoDS_Shape& Sh, NCollection_List<TopoDS_Shape>& LF)
{
  LF.Clear();
  // Use the allocator of the result LF for intermediate results
  NCollection_List<TopoDS_Shape> LTri(LF.Allocator()), LPlan(LF.Allocator()), LCyl(LF.Allocator()),
    LCon(LF.Allocator()), LSphere(LF.Allocator()), LTor(LF.Allocator()), LOther(LF.Allocator());
  TopExp_Explorer exp(Sh, TopAbs_FACE);
  TopLoc_Location l;

  for (; exp.More(); exp.Next())
  {
    const TopoDS_Face&               F = TopoDS::Face(exp.Current());
    const occ::handle<Geom_Surface>& S = BRep_Tool::Surface(F, l);
    if (!S.IsNull())
    {
      GeomAdaptor_Surface AS(S);
      switch (AS.GetType())
      {
        case GeomAbs_Plane: {
          LPlan.Append(F);
          break;
        }
        case GeomAbs_Cylinder: {
          LCyl.Append(F);
          break;
        }
        case GeomAbs_Cone: {
          LCon.Append(F);
          break;
        }
        case GeomAbs_Sphere: {
          LSphere.Append(F);
          break;
        }
        case GeomAbs_Torus: {
          LTor.Append(F);
          break;
        }
        default:
          LOther.Append(F);
      }
    }
    else
      LTri.Append(F);
  }
  LF.Append(LTri);
  LF.Append(LOther);
  LF.Append(LTor);
  LF.Append(LSphere);
  LF.Append(LCon);
  LF.Append(LCyl);
  LF.Append(LPlan);
}

//=================================================================================================

void BRepLib::BoundingVertex(const NCollection_List<TopoDS_Shape>& theLV,
                             gp_Pnt&                               theNewCenter,
                             double&                               theNewTol)
{
  int aNb;
  //
  aNb = theLV.Extent();
  if (aNb < 2)
  {
    return;
  }
  //
  else if (aNb == 2)
  {
    int           m, n;
    double        aR[2], dR, aD, aEps;
    TopoDS_Vertex aV[2];
    gp_Pnt        aP[2];
    //
    aEps = RealEpsilon();
    for (m = 0; m < aNb; ++m)
    {
      aV[m] = (!m) ? *((TopoDS_Vertex*)(&theLV.First())) : *((TopoDS_Vertex*)(&theLV.Last()));
      aP[m] = BRep_Tool::Pnt(aV[m]);
      aR[m] = BRep_Tool::Tolerance(aV[m]);
    }
    //
    m = 0; // max R
    n = 1; // min R
    if (aR[0] < aR[1])
    {
      m = 1;
      n = 0;
    }
    //
    dR = aR[m] - aR[n]; // dR >= 0.
    gp_Vec aVD(aP[m], aP[n]);
    aD = aVD.Magnitude();
    //
    if (aD <= dR || aD < aEps)
    {
      theNewCenter = aP[m];
      theNewTol    = aR[m];
    }
    else
    {
      double aRr;
      gp_XYZ aXYZr;
      gp_Pnt aPr;
      //
      aRr   = 0.5 * (aR[m] + aR[n] + aD);
      aXYZr = 0.5 * (aP[m].XYZ() + aP[n].XYZ() - aVD.XYZ() * (dR / aD));
      aPr.SetXYZ(aXYZr);
      //
      theNewCenter = aPr;
      theNewTol    = aRr;
      // aBB.MakeVertex (aVnew, aPr, aRr);
    }
    return;
  } // else if (aNb==2) {
  //
  else
  { // if (aNb>2)
    // compute the point
    //
    // issue 0027540 - sum of doubles may depend on the order
    // of addition, thus sort the coordinates for stable result
    int                                      i;
    NCollection_Array1<gp_Pnt>               aPoints(0, aNb - 1);
    NCollection_List<TopoDS_Shape>::Iterator aIt(theLV);
    for (i = 0; aIt.More(); aIt.Next(), ++i)
    {
      const TopoDS_Vertex& aVi = *((TopoDS_Vertex*)(&aIt.Value()));
      gp_Pnt               aPi = BRep_Tool::Pnt(aVi);
      aPoints(i)               = aPi;
    }
    //
    std::sort(aPoints.begin(), aPoints.end(), BRepLib_ComparePoints());
    //
    gp_XYZ aXYZ(0., 0., 0.);
    for (i = 0; i < aNb; ++i)
    {
      aXYZ += aPoints(i).XYZ();
    }
    aXYZ.Divide((double)aNb);
    //
    gp_Pnt aP(aXYZ);
    //
    // compute the tolerance for the new vertex
    double aTi, aDi, aDmax;
    //
    aDmax = -1.;
    aIt.Initialize(theLV);
    for (; aIt.More(); aIt.Next())
    {
      TopoDS_Vertex& aVi = *((TopoDS_Vertex*)(&aIt.Value()));
      gp_Pnt         aPi = BRep_Tool::Pnt(aVi);
      aTi                = BRep_Tool::Tolerance(aVi);
      aDi                = aP.SquareDistance(aPi);
      aDi                = sqrt(aDi);
      aDi                = aDi + aTi;
      if (aDi > aDmax)
      {
        aDmax = aDi;
      }
    }
    //
    theNewCenter = aP;
    theNewTol    = aDmax;
  }
}

//=================================================================================================

void BRepLib::ExtendFace(const TopoDS_Face& theF,
                         const double       theExtVal,
                         const bool         theExtUMin,
                         const bool         theExtUMax,
                         const bool         theExtVMin,
                         const bool         theExtVMax,
                         TopoDS_Face&       theFExtended)
{
  // Get face bounds
  BRepAdaptor_Surface aBAS(theF);
  double              aFUMin = aBAS.FirstUParameter(), aFUMax = aBAS.LastUParameter(),
         aFVMin = aBAS.FirstVParameter(), aFVMax = aBAS.LastVParameter();
  const double aTol = BRep_Tool::Tolerance(theF);

  // Surface to build the face
  occ::handle<Geom_Surface> aS;

  const GeomAbs_SurfaceType aType = aBAS.GetType();
  // treat analytical surfaces first
  if (aType == GeomAbs_Plane || aType == GeomAbs_Sphere || aType == GeomAbs_Cylinder
      || aType == GeomAbs_Torus || aType == GeomAbs_Cone)
  {
    // Get basis transformed basis surface
    occ::handle<Geom_Surface> aSurf =
      occ::down_cast<Geom_Surface>(aBAS.Surface().Surface()->Transformed(aBAS.Trsf()));

    // Get bounds of the basis surface
    double aSUMin, aSUMax, aSVMin, aSVMax;
    aSurf->Bounds(aSUMin, aSUMax, aSVMin, aSVMax);

    bool   isUPeriodic = aBAS.IsUPeriodic();
    double anUPeriod   = isUPeriodic ? aBAS.UPeriod() : 0.0;
    if (isUPeriodic)
    {
      // Adjust face bounds to first period
      double aDelta = aFUMax - aFUMin;
      aFUMin = std::max(aSUMin, aFUMin + anUPeriod * std::ceil((aSUMin - aFUMin) / anUPeriod));
      aFUMax = aFUMin + aDelta;
    }

    bool   isVPeriodic = aBAS.IsVPeriodic();
    double aVPeriod    = isVPeriodic ? aBAS.VPeriod() : 0.0;
    if (isVPeriodic)
    {
      // Adjust face bounds to first period
      double aDelta = aFVMax - aFVMin;
      aFVMin        = std::max(aSVMin, aFVMin + aVPeriod * std::ceil((aSVMin - aFVMin) / aVPeriod));
      aFVMax        = aFVMin + aDelta;
    }

    // Enlarge the face
    double anURes = 0., aVRes = 0.;
    if (theExtUMin || theExtUMax)
      anURes = aBAS.UResolution(theExtVal);
    if (theExtVMin || theExtVMax)
      aVRes = aBAS.VResolution(theExtVal);

    if (theExtUMin)
      aFUMin = std::max(aSUMin, aFUMin - anURes);
    if (theExtUMax)
      aFUMax = std::min(isUPeriodic ? aFUMin + anUPeriod : aSUMax, aFUMax + anURes);
    if (theExtVMin)
      aFVMin = std::max(aSVMin, aFVMin - aVRes);
    if (theExtVMax)
      aFVMax = std::min(isVPeriodic ? aFVMin + aVPeriod : aSVMax, aFVMax + aVRes);

    // Check if the periodic surface should become closed.
    // In this case, use the basis surface with basis bounds.
    constexpr double anEps = Precision::PConfusion();
    if (isUPeriodic && std::abs(aFUMax - aFUMin - anUPeriod) < anEps)
    {
      aFUMin = aSUMin;
      aFUMax = aSUMax;
    }
    if (isVPeriodic && std::abs(aFVMax - aFVMin - aVPeriod) < anEps)
    {
      aFVMin = aSVMin;
      aFVMax = aSVMax;
    }

    aS = aSurf;
  }
  else
  {
    // General case

    occ::handle<Geom_BoundedSurface> aSB =
      occ::down_cast<Geom_BoundedSurface>(BRep_Tool::Surface(theF));
    if (aSB.IsNull())
    {
      theFExtended = theF;
      return;
    }

    // Get surfaces bounds
    double aSUMin, aSUMax, aSVMin, aSVMax;
    aSB->Bounds(aSUMin, aSUMax, aSVMin, aSVMax);

    bool isUClosed = aSB->IsUClosed();
    bool isVClosed = aSB->IsVClosed();

    // Check if the extension in necessary directions is done
    bool isExtUMin = false, isExtUMax = false, isExtVMin = false, isExtVMax = false;

    // UMin
    if (theExtUMin && !isUClosed && !Precision::IsInfinite(aSUMin))
    {
      GeomLib::ExtendSurfByLength(aSB, theExtVal, 1, true, false);
      isExtUMin = true;
    }
    // UMax
    if (theExtUMax && !isUClosed && !Precision::IsInfinite(aSUMax))
    {
      GeomLib::ExtendSurfByLength(aSB, theExtVal, 1, true, true);
      isExtUMax = true;
    }
    // VMin
    if (theExtVMin && !isVClosed && !Precision::IsInfinite(aSVMax))
    {
      GeomLib::ExtendSurfByLength(aSB, theExtVal, 1, false, false);
      isExtVMin = true;
    }
    // VMax
    if (theExtVMax && !isVClosed && !Precision::IsInfinite(aSVMax))
    {
      GeomLib::ExtendSurfByLength(aSB, theExtVal, 1, false, true);
      isExtVMax = true;
    }

    aS = aSB;

    // Get new bounds
    aS->Bounds(aSUMin, aSUMax, aSVMin, aSVMax);
    if (isExtUMin)
      aFUMin = aSUMin;
    if (isExtUMax)
      aFUMax = aSUMax;
    if (isExtVMin)
      aFVMin = aSVMin;
    if (isExtVMax)
      aFVMax = aSVMax;
  }

  BRepLib_MakeFace aMF(aS, aFUMin, aFUMax, aFVMin, aFVMax, aTol);
  theFExtended = *(TopoDS_Face*)&aMF.Shape();
  if (theF.Orientation() == TopAbs_REVERSED)
    theFExtended.Reverse();
}
