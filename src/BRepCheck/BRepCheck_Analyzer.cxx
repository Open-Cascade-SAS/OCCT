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
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
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
#include <Standard_Mutex.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

//! Functor for multi-threaded execution.
class BRepCheck_ParallelAnalyzer
{
public:
  BRepCheck_ParallelAnalyzer (NCollection_Array1< NCollection_Array1<TopoDS_Shape> >& theArray,
                              const BRepCheck_IndexedDataMapOfShapeResult& theMap)
  : myArray (theArray),
    myMap (theMap)
  {
    //
  }

  void operator() (const Standard_Integer theVectorIndex) const
  {
    TopExp_Explorer exp;
    for (Standard_Integer aShapeIter = myArray[theVectorIndex].Lower();
         aShapeIter <= myArray[theVectorIndex].Upper(); ++aShapeIter)
    {
      const TopoDS_Shape& aShape = myArray[theVectorIndex][aShapeIter];
      const TopAbs_ShapeEnum aType = aShape.ShapeType();
      const Handle(BRepCheck_Result)& aResult = myMap.FindFromKey (aShape);
      switch (aType)
      {
        case TopAbs_VERTEX:
        {
          // modified by NIZHNY-MKK  Wed May 19 16:56:16 2004.BEGIN
          // There is no need to check anything.
          //       if (aShape.IsSame(S)) {
          //  myMap(S)->Blind();
          //       }
          // modified by NIZHNY-MKK  Wed May 19 16:56:23 2004.END
          break;
        }
        case TopAbs_EDGE:
        {
          try
          {
            Handle(BRepCheck_Edge) aResEdge = Handle(BRepCheck_Edge)::DownCast(aResult);
            const BRepCheck_Status ste = aResEdge->CheckPolygonOnTriangulation (TopoDS::Edge (aShape));
            if (ste != BRepCheck_NoError)
            {
              aResEdge->SetStatus (ste);
            }
          }
          catch (Standard_Failure const& anException)
          {
            (void)anException;
            if (!aResult.IsNull())
            {
              aResult->SetFailStatus (aShape);
            }
          }

          TopTools_MapOfShape MapS;
          for (exp.Init (aShape, TopAbs_VERTEX); exp.More(); exp.Next())
          {
            const TopoDS_Shape& aVertex = exp.Current();
            Handle(BRepCheck_Result) aResOfVertex = myMap.FindFromKey (aVertex);
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add (aVertex))
              {
                aResOfVertex->InContext (aShape);
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus (aShape);
              }

              if (!aResOfVertex.IsNull())
              {
                aResOfVertex->SetFailStatus (aVertex);
                aResOfVertex->SetFailStatus (aShape);
              }
            }
          }
          break;
        }
        case TopAbs_WIRE:
        {
          break;
        }
        case TopAbs_FACE:
        {
          TopTools_MapOfShape MapS;
          for (exp.Init (aShape, TopAbs_VERTEX); exp.More(); exp.Next())
          {
            Handle(BRepCheck_Result) aFaceVertexRes = myMap.FindFromKey (exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add (exp.Current()))
              {
                aFaceVertexRes->InContext (aShape);
              }
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus (aShape);
              }
              if (!aFaceVertexRes.IsNull())
              {
                aFaceVertexRes->SetFailStatus (exp.Current());
                aFaceVertexRes->SetFailStatus (aShape);
              }
            }
          }

          Standard_Boolean performwire = Standard_True;
          Standard_Boolean isInvalidTolerance = Standard_False;
          MapS.Clear();
          for (exp.Init (aShape, TopAbs_EDGE); exp.More(); exp.Next())
          {
            const Handle(BRepCheck_Result)& aFaceEdgeRes = myMap.FindFromKey (exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              if (MapS.Add (exp.Current()))
              {
                aFaceEdgeRes->InContext (aShape);

                if (performwire)
                {
                  Standard_Mutex::Sentry aLock (aFaceEdgeRes->GetMutex());
                  if (aFaceEdgeRes->IsStatusOnShape(aShape))
                  {
                    BRepCheck_ListIteratorOfListOfStatus itl (aFaceEdgeRes->StatusOnShape (aShape));
                    for (; itl.More(); itl.Next())
                    {
                      const BRepCheck_Status ste = itl.Value();
                      if (ste == BRepCheck_NoCurveOnSurface ||
                          ste == BRepCheck_InvalidCurveOnSurface ||
                          ste == BRepCheck_InvalidRange ||
                          ste == BRepCheck_InvalidCurveOnClosedSurface)
                      {
                        performwire = Standard_False;
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
                aResult->SetFailStatus (aShape);
              }
              if (!aFaceEdgeRes.IsNull())
              {
                aFaceEdgeRes->SetFailStatus (exp.Current());
                aFaceEdgeRes->SetFailStatus (aShape);
              }
            }
          }

          Standard_Boolean orientofwires = performwire;
          for (exp.Init (aShape, TopAbs_WIRE); exp.More(); exp.Next())
          {
            const Handle(BRepCheck_Result)& aFaceWireRes = myMap.FindFromKey (exp.Current());
            try
            {
              OCC_CATCH_SIGNALS
              aFaceWireRes->InContext (aShape);

              if (orientofwires)
              {
                Standard_Mutex::Sentry aLock (aFaceWireRes->GetMutex());
                if (aFaceWireRes->IsStatusOnShape (aShape))
                {
                  const BRepCheck_ListOfStatus& aStatusList = aFaceWireRes->StatusOnShape (aShape);
                  BRepCheck_ListIteratorOfListOfStatus itl (aStatusList);
                  for (; itl.More(); itl.Next())
                  {
                    BRepCheck_Status ste = itl.Value();
                    if (ste != BRepCheck_NoError)
                    {
                      orientofwires = Standard_False;
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
                aResult->SetFailStatus (aShape);
              }
              if (!aFaceWireRes.IsNull())
              {
                aFaceWireRes->SetFailStatus (exp.Current());
                aFaceWireRes->SetFailStatus (aShape);
              }
            }
          }

          try
          {
            OCC_CATCH_SIGNALS
            const Handle(BRepCheck_Face) aFaceRes = Handle(BRepCheck_Face)::DownCast(aResult);
            if (isInvalidTolerance)
            {
              aFaceRes->SetStatus (BRepCheck_InvalidToleranceValue);
            }
            else if (performwire)
            {
              if (orientofwires)
              {
                aFaceRes->OrientationOfWires (Standard_True);// on enregistre
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
              aResult->SetFailStatus (aShape);
            }

            for (exp.Init (aShape, TopAbs_WIRE); exp.More(); exp.Next())
            {
              Handle(BRepCheck_Result) aFaceCatchRes = myMap.FindFromKey (exp.Current());
              if (!aFaceCatchRes.IsNull())
              {
                aFaceCatchRes->SetFailStatus (exp.Current());
                aFaceCatchRes->SetFailStatus (aShape);
                aResult->SetFailStatus (exp.Current());
              }
            }
          }
          break;
        }
        case TopAbs_SHELL:
        {
          break;
        }
        case TopAbs_SOLID:
        {
          exp.Init (aShape, TopAbs_SHELL);
          for (; exp.More(); exp.Next())
          {
            const TopoDS_Shape& aShell = exp.Current();
            Handle(BRepCheck_Result) aSolidRes = myMap.FindFromKey (aShell);
            try
            {
              OCC_CATCH_SIGNALS
              aSolidRes->InContext (aShape);
            }
            catch (Standard_Failure const& anException)
            {
              (void)anException;
              if (!aResult.IsNull())
              {
                aResult->SetFailStatus (aShape);
              }
              if (!aSolidRes.IsNull())
              {
                aSolidRes->SetFailStatus (exp.Current());
                aSolidRes->SetFailStatus (aShape);
              }
            }
          }
          break;
        }
        default:
        {
          break;
        }
      }
    }
  }

private:
  BRepCheck_ParallelAnalyzer& operator=(const BRepCheck_ParallelAnalyzer&) Standard_DELETE;

private:
  NCollection_Array1< NCollection_Array1<TopoDS_Shape> >& myArray;
  const BRepCheck_IndexedDataMapOfShapeResult& myMap;
};

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void BRepCheck_Analyzer::Init (const TopoDS_Shape& theShape,
                               const Standard_Boolean B)
{
  if (theShape.IsNull())
  {
    throw Standard_NullObject ("BRepCheck_Analyzer::Init() - NULL shape");
  }

  myShape = theShape;
  myMap.Clear();
  Put (theShape, B);
  Perform();
}

//=======================================================================
//function : Put
//purpose  :
//=======================================================================
void BRepCheck_Analyzer::Put (const TopoDS_Shape& theShape,
                              const Standard_Boolean B)
{
  if (myMap.Contains (theShape))
  {
    return;
  }

  Handle(BRepCheck_Result) HR;
  switch (theShape.ShapeType())
  {
    case TopAbs_VERTEX:
      HR = new BRepCheck_Vertex (TopoDS::Vertex (theShape));
      break;
    case TopAbs_EDGE:
      HR = new BRepCheck_Edge (TopoDS::Edge (theShape));
      Handle(BRepCheck_Edge)::DownCast(HR)->GeometricControls (B);
      Handle(BRepCheck_Edge)::DownCast(HR)->SetExactMethod(myIsExact);
      break;
    case TopAbs_WIRE:
      HR = new BRepCheck_Wire (TopoDS::Wire (theShape));
      Handle(BRepCheck_Wire)::DownCast(HR)->GeometricControls (B);
      break;
    case TopAbs_FACE:
      HR = new BRepCheck_Face (TopoDS::Face (theShape));
      Handle(BRepCheck_Face)::DownCast(HR)->GeometricControls (B);
      break;
    case TopAbs_SHELL:
      HR = new BRepCheck_Shell (TopoDS::Shell (theShape));
      break;
    case TopAbs_SOLID:
      HR = new BRepCheck_Solid (TopoDS::Solid (theShape));
      break;
    case TopAbs_COMPSOLID:
    case TopAbs_COMPOUND:
      break;
    default:
      break;
  }

  if (!HR.IsNull())
  {
    HR->SetParallel (myIsParallel);
  }
  myMap.Add (theShape, HR);

  for (TopoDS_Iterator theIterator (theShape); theIterator.More(); theIterator.Next())
  {
    Put (theIterator.Value(), B); // performs minimum on each shape
  }
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void BRepCheck_Analyzer::Perform()
{
  const Standard_Integer aMapSize = myMap.Size();
  const Standard_Integer aMinTaskSize = 10;
  const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
  const Standard_Integer aNbThreads = aThreadPool->NbThreads();
  Standard_Integer aNbTasks = aNbThreads * 10;
  Standard_Integer aTaskSize = (Standard_Integer)Ceiling ((double)aMapSize / aNbTasks);
  if (aTaskSize < aMinTaskSize)
  {
    aTaskSize = aMinTaskSize;
    aNbTasks = (Standard_Integer)Ceiling ((double)aMapSize / aTaskSize);
  }

  NCollection_Array1< NCollection_Array1<TopoDS_Shape> > aArrayOfArray (0, aNbTasks - 1);
  for (Standard_Integer anI = 1; anI <= aMapSize; ++anI)
  {
    Standard_Integer aVectIndex  = (anI-1) / aTaskSize;
    Standard_Integer aShapeIndex = (anI-1) % aTaskSize;
    if (aShapeIndex == 0)
    {
      Standard_Integer aVectorSize = aTaskSize;
      Standard_Integer aTailSize = aMapSize - aVectIndex * aTaskSize;
      if (aTailSize < aTaskSize)
      {
        aVectorSize = aTailSize;
      }
      aArrayOfArray[aVectIndex].Resize (0, aVectorSize - 1, Standard_False);
    }
    aArrayOfArray[aVectIndex][aShapeIndex] = myMap.FindKey (anI);
  }

  BRepCheck_ParallelAnalyzer aParallelAnalyzer (aArrayOfArray, myMap);
  OSD_Parallel::For (0, aArrayOfArray.Size(), aParallelAnalyzer, !myIsParallel);
}

//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::IsValid(const TopoDS_Shape& S) const
{
  if (!myMap.FindFromKey (S).IsNull())
  {
    BRepCheck_ListIteratorOfListOfStatus itl;
    itl.Initialize (myMap.FindFromKey (S)->Status());
    if (itl.Value() != BRepCheck_NoError) { // a voir
      return Standard_False;
    }
  }

  for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
    if (!IsValid(theIterator.Value())) {
      return Standard_False;
    }
  }

  switch (S.ShapeType()) {
  case TopAbs_EDGE:
    {
      return ValidSub(S,TopAbs_VERTEX);
    }
//    break;
  case TopAbs_FACE:
    {
      Standard_Boolean valid = ValidSub(S,TopAbs_WIRE);
      valid = valid && ValidSub(S,TopAbs_EDGE);
      valid = valid && ValidSub(S,TopAbs_VERTEX);
      return valid;
    }

//    break;
  case TopAbs_SHELL:
//    return ValidSub(S,TopAbs_FACE);
    break;
  case TopAbs_SOLID:
//    return ValidSub(S,TopAbs_EDGE);
//    break;
    return ValidSub(S,TopAbs_SHELL);
    break;
  default:
    break;
  }

  return Standard_True;
}

//=======================================================================
//function : ValidSub
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::ValidSub
   (const TopoDS_Shape& S,
    const TopAbs_ShapeEnum SubType) const
{
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(S,SubType);exp.More(); exp.Next()) {
//  for (TopExp_Explorer exp(S,SubType);exp.More(); exp.Next()) {
    const Handle(BRepCheck_Result)& RV = myMap.FindFromKey(exp.Current());
    for (RV->InitContextIterator();
         RV->MoreShapeInContext(); 
         RV->NextShapeInContext()) {
      if (RV->ContextualShape().IsSame(S)) {
        break;
      }
    }

    if(!RV->MoreShapeInContext()) break;

    for (itl.Initialize(RV->StatusOnShape()); itl.More(); itl.Next()) {
      if (itl.Value() != BRepCheck_NoError) {
        return Standard_False;
      }
    }
  }
  return Standard_True ;
}
