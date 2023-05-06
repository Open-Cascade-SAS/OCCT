// Created on: 1992-08-28
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _BRepTools_HeaderFile
#define _BRepTools_HeaderFile

#include <TopTools_FormatVersion.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_OStream.hxx>
#include <Standard_IStream.hxx>
#include <Message_ProgressRange.hxx>
#include <TopTools_ListOfShape.hxx>

class TopoDS_Face;
class TopoDS_Wire;
class TopoDS_Edge;
class Bnd_Box2d;
class TopoDS_Vertex;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;
class TopoDS_Shape;
class BRep_Builder;
class Geom_Curve;
class Geom2d_Curve;
class Geom_Surface;
class OSD_FileSystem;


//! The BRepTools package provides  utilities for BRep
//! data structures.
//!
//! * WireExplorer : A tool to explore the topology of
//! a wire in the order of the edges.
//!
//! * ShapeSet :  Tools used for  dumping, writing and
//! reading.
//!
//! * UVBounds : Methods to compute the  limits of the
//! boundary  of a  face,  a wire or   an edge in  the
//! parametric space of a face.
//!
//! *  Update : Methods  to call when   a topology has
//! been created to compute all missing data.
//!
//! * UpdateFaceUVPoints: Method to update the UV points
//! stored with the edges on a face.
//!
//! * Compare : Method to compare two vertices.
//!
//! * Compare : Method to compare two edges.
//!
//! * OuterWire : A method to find the outer wire of a
//! face.
//!
//! * Map3DEdges : A method to map all the 3D Edges of
//! a Shape.
//!
//! * Dump : A method to dump a BRep object.
class BRepTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values of the wire in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, const TopoDS_Wire& W, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Returns in UMin,  UMax, VMin,  VMax  the  bounding
  //! values of the edge in the parametric space of F.
  Standard_EXPORT static void UVBounds (const TopoDS_Face& F, const TopoDS_Edge& E, Standard_Real& UMin, Standard_Real& UMax, Standard_Real& VMin, Standard_Real& VMax);
  
  //! Adds  to  the box <B>  the bounding values in  the
  //! parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, Bnd_Box2d& B);
  
  //! Adds  to the box  <B>  the bounding  values of the
  //! wire in the parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, const TopoDS_Wire& W, Bnd_Box2d& B);
  
  //! Adds to  the box <B>  the  bounding values  of the
  //! edge in the parametric space of F.
  Standard_EXPORT static void AddUVBounds (const TopoDS_Face& F, const TopoDS_Edge& E, Bnd_Box2d& B);
  
  //! Update a vertex (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Vertex& V);
  
  //! Update an edge, compute 2d bounding boxes.
  Standard_EXPORT static void Update (const TopoDS_Edge& E);
  
  //! Update a wire (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Wire& W);
  
  //! Update a Face, update UV points.
  Standard_EXPORT static void Update (const TopoDS_Face& F);
  
  //! Update a shell (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Shell& S);
  
  //! Update a solid (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Solid& S);
  
  //! Update a composite solid (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_CompSolid& C);
  
  //! Update a compound (nothing is done)
  Standard_EXPORT static void Update (const TopoDS_Compound& C);
  
  //! Update a shape, call the correct update.
  Standard_EXPORT static void Update (const TopoDS_Shape& S);
  
  //! For each edge of the face <F> reset the UV points
  //! to the bounding points of the parametric curve of the
  //! edge on the face.
  Standard_EXPORT static void UpdateFaceUVPoints (const TopoDS_Face& theF);
  
  //! Removes all cached polygonal representation of the shape,
  //! i.e. the triangulations of the faces of <S> and polygons on
  //! triangulations and polygons 3d of the edges.
  //! In case polygonal representation is the only available representation
  //! for the shape (shape does not have geometry) it is not removed.
  //! @param theShape  [in] the shape to clean
  //! @param theForce  [in] allows removing all polygonal representations from the shape,
  //!                       including polygons on triangulations irrelevant for the faces of the given shape.
  Standard_EXPORT static void Clean (const TopoDS_Shape& theShape, const Standard_Boolean theForce = Standard_False);
  
  //! Removes geometry (curves and surfaces) from all edges and faces of the shape
  Standard_EXPORT static void CleanGeometry(const TopoDS_Shape& theShape);

  //! Removes all the pcurves of the edges of <S> that
  //! refer to surfaces not belonging to any face of <S>
  Standard_EXPORT static void RemoveUnusedPCurves (const TopoDS_Shape& S);

public:

  //! Verifies that each Face from the shape has got a triangulation with a deflection smaller or equal to specified one
  //! and the Edges a discretization on this triangulation.
  //! @param theShape   [in] shape to verify
  //! @param theLinDefl [in] maximum allowed linear deflection
  //! @param theToCheckFreeEdges [in] if TRUE, then free Edges are required to have 3D polygon
  //! @return FALSE if input Shape contains Faces without triangulation,
  //!               or that triangulation has worse (greater) deflection than specified one,
  //!               or Edges in Shape lack polygons on triangulation
  //!               or free Edges in Shape lack 3D polygons
  Standard_EXPORT static Standard_Boolean Triangulation (const TopoDS_Shape& theShape,
                                                         const Standard_Real theLinDefl,
                                                         const Standard_Boolean theToCheckFreeEdges = Standard_False);

  //! Loads triangulation data for each face of the shape
  //! from some deferred storage using specified shared input file system
  //! @param theShape            [in] shape to load triangulations
  //! @param theTriangulationIdx [in] index defining what triangulation should be loaded. Starts from 0.
  //!        -1 is used in specific case to load currently already active triangulation.
  //!        If some face doesn't contain triangulation with this index, nothing will be loaded for it.
  //!        Exception will be thrown in case of invalid negative index
  //! @param theToSetAsActive    [in] flag to activate triangulation after its loading
  //! @param theFileSystem       [in] shared file system
  //! @return TRUE if at least one triangulation is loaded.
  Standard_EXPORT static Standard_Boolean LoadTriangulation (const TopoDS_Shape& theShape,
                                                             const Standard_Integer theTriangulationIdx = -1,
                                                             const Standard_Boolean theToSetAsActive = Standard_False,
                                                             const Handle(OSD_FileSystem)& theFileSystem = Handle(OSD_FileSystem)());

  //! Releases triangulation data for each face of the shape if there is deferred storage to load it later
  //! @param theShape            [in] shape to unload triangulations
  //! @param theTriangulationIdx [in] index defining what triangulation should be unloaded. Starts from 0.
  //!        -1 is used in specific case to unload currently already active triangulation.
  //!        If some face doesn't contain triangulation with this index, nothing will be unloaded for it.
  //!        Exception will be thrown in case of invalid negative index
  //! @return TRUE if at least one triangulation is unloaded.
  Standard_EXPORT static Standard_Boolean UnloadTriangulation (const TopoDS_Shape& theShape,
                                                               const Standard_Integer theTriangulationIdx = -1);

  //! Activates triangulation data for each face of the shape
  //! from some deferred storage using specified shared input file system
  //! @param theShape              [in] shape to activate triangulations
  //! @param theTriangulationIdx   [in] index defining what triangulation should be activated. Starts from 0.
  //!        Exception will be thrown in case of invalid negative index
  //! @param theToActivateStrictly [in] flag to activate exactly triangulation with defined theTriangulationIdx index.
  //!        In TRUE case if some face doesn't contain triangulation with this index, active triangulation
  //!        will not be changed for it. Else the last available triangulation will be activated.
  //! @return TRUE if at least one active triangulation was changed.
  Standard_EXPORT static Standard_Boolean ActivateTriangulation (const TopoDS_Shape& theShape,
                                                                 const Standard_Integer theTriangulationIdx,
                                                                 const Standard_Boolean theToActivateStrictly = false);

  //! Loads all available triangulations for each face of the shape
  //! from some deferred storage using specified shared input file system
  //! @param theShape      [in] shape to load triangulations
  //! @param theFileSystem [in] shared file system
  //! @return TRUE if at least one triangulation is loaded.
  Standard_EXPORT static Standard_Boolean LoadAllTriangulations (const TopoDS_Shape& theShape,
                                                                 const Handle(OSD_FileSystem)& theFileSystem = Handle(OSD_FileSystem)());

  //! Releases all available triangulations for each face of the shape if there is deferred storage to load them later
  //! @param theShape      [in] shape to unload triangulations
  //! @return TRUE if at least one triangulation is unloaded.
  Standard_EXPORT static Standard_Boolean UnloadAllTriangulations (const TopoDS_Shape& theShape);

public:

  //! Returns  True if  the    distance between the  two
  //! vertices is lower than their tolerance.
  Standard_EXPORT static Standard_Boolean Compare (const TopoDS_Vertex& V1, const TopoDS_Vertex& V2);
  
  //! Returns  True if  the    distance between the  two
  //! edges is lower than their tolerance.
  Standard_EXPORT static Standard_Boolean Compare (const TopoDS_Edge& E1, const TopoDS_Edge& E2);
  
  //! Returns the outer most wire of <F>. Returns a Null
  //! wire if <F> has no wires.
  Standard_EXPORT static TopoDS_Wire OuterWire (const TopoDS_Face& F);
  
  //! Stores in the map  <M> all the 3D topology edges
  //! of <S>.
  Standard_EXPORT static void Map3DEdges (const TopoDS_Shape& S, TopTools_IndexedMapOfShape& M);
  
  //! Verifies that the edge  <E> is found two  times on
  //! the face <F> before calling BRep_Tool::IsClosed.
  Standard_EXPORT static Standard_Boolean IsReallyClosed (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Detect closedness of face in U and V directions
  Standard_EXPORT static void DetectClosedness (const TopoDS_Face& theFace,
                                                Standard_Boolean&  theUclosed,
                                                Standard_Boolean&  theVclosed);
  
  //! Dumps the topological structure and the geometry
  //! of <Sh> on the stream <S>.
  Standard_EXPORT static void Dump (const TopoDS_Shape& Sh, Standard_OStream& S);

  //! Writes the shape to the stream in an ASCII format TopTools_FormatVersion_VERSION_1.
  //! This alias writes shape with triangulation data.
  //! @param theShape [in]       the shape to write
  //! @param theStream [in][out] the stream to output shape into
  //! @param theRange            the range of progress indicator to fill in
  static void Write (const TopoDS_Shape& theShape,
                     Standard_OStream& theStream,
                     const Message_ProgressRange& theProgress = Message_ProgressRange())
  {
    Write (theShape, theStream, Standard_True, Standard_False,
           TopTools_FormatVersion_CURRENT, theProgress);
  }

  //! Writes the shape to the stream in an ASCII format of specified version.
  //! @param theShape [in]         the shape to write
  //! @param theStream [in][out]   the stream to output shape into
  //! @param theWithTriangles [in] flag which specifies whether to save shape with (TRUE) or without (FALSE) triangles;
  //!                              has no effect on triangulation-only geometry
  //! @param theWithNormals [in]   flag which specifies whether to save triangulation with (TRUE) or without (FALSE) normals;
  //!                              has no effect on triangulation-only geometry
  //! @param theVersion [in]       the TopTools format version
  //! @param theProgress the range of progress indicator to fill in
  Standard_EXPORT static void Write (const TopoDS_Shape& theShape,
                                     Standard_OStream& theStream,
                                     const Standard_Boolean theWithTriangles,
                                     const Standard_Boolean theWithNormals,
                                     const TopTools_FormatVersion theVersion,
                                     const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Reads a Shape  from <S> in  returns it in  <Sh>.
  //! <B> is used to build the shape.
  Standard_EXPORT static void Read (TopoDS_Shape& Sh, Standard_IStream& S, const BRep_Builder& B,
                                    const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Writes the shape to the file in an ASCII format TopTools_FormatVersion_VERSION_1.
  //! This alias writes shape with triangulation data.
  //! @param theShape [in] the shape to write
  //! @param theFile [in]  the path to file to output shape into
  //! @param theProgress the range of progress indicator to fill in
  static Standard_Boolean Write (const TopoDS_Shape& theShape,
                                 const Standard_CString theFile,
                                 const Message_ProgressRange& theProgress = Message_ProgressRange())
  {
    return Write (theShape, theFile, Standard_True, Standard_False,
                  TopTools_FormatVersion_CURRENT, theProgress);
  }

  //! Writes the shape to the file in an ASCII format of specified version.
  //! @param theShape [in]         the shape to write
  //! @param theFile [in]          the path to file to output shape into
  //! @param theWithTriangles [in] flag which specifies whether to save shape with (TRUE) or without (FALSE) triangles;
  //!                              has no effect on triangulation-only geometry
  //! @param theWithNormals [in]   flag which specifies whether to save triangulation with (TRUE) or without (FALSE) normals;
  //!                              has no effect on triangulation-only geometry
  //! @param theVersion [in]       the TopTools format version
  //! @param theProgress the range of progress indicator to fill in
  Standard_EXPORT static Standard_Boolean Write (const TopoDS_Shape& theShape,
                                                 const Standard_CString theFile,
                                                 const Standard_Boolean theWithTriangles,
                                                 const Standard_Boolean theWithNormals,
                                                 const TopTools_FormatVersion theVersion,
                                                 const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Reads a Shape  from <File>,  returns it in  <Sh>.
  //! <B> is used to build the shape.
  Standard_EXPORT static Standard_Boolean Read (TopoDS_Shape& Sh, const Standard_CString File,
                                                const BRep_Builder& B,
                                                const Message_ProgressRange& theProgress = Message_ProgressRange());

  //! Evals real tolerance of edge  <theE>.
  //! <theC3d>, <theC2d>, <theS>, <theF>, <theL> are
  //! correspondently 3d curve of edge, 2d curve on surface <theS> and
  //! rang of edge
  //! If calculated tolerance is more then current edge tolerance, edge is updated.
  //! Method returns actual tolerance of edge
  Standard_EXPORT static Standard_Real EvalAndUpdateTol(const TopoDS_Edge& theE, 
                                                        const Handle(Geom_Curve)& theC3d, 
                                                        const Handle(Geom2d_Curve)& theC2d, 
                                                        const Handle(Geom_Surface)& theS,
                                                        const Standard_Real theF,
                                                        const Standard_Real theL);

  //! returns the cumul  of the orientation  of <Edge>
  //! and thc containing wire in <Face>
  Standard_EXPORT static TopAbs_Orientation OriEdgeInFace(const TopoDS_Edge& theEdge, 
                                                          const TopoDS_Face& theFace);

  //! Removes internal sub-shapes from the shape.
  //! The check on internal status is based on orientation of sub-shapes,
  //! classification is not performed.
  //! Before removal of internal sub-shapes the algorithm checks if such
  //! removal is not going to break topological connectivity between sub-shapes.
  //! The flag <theForce> if set to true disables the connectivity check and clears
  //! the given shape from all sub-shapes with internal orientation.
  Standard_EXPORT static void RemoveInternals(TopoDS_Shape& theS,
    const Standard_Boolean theForce = Standard_False);

  //! Check all locations of shape according criterium:
  //! aTrsf.IsNegative() || (Abs(Abs(aTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec())
  //! All sub-shapes having such locations are put in list theProblemShapes
  Standard_EXPORT static void CheckLocations(const TopoDS_Shape& theS,
                                             TopTools_ListOfShape& theProblemShapes);

};

#endif // _BRepTools_HeaderFile
