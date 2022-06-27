// Created on: 1993-01-14
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

#ifndef _TopTools_HeaderFile
#define _TopTools_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_OStream.hxx>
class TopoDS_Shape;


//! The  TopTools package provides   utilities for the
//! topological data structure.
//!
//! * ShapeMapHasher. Hash a  Shape base on the TShape
//! and the Location. The Orientation is not used.
//!
//! * OrientedShapeMapHasher. Hash a Shape base on the
//! TShape ,the Location and the Orientation.
//!
//! * Instantiations of TCollection for Shapes :
//! MapOfShape
//! IndexedMapOfShape
//! DataMapOfIntegerShape
//! DataMapOfShapeInteger
//! DataMapOfShapeReal
//! Array1OfShape
//! HArray1OfShape
//! SequenceOfShape
//! HSequenceOfShape
//! ListOfShape
//! Array1OfListShape
//! HArray1OfListShape
//! DataMapOfIntegerListOfShape
//! DataMapOfShapeListOfShape
//! DataMapOfShapeListOfInteger
//! IndexedDataMapOfShapeShape
//! IndexedDataMapOfShapeListOfShape
//! DataMapOfShapeShape
//! IndexedMapOfOrientedShape
//! DataMapOfShapeSequenceOfShape
//! IndexedDataMapOfShapeAddress
//! DataMapOfOrientedShapeShape
//!
//! * LocationSet : to write sets of locations.
//!
//! * ShapeSet : to writes sets of TShapes.
//!
//! Package Methods :
//!
//! Dump : To dump the topology of a Shape.
class TopTools 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! A set of Shapes. Can be dump, wrote or read.
  //! Dumps the topological structure  of <Sh>  on the
  //! stream <S>.
  Standard_EXPORT static void Dump (const TopoDS_Shape& Sh, Standard_OStream& S);
  
  //! This is to bypass an extraction bug. It will force
  //! the  inclusion    of  Standard_Integer.hxx  itself
  //! including Standard_OStream.hxx  at   the   correct
  //! position.
  Standard_EXPORT static void Dummy (const Standard_Integer I);

};

#endif // _TopTools_HeaderFile
