//--------------------------------------------------------------------
//
//  File Name : IGESGeom_BSplineCurve.cxx
//  Date      :
//  Author    : CKY / Contract Toubro-Larsen
//  Copyright : MATRA-DATAVISION 1993
//
//--------------------------------------------------------------------

#include <IGESGeom_BSplineCurve.ixx>
#include <Standard_OutOfRange.hxx>
#include <gp_GTrsf.hxx>


    IGESGeom_BSplineCurve::IGESGeom_BSplineCurve ()    {  }


    void IGESGeom_BSplineCurve::Init
  (const Standard_Integer anIndex,
   const Standard_Integer aDegree, const Standard_Boolean aPlanar,
   const Standard_Boolean aClosed, const Standard_Boolean aPolynom,
   const Standard_Boolean aPeriodic, 
   const Handle(TColStd_HArray1OfReal)& allKnots,
   const Handle(TColStd_HArray1OfReal)& allWeights,
   const Handle(TColgp_HArray1OfXYZ)& allPoles,
   const Standard_Real aUmin, const Standard_Real aUmax,
   const gp_XYZ& aNorm)
{
  if (!allPoles.IsNull()) {
    if (allPoles->Length() != allWeights->Length())
      Standard_DimensionMismatch::Raise("IGESGeom_BSplineCurve : Init");
    if (allKnots->Lower()   != -aDegree || allKnots->Upper()   != anIndex+1 ||
	allWeights->Upper() !=  anIndex ||
	allWeights->Lower() != 0        || allPoles->Lower() != 0)
      Standard_DimensionMismatch::Raise("IGESGeom_BSplineCurve : Init");
  }

  theIndex     = anIndex;
  theDegree    = aDegree;
  isPlanar     = aPlanar;
  isClosed     = aClosed;
  isPolynomial = aPolynom;
  isPeriodic   = aPeriodic;
  theKnots     = allKnots; 
  theWeights   = allWeights;
  thePoles     = allPoles;
  theUmin      = aUmin;
  theUmax      = aUmax;
  theNorm      = aNorm;
  InitTypeAndForm(126,FormNumber());
// FormNumber  precises the shape  0-5
}

    void  IGESGeom_BSplineCurve::SetFormNumber (const Standard_Integer form)
{
  if (form < 0 || form > 5) Standard_OutOfRange::Raise
    ("IGESGeom_BSplineCurve : SetFormNumber");
  InitTypeAndForm(126,form);
}

    Standard_Integer IGESGeom_BSplineCurve::UpperIndex () const
{
  return theIndex;
}

    Standard_Integer IGESGeom_BSplineCurve::Degree () const
{
  return theDegree;
}

    Standard_Boolean IGESGeom_BSplineCurve::IsPlanar () const
{
  return isPlanar;
}

    Standard_Boolean IGESGeom_BSplineCurve::IsClosed () const
{
  return isClosed;
}

    Standard_Boolean IGESGeom_BSplineCurve::IsPolynomial
  (const Standard_Boolean flag) const
{
  if (flag || theWeights.IsNull()) return isPolynomial;
  Standard_Integer i, i1 = theWeights->Lower(), i2 = theWeights->Upper();
  Standard_Real w0 = theWeights->Value(i1);
  for (i = i1+1; i <= i2; i ++)
    if (Abs (theWeights->Value(i) - w0) > 1.e-10) return Standard_False;
  return Standard_True;
}

    Standard_Boolean IGESGeom_BSplineCurve::IsPeriodic () const
{
  return isPeriodic;
}

    Standard_Integer IGESGeom_BSplineCurve::NbKnots () const
{
  return (theKnots.IsNull() ? 0 : theKnots->Length());
}

    Standard_Real IGESGeom_BSplineCurve::Knot
  (const Standard_Integer anIndex) const
{
  return theKnots->Value(anIndex);
}

    Standard_Integer IGESGeom_BSplineCurve::NbPoles () const
{
  return (thePoles.IsNull() ? 0 : thePoles->Length());
}

    Standard_Real IGESGeom_BSplineCurve::Weight
  (const Standard_Integer anIndex) const
{
  return theWeights->Value(anIndex);
}

    gp_Pnt IGESGeom_BSplineCurve::Pole (const Standard_Integer anIndex) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex);
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

    gp_Pnt IGESGeom_BSplineCurve::TransformedPole
  (const Standard_Integer anIndex) const
{
  gp_XYZ tempXYZ = thePoles->Value(anIndex);
  if (HasTransf()) Location().Transforms(tempXYZ);
  gp_Pnt Pole(tempXYZ);
  return Pole;
}

    Standard_Real IGESGeom_BSplineCurve::UMin () const
{
  return theUmin;
}

    Standard_Real IGESGeom_BSplineCurve::UMax () const
{
  return theUmax;
}

    gp_XYZ IGESGeom_BSplineCurve::Normal () const
{
  return theNorm;
}
