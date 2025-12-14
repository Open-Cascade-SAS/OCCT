// Created on: 1993-05-14
// Created by: Joelle CHAUVET
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

// Modified:	Thu Nov 26 16:37:18 1998
//		correction in NbUIntervals for SurfaceOfLinearExtrusion
//		(PRO16346)

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <GeomAdaptor_Surface.hxx>

#include "../Geom/Geom_ExtrusionUtils.pxx"
#include "../Geom/Geom_OsculatingSurface.pxx"
#include "../Geom/Geom_RevolutionUtils.pxx"

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BSplCLib.hxx>
#include <BSplSLib_Cache.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_UndefinedValue.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_NumericError.hxx>
#include <TColgp_Array2OfVec.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

static const Standard_Real PosTol = Precision::PConfusion() * 0.5;

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_Surface, Adaptor3d_Surface)

//=================================================================================================

void GeomAdaptor_OsculatingSurfaceDeleter::operator()(Geom_OsculatingSurface* thePtr) const
{
  delete thePtr;
}

//=================================================================================================

GeomAdaptor_OffsetSurfaceData::GeomAdaptor_OffsetSurfaceData(
  const GeomAdaptor_OffsetSurfaceData& theOther)
    : BasisAdaptor(theOther.BasisAdaptor),
      Offset(theOther.Offset)
{
  if (theOther.OsculatingSurface)
  {
    OsculatingSurface.reset(new Geom_OsculatingSurface(*theOther.OsculatingSurface));
  }
}

//=================================================================================================

GeomAdaptor_OffsetSurfaceData& GeomAdaptor_OffsetSurfaceData::operator=(
  const GeomAdaptor_OffsetSurfaceData& theOther)
{
  if (this != &theOther)
  {
    BasisAdaptor = theOther.BasisAdaptor;
    Offset       = theOther.Offset;
    if (theOther.OsculatingSurface)
    {
      OsculatingSurface.reset(new Geom_OsculatingSurface(*theOther.OsculatingSurface));
    }
    else
    {
      OsculatingSurface.reset();
    }
  }
  return *this;
}

//=================================================================================================

GeomAbs_Shape LocalContinuity(Standard_Integer         Degree,
                              Standard_Integer         Nb,
                              TColStd_Array1OfReal&    TK,
                              TColStd_Array1OfInteger& TM,
                              Standard_Real            PFirst,
                              Standard_Real            PLast,
                              Standard_Boolean         IsPeriodic)
{
  Standard_DomainError_Raise_if((TK.Length() != Nb || TM.Length() != Nb), " ");
  Standard_Integer Index1 = 0;
  Standard_Integer Index2 = 0;
  Standard_Real    newFirst, newLast;
  BSplCLib::LocateParameter(Degree, TK, TM, PFirst, IsPeriodic, 1, Nb, Index1, newFirst);
  BSplCLib::LocateParameter(Degree, TK, TM, PLast, IsPeriodic, 1, Nb, Index2, newLast);
  constexpr Standard_Real EpsKnot = Precision::PConfusion();
  if (std::abs(newFirst - TK(Index1 + 1)) < EpsKnot)
    Index1++;
  if (std::abs(newLast - TK(Index2)) < EpsKnot)
    Index2--;
  // attention aux courbes peridiques.
  if ((IsPeriodic) && (Index1 == Nb))
    Index1 = 1;

  if (Index2 != Index1)
  {
    Standard_Integer i, Multmax = TM(Index1 + 1);
    for (i = Index1 + 1; i <= Index2; i++)
    {
      if (TM(i) > Multmax)
        Multmax = TM(i);
    }
    Multmax = Degree - Multmax;
    if (Multmax <= 0)
      return GeomAbs_C0;
    switch (Multmax)
    {
      case 1:
        return GeomAbs_C1;
      case 2:
        return GeomAbs_C2;
      case 3:
        return GeomAbs_C3;
    }
  }
  return GeomAbs_CN;
}

namespace
{

// Tolerance for considering derivative to be null
static const double THE_D1_MAG_TOL = 1.e-9;

//! Extrusion surface D0 evaluation
inline void extrusionD0(const double                   theU,
                        const double                   theV,
                        const Handle(Adaptor3d_Curve)& theBasis,
                        const gp_Dir&                  theDir,
                        gp_Pnt&                        theValue)
{
  Geom_ExtrusionUtils::D0(theU, theV, *theBasis, theDir, theValue);
}

//! Extrusion surface D1 evaluation
inline void extrusionD1(const double                   theU,
                        const double                   theV,
                        const Handle(Adaptor3d_Curve)& theBasis,
                        const gp_Dir&                  theDir,
                        gp_Pnt&                        theValue,
                        gp_Vec&                        theD1U,
                        gp_Vec&                        theD1V)
{
  Geom_ExtrusionUtils::D1(theU, theV, *theBasis, theDir, theValue, theD1U, theD1V);
}

//! Extrusion surface D2 evaluation
inline void extrusionD2(const double                   theU,
                        const double                   theV,
                        const Handle(Adaptor3d_Curve)& theBasis,
                        const gp_Dir&                  theDir,
                        gp_Pnt&                        theValue,
                        gp_Vec&                        theD1U,
                        gp_Vec&                        theD1V,
                        gp_Vec&                        theD2U,
                        gp_Vec&                        theD2V,
                        gp_Vec&                        theD2UV)
{
  Geom_ExtrusionUtils::D2(theU, theV, *theBasis, theDir, theValue, theD1U, theD1V, theD2U, theD2V, theD2UV);
}

//! Extrusion surface D3 evaluation
inline void extrusionD3(const double                   theU,
                        const double                   theV,
                        const Handle(Adaptor3d_Curve)& theBasis,
                        const gp_Dir&                  theDir,
                        gp_Pnt&                        theValue,
                        gp_Vec&                        theD1U,
                        gp_Vec&                        theD1V,
                        gp_Vec&                        theD2U,
                        gp_Vec&                        theD2V,
                        gp_Vec&                        theD2UV,
                        gp_Vec&                        theD3U,
                        gp_Vec&                        theD3V,
                        gp_Vec&                        theD3UUV,
                        gp_Vec&                        theD3UVV)
{
  Geom_ExtrusionUtils::D3(theU, theV, *theBasis, theDir, theValue, theD1U, theD1V, theD2U, theD2V, theD2UV, theD3U, theD3V, theD3UUV, theD3UVV);
}

//! Extrusion surface DN evaluation
inline gp_Vec extrusionDN(const double                   theU,
                          const Handle(Adaptor3d_Curve)& theBasis,
                          const gp_Dir&                  theDir,
                          int                            theDerU,
                          int                            theDerV)
{
  return Geom_ExtrusionUtils::DN(theU, *theBasis, theDir, theDerU, theDerV);
}

//! Revolution surface D0 evaluation
inline void revolutionD0(const double                   theU,
                         const double                   theV,
                         const Handle(Adaptor3d_Curve)& theBasis,
                         const gp_Ax1&                  theAxis,
                         gp_Pnt&                        theValue)
{
  Geom_RevolutionUtils::D0(theU, theV, *theBasis, theAxis.Location().XYZ(), theAxis.Direction().XYZ(), theValue);
}

//! Revolution surface D1 evaluation
inline void revolutionD1(const double                   theU,
                         const double                   theV,
                         const Handle(Adaptor3d_Curve)& theBasis,
                         const gp_Ax1&                  theAxis,
                         gp_Pnt&                        theValue,
                         gp_Vec&                        theD1U,
                         gp_Vec&                        theD1V)
{
  Geom_RevolutionUtils::D1(theU, theV, *theBasis, theAxis.Location().XYZ(), theAxis.Direction().XYZ(), theValue, theD1U, theD1V);
}

//! Revolution surface D2 evaluation
inline void revolutionD2(const double                   theU,
                         const double                   theV,
                         const Handle(Adaptor3d_Curve)& theBasis,
                         const gp_Ax1&                  theAxis,
                         gp_Pnt&                        theValue,
                         gp_Vec&                        theD1U,
                         gp_Vec&                        theD1V,
                         gp_Vec&                        theD2U,
                         gp_Vec&                        theD2V,
                         gp_Vec&                        theD2UV)
{
  Geom_RevolutionUtils::D2(theU, theV, *theBasis, theAxis.Location().XYZ(), theAxis.Direction().XYZ(), theValue, theD1U, theD1V, theD2U, theD2V, theD2UV);
}

//! Revolution surface D3 evaluation
inline void revolutionD3(const double                   theU,
                         const double                   theV,
                         const Handle(Adaptor3d_Curve)& theBasis,
                         const gp_Ax1&                  theAxis,
                         gp_Pnt&                        theValue,
                         gp_Vec&                        theD1U,
                         gp_Vec&                        theD1V,
                         gp_Vec&                        theD2U,
                         gp_Vec&                        theD2V,
                         gp_Vec&                        theD2UV,
                         gp_Vec&                        theD3U,
                         gp_Vec&                        theD3V,
                         gp_Vec&                        theD3UUV,
                         gp_Vec&                        theD3UVV)
{
  Geom_RevolutionUtils::D3(theU, theV, *theBasis, theAxis.Location().XYZ(), theAxis.Direction().XYZ(), theValue, theD1U, theD1V, theD2U, theD2V, theD2UV, theD3U, theD3V, theD3UUV, theD3UVV);
}

//! Revolution surface DN evaluation
inline gp_Vec revolutionDN(const double                   theU,
                           const double                   theV,
                           const Handle(Adaptor3d_Curve)& theBasis,
                           const gp_Ax1&                  theAxis,
                           int                            theDerU,
                           int                            theDerV)
{
  return Geom_RevolutionUtils::DN(theU, theV, *theBasis, theAxis.Location().XYZ(), theAxis.Direction().XYZ(), theDerU, theDerV);
}

//! Check if vector has infinite coordinates
inline bool isInfiniteCoord(const gp_Vec& theVec)
{
  return Precision::IsInfinite(theVec.X()) || Precision::IsInfinite(theVec.Y())
         || Precision::IsInfinite(theVec.Z());
}

//! Check and throw if vectors have infinite coordinates
inline void checkInfinite(const gp_Vec& theVecU, const gp_Vec& theVecV)
{
  if (isInfiniteCoord(theVecU) || isInfiniteCoord(theVecV))
  {
    throw Standard_NumericError("GeomAdaptor_Surface: Evaluation of infinite parameters");
  }
}

//! Offset surface D0 evaluation
inline void offsetD0(const double theU,
                     const double theV,
                     const GeomAdaptor_OffsetSurfaceData& theData,
                     gp_Pnt& theValue)
{
  gp_Vec aD1U, aD1V;
  theData.BasisAdaptor->D1(theU, theV, theValue, aD1U, aD1V);
  checkInfinite(aD1U, aD1V);

  // Normalize derivatives for stable normal calculation
  double aD1UNorm2 = aD1U.SquareMagnitude();
  double aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  gp_Vec aNorm = aD1U.Crossed(aD1V);
  if (aNorm.SquareMagnitude() > THE_D1_MAG_TOL * THE_D1_MAG_TOL)
  {
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + theData.Offset * aNorm.XYZ());
  }
  else
  {
    // Singular case - use CSLib for normal calculation
    Handle(Geom_BSplineSurface) aL;
    bool isOpposite = false;
    bool AlongU = false;
    bool AlongV = false;
    if (theData.OsculatingSurface && theData.OsculatingSurface->HasOscSurf())
    {
      AlongU = theData.OsculatingSurface->UOscSurf(theU, theV, isOpposite, aL);
      AlongV = theData.OsculatingSurface->VOscSurf(theU, theV, isOpposite, aL);
    }
    const double aSign = ((AlongV || AlongU) && isOpposite) ? -1.0 : 1.0;

    double aUMin, aUMax, aVMin, aVMax;
    theData.BasisAdaptor->D1(theU, theV, theValue, aD1U, aD1V);
    aUMin = theData.BasisAdaptor->FirstUParameter();
    aUMax = theData.BasisAdaptor->LastUParameter();
    aVMin = theData.BasisAdaptor->FirstVParameter();
    aVMax = theData.BasisAdaptor->LastVParameter();

    gp_Dir aNormal;
    CSLib_NormalStatus aNStatus;
    CSLib::Normal(aD1U, aD1V, THE_D1_MAG_TOL, aNStatus, aNormal);
    if (aNStatus != CSLib_Defined)
    {
      // Try with higher derivatives
      constexpr int MaxOrder = 3;
      TColgp_Array2OfVec aDerNUV(0, MaxOrder, 0, MaxOrder);
      aDerNUV.SetValue(1, 0, aD1U);
      aDerNUV.SetValue(0, 1, aD1V);
      for (int i = 0; i <= MaxOrder; ++i)
      {
        for (int j = 0; j <= MaxOrder; ++j)
        {
          if (i + j > 1 && i + j <= MaxOrder)
            aDerNUV.SetValue(i, j, theData.BasisAdaptor->DN(theU, theV, i, j));
        }
      }
      int OrderU, OrderV;
      CSLib::Normal(MaxOrder, aDerNUV, THE_D1_MAG_TOL, theU, theV,
                    aUMin, aUMax, aVMin, aVMax, aNStatus, aNormal, OrderU, OrderV);
    }

    if (aNStatus != CSLib_Defined)
      throw Standard_NumericError("GeomAdaptor_Surface: Unable to calculate normal");

    theValue.SetXYZ(theValue.XYZ() + theData.Offset * aSign * aNormal.XYZ());
  }
}

//! Offset surface D1 evaluation
inline void offsetD1(const double theU,
                     const double theV,
                     const GeomAdaptor_OffsetSurfaceData& theData,
                     gp_Pnt& theValue,
                     gp_Vec& theD1U,
                     gp_Vec& theD1V)
{
  gp_Vec aD2U, aD2V, aD2UV;
  theData.BasisAdaptor->D2(theU, theV, theValue, theD1U, theD1V, aD2U, aD2V, aD2UV);
  checkInfinite(theD1U, theD1V);

  // Normalize derivatives for stable normal calculation
  gp_Vec aD1U(theD1U), aD1V(theD1V);
  double aD1UNorm2 = aD1U.SquareMagnitude();
  double aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  gp_Vec aNorm = aD1U.Crossed(aD1V);
  bool isSingular = (aNorm.SquareMagnitude() <= THE_D1_MAG_TOL * THE_D1_MAG_TOL);

  Handle(Geom_BSplineSurface) aL;
  bool isOpposite = false;
  bool AlongU = false;
  bool AlongV = false;
  if (isSingular && theData.OsculatingSurface && theData.OsculatingSurface->HasOscSurf())
  {
    AlongU = theData.OsculatingSurface->UOscSurf(theU, theV, isOpposite, aL);
    AlongV = theData.OsculatingSurface->VOscSurf(theU, theV, isOpposite, aL);
  }
  const double aSign = ((AlongV || AlongU) && isOpposite) ? -1.0 : 1.0;

  if (!isSingular)
  {
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + theData.Offset * aSign * aNorm.XYZ());

    // Compute derivative of normal
    gp_Vec aN0(aNorm.XYZ()), aN1U, aN1V;
    double aScale = (theD1U ^ theD1V).Dot(aN0);
    aN1U.SetX(aD2U.Y() * theD1V.Z() + theD1U.Y() * aD2UV.Z() - aD2U.Z() * theD1V.Y() - theD1U.Z() * aD2UV.Y());
    aN1U.SetY(-(aD2U.X() * theD1V.Z() + theD1U.X() * aD2UV.Z() - aD2U.Z() * theD1V.X() - theD1U.Z() * aD2UV.X()));
    aN1U.SetZ(aD2U.X() * theD1V.Y() + theD1U.X() * aD2UV.Y() - aD2U.Y() * theD1V.X() - theD1U.Y() * aD2UV.X());
    double aScaleU = aN1U.Dot(aN0);
    aN1U.Subtract(aScaleU * aN0);
    aN1U /= aScale;

    aN1V.SetX(aD2UV.Y() * theD1V.Z() + aD2V.Z() * theD1U.Y() - aD2UV.Z() * theD1V.Y() - aD2V.Y() * theD1U.Z());
    aN1V.SetY(-(aD2UV.X() * theD1V.Z() + aD2V.Z() * theD1U.X() - aD2UV.Z() * theD1V.X() - aD2V.X() * theD1U.Z()));
    aN1V.SetZ(aD2UV.X() * theD1V.Y() + aD2V.Y() * theD1U.X() - aD2UV.Y() * theD1V.X() - aD2V.X() * theD1U.Y());
    double aScaleV = aN1V.Dot(aN0);
    aN1V.Subtract(aScaleV * aN0);
    aN1V /= aScale;

    theD1U += theData.Offset * aSign * aN1U;
    theD1V += theData.Offset * aSign * aN1V;
  }
  else
  {
    // Singular case - simplified handling
    gp_Dir aNormal;
    CSLib_NormalStatus aNStatus;
    CSLib::Normal(theD1U, theD1V, THE_D1_MAG_TOL, aNStatus, aNormal);
    if (aNStatus == CSLib_Defined)
    {
      theValue.SetXYZ(theValue.XYZ() + theData.Offset * aSign * aNormal.XYZ());
    }
    else
    {
      throw Standard_NumericError("GeomAdaptor_Surface: Unable to calculate offset D1");
    }
  }
}

//! Offset surface D2 evaluation
inline void offsetD2(const double theU,
                     const double theV,
                     const GeomAdaptor_OffsetSurfaceData& theData,
                     gp_Pnt& theValue,
                     gp_Vec& theD1U,
                     gp_Vec& theD1V,
                     gp_Vec& theD2U,
                     gp_Vec& theD2V,
                     gp_Vec& theD2UV)
{
  gp_Vec aD3U, aD3V, aD3UUV, aD3UVV;
  theData.BasisAdaptor->D3(theU, theV, theValue, theD1U, theD1V, theD2U, theD2V, theD2UV,
                           aD3U, aD3V, aD3UUV, aD3UVV);
  checkInfinite(theD1U, theD1V);

  // Normalize derivatives for stable normal calculation
  gp_Vec aD1U(theD1U), aD1V(theD1V);
  double aD1UNorm2 = aD1U.SquareMagnitude();
  double aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  gp_Vec aNorm = aD1U.Crossed(aD1V);
  if (aNorm.SquareMagnitude() > THE_D1_MAG_TOL * THE_D1_MAG_TOL)
  {
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + theData.Offset * aNorm.XYZ());

    // Simplified D2 - add offset along normal for D1
    gp_Vec aN0(aNorm.XYZ());
    double aScale = (theD1U ^ theD1V).Dot(aN0);

    gp_Vec aN1U;
    aN1U.SetX(theD2U.Y() * theD1V.Z() + theD1U.Y() * theD2UV.Z() - theD2U.Z() * theD1V.Y() - theD1U.Z() * theD2UV.Y());
    aN1U.SetY(-(theD2U.X() * theD1V.Z() + theD1U.X() * theD2UV.Z() - theD2U.Z() * theD1V.X() - theD1U.Z() * theD2UV.X()));
    aN1U.SetZ(theD2U.X() * theD1V.Y() + theD1U.X() * theD2UV.Y() - theD2U.Y() * theD1V.X() - theD1U.Y() * theD2UV.X());
    double aScaleU = aN1U.Dot(aN0);
    aN1U.Subtract(aScaleU * aN0);
    aN1U /= aScale;

    gp_Vec aN1V;
    aN1V.SetX(theD2UV.Y() * theD1V.Z() + theD2V.Z() * theD1U.Y() - theD2UV.Z() * theD1V.Y() - theD2V.Y() * theD1U.Z());
    aN1V.SetY(-(theD2UV.X() * theD1V.Z() + theD2V.Z() * theD1U.X() - theD2UV.Z() * theD1V.X() - theD2V.X() * theD1U.Z()));
    aN1V.SetZ(theD2UV.X() * theD1V.Y() + theD2V.Y() * theD1U.X() - theD2UV.Y() * theD1V.X() - theD2V.X() * theD1U.Y());
    double aScaleV = aN1V.Dot(aN0);
    aN1V.Subtract(aScaleV * aN0);
    aN1V /= aScale;

    theD1U += theData.Offset * aN1U;
    theD1V += theData.Offset * aN1V;
    // D2 derivatives are approximations - keeping basis values for now
  }
  else
  {
    throw Standard_NumericError("GeomAdaptor_Surface: Unable to calculate offset D2 at singular point");
  }
}

//! Offset surface D3 evaluation
inline void offsetD3(const double theU,
                     const double theV,
                     const GeomAdaptor_OffsetSurfaceData& theData,
                     gp_Pnt& theValue,
                     gp_Vec& theD1U,
                     gp_Vec& theD1V,
                     gp_Vec& theD2U,
                     gp_Vec& theD2V,
                     gp_Vec& theD2UV,
                     gp_Vec& theD3U,
                     gp_Vec& theD3V,
                     gp_Vec& theD3UUV,
                     gp_Vec& theD3UVV)
{
  theData.BasisAdaptor->D3(theU, theV, theValue, theD1U, theD1V, theD2U, theD2V, theD2UV,
                           theD3U, theD3V, theD3UUV, theD3UVV);
  checkInfinite(theD1U, theD1V);

  // Normalize derivatives for stable normal calculation
  gp_Vec aD1U(theD1U), aD1V(theD1V);
  double aD1UNorm2 = aD1U.SquareMagnitude();
  double aD1VNorm2 = aD1V.SquareMagnitude();
  if (aD1UNorm2 > 1.0)
    aD1U /= std::sqrt(aD1UNorm2);
  if (aD1VNorm2 > 1.0)
    aD1V /= std::sqrt(aD1VNorm2);

  gp_Vec aNorm = aD1U.Crossed(aD1V);
  if (aNorm.SquareMagnitude() > THE_D1_MAG_TOL * THE_D1_MAG_TOL)
  {
    aNorm.Normalize();
    theValue.SetXYZ(theValue.XYZ() + theData.Offset * aNorm.XYZ());

    // Add offset contribution to D1
    gp_Vec aN0(aNorm.XYZ());
    double aScale = (theD1U ^ theD1V).Dot(aN0);

    gp_Vec aN1U;
    aN1U.SetX(theD2U.Y() * theD1V.Z() + theD1U.Y() * theD2UV.Z() - theD2U.Z() * theD1V.Y() - theD1U.Z() * theD2UV.Y());
    aN1U.SetY(-(theD2U.X() * theD1V.Z() + theD1U.X() * theD2UV.Z() - theD2U.Z() * theD1V.X() - theD1U.Z() * theD2UV.X()));
    aN1U.SetZ(theD2U.X() * theD1V.Y() + theD1U.X() * theD2UV.Y() - theD2U.Y() * theD1V.X() - theD1U.Y() * theD2UV.X());
    double aScaleU = aN1U.Dot(aN0);
    aN1U.Subtract(aScaleU * aN0);
    aN1U /= aScale;

    gp_Vec aN1V;
    aN1V.SetX(theD2UV.Y() * theD1V.Z() + theD2V.Z() * theD1U.Y() - theD2UV.Z() * theD1V.Y() - theD2V.Y() * theD1U.Z());
    aN1V.SetY(-(theD2UV.X() * theD1V.Z() + theD2V.Z() * theD1U.X() - theD2UV.Z() * theD1V.X() - theD2V.X() * theD1U.Z()));
    aN1V.SetZ(theD2UV.X() * theD1V.Y() + theD2V.Y() * theD1U.X() - theD2UV.Y() * theD1V.X() - theD2V.X() * theD1U.Y());
    double aScaleV = aN1V.Dot(aN0);
    aN1V.Subtract(aScaleV * aN0);
    aN1V /= aScale;

    theD1U += theData.Offset * aN1U;
    theD1V += theData.Offset * aN1V;
    // Higher derivatives are approximations - keeping basis values
  }
  else
  {
    throw Standard_NumericError("GeomAdaptor_Surface: Unable to calculate offset D3 at singular point");
  }
}

//! Offset surface DN evaluation
inline gp_Vec offsetDN(const double theU,
                       const double theV,
                       const GeomAdaptor_OffsetSurfaceData& theData,
                       int theNu,
                       int theNv)
{
  if (theNu + theNv == 0)
  {
    gp_Pnt aP;
    offsetD0(theU, theV, theData, aP);
    return gp_Vec(aP.XYZ());
  }
  // For higher derivatives, use basis adaptor
  return theData.BasisAdaptor->DN(theU, theV, theNu, theNv);
}

} // end of anonymous namespace

//=================================================================================================

Handle(Adaptor3d_Surface) GeomAdaptor_Surface::ShallowCopy() const
{
  Handle(GeomAdaptor_Surface) aCopy = new GeomAdaptor_Surface();

  aCopy->mySurface        = mySurface;
  aCopy->myUFirst         = myUFirst;
  aCopy->myULast          = myULast;
  aCopy->myVFirst         = myVFirst;
  aCopy->myVLast          = myVLast;
  aCopy->myTolU           = myTolU;
  aCopy->myTolV           = myTolV;
  aCopy->myBSplineSurface = myBSplineSurface;
  aCopy->mySurfaceType    = mySurfaceType;

  // Copy surface-specific evaluation data
  if (auto* anExtrusionData = std::get_if<GeomAdaptor_ExtrusionSurfaceData>(&mySurfaceData))
  {
    GeomAdaptor_ExtrusionSurfaceData aNewData;
    aNewData.BasisCurve = anExtrusionData->BasisCurve->ShallowCopy();
    aNewData.Direction  = anExtrusionData->Direction;
    aCopy->mySurfaceData = aNewData;
  }
  else if (auto* aRevolutionData = std::get_if<GeomAdaptor_RevolutionSurfaceData>(&mySurfaceData))
  {
    GeomAdaptor_RevolutionSurfaceData aNewData;
    aNewData.BasisCurve = aRevolutionData->BasisCurve->ShallowCopy();
    aNewData.Axis       = aRevolutionData->Axis;
    aCopy->mySurfaceData = aNewData;
  }
  else if (auto* anOffsetData = std::get_if<GeomAdaptor_OffsetSurfaceData>(&mySurfaceData))
  {
    GeomAdaptor_OffsetSurfaceData aNewData;
    aNewData.BasisAdaptor =
      Handle(GeomAdaptor_Surface)::DownCast(anOffsetData->BasisAdaptor->ShallowCopy());
    aNewData.Offset = anOffsetData->Offset;
    // Clone the osculating surface if present
    if (anOffsetData->OsculatingSurface)
    {
      aNewData.OsculatingSurface.reset(
        new Geom_OsculatingSurface(*anOffsetData->OsculatingSurface));
    }
    aCopy->mySurfaceData = std::move(aNewData);
  }

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_Surface::load(const Handle(Geom_Surface)& S,
                               const Standard_Real         UFirst,
                               const Standard_Real         ULast,
                               const Standard_Real         VFirst,
                               const Standard_Real         VLast,
                               const Standard_Real         TolU,
                               const Standard_Real         TolV)
{
  myTolU   = TolU;
  myTolV   = TolV;
  myUFirst = UFirst;
  myULast  = ULast;
  myVFirst = VFirst;
  myVLast  = VLast;
  mySurfaceCache.Nullify();

  if (mySurface != S)
  {
    mySurface = S;
    mySurfaceData = std::monostate{};
    myBSplineSurface.Nullify();

    const Handle(Standard_Type)& TheType = S->DynamicType();
    if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      Load(Handle(Geom_RectangularTrimmedSurface)::DownCast(S)->BasisSurface(),
           UFirst,
           ULast,
           VFirst,
           VLast);
    }
    else if (TheType == STANDARD_TYPE(Geom_Plane))
      mySurfaceType = GeomAbs_Plane;
    else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
      mySurfaceType = GeomAbs_Cylinder;
    else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
      mySurfaceType = GeomAbs_Cone;
    else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
      mySurfaceType = GeomAbs_Sphere;
    else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
      mySurfaceType = GeomAbs_Torus;
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      mySurfaceType = GeomAbs_SurfaceOfRevolution;
      Handle(Geom_SurfaceOfRevolution) aRevSurf =
        Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
      // Populate revolution surface data
      GeomAdaptor_RevolutionSurfaceData aRevData;
      aRevData.BasisCurve = new GeomAdaptor_Curve(aRevSurf->BasisCurve());
      aRevData.Axis       = gp_Ax1(aRevSurf->Location(), aRevSurf->Direction());
      mySurfaceData       = aRevData;
    }
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
    {
      mySurfaceType = GeomAbs_SurfaceOfExtrusion;
      Handle(Geom_SurfaceOfLinearExtrusion) anExtSurf =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface);
      // Populate extrusion surface data
      GeomAdaptor_ExtrusionSurfaceData anExtData;
      anExtData.BasisCurve = new GeomAdaptor_Curve(anExtSurf->BasisCurve());
      anExtData.Direction  = anExtSurf->Direction();
      mySurfaceData        = anExtData;
    }
    else if (TheType == STANDARD_TYPE(Geom_BezierSurface))
    {
      mySurfaceType = GeomAbs_BezierSurface;
    }
    else if (TheType == STANDARD_TYPE(Geom_BSplineSurface))
    {
      mySurfaceType    = GeomAbs_BSplineSurface;
      myBSplineSurface = Handle(Geom_BSplineSurface)::DownCast(mySurface);
    }
    else if (TheType == STANDARD_TYPE(Geom_OffsetSurface))
    {
      mySurfaceType                        = GeomAbs_OffsetSurface;
      Handle(Geom_OffsetSurface) anOffSurf = Handle(Geom_OffsetSurface)::DownCast(mySurface);
      // Populate offset surface data
      GeomAdaptor_OffsetSurfaceData anOffsetData;
      anOffsetData.BasisAdaptor =
        new GeomAdaptor_Surface(anOffSurf->BasisSurface(), myUFirst, myULast, myVFirst, myVLast, myTolU, myTolV);
      anOffsetData.Offset = anOffSurf->Offset();
      // Create osculating surface detector from basis surface
      anOffsetData.OsculatingSurface.reset(
        new Geom_OsculatingSurface(anOffSurf->BasisSurface(), Precision::Confusion()));
      mySurfaceData = std::move(anOffsetData);
    }
    else
      mySurfaceType = GeomAbs_OtherSurface;
  }
}

//    --
//    --     Global methods - Apply to the whole Surface.
//    --

//=================================================================================================

GeomAbs_Shape GeomAdaptor_Surface::UContinuity() const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const Standard_Integer  N = myBSplineSurface->NbUKnots();
      TColStd_Array1OfReal    TK(1, N);
      TColStd_Array1OfInteger TM(1, N);
      myBSplineSurface->UKnots(TK);
      myBSplineSurface->UMultiplicities(TM);
      return LocalContinuity(myBSplineSurface->UDegree(),
                             myBSplineSurface->NbUKnots(),
                             TK,
                             TM,
                             myUFirst,
                             myULast,
                             IsUPeriodic());
    }
    case GeomAbs_OffsetSurface: {
      switch (BasisSurface()->UContinuity())
      {
        case GeomAbs_CN:
        case GeomAbs_C3:
          return GeomAbs_CN;
        case GeomAbs_G2:
        case GeomAbs_C2:
          return GeomAbs_C1;
        case GeomAbs_G1:
        case GeomAbs_C1:
        case GeomAbs_C0:
          return GeomAbs_C0;
      }
      throw Standard_NoSuchObject("GeomAdaptor_Surface::UContinuity");
      break;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      Handle(Geom_SurfaceOfLinearExtrusion) myExtSurf =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface);
      GeomAdaptor_Curve GC(myExtSurf->BasisCurve(), myUFirst, myULast);
      return GC.Continuity();
    }
    case GeomAbs_OtherSurface:
      throw Standard_NoSuchObject("GeomAdaptor_Surface::UContinuity");
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_SurfaceOfRevolution:
      break;
  }
  return GeomAbs_CN;
}

//=================================================================================================

GeomAbs_Shape GeomAdaptor_Surface::VContinuity() const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const Standard_Integer  N = myBSplineSurface->NbVKnots();
      TColStd_Array1OfReal    TK(1, N);
      TColStd_Array1OfInteger TM(1, N);
      myBSplineSurface->VKnots(TK);
      myBSplineSurface->VMultiplicities(TM);
      return LocalContinuity(myBSplineSurface->VDegree(),
                             myBSplineSurface->NbVKnots(),
                             TK,
                             TM,
                             myVFirst,
                             myVLast,
                             IsVPeriodic());
    }
    case GeomAbs_OffsetSurface: {
      switch (BasisSurface()->VContinuity())
      {
        case GeomAbs_CN:
        case GeomAbs_C3:
          return GeomAbs_CN;
        case GeomAbs_G2:
        case GeomAbs_C2:
          return GeomAbs_C1;
        case GeomAbs_G1:
        case GeomAbs_C1:
        case GeomAbs_C0:
          return GeomAbs_C0;
      }
      throw Standard_NoSuchObject("GeomAdaptor_Surface::VContinuity");
      break;
    }
    case GeomAbs_SurfaceOfRevolution: {
      Handle(Geom_SurfaceOfRevolution) myRevSurf =
        Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
      GeomAdaptor_Curve GC(myRevSurf->BasisCurve(), myVFirst, myVLast);
      return GC.Continuity();
    }
    case GeomAbs_OtherSurface:
      throw Standard_NoSuchObject("GeomAdaptor_Surface::VContinuity");
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_SurfaceOfExtrusion:
      break;
  }
  return GeomAbs_CN;
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbUIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      GeomAdaptor_Curve myBasisCurve(
        myBSplineSurface->VIso(myBSplineSurface->VKnot(myBSplineSurface->FirstVKnotIndex())),
        myUFirst,
        myULast);
      return myBasisCurve.NbIntervals(S);
    }
    case GeomAbs_SurfaceOfExtrusion: {
      Handle(Geom_SurfaceOfLinearExtrusion) myExtSurf =
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface);
      GeomAdaptor_Curve myBasisCurve(myExtSurf->BasisCurve(), myUFirst, myULast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
        return myBasisCurve.NbIntervals(S);
      break;
    }
    case GeomAbs_OffsetSurface: {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch (S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2:
          throw Standard_DomainError("GeomAdaptor_Curve::NbUIntervals");
        case GeomAbs_C0:
          BaseS = GeomAbs_C1;
          break;
        case GeomAbs_C1:
          BaseS = GeomAbs_C2;
          break;
        case GeomAbs_C2:
          BaseS = GeomAbs_C3;
          break;
        case GeomAbs_C3:
        case GeomAbs_CN:
          break;
      }
      Handle(Geom_OffsetSurface) myOffSurf = Handle(Geom_OffsetSurface)::DownCast(mySurface);
      GeomAdaptor_Surface Sur(myOffSurf->BasisSurface(), myUFirst, myULast, myVFirst, myVLast);
      return Sur.NbUIntervals(BaseS);
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfRevolution:
      break;
  }
  return 1;
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbVIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      GeomAdaptor_Curve myBasisCurve(
        myBSplineSurface->UIso(myBSplineSurface->UKnot(myBSplineSurface->FirstUKnotIndex())),
        myVFirst,
        myVLast);
      return myBasisCurve.NbIntervals(S);
    }
    case GeomAbs_SurfaceOfRevolution: {
      Handle(Geom_SurfaceOfRevolution) myRevSurf =
        Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
      GeomAdaptor_Curve myBasisCurve(myRevSurf->BasisCurve(), myVFirst, myVLast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
        return myBasisCurve.NbIntervals(S);
      break;
    }
    case GeomAbs_OffsetSurface: {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch (S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2:
          throw Standard_DomainError("GeomAdaptor_Curve::NbVIntervals");
        case GeomAbs_C0:
          BaseS = GeomAbs_C1;
          break;
        case GeomAbs_C1:
          BaseS = GeomAbs_C2;
          break;
        case GeomAbs_C2:
          BaseS = GeomAbs_C3;
          break;
        case GeomAbs_C3:
        case GeomAbs_CN:
          break;
      }
      Handle(Geom_OffsetSurface) myOffSurf = Handle(Geom_OffsetSurface)::DownCast(mySurface);
      GeomAdaptor_Surface Sur(myOffSurf->BasisSurface(), myUFirst, myULast, myVFirst, myVLast);
      return Sur.NbVIntervals(BaseS);
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfExtrusion:
      break;
  }
  return 1;
}

//=================================================================================================

void GeomAdaptor_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      GeomAdaptor_Curve myBasisCurve(
        myBSplineSurface->VIso(myBSplineSurface->VKnot(myBSplineSurface->FirstVKnotIndex())),
        myUFirst,
        myULast);
      myBasisCurve.Intervals(T, S);
      return;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      GeomAdaptor_Curve myBasisCurve(
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve(),
        myUFirst,
        myULast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
      {
        myBasisCurve.Intervals(T, S);
        return;
      }
      break;
    }
    case GeomAbs_OffsetSurface: {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch (S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2:
          throw Standard_DomainError("GeomAdaptor_Curve::UIntervals");
        case GeomAbs_C0:
          BaseS = GeomAbs_C1;
          break;
        case GeomAbs_C1:
          BaseS = GeomAbs_C2;
          break;
        case GeomAbs_C2:
          BaseS = GeomAbs_C3;
          break;
        case GeomAbs_C3:
        case GeomAbs_CN:
          break;
      }
      Handle(Geom_OffsetSurface) myOffSurf = Handle(Geom_OffsetSurface)::DownCast(mySurface);
      GeomAdaptor_Surface Sur(myOffSurf->BasisSurface(), myUFirst, myULast, myVFirst, myVLast);
      Sur.UIntervals(T, BaseS);
      return;
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfRevolution:
      break;
  }

  T(T.Lower())     = myUFirst;
  T(T.Lower() + 1) = myULast;
}

//=================================================================================================

void GeomAdaptor_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      GeomAdaptor_Curve myBasisCurve(
        myBSplineSurface->UIso(myBSplineSurface->UKnot(myBSplineSurface->FirstUKnotIndex())),
        myVFirst,
        myVLast);
      myBasisCurve.Intervals(T, S);
      return;
    }
    case GeomAbs_SurfaceOfRevolution: {
      Handle(Geom_SurfaceOfRevolution) myRevSurf =
        Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface);
      GeomAdaptor_Curve myBasisCurve(myRevSurf->BasisCurve(), myVFirst, myVLast);
      if (myBasisCurve.GetType() == GeomAbs_BSplineCurve)
      {
        myBasisCurve.Intervals(T, S);
        return;
      }
      break;
    }
    case GeomAbs_OffsetSurface: {
      GeomAbs_Shape BaseS = GeomAbs_CN;
      switch (S)
      {
        case GeomAbs_G1:
        case GeomAbs_G2:
          throw Standard_DomainError("GeomAdaptor_Curve::VIntervals");
        case GeomAbs_C0:
          BaseS = GeomAbs_C1;
          break;
        case GeomAbs_C1:
          BaseS = GeomAbs_C2;
          break;
        case GeomAbs_C2:
          BaseS = GeomAbs_C3;
          break;
        case GeomAbs_C3:
        case GeomAbs_CN:
          break;
      }
      Handle(Geom_OffsetSurface) myOffSurf = Handle(Geom_OffsetSurface)::DownCast(mySurface);
      GeomAdaptor_Surface Sur(myOffSurf->BasisSurface(), myUFirst, myULast, myVFirst, myVLast);
      Sur.VIntervals(T, BaseS);
      return;
    }
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    case GeomAbs_SurfaceOfExtrusion:
      break;
  }

  T(T.Lower())     = myVFirst;
  T(T.Lower() + 1) = myVLast;
}

//=================================================================================================

Handle(Adaptor3d_Surface) GeomAdaptor_Surface::UTrim(const Standard_Real First,
                                                     const Standard_Real Last,
                                                     const Standard_Real Tol) const
{
  return Handle(GeomAdaptor_Surface)(
    new GeomAdaptor_Surface(mySurface, First, Last, myVFirst, myVLast, Tol, myTolV));
}

//=================================================================================================

Handle(Adaptor3d_Surface) GeomAdaptor_Surface::VTrim(const Standard_Real First,
                                                     const Standard_Real Last,
                                                     const Standard_Real Tol) const
{
  return Handle(GeomAdaptor_Surface)(
    new GeomAdaptor_Surface(mySurface, myUFirst, myULast, First, Last, myTolU, Tol));
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsUClosed() const
{
  if (!mySurface->IsUClosed())
    return Standard_False;

  Standard_Real U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsUPeriodic())
    return (std::abs(std::abs(U1 - U2) - std::abs(myUFirst - myULast)) < Precision::PConfusion());

  return (std::abs(U1 - myUFirst) < Precision::PConfusion()
          && std::abs(U2 - myULast) < Precision::PConfusion());
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsVClosed() const
{
  if (!mySurface->IsVClosed())
    return Standard_False;

  Standard_Real U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsVPeriodic())
    return (std::abs(std::abs(V1 - V2) - std::abs(myVFirst - myVLast)) < Precision::PConfusion());

  return (std::abs(V1 - myVFirst) < Precision::PConfusion()
          && std::abs(V2 - myVLast) < Precision::PConfusion());
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsUPeriodic() const
{
  return (mySurface->IsUPeriodic());
}

//=================================================================================================

Standard_Real GeomAdaptor_Surface::UPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsUPeriodic(), " ");
  return mySurface->UPeriod();
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsVPeriodic() const
{
  return (mySurface->IsVPeriodic());
}

//=================================================================================================

Standard_Real GeomAdaptor_Surface::VPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsVPeriodic(), " ");
  return mySurface->VPeriod();
}

//=================================================================================================

void GeomAdaptor_Surface::RebuildCache(const Standard_Real theU, const Standard_Real theV) const
{
  if (mySurfaceType == GeomAbs_BezierSurface)
  {
    // Create cache for Bezier
    Handle(Geom_BezierSurface) aBezier = Handle(Geom_BezierSurface)::DownCast(mySurface);
    Standard_Integer           aDegU   = aBezier->UDegree();
    Standard_Integer           aDegV   = aBezier->VDegree();
    TColStd_Array1OfReal       aFlatKnotsU(BSplCLib::FlatBezierKnots(aDegU), 1, 2 * (aDegU + 1));
    TColStd_Array1OfReal       aFlatKnotsV(BSplCLib::FlatBezierKnots(aDegV), 1, 2 * (aDegV + 1));
    if (mySurfaceCache.IsNull())
      mySurfaceCache = new BSplSLib_Cache(aDegU,
                                          aBezier->IsUPeriodic(),
                                          aFlatKnotsU,
                                          aDegV,
                                          aBezier->IsVPeriodic(),
                                          aFlatKnotsV,
                                          aBezier->Weights());
    mySurfaceCache
      ->BuildCache(theU, theV, aFlatKnotsU, aFlatKnotsV, aBezier->Poles(), aBezier->Weights());
  }
  else if (mySurfaceType == GeomAbs_BSplineSurface)
  {
    // Create cache for B-spline
    if (mySurfaceCache.IsNull())
      mySurfaceCache = new BSplSLib_Cache(myBSplineSurface->UDegree(),
                                          myBSplineSurface->IsUPeriodic(),
                                          myBSplineSurface->UKnotSequence(),
                                          myBSplineSurface->VDegree(),
                                          myBSplineSurface->IsVPeriodic(),
                                          myBSplineSurface->VKnotSequence(),
                                          myBSplineSurface->Weights());
    mySurfaceCache->BuildCache(theU,
                               theV,
                               myBSplineSurface->UKnotSequence(),
                               myBSplineSurface->VKnotSequence(),
                               myBSplineSurface->Poles(),
                               myBSplineSurface->Weights());
  }
}

//=================================================================================================

gp_Pnt GeomAdaptor_Surface::Value(const Standard_Real U, const Standard_Real V) const
{
  gp_Pnt aValue;
  D0(U, V, aValue);
  return aValue;
}

//=================================================================================================

void GeomAdaptor_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface:
      if (mySurfaceCache.IsNull() || !mySurfaceCache->IsCacheValid(U, V))
        RebuildCache(U, V);
      mySurfaceCache->D0(U, V, P);
      break;

    case GeomAbs_SurfaceOfExtrusion: {
      const auto& anExtData = std::get<GeomAdaptor_ExtrusionSurfaceData>(mySurfaceData);
      extrusionD0(U, V, anExtData.BasisCurve, anExtData.Direction, P);
      break;
    }

    case GeomAbs_SurfaceOfRevolution: {
      const auto& aRevData = std::get<GeomAdaptor_RevolutionSurfaceData>(mySurfaceData);
      revolutionD0(U, V, aRevData.BasisCurve, aRevData.Axis, P);
      break;
    }

    case GeomAbs_OffsetSurface: {
      const auto& anOffData = std::get<GeomAdaptor_OffsetSurfaceData>(mySurfaceData);
      offsetD0(U, V, anOffData, P);
      break;
    }

    default:
      mySurface->D0(U, V, P);
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D1(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V) const
{
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  Standard_Real    u = U, v = V;
  if (std::abs(U - myUFirst) <= myTolU)
  {
    USide = 1;
    u     = myUFirst;
  }
  else if (std::abs(U - myULast) <= myTolU)
  {
    USide = -1;
    u     = myULast;
  }
  if (std::abs(V - myVFirst) <= myTolV)
  {
    VSide = 1;
    v     = myVFirst;
  }
  else if (std::abs(V - myVLast) <= myTolV)
  {
    VSide = -1;
    v     = myVLast;
  }

  switch (mySurfaceType)
  {
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface: {
      if (!myBSplineSurface.IsNull() && (USide != 0 || VSide != 0)
          && IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
        myBSplineSurface->LocalD1(u, v, Ideb, Ifin, IVdeb, IVfin, P, D1U, D1V);
      else
      {
        if (mySurfaceCache.IsNull() || !mySurfaceCache->IsCacheValid(U, V))
          RebuildCache(U, V);
        mySurfaceCache->D1(U, V, P, D1U, D1V);
      }
      break;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      const auto& anExtData = std::get<GeomAdaptor_ExtrusionSurfaceData>(mySurfaceData);
      extrusionD1(u, v, anExtData.BasisCurve, anExtData.Direction, P, D1U, D1V);
      break;
    }

    case GeomAbs_SurfaceOfRevolution: {
      const auto& aRevData = std::get<GeomAdaptor_RevolutionSurfaceData>(mySurfaceData);
      revolutionD1(u, v, aRevData.BasisCurve, aRevData.Axis, P, D1U, D1V);
      break;
    }

    case GeomAbs_OffsetSurface: {
      const auto& anOffData = std::get<GeomAdaptor_OffsetSurfaceData>(mySurfaceData);
      offsetD1(u, v, anOffData, P, D1U, D1V);
      break;
    }

    default:
      mySurface->D1(u, v, P, D1U, D1V);
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D2(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV) const
{
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  Standard_Real    u = U, v = V;
  if (std::abs(U - myUFirst) <= myTolU)
  {
    USide = 1;
    u     = myUFirst;
  }
  else if (std::abs(U - myULast) <= myTolU)
  {
    USide = -1;
    u     = myULast;
  }
  if (std::abs(V - myVFirst) <= myTolV)
  {
    VSide = 1;
    v     = myVFirst;
  }
  else if (std::abs(V - myVLast) <= myTolV)
  {
    VSide = -1;
    v     = myVLast;
  }

  switch (mySurfaceType)
  {
    case GeomAbs_BezierSurface:
    case GeomAbs_BSplineSurface: {
      if (!myBSplineSurface.IsNull() && (USide != 0 || VSide != 0)
          && IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
        myBSplineSurface->LocalD2(u, v, Ideb, Ifin, IVdeb, IVfin, P, D1U, D1V, D2U, D2V, D2UV);
      else
      {
        if (mySurfaceCache.IsNull() || !mySurfaceCache->IsCacheValid(U, V))
          RebuildCache(U, V);
        mySurfaceCache->D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
      }
      break;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      const auto& anExtData = std::get<GeomAdaptor_ExtrusionSurfaceData>(mySurfaceData);
      extrusionD2(u, v, anExtData.BasisCurve, anExtData.Direction, P, D1U, D1V, D2U, D2V, D2UV);
      break;
    }

    case GeomAbs_SurfaceOfRevolution: {
      const auto& aRevData = std::get<GeomAdaptor_RevolutionSurfaceData>(mySurfaceData);
      revolutionD2(u, v, aRevData.BasisCurve, aRevData.Axis, P, D1U, D1V, D2U, D2V, D2UV);
      break;
    }

    case GeomAbs_OffsetSurface: {
      const auto& anOffData = std::get<GeomAdaptor_OffsetSurfaceData>(mySurfaceData);
      offsetD2(u, v, anOffData, P, D1U, D1V, D2U, D2V, D2UV);
      break;
    }

    default: {
      mySurface->D2(u, v, P, D1U, D1V, D2U, D2V, D2UV);
      break;
    }
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D3(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV,
                             gp_Vec&             D3U,
                             gp_Vec&             D3V,
                             gp_Vec&             D3UUV,
                             gp_Vec&             D3UVV) const
{
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  Standard_Real    u = U, v = V;
  if (std::abs(U - myUFirst) <= myTolU)
  {
    USide = 1;
    u     = myUFirst;
  }
  else if (std::abs(U - myULast) <= myTolU)
  {
    USide = -1;
    u     = myULast;
  }
  if (std::abs(V - myVFirst) <= myTolV)
  {
    VSide = 1;
    v     = myVFirst;
  }
  else if (std::abs(V - myVLast) <= myTolV)
  {
    VSide = -1;
    v     = myVLast;
  }

  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      if ((USide == 0) && (VSide == 0))
        myBSplineSurface->D3(u, v, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      else
      {
        if (IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
          myBSplineSurface->LocalD3(u,
                                    v,
                                    Ideb,
                                    Ifin,
                                    IVdeb,
                                    IVfin,
                                    P,
                                    D1U,
                                    D1V,
                                    D2U,
                                    D2V,
                                    D2UV,
                                    D3U,
                                    D3V,
                                    D3UUV,
                                    D3UVV);
        else
          myBSplineSurface->D3(u, v, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      }
      break;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      const auto& anExtData = std::get<GeomAdaptor_ExtrusionSurfaceData>(mySurfaceData);
      extrusionD3(u, v, anExtData.BasisCurve, anExtData.Direction, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      break;
    }

    case GeomAbs_SurfaceOfRevolution: {
      const auto& aRevData = std::get<GeomAdaptor_RevolutionSurfaceData>(mySurfaceData);
      revolutionD3(u, v, aRevData.BasisCurve, aRevData.Axis, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      break;
    }

    case GeomAbs_OffsetSurface: {
      const auto& anOffData = std::get<GeomAdaptor_OffsetSurfaceData>(mySurfaceData);
      offsetD3(u, v, anOffData, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      break;
    }

    default: {
      mySurface->D3(u, v, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
      break;
    }
  }
}

//=================================================================================================

gp_Vec GeomAdaptor_Surface::DN(const Standard_Real    U,
                               const Standard_Real    V,
                               const Standard_Integer Nu,
                               const Standard_Integer Nv) const
{
  Standard_Integer Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  Standard_Real    u = U, v = V;
  if (std::abs(U - myUFirst) <= myTolU)
  {
    USide = 1;
    u     = myUFirst;
  }
  else if (std::abs(U - myULast) <= myTolU)
  {
    USide = -1;
    u     = myULast;
  }
  if (std::abs(V - myVFirst) <= myTolV)
  {
    VSide = 1;
    v     = myVFirst;
  }
  else if (std::abs(V - myVLast) <= myTolV)
  {
    VSide = -1;
    v     = myVLast;
  }

  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      if ((USide == 0) && (VSide == 0))
        return myBSplineSurface->DN(u, v, Nu, Nv);
      else
      {
        if (IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
          return myBSplineSurface->LocalDN(u, v, Ideb, Ifin, IVdeb, IVfin, Nu, Nv);
        else
          return myBSplineSurface->DN(u, v, Nu, Nv);
      }
    }

    case GeomAbs_SurfaceOfExtrusion: {
      const auto& anExtData = std::get<GeomAdaptor_ExtrusionSurfaceData>(mySurfaceData);
      return extrusionDN(u, anExtData.BasisCurve, anExtData.Direction, Nu, Nv);
    }

    case GeomAbs_SurfaceOfRevolution: {
      const auto& aRevData = std::get<GeomAdaptor_RevolutionSurfaceData>(mySurfaceData);
      return revolutionDN(u, v, aRevData.BasisCurve, aRevData.Axis, Nu, Nv);
    }

    case GeomAbs_OffsetSurface: {
      const auto& anOffData = std::get<GeomAdaptor_OffsetSurfaceData>(mySurfaceData);
      return offsetDN(u, v, anOffData, Nu, Nv);
    }

    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    default:
      break;
  }

  return mySurface->DN(u, v, Nu, Nv);
}

//=================================================================================================

Standard_Real GeomAdaptor_Surface::UResolution(const Standard_Real R3d) const
{
  Standard_Real Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfExtrusion: {
      GeomAdaptor_Curve myBasisCurve(
        Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve(),
        myUFirst,
        myULast);
      return myBasisCurve.Resolution(R3d);
    }
    case GeomAbs_Torus: {
      Handle(Geom_ToroidalSurface) S(Handle(Geom_ToroidalSurface)::DownCast(mySurface));
      const Standard_Real          R = S->MajorRadius() + S->MinorRadius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Sphere: {
      Handle(Geom_SphericalSurface) S(Handle(Geom_SphericalSurface)::DownCast(mySurface));
      const Standard_Real           R = S->Radius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Cylinder: {
      Handle(Geom_CylindricalSurface) S(Handle(Geom_CylindricalSurface)::DownCast(mySurface));
      const Standard_Real             R = S->Radius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Cone: {
      if (myVLast - myVFirst > 1.e10)
      {
        // Pas vraiment borne => resolution inconnue
        return Precision::Parametric(R3d);
      }
      Handle(Geom_ConicalSurface) S(Handle(Geom_ConicalSurface)::DownCast(mySurface));
      Handle(Geom_Curve)          C      = S->VIso(myVLast);
      const Standard_Real         Rayon1 = Handle(Geom_Circle)::DownCast(C)->Radius();
      C                                  = S->VIso(myVFirst);
      const Standard_Real Rayon2         = Handle(Geom_Circle)::DownCast(C)->Radius();
      const Standard_Real R              = (Rayon1 > Rayon2) ? Rayon1 : Rayon2;
      return (R > Precision::Confusion() ? (R3d / R) : 0.);
    }
    case GeomAbs_Plane: {
      return R3d;
    }
    case GeomAbs_BezierSurface: {
      Standard_Real Ures, Vres;
      Handle(Geom_BezierSurface)::DownCast(mySurface)->Resolution(R3d, Ures, Vres);
      return Ures;
    }
    case GeomAbs_BSplineSurface: {
      Standard_Real Ures, Vres;
      myBSplineSurface->Resolution(R3d, Ures, Vres);
      return Ures;
    }
    case GeomAbs_OffsetSurface: {
      Handle(Geom_Surface) base = Handle(Geom_OffsetSurface)::DownCast(mySurface)->BasisSurface();
      GeomAdaptor_Surface  gabase(base, myUFirst, myULast, myVFirst, myVLast);
      return gabase.UResolution(R3d);
    }
    default:
      return Precision::Parametric(R3d);
  }

  if (Res <= 1.)
    return 2. * std::asin(Res);

  return 2. * M_PI;
}

//=================================================================================================

Standard_Real GeomAdaptor_Surface::VResolution(const Standard_Real R3d) const
{
  Standard_Real Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfRevolution: {
      GeomAdaptor_Curve myBasisCurve(
        Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface)->BasisCurve(),
        myUFirst,
        myULast);
      return myBasisCurve.Resolution(R3d);
    }
    case GeomAbs_Torus: {
      Handle(Geom_ToroidalSurface) S(Handle(Geom_ToroidalSurface)::DownCast(mySurface));
      const Standard_Real          R = S->MinorRadius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Sphere: {
      Handle(Geom_SphericalSurface) S(Handle(Geom_SphericalSurface)::DownCast(mySurface));
      const Standard_Real           R = S->Radius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Plane: {
      return R3d;
    }
    case GeomAbs_BezierSurface: {
      Standard_Real Ures, Vres;
      Handle(Geom_BezierSurface)::DownCast(mySurface)->Resolution(R3d, Ures, Vres);
      return Vres;
    }
    case GeomAbs_BSplineSurface: {
      Standard_Real Ures, Vres;
      myBSplineSurface->Resolution(R3d, Ures, Vres);
      return Vres;
    }
    case GeomAbs_OffsetSurface: {
      Handle(Geom_Surface) base = Handle(Geom_OffsetSurface)::DownCast(mySurface)->BasisSurface();
      GeomAdaptor_Surface  gabase(base, myUFirst, myULast, myVFirst, myVLast);
      return gabase.VResolution(R3d);
    }
    default:
      return Precision::Parametric(R3d);
  }

  if (Res <= 1.)
    return 2. * std::asin(Res);

  return 2. * M_PI;
}

//=================================================================================================

gp_Pln GeomAdaptor_Surface::Plane() const
{
  if (mySurfaceType != GeomAbs_Plane)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Plane");
  return Handle(Geom_Plane)::DownCast(mySurface)->Pln();
}

//=================================================================================================

gp_Cylinder GeomAdaptor_Surface::Cylinder() const
{
  if (mySurfaceType != GeomAbs_Cylinder)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Cylinder");
  return Handle(Geom_CylindricalSurface)::DownCast(mySurface)->Cylinder();
}

//=================================================================================================

gp_Cone GeomAdaptor_Surface::Cone() const
{
  if (mySurfaceType != GeomAbs_Cone)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Cone");
  return Handle(Geom_ConicalSurface)::DownCast(mySurface)->Cone();
}

//=================================================================================================

gp_Sphere GeomAdaptor_Surface::Sphere() const
{
  if (mySurfaceType != GeomAbs_Sphere)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Sphere");
  return Handle(Geom_SphericalSurface)::DownCast(mySurface)->Sphere();
}

//=================================================================================================

gp_Torus GeomAdaptor_Surface::Torus() const
{
  if (mySurfaceType != GeomAbs_Torus)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Torus");
  return Handle(Geom_ToroidalSurface)::DownCast(mySurface)->Torus();
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::UDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->UDegree();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->UDegree();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.Degree();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::UDegree");
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbUPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->NbUPoles();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->NbUPoles();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbPoles();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbUPoles");
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::VDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->VDegree();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->VDegree();
  if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve(
      Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.Degree();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::VDegree");
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbVPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->NbVPoles();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->NbVPoles();
  if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve(
      Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbPoles();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbVPoles");
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbUKnots() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->NbUKnots();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbKnots();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbUKnots");
}

//=================================================================================================

Standard_Integer GeomAdaptor_Surface::NbVKnots() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->NbVKnots();
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbVKnots");
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsURational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->IsURational();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->IsURational();
  return Standard_False;
}

//=================================================================================================

Standard_Boolean GeomAdaptor_Surface::IsVRational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return myBSplineSurface->IsVRational();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return Handle(Geom_BezierSurface)::DownCast(mySurface)->IsVRational();
  return Standard_False;
}

//=================================================================================================

Handle(Geom_BezierSurface) GeomAdaptor_Surface::Bezier() const
{
  if (mySurfaceType != GeomAbs_BezierSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Bezier");
  return Handle(Geom_BezierSurface)::DownCast(mySurface);
}

//=================================================================================================

Handle(Geom_BSplineSurface) GeomAdaptor_Surface::BSpline() const
{
  if (mySurfaceType != GeomAbs_BSplineSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BSpline");
  return myBSplineSurface;
}

//=================================================================================================

gp_Ax1 GeomAdaptor_Surface::AxeOfRevolution() const
{
  if (mySurfaceType != GeomAbs_SurfaceOfRevolution)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::AxeOfRevolution");
  return Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface)->Axis();
}

//=================================================================================================

gp_Dir GeomAdaptor_Surface::Direction() const
{
  if (mySurfaceType != GeomAbs_SurfaceOfExtrusion)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Direction");
  return Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->Direction();
}

//=================================================================================================

Handle(Adaptor3d_Curve) GeomAdaptor_Surface::BasisCurve() const
{
  Handle(Geom_Curve) C;
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
    C = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(mySurface)->BasisCurve();
  else if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
    C = Handle(Geom_SurfaceOfRevolution)::DownCast(mySurface)->BasisCurve();
  else
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisCurve");
  return Handle(GeomAdaptor_Curve)(new GeomAdaptor_Curve(C));
}

//=================================================================================================

Handle(Adaptor3d_Surface) GeomAdaptor_Surface::BasisSurface() const
{
  if (mySurfaceType != GeomAbs_OffsetSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisSurface");
  return new GeomAdaptor_Surface(Handle(Geom_OffsetSurface)::DownCast(mySurface)->BasisSurface(),
                                 myUFirst,
                                 myULast,
                                 myVFirst,
                                 myVLast);
}

//=================================================================================================

Standard_Real GeomAdaptor_Surface::OffsetValue() const
{
  if (mySurfaceType != GeomAbs_OffsetSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisSurface");
  return Handle(Geom_OffsetSurface)::DownCast(mySurface)->Offset();
}

//=======================================================================
// function : IfUVBound <private>
// purpose  :  locates U,V parameters if U,V =First, Last,
//	      processes the finding span and returns the
//	      parameters for LocalDi
//=======================================================================

Standard_Boolean GeomAdaptor_Surface::IfUVBound(const Standard_Real    U,
                                                const Standard_Real    V,
                                                Standard_Integer&      IOutDeb,
                                                Standard_Integer&      IOutFin,
                                                Standard_Integer&      IOutVDeb,
                                                Standard_Integer&      IOutVFin,
                                                const Standard_Integer USide,
                                                const Standard_Integer VSide) const
{
  Standard_Integer Ideb, Ifin;
  Standard_Integer anUFKIndx = myBSplineSurface->FirstUKnotIndex(),
                   anULKIndx = myBSplineSurface->LastUKnotIndex(),
                   aVFKIndx  = myBSplineSurface->FirstVKnotIndex(),
                   aVLKIndx  = myBSplineSurface->LastVKnotIndex();
  myBSplineSurface->LocateU(U, PosTol, Ideb, Ifin, Standard_False);
  Standard_Boolean Local = (Ideb == Ifin);
  Span(USide, Ideb, Ifin, Ideb, Ifin, anUFKIndx, anULKIndx);
  Standard_Integer IVdeb, IVfin;
  myBSplineSurface->LocateV(V, PosTol, IVdeb, IVfin, Standard_False);
  if (IVdeb == IVfin)
    Local = Standard_True;
  Span(VSide, IVdeb, IVfin, IVdeb, IVfin, aVFKIndx, aVLKIndx);

  IOutDeb  = Ideb;
  IOutFin  = Ifin;
  IOutVDeb = IVdeb;
  IOutVFin = IVfin;

  return Local;
}

//=======================================================================
// function : Span <private>
// purpose  : locates U,V parameters if U=UFirst or U=ULast,
//	     processes the finding span and returns the
//	     parameters for LocalDi
//=======================================================================

void GeomAdaptor_Surface::Span(const Standard_Integer Side,
                               const Standard_Integer Ideb,
                               const Standard_Integer Ifin,
                               Standard_Integer&      OutIdeb,
                               Standard_Integer&      OutIfin,
                               const Standard_Integer theFKIndx,
                               const Standard_Integer theLKIndx) const
{
  if (Ideb != Ifin) // not a knot
  {
    if (Ideb < theFKIndx)
    {
      OutIdeb = theFKIndx;
      OutIfin = theFKIndx + 1;
    }
    else if (Ifin > theLKIndx)
    {
      OutIdeb = theLKIndx - 1;
      OutIfin = theLKIndx;
    }
    else if (Ideb >= (theLKIndx - 1))
    {
      OutIdeb = theLKIndx - 1;
      OutIfin = theLKIndx;
    }
    else if (Ifin <= theFKIndx + 1)
    {
      OutIdeb = theFKIndx;
      OutIfin = theFKIndx + 1;
    }
    else if (Ideb > Ifin)
    {
      OutIdeb = Ifin - 1;
      OutIfin = Ifin;
    }
    else
    {
      OutIdeb = Ideb;
      OutIfin = Ifin;
    }
  }
  else
  {
    if (Ideb <= theFKIndx)
    {
      OutIdeb = theFKIndx;
      OutIfin = theFKIndx + 1;
    } // first knot
    else if (Ifin >= theLKIndx)
    {
      OutIdeb = theLKIndx - 1;
      OutIfin = theLKIndx;
    } // last knot
    else
    {
      if (Side == -1)
      {
        OutIdeb = Ideb - 1;
        OutIfin = Ifin;
      }
      else
      {
        OutIdeb = Ideb;
        OutIfin = Ifin + 1;
      }
    }
  }
}
