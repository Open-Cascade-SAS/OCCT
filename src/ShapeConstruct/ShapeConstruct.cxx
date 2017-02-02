// Created on: 1999-06-17
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


#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeConstruct_Curve.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_HSequenceOfShape.hxx>

//=======================================================================
//function : ConvertCurveToBSpline
//purpose  : 
//=======================================================================
Handle(Geom_BSplineCurve) ShapeConstruct::ConvertCurveToBSpline(const Handle(Geom_Curve)& C3D,
								const Standard_Real First,
								const Standard_Real Last,
								const Standard_Real Tol3d,
								const GeomAbs_Shape Continuity,
								const Standard_Integer MaxSegments,
								const Standard_Integer MaxDegree)
{
  Standard_Integer MaxDeg = MaxDegree;
  Handle(Geom_BSplineCurve) aBSpline;
  if(C3D->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) 
    aBSpline = Handle(Geom_BSplineCurve)::DownCast(C3D);
  else {
    if(C3D->IsKind(STANDARD_TYPE(Geom_Conic))) 
      MaxDeg = Min(MaxDeg,6);
 
    Handle(Geom_Curve) tcurve = new Geom_TrimmedCurve(C3D,First,Last); //protection agains parabols ets
    try {
      OCC_CATCH_SIGNALS
      GeomConvert_ApproxCurve approx (tcurve, Tol3d, Continuity, MaxSegments, MaxDeg);
      if ( approx.HasResult() )
	aBSpline = approx.Curve();
      else
	aBSpline = GeomConvert::CurveToBSplineCurve(C3D,Convert_QuasiAngular);
    }
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	    cout << "Warning: GeomConvert_ApproxSurface Exception:  ";
	    anException.Print(cout); cout << endl;
#endif 
	    (void)anException;
	    aBSpline = GeomConvert::CurveToBSplineCurve(C3D,Convert_QuasiAngular);    
	  }
  }
  return aBSpline;
}

//=======================================================================
//function : ConvertCurveToBSpline
//purpose  : 
//=======================================================================

Handle(Geom2d_BSplineCurve) ShapeConstruct::ConvertCurveToBSpline(const Handle(Geom2d_Curve)& C2D,
								  const Standard_Real First,
								  const Standard_Real Last,
								  const Standard_Real Tol2d,
								  const GeomAbs_Shape Continuity,
								  const Standard_Integer MaxSegments,
								  const Standard_Integer MaxDegree)
{
  Handle(Geom2d_BSplineCurve) aBSpline2d;
  if(C2D->IsKind(STANDARD_TYPE(Geom2d_Conic))) {
    Handle(Geom2d_Curve) tcurve = new Geom2d_TrimmedCurve(C2D,First,Last); //protection agains parabols ets
    Geom2dConvert_ApproxCurve approx (tcurve, Tol2d, Continuity, MaxSegments, MaxDegree);
    if ( approx.HasResult() )
      aBSpline2d = approx.Curve();
    else 
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve,Convert_QuasiAngular);
  } 
  else if(!C2D->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
    aBSpline2d = Geom2dConvert::CurveToBSplineCurve(C2D,Convert_QuasiAngular);
  }
  else
    aBSpline2d = Handle(Geom2d_BSplineCurve)::DownCast(C2D);
  
  return aBSpline2d;
}

//=======================================================================
//function : ConvertSurfaceToBSpline
//purpose  : 
//=======================================================================

// Note: this method has the same purpose as GeomConvert::SurfaceToBSplineSurface(),
// but treats more correctly offset surfaces and takes parameters such as UV limits 
// and degree as arguments instead of deducing them from the surface.
// Eventually it may be merged back to GeomConvert.

Handle(Geom_BSplineSurface) ShapeConstruct::ConvertSurfaceToBSpline(const Handle(Geom_Surface)& surf,
								    const Standard_Real UF,
								    const Standard_Real UL,
								    const Standard_Real VF,
								    const Standard_Real VL,
								    const Standard_Real Tol3d,
								    const GeomAbs_Shape Continuity,
								    const Standard_Integer MaxSegments,
								    const Standard_Integer MaxDegree)
{
  Handle(Geom_BSplineSurface) res;
  
  Handle(Geom_Surface) S = surf;
  if(surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) RTS = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
    S = RTS->BasisSurface();
  }
  
  // use GeomConvert for direct conversion of analytic surfaces
  if (S->IsKind(STANDARD_TYPE(Geom_ElementarySurface))) 
  {
    Handle(Geom_RectangularTrimmedSurface) aRTS = 
      new Geom_RectangularTrimmedSurface(S,UF,UL,VF,VL);
    return GeomConvert::SurfaceToBSplineSurface(aRTS);
  }

  if(S->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) extr = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(S);
    Handle(Geom_Curve) basis = extr->BasisCurve();
    //gp_Dir direction = extr->Direction(); // direction not used (skl)
    
    GeomAbs_Shape cnt = (Continuity > GeomAbs_C2 ? GeomAbs_C2: Continuity);
    Handle(Geom_BSplineCurve) bspl = ConvertCurveToBSpline(basis, UF, UL, Tol3d, cnt, MaxSegments, MaxDegree);
    
    gp_Trsf shiftF,shiftL;
    shiftF.SetTranslation(extr->Value(UF,0),extr->Value(UF,VF));
    shiftL.SetTranslation(extr->Value(UF,0),extr->Value(UF,VL));
        
    Standard_Integer nbPoles = bspl->NbPoles();
    TColgp_Array1OfPnt poles(1,nbPoles);
    TColStd_Array1OfReal weights(1,nbPoles);
    Standard_Integer nbKnots = bspl->NbKnots();
    TColStd_Array1OfReal knots(1,nbKnots);
    TColStd_Array1OfInteger mults(1,nbKnots);
    
    bspl->Poles(poles);
    bspl->Knots(knots);
    bspl->Multiplicities(mults);
    bspl->Weights(weights);
    
    TColgp_Array2OfPnt resPoles(1,nbPoles,1,2);
    TColStd_Array2OfReal resWeigth(1,nbPoles,1,2);
    for(Standard_Integer j = 1; j <= nbPoles; j++) {
      resPoles(j,1) = poles(j).Transformed(shiftF);
      resPoles(j,2) = poles(j).Transformed(shiftL);
      resWeigth(j,1)= weights(j);
      resWeigth(j,2)= weights(j);
    }
   
    TColStd_Array1OfReal vknots(1,2);
    TColStd_Array1OfInteger vmults(1,2);
    vknots(1) = VF;
    vknots(2) = VL;
    vmults(1) = vmults(2) = 2;
    
    Handle(Geom_BSplineSurface) bspline = new Geom_BSplineSurface(resPoles, resWeigth, knots, vknots, mults, vmults,
								  bspl->Degree(),1,bspl->IsPeriodic(),Standard_False);
    return bspline;
  }
  
  if(S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
    Handle(Geom_SurfaceOfRevolution) revol = Handle(Geom_SurfaceOfRevolution)::DownCast(S);
    Handle(Geom_Curve) basis = revol->BasisCurve();
    if(basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
      GeomAbs_Shape cnt = basis->Continuity();
      cnt = (cnt > GeomAbs_C2 ? GeomAbs_C2: cnt);
      Handle(Geom_BSplineCurve) bspl = ConvertCurveToBSpline(basis, VF, VL, Tol3d, cnt, MaxSegments, MaxDegree);
      gp_Ax1 axis = revol->Axis();
      Handle(Geom_SurfaceOfRevolution) newRevol = new Geom_SurfaceOfRevolution(bspl,axis);
#ifdef OCCT_DEBUG
      cout <<" Revolution on offset converted" << endl;
#endif
      S = newRevol;
    }
  }
    
  Handle(Geom_Surface) aSurface = new Geom_RectangularTrimmedSurface(S,UF,UL,VF,VL);
  Handle(Geom_BSplineSurface) errSpl;
  for(Standard_Integer cnt = (Continuity > GeomAbs_C3 ? GeomAbs_C3: Continuity); cnt >= 0 ; ) {
    try {
      OCC_CATCH_SIGNALS
      GeomAbs_Shape aCont = (GeomAbs_Shape) cnt;
      GeomConvert_ApproxSurface anApprox(aSurface,Tol3d/2,aCont,aCont,MaxDegree,MaxDegree,MaxSegments,0);
      Standard_Boolean Done = anApprox.IsDone();
      if (anApprox.MaxError() <= Tol3d && Done) {
	
#ifdef OCCT_DEBUG
	Standard_Integer nbOfSpan = (anApprox.Surface()->NbUKnots()-1)*(anApprox.Surface()->NbVKnots()-1);
	cout << "\terror = " << anApprox.MaxError() << "\tspans = " << nbOfSpan << endl;
	cout << " Surface is aproximated with continuity " << (GeomAbs_Shape)cnt <<endl;
#endif
	S = anApprox.Surface();
	Handle(Geom_BSplineSurface) Bsc = Handle(Geom_BSplineSurface)::DownCast(S);
	return Bsc;
      }
      else {
	if(anApprox.HasResult()) 
	  errSpl = anApprox.Surface();
#ifdef OCCT_DEBUG
	cout << "\terror = " << anApprox.MaxError() <<endl;
#endif
	break;
      }
    }
	
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: GeomConvert_ApproxSurface Exception: try to decrease continuity ";
      anException.Print(cout); cout << endl;
#endif
      (void)anException;
      if(cnt > 0) cnt--;
      continue;
    }
  }
  
  return errSpl;
}
//=======================================================================
//function : JoinPCurves
//purpose  : 
//=======================================================================

Standard_Boolean ShapeConstruct::JoinPCurves(const Handle(TopTools_HSequenceOfShape)& edges,
                                             const TopoDS_Face& theFace,
                                             TopoDS_Edge& theEdge)
{
  ShapeAnalysis_Edge sae;
  BRep_Builder B;
  
  try {
    OCC_CATCH_SIGNALS
    // check if current face is plane.
    Handle(Geom_Surface) aGeomSurf = BRep_Tool::Surface(theFace);
    while (aGeomSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      
      aGeomSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aGeomSurf)->BasisSurface();
    }
    if (aGeomSurf->IsKind(STANDARD_TYPE(Geom_Plane)))
      return Standard_True;
    
    
    Standard_Boolean IsEdgeSeam = Standard_False;
    Handle(Geom2d_Curve) aCrvRes1, aCrvRes2;
    TopAbs_Orientation resOrient;
    Standard_Real newf = 0.,newl = 0.;
    // iterates on edges
    Standard_Integer i = 1;
    for(; i <= edges->Length(); i++) {
      TopoDS_Edge Edge = TopoDS::Edge(edges->Value(i));
      if (i == 1)
        IsEdgeSeam = sae.IsSeam(Edge,theFace);
      else if (IsEdgeSeam && (!sae.IsSeam(Edge,theFace)))
        break; // different cases
      else if (!IsEdgeSeam && (sae.IsSeam(Edge,theFace)))
        break; // different cases
      
      resOrient = TopAbs_FORWARD;
      Handle(Geom2d_Curve) c2d,c2d2;
      Standard_Real first, last,first2, last2;
      if(!sae.PCurve ( Edge, theFace, c2d, first, last, Standard_False ))
        break;
      
      if(IsEdgeSeam) {
        TopoDS_Edge tmpE1 =TopoDS::Edge(Edge.Reversed());
        sae.PCurve ( tmpE1, theFace, c2d2, first2, last2, Standard_False );
      }
      
      if( i == 1) {
        aCrvRes1 = c2d;
        if(IsEdgeSeam) {
          aCrvRes2 = c2d2;
        }
        newf = first;
        newl = last;
        resOrient = Edge.Orientation();
      }
      else {
        Handle(Geom2d_Curve) newCrv;
        Standard_Boolean isRev1,isRev2;
        if(!JoinCurves(aCrvRes1,c2d,resOrient,Edge.Orientation(),newf,newl,first, last,newCrv,isRev1,isRev2)) 
          break;
        
        if(IsEdgeSeam) {
          Handle(Geom2d_Curve) newCrv2;
          Standard_Real newf2 = newf,newl2 = newl;
          
          if(!JoinCurves(aCrvRes2,c2d2,resOrient,Edge.Orientation(),newf2,newl2,first2, last2,newCrv2,isRev1,isRev2)) 
          break;
          aCrvRes2 = newCrv2;
        }
        aCrvRes1 = newCrv;
        Standard_Real fp2d = newCrv->FirstParameter();
        Standard_Real lp2d = newCrv->LastParameter();
        newl += (last - first);
        if(fp2d > newf) newf = fp2d;
        if(lp2d < newl) newl = lp2d;
        
      }
    }
    if (IsEdgeSeam)
      B.UpdateEdge(theEdge,aCrvRes1,aCrvRes2,theFace,0);
    else
      B.UpdateEdge(theEdge,aCrvRes1,theFace,0);
     B.Range(theEdge,theFace,newf,newl);
    B.SameRange(theEdge,Standard_False);
    B.SameParameter(theEdge,Standard_False);
    return (i <= edges->Length());
  }
  catch ( Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
    cout<<"Error: ShapeConstruct::JoinPCurves Exception in GeomConvert_CompCurveToBSplineCurve: ";
    anException.Print(cout); cout<<endl;
#endif
    (void)anException;
  }
  return Standard_False;
}

//=======================================================================
//function : JoinCurves
//purpose  : 
//=======================================================================

template<class HCurve> 
static inline HCurve GetCurveCopy(const HCurve& curve, 
                                  Standard_Real& first, Standard_Real& last, 
                                  const TopAbs_Orientation &orient)
{
  if ( orient == TopAbs_REVERSED ) {
    Standard_Real cf = first;
    first = curve->ReversedParameter ( last );
    last = curve->ReversedParameter ( cf );
    return curve->Reversed();
  }
  return HCurve::DownCast(curve->Copy());
}

template<class HCurve> 
static inline void SegmentCurve (HCurve& curve,
                                 const Standard_Real first,
                                 const Standard_Real last)
{
  if(curve->FirstParameter() < first - Precision::PConfusion() || 
     curve->LastParameter() > last + Precision::PConfusion()) {
    if(curve->IsPeriodic())
      curve->Segment(first,last);
    else curve->Segment(Max(curve->FirstParameter(),first),
                        Min(curve->LastParameter(),last));
  } 
}

template<class HPoint> 
static inline void GetReversedParameters(const HPoint& p11, 
                                  const HPoint& p12,
                                  const HPoint& p21,
                                  const HPoint& p22,
                                  Standard_Boolean& isRev1, 
                                  Standard_Boolean& isRev2)
{
  isRev1 = Standard_False;
  isRev2 = Standard_False;
   //gka protection against crossing seem on second face 
  
  Standard_Real d11 = p11.Distance(p21);
  Standard_Real d21 =p12.Distance(p21);
  
  Standard_Real d12 = p11.Distance(p22);
  Standard_Real d22 = p22.Distance(p12);
  Standard_Real Dmin1 = Min(d11,d21);
  Standard_Real Dmin2 = Min(d12,d22);
  if(fabs(Dmin1 - Dmin2) <= Precision::Confusion() || Dmin2 > Dmin1) {
    isRev1 = (d11 < d21 ? Standard_True : Standard_False);
  }
  else if(Dmin2 < Dmin1) {
    isRev1 = (d12 < d22 ? Standard_True  : Standard_False);
    isRev2 = Standard_True;
  }
 
  
}
//=======================================================================
//function : JoinCurves
//purpose  : 
//=======================================================================

Standard_Boolean ShapeConstruct::JoinCurves(const Handle(Geom_Curve)& ac3d1,
                                            const Handle(Geom_Curve)& ac3d2,
                                            const TopAbs_Orientation Orient1,
                                            const TopAbs_Orientation Orient2,
                                            Standard_Real& first1,
                                            Standard_Real& last1,
                                            Standard_Real& first2,
                                            Standard_Real& last2,
                                            Handle(Geom_Curve)& c3dOut,
                                            Standard_Boolean& isRev1,
                                            Standard_Boolean& isRev2)
                                            
{
  Handle(Geom_Curve) c3d1,c3d2;
 
  c3d1 = GetCurveCopy ( ac3d1, first1, last1, Orient1 );
  c3d2 = GetCurveCopy ( ac3d2, first2, last2, Orient2 );
  ShapeConstruct_Curve scc;
  Standard_Boolean After =  Standard_True;
  Handle(Geom_BSplineCurve) bsplc1 = scc.ConvertToBSpline(c3d1,first1, last1,Precision::Confusion());
  Handle(Geom_BSplineCurve) bsplc2 = scc.ConvertToBSpline(c3d2,first2, last2,Precision::Confusion());
//  newf = first1;
//  newl = last1 + last2 - first2;
  
  if(bsplc1.IsNull() || bsplc2.IsNull()) return Standard_False;
  
  SegmentCurve(bsplc1,first1, last1);
  SegmentCurve(bsplc2,first2, last2);
  
  //regression on file 866026_M-f276-f311.brep bug OCC482
  gp_Pnt pp11 =  bsplc1->Pole(1);
  gp_Pnt pp12 =  bsplc1->Pole(bsplc1->NbPoles());
  
  gp_Pnt pp21 =  bsplc2->Pole(1);
  gp_Pnt pp22 =  bsplc2->Pole(bsplc2->NbPoles());
  
  GetReversedParameters(pp11,pp12,pp21,pp22,isRev1,isRev2);
  
  if(isRev1) {
    bsplc1->Reverse();
  }
  if(isRev2)
    bsplc2->Reverse();
  
  gp_Pnt pmid = 0.5 * ( bsplc1->Pole(bsplc1->NbPoles()).XYZ() + bsplc2->Pole(1).XYZ() );
  bsplc1->SetPole(bsplc1->NbPoles(), pmid);
  bsplc2->SetPole(1, pmid);
  GeomConvert_CompCurveToBSplineCurve connect3d(bsplc1);
  if(!connect3d.Add(bsplc2,Precision::Confusion(), After, Standard_False)) return Standard_False;
  c3dOut = connect3d.BSplineCurve();
  return Standard_True;
}

//=======================================================================
//function : JoinCurves2d
//purpose  : 
//=======================================================================

 Standard_Boolean ShapeConstruct::JoinCurves(const Handle(Geom2d_Curve)& aC2d1,
                                   const Handle(Geom2d_Curve)& aC2d2,
                                   const TopAbs_Orientation Orient1,
                                   const TopAbs_Orientation Orient2,
                                   Standard_Real& first1,
                                   Standard_Real& last1,
                                   Standard_Real& first2,
                                   Standard_Real& last2,
                                   Handle(Geom2d_Curve)& C2dOut,
                                   Standard_Boolean& isRev1,
                                   Standard_Boolean& isRev2,
                                   const Standard_Boolean isError)
{
   Handle(Geom2d_Curve) c2d1,c2d2;
  c2d1 = GetCurveCopy ( aC2d1, first1, last1, Orient1 );
  c2d2 = GetCurveCopy ( aC2d2, first2, last2, Orient2 );
  ShapeConstruct_Curve scc;
  Standard_Boolean After =  Standard_True;
  
  Handle(Geom2d_BSplineCurve) bsplc12d = scc.ConvertToBSpline(c2d1,first1,last1,Precision::Confusion());
  Handle(Geom2d_BSplineCurve) bsplc22d = scc.ConvertToBSpline(c2d2,first2,last2,Precision::Confusion());
  
  if(bsplc12d.IsNull() || bsplc22d.IsNull()) return Standard_False;
  
  SegmentCurve(bsplc12d,first1,last1);
  SegmentCurve(bsplc22d,first2,last2);
  //gka protection against crossing seem on second face 
  gp_Pnt2d pp112d =  bsplc12d->Pole(1).XY();
  gp_Pnt2d pp122d =  bsplc12d->Pole(bsplc12d->NbPoles()).XY();
  
  gp_Pnt2d pp212d =  bsplc22d->Pole(1).XY();
  gp_Pnt2d pp222d =  bsplc22d->Pole(bsplc22d->NbPoles()).XY();
  
  GetReversedParameters(pp112d,pp122d,pp212d,pp222d,isRev1,isRev2);
  
  //regression on file 866026_M-f276-f311.brep bug OCC482
  //if(isRev1 || isRev2)
  //  return newedge1;
  if(isRev1) {
    bsplc12d->Reverse();
  }
  if(isRev2)
    bsplc22d->Reverse(); 
  
  
  //---------------------------------------------------------
  //protection against invalid topology Housing(sam1296.brep(face 707) - bugmergeedges4.brep)
  if(isError) {
    gp_Pnt2d pp1 = bsplc12d->Value(bsplc12d->FirstParameter());
    gp_Pnt2d pp2 = bsplc12d->Value(bsplc12d->LastParameter());
    gp_Pnt2d pp3 = bsplc12d->Value((bsplc12d->FirstParameter() + bsplc12d->LastParameter())*0.5);
    
    Standard_Real leng = pp1.Distance(pp2);
    Standard_Boolean isCircle = (leng < pp1.Distance(pp3) + Precision::PConfusion());
    if((pp1.Distance(bsplc22d->Pole(1)) < leng) && !isCircle) return Standard_False;
  }
  //-------------------------------------------------------
  gp_Pnt2d pmid1 = 0.5 * ( bsplc12d->Pole(bsplc12d->NbPoles()).XY() + bsplc22d->Pole(1).XY() );
      bsplc12d->SetPole(bsplc12d->NbPoles(), pmid1);
  bsplc22d->SetPole(1, pmid1);
  
  // abv 01 Sep 99: Geom2dConvert ALWAYS performs reparametrisation of the
  // second curve before merging; this is quite not suitable
  // Use 3d tool instead
//      Geom2dConvert_CompCurveToBSplineCurve connect2d(bsplc12d);
  gp_Pnt vPnt(0,0,0);
  gp_Vec vDir(0,0,1);
  gp_Pln vPln ( vPnt, vDir );
  Handle(Geom_BSplineCurve) bspl1 = 
    Handle(Geom_BSplineCurve)::DownCast ( GeomAPI::To3d ( bsplc12d, vPln ) );
  Handle(Geom_BSplineCurve) bspl2 = 
    Handle(Geom_BSplineCurve)::DownCast ( GeomAPI::To3d ( bsplc22d, vPln ) );
  GeomConvert_CompCurveToBSplineCurve connect2d(bspl1);
  if(!connect2d.Add(bspl2,Precision::PConfusion(), After, Standard_False)) return Standard_False;
   C2dOut = GeomAPI::To2d ( connect2d.BSplineCurve(), vPln );
  
  return Standard_True;
}
