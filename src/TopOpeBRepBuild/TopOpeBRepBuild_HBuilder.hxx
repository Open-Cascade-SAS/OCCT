// Created on: 1993-06-14
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

#ifndef _TopOpeBRepBuild_HBuilder_HeaderFile
#define _TopOpeBRepBuild_HBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepBuild_Builder1.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <TColStd_DataMapOfIntegerListOfInteger.hxx>
#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <Standard_Transient.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ListOfShape.hxx>
#include <Standard_Integer.hxx>
class Standard_NoSuchObject;
class TopOpeBRepDS_BuildTool;
class TopOpeBRepDS_HDataStructure;
class TopoDS_Shape;
class TopOpeBRepBuild_Builder;


class TopOpeBRepBuild_HBuilder;
DEFINE_STANDARD_HANDLE(TopOpeBRepBuild_HBuilder, Standard_Transient)

//! The HBuilder  algorithm    constructs   topological
//! objects  from   an    existing  topology  and  new
//! geometries attached to the topology. It is used to
//! construct the result of a topological operation;
//! the existing  topologies are the parts involved in
//! the  topological  operation and the new geometries
//! are the intersection lines and points.
class TopOpeBRepBuild_HBuilder : public Standard_Transient
{

public:

  
  Standard_EXPORT TopOpeBRepBuild_HBuilder(const TopOpeBRepDS_BuildTool& BT);
  
  Standard_EXPORT const TopOpeBRepDS_BuildTool& BuildTool() const;
  

  //! Stores the data structure <HDS>,
  //! Create shapes from the new geometries described in <HDS>.
  Standard_EXPORT void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS);
  

  //! Same as previous + evaluates if an operation performed on shapes S1,S2
  //! is a particular case.
  Standard_EXPORT void Perform (const Handle(TopOpeBRepDS_HDataStructure)& HDS, const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! Removes all split and merge already performed.
  //! Does NOT clear the handled DS.
  Standard_EXPORT void Clear();
  
  //! returns the DS handled by this builder
  Standard_EXPORT Handle(TopOpeBRepDS_HDataStructure) DataStructure() const;
  
  Standard_EXPORT TopOpeBRepDS_BuildTool& ChangeBuildTool();
  
  //! Merges the two shapes <S1> and <S2> keeping the
  //! parts of states <TB1>,<TB2> in <S1>,<S2>.
  Standard_EXPORT void MergeShapes (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopoDS_Shape& S2, const TopAbs_State TB2);
  
  //! Merges  the two solids <S1>   and <S2> keeping the
  //! parts in each solid of states <TB1> and <TB2>.
  Standard_EXPORT void MergeSolids (const TopoDS_Shape& S1, const TopAbs_State TB1, const TopoDS_Shape& S2, const TopAbs_State TB2);
  
  //! Merges the solid <S>  keeping the
  //! parts of state <TB>.
  Standard_EXPORT void MergeSolid (const TopoDS_Shape& S, const TopAbs_State TB);
  

  //! Returns True if the shape <S> has been split.
  Standard_EXPORT Standard_Boolean IsSplit (const TopoDS_Shape& S, const TopAbs_State ToBuild) const;
  

  //! Returns the split parts <ToBuild> of shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Splits (const TopoDS_Shape& S, const TopAbs_State ToBuild) const;
  

  //! Returns True if the shape <S> has been merged.
  Standard_EXPORT Standard_Boolean IsMerged (const TopoDS_Shape& S, const TopAbs_State ToBuild) const;
  

  //! Returns the merged parts <ToBuild> of shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Merged (const TopoDS_Shape& S, const TopAbs_State ToBuild) const;
  

  //! Returns the vertex created on point <I>.
  Standard_EXPORT const TopoDS_Shape& NewVertex (const Standard_Integer I) const;
  

  //! Returns the edges created on curve <I>.
  Standard_EXPORT const TopTools_ListOfShape& NewEdges (const Standard_Integer I) const;
  

  //! Returns the edges created on curve <I>.
  Standard_EXPORT TopTools_ListOfShape& ChangeNewEdges (const Standard_Integer I);
  

  //! Returns the faces created on surface <I>.
  Standard_EXPORT const TopTools_ListOfShape& NewFaces (const Standard_Integer I) const;
  
  Standard_EXPORT const TopTools_ListOfShape& Section();
  
  Standard_EXPORT void InitExtendedSectionDS (const Standard_Integer k = 3);
  
  Standard_EXPORT void InitSection (const Standard_Integer k = 3);
  
  Standard_EXPORT Standard_Boolean MoreSection() const;
  
  Standard_EXPORT void NextSection();
  
  Standard_EXPORT const TopoDS_Shape& CurrentSection() const;
  
  Standard_EXPORT Standard_Integer GetDSEdgeFromSectEdge (const TopoDS_Shape& E, const Standard_Integer rank);
  
  Standard_EXPORT TColStd_ListOfInteger& GetDSFaceFromDSEdge (const Standard_Integer indexEdg, const Standard_Integer rank);
  
  Standard_EXPORT Standard_Integer GetDSCurveFromSectEdge (const TopoDS_Shape& SectEdge);
  
  Standard_EXPORT Standard_Integer GetDSFaceFromDSCurve (const Standard_Integer indexCur, const Standard_Integer rank);
  
  Standard_EXPORT Standard_Integer GetDSPointFromNewVertex (const TopoDS_Shape& NewVert);
  
  //! search for the couple of face F1,F2
  //! (from arguments of supra Perform(S1,S2,HDS)) method which
  //! intersection gives section edge E built on an intersection curve.
  //! returns True if F1,F2 have been valued.
  //! returns False if E is not a section edge built
  //! on intersection curve IC.
  Standard_EXPORT Standard_Boolean EdgeCurveAncestors (const TopoDS_Shape& E, TopoDS_Shape& F1, TopoDS_Shape& F2, Standard_Integer& IC);
  
  //! search for the couple of face F1,F2
  //! (from arguments of supra Perform(S1,S2,HDS)) method which
  //! intersection gives section edge E built on at least one edge .
  //! returns True if F1,F2 have been valued.
  //! returns False if E is not a section edge built
  //! on at least one edge of S1 and/or S2.
  //! LE1,LE2 are edges of S1,S2 which common part is edge E.
  //! LE1 or LE2 may be empty() but not both.
  Standard_EXPORT Standard_Boolean EdgeSectionAncestors (const TopoDS_Shape& E, TopTools_ListOfShape& LF1, TopTools_ListOfShape& LF2, TopTools_ListOfShape& LE1, TopTools_ListOfShape& LE2);
  
  //! Returns 0 is standard operation, != 0 if particular case
  Standard_EXPORT Standard_Integer IsKPart();
  
  Standard_EXPORT void MergeKPart (const TopAbs_State TB1, const TopAbs_State TB2);
  
  Standard_EXPORT TopOpeBRepBuild_Builder& ChangeBuilder();




  DEFINE_STANDARD_RTTIEXT(TopOpeBRepBuild_HBuilder,Standard_Transient)

protected:


  TopOpeBRepBuild_Builder1 myBuilder;


private:

  
  Standard_EXPORT void MakeEdgeAncestorMap();
  
  Standard_EXPORT void MakeCurveAncestorMap();

  TopTools_DataMapOfShapeInteger mySectEdgeDSEdges1;
  TopTools_DataMapOfShapeInteger mySectEdgeDSEdges2;
  TColStd_DataMapOfIntegerListOfInteger myDSEdgesDSFaces1;
  TColStd_DataMapOfIntegerListOfInteger myDSEdgesDSFaces2;
  Standard_Boolean myMakeEdgeAncestorIsDone;
  TopTools_DataMapOfShapeInteger mySectEdgeDSCurve;
  Standard_Boolean myMakeCurveAncestorIsDone;
  TopTools_DataMapOfShapeInteger myNewVertexDSPoint;
  Standard_Boolean myMakePointAncestorIsDone;
  TopoDS_Shape myEmptyShape;
  TColStd_ListOfInteger myEmptyIntegerList;


};







#endif // _TopOpeBRepBuild_HBuilder_HeaderFile
