// Created on: 1995-10-20
// Created by: Yves FRICAUD
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

#ifndef _BRepOffset_Analyse_HeaderFile
#define _BRepOffset_Analyse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <BRepOffset_DataMapOfShapeListOfInterval.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <ChFiDS_TypeOfConcavity.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

#include <Message_ProgressRange.hxx>

class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class TopoDS_Compound;

//! Analyses the shape to find the parts of edges
//! connecting the convex, concave or tangent faces.
class BRepOffset_Analyse
{
public:
  DEFINE_STANDARD_ALLOC

public: //! @name Constructors
  //! Empty c-tor
  Standard_EXPORT BRepOffset_Analyse();

  //! C-tor performing the job inside
  Standard_EXPORT BRepOffset_Analyse(const TopoDS_Shape& theS, const Standard_Real theAngle);

public: //! @name Performing analysis
  //! Performs the analysis
  Standard_EXPORT void Perform(const TopoDS_Shape&          theS,
                               const Standard_Real          theAngle,
                               const Message_ProgressRange& theRange = Message_ProgressRange());

public: //! @name Results
  //! Returns status of the algorithm
  Standard_Boolean IsDone() const { return myDone; }

  //! Returns the connectivity type of the edge
  Standard_EXPORT const BRepOffset_ListOfInterval& Type(const TopoDS_Edge& theE) const;

  //! Stores in <L> all the edges of Type <T>
  //! on the vertex <V>.
  Standard_EXPORT void Edges(const TopoDS_Vertex&         theV,
                             const ChFiDS_TypeOfConcavity theType,
                             TopTools_ListOfShape&        theL) const;

  //! Stores in <L> all the edges of Type <T>
  //! on the face <F>.
  Standard_EXPORT void Edges(const TopoDS_Face&           theF,
                             const ChFiDS_TypeOfConcavity theType,
                             TopTools_ListOfShape&        theL) const;

  //! set in <Edges> all  the Edges of <Shape> which are
  //! tangent to <Edge> at the vertex <Vertex>.
  Standard_EXPORT void TangentEdges(const TopoDS_Edge&    theEdge,
                                    const TopoDS_Vertex&  theVertex,
                                    TopTools_ListOfShape& theEdges) const;

  //! Checks if the given shape has ancestors
  Standard_Boolean HasAncestor(const TopoDS_Shape& theS) const
  {
    return myAncestors.Contains(theS);
  }

  //! Returns ancestors for the shape
  const TopTools_ListOfShape& Ancestors(const TopoDS_Shape& theS) const
  {
    return myAncestors.FindFromKey(theS);
  }

  //! Explode in compounds of faces where
  //! all the connex edges are of type <Side>
  Standard_EXPORT void Explode(TopTools_ListOfShape&        theL,
                               const ChFiDS_TypeOfConcavity theType) const;

  //! Explode in compounds of faces where
  //! all the connex edges are of type <Side1> or <Side2>
  Standard_EXPORT void Explode(TopTools_ListOfShape&        theL,
                               const ChFiDS_TypeOfConcavity theType1,
                               const ChFiDS_TypeOfConcavity theType2) const;

  //! Add in <CO> the faces of the shell containing <Face>
  //! where all the connex edges are of type <Side>.
  Standard_EXPORT void AddFaces(const TopoDS_Face&           theFace,
                                TopoDS_Compound&             theCo,
                                TopTools_MapOfShape&         theMap,
                                const ChFiDS_TypeOfConcavity theType) const;

  //! Add in <CO> the faces of the shell containing <Face>
  //! where all the connex edges are of type <Side1> or <Side2>.
  Standard_EXPORT void AddFaces(const TopoDS_Face&           theFace,
                                TopoDS_Compound&             theCo,
                                TopTools_MapOfShape&         theMap,
                                const ChFiDS_TypeOfConcavity theType1,
                                const ChFiDS_TypeOfConcavity theType2) const;

  void SetOffsetValue(const Standard_Real theOffset) { myOffset = theOffset; }

  //! Sets the face-offset data map to analyze tangential cases
  void SetFaceOffsetMap(const TopTools_DataMapOfShapeReal& theMap) { myFaceOffsetMap = theMap; }

  //! Returns the new faces constructed between tangent faces
  //! having different offset values on the shape
  const TopTools_ListOfShape& NewFaces() const { return myNewFaces; }

  //! Returns the new face constructed for the edge connecting
  //! the two tangent faces having different offset values
  Standard_EXPORT TopoDS_Shape Generated(const TopoDS_Shape& theS) const;

  //! Checks if the edge has generated a new face.
  Standard_Boolean HasGenerated(const TopoDS_Shape& theS) const
  {
    return myGenerated.Seek(theS) != NULL;
  }

  //! Returns the replacement of the edge in the face.
  //! If no replacement exists, returns the edge
  Standard_EXPORT const TopoDS_Edge& EdgeReplacement(const TopoDS_Face& theFace,
                                                     const TopoDS_Edge& theEdge) const;

  //! Returns the shape descendants.
  Standard_EXPORT const TopTools_ListOfShape* Descendants(
    const TopoDS_Shape&    theS,
    const Standard_Boolean theUpdate = Standard_False) const;

public: //! @name Clearing the content
  //! Clears the content of the algorithm
  Standard_EXPORT void Clear();

private: //! @name Treatment of tangential cases
  //! Treatment of the tangential cases.
  //! @param theEdges List of edges connecting tangent faces
  Standard_EXPORT void TreatTangentFaces(const TopTools_ListOfShape&  theEdges,
                                         const Message_ProgressRange& theRange);

private: //! @name Fields
  // Inputs
  TopoDS_Shape  myShape; //!< Input shape to analyze
  Standard_Real myAngle; //!< Criteria angle to check tangency

  Standard_Real               myOffset;        //!< Offset value
  TopTools_DataMapOfShapeReal myFaceOffsetMap; //!< Map to store offset values for the faces.
                                               //!  Should be set by the calling algorithm.

  // Results
  Standard_Boolean myDone; //!< Status of the algorithm

  // clang-format off
  BRepOffset_DataMapOfShapeListOfInterval myMapEdgeType; //!< Map containing the list of intervals on the edge
  TopTools_IndexedDataMapOfShapeListOfShape myAncestors; //!< Ancestors map
  NCollection_DataMap<TopoDS_Shape,
                      TopTools_DataMapOfShapeShape,
                      TopTools_ShapeMapHasher> myReplacement; //!< Replacement of an edge in the face
  mutable TopTools_DataMapOfShapeListOfShape myDescendants; //!< Map of shapes descendants built on the base of
                                                            //!< Ancestors map. Filled on the first query.

  TopTools_ListOfShape myNewFaces; //!< New faces generated to close the gaps between adjacent
                                   //!  tangential faces having different offset values
  TopTools_DataMapOfShapeShape myGenerated; //!< Binding between edge and face generated from the edge
  // clang-format on
};

#endif // _BRepOffset_Analyse_HeaderFile
