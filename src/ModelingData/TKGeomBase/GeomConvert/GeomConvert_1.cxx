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

#include <GeomConvert.hxx>

#include <Convert_ConeToBSplineSurface.hxx>
#include <Convert_CylinderToBSplineSurface.hxx>
#include <Convert_ElementarySurfaceToBSplineSurface.hxx>
#include <Convert_SphereToBSplineSurface.hxx>
#include <Convert_TorusToBSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineCurve.hxx>
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
#include <GeomConvert_ApproxSurface.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

typedef Geom_Surface        Surface;
typedef Geom_BSplineSurface BSplineSurface;

//=================================================================================================

static occ::handle<Geom_BSplineSurface> BSplineSurfaceBuilder(
  const Convert_ElementarySurfaceToBSplineSurface& Convert)
{
  occ::handle<Geom_BSplineSurface> TheSurface;
  TheSurface = new BSplineSurface(Convert.Poles(),
                                  Convert.Weights(),
                                  Convert.UKnots(),
                                  Convert.VKnots(),
                                  Convert.UMultiplicities(),
                                  Convert.VMultiplicities(),
                                  Convert.UDegree(),
                                  Convert.VDegree(),
                                  Convert.IsUPeriodic(),
                                  Convert.IsVPeriodic());
  return TheSurface;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert::SplitBSplineSurface(
  const occ::handle<Geom_BSplineSurface>& S,
  const int                               FromUK1,
  const int                               ToUK2,
  const int                               FromVK1,
  const int                               ToVK2,
  const bool                              SameUOrientation,
  const bool                              SameVOrientation)
{
  int FirstU = S->FirstUKnotIndex();
  int FirstV = S->FirstVKnotIndex();
  int LastU  = S->LastUKnotIndex();
  int LastV  = S->LastVKnotIndex();
  if (FromUK1 == ToUK2 || FromVK1 == ToVK2)
    throw Standard_DomainError();
  int FirstUK = std::min(FromUK1, ToUK2);
  int LastUK  = std::max(FromUK1, ToUK2);
  int FirstVK = std::min(FromVK1, ToVK2);
  int LastVK  = std::max(FromVK1, ToVK2);
  if (FirstUK < FirstU || LastUK > LastU || FirstVK < FirstV || LastVK > LastV)
  {
    throw Standard_DomainError();
  }

  occ::handle<Geom_BSplineSurface> S1 = occ::down_cast<Geom_BSplineSurface>(S->Copy());

  S1->Segment(S1->UKnot(FirstUK), S1->UKnot(LastUK), S1->VKnot(FirstVK), S1->VKnot(LastVK));

  if (S->IsUPeriodic())
  {
    if (!SameUOrientation)
      S1->UReverse();
  }
  else
  {
    if (FromUK1 > ToUK2)
      S1->UReverse();
  }
  if (S->IsVPeriodic())
  {
    if (!SameVOrientation)
      S1->VReverse();
  }
  else
  {
    if (FromVK1 > ToVK2)
      S1->VReverse();
  }
  return S1;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert::SplitBSplineSurface(
  const occ::handle<Geom_BSplineSurface>& S,
  const int                               FromK1,
  const int                               ToK2,
  const bool                              USplit,
  const bool                              SameOrientation)
{
  if (FromK1 == ToK2)
    throw Standard_DomainError();

  occ::handle<Geom_BSplineSurface> S1 = occ::down_cast<Geom_BSplineSurface>(S->Copy());

  if (USplit)
  {

    int FirstU  = S->FirstUKnotIndex();
    int LastU   = S->LastUKnotIndex();
    int FirstUK = std::min(FromK1, ToK2);
    int LastUK  = std::max(FromK1, ToK2);
    if (FirstUK < FirstU || LastUK > LastU)
      throw Standard_DomainError();

    S1->Segment(S1->UKnot(FirstUK),
                S1->UKnot(LastUK),
                S1->VKnot(S1->FirstVKnotIndex()),
                S1->VKnot(S1->LastVKnotIndex()));

    if (S->IsUPeriodic())
    {
      if (!SameOrientation)
        S1->UReverse();
    }
    else
    {
      if (FromK1 > ToK2)
        S1->UReverse();
    }
  }
  else
  {

    int FirstV  = S->FirstVKnotIndex();
    int LastV   = S->LastVKnotIndex();
    int FirstVK = std::min(FromK1, ToK2);
    int LastVK  = std::max(FromK1, ToK2);
    if (FirstVK < FirstV || LastVK > LastV)
      throw Standard_DomainError();

    S1->Segment(S1->UKnot(S1->FirstUKnotIndex()),
                S1->UKnot(S1->LastUKnotIndex()),
                S1->VKnot(FirstVK),
                S1->VKnot(LastVK));

    if (S->IsVPeriodic())
    {
      if (!SameOrientation)
        S1->VReverse();
    }
    else
    {
      if (FromK1 > ToK2)
        S1->VReverse();
    }
  }
  return S1;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert::SplitBSplineSurface(
  const occ::handle<Geom_BSplineSurface>& S,
  const double                            FromU1,
  const double                            ToU2,
  const double                            FromV1,
  const double                            ToV2,
  //   const double ParametricTolerance,
  const double,
  const bool SameUOrientation,
  const bool SameVOrientation)
{
  double FirstU = std::min(FromU1, ToU2);
  double LastU  = std::max(FromU1, ToU2);
  double FirstV = std::min(FromV1, ToV2);
  double LastV  = std::max(FromV1, ToV2);

  occ::handle<Geom_BSplineSurface> NewSurface = occ::down_cast<Geom_BSplineSurface>(S->Copy());

  NewSurface->Segment(FirstU, LastU, FirstV, LastV);

  if (S->IsUPeriodic())
  {
    if (!SameUOrientation)
      NewSurface->UReverse();
  }
  else
  {
    if (FromU1 > ToU2)
      NewSurface->UReverse();
  }
  if (S->IsVPeriodic())
  {
    if (!SameVOrientation)
      NewSurface->VReverse();
  }
  else
  {
    if (FromV1 > ToV2)
      NewSurface->VReverse();
  }
  return NewSurface;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert::SplitBSplineSurface(
  const occ::handle<Geom_BSplineSurface>& S,
  const double                            FromParam1,
  const double                            ToParam2,
  const bool                              USplit,
  const double                            ParametricTolerance,
  const bool                              SameOrientation)
{
  if (std::abs(FromParam1 - ToParam2) <= std::abs(ParametricTolerance))
  {
    throw Standard_DomainError();
  }
  occ::handle<Geom_BSplineSurface> NewSurface = occ::down_cast<Geom_BSplineSurface>(S->Copy());

  if (USplit)
  {
    double FirstU = std::min(FromParam1, ToParam2);
    double LastU  = std::max(FromParam1, ToParam2);
    double FirstV = S->VKnot(S->FirstVKnotIndex());
    double LastV  = S->VKnot(S->LastVKnotIndex());

    NewSurface->Segment(FirstU, LastU, FirstV, LastV);

    if (S->IsUPeriodic())
    {
      if (!SameOrientation)
        NewSurface->UReverse();
    }
    else
    {
      if (FromParam1 > ToParam2)
        NewSurface->UReverse();
    }
  }
  else
  {
    double FirstU = S->UKnot(S->FirstUKnotIndex());
    double LastU  = S->UKnot(S->LastUKnotIndex());
    double FirstV = std::min(FromParam1, ToParam2);
    double LastV  = std::max(FromParam1, ToParam2);

    NewSurface->Segment(FirstU, LastU, FirstV, LastV);

    if (S->IsUPeriodic())
    {
      if (!SameOrientation)
        NewSurface->UReverse();
    }
    else
    {
      if (FromParam1 > ToParam2)
        NewSurface->UReverse();
    }
  }
  return NewSurface;
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomConvert::SurfaceToBSplineSurface(
  const occ::handle<Geom_Surface>& Sr)
{

  double U1, U2, V1, V2;
  Sr->Bounds(U1, U2, V1, V2);
  double UFirst = std::min(U1, U2);
  double ULast  = std::max(U1, U2);
  double VFirst = std::min(V1, V2);
  double VLast  = std::max(V1, V2);

  // If the surface Sr is infinite stop the computation
  if (Precision::IsNegativeInfinite(UFirst) || Precision::IsPositiveInfinite(ULast)
      || Precision::IsNegativeInfinite(VFirst) || Precision::IsPositiveInfinite(VLast))
  {
    throw Standard_DomainError("GeomConvert::SurfaceToBSplineSurface() - infinite surface");
  }

  occ::handle<Geom_BSplineSurface> TheSurface;
  occ::handle<Geom_Surface>        S;
  occ::handle<Geom_OffsetSurface>  OffsetSur;
  if (Sr->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    OffsetSur = occ::down_cast<Geom_OffsetSurface>(Sr);
    S         = OffsetSur->Surface();
    if (!S.IsNull())
    { // Convert the equivalent surface.
      return SurfaceToBSplineSurface(S);
    }
  }
  S = Sr;

  if (S->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> Strim =
      occ::down_cast<Geom_RectangularTrimmedSurface>(S);

    occ::handle<Geom_Surface> Surf = Strim->BasisSurface();
    UFirst                         = U1;
    ULast                          = U2;
    VFirst                         = V1;
    VLast                          = V2;
    if (Surf->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
    {
      occ::handle<Geom_OffsetSurface> OffsetSurBasis = occ::down_cast<Geom_OffsetSurface>(Surf);

      S = OffsetSurBasis->Surface();
      if (!S.IsNull())
      {
        Surf = S;
      }
      else
        S = Surf;
    }

    if (Surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      occ::handle<Geom_RectangularTrimmedSurface> aStrim =
        new (Geom_RectangularTrimmedSurface)(Surf, UFirst, ULast, VFirst, VLast);
      return SurfaceToBSplineSurface(aStrim);
    }
    //
    // For cylinders, cones, spheres, toruses
    const bool   isUClosed = std::abs((ULast - UFirst) - 2. * M_PI) <= Precision::PConfusion();
    const double eps       = 100. * Epsilon(2. * M_PI);
    //
    if (Surf->IsKind(STANDARD_TYPE(Geom_Plane)))
    {
      NCollection_Array2<gp_Pnt> Poles(1, 2, 1, 2);
      Poles(1, 1) = Strim->Value(U1, V1);
      Poles(1, 2) = Strim->Value(U1, V2);
      Poles(2, 1) = Strim->Value(U2, V1);
      Poles(2, 2) = Strim->Value(U2, V2);
      NCollection_Array1<double> UKnots(1, 2);
      NCollection_Array1<double> VKnots(1, 2);
      NCollection_Array1<int>    UMults(1, 2);
      NCollection_Array1<int>    VMults(1, 2);
      UKnots(1)   = U1;
      UKnots(2)   = U2;
      VKnots(1)   = V1;
      VKnots(2)   = V2;
      UMults(1)   = 2;
      UMults(2)   = 2;
      VMults(1)   = 2;
      VMults(2)   = 2;
      int UDegree = 1;
      int VDegree = 1;
      TheSurface = new Geom_BSplineSurface(Poles, UKnots, VKnots, UMults, VMults, UDegree, VDegree);
    }
    else if (Surf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)))
    {
      occ::handle<Geom_CylindricalSurface> TheElSurf =
        occ::down_cast<Geom_CylindricalSurface>(Surf);

      gp_Cylinder Cyl = TheElSurf->Cylinder();
      if (isUClosed)
      {
        Convert_CylinderToBSplineSurface Convert(Cyl, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Convert);
        int aNbK   = TheSurface->NbUKnots();
        if (std::abs(TheSurface->UKnot(1) - UFirst) > eps
            || std::abs(TheSurface->UKnot(aNbK) - ULast) > eps)
        {
          TheSurface->CheckAndSegment(UFirst, ULast, VFirst, VLast);
        }
      }
      else
      {
        Convert_CylinderToBSplineSurface Conv(Cyl, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Conv);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_ConicalSurface)))
    {
      occ::handle<Geom_ConicalSurface> TheElSurf = occ::down_cast<Geom_ConicalSurface>(Surf);
      gp_Cone                          Co        = TheElSurf->Cone();
      if (isUClosed)
      {
        Convert_ConeToBSplineSurface Convert(Co, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Convert);
        int aNbK   = TheSurface->NbUKnots();
        if (std::abs(TheSurface->UKnot(1) - UFirst) > eps
            || std::abs(TheSurface->UKnot(aNbK) - ULast) > eps)
        {
          TheSurface->CheckAndSegment(UFirst, ULast, VFirst, VLast);
        }
      }
      else
      {
        Convert_ConeToBSplineSurface Convert(Co, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Convert);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    {
      occ::handle<Geom_SphericalSurface> TheElSurf = occ::down_cast<Geom_SphericalSurface>(Surf);
      gp_Sphere                          Sph       = TheElSurf->Sphere();
      // OCC217
      if (isUClosed)
      {
        // if (Strim->IsVClosed()) {
        // Convert_SphereToBSplineSurface Convert (Sph, UFirst, ULast);
        Convert_SphereToBSplineSurface Convert(Sph, VFirst, VLast, false);
        TheSurface = BSplineSurfaceBuilder(Convert);
        int aNbK   = TheSurface->NbUKnots();
        if (std::abs(TheSurface->UKnot(1) - UFirst) > eps
            || std::abs(TheSurface->UKnot(aNbK) - ULast) > eps)
        {
          TheSurface->CheckAndSegment(UFirst, ULast, VFirst, VLast);
        }
      }
      else
      {
        Convert_SphereToBSplineSurface Convert(Sph, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Convert);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    {
      occ::handle<Geom_ToroidalSurface> TheElSurf = occ::down_cast<Geom_ToroidalSurface>(Surf);

      gp_Torus Tr = TheElSurf->Torus();
      //
      // if isUClosed = true and U trim does not coincide with first period of torus,
      // method CheckAndSegment shifts position of U seam boundary of surface.
      // probably bug? So, for this case we must build not periodic surface.
      bool isUFirstPeriod = UFirst >= 0. && ULast <= 2. * M_PI;
      bool isVFirstPeriod = VFirst >= 0. && VLast <= 2. * M_PI;
      if (isUClosed && isUFirstPeriod)
      {
        Convert_TorusToBSplineSurface Convert(Tr, VFirst, VLast, false);
        TheSurface = BSplineSurfaceBuilder(Convert);
        int aNbK   = TheSurface->NbUKnots();
        if (std::abs(TheSurface->UKnot(1) - UFirst) > eps
            || std::abs(TheSurface->UKnot(aNbK) - ULast) > eps)
        {
          TheSurface->CheckAndSegment(UFirst, ULast, VFirst, VLast);
        }
      }
      else if (Strim->IsVClosed() && isVFirstPeriod)
      {
        Convert_TorusToBSplineSurface Convert(Tr, UFirst, ULast);
        TheSurface = BSplineSurfaceBuilder(Convert);
        int aNbK   = TheSurface->NbVKnots();
        if (std::abs(TheSurface->VKnot(1) - VFirst) > eps
            || std::abs(TheSurface->VKnot(aNbK) - VLast) > eps)
        {
          TheSurface->CheckAndSegment(UFirst, ULast, VFirst, VLast);
        }
      }
      else
      {
        Convert_TorusToBSplineSurface Convert(Tr, UFirst, ULast, VFirst, VLast);
        TheSurface = BSplineSurfaceBuilder(Convert);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {
      occ::handle<Geom_SurfaceOfRevolution> Revol = occ::down_cast<Geom_SurfaceOfRevolution>(Surf);

      occ::handle<Geom_Curve>        Meridian = Revol->BasisCurve();
      occ::handle<Geom_BSplineCurve> C;
      if (Strim->IsVClosed())
      {
        C = GeomConvert::CurveToBSplineCurve(Meridian);
      }
      else
      {
        occ::handle<Geom_TrimmedCurve> CT = new Geom_TrimmedCurve(Meridian, VFirst, VLast);
        C                                 = GeomConvert::CurveToBSplineCurve(CT);
      }
      int  NbUPoles, NbUKnots;
      int  NbVPoles;
      bool periodic = false;

      // Poles of meridian = Vpoles
      NbVPoles = C->NbPoles();
      NCollection_Array1<gp_Pnt>        Poles(C->Poles());
      const NCollection_Array1<double>& Weights = C->WeightsArray();

      double AlfaU;
      if (Strim->IsUPeriodic())
      {
        NbUKnots = 4;
        AlfaU    = M_PI / 3.;
        NbUPoles = 6;
        periodic = true;
      }
      else
      {
        // Nombre de spans : ouverture maximale = 150 degres ( = PI / 1.2 rds)
        const int nbUSpans = (int)std::trunc(1.2 * (ULast - UFirst) / M_PI) + 1;
        AlfaU              = (ULast - UFirst) / (nbUSpans * 2);
        NbUPoles           = 2 * nbUSpans + 1;
        NbUKnots           = nbUSpans + 1;
      }
      // Compute Knots and Mults
      NCollection_Array1<double> UKnots(1, NbUKnots);
      NCollection_Array1<int>    UMults(1, NbUKnots);
      int                        i, j;
      for (i = 1; i <= NbUKnots; i++)
      {
        UKnots(i) = UFirst + (i - 1) * 2 * AlfaU;
        UMults(i) = 2;
      }
      if (!periodic)
      {
        UMults(1)++;
        UMults(NbUKnots)++;
      }
      const NCollection_Array1<double>& VKnots = C->Knots();
      const NCollection_Array1<int>&    VMults = C->Multiplicities();

      // Compute the poles.
      NCollection_Array2<gp_Pnt> NewPoles(1, NbUPoles, 1, NbVPoles);
      NCollection_Array2<double> NewWeights(1, NbUPoles, 1, NbVPoles);
      gp_Trsf                    Trsf;

      for (i = 1; i <= NbUPoles; i += 2)
      {
        Trsf.SetRotation(Revol->Axis(), UFirst + (i - 1) * AlfaU);
        for (j = 1; j <= NbVPoles; j++)
        {
          NewPoles(i, j)   = Poles(j).Transformed(Trsf);
          NewWeights(i, j) = Weights(j);
        }
      }
      gp_GTrsf Aff;
      Aff.SetAffinity(Revol->Axis(), 1 / std::cos(AlfaU));
      gp_XYZ coord;
      for (j = 1; j <= NbVPoles; j++)
      {
        coord = Poles(j).XYZ();
        Aff.Transforms(coord);
        Poles(j).SetXYZ(coord);
      }
      for (i = 2; i <= NbUPoles; i += 2)
      {
        Trsf.SetRotation(Revol->Axis(), UFirst + (i - 1) * AlfaU);
        for (j = 1; j <= NbVPoles; j++)
        {
          NewPoles(i, j)   = Poles(j).Transformed(Trsf);
          NewWeights(i, j) = Weights(j) * std::cos(AlfaU);
        }
      }

      TheSurface = new Geom_BSplineSurface(NewPoles,
                                           NewWeights,
                                           UKnots,
                                           VKnots,
                                           UMults,
                                           VMults,
                                           2,
                                           C->Degree(),
                                           periodic,
                                           C->IsPeriodic());
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)))
    {
      occ::handle<Geom_SurfaceOfLinearExtrusion> Extru =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(Surf);

      occ::handle<Geom_Curve>        Meridian = Extru->BasisCurve();
      occ::handle<Geom_BSplineCurve> C;
      if (Strim->IsUClosed())
      {
        C = GeomConvert::CurveToBSplineCurve(Meridian);
      }
      else
      {
        occ::handle<Geom_TrimmedCurve> CT = new Geom_TrimmedCurve(Meridian, UFirst, ULast);
        C                                 = GeomConvert::CurveToBSplineCurve(CT);
      }
      NCollection_Array2<gp_Pnt>        Poles(1, C->NbPoles(), 1, 2);
      NCollection_Array2<double>        Weights(1, C->NbPoles(), 1, 2);
      const NCollection_Array1<double>& UKnots = C->Knots();
      const NCollection_Array1<int>&    UMults = C->Multiplicities();
      NCollection_Array1<double>        VKnots(1, 2);
      VKnots(1) = VFirst;
      VKnots(2) = VLast;
      NCollection_Array1<int> VMults(1, 2);
      VMults.Init(2);

      gp_Vec D(Extru->Direction());
      gp_Vec DV1 = VFirst * D;
      gp_Vec DV2 = VLast * D;
      for (int i = 1; i <= C->NbPoles(); i++)
      {
        Poles(i, 1)   = C->Pole(i).Translated(DV1);
        Poles(i, 2)   = C->Pole(i).Translated(DV2);
        Weights(i, 1) = Weights(i, 2) = C->Weight(i);
      }
      TheSurface = new Geom_BSplineSurface(Poles,
                                           Weights,
                                           UKnots,
                                           VKnots,
                                           UMults,
                                           VMults,
                                           C->Degree(),
                                           1,
                                           C->IsPeriodic(),
                                           false);
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
    {

      occ::handle<Geom_BezierSurface> SBez = occ::down_cast<Geom_BezierSurface>(Surf->Copy());

      SBez->Segment(U1, U2, V1, V2);
      int                        UDegree = SBez->UDegree();
      int                        VDegree = SBez->VDegree();
      NCollection_Array1<double> UKnots(1, 2);
      NCollection_Array1<int>    UMults(1, 2);
      NCollection_Array1<double> VKnots(1, 2);
      NCollection_Array1<int>    VMults(1, 2);
      UKnots(1)                                = 0.0;
      UKnots(2)                                = 1.0;
      UMults(1)                                = UDegree + 1;
      UMults(2)                                = UDegree + 1;
      VKnots(1)                                = 0.0;
      VKnots(2)                                = 1.0;
      VMults(1)                                = VDegree + 1;
      VMults(2)                                = VDegree + 1;
      const NCollection_Array2<gp_Pnt>& aPoles = SBez->Poles();
      if (SBez->IsURational() || SBez->IsVRational())
      {
        TheSurface = new Geom_BSplineSurface(aPoles,
                                             SBez->WeightsArray(),
                                             UKnots,
                                             VKnots,
                                             UMults,
                                             VMults,
                                             UDegree,
                                             VDegree);
      }
      else
      {
        TheSurface =
          new Geom_BSplineSurface(aPoles, UKnots, VKnots, UMults, VMults, UDegree, VDegree);
      }
    }

    else if (Surf->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      occ::handle<Geom_BSplineSurface> BS = occ::down_cast<Geom_BSplineSurface>(Surf->Copy());
      double                           umin, umax, vmin, vmax;
      BS->Bounds(umin, umax, vmin, vmax);
      if (!BS->IsUPeriodic())
      {
        if (U1 < umin)
          U1 = umin;
        if (U2 > umax)
          U2 = umax;
      }

      if (!BS->IsVPeriodic())
      {
        if (V1 < vmin)
          V1 = vmin;
        if (V2 > vmax)
          V2 = vmax;
      }
      if (BS->IsUPeriodic() || BS->IsVPeriodic())
        BS->CheckAndSegment(U1, U2, V1, V2);
      else
        BS->Segment(U1, U2, V1, V2);
      TheSurface = BS;
    }

    else
    {
      double              Tol3d     = 1.e-4;
      int                 MaxDegree = 14, MaxSeg;
      GeomAbs_Shape       cont;
      GeomAdaptor_Surface AS(Sr);
      if (AS.NbUIntervals(GeomAbs_C2) > 1 || AS.NbVIntervals(GeomAbs_C2) > 1)
        cont = GeomAbs_C1;
      else
        cont = GeomAbs_C2;
      MaxSeg = 4 * (AS.NbUIntervals(GeomAbs_CN) + 1) * (AS.NbVIntervals(GeomAbs_CN) + 1);
      GeomConvert_ApproxSurface BSpS(Sr, Tol3d, cont, cont, MaxDegree, MaxDegree, MaxSeg, 1);
      TheSurface = BSpS.Surface();
    }
  } // Fin du cas Rectangular::TrimmedSurface

  else
  {

    if (S->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
    {
      occ::handle<Geom_SphericalSurface> TheElSurf = occ::down_cast<Geom_SphericalSurface>(S);

      gp_Sphere                      Sph = TheElSurf->Sphere();
      Convert_SphereToBSplineSurface Convert(Sph);
      TheSurface = BSplineSurfaceBuilder(Convert);
    }

    else if (S->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)))
    {
      occ::handle<Geom_ToroidalSurface> TheElSurf = occ::down_cast<Geom_ToroidalSurface>(S);

      gp_Torus                      Tr = TheElSurf->Torus();
      Convert_TorusToBSplineSurface Convert(Tr);
      TheSurface = BSplineSurfaceBuilder(Convert);
    }

    else if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
    {

      occ::handle<Geom_SurfaceOfRevolution> Revol = occ::down_cast<Geom_SurfaceOfRevolution>(S);

      occ::handle<Geom_Curve>        Meridian = Revol->BasisCurve();
      occ::handle<Geom_BSplineCurve> C        = GeomConvert::CurveToBSplineCurve(Meridian);

      int  NbUPoles, NbUKnots;
      int  NbVPoles;
      bool periodic = true;

      // Poles of meridian = Vpoles
      NbVPoles = C->NbPoles();
      NCollection_Array1<gp_Pnt>        Poles(C->Poles());
      const NCollection_Array1<double>& Weights = C->WeightsArray();

      double AlfaU;
      NbUKnots = 4;
      AlfaU    = M_PI / 3.;
      NbUPoles = 6;

      // Compute Knots and Mults
      NCollection_Array1<double> UKnots(1, NbUKnots);
      NCollection_Array1<int>    UMults(1, NbUKnots);
      int                        i, j;
      for (i = 1; i <= NbUKnots; i++)
      {
        UKnots(i) = UFirst + (i - 1) * 2 * AlfaU;
        UMults(i) = 2;
      }
      const NCollection_Array1<double>& VKnots = C->Knots();
      const NCollection_Array1<int>&    VMults = C->Multiplicities();

      // Compute the poles.
      NCollection_Array2<gp_Pnt> NewPoles(1, NbUPoles, 1, NbVPoles);
      NCollection_Array2<double> NewWeights(1, NbUPoles, 1, NbVPoles);
      gp_Trsf                    Trsf;

      for (i = 1; i <= NbUPoles; i += 2)
      {
        Trsf.SetRotation(Revol->Axis(), UFirst + (i - 1) * AlfaU);
        for (j = 1; j <= NbVPoles; j++)
        {
          NewPoles(i, j)   = Poles(j).Transformed(Trsf);
          NewWeights(i, j) = Weights(j);
        }
      }
      gp_GTrsf Aff;
      Aff.SetAffinity(Revol->Axis(), 1 / std::cos(AlfaU));
      gp_XYZ coord;
      for (j = 1; j <= NbVPoles; j++)
      {
        coord = Poles(j).XYZ();
        Aff.Transforms(coord);
        Poles(j).SetXYZ(coord);
      }
      for (i = 2; i <= NbUPoles; i += 2)
      {
        Trsf.SetRotation(Revol->Axis(), UFirst + (i - 1) * AlfaU);
        for (j = 1; j <= NbVPoles; j++)
        {
          NewPoles(i, j)   = Poles(j).Transformed(Trsf);
          NewWeights(i, j) = Weights(j) * std::cos(AlfaU);
        }
      }

      TheSurface = new Geom_BSplineSurface(NewPoles,
                                           NewWeights,
                                           UKnots,
                                           VKnots,
                                           UMults,
                                           VMults,
                                           2,
                                           C->Degree(),
                                           periodic,
                                           C->IsPeriodic());
    }

    else if (S->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
    {

      occ::handle<Geom_BezierSurface> SBez = occ::down_cast<Geom_BezierSurface>(S);

      int                        UDegree = SBez->UDegree();
      int                        VDegree = SBez->VDegree();
      NCollection_Array1<double> UKnots(1, 2);
      NCollection_Array1<int>    UMults(1, 2);
      NCollection_Array1<double> VKnots(1, 2);
      NCollection_Array1<int>    VMults(1, 2);
      UKnots(1)                                = 0.0;
      UKnots(2)                                = 1.0;
      UMults(1)                                = UDegree + 1;
      UMults(2)                                = UDegree + 1;
      VKnots(1)                                = 0.0;
      VKnots(2)                                = 1.0;
      VMults(1)                                = VDegree + 1;
      VMults(2)                                = VDegree + 1;
      const NCollection_Array2<gp_Pnt>& aPoles = SBez->Poles();
      if (SBez->IsURational() || SBez->IsVRational())
      {
        TheSurface = new Geom_BSplineSurface(aPoles,
                                             SBez->WeightsArray(),
                                             UKnots,
                                             VKnots,
                                             UMults,
                                             VMults,
                                             UDegree,
                                             VDegree);
      }
      else
      {
        TheSurface =
          new Geom_BSplineSurface(aPoles, UKnots, VKnots, UMults, VMults, UDegree, VDegree);
      }
    }

    else if (S->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
    {
      TheSurface = occ::down_cast<Geom_BSplineSurface>(S->Copy()); // Just a copy
    }

    else
    { // In other cases => Approx
      double              Tol3d     = 1.e-4;
      int                 MaxDegree = 14, MaxSeg;
      GeomAbs_Shape       ucont = GeomAbs_C0, vcont = GeomAbs_C0;
      GeomAdaptor_Surface AS(Sr);
      //
      if (Sr->IsCNu(2))
      {
        ucont = GeomAbs_C2;
      }
      else if (Sr->IsCNu(1))
      {
        ucont = GeomAbs_C1;
      }
      //
      if (Sr->IsCNv(2))
      {
        vcont = GeomAbs_C2;
      }
      else if (Sr->IsCNv(1))
      {
        vcont = GeomAbs_C1;
      }
      //
      MaxSeg = 4 * (AS.NbUIntervals(GeomAbs_CN) + 1) * (AS.NbVIntervals(GeomAbs_CN) + 1);
      GeomConvert_ApproxSurface BSpS(Sr, Tol3d, ucont, vcont, MaxDegree, MaxDegree, MaxSeg, 1);
      TheSurface = BSpS.Surface();
    }
  } // Fin du cas direct
  return TheSurface;
}
