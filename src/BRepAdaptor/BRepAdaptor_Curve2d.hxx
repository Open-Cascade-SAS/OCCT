// Created on: 1993-07-13
// Created by: Remi LEQUETTE
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

#ifndef _BRepAdaptor_Curve2d_HeaderFile
#define _BRepAdaptor_Curve2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom2dAdaptor_Curve.hxx>
class Standard_NullObject;
class TopoDS_Edge;
class TopoDS_Face;


//! The Curve2d from BRepAdaptor allows to use an Edge
//! on   a Face like   a  2d   curve. (curve  in   the
//! parametric space).
//!
//! It  has  the methods    of the class Curve2d  from
//! Adpator.
//!
//! It  is created or  initialized with a  Face and an
//! Edge.  The methods are  inherited from  Curve from
//! Geom2dAdaptor.
class BRepAdaptor_Curve2d  : public Geom2dAdaptor_Curve
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates an uninitialized curve2d.
  Standard_EXPORT BRepAdaptor_Curve2d();
  
  //! Creates with the pcurve of <E> on <F>.
  Standard_EXPORT BRepAdaptor_Curve2d(const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Initialize with the pcurve of <E> on <F>.
  Standard_EXPORT void Initialize (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  //! Returns the Edge.
  Standard_EXPORT const TopoDS_Edge& Edge() const;
  
  //! Returns the Face.
  Standard_EXPORT const TopoDS_Face& Face() const;




protected:





private:



  TopoDS_Edge myEdge;
  TopoDS_Face myFace;


};







#endif // _BRepAdaptor_Curve2d_HeaderFile
