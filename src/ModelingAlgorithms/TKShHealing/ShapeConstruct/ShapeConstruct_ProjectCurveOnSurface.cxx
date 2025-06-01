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

//: k1 abv 16.12.98 K4L PRO10107, PRO10108, PRO10109
//: j8 abv 10.12.98 TR10 r0501_db.stp #9423
//: S4030 abv, pdn: new methods - interface to standard ProjLib_CompProjectedCurve
//%12 pdn 15.02.99 PRO9234 optimizing
//%12 pdn 15.02.99 PRO9234 using improved ProjectDegenerated method
//    rln 03.03.99 S4135: bm2_sd_t4-A.stp treatment of Geom_SphericalSurface together with V-closed
//    surfaces
//: p9 abv 11.03.99 PRO7226 #489490: make IsAnIsoparametric to find nearest case
//: q1 abv 15.03.99 (pdn) PRO7226 #525030: limit NextValueOfUV() by tolerance
//: q5 abv 19.03.99 code improvement
//: q9 abv 23.03.99 PRO7226.stp #489490: cache for projecting end points
// #78 rln 12.03.99 S4135: checking spatial closure with myPreci
//     pdn 12.03.99 S4135: creating pcurve with minimal length in the case of densed points
//     abv 29.03.99 IsAnIsoparametric with Precision::Confusion
//     pdn 09.04.99 IsAnisoparametric uses already computed parameters (S4030, fix PRO14323)
// szv#4 S4163
//: s5 abv 22.04.99  Adding debug printouts in catch {} blocks
// #1  svv 11.01.00  Porting on DEC

#include <Approx_CurveOnSurface.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor.hxx>
#include <Geom2dAPI_Interpolate.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_Interpolate.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pnt2d.hxx>
#include <ElCLib.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Curve.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeConstruct_ProjectCurveOnSurface.hxx>
#include <ShapeExtend.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <IntRes2d_Domain.hxx>
#include <IntCurve_IntConicConic.hxx>

#include <algorithm>
IMPLEMENT_STANDARD_RTTIEXT(ShapeConstruct_ProjectCurveOnSurface, Standard_Transient)

#define NCONTROL 23

namespace
{

//=================================================================================================
// Forward declarations for internal functions
//=================================================================================================
Standard_Integer generateGeneralPoints(const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams);

Standard_Integer generateBSplinePoints(const Handle(Geom_BSplineCurve)&         theBSpline,
                                       const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams);

//=================================================================================================
// function : adjustSecondToFirstPoint
// purpose  : Adjust second point to first point considering surface periodicity
//=================================================================================================
void adjustSecondToFirstPoint(const gp_Pnt2d&             theFirstPoint,
                              gp_Pnt2d&                   theSecondPoint,
                              const Handle(Geom_Surface)& theSurface)
{
  if (theSurface->IsUPeriodic())
  {
    const Standard_Real anUPeriod = theSurface->UPeriod();
    const Standard_Real aNewU     = ElCLib::InPeriod(theSecondPoint.X(),
                                                 theFirstPoint.X() - anUPeriod / 2,
                                                 theFirstPoint.X() + anUPeriod / 2);
    theSecondPoint.SetX(aNewU);
  }
  if (theSurface->IsVPeriodic())
  {
    const Standard_Real aVPeriod = theSurface->VPeriod();
    const Standard_Real aNewV    = ElCLib::InPeriod(theSecondPoint.Y(),
                                                 theFirstPoint.Y() - aVPeriod / 2,
                                                 theFirstPoint.Y() + aVPeriod / 2);
    theSecondPoint.SetY(aNewV);
  }
}

//=================================================================================================
// function : getContainerValue
// purpose  : Get value from container using 1-based indexing
//=================================================================================================
template <typename T>
const typename T::value_type& getContainerValue(const T&               theSequence,
                                                const Standard_Integer theIndex)
{
  return theSequence.Value(theIndex - 1); // Sequence is 0-based, but we use 1-based index
}

//=================================================================================================
// function : changeContainerValue
// purpose  : Change value in container using 1-based indexing
//=================================================================================================
template <typename T>
typename T::value_type& changeContainerValue(T& theSequence, const Standard_Integer theIndex)
{
  return theSequence.ChangeValue(theIndex - 1); // Sequence is 0-based, but we use 1-based index
}

//=================================================================================================
// function : setContainerValue
// purpose  : Set value in container using 1-based indexing
//=================================================================================================
template <typename T>
void setContainerValue(T&                            theSequence,
                       const Standard_Integer        theIndex,
                       const typename T::value_type& theValue)
{
  theSequence.SetValue(theIndex - 1, theValue); // Sequence is 0-based, but we use 1-based index
}

//=================================================================================================
// function : fillPointsAndParams
// purpose  : Fill points and parameters arrays from curve adapter
//=================================================================================================
void fillPointsAndParams(const Handle(GeomAdaptor_Curve)&               theCurveAdaptor,
                         const NCollection_DynamicArray<Standard_Real>& theFinalParams,
                         NCollection_DynamicArray<gp_Pnt>&              thePoints,
                         NCollection_DynamicArray<Standard_Real>&       theParams)
{
  // Generate points at all final parameters
  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(theFinalParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aParam = anIterator.Value();
    gp_Pnt              aPoint;
    theCurveAdaptor->D0(aParam, aPoint);
    thePoints.Append(aPoint);
    theParams.Append(aParam);
  }
}

//=================================================================================================
// function : generateBSplinePoints
// purpose  : Generate parameters and points for B-spline curves
//=================================================================================================
Standard_Integer generateBSplinePoints(const Handle(Geom_BSplineCurve)&         theBSpline,
                                       const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams)
{
  // Extract B-spline properties for robust parameter generation
  const Standard_Integer aDegree    = theBSpline->Degree();
  const Standard_Integer aNbPoles   = theBSpline->NbPoles();
  const Standard_Integer aNbKnots   = theBSpline->NbKnots();
  const Standard_Boolean isPeriodic = theBSpline->IsPeriodic();
  const Standard_Real    anInterval = theLast - theFirst;
  
  // Safety check for degenerate cases
  if (aNbPoles < 2 || aDegree < 1 || anInterval <= Precision::PConfusion())
  {
    // Fallback to simple linear distribution
    return generateGeneralPoints(theCurveAdaptor, theFirst, theLast, theNbControlPoints, thePoints, theParams);
  }

  // Step 1: Calculate Greville parameters using BSplCLib utilities for maximum robustness
  // Greville parameter for control point i: t_i = (u_{i+1} + u_{i+2} + ... + u_{i+p}) / p
  // These parameters represent the optimal evaluation points for each control point's influence
  NCollection_DynamicArray<Standard_Real> aGrevilleParams;
  NCollection_DynamicArray<Standard_Real> aKnotParams;
  
  try 
  {
    // Use BSplCLib to get proper knot multiplicities and handle periodicity correctly
    const TColStd_Array1OfReal& aKnots = theBSpline->Knots();
    const TColStd_Array1OfInteger& aMults = theBSpline->Multiplicities();
    
    // Build complete knot vector with multiplicities using BSplCLib approach
    TColStd_Array1OfReal aFullKnotVector(1, BSplCLib::KnotSequenceLength(aMults, aDegree, isPeriodic));
    BSplCLib::KnotSequence(aKnots, aMults, aDegree, isPeriodic, aFullKnotVector);
    
    // Calculate Greville parameters for each control point
    for (Standard_Integer iPole = 1; iPole <= aNbPoles; ++iPole)
    {
      // For control point i, sum knots from (i+1) to (i+degree)
      Standard_Real aGrevilleSum = 0.0;
      Standard_Integer aValidKnots = 0;
      
      for (Standard_Integer j = 1; j <= aDegree; ++j)
      {
        const Standard_Integer aKnotIndex = iPole + j;
        if (aKnotIndex >= 1 && aKnotIndex <= aFullKnotVector.Length())
        {
          aGrevilleSum += aFullKnotVector(aKnotIndex);
          aValidKnots++;
        }
      }
      
      if (aValidKnots > 0)
      {
        const Standard_Real aGrevilleParam = aGrevilleSum / Standard_Real(aValidKnots);
        
        // Only include if within parameter range with tolerance
        if (aGrevilleParam >= theFirst - Precision::PConfusion() && 
            aGrevilleParam <= theLast + Precision::PConfusion())
        {
          // Clamp to exact bounds to prevent numerical drift
          const Standard_Real aClampedParam = Max(theFirst, Min(theLast, aGrevilleParam));
          aGrevilleParams.Append(aClampedParam);
        }
      }
    }
    
    // Also collect all distinct knot values as structural critical points
    for (Standard_Integer iKnot = 1; iKnot <= aKnots.Length(); ++iKnot)
    {
      const Standard_Real aKnotValue = aKnots(iKnot);
      if (aKnotValue >= theFirst - Precision::PConfusion() && 
          aKnotValue <= theLast + Precision::PConfusion())
      {
        const Standard_Real aClampedKnot = Max(theFirst, Min(theLast, aKnotValue));
        aKnotParams.Append(aClampedKnot);
      }
    }
  }
  catch (const Standard_Failure& anException)
  {
    // Enhanced fallback: use basic knot extraction if BSplCLib fails
    #ifdef OCCT_DEBUG
    std::cout << "BSplCLib failed, using fallback: " << anException.GetMessageString() << std::endl;
    #endif
    
    // Simple knot-based fallback
    for (Standard_Integer iKnot = 1; iKnot <= aNbKnots; ++iKnot)
    {
      const Standard_Real aKnotParam = theBSpline->Knot(iKnot);
      if (aKnotParam >= theFirst - Precision::PConfusion() && 
          aKnotParam <= theLast + Precision::PConfusion())
      {
        const Standard_Real aClampedParam = Max(theFirst, Min(theLast, aKnotParam));
        aGrevilleParams.Append(aClampedParam);
      }
    }
  }
  catch (...)
  {
    // Ultimate fallback to uniform distribution
    #ifdef OCCT_DEBUG
    std::cout << "All B-spline analysis failed, using uniform fallback" << std::endl;
    #endif
    
    return generateGeneralPoints(theCurveAdaptor, theFirst, theLast, theNbControlPoints, thePoints, theParams);
  }
  
  // Step 2: Merge all critical parameters (Greville + knots + boundaries)
  NCollection_DynamicArray<Standard_Real> aCriticalParams;
  
  // Always include exact boundaries
  aCriticalParams.Append(theFirst);
  aCriticalParams.Append(theLast);
  
  // Add all Greville parameters (these represent control point influence centers)
  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(aGrevilleParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aParam = anIterator.Value();
    if (aParam > theFirst + Precision::PConfusion() && 
        aParam < theLast - Precision::PConfusion())
    {
      aCriticalParams.Append(aParam);
    }
  }
  
  // Add all distinct knot parameters (structural discontinuities)
  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(aKnotParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aParam = anIterator.Value();
    if (aParam > theFirst + Precision::PConfusion() && 
        aParam < theLast - Precision::PConfusion())
    {
      aCriticalParams.Append(aParam);
    }
  }
  
  // Step 3: Sort and intelligently consolidate critical parameters
  std::sort(aCriticalParams.begin(), aCriticalParams.end());
  
  // Calculate adaptive spacing based on curve complexity and parameter range
  // For high-complexity curves (many poles/knots), use tighter spacing
  const Standard_Real aComplexityFactor = Standard_Real(aNbPoles + aNbKnots);
  const Standard_Real aBaseSpacing = anInterval / Max(aComplexityFactor * 0.5, 10.0);
  const Standard_Real aMinSpacing = Max(Precision::PConfusion() * 10.0, aBaseSpacing * 0.05);
  const Standard_Real aIdealSpacing = aBaseSpacing * 0.2;
  
  // Advanced consolidation: preserve critical parameters but merge very close ones intelligently
  NCollection_DynamicArray<Standard_Real> aConsolidatedCritical;
  
  if (aCriticalParams.Length() > 0)
  {
    aConsolidatedCritical.Append(aCriticalParams[0]); // Always keep first
    
    for (Standard_Integer i = 1; i < aCriticalParams.Length(); ++i)
    {
      const Standard_Real aCurrParam = aCriticalParams[i];
      const Standard_Real aPrevParam = aConsolidatedCritical.Last();
      const Standard_Real aDistance = aCurrParam - aPrevParam;
      
      if (aDistance >= aMinSpacing)
      {
        // Keep this parameter as it's sufficiently spaced
        aConsolidatedCritical.Append(aCurrParam);
      }
      else if (aDistance >= aMinSpacing * 0.5 && aConsolidatedCritical.Length() < aNbPoles)
      {
        // For moderate spacing and if we haven't reached pole count, keep it
        aConsolidatedCritical.Append(aCurrParam);
      }
      // Otherwise, skip this parameter as it's too close to the previous one
    }
  }
  
  // Step 4: Add intermediate parameters using adaptive density control
  // For complex B-splines, theNbControlPoints controls the overall target density
  NCollection_DynamicArray<Standard_Real> aFinalParams;
  
  // Calculate target parameter density based on B-spline complexity
  const Standard_Real aTargetDensity = Standard_Real(Max(theNbControlPoints, aNbPoles / 3));
  const Standard_Real aExpectedTotalParams = Min(aTargetDensity, aComplexityFactor);
  const Standard_Real aCurrentCriticalCount = Standard_Real(aConsolidatedCritical.Length());
  
  // Determine how many intermediate parameters we need
  const Standard_Real aNeededIntermediates = Max(0.0, aExpectedTotalParams - aCurrentCriticalCount);
  
  for (Standard_Integer i = 0; i < aConsolidatedCritical.Length(); ++i)
  {
    aFinalParams.Append(aConsolidatedCritical[i]);
    
    // Add intermediate points between consecutive critical parameters
    if (i < aConsolidatedCritical.Length() - 1)
    {
      const Standard_Real aStart = aConsolidatedCritical[i];
      const Standard_Real aEnd   = aConsolidatedCritical[i + 1];
      const Standard_Real aSegmentLength = aEnd - aStart;
      
      // Calculate intermediate points based on segment importance and remaining budget
      const Standard_Real aSegmentRatio = aSegmentLength / anInterval;
      const Standard_Real aSegmentBudget = aNeededIntermediates * aSegmentRatio;
      
      // Use minimum ideal spacing to determine actual intermediate count
      const Standard_Integer aMaxIntermediates = Standard_Integer(aSegmentLength / aIdealSpacing) - 1;
      const Standard_Integer aDesiredIntermediates = Standard_Integer(aSegmentBudget);
      const Standard_Integer aActualIntermediates = Min(Max(aDesiredIntermediates, 0), 
                                                         Min(aMaxIntermediates, 8)); // Cap at 8 per segment
      
      // Add intermediate points if the segment warrants them
      if (aActualIntermediates >= 1 && aSegmentLength > aIdealSpacing * 2.0)
      {
        for (Standard_Integer j = 1; j <= aActualIntermediates; ++j)
        {
          const Standard_Real aIntermediateParam = aStart + (aSegmentLength * j) / (aActualIntermediates + 1);
          aFinalParams.Append(aIntermediateParam);
        }
      }
    }
  }
  
  // Step 5: Final sort to ensure proper parameter ordering
  std::sort(aFinalParams.begin(), aFinalParams.end());
  
  // Step 6: Final validation and cleanup
  NCollection_DynamicArray<Standard_Real> aValidatedParams;
  Standard_Real aPrevValidParam = -Precision::Infinite();
  
  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(aFinalParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aParam = anIterator.Value();
    if (aParam >= theFirst && aParam <= theLast && 
        aParam - aPrevValidParam >= Precision::PConfusion())
    {
      aValidatedParams.Append(aParam);
      aPrevValidParam = aParam;
    }
  }
  
  // Ensure we have at least minimum required parameters
  if (aValidatedParams.Length() < 2)
  {
    aValidatedParams.Clear();
    aValidatedParams.Append(theFirst);
    aValidatedParams.Append(theLast);
  }
  
  //#ifdef OCCT_DEBUG
  std::cout << "Advanced B-spline parameter generation:" << std::endl
            << "  Input: degree=" << aDegree 
            << ", poles=" << aNbPoles 
            << ", knots=" << aNbKnots
            << ", periodic=" << (isPeriodic ? "yes" : "no")
            << ", range=[" << theFirst << ", " << theLast << "]"
            << ", interval=" << anInterval << std::endl
            << "  Analysis: Greville=" << aGrevilleParams.Length()
            << ", knot params=" << aKnotParams.Length()
            << ", critical before consolidation=" << aCriticalParams.Length()
            << ", critical after consolidation=" << aConsolidatedCritical.Length() << std::endl
            << "  Spacing: base=" << aBaseSpacing
            << ", min=" << aMinSpacing  
            << ", ideal=" << aIdealSpacing << std::endl
            << "  Result: final parameters=" << aValidatedParams.Length()
            << ", target density=" << aTargetDensity 
            << ", complexity factor=" << aComplexityFactor << std::endl;
  //#endif
  
  // Generate points at all validated parameters
  fillPointsAndParams(theCurveAdaptor, aValidatedParams, thePoints, theParams);
  
  return aValidatedParams.Length();
}

//=================================================================================================
// function : generateGeneralPoints
// purpose  : Generate parameters and points for general curves using uniform distribution
//=================================================================================================
Standard_Integer generateGeneralPoints(const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams)
{
  // For non-B-spline curves, use uniform distribution
  NCollection_DynamicArray<Standard_Real> aFinalParams;
  const Standard_Real aDeltaParam = (theLast - theFirst) / (theNbControlPoints - 1);

  for (Standard_Integer i = 1; i <= theNbControlPoints; ++i)
  {
    Standard_Real aParam;
    if (i == 1)
      aParam = theFirst;
    else if (i == theNbControlPoints)
      aParam = theLast;
    else
      aParam = theFirst + (i - 1) * aDeltaParam;

    aFinalParams.Append(aParam);
  }

  // Fill points and parameters
  fillPointsAndParams(theCurveAdaptor, aFinalParams, thePoints, theParams);

  return theNbControlPoints;
}

//=================================================================================================
// function : generateCurvePoints
// purpose  : Main function to generate points and parameters for any curve type
//=================================================================================================
Standard_Integer generateCurvePoints(const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                     const Handle(Geom_Curve)&                theCurve,
                                     const Standard_Real                      theFirst,
                                     const Standard_Real                      theLast,
                                     const Standard_Integer                   theNbControlPoints,
                                     NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                     NCollection_DynamicArray<Standard_Real>& theParams)
{
  // Check if it's a B-spline curve
  Handle(Geom_BSplineCurve) aBSpline;
  if (theCurve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    Handle(Geom_TrimmedCurve) aTrimmedCurve = Handle(Geom_TrimmedCurve)::DownCast(theCurve);
    aBSpline = Handle(Geom_BSplineCurve)::DownCast(aTrimmedCurve->BasisCurve());
  }
  else
  {
    aBSpline = Handle(Geom_BSplineCurve)::DownCast(theCurve);
  }

  if (!aBSpline.IsNull())
  {
    // Use B-spline specialized function
    return generateBSplinePoints(aBSpline,
                                 theCurveAdaptor,
                                 theFirst,
                                 theLast,
                                 theNbControlPoints,
                                 thePoints,
                                 theParams);
  }
  else
  {
    // Use general curve function
    return generateGeneralPoints(theCurveAdaptor,
                                 theFirst,
                                 theLast,
                                 theNbControlPoints,
                                 thePoints,
                                 theParams);
  }
}

} // namespace

//=================================================================================================

ShapeConstruct_ProjectCurveOnSurface::ShapeConstruct_ProjectCurveOnSurface()
{
  myPreci           = Precision::Confusion();
  myAdjustOverDegen = 1; //: c0 //szv#4:S4163:12Mar99 was boolean
  myNbCache         = 0; //: q9
  myStatus          = ShapeExtend::EncodeStatus(ShapeExtend_OK);
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::Init(const Handle(Geom_Surface)& surf,
                                                const Standard_Real         preci)
{
  Init(new ShapeAnalysis_Surface(surf), preci);
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::Init(const Handle(ShapeAnalysis_Surface)& surf,
                                                const Standard_Real                  preci)
{
  SetSurface(surf);
  SetPrecision(preci);
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetSurface(const Handle(Geom_Surface)& surf)
{
  SetSurface(new ShapeAnalysis_Surface(surf));
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetSurface(const Handle(ShapeAnalysis_Surface)& surf)
{
  if (mySurf == surf)
    return;
  mySurf    = surf;
  myNbCache = 0; //: q9
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::SetPrecision(const Standard_Real preci)
{
  myPreci = preci;
}

//=================================================================================================

//: c0

// szv#4:S4163:12Mar99 was Boolean
Standard_Integer& ShapeConstruct_ProjectCurveOnSurface::AdjustOverDegenMode()
{
  return myAdjustOverDegen;
}

//=================================================================================================

Standard_Boolean ShapeConstruct_ProjectCurveOnSurface::Status(
  const ShapeExtend_Status theStatus) const
{
  return ShapeExtend::DecodeStatus(myStatus, theStatus);
}

//=================================================================================================

Standard_Boolean ShapeConstruct_ProjectCurveOnSurface::Perform(const Handle(Geom_Curve)& theC3d,
                                                               const Standard_Real       First,
                                                               const Standard_Real       Last,
                                                               Handle(Geom2d_Curve)&     c2d,
                                                               const Standard_Real       TolFirst,
                                                               const Standard_Real       TolLast)
{
  myStatus = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  // Standard_Boolean OK = Standard_True; //szv#4:S4163:12Mar99 not needed

  if (mySurf.IsNull() || theC3d.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return Standard_False;
  }

  Handle(GeomAdaptor_Curve) c3d = new GeomAdaptor_Curve(theC3d, First, Last);

  //  Projection Analytique
  if (theC3d->IsKind(STANDARD_TYPE(Geom_BoundedCurve)))
  {
    c2d = ProjectAnalytic(theC3d);
  }
  else
  {
    c2d = ProjectAnalytic(new Geom_TrimmedCurve(theC3d, First, Last));
  }
  if (!c2d.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
    return Standard_True;
  }

  SequenceOfPnt  points(NCONTROL);
  SequenceOfReal params(NCONTROL);

  generateCurvePoints(c3d, theC3d, First, Last, NCONTROL, points, params);

  //  CALCUL par approximation
  SequenceOfPnt2d pnt2d;
  ApproxPCurve(points.Length(), c3d, TolFirst, TolLast, points, params, pnt2d, c2d);
  const Standard_Integer nbPini = points.Length();
  if (!c2d.IsNull())
  {
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    return Standard_True;
  } // cas particulier d iso

  Handle(TColgp_HArray1OfPnt2d) thePnts2d   = new TColgp_HArray1OfPnt2d(1, nbPini);
  Handle(TColStd_HArray1OfReal) theParams2d = new TColStd_HArray1OfReal(1, nbPini);
  for (Standard_Integer iPnt = 1; iPnt <= nbPini; iPnt++)
  {
    theParams2d->SetValue(iPnt, getContainerValue(params, iPnt));
    thePnts2d->SetValue(iPnt, getContainerValue(pnt2d, iPnt));
  }
  c2d = InterpolatePCurve(nbPini, thePnts2d, theParams2d);

  // Faut-il aussi reprendre la C3D ?
  myStatus |= ShapeExtend::EncodeStatus(c2d.IsNull() ? ShapeExtend_FAIL1 : ShapeExtend_DONE2);
  return Status(ShapeExtend_DONE);
}

//=================================================================================================

Standard_Boolean ShapeConstruct_ProjectCurveOnSurface::PerformByProjLib(
  const Handle(GeomAdaptor_Curve)& c3d,
  Handle(Geom2d_Curve)&            c2d)
{
  // Standard_Boolean OK = Standard_True; //szv#4:S4163:12Mar99 unused
  c2d.Nullify();
  if (mySurf.IsNull())
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL1);
    return Standard_False;
  }

  try
  {
    OCC_CATCH_SIGNALS
    Handle(GeomAdaptor_Surface) GAS = mySurf->Adaptor3d();
    Handle(GeomAdaptor_Curve)   GAC = c3d;
    ProjLib_ProjectedCurve      Projector(GAS, GAC);

    switch (Projector.GetType())
    {
      case GeomAbs_Line:
        c2d = new Geom2d_Line(Projector.Line());
        break;
      case GeomAbs_Circle:
        c2d = new Geom2d_Circle(Projector.Circle());
        break;
      case GeomAbs_Ellipse:
        c2d = new Geom2d_Ellipse(Projector.Ellipse());
        break;
      case GeomAbs_Parabola:
        c2d = new Geom2d_Parabola(Projector.Parabola());
        break;
      case GeomAbs_Hyperbola:
        c2d = new Geom2d_Hyperbola(Projector.Hyperbola());
        break;
      case GeomAbs_BSplineCurve:
        c2d = Projector.BSpline();
        break;
      default:
        // Not possible, handling added to avoid gcc warning.
        break;
    }

    if (c2d.IsNull())
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL2);
      return Standard_False;
    }
    else
    {
      myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      return Standard_True;
    }
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::PerformByProjLib(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL3);
    c2d.Nullify();
  }
  return Standard_False;
}

//=================================================================================================

Handle(Geom2d_Curve) ShapeConstruct_ProjectCurveOnSurface::ProjectAnalytic(
  const Handle(Geom_Curve)& c3d) const
{
  Handle(Geom2d_Curve) result;

  //: k1 abv 16 Dec 98: limit analytic cases by Plane surfaces only
  // This is necessary for K4L since it fails on other surfaces
  // when general method GeomProjLib::Curve2d() is used
  // Projection is done as in BRep_Tool and BRepCheck_Edge
  Handle(Geom_Surface) surf  = mySurf->Surface();
  Handle(Geom_Plane)   Plane = Handle(Geom_Plane)::DownCast(surf);
  if (Plane.IsNull())
  {
    Handle(Geom_RectangularTrimmedSurface) RTS =
      Handle(Geom_RectangularTrimmedSurface)::DownCast(surf);
    if (!RTS.IsNull())
      Plane = Handle(Geom_Plane)::DownCast(RTS->BasisSurface());
    else
    {
      Handle(Geom_OffsetSurface) OS = Handle(Geom_OffsetSurface)::DownCast(surf);
      if (!OS.IsNull())
        Plane = Handle(Geom_Plane)::DownCast(OS->BasisSurface());
    }
  }
  if (!Plane.IsNull())
  {
    Handle(Geom_Curve) ProjOnPlane =
      GeomProjLib::ProjectOnPlane(c3d, Plane, Plane->Position().Direction(), Standard_True);
    Handle(GeomAdaptor_Curve) HC = new GeomAdaptor_Curve(ProjOnPlane);
    ProjLib_ProjectedCurve    Proj(mySurf->Adaptor3d(), HC);

    result = Geom2dAdaptor::MakeCurve(Proj);
    if (result.IsNull())
      return result;
    if (result->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      Handle(Geom2d_TrimmedCurve) TC = Handle(Geom2d_TrimmedCurve)::DownCast(result);
      result                         = TC->BasisCurve();
    }

    return result;
  }

  return result;
}

//! Fix possible period jump and handle walking period parameter.
static Standard_Boolean fixPeriodictyTroubles(
  gp_Pnt2d*        thePnt,        // pointer to gp_Pnt2d[4] beginning
  Standard_Integer theIdx,        // Index of objective coord: 1 ~ X, 2 ~ Y
  Standard_Real    thePeriod,     // Period on objective coord
  Standard_Integer theSavedPoint, // Point number to choose period
  Standard_Real    theSavedParam)    // Param from cache to choose period
{
  Standard_Real    aSavedParam;
  Standard_Integer aSavedPoint;
  Standard_Real    aMinParam = 0.0, aMaxParam = thePeriod;
  if (theSavedPoint < 0)
  {
    // normalize to first period by default
    aSavedParam = 0.5 * thePeriod;
    aSavedPoint = 0;
  }
  else
  {
    aSavedParam = theSavedParam;
    aSavedPoint = theSavedPoint;
    while (aMinParam > aSavedParam)
    {
      aMinParam -= thePeriod;
      aMaxParam -= thePeriod;
    }
    while (aMaxParam < aSavedParam)
    {
      aMinParam += thePeriod;
      aMaxParam += thePeriod;
    }
  }

  Standard_Real    aFixIsoParam = aMinParam;
  Standard_Boolean isIsoLine    = Standard_False;
  if (aMaxParam - aSavedParam < Precision::PConfusion()
      || aSavedParam - aMinParam < Precision::PConfusion())
  {
    aFixIsoParam = aSavedParam;
    isIsoLine    = Standard_True;
  }
  // normalize all coordinates to [aMinParam, aMaxParam)
  for (Standard_Integer i = 0; i < 4; i++)
  {
    Standard_Real aParam = thePnt[i].Coord(theIdx);
    Standard_Real aShift = ShapeAnalysis::AdjustToPeriod(aParam, aMinParam, aMaxParam);
    aParam += aShift;
    // Walk over period coord -> not walking on another isoline in parameter space.
    if (isIsoLine)
    {
      if (aMaxParam - aParam < Precision::PConfusion()
          || aParam - aMinParam < Precision::PConfusion())
        aParam = aFixIsoParam;
    }
    else
    {
      if (aMaxParam - aParam < Precision::PConfusion())
        aParam = aMaxParam;
      if (aParam - aMinParam < Precision::PConfusion())
        aParam = aMinParam;
    }

    thePnt[i].SetCoord(theIdx, aParam);
  }

  // find possible period jump and increasing or decreasing coordinates vector
  Standard_Boolean isJump    = Standard_False;
  Standard_Real    aPrevDiff = 0.0;
  Standard_Real    aSumDiff  = 1.0;
  for (Standard_Integer i = 0; i < 3; i++)
  {
    Standard_Real aDiff = thePnt[i + 1].Coord(theIdx) - thePnt[i].Coord(theIdx);
    if (aDiff < -Precision::PConfusion())
    {
      aSumDiff *= -1.0;
    }
    // if first derivative changes its sign then period jump may exists in this place
    if (aDiff * aPrevDiff < -Precision::PConfusion())
    {
      isJump = Standard_True;
    }
    aPrevDiff = aDiff;
  }

  if (!isJump)
    return Standard_False;

  if (aSumDiff > 0)
  { // decreasing sequence (parameters decrease twice(--) and one period jump(+))
    for (Standard_Integer i = aSavedPoint; i > 0; i--)
      if (thePnt[i].Coord(theIdx) > thePnt[i - 1].Coord(theIdx))
      {
        thePnt[i - 1].SetCoord(theIdx, thePnt[i - 1].Coord(theIdx) + thePeriod);
      }
    for (Standard_Integer i = aSavedPoint; i < 3; i++)
      if (thePnt[i].Coord(theIdx) < thePnt[i + 1].Coord(theIdx))
      {
        thePnt[i + 1].SetCoord(theIdx, thePnt[i + 1].Coord(theIdx) - thePeriod);
      }
  }
  else
  { // increasing sequence (parameters increase twice(++) and one period jump(-))
    for (Standard_Integer i = aSavedPoint; i > 0; i--)
      if (thePnt[i].Coord(theIdx) < thePnt[i - 1].Coord(theIdx))
      {
        thePnt[i - 1].SetCoord(theIdx, thePnt[i - 1].Coord(theIdx) - thePeriod);
      }
    for (Standard_Integer i = aSavedPoint; i < 3; i++)
      if (thePnt[i].Coord(theIdx) > thePnt[i + 1].Coord(theIdx))
      {
        thePnt[i + 1].SetCoord(theIdx, thePnt[i + 1].Coord(theIdx) + thePeriod);
      }
  }

  // Do not return false, because for nonlinear 2d curves vector of parameters
  // may change its first derivative and shifted parameters will be broken for this case.
  return Standard_True;
}

//=================================================================================================

Handle(Geom2d_Curve) ShapeConstruct_ProjectCurveOnSurface::getLine(
  const SequenceOfPnt&  thepoints,
  const SequenceOfReal& theparams,
  SequenceOfPnt2d&      thePnt2d,
  Standard_Real         theTol,
  Standard_Boolean&     isRecompute,
  Standard_Boolean&     isFromCache) const
{
  Standard_Integer nb = thepoints.Length();
  gp_Pnt           aP[4];
  aP[0] = thepoints.First();
  aP[1] = getContainerValue(thepoints, 2);
  aP[2] = getContainerValue(thepoints, nb - 1);
  aP[3] = thepoints.Last();
  gp_Pnt2d         aP2d[4];
  Standard_Integer i = 0;

  Standard_Real    aTol2       = theTol * theTol;
  Standard_Boolean isPeriodicU = mySurf->Surface()->IsUPeriodic();
  Standard_Boolean isPeriodicV = mySurf->Surface()->IsVPeriodic();

  // Workaround:
  // Protection against bad "tolerance" shapes.
  if (aTol2 > 1.0)
  {
    theTol = Precision::Confusion();
    aTol2  = theTol * theTol;
  }
  if (aTol2 < Precision::SquareConfusion())
    aTol2 = Precision::SquareConfusion();
  Standard_Real anOldTol2 = aTol2;
  // auxiliary variables to choose period for connection with previous 2dcurve (if exist)
  Standard_Integer aSavedPointNum = -1;
  gp_Pnt2d         aSavedPoint;

  // project first and last points
  for (; i < 4; i += 3)
  {
    Standard_Integer j;
    for (j = 0; j < myNbCache; j++)
    {
      if (myCache3d[j].SquareDistance(aP[i]) < aTol2)
      {
        aP2d[i]        = mySurf->NextValueOfUV(myCache2d[j], aP[i], theTol, theTol);
        aSavedPointNum = i;
        aSavedPoint    = myCache2d[j];
        if (i == 0)
          isFromCache = Standard_True;
        break;
      }
    }

    if (j >= myNbCache)
      aP2d[i] = mySurf->ValueOfUV(aP[i], theTol);

    Standard_Real aDist    = mySurf->Gap();
    Standard_Real aCurDist = aDist * aDist;
    if (aTol2 < aDist * aDist)
      aTol2 = aCurDist;
  }

  if (isPeriodicU || isPeriodicV)
  {
    // Compute second and last but one c2d points.
    for (i = 1; i < 3; i++)
    {
      Standard_Integer j;
      for (j = 0; j < myNbCache; j++)
      {
        if (myCache3d[j].SquareDistance(aP[i]) < aTol2)
        {
          aP2d[i]        = mySurf->NextValueOfUV(myCache2d[j], aP[i], theTol, theTol);
          aSavedPointNum = i;
          aSavedPoint    = myCache2d[j];
          break;
        }
      }

      if (j >= myNbCache)
        aP2d[i] = mySurf->ValueOfUV(aP[i], theTol);

      Standard_Real aDist    = mySurf->Gap();
      Standard_Real aCurDist = aDist * aDist;
      if (aTol2 < aDist * aDist)
        aTol2 = aCurDist;
    }

    if (isPeriodicU)
    {
      isRecompute = fixPeriodictyTroubles(&aP2d[0],
                                          1 /* X Coord */,
                                          mySurf->Surface()->UPeriod(),
                                          aSavedPointNum,
                                          aSavedPoint.X());
    }

    if (isPeriodicV)
    {
      isRecompute = fixPeriodictyTroubles(&aP2d[0],
                                          2 /* Y Coord */,
                                          mySurf->Surface()->VPeriod(),
                                          aSavedPointNum,
                                          aSavedPoint.Y());
    }
  }

  if (isRecompute && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    // Do not try to make line, because in this case may be very special case when 3d curve
    // go over the pole of, e.g., sphere, and partly lies along seam
    // (see ApproxPCurve() for more information).
    return 0;
  }

  thePnt2d.ChangeFirst() = aP2d[0];
  thePnt2d.ChangeLast()  = aP2d[3];

  // Restore old tolerance in 2d space to avoid big gap cases.
  aTol2 = anOldTol2;
  // Check that straight line in 2d with parameterization as in 3d will fit
  // fit 3d curve at all points.
  const Standard_Real dPar = theparams.Last() - theparams.First();
  if (Abs(dPar) < Precision::PConfusion())
    return 0;
  gp_Vec2d             aVec0(aP2d[0], aP2d[3]);
  gp_Vec2d             aVec          = aVec0 / dPar;
  Handle(Geom_Surface) aSurf         = mySurf->Surface();
  Standard_Boolean     isNormalCheck = aSurf->IsCNu(1) && aSurf->IsCNv(1);
  if (isNormalCheck)
  {
    for (i = 1; i <= nb; i++)
    {
      gp_XY aCurPoint =
        aP2d[0].XY() + aVec.XY() * (getContainerValue(theparams, i) - theparams.First());
      gp_Pnt aCurP;
      gp_Vec aNormalVec, aDu, aDv;
      aSurf->D1(aCurPoint.X(), aCurPoint.Y(), aCurP, aDu, aDv);
      aNormalVec = aDu ^ aDv;
      if (aNormalVec.SquareMagnitude() < Precision::SquareConfusion())
      {
        isNormalCheck = Standard_False;
        break;
      }
      gp_Lin        aNormalLine(aCurP, gp_Dir(aNormalVec));
      Standard_Real aDist = aNormalLine.Distance(getContainerValue(thepoints, i));
      if (aDist > theTol)
        return 0;
    }
  }
  if (!isNormalCheck)
  {
    Standard_Real aFirstPointDist =
      mySurf->Surface()->Value(aP2d[0].X(), aP2d[0].Y()).SquareDistance(thepoints.First());
    aTol2 = Max(aTol2, aTol2 * 2 * aFirstPointDist);
    for (i = 2; i < nb; i++)
    {
      gp_XY aCurPoint =
        aP2d[0].XY() + aVec.XY() * (getContainerValue(theparams, i) - theparams.First());
      gp_Pnt aCurP;
      aSurf->D0(aCurPoint.X(), aCurPoint.Y(), aCurP);
      Standard_Real aDist1 = aCurP.SquareDistance(getContainerValue(thepoints, i));

      if (Abs(aFirstPointDist - aDist1) > aTol2)
        return 0;
    }
  }

  // check if pcurve can be represented by Geom2d_Line (parameterized by length)
  Standard_Real aLLength = aVec0.Magnitude();
  if (Abs(aLLength - dPar) <= Precision::PConfusion())
  {
    gp_XY    aDirL = aVec0.XY() / aLLength;
    gp_Pnt2d aPL(aP2d[0].XY() - theparams.First() * aDirL);
    return new Geom2d_Line(aPL, gp_Dir2d(aDirL));
  }

  // create straight bspline
  TColgp_Array1OfPnt2d aPoles(1, 2);
  aPoles(1) = aP2d[0];
  aPoles(2) = aP2d[3];

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = theparams.First();
  aKnots(2) = theparams.Last();

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1)                           = 2;
  aMults(2)                           = 2;
  Standard_Integer            aDegree = 1;
  Handle(Geom2d_BSplineCurve) abspl2d = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, aDegree);
  return abspl2d;
}

//=================================================================================================

Standard_Boolean ShapeConstruct_ProjectCurveOnSurface::ApproxPCurve(
  const Standard_Integer           nbrPnt,
  const Handle(GeomAdaptor_Curve)& c3d,
  const Standard_Real              TolFirst,
  const Standard_Real              TolLast,
  SequenceOfPnt&                   points,
  SequenceOfReal&                  params,
  SequenceOfPnt2d&                 pnt2d,
  Handle(Geom2d_Curve)&            c2d)
{
  // for performance, first try to handle typical case when pcurve is straight
  Standard_Boolean isRecompute     = Standard_False;
  Standard_Boolean isFromCacheLine = Standard_False;
  setContainerValue(pnt2d, nbrPnt, gp_Pnt2d(0., 0.)); // init all by {0, 0}
  c2d = getLine(points, params, pnt2d, myPreci, isRecompute, isFromCacheLine);
  if (!c2d.IsNull())
  {
    // fill cache
    Standard_Boolean ChangeCycle = Standard_False;
    if (myNbCache > 0
        && myCache3d[0].Distance(points.First()) > myCache3d[0].Distance(points.Last())
        && myCache3d[0].Distance(points.Last()) < Precision::Confusion())
      ChangeCycle = Standard_True;
    myNbCache = 2;
    if (ChangeCycle)
    {
      myCache3d[0] = points.First();
      myCache3d[1] = points.Last();
      myCache2d[0] = pnt2d.First();
      myCache2d[1] = pnt2d.Last();
    }
    else
    {
      myCache3d[1] = points.First();
      myCache3d[0] = points.Last();
      myCache2d[1] = pnt2d.First();
      myCache2d[0] = pnt2d.Last();
    }
    return Standard_True;
  }
  Standard_Boolean isDone = Standard_True;
  // test if the curve 3d is a boundary of the surface
  // (only for Bezier or BSpline surface)

  Standard_Boolean   isoParam, isoPar2d3d, isoTypeU, p1OnIso, p2OnIso, isoclosed;
  gp_Pnt2d           valueP1, valueP2;
  Handle(Geom_Curve) cIso;
  Standard_Real      t1, t2;

  Handle(Standard_Type) sType      = mySurf->Surface()->DynamicType();
  Standard_Boolean      isAnalytic = Standard_True;
  if (sType == STANDARD_TYPE(Geom_BezierSurface) || sType == STANDARD_TYPE(Geom_BSplineSurface))
    isAnalytic = Standard_False;
  Standard_Real uf, ul, vf, vl;
  mySurf->Surface()->Bounds(uf, ul, vf, vl);
  isoclosed = Standard_False;
  TColStd_Array1OfReal pout(1, nbrPnt);

  isoParam = IsAnIsoparametric(nbrPnt,
                               points,
                               params,
                               isoTypeU,
                               p1OnIso,
                               valueP1,
                               p2OnIso,
                               valueP2,
                               isoPar2d3d,
                               cIso,
                               t1,
                               t2,
                               pout);

  // projection of the points on surfaces

  gp_Pnt        p3d;
  gp_Pnt2d      p2d;
  Standard_Real isoValue = 0., isoPar1 = 0., isoPar2 = 0., tPar = 0., tdeb, tfin;
  Standard_Real Cf, Cl, parf, parl; // szv#4:S4163:12Mar99 dist not needed

  //  Le calcul part-il dans le bon sens, c-a-d deb et fin dans le bon ordre ?
  //  Si uclosed et iso en V, attention isoPar1 ET/OU 2 peut toucher la fermeture
  if (isoParam)
  {
    if (isoTypeU)
    {
      isoValue  = valueP1.X();
      isoPar1   = valueP1.Y();
      isoPar2   = valueP2.Y();
      isoclosed = mySurf->IsVClosed(myPreci); // #78 rln 12.03.99 S4135
      parf      = vf;
      parl      = vl;
    }
    else
    {
      isoValue  = valueP1.Y();
      isoPar1   = valueP1.X();
      isoPar2   = valueP2.X();
      isoclosed = mySurf->IsUClosed(myPreci); // #78 rln 12.03.99 S4135
      parf      = uf;
      parl      = ul;
    }
    if (!isoPar2d3d && !isAnalytic)
    {
      Cf = cIso->FirstParameter();
      Cl = cIso->LastParameter();
      if (Precision::IsInfinite(Cf))
        Cf = -1000;
      if (Precision::IsInfinite(Cl))
        Cl = +1000;
      // pdn S4030 optimizing and fix isopar case on PRO41323
      tdeb = pout(2);
      //  Chacun des par1 ou par2 est-il sur un bord. Attention first/last : recaler
      if (isoclosed && (isoPar1 == parf || isoPar1 == parl))
      {
        if (Abs(tdeb - parf) < Abs(tdeb - parl))
          isoPar1 = parf;
        else
          isoPar1 = parl;
        if (isoTypeU)
          valueP1.SetY(isoPar1);
        else
          valueP1.SetX(isoPar1);
      }
      if (isoclosed && (isoPar2 == parf || isoPar2 == parl))
      {
        // pdn S4030 optimizing and fix isopar case on PRO41323
        tfin = pout(nbrPnt - 1);
        if (Abs(tfin - parf) < Abs(tfin - parl))
          isoPar2 = parf;
        else
          isoPar2 = parl;
        if (isoTypeU)
          valueP2.SetY(isoPar2);
        else
          valueP2.SetX(isoPar2);
      }

      //  Interversion Par1/Par2 (ne veut que si les 2 sont sur les bords ...)
      //  Est-ce encore necessaire apres ce qui vient d etre fait ?

      // PTV 05.02.02 fix for translation face from 12_hp_mouse (PARASOLID) face 24008
      // if curve is periodic do not change the points
      // skl change "if" for pout(nbrPnt-1) 19.11.2003
      if (!isoclosed)
      {
        if ((Abs(tdeb - isoPar1) > Abs(tdeb - isoPar2))
            && (Abs(pout(nbrPnt - 1) - isoPar2) > Abs(pout(nbrPnt - 1) - isoPar1)))
        {
          gp_Pnt2d valueTmp = valueP1;
          valueP1           = valueP2;
          valueP2           = valueTmp;
          if (isoTypeU)
          {
            isoValue = valueP1.X();
            isoPar1  = valueP1.Y();
            isoPar2  = valueP2.Y();
          }
          else
          {
            isoValue = valueP1.Y();
            isoPar1  = valueP1.X();
            isoPar2  = valueP2.X();
          }
          //  Fin calcul sens de courbe iso
        }
      } // end of fix check 05.02.02
    }
  }

  Standard_Real    Up          = ul - uf;
  Standard_Real    Vp          = vl - vf;
  Standard_Real    gap         = myPreci;        //: q1
  Standard_Boolean ChangeCycle = Standard_False; // skl for OCC3430
  // auxiliaruy variables to shift 2dcurve, according to previous
  Standard_Boolean isFromCache = Standard_False;
  gp_Pnt2d         aSavedPoint;
  if (myNbCache > 0 && myCache3d[0].Distance(points.First()) > myCache3d[0].Distance(points.Last())
      && myCache3d[0].Distance(points.Last()) < Precision::Confusion())
  {
    ChangeCycle = Standard_True;
  }
  Standard_Boolean needResolveUJump = Standard_False;
  Standard_Boolean needResolveVJump = Standard_False;
  gp_Pnt           prevP3d;
  gp_Pnt2d         prevP2d;
  // for( i = 1; i <= nbrPnt; i ++) {
  for (Standard_Integer ii = 1; ii <= nbrPnt; ii++)
  {
    const Standard_Integer aPntIndex = ChangeCycle ? (nbrPnt - ii + 1) : ii;
    p3d                              = getContainerValue(points, aPntIndex);
    if (isoParam)
    {

      if (isoPar2d3d)
      {
        if (isoPar2 > isoPar1)
          tPar = getContainerValue(params, aPntIndex);
        else
          tPar = t1 + t2 - getContainerValue(params, aPntIndex);
      }
      else if (!isAnalytic)
      {
        // projection to iso
        if (aPntIndex == 1)
          tPar = isoPar1;
        else if (aPntIndex == nbrPnt)
          tPar = isoPar2;
        else
        {
          tPar = pout(aPntIndex);
          //: S4030  ShapeAnalysis_Curve().Project (cIso,p3d,myPreci,pt,tPar,Cf,Cl);
          //: //szv#4:S4163:12Mar99 `dist=` not needed
        }
      }

      if (!isoPar2d3d && isAnalytic)
      {
        if (aPntIndex == 1)
          p2d = valueP1;
        else if (aPntIndex == nbrPnt)
          p2d = valueP2;
        else
        {
          p2d = mySurf->NextValueOfUV(p2d,
                                      p3d,
                                      myPreci, //%12 pdn 15.02.99 optimizing
                                      Precision::Confusion() + 1000 * gap); //: q1
          gap = mySurf->Gap();
        }
      }
      else
      {
        if (isoTypeU)
        {
          p2d.SetX(isoValue);
          p2d.SetY(tPar);
        }
        else
        {
          p2d.SetX(tPar);
          p2d.SetY(isoValue);
        }
      }
    }

    else
    {
      if ((aPntIndex == 1) && p1OnIso)
        p2d = valueP1;
      else if ((aPntIndex == nbrPnt) && p2OnIso)
        p2d = valueP2;
      else
      { // general case (not an iso)  mais attention aux singularites !
        // first and last points are already computed by getLine()
        if (aPntIndex == 1 || aPntIndex == nbrPnt)
        {
          if (!isRecompute)
          {
            p2d = getContainerValue(pnt2d, aPntIndex);
            gap = mySurf->Gap();
            if (aPntIndex == 1)
            {
              isFromCache = isFromCacheLine;
              aSavedPoint = p2d;
            }
            continue;
          }
          else
          {
            //: q9 abv 23 Mar 99: use cache as 1st approach
            Standard_Integer j; // svv #1
            for (j = 0; j < myNbCache; ++j)
            {
              if (myCache3d[j].SquareDistance(p3d) < myPreci * myPreci)
              {
                p2d =
                  mySurf->NextValueOfUV(myCache2d[j], p3d, myPreci, Precision::Confusion() + gap);
                if (aPntIndex == 1)
                {
                  isFromCache = Standard_True;
                  aSavedPoint = myCache2d[j];
                }
                break;
              }
            }
            if (j >= myNbCache)
            {
              p2d = mySurf->ValueOfUV(p3d, myPreci);
            }
          }
        }
        else
        {
          p2d = mySurf->NextValueOfUV(p2d,
                                      p3d,
                                      myPreci,                              //: S4030: optimizing
                                      Precision::Confusion() + 1000 * gap); //: q1
        }
        gap = mySurf->Gap();
      }
    }
    changeContainerValue(pnt2d, aPntIndex) = p2d;
    if (nbrPnt > 23 && ii > 2 && ii < nbrPnt)
    {
      // additional check for possible invalid jump by U or V parameter
      if (fabs(p2d.X() - prevP2d.X()) > 0.95 * Up && prevP3d.Distance(p3d) < myPreci
          && !mySurf->IsUClosed(myPreci) && mySurf->NbSingularities(myPreci) > 0
          && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
      {
        needResolveUJump = Standard_True;
      }
      if (fabs(p2d.Y() - prevP2d.Y()) > 0.95 * Vp && prevP3d.Distance(p3d) < myPreci
          && !mySurf->IsVClosed(myPreci) && mySurf->NbSingularities(myPreci) > 0
          && mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_BSplineSurface)))
      {
        needResolveVJump = Standard_True;
      }
    }
    prevP3d = p3d;
    prevP2d = p2d;
    if (ii > 1)
    {
      if (ChangeCycle)
        p2d.SetXY(2. * p2d.XY() - getContainerValue(pnt2d, aPntIndex + 1).XY());
      else
        p2d.SetXY(2. * p2d.XY() - getContainerValue(pnt2d, aPntIndex - 1).XY());
    }
  }

  // pdn %12 11.02.99 PRO9234 entity 15402
  if (!isoPar2d3d)
  {
    mySurf->ProjectDegenerated(nbrPnt, points, pnt2d, myPreci, Standard_True);
    mySurf->ProjectDegenerated(nbrPnt, points, pnt2d, myPreci, Standard_False);
  }

  // Check the extremities of 3d curve for coinciding with singularities of surf
  // Standard_Integer NbSing = mySurf->NbSingularities(Precision::Confusion());
  gp_Pnt        PointFirst = points.First(), PointLast = points.Last();
  Standard_Real aTolFirst = (TolFirst == -1) ? Precision::Confusion() : TolFirst;
  Standard_Real aTolLast  = (TolLast == -1) ? Precision::Confusion() : TolLast;
  for (Standard_Integer i = 1;; i++)
  {
    Standard_Real    aPreci, aFirstPar, aLastPar;
    gp_Pnt           aP3d;
    gp_Pnt2d         aFirstP2d, aLastP2d;
    Standard_Boolean IsUiso;
    if (!mySurf->Singularity(i, aPreci, aP3d, aFirstP2d, aLastP2d, aFirstPar, aLastPar, IsUiso))
      break;
    if (aPreci <= Precision::Confusion() && PointFirst.Distance(aP3d) <= aTolFirst)
    {
      CorrectExtremity(c3d,
                       params,
                       pnt2d,
                       Standard_True, // first point
                       aFirstP2d,
                       IsUiso);
    }
    if (aPreci <= Precision::Confusion() && PointLast.Distance(aP3d) <= aTolLast)
    {
      CorrectExtremity(c3d,
                       params,
                       pnt2d,
                       Standard_False, // last point
                       aFirstP2d,
                       IsUiso);
    }
  }

  // Si la surface est UCLosed et VClosed, on recadre les points
  // algo un peu complique, on retarde l implementation
  // Standard_Real Up = ul - uf;
  // Standard_Real Vp = vl - vf;
  Standard_Real       dist2d;
  const Standard_Real TolOnUPeriod = Precision::Confusion() * Up;
  const Standard_Real TolOnVPeriod = Precision::Confusion() * Vp;
#ifdef OCCT_DEBUG
  if (mySurf->IsUClosed(myPreci) && mySurf->IsVClosed(myPreci))
  { // #78 rln 12.03.99 S4135
    std::cout << "WARNING : Recadrage incertain sur U & VClosed" << std::endl;
  }
#endif
  // Si la surface est UCLosed, on recadre les points
  if (mySurf->IsUClosed(myPreci) || needResolveUJump)
  { // #78 rln 12.03.99 S4135
    // Premier point dans le domain [uf, ul]
    Standard_Real prevX, firstX = pnt2d.First().X();
    if (!isFromCache)
    {
      // do not shift 2dcurve, if it connects to previous
      while (firstX < uf)
      {
        firstX += Up;
        pnt2d.ChangeFirst().SetX(firstX);
      }
      while (firstX > ul)
      {
        firstX -= Up;
        pnt2d.ChangeFirst().SetX(firstX);
      }
    }
    // shift first point, according to cache
    if (mySurf->Surface()->IsUPeriodic() && isFromCache)
    {
      Standard_Real aMinParam = uf, aMaxParam = ul;
      while (aMinParam > aSavedPoint.X())
      {
        aMinParam -= Up;
        aMaxParam -= Up;
      }
      while (aMaxParam < aSavedPoint.X())
      {
        aMinParam += Up;
        aMaxParam += Up;
      }
      Standard_Real aShift = ShapeAnalysis::AdjustToPeriod(firstX, aMinParam, aMaxParam);
      firstX += aShift;
      pnt2d.ChangeFirst().SetX(firstX);
    }
    prevX = firstX;

    //: 97 abv 1 Feb 98: treat case when curve is whole out of surface bounds
    Standard_Real    minX = firstX, maxX = firstX;
    Standard_Boolean ToAdjust = Standard_False;

    // On decalle toujours le suivant
    for (Standard_Integer aPntIter = 2; aPntIter <= pnt2d.Length(); ++aPntIter)
    {
      //      dist2d = pnt2d (aPntIter - 1).Distance(pnt2d (aPntIter));
      Standard_Real CurX = getContainerValue(pnt2d, aPntIter).X();
      dist2d             = Abs(CurX - prevX);
      if (dist2d > (Up / 2))
      {
        InsertAdditionalPointOrAdjust(ToAdjust,
                                      1,
                                      Up,
                                      TolOnUPeriod,
                                      CurX,
                                      prevX,
                                      c3d,
                                      aPntIter,
                                      points,
                                      params,
                                      pnt2d);
      }
      prevX = CurX;
      if (minX > CurX)
        minX = CurX; //: 97
      else if (maxX < CurX)
        maxX = CurX; //: 97
    }

    //: 97
    if (!isFromCache)
    {
      // do not shift 2dcurve, if it connects to previous
      Standard_Real midX   = 0.5 * (minX + maxX);
      Standard_Real shiftX = 0.;
      if (midX > ul)
        shiftX = -Up;
      else if (midX < uf)
        shiftX = Up;
      if (shiftX != 0.)
        for (Standard_Integer aPntIter = 1; aPntIter <= pnt2d.Length(); ++aPntIter)
          changeContainerValue(pnt2d, aPntIter)
            .SetX(getContainerValue(pnt2d, aPntIter).X() + shiftX);
    }
  }
  // Si la surface est VCLosed, on recadre les points
  // Same code as UClosed : optimisation souhaitable !!
  // CKY : d abord un code IDENTIQUE A UClosed; PUIS le special Seam ...
  // Si la surface est UCLosed, on recadre les points
  //
  // #69 rln 01.03.99 S4135 bm2_sd_t4-A.stp entity 30
  // #78 rln 12.03.99 S4135
  if (mySurf->IsVClosed(myPreci) || needResolveVJump
      || mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    // Premier point dans le domain [vf, vl]
    Standard_Real prevY, firstY = pnt2d.First().Y();
    if (!isFromCache)
    {
      // do not shift 2dcurve, if it connects to previous
      while (firstY < vf)
      {
        firstY += Vp;
        pnt2d.ChangeFirst().SetY(firstY);
      }
      while (firstY > vl)
      {
        firstY -= Vp;
        pnt2d.ChangeFirst().SetY(firstY);
      }
    }
    // shift first point, according to cache
    if (mySurf->Surface()->IsVPeriodic() && isFromCache)
    {
      Standard_Real aMinParam = vf, aMaxParam = vl;
      while (aMinParam > aSavedPoint.Y())
      {
        aMinParam -= Vp;
        aMaxParam -= Vp;
      }
      while (aMaxParam < aSavedPoint.Y())
      {
        aMinParam += Vp;
        aMaxParam += Vp;
      }
      Standard_Real aShift = ShapeAnalysis::AdjustToPeriod(firstY, aMinParam, aMaxParam);
      firstY += aShift;
      pnt2d.ChangeFirst().SetY(firstY);
    }
    prevY = firstY;

    //: 97 abv 1 Feb 98: treat case when curve is whole out of surface bounds
    Standard_Real    minY = firstY, maxY = firstY;
    Standard_Boolean ToAdjust = Standard_False;

    // On decalle toujours le suivant
    for (Standard_Integer aPntIter = 2; aPntIter <= pnt2d.Length(); ++aPntIter)
    {
      //      dist2d = pnt2d (i-1).Distance(pnt2d (i));
      Standard_Real CurY = getContainerValue(pnt2d, aPntIter).Y();
      dist2d             = Abs(CurY - prevY);
      if (dist2d > (Vp / 2))
      {
        InsertAdditionalPointOrAdjust(ToAdjust,
                                      2,
                                      Vp,
                                      TolOnVPeriod,
                                      CurY,
                                      prevY,
                                      c3d,
                                      aPntIter,
                                      points,
                                      params,
                                      pnt2d);
      }
      prevY = CurY;
      if (minY > CurY)
        minY = CurY; //: 97
      else if (maxY < CurY)
        maxY = CurY; //: 97
    }

    //: 97
    if (!isFromCache)
    {
      // do not shift 2dcurve, if it connects to previous
      Standard_Real midY   = 0.5 * (minY + maxY);
      Standard_Real shiftY = 0.;
      if (midY > vl)
        shiftY = -Vp;
      else if (midY < vf)
        shiftY = Vp;
      if (shiftY != 0.)
        for (Standard_Integer aPntIter = 1; aPntIter <= pnt2d.Length(); ++aPntIter)
          changeContainerValue(pnt2d, aPntIter)
            .SetY(getContainerValue(pnt2d, aPntIter).Y() + shiftY);
    }
  }

  // #69 rln 01.03.99 S4135 bm2_sd_t4-A.stp entity 30
  // #78 rln 12.03.99 S4135
  if (mySurf->IsVClosed(myPreci) || mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)))
  {
    for (Standard_Integer aPntIter = 2; aPntIter <= pnt2d.Length(); ++aPntIter)
    {
      // #1 rln 11/02/98 ca_exhaust.stp entity #9869 dist2d = pnt2d (i-1).Distance(pnt2d (i));
      dist2d =
        Abs(getContainerValue(pnt2d, aPntIter).Y() - getContainerValue(pnt2d, aPntIter - 1).Y());
      if (dist2d > (Vp / 2))
      {
        // ATTENTION : il faut regarder ou le decalage se fait.
        // si plusieurs points sont decalles, il faut plusieurs passes
        // pour obtenir un resultat correct.
        // NOT YET IMPLEMENTED

        // one of those point is incorrectly placed
        // i.e on the wrong side of the "seam"
        // on prend le point le plus pres des bords vf ou vl
        Standard_Boolean prevOnFirst = Standard_False;
        Standard_Boolean prevOnLast  = Standard_False;
        Standard_Boolean currOnFirst = Standard_False;
        Standard_Boolean currOnLast  = Standard_False;

        //  .X ?  plutot .Y ,  non ?
        Standard_Real distPrevVF = Abs(getContainerValue(pnt2d, aPntIter - 1).Y() - vf);
        Standard_Real distPrevVL = Abs(getContainerValue(pnt2d, aPntIter - 1).Y() - vl);
        Standard_Real distCurrVF = Abs(getContainerValue(pnt2d, aPntIter).Y() - vf);
        Standard_Real distCurrVL = Abs(getContainerValue(pnt2d, aPntIter).Y() - vl);

        Standard_Real theMin = distPrevVF;
        prevOnFirst          = Standard_True;
        if (distPrevVL < theMin)
        {
          theMin      = distPrevVL;
          prevOnFirst = Standard_False;
          prevOnLast  = Standard_True;
        }
        if (distCurrVF < theMin)
        {
          theMin      = distCurrVF;
          prevOnFirst = Standard_False;
          prevOnLast  = Standard_False;
          currOnFirst = Standard_True;
        }
        if (distCurrVL < theMin)
        {
          theMin      = distCurrVL;
          prevOnFirst = Standard_False;
          prevOnLast  = Standard_False;
          currOnFirst = Standard_False;
          currOnLast  = Standard_True;
        }
        //  Modifs RLN/Nijni  3-DEC-1997
        if (prevOnFirst)
        {
          // on decalle le point (aPntIter - 1) en V Last
          gp_Pnt2d newPrev(getContainerValue(pnt2d, aPntIter - 1).X(),
                           vf); // instead of  vl RLN/Nijni
          changeContainerValue(pnt2d, aPntIter - 1) = newPrev;
        }
        else if (prevOnLast)
        {
          // on decalle le point (aPntIter - 1) en V first
          gp_Pnt2d newPrev(getContainerValue(pnt2d, aPntIter - 1).X(),
                           vl); // instead of  vf RLN/Nijni
          changeContainerValue(pnt2d, aPntIter - 1) = newPrev;
        }
        else if (currOnFirst)
        {
          // on decalle le point (aPntIter) en V Last
          gp_Pnt2d newCurr(getContainerValue(pnt2d, aPntIter).X(), vf); // instead of vl  RLN/Nijni
          changeContainerValue(pnt2d, aPntIter) = newCurr;
        }
        else if (currOnLast)
        {
          // on decalle le point (aPntIter) en V First
          gp_Pnt2d newCurr(getContainerValue(pnt2d, aPntIter).X(), vl); // instead of vf  RLN/Nijni
          changeContainerValue(pnt2d, aPntIter) = newCurr;
        }
        // on verifie
#ifdef OCCT_DEBUG
        dist2d =
          getContainerValue(pnt2d, aPntIter - 1).Distance(getContainerValue(pnt2d, aPntIter));
        if (dist2d > (Vp / 2))
        {
          std::cout << "Echec dans le recadrage" << std::endl;
        }
#endif
      }
    }
  }

  //: c0 abv 20 Feb 98: treat very special case when 3d curve
  // go over the pole of, e.g., sphere, and partly lies along seam.
  // 2d representation of such a curve should consist of 3 parts - one on
  // regular part of surface (interior), one part along degenerated boundary
  // and one along seam.
  // Since it cannot be adjusted later by arranging pcurves (curve is single),
  // to fix it it's necessary to have a possibility of adjusting seam
  // part of such curve either to left or right boundary of surface.
  // Test is performed only if flag AdjustOverDegen is not -1.
  // If AdjustOverDegen is True, seam part of curve is adjusted to
  // the left, and if False - to the right parametric boundary
  // If treated case is detected, flag DONE4 is set to status
  // NOTE: currently, precision is Precision::PConfusion() since it
  // is enough on encountered example
  // (ug_turbine-A.stp from ProSTEP Benchmark #3, entities ##2470 & 5680)
  // (r1001_ac.stp from Test Rally #10, face #35027 and others)
  if (myAdjustOverDegen != -1)
  {
    if (mySurf->IsUClosed(myPreci))
    {                                                  // #78 rln 12.03.99 S4135
      mySurf->IsDegenerated(gp_Pnt(0, 0, 0), myPreci); // pour calculer les dgnr
      if (mySurf->NbSingularities(myPreci) > 0)
      { // rln S4135
        // 1st, find gap point (degenerated pole)
        Standard_Real    PrevX   = 0.;
        Standard_Integer OnBound = 0, PrevOnBound = 0;
        Standard_Integer ind; // svv #1
        Standard_Boolean start = Standard_True;
        for (ind = 1; ind <= pnt2d.Length(); ind++)
        {
          Standard_Real CurX = getContainerValue(pnt2d, ind).X();
          // abv 16 Mar 00: trj3_s1-ug.stp #697: ignore points in singularity
          if (mySurf->IsDegenerated(getContainerValue(points, ind), Precision::Confusion()))
            continue;
          OnBound = (Abs(Abs(CurX - 0.5 * (ul + uf)) - Up / 2) <= Precision::PConfusion());
          if (!start && Abs(Abs(CurX - PrevX) - Up / 2) <= 0.01 * Up)
            break;
          start       = Standard_False;
          PrevX       = CurX;
          PrevOnBound = OnBound;
        }
        // if found, adjust seam part
        if (ind <= pnt2d.Length())
        {
          PrevX            = (myAdjustOverDegen ? uf : ul);
          Standard_Real dU = Up / 2 + Precision::PConfusion();
          if (PrevOnBound)
          {
            changeContainerValue(pnt2d, ind - 1).SetX(PrevX);
            for (Standard_Integer j = ind - 2; j > 0; j--)
            {
              Standard_Real CurX = getContainerValue(pnt2d, j).X();
              while (CurX < PrevX - dU)
                changeContainerValue(pnt2d, j).SetX(CurX += Up);
              while (CurX > PrevX + dU)
                changeContainerValue(pnt2d, j).SetX(CurX -= Up);
            }
          }
          else if (OnBound)
          {
            changeContainerValue(pnt2d, ind).SetX(PrevX);
            for (Standard_Integer j = ind + 1; j <= pnt2d.Length(); j++)
            {
              Standard_Real CurX = getContainerValue(pnt2d, j).X();
              while (CurX < PrevX - dU)
                changeContainerValue(pnt2d, j).SetX(CurX += Up);
              while (CurX > PrevX + dU)
                changeContainerValue(pnt2d, j).SetX(CurX -= Up);
            }
          }
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        }
      }
    }
    else if (mySurf->IsVClosed(myPreci))
    {                                                  // #78 rln 12.03.99 S4135
      mySurf->IsDegenerated(gp_Pnt(0, 0, 0), myPreci); // pour calculer les dgnr
      if (mySurf->NbSingularities(myPreci) > 0)
      { // rln S4135
        // 1st, find gap point (degenerated pole)
        Standard_Real    PrevY   = 0.;
        Standard_Integer OnBound = 0, PrevOnBound = 0;
        Standard_Integer ind; // svv #1
        Standard_Boolean start = Standard_True;
        for (ind = 1; ind <= pnt2d.Length(); ind++)
        {
          Standard_Real CurY = getContainerValue(pnt2d, ind).Y();
          // abv 16 Mar 00: trj3_s1-ug.stp #697: ignore points in singularity
          if (mySurf->IsDegenerated(getContainerValue(points, ind), Precision::Confusion()))
            continue;
          OnBound = (Abs(Abs(CurY - 0.5 * (vl + vf)) - Vp / 2) <= Precision::PConfusion());
          if (!start && Abs(Abs(CurY - PrevY) - Vp / 2) <= 0.01 * Vp)
            break;
          start       = Standard_False;
          PrevY       = CurY;
          PrevOnBound = OnBound;
        }
        // if found, adjust seam part
        if (ind <= pnt2d.Length())
        {
          PrevY            = (myAdjustOverDegen ? vf : vl);
          Standard_Real dV = Vp / 2 + Precision::PConfusion();
          if (PrevOnBound)
          {
            changeContainerValue(pnt2d, ind - 1).SetY(PrevY);
            for (Standard_Integer j = ind - 2; j > 0; j--)
            {
              Standard_Real CurY = getContainerValue(pnt2d, j).Y();
              while (CurY < PrevY - dV)
                changeContainerValue(pnt2d, j).SetY(CurY += Vp);
              while (CurY > PrevY + dV)
                changeContainerValue(pnt2d, j).SetY(CurY -= Vp);
            }
          }
          else if (OnBound)
          {
            changeContainerValue(pnt2d, ind).SetY(PrevY);
            for (Standard_Integer j = ind + 1; j <= pnt2d.Length(); j++)
            {
              Standard_Real CurY = getContainerValue(pnt2d, j).Y();
              while (CurY < PrevY - dV)
                changeContainerValue(pnt2d, j).SetY(CurY += Vp);
              while (CurY > PrevY + dV)
                changeContainerValue(pnt2d, j).SetY(CurY -= Vp);
            }
          }
          myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE4);
        }
      }
    }
  }

  //: q9: fill cache
  myNbCache = 2;
  if (ChangeCycle)
  {
    myCache3d[0] = points.First();
    myCache3d[1] = points.Last();
    myCache2d[0] = pnt2d.First();
    myCache2d[1] = pnt2d.Last();
  }
  else
  {
    myCache3d[1] = points.First();
    myCache3d[0] = points.Last();
    myCache2d[1] = pnt2d.First();
    myCache2d[0] = pnt2d.Last();
  }
  return isDone;
}

//=================================================================================================

Handle(Geom2d_Curve) ShapeConstruct_ProjectCurveOnSurface::ApproximatePCurve(
  Handle(TColgp_HArray1OfPnt2d)& points2d,
  Handle(TColStd_HArray1OfReal)& params) const
{
  //  Standard_Real resol = Min(mySurf->Adaptor3d()->VResolution(myPreci),
  //  mySurf->Adaptor3d()->UResolution(myPreci));
  Standard_Real        theTolerance2d = myPreci; // (100*nbrPnt);//resol;
  Handle(Geom2d_Curve) C2d;
  try
  {
    OCC_CATCH_SIGNALS
    CheckPoints2d(points2d, params, theTolerance2d);
    Standard_Integer numberPnt = points2d->Length();

    TColgp_Array1OfPnt points3d(1, numberPnt);
    gp_Pnt2d           pnt2d;
    gp_Pnt             pnt;
    Standard_Integer   i; // svv #1
    for (i = 1; i <= numberPnt; i++)
    {
      pnt2d = points2d->Value(i);
      pnt.SetCoord(pnt2d.X(), pnt2d.Y(), 0);
      points3d(i) = pnt;
    }

    GeomAPI_PointsToBSpline appr(points3d, params->Array1(), 1, 10, GeomAbs_C1, theTolerance2d);
    const Handle(Geom_BSplineCurve)& crv3d   = appr.Curve();
    Standard_Integer                 NbPoles = crv3d->NbPoles();
    TColgp_Array1OfPnt               poles3d(1, NbPoles);
    TColgp_Array1OfPnt2d             poles2d(1, NbPoles);
    crv3d->Poles(poles3d);
    for (i = 1; i <= NbPoles; i++)
    {
      pnt2d.SetCoord(poles3d(i).X(), poles3d(i).Y());
      poles2d(i) = pnt2d;
    }
    TColStd_Array1OfReal    weights(1, NbPoles);
    TColStd_Array1OfInteger multiplicities(1, crv3d->NbKnots());
    TColStd_Array1OfReal    knots(1, crv3d->NbKnots());
    crv3d->Knots(knots);
    crv3d->Weights(weights);
    crv3d->Multiplicities(multiplicities);
    C2d = new Geom2d_BSplineCurve(poles2d,
                                  weights,
                                  knots,
                                  multiplicities,
                                  crv3d->Degree(),
                                  crv3d->IsPeriodic());
    return C2d;
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //: s5
    //    debug ...
    Standard_Integer nbp = params->Length();
    Standard_Integer nb2 = points2d->Length();
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::ApproximatePCurve(): Exception: ";
    anException.Print(std::cout);
    std::cout << "Pb Geom2dAPI_Approximate, tol2d=" << theTolerance2d << " NbParams=" << nbp
              << " NbPnts=" << nb2 << std::endl;
#endif
    (void)anException;
    C2d.Nullify();
  }
  return C2d;
}

//=================================================================================================

Handle(Geom2d_Curve) ShapeConstruct_ProjectCurveOnSurface::InterpolatePCurve(
  const Standard_Integer         nbrPnt,
  Handle(TColgp_HArray1OfPnt2d)& points2d,
  Handle(TColStd_HArray1OfReal)& params) const
{
  Handle(Geom2d_Curve) C2d; // NULL si echec
  Standard_Real        theTolerance2d = myPreci / (100 * nbrPnt);
  try
  {
    OCC_CATCH_SIGNALS
    // on verifie d abord s il n y a pas de points confondus
    // si besoin on retouche les valeurs ...
    CheckPoints2d(points2d, params, theTolerance2d);
    Geom2dAPI_Interpolate myInterPol2d(points2d, params, Standard_False, theTolerance2d);
    myInterPol2d.Perform();
    if (myInterPol2d.IsDone())
      C2d = myInterPol2d.Curve();
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    Standard_Integer nbp = params->Length();
    Standard_Integer nb2 = points2d->Length();
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::InterpolatePCurve(): Exception: ";
    anException.Print(std::cout);
    std::cout << "Pb Geom2dAPI_Interpolate, tol2d=" << theTolerance2d << " NbParams=" << nbp
              << " NbPnts=" << nb2 << std::endl;
#endif
    (void)anException;
    C2d.Nullify();
  }
  return C2d;
}

//=================================================================================================

Handle(Geom_Curve) ShapeConstruct_ProjectCurveOnSurface::InterpolateCurve3d(
  const Standard_Integer,
  Handle(TColgp_HArray1OfPnt)&   points,
  Handle(TColStd_HArray1OfReal)& params) const
{
  Handle(Geom_Curve) C3d; // NULL si echec
  try
  {
    OCC_CATCH_SIGNALS
    Standard_Real Tol = myPreci;
    CheckPoints(points, params, Tol);
    GeomAPI_Interpolate myInterPol(points, params, Standard_False, Tol);
    myInterPol.Perform();
    if (myInterPol.IsDone())
      C3d = myInterPol.Curve();
  }
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //: s5
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::InterpolateCurve3d(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    C3d.Nullify();
  }
  return C3d;
}

//============================================================================================
// function : CorrectExtremity
// purpose  : corrects first or last 2d point of future curve
//           in the case when it coincids with a singularity of surface
//============================================================================================

void ShapeConstruct_ProjectCurveOnSurface::CorrectExtremity(const Handle(GeomAdaptor_Curve)& theC3d,
                                                            const SequenceOfReal&  theParams,
                                                            SequenceOfPnt2d&       thePnt2d,
                                                            const Standard_Boolean theIsFirstPoint,
                                                            const gp_Pnt2d& thePointOnIsoLine,
                                                            const Standard_Boolean theIsUiso)
{
  Standard_Integer NbPnt            = thePnt2d.Length();
  Standard_Integer IndCoord         = (theIsUiso) ? 2 : 1;
  Standard_Real    SingularityCoord = thePointOnIsoLine.Coord(3 - IndCoord);
  gp_Pnt2d         EndPoint         = (theIsFirstPoint) ? thePnt2d.First() : thePnt2d.Last();
  Standard_Real    FinishCoord      = EndPoint.Coord(3 - IndCoord); // the constant coord of isoline

  gp_Dir2d        aDir = (theIsUiso) ? gp::DY2d() : gp::DX2d();
  gp_Lin2d        anIsoLine(EndPoint, aDir);
  IntRes2d_Domain Dom1, Dom2;

  Standard_Boolean IsPeriodic =
    (theIsUiso) ? mySurf->Surface()->IsVPeriodic() : mySurf->Surface()->IsUPeriodic();

  gp_Pnt2d      FirstPointOfLine, SecondPointOfLine;
  Standard_Real FinishParam, FirstParam, SecondParam;

  if (theIsFirstPoint)
  {
    FirstPointOfLine  = getContainerValue(thePnt2d, 3);
    SecondPointOfLine = getContainerValue(thePnt2d, 2);
    FinishParam       = getContainerValue(theParams, 1);
    FirstParam        = getContainerValue(theParams, 3);
    SecondParam       = getContainerValue(theParams, 2);
  }
  else // last point
  {
    FirstPointOfLine  = getContainerValue(thePnt2d, NbPnt - 2);
    SecondPointOfLine = getContainerValue(thePnt2d, NbPnt - 1);
    FinishParam       = getContainerValue(theParams, NbPnt);
    FirstParam        = getContainerValue(theParams, NbPnt - 2);
    SecondParam       = getContainerValue(theParams, NbPnt - 1);
  }

  if (SingularityCoord > FinishCoord && SecondPointOfLine.Coord(3 - IndCoord) > FinishCoord)
    return; // the curve passes through the singularity, do nothing
  if (SingularityCoord < FinishCoord && SecondPointOfLine.Coord(3 - IndCoord) < FinishCoord)
    return; // the curve passes through the singularity, do nothing
  // Check correctness of <EndPoint>
  {
    const Standard_Real aPrevDist =
      Abs(SecondPointOfLine.Coord(IndCoord) - FirstPointOfLine.Coord(IndCoord));
    const Standard_Real aCurDist =
      Abs(EndPoint.Coord(IndCoord) - SecondPointOfLine.Coord(IndCoord));
    if (aCurDist <= 2 * aPrevDist)
      return;
  }

  gp_Pnt2d FinishPoint =
    (theIsUiso)
      ? gp_Pnt2d(FinishCoord, SecondPointOfLine.Y())
      : gp_Pnt2d(SecondPointOfLine.X(), FinishCoord); // first approximation of <FinishPoint>

  for (;;)
  {
    if (Abs(SecondPointOfLine.Coord(3 - IndCoord) - FinishCoord) <= 2 * Precision::PConfusion())
      break;

    gp_Vec2d      aVec(FirstPointOfLine, SecondPointOfLine);
    Standard_Real aSqMagnitude = aVec.SquareMagnitude();
    if (aSqMagnitude <= 1.e-32)
      break;
    aDir.SetCoord(aVec.X(), aVec.Y());

    gp_Lin2d               aLine(FirstPointOfLine, aDir);
    IntCurve_IntConicConic Intersector(anIsoLine, Dom1, aLine, Dom2, 1.e-10, 1.e-10);
    if (Intersector.IsDone() && !Intersector.IsEmpty())
    {
      IntRes2d_IntersectionPoint IntPoint = Intersector.Point(1);
      FinishPoint                         = IntPoint.Value();
    }
    else
      FinishPoint = (theIsUiso) ? gp_Pnt2d(FinishCoord, SecondPointOfLine.Y())
                                : gp_Pnt2d(SecondPointOfLine.X(), FinishCoord);

    gp_Pnt2d PrevPoint = FirstPointOfLine;
    FirstPointOfLine   = SecondPointOfLine;
    FirstParam         = SecondParam;
    SecondParam        = (FirstParam + FinishParam) / 2;
    if (Abs(SecondParam - FirstParam) <= 2 * Precision::PConfusion())
      break;
    gp_Pnt aP3d;
    theC3d->D0(SecondParam, aP3d);
    SecondPointOfLine =
      mySurf->NextValueOfUV(FirstPointOfLine, aP3d, myPreci, Precision::Confusion());
    if (IsPeriodic)
      adjustSecondToFirstPoint(FirstPointOfLine, SecondPointOfLine, mySurf->Surface());

    // Check <SecondPointOfLine> to be enough close to <FirstPointOfLine>
    // because when a projected point is too close to singularity,
    // the non-constant coordinate becomes random.
    const Standard_Real aPrevDist =
      Abs(FirstPointOfLine.Coord(IndCoord) - PrevPoint.Coord(IndCoord));
    const Standard_Real aCurDist =
      Abs(SecondPointOfLine.Coord(IndCoord) - FirstPointOfLine.Coord(IndCoord));
    if (aCurDist > 2 * aPrevDist)
      break;
  }

  if (theIsFirstPoint)
  {
    thePnt2d.ChangeFirst() = FinishPoint;
  }
  else
  {
    thePnt2d.ChangeLast() = FinishPoint;
  }
}

//============================================================================================
// function : InsertAdditionalPointOrAdjust
// purpose  : If the current point is too far from the previous point
//           (more than half-period of surface), it can happen in two cases:
//           1. Real current step on corresponding coordinate is small, all we need is adjust;
//           2. Current step on corresponding coordinate is really bigger than half-period of
//           surface in this parametric direction, so we must add additional point to exclude
//           such big intervals between points in 2d space.
//============================================================================================

void ShapeConstruct_ProjectCurveOnSurface::InsertAdditionalPointOrAdjust(
  Standard_Boolean&                ToAdjust,
  const Standard_Integer           theIndCoord,
  const Standard_Real              Period,
  const Standard_Real              TolOnPeriod,
  Standard_Real&                   CurCoord,
  const Standard_Real              prevCoord,
  const Handle(GeomAdaptor_Curve)& c3d,
  Standard_Integer&                theIndex,
  SequenceOfPnt&                   points,
  SequenceOfReal&                  params,
  SequenceOfPnt2d&                 pnt2d)
{

  // Lambda for inserting element at specified index using SetValue
  auto insertAt = [](auto& theArray, const Standard_Integer theIndex, const auto& theValue) {
    const Standard_Integer aLength = theArray.Length();

    // Extend array by setting a value at the new last position
    theArray.SetValue(aLength, theArray.Value(aLength - 1));

    // Move elements from the end towards the insertion point
    for (Standard_Integer i = aLength; i > theIndex; i--)
    {
      theArray.SetValue(i, theArray.Value(i - 1));
    }

    // Set the new value at the insertion point
    theArray.SetValue(theIndex, theValue);
  };

  Standard_Real CorrectedCurCoord =
    ElCLib::InPeriod(CurCoord, prevCoord - Period / 2, prevCoord + Period / 2);
  if (!ToAdjust)
  {
    Standard_Real CurPar  = getContainerValue(params, theIndex);
    Standard_Real PrevPar = getContainerValue(params, theIndex - 1);
    Standard_Real MidPar  = (PrevPar + CurPar) / 2;
    gp_Pnt        MidP3d;
    c3d->D0(MidPar, MidP3d);
    gp_Pnt2d      MidP2d   = mySurf->ValueOfUV(MidP3d, myPreci);
    Standard_Real MidCoord = MidP2d.Coord(theIndCoord);
    MidCoord = ElCLib::InPeriod(MidCoord, prevCoord - Period / 2, prevCoord + Period / 2);
    Standard_Real FirstCoord = prevCoord, LastCoord = CorrectedCurCoord;
    if (LastCoord < FirstCoord)
    {
      Standard_Real tmp = FirstCoord;
      FirstCoord        = LastCoord;
      LastCoord         = tmp;
    }
    if (LastCoord - FirstCoord <= TolOnPeriod)
      ToAdjust = Standard_True;
    else if (FirstCoord <= MidCoord && MidCoord <= LastCoord)
      ToAdjust = Standard_True;
    else // add mid point
    {
      // Standard_Real RefU = prevX;
      Standard_Boolean Success = Standard_True;
      Standard_Real    FirstT  = PrevPar; // getContainerValue(params, i-1)
      Standard_Real    LastT   = CurPar;  // getContainerValue(params, i)
      MidCoord                 = MidP2d.Coord(theIndCoord);
      while (Abs(MidCoord - prevCoord) >= Period / 2 - TolOnPeriod
             || Abs(CurCoord - MidCoord) >= Period / 2 - TolOnPeriod)
      {
        if (MidPar - FirstT <= Precision::PConfusion() || LastT - MidPar <= Precision::PConfusion())
        {
          Success = Standard_False;
          break; // wrong choice
        }
        if (Abs(MidCoord - prevCoord) >= Period / 2 - TolOnPeriod)
          LastT = (FirstT + LastT) / 2;
        else
          FirstT = (FirstT + LastT) / 2;
        MidPar = (FirstT + LastT) / 2;
        c3d->D0(MidPar, MidP3d);
        MidP2d   = mySurf->ValueOfUV(MidP3d, myPreci);
        MidCoord = MidP2d.Coord(theIndCoord);
      }
      if (Success)
      {
        // Insert operations using lambda
        insertAt(points, theIndex - 1, MidP3d);
        insertAt(params, theIndex - 1, MidPar);
        insertAt(pnt2d, theIndex - 1, MidP2d);
        theIndex++;
      }
      else
        ToAdjust = Standard_True;
    } // add mid point
  } // if (!ToAdjust)
  if (ToAdjust)
  {
    CurCoord = CorrectedCurCoord;
    changeContainerValue(pnt2d, theIndex).SetCoord(theIndCoord, CurCoord);
  }
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::CheckPoints(Handle(TColgp_HArray1OfPnt)&   points,
                                                       Handle(TColStd_HArray1OfReal)& params,
                                                       Standard_Real&                 preci) const
{
  Standard_Integer firstElem = points->Lower();
  Standard_Integer lastElem  = points->Upper();
  Standard_Integer i;
  Standard_Integer nbPntDropped = 0;
  Standard_Integer lastValid    = firstElem; // indice of last undropped point

  // will store 0 when the point is to be removed, 1 otherwise
  TColStd_Array1OfInteger tmpParam(firstElem, lastElem);
  for (i = firstElem; i <= lastElem; i++)
    tmpParam.SetValue(i, 1);
  Standard_Real DistMin2 = RealLast();
  gp_Pnt        Prev     = points->Value(lastValid);
  gp_Pnt        Curr;
  for (i = firstElem + 1; i <= lastElem; i++)
  {
    Curr                   = points->Value(i);
    Standard_Real CurDist2 = Prev.SquareDistance(Curr);
    if (CurDist2 < gp::Resolution())
    { // test 0
      nbPntDropped++;
      if (i == lastElem)
        tmpParam.SetValue(lastValid, 0); // last point kept
      else
        tmpParam.SetValue(i, 0); // current dropped, lastValid unchanged
    }
    else
    {
      if (CurDist2 < DistMin2)
        DistMin2 = CurDist2;
      // lastValid becomes the current (i.e. i)
      lastValid = i;
      Prev      = Curr;
    }
  }
  if (DistMin2 < RealLast())
  {
    // preci est la distance min entre les points on la reduit un peu
    preci = 0.9 * Sqrt(DistMin2);
  }
  if (nbPntDropped == 0)
    return;

#ifdef OCCT_DEBUG
  std::cout << "Warning : removing 3d points for interpolation" << std::endl;
#endif
  // Build new HArrays
  Standard_Integer newLast = lastElem - nbPntDropped;
  if ((newLast - firstElem + 1) < 2)
  {
#ifdef OCCT_DEBUG
    std::cout << "Too many degenerated points for 3D interpolation" << std::endl;
#endif
    return;
  }
  Handle(TColgp_HArray1OfPnt)   newPnts   = new TColgp_HArray1OfPnt(firstElem, newLast);
  Handle(TColStd_HArray1OfReal) newParams = new TColStd_HArray1OfReal(firstElem, newLast);
  Standard_Integer              newCurr   = 1;
  for (i = firstElem; i <= lastElem; i++)
  {
    if (tmpParam.Value(i) == 1)
    {
      newPnts->SetValue(newCurr, points->Value(i));
      newParams->SetValue(newCurr, params->Value(i));
      newCurr++;
    }
  }
  points = newPnts;
  params = newParams;
  // on la reduit un peu
}

//=================================================================================================

void ShapeConstruct_ProjectCurveOnSurface::CheckPoints2d(Handle(TColgp_HArray1OfPnt2d)& points,
                                                         Handle(TColStd_HArray1OfReal)& params,
                                                         Standard_Real&                 preci) const
{
  Standard_Integer firstElem = points->Lower();
  Standard_Integer lastElem  = points->Upper();
  Standard_Integer i;
  Standard_Integer nbPntDropped = 0;
  Standard_Integer lastValid    = firstElem; // indice of last undropped point

  // will store 0 when the point is to be removed, 1 otherwise
  TColStd_Array1OfInteger tmpParam(firstElem, lastElem);
  for (i = firstElem; i <= lastElem; i++)
  {
    tmpParam.SetValue(i, 1);
  }
  Standard_Real DistMin2 = RealLast();
  gp_Pnt2d      Prev     = points->Value(lastValid);
  gp_Pnt2d      Curr;
  for (i = firstElem + 1; i <= lastElem; i++)
  {
    Curr                   = points->Value(i);
    Standard_Real CurDist2 = Prev.SquareDistance(Curr);
    if (CurDist2 < gp::Resolution())
    { // test 0
      nbPntDropped++;
      if (i == lastElem)
        tmpParam.SetValue(lastValid, 0); // last point kept
      else
        tmpParam.SetValue(i, 0); // current dropped, lastValid unchanged
    }
    else
    {
      if (CurDist2 < DistMin2)
        DistMin2 = CurDist2;
      // lastValid becomes the current (i.e. i)
      lastValid = i;
      Prev      = Curr;
    }
  }
  if (DistMin2 < RealLast())
    preci = 0.9 * Sqrt(DistMin2);
  if (nbPntDropped == 0)
    return;

#ifdef OCCT_DEBUG
  std::cout << "Warning : removing 2d points for interpolation" << std::endl;
#endif
  // Build new HArrays
  Standard_Integer newLast = lastElem - nbPntDropped;
  if ((newLast - firstElem + 1) < 2)
  {
#ifdef OCCT_DEBUG
    std::cout << "Too many degenerated points for 2D interpolation" << std::endl;
#endif
    // pdn 12.02.99 S4135 Creating pcurve with minimal length.
    tmpParam.SetValue(firstElem, 1);
    tmpParam.SetValue(lastElem, 1);
    gp_XY lastPnt = points->Value(lastElem).XY();
    lastPnt.Add(gp_XY(preci, preci));
    points->SetValue(lastElem, lastPnt);
    newLast = firstElem + 1;
    // return;
  }
  Handle(TColgp_HArray1OfPnt2d) newPnts   = new TColgp_HArray1OfPnt2d(firstElem, newLast);
  Handle(TColStd_HArray1OfReal) newParams = new TColStd_HArray1OfReal(firstElem, newLast);
  Standard_Integer              newCurr   = 1;
  for (i = firstElem; i <= lastElem; i++)
  {
    if (tmpParam.Value(i) == 1)
    {
#ifdef OCCT_DEBUG
      std::cout << "Point " << i << " : " << points->Value(i).X() << " " << points->Value(i).Y()
                << " at param " << params->Value(i) << std::endl;
#endif
      newPnts->SetValue(newCurr, points->Value(i));
      newParams->SetValue(newCurr, params->Value(i));
      newCurr++;
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "Removed " << i << " : " << points->Value(i).X() << " " << points->Value(i).Y()
                << " at param " << params->Value(i) << std::endl;
#endif
    }
  }
  points = newPnts;
  params = newParams;
}

//=================================================================================================

//: S4030: modified for optimization
//: p9 abv 11 Mar 99: PRO7226 #489490: find nearest boundary instead of first one

Standard_Boolean ShapeConstruct_ProjectCurveOnSurface::IsAnIsoparametric(
  const Standard_Integer nbrPnt,
  const SequenceOfPnt&   points,
  const SequenceOfReal&  params,
  Standard_Boolean&      isoTypeU,
  Standard_Boolean&      p1OnIso,
  gp_Pnt2d&              valueP1,
  Standard_Boolean&      p2OnIso,
  gp_Pnt2d&              valueP2,
  Standard_Boolean&      isoPar2d3d,
  Handle(Geom_Curve)&    cIso,
  Standard_Real&         t1,
  Standard_Real&         t2,
  TColStd_Array1OfReal&  pout) const
{
  try
  { // RAJOUT
    OCC_CATCH_SIGNALS

    constexpr Standard_Real prec = Precision::Confusion(); // myPreci;

    Standard_Boolean isoParam = Standard_False;
    isoPar2d3d                = Standard_False;

    Standard_Real U1, U2, V1, V2;
    mySurf->Bounds(U1, U2, V1, V2);

    if (mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface)))
    {
      Handle(Geom_RectangularTrimmedSurface) sTrim =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(mySurf->Surface());
      sTrim->Bounds(U1, U2, V1, V2);
    }

    gp_Pnt           pt;
    Standard_Integer mpt[2];
    mpt[0] = mpt[1] = 0;
    Standard_Real t, tpar[2] = {0.0, 0.0}, isoValue = 0.;
    Standard_Real mindist2;
    Standard_Real mind2[2];
    mindist2 = mind2[0] = mind2[1] = 4 * prec * prec;

    p1OnIso             = Standard_False;
    p2OnIso             = Standard_False;
    const Bnd_Box* aBox = 0;

    for (Standard_Integer j = 1; (j <= 4) /*&& !isoParam*/; j++)
    {
      Standard_Real    isoVal = 0.;
      Standard_Boolean isoU = Standard_False; // szv#4:S4163:12Mar99 `isoU` must be Standard_Boolean
      Handle(Geom_Curve) cI;
      Standard_Real      tt1, tt2;

      if (j == 1)
      {
        if (Precision::IsInfinite(U1))
          continue;
        cI     = mySurf->UIso(U1);
        isoU   = Standard_True;
        isoVal = U1;
        aBox   = &mySurf->GetBoxUF();
      }
      else if (j == 2)
      {
        if (Precision::IsInfinite(U2))
          continue;
        cI     = mySurf->UIso(U2);
        isoU   = Standard_True;
        isoVal = U2;
        aBox   = &mySurf->GetBoxUL();
      }
      else if (j == 3)
      {
        if (Precision::IsInfinite(V1))
          continue;
        cI     = mySurf->VIso(V1);
        isoU   = Standard_False;
        isoVal = V1;
        aBox   = &mySurf->GetBoxVF();
      }
      else if (j == 4)
      {
        if (Precision::IsInfinite(V2))
          continue;
        cI     = mySurf->VIso(V2);
        isoU   = Standard_False;
        isoVal = V2;
        aBox   = &mySurf->GetBoxVL();
      }
      if (cI.IsNull())
        continue;

      if (isoU)
      {
        tt1 = V1;
        tt2 = V2;
      }
      else
      {
        tt1 = U1;
        tt2 = U2;
      }

      gp_Pnt ext1, ext2;
      cI->D0(tt1, ext1);
      cI->D0(tt2, ext2);

      // PATCH CKY 9-JUL-1998 : protection contre singularite
      gp_Pnt extmi;
      cI->D0((tt1 + tt2) / 2, extmi);
      if (ext1.IsEqual(ext2, prec) && ext1.IsEqual(extmi, prec))
        continue;

      Standard_Boolean PtEQext1 = Standard_False;
      Standard_Boolean PtEQext2 = Standard_False;

      Standard_Real    currd2[2], tp[2] = {0, 0};
      Standard_Integer mp[2];

      for (Standard_Integer i = 0; i < 2; i++)
      {
        mp[i]              = 0;
        Standard_Integer k = (i == 0 ? 1 : nbrPnt);

        // si ext1 == ext2 => valueP1 == valueP2 => vect null plus tard
        currd2[i] = getContainerValue(points, k).SquareDistance(ext1);
        if (currd2[i] <= prec * prec && !PtEQext1)
        {
          mp[i]    = 1;
          tp[i]    = tt1;
          PtEQext1 = Standard_True;
          continue;
        }

        currd2[i] = getContainerValue(points, k).SquareDistance(ext2);
        if (currd2[i] <= prec * prec && !PtEQext2)
        {
          mp[i]    = 2;
          tp[i]    = tt2;
          PtEQext2 = Standard_True;
          continue;
        }

        // On evite de projecter sur un iso degenere
        // on doit egalement le faire pour l apex du cone
        if (mySurf->Surface()->IsKind(STANDARD_TYPE(Geom_SphericalSurface)) && !isoU)
        {
          continue;
        }

        if (aBox->IsOut(getContainerValue(points, k)))
          continue;

        Standard_Real Cf = cI->FirstParameter();
        Standard_Real Cl = cI->LastParameter();
        if (Precision::IsInfinite(Cf))
          Cf = -1000;
        if (Precision::IsInfinite(Cl))
          Cl = +1000;

        ShapeAnalysis_Curve sac;
        Standard_Real dist = sac.Project(cI, getContainerValue(points, k), prec, pt, t, Cf, Cl);
        currd2[i]          = dist * dist;
        if ((dist <= prec) && (t >= Cf) && (t <= Cl))
        {
          mp[i] = 3;
          tp[i] = t;
        }
      }

      //: e7 abv 21 Apr 98: ProSTEP TR8, r0501_pe #56679:
      // avoid possible null-length curves
      if (mp[0] > 0 && mp[1] > 0 && Abs(tp[0] - tp[1]) < Precision::PConfusion())
        continue;

      if (mp[0] > 0 && (!p1OnIso || currd2[0] < mind2[0]))
      {
        p1OnIso = Standard_True;
        // LP2.stp #105899: FLT_INVALID_OPERATION on Windows 7 VC 9 Release mode on the whole file
        mind2[0] = currd2[0];
        if (isoU)
          valueP1.SetCoord(isoVal, tp[0]);
        else
          valueP1.SetCoord(tp[0], isoVal);
      }

      if (mp[1] > 0 && (!p2OnIso || currd2[1] < mind2[1]))
      {
        p2OnIso  = Standard_True;
        mind2[1] = currd2[1];
        if (isoU)
          valueP2.SetCoord(isoVal, tp[1]);
        else
          valueP2.SetCoord(tp[1], isoVal);
      }

      if (mp[0] <= 0 || mp[1] <= 0)
        continue;

      Standard_Real md2 = currd2[0] + currd2[1];
      if (mindist2 <= md2)
        continue;

      mindist2 = md2;
      mpt[0]   = mp[0];
      mpt[1]   = mp[1];
      tpar[0]  = tp[0];
      tpar[1]  = tp[1];
      isoTypeU = isoU;
      isoValue = isoVal;
      cIso     = cI;
      t1       = tt1;
      t2       = tt2;
    }

    // probablely it concerns an isoparametrics
    if (mpt[0] > 0 && mpt[1] > 0)
    {

      p1OnIso = p2OnIso = Standard_True;
      if (isoTypeU)
      {
        valueP1.SetCoord(isoValue, tpar[0]);
        valueP2.SetCoord(isoValue, tpar[1]);
      }
      else
      {
        valueP1.SetCoord(tpar[0], isoValue);
        valueP2.SetCoord(tpar[1], isoValue);
      }

      if (mpt[0] != 3 && mpt[1] != 3)
      {
        isoPar2d3d = Standard_True;
        for (Standard_Integer i = 2; i < nbrPnt && isoPar2d3d; i++)
        {
          if (tpar[1] > tpar[0])
            t = getContainerValue(params, i);
          else
            t = t1 + t2 - getContainerValue(params, i);
          cIso->D0(t, pt);
          if (!getContainerValue(points, i).IsEqual(pt, prec))
            isoPar2d3d = Standard_False;
        }
      }

      if (isoPar2d3d)
        isoParam = Standard_True;
      else
      {
        Standard_Real    prevParam = tpar[0];
        Standard_Real    Cf, Cl;
        Standard_Boolean isoByDistance = Standard_True;
        Cf                             = cIso->FirstParameter();
        Cl                             = cIso->LastParameter();
        if (Precision::IsInfinite(Cf))
          Cf = -1000;
        if (Precision::IsInfinite(Cl))
          Cl = +1000;

        ShapeAnalysis_Curve sac;
        for (Standard_Integer i = 2; i < nbrPnt && isoByDistance; i++)
        {
          Standard_Real dist =
            sac.NextProject(prevParam,
                            cIso,
                            getContainerValue(points, i),
                            prec,
                            pt,
                            t,
                            Cf,
                            Cl,
                            //: j8 abv 10.12.98: TR10 r0501_db.stp #9423: avoid adjusting to ends
                            Standard_False);
          prevParam = t;
          pout(i)   = t;
          if ((dist > prec) || (t < Cf) || (t > Cl))
            isoByDistance = Standard_False;
        }
        if (isoByDistance)
          isoParam = Standard_True;
      }
    }
    return isoParam;
  } // RAJOUT
  catch (Standard_Failure const& anException)
  {
#ifdef OCCT_DEBUG
    //  pb : on affiche ce qu on peut
    for (Standard_Integer numpnt = 1; numpnt <= nbrPnt; numpnt++)
    {
      std::cout << "[" << numpnt << "]param=" << getContainerValue(params, numpnt) << " point=("
                << getContainerValue(points, numpnt).X() << "  "
                << getContainerValue(points, numpnt).Y() << "  "
                << getContainerValue(points, numpnt).Z() << ")" << std::endl;
    }
    std::cout << "Warning: ShapeConstruct_ProjectCurveOnSurface::IsAnIsoparametric(): Exception: ";
    anException.Print(std::cout);
    std::cout << std::endl;
#endif
    (void)anException;
    return Standard_False;
  }
}
