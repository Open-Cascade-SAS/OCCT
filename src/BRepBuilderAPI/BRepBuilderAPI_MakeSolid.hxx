// Created on: 1993-07-21
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

#ifndef _BRepBuilderAPI_MakeSolid_HeaderFile
#define _BRepBuilderAPI_MakeSolid_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Boolean.hxx>
class TopoDS_CompSolid;
class TopoDS_Shell;
class TopoDS_Solid;
class TopoDS_Shape;


//! Describes functions to build a solid from shells.
//! A solid is made of one shell, or a series of shells, which
//! do not intersect each other. One of these shells
//! constitutes the outside skin of the solid. It may be closed
//! (a finite solid) or open (an infinite solid). Other shells
//! form hollows (cavities) in these previous ones. Each
//! must bound a closed volume.
//! A MakeSolid object provides a framework for:
//! -   defining and implementing the construction of a solid, and
//! -   consulting the result.
class BRepBuilderAPI_MakeSolid  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initializes the construction of a solid. An empty solid is
  //! considered to cover the whole space. The Add function
  //! is used to define shells to bound it.
  Standard_EXPORT BRepBuilderAPI_MakeSolid();
  
  //! Make a solid from a CompSolid.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_CompSolid& S);
  
  //! Make a solid from a shell.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S);
  
  //! Make a solid from two shells.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2);
  
  //! Make a solid from three shells.
  //! Constructs a solid
  //! -   covering the whole space, or
  //! -   from shell S, or
  //! -   from two shells S1 and S2, or
  //! -   from three shells S1, S2 and S3, or
  //! Warning
  //! No check is done to verify the conditions of coherence
  //! of the resulting solid. In particular, S1, S2 (and S3) must
  //! not intersect each other.
  //! Besides, after all shells have been added using the Add
  //! function, one of these shells should constitute the outside
  //! skin of the solid; it may be closed (a finite solid) or open
  //! (an infinite solid). Other shells form hollows (cavities) in
  //! these previous ones. Each must bound a closed volume.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_Shell& S1, const TopoDS_Shell& S2, const TopoDS_Shell& S3);
  
  //! Make a solid from a solid. useful for adding later.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So);
  
  //! Add a shell to a solid.
  //!
  //! Constructs a solid:
  //! -   from the solid So, to which shells can be added, or
  //! -   by adding the shell S to the solid So.
  //! Warning
  //! No check is done to verify the conditions of coherence
  //! of the resulting solid. In particular S must not intersect the solid S0.
  //! Besides, after all shells have been added using the Add
  //! function, one of these shells should constitute the outside
  //! skin of the solid. It may be closed (a finite solid) or open
  //! (an infinite solid). Other shells form hollows (cavities) in
  //! the previous ones. Each must bound a closed volume.
  Standard_EXPORT BRepBuilderAPI_MakeSolid(const TopoDS_Solid& So, const TopoDS_Shell& S);
  
  //! Adds the shell to the current solid.
  //! Warning
  //! No check is done to verify the conditions of coherence
  //! of the resulting solid. In particular, S must not intersect
  //! other shells of the solid under construction.
  //! Besides, after all shells have been added, one of
  //! these shells should constitute the outside skin of the
  //! solid. It may be closed (a finite solid) or open (an
  //! infinite solid). Other shells form hollows (cavities) in
  //! these previous ones. Each must bound a closed volume.
  Standard_EXPORT void Add (const TopoDS_Shell& S);
  
  //! Returns true if the solid is built.
  //! For this class, a solid under construction is always valid.
  //! If no shell has been added, it could be a whole-space
  //! solid. However, no check was done to verify the
  //! conditions of coherence of the resulting solid.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;
  
  //! Returns the new Solid.
  Standard_EXPORT const TopoDS_Solid& Solid();
  Standard_EXPORT operator TopoDS_Solid();
  
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& S) Standard_OVERRIDE;




protected:





private:



  BRepLib_MakeSolid myMakeSolid;


};







#endif // _BRepBuilderAPI_MakeSolid_HeaderFile
