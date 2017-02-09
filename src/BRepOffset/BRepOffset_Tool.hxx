// Created on: 1995-10-23
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

#ifndef _BRepOffset_Tool_HeaderFile
#define _BRepOffset_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopAbs_Orientation.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class BRepOffset_Analyse;
class TopoDS_Wire;
class TopoDS_Shape;
class BRepAlgo_AsDes;
class BRepAlgo_Image;
class Geom_Curve;



class BRepOffset_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! <V1> is the FirstVertex ,<V2> is the Last Vertex of <Edge>
  //! taking account the orientation of Edge.
  Standard_EXPORT static void EdgeVertices (const TopoDS_Edge& E, TopoDS_Vertex& V1, TopoDS_Vertex& V2);
  
  //! returns the cumul  of the orientation  of <Edge>
  //! and thc containing wire in <Face>
  Standard_EXPORT static TopAbs_Orientation OriEdgeInFace (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! <E> is a section  between <F1> and <F2>.  Computes
  //! <O1> the orientation of <E> in <F1> influenced by <F2>.
  //! idem for <O2>.
  Standard_EXPORT static void OrientSection (const TopoDS_Edge& E, const TopoDS_Face& F1, const TopoDS_Face& F2, TopAbs_Orientation& O1, TopAbs_Orientation& O2);
  
  //! Looks for the common Vertices and Edges between faces <theF1> and <theF2>.<br>
  //! Returns TRUE if common shapes have been found.<br>
  //! <theLE> will contain the found common edges;<br>
  //! <theLV> will contain the found common vertices.
  Standard_EXPORT static Standard_Boolean FindCommonShapes(const TopoDS_Face& theF1,
                                                           const TopoDS_Face& theF2,
                                                           TopTools_ListOfShape& theLE,
                                                           TopTools_ListOfShape& theLV);

  //! Looks for the common shapes of type <theType> between shapes <theS1> and <theS2>.<br>
  //! Returns TRUE if common shapes have been found.<br>
  //! <theLSC> will contain the found common shapes.
  Standard_EXPORT static Standard_Boolean FindCommonShapes(const TopoDS_Shape& theS1,
                                                           const TopoDS_Shape& theS2,
                                                           const TopAbs_ShapeEnum theType,
                                                           TopTools_ListOfShape& theLSC);

  //! Computes the   Section betwwen  <F1> and  <F2> the
  //! edges solution   are  stored in <LInt1>  with  the
  //! orientation on <F1>, the sames edges are stored in
  //! <Lint2> with the orientation on <F2>.
  Standard_EXPORT static void Inter3D (const TopoDS_Face& F1, const TopoDS_Face& F2, TopTools_ListOfShape& LInt1, TopTools_ListOfShape& LInt2, const TopAbs_State Side, const TopoDS_Edge& RefEdge, const Standard_Boolean IsRefEdgeDefined = Standard_False);
  
  //! Find if the edges <Edges> of the face <F2> are on
  //! the face <F1>.
  //! Set in <LInt1> <LInt2> the updated edges.
  //! If all the edges are computed, returns true.
  Standard_EXPORT static Standard_Boolean TryProject (const TopoDS_Face& F1, const TopoDS_Face& F2, const TopTools_ListOfShape& Edges, TopTools_ListOfShape& LInt1, TopTools_ListOfShape& LInt2, const TopAbs_State Side, const Standard_Real TolConf);
  
  Standard_EXPORT static void PipeInter (const TopoDS_Face& F1, const TopoDS_Face& F2, TopTools_ListOfShape& LInt1, TopTools_ListOfShape& LInt2, const TopAbs_State Side);
  
  Standard_EXPORT static void Inter2d (const TopoDS_Face& F, const TopoDS_Edge& E1, const TopoDS_Edge& E2, TopTools_ListOfShape& LV, const Standard_Real Tol);
  
  Standard_EXPORT static void InterOrExtent (const TopoDS_Face& F1, const TopoDS_Face& F2, TopTools_ListOfShape& LInt1, TopTools_ListOfShape& LInt2, const TopAbs_State Side);
  
  Standard_EXPORT static void CheckBounds (const TopoDS_Face& F, const BRepOffset_Analyse& Analyse, Standard_Boolean& enlargeU, Standard_Boolean& enlargeVfirst, Standard_Boolean& enlargeVlast);
  
  //! if <ChangeGeom> is TRUE  ,   the surface  can  be
  //! changed .
  //! if <UpdatePCurve>  is  TRUE, update the  pcurves of the
  //! edges of <F> on   the new surface.if the surface has  been changed,
  //! Returns  True if The Surface of  <NF> has changed.
  Standard_EXPORT static Standard_Boolean EnLargeFace (const TopoDS_Face& F, TopoDS_Face& NF, const Standard_Boolean ChangeGeom, const Standard_Boolean UpDatePCurve = Standard_False, const Standard_Boolean enlargeU = Standard_True, const Standard_Boolean enlargeVfirst = Standard_True, const Standard_Boolean enlargeVlast = Standard_True);
  
  Standard_EXPORT static void ExtentFace (const TopoDS_Face& F, TopTools_DataMapOfShapeShape& ConstShapes, TopTools_DataMapOfShapeShape& ToBuild, const TopAbs_State Side, const Standard_Real TolConf, TopoDS_Face& NF);
  
  //! Via the wire explorer store in <NOnV1> for
  //! an Edge <E> of <W> his Edge neighbour on the first
  //! vertex <V1> of <E>.
  //! Store in NOnV2 the Neighbour of <E>on the last
  //! vertex <V2> of <E>.
  Standard_EXPORT static void BuildNeighbour (const TopoDS_Wire& W, const TopoDS_Face& F, TopTools_DataMapOfShapeShape& NOnV1, TopTools_DataMapOfShapeShape& NOnV2);
  
  //! Store in MVE for a vertex <V>  in <S> the incident
  //! edges <E> in <S>.
  //! An Edge is Store only one Time for a vertex.
  Standard_EXPORT static void MapVertexEdges (const TopoDS_Shape& S, TopTools_DataMapOfShapeListOfShape& MVE);
  
  //! Remove the non valid   part of an offsetshape
  //! 1 - Remove all the free boundary  and the faces
  //! connex to such edges.
  //! 2 - Remove all the shapes not  valid in the result
  //! (according to the side of offseting)
  //! in this verion only the first point is implemented.
  Standard_EXPORT static TopoDS_Shape Deboucle3D (const TopoDS_Shape& S, const TopTools_MapOfShape& Boundary);
  
  Standard_EXPORT static void CorrectOrientation (const TopoDS_Shape& SI, const TopTools_IndexedMapOfShape& NewEdges, Handle(BRepAlgo_AsDes)& AsDes, BRepAlgo_Image& InitOffset, const Standard_Real Offset);
  
  Standard_EXPORT static Standard_Real Gabarit (const Handle(Geom_Curve)& aCurve);

  //! Compares the normal directions of the planar faces and returns
  //! TRUE if the directions are the same with the given precision.<br>
  Standard_EXPORT static Standard_Boolean CheckPlanesNormals(const TopoDS_Face& theFace1,
                                                             const TopoDS_Face& theFace2,
                                                             const Standard_Real theTolAng = 1.e-8);

protected:

private:

};

#endif // _BRepOffset_Tool_HeaderFile
