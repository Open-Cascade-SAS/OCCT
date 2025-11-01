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
#include <TopTools_MapOfShape.hxx>
#include <BRepFilletAPI_LocalOperation.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <ChFiDS_SecHArray1.hxx>
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
  Standard_EXPORT void Add(const TopoDS_Edge& E) Standard_OVERRIDE;

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
  Standard_EXPORT void Add(const Standard_Real Dis, const TopoDS_Edge& E);

  //! Sets the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index
  //! IC generated using the Add function in the internal
  //! data structure of this algorithm. The face F identifies
  //! the side where Dis1 is measured.
  //! Warning
  //! Nothing is done if either the edge E or the face F
  //! does not belong to the initial shape.
  Standard_EXPORT void SetDist(const Standard_Real    Dis,
                               const Standard_Integer IC,
                               const TopoDS_Face&     F);

  Standard_EXPORT void GetDist(const Standard_Integer IC, Standard_Real& Dis) const;

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
  Standard_EXPORT void Add(const Standard_Real Dis1,
                           const Standard_Real Dis2,
                           const TopoDS_Edge&  E,
                           const TopoDS_Face&  F);

  //! Sets the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index
  //! IC generated using the Add function in the internal
  //! data structure of this algorithm. The face F identifies
  //! the side where Dis1 is measured.
  //! Warning
  //! Nothing is done if either the edge E or the face F
  //! does not belong to the initial shape.
  Standard_EXPORT void SetDists(const Standard_Real    Dis1,
                                const Standard_Real    Dis2,
                                const Standard_Integer IC,
                                const TopoDS_Face&     F);

  //! Returns the distances Dis1 and Dis2 which give the
  //! parameters of the chamfer along the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! -1. is returned if IC is outside the bounds of the table of contours.
  Standard_EXPORT void Dists(const Standard_Integer IC,
                             Standard_Real&         Dis1,
                             Standard_Real&         Dis2) const;

  //! Adds a  fillet contour in  the  builder  (builds a
  //! contour  of tangent edges to <E> and sets the
  //! distance <Dis1> and angle <Angle> ( parameters of the chamfer ) ).
  Standard_EXPORT void AddDA(const Standard_Real Dis,
                             const Standard_Real Angle,
                             const TopoDS_Edge&  E,
                             const TopoDS_Face&  F);

  //! set the distance <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS with <Dis> on <F>.
  //! if the face <F> is not one of common faces
  //! of an edge of the contour <IC>
  Standard_EXPORT void SetDistAngle(const Standard_Real    Dis,
                                    const Standard_Real    Angle,
                                    const Standard_Integer IC,
                                    const TopoDS_Face&     F);

  //! gives the distances <Dis> and <Angle> of the fillet
  //! contour of index <IC> in the DS
  Standard_EXPORT void GetDistAngle(const Standard_Integer IC,
                                    Standard_Real&         Dis,
                                    Standard_Real&         Angle) const;

  //! Sets the mode of chamfer
  Standard_EXPORT void SetMode(const ChFiDS_ChamfMode theMode);

  //! return True if chamfer symmetric false else.
  Standard_EXPORT Standard_Boolean IsSymetric(const Standard_Integer IC) const;

  //! return True if chamfer is made with two distances false else.
  Standard_EXPORT Standard_Boolean IsTwoDistances(const Standard_Integer IC) const;

  //! return True if chamfer is made with distance and angle false else.
  Standard_EXPORT Standard_Boolean IsDistanceAngle(const Standard_Integer IC) const;

  //! Erases the chamfer parameters on the contour of
  //! index IC in the internal data structure of this algorithm.
  //! Use the SetDists function to reset this data.
  //! Warning
  //! Nothing is done if IC is outside the bounds of the table of contours.
  Standard_EXPORT void ResetContour(const Standard_Integer IC) Standard_OVERRIDE;

  //! Returns the number of contours generated using the
  //! Add function in the internal data structure of this algorithm.
  Standard_EXPORT Standard_Integer NbContours() const Standard_OVERRIDE;

  //! Returns the index of the contour in the internal data
  //! structure of this algorithm, which contains the edge E of the shape.
  //! This function returns 0 if the edge E does not belong to any contour.
  //! Warning
  //! This index can change if a contour is removed from the
  //! internal data structure of this algorithm using the function Remove.
  Standard_EXPORT Standard_Integer Contour(const TopoDS_Edge& E) const Standard_OVERRIDE;

  //! Returns the number of edges in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns 0 if I is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Integer NbEdges(const Standard_Integer I) const Standard_OVERRIDE;

  //! Returns the edge of index J in the contour of index I in
  //! the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if:
  //! -   I is outside the bounds of the table of contours, or
  //! -   J is outside the bounds of the table of edges of the contour of index I.
  Standard_EXPORT const TopoDS_Edge& Edge(const Standard_Integer I,
                                          const Standard_Integer J) const Standard_OVERRIDE;

  //! Removes the contour in the internal data structure of
  //! this algorithm which contains the edge E of the shape.
  //! Warning
  //! Nothing is done if the edge E does not belong to the
  //! contour in the internal data structure of this algorithm.
  Standard_EXPORT void Remove(const TopoDS_Edge& E) Standard_OVERRIDE;

  //! Returns the length of the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Real Length(const Standard_Integer IC) const Standard_OVERRIDE;

  //! Returns the first vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex FirstVertex(const Standard_Integer IC) const Standard_OVERRIDE;

  //! Returns the last vertex of the contour of index IC
  //! in the internal data structure of this algorithm.
  //! Warning
  //! Returns a null shape if IC is outside the bounds of the table of contours.
  Standard_EXPORT TopoDS_Vertex LastVertex(const Standard_Integer IC) const Standard_OVERRIDE;

  //! Returns the curvilinear abscissa of the vertex V on the
  //! contour of index IC in the internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT Standard_Real Abscissa(const Standard_Integer IC,
                                         const TopoDS_Vertex&   V) const Standard_OVERRIDE;

  //! Returns the relative curvilinear abscissa (i.e. between 0
  //! and 1) of the vertex V on the contour of index IC in the
  //! internal data structure of this algorithm.
  //! Warning
  //! Returns -1. if:
  //! -   IC is outside the bounds of the table of contours, or
  //! -   V is not on the contour of index IC.
  Standard_EXPORT Standard_Real RelativeAbscissa(const Standard_Integer IC,
                                                 const TopoDS_Vertex&   V) const Standard_OVERRIDE;

  //! eturns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed and tangential at the point of closure.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Boolean
    ClosedAndTangent(const Standard_Integer IC) const Standard_OVERRIDE;

  //! Returns true if the contour of index IC in the internal
  //! data structure of this algorithm is closed.
  //! Warning
  //! Returns false if IC is outside the bounds of the table of contours.
  Standard_EXPORT Standard_Boolean Closed(const Standard_Integer IC) const Standard_OVERRIDE;

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
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! Reinitializes this algorithm, thus canceling the effects of the Build function.
  //! This function allows modifications to be made to the
  //! contours and chamfer parameters in order to rebuild the shape.
  Standard_EXPORT void Reset() Standard_OVERRIDE;

  //! Returns the internal filleting algorithm.
  Standard_EXPORT Handle(TopOpeBRepBuild_HBuilder) Builder() const;

  //! Returns the  list   of shapes generated   from the
  //! shape <EorV>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated(const TopoDS_Shape& EorV)
    Standard_OVERRIDE;

  //! Returns the list  of shapes modified from the shape
  //! <F>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified(const TopoDS_Shape& F)
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean IsDeleted(const TopoDS_Shape& F) Standard_OVERRIDE;

  Standard_EXPORT void Simulate(const Standard_Integer IC) Standard_OVERRIDE;

  Standard_EXPORT Standard_Integer NbSurf(const Standard_Integer IC) const Standard_OVERRIDE;

  Standard_EXPORT Handle(ChFiDS_SecHArray1) Sect(const Standard_Integer IC,
                                                 const Standard_Integer IS) const Standard_OVERRIDE;

protected:
private:
  ChFi3d_ChBuilder    myBuilder;
  TopTools_MapOfShape myMap;
};

#endif // _BRepFilletAPI_MakeChamfer_HeaderFile
