// Created on: 1995-12-21
// Created by: Jean Yves LEBEY
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

#ifndef _TopOpeBRepBuild_FaceBuilder_HeaderFile
#define _TopOpeBRepBuild_FaceBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Face.hxx>
#include <TopOpeBRepBuild_LoopSet.hxx>
#include <TopOpeBRepBuild_BlockIterator.hxx>
#include <TopOpeBRepBuild_BlockBuilder.hxx>
#include <TopOpeBRepBuild_FaceAreaBuilder.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Integer.hxx>
class TopOpeBRepBuild_WireEdgeSet;
class TopoDS_Shape;
class TopOpeBRepBuild_ShapeSet;



class TopOpeBRepBuild_FaceBuilder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TopOpeBRepBuild_FaceBuilder();
  
  //! Create a FaceBuilder to build the faces on
  //! the shapes (wires, blocks of edge) described by <LS>.
  Standard_EXPORT TopOpeBRepBuild_FaceBuilder(TopOpeBRepBuild_WireEdgeSet& ES, const TopoDS_Shape& F, const Standard_Boolean ForceClass = Standard_False);
  
  Standard_EXPORT void InitFaceBuilder (TopOpeBRepBuild_WireEdgeSet& ES, const TopoDS_Shape& F, const Standard_Boolean ForceClass);
  
  //! Removes   are  non 3d-closed  wires.
  //! Fills  up maps <mapVVsameG> and  <mapVon1Edge>,  in order to
  //! correct 3d-closed but unclosed (topologic connexity) wires.
  //! modifies myBlockBuilder
  Standard_EXPORT void DetectUnclosedWire (TopTools_IndexedDataMapOfShapeShape& mapVVsameG, TopTools_IndexedDataMapOfShapeShape& mapVon1Edge);
  
  //! Using the given maps, change the topology of the 3d-closed
  //! wires, in order to get closed wires.
  Standard_EXPORT void CorrectGclosedWire (const TopTools_IndexedDataMapOfShapeShape& mapVVref, const TopTools_IndexedDataMapOfShapeShape& mapVon1Edge);
  
  //! Removes edges appearing twice (FORWARD,REVERSED) with a bounding
  //! vertex not connected to any other edge.
  //! mapE contains edges found.
  //! modifies myBlockBuilder.
  Standard_EXPORT void DetectPseudoInternalEdge (TopTools_IndexedMapOfShape& mapE);
  
  //! return myFace
  Standard_EXPORT const TopoDS_Shape& Face() const;
  
  Standard_EXPORT Standard_Integer InitFace();
  
  Standard_EXPORT Standard_Boolean MoreFace() const;
  
  Standard_EXPORT void NextFace();
  
  Standard_EXPORT Standard_Integer InitWire();
  
  Standard_EXPORT Standard_Boolean MoreWire() const;
  
  Standard_EXPORT void NextWire();
  
  Standard_EXPORT Standard_Boolean IsOldWire() const;
  
  //! Returns current wire
  //! This wire may be :
  //! * an old wire OldWire(), which has not been reconstructed;
  //! * a new wire made of edges described by ...NewEdge() methods.
  Standard_EXPORT const TopoDS_Shape& OldWire() const;
  
  //! Iterates on myBlockIterator until finding a valid element
  Standard_EXPORT void FindNextValidElement();
  
  Standard_EXPORT Standard_Integer InitEdge();
  
  Standard_EXPORT Standard_Boolean MoreEdge() const;
  
  Standard_EXPORT void NextEdge();
  
  //! Returns current new edge of current new wire.
  Standard_EXPORT const TopoDS_Shape& Edge() const;
  
  Standard_EXPORT Standard_Integer EdgeConnexity (const TopoDS_Shape& E) const;
  
  Standard_EXPORT Standard_Integer AddEdgeWire (const TopoDS_Shape& E, TopoDS_Shape& W) const;




protected:





private:

  
  Standard_EXPORT void MakeLoops (TopOpeBRepBuild_ShapeSet& SS);


  TopoDS_Face myFace;
  TopOpeBRepBuild_LoopSet myLoopSet;
  TopOpeBRepBuild_BlockIterator myBlockIterator;
  TopOpeBRepBuild_BlockBuilder myBlockBuilder;
  TopOpeBRepBuild_FaceAreaBuilder myFaceAreaBuilder;
  TopTools_DataMapOfShapeInteger myMOSI;


};







#endif // _TopOpeBRepBuild_FaceBuilder_HeaderFile
