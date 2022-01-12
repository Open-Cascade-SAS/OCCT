// Created on: 1996-01-09
// Created by: Denis PASCAL
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _DrawDim_HeaderFile
#define _DrawDim_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Shape;
class gp_Pnt;
class TopoDS_Edge;
class gp_Lin;
class gp_Circ;
class TopoDS_Face;
class gp_Pln;


//! This package provides Drawable Dimensions.
//!
//! The classes PlanarDimension and subclasses provide
//! services  to  build  drawable dimensions between
//! point line and circle in a given 3d plane.
//!
//! The   classes  Dimension and   subclasses provide
//! services  to build  drawable  dimensions between
//! plane and cylindrical surfaces.
class DrawDim 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Commands
  //! ========
  Standard_EXPORT static void DrawShapeName (const TopoDS_Shape& ashape, const Standard_CString aname);
  
  Standard_EXPORT static void AllCommands (Draw_Interpretor& I);
  
  //! tools
  //! =====
  Standard_EXPORT static void PlanarDimensionCommands (Draw_Interpretor& I);
  
  Standard_EXPORT static gp_Pnt Nearest (const TopoDS_Shape& aShape, const gp_Pnt& apoint);
  
  //! false if <e> is not a linear edge
  Standard_EXPORT static Standard_Boolean Lin (const TopoDS_Edge& e, gp_Lin& l, Standard_Boolean& infinite, Standard_Real& first, Standard_Real& last);
  
  //! false if <e> is not a circular edge
  Standard_EXPORT static Standard_Boolean Circ (const TopoDS_Edge& e, gp_Circ& l, Standard_Real& first, Standard_Real& last);
  
  //! false if <f> is not a planar face
  Standard_EXPORT static Standard_Boolean Pln (const TopoDS_Face& f, gp_Pln& p);

};

#endif // _DrawDim_HeaderFile
