// Created on: 1995-01-04
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

#ifndef _BRepLib_MakeShell_HeaderFile
#define _BRepLib_MakeShell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepLib_ShellError.hxx>
#include <BRepLib_MakeShape.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class Geom_Surface;
class TopoDS_Shell;


//! Provides methods to build shells.
//!
//! Build a shell from a set of faces.
//! Build untied shell from a non C2 surface
//! splitting it into C2-continuous parts.
class BRepLib_MakeShell  : public BRepLib_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Not done.
  Standard_EXPORT BRepLib_MakeShell();
  
  Standard_EXPORT BRepLib_MakeShell(const Handle(Geom_Surface)& S, const Standard_Boolean Segment = Standard_False);
  
  Standard_EXPORT BRepLib_MakeShell(const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Boolean Segment = Standard_False);
  
  //! Creates the shell from the surface  and the min-max
  //! values.
  Standard_EXPORT void Init (const Handle(Geom_Surface)& S, const Standard_Real UMin, const Standard_Real UMax, const Standard_Real VMin, const Standard_Real VMax, const Standard_Boolean Segment = Standard_False);
  
  Standard_EXPORT BRepLib_ShellError Error() const;
  
  //! Returns the new Shell.
  Standard_EXPORT const TopoDS_Shell& Shell() const;
Standard_EXPORT operator TopoDS_Shell() const;




protected:





private:



  BRepLib_ShellError myError;


};







#endif // _BRepLib_MakeShell_HeaderFile
