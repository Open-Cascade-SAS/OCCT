// Created on: 1995-11-10
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

#ifndef _BRepAlgo_Loop_HeaderFile
#define _BRepAlgo_Loop_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepAlgo_Image.hxx>
class TopoDS_Edge;

//! Builds the loops from a set of edges on a face.
class BRepAlgo_Loop
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepAlgo_Loop();

  //! Init with <F> the set of edges must have
  //! pcurves on <F>.
  Standard_EXPORT void Init(const TopoDS_Face& F);

  //! Add E with <LV>. <E> will be copied and trim
  //! by vertices in <LV>.
  Standard_EXPORT void AddEdge(TopoDS_Edge& E, const TopTools_ListOfShape& LV);

  //! Add <E> as const edge, E can be in the result.
  Standard_EXPORT void AddConstEdge(const TopoDS_Edge& E);

  //! Add <LE> as a set of const edges.
  Standard_EXPORT void AddConstEdges(const TopTools_ListOfShape& LE);

  //! Sets the Image Vertex - Vertex
  Standard_EXPORT void SetImageVV(const BRepAlgo_Image& theImageVV);

  //! Make loops.
  Standard_EXPORT void Perform();

  //! Update VE map according to Image Vertex - Vertex
  Standard_EXPORT void UpdateVEmap(TopTools_IndexedDataMapOfShapeListOfShape& theVEmap);

  //! Cut the  edge <E>  in  several edges  <NE> on the
  //! vertices<VonE>.
  Standard_EXPORT void CutEdge(const TopoDS_Edge&          E,
                               const TopTools_ListOfShape& VonE,
                               TopTools_ListOfShape&       NE) const;

  //! Returns the list of wires performed.
  //! can be an empty list.
  Standard_EXPORT const TopTools_ListOfShape& NewWires() const;

  //! Build faces from the wires result.
  Standard_EXPORT void WiresToFaces();

  //! Returns the list of faces.
  //! Warning: The method <WiresToFaces> as to be called before.
  //! can be an empty list.
  Standard_EXPORT const TopTools_ListOfShape& NewFaces() const;

  //! Returns the list of new edges built from an edge <E>
  //! it can be an empty list.
  Standard_EXPORT const TopTools_ListOfShape& NewEdges(const TopoDS_Edge& E) const;

  //! Returns the datamap of vertices with their substitutes.
  Standard_EXPORT void GetVerticesForSubstitute(TopTools_DataMapOfShapeShape& VerVerMap) const;

  Standard_EXPORT void VerticesForSubstitute(TopTools_DataMapOfShapeShape& VerVerMap);

  //! Set maximal tolerance used for comparing distances between vertices.
  void SetTolConf(const Standard_Real theTolConf) { myTolConf = theTolConf; }

  //! Get maximal tolerance used for comparing distances between vertices.
  Standard_Real GetTolConf() const { return myTolConf; }

protected:
private:
  TopoDS_Face                        myFace;
  TopTools_ListOfShape               myConstEdges;
  TopTools_ListOfShape               myEdges;
  TopTools_DataMapOfShapeListOfShape myVerOnEdges;
  TopTools_ListOfShape               myNewWires;
  TopTools_ListOfShape               myNewFaces;
  TopTools_DataMapOfShapeListOfShape myCutEdges;
  TopTools_DataMapOfShapeShape       myVerticesForSubstitute;
  BRepAlgo_Image                     myImageVV;
  Standard_Real                      myTolConf;
};

#endif // _BRepAlgo_Loop_HeaderFile
