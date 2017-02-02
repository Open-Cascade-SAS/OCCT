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

#include <Adaptor3d_HSurface.hxx>
#include <Extrema_ExtPS.hxx>
#include <Extrema_GenLocateExtPS.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <IntPatch_Point.hxx>
#include <IntSurf.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <Standard_TypeMismatch.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Matrix.hxx>

// The function for searching intersection point, which 
// lies in the seam-edge of the quadric definetely.
class FuncPreciseSeam: public math_FunctionSetWithDerivatives
{
public:
  FuncPreciseSeam(const Handle(Adaptor3d_HSurface)& theQSurf, // quadric
                  const Handle(Adaptor3d_HSurface)& thePSurf, // another surface
                  const Standard_Boolean isTheUSeam): 
        myQSurf(theQSurf),
        myPSurf(thePSurf),
        mySeamCoordInd(isTheUSeam? 1 : 0) // Defines, U- or V-seam is used
  {
  };
  
  virtual Standard_Integer NbVariables() const
  {
    return 3;
  };

  virtual Standard_Integer NbEquations() const
  {
    return 3;
  }

  virtual Standard_Boolean Value(const math_Vector& theX,
                                 math_Vector& theF)
  {
    try
    {
      const Standard_Integer anIndX = theX.Lower(), anIndF = theF.Lower();
      Standard_Real aUV[] = {0.0, 0.0};
      aUV[mySeamCoordInd] = theX(anIndX+2);
      const gp_Pnt aP1(myPSurf->Value(theX(anIndX), theX(anIndX+1)));
      const gp_Pnt aP2(myQSurf->Value(aUV[0], aUV[1]));

      (aP1.XYZ()-aP2.XYZ()).Coord(theF(anIndF), theF(anIndF+1), theF(anIndF+2));
    }
    catch(Standard_Failure)
    {
      return Standard_False;
    }

    return Standard_True;
  };

  virtual Standard_Boolean Derivatives(const math_Vector& theX,
                                       math_Matrix& theD)
  {
    try
    {
      const Standard_Integer anIndX = theX.Lower(),
                             anIndRD = theD.LowerRow(),
                             anIndCD = theD.LowerCol();
      Standard_Real aUV[] = {0.0, 0.0};
      aUV[mySeamCoordInd] = theX(anIndX+2);

      gp_Pnt aPt;

      //0 for U-coordinate, 1 - for V one
      gp_Vec aD1[2], aD2[2];
      myPSurf->D1(theX(anIndX), theX(anIndX+1), aPt, aD1[0], aD1[1]);
      myQSurf->D1(aUV[0], aUV[1], aPt, aD2[0], aD2[1]);

      // d/dX1
      aD1[0].Coord(theD(anIndRD, anIndCD),
                      theD(anIndRD+1, anIndCD), theD(anIndRD+2, anIndCD));

      // d/dX2
      aD1[1].Coord(theD(anIndRD, anIndCD+1),
                      theD(anIndRD+1, anIndCD+1), theD(anIndRD+2, anIndCD+1));

      // d/dX3
      aD2[mySeamCoordInd].Reversed().Coord(theD(anIndRD, anIndCD+2),
                                theD(anIndRD+1, anIndCD+2), theD(anIndRD+2, anIndCD+2));
    }
    catch(Standard_Failure)
    {
      return Standard_False;
    }

    return Standard_True;
  };

  virtual Standard_Boolean Values (const math_Vector& theX,
                                   math_Vector& theF,
                                   math_Matrix& theD)
  {
    if(!Value(theX, theF))
      return Standard_False;

    if(!Derivatives(theX, theD))
      return Standard_False;

    return Standard_True;
  }

protected:
  FuncPreciseSeam operator=(FuncPreciseSeam&);

private:
  const Handle(Adaptor3d_HSurface)& myQSurf;
  const Handle(Adaptor3d_HSurface)& myPSurf;

  // 1 for U-coordinate, 0 - for V one. 
  const Standard_Integer mySeamCoordInd;
};

//=======================================================================
//function : IsPointOnSurface
//purpose  : Checks if thePt is in theSurf (with given tolerance).
//            Returns the foot of projection (theProjPt) and its parameters
//           on theSurf.
//=======================================================================
static Standard_Boolean IsPointOnSurface(const Handle(Adaptor3d_HSurface)& theSurf,
                                         const gp_Pnt& thePt,
                                         const Standard_Real theTol,
                                         gp_Pnt& theProjPt,
                                         Standard_Real& theUpar,
                                         Standard_Real& theVpar)
{
  Standard_Boolean aRetVal = Standard_False;

  switch(theSurf->GetType())
  {
  case GeomAbs_Plane:
  case GeomAbs_Cylinder:
  case GeomAbs_Cone:
  case GeomAbs_Sphere:
  case GeomAbs_Torus:
  case GeomAbs_SurfaceOfExtrusion:
  case GeomAbs_SurfaceOfRevolution:
    {
      Extrema_ExtPS anExtr(thePt, theSurf->Surface(), theSurf->UResolution(theTol),
                              theSurf->VResolution(theTol), Extrema_ExtFlag_MIN);
      if(!anExtr.IsDone() || (anExtr.NbExt() < 1))
      {
        aRetVal = Standard_False;
      }
      else
      {
        Standard_Integer anExtrIndex = 1;
        Standard_Real aSqDistMin = anExtr.SquareDistance(anExtrIndex);
        for(Standard_Integer i = anExtrIndex + 1; i <= anExtr.NbExt(); i++)
        {
          const Standard_Real aSqD = anExtr.SquareDistance(i);
          if(aSqD < aSqDistMin)
          {
            aSqDistMin = aSqD;
            anExtrIndex = i;
          }
        }

        if(aSqDistMin > theTol*theTol)
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
  default:
    {
      Extrema_GenLocateExtPS anExtr(theSurf->Surface());
      anExtr.Perform(thePt, theUpar, theVpar);
      if(!anExtr.IsDone() || (anExtr.SquareDistance() > theTol*theTol))
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
//function : AddCrossUVIsoPoint
//purpose  : theQSurf is the surface possibly containing special point, 
//            thePSurf is another surface to intersect.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::
                      AddCrossUVIsoPoint(const Handle(Adaptor3d_HSurface)& theQSurf,
                                         const Handle(Adaptor3d_HSurface)& thePSurf,
                                         const IntSurf_PntOn2S& theRefPt,
                                         const Standard_Real theTol,
                                         IntSurf_PntOn2S& theAddedPoint,
                                         const Standard_Boolean theIsReversed)
{
  Standard_Real anArrOfPeriod[4] = {0.0, 0.0, 0.0, 0.0};
  IntSurf::SetPeriod(theIsReversed ? thePSurf : theQSurf,
                     theIsReversed ? theQSurf : thePSurf, anArrOfPeriod);

  gp_Pnt aPQuad;

  //Not quadric point
  Standard_Real aU0 = 0.0, aV0 = 0.0;
  if(theIsReversed)
    theRefPt.ParametersOnS1(aU0, aV0);
  else
    theRefPt.ParametersOnS2(aU0, aV0);

  //Quadric point
  Standard_Real aUquad = 0.0, aVquad = 0.0; 

  theQSurf->D0(aUquad, aVquad, aPQuad);

  Extrema_GenLocateExtPS anExtr(thePSurf->Surface());
  anExtr.Perform(aPQuad, aU0, aV0);

  if(!anExtr.IsDone())
  {
    return Standard_False;
  }

  if(anExtr.SquareDistance() > theTol*theTol)
  {
    return Standard_False;
  }

  anExtr.Point().Parameter(aU0, aV0);
  gp_Pnt aP0(anExtr.Point().Value());
  
  if(theIsReversed)
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theAddedPoint);

  return Standard_True;
}

//=======================================================================
//function : AddPointOnUorVIso
//purpose  : theQSurf is the surface possibly containing special point, 
//            thePSurf is another surface to intersect.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::
                      AddPointOnUorVIso(const Handle(Adaptor3d_HSurface)& theQSurf,
                                        const Handle(Adaptor3d_HSurface)& thePSurf,
                                        const IntSurf_PntOn2S& theRefPt,
                                        const Standard_Boolean theIsU,
                                        const math_Vector& theToler,
                                        const math_Vector& theInitPoint,
                                        const math_Vector& theInfBound,
                                        const math_Vector& theSupBound,
                                        IntSurf_PntOn2S& theAddedPoint,
                                        const Standard_Boolean theIsReversed)
{
  Standard_Real anArrOfPeriod[4] = {0.0, 0.0, 0.0, 0.0};
  IntSurf::SetPeriod(theIsReversed ? thePSurf : theQSurf,
                     theIsReversed ? theQSurf : thePSurf, anArrOfPeriod);

  FuncPreciseSeam aF(theQSurf, thePSurf, theIsU);

  math_FunctionSetRoot aSRF(aF, theToler);
  aSRF.Perform(aF, theInitPoint, theInfBound, theSupBound);

  if(!aSRF.IsDone())
  {
    return Standard_False;
  }

  math_Vector aRoots(theInitPoint.Lower(), theInitPoint.Upper());
  aSRF.Root(aRoots);

  //On parametric
  Standard_Real aU0 = aRoots(1), aV0 = aRoots(2);

  //On quadric
  Standard_Real aUquad = theIsU ? 0.0 : aRoots(3);
  Standard_Real aVquad = theIsU ? aRoots(3) : 0.0; 
  const gp_Pnt aPQuad(theQSurf->Value(aUquad, aVquad));
  const gp_Pnt aP0(thePSurf->Value(aU0, aV0));

  if(theIsReversed)
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theAddedPoint);
  return Standard_True;
}

//=======================================================================
//function : AddSingularPole
//purpose  : theQSurf is the surface possibly containing special point, 
//            thePSurf is another surface to intersect.
//           Returns TRUE, if the pole is an intersection point.
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::
                      AddSingularPole(const Handle(Adaptor3d_HSurface)& theQSurf,
                                      const Handle(Adaptor3d_HSurface)& thePSurf,
                                      const IntSurf_PntOn2S& thePtIso,
                                      const Standard_Real theTol,
                                      IntPatch_Point& theVertex,
                                      IntSurf_PntOn2S& theAddedPoint,                                      
                                      const Standard_Boolean theIsReversed,
                                      const Standard_Boolean theIsReqRefCheck)
{
  const Standard_Real aUpPeriod = thePSurf->IsUPeriodic() ? thePSurf->UPeriod() : 0.0;
  const Standard_Real aUqPeriod = theQSurf->IsUPeriodic() ? theQSurf->UPeriod() : 0.0;
  const Standard_Real aVpPeriod = thePSurf->IsVPeriodic() ? thePSurf->VPeriod() : 0.0;
  const Standard_Real aVqPeriod = theQSurf->IsVPeriodic() ? theQSurf->VPeriod() : 0.0;

  const Standard_Real anArrOfPeriod[4] = {theIsReversed? aUpPeriod : aUqPeriod,
                                          theIsReversed? aVpPeriod : aVqPeriod,
                                          theIsReversed? aUqPeriod : aUpPeriod,
                                          theIsReversed? aVqPeriod : aVpPeriod};

  //On parametric
  Standard_Real aU0 = 0.0, aV0 = 0.0;
  //aPQuad is Pole
  gp_Pnt aPQuad, aP0;
  Standard_Real aUquad = 0.0, aVquad = 0.0;
  if(theIsReversed)
    theVertex.Parameters(aU0, aV0, aUquad, aVquad);
  else
    theVertex.Parameters(aUquad, aVquad, aU0, aV0);

  aUquad = 0.0;

  if(theQSurf->GetType() == GeomAbs_Sphere)
  {
    aVquad = Sign(M_PI_2, aVquad);
  }
  else if(theQSurf->GetType() == GeomAbs_Cone)
  {
    const gp_Cone aCo = theQSurf->Cone();
    const Standard_Real aRadius = aCo.RefRadius();
    const Standard_Real aSemiAngle = aCo.SemiAngle();
    aVquad = -aRadius / sin(aSemiAngle);
  }
  else
  {
    throw Standard_TypeMismatch( "IntPatch_SpecialPoints::AddSingularPole(),"
                                  "Unsupported quadric with Pole");
  }

  theQSurf->D0(aUquad, aVquad, aPQuad);

  if (theIsReqRefCheck && (aPQuad.SquareDistance(theVertex.Value()) >= theTol*theTol))
  {
    return Standard_False;
  }

  if(!IsPointOnSurface(thePSurf, aPQuad, theTol, aP0, aU0, aV0))
  {
    return Standard_False;
  }

  //Pole is an intersection point
  //(lies in the quadric and the parametric surface)

  if(theIsReversed)
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  Standard_Boolean isSame = Standard_False;

  if (theAddedPoint.IsSame(theVertex.PntOn2S(), Precision::Confusion()))
  {
    isSame = Standard_True;
  }

  //Found pole does not exist in the Walking-line
  //It must be added there (with correct 2D-parameters)

  //2D-parameters of theparametric surface have already been found (aU0, aV0).
  //Let find 2D-parameters on the quadric.

  //The algorithm depends on the type of the quadric. Here we consider a Sphere only.
  //Analogical result can be made for another types (e.g. cone, but formulas will
  //be different) in case of need.

  //First of all, we need in adjusting thePSurf in the coordinate system of the Sphere
  //(in order to make the equation of the sphere maximal simple). However, as it will be
  //shown later, thePSurf is used in algorithm in order to get its derivatives.
  //Therefore, for improving performance, transformation of these vectors is enough
  //(there is no point in transformation of full surface).
  
  gp_Pnt aPtemp;
  gp_Vec aVecDu, aVecDv;
  thePSurf->D1(aU0, aV0, aPtemp, aVecDu, aVecDv);

  //Transforms parametric surface in coordinate-system of the quadric
  gp_Trsf aTr;
  aTr.SetTransformation((theQSurf->GetType() == GeomAbs_Sphere) ?
                          theQSurf->Sphere().Position() :
                                      theQSurf->Cone().Position());

  //Derivatives of transformed thePSurf
  aVecDu.Transform(aTr);
  aVecDv.Transform(aTr);

  Standard_Boolean isIsoChoosen = Standard_False;

  if(theQSurf->GetType() == GeomAbs_Sphere)
  {
    //The intersection point (including the pole)
    //must be satisfied to the following system:
    
    //    \left\{\begin{matrix}
    //    R*\cos (U_{q})*\cos (V_{q})=S_{x}(U_{s},V_{s})
    //    R*\sin (U_{q})*\cos (V_{q})=S_{y}(U_{s},V_{s})
    //    R*\sin (V_{q})=S_{z}(U_{s},V_{s})
    //    \end{matrix}\right,
    //where 
    //  R is the radius of the sphere;
    //  @S_{x}@, @S_{y}@ and @S_{z}@ are X, Y and Z-coordinates of thePSurf;
    //  @U_{s}@ and @V_{s}@ are equal to aU0 and aV0 corespondingly;
    //  @U_{q}@ and @V_{q}@ are equal to aUquad and aVquad corespondingly.

    //Consequently (from first two equations), 
    //  \left\{\begin{matrix}
    //  \cos (U_{q}) = \frac{S_{x}(U_{s},V_{s})}{R*\cos (V_{q})}
    //  \sin (U_{q}) = \frac{S_{y}(U_{s},V_{s})}{R*\cos (V_{q})}
    //  \end{matrix}\right.

    //For pole, 
    //  V_{q}=\pm \pi /2 \Rightarrow \cos (V_{q}) = 0 (denominator is equal to 0).

    //Therefore, computation U_{q} directly is impossibly.
    //
    //Let @V_{q}@ tends to @\pm \pi /2@.
    //Then (indeterminate form is evaluated in accordance of L'Hospital rule),
    //  \cos (U_{q}) = \lim_{V_{q} \to (\pi /2-0)} 
    //  \frac{S_{x}(U_{s},V_{s})}{R*\cos (V_{q})}= 
    //  -\lim_{V_{q} \to (\pi /2-0)}
    //  \frac{\frac{\partial S_{x}}
    //  {\partial U_{s}}*\frac{\mathrm{d} U_{s}} 
    //  {\mathrm{d} V_{q}}+\frac{\partial S_{x}} 
    //  {\partial V_{s}}*\frac{\mathrm{d} V_{s}} 
    //  {\mathrm{d} V_{q}}}{R*\sin (V_{q})} =  
    //  -\frac{1}{R}*\frac{\mathrm{d} U_{s}}
    //  {\mathrm{d} V_{q}}*(\frac{\partial S_{x}} 
    //  {\partial U_{s}}+\frac{\partial S_{x}}
    //  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}
    //  {\mathrm{d} U_{s}}) =
    //  -\frac{1}{R}*\frac{\mathrm{d} V_{s}}
    //  {\mathrm{d} V_{q}}*(\frac{\partial S_{x}} 
    //  {\partial U_{s}}*\frac{\mathrm{d} U_{s}}
    //  {\mathrm{d} V_{s}}+\frac{\partial S_{x}}
    //  {\partial V_{s}}).

    //Analogicaly for @\sin (U_{q})@ (@S_{x}@ is substituted to @S_{y}@).

    //Let mean, that
    //  \cos (U_{q}) \left | _{V_{q} \to (-\pi /2+0)} = \cos (U_{q}) \left | _{V_{q} \to (\pi /2-0)}
    //  \sin (U_{q}) \left | _{V_{q} \to (-\pi /2+0)} = \sin (U_{q}) \left | _{V_{q} \to (\pi /2-0)}

    //From the 3rd equation of the system, we obtain
    //  \frac{\mathrm{d} (R*\sin (V_{q}))}{\mathrm{d} V_{q}} =
    //  \frac{\mathrm{d} S_{z}(U_{s},V_{s})}{\mathrm{d} V_{q}}
    //or
    //  R*\cos (V_{q}) = \frac{\partial S_{z}}{\partial U_{s}}*
    //  \frac{\mathrm{d} U_{s}} {\mathrm{d} V_{q}}+\frac{\partial S_{z}}
    //  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}{\mathrm{d} V_{q}}.

    //If @V_{q}=\pm \pi /2@, then
    //  \frac{\partial S_{z}}{\partial U_{s}}*
    //  \frac{\mathrm{d} U_{s}} {\mathrm{d} V_{q}}+\frac{\partial S_{z}}
    //  {\partial V_{s}}*\frac{\mathrm{d} V_{s}}{\mathrm{d} V_{q}} = 0.

    //Consequently, if @\frac{\partial S_{z}}{\partial U_{s}} \neq 0 @ then
    //  \frac{\mathrm{d} U_{s}}{\mathrm{d} V_{s}} =
    //  -\frac{\frac{\partial S_{z}}{\partial V_{s}}}
    //  {\frac{\partial S_{z}}{\partial U_{s}}}.

    //If @ \frac{\partial S_{z}}{\partial V_{s}} \neq 0 @ then
    //  \frac{\mathrm{d} V_{s}}{\mathrm{d} U_{s}} =
    //  -\frac{\frac{\partial S_{z}}{\partial U_{s}}}
    //  {\frac{\partial S_{z}}{\partial V_{s}}}

    //Cases, when @ \frac{\partial S_{z}}{\partial U_{s}} = 
    //\frac{\partial S_{z}}{\partial V_{s}} = 0 @ are not consider here.
    //The reason is written below.

    //Vector with {@ \cos (U_{q}) @, @ \sin (U_{q}) @} coordinates.
    //Ask to pay attention to the fact that this vector is always normalyzed.
    gp_Vec2d aV1;

    if( (Abs(aVecDu.Z()) < Precision::PConfusion()) &&
        (Abs(aVecDv.Z()) < Precision::PConfusion()))
    {
      //Example of this case is an intersection of a plane with a sphere
      //when the plane tangents the sphere in some pole (i.e. only one 
      //intersection point, not line). In this case, U-coordinate of the
      //sphere is undefined (can be realy anything).
      //Another reason is that we have tangent zone around the pole
      //(see bug #26576).
      //Computation of correct value of aUquad is impossible.
      //Therefore, (in oreder to return something) we will consider
      //the intersection line goes along some isoline in neighbourhood
      //of the pole.

#ifdef INTPATCH_ADDSPECIALPOINTS_DEBUG
      cout << "Cannot find UV-coordinate for quadric in the pole."
        " See considered comment above. IntPatch_AddSpecialPoints.cxx,"
        " AddSingularPole(...)" << endl;
#endif
      Standard_Real aUIso = 0.0, aVIso = 0.0;
      if(theIsReversed)
        thePtIso.ParametersOnS2(aUIso, aVIso);
      else
        thePtIso.ParametersOnS1(aUIso, aVIso);

      aUquad = aUIso;
      isIsoChoosen = Standard_True;
    }
    else
    {
      if(Abs(aVecDu.Z()) > Abs(aVecDv.Z()))
      {
        const Standard_Real aDusDvs = aVecDv.Z()/aVecDu.Z();
        aV1.SetCoord( aVecDu.X()*aDusDvs - aVecDv.X(),
                      aVecDu.Y()*aDusDvs - aVecDv.Y());
      }
      else
      {
        const Standard_Real aDvsDus = aVecDu.Z()/aVecDv.Z();
        aV1.SetCoord( aVecDv.X()*aDvsDus - aVecDu.X(),
                      aVecDv.Y()*aDvsDus - aVecDu.Y());
      }

      aV1.Normalize();

      if(Abs(aV1.X()) > Abs(aV1.Y()))
        aUquad = Sign(asin(aV1.Y()), aVquad);
      else
        aUquad = Sign(acos(aV1.X()), aVquad);
    }
  }
  else //if(theQSurf->GetType() == GeomAbs_Cone)
  {
    // This case is not processed. However,
    // it can be done using the same algorithm
    // as for sphere (formulas will be different).
    return Standard_False;
  }

  if(theIsReversed)
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aU0, aV0, aUquad, aVquad);
  else
    theAddedPoint.SetValue(0.5*(aP0.XYZ() + aPQuad.XYZ()), aUquad, aVquad, aU0, aV0);

  if (isSame)
  {
    theVertex.SetValue(theAddedPoint);
    return Standard_True;
  }

  if (!isIsoChoosen)
  {
    AdjustPointAndVertex(theVertex.PntOn2S(), anArrOfPeriod, theAddedPoint);
  }
  else
  {
    theVertex.SetValue(theAddedPoint);
  }

  return Standard_True;
}

//=======================================================================
//function : ContinueAfterSpecialPoint
//purpose  : 
//=======================================================================
Standard_Boolean IntPatch_SpecialPoints::
                  ContinueAfterSpecialPoint(const Handle(Adaptor3d_HSurface)& theQSurf,
                                            const Handle(Adaptor3d_HSurface)& thePSurf,
                                            const IntSurf_PntOn2S& theRefPt,
                                            const IntPatch_SpecPntType theSPType,
                                            const Standard_Real theTol2D,
                                            IntSurf_PntOn2S& theNewPoint,
                                            const Standard_Boolean theIsReversed)
{
  if(theSPType == IntPatch_SPntNone)
    return Standard_False;

  //If the last point of the line is the pole of the quadric.
  //In this case, Walking-line has been broken in this point.
  //However, new line must start from this point. Here we must
  //find its 2D-coordinates.

  //For sphere and cone, some intersection point is satisfied to the system
  //  \cos(U_{q}) = S_{x}(U_{s},V_{s})/F(V_{q}) 
  //  \sin(U_{q}) = S_{y}(U_{s},V_{s})/F(V_{q}) 

  //where 
  //  @S_{x}@, @S_{y}@ are X and Y-coordinates of thePSurf;
  //  @U_{s}@ and @V_{s}@ are UV-parameters on thePSurf;
  //  @U_{q}@ and @V_{q}@ are UV-parameters on theQSurf;
  //  @F(V_{q}) @ is some function, which value independs on @U_{q}@
  //              (form of this function depends on the type of the quadric).

  //When we go through the pole/apex, the function @F(V_{q}) @ changes sign.
  //Therefore, some cases are possible, when only @\cos(U_{q}) @ or
  //only @ \sin(U_{q}) @ change sign.

  //Consequently, when the line goes throug the pole, @U_{q}@ can be
  //changed on @\pi /2 @ (but not less).
    
  if(theNewPoint.IsSame(theRefPt, Precision::Confusion(), theTol2D))
  {
    return Standard_False;
  }

  //Here, in case of pole/apex adding, we forbid "jumping" between two neighbor
  //Walking-point with step greater than pi/4
  const Standard_Real aPeriod = (theSPType == IntPatch_SPntPole)? M_PI_2 : 2.0*M_PI;

  const Standard_Real aUpPeriod = thePSurf->IsUPeriodic() ? thePSurf->UPeriod() : 0.0;
  const Standard_Real aUqPeriod = theQSurf->IsUPeriodic() ? aPeriod : 0.0;
  const Standard_Real aVpPeriod = thePSurf->IsVPeriodic() ? thePSurf->VPeriod() : 0.0;
  const Standard_Real aVqPeriod = theQSurf->IsVPeriodic() ? aPeriod : 0.0;

  const Standard_Real anArrOfPeriod[4] = {theIsReversed? aUpPeriod : aUqPeriod,
                                          theIsReversed? aVpPeriod : aVqPeriod,
                                          theIsReversed? aUqPeriod : aUpPeriod,
                                          theIsReversed? aVqPeriod : aVpPeriod};

  AdjustPointAndVertex(theRefPt, anArrOfPeriod, theNewPoint);
  return Standard_True;
}

//=======================================================================
//function : AdjustPointAndVertex
//purpose  : 
//=======================================================================
void IntPatch_SpecialPoints::
                AdjustPointAndVertex(const IntSurf_PntOn2S &theRefPoint,
                                     const Standard_Real theArrPeriods[4],
                                     IntSurf_PntOn2S &theNewPoint,
                                     IntPatch_Point* const theVertex)
{
  Standard_Real aRefPar[2] = {0.0, 0.0};
  Standard_Real aPar[4] = {0.0, 0.0, 0.0, 0.0};
  theNewPoint.Parameters(aPar[0], aPar[1], aPar[2], aPar[3]);

  for(Standard_Integer i = 0; i < 4; i++)
  {
    if(theArrPeriods[i] == 0)
      continue;

    const Standard_Real aPeriod = theArrPeriods[i], aHalfPeriod = 0.5*theArrPeriods[i];

    if(i < 2)
    {// 1st surface is used
      theRefPoint.ParametersOnS1(aRefPar[0], aRefPar[1]);
    }
    else
    {
      theRefPoint.ParametersOnS2(aRefPar[0], aRefPar[1]);
    }

    const Standard_Integer aRefInd = i%2;

    {
      Standard_Real aDeltaPar = aRefPar[aRefInd]-aPar[i];
      const Standard_Real anIncr = Sign(aPeriod, aDeltaPar);
      while((aDeltaPar > aHalfPeriod) || (aDeltaPar < -aHalfPeriod))
      {
        aPar[i] += anIncr;
        aDeltaPar = aRefPar[aRefInd]-aPar[i];
      }
    }
  }

  if(theVertex)
    (*theVertex).SetParameters(aPar[0], aPar[1], aPar[2], aPar[3]);

  theNewPoint.SetValue(aPar[0], aPar[1], aPar[2], aPar[3]);
}

