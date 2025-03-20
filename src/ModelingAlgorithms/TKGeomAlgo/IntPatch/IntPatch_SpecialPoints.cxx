//! Created on: 2016-06-03
//! Created by: NIKOLAI BUKHALOV
//! Copyright (c) 2016 OPEN CASCADE SAS
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

#include <IntPatch_SpecialPoints.hxx>

#include <Adaptor3d_Surface.hxx>
#include <ElCLib.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <IntPatch_Point.hxx>
#include <IntSurf.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_TypeMismatch.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>

// The function for searching intersection point, which
// lies in the seam-edge of the quadric definitely.
class FuncPreciseSeam : public math_FunctionSetWithDerivatives
{
public:
  FuncPreciseSeam(const Handle(Adaptor3d_Surface)& theQSurf, // quadric
                  const Handle(Adaptor3d_Surface)& thePSurf, // another surface
                  const Standard_Boolean           isTheUSeam,
                  const Standard_Real              theIsoParameter)
      : myQSurf(theQSurf),
        myPSurf(thePSurf),
        mySeamCoordInd(isTheUSeam ? 1 : 0), // Defines, U- or V-seam is used
        myIsoParameter(theIsoParameter) {};

  virtual Standard_Integer NbVariables() const { return 3; };

  virtual Standard_Integer NbEquations() const { return 3; }

  virtual Standard_Boolean Value(const math_Vector& theX, math_Vector& theF)
  {
    try
    {
      const Standard_Integer anIndX = theX.Lower(), anIndF = theF.Lower();
      Standard_Real          aUV[] = {myIsoParameter, myIsoParameter};
      aUV[mySeamCoordInd]          = theX(anIndX + 2);
      const gp_Pnt aP1(myPSurf->Value(theX(anIndX), theX(anIndX + 1)));
      const gp_Pnt aP2(myQSurf->Value(aUV[0], aUV[1]));

      (aP1.XYZ() - aP2.XYZ()).Coord(theF(anIndF), theF(anIndF + 1), theF(anIndF + 2));
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }

    return Standard_True;
  };

  virtual Standard_Boolean Derivatives(const math_Vector& theX, math_Matrix& theD)
  {
    try
    {
      const Standard_Integer anIndX = theX.Lower(), anIndRD = theD.LowerRow(),
                             anIndCD = theD.LowerCol();
      Standard_Real aUV[]            = {myIsoParameter, myIsoParameter};
      aUV[mySeamCoordInd]            = theX(anIndX + 2);

      gp_Pnt aPt;

      // 0 for U-coordinate, 1 - for V one
      gp_Vec aD1[2], aD2[2];
      myPSurf->D1(theX(anIndX), theX(anIndX + 1), aPt, aD1[0], aD1[1]);
      myQSurf->D1(aUV[0], aUV[1], aPt, aD2[0], aD2[1]);

      // d/dX1
      aD1[0].Coord(theD(anIndRD, anIndCD), theD(anIndRD + 1, anIndCD), theD(anIndRD + 2, anIndCD));

      // d/dX2
      aD1[1].Coord(theD(anIndRD, anIndCD + 1),
                   theD(anIndRD + 1, anIndCD + 1),
                   theD(anIndRD + 2, anIndCD + 1));

      // d/dX3
      aD2[mySeamCoordInd].Reversed().Coord(theD(anIndRD, anIndCD + 2),
                                           theD(anIndRD + 1, anIndCD + 2),
                                           theD(anIndRD + 2, anIndCD + 2));
    }
    catch (Standard_Failure const&)
    {
      return Standard_False;
    }

    return Standard_True;
  };

  virtual Standard_Boolean Values(const math_Vector& theX, math_Vector& theF, math_Matrix& theD)
  {
    if (!Value(theX, theF))
      return Standard_False;

    if (!Derivatives(theX, theD))
      return Standard_False;

    return Standard_True;
  }

protected:
  FuncPreciseSeam operator=(FuncPreciseSeam&);

private:
  const Handle(Adaptor3d_Surface)& myQSurf;
  const Handle(Adaptor3d_Surface)& myPSurf;

  //! 1 for U-coordinate, 0 - for V one.
  const Standard_Integer mySeamCoordInd;

  //! Constant parameter of iso-line
  const Standard_Real myIsoParameter;
};

//=======================================================================
// function : GetTangent
// purpose  : Computes tangent having the given parameter.
//            See calling method(s) for detailed information
//=======================================================================
static inline void GetTangent(const Standard_Real theConeSemiAngle,
                              const Standard_Real theParameter,
                              gp_XYZ&             theResult)
{
  const Standard_Real aW2    = theParameter * theParameter;
  const Standard_Real aCosUn = (1.0 - aW2) / (1.0 + aW2);
  const Standard_Real aSinUn = 2.0 * theParameter / (1.0 + aW2);

  const Standard_Real aTanA = Tan(theConeSemiAngle);
  theResult.SetCoord(aTanA * aCosUn, aTanA * aSinUn, 1.0);
}

//=======================================================================
// function : IsPointOnSurface
// purpose  : Checks if thePt is in theSurf (with given tolerance).
//            Returns the foot of projection (theProjPt) and its parameters
//           on theSurf.
//=======================================================================
static Standard_Boolean IsPointOnSurface(const Handle(Adaptor3d_Surface)& theSurf,
                                         const gp_Pnt&                    thePt,
                                         const Standard_Real              theTol,
                                         gp_Pnt&                          theProjPt,
                                         Standard_Real&                   theUpar,
                                         Standard_Real&                   theVpar)
{
  Standard_Boolean aRetVal = Standard_False;

  switch (theSurf->GetType())
  {
    case GeomAbs_Plane:
    case GeomAbs_Cylinder:
    case GeomAbs_Cone:
    case GeomAbs_Sphere:
    case GeomAbs_Torus:
    case GeomAbs_SurfaceOfExtrusion:
    case GeomAbs_SurfaceOfRevolution: {
      Extrema_ExtPS anExtr(thePt,
                           *theSurf,
                           theSurf->UResolution(theTol),
                           theSurf->VResolution(theTol),
                           Extrema_ExtFlag_MIN);
      if (!anExtr.IsDone() || (anExtr.NbExt() < 1))
      {
        aRetVal = Standard_False;
      }
      else
      {
        Standard_Integer anExtrIndex = 1;
        Standard_Real    aSqDistMin  = anExtr.SquareDistance(anExtrIndex);
        for (Standard_Integer i = anExtrIndex + 1; i <= anExtr.NbExt(); i++)
        {
          const Standard_Real aSqD = anExtr.SquareDistance(i);
          if (aSqD < aSqDistMin)
          {
            aSqDistMin  = aSqD;
            anExtrIndex = i;
          }
        }

        if (aSqDistMin > theTol * theTol)
        {
          aRetVal = Standard_False;
        }
        else
        {
          theProjPt.SetXYZ(anExtr.Point(anExtrIndex).Value().XYZ());
          anExtr.Point(anExtrIndex).Parameter(theUpar, theVpar);
          aRetVal = Standard_True;
        }
      }
    }
    break;
    default: {
      Extrema_GenLocateExtPS anExtr(*theSurf);
      anExtr.Perform(thePt, theUpar, theVpar);
      if (!anExtr.IsDone() || (anExtr.SquareDistance() > theTol * theTol))
      {
        aRetVal = Standard_False;
      }
      else
      {
        anExtr.Point().Parameter(theUpar, theVpar);
        theProjPt.SetXYZ(anExtr.Point().Value().XYZ());
        aRetVal = Standard_True;
      }
    }
    break;
  }

  return aRetVal;
}

//=======================================================================
// function : AddCrossUVIsoPoint
// purpose  : theQSurf is the surface possibly containing special point,
//            thePSurf is another surface to intersect.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::AddCrossUVIsoPoint(
  const Handle(Adaptor3d_Surface)& theQSurf,
  const Handle(Adaptor3d_Surface)& thePSurf,
  const IntSurf_PntOn2S&           theRefPt,
  const Standard_Real              theTol,
  IntSurf_PntOn2S&                 theAddedPoint,
  const Standard_Boolean           theIsReversed)
{
  Standard_Real anArrOfPeriod[4] = {0.0, 0.0, 0.0, 0.0};
  IntSurf::SetPeriod(theIsReversed ? thePSurf : theQSurf,
                     theIsReversed ? theQSurf : thePSurf,
                     anArrOfPeriod);

  gp_Pnt aPQuad;

  // Not quadric point
  Standard_Real aU0 = 0.0, aV0 = 0.0;
  if (theIsReversed)
    theRefPt.ParametersOnS1(aU0, aV0);
  else
    theRefPt.ParametersOnS2(aU0, aV0);

  // Quadric point
  Standard_Real aUquad = 0.0, aVquad = 0.0;

  theQSurf->D0(aUquad, aVquad, aPQuad);

  Extrema_GenLocateExtPS anExtr(*thePSurf);
  anExtr.Perform(aPQuad, aU0, aV0);

  if (!anExtr.IsDone())
  {
    return Standard_False;
  }

  if (anExtr.SquareDistance() > theTol * theTol)
  {
    return Standard_False;
  }

  anExtr.Point().Parameter(aU0, aV0);
  gp_Pnt aP0(anExtr.Point().Value());

  if (theIsReversed)
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theAddedPoint);

  return Standard_True;
}

//=======================================================================
// function : AddPointOnUorVIso
// purpose  : theQSurf is the surface possibly containing special point,
//            thePSurf is another surface to intersect.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::AddPointOnUorVIso(
  const Handle(Adaptor3d_Surface)& theQSurf,
  const Handle(Adaptor3d_Surface)& thePSurf,
  const IntSurf_PntOn2S&           theRefPt,
  const Standard_Boolean           theIsU,
  const Standard_Real              theIsoParameter,
  const math_Vector&               theToler,
  const math_Vector&               theInitPoint,
  const math_Vector&               theInfBound,
  const math_Vector&               theSupBound,
  IntSurf_PntOn2S&                 theAddedPoint,
  const Standard_Boolean           theIsReversed)
{
  Standard_Real anArrOfPeriod[4] = {0.0, 0.0, 0.0, 0.0};
  IntSurf::SetPeriod(theIsReversed ? thePSurf : theQSurf,
                     theIsReversed ? theQSurf : thePSurf,
                     anArrOfPeriod);

  FuncPreciseSeam aF(theQSurf, thePSurf, theIsU, theIsoParameter);

  math_FunctionSetRoot aSRF(aF, theToler);
  aSRF.Perform(aF, theInitPoint, theInfBound, theSupBound);

  if (!aSRF.IsDone())
  {
    return Standard_False;
  }

  math_Vector aRoots(theInitPoint.Lower(), theInitPoint.Upper());
  aSRF.Root(aRoots);

  // On parametric
  Standard_Real aU0 = aRoots(1), aV0 = aRoots(2);

  // On quadric
  Standard_Real aUquad = theIsU ? 0.0 : aRoots(3);
  Standard_Real aVquad = theIsU ? aRoots(3) : 0.0;
  const gp_Pnt  aPQuad(theQSurf->Value(aUquad, aVquad));
  const gp_Pnt  aP0(thePSurf->Value(aU0, aV0));

  if (theIsReversed)
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theAddedPoint);
  return Standard_True;
}

//=======================================================================
// function : ProcessSphere
// purpose  :
/*
The intersection point (including the pole)
must be satisfied to the following system:

    \left\{\begin{matrix}
    R*\cos (U_{q})*\cos (V_{q})=S_{x}(U_{s},V_{s})
    R*\sin (U_{q})*\cos (V_{q})=S_{y}(U_{s},V_{s})
    R*\sin (V_{q})=S_{z}(U_{s},V_{s})
    \end{matrix}\right,
where
  R is the radius of the sphere;
  @S_{x}@, @S_{y}@ and @S_{z}@ are X, Y and Z-coordinates of thePSurf;
  @U_{s}@ and @V_{s}@ are parameters on the parametric surface;
  @U_{q}@ and @V_{q}@ are equal to theUquad and theVquad correspondingly.

Consequently (from first two equations),
  \left\{\begin{matrix}
  \cos (U_{q}) = \frac{S_{x}(U_{s},V_{s})}{R*\cos (V_{q})}
  \sin (U_{q}) = \frac{S_{y}(U_{s},V_{s})}{R*\cos (V_{q})}
  \end{matrix}\right.

For pole,
  V_{q}=\pm \pi /2 \Rightarrow \cos (V_{q}) = 0 (denominator is equal to 0).

Therefore, computation U_{q} directly is impossibly.

Let @V_{q}@ tends to @\pm \pi /2@.
Then (indeterminate form is evaluated in accordance of L'Hospital rule),
  \cos (U_{q}) = \lim_{V_{q} \to (\pi /2-0)}
  \frac{S_{x}(U_{s},V_{s})}{R*\cos (V_{q})}=
  -\lim_{V_{q} \to (\pi /2-0)}
  \frac{\frac{\partial S_{x}}
  {\partial U_{s}}*\frac{\mathrm{d} U_{s}}
  {\mathrm{d} V_{q}}+\frac{\partial S_{x}}
  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}
  {\mathrm{d} V_{q}}}{R*\sin (V_{q})} =
  -\frac{1}{R}*\frac{\mathrm{d} U_{s}}
  {\mathrm{d} V_{q}}*(\frac{\partial S_{x}}
  {\partial U_{s}}+\frac{\partial S_{x}}
  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}
  {\mathrm{d} U_{s}}) =
  -\frac{1}{R}*\frac{\mathrm{d} V_{s}}
  {\mathrm{d} V_{q}}*(\frac{\partial S_{x}}
  {\partial U_{s}}*\frac{\mathrm{d} U_{s}}
  {\mathrm{d} V_{s}}+\frac{\partial S_{x}}
  {\partial V_{s}}).

Analogically for @\sin (U_{q})@ (@S_{x}@ is substituted to @S_{y}@).

Let mean, that
  \cos (U_{q}) \left | _{V_{q} \to (-\pi /2+0)} = \cos (U_{q}) \left | _{V_{q} \to (\pi /2-0)}
  \sin (U_{q}) \left | _{V_{q} \to (-\pi /2+0)} = \sin (U_{q}) \left | _{V_{q} \to (\pi /2-0)}

From the 3rd equation of the system, we obtain
  \frac{\mathrm{d} (R*\sin (V_{q}))}{\mathrm{d} V_{q}} =
  \frac{\mathrm{d} S_{z}(U_{s},V_{s})}{\mathrm{d} V_{q}}
or
  R*\cos (V_{q}) = \frac{\partial S_{z}}{\partial U_{s}}*
  \frac{\mathrm{d} U_{s}} {\mathrm{d} V_{q}}+\frac{\partial S_{z}}
  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}{\mathrm{d} V_{q}}.

If @V_{q}=\pm \pi /2@, then
  \frac{\partial S_{z}}{\partial U_{s}}*
  \frac{\mathrm{d} U_{s}} {\mathrm{d} V_{q}}+\frac{\partial S_{z}}
  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}{\mathrm{d} V_{q}} = 0.

Consequently, if @\frac{\partial S_{z}}{\partial U_{s}} \neq 0 @ then
  \frac{\mathrm{d} U_{s}}{\mathrm{d} V_{s}} =
  -\frac{\frac{\partial S_{z}}{\partial V_{s}}}
  {\frac{\partial S_{z}}{\partial U_{s}}}.

If @ \frac{\partial S_{z}}{\partial V_{s}} \neq 0 @ then
  \frac{\mathrm{d} V_{s}}{\mathrm{d} U_{s}} =
  -\frac{\frac{\partial S_{z}}{\partial U_{s}}}
  {\frac{\partial S_{z}}{\partial V_{s}}}

Cases, when @ \frac{\partial S_{z}}{\partial U_{s}} =
\frac{\partial S_{z}}{\partial V_{s}} = 0 @ are not consider here.
The reason is written below.
*/
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::ProcessSphere(const IntSurf_PntOn2S& thePtIso,
                                                       const gp_Vec&          theDUofPSurf,
                                                       const gp_Vec&          theDVofPSurf,
                                                       const Standard_Boolean theIsReversed,
                                                       const Standard_Real    theVquad,
                                                       Standard_Real&         theUquad,
                                                       Standard_Boolean&      theIsIsoChoosen)
{
  theIsIsoChoosen = Standard_False;

  // Vector with {@ \cos (U_{q}) @, @ \sin (U_{q}) @} coordinates.
  // Ask to pay attention to the fact that this vector is always normalized.
  gp_Vec2d aV1;

  if ((Abs(theDUofPSurf.Z()) < Precision::PConfusion())
      && (Abs(theDVofPSurf.Z()) < Precision::PConfusion()))
  {
    // Example of this case is an intersection of a plane with a sphere
    // when the plane tangents the sphere in some pole (i.e. only one
    // intersection point, not line). In this case, U-coordinate of the
    // sphere is undefined (can be really anything).
    // Another reason is that we have tangent zone around the pole
    //(see bug #26576).
    // Computation of correct value of theUquad is impossible.
    // Therefore, (in order to return something) we will consider
    // the intersection line goes along some isoline in neighborhood
    // of the pole.

#ifdef INTPATCH_ADDSPECIALPOINTS_DEBUG
    std::cout << "Cannot find UV-coordinate for quadric in the pole."
                 " See considered comment above. IntPatch_SpecialPoints.cxx,"
                 " ProcessSphere(...)"
              << std::endl;
#endif
    Standard_Real aUIso = 0.0, aVIso = 0.0;
    if (theIsReversed)
      thePtIso.ParametersOnS2(aUIso, aVIso);
    else
      thePtIso.ParametersOnS1(aUIso, aVIso);

    theUquad        = aUIso;
    theIsIsoChoosen = Standard_True;
  }
  else
  {
    if (Abs(theDUofPSurf.Z()) > Abs(theDVofPSurf.Z()))
    {
      const Standard_Real aDusDvs = theDVofPSurf.Z() / theDUofPSurf.Z();
      aV1.SetCoord(theDUofPSurf.X() * aDusDvs - theDVofPSurf.X(),
                   theDUofPSurf.Y() * aDusDvs - theDVofPSurf.Y());
    }
    else
    {
      const Standard_Real aDvsDus = theDUofPSurf.Z() / theDVofPSurf.Z();
      aV1.SetCoord(theDVofPSurf.X() * aDvsDus - theDUofPSurf.X(),
                   theDVofPSurf.Y() * aDvsDus - theDUofPSurf.Y());
    }

    aV1.Normalize();

    if (Abs(aV1.X()) > Abs(aV1.Y()))
      theUquad = Sign(asin(aV1.Y()), theVquad);
    else
      theUquad = Sign(acos(aV1.X()), theVquad);
  }

  return Standard_True;
}

//=======================================================================
// function : ProcessCone
// purpose  :
/*
The intersection point (including the pole)
must be satisfied to the following system:

  \left\{\begin {matrix}
  (V_{q}\sin(a) + R)*\cos(U_{q})) = S_{x}(U_{s}, V_{s})\\
  (V_{q}\sin(a) + R)*\sin(U_{q})) = S_{y}(U_{s}, V_{s})\\
  V_{q}\cos(a) = S_{z}(U_{s}, V_{s})
  \end {matrix}\right,
where
  R is the radius of the cone;
  a is its semi-angle;
  @S_{x}@, @S_{y}@ and @S_{z}@ are X, Y and Z-coordinates of thePSurf;
  @U_{s}@ and @V_{s}@ are parameters on the parametric surface;
  @U_{q}@ and @V_{q}@ are equal to theUquad and theVquad correspondingly.

Consequently (from first two equations),
  \left\{\begin{matrix}
  \cos(U_{q})=\frac{S_{x}(U_{s},V_{s})}{(V_{q}\sin(a)+R)}\\
  \sin(U_{q})=\frac{S_{y}(U_{s}, V_{s})}{(V_{q}\sin(a)+R)}
  \end{matrix}\right.

For pole, the denominator of these two equations is equal to 0.
Therefore, computation U_{q} directly is impossibly.

Let @V_{q}@ tends to @\frac{-R}{\sin(a)})@.
Then (indeterminate form is evaluated in accordance of L'Hospital rule),

  \cos (U_{q}) =
  \lim_{V_{q} \to (\frac{-R}{\sin(a)})}\frac{S_{x}(U_{s},V_{s})}{(V_{q}\sin(a)+R)}=
  \frac{1}{\sin(a)}* \lim_{V_{q} \to (\frac{-R}{\sin(a)})}\frac{dU_{s}}{dV_{q}}*
  (\frac{\partial S_{x}}{\partial U_{s}}+\frac{\partial S_{x}}{\partial V_{s}}*
  \frac{dV_{s}}{dU_{s}})=
  \frac{1}{\sin(a)}* \lim_{V_{q} \to (\frac{-R}{\sin(a)})}\frac{dV_{s}}{dV_{q}}*
  (\frac{\partial S_{x}}{\partial U_{s}}*
  \frac{dU_{s}}{dV_{s}}+\frac{\partial S_{x}}{\partial V_{s}})

Analogically for @\sin (U_{q})@ (@S_{x}@ is substituted to @S_{y}@).

After differentiating 3rd equation of the system, we will obtain
  \cos(a)=\frac{dS_{z}}{dV_{q}}=\frac{dU_{s}}{dV_{q}}*
  (\frac{\partial S_{z}}{\partial U_{s}}+\frac{\partial S_{z}}{\partial V_{s}}*
  \frac{dV_{s}}{dU_{s}})
or
  \frac{dU_{s}}{dV_{q}}=\frac{\cos(a)}{\frac{\partial S_{z}}{\partial U_{s}}+
  \frac{\partial S_{z}}{\partial V_{s}}*\frac{dV_{s}}{dU_{s}}}

After substituting we will obtain
  \cos (U_{q}) =
  \cot(a)*\frac{\frac{\partial S_{x}}{\partial U_{s}}+\frac{\partial S_{x}}
  {\partial V_{s}}*\frac{dV_{s}}{dU_{s}}}{\frac{\partial S_{z}}
  {\partial U_{s}}+\frac{\partial S_{z}}{\partial V_{s}}*\frac{dV_{s}}{dU_{s}}}

  \sin (U_{q}) =
  \cot(a)*\frac{\frac{\partial S_{y}}{\partial U_{s}}+\frac{\partial S_{y}}
  {\partial V_{s}}*\frac{dV_{s}}{dU_{s}}}{\frac{\partial S_{z}}
  {\partial U_{s}}+\frac{\partial S_{z}}{\partial V_{s}}*\frac{dV_{s}}{dU_{s}}}

So, we have obtained vector with coordinates {@ \cos (U_{q}) @, @ \sin (U_{q}) @}.
Ask to pay attention to the fact that this vector is always normalized.
And after normalization this vector will have coordinates
  {\cos (U_{q}), \sin (U_{q})} = {dS_{x}, dS_{y}}.Normalized().

It means that we have to compute a tangent to the intersection curve in
the cone apex point. After that, just take its X- and Y-coordinates.

However, we have to compute derivative @\frac{dV_{s}}{dU_{s}}@ in order
to compute this vector. In order to find this derivative, we use the
information about direction of tangent to the intersection curve.
This tangent will be directed along the cone generatrix obtained by intersection
of the cone with a plane tangent to 2nd (intersected) surface.
*/
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::ProcessCone(const IntSurf_PntOn2S& thePtIso,
                                                     const gp_Vec&          theDUofPSurf,
                                                     const gp_Vec&          theDVofPSurf,
                                                     const gp_Cone&         theCone,
                                                     const Standard_Boolean theIsReversed,
                                                     Standard_Real&         theUquad,
                                                     Standard_Boolean&      theIsIsoChoosen)
{
  theIsIsoChoosen = Standard_False;

  // A plane tangent to 2nd (intersected) surface.
  // Its normal.
  const gp_XYZ        aTgPlaneZ(theDUofPSurf.Crossed(theDVofPSurf).XYZ());
  const Standard_Real aSqModTg = aTgPlaneZ.SquareModulus();
  if (aSqModTg < Precision::SquareConfusion())
  {
    theIsIsoChoosen = Standard_True;
  }

  gp_XYZ                 aTgILine[2];
  const Standard_Integer aNbTangent =
    !theIsIsoChoosen
      ? GetTangentToIntLineForCone(theCone.SemiAngle(), aTgPlaneZ.Divided(Sqrt(aSqModTg)), aTgILine)
      : 0;

  if (aNbTangent == 0)
  {
    theIsIsoChoosen = Standard_True;
  }
  else
  {
    const Standard_Real aPeriod = M_PI + M_PI;
    Standard_Real       aUIso = 0.0, aVIso = 0.0;
    if (theIsReversed)
      thePtIso.ParametersOnS2(aUIso, aVIso);
    else
      thePtIso.ParametersOnS1(aUIso, aVIso);

    aUIso = ElCLib::InPeriod(aUIso, 0.0, aPeriod);

    // Sought U-parameter in the apex point

    // For 2 possible parameters value,
    // one will be chosen which is nearer
    // to aUIso. Following variables will help to chose.
    Standard_Real aMinDelta = RealLast();
    for (Standard_Integer anIdx = 0; anIdx < aNbTangent; anIdx++)
    {
      // Vector {@\cos(a), \sin(a)@}
      gp_Vec2d            aVecCS(aTgILine[anIdx].X(), aTgILine[anIdx].Y());
      const Standard_Real aSqMod = aVecCS.SquareMagnitude();
      if (aSqMod < Precision::SquareConfusion())
      {
        theIsIsoChoosen = Standard_True;
        break;
      }

      // Normalize
      aVecCS.Divide(Sqrt(aSqMod));

      // Angle in range [0, PI/2]
      Standard_Real anUq =
        (Abs(aVecCS.X()) < Abs(aVecCS.Y())) ? ACos(Abs(aVecCS.X())) : ASin(Abs(aVecCS.Y()));

      // Convert angles to the range [0, 2*PI]
      if (aVecCS.Y() < 0.0)
      {
        if (aVecCS.X() > 0.0)
        {
          anUq = -anUq;
        }
        else
        {
          anUq += M_PI;
        }
      }
      else if (aVecCS.X() < 0.0)
      {
        anUq = M_PI - anUq;
      }

      // Select the parameter the nearest to aUIso
      anUq                 = ElCLib::InPeriod(anUq, 0.0, aPeriod);
      Standard_Real aDelta = Abs(anUq - aUIso);
      if (aDelta > M_PI)
        aDelta = aPeriod - aDelta;

      if (aDelta < aMinDelta)
      {
        aMinDelta = aDelta;
        theUquad  = anUq;
      }
    }
  }

  if (theIsIsoChoosen)
  {
#ifdef INTPATCH_ADDSPECIALPOINTS_DEBUG
    std::cout << "Cannot find UV-coordinate for quadric in the pole."
                 " IntPatch_AddSpecialPoints.cxx, ProcessCone(...)"
              << std::endl;
#endif
    theIsIsoChoosen = Standard_True;

    Standard_Real aUIso = 0.0, aVIso = 0.0;
    if (theIsReversed)
      thePtIso.ParametersOnS2(aUIso, aVIso);
    else
      thePtIso.ParametersOnS1(aUIso, aVIso);

    theUquad = aUIso;
    return Standard_True;
  }
  else
  {
    return Standard_True;
  }

  // return Standard_False;
}

//=======================================================================
// function : GetTangentToIntLineForCone
// purpose  : The following conditions must be satisfied:
// 1. The cone is represented in its canonical form.
// 2. The plane goes through the cone apex and has the normal vector thePlnNormal.
// 3. Vector thePlnNormal has already been normalized
/*
Let us enter the new coordinate system where the origin will be in the cone apex
and axes are the same as in World-Coordinate-System (WCS).
There the horizontal plane (which is parallel XY-plane) with the origin
(0, 0, 1) will intersect the cone by the circle with center (0, 0, 1),
direction {0, 0, 1} and radius tg(a) where a is the cone semi-angle.
Its equation will be
\left\{\begin{matrix}
x(U_{n}) = \tan(a)*\cos(U_{n}) = \tan(a)*\frac{1-\tan^{2}(U_{n}/2)}{1+\tan^{2}(U_{n}/2)}\\
y(U_{n}) = \tan(a)*\sin (U_{n}) = \tan(a)*\frac{2*\tan(U_{n}/2)}{1+\tan^{2}(U_{n}/2)}\\
z(U_{n}) = 1
\end{matrix}\right.

The given plane has (in this coordinate system) location (0, 0, 0) and
the same normal thePlnNormal=={nx,ny,nz}. Its equation is:
nx*x+ny*y+nz*z==0

After substitution circle's equation to the plane's equation
we will obtain a quadratic equation
aA*w^2 + 2*aB*w + aC = 0.
*/
//=======================================================================
Standard_Integer IntPatch_SpecialPoints::GetTangentToIntLineForCone(
  const Standard_Real theConeSemiAngle,
  const gp_XYZ&       thePlnNormal,
  gp_XYZ              theResult[2])
{
  const Standard_Real aNullTol = Epsilon(1.0);
  const Standard_Real aTanA    = Tan(theConeSemiAngle);
  const Standard_Real aA       = thePlnNormal.Z() / aTanA - thePlnNormal.X();
  const Standard_Real aB       = thePlnNormal.Y();
  const Standard_Real aC       = thePlnNormal.Z() / aTanA + thePlnNormal.X();

  if (Abs(aA) < aNullTol)
  {
    if (Abs(aB) > aNullTol)
    {
      // The plane goes along the cone generatrix.
      GetTangent(theConeSemiAngle, -aC / (aB + aB), theResult[0]);
      return 1;
    }

    // The cone and the plane have only one common point.
    // It is the cone apex.
    return 0;
  }

  // Discriminant of this equation is equal to
  Standard_Real aDiscr = thePlnNormal.Z() / Sin(theConeSemiAngle);
  aDiscr               = 1.0 - aDiscr * aDiscr;

  if (Abs(aDiscr) < aNullTol)
  {
    // The plane goes along the cone generatrix.
    //  Attention! Mathematically, this cond. is equivalent to
    //  above processed one (Abs(aA) < aNullTol && (Abs(aB) > aNullTol)).
    //  However, we separate this branch in order to eliminate numerical
    //  instability.

    GetTangent(theConeSemiAngle, -aB / aA, theResult[0]);
    return 1;
  }
  else if (aDiscr > 0.0)
  {
    const Standard_Real aRD = Sqrt(aDiscr);
    GetTangent(theConeSemiAngle, (-aB + aRD) / aA, theResult[0]);
    GetTangent(theConeSemiAngle, (-aB - aRD) / aA, theResult[1]);
    return 2;
  }

  // We will never come here.
  return 0;
}

//=======================================================================
// function : AddSingularPole
// purpose  : theQSurf is the surface possibly containing special point,
//            thePSurf is another surface to intersect.
//           Returns TRUE, if the pole is an intersection point.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::AddSingularPole(const Handle(Adaptor3d_Surface)& theQSurf,
                                                         const Handle(Adaptor3d_Surface)& thePSurf,
                                                         const IntSurf_PntOn2S&           thePtIso,
                                                         IntPatch_Point&                  theVertex,
                                                         IntSurf_PntOn2S&       theAddedPoint,
                                                         const Standard_Boolean theIsReversed,
                                                         const Standard_Boolean theIsReqRefCheck)
{
  // On parametric
  Standard_Real aU0 = 0.0, aV0 = 0.0;
  // aPQuad is Pole
  gp_Pnt        aPQuad, aP0;
  Standard_Real aUquad = 0.0, aVquad = 0.0;
  if (theIsReversed)
    theVertex.Parameters(aU0, aV0, aUquad, aVquad);
  else
    theVertex.Parameters(aUquad, aVquad, aU0, aV0);

  aUquad = 0.0;

  if (theQSurf->GetType() == GeomAbs_Sphere)
  {
    aVquad = Sign(M_PI_2, aVquad);
  }
  else if (theQSurf->GetType() == GeomAbs_Cone)
  {
    const gp_Cone       aCo        = theQSurf->Cone();
    const Standard_Real aRadius    = aCo.RefRadius();
    const Standard_Real aSemiAngle = aCo.SemiAngle();
    aVquad                         = -aRadius / sin(aSemiAngle);
  }
  else
  {
    throw Standard_TypeMismatch("IntPatch_SpecialPoints::AddSingularPole(),"
                                "Unsupported quadric with Pole");
  }

  theQSurf->D0(aUquad, aVquad, aPQuad);
  const Standard_Real aTol = theVertex.Tolerance();
  if (theIsReqRefCheck && (aPQuad.SquareDistance(theVertex.Value()) >= aTol * aTol))
  {
    return Standard_False;
  }

  if (!IsPointOnSurface(thePSurf, aPQuad, aTol, aP0, aU0, aV0))
  {
    return Standard_False;
  }

  // Pole is an intersection point
  //(lies in the quadric and the parametric surface)

  if (theIsReversed)
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  const Standard_Boolean isSame = theAddedPoint.IsSame(theVertex.PntOn2S(), Precision::Confusion());

  // Found pole does not exist in the Walking-line
  // It must be added there (with correct 2D-parameters)

  // 2D-parameters of thePSurf surface have already been found (aU0, aV0).
  // Let find 2D-parameters on the quadric.

  // The algorithm depends on the type of the quadric.
  // Here we consider a Sphere and cone only.

  // First of all, we need in adjusting thePSurf in the coordinate system of the Sphere/Cone
  //(in order to make its equation maximal simple). However, as it will be
  // shown later, thePSurf is used in algorithm in order to get its derivatives.
  // Therefore, for improving performance, transformation of these vectors is enough
  //(there is no point in transformation of full surface).

  gp_Pnt aPtemp;
  gp_Vec aVecDu, aVecDv;
  thePSurf->D1(aU0, aV0, aPtemp, aVecDu, aVecDv);

  // Transforms parametric surface in coordinate-system of the quadric
  gp_Trsf aTr;
  aTr.SetTransformation((theQSurf->GetType() == GeomAbs_Sphere) ? theQSurf->Sphere().Position()
                                                                : theQSurf->Cone().Position());

  // Derivatives of transformed thePSurf
  aVecDu.Transform(aTr);
  aVecDv.Transform(aTr);

  Standard_Boolean isIsoChoosen = Standard_False;

  if (theQSurf->GetType() == GeomAbs_Sphere)
  {
    if (!ProcessSphere(thePtIso, aVecDu, aVecDv, theIsReversed, aVquad, aUquad, isIsoChoosen))
    {
      return Standard_False;
    }
  }
  else // if(theQSurf->GetType() == GeomAbs_Cone)
  {
    if (!ProcessCone(thePtIso,
                     aVecDu,
                     aVecDv,
                     theQSurf->Cone(),
                     theIsReversed,
                     aUquad,
                     isIsoChoosen))
    {
      return Standard_False;
    }
  }

  if (theIsReversed)
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5 * (aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  if (isSame)
  {
    theVertex.SetValue(theAddedPoint);
    return Standard_True;
  }

  if (!isIsoChoosen)
  {
    Standard_Real anArrOfPeriod[4];
    if (theIsReversed)
    {
      IntSurf::SetPeriod(thePSurf, theQSurf, anArrOfPeriod);
    }
    else
    {
      IntSurf::SetPeriod(theQSurf, thePSurf, anArrOfPeriod);
    }

    AdjustPointAndVertex(theVertex.PntOn2S(), anArrOfPeriod, theAddedPoint);
  }
  else
  {
    theVertex.SetValue(theAddedPoint);
  }

  return Standard_True;
}

//=======================================================================
// function : ContinueAfterSpecialPoint
// purpose  : If the last point of the line is the pole of the quadric then
//            the Walking-line has been broken in this point.
//           However, new line must start from this point. Here we must
//            find 2D-coordinates of "this new" point.
/*
The inters. line in the neighborhood of the Apex/Pole(s) can be
approximated by the intersection result of the Cone/Sphere with
the plane going through the Apex/Pole and being tangent to the
2nd intersected surface. This intersection result is well known.

In case of sphere, the inters. result is a circle.
If we go along this circle and across the Pole then U-parameter of
the sphere (@U_{q}@) will change to +/-PI.

In case of cone, the inters. result is two intersected lines (which
can be merged to one in a special case when the plane goes along
some generatrix of the cone). The direction of these lines
are computed by GetTangentToIntLineForCone(...) method).

When the real (not lines) inters. curve goes through the cone apex then
two variants are possible:
a) The tangent line to the inters. curve will be left. In this case
U-parameter of the cone (@U_{q}@) will be change to +/-PI.
b) Another line (as inters. result of cone + plane) will tangent
to the inters. curve. In this case @U_{q}@ must be recomputed.
*/
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::ContinueAfterSpecialPoint(
  const Handle(Adaptor3d_Surface)& theQSurf,
  const Handle(Adaptor3d_Surface)& thePSurf,
  const IntSurf_PntOn2S&           theRefPt,
  const IntPatch_SpecPntType       theSPType,
  const Standard_Real              theTol2D,
  IntSurf_PntOn2S&                 theNewPoint,
  const Standard_Boolean           theIsReversed)
{
  if (theSPType == IntPatch_SPntNone)
    return Standard_False;

  if (theNewPoint.IsSame(theRefPt, Precision::Confusion(), theTol2D))
  {
    return Standard_False;
  }

  if ((theSPType == IntPatch_SPntPole) && (theQSurf->GetType() == GeomAbs_Cone))
  {
    // Check if the condition b) is satisfied.
    // Repeat the same steps as in
    // IntPatch_SpecialPoints::AddSingularPole(...) method.

    // On parametric
    Standard_Real aU0 = 0.0, aV0 = 0.0;
    // On quadric
    Standard_Real aUquad = 0.0, aVquad = 0.0;

    if (theIsReversed)
      theNewPoint.Parameters(aU0, aV0, aUquad, aVquad);
    else
      theNewPoint.Parameters(aUquad, aVquad, aU0, aV0);

    gp_Pnt aPtemp;
    gp_Vec aVecDu, aVecDv;
    thePSurf->D1(aU0, aV0, aPtemp, aVecDu, aVecDv);

    // Transforms parametric surface in coordinate-system of the quadric
    gp_Trsf aTr;
    aTr.SetTransformation(theQSurf->Cone().Position());

    // Derivatives of transformed thePSurf
    aVecDu.Transform(aTr);
    aVecDv.Transform(aTr);

    Standard_Boolean isIsoChoosen = Standard_False;
    ProcessCone(theRefPt, aVecDu, aVecDv, theQSurf->Cone(), theIsReversed, aUquad, isIsoChoosen);

    theNewPoint.SetValue(!theIsReversed, aUquad, aVquad);
  }

  // As it has already been said, in case of going through the Pole/Apex,
  // U-parameter of the quadric surface will change to +/-PI. This rule has some
  // exceptions:
  // 1. When 2nd surface has C0-continuity in the point common with the Apex/Pole.
  //     In this case, the tangent line to the intersection curve after the Apex/Pole
  //     must be totally recomputed according to the new derivatives of the 2nd surface.
  //     Currently, it is not implemented but will be able to be done after the
  //     corresponding demand.
  // 2. The inters. curve has C1 continuity but huge curvature in the point common with
  //     the Apex/Pole. Existing inters. algorithm does not allow putting many points
  //     near to the Apex/Pole in order to cover this "sharp" piece of the inters. curve.
  //     Therefore, we use adjusting U-parameter of the quadric surface with
  //     period PI/2 instead of 2PI. It does not have any mathematical idea
  //     but allows creating WLine with more or less uniform distributed points.
  //     In other words, we forbid "jumping" between two neighbor Walking-points
  //     with step greater than PI/4.

  const Standard_Real aPeriod = (theSPType == IntPatch_SPntPole) ? M_PI_2 : 2.0 * M_PI;

  const Standard_Real aUpPeriod = thePSurf->IsUPeriodic() ? thePSurf->UPeriod() : 0.0;
  const Standard_Real aUqPeriod = theQSurf->IsUPeriodic() ? aPeriod : 0.0;
  const Standard_Real aVpPeriod = thePSurf->IsVPeriodic() ? thePSurf->VPeriod() : 0.0;
  const Standard_Real aVqPeriod = theQSurf->IsVPeriodic() ? aPeriod : 0.0;

  const Standard_Real anArrOfPeriod[4] = {theIsReversed ? aUpPeriod : aUqPeriod,
                                          theIsReversed ? aVpPeriod : aVqPeriod,
                                          theIsReversed ? aUqPeriod : aUpPeriod,
                                          theIsReversed ? aVqPeriod : aVpPeriod};

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theNewPoint);
  return Standard_True;
}

//=================================================================================================

void IntPatch_SpecialPoints::AdjustPointAndVertex(const IntSurf_PntOn2S& theRefPoint,
                                                  const Standard_Real    theArrPeriods[4],
                                                  IntSurf_PntOn2S&       theNewPoint,
                                                  IntPatch_Point* const  theVertex)
{
  Standard_Real aRefPar[2] = {0.0, 0.0};
  Standard_Real aPar[4]    = {0.0, 0.0, 0.0, 0.0};
  theNewPoint.Parameters(aPar[0], aPar[1], aPar[2], aPar[3]);

  for (Standard_Integer i = 0; i < 4; i++)
  {
    if (theArrPeriods[i] == 0)
      continue;

    const Standard_Real aPeriod = theArrPeriods[i], aHalfPeriod = 0.5 * theArrPeriods[i];

    if (i < 2)
    { // 1st surface is used
      theRefPoint.ParametersOnS1(aRefPar[0], aRefPar[1]);
    }
    else
    {
      theRefPoint.ParametersOnS2(aRefPar[0], aRefPar[1]);
    }

    const Standard_Integer aRefInd = i % 2;

    {
      Standard_Real       aDeltaPar = aRefPar[aRefInd] - aPar[i];
      const Standard_Real anIncr    = Sign(aPeriod, aDeltaPar);
      while ((aDeltaPar > aHalfPeriod) || (aDeltaPar < -aHalfPeriod))
      {
        aPar[i] += anIncr;
        aDeltaPar = aRefPar[aRefInd] - aPar[i];
      }
    }
  }

  if (theVertex)
    (*theVertex).SetParameters(aPar[0], aPar[1], aPar[2], aPar[3]);

  theNewPoint.SetValue(aPar[0], aPar[1], aPar[2], aPar[3]);
}
