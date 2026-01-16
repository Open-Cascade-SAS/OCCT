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

#ifndef _BOPDS_DS_HeaderFile
#define _BOPDS_DS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <NCollection_DataMap.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <NCollection_Vector.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>

class BOPDS_IndexRange;
class BOPDS_ShapeInfo;
class TopoDS_Shape;
class BOPDS_PaveBlock;
class BOPDS_CommonBlock;
class BOPDS_FaceInfo;
class Bnd_Box;

//! The class BOPDS_DS provides the control
//! of data structure for the algorithms in the
//! Boolean Component such as General Fuse, Boolean operations,
//! Section, Maker Volume, Splitter and Cells Builder.
//!
//! The data structure has the following contents:
//! 1. the arguments of an operation [myArguments];
//! 2. the information about arguments/new shapes
//! and their sub-shapes (type of the shape,
//! bounding box, etc) [myLines];
//! 3. each argument shape(and its subshapes)
//! has/have own range of indices (rank);
//! 4. pave blocks on source edges [myPaveBlocksPool];
//! 5. the state of source faces [myFaceInfoPool];
//! 6. the collection of same domain shapes [myShapesSD];
//! 7. the collection of interferences [myInterfTB, myInterfVV,..myInterfFF]
class BOPDS_DS
{
public:
  DEFINE_STANDARD_ALLOC

  //! Empty constructor
  Standard_EXPORT BOPDS_DS();

  Standard_EXPORT virtual ~BOPDS_DS();

  //! Constructor
  //! @param theAllocator the allocator to manage the memory
  Standard_EXPORT BOPDS_DS(const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! Clears the contents
  Standard_EXPORT void Clear();

  //! Selector
  Standard_EXPORT const occ::handle<NCollection_BaseAllocator>& Allocator() const;

  //! Modifier
  //! Sets the arguments [theLS] of an operation
  Standard_EXPORT void SetArguments(const NCollection_List<TopoDS_Shape>& theLS);

  //! Selector
  //! Returns the arguments of an operation
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Arguments() const;

  //! Initializes the data structure for
  //! the arguments
  Standard_EXPORT void Init(const double theFuzz = Precision::Confusion());

  //! Selector
  //! Returns the total number of shapes stored
  Standard_EXPORT int NbShapes() const;

  //! Selector
  //! Returns the total number of source shapes stored
  Standard_EXPORT int NbSourceShapes() const;

  //! Selector
  //! Returns the number of index ranges
  Standard_EXPORT int NbRanges() const;

  //! Selector
  //! Returns the index range "i"
  Standard_EXPORT const BOPDS_IndexRange& Range(const int theIndex) const;

  //! Selector
  //! Returns the rank of the shape of index "i"
  Standard_EXPORT int Rank(const int theIndex) const;

  //! Returns true if the shape of index "i" is not
  //! the source shape/sub-shape
  Standard_EXPORT bool IsNewShape(const int theIndex) const;

  //! Modifier
  //! Appends the information about the shape [theSI]
  //! to the data structure
  //! Returns the index of theSI in the data structure
  Standard_EXPORT int Append(const BOPDS_ShapeInfo& theSI);

  //! Modifier
  //! Appends the default information about the shape [theS]
  //! to the data structure
  //! Returns the index of theS in the data structure
  Standard_EXPORT int Append(const TopoDS_Shape& theS);

  //! Selector
  //! Returns the information about the shape
  //! with index theIndex
  Standard_EXPORT const BOPDS_ShapeInfo& ShapeInfo(const int theIndex) const;

  //! Selector/Modifier
  //! Returns the information about the shape
  //! with index theIndex
  Standard_EXPORT BOPDS_ShapeInfo& ChangeShapeInfo(const int theIndex);

  //! Selector
  //! Returns the shape
  //! with index theIndex
  Standard_EXPORT const TopoDS_Shape& Shape(const int theIndex) const;

  //! Selector
  //! Returns the index of the shape theS
  Standard_EXPORT int Index(const TopoDS_Shape& theS) const;

  //! Selector
  //! Returns the information about pave blocks on source edges
  Standard_EXPORT const NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>&
                        PaveBlocksPool() const;

  //! Selector/Modifier
  //! Returns the information about pave blocks on source edges
  Standard_EXPORT NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>>&
                  ChangePaveBlocksPool();

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! information about pave blocks
  Standard_EXPORT bool HasPaveBlocks(const int theIndex) const;

  //! Selector
  //! Returns the pave blocks for the shape with index theIndex
  Standard_EXPORT const NCollection_List<occ::handle<BOPDS_PaveBlock>>& PaveBlocks(
    const int theIndex) const;

  //! Selector/Modifier
  //! Returns the pave blocks for the shape with index theIndex
  Standard_EXPORT NCollection_List<occ::handle<BOPDS_PaveBlock>>& ChangePaveBlocks(
    const int theIndex);

  //! Update the pave blocks for the all shapes in data structure
  Standard_EXPORT void UpdatePaveBlocks();

  //! Update the pave block thePB
  Standard_EXPORT void UpdatePaveBlock(const occ::handle<BOPDS_PaveBlock>& thePB);

  //! Update the common block theCB
  Standard_EXPORT void UpdateCommonBlock(const occ::handle<BOPDS_CommonBlock>& theCB,
                                         const double                          theFuzz);

  //! Query
  //! Returns true if the pave block is common block
  Standard_EXPORT bool IsCommonBlock(const occ::handle<BOPDS_PaveBlock>& thePB) const;

  //! Selector
  //! Returns the common block
  Standard_EXPORT occ::handle<BOPDS_CommonBlock> CommonBlock(
    const occ::handle<BOPDS_PaveBlock>& thePB) const;

  //! Modifier
  //! Sets the common block <theCB>
  Standard_EXPORT void SetCommonBlock(const occ::handle<BOPDS_PaveBlock>&   thePB,
                                      const occ::handle<BOPDS_CommonBlock>& theCB);

  //! Selector
  //! Returns the real first pave block
  Standard_EXPORT occ::handle<BOPDS_PaveBlock> RealPaveBlock(
    const occ::handle<BOPDS_PaveBlock>& thePB) const;

  //! Query
  //! Returns true if common block contains more then one pave block
  Standard_EXPORT bool IsCommonBlockOnEdge(const occ::handle<BOPDS_PaveBlock>& thePB) const;

  //! Selector
  //! Returns the information about state of faces
  Standard_EXPORT const NCollection_Vector<BOPDS_FaceInfo>& FaceInfoPool() const;

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! information about state of face
  Standard_EXPORT bool HasFaceInfo(const int theIndex) const;

  //! Selector
  //! Returns the state of face with index theIndex
  Standard_EXPORT const BOPDS_FaceInfo& FaceInfo(const int theIndex) const;

  //! Selector/Modifier
  //! Returns the state of face with index theIndex
  Standard_EXPORT BOPDS_FaceInfo& ChangeFaceInfo(const int theIndex);

  //! Update the state In of face with index theIndex
  Standard_EXPORT void UpdateFaceInfoIn(const int theIndex);

  //! Update the state IN for all faces in the given map
  Standard_EXPORT void UpdateFaceInfoIn(const NCollection_Map<int>& theFaces);

  //! Update the state On of face with index theIndex
  Standard_EXPORT void UpdateFaceInfoOn(const int theIndex);

  //! Update the state ON for all faces in the given map
  Standard_EXPORT void UpdateFaceInfoOn(const NCollection_Map<int>& theFaces);

  //! Selector
  //! Returns the state On
  //! [theMPB,theMVP] of face with index theIndex
  Standard_EXPORT void FaceInfoOn(const int                                             theIndex,
                                  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
                                  NCollection_Map<int>&                                 theMVP);

  //! Selector
  //! Returns the state In
  //! [theMPB,theMVP] of face with index theIndex
  Standard_EXPORT void FaceInfoIn(const int                                             theIndex,
                                  NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& theMPB,
                                  NCollection_Map<int>&                                 theMVP);

  //! Selector
  //! Returns the indices of alone vertices
  //! for the face with index @p theFaceIndex
  Standard_EXPORT void AloneVertices(const int              theFaceIndex,
                                     NCollection_List<int>& theVertexList) const;

  //! Refine the state On for the all faces having
  //! state information
  //!
  //! ++
  Standard_EXPORT void RefineFaceInfoOn();

  //! Removes any pave block from list of having IN state if it has also the state ON.
  Standard_EXPORT void RefineFaceInfoIn();

  //! Returns information about ON/IN sub-shapes of the given faces.
  //! @param theFaceIndex1  the index of the first face
  //! @param theFaceIndex2  the index of the second face
  //! @param theMVOnIn  the indices of ON/IN vertices from both faces
  //! @param theMVCommon the indices of common vertices for both faces
  //! @param thePBOnIn  all On/In pave blocks from both faces
  //! @param theCommonPaveBlocks  the common pave blocks (that are shared by both faces).
  Standard_EXPORT void SubShapesOnIn(
    const int                                             theFaceIndex1,
    const int                                             theFaceIndex2,
    NCollection_Map<int>&                                 theMVOnIn,
    NCollection_Map<int>&                                 theMVCommon,
    NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& thePBOnIn,
    NCollection_Map<occ::handle<BOPDS_PaveBlock>>&        theCommonPaveBlocks) const;

  //! Returns the indices of edges that are shared
  //! for the faces with indices @p theFaceIndex1 and @p theFaceIndex2.
  Standard_EXPORT void SharedEdges(const int                                     theFaceIndex1,
                                   const int                                     theFaceIndex2,
                                   NCollection_List<int>&                        theEdgeList,
                                   const occ::handle<NCollection_BaseAllocator>& theAllocator);

  //! Selector
  //! Returns the collection same domain shapes
  Standard_EXPORT NCollection_DataMap<int, int>& ShapesSD();

  //! Modifier
  //! Adds the information about same domain shapes
  //! with indices theIndex, theIndexSD
  Standard_EXPORT void AddShapeSD(const int theIndex, const int theIndexSD);

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! same domain shape. In this case theIndexSD will contain
  //! the index of same domain shape found
  //!
  //! interferences
  Standard_EXPORT bool HasShapeSD(const int theIndex, int& theIndexSD) const;

  //! Returns the index of same domain shape for the shape
  //! with index @p theIndex. If there is no same domain shape, returns @p theIndex itself.
  Standard_EXPORT int GetSameDomainIndex(const int theIndex) const;

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Vertex
  NCollection_Vector<BOPDS_InterfVV>& InterfVV();

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Edge
  NCollection_Vector<BOPDS_InterfVE>& InterfVE();

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Face
  NCollection_Vector<BOPDS_InterfVF>& InterfVF();

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Edge
  NCollection_Vector<BOPDS_InterfEE>& InterfEE();

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Face
  NCollection_Vector<BOPDS_InterfEF>& InterfEF();

  //! Selector/Modifier
  //! Returns the collection of interferences Face/Face
  NCollection_Vector<BOPDS_InterfFF>& InterfFF();

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Solid
  NCollection_Vector<BOPDS_InterfVZ>& InterfVZ();

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Solid
  NCollection_Vector<BOPDS_InterfEZ>& InterfEZ();

  //! Selector/Modifier
  //! Returns the collection of interferences Face/Solid
  NCollection_Vector<BOPDS_InterfFZ>& InterfFZ();

  //! Selector/Modifier
  //! Returns the collection of interferences Solid/Solid
  NCollection_Vector<BOPDS_InterfZZ>& InterfZZ();

  //! Returns the number of types of the interferences
  static int NbInterfTypes();

  //! Modifier
  //! Adds the information about an interference between
  //! shapes with indices theI1, theI2 to the summary
  //! table of interferences
  bool AddInterf(const int theI1, const int theI2);

  //! Query
  //! Returns true if the shape with index theI
  //! is interferred
  bool HasInterf(const int theI) const;

  //! Query
  //! Returns true if the shapes with indices theI1, theI2
  //! are interferred
  bool HasInterf(const int theI1, const int theI2) const;

  //! Query
  //! Returns true if the shape with index theIndex1 is interfered
  //! with
  //! any sub-shape of the shape with index theIndex2  (theAnyInterference=true)
  //! all sub-shapes of the shape with index theIndex2 (theAnyInterference=false)
  Standard_EXPORT bool HasInterfShapeSubShapes(const int  theIndex1,
                                               const int  theIndex2,
                                               const bool theAnyInterference = true) const;

  //! Query
  //! Returns true if the shapes with indices theIndex1, theIndex2
  //! have interferred sub-shapes
  Standard_EXPORT bool HasInterfSubShapes(const int theIndex1, const int theIndex2) const;

  //! Selector
  //! Returns the table of interferences
  //!
  //! debug
  const NCollection_Map<BOPDS_Pair>& Interferences() const;

  Standard_EXPORT void Dump() const;

  //! Returns true if the shape with index @p theCandidate is a sub-shape
  //! of the shape with index @p theParent
  Standard_EXPORT bool IsSubShape(const int theCandidate, const int theParent);

  //! Fills theLP with sorted paves
  //! of the shape with index theIndex
  Standard_EXPORT void Paves(const int theIndex, NCollection_List<BOPDS_Pave>& theLP);

  //! Update the pave blocks for all shapes in data structure
  Standard_EXPORT void UpdatePaveBlocksWithSDVertices();

  //! Update the pave block for all shapes in data structure
  Standard_EXPORT void UpdatePaveBlockWithSDVertices(const occ::handle<BOPDS_PaveBlock>& thePB);

  //! Update the pave block of the common block for all shapes in data structure
  Standard_EXPORT void UpdateCommonBlockWithSDVertices(const occ::handle<BOPDS_CommonBlock>& theCB);

  Standard_EXPORT void InitPaveBlocksForVertex(const int theNV);

  //! Clears information about PaveBlocks for the untouched edges
  Standard_EXPORT void ReleasePaveBlocks();

  //! Checks if the existing shrunk data of the pave block is still valid.
  //! The shrunk data may become invalid if e.g. the vertices of the pave block
  //! have been replaced with the new one with bigger tolerances, or the tolerances
  //! of the existing vertices have been increased.
  Standard_EXPORT bool IsValidShrunkData(const occ::handle<BOPDS_PaveBlock>& thePB);

  //! Computes bounding box <theBox> for the solid with DS-index <theIndex>.
  //! The flag <theCheckInverted> enables/disables the check of the solid
  //! for inverted status. By default the solids will be checked.
  Standard_EXPORT void BuildBndBoxSolid(const int  theIndex,
                                        Bnd_Box&   theBox,
                                        const bool theCheckInverted = true);

private:
  //! Initializes the pave blocks for the shape with index theIndex
  void InitPaveBlocks(const int theIndex);

  //! Initializes the state of face with index theIndex
  void InitFaceInfo(const int theIndex);

  //! Initializes the FaceInfo structure for face with index theIndex with elements
  //! having IN state for the face
  void InitFaceInfoIn(const int theIndex);

  void InitShape(const int theIndex, const TopoDS_Shape& theS);

  bool CheckCoincidence(const occ::handle<BOPDS_PaveBlock>& thePB1,
                        const occ::handle<BOPDS_PaveBlock>& thePB2,
                        const double                        theFuzz);

  //! Prepares vertices, updates their bounding boxes.
  //! @param theAdditionalTolerance The additional tolerance to be added to the
  //! gaps of the bounding boxes.
  //! @return The number of vertices processed.
  int prepareVertices(const double theAdditionalTolerance);

  //! Prepares edges, updates their bounding boxes,
  //! sets degenerated flag for degenerated edges, creates start/end vertices for infinite edges.
  //! @param theAdditionalTolerance The additional tolerance to be added to the
  //! gaps of the bounding boxes.
  //! @return The number of edges processed.
  int prepareEdges(const double theAdditionalTolerance);

  //! Prepares faces, updates their bounding boxes and sub-shapes.
  //! Initially, subshapes of the faces are wires. They will be updated to
  //! contain edges and vertices.
  //! @param theAdditionalTolerance The additional tolerance to be added to the
  //! gaps of the bounding boxes.
  //! @return The number of faces processed.
  int prepareFaces(const double theAdditionalTolerance);

  //! Prepares solids, updates their bounding boxes and sub-shapes.
  //! Initially, subshapes of the solids are shells. They will be updated to
  //! contain faces and edges.
  //! @param theAdditionalTolerance The additional tolerance to be added to the
  //! gaps of the bounding boxes.
  //! @return The number of solids processed.
  int prepareSolids();

  //! Prepares the Vertex-Edge connection map.
  //! For the index of each vertex in the data structure,
  //! finds all edges sharing this vertex and
  //! stores the indices of these edges in a map.
  void buildVertexEdgeMap();

private:
  occ::handle<NCollection_BaseAllocator>                             myAllocator;
  NCollection_List<TopoDS_Shape>                                     myArguments;
  int                                                                myNbShapes;
  int                                                                myNbSourceShapes;
  NCollection_Vector<BOPDS_IndexRange>                               myRanges;
  NCollection_Vector<BOPDS_ShapeInfo>                                myLines;
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher>    myMapShapeIndex;
  NCollection_Vector<NCollection_List<occ::handle<BOPDS_PaveBlock>>> myPaveBlocksPool;
  NCollection_DataMap<occ::handle<BOPDS_PaveBlock>, occ::handle<BOPDS_CommonBlock>> myMapPBCB;
  NCollection_Vector<BOPDS_FaceInfo>                                                myFaceInfoPool;
  NCollection_DataMap<int, int>                                                     myShapesSD;
  NCollection_DataMap<int, NCollection_List<int>>                                   myMapVE;
  NCollection_Map<BOPDS_Pair>                                                       myInterfTB;
  NCollection_Vector<BOPDS_InterfVV>                                                myInterfVV;
  NCollection_Vector<BOPDS_InterfVE>                                                myInterfVE;
  NCollection_Vector<BOPDS_InterfVF>                                                myInterfVF;
  NCollection_Vector<BOPDS_InterfEE>                                                myInterfEE;
  NCollection_Vector<BOPDS_InterfEF>                                                myInterfEF;
  NCollection_Vector<BOPDS_InterfFF>                                                myInterfFF;
  NCollection_Vector<BOPDS_InterfVZ>                                                myInterfVZ;
  NCollection_Vector<BOPDS_InterfEZ>                                                myInterfEZ;
  NCollection_Vector<BOPDS_InterfFZ>                                                myInterfFZ;
  NCollection_Vector<BOPDS_InterfZZ>                                                myInterfZZ;
  NCollection_Map<int>                                                              myInterfered;
};

#include <BOPDS_DS.lxx>

#endif // _BOPDS_DS_HeaderFile
