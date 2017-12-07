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

#include <BOPDS_DataMapOfPaveBlockCommonBlock.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>
#include <BOPDS_VectorOfIndexRange.hxx>
#include <BOPDS_VectorOfInterfEE.hxx>
#include <BOPDS_VectorOfInterfEF.hxx>
#include <BOPDS_VectorOfInterfEZ.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_VectorOfInterfFZ.hxx>
#include <BOPDS_VectorOfInterfVE.hxx>
#include <BOPDS_VectorOfInterfVF.hxx>
#include <BOPDS_VectorOfInterfVV.hxx>
#include <BOPDS_VectorOfInterfVZ.hxx>
#include <BOPDS_VectorOfInterfZZ.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPDS_VectorOfShapeInfo.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Precision.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_ListOfShape.hxx>

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
//! Section, Maker Volume, Splitter and Cells Builder.<br>
//!
//! The data structure has the  following contents:<br>
//! 1. the arguments of an operation [myArguments];<br>
//! 2  the information about arguments/new shapes
//! and their sub-shapes (type of the shape,
//! bounding box, etc) [myLines];<br>
//! 3. each argument shape(and its subshapes)
//! has/have own range of indices (rank);<br>
//! 4. pave blocks on source edges [myPaveBlocksPool];<br>
//! 5. the state of source faces  [myFaceInfoPool];<br>
//! 6  the collection of same domain shapes [myShapesSD];<br>
//! 7  the collection of interferences  [myInterfTB,
//! myInterfVV,..myInterfFF]
class BOPDS_DS 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! Empty contructor
  Standard_EXPORT BOPDS_DS();
Standard_EXPORT virtual ~BOPDS_DS();
  

  //! Contructor
  //! theAllocator - the allocator to manage the memory
  Standard_EXPORT BOPDS_DS(const Handle(NCollection_BaseAllocator)& theAllocator);
  

  //! Clears the contents
  Standard_EXPORT void Clear();
  

  //! Selector
  Standard_EXPORT const Handle(NCollection_BaseAllocator)& Allocator() const;
  

  //! Modifier
  //! Sets the arguments [theLS] of an operation
  Standard_EXPORT void SetArguments (const TopTools_ListOfShape& theLS);
  

  //! Selector
  //! Returns the arguments of an operation
  Standard_EXPORT const TopTools_ListOfShape& Arguments() const;
  

  //! Initializes the data structure for
  //! the arguments
  Standard_EXPORT void Init(const Standard_Real theFuzz = Precision::Confusion());
  

  //! Selector
  //! Returns the total number of shapes stored
  Standard_EXPORT Standard_Integer NbShapes() const;
  

  //! Selector
  //! Returns the total number of source shapes stored
  Standard_EXPORT Standard_Integer NbSourceShapes() const;
  

  //! Selector
  //! Returns the number of index ranges
  Standard_EXPORT Standard_Integer NbRanges() const;
  

  //! Selector
  //! Returns the index range "i"
  Standard_EXPORT const BOPDS_IndexRange& Range (const Standard_Integer theIndex) const;
  

  //! Selector
  //! Returns the rank of the shape of index "i"
  Standard_EXPORT Standard_Integer Rank (const Standard_Integer theIndex) const;
  

  //! Returns true if the shape of index "i" is not
  //! the source shape/sub-shape
  Standard_EXPORT Standard_Boolean IsNewShape (const Standard_Integer theIndex) const;
  

  //! Modifier
  //! Appends the information about the shape [theSI]
  //! to the data structure
  //! Returns the index of theSI in the data structure
  Standard_EXPORT Standard_Integer Append (const BOPDS_ShapeInfo& theSI);
  

  //! Modifier
  //! Appends the default information about the shape [theS]
  //! to the data structure
  //! Returns the index of theS in the data structure
  Standard_EXPORT Standard_Integer Append (const TopoDS_Shape& theS);
  

  //! Selector
  //! Returns the information about the shape
  //! with index theIndex
  Standard_EXPORT const BOPDS_ShapeInfo& ShapeInfo (const Standard_Integer theIndex) const;
  

  //! Selector/Modifier
  //! Returns the information about the shape
  //! with index theIndex
  Standard_EXPORT BOPDS_ShapeInfo& ChangeShapeInfo (const Standard_Integer theIndex);
  

  //! Selector
  //! Returns the shape
  //! with index theIndex
  Standard_EXPORT const TopoDS_Shape& Shape (const Standard_Integer theIndex) const;
  

  //! Selector
  //! Returns the index  of the shape theS
  Standard_EXPORT Standard_Integer Index (const TopoDS_Shape& theS) const;
  

  //! Selector
  //! Returns the information about pave blocks on source edges
  Standard_EXPORT const BOPDS_VectorOfListOfPaveBlock& PaveBlocksPool() const;
  

  //! Selector/Modifier
  //! Returns the information about pave blocks on source edges
  Standard_EXPORT BOPDS_VectorOfListOfPaveBlock& ChangePaveBlocksPool();
  

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! information about pave blocks
  Standard_EXPORT Standard_Boolean HasPaveBlocks (const Standard_Integer theIndex) const;
  

  //! Selector
  //! Returns the pave blocks for the shape with index theIndex
  Standard_EXPORT const BOPDS_ListOfPaveBlock& PaveBlocks (const Standard_Integer theIndex) const;
  

  //! Selector/Modifier
  //! Returns the pave blocks for the shape with index theIndex
  Standard_EXPORT BOPDS_ListOfPaveBlock& ChangePaveBlocks (const Standard_Integer theIndex);
  

  //! Update the pave blocks for the all shapes in data structure
  Standard_EXPORT void UpdatePaveBlocks();
  

  //! Update the pave block thePB
  Standard_EXPORT void UpdatePaveBlock (const Handle(BOPDS_PaveBlock)& thePB);
  

  //! Update the common block theCB
  Standard_EXPORT void UpdateCommonBlock (const Handle(BOPDS_CommonBlock)& theCB,
                                          const Standard_Real theFuzz);
  

  //! Query
  //! Returns true if the pave block is common block
  Standard_EXPORT Standard_Boolean IsCommonBlock (const Handle(BOPDS_PaveBlock)& thePB) const;
  

  //! Selector
  //! Returns the common block
  Standard_EXPORT Handle(BOPDS_CommonBlock) CommonBlock (const Handle(BOPDS_PaveBlock)& thePB) const;
  

  //! Modifier
  //! Sets the common block <theCB>
  Standard_EXPORT void SetCommonBlock (const Handle(BOPDS_PaveBlock)& thePB, const Handle(BOPDS_CommonBlock)& theCB);
  

  //! Selector
  //! Returns the real first pave block
  Standard_EXPORT Handle(BOPDS_PaveBlock) RealPaveBlock (const Handle(BOPDS_PaveBlock)& thePB) const;
  

  //! Query
  //! Returns true if common block contains more then one pave block
  Standard_EXPORT Standard_Boolean IsCommonBlockOnEdge (const Handle(BOPDS_PaveBlock)& thePB) const;
  

  //! Selector
  //! Returns the information about state of faces
  Standard_EXPORT const BOPDS_VectorOfFaceInfo& FaceInfoPool() const;
  

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! information about state of face
  Standard_EXPORT Standard_Boolean HasFaceInfo (const Standard_Integer theIndex) const;
  

  //! Selector
  //! Returns the state of face with index theIndex
  Standard_EXPORT const BOPDS_FaceInfo& FaceInfo (const Standard_Integer theIndex) const;
  

  //! Selector/Modifier
  //! Returns the state of face with index theIndex
  Standard_EXPORT BOPDS_FaceInfo& ChangeFaceInfo (const Standard_Integer theIndex);
  

  //! Update the state In of face with index theIndex
  Standard_EXPORT void UpdateFaceInfoIn (const Standard_Integer theIndex);
  

  //! Update the state On of face with index theIndex
  Standard_EXPORT void UpdateFaceInfoOn (const Standard_Integer theIndex);
  

  //! Selector
  //! Returns the state On
  //! [theMPB,theMVP] of face with index theIndex
  Standard_EXPORT void FaceInfoOn (const Standard_Integer theIndex, BOPDS_IndexedMapOfPaveBlock& theMPB, TColStd_MapOfInteger& theMVP);
  

  //! Selector
  //! Returns the state In
  //! [theMPB,theMVP] of face with index theIndex
  Standard_EXPORT void FaceInfoIn (const Standard_Integer theIndex, BOPDS_IndexedMapOfPaveBlock& theMPB, TColStd_MapOfInteger& theMVP);
  

  //! Selector
  //! Returns the indices of alone vertices
  //! for the face with index theIndex
  Standard_EXPORT void AloneVertices (const Standard_Integer theF, TColStd_ListOfInteger& theLI) const;
  

  //! Refine the state On for the all faces having
  //! state information
  //!
  //! ++
  Standard_EXPORT void RefineFaceInfoOn();
  

  //! Returns information about ON/IN sub-shapes of the given faces.
  //! @param theMVOnIn  the indices of ON/IN vertices from both faces
  //! @param theMVCommon the indices of common vertices for both faces
  //! @param thePBOnIn  all On/In pave blocks from both faces
  //! @param theCommonPB  the common pave blocks (that are shared by both faces).
  Standard_EXPORT void SubShapesOnIn(const Standard_Integer theNF1,
                                     const Standard_Integer theNF2,
                                     TColStd_MapOfInteger& theMVOnIn,
                                     TColStd_MapOfInteger& theMVCommon,
                                     BOPDS_IndexedMapOfPaveBlock& thePBOnIn,
                                     BOPDS_MapOfPaveBlock& theCommonPB) const;
  

  //! Returns the indices of edges that are  shared
  //! for the faces with indices theF1, theF2
  //!
  //! same domain shapes
  Standard_EXPORT void SharedEdges (const Standard_Integer theF1, const Standard_Integer theF2, TColStd_ListOfInteger& theLI, const Handle(NCollection_BaseAllocator)& theAllocator);
  

  //! Selector
  //! Returns the collection same domain shapes
  Standard_EXPORT TColStd_DataMapOfIntegerInteger& ShapesSD();
  

  //! Modifier
  //! Adds the information about same domain shapes
  //! with indices theIndex, theIndexSD
  Standard_EXPORT void AddShapeSD (const Standard_Integer theIndex, const Standard_Integer theIndexSD);
  

  //! Query
  //! Returns true if the shape with index theIndex has the
  //! same domain shape. In this case theIndexSD will contain
  //! the index of same domain shape found
  //!
  //! interferences
  Standard_EXPORT Standard_Boolean HasShapeSD (const Standard_Integer theIndex, Standard_Integer& theIndexSD) const;
  

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Vertex
    BOPDS_VectorOfInterfVV& InterfVV();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Edge
    BOPDS_VectorOfInterfVE& InterfVE();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Face
    BOPDS_VectorOfInterfVF& InterfVF();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Edge
    BOPDS_VectorOfInterfEE& InterfEE();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Face
    BOPDS_VectorOfInterfEF& InterfEF();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Face/Face
    BOPDS_VectorOfInterfFF& InterfFF();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Vertex/Solid
    BOPDS_VectorOfInterfVZ& InterfVZ();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Edge/Solid
    BOPDS_VectorOfInterfEZ& InterfEZ();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Face/Solid
    BOPDS_VectorOfInterfFZ& InterfFZ();
  

  //! Selector/Modifier
  //! Returns the collection of interferences Solid/Solid
    BOPDS_VectorOfInterfZZ& InterfZZ();
  

  //! Returns the number of types of the interferences
    static Standard_Integer NbInterfTypes();
  

  //! Modifier
  //! Adds the information about an interference between
  //! shapes with indices theI1, theI2 to the summary
  //! table of interferences
    void AddInterf (const Standard_Integer theI1, const Standard_Integer theI2);
  

  //! Query
  //! Returns true if the shape with index theI
  //! is interferred
  Standard_EXPORT Standard_Boolean HasInterf (const Standard_Integer theI) const;
  

  //! Query
  //! Returns true if the shapes with indices theI1, theI2
  //! are interferred
    Standard_Boolean HasInterf (const Standard_Integer theI1, const Standard_Integer theI2) const;
  

  //! Query
  //! Returns true if the shape with index theI1 is interfered
  //! with
  //! any sub-shape of the shape with index theI2  (theFlag=true)
  //! all sub-shapes of the shape with index theI2 (theFlag=false)
  Standard_EXPORT Standard_Boolean HasInterfShapeSubShapes (const Standard_Integer theI1, const Standard_Integer theI2, const Standard_Boolean theFlag = Standard_True) const;
  

  //! Query
  //! Returns true if the shapes with indices theI1, theI2
  //! have interferred sub-shapes
  Standard_EXPORT Standard_Boolean HasInterfSubShapes (const Standard_Integer theI1, const Standard_Integer theI2) const;
  

  //! Selector
  //! Returns the table of interferences
  //!
  //! debug
    const BOPDS_MapOfPair& Interferences() const;
  
  Standard_EXPORT void Dump() const;
  
  Standard_EXPORT Standard_Boolean IsSubShape (const Standard_Integer theI1, const Standard_Integer theI2);
  
  //! Fills theLP with sorted paves
  //! of the shape with index theIndex
  Standard_EXPORT void Paves (const Standard_Integer theIndex, BOPDS_ListOfPave& theLP);
  

  //! Updates tolerance of the sub-shapes of the shape with index <theIndex>.
  Standard_EXPORT void UpdateEdgeTolerance (const Standard_Integer theIndex,
                                            const Standard_Real theTolerance,
                                            const Standard_Real theFuzz = Precision::Confusion());

  //! Update the pave blocks for all shapes in data structure
  Standard_EXPORT void UpdatePaveBlocksWithSDVertices();

  //! Update the pave block for all shapes in data structure
  Standard_EXPORT void UpdatePaveBlockWithSDVertices(const Handle(BOPDS_PaveBlock)& thePB);

  //! Update the pave block of the common block for all shapes in data structure
  Standard_EXPORT void UpdateCommonBlockWithSDVertices(const Handle(BOPDS_CommonBlock)& theCB);

  Standard_EXPORT void InitPaveBlocksForVertex(const Standard_Integer theNV);

  //! Clears information about PaveBlocks for the untouched edges
  Standard_EXPORT void ReleasePaveBlocks();

  //! Checks if the existing shrunk data of the pave block is still valid.<br>
  //! The shrunk data may become invalid if e.g. the vertices of the pave block
  //! have been replaced with the new one with bigger tolerances, or the tolerances
  //! of the existing vertices have been increased.
  Standard_EXPORT Standard_Boolean IsValidShrunkData(const Handle(BOPDS_PaveBlock)& thePB);

  //! Computes bounding box <theBox> for the solid with DS-index <theIndex>.
  //! The flag <theCheckInverted> enables/disables the check of the solid
  //! for inverted status. By default the solids will be checked.
  Standard_EXPORT void BuildBndBoxSolid (const Standard_Integer theIndex,
                                         Bnd_Box& theBox,
                                         const Standard_Boolean theCheckInverted = Standard_True);

protected:

  

  //! Initializes the pave blocks for the shape with index theIndex
  Standard_EXPORT void InitPaveBlocks (const Standard_Integer theIndex);

  //! Initializes the state of face with index theIndex
  Standard_EXPORT void InitFaceInfo (const Standard_Integer theIndex);
  
  Standard_EXPORT void InitShape (const Standard_Integer theIndex, const TopoDS_Shape& theS);
  
  Standard_EXPORT Standard_Boolean CheckCoincidence (const Handle(BOPDS_PaveBlock)& thePB1,
                                                     const Handle(BOPDS_PaveBlock)& thePB2,
                                                     const Standard_Real theFuzz);


  Handle(NCollection_BaseAllocator) myAllocator;
  TopTools_ListOfShape myArguments;
  Standard_Integer myNbShapes;
  Standard_Integer myNbSourceShapes;
  BOPDS_VectorOfIndexRange myRanges;
  BOPDS_VectorOfShapeInfo myLines;
  TopTools_DataMapOfShapeInteger myMapShapeIndex;
  BOPDS_VectorOfListOfPaveBlock myPaveBlocksPool;
  BOPDS_DataMapOfPaveBlockCommonBlock myMapPBCB;
  BOPDS_VectorOfFaceInfo myFaceInfoPool;
  TColStd_DataMapOfIntegerInteger myShapesSD;
  TColStd_DataMapOfIntegerListOfInteger myMapVE;
  BOPDS_MapOfPair myInterfTB;
  BOPDS_VectorOfInterfVV myInterfVV;
  BOPDS_VectorOfInterfVE myInterfVE;
  BOPDS_VectorOfInterfVF myInterfVF;
  BOPDS_VectorOfInterfEE myInterfEE;
  BOPDS_VectorOfInterfEF myInterfEF;
  BOPDS_VectorOfInterfFF myInterfFF;
  BOPDS_VectorOfInterfVZ myInterfVZ;
  BOPDS_VectorOfInterfEZ myInterfEZ;
  BOPDS_VectorOfInterfFZ myInterfFZ;
  BOPDS_VectorOfInterfZZ myInterfZZ;


private:





};


#include <BOPDS_DS.lxx>





#endif // _BOPDS_DS_HeaderFile
