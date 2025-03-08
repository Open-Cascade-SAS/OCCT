// Created on: 1998-01-29
// Created by: Laurent BOURESCHE
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFilletAPI_LocalOperation_HeaderFile
#define _BRepFilletAPI_LocalOperation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Integer.hxx>
#include <ChFiDS_SecHArray1.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;

//! Construction of fillets on the edges of a Shell.
class BRepFilletAPI_LocalOperation : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds a  contour in  the  builder  (builds a
  //! contour  of tangent edges).
  Standard_EXPORT virtual void Add(const TopoDS_Edge& E) = 0;

  //! Reset the contour of index IC, there is nomore
  //! information in the contour.
  Standard_EXPORT virtual void ResetContour(const Standard_Integer IC) = 0;

  //! Number of contours.
  Standard_EXPORT virtual Standard_Integer NbContours() const = 0;

  //! Returns the index of  the  contour containing the edge
  //! E, returns 0 if E doesn't belong to any contour.
  Standard_EXPORT virtual Standard_Integer Contour(const TopoDS_Edge& E) const = 0;

  //! Number of Edges in the contour I.
  Standard_EXPORT virtual Standard_Integer NbEdges(const Standard_Integer I) const = 0;

  //! Returns the Edge J in the contour I.
  Standard_EXPORT virtual const TopoDS_Edge& Edge(const Standard_Integer I,
                                                  const Standard_Integer J) const = 0;

  //! remove the contour containing the Edge E.
  Standard_EXPORT virtual void Remove(const TopoDS_Edge& E) = 0;

  //! returns the length the contour of index IC.
  Standard_EXPORT virtual Standard_Real Length(const Standard_Integer IC) const = 0;

  //! Returns the first Vertex of the contour of index IC.
  Standard_EXPORT virtual TopoDS_Vertex FirstVertex(const Standard_Integer IC) const = 0;

  //! Returns the last Vertex of the contour of index IC.
  Standard_EXPORT virtual TopoDS_Vertex LastVertex(const Standard_Integer IC) const = 0;

  //! returns the abscissa of the vertex V on
  //! the contour of index IC.
  Standard_EXPORT virtual Standard_Real Abscissa(const Standard_Integer IC,
                                                 const TopoDS_Vertex&   V) const = 0;

  //! returns the relative abscissa([0.,1.]) of the
  //! vertex V on the contour of index IC.
  Standard_EXPORT virtual Standard_Real RelativeAbscissa(const Standard_Integer IC,
                                                         const TopoDS_Vertex&   V) const = 0;

  //! returns true if the contour of index IC is closed
  //! an tangent.
  Standard_EXPORT virtual Standard_Boolean ClosedAndTangent(const Standard_Integer IC) const = 0;

  //! returns true if the contour of index IC is closed
  Standard_EXPORT virtual Standard_Boolean Closed(const Standard_Integer IC) const = 0;

  //! Reset all the fields updated   by Build operation  and
  //! leave the  algorithm in  the  same state  than  before
  //! build    call.  It   allows   contours    and   radius
  //! modifications  to build the result another time.
  Standard_EXPORT virtual void Reset() = 0;

  Standard_EXPORT virtual void Simulate(const Standard_Integer IC) = 0;

  Standard_EXPORT virtual Standard_Integer NbSurf(const Standard_Integer IC) const = 0;

  Standard_EXPORT virtual Handle(ChFiDS_SecHArray1) Sect(const Standard_Integer IC,
                                                         const Standard_Integer IS) const = 0;

protected:
private:
};

#endif // _BRepFilletAPI_LocalOperation_HeaderFile
