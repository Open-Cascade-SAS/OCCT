// Created on: 1993-06-16
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

#ifndef _TopOpeBRepBuild_WireEdgeSet_HeaderFile
#define _TopOpeBRepBuild_WireEdgeSet_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Face.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
class TopoDS_Shape;
class gp_Pnt2d;
class gp_Vec2d;
class TopoDS_Edge;
class TCollection_AsciiString;

//! a bound is a wire, a boundelement is an edge.
//! The ShapeSet stores :
//! - a list of wire (bounds),
//! - a list of edge (boundelements) to start reconstructions,
//! - a map of vertex giving the list of edge incident to a vertex.
class TopOpeBRepBuild_WireEdgeSet : public TopOpeBRepBuild_ShapeSet
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a WireEdgeSet to build edges connected by vertices
  //! on face F. Edges of the WireEdgeSet must have a representation
  //! on surface of face F.
  Standard_EXPORT TopOpeBRepBuild_WireEdgeSet(const TopoDS_Shape&    F,
                                              const Standard_Address Addr = NULL);

  //! value of field myFace
  Standard_EXPORT const TopoDS_Face& Face() const;

  Standard_EXPORT virtual void AddShape(const TopoDS_Shape& S) Standard_OVERRIDE;

  Standard_EXPORT virtual void AddStartElement(const TopoDS_Shape& S) Standard_OVERRIDE;

  Standard_EXPORT virtual void AddElement(const TopoDS_Shape& S) Standard_OVERRIDE;

  Standard_EXPORT virtual void InitNeighbours(const TopoDS_Shape& E) Standard_OVERRIDE;

  //! Build the list of neighbour edges of edge myCurrentShape
  //! Initialize iterator of neighbour edges to edge myCurrentShape
  Standard_EXPORT virtual void FindNeighbours() Standard_OVERRIDE;

  Standard_EXPORT virtual const TopTools_ListOfShape& MakeNeighboursList(const TopoDS_Shape& E,
                                                                         const TopoDS_Shape& V)
    Standard_OVERRIDE;

  Standard_EXPORT static void IsUVISO(const TopoDS_Edge& E,
                                      const TopoDS_Face& F,
                                      Standard_Boolean&  uiso,
                                      Standard_Boolean&  viso);

  Standard_EXPORT virtual void DumpSS() Standard_OVERRIDE;

  Standard_EXPORT virtual TCollection_AsciiString SName(
    const TopoDS_Shape&            S,
    const TCollection_AsciiString& sb = "",
    const TCollection_AsciiString& sa = "") const Standard_OVERRIDE;

  Standard_EXPORT virtual TCollection_AsciiString SName(
    const TopTools_ListOfShape&    S,
    const TCollection_AsciiString& sb = "",
    const TCollection_AsciiString& sa = "") const Standard_OVERRIDE;

  Standard_EXPORT virtual TCollection_AsciiString SNameori(
    const TopoDS_Shape&            S,
    const TCollection_AsciiString& sb = "",
    const TCollection_AsciiString& sa = "") const Standard_OVERRIDE;

  Standard_EXPORT virtual TCollection_AsciiString SNameori(
    const TopTools_ListOfShape&    S,
    const TCollection_AsciiString& sb = "",
    const TCollection_AsciiString& sa = "") const Standard_OVERRIDE;

protected:
private:
  //! Indicates whether vertex V is a good connexity vertex between
  //! edges E1 and E2.
  //! i.e, returns True if V is shared by E1 and E2 and if V has different
  //! orientations on E1 and E2.
  //! If V is shared by E1 and E2, returns the orientation of V on E1 and E2
  Standard_EXPORT Standard_Boolean VertexConnectsEdges(const TopoDS_Shape& V,
                                                       const TopoDS_Shape& E1,
                                                       const TopoDS_Shape& E2,
                                                       TopAbs_Orientation& O1,
                                                       TopAbs_Orientation& O2) const;

  Standard_EXPORT Standard_Boolean VertexConnectsEdgesClosing(const TopoDS_Shape& V,
                                                              const TopoDS_Shape& E1,
                                                              const TopoDS_Shape& E2) const;

  Standard_EXPORT Standard_Integer NbClosingShapes(const TopTools_ListOfShape& L) const;

  Standard_EXPORT void LocalD1(const TopoDS_Shape& F,
                               const TopoDS_Shape& E,
                               const TopoDS_Shape& V,
                               gp_Pnt2d&           p2,
                               gp_Vec2d&           v2) const;

  //! indicates if the edge <E> is a closing edge of myFace
  Standard_EXPORT Standard_Boolean IsClosed(const TopoDS_Shape& E) const;

  //! indicates if the edge <E> is a closing edge on U of myFace
  Standard_EXPORT Standard_Boolean IsUClosed(const TopoDS_Shape& E) const;

  //! indicates if the edge <E> is a closing edge on V of myFace
  Standard_EXPORT Standard_Boolean IsVClosed(const TopoDS_Shape& E) const;

  Standard_EXPORT TCollection_AsciiString SNameVEE(const TopoDS_Shape& V,
                                                   const TopoDS_Shape& E1,
                                                   const TopoDS_Shape& E2) const;

  Standard_EXPORT TCollection_AsciiString SNameVEL(const TopoDS_Shape&         V,
                                                   const TopoDS_Shape&         E,
                                                   const TopTools_ListOfShape& L) const;

  TopoDS_Face myFace;
};

#endif // _TopOpeBRepBuild_WireEdgeSet_HeaderFile
