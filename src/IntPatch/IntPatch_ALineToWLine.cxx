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

static inline void AddPointIntoLine(Handle(IntSurf_LineOn2S)&  theLine,
                                    const Standard_Real* const theArrPeriods,
                                    IntSurf_PntOn2S&           thePoint,
                                    IntPatch_Point*            theVertex = 0)
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
static void AddVertexPoint(Handle(IntSurf_LineOn2S)&  theLine,
                           IntPatch_Point&            theVertex,
                           const Standard_Real* const theArrPeriods)
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
static IntPatch_SpecPntType IsPoleOrSeam(const Handle(Adaptor3d_Surface)& theS1,
                                         const Handle(Adaptor3d_Surface)& theS2,
                                         const IntSurf_PntOn2S&           thePIsoRef,
                                         Handle(IntSurf_LineOn2S)&        theLine,
                                         IntPatch_Point&                  theVertex,
                                         const Standard_Real              theArrPeriods[4],
                                         const Standard_Real              theTol3d,
                                         Standard_Integer&                theSingularSurfaceID)
{
  theSingularSurfaceID = 0;

  for (Standard_Integer i = 0; i < 2; i++)
  {
    const Standard_Boolean    isReversed = (i > 0);
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
                                                    Standard_True))
        {
          anAddedPType = IntPatch_SPntPole;
          break;
        }
      }
        Standard_FALLTHROUGH
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
        Standard_FALLTHROUGH
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

IntPatch_ALineToWLine::IntPatch_ALineToWLine(const Handle(Adaptor3d_Surface)& theS1,
                                             const Handle(Adaptor3d_Surface)& theS2,
                                             const Standard_Integer           theNbPoints)
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

void IntPatch_ALineToWLine::SetTol3D(const Standard_Real aTol)
{
  myTol3D = aTol;
}

//=================================================================================================

Standard_Real IntPatch_ALineToWLine::Tol3D() const
{
  return myTol3D;
}

//=================================================================================================

void IntPatch_ALineToWLine::SetTolTransition(const Standard_Real aTol)
{
  myTolTransition = aTol;
}

//=================================================================================================

Standard_Real IntPatch_ALineToWLine::TolTransition() const
{
  return myTolTransition;
}

//=================================================================================================

void IntPatch_ALineToWLine::SetTolOpenDomain(const Standard_Real aTol)
{
  myTolOpenDomain = aTol;
}

//=================================================================================================

Standard_Real IntPatch_ALineToWLine::TolOpenDomain() const
{
  return myTolOpenDomain;
}

//=================================================================================================

void IntPatch_ALineToWLine::CorrectEndPoint(Handle(IntSurf_LineOn2S)& theLine,
                                            const Standard_Integer    theIndex) const
{
  const Standard_Real aTol   = 1.e-5;
  const Standard_Real aSqTol = 1.e-10;

  // Perform linear extrapolation from two previous points
  Standard_Integer anIndFirst, anIndSecond;
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

  for (Standard_Integer ii = 1; ii <= 2; ii++)
  {
    Standard_Boolean anIsOnFirst = (ii == 1);

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
      Standard_Real aU, aV;
      aPntOn2S.ParametersOnSurface(anIsOnFirst, aU, aV);
      if (Abs(aV - M_PI / 2) > aTol && Abs(aV + M_PI / 2) > aTol)
        continue;
    }
    else
      continue;

    gp_Pnt2d      PrevPrevP2d = theLine->Value(anIndFirst).ValueOnSurface(anIsOnFirst);
    gp_Pnt2d      PrevP2d     = theLine->Value(anIndSecond).ValueOnSurface(anIsOnFirst);
    gp_Dir2d      aDir        = gp_Vec2d(PrevPrevP2d, PrevP2d);
    Standard_Real aX0 = PrevPrevP2d.X(), aY0 = PrevPrevP2d.Y();
    Standard_Real aXend, aYend;
    aPntOn2S.ParametersOnSurface(anIsOnFirst, aXend, aYend);

    if (Abs(aDir.Y()) < gp::Resolution())
      continue;

    Standard_Real aNewXend = aDir.X() / aDir.Y() * (aYend - aY0) + aX0;

    theLine->SetUV(theIndex, anIsOnFirst, aNewXend, aYend);
  }
}

//=================================================================================================

Standard_Real IntPatch_ALineToWLine::GetSectionRadius(const gp_Pnt& thePnt3d) const
{
  Standard_Real aRetVal = RealLast();
  for (Standard_Integer i = 0; i < 2; i++)
  {
    const IntSurf_Quadric& aQuad = i ? myQuad2 : myQuad1;
    if (aQuad.TypeQuadric() == GeomAbs_Cone)
    {
      const gp_Cone aCone = aQuad.Cone();
      const gp_XYZ  aRVec = thePnt3d.XYZ() - aCone.Apex().XYZ();
      const gp_XYZ& aDir  = aCone.Axis().Direction().XYZ();

      aRetVal = Min(aRetVal, Abs(aRVec.Dot(aDir) * Tan(aCone.SemiAngle())));
    }
    else if (aQuad.TypeQuadric() == GeomAbs_Sphere)
    {
      const gp_Sphere     aSphere = aQuad.Sphere();
      const gp_XYZ        aRVec   = thePnt3d.XYZ() - aSphere.Location().XYZ();
      const gp_XYZ&       aDir    = aSphere.Position().Direction().XYZ();
      const Standard_Real aR      = aSphere.Radius();
      const Standard_Real aD      = aRVec.Dot(aDir);
      const Standard_Real aDelta  = aR * aR - aD * aD;
      if (aDelta <= 0.0)
      {
        aRetVal = 0.0;
        break;
      }
      else
      {
        aRetVal = Min(aRetVal, Sqrt(aDelta));
      }
    }
  }

  return aRetVal;
}

//=================================================================================================

void IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& theAline,
                                      IntPatch_SequenceOfLine&      theLines) const
{
  Standard_Boolean included;
  Standard_Real    f = theAline->FirstParameter(included);
  if (!included)
  {
    f += myTolOpenDomain;
  }
  Standard_Real l = theAline->LastParameter(included);
  if (!included)
  {
    l -= myTolOpenDomain;
  }

  MakeWLine(theAline, f, l, theLines);
}

//=================================================================================================

void IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& theALine,
                                      const Standard_Real           theFPar,
                                      const Standard_Real           theLPar,
                                      IntPatch_SequenceOfLine&      theLines) const
{
  const Standard_Integer aNbVert = theALine->NbVertex();
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
      const Standard_Integer NbSamples = 20;
      const Standard_Real aStep = (theLPar - theFPar) / NbSamples;
      char* name = new char[100];
      
      for (Standard_Integer ii = 0; ii <= NbSamples; ii++)
      {
        Standard_Real aPrm = theFPar + ii * aStep;
        const gp_Pnt aPP(theALine->Value(aPrm));
        std::cout << "vertex v" << ii << " " << aPP.X() << " " << aPP.Y() << " " << aPP.Z() << std::endl;

        sprintf(name, "p%d_%d", ii, ind);
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
  for (Standard_Integer i = 1; i < aNbVert; i++)
  {
    IntPatch_Point&        aCurVert  = theALine->ChangeVertex(i);
    const IntSurf_PntOn2S& aCurrPt   = aCurVert.PntOn2S();
    const Standard_Real    aCurToler = aCurVert.Tolerance();
    for (Standard_Integer j = i + 1; j <= aNbVert; j++)
    {
      IntPatch_Point&        aVert  = theALine->ChangeVertex(j);
      const IntSurf_PntOn2S& aNewPt = aVert.PntOn2S();
      const Standard_Real    aToler = aVert.Tolerance();

      const Standard_Real aSumTol = aCurToler + aToler;
      if (aCurrPt.IsSame(aNewPt, aSumTol))
      {
        aCurVert.SetTolerance(aSumTol);
        aVert.SetTolerance(aSumTol);
      }
    }
  }

  const Standard_Real aTol    = 2.0 * myTol3D + Precision::Confusion();
  const Standard_Real aPrmTol = Max(1.0e-4 * (theLPar - theFPar), Precision::PConfusion());

  IntPatch_SpecPntType aPrePointExist = IntPatch_SPntNone;

  NCollection_Array1<Standard_Real>  aVertexParams(1, aNbVert);
  NCollection_Array1<IntPatch_Point> aSeqVertex(1, aNbVert);

  // It is possible to have several vertices with equal parameters.
  NCollection_Array1<Standard_Boolean> hasVertexBeenChecked(1, aNbVert);

  Handle(IntSurf_LineOn2S) aLinOn2S;
  Standard_Real            aParameter = theFPar;

  for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
  {
    const IntPatch_Point& aVert = theALine->Vertex(i);
    const Standard_Real   aPar  = aVert.ParameterOnLine();
    aVertexParams(i)            = aPar;
    hasVertexBeenChecked(i)     = Standard_False;
  }

  Standard_Integer aSingularSurfaceID = 0;
  Standard_Real    anArrPeriods[]     = {0.0,  // U1
                                         0.0,  // V1
                                         0.0,  // U2
                                         0.0}; // V2

  IntSurf::SetPeriod(myS1, myS2, anArrPeriods);

  IntSurf_PntOn2S aPrevLPoint;

  while (aParameter < theLPar)
  {
    Standard_Real aStep = (theLPar - aParameter) / (Standard_Real)(myNbPointsInWline - 1);
    if (aStep < Epsilon(theLPar))
      break;

    Standard_Boolean isStepReduced = Standard_False;
    Standard_Real    aLPar         = theLPar;

    for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
    {
      if (hasVertexBeenChecked(i))
        continue;

      aLPar = aVertexParams(i);
      if (Abs(aLPar - aParameter) < aPrmTol)
        continue;

      break;
    }

    if ((aStep - (aLPar - aParameter) > aPrmTol) && (Abs(aLPar - aParameter) > aPrmTol))
    {
      aStep         = Max((aLPar - aParameter) / 5, 1.e-5);
      isStepReduced = Standard_True;
    }

    Standard_Integer aNewVertID           = 0;
    aLinOn2S                              = new IntSurf_LineOn2S;
    Standard_Boolean anIsFirstDegenerated = Standard_False, anIsLastDegenerated = Standard_False;

    Standard_Real aStepMin = 0.1 * aStep, aStepMax = 10.0 * aStep;

    Standard_Boolean isLast     = Standard_False;
    Standard_Real    aPrevParam = aParameter;
    for (; !isLast; aParameter += aStep)
    {
      IntSurf_PntOn2S aPOn2S;

      if (theLPar <= aParameter)
      {
        isLast = Standard_True;
        if (aPrePointExist != IntPatch_SPntNone)
        {
          break;
        }
        else
        {
          aParameter = theLPar;
        }
      }

      Standard_Boolean isPointValid = Standard_False;
      Standard_Real    aTgMagn      = 0.0;
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

          isPointValid = Standard_False;
        }
        else
        {
          isPointValid = Standard_True;
        }

        aTgMagn          = aTg.Magnitude();
        Standard_Real u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;
        myQuad1.Parameters(aPnt3d, u1, v1);
        myQuad2.Parameters(aPnt3d, u2, v2);
        aPOn2S.SetValue(aPnt3d, u1, v1, u2, v2);
      }

      if (aPrePointExist != IntPatch_SPntNone)
      {
        const Standard_Real aURes = Max(myS1->UResolution(myTol3D), myS2->UResolution(myTol3D)),
                            aVRes = Max(myS1->VResolution(myTol3D), myS2->VResolution(myTol3D));

        const Standard_Real aTol2d = (aPrePointExist == IntPatch_SPntPole)     ? -1.0
                                     : (aPrePointExist == IntPatch_SPntSeamV)  ? aVRes
                                     : (aPrePointExist == IntPatch_SPntSeamUV) ? Max(aURes, aVRes)
                                                                               : aURes;

        IntSurf_PntOn2S aRPT = aPOn2S;

        if (aPrePointExist == IntPatch_SPntPole)
        {
          Standard_Real aPrt = 0.5 * (aPrevParam + theLPar);
          for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
          {
            const Standard_Real aParam = aVertexParams(i);

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

          const gp_Pnt  aPnt3d(theALine->Value(aPrt));
          Standard_Real u1, v1, u2, v2;
          myQuad1.Parameters(aPnt3d, u1, v1);
          myQuad2.Parameters(aPnt3d, u2, v2);
          aRPT.SetValue(aPnt3d, u1, v1, u2, v2);

          if (aPOn2S.IsSame(aPrevLPoint, Max(Precision::Approximation(), aTol)))
          {
            // Set V-parameter as precise value found on the previous step.
            if (aSingularSurfaceID == 1)
            {
              aPOn2S.ParametersOnS1(u2, v2);
              aPOn2S.SetValue(Standard_True, u1, v2);
            }
            else // if (aSingularSurfaceID == 2)
            {
              aPOn2S.ParametersOnS2(u1, v1);
              aPOn2S.SetValue(Standard_False, u2, v1);
            }
          }
        }

        if (IntPatch_SpecialPoints::ContinueAfterSpecialPoint(myS1,
                                                              myS2,
                                                              aRPT,
                                                              aPrePointExist,
                                                              aTol2d,
                                                              aPrevLPoint,
                                                              Standard_False))
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
          Standard_Real anU1 = 0.0, aV1 = 0.0, anU2 = 0.0, aV2 = 0.0;
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

      Standard_Integer aVertexNumber = -1;
      for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
      {
        if (hasVertexBeenChecked(i))
          continue;

        const IntPatch_Point& aVP    = theALine->Vertex(i);
        const Standard_Real   aParam = aVertexParams(i);
        if (((aPrevParam < aParam) && (aParam <= aParameter))
            || ((aPrevParam == aParameter) && (aParam == aParameter))
            || (aPOn2S.IsSame(aVP.PntOn2S(), aVP.Tolerance())
                && (Abs(aVP.ParameterOnLine() - aParameter) < aPrmTol)))
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
          // add point, set correxponding status: to be corrected later
          Standard_Boolean ToAdd = Standard_False;
          if (aLinOn2S->NbPoints() == 0)
          {
            anIsFirstDegenerated = Standard_True;
            ToAdd                = Standard_True;
          }
          else if (aLinOn2S->NbPoints() > 1)
          {
            anIsLastDegenerated = Standard_True;
            ToAdd               = Standard_True;
          }

          if (ToAdd)
          {
            AddPointIntoLine(aLinOn2S, anArrPeriods, aPOn2S);
            aPrevLPoint = aPOn2S;
          }
        }

        continue;
      }

      IntPatch_Point   aVtx            = theALine->Vertex(aVertexNumber);
      Standard_Real    aNewVertexParam = aLinOn2S->NbPoints() + 1;
      Standard_Integer aNbPointsPrev   = aLinOn2S->NbPoints();

      // ATTENTION!!!
      //  IsPoleOrSeam inserts new point in aLinOn2S if aVtx respects
      // to some special point. Otherwise, aLinOn2S is not changed.

      // Find a point for reference parameter. It will be used
      // if real parameter value cannot be precise (see comment to
      // IsPoleOrSeam(...) function).
      IntSurf_PntOn2S aPrefIso = aVtx.PntOn2S();
      if (aLinOn2S->NbPoints() < 1)
      {
        for (Standard_Integer i = aVertexNumber + 1; i <= aVertexParams.Upper(); i++)
        {
          const Standard_Real aParam = aVertexParams(i);
          if ((aParam - aVertexParams(aVertexNumber)) > Precision::PConfusion())
          {
            const Standard_Real aPrm = 0.5 * (aParam + aVertexParams(aVertexNumber));
            const gp_Pnt        aPnt3d(theALine->Value(aPrm));
            Standard_Real       u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;
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
        // set correxponding status: to be corrected later
        if (aLinOn2S->NbPoints() == 1)
          anIsFirstDegenerated = Standard_True;
        else
          anIsLastDegenerated = Standard_True;
      }

      const Standard_Real aCurVertParam = aVtx.ParameterOnLine();
      if (aPrePointExist != IntPatch_SPntNone)
      {
        if (aNbPointsPrev == aLinOn2S->NbPoints())
        {
          // Vertex coinsides any point of line and was not added into line
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
        Standard_Boolean       isFound = Standard_False;
        const Standard_Real    aSqTol  = aTol * aTol;
        const gp_Pnt           aP1(theALine->Value(aCurVertParam));
        const IntSurf_PntOn2S& aVertP2S   = aVtx.PntOn2S();
        const Standard_Real    aVertToler = aVtx.Tolerance();

        for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
        {
          if (hasVertexBeenChecked(i))
            continue;

          const gp_Pnt aP2(theALine->Value(aVertexParams(i)));

          if (aP1.SquareDistance(aP2) < aSqTol)
          {
            IntPatch_Point aLVtx = theALine->Vertex(i);
            aLVtx.SetValue(aVertP2S);
            aLVtx.SetTolerance(aVertToler);
            Standard_Real aParam = aLVtx.ParameterOnLine();
            if (Abs(aParam - theLPar) <= Precision::PConfusion()) // in the case of closed curve,
              aLVtx.SetParameter(-1); // we don't know yet the number of points in the curve
            else
              aLVtx.SetParameter(aNewVertexParam);
            aSeqVertex(++aNewVertID) = aLVtx;
            hasVertexBeenChecked(i)  = Standard_True;
            isFound                  = Standard_True;
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
        isStepReduced = Standard_False;

        aStep = (theLPar - aParameter) / (Standard_Real)(myNbPointsInWline - 1);
        if (aStep < Epsilon(theLPar))
          break;

        aLPar = aVertexParams(aNbVert);
        for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
        {
          if (hasVertexBeenChecked(i))
            continue;

          aLPar = aVertexParams(i);
          if (Abs(aLPar - aParameter) < aPrmTol)
            continue;

          break;
        }

        if ((aStep - (aLPar - aParameter) > aPrmTol) && (Abs(aLPar - aParameter) > aPrmTol))
        {
          aStep         = Max((aLPar - aParameter) / 5, 1.e-5);
          isStepReduced = Standard_True;
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
    Handle(IntPatch_WLine) aWLine;
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
      const Standard_Integer indice1 = Max(aLinOn2S->NbPoints() / 3, 2);
      const gp_Pnt &         aPP0    = aLinOn2S->Value(indice1 - 1).Value(),
                   &aPP1             = aLinOn2S->Value(indice1).Value();
      const gp_Vec tgvalid(aPP0, aPP1);
      const gp_Vec aNQ1(myQuad1.Normale(aPP0)), aNQ2(myQuad2.Normale(aPP0));

      const Standard_Real dotcross = tgvalid.DotCross(aNQ2, aNQ1);

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

    for (Standard_Integer i = aSeqVertex.Lower(); i <= aNewVertID; i++)
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
      aWLine->EnablePurging(Standard_False);
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
Standard_Integer IntPatch_ALineToWLine::CheckDeflection(const gp_XYZ&       theMidPt,
                                                        const Standard_Real theMaxDeflection) const
{
  Standard_Real aDist = Abs(myQuad1.Distance(theMidPt));
  if (aDist > theMaxDeflection)
    return 1;

  aDist = Max(Abs(myQuad2.Distance(theMidPt)), aDist);

  if (aDist > theMaxDeflection)
    return 1;

  if ((aDist + aDist) < theMaxDeflection)
    return -1;

  return 0;
}

//=================================================================================================

Standard_Boolean IntPatch_ALineToWLine::StepComputing(const Handle(IntPatch_ALine)& theALine,
                                                      const IntSurf_PntOn2S&        thePOn2S,
                                                      const Standard_Real theLastParOfAline,
                                                      const Standard_Real theCurParam,
                                                      const Standard_Real theTgMagnitude,
                                                      const Standard_Real theStepMin,
                                                      const Standard_Real theStepMax,
                                                      const Standard_Real theMaxDeflection,
                                                      Standard_Real&      theStep) const
{
  if (theTgMagnitude < Precision::Confusion())
    return Standard_False;

  const Standard_Real anEps = myTol3D;

  // Indeed, 1.0e+15 < 2^50 < 1.0e+16. Therefore,
  // if we apply bisection method to the range with length
  // 1.0e+6 then we will be able to find solution with max error ~1.0e-9.
  const Standard_Integer aNbIterMax = 50;

  const Standard_Real aNotFilledRange = theLastParOfAline - theCurParam;
  Standard_Real       aMinStep = theStepMin, aMaxStep = Min(theStepMax, aNotFilledRange);

  if (aMinStep > aMaxStep)
  {
    theStep = aMaxStep;
    return Standard_True;
  }

  const Standard_Real aR = IntPatch_PointLine::CurvatureRadiusOfIntersLine(myS1, myS2, thePOn2S);

#if 0
  {
    static int zzz = 0;
    zzz++;
    std::cout << "*** R" << zzz << " (begin)" << std::endl;
    Standard_Real aU1, aV1, aU2, aV2;
    thePOn2S.Parameters(aU1, aV1, aU2, aV2);
    std::cout << "Prms: " << aU1 << ", " << aV1 << ", " << aU2 << ", " << aV2 << std::endl;
    std::cout << "Radius = " << aR << std::endl;
    std::cout << "*** R" << zzz << " (end)" << std::endl;
  }
#endif

  if (aR < 0.0)
  {
    return Standard_False;
  }
  else
  {
    // The 3D-step is defined as length of the tangent to the osculating circle
    // by the condition that the distance from end point of the tangent to the
    // circle is no greater than anEps. theStep is the step in
    // parameter space of intersection curve (must be converted from 3D-step).

    theStep = Min(sqrt(anEps * (2.0 * aR + anEps)) / theTgMagnitude, aMaxStep);
    theStep = Max(theStep, aMinStep);
  }

  // The step value has been computed for osculating circle.
  // Now it should be checked for real intersection curve
  // and is made more precise in case of necessity.

  Standard_Integer aNbIter = 0;
  do
  {
    aNbIter++;

    const gp_XYZ&          aP1 = thePOn2S.Value().XYZ();
    const gp_XYZ           aP2(theALine->Value(theCurParam + theStep).XYZ());
    const Standard_Integer aStatus = CheckDeflection(0.5 * (aP1 + aP2), theMaxDeflection);

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
    return Standard_False;

  return Standard_True;
}
