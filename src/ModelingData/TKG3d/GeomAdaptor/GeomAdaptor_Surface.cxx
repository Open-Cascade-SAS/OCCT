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
#include "../Geom/Geom_OffsetSurfaceUtils.pxx"
#include "../Geom/Geom_RevolutionUtils.pxx"

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BSplCLib.hxx>
#include <ElSLib.hxx>
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
#include <Geom_UndefinedDerivative.hxx>
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
#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

static const double PosTol = Precision::PConfusion() * 0.5;

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_Surface, Adaptor3d_Surface)

namespace
{
bool hasEvalRep(const GeomAdaptor_Surface::SurfaceDataVariant& theData)
{
  if (const GeomAdaptor_Surface::BezierData* aBezierData = std::get_if<GeomAdaptor_Surface::BezierData>(&theData))
  {
    return !aBezierData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Surface::BSplineData* aBSplineData = std::get_if<GeomAdaptor_Surface::BSplineData>(&theData))
  {
    return !aBSplineData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Surface::ExtrusionData* anExtrusionData = std::get_if<GeomAdaptor_Surface::ExtrusionData>(&theData))
  {
    return !anExtrusionData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Surface::RevolutionData* aRevolutionData = std::get_if<GeomAdaptor_Surface::RevolutionData>(&theData))
  {
    return !aRevolutionData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Surface::OffsetData* anOffsetData = std::get_if<GeomAdaptor_Surface::OffsetData>(&theData))
  {
    return !anOffsetData->EvalRep.IsNull();
  }
  return false;
}

//=================================================================================================

GeomAbs_Shape LocalContinuity(int                         Degree,
                              int                         Nb,
                              NCollection_Array1<double>& TK,
                              NCollection_Array1<int>&    TM,
                              double                      PFirst,
                              double                      PLast,
                              bool                        IsPeriodic)
{
  Standard_DomainError_Raise_if((TK.Length() != Nb || TM.Length() != Nb), " ");
  int    Index1 = 0;
  int    Index2 = 0;
  double newFirst, newLast;
  BSplCLib::LocateParameter(Degree, TK, TM, PFirst, IsPeriodic, 1, Nb, Index1, newFirst);
  BSplCLib::LocateParameter(Degree, TK, TM, PLast, IsPeriodic, 1, Nb, Index2, newLast);
  constexpr double EpsKnot = Precision::PConfusion();
  if (std::abs(newFirst - TK(Index1 + 1)) < EpsKnot)
    Index1++;
  if (std::abs(newLast - TK(Index2)) < EpsKnot)
    Index2--;
  // attention aux courbes peridiques.
  if ((IsPeriodic) && (Index1 == Nb))
    Index1 = 1;

  if (Index2 != Index1)
  {
    int i, Multmax = TM(Index1 + 1);
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

//! Offset surface D0 evaluation with retry mechanism for singular points.
//! Uses equivalent surface adaptor for faster evaluation when available.
//! @return true if evaluation succeeded, false on failure
inline bool offsetD0(const double                           theU,
                     const double                           theV,
                     const GeomAdaptor_Surface::OffsetData& theData,
                     gp_Pnt&                                theValue)
{
  if (!theData.EquivalentAdaptor.IsNull())
  {
    std::optional<gp_Pnt> aResult = theData.EquivalentAdaptor->EvalD0(theU, theV);
    if (!aResult)
      return false;
    theValue = *aResult;
    return true;
  }
  return Geom_OffsetSurfaceUtils::EvaluateD0(theU,
                                             theV,
                                             theData.BasisAdaptor,
                                             theData.Offset,
                                             theData.OffsetSurface.get(),
                                             theValue);
}

//! Offset surface D1 evaluation with retry mechanism for singular points.
//! Uses equivalent surface adaptor for faster evaluation when available.
//! @return true if evaluation succeeded, false on failure
inline bool offsetD1(const double                           theU,
                     const double                           theV,
                     const GeomAdaptor_Surface::OffsetData& theData,
                     gp_Pnt&                                theValue,
                     gp_Vec&                                theD1U,
                     gp_Vec&                                theD1V)
{
  if (!theData.EquivalentAdaptor.IsNull())
  {
    std::optional<Geom_Surface::ResD1> aResult = theData.EquivalentAdaptor->EvalD1(theU, theV);
    if (!aResult)
      return false;
    theValue = aResult->Point;
    theD1U   = aResult->D1U;
    theD1V   = aResult->D1V;
    return true;
  }
  return Geom_OffsetSurfaceUtils::EvaluateD1(theU,
                                             theV,
                                             theData.BasisAdaptor,
                                             theData.Offset,
                                             theData.OffsetSurface.get(),
                                             theValue,
                                             theD1U,
                                             theD1V);
}

//! Offset surface D2 evaluation with retry mechanism for singular points.
//! Uses equivalent surface adaptor for faster evaluation when available.
//! @return true if evaluation succeeded, false on failure
inline bool offsetD2(const double                           theU,
                     const double                           theV,
                     const GeomAdaptor_Surface::OffsetData& theData,
                     gp_Pnt&                                theValue,
                     gp_Vec&                                theD1U,
                     gp_Vec&                                theD1V,
                     gp_Vec&                                theD2U,
                     gp_Vec&                                theD2V,
                     gp_Vec&                                theD2UV)
{
  if (!theData.EquivalentAdaptor.IsNull())
  {
    std::optional<Geom_Surface::ResD2> aResult = theData.EquivalentAdaptor->EvalD2(theU, theV);
    if (!aResult)
      return false;
    theValue = aResult->Point;
    theD1U   = aResult->D1U;
    theD1V   = aResult->D1V;
    theD2U   = aResult->D2U;
    theD2V   = aResult->D2V;
    theD2UV  = aResult->D2UV;
    return true;
  }
  return Geom_OffsetSurfaceUtils::EvaluateD2(theU,
                                             theV,
                                             theData.BasisAdaptor,
                                             theData.Offset,
                                             theData.OffsetSurface.get(),
                                             theValue,
                                             theD1U,
                                             theD1V,
                                             theD2U,
                                             theD2V,
                                             theD2UV);
}

//! Offset surface D3 evaluation with retry mechanism for singular points.
//! Uses equivalent surface adaptor for faster evaluation when available.
//! @return true if evaluation succeeded, false on failure
inline bool offsetD3(const double                           theU,
                     const double                           theV,
                     const GeomAdaptor_Surface::OffsetData& theData,
                     gp_Pnt&                                theValue,
                     gp_Vec&                                theD1U,
                     gp_Vec&                                theD1V,
                     gp_Vec&                                theD2U,
                     gp_Vec&                                theD2V,
                     gp_Vec&                                theD2UV,
                     gp_Vec&                                theD3U,
                     gp_Vec&                                theD3V,
                     gp_Vec&                                theD3UUV,
                     gp_Vec&                                theD3UVV)
{
  if (!theData.EquivalentAdaptor.IsNull())
  {
    std::optional<Geom_Surface::ResD3> aResult = theData.EquivalentAdaptor->EvalD3(theU, theV);
    if (!aResult)
      return false;
    theValue = aResult->Point;
    theD1U   = aResult->D1U;
    theD1V   = aResult->D1V;
    theD2U   = aResult->D2U;
    theD2V   = aResult->D2V;
    theD2UV  = aResult->D2UV;
    theD3U   = aResult->D3U;
    theD3V   = aResult->D3V;
    theD3UUV = aResult->D3UUV;
    theD3UVV = aResult->D3UVV;
    return true;
  }
  return Geom_OffsetSurfaceUtils::EvaluateD3(theU,
                                             theV,
                                             theData.BasisAdaptor,
                                             theData.Offset,
                                             theData.OffsetSurface.get(),
                                             theValue,
                                             theD1U,
                                             theD1V,
                                             theD2U,
                                             theD2V,
                                             theD2UV,
                                             theD3U,
                                             theD3V,
                                             theD3UUV,
                                             theD3UVV);
}

//! Offset surface DN evaluation.
//! Uses equivalent surface adaptor for faster evaluation when available.
//! @return true if evaluation succeeded, false on failure
inline bool offsetDN(const double                           theU,
                     const double                           theV,
                     const GeomAdaptor_Surface::OffsetData& theData,
                     int                                    theNu,
                     int                                    theNv,
                     gp_Vec&                                theResult)
{
  if (!theData.EquivalentAdaptor.IsNull())
  {
    std::optional<gp_Vec> aResult = theData.EquivalentAdaptor->EvalDN(theU, theV, theNu, theNv);
    if (!aResult)
      return false;
    theResult = *aResult;
    return true;
  }
  return Geom_OffsetSurfaceUtils::EvaluateDN(theU,
                                             theV,
                                             theNu,
                                             theNv,
                                             theData.BasisAdaptor.get(),
                                             theData.Offset,
                                             theData.OffsetSurface.get(),
                                             theResult);
}

} // end of anonymous namespace

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_Surface::ShallowCopy() const
{
  occ::handle<GeomAdaptor_Surface> aCopy = new GeomAdaptor_Surface();

  aCopy->mySurface     = mySurface;
  aCopy->myUFirst      = myUFirst;
  aCopy->myULast       = myULast;
  aCopy->myVFirst      = myVFirst;
  aCopy->myVLast       = myVLast;
  aCopy->myTolU        = myTolU;
  aCopy->myTolV        = myTolV;
  aCopy->mySurfaceType = mySurfaceType;

  // Copy surface-specific evaluation data
  if (auto* anExtrusionData = std::get_if<GeomAdaptor_Surface::ExtrusionData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::ExtrusionData aNewData;
    aNewData.BasisCurve  = anExtrusionData->BasisCurve->ShallowCopy();
    aNewData.Direction   = anExtrusionData->Direction;
    aNewData.EvalRep     = anExtrusionData->EvalRep;
    aCopy->mySurfaceData = aNewData;
  }
  else if (auto* aRevolutionData = std::get_if<GeomAdaptor_Surface::RevolutionData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::RevolutionData aNewData;
    aNewData.BasisCurve  = aRevolutionData->BasisCurve->ShallowCopy();
    aNewData.Axis        = aRevolutionData->Axis;
    aNewData.EvalRep     = aRevolutionData->EvalRep;
    aCopy->mySurfaceData = aNewData;
  }
  else if (auto* anOffsetData = std::get_if<GeomAdaptor_Surface::OffsetData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::OffsetData aNewData;
    aNewData.BasisAdaptor =
      occ::down_cast<GeomAdaptor_Surface>(anOffsetData->BasisAdaptor->ShallowCopy());
    if (!anOffsetData->EquivalentAdaptor.IsNull())
    {
      aNewData.EquivalentAdaptor =
        occ::down_cast<GeomAdaptor_Surface>(anOffsetData->EquivalentAdaptor->ShallowCopy());
    }
    aNewData.OffsetSurface = anOffsetData->OffsetSurface; // Shared handle to original surface
    aNewData.Offset        = anOffsetData->Offset;
    aNewData.EvalRep       = anOffsetData->EvalRep;
    aCopy->mySurfaceData   = std::move(aNewData);
  }
  else if (auto* aBSplineData = std::get_if<GeomAdaptor_Surface::BSplineData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::BSplineData aNewData;
    aNewData.Surface = aBSplineData->Surface;
    aNewData.EvalRep = aBSplineData->EvalRep;
    // Cache is not copied - will be rebuilt on demand
    aCopy->mySurfaceData = aNewData;
  }
  else if (auto* aBezierData = std::get_if<GeomAdaptor_Surface::BezierData>(&mySurfaceData))
  {
    GeomAdaptor_Surface::BezierData aNewData;
    aNewData.Surface = aBezierData->Surface;
    aNewData.EvalRep = aBezierData->EvalRep;
    aCopy->mySurfaceData = aNewData;
  }
  else
  {
    // Elementary surface types (gp_Pln, gp_Cylinder, etc.) are value types - direct copy
    aCopy->mySurfaceData = mySurfaceData;
  }

  return aCopy;
}

//=================================================================================================

void GeomAdaptor_Surface::load(const occ::handle<Geom_Surface>& S,
                               const double                     UFirst,
                               const double                     ULast,
                               const double                     VFirst,
                               const double                     VLast,
                               const double                     TolU,
                               const double                     TolV)
{
  myTolU   = TolU;
  myTolV   = TolV;
  myUFirst = UFirst;
  myULast  = ULast;
  myVFirst = VFirst;
  myVLast  = VLast;

  if (mySurface != S)
  {
    mySurface     = S;
    mySurfaceData = std::monostate{};

    const occ::handle<Standard_Type>& TheType = S->DynamicType();
    if (TheType == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
    {
      Load(occ::down_cast<Geom_RectangularTrimmedSurface>(S)->BasisSurface(),
           UFirst,
           ULast,
           VFirst,
           VLast);
    }
    else if (TheType == STANDARD_TYPE(Geom_Plane))
    {
      mySurfaceType = GeomAbs_Plane;
      mySurfaceData = occ::down_cast<Geom_Plane>(S)->Pln();
    }
    else if (TheType == STANDARD_TYPE(Geom_CylindricalSurface))
    {
      mySurfaceType = GeomAbs_Cylinder;
      mySurfaceData = occ::down_cast<Geom_CylindricalSurface>(S)->Cylinder();
    }
    else if (TheType == STANDARD_TYPE(Geom_ConicalSurface))
    {
      mySurfaceType = GeomAbs_Cone;
      mySurfaceData = occ::down_cast<Geom_ConicalSurface>(S)->Cone();
    }
    else if (TheType == STANDARD_TYPE(Geom_SphericalSurface))
    {
      mySurfaceType = GeomAbs_Sphere;
      mySurfaceData = occ::down_cast<Geom_SphericalSurface>(S)->Sphere();
    }
    else if (TheType == STANDARD_TYPE(Geom_ToroidalSurface))
    {
      mySurfaceType = GeomAbs_Torus;
      mySurfaceData = occ::down_cast<Geom_ToroidalSurface>(S)->Torus();
    }
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    {
      mySurfaceType = GeomAbs_SurfaceOfRevolution;
      occ::handle<Geom_SurfaceOfRevolution> aRevSurf =
        occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
      // Populate revolution surface data
      GeomAdaptor_Surface::RevolutionData aRevData;
      aRevData.BasisCurve = new GeomAdaptor_Curve(aRevSurf->BasisCurve());
      aRevData.Axis       = aRevSurf->Axis();
      aRevData.EvalRep    = aRevSurf->EvalRepresentation();
      mySurfaceData       = aRevData;
    }
    else if (TheType == STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))
    {
      mySurfaceType = GeomAbs_SurfaceOfExtrusion;
      occ::handle<Geom_SurfaceOfLinearExtrusion> anExtSurf =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface);
      // Populate extrusion surface data with XYZ for fast evaluation
      GeomAdaptor_Surface::ExtrusionData anExtData;
      anExtData.BasisCurve = new GeomAdaptor_Curve(anExtSurf->BasisCurve());
      anExtData.Direction  = anExtSurf->Direction().XYZ();
      anExtData.EvalRep    = anExtSurf->EvalRepresentation();
      mySurfaceData        = anExtData;
    }
    else if (TheType == STANDARD_TYPE(Geom_BezierSurface))
    {
      mySurfaceType = GeomAbs_BezierSurface;
      GeomAdaptor_Surface::BezierData aBezierData;
      aBezierData.Surface = occ::down_cast<Geom_BezierSurface>(mySurface);
      aBezierData.EvalRep = aBezierData.Surface->EvalRepresentation();
      mySurfaceData       = std::move(aBezierData);
    }
    else if (TheType == STANDARD_TYPE(Geom_BSplineSurface))
    {
      mySurfaceType = GeomAbs_BSplineSurface;
      GeomAdaptor_Surface::BSplineData aBSplineData;
      aBSplineData.Surface = occ::down_cast<Geom_BSplineSurface>(mySurface);
      aBSplineData.EvalRep = aBSplineData.Surface->EvalRepresentation();
      mySurfaceData        = aBSplineData;
    }
    else if (TheType == STANDARD_TYPE(Geom_OffsetSurface))
    {
      mySurfaceType                             = GeomAbs_OffsetSurface;
      occ::handle<Geom_OffsetSurface> anOffSurf = occ::down_cast<Geom_OffsetSurface>(mySurface);
      // Populate offset surface data - reuse the original surface for osculating queries
      GeomAdaptor_Surface::OffsetData anOffsetData;
      anOffsetData.BasisAdaptor  = new GeomAdaptor_Surface(anOffSurf->BasisSurface(),
                                                          myUFirst,
                                                          myULast,
                                                          myVFirst,
                                                          myVLast,
                                                          myTolU,
                                                          myTolV);
      anOffsetData.OffsetSurface = anOffSurf;
      anOffsetData.Offset        = anOffSurf->Offset();
      anOffsetData.EvalRep       = anOffSurf->EvalRepresentation();
      // Check if equivalent canonical surface exists for faster evaluation
      occ::handle<Geom_Surface> anEquivSurf = anOffSurf->Surface();
      if (!anEquivSurf.IsNull())
      {
        anOffsetData.EquivalentAdaptor = new GeomAdaptor_Surface(anEquivSurf,
                                                                 myUFirst,
                                                                 myULast,
                                                                 myVFirst,
                                                                 myVLast,
                                                                 myTolU,
                                                                 myTolV);
      }
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
      const auto&                aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      NCollection_Array1<double> TK(aBSpl->UKnots());
      NCollection_Array1<int>    TM(aBSpl->UMultiplicities());
      return LocalContinuity(aBSpl->UDegree(),
                             aBSpl->NbUKnots(),
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
      occ::handle<Geom_SurfaceOfLinearExtrusion> myExtSurf =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface);
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
      const auto&                aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      NCollection_Array1<double> TK(aBSpl->VKnots());
      NCollection_Array1<int>    TM(aBSpl->VMultiplicities());
      return LocalContinuity(aBSpl->VDegree(),
                             aBSpl->NbVKnots(),
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
      occ::handle<Geom_SurfaceOfRevolution> myRevSurf =
        occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
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

int GeomAdaptor_Surface::NbUIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const auto&       aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      GeomAdaptor_Curve myBasisCurve(aBSpl->VIso(aBSpl->VKnot(aBSpl->FirstVKnotIndex())),
                                     myUFirst,
                                     myULast);
      return myBasisCurve.NbIntervals(S);
    }
    case GeomAbs_SurfaceOfExtrusion: {
      occ::handle<Geom_SurfaceOfLinearExtrusion> myExtSurf =
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface);
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
      occ::handle<Geom_OffsetSurface> myOffSurf = occ::down_cast<Geom_OffsetSurface>(mySurface);
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

int GeomAdaptor_Surface::NbVIntervals(const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const auto&       aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      GeomAdaptor_Curve myBasisCurve(aBSpl->UIso(aBSpl->UKnot(aBSpl->FirstUKnotIndex())),
                                     myVFirst,
                                     myVLast);
      return myBasisCurve.NbIntervals(S);
    }
    case GeomAbs_SurfaceOfRevolution: {
      occ::handle<Geom_SurfaceOfRevolution> myRevSurf =
        occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
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
      occ::handle<Geom_OffsetSurface> myOffSurf = occ::down_cast<Geom_OffsetSurface>(mySurface);
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

void GeomAdaptor_Surface::UIntervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const auto&       aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      GeomAdaptor_Curve myBasisCurve(aBSpl->VIso(aBSpl->VKnot(aBSpl->FirstVKnotIndex())),
                                     myUFirst,
                                     myULast);
      myBasisCurve.Intervals(T, S);
      return;
    }
    case GeomAbs_SurfaceOfExtrusion: {
      GeomAdaptor_Curve myBasisCurve(
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve(),
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
      occ::handle<Geom_OffsetSurface> myOffSurf = occ::down_cast<Geom_OffsetSurface>(mySurface);
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

void GeomAdaptor_Surface::VIntervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  switch (mySurfaceType)
  {
    case GeomAbs_BSplineSurface: {
      const auto&       aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      GeomAdaptor_Curve myBasisCurve(aBSpl->UIso(aBSpl->UKnot(aBSpl->FirstUKnotIndex())),
                                     myVFirst,
                                     myVLast);
      myBasisCurve.Intervals(T, S);
      return;
    }
    case GeomAbs_SurfaceOfRevolution: {
      occ::handle<Geom_SurfaceOfRevolution> myRevSurf =
        occ::down_cast<Geom_SurfaceOfRevolution>(mySurface);
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
      occ::handle<Geom_OffsetSurface> myOffSurf = occ::down_cast<Geom_OffsetSurface>(mySurface);
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

occ::handle<Adaptor3d_Surface> GeomAdaptor_Surface::UTrim(const double First,
                                                          const double Last,
                                                          const double Tol) const
{
  return occ::handle<GeomAdaptor_Surface>(
    new GeomAdaptor_Surface(mySurface, First, Last, myVFirst, myVLast, Tol, myTolV));
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_Surface::VTrim(const double First,
                                                          const double Last,
                                                          const double Tol) const
{
  return occ::handle<GeomAdaptor_Surface>(
    new GeomAdaptor_Surface(mySurface, myUFirst, myULast, First, Last, myTolU, Tol));
}

//=================================================================================================

bool GeomAdaptor_Surface::IsUClosed() const
{
  if (!mySurface->IsUClosed())
    return false;

  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsUPeriodic())
    return (std::abs(std::abs(U1 - U2) - std::abs(myUFirst - myULast)) < Precision::PConfusion());

  return (std::abs(U1 - myUFirst) < Precision::PConfusion()
          && std::abs(U2 - myULast) < Precision::PConfusion());
}

//=================================================================================================

bool GeomAdaptor_Surface::IsVClosed() const
{
  if (!mySurface->IsVClosed())
    return false;

  double U1, U2, V1, V2;
  mySurface->Bounds(U1, U2, V1, V2);
  if (mySurface->IsVPeriodic())
    return (std::abs(std::abs(V1 - V2) - std::abs(myVFirst - myVLast)) < Precision::PConfusion());

  return (std::abs(V1 - myVFirst) < Precision::PConfusion()
          && std::abs(V2 - myVLast) < Precision::PConfusion());
}

//=================================================================================================

bool GeomAdaptor_Surface::IsUPeriodic() const
{
  return (mySurface->IsUPeriodic());
}

//=================================================================================================

double GeomAdaptor_Surface::UPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsUPeriodic(), " ");
  return mySurface->UPeriod();
}

//=================================================================================================

bool GeomAdaptor_Surface::IsVPeriodic() const
{
  return (mySurface->IsVPeriodic());
}

//=================================================================================================

double GeomAdaptor_Surface::VPeriod() const
{
  Standard_NoSuchObject_Raise_if(!IsVPeriodic(), " ");
  return mySurface->VPeriod();
}

//=================================================================================================

void GeomAdaptor_Surface::RebuildCache(const double theU, const double theV) const
{
  if (mySurfaceType == GeomAbs_BezierSurface)
  {
    // Create cache for Bezier
    auto&                           aBezData = std::get<BezierData>(mySurfaceData);
    occ::handle<Geom_BezierSurface> aBezier  = occ::down_cast<Geom_BezierSurface>(mySurface);
    int                             aDegU    = aBezier->UDegree();
    int                             aDegV    = aBezier->VDegree();
    NCollection_Array1<double> aFlatKnotsU(BSplCLib::FlatBezierKnots(aDegU), 1, 2 * (aDegU + 1));
    NCollection_Array1<double> aFlatKnotsV(BSplCLib::FlatBezierKnots(aDegV), 1, 2 * (aDegV + 1));
    if (aBezData.Cache.IsNull())
      aBezData.Cache = new BSplSLib_Cache(aDegU,
                                          aBezier->IsUPeriodic(),
                                          aFlatKnotsU,
                                          aDegV,
                                          aBezier->IsVPeriodic(),
                                          aFlatKnotsV,
                                          aBezier->Weights());
    aBezData.Cache
      ->BuildCache(theU, theV, aFlatKnotsU, aFlatKnotsV, aBezier->Poles(), aBezier->Weights());
  }
  else if (mySurfaceType == GeomAbs_BSplineSurface)
  {
    // Create cache for B-spline
    auto&       aBSplData = std::get<BSplineData>(mySurfaceData);
    const auto& aBSpl     = aBSplData.Surface;
    if (aBSplData.Cache.IsNull())
      aBSplData.Cache = new BSplSLib_Cache(aBSpl->UDegree(),
                                           aBSpl->IsUPeriodic(),
                                           aBSpl->UKnotSequence(),
                                           aBSpl->VDegree(),
                                           aBSpl->IsVPeriodic(),
                                           aBSpl->VKnotSequence(),
                                           aBSpl->Weights());
    aBSplData.Cache->BuildCache(theU,
                                theV,
                                aBSpl->UKnotSequence(),
                                aBSpl->VKnotSequence(),
                                aBSpl->Poles(),
                                aBSpl->Weights());
  }
}

//=================================================================================================

gp_Pnt GeomAdaptor_Surface::Value(const double U, const double V) const
{
  gp_Pnt aValue;
  D0(U, V, aValue);
  return aValue;
}

//=================================================================================================

void GeomAdaptor_Surface::D0(const double U, const double V, gp_Pnt& P) const
{
  std::optional<gp_Pnt> aResult = EvalD0(U, V);
  if (!aResult)
    throw Geom_UndefinedValue("GeomAdaptor_Surface::D0: evaluation failed");
  P = *aResult;
}

//=================================================================================================

std::optional<gp_Pnt> GeomAdaptor_Surface::EvalD0(double U, double V) const
{
  gp_Pnt P;
  switch (mySurfaceType)
  {
    case GeomAbs_Plane:
      ElSLib::D0(U, V, std::get<gp_Pln>(mySurfaceData), P);
      return P;
    case GeomAbs_Cylinder:
      ElSLib::D0(U, V, std::get<gp_Cylinder>(mySurfaceData), P);
      return P;
    case GeomAbs_Cone:
      ElSLib::D0(U, V, std::get<gp_Cone>(mySurfaceData), P);
      return P;
    case GeomAbs_Sphere:
      ElSLib::D0(U, V, std::get<gp_Sphere>(mySurfaceData), P);
      return P;
    case GeomAbs_Torus:
      ElSLib::D0(U, V, std::get<gp_Torus>(mySurfaceData), P);
      return P;

    case GeomAbs_BezierSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD0(U, V);
      }
      auto& aCache = std::get<BezierData>(mySurfaceData).Cache;
      if (aCache.IsNull())
        RebuildCache(U, V);
      aCache->D0(U, V, P);
      return P;
    }
    case GeomAbs_BSplineSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD0(U, V);
      }
      auto& aCache = std::get<BSplineData>(mySurfaceData).Cache;
      if (aCache.IsNull() || !aCache->IsCacheValid(U, V))
        RebuildCache(U, V);
      aCache->D0(U, V, P);
      return P;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD0(U, V);
      }
      const auto& anExtData = std::get<GeomAdaptor_Surface::ExtrusionData>(mySurfaceData);
      if (!Geom_ExtrusionUtils::D0(U, V, *anExtData.BasisCurve, anExtData.Direction, P))
        return std::nullopt;
      return P;
    }

    case GeomAbs_SurfaceOfRevolution: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD0(U, V);
      }
      const auto& aRevData = std::get<GeomAdaptor_Surface::RevolutionData>(mySurfaceData);
      if (!Geom_RevolutionUtils::D0(U, V, *aRevData.BasisCurve, aRevData.Axis, P))
        return std::nullopt;
      return P;
    }

    case GeomAbs_OffsetSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD0(U, V);
      }
      const auto& anOffData = std::get<GeomAdaptor_Surface::OffsetData>(mySurfaceData);
      if (!offsetD0(U, V, anOffData, P))
        return std::nullopt;
      return P;
    }

    default:
      return mySurface->EvalD0(U, V);
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D1(const double U,
                             const double V,
                             gp_Pnt&      P,
                             gp_Vec&      D1U,
                             gp_Vec&      D1V) const
{
  std::optional<Geom_Surface::ResD1> aResult = EvalD1(U, V);
  if (!aResult)
    throw Geom_UndefinedDerivative("GeomAdaptor_Surface::D1: evaluation failed");
  P   = aResult->Point;
  D1U = aResult->D1U;
  D1V = aResult->D1V;
}

//=================================================================================================

std::optional<Geom_Surface::ResD1> GeomAdaptor_Surface::EvalD1(double U, double V) const
{
  Geom_Surface::ResD1 aResult;
  int                 Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  double              u = U, v = V;
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
    case GeomAbs_Plane:
      ElSLib::D1(u, v, std::get<gp_Pln>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      return aResult;
    case GeomAbs_Cylinder:
      ElSLib::D1(u,
                 v,
                 std::get<gp_Cylinder>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V);
      return aResult;
    case GeomAbs_Cone:
      ElSLib::D1(u, v, std::get<gp_Cone>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      return aResult;
    case GeomAbs_Sphere:
      ElSLib::D1(u, v, std::get<gp_Sphere>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      return aResult;
    case GeomAbs_Torus:
      ElSLib::D1(u, v, std::get<gp_Torus>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      return aResult;

    case GeomAbs_BezierSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD1(U, V);
      }
      auto& aCache = std::get<BezierData>(mySurfaceData).Cache;
      if (aCache.IsNull())
        RebuildCache(U, V);
      aCache->D1(U, V, aResult.Point, aResult.D1U, aResult.D1V);
      return aResult;
    }
    case GeomAbs_BSplineSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD1(U, V);
      }
      auto&       aBSplData = std::get<BSplineData>(mySurfaceData);
      const auto& aBSpl     = aBSplData.Surface;
      if ((USide != 0 || VSide != 0) && IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
        aBSpl->LocalD1(u, v, Ideb, Ifin, IVdeb, IVfin, aResult.Point, aResult.D1U, aResult.D1V);
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U, V))
          RebuildCache(U, V);
        aBSplData.Cache->D1(U, V, aResult.Point, aResult.D1U, aResult.D1V);
      }
      return aResult;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD1(U, V);
      }
      const auto& anExtData = std::get<GeomAdaptor_Surface::ExtrusionData>(mySurfaceData);
      if (!Geom_ExtrusionUtils::D1(u,
                                   v,
                                   *anExtData.BasisCurve,
                                   anExtData.Direction,
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_SurfaceOfRevolution: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD1(U, V);
      }
      const auto& aRevData = std::get<GeomAdaptor_Surface::RevolutionData>(mySurfaceData);
      if (!Geom_RevolutionUtils::D1(u,
                                    v,
                                    *aRevData.BasisCurve,
                                    aRevData.Axis,
                                    aResult.Point,
                                    aResult.D1U,
                                    aResult.D1V))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_OffsetSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD1(U, V);
      }
      const auto& anOffData = std::get<GeomAdaptor_Surface::OffsetData>(mySurfaceData);
      if (!offsetD1(u, v, anOffData, aResult.Point, aResult.D1U, aResult.D1V))
        return std::nullopt;
      return aResult;
    }

    default:
      return mySurface->EvalD1(u, v);
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D2(const double U,
                             const double V,
                             gp_Pnt&      P,
                             gp_Vec&      D1U,
                             gp_Vec&      D1V,
                             gp_Vec&      D2U,
                             gp_Vec&      D2V,
                             gp_Vec&      D2UV) const
{
  std::optional<Geom_Surface::ResD2> aResult = EvalD2(U, V);
  if (!aResult)
    throw Geom_UndefinedDerivative("GeomAdaptor_Surface::D2: evaluation failed");
  P    = aResult->Point;
  D1U  = aResult->D1U;
  D1V  = aResult->D1V;
  D2U  = aResult->D2U;
  D2V  = aResult->D2V;
  D2UV = aResult->D2UV;
}

//=================================================================================================

std::optional<Geom_Surface::ResD2> GeomAdaptor_Surface::EvalD2(double U, double V) const
{
  Geom_Surface::ResD2 aResult;
  int                 Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  double              u = U, v = V;
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
    case GeomAbs_Plane:
      ElSLib::D1(u, v, std::get<gp_Pln>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      aResult.D2U.SetCoord(0., 0., 0.);
      aResult.D2V.SetCoord(0., 0., 0.);
      aResult.D2UV.SetCoord(0., 0., 0.);
      return aResult;
    case GeomAbs_Cylinder:
      ElSLib::D2(u,
                 v,
                 std::get<gp_Cylinder>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV);
      return aResult;
    case GeomAbs_Cone:
      ElSLib::D2(u,
                 v,
                 std::get<gp_Cone>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV);
      return aResult;
    case GeomAbs_Sphere:
      ElSLib::D2(u,
                 v,
                 std::get<gp_Sphere>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV);
      return aResult;
    case GeomAbs_Torus:
      ElSLib::D2(u,
                 v,
                 std::get<gp_Torus>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV);
      return aResult;

    case GeomAbs_BezierSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD2(U, V);
      }
      auto& aCache = std::get<BezierData>(mySurfaceData).Cache;
      if (aCache.IsNull())
        RebuildCache(U, V);
      aCache
        ->D2(U, V, aResult.Point, aResult.D1U, aResult.D1V, aResult.D2U, aResult.D2V, aResult.D2UV);
      return aResult;
    }
    case GeomAbs_BSplineSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD2(U, V);
      }
      auto&       aBSplData = std::get<BSplineData>(mySurfaceData);
      const auto& aBSpl     = aBSplData.Surface;
      if ((USide != 0 || VSide != 0) && IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
        aBSpl->LocalD2(u,
                       v,
                       Ideb,
                       Ifin,
                       IVdeb,
                       IVfin,
                       aResult.Point,
                       aResult.D1U,
                       aResult.D1V,
                       aResult.D2U,
                       aResult.D2V,
                       aResult.D2UV);
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U, V))
          RebuildCache(U, V);
        aBSplData.Cache->D2(U,
                            V,
                            aResult.Point,
                            aResult.D1U,
                            aResult.D1V,
                            aResult.D2U,
                            aResult.D2V,
                            aResult.D2UV);
      }
      return aResult;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD2(U, V);
      }
      const auto& anExtData = std::get<GeomAdaptor_Surface::ExtrusionData>(mySurfaceData);
      if (!Geom_ExtrusionUtils::D2(u,
                                   v,
                                   *anExtData.BasisCurve,
                                   anExtData.Direction,
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V,
                                   aResult.D2U,
                                   aResult.D2V,
                                   aResult.D2UV))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_SurfaceOfRevolution: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD2(U, V);
      }
      const auto& aRevData = std::get<GeomAdaptor_Surface::RevolutionData>(mySurfaceData);
      if (!Geom_RevolutionUtils::D2(u,
                                    v,
                                    *aRevData.BasisCurve,
                                    aRevData.Axis,
                                    aResult.Point,
                                    aResult.D1U,
                                    aResult.D1V,
                                    aResult.D2U,
                                    aResult.D2V,
                                    aResult.D2UV))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_OffsetSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD2(U, V);
      }
      const auto& anOffData = std::get<GeomAdaptor_Surface::OffsetData>(mySurfaceData);
      if (!offsetD2(u,
                    v,
                    anOffData,
                    aResult.Point,
                    aResult.D1U,
                    aResult.D1V,
                    aResult.D2U,
                    aResult.D2V,
                    aResult.D2UV))
        return std::nullopt;
      return aResult;
    }

    default:
      return mySurface->EvalD2(u, v);
  }
}

//=================================================================================================

void GeomAdaptor_Surface::D3(const double U,
                             const double V,
                             gp_Pnt&      P,
                             gp_Vec&      D1U,
                             gp_Vec&      D1V,
                             gp_Vec&      D2U,
                             gp_Vec&      D2V,
                             gp_Vec&      D2UV,
                             gp_Vec&      D3U,
                             gp_Vec&      D3V,
                             gp_Vec&      D3UUV,
                             gp_Vec&      D3UVV) const
{
  std::optional<Geom_Surface::ResD3> aResult = EvalD3(U, V);
  if (!aResult)
    throw Geom_UndefinedDerivative("GeomAdaptor_Surface::D3: evaluation failed");
  P     = aResult->Point;
  D1U   = aResult->D1U;
  D1V   = aResult->D1V;
  D2U   = aResult->D2U;
  D2V   = aResult->D2V;
  D2UV  = aResult->D2UV;
  D3U   = aResult->D3U;
  D3V   = aResult->D3V;
  D3UUV = aResult->D3UUV;
  D3UVV = aResult->D3UVV;
}

//=================================================================================================

std::optional<Geom_Surface::ResD3> GeomAdaptor_Surface::EvalD3(double U, double V) const
{
  Geom_Surface::ResD3 aResult;
  int                 Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  double              u = U, v = V;
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
    case GeomAbs_Plane:
      ElSLib::D1(u, v, std::get<gp_Pln>(mySurfaceData), aResult.Point, aResult.D1U, aResult.D1V);
      aResult.D2U.SetCoord(0., 0., 0.);
      aResult.D2V.SetCoord(0., 0., 0.);
      aResult.D2UV.SetCoord(0., 0., 0.);
      aResult.D3U.SetCoord(0., 0., 0.);
      aResult.D3V.SetCoord(0., 0., 0.);
      aResult.D3UUV.SetCoord(0., 0., 0.);
      aResult.D3UVV.SetCoord(0., 0., 0.);
      return aResult;
    case GeomAbs_Cylinder:
      ElSLib::D3(u,
                 v,
                 std::get<gp_Cylinder>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV,
                 aResult.D3U,
                 aResult.D3V,
                 aResult.D3UUV,
                 aResult.D3UVV);
      return aResult;
    case GeomAbs_Cone:
      ElSLib::D3(u,
                 v,
                 std::get<gp_Cone>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV,
                 aResult.D3U,
                 aResult.D3V,
                 aResult.D3UUV,
                 aResult.D3UVV);
      return aResult;
    case GeomAbs_Sphere:
      ElSLib::D3(u,
                 v,
                 std::get<gp_Sphere>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV,
                 aResult.D3U,
                 aResult.D3V,
                 aResult.D3UUV,
                 aResult.D3UVV);
      return aResult;
    case GeomAbs_Torus:
      ElSLib::D3(u,
                 v,
                 std::get<gp_Torus>(mySurfaceData),
                 aResult.Point,
                 aResult.D1U,
                 aResult.D1V,
                 aResult.D2U,
                 aResult.D2V,
                 aResult.D2UV,
                 aResult.D3U,
                 aResult.D3V,
                 aResult.D3UUV,
                 aResult.D3UVV);
      return aResult;

    case GeomAbs_BSplineSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD3(U, V);
      }
      const auto& aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      if ((USide == 0) && (VSide == 0))
      {
        std::optional<Geom_Surface::ResD3> aD3 = aBSpl->EvalD3(u, v);
        if (!aD3)
          return std::nullopt;
        return *aD3;
      }
      else
      {
        if (IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
          aBSpl->LocalD3(u,
                         v,
                         Ideb,
                         Ifin,
                         IVdeb,
                         IVfin,
                         aResult.Point,
                         aResult.D1U,
                         aResult.D1V,
                         aResult.D2U,
                         aResult.D2V,
                         aResult.D2UV,
                         aResult.D3U,
                         aResult.D3V,
                         aResult.D3UUV,
                         aResult.D3UVV);
        else
        {
          std::optional<Geom_Surface::ResD3> aD3 = aBSpl->EvalD3(u, v);
          if (!aD3)
            return std::nullopt;
          return *aD3;
        }
      }
      return aResult;
    }

    case GeomAbs_SurfaceOfExtrusion: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD3(U, V);
      }
      const auto& anExtData = std::get<GeomAdaptor_Surface::ExtrusionData>(mySurfaceData);
      if (!Geom_ExtrusionUtils::D3(u,
                                   v,
                                   *anExtData.BasisCurve,
                                   anExtData.Direction,
                                   aResult.Point,
                                   aResult.D1U,
                                   aResult.D1V,
                                   aResult.D2U,
                                   aResult.D2V,
                                   aResult.D2UV,
                                   aResult.D3U,
                                   aResult.D3V,
                                   aResult.D3UUV,
                                   aResult.D3UVV))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_SurfaceOfRevolution: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD3(U, V);
      }
      const auto& aRevData = std::get<GeomAdaptor_Surface::RevolutionData>(mySurfaceData);
      if (!Geom_RevolutionUtils::D3(u,
                                    v,
                                    *aRevData.BasisCurve,
                                    aRevData.Axis,
                                    aResult.Point,
                                    aResult.D1U,
                                    aResult.D1V,
                                    aResult.D2U,
                                    aResult.D2V,
                                    aResult.D2UV,
                                    aResult.D3U,
                                    aResult.D3V,
                                    aResult.D3UUV,
                                    aResult.D3UVV))
        return std::nullopt;
      return aResult;
    }

    case GeomAbs_OffsetSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalD3(U, V);
      }
      const auto& anOffData = std::get<GeomAdaptor_Surface::OffsetData>(mySurfaceData);
      if (!offsetD3(u,
                    v,
                    anOffData,
                    aResult.Point,
                    aResult.D1U,
                    aResult.D1V,
                    aResult.D2U,
                    aResult.D2V,
                    aResult.D2UV,
                    aResult.D3U,
                    aResult.D3V,
                    aResult.D3UUV,
                    aResult.D3UVV))
        return std::nullopt;
      return aResult;
    }

    default:
      return mySurface->EvalD3(u, v);
  }
}

//=================================================================================================

gp_Vec GeomAdaptor_Surface::DN(const double U, const double V, const int Nu, const int Nv) const
{
  std::optional<gp_Vec> aResult = EvalDN(U, V, Nu, Nv);
  if (!aResult)
    throw Geom_UndefinedDerivative("GeomAdaptor_Surface::DN: evaluation failed");
  return *aResult;
}

//=================================================================================================

std::optional<gp_Vec> GeomAdaptor_Surface::EvalDN(double U, double V, int Nu, int Nv) const
{
  int    Ideb, Ifin, IVdeb, IVfin, USide = 0, VSide = 0;
  double u = U, v = V;
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
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalDN(U, V, Nu, Nv);
      }
      const auto& aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
      if ((USide == 0) && (VSide == 0))
        return aBSpl->EvalDN(u, v, Nu, Nv);
      else
      {
        if (IfUVBound(u, v, Ideb, Ifin, IVdeb, IVfin, USide, VSide))
          return aBSpl->LocalDN(u, v, Ideb, Ifin, IVdeb, IVfin, Nu, Nv);
        else
          return aBSpl->EvalDN(u, v, Nu, Nv);
      }
    }

    case GeomAbs_SurfaceOfExtrusion: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalDN(U, V, Nu, Nv);
      }
      const auto& anExtData = std::get<GeomAdaptor_Surface::ExtrusionData>(mySurfaceData);
      gp_Vec      aDN;
      if (!Geom_ExtrusionUtils::DN(u, *anExtData.BasisCurve, anExtData.Direction, Nu, Nv, aDN))
        return std::nullopt;
      return aDN;
    }

    case GeomAbs_SurfaceOfRevolution: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalDN(U, V, Nu, Nv);
      }
      const auto& aRevData = std::get<GeomAdaptor_Surface::RevolutionData>(mySurfaceData);
      gp_Vec      aDN;
      if (!Geom_RevolutionUtils::DN(u, v, *aRevData.BasisCurve, aRevData.Axis, Nu, Nv, aDN))
        return std::nullopt;
      return aDN;
    }

    case GeomAbs_OffsetSurface: {
      if (hasEvalRep(mySurfaceData))
      {
        return mySurface->EvalDN(U, V, Nu, Nv);
      }
      const auto& anOffData = std::get<GeomAdaptor_Surface::OffsetData>(mySurfaceData);
      gp_Vec      aDN;
      if (!offsetDN(u, v, anOffData, Nu, Nv, aDN))
        return std::nullopt;
      return aDN;
    }

    case GeomAbs_Plane:
      return ElSLib::DN(u, v, std::get<gp_Pln>(mySurfaceData), Nu, Nv);
    case GeomAbs_Cylinder:
      return ElSLib::DN(u, v, std::get<gp_Cylinder>(mySurfaceData), Nu, Nv);
    case GeomAbs_Cone:
      return ElSLib::DN(u, v, std::get<gp_Cone>(mySurfaceData), Nu, Nv);
    case GeomAbs_Sphere:
      return ElSLib::DN(u, v, std::get<gp_Sphere>(mySurfaceData), Nu, Nv);
    case GeomAbs_Torus:
      return ElSLib::DN(u, v, std::get<gp_Torus>(mySurfaceData), Nu, Nv);

    case GeomAbs_BezierSurface:
    case GeomAbs_OtherSurface:
    default:
      break;
  }

  return mySurface->EvalDN(u, v, Nu, Nv);
}

//=================================================================================================

double GeomAdaptor_Surface::UResolution(const double R3d) const
{
  double Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfExtrusion: {
      GeomAdaptor_Curve myBasisCurve(
        occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve(),
        myUFirst,
        myULast);
      return myBasisCurve.Resolution(R3d);
    }
    case GeomAbs_Torus: {
      occ::handle<Geom_ToroidalSurface> S(occ::down_cast<Geom_ToroidalSurface>(mySurface));
      const double                      R = S->MajorRadius() + S->MinorRadius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Sphere: {
      occ::handle<Geom_SphericalSurface> S(occ::down_cast<Geom_SphericalSurface>(mySurface));
      const double                       R = S->Radius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Cylinder: {
      occ::handle<Geom_CylindricalSurface> S(occ::down_cast<Geom_CylindricalSurface>(mySurface));
      const double                         R = S->Radius();
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
      occ::handle<Geom_ConicalSurface> S(occ::down_cast<Geom_ConicalSurface>(mySurface));
      occ::handle<Geom_Curve>          C      = S->VIso(myVLast);
      const double                     Rayon1 = occ::down_cast<Geom_Circle>(C)->Radius();
      C                                       = S->VIso(myVFirst);
      const double Rayon2                     = occ::down_cast<Geom_Circle>(C)->Radius();
      const double R                          = (Rayon1 > Rayon2) ? Rayon1 : Rayon2;
      return (R > Precision::Confusion() ? (R3d / R) : 0.);
    }
    case GeomAbs_Plane: {
      return R3d;
    }
    case GeomAbs_BezierSurface: {
      double Ures, Vres;
      occ::down_cast<Geom_BezierSurface>(mySurface)->Resolution(R3d, Ures, Vres);
      return Ures;
    }
    case GeomAbs_BSplineSurface: {
      double Ures, Vres;
      std::get<BSplineData>(mySurfaceData).Surface->Resolution(R3d, Ures, Vres);
      return Ures;
    }
    case GeomAbs_OffsetSurface: {
      occ::handle<Geom_Surface> base =
        occ::down_cast<Geom_OffsetSurface>(mySurface)->BasisSurface();
      GeomAdaptor_Surface gabase(base, myUFirst, myULast, myVFirst, myVLast);
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

double GeomAdaptor_Surface::VResolution(const double R3d) const
{
  double Res = 0.;

  switch (mySurfaceType)
  {
    case GeomAbs_SurfaceOfRevolution: {
      GeomAdaptor_Curve myBasisCurve(
        occ::down_cast<Geom_SurfaceOfRevolution>(mySurface)->BasisCurve(),
        myUFirst,
        myULast);
      return myBasisCurve.Resolution(R3d);
    }
    case GeomAbs_Torus: {
      occ::handle<Geom_ToroidalSurface> S(occ::down_cast<Geom_ToroidalSurface>(mySurface));
      const double                      R = S->MinorRadius();
      if (R > Precision::Confusion())
        Res = R3d / (2. * R);
      break;
    }
    case GeomAbs_Sphere: {
      occ::handle<Geom_SphericalSurface> S(occ::down_cast<Geom_SphericalSurface>(mySurface));
      const double                       R = S->Radius();
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
      double Ures, Vres;
      occ::down_cast<Geom_BezierSurface>(mySurface)->Resolution(R3d, Ures, Vres);
      return Vres;
    }
    case GeomAbs_BSplineSurface: {
      double Ures, Vres;
      std::get<BSplineData>(mySurfaceData).Surface->Resolution(R3d, Ures, Vres);
      return Vres;
    }
    case GeomAbs_OffsetSurface: {
      occ::handle<Geom_Surface> base =
        occ::down_cast<Geom_OffsetSurface>(mySurface)->BasisSurface();
      GeomAdaptor_Surface gabase(base, myUFirst, myULast, myVFirst, myVLast);
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
  return std::get<gp_Pln>(mySurfaceData);
}

//=================================================================================================

gp_Cylinder GeomAdaptor_Surface::Cylinder() const
{
  if (mySurfaceType != GeomAbs_Cylinder)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Cylinder");
  return std::get<gp_Cylinder>(mySurfaceData);
}

//=================================================================================================

gp_Cone GeomAdaptor_Surface::Cone() const
{
  if (mySurfaceType != GeomAbs_Cone)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Cone");
  return std::get<gp_Cone>(mySurfaceData);
}

//=================================================================================================

gp_Sphere GeomAdaptor_Surface::Sphere() const
{
  if (mySurfaceType != GeomAbs_Sphere)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Sphere");
  return std::get<gp_Sphere>(mySurfaceData);
}

//=================================================================================================

gp_Torus GeomAdaptor_Surface::Torus() const
{
  if (mySurfaceType != GeomAbs_Torus)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Torus");
  return std::get<gp_Torus>(mySurfaceData);
}

//=================================================================================================

int GeomAdaptor_Surface::UDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->UDegree();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->UDegree();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.Degree();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::UDegree");
}

//=================================================================================================

int GeomAdaptor_Surface::NbUPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->NbUPoles();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->NbUPoles();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbPoles();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbUPoles");
}

//=================================================================================================

int GeomAdaptor_Surface::VDegree() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->VDegree();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->VDegree();
  if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve(
      occ::down_cast<Geom_SurfaceOfRevolution>(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.Degree();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::VDegree");
}

//=================================================================================================

int GeomAdaptor_Surface::NbVPoles() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->NbVPoles();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->NbVPoles();
  if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
  {
    GeomAdaptor_Curve myBasisCurve(
      occ::down_cast<Geom_SurfaceOfRevolution>(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbPoles();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbVPoles");
}

//=================================================================================================

int GeomAdaptor_Surface::NbUKnots() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->NbUKnots();
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
  {
    GeomAdaptor_Curve myBasisCurve(
      occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve(),
      myUFirst,
      myULast);
    return myBasisCurve.NbKnots();
  }
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbUKnots");
}

//=================================================================================================

int GeomAdaptor_Surface::NbVKnots() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->NbVKnots();
  throw Standard_NoSuchObject("GeomAdaptor_Surface::NbVKnots");
}

//=================================================================================================

bool GeomAdaptor_Surface::IsURational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->IsURational();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->IsURational();
  return false;
}

//=================================================================================================

bool GeomAdaptor_Surface::IsVRational() const
{
  if (mySurfaceType == GeomAbs_BSplineSurface)
    return std::get<BSplineData>(mySurfaceData).Surface->IsVRational();
  if (mySurfaceType == GeomAbs_BezierSurface)
    return occ::down_cast<Geom_BezierSurface>(mySurface)->IsVRational();
  return false;
}

//=================================================================================================

occ::handle<Geom_BezierSurface> GeomAdaptor_Surface::Bezier() const
{
  if (mySurfaceType != GeomAbs_BezierSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Bezier");
  return occ::down_cast<Geom_BezierSurface>(mySurface);
}

//=================================================================================================

occ::handle<Geom_BSplineSurface> GeomAdaptor_Surface::BSpline() const
{
  if (mySurfaceType != GeomAbs_BSplineSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BSpline");
  return std::get<BSplineData>(mySurfaceData).Surface;
}

//=================================================================================================

gp_Ax1 GeomAdaptor_Surface::AxeOfRevolution() const
{
  if (mySurfaceType != GeomAbs_SurfaceOfRevolution)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::AxeOfRevolution");
  return occ::down_cast<Geom_SurfaceOfRevolution>(mySurface)->Axis();
}

//=================================================================================================

gp_Dir GeomAdaptor_Surface::Direction() const
{
  if (mySurfaceType != GeomAbs_SurfaceOfExtrusion)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::Direction");
  return occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->Direction();
}

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_Surface::BasisCurve() const
{
  occ::handle<Geom_Curve> C;
  if (mySurfaceType == GeomAbs_SurfaceOfExtrusion)
    C = occ::down_cast<Geom_SurfaceOfLinearExtrusion>(mySurface)->BasisCurve();
  else if (mySurfaceType == GeomAbs_SurfaceOfRevolution)
    C = occ::down_cast<Geom_SurfaceOfRevolution>(mySurface)->BasisCurve();
  else
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisCurve");
  return occ::handle<GeomAdaptor_Curve>(new GeomAdaptor_Curve(C));
}

//=================================================================================================

occ::handle<Adaptor3d_Surface> GeomAdaptor_Surface::BasisSurface() const
{
  if (mySurfaceType != GeomAbs_OffsetSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisSurface");
  return new GeomAdaptor_Surface(occ::down_cast<Geom_OffsetSurface>(mySurface)->BasisSurface(),
                                 myUFirst,
                                 myULast,
                                 myVFirst,
                                 myVLast);
}

//=================================================================================================

double GeomAdaptor_Surface::OffsetValue() const
{
  if (mySurfaceType != GeomAbs_OffsetSurface)
    throw Standard_NoSuchObject("GeomAdaptor_Surface::BasisSurface");
  return occ::down_cast<Geom_OffsetSurface>(mySurface)->Offset();
}

//=======================================================================
// function : IfUVBound <private>
// purpose  :  locates U,V parameters if U,V =First, Last,
//	      processes the finding span and returns the
//	      parameters for LocalDi
//=======================================================================

bool GeomAdaptor_Surface::IfUVBound(const double U,
                                    const double V,
                                    int&         IOutDeb,
                                    int&         IOutFin,
                                    int&         IOutVDeb,
                                    int&         IOutVFin,
                                    const int    USide,
                                    const int    VSide) const
{
  const auto& aBSpl = std::get<BSplineData>(mySurfaceData).Surface;
  int         Ideb, Ifin;
  int         anUFKIndx = aBSpl->FirstUKnotIndex(), anULKIndx = aBSpl->LastUKnotIndex(),
      aVFKIndx = aBSpl->FirstVKnotIndex(), aVLKIndx = aBSpl->LastVKnotIndex();
  aBSpl->LocateU(U, PosTol, Ideb, Ifin, false);
  bool Local = (Ideb == Ifin);
  Span(USide, Ideb, Ifin, Ideb, Ifin, anUFKIndx, anULKIndx);
  int IVdeb, IVfin;
  aBSpl->LocateV(V, PosTol, IVdeb, IVfin, false);
  if (IVdeb == IVfin)
    Local = true;
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

void GeomAdaptor_Surface::Span(const int Side,
                               const int Ideb,
                               const int Ifin,
                               int&      OutIdeb,
                               int&      OutIfin,
                               const int theFKIndx,
                               const int theLKIndx) const
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
