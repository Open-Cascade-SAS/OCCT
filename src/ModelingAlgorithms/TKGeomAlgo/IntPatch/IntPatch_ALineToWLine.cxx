// Created on: 1993-11-26
// Created by: Modelistation
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

#include <IntPatch_ALineToWLine.hxx>

#include <Adaptor3d_Surface.hxx>
#include <ElSLib.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_SpecialPoints.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf.hxx>
#include <IntSurf_LineOn2S.hxx>

//=================================================================================================

static inline void AddPointIntoLine(occ::handle<IntSurf_LineOn2S>& theLine,
                                    const double* const            theArrPeriods,
                                    IntSurf_PntOn2S&               thePoint,
                                    IntPatch_Point*                theVertex = nullptr)
{
  if (theLine->NbPoints() > 0)
  {
    if (thePoint.IsSame(theLine->Value(theLine->NbPoints()), Precision::Confusion()))
      return;

    IntPatch_SpecialPoints::AdjustPointAndVertex(theLine->Value(theLine->NbPoints()),
                                                 theArrPeriods,
                                                 thePoint,
                                                 theVertex);
  }

  theLine->Add(thePoint);
}

//=======================================================================
// function : AddVertexPoint
// purpose  : Extracts IntSurf_PntOn2S from theVertex and adds result in theLine.
//=======================================================================
static void AddVertexPoint(occ::handle<IntSurf_LineOn2S>& theLine,
                           IntPatch_Point&                theVertex,
                           const double* const            theArrPeriods)
{
  IntSurf_PntOn2S anApexPoint = theVertex.PntOn2S();
  AddPointIntoLine(theLine, theArrPeriods, anApexPoint, &theVertex);
}

//=======================================================================
// function : IsPoleOrSeam
// purpose  : Processes theVertex depending on its type
//            (pole/apex/point on boundary etc.) and adds it in theLine.
//           thePIsoRef is the reference point using in case when the
//            value of correspond parameter cannot be precise.
//           theSingularSurfaceID contains the ID of surface with
//            special point (0 - none, 1 - theS1, 2 - theS2)
//=======================================================================
static IntPatch_SpecPntType IsPoleOrSeam(const occ::handle<Adaptor3d_Surface>& theS1,
                                         const occ::handle<Adaptor3d_Surface>& theS2,
                                         const IntSurf_PntOn2S&                thePIsoRef,
                                         occ::handle<IntSurf_LineOn2S>&        theLine,
                                         IntPatch_Point&                       theVertex,
                                         const double                          theArrPeriods[4],
                                         const double                          theTol3d,
                                         int&                                  theSingularSurfaceID)
{
  theSingularSurfaceID = 0;

  for (int i = 0; i < 2; i++)
  {
    const bool                isReversed = (i > 0);
    const GeomAbs_SurfaceType aType      = isReversed ? theS2->GetType() : theS1->GetType();

    IntPatch_SpecPntType anAddedPType = IntPatch_SPntNone;
    IntSurf_PntOn2S      anApexPoint;

    switch (aType)
    {
      case GeomAbs_Sphere:
      case GeomAbs_Cone: {
        if (IntPatch_SpecialPoints::AddSingularPole((isReversed ? theS2 : theS1),
                                                    (isReversed ? theS1 : theS2),
                                                    thePIsoRef,
                                                    theVertex,
                                                    anApexPoint,
                                                    isReversed,
                                                    true))
        {
          anAddedPType = IntPatch_SPntPole;
          break;
        }
      }
        [[fallthrough]];
      case GeomAbs_Torus:
        if (aType == GeomAbs_Torus)
        {
          if (IntPatch_SpecialPoints::AddCrossUVIsoPoint((isReversed ? theS2 : theS1),
                                                         (isReversed ? theS1 : theS2),
                                                         thePIsoRef,
                                                         theTol3d,
                                                         anApexPoint,
                                                         isReversed))
          {
            anAddedPType = IntPatch_SPntSeamUV;
            break;
          }
        }
        [[fallthrough]];
      case GeomAbs_Cylinder:
        theSingularSurfaceID = i + 1;
        AddVertexPoint(theLine, theVertex, theArrPeriods);
        return IntPatch_SPntSeamU;

      default:
        break;
    }

    if (anAddedPType != IntPatch_SPntNone)
    {
      theSingularSurfaceID = i + 1;
      AddPointIntoLine(theLine, theArrPeriods, anApexPoint, &theVertex);
      return anAddedPType;
    }
  }

  return IntPatch_SPntNone;
}

//=================================================================================================

IntPatch_ALineToWLine::IntPatch_ALineToWLine(const occ::handle<Adaptor3d_Surface>& theS1,
                                             const occ::handle<Adaptor3d_Surface>& theS2,
                                             const int                             theNbPoints)
    : myS1(theS1),
      myS2(theS2),
      myNbPointsInWline(theNbPoints),
      myTolOpenDomain(1.e-9),
      myTolTransition(1.e-8),
      myTol3D(Precision::Confusion())
{
  const GeomAbs_SurfaceType aTyps1 = theS1->GetType();
  const GeomAbs_SurfaceType aTyps2 = theS2->GetType();

  switch (aTyps1)
  {
    case GeomAbs_Plane:
      myQuad1.SetValue(theS1->Plane());
      break;

    case GeomAbs_Cylinder:
      myQuad1.SetValue(theS1->Cylinder());
      break;

    case GeomAbs_Sphere:
      myQuad1.SetValue(theS1->Sphere());
      break;

    case GeomAbs_Cone:
      myQuad1.SetValue(theS1->Cone());
      break;

    case GeomAbs_Torus:
      myQuad1.SetValue(theS1->Torus());
      break;

    default:
      break;
  }

  switch (aTyps2)
  {
    case GeomAbs_Plane:
      myQuad2.SetValue(theS2->Plane());
      break;
    case GeomAbs_Cylinder:
      myQuad2.SetValue(theS2->Cylinder());
      break;

    case GeomAbs_Sphere:
      myQuad2.SetValue(theS2->Sphere());
      break;

    case GeomAbs_Cone:
      myQuad2.SetValue(theS2->Cone());
      break;

    case GeomAbs_Torus:
      myQuad2.SetValue(theS2->Torus());
      break;

    default:
      break;
  }
}

//=================================================================================================

void IntPatch_ALineToWLine::SetTol3D(const double aTol)
{
  myTol3D = aTol;
}

//=================================================================================================

double IntPatch_ALineToWLine::Tol3D() const
{
  return myTol3D;
}

//=================================================================================================

void IntPatch_ALineToWLine::SetTolTransition(const double aTol)
{
  myTolTransition = aTol;
}

//=================================================================================================

double IntPatch_ALineToWLine::TolTransition() const
{
  return myTolTransition;
}

//=================================================================================================

void IntPatch_ALineToWLine::SetTolOpenDomain(const double aTol)
{
  myTolOpenDomain = aTol;
}

//=================================================================================================

double IntPatch_ALineToWLine::TolOpenDomain() const
{
  return myTolOpenDomain;
}

//=================================================================================================

void IntPatch_ALineToWLine::CorrectEndPoint(occ::handle<IntSurf_LineOn2S>& theLine,
                                            const int                      theIndex) const
{
  const double aTol   = 1.e-5;
  const double aSqTol = 1.e-10;

  // Perform linear extrapolation from two previous points
  int anIndFirst, anIndSecond;
  if (theIndex == 1)
  {
    anIndFirst  = 3;
    anIndSecond = 2;
  }
  else
  {
    anIndFirst  = theIndex - 2;
    anIndSecond = theIndex - 1;
  }
  IntSurf_PntOn2S aPntOn2S = theLine->Value(theIndex);

  for (int ii = 1; ii <= 2; ii++)
  {
    bool anIsOnFirst = (ii == 1);

    const IntSurf_Quadric& aQuad = (ii == 1) ? myQuad1 : myQuad2;
    if (aQuad.TypeQuadric() == GeomAbs_Cone)
    {
      const gp_Cone aCone  = aQuad.Cone();
      const gp_Pnt  anApex = aCone.Apex();
      if (anApex.SquareDistance(aPntOn2S.Value()) > aSqTol)
        continue;
    }
    else if (aQuad.TypeQuadric() == GeomAbs_Sphere)
    {
      double aU, aV;
      aPntOn2S.ParametersOnSurface(anIsOnFirst, aU, aV);
      if (std::abs(aV - M_PI / 2) > aTol && std::abs(aV + M_PI / 2) > aTol)
        continue;
    }
    else
      continue;

    gp_Pnt2d PrevPrevP2d = theLine->Value(anIndFirst).ValueOnSurface(anIsOnFirst);
    gp_Pnt2d PrevP2d     = theLine->Value(anIndSecond).ValueOnSurface(anIsOnFirst);
    gp_Dir2d aDir        = gp_Vec2d(PrevPrevP2d, PrevP2d);
    double   aX0 = PrevPrevP2d.X(), aY0 = PrevPrevP2d.Y();
    double   aXend, aYend;
    aPntOn2S.ParametersOnSurface(anIsOnFirst, aXend, aYend);

    if (std::abs(aDir.Y()) < gp::Resolution())
      continue;

    double aNewXend = aDir.X() / aDir.Y() * (aYend - aY0) + aX0;

    theLine->SetUV(theIndex, anIsOnFirst, aNewXend, aYend);
  }
}

//=================================================================================================

double IntPatch_ALineToWLine::GetSectionRadius(const gp_Pnt& thePnt3d) const
{
  double aRetVal = RealLast();
  for (int i = 0; i < 2; i++)
  {
    const IntSurf_Quadric& aQuad = i ? myQuad2 : myQuad1;
    if (aQuad.TypeQuadric() == GeomAbs_Cone)
    {
      const gp_Cone aCone = aQuad.Cone();
      const gp_XYZ  aRVec = thePnt3d.XYZ() - aCone.Apex().XYZ();
      const gp_XYZ& aDir  = aCone.Axis().Direction().XYZ();

      aRetVal = std::min(aRetVal, std::abs(aRVec.Dot(aDir) * std::tan(aCone.SemiAngle())));
    }
    else if (aQuad.TypeQuadric() == GeomAbs_Sphere)
    {
      const gp_Sphere aSphere = aQuad.Sphere();
      const gp_XYZ    aRVec   = thePnt3d.XYZ() - aSphere.Location().XYZ();
      const gp_XYZ&   aDir    = aSphere.Position().Direction().XYZ();
      const double    aR      = aSphere.Radius();
      const double    aD      = aRVec.Dot(aDir);
      const double    aDelta  = aR * aR - aD * aD;
      if (aDelta <= 0.0)
      {
        aRetVal = 0.0;
        break;
      }
      else
      {
        aRetVal = std::min(aRetVal, std::sqrt(aDelta));
      }
    }
  }

  return aRetVal;
}

//=================================================================================================

void IntPatch_ALineToWLine::MakeWLine(
  const occ::handle<IntPatch_ALine>&                theAline,
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theLines) const
{
  bool   included;
  double f = theAline->FirstParameter(included);
  if (!included)
  {
    f += myTolOpenDomain;
  }
  double l = theAline->LastParameter(included);
  if (!included)
  {
    l -= myTolOpenDomain;
  }

  MakeWLine(theAline, f, l, theLines);
}

//=================================================================================================

void IntPatch_ALineToWLine::MakeWLine(
  const occ::handle<IntPatch_ALine>&                theALine,
  const double                                      theFPar,
  const double                                      theLPar,
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theLines) const
{
  const int aNbVert = theALine->NbVertex();
  if (aNbVert == 0)
  {
    return;
  }

#if 0
  //To draw ALine as a wire DRAW-object use the following code.
  {
    static int ind = 0;
    ind++;

    bool flShow = true;

    if (flShow)
    {
      std::cout << " +++ DUMP ALine (begin) +++++" << std::endl;
      const int NbSamples = 20;
      const double aStep = (theLPar - theFPar) / NbSamples;
      char* name = new char[100];
      
      for (int ii = 0; ii <= NbSamples; ii++)
      {
        double aPrm = theFPar + ii * aStep;
        const gp_Pnt aPP(theALine->Value(aPrm));
        std::cout << "vertex v" << ii << " " << aPP.X() << " " << aPP.Y() << " " << aPP.Z() << std::endl;

        Sprintf(name, "p%d_%d", ii, ind);
        Draw::Set(name, aPP);
      }
      std::cout << " --- DUMP ALine (end) -----" << std::endl;
    }
  }

  //Copy all output information and apply it as a TCL-code in DRAW.

  //After that, use TCL-script below:

  /* ********************************* Script (begin)
  shape ww w
  copy v1 vprev
  for {set i 2} {$i <= 10000} {incr i} {
    distmini dd vprev v$i;

    if { [dval dd_val] > 1.0e-7} {
      edge ee vprev v$i;
      add ee ww;
      copy v$i vprev;
    }
  }
  ********************************** Script (end) */
#endif

  // The same points can be marked by different vertices.
  // The code below unifies tolerances of all vertices
  // marking the same points.
  for (int i = 1; i < aNbVert; i++)
  {
    IntPatch_Point&        aCurVert  = theALine->ChangeVertex(i);
    const IntSurf_PntOn2S& aCurrPt   = aCurVert.PntOn2S();
    const double           aCurToler = aCurVert.Tolerance();
    for (int j = i + 1; j <= aNbVert; j++)
    {
      IntPatch_Point&        aVert  = theALine->ChangeVertex(j);
      const IntSurf_PntOn2S& aNewPt = aVert.PntOn2S();
      const double           aToler = aVert.Tolerance();

      const double aSumTol = aCurToler + aToler;
      if (aCurrPt.IsSame(aNewPt, aSumTol))
      {
        aCurVert.SetTolerance(aSumTol);
        aVert.SetTolerance(aSumTol);
      }
    }
  }

  const double aTol    = 2.0 * myTol3D + Precision::Confusion();
  const double aPrmTol = std::max(1.0e-4 * (theLPar - theFPar), Precision::PConfusion());

  IntPatch_SpecPntType aPrePointExist = IntPatch_SPntNone;

  NCollection_Array1<double>         aVertexParams(1, aNbVert);
  NCollection_Array1<IntPatch_Point> aSeqVertex(1, aNbVert);

  // It is possible to have several vertices with equal parameters.
  NCollection_Array1<bool> hasVertexBeenChecked(1, aNbVert);

  occ::handle<IntSurf_LineOn2S> aLinOn2S;
  double                        aParameter = theFPar;

  for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
  {
    const IntPatch_Point& aVert = theALine->Vertex(i);
    const double          aPar  = aVert.ParameterOnLine();
    aVertexParams(i)            = aPar;
    hasVertexBeenChecked(i)     = false;
  }

  int    aSingularSurfaceID = 0;
  double anArrPeriods[]     = {0.0,  // U1
                               0.0,  // V1
                               0.0,  // U2
                               0.0}; // V2

  IntSurf::SetPeriod(myS1, myS2, anArrPeriods);

  IntSurf_PntOn2S aPrevLPoint;

  while (aParameter < theLPar)
  {
    double aStep = (theLPar - aParameter) / (double)(myNbPointsInWline - 1);
    if (aStep < Epsilon(theLPar))
      break;

    bool   isStepReduced = false;
    double aLPar         = theLPar;

    for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
    {
      if (hasVertexBeenChecked(i))
        continue;

      aLPar = aVertexParams(i);
      if (std::abs(aLPar - aParameter) < aPrmTol)
        continue;

      break;
    }

    if ((aStep - (aLPar - aParameter) > aPrmTol) && (std::abs(aLPar - aParameter) > aPrmTol))
    {
      aStep         = std::max((aLPar - aParameter) / 5, 1.e-5);
      isStepReduced = true;
    }

    int aNewVertID            = 0;
    aLinOn2S                  = new IntSurf_LineOn2S;
    bool anIsFirstDegenerated = false, anIsLastDegenerated = false;

    double aStepMin = 0.1 * aStep, aStepMax = 10.0 * aStep;

    bool   isLast     = false;
    double aPrevParam = aParameter;
    for (; !isLast; aParameter += aStep)
    {
      IntSurf_PntOn2S aPOn2S;

      if (theLPar <= aParameter)
      {
        isLast = true;
        if (aPrePointExist != IntPatch_SPntNone)
        {
          break;
        }
        else
        {
          aParameter = theLPar;
        }
      }

      bool   isPointValid = false;
      double aTgMagn      = 0.0;
      {
        gp_Pnt aPnt3d;
        gp_Vec aTg;
        theALine->D1(aParameter, aPnt3d, aTg);
        if (GetSectionRadius(aPnt3d) < 5.0e-6)
        {
          // We cannot compute 2D-parameters of
          // aPOn2S correctly.

          if (anIsLastDegenerated) // the current last point is wrong
            aLinOn2S->RemovePoint(aLinOn2S->NbPoints());

          isPointValid = false;
        }
        else
        {
          isPointValid = true;
        }

        aTgMagn   = aTg.Magnitude();
        double u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;
        myQuad1.Parameters(aPnt3d, u1, v1);
        myQuad2.Parameters(aPnt3d, u2, v2);
        aPOn2S.SetValue(aPnt3d, u1, v1, u2, v2);
      }

      if (aPrePointExist != IntPatch_SPntNone)
      {
        const double aURes = std::max(myS1->UResolution(myTol3D), myS2->UResolution(myTol3D)),
                     aVRes = std::max(myS1->VResolution(myTol3D), myS2->VResolution(myTol3D));

        const double aTol2d = (aPrePointExist == IntPatch_SPntPole)     ? -1.0
                              : (aPrePointExist == IntPatch_SPntSeamV)  ? aVRes
                              : (aPrePointExist == IntPatch_SPntSeamUV) ? std::max(aURes, aVRes)
                                                                        : aURes;

        IntSurf_PntOn2S aRPT = aPOn2S;

        if (aPrePointExist == IntPatch_SPntPole)
        {
          double aPrt = 0.5 * (aPrevParam + theLPar);
          for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
          {
            const double aParam = aVertexParams(i);

            if (aParam <= aPrevParam)
              continue;

            if ((aParam - aPrevParam) < aPrmTol)
            {
              const gp_Pnt aPnt3d(theALine->Value(aParam));
              if (aPOn2S.Value().SquareDistance(aPnt3d) < Precision::SquareConfusion())
              {
                // i-th vertex is the same as a Pole/Apex.
                // So, it should be ignored.
                continue;
              }
            }

            aPrt = 0.5 * (aParam + aPrevParam);
            break;
          }

          const gp_Pnt aPnt3d(theALine->Value(aPrt));
          double       u1, v1, u2, v2;
          myQuad1.Parameters(aPnt3d, u1, v1);
          myQuad2.Parameters(aPnt3d, u2, v2);
          aRPT.SetValue(aPnt3d, u1, v1, u2, v2);

          if (aPOn2S.IsSame(aPrevLPoint, std::max(Precision::Approximation(), aTol)))
          {
            // Set V-parameter as precise value found on the previous step.
            if (aSingularSurfaceID == 1)
            {
              aPOn2S.ParametersOnS1(u2, v2);
              aPOn2S.SetValue(true, u1, v2);
            }
            else // if (aSingularSurfaceID == 2)
            {
              aPOn2S.ParametersOnS2(u1, v1);
              aPOn2S.SetValue(false, u2, v1);
            }
          }
        }

        if (IntPatch_SpecialPoints::ContinueAfterSpecialPoint(myS1,
                                                              myS2,
                                                              aRPT,
                                                              aPrePointExist,
                                                              aTol2d,
                                                              aPrevLPoint,
                                                              false))
        {
          AddPointIntoLine(aLinOn2S, anArrPeriods, aPrevLPoint);
        }
        else if (aParameter == theLPar)
        { // Strictly equal!!!
          break;
        }
        else if (aParameter + aStep < theLPar)
        {
          // Prediction of the next point
          gp_Pnt aPnt3dNext;
          gp_Vec aTg;
          theALine->D1(aParameter + aStep, aPnt3dNext, aTg);
          double anU1 = 0.0, aV1 = 0.0, anU2 = 0.0, aV2 = 0.0;
          myQuad1.Parameters(aPnt3dNext, anU1, aV1);
          myQuad2.Parameters(aPnt3dNext, anU2, aV2);
          IntSurf_PntOn2S aPOn2SNext;
          aPOn2SNext.SetValue(aPnt3dNext, anU1, aV1, anU2, aV2);

          if (aPOn2SNext.ValueOnSurface(0).SquareDistance(aRPT.ValueOnSurface(0)) > M_PI * M_PI
              || aPOn2SNext.ValueOnSurface(1).SquareDistance(aRPT.ValueOnSurface(1)) > M_PI * M_PI)
          {
            aPrevLPoint = aRPT;
            aPrevParam  = aParameter;
            continue;
          }
        }
      }

      aPrePointExist = IntPatch_SPntNone;

      int aVertexNumber = -1;
      for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
      {
        if (hasVertexBeenChecked(i))
          continue;

        const IntPatch_Point& aVP    = theALine->Vertex(i);
        const double          aParam = aVertexParams(i);
        if (((aPrevParam < aParam) && (aParam <= aParameter))
            || ((aPrevParam == aParameter) && (aParam == aParameter))
            || (aPOn2S.IsSame(aVP.PntOn2S(), aVP.Tolerance())
                && (std::abs(aVP.ParameterOnLine() - aParameter) < aPrmTol)))
        {
          // We have either jumped over the vertex or "fell" on the vertex.
          // However, ALine can be self-interfered. Therefore, we need to check
          // vertex parameter and 3D-distance together.

          aVertexNumber = i;
          break;
        }
      }

      aPrevParam = aParameter;

      if (aVertexNumber < 0)
      {
        if (isPointValid)
        {
          if (!isStepReduced)
          {
            StepComputing(theALine,
                          aPOn2S,
                          theLPar,
                          aParameter,
                          aTgMagn,
                          aStepMin,
                          aStepMax,
                          myTol3D,
                          aStep);
          }

          AddPointIntoLine(aLinOn2S, anArrPeriods, aPOn2S);
          aPrevLPoint = aPOn2S;
        }
        else
        {
          // add point, set corresponding status: to be corrected later
          bool ToAdd = false;
          if (aLinOn2S->NbPoints() == 0)
          {
            anIsFirstDegenerated = true;
            ToAdd                = true;
          }
          else if (aLinOn2S->NbPoints() > 1)
          {
            anIsLastDegenerated = true;
            ToAdd               = true;
          }

          if (ToAdd)
          {
            AddPointIntoLine(aLinOn2S, anArrPeriods, aPOn2S);
            aPrevLPoint = aPOn2S;
          }
        }

        continue;
      }

      IntPatch_Point aVtx            = theALine->Vertex(aVertexNumber);
      double         aNewVertexParam = aLinOn2S->NbPoints() + 1;
      int            aNbPointsPrev   = aLinOn2S->NbPoints();

      // ATTENTION!!!
      //  IsPoleOrSeam inserts new point in aLinOn2S if aVtx respects
      // to some special point. Otherwise, aLinOn2S is not changed.

      // Find a point for reference parameter. It will be used
      // if real parameter value cannot be precise (see comment to
      // IsPoleOrSeam(...) function).
      IntSurf_PntOn2S aPrefIso = aVtx.PntOn2S();
      if (aLinOn2S->NbPoints() < 1)
      {
        for (int i = aVertexNumber + 1; i <= aVertexParams.Upper(); i++)
        {
          const double aParam = aVertexParams(i);
          if ((aParam - aVertexParams(aVertexNumber)) > Precision::PConfusion())
          {
            const double aPrm = 0.5 * (aParam + aVertexParams(aVertexNumber));
            const gp_Pnt aPnt3d(theALine->Value(aPrm));
            double       u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;
            myQuad1.Parameters(aPnt3d, u1, v1);
            myQuad2.Parameters(aPnt3d, u2, v2);
            aPrefIso.SetValue(aPnt3d, u1, v1, u2, v2);
            break;
          }
        }
      }
      else
      {
        aPrefIso = aLinOn2S->Value(aLinOn2S->NbPoints());
      }

      aPrePointExist =
        IsPoleOrSeam(myS1, myS2, aPrefIso, aLinOn2S, aVtx, anArrPeriods, aTol, aSingularSurfaceID);

      if (aPrePointExist == IntPatch_SPntPole || aPrePointExist == IntPatch_SPntPoleSeamU)
      {
        // set corresponding status: to be corrected later
        if (aLinOn2S->NbPoints() == 1)
          anIsFirstDegenerated = true;
        else
          anIsLastDegenerated = true;
      }

      const double aCurVertParam = aVtx.ParameterOnLine();
      if (aPrePointExist != IntPatch_SPntNone)
      {
        if (aNbPointsPrev == aLinOn2S->NbPoints())
        {
          // Vertex coincides any point of line and was not added into line
          aNewVertexParam = aNbPointsPrev;
        }
        aPrevParam = aParameter = aCurVertParam;
      }
      else
      {
        if (!isPointValid)
        {
          // Take a farther point of ALine (with greater parameter)
          continue;
        }

        if (aVtx.Tolerance() > aTol)
        {
          aVtx.SetValue(aPOn2S);
          AddPointIntoLine(aLinOn2S, anArrPeriods, aPOn2S);
        }
        else
        {
          AddVertexPoint(aLinOn2S, aVtx, anArrPeriods);
        }
      }

      aPrevLPoint = aPOn2S = aLinOn2S->Value(aLinOn2S->NbPoints());

      {
        bool                   isFound = false;
        const double           aSqTol  = aTol * aTol;
        const gp_Pnt           aP1(theALine->Value(aCurVertParam));
        const IntSurf_PntOn2S& aVertP2S   = aVtx.PntOn2S();
        const double           aVertToler = aVtx.Tolerance();

        for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
        {
          if (hasVertexBeenChecked(i))
            continue;

          const gp_Pnt aP2(theALine->Value(aVertexParams(i)));

          if (aP1.SquareDistance(aP2) < aSqTol)
          {
            IntPatch_Point aLVtx = theALine->Vertex(i);
            aLVtx.SetValue(aVertP2S);
            aLVtx.SetTolerance(aVertToler);
            double aParam = aLVtx.ParameterOnLine();
            if (std::abs(aParam - theLPar)
                <= Precision::PConfusion()) // in the case of closed curve,
              aLVtx.SetParameter(-1);       // we don't know yet the number of points in the curve
            else
              aLVtx.SetParameter(aNewVertexParam);
            aSeqVertex(++aNewVertID) = aLVtx;
            hasVertexBeenChecked(i)  = true;
            isFound                  = true;
          }
          else if (isFound)
          {
            break;
          }
        }
      }

      if ((aPrePointExist != IntPatch_SPntNone) && (aLinOn2S->NbPoints() > 1))
        break;

      if (isStepReduced)
      {
        isStepReduced = false;

        aStep = (theLPar - aParameter) / (double)(myNbPointsInWline - 1);
        if (aStep < Epsilon(theLPar))
          break;

        aLPar = aVertexParams(aNbVert);
        for (int i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
        {
          if (hasVertexBeenChecked(i))
            continue;

          aLPar = aVertexParams(i);
          if (std::abs(aLPar - aParameter) < aPrmTol)
            continue;

          break;
        }

        if ((aStep - (aLPar - aParameter) > aPrmTol) && (std::abs(aLPar - aParameter) > aPrmTol))
        {
          aStep         = std::max((aLPar - aParameter) / 5, 1.e-5);
          isStepReduced = true;
        }

        aStepMin = 0.1 * aStep;
        aStepMax = 10.0 * aStep;
      }
    } // for(; !isLast; aParameter += aStep)

    if (aLinOn2S->NbPoints() < 2)
    {
      aParameter += aStep;
      continue;
    }

    // Correct first and last points if needed
    if (aLinOn2S->NbPoints() >= 3)
    {
      if (anIsFirstDegenerated)
        CorrectEndPoint(aLinOn2S, 1);
      if (anIsLastDegenerated)
        CorrectEndPoint(aLinOn2S, aLinOn2S->NbPoints());
    }

    //-----------------------------------------------------------------
    //--              W  L  i  n  e     c  r  e  a  t  i  o  n      ---
    //-----------------------------------------------------------------
    occ::handle<IntPatch_WLine> aWLine;
    //
    if (theALine->TransitionOnS1() == IntSurf_Touch)
    {
      aWLine = new IntPatch_WLine(aLinOn2S,
                                  theALine->IsTangent(),
                                  theALine->SituationS1(),
                                  theALine->SituationS2());
      aWLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLImpImp);
    }
    else if (theALine->TransitionOnS1() == IntSurf_Undecided)
    {
      aWLine = new IntPatch_WLine(aLinOn2S, theALine->IsTangent());
      aWLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLImpImp);
    }
    else
    {
      // Computation of transitions of the line on two surfaces    ---
      const int     indice1 = std::max(aLinOn2S->NbPoints() / 3, 2);
      const gp_Pnt &aPP0    = aLinOn2S->Value(indice1 - 1).Value(),
                   &aPP1    = aLinOn2S->Value(indice1).Value();
      const gp_Vec tgvalid(aPP0, aPP1);
      const gp_Vec aNQ1(myQuad1.Normale(aPP0)), aNQ2(myQuad2.Normale(aPP0));

      const double dotcross = tgvalid.DotCross(aNQ2, aNQ1);

      IntSurf_TypeTrans trans1 = IntSurf_Undecided, trans2 = IntSurf_Undecided;

      if (dotcross > myTolTransition)
      {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if (dotcross < -myTolTransition)
      {
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      }

      aWLine = new IntPatch_WLine(aLinOn2S, theALine->IsTangent(), trans1, trans2);
      aWLine->SetCreatingWayInfo(IntPatch_WLine::IntPatch_WLImpImp);
    }

    for (int i = aSeqVertex.Lower(); i <= aNewVertID; i++)
    {
      IntPatch_Point aVtx = aSeqVertex(i);
      if (aVtx.ParameterOnLine() == -1)      // in the case of closed curve,
        aVtx.SetParameter(aWLine->NbPnts()); // we set the last parameter
      aWLine->AddVertex(aVtx);
    }

    aWLine->SetPeriod(anArrPeriods[0], anArrPeriods[1], anArrPeriods[2], anArrPeriods[3]);

    // the method ComputeVertexParameters can reduce the number of points in <aWLine>
    aWLine->ComputeVertexParameters(myTol3D);

    if (aWLine->NbPnts() > 1)
    {
      aWLine->EnablePurging(false);
#ifdef INTPATCH_ALINETOWLINE_DEBUG
      aWLine->Dump(0);
#endif
      theLines.Append(aWLine);
    }
  } // while(aParameter < theLPar)
}

//=======================================================================
// function : CheckDeflection
// purpose  : Returns:
//            -1 - step is too small
//            0  - step is normal
//            +1 - step is too big
//=======================================================================
int IntPatch_ALineToWLine::CheckDeflection(const gp_XYZ& theMidPt,
                                           const double  theMaxDeflection) const
{
  double aDist = std::abs(myQuad1.Distance(theMidPt));
  if (aDist > theMaxDeflection)
    return 1;

  aDist = std::max(std::abs(myQuad2.Distance(theMidPt)), aDist);

  if (aDist > theMaxDeflection)
    return 1;

  if ((aDist + aDist) < theMaxDeflection)
    return -1;

  return 0;
}

//=================================================================================================

bool IntPatch_ALineToWLine::StepComputing(const occ::handle<IntPatch_ALine>& theALine,
                                          const IntSurf_PntOn2S&             thePOn2S,
                                          const double                       theLastParOfAline,
                                          const double                       theCurParam,
                                          const double                       theTgMagnitude,
                                          const double                       theStepMin,
                                          const double                       theStepMax,
                                          const double                       theMaxDeflection,
                                          double&                            theStep) const
{
  if (theTgMagnitude < Precision::Confusion())
    return false;

  const double anEps = myTol3D;

  // Indeed, 1.0e+15 < 2^50 < 1.0e+16. Therefore,
  // if we apply bisection method to the range with length
  // 1.0e+6 then we will be able to find solution with max error ~1.0e-9.
  const int aNbIterMax = 50;

  const double aNotFilledRange = theLastParOfAline - theCurParam;
  double       aMinStep = theStepMin, aMaxStep = std::min(theStepMax, aNotFilledRange);

  if (aMinStep > aMaxStep)
  {
    theStep = aMaxStep;
    return true;
  }

  const double aR = IntPatch_PointLine::CurvatureRadiusOfIntersLine(myS1, myS2, thePOn2S);

#if 0
  {
    static int zzz = 0;
    zzz++;
    std::cout << "*** R" << zzz << " (begin)" << std::endl;
    double aU1, aV1, aU2, aV2;
    thePOn2S.Parameters(aU1, aV1, aU2, aV2);
    std::cout << "Prms: " << aU1 << ", " << aV1 << ", " << aU2 << ", " << aV2 << std::endl;
    std::cout << "Radius = " << aR << std::endl;
    std::cout << "*** R" << zzz << " (end)" << std::endl;
  }
#endif

  if (aR < 0.0)
  {
    return false;
  }
  else
  {
    // The 3D-step is defined as length of the tangent to the osculating circle
    // by the condition that the distance from end point of the tangent to the
    // circle is no greater than anEps. theStep is the step in
    // parameter space of intersection curve (must be converted from 3D-step).

    theStep = std::min(sqrt(anEps * (2.0 * aR + anEps)) / theTgMagnitude, aMaxStep);
    theStep = std::max(theStep, aMinStep);
  }

  // The step value has been computed for osculating circle.
  // Now it should be checked for real intersection curve
  // and is made more precise in case of necessity.

  int aNbIter = 0;
  do
  {
    aNbIter++;

    const gp_XYZ& aP1 = thePOn2S.Value().XYZ();
    const gp_XYZ  aP2(theALine->Value(theCurParam + theStep).XYZ());
    const int     aStatus = CheckDeflection(0.5 * (aP1 + aP2), theMaxDeflection);

    if (aStatus == 0)
      break;

    if (aStatus < 0)
    {
      aMinStep = theStep;
    }
    else // if(aStatus > 0)
    {
      aMaxStep = theStep;
    }

    theStep = 0.5 * (aMinStep + aMaxStep);
  } while (((aMaxStep - aMinStep) > Precision::PConfusion()) && (aNbIter <= aNbIterMax));

  if (aNbIter > aNbIterMax)
    return false;

  return true;
}
