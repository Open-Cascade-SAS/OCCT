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

#include <Adaptor3d_HSurface.hxx>
#include <ElSLib.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_Point.hxx>
#include <IntPatch_SpecialPoints.hxx>
#include <IntPatch_WLine.hxx>
#include <IntSurf.hxx>
#include <IntSurf_LineOn2S.hxx>

//=======================================================================
//function : AddPointIntoLine
//purpose  : 
//=======================================================================
static inline void AddPointIntoLine(Handle(IntSurf_LineOn2S) theLine,
                                    const Standard_Real* const theArrPeriods,
                                    IntSurf_PntOn2S &thePoint,
                                    IntPatch_Point* theVertex = 0)
{
   if(theLine->NbPoints() > 0)
   {
     if(thePoint.IsSame(theLine->Value(theLine->NbPoints()), Precision::Confusion()))
       return;

     IntPatch_SpecialPoints::AdjustPointAndVertex(theLine->Value(theLine->NbPoints()),
                                                    theArrPeriods, thePoint, theVertex);
   }

   theLine->Add(thePoint);
}

//=======================================================================
//function : AddVertexPoint
//purpose  : Extracts IntSurf_PntOn2S from theVertex and adds result in theLine.
//=======================================================================
static void AddVertexPoint(Handle(IntSurf_LineOn2S)& theLine,
                                       IntPatch_Point &theVertex,
                                       const Standard_Real* const theArrPeriods)
{
  IntSurf_PntOn2S anApexPoint = theVertex.PntOn2S();
  AddPointIntoLine(theLine, theArrPeriods, anApexPoint, &theVertex);
}

//=======================================================================
//function : IsPoleOrSeam
//purpose  : Processes theVertex depending on its type
//            (pole/apex/point on boundary etc.) and adds it in theLine.
//           theSingularSurfaceID contains the ID of surface with
//            special point (0 - none, 1 - theS1, 2 - theS2)
//=======================================================================
static IntPatch_SpecPntType IsPoleOrSeam(const Handle(Adaptor3d_HSurface)& theS1,
                                         const Handle(Adaptor3d_HSurface)& theS2,
                                         Handle(IntSurf_LineOn2S)& theLine,
                                         IntPatch_Point &theVertex,
                                         const Standard_Real* const theArrPeriods,
                                         const Standard_Real theTol3d,
                                         Standard_Integer& theSingularSurfaceID)
{
  const Standard_Integer aNbPnts = theLine->NbPoints();
  if(aNbPnts == 0)
    return IntPatch_SPntNone;

  theSingularSurfaceID = 0;

  for(Standard_Integer i = 0; i < 2; i++)
  {
    const Standard_Boolean isReversed = (i > 0);
    const GeomAbs_SurfaceType aType = isReversed? theS2->GetType() : theS1->GetType();

    IntPatch_SpecPntType anAddedPType = IntPatch_SPntNone;
    IntSurf_PntOn2S anApexPoint;

    switch(aType)
    {
    case GeomAbs_Sphere:
    case GeomAbs_Cone:
      {
        if(IntPatch_SpecialPoints::
              AddSingularPole((isReversed? theS2 : theS1), (isReversed? theS1 : theS2),
                               theLine->Value(aNbPnts), theTol3d, theVertex,
                               anApexPoint, isReversed, Standard_True))
        {
          anAddedPType = IntPatch_SPntPole;
          break;
        }
      }
      Standard_FALLTHROUGH
    case GeomAbs_Torus:
      if(aType == GeomAbs_Torus)
      {
        if(IntPatch_SpecialPoints::
            AddCrossUVIsoPoint((isReversed? theS2 : theS1), (isReversed? theS1 : theS2),
                                          theLine->Value(aNbPnts), theTol3d,
                                          anApexPoint, isReversed))
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

    if(anAddedPType != IntPatch_SPntNone)
    {
      theSingularSurfaceID = i + 1;
      AddPointIntoLine(theLine, theArrPeriods, anApexPoint, &theVertex);
      return anAddedPType;
    }
  }

  return IntPatch_SPntNone;
}

//=======================================================================
//function : IntPatch_ALineToWLine
//purpose  : 
//=======================================================================
IntPatch_ALineToWLine::IntPatch_ALineToWLine(const Handle(Adaptor3d_HSurface)& theS1,
                                             const Handle(Adaptor3d_HSurface)& theS2,
                                             const Standard_Integer theNbPoints) :
  myS1(theS1),
  myS2(theS2),
  myNbPointsInWline(theNbPoints),
  myTolOpenDomain(1.e-9),
  myTolTransition(1.e-8),
  myTol3D(Precision::Confusion())
{ 
  const GeomAbs_SurfaceType aTyps1 = theS1->GetType();
  const GeomAbs_SurfaceType aTyps2 = theS2->GetType();

  switch(aTyps1)
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

  switch(aTyps2)
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

//=======================================================================
//function : SetTol3D
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTol3D(const Standard_Real aTol)
{
  myTol3D = aTol;
}
//=======================================================================
//function : Tol3D
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::Tol3D()const
{
  return myTol3D;
}
//=======================================================================
//function : SetTolTransition
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTolTransition(const Standard_Real aTol)
{
  myTolTransition = aTol;
}
//=======================================================================
//function : TolTransition
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::TolTransition()const
{
  return myTolTransition;
}
//=======================================================================
//function : SetTolOpenDomain
//purpose  : 
//=======================================================================
  void IntPatch_ALineToWLine::SetTolOpenDomain(const Standard_Real aTol)
{
  myTolOpenDomain = aTol;
}
//=======================================================================
//function : TolOpenDomain
//purpose  : 
//=======================================================================
  Standard_Real IntPatch_ALineToWLine::TolOpenDomain()const
{
  return myTolOpenDomain;
}
//=======================================================================
//function : MakeWLine
//purpose  : 
//=======================================================================
void IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& theAline,
                                      IntPatch_SequenceOfLine& theLines) const
{ 
  Standard_Boolean included;
  Standard_Real f = theAline->FirstParameter(included); 
  if(!included) {
    f+=myTolOpenDomain;
  }
  Standard_Real l = theAline->LastParameter(included); 
  if(!included) { 
    l-=myTolOpenDomain;
  }

  MakeWLine(theAline, f, l, theLines);
}

//=======================================================================
//function : MakeWLine
//purpose  : 
//=======================================================================
void IntPatch_ALineToWLine::MakeWLine(const Handle(IntPatch_ALine)& theALine,
                                      const Standard_Real theFPar,
                                      const Standard_Real theLPar,
                                      IntPatch_SequenceOfLine& theLines) const 
{
  const Standard_Integer aNbVert = theALine->NbVertex();
  if (!aNbVert) {
    return;
  }
  const Standard_Real aTol = 2.0*myTol3D+Precision::Confusion();
  IntPatch_SpecPntType aPrePointExist = IntPatch_SPntNone;
  
  NCollection_Array1<Standard_Real> aVertexParams(1, aNbVert);
  NCollection_Array1<IntPatch_Point> aSeqVertex(1, aNbVert);

  //It is possible to have several vertices with equal parameters.
  NCollection_Array1<Standard_Boolean> hasVertexBeenChecked(1, aNbVert);

  Handle(IntSurf_LineOn2S) aLinOn2S;
  Standard_Real aParameter = theFPar;
  
  for(Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
  {
    const Standard_Real aPar = theALine->Vertex(i).ParameterOnLine();
    aVertexParams(i) = aPar;
    hasVertexBeenChecked(i) = Standard_False;
  }

  Standard_Integer aSingularSurfaceID = 0;
  Standard_Real anArrPeriods[] = { 0.0,  //U1
                                   0.0,  //V1
                                   0.0,  //U2
                                   0.0}; //V2

  IntSurf::SetPeriod(myS1, myS2, anArrPeriods);

  IntSurf_PntOn2S aPrevLPoint;

  while(aParameter < theLPar)
  {
    Standard_Real aStep = (theLPar - aParameter) / (Standard_Real)(myNbPointsInWline - 1);
    if(aStep < Epsilon(theLPar))
      break;

    Standard_Integer aNewVertID = 0;
    aLinOn2S = new IntSurf_LineOn2S;
    
    const Standard_Real aStepMin = 0.1*aStep, aStepMax = 10.0*aStep;

    Standard_Boolean isLast = Standard_False;
    Standard_Real aPrevParam = aParameter;
    for(; !isLast; aParameter += aStep)
    {
      IntSurf_PntOn2S aPOn2S;

      if(theLPar <= aParameter)
      {
        isLast = Standard_True;
        if(aPrePointExist != IntPatch_SPntNone)
        {
          break;
        }
        else
        {
          aParameter = theLPar;
        }
      }

      Standard_Real aTgMagn = 0.0;
      {
        gp_Pnt aPnt3d;
        gp_Vec aTg;
        theALine->D1(aParameter, aPnt3d, aTg);
        aTgMagn = aTg.Magnitude();
        Standard_Real u1 = 0.0, v1 = 0.0, u2 = 0.0, v2 = 0.0;
        myQuad1.Parameters(aPnt3d, u1, v1);
        myQuad2.Parameters(aPnt3d, u2, v2);
        aPOn2S.SetValue(aPnt3d, u1, v1, u2, v2);
      }

      if(aPrePointExist != IntPatch_SPntNone)
      {
        const Standard_Real aURes = Max(myS1->UResolution(myTol3D),
                                                myS2->UResolution(myTol3D)),
                            aVRes = Max(myS1->VResolution(myTol3D),
                                                myS2->VResolution(myTol3D));

        const Standard_Real aTol2d = (aPrePointExist == IntPatch_SPntPole) ? -1.0 : 
                    (aPrePointExist == IntPatch_SPntSeamV)? aVRes :
                    (aPrePointExist == IntPatch_SPntSeamUV)? Max(aURes, aVRes) : aURes;

        IntSurf_PntOn2S aRPT = aPOn2S;

        if (aPrePointExist == IntPatch_SPntPole)
        {
          Standard_Real aPrt = 0.5*(aPrevParam + theLPar);
          for (Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
          {
            const Standard_Real aParam = aVertexParams(i);

            if (aParam <= aPrevParam)
              continue;

            aPrt = 0.5*(aParam + aPrevParam);
            break;
          }

          const gp_Pnt aPnt3d(theALine->Value(aPrt));
          Standard_Real u1, v1, u2, v2;
          myQuad1.Parameters(aPnt3d, u1, v1);
          myQuad2.Parameters(aPnt3d, u2, v2);
          aRPT.SetValue(aPnt3d, u1, v1, u2, v2);

          if (aPOn2S.IsSame(aPrevLPoint, Max(Precision::Approximation(), aTol)))
          {
            //Set V-parameter as precise value found on the previous step.
            if (aSingularSurfaceID == 1)
            {
              aPOn2S.ParametersOnS1(u2, v2);
              aPOn2S.SetValue(Standard_True, u1, v2);
            }
            else //if (aSingularSurfaceID == 2)
            {
              aPOn2S.ParametersOnS2(u1, v1);
              aPOn2S.SetValue(Standard_False, u2, v1);
            }
          }
        }

        if(IntPatch_SpecialPoints::
                      ContinueAfterSpecialPoint(myS1, myS2, aRPT,
                                                aPrePointExist, aTol2d,
                                                aPrevLPoint, Standard_False))
        {
          AddPointIntoLine(aLinOn2S, anArrPeriods, aPrevLPoint);
        }
        else if(aParameter == theLPar)
        {// Strictly equal!!!
          break;
        }
      }

      aPrePointExist = IntPatch_SPntNone;

      Standard_Integer aVertexNumber = -1;
      for(Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
      {
        if(hasVertexBeenChecked(i))
          continue;

        const Standard_Real aParam = aVertexParams(i);
        if( ((aPrevParam < aParam) && (aParam <= aParameter)) ||
            ((aPrevParam == aParameter) && (aParam == aParameter)))
        {
          aVertexNumber = i;
          break;
        }
      }

      aPrevParam = aParameter;
      
      if(aVertexNumber < 0)
      {
        StepComputing(theALine, aPOn2S, theLPar, aParameter, aTgMagn,
                              aStepMin, aStepMax, myTol3D, aStep);
        AddPointIntoLine(aLinOn2S, anArrPeriods, aPOn2S);
        aPrevLPoint = aPOn2S;
        continue;
      }

      IntPatch_Point aVtx = theALine->Vertex(aVertexNumber);
      const Standard_Real aNewVertexParam = aLinOn2S->NbPoints() + 1;

      //ATTENTION!!!
      // IsPoleOrSeam inserts new point in aLinOn2S if aVtx respects
      //to some special point. Otherwise, aLinOn2S is not changed.

      aPrePointExist = IsPoleOrSeam(myS1, myS2, aLinOn2S, aVtx,
                                anArrPeriods, aTol, aSingularSurfaceID);

      const Standard_Real aCurVertParam = aVtx.ParameterOnLine();
      if(aPrePointExist != IntPatch_SPntNone)
      {
        aPrevParam = aParameter = aCurVertParam;
      }
      else
      {
        if(aVtx.Tolerance() > aTol)
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
        Standard_Boolean isFound = Standard_False;
        const Standard_Real aSqTol = aTol*aTol;
        const gp_Pnt aP1(theALine->Value(aCurVertParam));
        const IntSurf_PntOn2S& aVertP2S = aVtx.PntOn2S();
        const Standard_Real aVertToler  = aVtx.Tolerance();

        for(Standard_Integer i = aVertexParams.Lower(); i <= aVertexParams.Upper(); i++)
        {
          if(hasVertexBeenChecked(i))
            continue;

          const gp_Pnt aP2(theALine->Value(aVertexParams(i)));
          
          if(aP1.SquareDistance(aP2) < aSqTol)
          {
            IntPatch_Point aLVtx = theALine->Vertex(i);
            aLVtx.SetValue(aVertP2S);
            aLVtx.SetTolerance(aVertToler);
            aLVtx.SetParameter(aNewVertexParam);
            aSeqVertex(++aNewVertID) = aLVtx;
            hasVertexBeenChecked(i) = Standard_True;
            isFound = Standard_True;
          }
          else if(isFound)
          {
            break;
          }
        }
      }

      if(aPrePointExist != IntPatch_SPntNone)
        break;
    }//for(; !isLast; aParameter += aStep)

    if(aLinOn2S->NbPoints() < 2)
    {
      aParameter += aStep;
      continue;
    }

    //-----------------------------------------------------------------
    //--  Computation of transitions of the line on two surfaces    ---
    //-----------------------------------------------------------------
    IntSurf_TypeTrans trans1,trans2;
    {
      Standard_Integer indice1;
      Standard_Real dotcross;
      gp_Pnt aPP0, aPP1;
      //
      trans1=IntSurf_Undecided;
      trans2=IntSurf_Undecided;
      //
      indice1 = aLinOn2S->NbPoints()/3;
      if(indice1<=2) {
        indice1 = 2;
      }
      //
      aPP1=aLinOn2S->Value(indice1).Value();
      aPP0=aLinOn2S->Value(indice1-1).Value();
      //
      gp_Vec tgvalid(aPP0, aPP1);
      gp_Vec aNQ1 = myQuad1.Normale(aPP0);
      gp_Vec aNQ2 = myQuad2.Normale(aPP0);
      //
      dotcross = tgvalid.DotCross(aNQ2, aNQ1);
      if (dotcross > myTolTransition) {
        trans1 = IntSurf_Out;
        trans2 = IntSurf_In;
      }
      else if(dotcross < -myTolTransition) { 
        trans1 = IntSurf_In;
        trans2 = IntSurf_Out;
      } 
    }

    //-----------------------------------------------------------------
    //--              W  L  i  n  e     c  r  e  a  t  i  o  n      ---
    //-----------------------------------------------------------------
    Handle(IntPatch_WLine) aWLine;
    //
    if(theALine->TransitionOnS1() == IntSurf_Touch)  { 
                                  aWLine = new IntPatch_WLine(aLinOn2S,
                                  theALine->IsTangent(),
                                  theALine->SituationS1(),
                                  theALine->SituationS2());
    }
    else if(theALine->TransitionOnS1() == IntSurf_Undecided)  { 
      aWLine = new IntPatch_WLine(aLinOn2S, theALine->IsTangent());
    }
    else { 
      aWLine = new IntPatch_WLine(aLinOn2S, theALine->IsTangent(),
                                  trans1, // aline->TransitionOnS1(),
                                  trans2);  //aline->TransitionOnS2());
    }

    for(Standard_Integer i = aSeqVertex.Lower(); i <= aNewVertID; i++)
    {
      const IntPatch_Point& aVtx = aSeqVertex(i);
      aWLine->AddVertex(aVtx);
    }

    aWLine->SetPeriod(anArrPeriods[0],anArrPeriods[1],anArrPeriods[2],anArrPeriods[3]);

    //the method ComputeVertexParameters can reduce the number of points in <aWLine>
    aWLine->ComputeVertexParameters(myTol3D);

    if (aWLine->NbPnts() > 1)
    {
      aWLine->EnablePurging(Standard_False);
      theLines.Append(aWLine);
    }
  }//while(aParameter < theLPar)
}

//=======================================================================
//function : CheckDeflection
//purpose  : Returns:
//            -1 - step is too small
//            0  - step is normal
//            +1 - step is too big
//=======================================================================
Standard_Integer IntPatch_ALineToWLine::CheckDeflection(const gp_XYZ& theMidPt,
                                                        const Standard_Real theMaxDeflection) const
{
  Standard_Real aDist = Abs(myQuad1.Distance(theMidPt));
  if(aDist > theMaxDeflection)
    return 1;

  aDist = Max(Abs(myQuad2.Distance(theMidPt)), aDist);
  
  if(aDist > theMaxDeflection)
    return 1;

  if((aDist + aDist) < theMaxDeflection)
    return -1;

  return 0;
}

//=======================================================================
//function : StepComputing
//purpose  : 
//=======================================================================
Standard_Boolean IntPatch_ALineToWLine::
                      StepComputing(const Handle(IntPatch_ALine)& theALine,
                                    const IntSurf_PntOn2S& thePOn2S,
                                    const Standard_Real theLastParOfAline,
                                    const Standard_Real theCurParam,
                                    const Standard_Real theTgMagnitude,
                                    const Standard_Real theStepMin,
                                    const Standard_Real theStepMax,
                                    const Standard_Real theMaxDeflection,
                                    Standard_Real& theStep) const
{
  if(theTgMagnitude < Precision::Confusion())
    return Standard_False;
  
  const Standard_Real anEps = myTol3D;

  //Indeed, 1.0e+15 < 2^50 < 1.0e+16. Therefore,
  //if we apply bisection method to the range with length
  //1.0e+6 then we will be able to find solution with max error ~1.0e-9.
  const Standard_Integer aNbIterMax = 50;

  const Standard_Real aNotFilledRange = theLastParOfAline - theCurParam;
  Standard_Real aMinStep = theStepMin, aMaxStep = Min(theStepMax, aNotFilledRange);

  if(aMinStep > aMaxStep)
  {
    theStep = aMaxStep;
    return Standard_True;
  }

  const Standard_Real aR = IntPatch_PointLine::
                            CurvatureRadiusOfIntersLine(myS1, myS2, thePOn2S);

  if(aR < 0.0)
  {
    return Standard_False;
  }
  else
  {
    //The 3D-step is defined as length of the tangent to the osculating circle
    //by the condition that the distance from end point of the tangent to the
    //circle is no greater than anEps. theStep is the step in
    //parameter space of intersection curve (must be converted from 3D-step).

    theStep = Min(sqrt(anEps*(2.0*aR + anEps))/theTgMagnitude, aMaxStep);
    theStep = Max(theStep, aMinStep);
  }

  //The step value has been computed for osculating circle.
  //Now it should be checked for real intersection curve
  //and is made more precise in case of necessity.

  Standard_Integer aNbIter = 0;
  do
  {
    aNbIter++;

    const gp_XYZ& aP1 = thePOn2S.Value().XYZ();
    const gp_XYZ aP2(theALine->Value(theCurParam + theStep).XYZ());
    const Standard_Integer aStatus = CheckDeflection(0.5*(aP1 + aP2), theMaxDeflection);

    if(aStatus == 0)
      break;

    if(aStatus < 0)
    {
      aMinStep = theStep;
    }
    else //if(aStatus > 0)
    {
      aMaxStep = theStep;
    }

    theStep = 0.5*(aMinStep + aMaxStep);
  }
  while(((aMaxStep - aMinStep) > Precision::PConfusion()) && (aNbIter <= aNbIterMax));

  if(aNbIter > aNbIterMax)
    return Standard_False;

  return Standard_True;
}
