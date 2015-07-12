// Created on: 1993-06-22
// Created by: Laurent BOURESCHE
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

#ifndef _BRepSweep_Prism_HeaderFile
#define _BRepSweep_Prism_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepSweep_Translation.hxx>
#include <Standard_Boolean.hxx>
class Standard_ConstructionError;
class TopoDS_Shape;
class gp_Vec;
class gp_Dir;
class Sweep_NumShape;
class TopLoc_Location;


//! Provides natural constructors to build BRepSweep
//! translated swept Primitives.
class BRepSweep_Prism 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Builds the prism of base S and vector V. If C is true,
  //! S is copied. If Canonize is true then generated surfaces
  //! are attempted to be canonized in simple types
  Standard_EXPORT BRepSweep_Prism(const TopoDS_Shape& S, const gp_Vec& V, const Standard_Boolean Copy = Standard_False, const Standard_Boolean Canonize = Standard_True);
  
  //! Builds a semi-infinite or an infinite prism of base S.
  //! If Copy is true S is copied.  If Inf is true the prism
  //! is infinite, if Inf is false the  prism is infinite in
  //! the direction D. If Canonize is true then generated surfaces
  //! are attempted to be canonized in simple types
  Standard_EXPORT BRepSweep_Prism(const TopoDS_Shape& S, const gp_Dir& D, const Standard_Boolean Inf = Standard_True, const Standard_Boolean Copy = Standard_False, const Standard_Boolean Canonize = Standard_True);
  
  //! Returns the TopoDS Shape attached to the prism.
  Standard_EXPORT TopoDS_Shape Shape();
  
  //! Returns    the  TopoDS  Shape   generated  with  aGenS
  //! (subShape  of the generating shape).
  Standard_EXPORT TopoDS_Shape Shape (const TopoDS_Shape& aGenS);
  
  //! Returns the  TopoDS  Shape of the bottom of the prism.
  Standard_EXPORT TopoDS_Shape FirstShape();
  
  //! Returns the TopoDS Shape of the bottom  of the  prism.
  //! generated  with  aGenS  (subShape  of  the  generating
  //! shape).
  Standard_EXPORT TopoDS_Shape FirstShape (const TopoDS_Shape& aGenS);
  
  //! Returns the TopoDS Shape of the top of the prism.
  Standard_EXPORT TopoDS_Shape LastShape();
  
  //! Returns the  TopoDS  Shape of the top  of  the  prism.
  //! generated  with  aGenS  (subShape  of  the  generating
  //! shape).
  Standard_EXPORT TopoDS_Shape LastShape (const TopoDS_Shape& aGenS);
  
  //! Returns the Vector of the Prism,  if it is an infinite
  //! prism the Vec is unitar.
  Standard_EXPORT gp_Vec Vec() const;




protected:





private:

  
  //! used to build the NumShape of a limited prism.
  Standard_EXPORT Sweep_NumShape NumShape() const;
  
  //! used to build the NumShape of an infinite prism.
  Standard_EXPORT Sweep_NumShape NumShape (const Standard_Boolean Inf) const;
  
  //! used to build the Location.
  Standard_EXPORT TopLoc_Location Location (const gp_Vec& V) const;


  BRepSweep_Translation myTranslation;


};







#endif // _BRepSweep_Prism_HeaderFile
