// Created on: 1995-02-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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

#include <vector>
#include <GeomInt_LineTool.hxx>

#include <Extrema_ExtPS.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_List.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_StdAllocator.hxx>
#include <TColStd_Array1OfListOfInteger.hxx>

class ProjectPointOnSurf
{
 public:
  ProjectPointOnSurf() : myIsDone (Standard_False),myIndex(0) {}
  void Init(const Handle(Geom_Surface)& Surface,
	    const Standard_Real Umin,
	    const Standard_Real Usup,
	    const Standard_Real Vmin,
	    const Standard_Real Vsup);
  void Init ();
  void Perform(const gp_Pnt& P);
  Standard_Boolean IsDone () const { return myIsDone; }
  void LowerDistanceParameters(Standard_Real& U, Standard_Real& V ) const;
  Standard_Real LowerDistance() const;
 protected:
  Standard_Boolean myIsDone;
  Standard_Integer myIndex;
  Extrema_ExtPS myExtPS;
  GeomAdaptor_Surface myGeomAdaptor;
};

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::Init ( const Handle(Geom_Surface)& Surface,
                                const Standard_Real Umin,
                                const Standard_Real Usup,
                                const Standard_Real Vmin,
                                const Standard_Real Vsup )
{
  const Standard_Real Tolerance = Precision::PConfusion();
  //
  myGeomAdaptor.Load(Surface, Umin,Usup,Vmin,Vsup);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myIsDone = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::Init ()
{
  myIsDone = myExtPS.IsDone() && (myExtPS.NbExt() > 0);
  if (myIsDone) {
    // evaluate the lower distance and its index;
    Standard_Real Dist2Min = myExtPS.SquareDistance(1);
    myIndex = 1;
    for (Standard_Integer i = 2; i <= myExtPS.NbExt(); i++)
    {
      const Standard_Real Dist2 = myExtPS.SquareDistance(i);
      if (Dist2 < Dist2Min) {
        Dist2Min = Dist2;
        myIndex = i;
      }
    }
  }
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::Perform(const gp_Pnt& P)
{
  myExtPS.Perform(P);
  Init ();
}

//=======================================================================
//function : LowerDistanceParameters
//purpose  : 
//=======================================================================
void ProjectPointOnSurf::LowerDistanceParameters (Standard_Real& U,
                                                  Standard_Real& V ) const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistanceParameters");
  (myExtPS.Point(myIndex)).Parameter(U,V);
}

//=======================================================================
//function : LowerDistance
//purpose  : 
//=======================================================================
Standard_Real ProjectPointOnSurf::LowerDistance() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistance");
  return sqrt(myExtPS.SquareDistance(myIndex));
}

//=======================================================================
//function : AdjustPeriodic
//purpose  : 
//=======================================================================
static Standard_Real AdjustPeriodic(const Standard_Real theParameter,
                                    const Standard_Real parmin,
                                    const Standard_Real parmax,
                                    const Standard_Real thePeriod,
                                    Standard_Real&      theOffset)
{
  Standard_Real aresult = theParameter;
  theOffset = 0.;
  while(aresult < parmin) {
    aresult += thePeriod;
    theOffset += thePeriod;
  }
  while(aresult > parmax) {
    aresult -= thePeriod;
    theOffset -= thePeriod;
  }
  return aresult;
}

//=======================================================================
//function : IsPointOnBoundary
//purpose  : 
//=======================================================================
static Standard_Boolean IsPointOnBoundary(const Standard_Real theParameter,
                                          const Standard_Real theFirstBoundary,
                                          const Standard_Real theSecondBoundary,
                                          const Standard_Real theResolution,
                                          Standard_Boolean&   IsOnFirstBoundary)
{
  IsOnFirstBoundary = Standard_True;
  if(fabs(theParameter - theFirstBoundary) < theResolution)
    return Standard_True;
  if(fabs(theParameter - theSecondBoundary) < theResolution)
  {
    IsOnFirstBoundary = Standard_False;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : FindPoint
//purpose  : 
//=======================================================================
static Standard_Boolean FindPoint(const gp_Pnt2d&     theFirstPoint,
                                  const gp_Pnt2d&     theLastPoint,
                                  const Standard_Real theUmin,
                                  const Standard_Real theUmax,
                                  const Standard_Real theVmin,
                                  const Standard_Real theVmax,
                                  gp_Pnt2d&           theNewPoint)
{
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  Standard_Integer i = 0, j = 0;

  for(i = 0; i < 4; i++) {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;    

    if((i % 2) == 0) {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if(i < 2)
	aprojpoint.SetX(theUmin);
      else
	aprojpoint.SetX(theUmax);
    }
    else {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if(i < 2)
	aprojpoint.SetY(theVmin);
      else
	aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    Standard_Real adot1 = anormvec.Dot(anOtherVecNormal);

    if(fabs(adot1) < Precision::Angular())
      continue;
    Standard_Real adist = 0.;

    if((i % 2) == 0) {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
    }
    else {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
    }
    Standard_Real anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for(j = 0; j < 2; j++) {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY()*anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);

      //
      Standard_Real aDotX, anAngleX, aPC;
      //
      aDotX=aVec.Dot(acurvec);
      anAngleX=aVec.Angle(acurvec);
      aPC=Precision::PConfusion();
      //
      if(aDotX > 0. && fabs(anAngleX) < aPC) {
      //
	if((i % 2) == 0) {
	  if((acurpoint.Y() >= theVmin) &&
	     (acurpoint.Y() <= theVmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
	else {
	  if((acurpoint.X() >= theUmin) &&
	     (acurpoint.X() <= theUmax)) {
	    theNewPoint = acurpoint;
	    return Standard_True;
	  }
	}
      }
    }
  }
  return Standard_False;
}


//=======================================================================
//function : NbVertex
//purpose  : 
//=======================================================================
Standard_Integer GeomInt_LineTool::NbVertex(const Handle(IntPatch_Line)& L)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic:    return Handle(IntPatch_ALine)::DownCast(L)->NbVertex();
    case IntPatch_Restriction: return Handle(IntPatch_RLine)::DownCast(L)->NbVertex();
    case IntPatch_Walking:     return Handle(IntPatch_WLine)::DownCast(L)->NbVertex();
    default: break;
  }
  return Handle(IntPatch_GLine)::DownCast(L)->NbVertex();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================
const IntPatch_Point & GeomInt_LineTool::Vertex(const Handle(IntPatch_Line)& L,
                                                const Standard_Integer I)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic: return Handle(IntPatch_ALine)::DownCast(L)->Vertex(I);
    case IntPatch_Restriction: return Handle(IntPatch_RLine)::DownCast(L)->Vertex(I);
    case IntPatch_Walking: return Handle(IntPatch_WLine)::DownCast(L)->Vertex(I);
    default: break;
  }
  return Handle(IntPatch_GLine)::DownCast(L)->Vertex(I);
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================
Standard_Real GeomInt_LineTool::FirstParameter (const Handle(IntPatch_Line)& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin = Handle(IntPatch_ALine)::DownCast(L);
      if (alin->HasFirstPoint())
        return alin->FirstPoint().ParameterOnLine();
      Standard_Boolean included;
      Standard_Real firstp = alin->FirstParameter(included);
      if (!included)
        firstp += Epsilon(firstp);
      return firstp;
    }

    case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin = Handle(IntPatch_RLine)::DownCast(L);
	  return (rlin->HasFirstPoint()? rlin->FirstPoint().ParameterOnLine() : -Precision::Infinite()); // a voir selon le type de la ligne 2d
    }

    case IntPatch_Walking:
    {
      Handle(IntPatch_WLine) wlin = Handle(IntPatch_WLine)::DownCast(L);
	  return (wlin->HasFirstPoint()? wlin->FirstPoint().ParameterOnLine() : 1.);
    }

    default:
    {
      Handle(IntPatch_GLine) glin = Handle(IntPatch_GLine)::DownCast(L);
      if (glin->HasFirstPoint())
        return glin->FirstPoint().ParameterOnLine();
      switch (typl)
      {
        case IntPatch_Lin:
        case IntPatch_Parabola:
        case IntPatch_Hyperbola:
          return -Precision::Infinite();
        default: break;
      }
    }
  }
  return 0.0;
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================
Standard_Real GeomInt_LineTool::LastParameter (const Handle(IntPatch_Line)& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic:
    {
      Handle(IntPatch_ALine) alin = Handle(IntPatch_ALine)::DownCast(L);
      if (alin->HasLastPoint())
        return alin->LastPoint().ParameterOnLine();
      Standard_Boolean included;
      Standard_Real lastp = alin->LastParameter(included);
      if (!included)
        lastp -=Epsilon(lastp);
      return lastp;
    }

    case IntPatch_Restriction:
    {
      Handle(IntPatch_RLine) rlin = Handle(IntPatch_RLine)::DownCast(L);
	  return (rlin->HasLastPoint()? rlin->LastPoint().ParameterOnLine() : Precision::Infinite()); // a voir selon le type de la ligne 2d
    }

    case IntPatch_Walking:
    {
      Handle(IntPatch_WLine) wlin = Handle(IntPatch_WLine)::DownCast(L);
	  return (wlin->HasLastPoint()? wlin->LastPoint().ParameterOnLine() : wlin->NbPnts());
    }

    default:
    {
      Handle(IntPatch_GLine) glin = Handle(IntPatch_GLine)::DownCast(L);
      if (glin->HasLastPoint())
        return glin->LastPoint().ParameterOnLine();
      switch (typl)
      {
        case IntPatch_Lin:
        case IntPatch_Parabola:
        case IntPatch_Hyperbola:
          return Precision::Infinite();
        case IntPatch_Circle:
        case IntPatch_Ellipse:
          return 2.*M_PI;
        default: break;
      }
    }
  }
  return 0.0;
}

//=======================================================================
//function : DecompositionOfWLine
//purpose  : 
//=======================================================================
Standard_Boolean GeomInt_LineTool::
            DecompositionOfWLine( const Handle(IntPatch_WLine)& theWLine,
                                  const Handle(GeomAdaptor_HSurface)& theSurface1,
                                  const Handle(GeomAdaptor_HSurface)& theSurface2,
                                  const Standard_Real aTolSum,
                                  const GeomInt_LineConstructor& theLConstructor,
                                  IntPatch_SequenceOfLine& theNewLines)
{
  typedef NCollection_List<Standard_Integer> ListOfInteger;
  //have to use std::vector, not NCollection_Vector in order to use copy constructor of
  //ListOfInteger which will be created with specific allocator instance
  typedef std::vector<ListOfInteger, NCollection_StdAllocator<
      ListOfInteger> > ArrayOfListOfInteger;

  Standard_Boolean bIsPrevPointOnBoundary, bIsCurrentPointOnBoundary;
  Standard_Integer nblines, aNbPnts, aNbParts, pit, i, j, aNbListOfPointIndex;
  Standard_Real aTol, umin, umax, vmin, vmax;

  //an inc allocator, it will contain wasted space (upon list's Clear()) but it should
  //still be faster than the standard allocator, and wasted memory should not be
  //significant and will be limited by time span of this function;
  //this is a separate allocator from the anIncAlloc below what provides better data
  //locality in the latter (by avoiding wastes which will only be in anIdxAlloc)
  Handle(NCollection_IncAllocator) anIdxAlloc = new NCollection_IncAllocator();
  ListOfInteger aListOfPointIndex (anIdxAlloc);

  //GeomAPI_ProjectPointOnSurf aPrj1, aPrj2;
  ProjectPointOnSurf aPrj1, aPrj2;
  Handle(Geom_Surface) aSurf1,  aSurf2;
  //
  aNbParts=theLConstructor.NbParts();
  aNbPnts=theWLine->NbPnts();
  //
  if((!aNbPnts) || (!aNbParts)){
    return Standard_False;
  }
  //
  Handle(NCollection_IncAllocator) anIncAlloc = new NCollection_IncAllocator();
  NCollection_StdAllocator<ListOfInteger> anAlloc (anIncAlloc);
  const ListOfInteger aDummy (anIncAlloc); //empty list to be copy constructed from
  ArrayOfListOfInteger anArrayOfLines (aNbPnts + 1, aDummy, anAlloc);

  NCollection_LocalArray<Standard_Integer> anArrayOfLineTypeArr (aNbPnts + 1);
  Standard_Integer* anArrayOfLineType = anArrayOfLineTypeArr;
  //
  nblines = 0;
  aTol = Precision::Confusion();
  //
  aSurf1 = theSurface1->ChangeSurface().Surface();
  aSurf1->Bounds(umin, umax, vmin, vmax);
  aPrj1.Init(aSurf1, umin, umax, vmin, vmax);
  //
  aSurf2 = theSurface2->ChangeSurface().Surface();
  aSurf2->Bounds(umin, umax, vmin, vmax);
  aPrj2.Init(aSurf2, umin, umax, vmin, vmax);
  //
  //
  bIsPrevPointOnBoundary=Standard_False;
  for(pit=1; pit<=aNbPnts; pit++) {
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    bIsCurrentPointOnBoundary=Standard_False;
    //
    // whether aPoint is on boundary or not
    //
    for(i=0; i<2; i++) {// exploration Surface 1,2 
      Handle(GeomAdaptor_HSurface) aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->ChangeSurface().Surface()->Bounds(umin, umax, vmin, vmax);
      //
      for(j=0; j<2; j++) {// exploration of coordinate U,V
	Standard_Boolean isperiodic;
	//
	isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
	if(!isperiodic) {
	  continue;
	}
	//
	Standard_Real aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
	Standard_Real aParameter, anoffset, anAdjustPar;
	Standard_Boolean bIsOnFirstBoundary, bIsPointOnBoundary;
	//
	aResolution = (!j) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
	aPeriod     = (!j) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	alowerboundary = (!j) ? umin : vmin;
	aupperboundary = (!j) ? umax : vmax;
	U=0.;V=0.;//?
	//
	if(!i){
	  aPoint.ParametersOnS1(U, V);
	}
	else{
	  aPoint.ParametersOnS2(U, V);
	}
	//
	aParameter = (!j) ? U : V;
	anoffset=0.;
	anAdjustPar=AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);
	//
	bIsOnFirstBoundary=Standard_True;
	//
	bIsPointOnBoundary=
	  IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary);
	
	if(bIsPointOnBoundary) {
	  bIsCurrentPointOnBoundary = Standard_True;
	  break;
	}
      }// for(j=0; j<2; j++)
      
      if(bIsCurrentPointOnBoundary){
	break;
      }
    }// for(i=0; i<2; i++) 
    //
    if((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary)) {

      if(!aListOfPointIndex.IsEmpty()) {
	nblines++;
	anArrayOfLines[nblines] = aListOfPointIndex;
	anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
	aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } // for(pit=1; pit<=aNbPnts; pit++)
  //
  aNbListOfPointIndex=aListOfPointIndex.Extent();
  if(aNbListOfPointIndex) {
    nblines++;
    anArrayOfLines[nblines].Assign (aListOfPointIndex);
    anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
    aListOfPointIndex.Clear();
  }
  //
  if(nblines <= 1){
    return Standard_False;
  }
  //
  // Correct wlines.begin
  Standard_Integer aLineType;
  TColStd_Array1OfListOfInteger anArrayOfLineEnds(1, nblines);
  Handle(IntSurf_LineOn2S) aSeqOfPntOn2S = new IntSurf_LineOn2S (new NCollection_IncAllocator());
  //
  for(i = 1; i <= nblines; i++) {
    aLineType=anArrayOfLineType[i];
    if(aLineType) {
      continue;
    }
    //
    const ListOfInteger& aListOfIndex = anArrayOfLines[i];
    if(aListOfIndex.Extent() < 2) {
      continue;
    }
    //
    TColStd_ListOfInteger aListOfFLIndex;
    Standard_Integer aneighbourindex, aLineTypeNeib;
    //
    for(j = 0; j < 2; j++) {// neighbour line choice 
      aneighbourindex = (!j) ? (i-1) : (i+1);
      if((aneighbourindex < 1) || (aneighbourindex > nblines)){
	continue;
      }
      //
      aLineTypeNeib=anArrayOfLineType[aneighbourindex];
      if(!aLineTypeNeib){
	continue;
      }
      //
      const ListOfInteger& aNeighbour = anArrayOfLines[aneighbourindex];
      Standard_Integer anIndex = (!j) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint = theWLine->Point(anIndex);
      // check if need use derivative.begin .end [absence]
      //
      IntSurf_PntOn2S aNewP = aPoint;
      Standard_Integer surfit, parit;
      //
      for(surfit = 0; surfit < 2; ++surfit) {

	Handle(GeomAdaptor_HSurface) aGASurface = (!surfit) ? theSurface1 : theSurface2;
	
        umin = aGASurface->FirstUParameter();
        umax = aGASurface->LastUParameter();
        vmin = aGASurface->FirstVParameter();
        vmax = aGASurface->LastVParameter();
	Standard_Real U=0., V=0.;

	if(!surfit) {
	  aNewP.ParametersOnS1(U, V);
	}
	else {
	  aNewP.ParametersOnS2(U, V);
	}
	//
	Standard_Integer nbboundaries = 0;
	Standard_Integer bIsUBoundary = Standard_False; // use if nbboundaries == 1
	Standard_Integer bIsFirstBoundary = Standard_False; // use if nbboundaries == 1
	//
	for(parit = 0; parit < 2; parit++) {
	  Standard_Boolean isperiodic = (!parit) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  Standard_Real aResolution = (!parit) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
	  Standard_Real alowerboundary = (!parit) ? umin : vmin;
	  Standard_Real aupperboundary = (!parit) ? umax : vmax;

	  Standard_Real aParameter = (!parit) ? U : V;
	  Standard_Boolean bIsOnFirstBoundary = Standard_True;
  
	  if(!isperiodic) {
	    if(IsPointOnBoundary(aParameter, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary)) {
	      bIsUBoundary = (!parit);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	  }
	  else {
	    Standard_Real aPeriod     = (!parit) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    if(IsPointOnBoundary(anAdjustPar, alowerboundary, aupperboundary, aResolution, bIsOnFirstBoundary)) {
	      bIsUBoundary = (parit == 0);
	      bIsFirstBoundary = bIsOnFirstBoundary;
	      nbboundaries++;
	    }
	  }
	}
	//
	Standard_Boolean bComputeLineEnd = Standard_False;
	
	if(nbboundaries == 2) {
	  bComputeLineEnd = Standard_True;
	}
	else if(nbboundaries == 1) {
	  Standard_Boolean isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

	  if(isperiodic) {
	    Standard_Real alowerboundary = (bIsUBoundary) ? umin : vmin;
	    Standard_Real aupperboundary = (bIsUBoundary) ? umax : vmax;
	    Standard_Real aPeriod     = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
	    Standard_Real aParameter = (bIsUBoundary) ? U : V;
	    Standard_Real anoffset = 0.;
	    Standard_Real anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

	    Standard_Real adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary) : fabs(anAdjustPar - aupperboundary);
	    Standard_Real anotherPar = (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
	    anotherPar += anoffset;
	    Standard_Integer aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	    const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
	    Standard_Real nU1, nV1;

	    if(surfit == 0)
	      aNeighbourPoint.ParametersOnS1(nU1, nV1);
	    else
	      aNeighbourPoint.ParametersOnS2(nU1, nV1);
	    
	    Standard_Real adist1 = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
	    Standard_Real adist2 = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
	    bComputeLineEnd = Standard_True;
	    Standard_Boolean bCheckAngle1 = Standard_False;
	    Standard_Boolean bCheckAngle2 = Standard_False;
	    gp_Vec2d aNewVec;
	    Standard_Real anewU = (bIsUBoundary) ? anotherPar : U;
	    Standard_Real anewV = (bIsUBoundary) ? V : anotherPar;
	    //
	    if(((adist1 - adist2) > Precision::PConfusion()) && 
	       (adist2 < (aPeriod / 4.))) {
	      bCheckAngle1 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		aNewP.SetValue((surfit == 0), anewU, anewV);
		bCheckAngle1 = Standard_False;
	      }
	    }
	    else if(adist1 < (aPeriod / 4.)) {
	      bCheckAngle2 = Standard_True;
	      aNewVec = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

	      if(aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution())) {
		bCheckAngle2 = Standard_False;
	      }
	    }
	    //
	    if(bCheckAngle1 || bCheckAngle2) {
	      // assume there are at least two points in line (see "if" above)
	      Standard_Integer anindexother = aneighbourpointindex;
	      
	      while((anindexother <= aListOfIndex.Last()) && (anindexother >= aListOfIndex.First())) {
		anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
		const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
		Standard_Real nU2, nV2;
		
		if(surfit == 0)
		  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
		else
		  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
		gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

		if(aVecOld.SquareMagnitude() <= (gp::Resolution() * gp::Resolution())) {
		  continue;
		}
		else {
		  Standard_Real anAngle = aNewVec.Angle(aVecOld);

		  if((fabs(anAngle) < (M_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.)) {

		    if(bCheckAngle1) {
		      Standard_Real U1, U2, V1, V2;
		      IntSurf_PntOn2S atmppoint = aNewP;
		      atmppoint.SetValue((surfit == 0), anewU, anewV);
		      atmppoint.Parameters(U1, V1, U2, V2);
		      gp_Pnt P1 = theSurface1->Value(U1, V1);
		      gp_Pnt P2 = theSurface2->Value(U2, V2);
		      gp_Pnt P0 = aPoint.Value();

		      if(P0.IsEqual(P1, aTol) &&
			 P0.IsEqual(P2, aTol) &&
			 P1.IsEqual(P2, aTol)) {
			bComputeLineEnd = Standard_False;
			aNewP.SetValue((surfit == 0), anewU, anewV);
		      }
		    }
		    
		    if(bCheckAngle2) {
		      bComputeLineEnd = Standard_False;
		    }
		  }
		  break;
		}
	      } // end while(anindexother...)
	    }
	  }
	}
	//
	if(bComputeLineEnd) {
	  Standard_Integer aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
	  const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
	  Standard_Real nU1, nV1;

	  if(surfit == 0)
	    aNeighbourPoint.ParametersOnS1(nU1, nV1);
	  else
	    aNeighbourPoint.ParametersOnS2(nU1, nV1);
	  gp_Pnt2d ap1(nU1, nV1);
	  gp_Pnt2d ap2(nU1, nV1);
	  Standard_Integer aneighbourpointindex2 = aneighbourpointindex1;

	  while((aneighbourpointindex2 <= aListOfIndex.Last()) && (aneighbourpointindex2 >= aListOfIndex.First())) {
	    aneighbourpointindex2 = (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
	    const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
	    Standard_Real nU2, nV2;

	    if(surfit == 0)
	      aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
	    else
	      aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
	    ap2.SetX(nU2);
	    ap2.SetY(nV2);

	    if(ap1.SquareDistance(ap2) > (gp::Resolution() * gp::Resolution())) {
	      break;
	    }
	  }	  
	  gp_Pnt2d anewpoint;
	  Standard_Boolean found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);

	  if(found) {
	    // check point
	    Standard_Real aCriteria =aTolSum;// BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
	    //GeomAPI_ProjectPointOnSurf& aProjector = (surfit == 0) ? aPrj2 : aPrj1;
	    ProjectPointOnSurf& aProjector = (surfit == 0) ? aPrj2 : aPrj1;
	    Handle(GeomAdaptor_HSurface) aSurface = (surfit == 0) ? theSurface1 : theSurface2;

	    gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
	    aProjector.Perform(aP3d);

	    if(aProjector.IsDone()) {
	      if(aProjector.LowerDistance() < aCriteria) {
		Standard_Real foundU = U, foundV = V;
		aProjector.LowerDistanceParameters(foundU, foundV);

		if(surfit == 0)
		  aNewP.SetValue(aP3d, anewpoint.X(), anewpoint.Y(), foundU, foundV);
		else
		  aNewP.SetValue(aP3d, foundU, foundV, anewpoint.X(), anewpoint.Y());
	      }
	    }
	  }
	}
      }
      aSeqOfPntOn2S->Add(aNewP);
      aListOfFLIndex.Append(aSeqOfPntOn2S->NbPoints());
    }
    anArrayOfLineEnds.SetValue(i, aListOfFLIndex);
  }
  // Correct wlines.end

  // Split wlines.begin
  for(j = 1; j <= theLConstructor.NbParts(); j++) {
    Standard_Real fprm = 0., lprm = 0.;
    theLConstructor.Part(j, fprm, lprm);
    Standard_Integer ifprm = (Standard_Integer)fprm;
    Standard_Integer ilprm = (Standard_Integer)lprm;
    //
    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
    //
    for(i = 1; i <= nblines; i++) {
      if(anArrayOfLineType[i] != 0) {
	continue;
      }
      const ListOfInteger& aListOfIndex = anArrayOfLines[i];

      if(aListOfIndex.Extent() < 2) {
	continue;
      }
      const TColStd_ListOfInteger& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      Standard_Boolean bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      Standard_Boolean bhaslastpoint = (aListOfFLIndex.Extent() == 2);

      if(!bhasfirstpoint && !aListOfFLIndex.IsEmpty()) {
	bhasfirstpoint = (i != 1);
      }

      if(!bhaslastpoint && !aListOfFLIndex.IsEmpty()) {
	bhaslastpoint = (i != nblines);
      }
      Standard_Boolean bIsFirstInside = ((ifprm >= aListOfIndex.First()) && (ifprm <= aListOfIndex.Last()));
      Standard_Boolean bIsLastInside =  ((ilprm >= aListOfIndex.First()) && (ilprm <= aListOfIndex.Last()));

      if(!bIsFirstInside && !bIsLastInside) {
	if((ifprm < aListOfIndex.First()) && (ilprm > aListOfIndex.Last())) {
	  // append whole line, and boundaries if neccesary
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }

	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

	    if((anArrayOfLineType[aneighbour] != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if(bIsFirstInside && bIsLastInside) {
	// append inside points between ifprm and ilprm
	ListOfInteger::Iterator anIt(aListOfIndex);

	for(; anIt.More(); anIt.Next()) {
	  if((anIt.Value() < ifprm) || (anIt.Value() > ilprm))
	    continue;
	  const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	  aLineOn2S->Add(aP);
	}
      }
      else {

	if(bIsFirstInside) {
	  // append points from ifprm to last point + boundary point
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() < ifprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }

	  if(bhaslastpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
	    aLineOn2S->Add(aP);
	  }
	  // check end of split line (end is almost always)
	  Standard_Integer aneighbour = i + 1;
	  Standard_Boolean bIsEndOfLine = Standard_True;

	  if(aneighbour <= nblines) {
	    const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

	    if((anArrayOfLineType[aneighbour] != 0) &&
	       (aListOfNeighbourIndex.IsEmpty())) {
	      bIsEndOfLine = Standard_False;
	    }
	  }

	  if(bIsEndOfLine) {
	    if(aLineOn2S->NbPoints() > 1) {
	      Handle(IntPatch_WLine) aNewWLine = 
		new IntPatch_WLine(aLineOn2S, Standard_False);
	      theNewLines.Append(aNewWLine);
	    }
	    aLineOn2S = new IntSurf_LineOn2S();
	  }
	}
	// end if(bIsFirstInside)

	if(bIsLastInside) {
	  // append points from first boundary point to ilprm
	  if(bhasfirstpoint) {
	    const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
	    aLineOn2S->Add(aP);
	  }
	  ListOfInteger::Iterator anIt(aListOfIndex);

	  for(; anIt.More(); anIt.Next()) {
	    if(anIt.Value() > ilprm)
	      continue;
	    const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
	    aLineOn2S->Add(aP);
	  }
	}
	//end if(bIsLastInside)
      }
    }

    if(aLineOn2S->NbPoints() > 1) {
      Handle(IntPatch_WLine) aNewWLine = 
	new IntPatch_WLine(aLineOn2S, Standard_False);
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end
  //
  // cda002/I3
  Standard_Real fprm, lprm;
  Standard_Integer ifprm, ilprm, aNbPoints, aIndex;
  //
  aNbParts=theLConstructor.NbParts();
  //
  for(j = 1; j <= aNbParts; j++) {
    theLConstructor.Part(j, fprm, lprm);
    ifprm=(Standard_Integer)fprm;
    ilprm=(Standard_Integer)lprm;
    //
    if ((ilprm-ifprm)==1) {
      for(i = 1; i <= nblines; i++) {
	aLineType=anArrayOfLineType[i];
	if(aLineType) {
	  continue;
	}
	//
	const ListOfInteger& aListOfIndex = anArrayOfLines[i];
	aNbPoints=aListOfIndex.Extent();
	if(aNbPoints==1) {
	  aIndex=aListOfIndex.First();
	  if (aIndex==ifprm || aIndex==ilprm) {
	    Handle(IntSurf_LineOn2S) aLineOn2S = new IntSurf_LineOn2S();
	    const IntSurf_PntOn2S& aP1 = theWLine->Point(ifprm);
	    const IntSurf_PntOn2S& aP2 = theWLine->Point(ilprm);
	    aLineOn2S->Add(aP1);
	    aLineOn2S->Add(aP2);
	    Handle(IntPatch_WLine) aNewWLine = 
	      new IntPatch_WLine(aLineOn2S, Standard_False);
	    theNewLines.Append(aNewWLine);
	  }
	}
      }
    }
  }
  //
  return Standard_True;
}
