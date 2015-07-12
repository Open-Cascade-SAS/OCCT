// Created on: 2001-05-03
// Created by: Michael SAZONOV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _QANewModTopOpe_Glue_HeaderFile
#define _QANewModTopOpe_Glue_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <BRepTools_Substitution.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class TopoDS_Face;
class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Vertex;


//! Perform the gluing topological operation
//! (topological  sewing  of  two  topological  objects).
class QANewModTopOpe_Glue  : public BRepAlgoAPI_BooleanOperation
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Defines 2 operands.
  //! If one of operands is Solid and another is Shell and Shell
  //! goes inside Solid, the <allowCutting> determines what to do:
  //! if True, Shell is cut by Solid during the operation;
  //! if False, Null shape is returned, IsDone() returns False.
  //! If <thePerformNow> is False then does not compute immediately.
  Standard_EXPORT QANewModTopOpe_Glue(const TopoDS_Shape& theS1, const TopoDS_Shape& theS2, const Standard_Boolean theAllowCutting = Standard_False, const Standard_Boolean thePerformNow = Standard_True);
  
  //! Computation; is usefull when Create was called with thePerformNow
  //! being False
  Standard_EXPORT void Build() Standard_OVERRIDE;
  
  //! Returns the  list   of shapes generated   from the
  //! shape <theS>.
  Standard_EXPORT const TopTools_ListOfShape& Generated (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns the list  of shapes modified from the shape
  //! <theS>.
  Standard_EXPORT const TopTools_ListOfShape& Modified (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns True if the shape <theS> existed in one of operands
  //! and is absent in the result.
  Standard_EXPORT Standard_Boolean IsDeleted (const TopoDS_Shape& theS) Standard_OVERRIDE;
  
  //! Returns True if there is at leat one generated shape
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const Standard_OVERRIDE;
  
  //! Returns True if there is at leat one modified shape
  Standard_EXPORT virtual Standard_Boolean HasModified() const Standard_OVERRIDE;
  
  //! Returns True if there is at leat one deleted shape
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const Standard_OVERRIDE;
  
  Standard_EXPORT static Standard_Boolean ProjPointOnEdge (const gp_Pnt& thePnt, const TopoDS_Edge& theEdge, Standard_Real& thePar, Standard_Real& theDist);
  
  Standard_EXPORT static void InsertVertexInEdge (const TopoDS_Edge& theEdge, const TopoDS_Vertex& theVer, const Standard_Real thePar, TopoDS_Edge& theNewEdge);
  
  Standard_EXPORT static void SplitEdgeByVertex (const TopoDS_Edge& theEdge, const TopoDS_Vertex& theVer, const Standard_Real thePar, TopTools_ListOfShape& theListE);
  
  Standard_EXPORT static Standard_Boolean CompareVertices (const TopoDS_Vertex& theV1, const TopoDS_Vertex& theV2, Standard_Real& theDist);




protected:





private:

  
  //! Performs gluing Shell-Wire
  Standard_EXPORT void PerformShellWire();
  
  //! Performs gluing Solid-Vertex and Shell-Vertex
  Standard_EXPORT void PerformVertex();
  
  //! Performs gluing Solid-Shell and Shell-Shell
  Standard_EXPORT void PerformShell();
  
  //! Performs gluing Wire-Wire
  Standard_EXPORT void PerformWires();
  
  //! This function performs gluing operation of same domain
  //! faces theFirstSDFace and theSecondSDFace on shapes
  //! theNewSolid1 and theNewSolid2 and returns them.
  //! theMapOfChangedFaces contains changed faces as keys and
  //! lists of their splits as items.
  Standard_EXPORT Standard_Boolean SubstitudeSDFaces (const TopoDS_Shape& theFirstSDFace, const TopoDS_Shape& theSecondSDFace, TopoDS_Shape& theNewSolid1, TopoDS_Shape& theNewSolid2, TopTools_DataMapOfShapeListOfShape& theMapOfChangedFaces);
  
  //! Performs gluing between same domain faces of object and tool
  Standard_EXPORT void PerformSDFaces();
  
  //! For the case Solid-Shell, <aFace> is from Shell.
  //! Splits <theFace> onto faces by section edges <theListSE> and
  //! add <theFace> for substitution by list of faces which are "out"
  //! of Solid
  Standard_EXPORT Standard_Boolean CutFace (const TopoDS_Face& theFace, const TopTools_ListOfShape& theListSE);
  
  //! For the case Solid-Shell, <theFace> is a split of Shell's face.
  //! Returns the state of <theFace> relatively Solid.
  Standard_EXPORT TopAbs_State ClassifyFace (const TopoDS_Face& theFace, const TopTools_ListOfShape& theListSE) const;
  
  //! Inserts "internal" elements (wires, edges, vertices) computed
  //! from a list of section edges <theListSE> into <theFace>.
  Standard_EXPORT void SectionInsideFace (const TopoDS_Face& theFace, const TopTools_ListOfShape& theListSE, const Standard_Integer theShapeNum, const TopTools_MapOfShape& theGenEdges);
  
  Standard_EXPORT static const TopoDS_Shape& FindWireOrUpdateMap (const TopoDS_Shape& theWire, TopTools_IndexedDataMapOfShapeListOfShape& theMapELW);


  Standard_Boolean myCompleted;
  Standard_Boolean myAllowCutting;
  BRepTools_Substitution mySubst;
  TopTools_DataMapOfShapeListOfShape myMapSEdgeFaces1;
  TopTools_DataMapOfShapeListOfShape myMapSEdgeFaces2;
  TopTools_DataMapOfShapeShape myMapSEdgeCrossFace1;
  TopTools_DataMapOfShapeShape myMapSEdgeCrossFace2;
  TopTools_IndexedDataMapOfShapeListOfShape myMapEdgeWires;
  TopTools_MapOfShape myEdgesToLeave;
  TopTools_DataMapOfShapeListOfShape myMapModif;
  TopTools_DataMapOfShapeListOfShape myMapGener;


};







#endif // _QANewModTopOpe_Glue_HeaderFile
