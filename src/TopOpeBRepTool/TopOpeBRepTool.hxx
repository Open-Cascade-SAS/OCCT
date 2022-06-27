// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_HeaderFile
#define _TopOpeBRepTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TopTools_IndexedMapOfOrientedShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <Standard_OStream.hxx>
#include <TopOpeBRepTool_OutCurveType.hxx>
class TopoDS_Face;
class TopoDS_Solid;


//! This package provides services used by the TopOpeBRep
//! package performing topological operations on the BRep
//! data structure.
class TopOpeBRepTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Fuse  edges (in a   wire) of a  shape   where we have
  //! useless vertex.
  //! In case face <FF> is built on UV-non-connexed  wires
  //! (with the two closing edges  FORWARD and REVERSED, in
  //! spite of one only), we find out the faulty edge, add
  //! the faulty shapes (edge,wire,face) to <MshNOK>.
  //! <FF> is a face descendant of <F>.
  //! <MWisOld>(wire) = 1 if wire is wire of <F>
  //! 0    wire results from <F>'s wire split.
  //! returns false if purge fails
  Standard_EXPORT static Standard_Boolean PurgeClosingEdges (const TopoDS_Face& F, const TopoDS_Face& FF, const TopTools_DataMapOfShapeInteger& MWisOld, TopTools_IndexedMapOfOrientedShape& MshNOK);
  
  Standard_EXPORT static Standard_Boolean PurgeClosingEdges (const TopoDS_Face& F, const TopTools_ListOfShape& LOF, const TopTools_DataMapOfShapeInteger& MWisOld, TopTools_IndexedMapOfOrientedShape& MshNOK);
  
  Standard_EXPORT static Standard_Boolean CorrectONUVISO (const TopoDS_Face& F, TopoDS_Face& Fsp);
  
  //! Builds up the correct list of faces <LOFF> from <LOF>, using
  //! faulty shapes from map <MshNOK>.
  //! <LOF> is the list of <F>'s descendant faces.
  //! returns false if building fails
  Standard_EXPORT static Standard_Boolean MakeFaces (const TopoDS_Face& F, const TopTools_ListOfShape& LOF, const TopTools_IndexedMapOfOrientedShape& MshNOK, TopTools_ListOfShape& LOFF);
  
  //! Returns <False>  if  the  face is  valid (the UV
  //! representation  of  the  face is   a set   of  pcurves
  //! connexed by points with   connexity 2).
  //! Else,  splits <aFace> in order to return a list of valid
  //! faces.
  Standard_EXPORT static Standard_Boolean Regularize (const TopoDS_Face& aFace, TopTools_ListOfShape& aListOfFaces, TopTools_DataMapOfShapeListOfShape& ESplits);
  
  //! Returns <False>  if  the  face is  valid (the UV
  //! representation  of  the  face is   a set   of  pcurves
  //! connexed by points with   connexity 2).
  //! Else,  splits wires of the face, these are boundaries of the
  //! new faces to build up; <OldWiresNewWires> describes (wire,
  //! splits of wire); <ESplits> describes (edge, edge's splits)
  Standard_EXPORT static Standard_Boolean RegularizeWires (const TopoDS_Face& aFace, TopTools_DataMapOfShapeListOfShape& OldWiresNewWires, TopTools_DataMapOfShapeListOfShape& ESplits);
  
  //! Classify wire's splits of map <OldWiresnewWires> in order to
  //! compute <aListOfFaces>, the splits of <aFace>.
  Standard_EXPORT static Standard_Boolean RegularizeFace (const TopoDS_Face& aFace, const TopTools_DataMapOfShapeListOfShape& OldWiresnewWires, TopTools_ListOfShape& aListOfFaces);
  
  //! Returns <False> if the shell is valid (the solid is a set
  //! of faces connexed by edges with connexity 2).
  //! Else, splits faces of the shell; <OldFacesnewFaces> describes
  //! (face, splits of face).
  Standard_EXPORT static Standard_Boolean RegularizeShells (const TopoDS_Solid& aSolid, TopTools_DataMapOfShapeListOfShape& OldSheNewShe, TopTools_DataMapOfShapeListOfShape& FSplits);
  
  //! Prints <OCT> as string on stream <S>; returns <S>.
  Standard_EXPORT static Standard_OStream& Print (const TopOpeBRepTool_OutCurveType OCT, Standard_OStream& S);

};

#endif // _TopOpeBRepTool_HeaderFile
