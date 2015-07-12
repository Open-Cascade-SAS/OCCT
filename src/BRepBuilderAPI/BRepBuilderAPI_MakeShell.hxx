// Created on: 1994-02-16
// Created by: Remi LEQUETTE
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

#ifndef _BRepBuilderAPI_MakeShell_HeaderFile
#define _BRepBuilderAPI_MakeShell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_MakeShell.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <BRepBuilderAPI_ShellError.hxx>
class StdFail_NotDone;
class Geom_Surface;
class TopoDS_Shell;


//! Describes functions to build a
//! shape corresponding to the skin of a surface.
//! Note that the term shell in the class name has the same definition
//! as that of a shell in STEP, in other words the skin of a shape,
//! and not a solid model defined by surface and thickness. If you want
//! to build the second sort of shell, you must use
//! BRepOffsetAPI_MakeOffsetShape. A shell is made of a series of
//! faces connected by their common edges.
//! If the underlying surface of a face is not C2 continuous and
//! the flag Segment is True, MakeShell breaks the surface down into
//! several faces which are all C2 continuous and which are
//! connected along the non-regular curves on the surface.
//! The resulting shell contains all these faces.
//! Construction of a Shell from a non-C2 continuous Surface
//! A MakeShell object provides a framework for:
//! -      defining the construction of a shell,
//! -      implementing the construction algorithm, and
//! -      consulting the result.
//! Warning
//! The connected C2 faces in the shell resulting from a decomposition of
//! the surface are not sewn. For a sewn result, you need to use
//! BRepOffsetAPI_Sewing. For a shell with thickness, you need to use
//! BRepOffsetAPI_MakeOffsetShape.
class BRepBuilderAPI_MakeShell  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs an empty shell framework. The Init
  //! function is used to define the construction arguments.
  //! Warning
  //! The function Error will return
  //! BRepBuilderAPI_EmptyShell if it is called before the function Init.
  Standard_EXPORT BRepBuilderAPI_MakeShell();
  
  //! Constructs a shell from the surface S.
  Standard_EXPORT BRepBuilderAPI_MakeShell(const Handle(Geom_Surface)& S, const Standard_Boolean Segment = Standard_False);
  
  //! Constructs a shell from the surface S,
  //! limited in the u parametric direction by the two
  //! parameter values UMin and UMax, and limited in the v
  //! parametric direction by the two parameter values VMin and VMax.
  Standard_EXPORT BRepBuilderAPI_MakeShell(const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Boolean Segment = Standard_False);
  
  //! Defines or redefines the arguments
  //! for the construction of a shell. The construction is initialized
  //! with the surface S, limited in the u parametric direction by the
  //! two parameter values UMin and UMax, and in the v parametric
  //! direction by the two parameter values VMin and VMax.
  //! Warning
  //! The function Error returns:
  //! -      BRepBuilderAPI_ShellParametersOutOfRange
  //! when the given parameters are outside the bounds of the
  //! surface or the basis surface if S is trimmed
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Boolean Segment = Standard_False);
  
  //! Returns true if the shell is built.
  Standard_EXPORT virtual Standard_Boolean IsDone() const Standard_OVERRIDE;
  
  //! Returns the construction status:
  //! -   BRepBuilderAPI_ShellDone if the shell is built, or
  //! -   another value of the BRepBuilderAPI_ShellError
  //! enumeration indicating why the construction failed.
  //! This is frequently BRepBuilderAPI_ShellParametersOutOfRange
  //! indicating that the given parameters are outside the bounds of the surface.
  Standard_EXPORT BRepBuilderAPI_ShellError Error() const;
  
  //! Returns the new Shell.
  Standard_EXPORT const TopoDS_Shell& Shell() const;
Standard_EXPORT operator TopoDS_Shell() const;




protected:





private:



  BRepLib_MakeShell myMakeShell;


};







#endif // _BRepBuilderAPI_MakeShell_HeaderFile
