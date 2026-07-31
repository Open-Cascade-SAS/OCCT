// Created on: 1995-06-22
// Created by: Flore Lantheaume
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

#ifndef _BRepFilletAPI_MakeChamfer_HeaderFile
#define _BRepFilletAPI_MakeChamfer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <ChFi3d_ChBuilder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <BRepFilletAPI_LocalOperation.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_CircSection.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Face;
class TopoDS_Vertex;
class TopOpeBRepBuild_HBuilder;

//! Describes functions to build chamfers on edges of a shell or solid.
//! Chamfered Edge of a Shell or Solid
//! A MakeChamfer object provides a framework for:
//! -   initializing the construction algorithm with a given shape,
//! -   acquiring the data characterizing the chamfers,
//! -   building the chamfers and constructing the resulting shape, and
//! -   consulting the result.
class BRepFilletAPI_MakeChamfer : public BRepFilletAPI_LocalOperation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes an algorithm for computing chamfers on the shape S.
  //! The edges on which chamfers are built are defined using the Add function.
  Standard_EXPORT BRepFilletAPI_MakeChamfer(const TopoDS_Shape& S);

  //! Adds edge E to the table of edges used by this
  //! algorithm to build chamfers, where the parameters
  //! of the chamfer must be set after the
  Standard_EXPORT void Add(const TopoDS_Edge& E) override;

  //! Adds edge E to the table of edges used by this
  //! algorithm to build chamfers, where
  //! the parameters of the chamfer are given by
  //! the distance Dis (symmetric chamfer).
  //! The Add function results in a contour being built by
  //! propagation from the edge E (i.e. the contour contains at
  //! least this edge). This contour is composed of edges of
  //! the shape which are tangential to one another and
  //! which delimit two series of tangential faces, with one
  //! series of faces being located on either side of the contour.
  //! Warning
  //! Nothing is done if edge E or the face F does not belong to the initial shape.
  Standard_EXPORT void Add(const double Dis, const TopoDS_Edge& E);

  //! Sets the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index
  //! IC generated using the Add function in the internal
  //! data structure of this algorithm. The face F identifies
  //! the side where Dis1 is measured.
  //! Warning
  //! Nothing is done if either the edge E or the face F
  //! does not belong to the initial shape.
  Standard_EXPORT void SetDist(const double Dis, const int IC, const TopoDS_Face& F);

  Standard_EXPORT void GetDist(const int IC, double& Dis) const;

  //! Adds edge E to the table of edges used by this
  //! algorithm to build chamfers, where
  //! the parameters of the chamfer are given by the two
  //! distances Dis1 and Dis2; the face F identifies the side
  //! where Dis1 is measured.
  //! The Add function results in a contour being built by
  //! propagation from the edge E (i.e. the contour contains at
  //! least this edge). This contour is composed of edges of
  //! the shape which are tangential to one another and
  //! which delimit two series of tangential faces, with one
  //! series of faces being located on either side of the contour.
  //! Warning
  //! Nothing is done if edge E or the face F does not belong to the initial shape.
  Standard_EXPORT void Add(const double       Dis1,
                           const double       Dis2,
                           const TopoDS_Edge& E,
                           const TopoDS_Face& F);

  //! Sets the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index
  //! IC generated using the Add function in the internal
  //! data structure of this algorithm. The face F identifies
  //! the side where Dis1 is measured.
  //! Warning
  //! Nothing is done if either the edge E or the face F
  //! does not belong to the initial shape.
  Standard_EXPORT void SetDists(const double       Dis1,
                                const double       Dis2,
                                const int          IC,
                                const TopoDS_Face& F);

  //! Returns the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! -1. is returned if IC is outside the bounds of the table of contours.
  Standard_EXPORT void Dists(const int IC, double& Dis1, double& Dis2) const;

  //! Adds a fillet contour in the builder (builds a
  //! contour of tangent edges to <E> and sets the
  //! distance <Dis1> and angle <Angle> ( parameters of the chamfer ) ).
  Standard_EXPORT void AddDA(const double       Dis,
                             const double       Angle,
                             const TopoDS_Edge& E,
                             const TopoDS_Face& F);

  //! set the distance <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS with <Dis> on <F>.
  //! if the face <F> is not one of common faces
  //! of an edge of the contour <IC>
  Standard_EXPORT void SetDistAngle(const double       Dis,
                                    const double       Angle,
                                    const int          IC,
                                    const TopoDS_Face& F);

  //! gives the distances <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS
  Standard_EXPORT void GetDistAngle(const int IC, double& Dis, double& Angle) const;

  //! Sets the mode of chamfer
  Standard_EXPORT void SetMode(const ChFiDS_ChamfMode theMode);

  //! return True if chamfer symmetric false else.
  Standard_EXPORT bool IsSymetric(const int IC) const;

  //! return True if chamfer is made with two distances false else.
  Standard_EXPORT bool IsTwoDistances(const int IC) const;

  //! return True if chamfer is made with distance and angle false else.
  Standard_EXPORT bool IsDistanceAngle(const int IC) const;

  //! Erases the chamfer parameters on the contour of
  //! index IC in the internal data structure of this algorithm.
  //! Use the SetDists function to reset this data.
  //! Warning
  //! Nothing is done if IC is outside the bounds of the table of contours.
  Standard_EXPORT void ResetContour(const int IC) override;

  //! Returns the number of contours generated using the
  //! Add function in the internal data structure of this algorithm.
  Standard_EXPORT int NbContours() const override;

  //! Returns the index of the contour in the internal data
  //! structure of this algorithm, which contains the edge E of the shape.
  //! This function returns 0 if the edge E does not belong to any contour.
  //! Warning
  //! This index can change if a contour is removed from the
  //! internal data structure of this algorithm using the function Remove.
  Standard_EXPORT int Contour(const TopoDS_Edge& E) const override;

  //! Returns the number of edges in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns 0 if I is outside the bounds of the table of contours.
  Standard_EXPORT int NbEdges(const int I) const override;

  //! Returns the edge of index J in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if:
  //! -   I is outside the bounds of the table of contours, or
  //! -   J is outside the bounds of the table of edges of the contour of index I.
  Standard_EXPORT const TopoDS_Edge& Edge(const int I, const int J) const override;

  //! Removes the contour in the internal data structure of
  //! this algorithm which contains the edge E of the shape.
  //! Warning
  //! Nothing is done if the edge E does not belong to the
  //! contour in the internal data structure of this algorithm.
  Standard_EXPORT void Remove(const TopoDS_Edge& E) override;

  //! Returns the length of the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if IC is outside the bounds of the table of contours.
  Standard_EXPORT double Length(const int IC) const override;

  //! Returns the first vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex FirstVertex(const int IC) const override;

  //! Returns the last vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex LastVertex(const int IC) const override;

  //! Returns the curvilinear abscissa of the vertex V on the
  //! contour of index IC in the internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT double Abscissa(const int IC, const TopoDS_Vertex& V) const override;

  //! Returns the relative curvilinear abscissa (i.e. between 0
  //! and 1) of the vertex V on the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT double RelativeAbscissa(const int IC, const TopoDS_Vertex& V) const override;

  //! eturns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed and tangential at the point of closure.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT bool ClosedAndTangent(const int IC) const override;

  //! Returns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT bool Closed(const int IC) const override;

  //! Builds the chamfers on all the contours in the internal
  //! data structure of this algorithm and constructs the resulting shape.
  //! Use the function IsDone to verify that the chamfered
  //! shape is built. Use the function Shape to retrieve the chamfered shape.
  //! Warning
  //! The construction of chamfers implements highly complex
  //! construction algorithms. Consequently, there may be
  //! instances where the algorithm fails, for example if the
  //! data defining the parameters of the chamfer is not
  //! compatible with the geometry of the initial shape. There
  //! is no initial analysis of errors and these only become
  //! evident at the construction stage.
  //! Additionally, in the current software release, the following
  //! cases are not handled:
  //! -   the end point of the contour is the point of
  //! intersection of 4 or more edges of the shape, or
  //! -   the intersection of the chamfer with a face which
  //! limits the contour is not fully contained in this face.
  Standard_EXPORT void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) override;

  //! Reinitializes this algorithm, thus canceling the effects of the Build function.
  //! This function allows modifications to be made to the
  //! contours and chamfer parameters in order to rebuild the shape.
  Standard_EXPORT void Reset() override;

  //! Returns the internal filleting algorithm.
  Standard_EXPORT occ::handle<TopOpeBRepBuild_HBuilder> Builder() const;

  //! Returns the list of shapes generated from the
  //! shape <EorV>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Generated(
    const TopoDS_Shape& EorV) override;

  //! Returns the list of shapes modified from the shape
  //! <F>.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& Modified(const TopoDS_Shape& F) override;

  Standard_EXPORT bool IsDeleted(const TopoDS_Shape& F) override;

  Standard_EXPORT void Simulate(const int IC) override;

  Standard_EXPORT int NbSurf(const int IC) const override;

  Standard_EXPORT occ::handle<NCollection_HArray1<ChFiDS_CircSection>> Sect(
    const int IC,
    const int IS) const override;

private:
  ChFi3d_ChBuilder                                       myBuilder;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> myMap;
};

#endif // _BRepFilletAPI_MakeChamfer_HeaderFile
