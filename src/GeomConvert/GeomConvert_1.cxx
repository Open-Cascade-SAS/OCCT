// Copyright (c) 1995-1999 Matra Datavision
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

//Jean-Claude Vauthier Novembre 1991
//Passage sur C1 Aout 1992 et ajout transformation Bezier->BSpline
//Modif JCV correction bug le 02/08/1993

#include <BSplCLib.hxx>
#include <Convert_ConeToBSplineSurface.hxx>
#include <Convert_CylinderToBSplineSurface.hxx>
#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
#include <Convert_SphereToBSplineSurface.hxx>
#include <Convert_TorusToBSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxSurface.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array2OfInteger.hxx>
#include <TColStd_Array2OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

typedef Geom_Surface                          Surface;
typedef Geom_BSplineSurface                   BSplineSurface;
typedef TColStd_Array1OfReal                 Array1OfReal;
typedef TColStd_Array2OfReal                 Array2OfReal;
typedef TColStd_Array1OfInteger              Array1OfInteger;
typedef TColStd_Array2OfInteger              Array2OfInteger;
typedef TColgp_Array2OfPnt                          Array2OfPnt;
typedef TColgp_Array1OfPnt                          Array1OfPnt;
typedef gp_Pnt Pnt;

//=======================================================================
//function : BSplineSurfaceBuilder
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) BSplineSurfaceBuilder 
  (const Convert_ElementarySurfaceToBSplineSurface& Convert) 
{
  Handle(Geom_BSplineSurface) TheSurface;
  Standard_Integer UDegree  = Convert.UDegree ();
  Standard_Integer VDegree  = Convert.VDegree ();
  Standard_Integer NbUPoles = Convert.NbUPoles();
  Standard_Integer NbVPoles = Convert.NbVPoles();
  Standard_Integer NbUKnots = Convert.NbUKnots();
  Standard_Integer NbVKnots = Convert.NbVKnots();
  Array2OfPnt     Poles   (1, NbUPoles, 1, NbVPoles);
  Array2OfReal    Weights (1, NbUPoles, 1, NbVPoles);
  Array1OfReal    UKnots  (1, NbUKnots);
  Array1OfReal    VKnots  (1, NbVKnots);
  Array1OfInteger UMults  (1, NbUKnots);
  Array1OfInteger VMults  (1, NbVKnots);
  Standard_Integer i, j;
  for (j = 1; j <= NbVPoles; j++) {
    for (i = 1; i <= NbUPoles; i++) {
      Poles   (i, j) = Convert.Pole   (i, j);
      Weights (i, j) = Convert.Weight (i, j);         
    }
  }
  for (i = 1; i <= NbUKnots; i++) {
    UKnots (i) = Convert.UKnot (i);
    UMults (i) = Convert.UMultiplicity (i);
  }
  for (i = 1; i <= NbVKnots; i++) {
    VKnots (i) = Convert.VKnot (i);
    VMults (i) = Convert.VMultiplicity (i);
  }
  TheSurface = new BSplineSurface (Poles, Weights, UKnots, VKnots, 
    UMults, VMults, UDegree, VDegree,
    Convert.IsUPeriodic(),
    Convert.IsVPeriodic());
  return TheSurface;
}

//=======================================================================
//function : SplitBSplineSurface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomConvert::SplitBSplineSurface 
  (const Handle(Geom_BSplineSurface)& S,
  const Standard_Integer FromUK1, 
  const Standard_Integer ToUK2,
  const Standard_Integer FromVK1, 
  const Standard_Integer ToVK2,
  const Standard_Boolean SameUOrientation, 
  const Standard_Boolean SameVOrientation ) 
{
  Standard_Integer FirstU = S->FirstUKnotIndex ();
  Standard_Integer FirstV = S->FirstVKnotIndex ();
  Standard_Integer LastU  = S->LastUKnotIndex  ();
  Standard_Integer LastV  = S->LastVKnotIndex  ();
  if (FromUK1 == ToUK2 || FromVK1 == ToVK2)   throw Standard_DomainError();
  Standard_Integer FirstUK = Min (FromUK1, ToUK2);
  Standard_Integer LastUK  = Max (FromUK1, ToUK2);
  Standard_Integer FirstVK = Min (FromVK1, ToVK2);
  Standard_Integer LastVK  = Max (FromVK1, ToVK2);
  if (FirstUK < FirstU || LastUK > LastU || 
    FirstVK < FirstV || LastVK > LastV) { throw Standard_DomainError(); }

  Handle(Geom_BSplineSurface) S1= Handle(Geom_BSplineSurface)::DownCast(S->Copy());

  S1->Segment(S1->UKnot(FirstUK),S1->UKnot(LastUK),
    S1->VKnot(FirstVK),S1->VKnot(LastVK));

  if (S->IsUPeriodic()) {
    if (!SameUOrientation) S1->UReverse();
  }
  else {
    if (FromUK1 > ToUK2)   S1->UReverse();
  }
  if (S->IsVPeriodic()) {
    if (!SameVOrientation) S1->VReverse();
  }
  else {
    if (FromVK1 > ToVK2)   S1->VReverse();
  }
  return S1;
}


//=======================================================================
//function : SplitBSplineSurface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomConvert::SplitBSplineSurface 
  (const Handle(Geom_BSplineSurface)& S,
  const Standard_Integer FromK1,
  const Standard_Integer ToK2,
  const Standard_Boolean USplit, 
  const Standard_Boolean SameOrientation )
{
  if (FromK1 == ToK2)   throw Standard_DomainError();


  Handle(Geom_BSplineSurface) S1 = Handle(Geom_BSplineSurface)::DownCast(S->Copy());

  if (USplit) {

    Standard_Integer FirstU = S->FirstUKnotIndex ();
    Standard_Integer LastU  = S->LastUKnotIndex  ();
    Standard_Integer FirstUK = Min (FromK1, ToK2);
    Standard_Integer LastUK  = Max (FromK1, ToK2);
    if (FirstUK < FirstU || LastUK > LastU)  throw Standard_DomainError();

    S1->Segment( S1->UKnot(FirstUK), 
      S1->UKnot(LastUK),
      S1->VKnot(S1->FirstVKnotIndex()),
      S1->VKnot(S1->LastVKnotIndex()));

    if (S->IsUPeriodic()) {
      if (!SameOrientation) S1->UReverse();
    }
    else {
      if (FromK1 > ToK2)    S1->UReverse();
    }

  }
  else {

    Standard_Integer FirstV = S->FirstVKnotIndex ();
    Standard_Integer LastV  = S->LastVKnotIndex  ();
    Standard_Integer FirstVK = Min (FromK1, ToK2);
    Standard_Integer LastVK  = Max (FromK1, ToK2);
    if (FirstVK < FirstV || LastVK > LastV)  throw Standard_DomainError();

    S1->Segment( S1->UKnot(S1->FirstUKnotIndex()), 
      S1->UKnot(S1->LastUKnotIndex()),
      S1->VKnot(FirstVK),
      S1->VKnot(LastVK));


    if (S->IsVPeriodic()) {
      if (!SameOrientation) S1->VReverse();
    }
    else {
      if (FromK1 > ToK2)    S1->VReverse();
    }
  }
  return S1;
}


//=======================================================================
//function : SplitBSplineSurface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomConvert::SplitBSplineSurface 
  (const Handle(Geom_BSplineSurface)& S, 
  const Standard_Real FromU1, 
  const Standard_Real ToU2,
  const Standard_Real FromV1, 
  const Standard_Real ToV2,
  //   const Standard_Real ParametricTolerance,
  const Standard_Real ,
  const Standard_Boolean SameUOrientation, 
  const Standard_Boolean SameVOrientation )
{
  Standard_Real FirstU = Min( FromU1, ToU2);
  Standard_Real LastU  = Max( FromU1, ToU2);
  Standard_Real FirstV = Min( FromV1, ToV2);
  Standard_Real LastV  = Max( FromV1, ToV2);

  Handle (Geom_BSplineSurface) NewSurface 
    = Handle(Geom_BSplineSurface)::DownCast(S->Copy());

  NewSurface->Segment(FirstU, LastU, FirstV, LastV);

  if (S->IsUPeriodic()) { 
    if (!SameUOrientation)  NewSurface->UReverse(); 
  }
  else { 
    if (FromU1 > ToU2)    NewSurface->UReverse(); 
  }
  if (S->IsVPeriodic()) {
    if (!SameVOrientation)  NewSurface->VReverse();
  }
  else {
    if (FromV1 > ToV2)     NewSurface->VReverse();
  }
  return NewSurface;
}


//=======================================================================
//function : SplitBSplineSurface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomConvert::SplitBSplineSurface 
  (const Handle(Geom_BSplineSurface)& S, 
  const Standard_Real                    FromParam1, 
  const Standard_Real                    ToParam2,
  const Standard_Boolean                 USplit, 
  const Standard_Real                    ParametricTolerance, 
  const Standard_Boolean                 SameOrientation  )
{
  if (Abs (FromParam1 - ToParam2) <= Abs(ParametricTolerance)) {
    throw Standard_DomainError();
  }
  Handle(Geom_BSplineSurface) NewSurface
    = Handle(Geom_BSplineSurface)::DownCast(S->Copy());

  if (USplit) {
    Standard_Real FirstU = Min( FromParam1, ToParam2);
    Standard_Real LastU  = Max( FromParam1, ToParam2);
    Standard_Real FirstV = S->VKnot(S->FirstVKnotIndex());
    Standard_Real LastV  = S->VKnot(S->LastVKnotIndex());

    NewSurface->Segment(FirstU, LastU, FirstV, LastV);

    if (S->IsUPeriodic()) { 
      if (!SameOrientation)  NewSurface->UReverse(); 
    }
    else { 
      if (FromParam1 > ToParam2)    NewSurface->UReverse(); 
    }

  }
  else {
    Standard_Real FirstU = S->UKnot(S->FirstUKnotIndex());
    Standard_Real LastU  = S->UKnot(S->LastUKnotIndex());
    Standard_Real FirstV = Min( FromParam1, ToParam2);
    Standard_Real LastV  = Max( FromParam1, ToParam2);

    NewSurface->Segment(FirstU, LastU, FirstV, LastV);

    if (S->IsUPeriodic()) { 
      if (!SameOrientation)  NewSurface->UReverse(); 
    }
    else { 
      if (FromParam1 > ToParam2)    NewSurface->UReverse(); 
    }

  }
  return NewSurface;
}


//=======================================================================
//function : SurfaceToBSplineSurface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomConvert::SurfaceToBSplineSurface
  (const Handle(Geom_Surface)& Sr) 
{
  Standard_Real U1, U2, V1, V2;
  Sr->Bounds (U1, U2, V1, V2);
  Standard_Real UFirst = Min (U1, U2);
  Standard_Real ULast  = Max (U1, U2);
  Standard_Real VFirst = Min (V1, V2);
  Standard_Real VLast  = Max (V1, V2);

  //If the surface Sr is infinite stop the computation
  if (Precision::IsNegativeInfinite(UFirst) || 
    Precision::IsPositiveInfinite(ULast)  || 
    Precision::IsNegativeInfinite(VFirst) || 
    Precision::IsPositiveInfinite(VLast) )   {
      throw Standard_DomainError ("GeomConvert::SurfaceToBSplineSurface() - infinite surface");
  }

  Handle(Geom_BSplineSurface) TheSurface;
  Handle(Geom_Surface) S; 
  Handle(Geom_OffsetSurface) OffsetSur;
  if (Sr->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    OffsetSur = Handle(Geom_OffsetSurface)::DownCast (Sr);
    S = OffsetSur->Surface();
    if (!S.IsNull()) { // Convert the equivalent surface.
      return SurfaceToBSplineSurface(S);
    }
  }
  S = Sr;

  if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) Strim = 
      Handle(Geom_RectangularTrimmedSurface)::DownCast(S);

    Handle(Geom_Surface) Surf = Strim->BasisSurface();
    UFirst = U1;  ULast = U2; VFirst = V1; VLast = V2;
    if (Surf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
      Handle(Geom_OffsetSurface) OffsetSurBasis =
        Handle(Geom_OffsetSurface)::DownCast(Surf);

      S = OffsetSurBasis->Surface();
      if (!S.IsNull()) {
        Surf = S;
      } 
      else S = Surf;
    }

    if  (Surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
      Handle(Geom_RectangularTrimmedSurface) aStrim = new 
        (Geom_RectangularTrimmedSurface) (Surf, 
        UFirst, ULast, 
        VFirst, VLast);
      return SurfaceToBSplineSurface(aStrim);
    }

    if (Surf->IsKind(STANDARD_TYPE(Geom_Plane))) {
      TColgp_Array2OfPnt Poles (1, 2, 1, 2);
      Poles (1, 1) = Strim->Value (U1, V1);
      Poles (1, 2) = Strim->Value (U1, V2);
      Poles (2, 1) = Strim->Value (U2, V1);
      Poles (2, 2) = Strim->Value (U2, V2);
      TColStd_Array1OfReal UKnots (1, 2);
      TColStd_Array1OfReal VKnots (1, 2);
      TColStd_Array1OfInteger UMults (1, 2);
      TColStd_Array1OfInteger VMults (1, 2);
      UKnots (1) = U1;
      UKnots (2) = U2;
      VKnots (1) = V1;
      VKnots (2) = V2;
      UMults (1) = 2;
      UMults (2) = 2;
      VMults (1) = 2;
      VMults (2) = 2;
      Standard_Integer UDegree = 1;
      Standard_Integer VDegree = 1;
      TheSurface = new Geom_BSplineSurface (Poles, UKnots, VKnots, UMults,
        VMults, UDegree, VDegree);
    }
    else if (Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
      Handle(Geom_CylindricalSurface) TheElSurf= 
        Handle(Geom_CylindricalSurface)::DownCast(Surf);

      gp_Cylinder Cyl = TheElSurf->Cylinder();
      if (Strim->IsUClosed()) {
        Convert_CylinderToBSplineSurface Convert (Cyl, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
      else {
        Convert_CylinderToBSplineSurface 
          Conv (Cyl, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Conv);
      }
    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
      Handle(Geom_ConicalSurface) TheElSurf = 
        Handle(Geom_ConicalSurface)::DownCast(Surf);
      gp_Cone Co = TheElSurf->Cone();
      if (Strim->IsUClosed()) {
        Convert_ConeToBSplineSurface Convert (Co, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
      else {
        Convert_ConeToBSplineSurface 
          Convert (Co, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
      Handle(Geom_SphericalSurface) TheElSurf = 
        Handle(Geom_SphericalSurface)::DownCast(Surf);
      gp_Sphere Sph = TheElSurf->Sphere();
      //OCC217
      if (Strim->IsUClosed()) {
        //if (Strim->IsVClosed()) {
        //Convert_SphereToBSplineSurface Convert (Sph, UFirst, ULast);
        Convert_SphereToBSplineSurface Convert (Sph, VFirst, VLast, Standard_False);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
      else {
        Convert_SphereToBSplineSurface 
          Convert (Sph, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
      Handle(Geom_ToroidalSurface) TheElSurf = 
        Handle(Geom_ToroidalSurface)::DownCast(Surf);

      gp_Torus Tr = TheElSurf->Torus();
      if (Strim->IsUClosed()) { 
        Convert_TorusToBSplineSurface Convert (Tr, VFirst, VLast, 
          Standard_False);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
      else if (Strim->IsVClosed()) {
        Convert_TorusToBSplineSurface Convert (Tr, UFirst, ULast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
      else {
        Convert_TorusToBSplineSurface 
          Convert (Tr, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder (Convert);
      }
    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {
      Handle(Geom_SurfaceOfRevolution) Revol = 
        Handle(Geom_SurfaceOfRevolution)::DownCast(Surf);

      Handle(Geom_Curve) Meridian = Revol->BasisCurve();
      Handle(Geom_BSplineCurve) C; 
      if (Strim->IsVClosed()) {
        C =  GeomConvert::CurveToBSplineCurve (Meridian);
      }
      else {
        Handle(Geom_TrimmedCurve) CT = 
          new Geom_TrimmedCurve( Meridian, VFirst, VLast);
        C =  GeomConvert::CurveToBSplineCurve (CT);
      }
      Standard_Integer NbUPoles, NbUKnots;
      Standard_Integer NbVPoles, NbVKnots;
      Standard_Boolean periodic = Standard_False;

      // Poles of meridian = Vpoles
      NbVPoles = C->NbPoles();
      TColgp_Array1OfPnt Poles(1, NbVPoles);
      C->Poles(Poles);
      TColStd_Array1OfReal Weights( 1, NbVPoles);
      Weights.Init(1.);
      if ( C->IsRational()) C->Weights(Weights);

      Standard_Integer nbUSpans;
      Standard_Real AlfaU;
      if (Strim->IsUPeriodic()) {
        NbUKnots = 4;
        nbUSpans = 3;
        AlfaU    = M_PI / 3.;
        NbUPoles = 6;
        periodic = Standard_True;
      }
      else {
        // Nombre de spans : ouverture maximale = 150 degres ( = PI / 1.2 rds)
        nbUSpans = 
          (Standard_Integer)IntegerPart( 1.2 * (ULast - UFirst) / M_PI) + 1;
        AlfaU = (ULast - UFirst) / ( nbUSpans * 2);
        NbUPoles = 2 * nbUSpans + 1;
        NbUKnots = nbUSpans + 1;
      }
      // Compute Knots and Mults
      TColStd_Array1OfReal    UKnots(1, NbUKnots);
      TColStd_Array1OfInteger UMults( 1, NbUKnots);
      Standard_Integer i,j;
      for ( i = 1; i <= NbUKnots; i++) {
        UKnots(i) = UFirst + (i-1) * 2 * AlfaU;
        UMults(i) = 2;
      }
      if (!periodic) {
        UMults(1)++; UMults(NbUKnots)++;
      }
      NbVKnots = C->NbKnots();
      TColStd_Array1OfReal    VKnots(1, NbVKnots);
      TColStd_Array1OfInteger VMults(1, NbVKnots);
      C->Knots(VKnots);
      C->Multiplicities(VMults);

      // Compute the poles.
      TColgp_Array2OfPnt   NewPoles  ( 1, NbUPoles, 1, NbVPoles);
      TColStd_Array2OfReal NewWeights( 1, NbUPoles, 1, NbVPoles);
      gp_Trsf Trsf;

      for ( i = 1; i<= NbUPoles; i+=2) {
        Trsf.SetRotation( Revol->Axis(), UFirst + (i-1)*AlfaU);
        for ( j = 1; j <= NbVPoles; j++) {
          NewPoles(i,j) = Poles(j).Transformed(Trsf);
          NewWeights(i,j) = Weights(j);
        }
      }
      gp_GTrsf Aff;
      Aff.SetAffinity( Revol->Axis(), 1/Cos(AlfaU));
      gp_XYZ coord;
      for ( j= 1; j<= NbVPoles; j++) {
        coord = Poles(j).XYZ();
        Aff.Transforms(coord);
        Poles(j).SetXYZ(coord);
      }
      for ( i = 2; i<= NbUPoles; i+=2) {
        Trsf.SetRotation( Revol->Axis(), UFirst + (i-1)*AlfaU);
        for ( j = 1; j <= NbVPoles; j++) {
          NewPoles(i,j) = Poles(j).Transformed(Trsf);
          NewWeights(i,j) = Weights(j) * Cos(AlfaU);
        }
      }

      TheSurface = new Geom_BSplineSurface(NewPoles, NewWeights, 
        UKnots, VKnots, 
        UMults, VMults,
        2 , C->Degree(),
        periodic, C->IsPeriodic());


    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) {
      Handle(Geom_SurfaceOfLinearExtrusion) Extru = 
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(Surf);

      Handle(Geom_Curve) Meridian = Extru->BasisCurve();
      Handle(Geom_BSplineCurve) C; 
      if (Strim->IsUClosed()) {
        C =  GeomConvert::CurveToBSplineCurve (Meridian);
      }
      else {
        Handle(Geom_TrimmedCurve) CT = 
          new Geom_TrimmedCurve( Meridian, UFirst, ULast);
        C =  GeomConvert::CurveToBSplineCurve (CT);
      }
      TColgp_Array2OfPnt      Poles  ( 1, C->NbPoles(), 1, 2);
      TColStd_Array2OfReal    Weights( 1, C->NbPoles(), 1, 2);
      TColStd_Array1OfReal    UKnots ( 1, C->NbKnots());
      C->Knots(UKnots);
      TColStd_Array1OfInteger UMults ( 1, C->NbKnots());
      C->Multiplicities(UMults);
      TColStd_Array1OfReal    VKnots ( 1, 2);
      VKnots(1) = VFirst; 
      VKnots(2) = VLast;
      TColStd_Array1OfInteger VMults ( 1, 2);
      VMults.Init(2);

      gp_Vec D( Extru->Direction());
      gp_Vec DV1 = VFirst * D;
      gp_Vec DV2 = VLast  * D;
      for (Standard_Integer i = 1; i <= C->NbPoles(); i++) {
        Poles(i,1) = C->Pole(i).Translated(DV1);
        Poles(i,2) = C->Pole(i).Translated(DV2);
        Weights(i,1) = Weights(i,2) = C->Weight(i);
      } 
      TheSurface = new Geom_BSplineSurface(Poles, Weights, UKnots, VKnots, 
        UMults, VMults,
        C->Degree(), 1,
        C->IsPeriodic(), Standard_False);
    }


    else if (Surf->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {

      Handle(Geom_BezierSurface) SBez = 
        Handle(Geom_BezierSurface)::DownCast(Surf->Copy());

      SBez->Segment (U1, U2, V1, V2);
      Standard_Integer NbUPoles = SBez->NbUPoles();
      Standard_Integer NbVPoles = SBez->NbVPoles();
      Standard_Integer UDegree  = SBez->UDegree();
      Standard_Integer VDegree  = SBez->VDegree();
      TColgp_Array2OfPnt      Poles    (1, NbUPoles, 1, NbVPoles);
      TColStd_Array1OfReal    UKnots   (1, 2);
      TColStd_Array1OfInteger UMults   (1, 2);
      TColStd_Array1OfReal    VKnots   (1, 2);
      TColStd_Array1OfInteger VMults   (1, 2);
      UKnots (1) = 0.0;
      UKnots (2) = 1.0;
      UMults (1) = UDegree + 1;
      UMults (2) = UDegree + 1;
      VKnots (1) = 0.0;
      VKnots (2) = 1.0;
      VMults (1) = VDegree + 1;
      VMults (2) = VDegree + 1;
      SBez->Poles (Poles);
      if (SBez->IsURational() || SBez->IsVRational()) {    
        TColStd_Array2OfReal Weights (1, NbUPoles, 1, NbVPoles);
        SBez->Weights (Weights);
        TheSurface = new Geom_BSplineSurface (Poles, Weights, UKnots, VKnots,
          UMults, VMults,
          UDegree, VDegree);
      }
      else {
        TheSurface = new Geom_BSplineSurface (Poles, UKnots, VKnots, 
          UMults, VMults,
          UDegree, VDegree);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
      Handle(Geom_BSplineSurface) BS = 
        Handle(Geom_BSplineSurface)::DownCast(Surf->Copy());
      Standard_Real umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      if (!BS->IsUPeriodic()) {
        if (U1 < umin)
          U1 = umin;
        if (U2 > umax) 
          U2 = umax;
      }

      if (!BS->IsVPeriodic()) {
        if (V1 < vmin)
          V1 = vmin;
        if (V2 > vmax)
          V2 = vmax;
      }
      if (BS->IsUPeriodic() || BS->IsVPeriodic())
        BS->CheckAndSegment (U1, U2, V1, V2);
      else
        BS->Segment (U1, U2, V1, V2);
      TheSurface = BS;
    }

    else {
      Standard_Real Tol3d=1.e-4;
      Standard_Integer MaxDegree =14, MaxSeg;
      GeomAbs_Shape cont;
      GeomAdaptor_Surface AS(Sr);
      if (AS.NbUIntervals(GeomAbs_C2) > 1 || AS.NbVIntervals(GeomAbs_C2) > 1 ) 
        cont=GeomAbs_C1;
      else
        cont=GeomAbs_C2;
      MaxSeg = 4*(AS.NbUIntervals(GeomAbs_CN)+1)*(AS.NbVIntervals(GeomAbs_CN)+1); 
      GeomConvert_ApproxSurface BSpS(Sr, Tol3d, cont, cont,
        MaxDegree, MaxDegree, MaxSeg, 1);
      TheSurface = BSpS.Surface();
    }
  } // Fin du cas Rectangular::TrimmedSurface

  else { 

    if (S->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
      Handle(Geom_SphericalSurface) TheElSurf = 
        Handle(Geom_SphericalSurface)::DownCast(S);

      gp_Sphere Sph = TheElSurf->Sphere();
      Convert_SphereToBSplineSurface Convert(Sph);
      TheSurface = BSplineSurfaceBuilder(Convert);
    }


    else if (S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
      Handle(Geom_ToroidalSurface) TheElSurf = 
        Handle(Geom_ToroidalSurface)::DownCast(S);

      gp_Torus Tr = TheElSurf->Torus();
      Convert_TorusToBSplineSurface Convert(Tr);
      TheSurface = BSplineSurfaceBuilder(Convert);
    }


    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution))) {

      Handle(Geom_SurfaceOfRevolution) Revol = 
        Handle(Geom_SurfaceOfRevolution)::DownCast(S);

      Handle(Geom_Curve) Meridian = Revol->BasisCurve();
      Handle(Geom_BSplineCurve) C 
        = GeomConvert::CurveToBSplineCurve (Meridian);

      Standard_Integer NbUPoles, NbUKnots;
      Standard_Integer NbVPoles, NbVKnots;
      Standard_Boolean periodic = Standard_True;

      // Poles of meridian = Vpoles
      NbVPoles = C->NbPoles();
      TColgp_Array1OfPnt Poles(1, NbVPoles);
      C->Poles(Poles);
      TColStd_Array1OfReal Weights( 1, NbVPoles);
      Weights.Init(1.);
      if ( C->IsRational()) C->Weights(Weights);

      Standard_Real AlfaU;
      NbUKnots = 4;
      AlfaU    = M_PI / 3.;
      NbUPoles = 6;

      // Compute Knots and Mults
      TColStd_Array1OfReal    UKnots(1, NbUKnots);
      TColStd_Array1OfInteger UMults( 1, NbUKnots);
      Standard_Integer i,j;
      for ( i = 1; i <= NbUKnots; i++) {
        UKnots(i) = UFirst + (i-1) * 2 * AlfaU;
        UMults(i) = 2;
      }
      NbVKnots = C->NbKnots();
      TColStd_Array1OfReal    VKnots(1, NbVKnots);
      TColStd_Array1OfInteger VMults(1, NbVKnots);
      C->Knots(VKnots);
      C->Multiplicities(VMults);

      // Compute the poles.
      TColgp_Array2OfPnt   NewPoles  ( 1, NbUPoles, 1, NbVPoles);
      TColStd_Array2OfReal NewWeights( 1, NbUPoles, 1, NbVPoles);
      gp_Trsf Trsf;

      for ( i = 1; i<= NbUPoles; i+=2) {
        Trsf.SetRotation( Revol->Axis(), UFirst + (i-1)*AlfaU);
        for ( j = 1; j <= NbVPoles; j++) {
          NewPoles(i,j) = Poles(j).Transformed(Trsf);
          NewWeights(i,j) = Weights(j);
        }
      }
      gp_GTrsf Aff;
      Aff.SetAffinity( Revol->Axis(), 1/Cos(AlfaU));
      gp_XYZ coord;
      for ( j= 1; j<= NbVPoles; j++) {
        coord = Poles(j).XYZ();
        Aff.Transforms(coord);
        Poles(j).SetXYZ(coord);
      }
      for ( i = 2; i<= NbUPoles; i+=2) {
        Trsf.SetRotation( Revol->Axis(), UFirst + (i-1)*AlfaU);
        for ( j = 1; j <= NbVPoles; j++) {
          NewPoles(i,j) = Poles(j).Transformed(Trsf);
          NewWeights(i,j) = Weights(j) * Cos(AlfaU);
        }
      }

      TheSurface = new Geom_BSplineSurface(NewPoles, NewWeights, 
        UKnots, VKnots, 
        UMults, VMults,
        2 , C->Degree(),
        periodic, C->IsPeriodic());
    }


    else if (S->IsKind(STANDARD_TYPE(Geom_BezierSurface))) {

      Handle(Geom_BezierSurface) SBez = 
        Handle(Geom_BezierSurface)::DownCast(S);

      Standard_Integer NbUPoles = SBez->NbUPoles();
      Standard_Integer NbVPoles = SBez->NbVPoles();
      Standard_Integer UDegree  = SBez->UDegree();
      Standard_Integer VDegree  = SBez->VDegree();
      TColgp_Array2OfPnt      Poles (1, NbUPoles, 1, NbVPoles);
      TColStd_Array1OfReal    UKnots(1, 2);
      TColStd_Array1OfInteger UMults(1, 2);
      TColStd_Array1OfReal    VKnots(1, 2);
      TColStd_Array1OfInteger VMults(1, 2);
      UKnots (1) = 0.0;
      UKnots (2) = 1.0;
      UMults (1) = UDegree + 1;
      UMults (2) = UDegree + 1;
      VKnots (1) = 0.0;
      VKnots (2) = 1.0;
      VMults (1) = VDegree + 1;
      VMults (2) = VDegree + 1;
      SBez->Poles (Poles);
      if (SBez->IsURational() || SBez->IsVRational()) {    
        TColStd_Array2OfReal    Weights (1, NbUPoles, 1, NbVPoles);
        SBez->Weights (Weights);
        TheSurface = new Geom_BSplineSurface (Poles, Weights, UKnots, VKnots,
          UMults, VMults,
          UDegree, VDegree);
      }
      else {
        TheSurface = new Geom_BSplineSurface (Poles, UKnots, VKnots, 
          UMults, VMults,
          UDegree, VDegree);
      }
    }

    else if (S->IsKind(STANDARD_TYPE(Geom_BSplineSurface))) {
      TheSurface = Handle(Geom_BSplineSurface)::DownCast(S->Copy()); //Just a copy
    }

    else { // In other cases => Approx
      Standard_Real Tol3d=1.e-4;
      Standard_Integer MaxDegree = 14, MaxSeg;
      GeomAbs_Shape ucont = GeomAbs_C0, vcont = GeomAbs_C0;
      GeomAdaptor_Surface AS(Sr);
      //
      if (Sr->IsCNu(2)) 
      {
        ucont=GeomAbs_C2;
      }
      else if(Sr->IsCNu(1)) 
      {
        ucont=GeomAbs_C1;
      }
      //
      if (Sr->IsCNv(2)) 
      {
        vcont=GeomAbs_C2;
      }
      else if(Sr->IsCNv(1)) 
      {
        vcont=GeomAbs_C1;
      }
      //
      MaxSeg = 4*(AS.NbUIntervals(GeomAbs_CN)+1)*(AS.NbVIntervals(GeomAbs_CN)+1);      
      GeomConvert_ApproxSurface BSpS(Sr, Tol3d, ucont, vcont,
                                     MaxDegree, MaxDegree, MaxSeg, 1);
      TheSurface = BSpS.Surface();
    }
  } // Fin du cas direct
  return TheSurface;
}



