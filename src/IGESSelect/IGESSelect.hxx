// Created on: 1994-05-31
// Created by: Christian CAILLET
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

#ifndef _IGESSelect_HeaderFile
#define _IGESSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
class IGESData_IGESEntity;
class Interface_Graph;


//! This package defines the library of the most used tools for
//! IGES Files : Selections & Modifiers specific to the IGES norm,
//! and the most needed converters
class IGESSelect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Simply gives a prompt for a conversational action on standard
  //! input/output. Returns the status of a
  Standard_EXPORT static void Run();
  
  //! Gives a quick analysis of an IGES Entity in the context of a
  //! model (i.e. a File) described by a Graph.
  //! Returned values are :
  //! <sup> : the most meaningful super entity, if any (else Null)
  //! <index> : meaningful index relating to super entity, if any
  //! <returned> : a status which helps exploitation of <sup>, by
  //! giving a case
  //! (normally, types of <ent> and <sup> should suffice to
  //! known the case)
  Standard_EXPORT static Standard_Integer WhatIges (const Handle(IGESData_IGESEntity)& ent, const Interface_Graph& G, Handle(IGESData_IGESEntity)& sup, Standard_Integer& index);

};

#endif // _IGESSelect_HeaderFile
