// Created on: 1996-07-12
// Created by: Stagiaire Mary FABIEN
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepTools_NurbsConvertModification.hxx>

#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <BRepTools.hxx>
#include <ElSLib.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Extrema_LocateExtPC.hxx>
#include <Extrema_LocateExtPC2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Pnt.hxx>
#include <ProjLib_ComputeApprox.hxx>
#include <ProjLib_ComputeApproxOnPolarSurface.hxx>
#include <Standard_Type.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Builder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepTools_NurbsConvertModification,BRepTools_CopyModification)

//
namespace
{
  static void GeomLib_ChangeUBounds(Handle(Geom_BSplineSurface)& aSurface,
                                    const Standard_Real newU1,
                                    const Standard_Real newU2)
  {
    TColStd_Array1OfReal  knots(1, aSurface->NbUKnots());
    aSurface->UKnots(knots);
    BSplCLib::Reparametrize(newU1, newU2, knots);
    aSurface->SetUKnots(knots);
  }

  static void GeomLib_ChangeVBounds(Handle(Geom_BSplineSurface)& aSurface,
                                    const Standard_Real newV1,
                                    const Standard_Real newV2)
  {
    TColStd_Array1OfReal  knots(1, aSurface->NbVKnots());
    aSurface->VKnots(knots);
    BSplCLib::Reparametrize(newV1, newV2, knots);
    aSurface->SetVKnots(knots);
  }

  // find 3D curve from theEdge in theMap, and return the transformed curve or NULL
  static Handle(Geom_Curve) newCurve(const TColStd_IndexedDataMapOfTransientTransient& theMap,
                                     const TopoDS_Edge& theEdge,
                                     Standard_Real& theFirst,
                                     Standard_Real& theLast)
  {
    Handle(Geom_Curve) aNewCurve;

    TopLoc_Location aLoc;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, aLoc, theFirst, theLast);
    if (!aCurve.IsNull() && theMap.Contains(aCurve))
    {
      aNewCurve = Handle(Geom_Curve)::DownCast(theMap.FindFromKey(aCurve));
      aNewCurve = Handle(Geom_Curve)::DownCast(aNewCurve->Transformed(aLoc.Transformation()));
    }
    return aNewCurve;
  }

  // find 2D curve from theEdge on theFace in theMap, and return the transformed curve or NULL
  static Handle(Geom2d_Curve) newCurve(const TColStd_IndexedDataMapOfTransientTransient& theMap,
                                       const TopoDS_Edge& theEdge,
                                       const TopoDS_Face& theFace,
                                       Standard_Real& theFirst,
                                       Standard_Real& theLast)
  {
    Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(theEdge, theFace, theFirst, theLast);
    return (!aC2d.IsNull() && theMap.Contains(aC2d)) ? Handle(Geom2d_Curve)::DownCast(theMap.FindFromKey(aC2d))
                                                     : Handle(Geom2d_Curve)();
  }

  // find surface from theFace in theMap, and return the transformed surface or NULL
  static Handle(Geom_Surface) newSurface(const TColStd_IndexedDataMapOfTransientTransient& theMap,
                                         const TopoDS_Face& theFace)
  {
    Handle(Geom_Surface) aNewSurf;

    TopLoc_Location aLoc;
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace, aLoc);
    if (!aSurf.IsNull() && theMap.Contains(aSurf))
    {
      aNewSurf = Handle(Geom_Surface)::DownCast(theMap.FindFromKey(aSurf));
      aNewSurf = Handle(Geom_Surface)::DownCast(aNewSurf->Transformed(aLoc.Transformation()));
    }
    return aNewSurf;
  }

  static Standard_Boolean newParameter(const gp_Pnt& thePoint,
                                       const Handle(Geom_Curve)& theCurve,
                                       const Standard_Real theFirst,
                                       const Standard_Real theLast,
                                       const Standard_Real theTol,
                                       Standard_Real& theParam)
  {
    GeomAdaptor_Curve anAdaptor(theCurve);
    Extrema_LocateExtPC proj(thePoint, anAdaptor, theParam, theFirst, theLast, Precision::PConfusion());
    if (proj.IsDone())
    {
      Standard_Real aDist2Min = proj.SquareDistance();
      if (aDist2Min < theTol * theTol)
      {
        theParam = proj.Point().Parameter();
        return Standard_True;
      }
    }
    return Standard_False;
  }

  static Standard_Boolean newParameter(const gp_Pnt2d& theUV,
                                       const Handle(Geom2d_Curve)& theCurve2d,
                                       const Standard_Real theFirst,
                                       const Standard_Real theLast,
                                       const Standard_Real theTol,
                                       Standard_Real& theParam)
  {
    Geom2dAdaptor_Curve anAdaptor(theCurve2d);
    Extrema_LocateExtPC2d aProj(theUV, anAdaptor, theParam, Precision::PConfusion());
    if (aProj.IsDone())
    {
      Standard_Real aDist2Min = aProj.SquareDistance();
      if (aDist2Min < theTol * theTol)
      {
        theParam = aProj.Point().Parameter();
        return Standard_True;
      }
    }
    else
    {
      // Try to use general extrema to find the parameter, because Extrema_LocateExtPC2d
      // sometimes could not find a solution if the parameter's first approach is several
      // spans away from the expected solution (test bugs/modalg_7/bug28722).
      Extrema_ExtPC2d anExt(theUV, anAdaptor, theFirst, theLast);
      if (anExt.IsDone())
      {
        Standard_Integer aMinInd = 0;
        Standard_Real aMinSqDist = Precision::Infinite();
        for (Standard_Integer anIndex = 1; anIndex <= anExt.NbExt(); ++anIndex)
          if (anExt.SquareDistance(anIndex) < aMinSqDist)
          {
            aMinSqDist = anExt.SquareDistance(anIndex);
            aMinInd = anIndex;
          }
        if (aMinSqDist < theTol * theTol)
        {
          theParam = anExt.Point(aMinInd).Parameter();
          return Standard_True;
        }
      }
    }
    return Standard_False;
  }

  static Standard_Boolean newUV(const gp_Pnt& thePoint,
                                const Handle(Geom_Surface)& theSurf,
                                const Standard_Real theTol,
                                gp_Pnt2d& theUV)
  {
    GeomAdaptor_Surface anAdaptor(theSurf);
    Extrema_GenLocateExtPS aProj(anAdaptor);
    aProj.Perform(thePoint, theUV.X(), theUV.Y());
    if (aProj.IsDone())
    {
      Standard_Real aDist2Min = aProj.SquareDistance();
      if (aDist2Min < theTol * theTol)
      {
        gp_XY& aUV = theUV.ChangeCoord();
        aProj.Point().Parameter(aUV.ChangeCoord(1), aUV.ChangeCoord(2));
        return Standard_True;
      }
    }
    return Standard_False;
  }
}

//=======================================================================
//function : BRepTools_NurbsConvertModification
//purpose  : 
//=======================================================================

BRepTools_NurbsConvertModification::BRepTools_NurbsConvertModification()
{
}



//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewSurface
      (const TopoDS_Face& F,
       Handle(Geom_Surface)& S,
       TopLoc_Location& L,
       Standard_Real& Tol,
       Standard_Boolean& RevWires,
       Standard_Boolean& RevFace)
{
  Standard_Real U1, U2, curvU1, curvU2, surfU1, surfU2, UTol;
  Standard_Real V1, V2, curvV1, curvV2, surfV1, surfV2, VTol;
  RevWires = Standard_False;
  RevFace = Standard_False;
  Handle(Geom_Surface) SS = BRep_Tool::Surface(F,L);
  if (SS.IsNull())
  {
    //processing the case when there is no geometry
    return Standard_False;
  }

  Handle(Standard_Type) TheTypeSS = SS->DynamicType();
  if ((TheTypeSS == STANDARD_TYPE(Geom_BSplineSurface)) ||
      (TheTypeSS == STANDARD_TYPE(Geom_BezierSurface))) {
    return Standard_False;
  }
  S = SS;
  BRepTools::UVBounds(F,curvU1,curvU2,curvV1,curvV2);
  Tol = BRep_Tool::Tolerance(F);
  Standard_Real TolPar = 0.1*Tol;
  Standard_Boolean IsUp = S->IsUPeriodic(), IsVp = S->IsVPeriodic();
  //OCC466(apo)->
  U1 = curvU1;  U2 = curvU2;  
  V1 = curvV1;  V2 = curvV2;
  S->Bounds(surfU1,surfU2,surfV1,surfV2);

  if (Abs(U1 - surfU1) <= TolPar)
    U1 = surfU1;
  if (Abs(U2 - surfU2) <= TolPar)
    U2 = surfU2;
  if (Abs(V1 - surfV1) <= TolPar)
    V1 = surfV1;
  if (Abs(V2 - surfV2) <= TolPar)
    V2 = surfV2;
  
  if(!IsUp){
    U1 = Max(surfU1,curvU1);
    U2 = Min(surfU2,curvU2);
  }
  if(!IsVp){
    V1 = Max(surfV1,curvV1);
    V2 = Min(surfV2,curvV2);
  }
  //<-OCC466(apo)

  if (IsUp)
  {
    Standard_Real Up = S->UPeriod();
    if (U2 - U1 > Up)
      U2 = U1 + Up;
  }
  if (IsVp)
  {
    Standard_Real Vp = S->VPeriod();
    if (V2 - V1 > Vp)
      V2 = V1 + Vp;
  }
  
  /*
  if(IsUp && IsVp) {
    Standard_Real dU = Abs(U2 - U1), dV = Abs(V2 - V1);
    Standard_Real Up = S->UPeriod(), Vp = S->VPeriod();
    if(Abs(dU - Up) <= TolPar && U2 <= Up) {
      if(Abs(dV - Vp) <= TolPar && V2 <= Vp) { }
      else {
    SS = new Geom_RectangularTrimmedSurface(S, V1+1e-9, V2-1e-9, Standard_False);
      }
    }
    else {
      if(Abs(dV - Vp) <= TolPar && V2 <= Vp) 
    SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, Standard_True);
      else
    SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, V1+1e-9, V2-1e-9);
    }
  }

  if(IsUp && !IsVp) {
    Standard_Real dU = Abs(U2 - U1);
    Standard_Real Up = S->UPeriod();
    if(Abs(dU - Up) <= TolPar && U2 <= Up) 
      SS = new Geom_RectangularTrimmedSurface(S, V1+1e-9, V2-1e-9, Standard_False);
    else 
      SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, V1+1e-9, V2-1e-9);
  }
    
  if(!IsUp && IsVp) {
    Standard_Real dV = Abs(V2 - V1);
    Standard_Real Vp = S->VPeriod();
    if(Abs(dV - Vp) <= TolPar && V2 <= Vp) 
      SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, Standard_True);
    else
      SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, V1+1e-9, V2-1e-9);
  }

  if(!IsUp && !IsVp) {
    SS = new Geom_RectangularTrimmedSurface(S, U1+1e-9, U2-1e-9, V1+1e-9, V2-1e-9);
  }
  */

  if (Abs(surfU1-U1) > Tol || Abs(surfU2-U2) > Tol ||
      Abs(surfV1-V1) > Tol || Abs(surfV2-V2) > Tol)
    S = new Geom_RectangularTrimmedSurface(S, U1, U2, V1, V2);
  S->Bounds(surfU1,surfU2,surfV1,surfV2); 

  S = GeomConvert::SurfaceToBSplineSurface(S);
  Handle(Geom_BSplineSurface) BS = Handle(Geom_BSplineSurface)::DownCast(S) ;
  BS->Resolution(Tol, UTol, VTol) ;
  
  // 
  // on recadre les bornes de S  sinon les anciennes PCurves sont aux fraises
  //

  if (Abs(curvU1-surfU1) > UTol && !BS->IsUPeriodic()) {
    GeomLib_ChangeUBounds(BS, U1,U2) ;
  }
  if (Abs(curvV1-surfV1) > VTol && !BS->IsVPeriodic()) {
    GeomLib_ChangeVBounds(BS, V1, V2) ;
  }

  if (!myMap.Contains(SS)) {
    myMap.Add(SS, S);
  }
  return Standard_True;
}

static Standard_Boolean IsConvert(const TopoDS_Edge& E)
{
  Standard_Boolean isConvert = Standard_False;
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
  // iterate on pcurves
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  for ( ; itcr.More() && !isConvert; itcr.Next() ) {
    Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
    if ( GC.IsNull() || ! GC->IsCurveOnSurface() ) continue;
    Handle(Geom_Surface) aSurface = GC->Surface();
    Handle(Geom2d_Curve) aCurve2d = GC->PCurve();
    isConvert =((!aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)) && 
                !aSurface->IsKind(STANDARD_TYPE(Geom_BezierSurface))) ||
               (!aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) &&
               !aCurve2d->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))));
     
  }
  return isConvert;
  
}

//=======================================================================
//function : NewTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewTriangulation(const TopoDS_Face&          theFace,
                                                                      Handle(Poly_Triangulation)& theTri)
{
  if (!BRepTools_CopyModification::NewTriangulation(theFace, theTri))
  {
    return Standard_False;
  }

  // convert UV nodes of the mesh
  if (theTri->HasUVNodes())
  {
    TopLoc_Location aLoc;
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace, aLoc);
    Handle(Geom_Surface) aNewSurf = newSurface(myMap, theFace);
    if (!aSurf.IsNull() && !aNewSurf.IsNull())
    {
      Standard_Real aTol = BRep_Tool::Tolerance(theFace);
      for (Standard_Integer anInd = 1; anInd <= theTri->NbNodes(); ++anInd)
      {
        gp_Pnt2d aUV = theTri->UVNode(anInd);
        gp_Pnt aPoint = aSurf->Value(aUV.X(), aUV.Y());
        if (newUV(aPoint, aNewSurf, aTol, aUV))
          theTri->SetUVNode(anInd, aUV);
      }
    }
  }

  return Standard_True;
}

//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewCurve
(const TopoDS_Edge& E, 
 Handle(Geom_Curve)& C,
 TopLoc_Location& L,
 Standard_Real& Tol)
{

  Tol = BRep_Tool::Tolerance(E);
  if(BRep_Tool::Degenerated(E)) {
    C.Nullify();
    L.Identity();
    return Standard_True;
  }
  Standard_Real f, l;

  Handle(Geom_Curve) Caux = BRep_Tool::Curve(E, L, f, l);

  if ( Caux.IsNull()) {
    L.Identity();    
    return Standard_False;
  }
  Handle(Standard_Type) TheType = Caux->DynamicType();
  if ((TheType == STANDARD_TYPE(Geom_BSplineCurve)) ||
      (TheType == STANDARD_TYPE(Geom_BezierCurve))) {
    if(IsConvert(E)) {
      C = Handle(Geom_Curve)::DownCast(Caux->Copy());
      return Standard_True;
    }
    return Standard_False;
  } 

  C = Caux; 

  Standard_Real TolPar = Tol *.1;

  if(C->IsPeriodic()) {
    Standard_Real p = C->Period();
    Standard_Real d = Abs(l - f);
    if(Abs(d - p) <= TolPar && l <= p) {}
    else
      C = new Geom_TrimmedCurve(C, f, l);
  } 
  else 
    C = new Geom_TrimmedCurve(C, f, l);

//modif WOK++ portage hp (fbi du 14/03/97)
//  gp_Trsf trsf(L);
//  gp_Trsf trsf = L.Transformation();

//  C = GeomConvert::CurveToBSplineCurve(C,Convert_QuasiAngular);

  C = GeomConvert::CurveToBSplineCurve(C);

  Standard_Real fnew = C->FirstParameter(), lnew = C->LastParameter(), UTol;

  Handle(Geom_BSplineCurve) BC = Handle(Geom_BSplineCurve)::DownCast(C) ;

  if(!BC->IsPeriodic()) {
    BC->Resolution(Tol, UTol) ;
    if(Abs(f - fnew) > UTol || Abs(l - lnew) > UTol) {
      TColStd_Array1OfReal  knots(1,BC->NbKnots()) ;
      BC->Knots(knots) ;
      BSplCLib::Reparametrize(f, l, knots) ;
      BC->SetKnots(knots) ;
    }
  }

  if(!myMap.Contains(Caux)) {
    myMap.Add(Caux,C);
  }
  return Standard_True ;
}

//=======================================================================
//function : NewPolygon
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewPolygon(const TopoDS_Edge&      theEdge,
                                                                Handle(Poly_Polygon3D)& thePoly)
{
  if (!BRepTools_CopyModification::NewPolygon(theEdge, thePoly))
  {
    return Standard_False;
  }

  // update parameters of polygon
  if (thePoly->HasParameters())
  {
    Standard_Real aTol = BRep_Tool::Tolerance(theEdge);
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(theEdge, aFirst, aLast);
    Handle(Geom_Curve) aNewCurve = newCurve(myMap, theEdge, aFirst, aLast);
    if (!aCurve.IsNull() && !aNewCurve.IsNull()) // skip processing degenerated edges
    {
      TColStd_Array1OfReal& aParams = thePoly->ChangeParameters();
      for (Standard_Integer anInd = aParams.Lower(); anInd <= aParams.Upper(); ++anInd)
      {
        Standard_Real& aParam = aParams(anInd);
        gp_Pnt aPoint = aCurve->Value(aParam);
        newParameter(aPoint, aNewCurve, aFirst, aLast, aTol, aParam);
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewPoint
    (const TopoDS_Vertex&, gp_Pnt&, Standard_Real& )
{
  return Standard_False;
}


//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewCurve2d
  (const TopoDS_Edge& E, 
   const TopoDS_Face& F, 
   const TopoDS_Edge& newE, 
   const TopoDS_Face& newF, 
   Handle(Geom2d_Curve)& Curve2d,
   Standard_Real& Tol)
{

  Tol = BRep_Tool::Tolerance(E);
  Standard_Real f2d,l2d;
  Handle(Geom2d_Curve) aBaseC2d = BRep_Tool::CurveOnSurface(E,F,f2d,l2d);
  Standard_Real f3d,l3d;
  TopLoc_Location Loc;
  Handle(Geom_Curve) C3d = BRep_Tool::Curve(E, Loc, f3d,l3d);
  Standard_Boolean isConvert2d = ((!C3d.IsNull() && !C3d->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) &&
    !C3d->IsKind(STANDARD_TYPE(Geom_BezierCurve))) ||
    IsConvert(E));

  Handle(Geom2d_Curve) C2d = aBaseC2d;
  if(BRep_Tool::Degenerated(E)) {
    //Curve2d = C2d;
    if(!C2d->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      Handle(Geom2d_TrimmedCurve) aTrimC = new Geom2d_TrimmedCurve(C2d,f2d,l2d);
      C2d = aTrimC;
    }
    Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
    myMap.Add(aBaseC2d, Curve2d);
    return Standard_True;
  }
  if(!BRepTools::IsReallyClosed(E,F)) {
    Handle(Standard_Type) TheTypeC2d = C2d->DynamicType();

    if(TheTypeC2d == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
      Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(C2d);
      C2d = TC->BasisCurve();
    }

    Standard_Real fc = C2d->FirstParameter(), lc = C2d->LastParameter();

    if(!C2d->IsPeriodic()) {
      if(fc - f2d > Precision::PConfusion()) f2d = fc;
      if(l2d - lc > Precision::PConfusion()) l2d = lc;
    }

    C2d = new Geom2d_TrimmedCurve(C2d, f2d, l2d);

    Geom2dAdaptor_Curve   G2dAC(C2d, f2d, l2d);
    Handle(Geom2dAdaptor_Curve) G2dAHC = new Geom2dAdaptor_Curve(G2dAC);
    
    if(!newE.IsNull()) {
      C3d = BRep_Tool::Curve(newE, f3d, l3d);
    }
    if (C3d.IsNull()) {
      C3d = BRep_Tool::Curve(E,f3d,l3d);
    }
    if (C3d.IsNull())
      return Standard_False;
    GeomAdaptor_Curve   G3dAC(C3d, f3d, l3d);
    Handle(GeomAdaptor_Curve) G3dAHC = new GeomAdaptor_Curve(G3dAC);
    
    Standard_Real Uinf, Usup, Vinf, Vsup, u = 0, v = 0;
    Handle(Geom_Surface) S = BRep_Tool::Surface(F);
    Handle(Standard_Type) myT = S->DynamicType();
    if(myT != STANDARD_TYPE(Geom_Plane)) {
      if(newF.IsNull()) {
        Handle(Standard_Type) st = C2d->DynamicType();
        if ((st == STANDARD_TYPE(Geom2d_BSplineCurve)) ||
            (st == STANDARD_TYPE(Geom2d_BezierCurve))) {
          if(isConvert2d) {
            Curve2d = Handle(Geom2d_Curve)::DownCast(C2d->Copy());
            Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
            if(newTol > Tol)
            {
              Tol = newTol;
              myUpdatedEdges.Append(newE);
            }
            myMap.Add(aBaseC2d, Curve2d);
            return Standard_True;
          }
          return Standard_False;
        }
      }
      else {
        Handle(Geom_Surface) aNewS = BRep_Tool::Surface(newF);
        if (!aNewS.IsNull())
          S = aNewS;
      }
      S->Bounds(Uinf, Usup, Vinf, Vsup);
      //Uinf -= 1e-9; Usup += 1e-9; Vinf -= 1e-9; Vsup += 1e-9;
      u = (Usup - Uinf)*0.1;
      v = (Vsup - Vinf)*0.1;
      if(S->IsUPeriodic()) {
        Standard_Real uperiod = S->UPeriod();
        if(uperiod < (Usup+2*u-Uinf)) {
          if(uperiod <= (Usup-Uinf))  {
            u = 0;
          } 
          else {
            u = (uperiod-(Usup-Uinf))*0.5;
          }
        }
      }
      if(S->IsVPeriodic()) {
        Standard_Real vperiod = S->VPeriod();
        if(vperiod < (Vsup+2*v-Vinf)) {
          if(vperiod <= (Vsup-Vinf)) {
            v = 0;
          }
          else {
            v = (vperiod-(Vsup-Vinf))*0.5;
          }
        }
      }
    }
    else {
      S = BRep_Tool::Surface(F);// Si S est un plan, pas de changement de parametrisation
      GeomAdaptor_Surface GAS(S);
      Handle(GeomAdaptor_Surface) GAHS = new GeomAdaptor_Surface(GAS);
      ProjLib_ComputeApprox ProjOnCurve(G3dAHC,GAHS,Tol);
      if(ProjOnCurve.BSpline().IsNull()) {
        Curve2d = Geom2dConvert::CurveToBSplineCurve(ProjOnCurve.Bezier());
        Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if(newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        myMap.Add(aBaseC2d, Curve2d);
        return Standard_True;
      }
      Curve2d = ProjOnCurve.BSpline();
      Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if(newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return Standard_True;
    }

    //
    GeomAdaptor_Surface GAS(S,Uinf-u,Usup+u,Vinf-v,Vsup+v);

    Handle(GeomAdaptor_Surface) GAHS = new GeomAdaptor_Surface(GAS);

    ProjLib_ComputeApproxOnPolarSurface ProjOnCurve(G2dAHC,G3dAHC,GAHS,Tol);

    if(ProjOnCurve.IsDone()) {
      Curve2d = ProjOnCurve.BSpline();
      if (S->IsUPeriodic() || S->IsVPeriodic())
      {
        //Surface is periodic, checking curve2d domain 
        //Old domain
        Standard_Real aMinDist = Precision::Infinite();
        if (S->IsUPeriodic())
        {
          aMinDist = Min(0.5 * S->UPeriod(), aMinDist);
        }
        if (S->IsVPeriodic())
        {
          aMinDist = Min(0.5 * S->VPeriod(), aMinDist);
        }
        aMinDist *= aMinDist;
        //Old domain
        gp_Pnt2d aPf = C2d->Value(f2d);
        //New domain
        gp_Pnt2d aNewPf = Curve2d->Value(f2d);
        gp_Vec2d aT(aNewPf, aPf);
        if (aT.SquareMagnitude() > aMinDist)
        {
          Curve2d = Handle(Geom2d_Curve)::DownCast(Curve2d->Translated(aT));
        }
      }
      Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if(newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return Standard_True;
    }
    else {
      Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
      Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
      if(newTol > Tol)
      {
        Tol = newTol;
        myUpdatedEdges.Append(newE);
      }
      myMap.Add(aBaseC2d, Curve2d);
      return Standard_True;
    }
  }
  else  {
    TopTools_ListIteratorOfListOfShape itled;
    TColStd_ListIteratorOfListOfTransient itlcu;
    
    for (itled.Initialize(myled), itlcu.Initialize(mylcu);
      itled.More(); // itlcu.More()
      itled.Next(),itlcu.Next()) {
      if (itled.Value().IsSame(E)) {
        // deja traitee
        break;
      }
    }
    if (!itled.More()) { // on stocke l`edge et la curve2d
      Handle(Geom2d_Curve) C2dBis;
      Standard_Real f2dBis,l2dBis;
      C2d = new Geom2d_TrimmedCurve(C2d, f2d, l2d);
      Geom2dAdaptor_Curve G2dAC(C2d, f2d, l2d);
      Handle(Geom2dAdaptor_Curve) G2dAHC = new Geom2dAdaptor_Curve(G2dAC);
      TopoDS_Edge ERevers = E;
      ERevers.Reverse();
      C2dBis = BRep_Tool::CurveOnSurface(ERevers,F,f2dBis,l2dBis);
      Handle(Standard_Type) TheTypeC2dBis = C2dBis->DynamicType();
      C2dBis = new Geom2d_TrimmedCurve(C2dBis,f2dBis, l2dBis);
      Geom2dAdaptor_Curve   G2dACBis(C2dBis, f2dBis, l2dBis); 
      Handle(Geom2dAdaptor_Curve) G2dAHCBis = new Geom2dAdaptor_Curve(G2dACBis);
      
      if(C3d.IsNull()) {
         if(isConvert2d) {
           Curve2d = Handle(Geom2d_Curve)::DownCast(C2d->Copy());

           Handle(Geom_Surface) S;
           if(newF.IsNull())
             S = BRep_Tool::Surface(F);
           else
             S = BRep_Tool::Surface(newF);
           //
           Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
           if(newTol > Tol)
           {
             Tol = newTol;
             myUpdatedEdges.Append(newE);
           }
           myMap.Add(aBaseC2d, Curve2d);
           return Standard_True;
         }
        return Standard_False;
      }
      if(!newE.IsNull()) {
        C3d = BRep_Tool::Curve(newE, f3d,l3d);
      }
      GeomAdaptor_Curve G3dAC(C3d, f3d, l3d);
      Handle(GeomAdaptor_Curve) G3dAHC = new GeomAdaptor_Curve(G3dAC);
      
      Handle(Geom_Surface) S = BRep_Tool::Surface(F);
      Handle(Standard_Type) myT = S->DynamicType();
      if(newF.IsNull()) {
        mylcu.Append(C2dBis);
        Handle(Standard_Type) st = C2d->DynamicType();
        if ((st == STANDARD_TYPE(Geom2d_BSplineCurve)) ||
            (st == STANDARD_TYPE(Geom2d_BezierCurve))) {
          if(isConvert2d) {
            Curve2d = Handle(Geom2d_Curve)::DownCast(C2d->Copy());
            Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
            if(newTol > Tol)
            {
              Tol = newTol;
              myUpdatedEdges.Append(newE);
            }
            myMap.Add(aBaseC2d, Curve2d);
            return Standard_True;
          }
          return Standard_False;
        }
      }
      else {
        S = BRep_Tool::Surface(newF);// S est une BSplineSurface : pas besoin de la trimmed
      }
      Standard_Real Uinf, Usup, Vinf, Vsup, u = 0, v = 0;
      S->Bounds(Uinf, Usup, Vinf, Vsup);
      //Uinf -= 1e-9; Usup += 1e-9; Vinf -= 1e-9; Vsup += 1e-9;
      u = (Usup - Uinf)*0.1;
      v = (Vsup - Vinf)*0.1;
      if(S->IsUPeriodic()) {
        Standard_Real uperiod = S->UPeriod();
        if(uperiod < (Usup+2*u-Uinf)) {
          if(uperiod <= (Usup-Uinf))
            u = 0;
          else
            u = (uperiod-(Usup-Uinf))*0.5;
        }
      }
      if(S->IsVPeriodic()) {
        Standard_Real vperiod = S->VPeriod();
        if(vperiod < (Vsup+2*v-Vinf)) {
          if(vperiod <= (Vsup-Vinf))
            v = 0;
          else
            v = (vperiod-(Vsup-Vinf))*0.5;
        }
      }
      GeomAdaptor_Surface GAS(S, Uinf-u,Usup+u,Vinf-v,Vsup+v);
      Handle(GeomAdaptor_Surface) GAHS = new GeomAdaptor_Surface(GAS);
      myled.Append(E);

      ProjLib_ComputeApproxOnPolarSurface 
      ProjOnCurve(G2dAHC,G2dAHCBis,G3dAHC,GAHS,Tol);

      if(ProjOnCurve.IsDone()) {
        Curve2d = ProjOnCurve.BSpline();
        mylcu.Append(ProjOnCurve.Curve2d());
        Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if(newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        myMap.Add(aBaseC2d, Curve2d);
        return Standard_True;
      }
      else {
        Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
        Standard_Real newTol = BRepTools::EvalAndUpdateTol(newE, C3d, Curve2d, S, f3d, l3d);
        if(newTol > Tol)
        {
          Tol = newTol;
          myUpdatedEdges.Append(newE);
        }
        mylcu.Append(C2dBis);
        myMap.Add(aBaseC2d, Curve2d);
        return Standard_True;
      }
    }
    else { // on est au 2ieme tour 
      C2d = Handle(Geom2d_Curve)::DownCast(itlcu.Value());
      Handle(Standard_Type) st = C2d->DynamicType();
      if (!(st == STANDARD_TYPE(Geom2d_BSplineCurve)) &&
      !(st == STANDARD_TYPE(Geom2d_BezierCurve))) {
        return Standard_False;
      }
      Curve2d = Geom2dConvert::CurveToBSplineCurve(C2d);
      myMap.Add(aBaseC2d, Curve2d);
      return Standard_True;
    }
  }
}

//=======================================================================
//function : NewPolygonOnTriangulation
//purpose  : 
//=======================================================================
Standard_Boolean BRepTools_NurbsConvertModification::NewPolygonOnTriangulation(
  const TopoDS_Edge&                   theEdge,
  const TopoDS_Face&                   theFace,
  Handle(Poly_PolygonOnTriangulation)& thePoly)
{
  if (!BRepTools_CopyModification::NewPolygonOnTriangulation(theEdge, theFace, thePoly))
  {
    return Standard_False;
  }

  // update parameters of 2D polygon
  if (thePoly->HasParameters())
  {
    Standard_Real aTol = Max(BRep_Tool::Tolerance(theEdge), thePoly->Deflection());
    TopLoc_Location aLoc;
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace, aLoc);
    Handle(Geom_Surface) aNewSurf = newSurface(myMap, theFace);
    Standard_Real aFirst, aLast;
    Handle(Geom2d_Curve) aC2d = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);
    Handle(Geom2d_Curve) aNewC2d = newCurve(myMap, theEdge, theFace, aFirst, aLast);
    if (!aSurf.IsNull() && !aC2d.IsNull() && !aNewSurf.IsNull() && !aNewC2d.IsNull())
    {
      // compute 2D tolerance
      GeomAdaptor_Surface aSurfAdapt(aSurf);
      Standard_Real aTol2D = Max(aSurfAdapt.UResolution(aTol), aSurfAdapt.VResolution(aTol));

      for (Standard_Integer anInd = 1; anInd <= thePoly->NbNodes(); ++anInd)
      {
        Standard_Real aParam = thePoly->Parameter(anInd);
        gp_Pnt2d aUV = aC2d->Value(aParam);
        gp_Pnt aPoint = aSurf->Value(aUV.X(), aUV.Y());
        if (newUV(aPoint, aNewSurf, aTol, aUV) &&
            newParameter(aUV, aNewC2d, aFirst, aLast, aTol2D, aParam))
        {
          thePoly->SetParameter(anInd, aParam);
        }
      }
    }
  }
  return Standard_True;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean BRepTools_NurbsConvertModification::NewParameter
   (const TopoDS_Vertex& V, 
    const TopoDS_Edge& E, 
    Standard_Real& P, 
    Standard_Real& Tol)
{
  Tol =  BRep_Tool::Tolerance(V);
  if(BRep_Tool::Degenerated(E))
    return Standard_False;

  gp_Pnt pnt = BRep_Tool::Pnt(V);
  P = BRep_Tool::Parameter(V,E);
  Standard_Real aFirst, aLast;
  Handle(Geom_Curve) aNewCurve = newCurve(myMap, E, aFirst, aLast);
  return !aNewCurve.IsNull() && newParameter(pnt, aNewCurve, aFirst, aLast, Tol, P);
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape BRepTools_NurbsConvertModification::Continuity
  (const TopoDS_Edge& E,
   const TopoDS_Face& F1,
   const TopoDS_Face& F2,
   const TopoDS_Edge&,
   const TopoDS_Face&,
   const TopoDS_Face&)
{
  return BRep_Tool::Continuity(E,F1,F2);
}


//=======================================================================
//function : GetUpdatedEdges
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& 
        BRepTools_NurbsConvertModification::GetUpdatedEdges() const
{
  return myUpdatedEdges;
}
