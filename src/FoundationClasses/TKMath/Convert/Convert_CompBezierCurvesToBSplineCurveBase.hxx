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

#ifndef _Convert_CompBezierCurvesToBSplineCurveBase_HeaderFile
#define _Convert_CompBezierCurvesToBSplineCurveBase_HeaderFile

#include <BSplCLib.hxx>
#include <gp.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>

#include <type_traits>

class gp_Pnt;
class gp_Pnt2d;

//! Template base class for converting a sequence of adjacent
//! non-rational Bezier curves into a BSpline curve.
//! PointType is gp_Pnt or gp_Pnt2d; VecType is gp_Vec or gp_Vec2d.
template <typename PointType, typename VecType>
class Convert_CompBezierCurvesToBSplineCurveBase
{
public:
  //! Constructs a framework for converting a sequence of
  //! adjacent non-rational Bezier curves into a BSpline curve.
  //! @param[in] theAngularTolerance angular tolerance in radians
  //!            for checking tangent parallelism at junction points
  explicit Convert_CompBezierCurvesToBSplineCurveBase(const double theAngularTolerance = 1.0e-4)
      : myDegree(0),
        myAngular(theAngularTolerance)
  {
  }

  //! Adds the Bezier curve defined by the table of poles to
  //! the sequence of adjacent Bezier curves to be converted.
  //! @param[in] thePoles poles of the Bezier curve to add
  void AddCurve(const NCollection_Array1<PointType>& thePoles)
  {
    if (!mySequence.IsEmpty())
    {
      [[maybe_unused]] const PointType& aP1 =
        mySequence.Last().Value(mySequence.Last().Upper());
      [[maybe_unused]] const PointType& aP2 = thePoles(thePoles.Lower());

#ifdef OCCT_DEBUG
      if (!aP1.IsEqual(aP2, Precision::Confusion()))
        std::cout << "Convert_CompBezierCurvesToBSplineCurve::AddCurve" << std::endl;
#endif
    }
    mySequence.Append(thePoles);
  }

  //! Computes all the data needed to build a BSpline curve
  //! equivalent to the adjacent Bezier curve sequence.
  void Perform()
  {
    myCurvePoles.Clear();
    myCurveKnots.Clear();
    myKnotsMults.Clear();
    const int aLowerI  = 1;
    const int anUpperI = mySequence.Length();
    const int aNbrCurv = anUpperI - aLowerI + 1;
    NCollection_Array1<double> aCurveKnVals(1, aNbrCurv);

    myDegree = 0;
    for (int i = 1; i <= mySequence.Length(); i++)
    {
      myDegree = std::max(myDegree, mySequence(i).Length() - 1);
    }

    double    aDet = 0;
    PointType aP1, aP2, aP3;
    const int aMaxDegree = myDegree;
    NCollection_Array1<PointType> aPoints(1, myDegree + 1);

    for (int i = aLowerI; i <= anUpperI; i++)
    {
      // 1- Raise the Bezier curve to the maximum degree.
      const int aDeg = mySequence(i).Length() - 1;
      const int anInc = myDegree - aDeg;
      if (anInc > 0)
      {
        BSplCLib::IncreaseDegree(myDegree,
                                 mySequence(i),
                                 BSplCLib::NoWeights(),
                                 aPoints,
                                 BSplCLib::NoWeights());
      }
      else
      {
        aPoints = mySequence(i);
      }

      // 2- Process the node of junction between 2 Bezier curves.
      if (i == aLowerI)
      {
        // Processing of the initial node of the BSpline.
        for (int j = 1; j <= aMaxDegree; j++)
        {
          myCurvePoles.Append(aPoints(j));
        }
        aCurveKnVals(1) = 1.; // To begin the series.
        myKnotsMults.Append(aMaxDegree + 1);
        aDet = 1.;
      }

      if (i != aLowerI)
      {
        aP2 = aPoints(1);
        aP3 = aPoints(2);
        VecType aV1(aP1, aP2), aV2(aP2, aP3);

        // Processing of the tangency between Bezier and the previous.
        // This allows to guarantee at least a C1 continuity if the tangents are coherent.
        const double aD1 = aV1.SquareMagnitude();
        const double aD2 = aV2.SquareMagnitude();
        if (aMaxDegree > 1 && aD1 > gp::Resolution() && aD2 > gp::Resolution()
            && aV1.IsParallel(aV2, myAngular))
        {
          const double aLambda = std::sqrt(aD2 / aD1);
          if constexpr (std::is_same_v<PointType, gp_Pnt>)
          {
            // 3D-specific epsilon guard to avoid numerical issues
            // when accumulated knot values become too small relative to Det.
            if (aCurveKnVals(i - 1) * aLambda > 10. * Epsilon(aDet))
            {
              myKnotsMults.Append(aMaxDegree - 1);
              aCurveKnVals(i) = aCurveKnVals(i - 1) * aLambda;
            }
            else
            {
              myCurvePoles.Append(aPoints(1));
              myKnotsMults.Append(aMaxDegree);
              aCurveKnVals(i) = 1.0;
            }
          }
          else
          {
            myKnotsMults.Append(aMaxDegree - 1);
            aCurveKnVals(i) = aCurveKnVals(i - 1) * aLambda;
          }
        }
        else
        {
          myCurvePoles.Append(aPoints(1));
          myKnotsMults.Append(aMaxDegree);
          aCurveKnVals(i) = 1.0;
        }
        aDet += aCurveKnVals(i);

        // Store the poles.
        for (int j = 2; j <= aMaxDegree; j++)
        {
          myCurvePoles.Append(aPoints(j));
        }
      }

      if (i == anUpperI)
      {
        // Processing of the end node of the BSpline.
        myCurvePoles.Append(aPoints(aMaxDegree + 1));
        myKnotsMults.Append(aMaxDegree + 1);
      }
      aP1 = aPoints(aMaxDegree);
    }

    // Correct nodal values to make them variable within [0.,1.].
    myCurveKnots.Append(0.0);
    for (int i = 2; i <= aNbrCurv; i++)
    {
      myCurveKnots.Append(myCurveKnots(i - 1) + (aCurveKnVals(i - 1) / aDet));
    }
    myCurveKnots.Append(1.0);
  }

  //! Returns the degree of the BSpline curve.
  [[nodiscard]] int Degree() const { return myDegree; }

  //! Returns the number of poles of the BSpline curve.
  [[nodiscard]] int NbPoles() const { return myCurvePoles.Length(); }

  //! Loads the Poles table with the poles of the BSpline curve.
  //! @param[out] thePoles array to fill with poles
  void Poles(NCollection_Array1<PointType>& thePoles) const
  {
    int k = 1;
    for (int i = thePoles.Lower(); i <= thePoles.Upper(); i++)
    {
      thePoles(i) = myCurvePoles(k++);
    }
  }

  //! Returns the number of knots of the BSpline curve.
  [[nodiscard]] int NbKnots() const { return myCurveKnots.Length(); }

  //! Loads the Knots and Mults tables with the knots
  //! and corresponding multiplicities of the BSpline curve.
  //! @param[out] theKnots array to fill with knots
  //! @param[out] theMults array to fill with multiplicities
  void KnotsAndMults(NCollection_Array1<double>& theKnots,
                     NCollection_Array1<int>&    theMults) const
  {
    int k = 1;
    for (int i = theKnots.Lower(); i <= theKnots.Upper(); i++)
    {
      theKnots(i) = myCurveKnots(k++);
    }
    k = 1;
    for (int i = theMults.Lower(); i <= theMults.Upper(); i++)
    {
      theMults(i) = myKnotsMults(k++);
    }
  }

private:
  NCollection_Sequence<NCollection_Array1<PointType>> mySequence;
  NCollection_Sequence<PointType>                                    myCurvePoles;
  NCollection_Sequence<double>                                       myCurveKnots;
  NCollection_Sequence<int>                                          myKnotsMults;
  int                                                                myDegree;
  double                                                             myAngular;
};

#endif // _Convert_CompBezierCurvesToBSplineCurveBase_HeaderFile
