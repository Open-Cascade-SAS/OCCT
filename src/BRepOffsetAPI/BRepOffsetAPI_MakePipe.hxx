// Created on: 1994-07-12
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepOffsetAPI_MakePipe_HeaderFile
#define _BRepOffsetAPI_MakePipe_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepFill_Pipe.hxx>
#include <BRepPrimAPI_MakeSweep.hxx>
#include <GeomFill_Trihedron.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TopoDS_Wire;
class TopoDS_Shape;
class BRepFill_Pipe;


//! Describes functions to build pipes.
//! A pipe is built a basis shape (called the profile) along
//! a wire (called the spine) by sweeping.
//! The profile must not contain solids.
//! A MakePipe object provides a framework for:
//! - defining the construction of a pipe,
//! - implementing the construction algorithm, and
//! - consulting the result.
//! Warning
//! The MakePipe class implements pipe constructions
//! with G1 continuous spines only.
class BRepOffsetAPI_MakePipe  : public BRepPrimAPI_MakeSweep
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a pipe by sweeping the shape Profile along
  //! the wire Spine.The angle made by the spine with the profile is
  //! maintained along the length of the pipe.
  //! Warning
  //! Spine must be G1 continuous; that is, on the connection
  //! vertex of two edges of the wire, the tangent vectors on
  //! the left and on the right must have the same direction,
  //! though not necessarily the same magnitude.
  //! Exceptions
  //! Standard_DomainError if the profile is a solid or a
  //! composite solid.
  Standard_EXPORT BRepOffsetAPI_MakePipe(const TopoDS_Wire& Spine, const TopoDS_Shape& Profile);
  
  //! the same as previous but with setting of
  //! mode of sweep and the flag that indicates attempt
  //! to approximate a C1-continuous surface if a swept
  //! surface proved to be C0.
  Standard_EXPORT BRepOffsetAPI_MakePipe(const TopoDS_Wire& Spine, const TopoDS_Shape& Profile, const GeomFill_Trihedron aMode, const Standard_Boolean ForceApproxC1 = Standard_False);
  
  Standard_EXPORT const BRepFill_Pipe& Pipe() const;
  
  //! Builds the resulting shape (redefined from MakeShape).
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns the  TopoDS  Shape of the bottom of the prism.
  Standard_EXPORT TopoDS_Shape FirstShape() Standard_OVERRIDE;
  
  //! Returns the TopoDS Shape of the top of the prism.
  Standard_EXPORT TopoDS_Shape LastShape() Standard_OVERRIDE;
  
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  Standard_EXPORT TopoDS_Shape Generated(const TopoDS_Shape& SSpine, const TopoDS_Shape& SProfile);
  
  Standard_EXPORT Standard_Real ErrorOnSurface() const;




protected:





private:



  BRepFill_Pipe myPipe;


};







#endif // _BRepOffsetAPI_MakePipe_HeaderFile
