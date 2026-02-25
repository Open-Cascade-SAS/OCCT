// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <BRepProp_Curve.hxx>

#include <gp_Dir.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Edge.hxx>

//==================================================================================================

BRepProp_Curve::BRepProp_Curve(const TopoDS_Edge& theEdge)
{
  initialization(theEdge);
}

//==================================================================================================

BRepProp_Curve::BRepProp_Curve(const BRepAdaptor_Curve& theCurve)
{
  initialization(theCurve);
}

//==================================================================================================

BRepProp_Curve::BRepProp_Curve(const occ::handle<BRepAdaptor_Curve>& theCurve)
{
  initialization(theCurve);
}

//==================================================================================================

void BRepProp_Curve::initialization(const TopoDS_Edge& theEdge)
{
  if (theEdge.IsNull())
  {
    myOwned.Nullify();
    myPtr = nullptr;
    return;
  }
  myOwned = new BRepAdaptor_Curve(theEdge);
  myPtr   = myOwned.get();
}

//==================================================================================================

void BRepProp_Curve::initialization(const BRepAdaptor_Curve& theCurve)
{
  myOwned.Nullify();
  myPtr = &theCurve;
}

//==================================================================================================

void BRepProp_Curve::initialization(const occ::handle<BRepAdaptor_Curve>& theCurve)
{
  myOwned = theCurve;
  myPtr   = myOwned.get();
}

//==================================================================================================

GeomProp::TangentResult BRepProp_Curve::Tangent(const double theParam, const double theTol) const
{
  gp_Pnt aPnt;
  gp_Vec aD1, aD2, aD3;
  myPtr->D3(theParam, aPnt, aD1, aD2, aD3);
  return GeomProp::ComputeTangent(aD1, aD2, aD3, theTol);
}

//==================================================================================================

GeomProp::CurvatureResult BRepProp_Curve::Curvature(const double theParam,
                                                    const double theTol) const
{
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myPtr->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCurvature(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::NormalResult BRepProp_Curve::Normal(const double theParam, const double theTol) const
{
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myPtr->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeNormal(aD1, aD2, theTol);
}

//==================================================================================================

GeomProp::CentreResult BRepProp_Curve::CentreOfCurvature(const double theParam,
                                                         const double theTol) const
{
  gp_Pnt aPnt;
  gp_Vec aD1, aD2;
  myPtr->D2(theParam, aPnt, aD1, aD2);
  return GeomProp::ComputeCentreOfCurvature(aPnt, aD1, aD2, theTol);
}

//==================================================================================================

GeomAbs_Shape BRepProp_Curve::Continuity(const BRepAdaptor_Curve& theCurve1,
                                         const BRepAdaptor_Curve& theCurve2,
                                         const double             theU1,
                                         const double             theU2,
                                         const double             theTolLinear,
                                         const double             theTolAngular)
{
  GeomAbs_Shape aCont     = GeomAbs_C0;
  bool          isChecked = false;

  // Determine maximum derivative order for each curve.
  const GeomAbs_Shape aCont1 = theCurve1.Continuity();
  const GeomAbs_Shape aCont2 = theCurve2.Continuity();
  int                 aN1    = 0;
  int                 aN2    = 0;
  if (aCont1 >= 5)
    aN1 = 3;
  else if (aCont1 == 4)
    aN1 = 2;
  else if (aCont1 == 2)
    aN1 = 1;
  if (aCont2 >= 5)
    aN2 = 3;
  else if (aCont2 == 4)
    aN2 = 2;
  else if (aCont2 == 2)
    aN2 = 1;

  // Evaluate properties at junction points.
  BRepProp_Curve aProp1(theCurve1);
  BRepProp_Curve aProp2(theCurve2);

  // Check point coincidence.
  gp_Pnt aPnt1, aPnt2;
  gp_Vec aD1_1, aD1_2;
  theCurve1.D1(theU1, aPnt1, aD1_1);
  theCurve2.D1(theU2, aPnt2, aD1_2);
  if (!aPnt1.IsEqual(aPnt2, theTolLinear))
  {
    throw Standard_Failure("Curves not connected");
  }

  const int aMinOrder = std::min(aN1, aN2);
  if (aMinOrder >= 1)
  {
    // Account for edge orientation.
    gp_Vec aVec1 = aD1_1;
    gp_Vec aVec2 = aD1_2;
    if (theCurve1.Edge().Orientation() == TopAbs_REVERSED)
      aVec1.Reverse();
    if (theCurve2.Edge().Orientation() == TopAbs_REVERSED)
      aVec2.Reverse();

    if (aVec1.IsEqual(aVec2, theTolLinear, theTolAngular))
    {
      aCont = GeomAbs_C1;
    }
    else
    {
      // Check G1 continuity via tangent directions.
      const GeomProp::TangentResult aTan1 = aProp1.Tangent(theU1, theTolLinear);
      const GeomProp::TangentResult aTan2 = aProp2.Tangent(theU2, theTolLinear);
      if (aTan1.IsDefined && aTan2.IsDefined)
      {
        gp_Dir aDir1 = aTan1.Direction;
        gp_Dir aDir2 = aTan2.Direction;
        if (theCurve1.Edge().Orientation() == TopAbs_REVERSED)
          aDir1.Reverse();
        if (theCurve2.Edge().Orientation() == TopAbs_REVERSED)
          aDir2.Reverse();
        if (aDir1.IsEqual(aDir2, theTolAngular))
        {
          aCont = GeomAbs_G1;
        }
        isChecked = true;
      }
      else
      {
        isChecked = true;
      }
    }
  }

  if (aMinOrder >= 2 && !isChecked)
  {
    // Compare second derivatives.
    gp_Pnt aDummy;
    gp_Vec aDummy1, aDummy2;
    gp_Vec aD2_1, aD2_2;
    theCurve1.D2(theU1, aDummy, aDummy1, aD2_1);
    theCurve2.D2(theU2, aDummy, aDummy2, aD2_2);
    if (aD2_1.IsEqual(aD2_2, theTolLinear, theTolAngular))
    {
      aCont = GeomAbs_C2;
    }
  }

  // Same periodic edge implies CN.
  const TopoDS_Edge& anEdge1 = theCurve1.Edge();
  const TopoDS_Edge& anEdge2 = theCurve2.Edge();
  if (anEdge1.IsSame(anEdge2) && theCurve1.IsPeriodic() && aCont >= GeomAbs_G1)
    aCont = GeomAbs_CN;

  return aCont;
}

//==================================================================================================

GeomAbs_Shape BRepProp_Curve::Continuity(const BRepAdaptor_Curve& theCurve1,
                                         const BRepAdaptor_Curve& theCurve2,
                                         const double             theU1,
                                         const double             theU2)
{
  return Continuity(theCurve1,
                    theCurve2,
                    theU1,
                    theU2,
                    Precision::Confusion(),
                    Precision::Angular());
}
