// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_DefSwitch_HeaderFile
#define _IGESData_DefSwitch_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IGESData_DefType.hxx>

//! description of a directory component which can be either
//! undefined (let Void), defined as a Reference to an entity,
//! or as a Rank, integer value addressing a builtin table
//! The entity reference is not included here, only reference
//! status is kept (because entity type must be adapted)
class IGESData_DefSwitch 
{
public:

  DEFINE_STANDARD_ALLOC

  //! creates a DefSwitch as Void
  Standard_EXPORT IGESData_DefSwitch();
  
  //! sets DefSwitch to "Void" status (in file : Integer = 0)
  Standard_EXPORT void SetVoid();
  
  //! sets DefSwitch to "Reference" Status (in file : Integer < 0)
  Standard_EXPORT void SetReference();
  
  //! sets DefSwitch to "Rank" with a Value (in file : Integer > 0)
  Standard_EXPORT void SetRank (const Standard_Integer val);
  
  //! returns DefType status (Void,Reference,Rank)
  Standard_EXPORT IGESData_DefType DefType() const;
  
  //! returns Value as Integer (sensefull for a Rank)
  Standard_EXPORT Standard_Integer Value() const;

private:

  Standard_Integer theval;

};

#endif // _IGESData_DefSwitch_HeaderFile
