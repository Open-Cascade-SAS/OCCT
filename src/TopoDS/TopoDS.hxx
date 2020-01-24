// Created on: 1990-12-11
// Created by: Remi Lequette
// Copyright (c) 1990-1999 Matra Datavision
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

#ifndef _TopoDS_HeaderFile
#define _TopoDS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TopoDS_Vertex;
class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_CompSolid;
class TopoDS_Compound;
class TopoDS_Shape;
class TopoDS_HShape;
class TopoDS_TShape;
class TopoDS_TVertex;
class TopoDS_Vertex;
class TopoDS_TEdge;
class TopoDS_Edge;
class TopoDS_TWire;
class TopoDS_Wire;
class TopoDS_TFace;
class TopoDS_Face;
class TopoDS_TShell;
class TopoDS_Shell;
class TopoDS_TSolid;
class TopoDS_Solid;
class TopoDS_TCompSolid;
class TopoDS_CompSolid;
class TopoDS_TCompound;
class TopoDS_Compound;
class TopoDS_Builder;
class TopoDS_Iterator;


//! Provides methods to cast objects of class
//! TopoDS_Shape to be objects of more specialized
//! sub-classes. Types are verified, thus in the example
//! below, the first two blocks are correct but the third is
//! rejected by the compiler.
class TopoDS 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Basic tool to access the data structure.
  //! Casts shape S to the more specialized return type, Vertex.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Vertex& Vertex (const TopoDS_Shape& S);
inline static TopoDS_Vertex& Vertex(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Edge
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Edge& Edge (const TopoDS_Shape& S);
inline static TopoDS_Edge& Edge(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Wire.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Wire& Wire (const TopoDS_Shape& S);
inline static TopoDS_Wire& Wire(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Face.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Face& Face (const TopoDS_Shape& S);
inline static TopoDS_Face& Face(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Shell.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Shell& Shell (const TopoDS_Shape& S);
inline static TopoDS_Shell& Shell(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Solid.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Solid& Solid (const TopoDS_Shape& S);
inline static TopoDS_Solid& Solid(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, CompSolid.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_CompSolid& CompSolid (const TopoDS_Shape& S);
inline static TopoDS_CompSolid& CompSolid(TopoDS_Shape&);
  
  //! Casts shape S to the more specialized return type, Compound.
  //! Exceptions
  //! Standard_TypeMismatch if S cannot be cast to this return type.
    static const TopoDS_Compound& Compound (const TopoDS_Shape& S);
inline static TopoDS_Compound& Compound(TopoDS_Shape&);




protected:





private:




friend class TopoDS_Shape;
friend class TopoDS_HShape;
friend class TopoDS_TShape;
friend class TopoDS_TVertex;
friend class TopoDS_Vertex;
friend class TopoDS_TEdge;
friend class TopoDS_Edge;
friend class TopoDS_TWire;
friend class TopoDS_Wire;
friend class TopoDS_TFace;
friend class TopoDS_Face;
friend class TopoDS_TShell;
friend class TopoDS_Shell;
friend class TopoDS_TSolid;
friend class TopoDS_Solid;
friend class TopoDS_TCompSolid;
friend class TopoDS_CompSolid;
friend class TopoDS_TCompound;
friend class TopoDS_Compound;
friend class TopoDS_Builder;
friend class TopoDS_Iterator;

};


#include <TopoDS.lxx>





#endif // _TopoDS_HeaderFile
