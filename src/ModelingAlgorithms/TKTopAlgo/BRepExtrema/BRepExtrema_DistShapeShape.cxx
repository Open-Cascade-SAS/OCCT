// Created on: 1996-04-22
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified: Mps(10-04-97) portage WNT

#include <BRepExtrema_DistShapeShape.hxx>

#include <Standard_OStream.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <TopExp.hxx>
#include <BRepExtrema_DistanceSS.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Standard_MemoryUtils.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Precision.hxx>
#include <BRepExtrema_UnCompatibleShape.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <NCollection_Vector.hxx>
#include <OSD_Parallel.hxx>
#include <StdFail_NotDone.hxx>

#include <algorithm>
#include <atomic>
#include <mutex>

namespace
{

static void Decomposition(const TopoDS_Shape&                                            S,
                          NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MapV,
                          NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MapE,
                          NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MapF)
{
  MapV.Clear();
  MapE.Clear();
  MapF.Clear();
  TopExp::MapShapes(S, TopAbs_VERTEX, MapV);
  TopExp::MapShapes(S, TopAbs_EDGE, MapE);
  TopExp::MapShapes(S, TopAbs_FACE, MapF);
}

static void BoxCalculation(const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& Map,
                           NCollection_Array1<Bnd_Box>& SBox)
{
  for (int i = 1; i <= Map.Extent(); i++)
  {
    Bnd_Box box;
    BRepBndLib::Add(Map(i), box);
    SBox[i] = box;
  }
}

inline double DistanceInitiale(const TopoDS_Vertex& V1, const TopoDS_Vertex& V2)
{
  return (BRep_Tool::Pnt(V1).Distance(BRep_Tool::Pnt(V2)));
}

//! Pair of objects to check extrema.
struct BRepExtrema_CheckPair
{
  int    Index1;   //!< Index of the 1st sub-shape
  int    Index2;   //!< Index of the 2nd sub-shape
  double Distance; //!< Distance between sub-shapes

  //! Uninitialized constructor for collection.
  BRepExtrema_CheckPair()
      : Index1(0),
        Index2(0),
        Distance(0.0)
  {
  }

  //! Creates new pair of sub-shapes.
  BRepExtrema_CheckPair(int theIndex1, int theIndex2, double theDistance)
      : Index1(theIndex1),
        Index2(theIndex2),
        Distance(theDistance)
  {
  }
};

// Used by std::sort function
static bool BRepExtrema_CheckPair_Comparator(const BRepExtrema_CheckPair& theLeft,
                                             const BRepExtrema_CheckPair& theRight)
{
  return (theLeft.Distance < theRight.Distance);
}
} // namespace

//=================================================================================================

struct IndexBand
{
  IndexBand()
      : First(0),
        Last(0)
  {
  }

  IndexBand(int theFirtsIndex, int theLastIndex)
      : First(theFirtsIndex),
        Last(theLastIndex)
  {
  }

  int First;
  int Last;
};

//=================================================================================================

struct ThreadSolution
{
  ThreadSolution(int theTaskNum)
      : Shape1(0, theTaskNum - 1),
        Shape2(0, theTaskNum - 1),
        Dist(0, theTaskNum - 1)
  {
    Dist.Init(DBL_MAX);
  }

  NCollection_Array1<NCollection_Sequence<BRepExtrema_SolutionElem>> Shape1;
  NCollection_Array1<NCollection_Sequence<BRepExtrema_SolutionElem>> Shape2;
  NCollection_Array1<double>                                         Dist;
};

//=================================================================================================

struct VertexFunctor
{
  VertexFunctor(NCollection_Array1<IndexBand>* theBandArray, const Message_ProgressRange& theRange)
      : BandArray(theBandArray),
        Solution(theBandArray->Size()),
        Map1(nullptr),
        Map2(nullptr),
        Scope(theRange, "Vertices distances calculating", theBandArray->Size()),
        Ranges(0, theBandArray->Size() - 1),
        Eps(Precision::Confusion()),
        StartDist(0.0)
  {
    for (int i = 0; i < theBandArray->Size(); ++i)
    {
      Ranges.SetValue(i, Scope.Next());
    }
  }

  void operator()(const int theIndex) const
  {
    const int aCount2       = Map2->Extent();
    const int aFirst        = BandArray->Value(theIndex).First;
    const int aLast         = BandArray->Value(theIndex).Last;
    Solution.Dist[theIndex] = StartDist;

    Message_ProgressScope aScope(Ranges[theIndex], nullptr, (double)aLast - aFirst);

    for (int anIdx1 = aFirst; anIdx1 <= aLast; ++anIdx1)
    {
      if (!aScope.More())
      {
        break;
      }
      aScope.Next();

      const TopoDS_Vertex& aVertex1 = TopoDS::Vertex(Map1->FindKey(anIdx1));
      const gp_Pnt         aPoint1  = BRep_Tool::Pnt(aVertex1);
      for (int anIdx2 = 1; anIdx2 <= aCount2; ++anIdx2)
      {
        const TopoDS_Vertex& aVertex2 = TopoDS::Vertex(Map2->FindKey(anIdx2));
        const gp_Pnt         aPoint2  = BRep_Tool::Pnt(aVertex2);
        const double         aDist    = aPoint1.Distance(aPoint2);
        {
          if (aDist < Solution.Dist[theIndex] - Eps)
          {
            const BRepExtrema_SolutionElem Sol1(aDist, aPoint1, BRepExtrema_IsVertex, aVertex1);
            const BRepExtrema_SolutionElem Sol2(aDist, aPoint2, BRepExtrema_IsVertex, aVertex2);

            Solution.Shape1[theIndex].Clear();
            Solution.Shape2[theIndex].Clear();
            Solution.Shape1[theIndex].Append(Sol1);
            Solution.Shape2[theIndex].Append(Sol2);

            Solution.Dist[theIndex] = aDist;
          }
          else if (std::abs(aDist - Solution.Dist[theIndex]) < Eps)
          {
            const BRepExtrema_SolutionElem Sol1(aDist, aPoint1, BRepExtrema_IsVertex, aVertex1);
            const BRepExtrema_SolutionElem Sol2(aDist, aPoint2, BRepExtrema_IsVertex, aVertex2);
            Solution.Shape1[theIndex].Append(Sol1);
            Solution.Shape2[theIndex].Append(Sol2);

            if (Solution.Dist[theIndex] > aDist)
            {
              Solution.Dist[theIndex] = aDist;
            }
          }
        }
      }
    }
  }

  NCollection_Array1<IndexBand>*                                       BandArray;
  mutable ThreadSolution                                               Solution;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* Map1;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* Map2;
  Message_ProgressScope                                                Scope;
  NCollection_Array1<Message_ProgressRange>                            Ranges;
  double                                                               Eps;
  double                                                               StartDist;
};

//=================================================================================================

bool BRepExtrema_DistShapeShape::DistanceVertVert(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap2,
  const Message_ProgressRange&                                         theRange)
{
  const int                          aCount1      = theMap1.Extent();
  const int                          aMinTaskSize = aCount1 < 10 ? aCount1 : 10;
  const occ::handle<OSD_ThreadPool>& aThreadPool  = OSD_ThreadPool::DefaultPool();
  const int                          aNbThreads   = aThreadPool->NbThreads();
  int                                aNbTasks     = aNbThreads;
  int                                aTaskSize    = (int)std::ceil((double)aCount1 / aNbTasks);
  if (aTaskSize < aMinTaskSize)
  {
    aTaskSize = aMinTaskSize;
    aNbTasks  = (int)std::ceil((double)aCount1 / aTaskSize);
  }

  int                           aFirstIndex(1);
  NCollection_Array1<IndexBand> aBandArray(0, aNbTasks - 1);
  Message_ProgressScope         aDistScope(theRange, nullptr, 1);

  for (int anI = 0; anI < aBandArray.Size(); ++anI)
  {
    if (aCount1 < aFirstIndex + aTaskSize - 1)
    {
      aTaskSize = aCount1 - aFirstIndex + 1;
    }
    aBandArray.SetValue(anI, IndexBand(aFirstIndex, aFirstIndex + aTaskSize - 1));
    aFirstIndex += aTaskSize;
  }

  VertexFunctor aFunctor(&aBandArray, aDistScope.Next());
  aFunctor.Map1      = &theMap1;
  aFunctor.Map2      = &theMap2;
  aFunctor.StartDist = myDistRef;
  aFunctor.Eps       = myEps;

  OSD_Parallel::For(0, aNbTasks, aFunctor, !myIsMultiThread);
  if (!aDistScope.More())
  {
    return false;
  }
  for (int anI = 0; anI < aFunctor.Solution.Dist.Size(); ++anI)
  {
    double aDist = aFunctor.Solution.Dist[anI];
    if (aDist < myDistRef - myEps)
    {
      mySolutionsShape1.Clear();
      mySolutionsShape2.Clear();
      mySolutionsShape1.Append(aFunctor.Solution.Shape1[anI]);
      mySolutionsShape2.Append(aFunctor.Solution.Shape2[anI]);
      myDistRef = aDist;
    }
    else if (std::abs(aDist - myDistRef) < myEps)
    {
      mySolutionsShape1.Append(aFunctor.Solution.Shape1[anI]);
      mySolutionsShape2.Append(aFunctor.Solution.Shape2[anI]);
      myDistRef = aDist;
    }
  }
  return true;
}

//=================================================================================================

struct DistanceFunctor
{
  DistanceFunctor(NCollection_Array1<NCollection_Array1<BRepExtrema_CheckPair>>* theArrayOfArrays,
                  const Message_ProgressRange&                                   theRange)
      : ArrayOfArrays(theArrayOfArrays),
        Solution(ArrayOfArrays->Size()),
        Map1(nullptr),
        Map2(nullptr),
        LBox1(nullptr),
        LBox2(nullptr),
        Scope(theRange, "Shapes distances calculating", theArrayOfArrays->Size()),
        Ranges(0, theArrayOfArrays->Size() - 1),
        Eps(Precision::Confusion()),
        StartDist(0.0)
  {
    for (int i = 0; i < theArrayOfArrays->Size(); ++i)
    {
      Ranges.SetValue(i, Scope.Next());
    }
  }

  void operator()(const int theIndex) const
  {
    Message_ProgressScope aScope(Ranges[theIndex], nullptr, ArrayOfArrays->Value(theIndex).Size());
    Solution.Dist[theIndex] = StartDist;
    for (int i = 0; i < ArrayOfArrays->Value(theIndex).Size(); i++)
    {
      if (!aScope.More())
      {
        return;
      }
      aScope.Next();
      const BRepExtrema_CheckPair& aPair = ArrayOfArrays->Value(theIndex).Value(i);
      if (aPair.Distance > Solution.Dist[theIndex] + Eps)
      {
        break; // early search termination
      }
      const Bnd_Box&         aBox1   = LBox1->Value(aPair.Index1);
      const Bnd_Box&         aBox2   = LBox2->Value(aPair.Index2);
      const TopoDS_Shape&    aShape1 = Map1->FindKey(aPair.Index1);
      const TopoDS_Shape&    aShape2 = Map2->FindKey(aPair.Index2);
      BRepExtrema_DistanceSS aDistTool(aShape1,
                                       aShape2,
                                       aBox1,
                                       aBox2,
                                       Solution.Dist[theIndex],
                                       Eps);
      const double           aDist = aDistTool.DistValue();
      if (aDistTool.IsDone())
      {
        if (aDist < Solution.Dist[theIndex] - Eps)
        {
          Solution.Shape1[theIndex].Clear();
          Solution.Shape2[theIndex].Clear();

          NCollection_Sequence<BRepExtrema_SolutionElem> aSeq1 = aDistTool.Seq1Value();
          NCollection_Sequence<BRepExtrema_SolutionElem> aSeq2 = aDistTool.Seq2Value();

          Solution.Shape1[theIndex].Append(aSeq1);
          Solution.Shape2[theIndex].Append(aSeq2);

          Solution.Dist[theIndex] = aDistTool.DistValue();
        }
        else if (std::abs(aDist - Solution.Dist[theIndex]) < Eps)
        {
          NCollection_Sequence<BRepExtrema_SolutionElem> aSeq1 = aDistTool.Seq1Value();
          NCollection_Sequence<BRepExtrema_SolutionElem> aSeq2 = aDistTool.Seq2Value();

          Solution.Shape1[theIndex].Append(aSeq1);
          Solution.Shape2[theIndex].Append(aSeq2);
          if (Solution.Dist[theIndex] > aDist)
          {
            Solution.Dist[theIndex] = aDist;
          }
        }
      }
    }
  }

  NCollection_Array1<NCollection_Array1<BRepExtrema_CheckPair>>*       ArrayOfArrays;
  mutable ThreadSolution                                               Solution;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* Map1;
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>* Map2;
  const NCollection_Array1<Bnd_Box>*                                   LBox1;
  const NCollection_Array1<Bnd_Box>*                                   LBox2;
  Message_ProgressScope                                                Scope;
  NCollection_Array1<Message_ProgressRange>                            Ranges;
  double                                                               Eps;
  double                                                               StartDist;
};

//=================================================================================================

struct DistancePairFunctor
{
  DistancePairFunctor(NCollection_Array1<IndexBand>* theBandArray,
                      const Message_ProgressRange&   theRange)
      : BandArray(theBandArray),
        PairList(0, theBandArray->Size() - 1),
        LBox1(nullptr),
        LBox2(nullptr),
        Scope(theRange, "Boxes distances calculating", theBandArray->Size()),
        Ranges(0, theBandArray->Size() - 1),
        DistRef(0),
        Eps(Precision::Confusion())
  {
    for (int i = 0; i < theBandArray->Size(); ++i)
    {
      Ranges.SetValue(i, Scope.Next());
    }
  }

  void operator()(const int theIndex) const
  {
    const int aFirst = BandArray->Value(theIndex).First;
    const int aLast  = BandArray->Value(theIndex).Last;

    Message_ProgressScope aScope(Ranges[theIndex], nullptr, (double)aLast - aFirst);

    for (int anIdx1 = aFirst; anIdx1 <= aLast; ++anIdx1)
    {
      if (!aScope.More())
      {
        break;
      }
      aScope.Next();

      for (int anIdx2 = 1; anIdx2 <= LBox2->Size(); ++anIdx2)
      {
        const Bnd_Box& aBox1 = LBox1->Value(anIdx1);
        const Bnd_Box& aBox2 = LBox2->Value(anIdx2);
        if (aBox1.IsVoid() || aBox2.IsVoid())
        {
          continue;
        }

        const double aDist = aBox1.Distance(aBox2);
        if (aDist - DistRef < Eps)
        {
          PairList[theIndex].Append(BRepExtrema_CheckPair(anIdx1, anIdx2, aDist));
        }
      }
    }
  }

  int ListSize()
  {
    int aSize(0);
    for (int anI = PairList.Lower(); anI <= PairList.Upper(); ++anI)
    {
      aSize += PairList[anI].Size();
    }
    return aSize;
  }

  NCollection_Array1<IndexBand>*                                        BandArray;
  mutable NCollection_Array1<NCollection_Vector<BRepExtrema_CheckPair>> PairList;
  const NCollection_Array1<Bnd_Box>*                                    LBox1;
  const NCollection_Array1<Bnd_Box>*                                    LBox2;
  Message_ProgressScope                                                 Scope;
  NCollection_Array1<Message_ProgressRange>                             Ranges;
  double                                                                DistRef;
  double                                                                Eps;
};

//=================================================================================================

bool BRepExtrema_DistShapeShape::DistanceMapMap(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap1,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap2,
  const NCollection_Array1<Bnd_Box>&                                   theLBox1,
  const NCollection_Array1<Bnd_Box>&                                   theLBox2,
  const Message_ProgressRange&                                         theRange)
{
  const int aCount1 = theMap1.Extent();
  const int aCount2 = theMap2.Extent();

  if (aCount1 == 0 || aCount2 == 0)
  {
    return true;
  }

  Message_ProgressScope aTwinScope(theRange, nullptr, 1.0);

  const occ::handle<OSD_ThreadPool>& aThreadPool      = OSD_ThreadPool::DefaultPool();
  const int                          aNbThreads       = aThreadPool->NbThreads();
  const int                          aMinPairTaskSize = aCount1 < 10 ? aCount1 : 10;
  int                                aNbPairTasks     = aNbThreads;
  int                                aPairTaskSize = (int)std::ceil((double)aCount1 / aNbPairTasks);
  if (aPairTaskSize < aMinPairTaskSize)
  {
    aPairTaskSize = aMinPairTaskSize;
    aNbPairTasks  = (int)std::ceil((double)aCount1 / aPairTaskSize);
  }

  int                           aFirstIndex(1);
  NCollection_Array1<IndexBand> aBandArray(0, aNbPairTasks - 1);

  for (int anI = 0; anI < aBandArray.Size(); ++anI)
  {
    if (aCount1 < aFirstIndex + aPairTaskSize - 1)
    {
      aPairTaskSize = aCount1 - aFirstIndex + 1;
    }
    aBandArray.SetValue(anI, IndexBand(aFirstIndex, aFirstIndex + aPairTaskSize - 1));
    aFirstIndex += aPairTaskSize;
  }

  aTwinScope.Next(0.15);
  DistancePairFunctor aPairFunctor(&aBandArray, aTwinScope.Next(0.15));
  aPairFunctor.LBox1   = &theLBox1;
  aPairFunctor.LBox2   = &theLBox2;
  aPairFunctor.DistRef = myDistRef;
  aPairFunctor.Eps     = myEps;

  OSD_Parallel::For(0, aNbPairTasks, aPairFunctor, !myIsMultiThread);
  if (!aTwinScope.More())
  {
    return false;
  }
  int aListSize = aPairFunctor.ListSize();
  if (aListSize == 0)
  {
    return true;
  }
  NCollection_Array1<BRepExtrema_CheckPair> aPairList(0, aListSize - 1);
  int                                       aListIndex(0);
  for (int anI = 0; anI < aPairFunctor.PairList.Size(); ++anI)
  {
    for (int aJ = 0; aJ < aPairFunctor.PairList[anI].Size(); ++aJ)
    {
      aPairList[aListIndex] = aPairFunctor.PairList[anI][aJ];
      ++aListIndex;
    }
  }

  std::stable_sort(aPairList.begin(), aPairList.end(), BRepExtrema_CheckPair_Comparator);

  const int aMapSize  = aPairList.Size();
  int       aNbTasks  = aMapSize < aNbThreads ? aMapSize : aNbThreads;
  int       aTaskSize = (int)std::ceil((double)aMapSize / aNbTasks);

  NCollection_Array1<NCollection_Array1<BRepExtrema_CheckPair>> anArrayOfArray(0, aNbTasks - 1);
  // Since aPairList is sorted in ascending order of distances between Bnd_Boxes,
  // BRepExtrema_CheckPair are distributed to tasks one by one from smallest to largest,
  // and not ranges, as for DistancePairFunctor.
  // Since aMapSize may not be divisible entirely by the number of tasks,
  // some tasks should receive one BRepExtrema_CheckPair less than the rest.
  // aLastRowLimit defines the task number from which to start tasks containing
  // fewer BRepExtrema_CheckPair
  int aLastRowLimit = ((aMapSize % aNbTasks) == 0) ? aNbTasks : (aMapSize % aNbTasks);
  for (int anI = 0; anI < aTaskSize; ++anI)
  {
    for (int aJ = 0; aJ < aNbTasks; ++aJ)
    {
      if (anI == 0)
      {
        int aVectorSize = aTaskSize;
        if (aJ >= aLastRowLimit)
        {
          aVectorSize--;
        }
        anArrayOfArray[aJ].Resize(0, aVectorSize - 1, false);
      }
      if (anI < anArrayOfArray[aJ].Size())
      {
        anArrayOfArray[aJ][anI] = aPairList(anI * aNbTasks + aJ);
      }
      else
      {
        break;
      }
    }
  }
  DistanceFunctor aFunctor(&anArrayOfArray, aTwinScope.Next(0.85));
  aFunctor.Map1      = &theMap1;
  aFunctor.Map2      = &theMap2;
  aFunctor.LBox1     = &theLBox1;
  aFunctor.LBox2     = &theLBox2;
  aFunctor.Eps       = myEps;
  aFunctor.StartDist = myDistRef;

  OSD_Parallel::For(0, aNbTasks, aFunctor, !myIsMultiThread);
  if (!aTwinScope.More())
  {
    return false;
  }

  for (int anI = 0; anI < aFunctor.Solution.Dist.Size(); ++anI)
  {
    double aDist = aFunctor.Solution.Dist[anI];
    if (aDist < myDistRef - myEps)
    {
      mySolutionsShape1.Clear();
      mySolutionsShape2.Clear();
      mySolutionsShape1.Append(aFunctor.Solution.Shape1[anI]);
      mySolutionsShape2.Append(aFunctor.Solution.Shape2[anI]);
      myDistRef = aDist;
    }
    else if (std::abs(aDist - myDistRef) < myEps)
    {
      mySolutionsShape1.Append(aFunctor.Solution.Shape1[anI]);
      mySolutionsShape2.Append(aFunctor.Solution.Shape2[anI]);
      if (myDistRef > aDist)
      {
        myDistRef = aDist;
      }
    }
  }
  return true;
}

//=================================================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape()
    : myDistRef(0.0),
      myIsDone(false),
      myInnerSol(false),
      myEps(Precision::Confusion()),
      myIsInitS1(false),
      myIsInitS2(false),
      myFlag(Extrema_ExtFlag_MINMAX),
      myAlgo(Extrema_ExtAlgo_Grad),
      myIsMultiThread(false)
{
}

//=================================================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape&          Shape1,
                                                       const TopoDS_Shape&          Shape2,
                                                       const Extrema_ExtFlag        F,
                                                       const Extrema_ExtAlgo        A,
                                                       const Message_ProgressRange& theRange)
    : myDistRef(0.0),
      myIsDone(false),
      myInnerSol(false),
      myEps(Precision::Confusion()),
      myIsInitS1(false),
      myIsInitS2(false),
      myFlag(F),
      myAlgo(A),
      myIsMultiThread(false)
{
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform(theRange);
}

//=================================================================================================

BRepExtrema_DistShapeShape::BRepExtrema_DistShapeShape(const TopoDS_Shape&          Shape1,
                                                       const TopoDS_Shape&          Shape2,
                                                       const double                 theDeflection,
                                                       const Extrema_ExtFlag        F,
                                                       const Extrema_ExtAlgo        A,
                                                       const Message_ProgressRange& theRange)
    : myDistRef(0.0),
      myIsDone(false),
      myInnerSol(false),
      myEps(theDeflection),
      myIsInitS1(false),
      myIsInitS2(false),
      myFlag(F),
      myAlgo(A),
      myIsMultiThread(false)
{
  LoadS1(Shape1);
  LoadS2(Shape2);
  Perform(theRange);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::LoadS1(const TopoDS_Shape& Shape1)
{
  myShape1   = Shape1;
  myIsInitS1 = false;
  Decomposition(Shape1, myMapV1, myMapE1, myMapF1);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::LoadS2(const TopoDS_Shape& Shape2)
{
  myShape2   = Shape2;
  myIsInitS2 = false;
  Decomposition(Shape2, myMapV2, myMapE2, myMapF2);
}

//=================================================================================================

struct TreatmentFunctor
{
  TreatmentFunctor(NCollection_Array1<NCollection_Array1<TopoDS_Shape>>* theArrayOfArrays,
                   const Message_ProgressRange&                          theRange)
      : ArrayOfArrays(theArrayOfArrays),
        SolutionsShape1(nullptr),
        SolutionsShape2(nullptr),
        Scope(theRange, "Search for the inner solid", theArrayOfArrays->Size()),
        Ranges(0, theArrayOfArrays->Size() - 1),
        DistRef(nullptr),
        InnerSol(nullptr),
        IsDone(nullptr),
        Mutex()
  {
    for (int i = 0; i < theArrayOfArrays->Size(); ++i)
    {
      Ranges.SetValue(i, Scope.Next());
    }
  }

  void operator()(const int theIndex) const
  {
    const double          aTolerance = 0.001;
    Message_ProgressScope aScope(Ranges[theIndex], nullptr, ArrayOfArrays->Value(theIndex).Size());
    BRepClass3d_SolidClassifier aClassifier(Shape);

    for (int i = 0; i < ArrayOfArrays->Value(theIndex).Size(); i++)
    {
      if (!aScope.More())
      {
        break;
      }
      aScope.Next();
      if (IsDone->load(std::memory_order_acquire))
      {
        break;
      }

      const TopoDS_Vertex& aVertex = TopoDS::Vertex(ArrayOfArrays->Value(theIndex).Value(i));
      const gp_Pnt         aPnt    = BRep_Tool::Pnt(aVertex);
      aClassifier.Perform(aPnt, aTolerance);
      if (aClassifier.State() == TopAbs_IN)
      {
        std::unique_lock<std::mutex> aLock =
          Mutex ? std::unique_lock<std::mutex>(*Mutex) : std::unique_lock<std::mutex>();
        InnerSol->store(true, std::memory_order_release);
        *DistRef = 0.;
        IsDone->store(true, std::memory_order_release);
        BRepExtrema_SolutionElem aSolElem(0, aPnt, BRepExtrema_IsVertex, aVertex);
        SolutionsShape1->Append(aSolElem);
        SolutionsShape2->Append(aSolElem);
        break;
      }
    }
  }

  NCollection_Array1<NCollection_Array1<TopoDS_Shape>>* ArrayOfArrays;
  NCollection_Sequence<BRepExtrema_SolutionElem>*       SolutionsShape1;
  NCollection_Sequence<BRepExtrema_SolutionElem>*       SolutionsShape2;
  TopoDS_Shape                                          Shape;
  Message_ProgressScope                                 Scope;
  NCollection_Array1<Message_ProgressRange>             Ranges;
  double*                                               DistRef;
  std::atomic<bool>*                                    InnerSol;
  std::atomic<bool>*                                    IsDone;
  std::unique_ptr<std::mutex>                           Mutex;
};

//=================================================================================================

bool BRepExtrema_DistShapeShape::SolidTreatment(
  const TopoDS_Shape&                                                  theShape,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theVertexMap,
  const Message_ProgressRange&                                         theRange)
{
  const int                          aMapSize     = theVertexMap.Extent();
  const int                          aMinTaskSize = 3;
  const occ::handle<OSD_ThreadPool>& aThreadPool  = OSD_ThreadPool::DefaultPool();
  const int                          aNbThreads   = aThreadPool->NbThreads();
  int                                aNbTasks     = aNbThreads * 10;
  int                                aTaskSize    = (int)std::ceil((double)aMapSize / aNbTasks);
  if (aTaskSize < aMinTaskSize)
  {
    aTaskSize = aMinTaskSize;
    aNbTasks  = (int)std::ceil((double)aMapSize / aTaskSize);
  }

  NCollection_Array1<NCollection_Array1<TopoDS_Shape>> anArrayOfArray(0, aNbTasks - 1);
  for (int anI = 1; anI <= aMapSize; ++anI)
  {
    int aVectIndex  = (anI - 1) / aTaskSize;
    int aShapeIndex = (anI - 1) % aTaskSize;
    if (aShapeIndex == 0)
    {
      int aVectorSize = aTaskSize;
      int aTailSize   = aMapSize - aVectIndex * aTaskSize;
      if (aTailSize < aTaskSize)
      {
        aVectorSize = aTailSize;
      }
      anArrayOfArray[aVectIndex].Resize(0, aVectorSize - 1, false);
    }
    anArrayOfArray[aVectIndex][aShapeIndex] = theVertexMap(anI);
  }

  // Create local atomic variables for thread-safe communication during parallel section
  std::atomic<bool> anAtomicInnerSol(myInnerSol);
  std::atomic<bool> anAtomicIsDone(myIsDone);

  Message_ProgressScope aScope(theRange, "Solid treatment", aNbTasks);
  TreatmentFunctor      aFunctor(&anArrayOfArray, aScope.Next());
  aFunctor.SolutionsShape1 = &mySolutionsShape1;
  aFunctor.SolutionsShape2 = &mySolutionsShape2;
  aFunctor.Shape           = theShape;
  aFunctor.DistRef         = &myDistRef;
  aFunctor.InnerSol        = &anAtomicInnerSol;
  aFunctor.IsDone          = &anAtomicIsDone;
  if (myIsMultiThread && !aFunctor.Mutex)
  {
    aFunctor.Mutex = std::make_unique<std::mutex>();
  }

  OSD_Parallel::For(0, aNbTasks, aFunctor, !myIsMultiThread);

  // Copy atomic results back to class members after parallel section completes
  myInnerSol = anAtomicInnerSol.load(std::memory_order_acquire);
  myIsDone   = anAtomicIsDone.load(std::memory_order_acquire);

  if (!aScope.More())
  {
    return false;
  }
  return true;
}

//=================================================================================================

bool BRepExtrema_DistShapeShape::Perform(const Message_ProgressRange& theRange)
{
  myIsDone   = false;
  myInnerSol = false;
  mySolutionsShape1.Clear();
  mySolutionsShape2.Clear();

  if (myShape1.IsNull() || myShape2.IsNull())
    return false;

  // Treatment of solids
  bool anIsSolid1 =
    (myShape1.ShapeType() == TopAbs_SOLID) || (myShape1.ShapeType() == TopAbs_COMPSOLID);
  bool anIsSolid2 =
    (myShape2.ShapeType() == TopAbs_SOLID) || (myShape2.ShapeType() == TopAbs_COMPSOLID);
  int aRootStepsNum = 9; // By num of DistanceMapMap calls
  aRootStepsNum     = anIsSolid1 ? aRootStepsNum + 1 : aRootStepsNum;
  aRootStepsNum     = anIsSolid2 ? aRootStepsNum + 1 : aRootStepsNum;
  Message_ProgressScope aRootScope(theRange, "calculating distance", aRootStepsNum);

  if (anIsSolid1)
  {
    if (!SolidTreatment(myShape1, myMapV2, aRootScope.Next()))
    {
      return false;
    }
  }

  if (anIsSolid2 && (!myInnerSol))
  {
    if (!SolidTreatment(myShape2, myMapV1, aRootScope.Next()))
    {
      return false;
    }
  }

  if (!myInnerSol)
  {
    if (!myIsInitS1) // rebuild cached data for 1st shape
    {
      if (!myMapV1.IsEmpty())
      {
        myBV1.Resize(1, myMapV1.Extent(), false);
      }
      if (!myMapE1.IsEmpty())
      {
        myBE1.Resize(1, myMapE1.Extent(), false);
      }
      if (!myMapF1.IsEmpty())
      {
        myBF1.Resize(1, myMapF1.Extent(), false);
      }

      BoxCalculation(myMapV1, myBV1);
      BoxCalculation(myMapE1, myBE1);
      BoxCalculation(myMapF1, myBF1);

      myIsInitS1 = true;
    }

    if (!myIsInitS2) // rebuild cached data for 2nd shape
    {
      if (!myMapV2.IsEmpty())
      {
        myBV2.Resize(1, myMapV2.Extent(), false);
      }
      if (!myMapE2.IsEmpty())
      {
        myBE2.Resize(1, myMapE2.Extent(), false);
      }
      if (!myMapF2.IsEmpty())
      {
        myBF2.Resize(1, myMapF2.Extent(), false);
      }

      BoxCalculation(myMapV2, myBV2);
      BoxCalculation(myMapE2, myBE2);
      BoxCalculation(myMapF2, myBF2);

      myIsInitS2 = true;
    }

    if (myMapV1.Extent() && myMapV2.Extent())
    {
      const TopoDS_Vertex& V1 = TopoDS::Vertex(myMapV1(1));
      const TopoDS_Vertex& V2 = TopoDS::Vertex(myMapV2(1));
      myDistRef               = DistanceInitiale(V1, V2);
    }
    else
      myDistRef = 1.e30; // szv:!!!

    if (!DistanceVertVert(myMapV1, myMapV2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapV1, myMapE2, myBV1, myBE2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapE1, myMapV2, myBE1, myBV2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapV1, myMapF2, myBV1, myBF2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapF1, myMapV2, myBF1, myBV2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapE1, myMapE2, myBE1, myBE2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapE1, myMapF2, myBE1, myBF2, aRootScope.Next()))
    {
      return false;
    }
    if (!DistanceMapMap(myMapF1, myMapE2, myBF1, myBE2, aRootScope.Next()))
    {
      return false;
    }

    if (std::abs(myDistRef) > myEps)
    {
      if (!DistanceMapMap(myMapF1, myMapF2, myBF1, myBF2, aRootScope.Next()))
      {
        return false;
      }
    }

    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:03 2001 Begin
    int i = 1;
    for (; i <= mySolutionsShape1.Length(); i++)
      if (mySolutionsShape1.Value(i).Dist() > myDistRef + myEps)
      {
        mySolutionsShape1.Remove(i);
        mySolutionsShape2.Remove(i);
      }
    //  Modified by Sergey KHROMOV - Tue Mar  6 11:55:04 2001 End
    myIsDone = (mySolutionsShape1.Length() > 0);
  }

  return myIsDone;
}

//=================================================================================================

double BRepExtrema_DistShapeShape::Value() const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::Value: There's no solution ");

  return myDistRef;
}

//=================================================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape1(const int N) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::SupportOnShape1: There's no solution ");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape1.Value(N);
  switch (sol.SupportKind())
  {
    case BRepExtrema_IsVertex:
      return sol.Vertex();
    case BRepExtrema_IsOnEdge:
      return sol.Edge();
    case BRepExtrema_IsInFace:
      return sol.Face();
  }
  return TopoDS_Shape();
}

//=================================================================================================

TopoDS_Shape BRepExtrema_DistShapeShape::SupportOnShape2(const int N) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::SupportOnShape2: There's no solution ");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape2.Value(N);
  switch (sol.SupportKind())
  {
    case BRepExtrema_IsVertex:
      return sol.Vertex();
    case BRepExtrema_IsOnEdge:
      return sol.Edge();
    case BRepExtrema_IsInFace:
      return sol.Face();
  }
  return TopoDS_Shape();
}

//=================================================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS1(const int N, double& t) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnEdgeS1: There's no solution");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    throw BRepExtrema_UnCompatibleShape(
      "BRepExtrema_DistShapeShape::ParOnEdgeS1: ParOnEdgeS1 is impossible without EDGE");

  sol.EdgeParameter(t);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::ParOnEdgeS2(const int N, double& t) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnEdgeS2: There's no solution");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsOnEdge)
    throw BRepExtrema_UnCompatibleShape(
      "BRepExtrema_DistShapeShape::ParOnEdgeS2: ParOnEdgeS2 is impossible without EDGE");

  sol.EdgeParameter(t);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS1(const int N, double& u, double& v) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnFaceS1: There's no solution");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape1.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    throw BRepExtrema_UnCompatibleShape(
      "BRepExtrema_DistShapeShape::ParOnFaceS1: ParOnFaceS1 is impossible without FACE");

  sol.FaceParameter(u, v);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::ParOnFaceS2(const int N, double& u, double& v) const
{
  if (!myIsDone)
    throw StdFail_NotDone("BRepExtrema_DistShapeShape::ParOnFaceS2: There's no solution");

  const BRepExtrema_SolutionElem& sol = mySolutionsShape2.Value(N);
  if (sol.SupportKind() != BRepExtrema_IsInFace)
    throw BRepExtrema_UnCompatibleShape(
      "BRepExtrema_DistShapeShape::ParOnFaceS2:ParOnFaceS2 is impossible without FACE ");

  sol.FaceParameter(u, v);
}

//=================================================================================================

void BRepExtrema_DistShapeShape::Dump(Standard_OStream& o) const
{
  int    i;
  double r1, r2;

  o << "the distance  value is :  " << Value() << std::endl;
  o << "the number of solutions is :" << NbSolution() << std::endl;
  o << std::endl;
  for (i = 1; i <= NbSolution(); i++)
  {
    o << "solution number " << i << ": " << std::endl;
    o << "the type of the solution on the first shape is " << int(SupportTypeShape1(i))
      << std::endl;
    o << "the type of the solution on the second shape is " << int(SupportTypeShape2(i))
      << std::endl;
    o << "the coordinates of  the point on the first shape are: " << std::endl;
    o << "X=" << PointOnShape1(i).X() << " Y=" << PointOnShape1(i).Y()
      << " Z=" << PointOnShape1(i).Z() << std::endl;
    o << "the coordinates of  the point on the second shape are: " << std::endl;
    o << "X=" << PointOnShape2(i).X() << " Y=" << PointOnShape2(i).Y()
      << " Z=" << PointOnShape2(i).Z() << std::endl;

    switch (SupportTypeShape1(i))
    {
      case BRepExtrema_IsVertex:
        break;
      case BRepExtrema_IsOnEdge:
        ParOnEdgeS1(i, r1);
        o << "parameter on the first edge :  t= " << r1 << std::endl;
        break;
      case BRepExtrema_IsInFace:
        ParOnFaceS1(i, r1, r2);
        o << "parameters on the first face :  u= " << r1 << " v=" << r2 << std::endl;
        break;
    }
    switch (SupportTypeShape2(i))
    {
      case BRepExtrema_IsVertex:
        break;
      case BRepExtrema_IsOnEdge:
        ParOnEdgeS2(i, r1);
        o << "parameter on the second edge : t=" << r1 << std::endl;
        break;
      case BRepExtrema_IsInFace:
        ParOnFaceS2(i, r1, r2);
        o << "parameters on the second face : u= " << r1 << " v=" << r2 << std::endl;
        break;
    }
    o << std::endl;
  }
}
