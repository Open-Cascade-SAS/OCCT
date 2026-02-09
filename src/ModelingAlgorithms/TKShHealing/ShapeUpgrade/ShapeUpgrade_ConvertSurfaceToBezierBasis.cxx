// Created on: 1999-05-21
// Created by: Pavel DURANDIN
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

//   svv  10.01.00 porting on DEC

#include <Geom_BezierCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert_BSplineSurfaceToBezierSurface.hxx>
#include <Precision.hxx>
#include <ShapeExtend.hxx>
#include <ShapeExtend_CompositeSurface.hxx>
#include <ShapeUpgrade_ConvertCurve3dToBezier.hxx>
#include <ShapeUpgrade_ConvertSurfaceToBezierBasis.hxx>
#include <Standard_Type.hxx>
#include <Geom_BezierSurface.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_HArray2.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_ConvertSurfaceToBezierBasis, ShapeUpgrade_SplitSurface)

ShapeUpgrade_ConvertSurfaceToBezierBasis::ShapeUpgrade_ConvertSurfaceToBezierBasis()
{
  myPlaneMode      = true;
  myRevolutionMode = true;
  myExtrusionMode  = true;
  myBSplineMode    = true;
}

//=================================================================================================

void ShapeUpgrade_ConvertSurfaceToBezierBasis::Compute(const bool Segment)
{
  if (!Segment)
  {
    double UF, UL, VF, VL;
    mySurface->Bounds(UF, UL, VF, VL);
    if (!Precision::IsInfinite(UF))
      myUSplitValues->SetValue(1, UF);
    if (!Precision::IsInfinite(UL))
      myUSplitValues->SetValue(myUSplitValues->Length(), UL);
    if (!Precision::IsInfinite(VF))
      myVSplitValues->SetValue(1, VF);
    if (!Precision::IsInfinite(VL))
      myVSplitValues->SetValue(myVSplitValues->Length(), VL);
  }

  double           UFirst    = myUSplitValues->Value(1);
  double           ULast     = myUSplitValues->Value(myUSplitValues->Length());
  double           VFirst    = myVSplitValues->Value(1);
  double           VLast     = myVSplitValues->Value(myVSplitValues->Length());
  constexpr double precision = Precision::PConfusion();

  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> Surface =
      occ::down_cast<Geom_RectangularTrimmedSurface>(mySurface);
    occ::handle<Geom_Surface>                BasSurf = Surface->BasisSurface();
    ShapeUpgrade_ConvertSurfaceToBezierBasis converter;
    converter.Init(BasSurf, UFirst, ULast, VFirst, VLast);
    converter.SetUSplitValues(myUSplitValues);
    converter.SetVSplitValues(myVSplitValues);
    converter.Compute(true);
    myUSplitValues->ChangeSequence() = converter.USplitValues()->Sequence();
    myVSplitValues->ChangeSequence() = converter.VSplitValues()->Sequence();
    myStatus |= converter.myStatus;
    mySegments = converter.Segments();
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_OffsetSurface>          Offset = occ::down_cast<Geom_OffsetSurface>(mySurface);
    occ::handle<Geom_Surface>                BasSurf = Offset->BasisSurface();
    ShapeUpgrade_ConvertSurfaceToBezierBasis converter;
    converter.Init(BasSurf, UFirst, ULast, VFirst, VLast);
    converter.SetUSplitValues(myUSplitValues);
    converter.SetVSplitValues(myVSplitValues);
    converter.Compute(true);
    myUSplitValues->ChangeSequence() = converter.USplitValues()->Sequence();
    myVSplitValues->ChangeSequence() = converter.VSplitValues()->Sequence();
    myStatus |= converter.myStatus;
    mySegments = converter.Segments();
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_Plane)) && myPlaneMode)
  {
    occ::handle<Geom_Plane>    pln = occ::down_cast<Geom_Plane>(mySurface);
    NCollection_Array2<gp_Pnt> poles(1, 2, 1, 2);
    gp_Pnt                     dp;
    poles(1, 1) = dp = pln->Value(UFirst, VFirst);
    poles(1, 2) = dp = pln->Value(UFirst, VLast);
    poles(2, 1) = dp = pln->Value(ULast, VFirst);
    poles(2, 2) = dp                       = pln->Value(ULast, VLast);
    occ::handle<Geom_BezierSurface> bezier = new Geom_BezierSurface(poles);
    NCollection_Array1<double>      UJoints(1, 2);
    UJoints(1) = UFirst;
    UJoints(2) = ULast;
    NCollection_Array1<double> VJoints(1, 2);
    VJoints(1) = VFirst;
    VJoints(2) = VLast;
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> surf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
    surf->SetValue(1, 1, bezier);
    mySegments = new ShapeExtend_CompositeSurface(surf, UJoints, VJoints);
    myStatus   = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    occ::handle<Geom_BezierSurface> bezier = occ::down_cast<Geom_BezierSurface>(mySurface);
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> surf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
    NCollection_Array1<double> UJoints(1, 2);
    UJoints(1) = UFirst;
    UJoints(2) = ULast;
    NCollection_Array1<double> VJoints(1, 2);
    VJoints(1) = VFirst;
    VJoints(2) = VLast;
    if (UFirst < precision && ULast > 1 - precision && VFirst < precision && VLast > 1 - precision)
    {
      surf->SetValue(1, 1, bezier);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    }
    else
    {
      occ::handle<Geom_BezierSurface> besNew = occ::down_cast<Geom_BezierSurface>(bezier->Copy());
      // pdn K4L+ (work around)
      //  double u1 = 2*UFirst - 1;
      //  double u2 = 2*ULast - 1;
      //  double v1 = 2*VFirst - 1;
      //  double v2 = 2*VLast - 1;
      // rln C30 (direct use)
      double u1 = UFirst;
      double u2 = ULast;
      double v1 = VFirst;
      double v2 = VLast;
      besNew->Segment(u1, u2, v1, v2);
      surf->SetValue(1, 1, besNew);
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    }
    mySegments = new ShapeExtend_CompositeSurface(surf, UJoints, VJoints);
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_BSplineSurface)) && myBSplineMode)
  {
    occ::handle<Geom_BSplineSurface> bspline = occ::down_cast<Geom_BSplineSurface>(mySurface);
    // pdn
    double u1, u2, v1, v2;
    bspline->Bounds(u1, u2, v1, v2);
    // clang-format off
    GeomConvert_BSplineSurfaceToBezierSurface converter(bspline);//,UFirst,ULast,VFirst,VLast,precision;
    // clang-format on
    int                        nbUPatches = converter.NbUPatches();
    int                        nbVPatches = converter.NbVPatches();
    NCollection_Array1<double> UJoints(1, nbUPatches + 1);
    NCollection_Array1<bool>   UReject(1, nbUPatches + 1);
    UReject.Init(false);
    NCollection_Array1<double> VJoints(1, nbVPatches + 1);
    NCollection_Array1<bool>   VReject(1, nbVPatches + 1);
    VReject.Init(false);
    int NbUFiltered = 0;
    int NbVFiltered = 0;

    converter.UKnots(UJoints);
    NCollection_Sequence<double> UFilteredJoints;
    UFilteredJoints.Append(UJoints(1));
    int i;
    for (i = 2; i <= nbUPatches + 1; i++)
      if (UJoints(i) - UJoints(i - 1) < precision)
      {
        NbUFiltered++;
        UReject(i - 1) = true;
      }
      else
        UFilteredJoints.Append(UJoints(i));

    converter.VKnots(VJoints);
    NCollection_Sequence<double> VFilteredJoints;
    VFilteredJoints.Append(VJoints(1));
    for (i = 2; i <= nbVPatches + 1; i++)
      if (VJoints(i) - VJoints(i - 1) < precision)
      {
        NbVFiltered++;
        VReject(i - 1) = true;
      }
      else
        VFilteredJoints.Append(VJoints(i));

#ifdef OCCT_DEBUG
    if (NbVFiltered || NbUFiltered)
      std::cout << "Warning: ShapeUpgrade_ConvertSurfaceToBezierBasis: thin patches dropped."
                << std::endl;
#endif

    NCollection_Array2<occ::handle<Geom_BezierSurface>> Surfaces(1, nbUPatches, 1, nbVPatches);
    converter.Patches(Surfaces);
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> srf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1,
                                                         nbUPatches - NbUFiltered,
                                                         1,
                                                         nbVPatches - NbVFiltered);
    int indApp1 = 0;
    for (int ind1 = 1; ind1 <= nbUPatches; ind1++)
    {
      if (UReject(ind1))
        continue;
      indApp1++;
      int indApp2 = 0;
      for (int ind2 = 1; ind2 <= nbVPatches; ind2++)
      {
        if (VReject(ind2))
          continue;
        indApp2++;
        srf->SetValue(indApp1, indApp2, Surfaces(ind1, ind2));
      }
    }

    NCollection_Array1<double> uj(1, UFilteredJoints.Length());
    for (i = 1; i <= UFilteredJoints.Length(); i++)
      uj(i) = UFilteredJoints.Value(i);

    NCollection_Array1<double> vj(1, VFilteredJoints.Length());
    for (i = 1; i <= VFilteredJoints.Length(); i++)
      vj(i) = VFilteredJoints.Value(i);

    mySegments = new ShapeExtend_CompositeSurface(srf, uj, vj);

    int j; // svv #1
    for (j = 2; j <= myUSplitValues->Length(); j++)
    {
      ULast = myUSplitValues->Value(j);
      for (int ii = 2; ii <= nbUPatches + 1; ii++)
      {
        double valknot = UJoints(ii);
        if (valknot - UFirst <= precision)
          continue;
        if (ULast - valknot <= precision)
          break;
        myUSplitValues->InsertBefore(j++, valknot);
      }
      UFirst = ULast;
    }
    for (j = 2; j <= myVSplitValues->Length(); j++)
    {
      VLast = myVSplitValues->Value(j);
      for (int ii = 2; ii <= nbVPatches + 1; ii++)
      {
        double valknot = VJoints(ii);
        if (valknot - VFirst <= precision)
          continue;
        if (VLast - valknot <= precision)
          break;
        myVSplitValues->InsertBefore(j++, valknot);
      }
      VFirst = VLast;
    }
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) && myRevolutionMode)
  {
    occ::handle<Geom_SurfaceOfRevolution> revol =
      occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
    occ::handle<Geom_Curve> basis = revol->BasisCurve();
    if (basis->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
    {
      occ::handle<Geom_TrimmedCurve> tc = occ::down_cast<Geom_TrimmedCurve>(basis);
      basis                             = tc->BasisCurve();
    }
    occ::handle<NCollection_HArray1<occ::handle<Geom_Curve>>> curves;
    int                                                       nbCurves;
    occ::handle<NCollection_HSequence<double>> vPar  = new NCollection_HSequence<double>;
    occ::handle<NCollection_HSequence<double>> vSVal = new NCollection_HSequence<double>;
    if (basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      occ::handle<Geom_OffsetCurve>       offset    = occ::down_cast<Geom_OffsetCurve>(basis);
      double                              value     = offset->Offset();
      gp_Dir                              direction = offset->Direction();
      occ::handle<Geom_Curve>             bas       = offset->BasisCurve();
      ShapeUpgrade_ConvertCurve3dToBezier converter;
      converter.Init(bas, VFirst, VLast);
      converter.Perform(true);
      if (converter.Status(ShapeExtend_DONE))
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      else
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

      vPar->ChangeSequence()  = converter.SplitParams()->Sequence();
      vSVal->ChangeSequence() = converter.SplitValues()->Sequence();
      curves                  = converter.GetCurves();
      nbCurves                = curves->Length();
      for (int i = 1; i <= nbCurves; i++)
      {
        occ::handle<Geom_OffsetCurve> offCur =
          new Geom_OffsetCurve(curves->Value(i), value, direction);
        curves->SetValue(i, offCur);
      }
    }
    else
    {
      ShapeUpgrade_ConvertCurve3dToBezier converter;
      converter.Init(basis, VFirst, VLast);
      converter.Perform(true);
      if (converter.Status(ShapeExtend_DONE))
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      else
        myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);

      vPar->ChangeSequence()  = converter.SplitParams()->Sequence();
      vSVal->ChangeSequence() = converter.SplitValues()->Sequence();
      curves                  = converter.GetCurves();
      nbCurves                = curves->Length();
    }

    gp_Ax1                                                      axis = revol->Axis();
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> surf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, nbCurves);
    double Umin, Umax, Vmin, Vmax;
    mySurface->Bounds(Umin, Umax, Vmin, Vmax);
    int i; // svv #1
    for (i = 1; i <= nbCurves; i++)
    {
      occ::handle<Geom_SurfaceOfRevolution> rev =
        new Geom_SurfaceOfRevolution(curves->Value(i), axis);
      if (UFirst - Umin < Precision::PConfusion() && Umax - ULast < Precision::PConfusion())
        surf->SetValue(1, i, rev);
      else
      {
        occ::handle<Geom_RectangularTrimmedSurface> rect =
          new Geom_RectangularTrimmedSurface(rev, UFirst, ULast, true);
        surf->SetValue(1, i, rect);
      }
    }
    NCollection_Array1<double> UJoints(1, 2);
    NCollection_Array1<double> VJoints(1, nbCurves + 1);
    UJoints(1) = UFirst;
    UJoints(2) = ULast;
    for (i = 1; i <= nbCurves + 1; i++)
      VJoints(i) = vPar->Value(i);

    mySegments = new ShapeExtend_CompositeSurface(surf, UJoints, VJoints);

    for (int j = 2; j <= myVSplitValues->Length(); j++)
    {
      VLast = myVSplitValues->Value(j);
      for (int ii = 2; ii <= nbCurves + 1; ii++)
      {
        double valknot = vSVal->Value(ii);
        if (valknot - VFirst <= precision)
          continue;
        if (VLast - valknot <= precision)
          break;
        myVSplitValues->InsertBefore(j++, valknot);
      }
      VFirst = VLast;
    }
    return;
  }
  else if (mySurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) && myExtrusionMode)
  {
    occ::handle<Geom_SurfaceOfLinearExtrusion> extr =
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface);
    occ::handle<Geom_Curve> basis = extr->BasisCurve();
    // gp_Dir direction = extr->Direction(); // direction not used (skl)

    occ::handle<NCollection_HArray1<occ::handle<Geom_Curve>>> curves;
    int                                                       nbCurves;
    occ::handle<NCollection_HSequence<double>> uPar  = new NCollection_HSequence<double>;
    occ::handle<NCollection_HSequence<double>> uSVal = new NCollection_HSequence<double>;
    ShapeUpgrade_ConvertCurve3dToBezier        converter;
    converter.Init(basis, UFirst, ULast);
    converter.Perform(true);
    uPar->ChangeSequence()  = converter.SplitParams()->Sequence();
    uSVal->ChangeSequence() = converter.SplitValues()->Sequence();
    curves                  = converter.GetCurves();
    nbCurves                = curves->Length();

    gp_Trsf shiftF, shiftL;
    shiftF.SetTranslation(extr->Value(UFirst, 0), extr->Value(UFirst, VFirst));
    shiftL.SetTranslation(extr->Value(UFirst, 0), extr->Value(UFirst, VLast));
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> surf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1, nbCurves, 1, 1);

    int i; // svv #1
    for (i = 1; i <= nbCurves; i++)
    {
      occ::handle<Geom_BezierCurve>   bez     = occ::down_cast<Geom_BezierCurve>(curves->Value(i));
      int                             nbPoles = bez->NbPoles();
      const NCollection_Array1<gp_Pnt>& poles = bez->Poles();
      NCollection_Array2<gp_Pnt> resPoles(1, nbPoles, 1, 2);
      for (int j = 1; j <= nbPoles; j++)
      {
        resPoles(j, 1) = poles(j).Transformed(shiftF);
        resPoles(j, 2) = poles(j).Transformed(shiftL);
      }
      occ::handle<Geom_BezierSurface> bezier = new Geom_BezierSurface(resPoles);
      surf->SetValue(i, 1, bezier);
    }

    NCollection_Array1<double> UJoints(1, nbCurves + 1);
    NCollection_Array1<double> VJoints(1, 2);
    VJoints(1) = VFirst;
    VJoints(2) = VLast;
    for (i = 1; i <= nbCurves + 1; i++)
      UJoints(i) = uPar->Value(i);

    mySegments = new ShapeExtend_CompositeSurface(surf, UJoints, VJoints);

    for (int j = 2; j <= myUSplitValues->Length(); j++)
    {
      ULast = myUSplitValues->Value(j);
      for (int ii = 2; ii <= nbCurves + 1; ii++)
      {
        double valknot = uSVal->Value(ii);
        if (valknot + UFirst <= precision)
          continue;
        if (ULast - valknot <= precision)
          break;
        myUSplitValues->InsertBefore(j++, valknot);
      }
      UFirst = ULast;
    }
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return;
  }
  else
  {
    NCollection_Array1<double> UJoints(1, 2);
    UJoints(1) = UFirst;
    UJoints(2) = ULast;
    NCollection_Array1<double> VJoints(1, 2);
    VJoints(1) = VFirst;
    VJoints(2) = VLast;
    occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> surf =
      new NCollection_HArray2<occ::handle<Geom_Surface>>(1, 1, 1, 1);
    double U1, U2, V1, V2;
    mySurface->Bounds(U1, U2, V1, V2);
    occ::handle<Geom_Surface> S;
    if (U1 - UFirst < precision && ULast - U2 < precision && V2 - VFirst < precision
        && VLast - V2 < precision)
      S = mySurface;
    else
    {
      occ::handle<Geom_RectangularTrimmedSurface> rts =
        new Geom_RectangularTrimmedSurface(mySurface, UFirst, ULast, VFirst, VLast);
      S = rts;
    }
    surf->SetValue(1, 1, S);
    mySegments = new ShapeExtend_CompositeSurface(surf, UJoints, VJoints);
    myStatus   = ShapeExtend::EncodeStatus(ShapeExtend_OK);
    return;
  }
}

//=================================================================================================

static occ::handle<Geom_Surface> GetSegment(const occ::handle<Geom_Surface>& surf,
                                            const double                     U1,
                                            const double                     U2,
                                            const double                     V1,
                                            const double                     V2)
{
  if (surf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
  {
    occ::handle<Geom_BezierSurface> bezier = occ::down_cast<Geom_BezierSurface>(surf->Copy());
    constexpr double                prec   = Precision::PConfusion();
    if (U1 < prec && U2 > 1 - prec && V1 < prec && V2 > 1 - prec)
      return bezier;
    // pdn K4L+ (work around)
    //  double u1 = 2*U1 - 1;
    //  double u2 = 2*U2 - 1;
    //  double v1 = 2*V1 - 1;
    //  double v2 = 2*V2 - 1;
    // rln C30 (direct use)
    double u1 = U1;
    double u2 = U2;
    double v1 = V1;
    double v2 = V2;
    bezier->Segment(u1, u2, v1, v2);
    return bezier;
  }

  occ::handle<Geom_Surface> S;
  if (surf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> rect =
      occ::down_cast<Geom_RectangularTrimmedSurface>(surf);
    S = rect->BasisSurface();
  }
  else
    S = surf;

  if (S->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))
  {
    occ::handle<Geom_SurfaceOfRevolution> revol =
      occ::down_cast<Geom_SurfaceOfRevolution>(S->Copy());
    double Umin, Umax, Vmin, Vmax;
    revol->Bounds(Umin, Umax, Vmin, Vmax);
    occ::handle<Geom_Curve> basis = revol->BasisCurve();
    if (basis->IsKind(STANDARD_TYPE(Geom_OffsetCurve)))
    {
      occ::handle<Geom_OffsetCurve> offset = occ::down_cast<Geom_OffsetCurve>(basis);
      basis                                = offset->BasisCurve();
    }
    if (basis->IsKind(STANDARD_TYPE(Geom_BezierCurve)))
    {
      occ::handle<Geom_BezierCurve> bezier = occ::down_cast<Geom_BezierCurve>(basis);
      bezier->Segment(V1, V2);
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "Warning: Resulting path is not surface of revolution basis on bezier curve"
                << std::endl;
#endif
    }
    if (std::abs(U1 - Umin) < Precision::PConfusion()
        && std::abs(U2 - Umax) < Precision::PConfusion())
      return revol;

    occ::handle<Geom_RectangularTrimmedSurface> res =
      new Geom_RectangularTrimmedSurface(revol, U1, U2, true);
    return res;
  }
  else
  {
    double Umin, Umax, Vmin, Vmax;
    surf->Bounds(Umin, Umax, Vmin, Vmax);
    if (U1 - Umin < Precision::PConfusion() && Umax - U2 < Precision::PConfusion()
        && V1 - Vmin < Precision::PConfusion() && Vmax - V2 < Precision::PConfusion())
      return surf;

    occ::handle<Geom_RectangularTrimmedSurface> res =
      new Geom_RectangularTrimmedSurface(surf, U1, U2, V1, V2);
    return res;
  }
}

//=================================================================================================

void ShapeUpgrade_ConvertSurfaceToBezierBasis::Build(const bool /*Segment*/)
{
  bool                      isOffset    = false;
  double                    offsetValue = 0;
  occ::handle<Geom_Surface> S;
  if (mySurface->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
  {
    occ::handle<Geom_RectangularTrimmedSurface> Surface =
      occ::down_cast<Geom_RectangularTrimmedSurface>(mySurface);
    S = Surface->BasisSurface();
  }
  else
    S = mySurface;
  if (S->IsKind(STANDARD_TYPE(Geom_OffsetSurface)))
  {
    occ::handle<Geom_OffsetSurface> offSur = occ::down_cast<Geom_OffsetSurface>(S);
    isOffset                               = true;
    offsetValue                            = offSur->Offset();
  }

  constexpr double                         prec           = Precision::PConfusion();
  occ::handle<NCollection_HArray1<double>> myUSplitParams = mySegments->UJointValues();
  occ::handle<NCollection_HArray1<double>> myVSplitParams = mySegments->VJointValues();
  int                                      nbU            = myUSplitValues->Length();
  int                                      nbV            = myVSplitValues->Length();

  occ::handle<NCollection_HArray2<occ::handle<Geom_Surface>>> resSurfaces =
    new NCollection_HArray2<occ::handle<Geom_Surface>>(1, nbU - 1, 1, nbV - 1);
  int j1 = 2;
  for (int i1 = 2; i1 <= nbU; i1++)
  {
    double parU = myUSplitValues->Value(i1);
    for (; j1 <= myUSplitParams->Length(); j1++)
    {
      double param = myUSplitParams->Value(j1);
      if (parU - param < prec)
        break;
    }

    int j2 = 2;
    for (int i2 = 2; i2 <= nbV; i2++)
    {
      double parV = myVSplitValues->Value(i2);
      for (; j2 <= myVSplitParams->Length(); j2++)
        if (parV - myVSplitParams->Value(j2) < prec)
          break;

      occ::handle<Geom_Surface> patch = mySegments->Patch(j1 - 1, j2 - 1);
      double                    U1, U2, V1, V2;
      patch->Bounds(U1, U2, V1, V2);
      // linear recomputation of part:
      double uFirst = myUSplitParams->Value(j1 - 1);
      double uLast  = myUSplitParams->Value(j1);
      double vFirst = myVSplitParams->Value(j2 - 1);
      double vLast  = myVSplitParams->Value(j2);
      double uFact  = (U2 - U1) / (uLast - uFirst);
      double vFact  = (V2 - V1) / (vLast - vFirst);
      double ppU    = myUSplitValues->Value(i1 - 1);
      double ppV    = myVSplitValues->Value(i2 - 1);
      // defining a part
      double                    uL1 = U1 + (ppU - uFirst) * uFact;
      double                    uL2 = U1 + (parU - uFirst) * uFact;
      double                    vL1 = V1 + (ppV - vFirst) * vFact;
      double                    vL2 = V1 + (parV - vFirst) * vFact;
      occ::handle<Geom_Surface> res = GetSegment(patch, uL1, uL2, vL1, vL2);
      if (isOffset)
      {
        occ::handle<Geom_OffsetSurface> resOff = new Geom_OffsetSurface(res, offsetValue);
        res                                    = resOff;
      }
      resSurfaces->SetValue(i1 - 1, i2 - 1, res);
    }
  }

  NCollection_Array1<double> UJoints(1, nbU);
  int                        i; // svv #1
  for (i = 1; i <= nbU; i++)
    UJoints(i) = myUSplitValues->Value(i);

  NCollection_Array1<double> VJoints(1, nbV);
  for (i = 1; i <= nbV; i++)
    VJoints(i) = myVSplitValues->Value(i);

  myResSurfaces = new ShapeExtend_CompositeSurface(resSurfaces, UJoints, VJoints);
}

//=================================================================================================

occ::handle<ShapeExtend_CompositeSurface> ShapeUpgrade_ConvertSurfaceToBezierBasis::Segments() const
{
  return mySegments;
}
