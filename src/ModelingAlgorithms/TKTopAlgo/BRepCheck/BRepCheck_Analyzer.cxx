// Created on: 1995-12-08
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

#include <BRepCheck_Analyzer.hxx>

#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Solid.hxx>
#include <BRepCheck_Vertex.hxx>
#include <BRepCheck_Wire.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Shared.hxx>
#include <OSD_Parallel.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>

#include <mutex>

//! Functor for multi-threaded execution.
class BRepCheck_ParallelAnalyzer
{
public:
  BRepCheck_ParallelAnalyzer(NCollection_Array1<NCollection_Array1<TopoDS_Shape>>& theArray,
                             const NCollection_IndexedDataMap<TopoDS_Shape, occ::handle<BRepCheck_Result>>&          theMap)
      : myArray(theArray),
        myMap(theMap)
  {
    //
  }

  void operator()(const int theVectorIndex) const
  {
    TopExp_Explorer exp;
    for (int aShapeIter = myArray[theVectorIndex].Lower();
         aShapeIter <= myArray[theVectorIndex].Upper();
         ++aShapeIter)
    {
      const TopoDS_Shape&             aShape  = myArray[theVectorIndex][aShapeIter];
      const TopAbs_ShapeEnum          aType   = aShape.ShapeType();
      const occ::handle<BRepCheck_Result>& aResult = myMap.FindFromKey(aShape);
      switch (aType)
      {
        case TopAbs_VERTEX: {
          // modified by NIZHNY-MKK  Wed May 19 16:56:16 2004.BEGIN
          // There is no need to check anything.
          //       if (aShape.IsSame(S)) {
          //  myMap(S)->Blind();
          //       }
          // modified by NIZHNY-MKK  Wed May 19 16:56:23 2004.END
          break;
        }
        case TopAbs_EDGE: {
          try
          {
            occ::handle<BRepCheck_Edge> aResEdge = occ::down_cast<BRepCheck_Edge>(aResult);
            const BRepCheck_Status ste =
              aResEdge->CheckPolygonOnTriangulation(TopoDS::Edge(aShape));
            if (ste != BRepCheck_NoError)
            {
              aResEdge->SetStatus(ste);
            }
          }
          catch (Standard_Failure const& anException)
          {
            (void)anException;
            if (!aResult.IsNull())
            {
              aResult->SetFailStatus(aShape);
            }
          }

          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapS;
          for (exp.Init(aShape, TopAbs_VERTEX); exp.More(); exp.Next())
          {
            const TopoDS_Shape&      aVertex      = exp.Current();
            occ::handle<BRepCheck_Result> aResOfVertex = myMap.FindFromKey(aVertex);
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add(aVertex))
              {
                aResOfVertex->InContext(aShape);
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus(aShape);
              }

              if (!aResOfVertex.IsNull())
              {
                aResOfVertex->SetFailStatus(aVertex);
                aResOfVertex->SetFailStatus(aShape);
              }
            }
          }
          break;
        }
        case TopAbs_WIRE: {
          break;
        }
        case TopAbs_FACE: {
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> MapS;
          for (exp.Init(aShape, TopAbs_VERTEX); exp.More(); exp.Next())
          {
            occ::handle<BRepCheck_Result> aFaceVertexRes = myMap.FindFromKey(exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add(exp.Current()))
              {
                aFaceVertexRes->InContext(aShape);
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus(aShape);
              }
              if (!aFaceVertexRes.IsNull())
              {
                aFaceVertexRes->SetFailStatus(exp.Current());
                aFaceVertexRes->SetFailStatus(aShape);
              }
            }
          }

          bool performwire        = true;
          bool isInvalidTolerance = false;
          MapS.Clear();
          for (exp.Init(aShape, TopAbs_EDGE); exp.More(); exp.Next())
          {
            const occ::handle<BRepCheck_Result>& aFaceEdgeRes = myMap.FindFromKey(exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add(exp.Current()))
              {
                aFaceEdgeRes->InContext(aShape);

                if (performwire)
                {
                  std::unique_lock<std::mutex> aLock =
                    aFaceEdgeRes->GetMutex()
                      ? std::unique_lock<std::mutex>(*aFaceEdgeRes->GetMutex())
                      : std::unique_lock<std::mutex>();
                  if (aFaceEdgeRes->IsStatusOnShape(aShape))
                  {
                    NCollection_List<BRepCheck_Status>::Iterator itl(aFaceEdgeRes->StatusOnShape(aShape));
                    for (; itl.More(); itl.Next())
                    {
                      const BRepCheck_Status ste = itl.Value();
                      if (ste == BRepCheck_NoCurveOnSurface
                          || ste == BRepCheck_InvalidCurveOnSurface || ste == BRepCheck_InvalidRange
                          || ste == BRepCheck_InvalidCurveOnClosedSurface)
                      {
                        performwire = false;
                        break;
                      }
                    }
                  }
                }
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus(aShape);
              }
              if (!aFaceEdgeRes.IsNull())
              {
                aFaceEdgeRes->SetFailStatus(exp.Current());
                aFaceEdgeRes->SetFailStatus(aShape);
              }
            }
          }

          bool orientofwires = performwire;
          for (exp.Init(aShape, TopAbs_WIRE); exp.More(); exp.Next())
          {
            const occ::handle<BRepCheck_Result>& aFaceWireRes = myMap.FindFromKey(exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              aFaceWireRes->InContext(aShape);

              if (orientofwires)
              {
                std::unique_lock<std::mutex> aLock =
                  aFaceWireRes->GetMutex() ? std::unique_lock<std::mutex>(*aFaceWireRes->GetMutex())
                                           : std::unique_lock<std::mutex>();
                if (aFaceWireRes->IsStatusOnShape(aShape))
                {
                  const NCollection_List<BRepCheck_Status>& aStatusList = aFaceWireRes->StatusOnShape(aShape);
                  NCollection_List<BRepCheck_Status>::Iterator itl(aStatusList);
                  for (; itl.More(); itl.Next())
                  {
                    BRepCheck_Status ste = itl.Value();
                    if (ste != BRepCheck_NoError)
                    {
                      orientofwires = false;
                      break;
                    }
                  }
                }
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus(aShape);
              }
              if (!aFaceWireRes.IsNull())
              {
                aFaceWireRes->SetFailStatus(exp.Current());
                aFaceWireRes->SetFailStatus(aShape);
              }
            }
          }

          try
          {
            OCC_CATCH_SIGNALS
            const occ::handle<BRepCheck_Face> aFaceRes = occ::down_cast<BRepCheck_Face>(aResult);
            if (isInvalidTolerance)
            {
              aFaceRes->SetStatus(BRepCheck_InvalidToleranceValue);
            }
            else if (performwire)
            {
              if (orientofwires)
              {
                aFaceRes->OrientationOfWires(true); // on enregistre
              }
              else
              {
                aFaceRes->SetUnorientable();
              }
            }
            else
            {
              aFaceRes->SetUnorientable();
            }
          }
          catch (Standard_Failure const& anException)
          {
            (void)anException;
            if (!aResult.IsNull())
            {
              aResult->SetFailStatus(aShape);
            }

            for (exp.Init(aShape, TopAbs_WIRE); exp.More(); exp.Next())
            {
              occ::handle<BRepCheck_Result> aFaceCatchRes = myMap.FindFromKey(exp.Current());
              if (!aFaceCatchRes.IsNull())
              {
                aFaceCatchRes->SetFailStatus(exp.Current());
                aFaceCatchRes->SetFailStatus(aShape);
                aResult->SetFailStatus(exp.Current());
              }
            }
          }
          break;
        }
        case TopAbs_SHELL: {
          break;
        }
        case TopAbs_SOLID: {
          exp.Init(aShape, TopAbs_SHELL);
          for (; exp.More(); exp.Next())
          {
            const TopoDS_Shape&      aShell    = exp.Current();
            occ::handle<BRepCheck_Result> aSolidRes = myMap.FindFromKey(aShell);
            try
            {
              OCC_CATCH_SIGNALS
              aSolidRes->InContext(aShape);
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus(aShape);
              }
              if (!aSolidRes.IsNull())
              {
                aSolidRes->SetFailStatus(exp.Current());
                aSolidRes->SetFailStatus(aShape);
              }
            }
          }
          break;
        }
        default: {
          break;
        }
      }
    }
  }

private:
  BRepCheck_ParallelAnalyzer& operator=(const BRepCheck_ParallelAnalyzer&) = delete;

private:
  NCollection_Array1<NCollection_Array1<TopoDS_Shape>>& myArray;
  const NCollection_IndexedDataMap<TopoDS_Shape, occ::handle<BRepCheck_Result>>&          myMap;
};

//=================================================================================================

void BRepCheck_Analyzer::Init(const TopoDS_Shape& theShape, const bool B)
{
  if (theShape.IsNull())
  {
    throw Standard_NullObject("BRepCheck_Analyzer::Init() - NULL shape");
  }

  myShape = theShape;
  myMap.Clear();
  Put(theShape, B);
  Perform();
}

//=================================================================================================

void BRepCheck_Analyzer::Put(const TopoDS_Shape& theShape, const bool B)
{
  if (myMap.Contains(theShape))
  {
    return;
  }

  occ::handle<BRepCheck_Result> HR;
  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX:
      HR = new BRepCheck_Vertex(TopoDS::Vertex(theShape));
      break;
    case TopAbs_EDGE:
      HR = new BRepCheck_Edge(TopoDS::Edge(theShape));
      occ::down_cast<BRepCheck_Edge>(HR)->GeometricControls(B);
      occ::down_cast<BRepCheck_Edge>(HR)->SetExactMethod(myIsExact);
      break;
    case TopAbs_WIRE:
      HR = new BRepCheck_Wire(TopoDS::Wire(theShape));
      occ::down_cast<BRepCheck_Wire>(HR)->GeometricControls(B);
      break;
    case TopAbs_FACE:
      HR = new BRepCheck_Face(TopoDS::Face(theShape));
      occ::down_cast<BRepCheck_Face>(HR)->GeometricControls(B);
      break;
    case TopAbs_SHELL:
      HR = new BRepCheck_Shell(TopoDS::Shell(theShape));
      break;
    case TopAbs_SOLID:
      HR = new BRepCheck_Solid(TopoDS::Solid(theShape));
      break;
    case TopAbs_COMPSOLID:
    case TopAbs_COMPOUND:
      break;
    default:
      break;
  }

  if (!HR.IsNull())
  {
    HR->SetParallel(myIsParallel);
  }
  myMap.Add(theShape, HR);

  for (TopoDS_Iterator theIterator(theShape); theIterator.More(); theIterator.Next())
  {
    Put(theIterator.Value(), B); // performs minimum on each shape
  }
}

//=================================================================================================

void BRepCheck_Analyzer::Perform()
{
  const int        aMapSize     = myMap.Size();
  const int        aMinTaskSize = 10;
  const occ::handle<OSD_ThreadPool>& aThreadPool  = OSD_ThreadPool::DefaultPool();
  const int        aNbThreads   = aThreadPool->NbThreads();
  int              aNbTasks     = aNbThreads * 10;
  int aTaskSize = (int)std::ceil((double)aMapSize / aNbTasks);
  if (aTaskSize < aMinTaskSize)
  {
    aTaskSize = aMinTaskSize;
    aNbTasks  = (int)std::ceil((double)aMapSize / aTaskSize);
  }

  NCollection_Array1<NCollection_Array1<TopoDS_Shape>> aArrayOfArray(0, aNbTasks - 1);
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
      aArrayOfArray[aVectIndex].Resize(0, aVectorSize - 1, false);
    }
    aArrayOfArray[aVectIndex][aShapeIndex] = myMap.FindKey(anI);
  }

  BRepCheck_ParallelAnalyzer aParallelAnalyzer(aArrayOfArray, myMap);
  OSD_Parallel::For(0, aArrayOfArray.Size(), aParallelAnalyzer, !myIsParallel);
}

//=================================================================================================

bool BRepCheck_Analyzer::IsValid(const TopoDS_Shape& S) const
{
  if (!myMap.FindFromKey(S).IsNull())
  {
    NCollection_List<BRepCheck_Status>::Iterator itl;
    itl.Initialize(myMap.FindFromKey(S)->Status());
    if (itl.Value() != BRepCheck_NoError)
    { // a voir
      return false;
    }
  }

  for (TopoDS_Iterator theIterator(S); theIterator.More(); theIterator.Next())
  {
    if (!IsValid(theIterator.Value()))
    {
      return false;
    }
  }

  switch (S.ShapeType())
  {
    case TopAbs_EDGE: {
      return ValidSub(S, TopAbs_VERTEX);
    }
      //    break;
    case TopAbs_FACE: {
      bool valid = ValidSub(S, TopAbs_WIRE);
      valid                  = valid && ValidSub(S, TopAbs_EDGE);
      valid                  = valid && ValidSub(S, TopAbs_VERTEX);
      return valid;
    }

      //    break;
    case TopAbs_SHELL:
      //    return ValidSub(S,TopAbs_FACE);
      break;
    case TopAbs_SOLID:
      //    return ValidSub(S,TopAbs_EDGE);
      //    break;
      return ValidSub(S, TopAbs_SHELL);
      break;
    default:
      break;
  }

  return true;
}

//=================================================================================================

bool BRepCheck_Analyzer::ValidSub(const TopoDS_Shape&    S,
                                              const TopAbs_ShapeEnum SubType) const
{
  NCollection_List<BRepCheck_Status>::Iterator itl;
  TopExp_Explorer                      exp;
  for (exp.Init(S, SubType); exp.More(); exp.Next())
  {
    //  for (TopExp_Explorer exp(S,SubType);exp.More(); exp.Next()) {
    const occ::handle<BRepCheck_Result>& RV = myMap.FindFromKey(exp.Current());
    for (RV->InitContextIterator(); RV->MoreShapeInContext(); RV->NextShapeInContext())
    {
      if (RV->ContextualShape().IsSame(S))
      {
        break;
      }
    }

    if (!RV->MoreShapeInContext())
      break;

    for (itl.Initialize(RV->StatusOnShape()); itl.More(); itl.Next())
    {
      if (itl.Value() != BRepCheck_NoError)
      {
        return false;
      }
    }
  }
  return true;
}
