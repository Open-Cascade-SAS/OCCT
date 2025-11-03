#include <iostream>
#include <Geom_BSplineCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BSplCLib.hxx>
#include <NCollection_DynamicArray.hxx>

// Include our enhanced functions (just for testing)
namespace TestNamespace {

// Function prototypes for testing
void fillPointsAndParams(const Handle(GeomAdaptor_Curve)&               theCurveAdaptor,
                         const NCollection_DynamicArray<Standard_Real>& theFinalParams,
                         NCollection_DynamicArray<gp_Pnt>&              thePoints,
                         NCollection_DynamicArray<Standard_Real>&       theParams);

Standard_Integer generateBSplinePoints(const Handle(Geom_BSplineCurve)&         theBSpline,
                                       const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams);

} // namespace TestNamespace

int main()
{
  try 
  {
    // Create a test B-spline curve with multiple poles
    TColgp_Array1OfPnt poles(1, 5);
    poles(1) = gp_Pnt(0.0, 0.0, 0.0);
    poles(2) = gp_Pnt(1.0, 2.0, 0.5);
    poles(3) = gp_Pnt(3.0, 1.0, 1.0);
    poles(4) = gp_Pnt(4.5, 3.0, 0.5);
    poles(5) = gp_Pnt(6.0, 1.5, 0.0);

    TColStd_Array1OfReal knots(1, 3);
    knots(1) = 0.0;
    knots(2) = 0.5;
    knots(3) = 1.0;

    TColStd_Array1OfInteger mults(1, 3);
    mults(1) = 3; // degree + 1
    mults(2) = 1;
    mults(3) = 3; // degree + 1

    Standard_Integer degree = 2;

    Handle(Geom_BSplineCurve) bspline = new Geom_BSplineCurve(poles, knots, mults, degree);
    Handle(GeomAdaptor_Curve) adaptor = new GeomAdaptor_Curve(bspline);

    // Test our enhanced parameter generation
    NCollection_DynamicArray<gp_Pnt> points;
    NCollection_DynamicArray<Standard_Real> params;
    
    Standard_Integer numPoints = TestNamespace::generateBSplinePoints(
      bspline, adaptor, 0.0, 1.0, 23, points, params);

    std::cout << "Enhanced B-spline parameter generation:" << std::endl;
    std::cout << "Generated " << numPoints << " parameters for curve with " 
              << poles.Length() << " poles" << std::endl;
    std::cout << "Degree: " << degree << ", Knots: " << knots.Length() << std::endl;

    // Show first few parameters to verify intelligent distribution
    std::cout << "Parameter distribution (first 10):" << std::endl;
    for (Standard_Integer i = 0; i < Min(10, params.Length()); ++i)
    {
      std::cout << "  Param[" << i << "] = " << params.Value(i) << std::endl;
    }

    std::cout << "\nTest completed successfully!" << std::endl;
    return 0;
  }
  catch (const std::exception& e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unknown error occurred" << std::endl;
    return 1;
  }
}

// Simplified versions of our enhanced functions for testing
namespace TestNamespace {

void fillPointsAndParams(const Handle(GeomAdaptor_Curve)&               theCurveAdaptor,
                         const NCollection_DynamicArray<Standard_Real>& theFinalParams,
                         NCollection_DynamicArray<gp_Pnt>&              thePoints,
                         NCollection_DynamicArray<Standard_Real>&       theParams)
{
  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(theFinalParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aParam = anIterator.Value();
    gp_Pnt aPoint;
    theCurveAdaptor->D0(aParam, aPoint);
    thePoints.Append(aPoint);
    theParams.Append(aParam);
  }
}

Standard_Integer generateBSplinePoints(const Handle(Geom_BSplineCurve)&         theBSpline,
                                       const Handle(GeomAdaptor_Curve)&         theCurveAdaptor,
                                       const Standard_Real                      theFirst,
                                       const Standard_Real                      theLast,
                                       const Standard_Integer                   theNbControlPoints,
                                       NCollection_DynamicArray<gp_Pnt>&        thePoints,
                                       NCollection_DynamicArray<Standard_Real>& theParams)
{
  const Standard_Integer aDegree    = theBSpline->Degree();
  const Standard_Integer aNbKnots  = theBSpline->NbKnots();
  const Standard_Integer aNbPoles  = theBSpline->NbPoles();
  const Standard_Boolean isPeriodic = theBSpline->IsPeriodic();

  NCollection_DynamicArray<Standard_Real> aCriticalParams;
  aCriticalParams.Append(theFirst);
  aCriticalParams.Append(theLast);

  // Add all distinct knots within parameter range
  for (Standard_Integer i = 1; i <= aNbKnots; ++i)
  {
    const Standard_Real aKnot = theBSpline->Knot(i);
    if (aKnot > theFirst + Precision::PConfusion() && aKnot < theLast - Precision::PConfusion())
    {
      aCriticalParams.Append(aKnot);
    }
  }

  // Generate pole-influenced parameters using BSplCLib wisdom
  TColStd_Array1OfReal    aKnots(1, aNbKnots);
  TColStd_Array1OfInteger aMults(1, aNbKnots);
  
  for (Standard_Integer i = 1; i <= aNbKnots; ++i)
  {
    aKnots(i) = theBSpline->Knot(i);
    aMults(i) = theBSpline->Multiplicity(i);
  }

  const Standard_Integer aKnotSeqLength = BSplCLib::KnotSequenceLength(aMults, aDegree, isPeriodic);
  
  if (aKnotSeqLength > 0)
  {
    TColStd_Array1OfReal aKnotSequence(1, aKnotSeqLength);
    BSplCLib::KnotSequence(aKnots, aMults, aDegree, isPeriodic, aKnotSequence);
    
    // Add parameters based on the flat knot sequence
    for (Standard_Integer i = 1; i <= aKnotSeqLength - aDegree; ++i)
    {
      Standard_Real aPoleParam = 0.0;
      for (Standard_Integer j = 0; j < aDegree; ++j)
      {
        aPoleParam += aKnotSequence(i + j);
      }
      aPoleParam /= aDegree;
      
      if (aPoleParam > theFirst + Precision::PConfusion() && 
          aPoleParam < theLast - Precision::PConfusion())
      {
        aCriticalParams.Append(aPoleParam);
      }
    }
  }

  // Add additional uniform parameters
  Standard_Integer anAdaptiveCount = Max(aDegree + 1, theNbControlPoints);
  if (aNbPoles > 8)
  {
    anAdaptiveCount = Standard_Integer(anAdaptiveCount * (1.0 + aNbPoles / 15.0));
  }
  
  const Standard_Real anInterval = theLast - theFirst;
  const Standard_Integer aNeedPoints = anAdaptiveCount - aCriticalParams.Length();
  if (aNeedPoints > 0)
  {
    for (Standard_Integer i = 1; i <= aNeedPoints; ++i)
    {
      const Standard_Real aParam = theFirst + (anInterval * i) / (aNeedPoints + 1);
      aCriticalParams.Append(aParam);
    }
  }

  // Sort and remove duplicates
  std::sort(aCriticalParams.begin(), aCriticalParams.end());

  NCollection_DynamicArray<Standard_Real> aFinalParams;
  const Standard_Real aMinSpacing = Max(Precision::PConfusion(), anInterval / 1000.0);
  Standard_Real aPrevParam = -Precision::Infinite();

  for (NCollection_DynamicArray<Standard_Real>::Iterator anIterator(aCriticalParams);
       anIterator.More();
       anIterator.Next())
  {
    const Standard_Real aCurrentParam = anIterator.Value();
    if (aCurrentParam - aPrevParam > aMinSpacing)
    {
      aFinalParams.Append(aCurrentParam);
      aPrevParam = aCurrentParam;
    }
  }

  fillPointsAndParams(theCurveAdaptor, aFinalParams, thePoints, theParams);
  return aFinalParams.Length();
}

} // namespace TestNamespace
