// Created on: 1995-10-20
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

#ifndef _BRepOffset_Analyse_HeaderFile
#define _BRepOffset_Analyse_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepOffset_DataMapOfShapeListOfInterval.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <Standard_Real.hxx>
#include <BRepOffset_ListOfInterval.hxx>
#include <BRepOffset_Type.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Vertex;
class TopoDS_Face;
class TopoDS_Compound;


//! Analyse of a shape consit to
//! Find the part of edges convex concave tangent.
class BRepOffset_Analyse 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepOffset_Analyse();
  
  Standard_EXPORT BRepOffset_Analyse(const TopoDS_Shape& S, const Standard_Real Angle);
  
  Standard_EXPORT void Perform (const TopoDS_Shape& S, const Standard_Real Angle);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT void Clear();
  
  Standard_EXPORT const BRepOffset_ListOfInterval& Type (const TopoDS_Edge& E) const;
  
  //! Stores in <L> all the edges of Type <T>
  //! on the vertex <V>.
  Standard_EXPORT void Edges (const TopoDS_Vertex& V, const BRepOffset_Type T, TopTools_ListOfShape& L) const;
  
  //! Stores in <L> all the edges of Type <T>
  //! on the face <F>.
  Standard_EXPORT void Edges (const TopoDS_Face& F, const BRepOffset_Type T, TopTools_ListOfShape& L) const;
  
  //! set in <Edges> all  the Edges of <Shape> which are
  //! tangent to <Edge> at the vertex <Vertex>.
  Standard_EXPORT void TangentEdges (const TopoDS_Edge& Edge, const TopoDS_Vertex& Vertex, TopTools_ListOfShape& Edges) const;
  
  Standard_EXPORT Standard_Boolean HasAncestor (const TopoDS_Shape& S) const;
  
  Standard_EXPORT const TopTools_ListOfShape& Ancestors (const TopoDS_Shape& S) const;
  
  //! Explode in compounds of faces where
  //! all the connex edges are of type <Side>
  Standard_EXPORT void Explode (TopTools_ListOfShape& L, const BRepOffset_Type Type) const;
  
  //! Explode in compounds of faces where
  //! all the connex edges are of type <Side1> or <Side2>
  Standard_EXPORT void Explode (TopTools_ListOfShape& L, const BRepOffset_Type Type1, const BRepOffset_Type Type2) const;
  
  //! Add in <CO> the faces of the shell containing <Face>
  //! where all the connex edges are of type <Side>.
  Standard_EXPORT void AddFaces (const TopoDS_Face& Face, TopoDS_Compound& Co, TopTools_MapOfShape& Map, const BRepOffset_Type Type) const;
  
  //! Add in <CO> the faces of the shell containing <Face>
  //! where all the connex edges are of type <Side1> or <Side2>.
  Standard_EXPORT void AddFaces (const TopoDS_Face& Face, TopoDS_Compound& Co, TopTools_MapOfShape& Map, const BRepOffset_Type Type1, const BRepOffset_Type Type2) const;




protected:





private:



  Standard_Boolean myDone;
  TopoDS_Shape myShape;
  BRepOffset_DataMapOfShapeListOfInterval mapEdgeType;
  TopTools_IndexedDataMapOfShapeListOfShape ancestors;
  Standard_Real angle;


};







#endif // _BRepOffset_Analyse_HeaderFile
