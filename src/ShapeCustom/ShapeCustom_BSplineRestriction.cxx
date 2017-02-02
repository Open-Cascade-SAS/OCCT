// Created on: 1999-06-18
// Created by: Galina Koulikova
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
#include <BRep_GCurve.hxx>
#include <BRep_ListIteratorOfListOfCurveRepresentation.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Conic.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Conic.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ElementarySurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <Message_Msg.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeConstruct.hxx>
#include <ShapeCustom_BSplineRestriction.hxx>
#include <ShapeCustom_RestrictionParameters.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeCustom_BSplineRestriction,ShapeCustom_Modification)

static GeomAbs_Shape IntegerToGeomAbsShape(const Standard_Integer i)
{
  GeomAbs_Shape result = GeomAbs_C0;
  switch (i) {
    case 0: result = GeomAbs_C0; break;
    case 1: result = GeomAbs_C1; break;
    case 2: result = GeomAbs_C2; break;
    case 3: result = GeomAbs_C3; break;
    default : result = GeomAbs_CN; break;
  }
  return result;
}

static Standard_Integer ContToInteger( const GeomAbs_Shape Cont)
{
  Standard_Integer result =0;
  switch(Cont) {
    case GeomAbs_C0:
    case GeomAbs_G1: result = 0; break;
    case GeomAbs_C1:
    case GeomAbs_G2: result = 1; break;
    case GeomAbs_C2: result = 2; break;  
    case GeomAbs_C3: result = 3; break;
    default : result = 4; break;
  }
  return result;
}

static Standard_Boolean IsConvertCurve3d(const Handle(Geom_Curve)& aCurve,
                                         Standard_Integer Degree,
                                         Standard_Integer NbSeg,
                                         Standard_Boolean myRational,
                                         const Handle(ShapeCustom_RestrictionParameters)& aParameters)
{
  if(aCurve.IsNull()) return Standard_False;
  if(aParameters->ConvertCurve3d()) return Standard_True;
  if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) tmp = Handle(Geom_TrimmedCurve)::DownCast (aCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    return IsConvertCurve3d(BasCurve,Degree,NbSeg,myRational,aParameters);
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
    if(aParameters->ConvertOffsetCurv3d()) return Standard_True;
    Handle(Geom_OffsetCurve) tmp = Handle(Geom_OffsetCurve)::DownCast (aCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    return IsConvertCurve3d(BasCurve,Degree,NbSeg,myRational,aParameters); 
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    Handle(Geom_BSplineCurve) BsC = Handle(Geom_BSplineCurve)::DownCast(aCurve);
    if( BsC->Degree() > Degree || ((BsC->NbKnots() - 1) >= NbSeg))
      return Standard_True;
    if(myRational && BsC->IsRational())
      return Standard_True;
    else return Standard_False;
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)) && 
    (Handle(Geom_BezierCurve)::DownCast(aCurve)->Degree() > Degree ||
    (myRational &&  Handle(Geom_BezierCurve)::DownCast(aCurve)->IsRational())))
    return Standard_True;
  // else return Standard_False;
  return Standard_False;
}

static Standard_Boolean IsConvertSurface(const Handle(Geom_Surface)& aSurface,
                                         const Standard_Integer Degree,
                                         const Standard_Integer NbSeg,
                                         const Standard_Boolean myRational,
                                         const Handle(ShapeCustom_RestrictionParameters)& aParameters)
{
  if (aSurface.IsNull()) return Standard_False;
  if (aSurface->IsKind(STANDARD_TYPE(Geom_Plane))) {
    return aParameters->ConvertPlane();
  }
  else if(aSurface->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
    return aParameters->ConvertConicalSurf();
  else if(aSurface->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    return aParameters->ConvertSphericalSurf();
  else if(aSurface->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
    return aParameters->ConvertCylindricalSurf();
  else if(aSurface->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    return aParameters->ConvertToroidalSurf();

  //else if(aSurface->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))    {
  //  return aParameters->ConvertElementarySurf();
  // }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_SweptSurface))) {
    if(aSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) && aParameters->ConvertRevolutionSurf())
      return Standard_True;
    if(aSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) && aParameters->ConvertExtrusionSurf())
      return Standard_True;   
    Handle(Geom_SweptSurface) aSurf = Handle(Geom_SweptSurface)::DownCast(aSurface);
    Handle(Geom_Curve) BasCurve = aSurf->BasisCurve();
    return IsConvertCurve3d(BasCurve,Degree,NbSeg,myRational,aParameters);
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) aSurf = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    Handle(Geom_Surface) theSurf = aSurf->BasisSurface();
    return IsConvertSurface(theSurf,Degree,NbSeg,myRational,aParameters);
  }
  if(aSurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) { 
    if(aParameters->ConvertOffsetSurf()) return Standard_True;
    Handle(Geom_OffsetSurface) aSurf = Handle(Geom_OffsetSurface)::DownCast(aSurface);
    Handle(Geom_Surface) theSurf = aSurf->BasisSurface();
    return IsConvertSurface(theSurf,Degree,NbSeg,myRational,aParameters);
  }
  if (aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {

    Handle(Geom_BSplineSurface) theSurf = Handle(Geom_BSplineSurface)::DownCast(aSurface);
    if(theSurf->UDegree() > Degree || theSurf->VDegree() > Degree)  
      return Standard_True;
    if((theSurf->NbUKnots()-1) * (theSurf->NbVKnots()-1) > NbSeg)
      return Standard_True;
    if(myRational && (theSurf->IsURational() || theSurf->IsVRational())) 
      return Standard_True; 
    return Standard_False;
  } 

  if (aSurface->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    if(aParameters->ConvertBezierSurf())
      return Standard_True;
    Handle(Geom_BezierSurface) theSurf = Handle(Geom_BezierSurface)::DownCast(aSurface);
    if(theSurf->UDegree() > Degree || theSurf->VDegree() > Degree)
      return Standard_True;
    if( myRational && (theSurf->IsURational() || theSurf->IsVRational()))
      return Standard_True;
    return Standard_False;
  }
  return Standard_False;
}

static Standard_Boolean IsConvertCurve2d(const Handle(Geom2d_Curve)& aCurve,
                                         Standard_Integer Degree,
                                         Standard_Integer NbSeg,
                                         Standard_Boolean myRational,
                                         const Handle(ShapeCustom_RestrictionParameters)& aParameters)
{
  if (aCurve.IsNull()) return Standard_False;
  if (aParameters->ConvertCurve2d()) return Standard_True;
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (aCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    return IsConvertCurve2d(BasCurve,Degree,NbSeg,myRational,aParameters);
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) {
    if(aParameters->ConvertOffsetCurv2d()) return Standard_True;
    Handle(Geom2d_OffsetCurve) tmp = Handle(Geom2d_OffsetCurve)::DownCast (aCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    return IsConvertCurve2d(BasCurve,Degree,NbSeg,myRational,aParameters);
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) &&
    ((Handle(Geom2d_BSplineCurve)::DownCast(aCurve)->Degree() > Degree || 
    ((Handle(Geom2d_BSplineCurve)::DownCast(aCurve)->NbKnots() -1) > NbSeg )) ||
    (myRational && Handle(Geom2d_BSplineCurve)::DownCast(aCurve)->IsRational()))) 
    return Standard_True;
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) && 
    ((Handle(Geom2d_BezierCurve)::DownCast(aCurve)->Degree() > Degree) ||
    (myRational && Handle(Geom2d_BezierCurve)::DownCast(aCurve)->IsRational())))
    return Standard_True;
  // else return Standard_False;
  return Standard_False;
}

//=======================================================================
//function : ShapeCustom_BSplineRestriction
//purpose  : 
//=======================================================================

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction()
{
  myApproxSurfaceFlag = Standard_True;
  myApproxCurve3dFlag = Standard_True;
  myApproxCurve2dFlag = Standard_True; 
  myTol3d = 0.01;
  myTol2d = 1E-6;
  myContinuity3d = GeomAbs_C1;
  myContinuity2d =GeomAbs_C2 ;
  myMaxDegree = 9;
  myNbMaxSeg = 10000;
  mySurfaceError = Precision::Confusion();
  myCurve3dError = Precision::Confusion();
  myCurve2dError = Precision::PConfusion();
  myNbOfSpan = 0;
  myConvert = Standard_False;
  myDeg =Standard_True;
  myRational = Standard_False;
  myParameters = new ShapeCustom_RestrictionParameters;

}

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction(const Standard_Boolean anApproxSurfaceFlag,
                                                               const Standard_Boolean anApproxCurve3dFlag,
                                                               const Standard_Boolean anApproxCurve2dFlag,
                                                               const Standard_Real aTol3d,
                                                               const Standard_Real aTol2d,
                                                               const GeomAbs_Shape aContinuity3d,
                                                               const GeomAbs_Shape aContinuity2d,
                                                               const Standard_Integer aMaxDegree,
                                                               const Standard_Integer aNbMaxSeg,
                                                               const Standard_Boolean Deg,
                                                               const Standard_Boolean Rational)
{
  myApproxSurfaceFlag = anApproxSurfaceFlag;
  myApproxCurve3dFlag = anApproxCurve3dFlag;
  myApproxCurve2dFlag = anApproxCurve2dFlag;
  myTol3d = aTol3d;
  myTol2d = aTol2d;
  myMaxDegree = aMaxDegree;
  myContinuity3d = aContinuity3d;
  myContinuity2d = aContinuity2d;
  myNbMaxSeg = aNbMaxSeg;
  mySurfaceError = Precision::Confusion();
  myCurve3dError = Precision::Confusion();
  myCurve2dError = Precision::PConfusion();
  myNbOfSpan = 0;
  myConvert = Standard_False;
  myDeg = Deg;
  myRational = Rational;
  myParameters = new ShapeCustom_RestrictionParameters;

}

ShapeCustom_BSplineRestriction::ShapeCustom_BSplineRestriction(const Standard_Boolean anApproxSurfaceFlag,
                                                               const Standard_Boolean anApproxCurve3dFlag,
                                                               const Standard_Boolean anApproxCurve2dFlag,
                                                               const Standard_Real aTol3d,
                                                               const Standard_Real aTol2d,
                                                               const GeomAbs_Shape aContinuity3d,
                                                               const GeomAbs_Shape aContinuity2d,
                                                               const Standard_Integer aMaxDegree,
                                                               const Standard_Integer aNbMaxSeg,
                                                               const Standard_Boolean Deg,
                                                               const Standard_Boolean Rational,
                                                               const Handle(ShapeCustom_RestrictionParameters)& aModes)
{
  myApproxSurfaceFlag = anApproxSurfaceFlag;
  myApproxCurve3dFlag = anApproxCurve3dFlag;
  myApproxCurve2dFlag = anApproxCurve2dFlag;
  myTol3d = aTol3d;
  myTol2d = aTol2d;
  myMaxDegree = aMaxDegree;
  myContinuity3d = aContinuity3d;
  myContinuity2d = aContinuity2d;
  myNbMaxSeg = aNbMaxSeg;
  mySurfaceError = Precision::Confusion();
  myCurve3dError = Precision::Confusion();
  myCurve2dError = Precision::PConfusion();
  myNbOfSpan = 0;
  myConvert = Standard_False;
  myDeg = Deg;
  myRational = Rational;
  myParameters = aModes;

}

//=======================================================================
//function : NewSurface
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::NewSurface(const TopoDS_Face& F,
                                                            Handle(Geom_Surface)& S,
                                                            TopLoc_Location& L,
                                                            Standard_Real& Tol,
                                                            Standard_Boolean& RevWires,
                                                            Standard_Boolean& RevFace)
{
  if ( ! myApproxSurfaceFlag )
    return Standard_False;
  RevWires = Standard_False;
  RevFace = Standard_False;
  myConvert = Standard_False;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(F,L);
  if(aSurface.IsNull()) return Standard_False;
  Standard_Boolean IsOf = Standard_True;
  if(myParameters->ConvertOffsetSurf()) IsOf = Standard_False;
  Standard_Real UF,UL,VF,VL;
  aSurface->Bounds(UF,UL,VF,VL);
  Standard_Real Umin, Umax, Vmin, Vmax;
  BRepTools::UVBounds(F,Umin, Umax, Vmin, Vmax);
  if(myParameters->SegmentSurfaceMode()) {
    UF = Umin; UL =  Umax;
    VF = Vmin; VL =  Vmax;
  }
  else {
    if(Precision::IsInfinite(UF) || Precision::IsInfinite(UL)) {
      UF = Umin;
      UL = Umax;
    }
    if(Precision::IsInfinite(VF) || Precision::IsInfinite(VL)) {
      VF = Vmin;
      VL = Vmax;
    }
  }

  Standard_Boolean IsConv = ConvertSurface(aSurface,S,UF,UL,VF,VL,IsOf);
  Tol = Precision::Confusion();//mySurfaceError;

  if ( IsConv )
  {
    Standard_Boolean wasBSpline = aSurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface));
    Handle(Geom_RectangularTrimmedSurface) rts = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
    if ( !rts.IsNull() )
      wasBSpline = rts->BasisSurface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface));

    if ( wasBSpline )
      SendMsg( F, Message_Msg("BSplineRestriction.NewSurface.MSG1"));
    else
      SendMsg( F, Message_Msg("BSplineRestriction.NewSurface.MSG0"));
  }

  return IsConv;
}

//=======================================================================
//function : ConvertSurface
//purpose  : 
//=======================================================================

static void ConvertExtrusion(const Handle(Geom_Curve)& C,/*const gp_Dir& direction,*/
                             gp_Trsf& shiftF,gp_Trsf& shiftL,
                             const Standard_Real VF,const Standard_Real VL,
                             Handle(Geom_Surface)& bspline)
{
  Handle(Geom_BSplineCurve) bspl = Handle(Geom_BSplineCurve)::DownCast(C);  
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

  bspline = new Geom_BSplineSurface(resPoles, resWeigth, knots, vknots, mults, vmults,
    bspl->Degree(),1,bspl->IsPeriodic(),Standard_False);
}


Standard_Boolean ShapeCustom_BSplineRestriction::ConvertSurface(const Handle(Geom_Surface)& aSurface,
                                                                Handle(Geom_Surface)& S,
                                                                const Standard_Real UF,
                                                                const Standard_Real UL,
                                                                const Standard_Real VF,
                                                                const Standard_Real VL,
                                                                const Standard_Boolean IsOf)
{ 
  if(!IsConvertSurface(aSurface,myMaxDegree,myNbMaxSeg,myRational,myParameters))
    return Standard_False;

  Handle(Geom_Surface) aSurf = aSurface;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_Plane)) && myParameters->ConvertPlane()) {
    Handle(Geom_Plane) pln = Handle(Geom_Plane)::DownCast(aSurf);
    TColgp_Array2OfPnt poles(1,2,1,2);
    TColStd_Array1OfReal uknots(1,2);
    TColStd_Array1OfInteger umults(1,2);
    TColStd_Array1OfReal vknots(1,2);
    TColStd_Array1OfInteger vmults(1,2);

    poles(1,1) = pln->Value(UF,VF); poles(1,2) = pln->Value(UF,VL);
    poles(2,1) = pln->Value(UL,VF);  poles(2,2) = pln->Value(UL,VL);
    uknots(1) = UF; uknots(2) = UL; 
    vknots(1) = VF; vknots(2) = VL;
    umults(1) = umults(2) = vmults(1) = vmults(2) = 2;
    S = new Geom_BSplineSurface(poles, uknots, vknots, umults, vmults, 1, 1, Standard_False, Standard_False);
    return Standard_True;    
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {      
    Handle(Geom_SurfaceOfRevolution) Surface = Handle(Geom_SurfaceOfRevolution)::DownCast(aSurf);
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve();
    Handle(Geom_Curve) ResCurve;
    Standard_Real TolS = Precision::Confusion(); 
    if(myParameters->ConvertRevolutionSurf()) {
      if(BasCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve))) {
        GeomAbs_Shape cnt = BasCurve->Continuity();
        cnt = (cnt > GeomAbs_C2 ? GeomAbs_C2: cnt);
        if(ConvertCurve(BasCurve,ResCurve,Standard_False,Max(VF,BasCurve->FirstParameter()),Min(VL,BasCurve->LastParameter()),TolS,Standard_False)) {
          Handle(Geom_SurfaceOfRevolution) newRevol = new Geom_SurfaceOfRevolution(ResCurve,Surface->Axis());
          aSurf = newRevol;
#ifdef OCCT_DEBUG
          cout <<" Revolution on offset converted" << endl;
#endif
        }
      }

    }
    else {
      if(ConvertCurve(BasCurve,ResCurve,Standard_False,Max(VF,BasCurve->FirstParameter()),Min(VL,BasCurve->LastParameter()),TolS,IsOf)) {
        S = new Geom_SurfaceOfRevolution(ResCurve,Surface->Axis());
        return Standard_True;
      }
      else 
        return Standard_False;
    }
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
    Handle(Geom_SurfaceOfLinearExtrusion) Surface = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(aSurf);
    Handle(Geom_Curve) BasCurve = Surface->BasisCurve();
    Handle(Geom_Curve) ResCurve;
    Standard_Real TolS = Precision::Confusion();
    if(myParameters->ConvertExtrusionSurf()) {
      GeomAbs_Shape cnt = Surface->Continuity();
      cnt = (cnt > GeomAbs_C2 ? GeomAbs_C2: cnt);
      Handle(Geom_BSplineCurve) bspl = ShapeConstruct::ConvertCurveToBSpline(BasCurve, UF, UL, TolS, cnt, myNbMaxSeg, myMaxDegree);
      BasCurve = bspl;
      ConvertCurve(BasCurve,ResCurve,Standard_True,Max(UF,BasCurve->FirstParameter()),Min(UL,BasCurve->LastParameter()),TolS,IsOf);
      gp_Trsf shiftF,shiftL;
      shiftF.SetTranslation(Surface->Value(UF,0),Surface->Value(UF,VF));
      shiftL.SetTranslation(Surface->Value(UF,0),Surface->Value(UF,VL));
      ConvertExtrusion(ResCurve,/*Surface->Direction(),*/shiftF,shiftL,VF,VL,S);
      return Standard_True;
    }
    else {
      if(ConvertCurve(BasCurve,ResCurve,Standard_False,Max(UF,BasCurve->FirstParameter()),Min(UL,BasCurve->LastParameter()),TolS,IsOf)) {
        S = new Geom_SurfaceOfLinearExtrusion(ResCurve,Surface->Direction());
        return Standard_True;
      }
      else 
        return Standard_False;
    }
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) tmp = Handle(Geom_RectangularTrimmedSurface)::
      DownCast (aSurf);
    Standard_Real U1,U2,V1,V2;
    tmp->Bounds(U1,U2,V1,V2);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    Handle(Geom_Surface) ResSurface;
    if(ConvertSurface(theSurf,ResSurface,U1,U2,V1,V2,IsOf)) {
      //S = new Geom_RectangularTrimmedSurface(ResSurface,U1,U2,V1,V2);
      S = ResSurface;
      return Standard_True;
    }
    else 
      return Standard_False;

  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)) && IsOf) {
    Handle(Geom_OffsetSurface) tmp = Handle(Geom_OffsetSurface)::DownCast (aSurf);
    Handle(Geom_Surface) theSurf = tmp->BasisSurface();
    Handle(Geom_Surface) ResSurface;
    if(ConvertSurface(theSurf,ResSurface,UF,UL,VF,VL)) {
      if(ResSurface->Continuity() != GeomAbs_C0) {
        S = new Geom_OffsetSurface(ResSurface,tmp->Offset());
        return Standard_True;
      }
      else if(ConvertSurface(aSurf,S,UF,UL,VF,VL,Standard_False)) 
        return Standard_True;
      else return Standard_False;
    }
    else 
      return Standard_False;

  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)) && myParameters->ConvertBezierSurf()) {
    Handle(Geom_BezierSurface) bezier = Handle(Geom_BezierSurface)::DownCast(aSurf);
    Standard_Integer nbUPoles = bezier->NbUPoles();
    Standard_Integer nbVPoles = bezier->NbVPoles();
    Standard_Integer uDegree = bezier->UDegree();
    Standard_Integer vDegree = bezier->VDegree();
    TColgp_Array2OfPnt aPoles(1,nbUPoles,1,nbVPoles);
    TColStd_Array2OfReal aWeights(1,nbUPoles,1,nbVPoles);
    bezier->Poles(aPoles);
    bezier->Weights(aWeights);
    TColStd_Array1OfReal uKnots(1,2), vKnots(1,2);
    uKnots(1) = 0; uKnots(2) = 1;
    vKnots(1) = 0; vKnots(2) = 1;
    TColStd_Array1OfInteger uMults(1,2), vMults(1,2);
    uMults.Init(uDegree+1);
    vMults.Init(vDegree+1);
    Handle(Geom_BSplineSurface) bspline = new Geom_BSplineSurface(aPoles,aWeights,uKnots,vKnots,
      uMults,vMults,uDegree,vDegree);

    if(!ConvertSurface(bspline,S,UF,UL,VF,VL,IsOf))
      S = bspline;
    return Standard_True;
  }

  Standard_Integer NbSeg = 1;
  Standard_Boolean URat = Standard_False;
  Standard_Boolean VRat = Standard_False;
  //if (aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)) || 
  //    aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)) || 
  //    (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)) && !IsOf) ||
  //    aSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) ||
  //    aSurface->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))) {
  Standard_Integer UDeg=1,VDeg=1;
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
    Handle(Geom_BSplineSurface) BsS =  Handle(Geom_BSplineSurface)::DownCast (aSurf);
    UDeg = BsS->UDegree();
    VDeg = BsS->VDegree();
    NbSeg = (BsS->NbUKnots()-1)*(BsS->NbVKnots()-1);
    URat = BsS->IsURational();
    VRat = BsS->IsVRational();
    Standard_Boolean IsR = (myRational && (URat || VRat));
    if( UDeg <= myMaxDegree &&  VDeg <= myMaxDegree && NbSeg <=  myNbMaxSeg && !IsR ) 
      return Standard_False;
  }
  if (aSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {
    Handle(Geom_BezierSurface) BsZ = Handle(Geom_BezierSurface)::DownCast (aSurf);
    UDeg = BsZ->UDegree();
    VDeg = BsZ->VDegree();
    NbSeg =1;
    URat = BsZ->IsURational();
    VRat = BsZ->IsVRational();
    Standard_Boolean IsR = (myRational && (URat || VRat));
    if( UDeg <= myMaxDegree &&  VDeg <= myMaxDegree && NbSeg <=  myNbMaxSeg && !IsR ) 
      return Standard_False;

  }
  GeomAbs_Shape Cont = myContinuity3d;
  if(aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) Cont = GeomAbs_C0;
  /*    Standard_Boolean IsR = (myRational && (URat || VRat));
  if( UDeg <= myMaxDegree &&  VDeg <= myMaxDegree && NbSeg <=  myNbMaxSeg && !IsR ) {
  return Standard_False;

  }*/

  Standard_Real aTol3d;
  Standard_Integer nbOfSpan,imax=10;
  Standard_Integer MaxSeg = myNbMaxSeg;
  Standard_Integer MaxDeg = myMaxDegree;
  Standard_Real u1,u2,v1,v2;
  aSurf->Bounds(u1,u2,v1,v2);
  Standard_Real ShiftU = 0, ShiftV = 0;
  if( Abs(u1-UF) > Precision::PConfusion() || Abs(u2- UL) > Precision::PConfusion() || 
    Abs(v1-VF) > Precision::PConfusion() || Abs(v2- VL) > Precision::PConfusion()) {
      /*if(aSurf->IsUPeriodic() ) {
      Standard_Real aDelta = (UL > UF ? UL - UF : UF - UL );
      u1 = (aDelta > 2.*M_PI ? 0. : UF + ShapeAnalysis::AdjustByPeriod(UF,0.5*(UL+UF),2*M_PI)); 
      u2 = (aDelta > 2.*M_PI ? 2.*M_PI : u1 + aDelta); 
      }*/
      Standard_Boolean isTrim = Standard_False;
      if(!aSurf->IsUPeriodic() ) { //else {
        u1 = Max(u1,UF); u2 = Min(u2,UL);
        isTrim = Standard_True;
      }
      /*if(aSurf->IsVPeriodic()) {

      Standard_Real aDelta = (VL > VF ? VL - VF : VF - VL );
      v1 = (aDelta > 2.*M_PI ? 0. : VF + ShapeAnalysis::AdjustByPeriod(VF,0.5*(UL+UF),2*M_PI)); ; 
      v2 = (aDelta > 2.*M_PI ? 2.* M_PI : v1 + aDelta); 
      }*/
      if(!aSurf->IsVPeriodic()) {//else 
        v1 = Max(v1,VF); v2 = Min(v2,VL);
        isTrim = Standard_True;
      }

      if(isTrim && (u1 != u2) && (v1 != v2)) {
        Handle(Geom_RectangularTrimmedSurface) trSurface = new Geom_RectangularTrimmedSurface(aSurf,u1,u2,v1,v2);
        Standard_Real ur1,ur2,vr1,vr2;
        trSurface->Bounds(ur1,ur2,vr1,vr2);
        ShiftU = u1-ur1;
        ShiftV = v1-vr1;
        aSurf = trSurface;
      }
  }
  Standard_Integer aCU= Min(ContToInteger(Cont),ContToInteger(aSurf->Continuity()));
  Standard_Integer aCV = Min(ContToInteger(Cont),ContToInteger( aSurf->Continuity()));
  if(!aCU)
    aCU = ContToInteger(Cont);
  if(!aCV)
    aCV = ContToInteger(Cont);

  for(; ;) {
    Standard_Real prevTol = RealLast(),newTol =0;
    for (Standard_Integer i=1; i <= imax; i++) {
      aTol3d = myTol3d*i/2;
      while (aCU >= 0 || aCV >= 0) {
        try {
          OCC_CATCH_SIGNALS
            GeomAbs_Shape aContV = IntegerToGeomAbsShape(aCV);
          GeomAbs_Shape aContU = IntegerToGeomAbsShape(aCU);

          GeomConvert_ApproxSurface anApprox(aSurf,aTol3d,aContU,aContV,MaxDeg,MaxDeg,MaxSeg,0);
          Standard_Boolean Done = anApprox.IsDone();
          newTol = anApprox.MaxError();
          if (anApprox.MaxError() <= myTol3d && Done) {

            nbOfSpan = (anApprox.Surface()->NbUKnots()-1)*(anApprox.Surface()->NbVKnots()-1);
#ifdef OCCT_DEBUG
            if((imax-i+1)!=1) {
              cout << " iteration = " << i
                <<    "\terror = " << anApprox.MaxError()
                <<    "\tspans = " << nbOfSpan << endl;
              cout<< " Surface is aproximated with continuity " << IntegerToGeomAbsShape(Min(aCU,aCV)) <<endl;
            }
#endif
            S = anApprox.Surface();
            Handle(Geom_BSplineSurface) Bsc = Handle(Geom_BSplineSurface)::DownCast(S);
            if(aSurface->IsUPeriodic() )
              Bsc->SetUPeriodic();
            if(aSurface->IsVPeriodic() )
              Bsc->SetVPeriodic();
            //Standard_Integer DegU = Bsc->UDegree(); // DegU not used (skl)
            //Standard_Integer DegV = Bsc->VDegree(); // DegV not used (skl)
            //Standard_Integer nbVK = Bsc->NbVKnots(); // nbVK not used (skl)
            //Standard_Integer nbUK = Bsc->NbUKnots(); // nbUK not used (skl)
            myConvert = Standard_True;
            myNbOfSpan = myNbOfSpan + nbOfSpan;
            mySurfaceError = Max(mySurfaceError,anApprox.MaxError());
            if(Abs(ShiftU) > Precision::PConfusion()) {
              Standard_Integer nb = Bsc->NbUKnots();
              TColStd_Array1OfReal uknots(1,nb);
              Bsc->UKnots(uknots);
              for(Standard_Integer j = 1; j <= nb; j++)
                uknots(j)+=ShiftU;
              Bsc->SetUKnots(uknots);
            }
            if(Abs(ShiftV) > Precision::PConfusion()) {
              Standard_Integer nb = Bsc->NbVKnots();
              TColStd_Array1OfReal vknots(1,nb);
              Bsc->VKnots(vknots);
              for(Standard_Integer j = 1; j <= nb; j++)
                vknots(j)+=ShiftV;
              Bsc->SetVKnots(vknots);
            }

            return Standard_True;
          }
          else {
            break;
          }

        }

        catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout << "Warning: GeomConvert_ApproxSurface Exception: try to decrease continuity ";
          anException.Print(cout); cout << endl;
#endif
          (void)anException;
          //szv: protection against loop
          if(aCU == 0 && aCV == 0) break;
          if(aCU > 0) aCU--;
          if(aCV > 0) aCV--;
        }
      }
      if(prevTol <= newTol) break;
      else prevTol = newTol;
    }
    //Standard_Integer GMaxDegree = 15;//Geom_BSplineSurface::MaxDegree();

    if(myDeg) {
      if(MaxSeg < myParameters->GMaxSeg()){ 
        if(aCV != 0 || aCU != 0) {
          if(aCV > 0) aCV--;
          if(aCU > 0) aCU--;
        }
        else MaxSeg = 2*MaxSeg; //myGMaxSeg; 
        if(MaxSeg > myParameters->GMaxSeg()) 
          MaxSeg = myParameters->GMaxSeg();
        else continue;
      }
      else { 
#ifdef OCCT_DEBUG
        cout<<" Approximation iteration out. Surface is not aproximated." << endl;
#endif
        return Standard_False;
      }
    }
    else {
      if(MaxDeg < myParameters->GMaxDegree())
      { MaxDeg = myParameters->GMaxDegree(); continue;}
      else {
#ifdef OCCT_DEBUG	
        cout<<" Approximation iteration out. Surface is not aproximated." << endl;
#endif
        return Standard_False;
      }
    }
  }
  //}
  //else 
  //Surface is not BSpline or Bezier
  //  return Standard_False;
}

//=======================================================================
//function : NewCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::NewCurve(const TopoDS_Edge& E,
                                                          Handle(Geom_Curve)& C,
                                                          TopLoc_Location& L,
                                                          Standard_Real& Tol) 
{
  if ( ! myApproxCurve3dFlag )
    return Standard_False;
  Standard_Real First, Last;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(E,L,First, Last);
  Standard_Real TolCur = BRep_Tool::Tolerance(E);
  //if(aCurve.IsNull()) return Standard_False;
  Standard_Boolean IsConvert = Standard_False;
  Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
  // iterate on pcurves
  BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
  for ( ; itcr.More(); itcr.Next() ) {
    Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
    if ( GC.IsNull() || ! GC->IsCurveOnSurface() ) continue;
    Handle(Geom_Surface) aSurface = GC->Surface();
    Handle(Geom2d_Curve) aCurve2d = GC->PCurve();
    if((myApproxSurfaceFlag && 
      IsConvertSurface(aSurface,myMaxDegree,myNbMaxSeg,myRational,myParameters)) || 
      (myApproxCurve2dFlag && IsConvertCurve2d(aCurve2d,myMaxDegree,myNbMaxSeg,myRational,myParameters))) {
        IsConvert = Standard_True;
        break;
    }
  }
  if(aCurve.IsNull()) {
    if(IsConvert) {
      C = aCurve;
      Tol = TolCur;
      return Standard_True;
    }
    else return Standard_False;
  }
  Standard_Boolean IsOf = Standard_True;
  if(myParameters->ConvertOffsetCurv3d())  IsOf = Standard_False;
  Standard_Boolean IsConv = ConvertCurve(aCurve,C,IsConvert,First,Last,TolCur,IsOf);
  Tol= BRep_Tool::Tolerance(E);//TolCur;

  if ( IsConv )
  {
    Standard_Boolean wasBSpline = aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve));
    Handle(Geom_TrimmedCurve) tc = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
    if ( !tc.IsNull() )
      wasBSpline = tc->BasisCurve()->IsKind(STANDARD_TYPE(Geom_BSplineCurve));

    if ( wasBSpline )
      SendMsg( E, Message_Msg("BSplineRestriction.NewCurve.MSG1"));
    else
      SendMsg( E, Message_Msg("BSplineRestriction.NewCurve.MSG0"));
  }
  return IsConv;
}

//=======================================================================
//function : ConvertCurve
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::ConvertCurve(const Handle(Geom_Curve)& aCurve,
                                                              Handle(Geom_Curve)& C,
                                                              const Standard_Boolean IsConvert,
                                                              const Standard_Real First,
                                                              const Standard_Real Last,
                                                              Standard_Real& TolCur,
                                                              const Standard_Boolean IsOf) 
{
  //  TolCur =  Precision::Confusion();
  if (aCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
    Handle(Geom_TrimmedCurve) tmp = Handle(Geom_TrimmedCurve)::DownCast (aCurve);
    //Standard_Real pf =tmp->FirstParameter(), pl =  tmp->LastParameter(); // pf,pl not used - see below (skl)
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    Handle(Geom_Curve) ResCurve;
    if(ConvertCurve(BasCurve,ResCurve,IsConvert,First,Last,TolCur,IsOf)) {
      //      Stanadrd_Real F = Max(pf,First), L = Min(pl,Last);
      //      if(First != Last)
      //	C = new Geom_TrimmedCurve(ResCurve,Max(First,ResCurve->FirstParameter()),Min(Last,ResCurve->LastParameter()));
      //else 
      C = ResCurve;
      return Standard_True;
    }
    else {
      if(IsConvert) {
        C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
        TolCur =  Precision::Confusion();
        return Standard_True;
      }

      return Standard_False;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)) && myParameters->ConvertCurve3d()) {
    Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(aCurve);
    TColgp_Array1OfPnt poles(1,2);
    poles(1) = aLine->Value(First);
    poles(2) = aLine->Value(Last);
    TColStd_Array1OfReal knots(1,2);
    knots(1) = First; knots(2) = Last;
    TColStd_Array1OfInteger mults(1,2);
    mults.Init(2);
    Handle(Geom_BSplineCurve) res = new Geom_BSplineCurve(poles,knots,mults,1);
    C = res;
    return Standard_True;
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_Conic)) && myParameters->ConvertCurve3d()) {
    Handle(Geom_BSplineCurve) aBSpline;
    Handle(Geom_Curve) tcurve = new Geom_TrimmedCurve(aCurve,First,Last); //protection agains parabols ets
    GeomConvert_ApproxCurve approx (tcurve, myTol3d/*Precision::Approximation()*/, myContinuity2d, myNbMaxSeg, 6 );
    if ( approx.HasResult() )
      aBSpline = approx.Curve();
    else 
      aBSpline = GeomConvert::CurveToBSplineCurve(tcurve,Convert_QuasiAngular);

    Standard_Real Shift = First - aBSpline->FirstParameter();
    if(Abs(Shift) > Precision::PConfusion()) {
      Standard_Integer nbKnots = aBSpline->NbKnots();
      TColStd_Array1OfReal newKnots(1,nbKnots);
      aBSpline->Knots(newKnots);
      for (Standard_Integer i = 1; i <= nbKnots; i++)
        newKnots(i)+=Shift;
      aBSpline->SetKnots(newKnots);
    }
    Handle(Geom_Curve) ResCurve;
    if(ConvertCurve(aBSpline,ResCurve,IsConvert,First,Last,TolCur,Standard_False)) {
      C = ResCurve;
      return Standard_True;
    }
    else {
      C = aBSpline;
      TolCur = Precision::PConfusion();
      return Standard_True;
    } 
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)) && myParameters->ConvertCurve3d()) {
    Handle(Geom_Curve) aBSpline 
      = GeomConvert::CurveToBSplineCurve(aCurve,Convert_QuasiAngular);
    Handle(Geom_Curve) ResCurve;
    if(ConvertCurve(aBSpline,ResCurve,IsConvert,First,Last,TolCur,Standard_False)) {
      C = ResCurve;
      return Standard_True;
    }
    else {
      C = aBSpline;
      TolCur = Precision::PConfusion();
      return Standard_True;
    } 
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)) && IsOf) {
    Handle(Geom_OffsetCurve) tmp = Handle(Geom_OffsetCurve)::DownCast (aCurve);
    Handle(Geom_Curve) BasCurve = tmp->BasisCurve();
    Handle(Geom_Curve) ResCurve;
    if(ConvertCurve(BasCurve,ResCurve,IsConvert,First,Last,TolCur)) {
      if(ResCurve->Continuity() != GeomAbs_C0) {
        C = new Geom_OffsetCurve(ResCurve,tmp->Offset(),tmp->Direction());
        return Standard_True;
      }
      else if(ConvertCurve(aCurve,C,IsConvert,First,Last,TolCur,Standard_False))
        return Standard_True;
      else {
        if(IsConvert) {
          C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
          TolCur =  Precision::Confusion();
          return Standard_True;
        }
        return Standard_False;
      }
    } 
    else {
      if(IsConvert) {
        C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
        TolCur =  Precision::Confusion();
        return Standard_True;
      }
      return Standard_False;
    }
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve)) || 
    aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve)) ||
    (aCurve->IsKind(STANDARD_TYPE(Geom_OffsetCurve)) && !IsOf))  {
      Standard_Integer Deg=1;

      if (aCurve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
        Handle(Geom_BSplineCurve) BsC = Handle(Geom_BSplineCurve)::DownCast (aCurve);
        Deg =BsC->Degree();
        Standard_Boolean IsR = (myRational && BsC->IsRational());
        if(!IsR && Deg <= myMaxDegree && (BsC->NbKnots() - 1) <= myNbMaxSeg) {
          if(IsConvert) {
            C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
            TolCur =  Precision::Confusion();
            return Standard_True;
          }
          else return Standard_False;
        }
      }
      if (aCurve->IsKind(STANDARD_TYPE(Geom_BezierCurve))) {
        Handle(Geom_BezierCurve) BzC =  Handle(Geom_BezierCurve)::DownCast (aCurve);
        Deg =BzC->Degree();
        Standard_Boolean IsR = (myRational && BzC->IsRational());
        if(!IsR && Deg <= myMaxDegree ) {
          if(IsConvert) {
            C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
            TolCur =  Precision::Confusion();
            return Standard_True;
          }
          else return Standard_False;
        }
      }
      Handle(Geom_Curve) aCurve1;
      Standard_Real pf =aCurve->FirstParameter(), pl =  aCurve->LastParameter();
      // 15.11.2002 PTV OCC966
      if(ShapeAnalysis_Curve::IsPeriodic(aCurve) && (First != Last))  aCurve1 = new Geom_TrimmedCurve(aCurve,First,Last);
      else if(pf < (First - Precision::PConfusion()) || 
        pl > (Last + Precision::PConfusion())) {
          Standard_Real F = Max(First,pf),
            L = Min(Last,pl);
          if(F != L)
            aCurve1 = new Geom_TrimmedCurve(aCurve,F,L);
          else aCurve1 = aCurve; 
      }
      else aCurve1 = aCurve; 
      Standard_Integer  aC = Min(ContToInteger(myContinuity3d),ContToInteger(aCurve->Continuity()));
      if(!aC)
        aC = ContToInteger(myContinuity3d);
      //aC = Min(aC,(Deg -1));
      Standard_Integer MaxSeg = myNbMaxSeg;
      Standard_Integer MaxDeg = myMaxDegree;
      //GeomAbs_Shape aCont = IntegerToGeomAbsShape(aC);
      Standard_Integer aC1 = aC;
      //Standard_Integer GMaxDegree = 15; //Geom_BSplineCurve::MaxDegree();
      for(; aC >= 0; aC--) {
        try {
          OCC_CATCH_SIGNALS
            for(Standard_Integer j = 1; j <=2 ; j++) {
              GeomAbs_Shape aCont = IntegerToGeomAbsShape(aC);
              GeomConvert_ApproxCurve anApprox(aCurve1,myTol3d,aCont,MaxSeg,MaxDeg);
              Standard_Boolean Done = anApprox.IsDone();
              C=anApprox.Curve();
              Standard_Integer Nbseg = Handle(Geom_BSplineCurve)::DownCast(C)->NbKnots() - 1;
              Standard_Integer DegC = Handle(Geom_BSplineCurve)::DownCast(C)->Degree();
              if( myDeg && ((DegC > MaxDeg) || !Done || 
                (anApprox.MaxError() >= Max(TolCur,myTol3d)))) {
                  if(MaxSeg < myParameters->GMaxSeg()) { MaxSeg = myParameters->GMaxSeg(); aC =aC1; continue;}
                  else {
#ifdef OCCT_DEBUG
                    cout << "Curve is not aproxed with continuity  "<< aCont<<endl;
#endif	      
                    if(IsConvert) {
                      C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
                      TolCur =  Precision::Confusion();
                      return Standard_True;
                    } 
                  }
              }
              if(!myDeg && ((Nbseg > myParameters->GMaxSeg()) || !Done || 
                (anApprox.MaxError() >= Max(TolCur,myTol3d)))) {
                  if(MaxDeg < myParameters->GMaxDegree()) { 
                    MaxDeg = myParameters->GMaxDegree(); aC = aC1; continue;
                  }
                  else {
#ifdef OCCT_DEBUG
                    cout << "Curve is not aproxed with continuity  "<< aCont<<endl;
#endif
                    if(IsConvert) {
                      C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
                      TolCur =  Precision::Confusion();
                      return Standard_True;
                    } 
                  }
              }
              myConvert = Standard_True;
              TolCur =  anApprox.MaxError();
              myCurve3dError = Max(myCurve3dError,anApprox.MaxError());
              return Standard_True;
            }
        }
        catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout << "Warning: GeomConvert_ApproxCurve Exception: Wrong Coefficient : Decrease continuity    ";
          anException.Print(cout); cout << endl;
#endif
          (void)anException;
          continue;
        } 
      }
      return Standard_False;
  }  
  else {
    if(IsConvert) {
      C = Handle(Geom_Curve)::DownCast(aCurve->Copy());
      TolCur =  Precision::Confusion();
      return Standard_True;
    }
    return Standard_False;
  }
}

//=======================================================================
//function : NewCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::NewCurve2d(const TopoDS_Edge& E,
                                                            const TopoDS_Face& F,
                                                            const TopoDS_Edge& NewE,
                                                            const TopoDS_Face& /*NewF*/,
                                                            Handle(Geom2d_Curve)& C,
                                                            Standard_Real& Tol)
{
  if ( ! myApproxCurve2dFlag && !myApproxSurfaceFlag)
    return Standard_False;
  Standard_Real First, Last,F1,L1;
  TopLoc_Location L,Loc1;
  Handle(Geom_Surface) aSurface = BRep_Tool::Surface(F,L);
  GeomAdaptor_Surface AdS(aSurface);
  Standard_Real TolCur = Min(AdS.UResolution(BRep_Tool::Tolerance(E)),AdS.VResolution(BRep_Tool::Tolerance(E)));
  Handle(Geom2d_Curve) aCurve = BRep_Tool::CurveOnSurface(E,F,First, Last);
  if(aCurve.IsNull()) return Standard_False;
  Handle(Geom_Curve) aCur3d = BRep_Tool::Curve(E,Loc1,F1, L1);
  //  Standard_Boolean IsConvert = (IsConvertSurface(aSurface,myMaxDegree,myNbMaxSeg) || !E.IsSame(NewE));

  Standard_Boolean IsConvert = 
    ((myApproxSurfaceFlag && IsConvertSurface(aSurface,myMaxDegree,myNbMaxSeg,myRational,myParameters)) || 
    (myApproxCurve3dFlag && IsConvertCurve3d(aCur3d,myMaxDegree,myNbMaxSeg,myRational,myParameters)));

  if(!IsConvert) {
    Handle(BRep_TEdge)& TE = *((Handle(BRep_TEdge)*)&E.TShape());
    // iterate on pcurves
    BRep_ListIteratorOfListOfCurveRepresentation itcr(TE->Curves());
    for ( ; itcr.More(); itcr.Next() ) {
      Handle(BRep_GCurve) GC = Handle(BRep_GCurve)::DownCast(itcr.Value());
      if ( GC.IsNull() || ! GC->IsCurveOnSurface() ) continue;
      Handle(Geom_Surface) aSurf = GC->Surface();
      Handle(Geom2d_Curve) aCur2d = GC->PCurve();
      if((myApproxSurfaceFlag && IsConvertSurface(aSurf,myMaxDegree,myNbMaxSeg,myRational,myParameters)) || 
        (myApproxCurve2dFlag && IsConvertCurve2d(aCur2d,myMaxDegree,myNbMaxSeg,myRational,myParameters))) {
          IsConvert = Standard_True;
          break;
      }
    }
  }
  if(! myApproxCurve2dFlag){
    if(IsConvert) {
      C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
      return Standard_True;
    }
    else
      return Standard_False;
  }
  Standard_Boolean IsOf = Standard_True;
  if(myParameters->ConvertOffsetCurv2d())  IsOf = Standard_False;
  Standard_Boolean IsConv = ConvertCurve2d(aCurve,C,IsConvert,First,Last,TolCur,IsOf);

  Tol= BRep_Tool::Tolerance(E);//TolCur;
  BRep_Builder B;
  if(!IsConv && !NewE.IsSame( E)) 
    B.Range(NewE,First,Last);
  return IsConv;
}

//=======================================================================
//function : ConvertCurve2d
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::ConvertCurve2d(const Handle(Geom2d_Curve)& aCurve,
                                                                Handle(Geom2d_Curve)& C, 
                                                                const Standard_Boolean IsConvert,
                                                                const Standard_Real First, 
                                                                const Standard_Real Last,
                                                                Standard_Real& TolCur,
                                                                const Standard_Boolean IsOf) 
{  
  //TolCur = Precision::PConfusion();
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve))) {
    Handle(Geom2d_TrimmedCurve) tmp = Handle(Geom2d_TrimmedCurve)::DownCast (aCurve);
    // Standard_Real pf =tmp->FirstParameter(), pl =  tmp->LastParameter();
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    Handle(Geom2d_Curve) ResCurve;
    if(ConvertCurve2d(BasCurve,ResCurve,IsConvert,First,Last,TolCur,IsOf)) {
      //      Standard_Real F = Max(ResCurve->FirstParameter(),First), L = Min(ResCurve->LastParameter(),Last);
      //      if(F != Last)
      //C = new Geom2d_TrimmedCurve(ResCurve,Max(First,ResCurve->FirstParameter()),Min(Last,ResCurve->LastParameter()));
      //else 
      C = ResCurve;
      return Standard_True;
    }
    else {
      if(IsConvert) {
        C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
        TolCur = Precision::PConfusion();
        return Standard_True;
      }
      else return Standard_False;
    }
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_Line)) && myParameters->ConvertCurve2d()) {
    Handle(Geom2d_Line) aLine2d = Handle(Geom2d_Line)::DownCast(aCurve);
    TColgp_Array1OfPnt2d poles(1,2);
    poles(1) = aLine2d->Value(First);
    poles(2) = aLine2d->Value(Last);
    TColStd_Array1OfReal knots(1,2);
    knots(1) = First; knots(2) = Last;
    TColStd_Array1OfInteger mults(1,2);
    mults.Init(2);
    Handle(Geom2d_BSplineCurve) res = new Geom2d_BSplineCurve(poles,knots,mults,1);
    C = res;
    return Standard_True;
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_Conic)) && myParameters->ConvertCurve2d()) {
    Handle(Geom2d_BSplineCurve) aBSpline2d;
    Handle(Geom2d_Curve) tcurve = new Geom2d_TrimmedCurve(aCurve,First,Last); //protection agains parabols ets
    Geom2dConvert_ApproxCurve approx (tcurve, myTol2d,myContinuity2d,myNbMaxSeg , 6 );
    if ( approx.HasResult() )
      aBSpline2d = approx.Curve();
    else 
      aBSpline2d = Geom2dConvert::CurveToBSplineCurve(tcurve,Convert_QuasiAngular);

    Standard_Real Shift = First - aBSpline2d->FirstParameter();
    if(Abs(Shift) > Precision::PConfusion()) {
      Standard_Integer nbKnots = aBSpline2d->NbKnots();
      TColStd_Array1OfReal newKnots(1,nbKnots);
      aBSpline2d->Knots(newKnots);
      for (Standard_Integer i = 1; i <= nbKnots; i++)
        newKnots(i)+=Shift;
      aBSpline2d->SetKnots(newKnots);
    }
    Handle(Geom2d_Curve) ResCurve;
    if(ConvertCurve2d(aBSpline2d,ResCurve,IsConvert,First,Last,TolCur,Standard_False)) {
      C = ResCurve;
      return Standard_True;
    }
    else {
      C = aBSpline2d;
      TolCur = Precision::PConfusion();
      return Standard_True;
    } 
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)) && myParameters->ConvertCurve2d()) {
    Handle(Geom2d_Curve) aBSpline2d 
      = Geom2dConvert::CurveToBSplineCurve(aCurve,Convert_QuasiAngular);
    Handle(Geom2d_Curve) ResCurve;
    if(ConvertCurve2d(aBSpline2d,ResCurve,IsConvert,First,Last,TolCur,Standard_False)) {
      C = ResCurve;
      return Standard_True;
    }
    else {
      C = aBSpline2d;
      TolCur = Precision::PConfusion();
      return Standard_True;
    } 
  }

  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)) && IsOf) {
    Handle(Geom2d_OffsetCurve) tmp = Handle(Geom2d_OffsetCurve)::DownCast (aCurve);
    Handle(Geom2d_Curve) BasCurve = tmp->BasisCurve();
    Handle(Geom2d_Curve) ResCurve;
    if(ConvertCurve2d(BasCurve,ResCurve,IsConvert,First,Last,TolCur)) {
      if(ResCurve->Continuity() != GeomAbs_C0) {
        C = new Geom2d_OffsetCurve(ResCurve,tmp->Offset());
        return Standard_True;
      }
      else if (ConvertCurve2d(aCurve,ResCurve,IsConvert,First,Last,TolCur,Standard_False))
        return Standard_True;
      else {
        if(IsConvert) {
          C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
          TolCur = Precision::PConfusion();
          return Standard_True;
        }
        else return Standard_False;

      }
    } 
    else {
      if(IsConvert) {
        C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
        TolCur = Precision::PConfusion();
        return Standard_True;
      }
      else return Standard_False;
    }
  }
  if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)) || 
      aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))  ||
      ((aCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve))) && !IsOf ))  {
      Standard_Integer Deg=1;

      if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve))) {
        Handle(Geom2d_BSplineCurve) BsC = Handle(Geom2d_BSplineCurve)::DownCast (aCurve);
        Deg =BsC->Degree();
        Standard_Boolean IsR = (myRational && BsC->IsRational());
        if(!IsR && Deg <= myMaxDegree && (BsC->NbKnots() -1) <= myNbMaxSeg) {
          if(IsConvert) {
            C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
            TolCur = Precision::PConfusion();
            return Standard_True;
          }
          else return Standard_False;
        }
      }
      if (aCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve))) {
        Handle(Geom2d_BezierCurve)BzC =  Handle(Geom2d_BezierCurve)::DownCast (aCurve);
        Deg =BzC->Degree();
        Standard_Boolean IsR = (myRational && BzC->IsRational());
        if(!IsR && Deg <= myMaxDegree) {
          if(IsConvert) {
            C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
            TolCur = Precision::PConfusion();
            return Standard_True;
          }
          else return Standard_False;
        }
      }
      Handle(Geom2d_Curve) aCurve1;
      Standard_Real pf =aCurve->FirstParameter(), pl =  aCurve->LastParameter();
      // 15.11.2002 PTV OCC966
      if(ShapeAnalysis_Curve::IsPeriodic(aCurve) && (First != Last)) aCurve1 = new Geom2d_TrimmedCurve(aCurve,First,Last);
      else if(aCurve->FirstParameter() < (First - Precision::PConfusion())  || 
        aCurve->LastParameter() > (Last + Precision::PConfusion())) {
          Standard_Real F = Max(First,pf),
            L = Min(Last,pl);
          if(F != L)
            aCurve1 = new Geom2d_TrimmedCurve(aCurve,F,L);
          else aCurve1 = aCurve;
      }
      else aCurve1 = aCurve;
      Standard_Integer aC = Min(ContToInteger(myContinuity2d),ContToInteger( aCurve->Continuity()));
      if(!aC)
        aC = ContToInteger(myContinuity2d);
      //aC = Min(aC,(Deg -1));
      Standard_Integer aC1 = aC;
      //GeomAbs_Shape aCont =IntegerToGeomAbsShape(aC);
      Standard_Integer MaxSeg = myNbMaxSeg;
      Standard_Integer MaxDeg = myMaxDegree;
      //Standard_Integer GMaxDegree = 15;//Geom2d_BSplineCurve::MaxDegree();
      for(; aC >= 0; aC--) {
        try {
          OCC_CATCH_SIGNALS
            GeomAbs_Shape aCont = IntegerToGeomAbsShape(aC);
          for(Standard_Integer j =1;j<=2 ;j++) {
            Geom2dConvert_ApproxCurve anApprox(aCurve1,myTol2d,aCont,MaxSeg,MaxDeg);
            Standard_Boolean Done = anApprox.IsDone();
            C=anApprox.Curve();
            Standard_Integer Nbseg = Handle(Geom2d_BSplineCurve)::DownCast(C)->NbKnots() -1;
            Standard_Integer DegC = Handle(Geom2d_BSplineCurve)::DownCast(C)->Degree();

            if(myDeg && ((DegC > MaxDeg)  || !Done || ( anApprox.MaxError() >= Max(myTol2d,TolCur)))) {
              if(MaxSeg < myParameters->GMaxSeg()) { MaxSeg = myParameters->GMaxSeg(); aC =aC1; continue;}
              else {
#ifdef OCCT_DEBUG
                cout << "Curve is not aproxed with continuity  "<< aCont<<endl;
#endif
                if(IsConvert) {
                  C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
                  TolCur = Precision::PConfusion();
                  return Standard_True;
                } 
              }
            }

            if(!myDeg && (( Nbseg >= MaxSeg)|| !Done || ( anApprox.MaxError() >= Max(myTol2d,TolCur)))) {
              if(MaxDeg < myParameters->GMaxDegree()) { 
                MaxDeg = myParameters->GMaxDegree(); aC =aC1; continue;
              }
              else {
#ifdef OCCT_DEBUG
                cout << "Curve is not aproxed with continuity  "<< aCont<<endl;
#endif
                if(IsConvert) {
                  C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
                  TolCur = Precision::PConfusion();
                  return Standard_True;
                } 
              }
            }
            myConvert= Standard_True; 
            TolCur = anApprox.MaxError();
            myCurve2dError = Max(myCurve2dError,anApprox.MaxError());
            return Standard_True;
          }
        }
        catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout << "Warning: Geom2dConvert_ApproxCurve Exception: Wrong Cofficient :Decrease Continuity    ";
          anException.Print(cout); cout << endl;
#endif
          (void)anException;
          continue;
        }
      }
      return Standard_False;
  }  
  else {
    if(IsConvert) {
      C = Handle(Geom2d_Curve)::DownCast(aCurve->Copy());
      TolCur = Precision::PConfusion();
      return Standard_True;
    }
    else return Standard_False;
  }
}

//=======================================================================
//function : NewPoint
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::NewPoint(const TopoDS_Vertex& V,
                                                          gp_Pnt& P,
                                                          Standard_Real& Tol)
{
  Tol = BRep_Tool::Tolerance(V);
  if(myConvert) {
    gp_Pnt p1(BRep_Tool::Pnt(V).XYZ());
    P = p1; 
    return Standard_True;
  }
  else 
    return Standard_False;
}

//=======================================================================
//function : NewParameter
//purpose  : 
//=======================================================================

Standard_Boolean ShapeCustom_BSplineRestriction::NewParameter(const TopoDS_Vertex& /*V*/,
                                                              const TopoDS_Edge& /*E*/,
                                                              Standard_Real& /*P*/,
                                                              Standard_Real& /*Tol*/)
{
  return Standard_False;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape ShapeCustom_BSplineRestriction::Continuity(const TopoDS_Edge& E,
                                                         const TopoDS_Face& F1,
                                                         const TopoDS_Face& F2,
                                                         const TopoDS_Edge& /*NewE*/,
                                                         const TopoDS_Face& /*NewF1*/,
                                                         const TopoDS_Face& /*NewF2*/)
{
  return BRep_Tool::Continuity(E,F1,F2);
}

//=======================================================================
//function : MaxErrors
//purpose  : 
//=======================================================================

Standard_Real ShapeCustom_BSplineRestriction::MaxErrors(Standard_Real& aCurve3dErr,Standard_Real& aCurve2dErr) const
{
  aCurve3dErr = myCurve3dError;
  aCurve2dErr = myCurve2dError;
  return mySurfaceError;
}

//=======================================================================
//function : NbOfSpan
//purpose  : 
//======================================================================

Standard_Integer ShapeCustom_BSplineRestriction::NbOfSpan() const
{
  return myNbOfSpan;
}

