// Created on: 1995-10-19
// Created by: Bruno DUMORTIER
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

#ifndef _BRepOffset_Offset_HeaderFile
#define _BRepOffset_Offset_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepOffset_Status.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <GeomAbs_JoinType.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Edge;
class TopoDS_Vertex;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! This class compute elemenary offset surface.
//! Evaluate the offset generated :
//! 1 - from a face.
//! 2 - from an edge.
//! 3 - from a vertex.
class BRepOffset_Offset 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_Offset();
  
  Standard_EXPORT BRepOffset_Offset(const TopoDS_Face& Face, const Standard_Real Offset, const Standard_Boolean OffsetOutside = Standard_True, const GeomAbs_JoinType JoinType = GeomAbs_Arc);
  
  //! This method will be  called when you want to share
  //! the  edges  soon generated  from  an other  face.
  //! e.g. when two faces are  tangents the common  edge
  //! will generate only one edge ( no pipe).
  //!
  //! The Map  will be fill  as  follow:
  //!
  //! Created(E) = E'
  //! with: E  = an edge of <Face>
  //! E' = the image of E in the offsetting  of
  //! another  face  sharing E  with a
  //! continuity at least G1
  Standard_EXPORT BRepOffset_Offset(const TopoDS_Face& Face, const Standard_Real Offset, const TopTools_DataMapOfShapeShape& Created, const Standard_Boolean OffsetOutside = Standard_True, const GeomAbs_JoinType JoinType = GeomAbs_Arc);
  
  Standard_EXPORT BRepOffset_Offset(const TopoDS_Edge& Path, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Real Offset, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  Standard_EXPORT BRepOffset_Offset(const TopoDS_Edge& Path, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Real Offset, const TopoDS_Edge& FirstEdge, const TopoDS_Edge& LastEdge, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  //! Tol and Conti are only used if Polynomial is True
  //! (Used to perform the approximation)
  Standard_EXPORT BRepOffset_Offset(const TopoDS_Vertex& Vertex, const TopTools_ListOfShape& LEdge, const Standard_Real Offset, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  Standard_EXPORT void Init (const TopoDS_Face& Face, const Standard_Real Offset, const Standard_Boolean OffsetOutside = Standard_True, const GeomAbs_JoinType JoinType = GeomAbs_Arc);
  
  Standard_EXPORT void Init (const TopoDS_Face& Face, const Standard_Real Offset, const TopTools_DataMapOfShapeShape& Created, const Standard_Boolean OffsetOutside = Standard_True, const GeomAbs_JoinType JoinType = GeomAbs_Arc);
  
  Standard_EXPORT void Init (const TopoDS_Edge& Path, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Real Offset, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  Standard_EXPORT void Init (const TopoDS_Edge& Path, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Real Offset, const TopoDS_Edge& FirstEdge, const TopoDS_Edge& LastEdge, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  //! Tol and Conti are only used if Polynomial is True
  //! (Used to perform the approximation)
  Standard_EXPORT void Init (const TopoDS_Vertex& Vertex, const TopTools_ListOfShape& LEdge, const Standard_Real Offset, const Standard_Boolean Polynomial = Standard_False, const Standard_Real Tol = 1.0e-4, const GeomAbs_Shape Conti = GeomAbs_C1);
  
  //! Only used in Rolling Ball. Pipe on Free Boundary
  Standard_EXPORT void Init (const TopoDS_Edge& Edge, const Standard_Real Offset);
  
    const TopoDS_Shape& InitialShape() const;
  
  Standard_EXPORT const TopoDS_Face& Face() const;
  
  Standard_EXPORT TopoDS_Shape Generated (const TopoDS_Shape& Shape) const;
  
  Standard_EXPORT BRepOffset_Status Status() const;




protected:





private:



  TopoDS_Shape myShape;
  BRepOffset_Status myStatus;
  TopoDS_Face myFace;
  TopTools_DataMapOfShapeShape myMap;


};


#include <BRepOffset_Offset.lxx>





#endif // _BRepOffset_Offset_HeaderFile
