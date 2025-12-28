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

#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_State.hxx>
#include <Standard_Real.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_IndexedMap.hxx>
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
  Standard_EXPORT static void EdgeVertices(const TopoDS_Edge& E,
                                           TopoDS_Vertex&     V1,
                                           TopoDS_Vertex&     V2);

  //! <E> is a section between <F1> and <F2>. Computes
  //! <O1> the orientation of <E> in <F1> influenced by <F2>.
  //! idem for <O2>.
  Standard_EXPORT static void OrientSection(const TopoDS_Edge&  E,
                                            const TopoDS_Face&  F1,
                                            const TopoDS_Face&  F2,
                                            TopAbs_Orientation& O1,
                                            TopAbs_Orientation& O2);

  //! Looks for the common Vertices and Edges between faces <theF1> and <theF2>.
  //! Returns TRUE if common shapes have been found.
  //! <theLE> will contain the found common edges;
  //! <theLV> will contain the found common vertices.
  Standard_EXPORT static bool FindCommonShapes(const TopoDS_Face&              theF1,
                                               const TopoDS_Face&              theF2,
                                               NCollection_List<TopoDS_Shape>& theLE,
                                               NCollection_List<TopoDS_Shape>& theLV);

  //! Looks for the common shapes of type <theType> between shapes <theS1> and <theS2>.
  //! Returns TRUE if common shapes have been found.
  //! <theLSC> will contain the found common shapes.
  Standard_EXPORT static bool FindCommonShapes(const TopoDS_Shape&             theS1,
                                               const TopoDS_Shape&             theS2,
                                               const TopAbs_ShapeEnum          theType,
                                               NCollection_List<TopoDS_Shape>& theLSC);

  //! Computes the Section between <F1> and <F2> the
  //! edges solution are stored in <LInt1> with the
  //! orientation on <F1>, the sames edges are stored in
  //! <Lint2> with the orientation on <F2>.
  Standard_EXPORT static void Inter3D(const TopoDS_Face&              F1,
                                      const TopoDS_Face&              F2,
                                      NCollection_List<TopoDS_Shape>& LInt1,
                                      NCollection_List<TopoDS_Shape>& LInt2,
                                      const TopAbs_State              Side,
                                      const TopoDS_Edge&              RefEdge,
                                      const TopoDS_Face&              RefFace1,
                                      const TopoDS_Face&              RefFace2);

  //! Find if the edges <Edges> of the face <F2> are on
  //! the face <F1>.
  //! Set in <LInt1> <LInt2> the updated edges.
  //! If all the edges are computed, returns true.
  Standard_EXPORT static bool TryProject(const TopoDS_Face&                    F1,
                                         const TopoDS_Face&                    F2,
                                         const NCollection_List<TopoDS_Shape>& Edges,
                                         NCollection_List<TopoDS_Shape>&       LInt1,
                                         NCollection_List<TopoDS_Shape>&       LInt2,
                                         const TopAbs_State                    Side,
                                         const double                          TolConf);

  Standard_EXPORT static void PipeInter(const TopoDS_Face&              F1,
                                        const TopoDS_Face&              F2,
                                        NCollection_List<TopoDS_Shape>& LInt1,
                                        NCollection_List<TopoDS_Shape>& LInt2,
                                        const TopAbs_State              Side);

  Standard_EXPORT static void Inter2d(const TopoDS_Face&              F,
                                      const TopoDS_Edge&              E1,
                                      const TopoDS_Edge&              E2,
                                      NCollection_List<TopoDS_Shape>& LV,
                                      const double                    Tol);

  Standard_EXPORT static void InterOrExtent(const TopoDS_Face&              F1,
                                            const TopoDS_Face&              F2,
                                            NCollection_List<TopoDS_Shape>& LInt1,
                                            NCollection_List<TopoDS_Shape>& LInt2,
                                            const TopAbs_State              Side);

  Standard_EXPORT static void CheckBounds(const TopoDS_Face&        F,
                                          const BRepOffset_Analyse& Analyse,
                                          bool&                     enlargeU,
                                          bool&                     enlargeVfirst,
                                          bool&                     enlargeVlast);

  //! Returns True if The Surface of <NF> has changed.
  //! if <ChangeGeom> is TRUE the surface can be
  //! changed .
  //! if <UpdatePCurve> is TRUE, update the pcurves of the
  //! edges of <F> on the new surface if the surface has been changed.
  //! <enlargeU>, <enlargeVfirst>, <enlargeVlast> allow or forbid
  //! enlargement in U and V directions correspondingly.
  //! <theExtensionMode> is a mode of extension of the surface of the face:
  //! if <theExtensionMode> equals 1, potentially infinite surfaces are extended by maximum value,
  //! and limited surfaces are extended by 25%.
  //! if <theExtensionMode> equals 2, potentially infinite surfaces are extended by
  //! 10*(correspondent size of face),
  //! and limited surfaces are extended by 100%.
  //! <theLenBeforeUfirst>, <theLenAfterUlast>, <theLenBeforeVfirst>, <theLenAfterVlast>
  //! set the values of enlargement on correspondent directions.
  //! If some of them equals -1, the default value of enlargement is used.
  Standard_EXPORT static bool EnLargeFace(const TopoDS_Face& F,
                                          TopoDS_Face&       NF,
                                          const bool         ChangeGeom,
                                          const bool         UpDatePCurve       = false,
                                          const bool         enlargeU           = true,
                                          const bool         enlargeVfirst      = true,
                                          const bool         enlargeVlast       = true,
                                          const int          theExtensionMode   = 1,
                                          const double       theLenBeforeUfirst = -1.,
                                          const double       theLenAfterUlast   = -1.,
                                          const double       theLenBeforeVfirst = -1.,
                                          const double       theLenAfterVlast   = -1.);

  Standard_EXPORT static void ExtentFace(
    const TopoDS_Face&                                                        F,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& ConstShapes,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& ToBuild,
    const TopAbs_State                                                        Side,
    const double                                                              TolConf,
    TopoDS_Face&                                                              NF);

  //! Via the wire explorer store in <NOnV1> for
  //! an Edge <E> of <W> his Edge neighbour on the first
  //! vertex <V1> of <E>.
  //! Store in NOnV2 the Neighbour of <E>on the last
  //! vertex <V2> of <E>.
  Standard_EXPORT static void BuildNeighbour(
    const TopoDS_Wire&                                                        W,
    const TopoDS_Face&                                                        F,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& NOnV1,
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& NOnV2);

  //! Store in MVE for a vertex <V> in <S> the incident
  //! edges <E> in <S>.
  //! An Edge is Store only one Time for a vertex.
  Standard_EXPORT static void MapVertexEdges(
    const TopoDS_Shape& S,
    NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>&
      MVE);

  //! Remove the non valid part of an offsetshape
  //! 1 - Remove all the free boundary and the faces
  //! connex to such edges.
  //! 2 - Remove all the shapes not valid in the result
  //! (according to the side of offsetting)
  //! in this version only the first point is implemented.
  Standard_EXPORT static TopoDS_Shape Deboucle3D(
    const TopoDS_Shape&                                           S,
    const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Boundary);

  Standard_EXPORT static void CorrectOrientation(
    const TopoDS_Shape&                                                  SI,
    const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& NewEdges,
    occ::handle<BRepAlgo_AsDes>&                                         AsDes,
    BRepAlgo_Image&                                                      InitOffset,
    const double                                                         Offset);

  Standard_EXPORT static double Gabarit(const occ::handle<Geom_Curve>& aCurve);

  //! Compares the normal directions of the planar faces and returns
  //! TRUE if the directions are the same with the given precision.
  Standard_EXPORT static bool CheckPlanesNormals(const TopoDS_Face& theFace1,
                                                 const TopoDS_Face& theFace2,
                                                 const double       theTolAng = 1.e-8);
};

#endif // _BRepOffset_Tool_HeaderFile
