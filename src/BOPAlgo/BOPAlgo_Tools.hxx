// Created by: Peter KURNEV
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

#ifndef _BOPAlgo_Tools_HeaderFile
#define _BOPAlgo_Tools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPDS_IndexedDataMapOfPaveBlockListOfInteger.hxx>
#include <BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_PDS.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopTools_DataMapOfShapeBox.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeReal.hxx>
#include <TopTools_ListOfListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Integer.hxx>

class BOPDS_PaveBlock;
class BOPDS_CommonBlock;
class IntTools_Context;
class TopoDS_Shape;

//! Provides tools used in the intersection part of Boolean operations
class BOPAlgo_Tools
{
public:

  //! Makes the chains of the connected elements from the given connexity map
  template <class theType, class theTypeHasher>
  static void MakeBlocks(const NCollection_IndexedDataMap<theType, NCollection_List<theType>, theTypeHasher>& theMILI,
                         NCollection_List<NCollection_List<theType>>& theMBlocks,
                         const Handle(NCollection_BaseAllocator)& theAllocator)
  {
    NCollection_Map<theType, theTypeHasher> aMFence;
    Standard_Integer i, aNb = theMILI.Extent();
    for (i = 1; i <= aNb; ++i) {
      const theType& n = theMILI.FindKey(i);
      if (!aMFence.Add(n))
        continue;
      //
      // Start the chain
      NCollection_List<theType>& aChain = theMBlocks.Append(NCollection_List<theType>(theAllocator));
      aChain.Append(n);
      // Look for connected elements
      typename NCollection_List<theType>::Iterator aItLChain(aChain);
      for (; aItLChain.More(); aItLChain.Next()) {
        const theType& n1 = aItLChain.Value();
        const NCollection_List<theType>& aLI = theMILI.FindFromKey(n1);
        // Add connected elements into the chain
        typename NCollection_List<theType>::Iterator aItLI(aLI);
        for (; aItLI.More(); aItLI.Next()) {
          const theType& n2 = aItLI.Value();
          if (aMFence.Add(n2)) {
            aChain.Append(n2);
          }
        }
      }
    }
  }

  //! Fills the map with the connected entities
  template <class theType, class theTypeHasher>
  static void FillMap(const theType& n1,
                      const theType& n2,
                      NCollection_IndexedDataMap<theType, NCollection_List<theType>, theTypeHasher>& theMILI,
                      const Handle(NCollection_BaseAllocator)& theAllocator)
  {
    NCollection_List<theType> *pList1 = theMILI.ChangeSeek(n1);
    if (!pList1) {
      pList1 = &theMILI(theMILI.Add(n1, NCollection_List<theType>(theAllocator)));
    }
    pList1->Append(n2);
    //
    NCollection_List<theType> *pList2 = theMILI.ChangeSeek(n2);
    if (!pList2) {
      pList2 = &theMILI(theMILI.Add(n2, NCollection_List<theType>(theAllocator)));
    }
    pList2->Append(n1);
  }

  Standard_EXPORT static void FillMap(const Handle(BOPDS_PaveBlock)& thePB1,
                                      const Standard_Integer theF,
                                      BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theMILI,
                                      const Handle(NCollection_BaseAllocator)& theAllocator);
  
  Standard_EXPORT static void PerformCommonBlocks(BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& theMBlocks,
                                                  const Handle(NCollection_BaseAllocator)& theAllocator,
                                                  BOPDS_PDS& theDS);

  Standard_EXPORT static void PerformCommonBlocks(const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& theMBlocks,
                                                  const Handle(NCollection_BaseAllocator)& theAllocator,
                                                  BOPDS_PDS& pDS);

  Standard_EXPORT static Standard_Real ComputeToleranceOfCB
                                        (const Handle(BOPDS_CommonBlock)& theCB,
                                         const BOPDS_PDS theDS,
                                         const Handle(IntTools_Context)& theContext);

  //! Creates planar wires from the given edges.<br>
  //! The input edges are expected to be planar. And for the performance
  //! sake the method does not check if the edges are really planar.<br>
  //! Thus, the result wires will also be not planar if the input edges are not planar.<br>
  //! The edges may be not shared, but the resulting wires will be sharing the
  //! coinciding parts and intersecting parts.<br>
  //! The output wires may be non-manifold and contain free and multi-connected vertices.<br>
  //! Parameters:
  //! <theEdges> - input edges;<br>
  //! <theWires> - output wires;<br>
  //! <theShared> - boolean flag which defines whether the input edges are already
  //!               shared or have to be intersected;<br>
  //! <theAngTol> - the angular tolerance which will be used for distinguishing
  //!               the planes in which the edges are located. Default value is
  //!               1.e-8 which is used for intersection of planes in IntTools_FaceFace.<br>
  //! Method returns the following error statuses:<br>
  //! 0 - in case of success (at least one wire has been built);<br>
  //! 1 - in case there are no edges in the given shape;<br>
  //! 2 - sharing of the edges has failed.<br>
  Standard_EXPORT static Standard_Integer EdgesToWires(const TopoDS_Shape& theEdges,
                                                       TopoDS_Shape& theWires,
                                                       const Standard_Boolean theShared = Standard_False,
                                                       const Standard_Real theAngTol = 1.e-8);

  //! Creates planar faces from given planar wires.<br>
  //! The method does not check if the wires are really planar.<br>
  //! The input wires may be non-manifold but should be shared.<br>
  //! The wires located in the same planes and included into other wires will create
  //! holes in the faces built from outer wires.<br>
  //! The tolerance values of the input shapes may be modified during the operation
  //! due to projection of the edges on the planes for creation of 2D curves.<br>
  //! Parameters:
  //! <theWires> - the given wires;<br>
  //! <theFaces> - the output faces;<br>
  //! <theAngTol> - the angular tolerance for distinguishing the planes in which
  //!               the wires are located. Default value is 1.e-8 which is used
  //!               for intersection of planes in IntTools_FaceFace.<br>
  //! Method returns TRUE in case of success, i.e. at least one face has been built.<br>
  Standard_EXPORT static Standard_Boolean WiresToFaces(const TopoDS_Shape& theWires,
                                                       TopoDS_Shape& theFaces,
                                                       const Standard_Real theAngTol = 1.e-8);

  //! Finds chains of intersecting vertices
  Standard_EXPORT static void IntersectVertices(const TopTools_IndexedDataMapOfShapeReal& theVertices,
                                                const Standard_Boolean theRunParallel,
                                                const Standard_Real theFuzzyValue,
                                                TopTools_ListOfListOfShape& theChains);

  //! Collect in the output list recursively all non-compound subshapes of the first level
  //! of the given shape theS. If a shape presents in the map theMFence it is skipped.
  //! All shapes put in the output are also added into theMFence.
  Standard_EXPORT static void TreatCompound(const TopoDS_Shape& theS,
                                            TopTools_MapOfShape& theMFence,
                                            TopTools_ListOfShape& theLS);

  //! Classifies the faces <theFaces> relatively solids <theSolids>.
  //! The IN faces for solids are stored into output data map <theInParts>.
  //!
  //! The map <theSolidsIF> contains INTERNAL faces of the solids, to avoid
  //! their additional classification.
  //!
  //! Firstly, it checks the intersection of bounding boxes of the shapes.
  //! If the Box is not stored in the <theShapeBoxMap> map, it builds the box.
  //! If the bounding boxes of solid and face are interfering the classification is performed.
  //!
  //! It is assumed that all faces and solids are already intersected and
  //! do not have any geometrically coinciding parts without topological
  //! sharing of these parts
  Standard_EXPORT static void ClassifyFaces(const TopTools_ListOfShape& theFaces,
                                            const TopTools_ListOfShape& theSolids,
                                            const Standard_Boolean theRunParallel,
                                            Handle(IntTools_Context)& theContext,
                                            TopTools_IndexedDataMapOfShapeListOfShape& theInParts,
                                            const TopTools_DataMapOfShapeBox& theShapeBoxMap = TopTools_DataMapOfShapeBox(),
                                            const TopTools_DataMapOfShapeListOfShape& theSolidsIF = TopTools_DataMapOfShapeListOfShape());

};

#endif // _BOPAlgo_Tools_HeaderFile
