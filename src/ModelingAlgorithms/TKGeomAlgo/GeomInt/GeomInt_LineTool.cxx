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

#include <GeomInt_LineTool.hxx>

#include <Extrema_ExtPS.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_WLine.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_List.hxx>
#include <NCollection_LocalArray.hxx>
#include <NCollection_OccAllocator.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#include <vector>

namespace
{
class ProjectPointOnSurf
{
public:
  ProjectPointOnSurf()
      : myIsDone(false),
        myIndex(0)
  {
  }

  void Init(const occ::handle<Geom_Surface>& Surface,
            const double                     Umin,
            const double                     Usup,
            const double                     Vmin,
            const double                     Vsup);
  void Init();
  void Perform(const gp_Pnt& P);

  bool IsDone() const { return myIsDone; }

  void   LowerDistanceParameters(double& U, double& V) const;
  double LowerDistance() const;

protected:
  bool                myIsDone;
  int                 myIndex;
  Extrema_ExtPS       myExtPS;
  GeomAdaptor_Surface myGeomAdaptor;
};

//=================================================================================================

void ProjectPointOnSurf::Init(const occ::handle<Geom_Surface>& Surface,
                              const double                     Umin,
                              const double                     Usup,
                              const double                     Vmin,
                              const double                     Vsup)
{
  constexpr double Tolerance = Precision::PConfusion();
  //
  myGeomAdaptor.Load(Surface, Umin, Usup, Vmin, Vsup);
  myExtPS.Initialize(myGeomAdaptor, Umin, Usup, Vmin, Vsup, Tolerance, Tolerance);
  myIsDone = false;
}

//=================================================================================================

void ProjectPointOnSurf::Init()
{
  myIsDone = myExtPS.IsDone() && (myExtPS.NbExt() > 0);
  if (myIsDone)
  {
    // evaluate the lower distance and its index;
    double Dist2Min = myExtPS.SquareDistance(1);
    myIndex         = 1;
    for (int i = 2; i <= myExtPS.NbExt(); i++)
    {
      const double Dist2 = myExtPS.SquareDistance(i);
      if (Dist2 < Dist2Min)
      {
        Dist2Min = Dist2;
        myIndex  = i;
      }
    }
  }
}

//=================================================================================================

void ProjectPointOnSurf::Perform(const gp_Pnt& P)
{
  myExtPS.Perform(P);
  Init();
}

//=================================================================================================

void ProjectPointOnSurf::LowerDistanceParameters(double& U, double& V) const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistanceParameters");
  (myExtPS.Point(myIndex)).Parameter(U, V);
}

//=================================================================================================

double ProjectPointOnSurf::LowerDistance() const
{
  StdFail_NotDone_Raise_if(!myIsDone, "GeomInt_IntSS::ProjectPointOnSurf::LowerDistance");
  return sqrt(myExtPS.SquareDistance(myIndex));
}
} // namespace

//=================================================================================================

static double AdjustPeriodic(const double theParameter,
                             const double parmin,
                             const double parmax,
                             const double thePeriod,
                             double&      theOffset)
{
  double aresult = theParameter;
  theOffset      = 0.;
  while (aresult < parmin)
  {
    aresult += thePeriod;
    theOffset += thePeriod;
  }
  while (aresult > parmax)
  {
    aresult -= thePeriod;
    theOffset -= thePeriod;
  }
  return aresult;
}

//=================================================================================================

static bool IsPointOnBoundary(const double theParameter,
                              const double theFirstBoundary,
                              const double theSecondBoundary,
                              const double theResolution,
                              bool&        IsOnFirstBoundary)
{
  IsOnFirstBoundary = true;
  if (fabs(theParameter - theFirstBoundary) < theResolution)
    return true;
  if (fabs(theParameter - theSecondBoundary) < theResolution)
  {
    IsOnFirstBoundary = false;
    return true;
  }
  return false;
}

//=================================================================================================

static bool FindPoint(const gp_Pnt2d& theFirstPoint,
                      const gp_Pnt2d& theLastPoint,
                      const double    theUmin,
                      const double    theUmax,
                      const double    theVmin,
                      const double    theVmax,
                      gp_Pnt2d&       theNewPoint)
{
  gp_Vec2d aVec(theFirstPoint, theLastPoint);
  int      i = 0, j = 0;

  for (i = 0; i < 4; i++)
  {
    gp_Vec2d anOtherVec;
    gp_Vec2d anOtherVecNormal;
    gp_Pnt2d aprojpoint = theLastPoint;

    if ((i % 2) == 0)
    {
      anOtherVec.SetX(0.);
      anOtherVec.SetY(1.);
      anOtherVecNormal.SetX(1.);
      anOtherVecNormal.SetY(0.);

      if (i < 2)
        aprojpoint.SetX(theUmin);
      else
        aprojpoint.SetX(theUmax);
    }
    else
    {
      anOtherVec.SetX(1.);
      anOtherVec.SetY(0.);
      anOtherVecNormal.SetX(0.);
      anOtherVecNormal.SetY(1.);

      if (i < 2)
        aprojpoint.SetY(theVmin);
      else
        aprojpoint.SetY(theVmax);
    }
    gp_Vec2d anormvec = aVec;
    anormvec.Normalize();
    double adot1 = anormvec.Dot(anOtherVecNormal);

    if (fabs(adot1) < Precision::Angular())
      continue;
    double adist = 0.;

    if ((i % 2) == 0)
    {
      adist = (i < 2) ? fabs(theLastPoint.X() - theUmin) : fabs(theLastPoint.X() - theUmax);
    }
    else
    {
      adist = (i < 2) ? fabs(theLastPoint.Y() - theVmin) : fabs(theLastPoint.Y() - theVmax);
    }
    double anoffset = adist * anOtherVec.Dot(anormvec) / adot1;

    for (j = 0; j < 2; j++)
    {
      anoffset = (j == 0) ? anoffset : -anoffset;
      gp_Pnt2d acurpoint(aprojpoint.XY() + (anOtherVec.XY() * anoffset));
      gp_Vec2d acurvec(theLastPoint, acurpoint);

      //
      double aDotX, anAngleX, aPC;
      //
      aDotX    = aVec.Dot(acurvec);
      anAngleX = aVec.Angle(acurvec);
      aPC      = Precision::PConfusion();
      //
      if (aDotX > 0. && fabs(anAngleX) < aPC)
      {
        //
        if ((i % 2) == 0)
        {
          if ((acurpoint.Y() >= theVmin) && (acurpoint.Y() <= theVmax))
          {
            theNewPoint = acurpoint;
            return true;
          }
        }
        else
        {
          if ((acurpoint.X() >= theUmin) && (acurpoint.X() <= theUmax))
          {
            theNewPoint = acurpoint;
            return true;
          }
        }
      }
    }
  }
  return false;
}

//=================================================================================================

int GeomInt_LineTool::NbVertex(const occ::handle<IntPatch_Line>& L)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic:
      return occ::down_cast<IntPatch_ALine>(L)->NbVertex();
    case IntPatch_Restriction:
      return occ::down_cast<IntPatch_RLine>(L)->NbVertex();
    case IntPatch_Walking:
      return occ::down_cast<IntPatch_WLine>(L)->NbVertex();
    default:
      break;
  }
  return occ::down_cast<IntPatch_GLine>(L)->NbVertex();
}

//=================================================================================================

const IntPatch_Point& GeomInt_LineTool::Vertex(const occ::handle<IntPatch_Line>& L, const int I)
{
  switch (L->ArcType())
  {
    case IntPatch_Analytic:
      return occ::down_cast<IntPatch_ALine>(L)->Vertex(I);
    case IntPatch_Restriction:
      return occ::down_cast<IntPatch_RLine>(L)->Vertex(I);
    case IntPatch_Walking:
      return occ::down_cast<IntPatch_WLine>(L)->Vertex(I);
    default:
      break;
  }
  return occ::down_cast<IntPatch_GLine>(L)->Vertex(I);
}

//=================================================================================================

double GeomInt_LineTool::FirstParameter(const occ::handle<IntPatch_Line>& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic: {
      occ::handle<IntPatch_ALine> alin = occ::down_cast<IntPatch_ALine>(L);
      if (alin->HasFirstPoint())
        return alin->FirstPoint().ParameterOnLine();
      bool   included;
      double firstp = alin->FirstParameter(included);
      if (!included)
        firstp += Epsilon(firstp);
      return firstp;
    }

    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> rlin = occ::down_cast<IntPatch_RLine>(L);
      // clang-format off
	  return (rlin->HasFirstPoint()? rlin->FirstPoint().ParameterOnLine() : -Precision::Infinite()); // a voir selon le type de la ligne 2d
      // clang-format on
    }

    case IntPatch_Walking: {
      occ::handle<IntPatch_WLine> wlin = occ::down_cast<IntPatch_WLine>(L);
      return (wlin->HasFirstPoint() ? wlin->FirstPoint().ParameterOnLine() : 1.);
    }

    default: {
      occ::handle<IntPatch_GLine> glin = occ::down_cast<IntPatch_GLine>(L);
      if (glin->HasFirstPoint())
        return glin->FirstPoint().ParameterOnLine();
      switch (typl)
      {
        case IntPatch_Lin:
        case IntPatch_Parabola:
        case IntPatch_Hyperbola:
          return -Precision::Infinite();
        default:
          break;
      }
    }
  }
  return 0.0;
}

//=================================================================================================

double GeomInt_LineTool::LastParameter(const occ::handle<IntPatch_Line>& L)
{
  const IntPatch_IType typl = L->ArcType();
  switch (typl)
  {
    case IntPatch_Analytic: {
      occ::handle<IntPatch_ALine> alin = occ::down_cast<IntPatch_ALine>(L);
      if (alin->HasLastPoint())
        return alin->LastPoint().ParameterOnLine();
      bool   included;
      double lastp = alin->LastParameter(included);
      if (!included)
        lastp -= Epsilon(lastp);
      return lastp;
    }

    case IntPatch_Restriction: {
      occ::handle<IntPatch_RLine> rlin = occ::down_cast<IntPatch_RLine>(L);
      // clang-format off
	  return (rlin->HasLastPoint()? rlin->LastPoint().ParameterOnLine() : Precision::Infinite()); // a voir selon le type de la ligne 2d
      // clang-format on
    }

    case IntPatch_Walking: {
      occ::handle<IntPatch_WLine> wlin = occ::down_cast<IntPatch_WLine>(L);
      return (wlin->HasLastPoint() ? wlin->LastPoint().ParameterOnLine() : wlin->NbPnts());
    }

    default: {
      occ::handle<IntPatch_GLine> glin = occ::down_cast<IntPatch_GLine>(L);
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
          return 2. * M_PI;
        default:
          break;
      }
    }
  }
  return 0.0;
}

//=================================================================================================

bool GeomInt_LineTool::DecompositionOfWLine(
  const occ::handle<IntPatch_WLine>&                theWLine,
  const occ::handle<GeomAdaptor_Surface>&           theSurface1,
  const occ::handle<GeomAdaptor_Surface>&           theSurface2,
  const double                                      aTolSum,
  const GeomInt_LineConstructor&                    theLConstructor,
  NCollection_Sequence<occ::handle<IntPatch_Line>>& theNewLines)
{
  typedef NCollection_List<int> ListOfInteger;
  // have to use std::vector, not NCollection_Vector in order to use copy constructor of
  // ListOfInteger which will be created with specific allocator instance
  typedef std::vector<ListOfInteger, NCollection_OccAllocator<ListOfInteger>> ArrayOfListOfInteger;

  bool   bIsPrevPointOnBoundary, bIsCurrentPointOnBoundary;
  int    nblines, aNbPnts, aNbParts, pit, i, j, aNbListOfPointIndex;
  double aTol, umin, umax, vmin, vmax;

  // an inc allocator, it will contain wasted space (upon list's Clear()) but it should
  // still be faster than the standard allocator, and wasted memory should not be
  // significant and will be limited by time span of this function;
  // this is a separate allocator from the anIncAlloc below what provides better data
  // locality in the latter (by avoiding wastes which will only be in anIdxAlloc)
  occ::handle<NCollection_IncAllocator> anIdxAlloc = new NCollection_IncAllocator();
  ListOfInteger                         aListOfPointIndex(anIdxAlloc);

  // GeomAPI_ProjectPointOnSurf aPrj1, aPrj2;
  ProjectPointOnSurf        aPrj1, aPrj2;
  occ::handle<Geom_Surface> aSurf1, aSurf2;
  //
  aNbParts = theLConstructor.NbParts();
  aNbPnts  = theWLine->NbPnts();
  //
  if ((!aNbPnts) || (!aNbParts))
  {
    return false;
  }
  //
  occ::handle<NCollection_IncAllocator>   anIncAlloc = new NCollection_IncAllocator();
  NCollection_OccAllocator<ListOfInteger> anAlloc(anIncAlloc);
  const ListOfInteger  aDummy(anIncAlloc); // empty list to be copy constructed from
  ArrayOfListOfInteger anArrayOfLines(aNbPnts + 1, aDummy, anAlloc);

  NCollection_LocalArray<int> anArrayOfLineTypeArr(aNbPnts + 1);
  int*                        anArrayOfLineType = anArrayOfLineTypeArr;
  //
  nblines = 0;
  aTol    = Precision::Confusion();
  //
  aSurf1 = theSurface1->Surface();
  aSurf1->Bounds(umin, umax, vmin, vmax);
  aPrj1.Init(aSurf1, umin, umax, vmin, vmax);
  //
  aSurf2 = theSurface2->Surface();
  aSurf2->Bounds(umin, umax, vmin, vmax);
  aPrj2.Init(aSurf2, umin, umax, vmin, vmax);
  //
  //
  bIsPrevPointOnBoundary = false;
  for (pit = 1; pit <= aNbPnts; pit++)
  {
    const IntSurf_PntOn2S& aPoint = theWLine->Point(pit);
    bIsCurrentPointOnBoundary     = false;
    //
    // whether aPoint is on boundary or not
    //
    for (i = 0; i < 2; i++)
    { // exploration Surface 1,2
      occ::handle<GeomAdaptor_Surface> aGASurface = (!i) ? theSurface1 : theSurface2;
      aGASurface->Surface()->Bounds(umin, umax, vmin, vmax);
      //
      for (j = 0; j < 2; j++)
      { // exploration of coordinate U,V
        bool isperiodic;
        //
        isperiodic = (!j) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();
        if (!isperiodic)
        {
          continue;
        }
        //
        double aResolution, aPeriod, alowerboundary, aupperboundary, U, V;
        double aParameter, anoffset, anAdjustPar;
        bool   bIsOnFirstBoundary, bIsPointOnBoundary;
        //
        aResolution    = (!j) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
        aPeriod        = (!j) ? aGASurface->UPeriod() : aGASurface->VPeriod();
        alowerboundary = (!j) ? umin : vmin;
        aupperboundary = (!j) ? umax : vmax;
        U              = 0.;
        V              = 0.; //?
        //
        if (!i)
        {
          aPoint.ParametersOnS1(U, V);
        }
        else
        {
          aPoint.ParametersOnS2(U, V);
        }
        //
        aParameter  = (!j) ? U : V;
        anoffset    = 0.;
        anAdjustPar = AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);
        //
        bIsOnFirstBoundary = true;
        //
        bIsPointOnBoundary = IsPointOnBoundary(anAdjustPar,
                                               alowerboundary,
                                               aupperboundary,
                                               aResolution,
                                               bIsOnFirstBoundary);

        if (bIsPointOnBoundary)
        {
          bIsCurrentPointOnBoundary = true;
          break;
        }
      } // for(j=0; j<2; j++)

      if (bIsCurrentPointOnBoundary)
      {
        break;
      }
    } // for(i=0; i<2; i++)
    //
    if ((bIsCurrentPointOnBoundary != bIsPrevPointOnBoundary))
    {

      if (!aListOfPointIndex.IsEmpty())
      {
        nblines++;
        anArrayOfLines[nblines]    = aListOfPointIndex;
        anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
        aListOfPointIndex.Clear();
      }
      bIsPrevPointOnBoundary = bIsCurrentPointOnBoundary;
    }
    aListOfPointIndex.Append(pit);
  } // for(pit=1; pit<=aNbPnts; pit++)
  //
  aNbListOfPointIndex = aListOfPointIndex.Extent();
  if (aNbListOfPointIndex)
  {
    nblines++;
    anArrayOfLines[nblines].Assign(aListOfPointIndex);
    anArrayOfLineType[nblines] = bIsPrevPointOnBoundary;
    aListOfPointIndex.Clear();
  }
  //
  if (nblines <= 1)
  {
    return false;
  }
  //
  // Correct wlines.begin
  int                                       aLineType;
  NCollection_Array1<NCollection_List<int>> anArrayOfLineEnds(1, nblines);
  occ::handle<IntSurf_LineOn2S>             aSeqOfPntOn2S =
    new IntSurf_LineOn2S(new NCollection_IncAllocator());
  //
  for (i = 1; i <= nblines; i++)
  {
    aLineType = anArrayOfLineType[i];
    if (aLineType)
    {
      continue;
    }
    //
    const ListOfInteger& aListOfIndex = anArrayOfLines[i];
    if (aListOfIndex.Extent() < 2)
    {
      continue;
    }
    //
    NCollection_List<int> aListOfFLIndex;
    int                   aneighbourindex, aLineTypeNeib;
    //
    for (j = 0; j < 2; j++)
    { // neighbour line choice
      aneighbourindex = (!j) ? (i - 1) : (i + 1);
      if ((aneighbourindex < 1) || (aneighbourindex > nblines))
      {
        continue;
      }
      //
      aLineTypeNeib = anArrayOfLineType[aneighbourindex];
      if (!aLineTypeNeib)
      {
        continue;
      }
      //
      const ListOfInteger&   aNeighbour = anArrayOfLines[aneighbourindex];
      int                    anIndex    = (!j) ? aNeighbour.Last() : aNeighbour.First();
      const IntSurf_PntOn2S& aPoint     = theWLine->Point(anIndex);
      // check if need use derivative.begin .end [absence]
      //
      IntSurf_PntOn2S aNewP = aPoint;
      int             surfit, parit;
      //
      for (surfit = 0; surfit < 2; ++surfit)
      {

        occ::handle<GeomAdaptor_Surface> aGASurface = (!surfit) ? theSurface1 : theSurface2;

        umin     = aGASurface->FirstUParameter();
        umax     = aGASurface->LastUParameter();
        vmin     = aGASurface->FirstVParameter();
        vmax     = aGASurface->LastVParameter();
        double U = 0., V = 0.;

        if (!surfit)
        {
          aNewP.ParametersOnS1(U, V);
        }
        else
        {
          aNewP.ParametersOnS2(U, V);
        }
        //
        int nbboundaries     = 0;
        int bIsUBoundary     = false; // use if nbboundaries == 1
        int bIsFirstBoundary = false; // use if nbboundaries == 1
        //
        for (parit = 0; parit < 2; parit++)
        {
          bool isperiodic = (!parit) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          double aResolution =
            (!parit) ? aGASurface->UResolution(aTol) : aGASurface->VResolution(aTol);
          double alowerboundary = (!parit) ? umin : vmin;
          double aupperboundary = (!parit) ? umax : vmax;

          double aParameter         = (!parit) ? U : V;
          bool   bIsOnFirstBoundary = true;

          if (!isperiodic)
          {
            if (IsPointOnBoundary(aParameter,
                                  alowerboundary,
                                  aupperboundary,
                                  aResolution,
                                  bIsOnFirstBoundary))
            {
              bIsUBoundary     = (!parit);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
          }
          else
          {
            double aPeriod  = (!parit) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            double anoffset = 0.;
            double anAdjustPar =
              AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

            if (IsPointOnBoundary(anAdjustPar,
                                  alowerboundary,
                                  aupperboundary,
                                  aResolution,
                                  bIsOnFirstBoundary))
            {
              bIsUBoundary     = (parit == 0);
              bIsFirstBoundary = bIsOnFirstBoundary;
              nbboundaries++;
            }
          }
        }
        //
        bool bComputeLineEnd = false;

        if (nbboundaries == 2)
        {
          bComputeLineEnd = true;
        }
        else if (nbboundaries == 1)
        {
          bool isperiodic = (bIsUBoundary) ? aGASurface->IsUPeriodic() : aGASurface->IsVPeriodic();

          if (isperiodic)
          {
            double alowerboundary = (bIsUBoundary) ? umin : vmin;
            double aupperboundary = (bIsUBoundary) ? umax : vmax;
            double aPeriod        = (bIsUBoundary) ? aGASurface->UPeriod() : aGASurface->VPeriod();
            double aParameter     = (bIsUBoundary) ? U : V;
            double anoffset       = 0.;
            double anAdjustPar =
              AdjustPeriodic(aParameter, alowerboundary, aupperboundary, aPeriod, anoffset);

            double adist = (bIsFirstBoundary) ? fabs(anAdjustPar - alowerboundary)
                                              : fabs(anAdjustPar - aupperboundary);
            double anotherPar =
              (bIsFirstBoundary) ? (aupperboundary - adist) : (alowerboundary + adist);
            anotherPar += anoffset;
            int aneighbourpointindex = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
            const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex);
            double                 nU1, nV1;

            if (surfit == 0)
              aNeighbourPoint.ParametersOnS1(nU1, nV1);
            else
              aNeighbourPoint.ParametersOnS2(nU1, nV1);

            double adist1   = (bIsUBoundary) ? fabs(nU1 - U) : fabs(nV1 - V);
            double adist2   = (bIsUBoundary) ? fabs(nU1 - anotherPar) : fabs(nV1 - anotherPar);
            bComputeLineEnd = true;
            bool     bCheckAngle1 = false;
            bool     bCheckAngle2 = false;
            gp_Vec2d aNewVec;
            double   anewU = (bIsUBoundary) ? anotherPar : U;
            double   anewV = (bIsUBoundary) ? V : anotherPar;
            //
            if (((adist1 - adist2) > Precision::PConfusion()) && (adist2 < (aPeriod / 4.)))
            {
              bCheckAngle1 = true;
              aNewVec      = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(anewU, anewV));

              if (aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution()))
              {
                aNewP.SetValue((surfit == 0), anewU, anewV);
                bCheckAngle1 = false;
              }
            }
            else if (adist1 < (aPeriod / 4.))
            {
              bCheckAngle2 = true;
              aNewVec      = gp_Vec2d(gp_Pnt2d(nU1, nV1), gp_Pnt2d(U, V));

              if (aNewVec.SquareMagnitude() < (gp::Resolution() * gp::Resolution()))
              {
                bCheckAngle2 = false;
              }
            }
            //
            if (bCheckAngle1 || bCheckAngle2)
            {
              // assume there are at least two points in line (see "if" above)
              int anindexother = aneighbourpointindex;

              while ((anindexother <= aListOfIndex.Last())
                     && (anindexother >= aListOfIndex.First()))
              {
                anindexother = (j == 0) ? (anindexother + 1) : (anindexother - 1);
                const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(anindexother);
                double                 nU2, nV2;

                if (surfit == 0)
                  aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
                else
                  aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
                gp_Vec2d aVecOld(gp_Pnt2d(nU2, nV2), gp_Pnt2d(nU1, nV1));

                if (aVecOld.SquareMagnitude() <= (gp::Resolution() * gp::Resolution()))
                {
                  continue;
                }
                else
                {
                  double anAngle = aNewVec.Angle(aVecOld);

                  if ((fabs(anAngle) < (M_PI * 0.25)) && (aNewVec.Dot(aVecOld) > 0.))
                  {

                    if (bCheckAngle1)
                    {
                      double          U1, U2, V1, V2;
                      IntSurf_PntOn2S atmppoint = aNewP;
                      atmppoint.SetValue((surfit == 0), anewU, anewV);
                      atmppoint.Parameters(U1, V1, U2, V2);
                      gp_Pnt P1 = theSurface1->Value(U1, V1);
                      gp_Pnt P2 = theSurface2->Value(U2, V2);
                      gp_Pnt P0 = aPoint.Value();

                      if (P0.IsEqual(P1, aTol) && P0.IsEqual(P2, aTol) && P1.IsEqual(P2, aTol))
                      {
                        bComputeLineEnd = false;
                        aNewP.SetValue((surfit == 0), anewU, anewV);
                      }
                    }

                    if (bCheckAngle2)
                    {
                      bComputeLineEnd = false;
                    }
                  }
                  break;
                }
              } // end while(anindexother...)
            }
          }
        }
        //
        if (bComputeLineEnd)
        {
          int aneighbourpointindex1 = (j == 0) ? aListOfIndex.First() : aListOfIndex.Last();
          const IntSurf_PntOn2S& aNeighbourPoint = theWLine->Point(aneighbourpointindex1);
          double                 nU1, nV1;

          if (surfit == 0)
            aNeighbourPoint.ParametersOnS1(nU1, nV1);
          else
            aNeighbourPoint.ParametersOnS2(nU1, nV1);
          gp_Pnt2d ap1(nU1, nV1);
          gp_Pnt2d ap2(nU1, nV1);
          int      aneighbourpointindex2 = aneighbourpointindex1;

          while ((aneighbourpointindex2 <= aListOfIndex.Last())
                 && (aneighbourpointindex2 >= aListOfIndex.First()))
          {
            aneighbourpointindex2 =
              (j == 0) ? (aneighbourpointindex2 + 1) : (aneighbourpointindex2 - 1);
            const IntSurf_PntOn2S& aPrevNeighbourPoint = theWLine->Point(aneighbourpointindex2);
            double                 nU2, nV2;

            if (surfit == 0)
              aPrevNeighbourPoint.ParametersOnS1(nU2, nV2);
            else
              aPrevNeighbourPoint.ParametersOnS2(nU2, nV2);
            ap2.SetX(nU2);
            ap2.SetY(nV2);

            if (ap1.SquareDistance(ap2) > (gp::Resolution() * gp::Resolution()))
            {
              break;
            }
          }
          gp_Pnt2d anewpoint;
          bool     found = FindPoint(ap2, ap1, umin, umax, vmin, vmax, anewpoint);

          if (found)
          {
            // check point
            // clang-format off
	    double aCriteria =aTolSum;// BRep_Tool::Tolerance(theFace1) + BRep_Tool::Tolerance(theFace2);
	    //GeomAPI_ProjectPointOnSurf& aProjector = (surfit == 0) ? aPrj2 : aPrj1;
            // clang-format on
            ProjectPointOnSurf&              aProjector = (surfit == 0) ? aPrj2 : aPrj1;
            occ::handle<GeomAdaptor_Surface> aSurface   = (surfit == 0) ? theSurface1 : theSurface2;

            gp_Pnt aP3d = aSurface->Value(anewpoint.X(), anewpoint.Y());
            aProjector.Perform(aP3d);

            if (aProjector.IsDone())
            {
              if (aProjector.LowerDistance() < aCriteria)
              {
                double foundU = U, foundV = V;
                aProjector.LowerDistanceParameters(foundU, foundV);

                if (surfit == 0)
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
  for (j = 1; j <= theLConstructor.NbParts(); j++)
  {
    double fprm = 0., lprm = 0.;
    theLConstructor.Part(j, fprm, lprm);
    int ifprm = (int)fprm;
    int ilprm = (int)lprm;
    //
    occ::handle<IntSurf_LineOn2S> aLineOn2S = new IntSurf_LineOn2S();
    //
    for (i = 1; i <= nblines; i++)
    {
      if (anArrayOfLineType[i] != 0)
      {
        continue;
      }
      const ListOfInteger& aListOfIndex = anArrayOfLines[i];

      if (aListOfIndex.Extent() < 2)
      {
        continue;
      }
      const NCollection_List<int>& aListOfFLIndex = anArrayOfLineEnds.Value(i);
      bool                         bhasfirstpoint = (aListOfFLIndex.Extent() == 2);
      bool                         bhaslastpoint  = (aListOfFLIndex.Extent() == 2);

      if (!bhasfirstpoint && !aListOfFLIndex.IsEmpty())
      {
        bhasfirstpoint = (i != 1);
      }

      if (!bhaslastpoint && !aListOfFLIndex.IsEmpty())
      {
        bhaslastpoint = (i != nblines);
      }
      bool bIsFirstInside = ((ifprm >= aListOfIndex.First()) && (ifprm <= aListOfIndex.Last()));
      bool bIsLastInside  = ((ilprm >= aListOfIndex.First()) && (ilprm <= aListOfIndex.Last()));

      if (!bIsFirstInside && !bIsLastInside)
      {
        if ((ifprm < aListOfIndex.First()) && (ilprm > aListOfIndex.Last()))
        {
          // append whole line, and boundaries if necessary
          if (bhasfirstpoint)
          {
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
            aLineOn2S->Add(aP);
          }
          ListOfInteger::Iterator anIt(aListOfIndex);

          for (; anIt.More(); anIt.Next())
          {
            const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
            aLineOn2S->Add(aP);
          }

          if (bhaslastpoint)
          {
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
            aLineOn2S->Add(aP);
          }

          // check end of split line (end is almost always)
          int  aneighbour   = i + 1;
          bool bIsEndOfLine = true;

          if (aneighbour <= nblines)
          {
            const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

            if ((anArrayOfLineType[aneighbour] != 0) && (aListOfNeighbourIndex.IsEmpty()))
            {
              bIsEndOfLine = false;
            }
          }

          if (bIsEndOfLine)
          {
            if (aLineOn2S->NbPoints() > 1)
            {
              occ::handle<IntPatch_WLine> aNewWLine = new IntPatch_WLine(aLineOn2S, false);
              aNewWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        continue;
      }
      // end if(!bIsFirstInside && !bIsLastInside)

      if (bIsFirstInside && bIsLastInside)
      {
        // append inside points between ifprm and ilprm
        ListOfInteger::Iterator anIt(aListOfIndex);

        for (; anIt.More(); anIt.Next())
        {
          if ((anIt.Value() < ifprm) || (anIt.Value() > ilprm))
            continue;
          const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
          aLineOn2S->Add(aP);
        }
      }
      else
      {

        if (bIsFirstInside)
        {
          // append points from ifprm to last point + boundary point
          ListOfInteger::Iterator anIt(aListOfIndex);

          for (; anIt.More(); anIt.Next())
          {
            if (anIt.Value() < ifprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
            aLineOn2S->Add(aP);
          }

          if (bhaslastpoint)
          {
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.Last());
            aLineOn2S->Add(aP);
          }
          // check end of split line (end is almost always)
          int  aneighbour   = i + 1;
          bool bIsEndOfLine = true;

          if (aneighbour <= nblines)
          {
            const ListOfInteger& aListOfNeighbourIndex = anArrayOfLines[aneighbour];

            if ((anArrayOfLineType[aneighbour] != 0) && (aListOfNeighbourIndex.IsEmpty()))
            {
              bIsEndOfLine = false;
            }
          }

          if (bIsEndOfLine)
          {
            if (aLineOn2S->NbPoints() > 1)
            {
              occ::handle<IntPatch_WLine> aNewWLine = new IntPatch_WLine(aLineOn2S, false);
              aNewWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
              theNewLines.Append(aNewWLine);
            }
            aLineOn2S = new IntSurf_LineOn2S();
          }
        }
        // end if(bIsFirstInside)

        if (bIsLastInside)
        {
          // append points from first boundary point to ilprm
          if (bhasfirstpoint)
          {
            const IntSurf_PntOn2S& aP = aSeqOfPntOn2S->Value(aListOfFLIndex.First());
            aLineOn2S->Add(aP);
          }
          ListOfInteger::Iterator anIt(aListOfIndex);

          for (; anIt.More(); anIt.Next())
          {
            if (anIt.Value() > ilprm)
              continue;
            const IntSurf_PntOn2S& aP = theWLine->Point(anIt.Value());
            aLineOn2S->Add(aP);
          }
        }
        // end if(bIsLastInside)
      }
    }

    if (aLineOn2S->NbPoints() > 1)
    {
      occ::handle<IntPatch_WLine> aNewWLine = new IntPatch_WLine(aLineOn2S, false);
      aNewWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
      theNewLines.Append(aNewWLine);
    }
  }
  // Split wlines.end
  //
  // cda002/I3
  double fprm, lprm;
  int    ifprm, ilprm, aNbPoints, aIndex;
  //
  aNbParts = theLConstructor.NbParts();
  //
  for (j = 1; j <= aNbParts; j++)
  {
    theLConstructor.Part(j, fprm, lprm);
    ifprm = (int)fprm;
    ilprm = (int)lprm;
    //
    if ((ilprm - ifprm) == 1)
    {
      for (i = 1; i <= nblines; i++)
      {
        aLineType = anArrayOfLineType[i];
        if (aLineType)
        {
          continue;
        }
        //
        const ListOfInteger& aListOfIndex = anArrayOfLines[i];
        aNbPoints                         = aListOfIndex.Extent();
        if (aNbPoints == 1)
        {
          aIndex = aListOfIndex.First();
          if (aIndex == ifprm || aIndex == ilprm)
          {
            occ::handle<IntSurf_LineOn2S> aLineOn2S = new IntSurf_LineOn2S();
            const IntSurf_PntOn2S&        aP1       = theWLine->Point(ifprm);
            const IntSurf_PntOn2S&        aP2       = theWLine->Point(ilprm);
            aLineOn2S->Add(aP1);
            aLineOn2S->Add(aP2);
            occ::handle<IntPatch_WLine> aNewWLine = new IntPatch_WLine(aLineOn2S, false);
            aNewWLine->SetCreatingWayInfo(theWLine->GetCreatingWay());
            theNewLines.Append(aNewWLine);
          }
        }
      }
    }
  }
  //
  return true;
}
