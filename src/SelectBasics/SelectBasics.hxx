// Created on: 1995-01-23
// Created by: Mister rmi
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

#ifndef _SelectBasics_HeaderFile
#define _SelectBasics_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Integer.hxx>

//! interface class for dynamic selection
class SelectBasics 
{
public:

  DEFINE_STANDARD_ALLOC

  //! Structure to provide all-in-one result of selection of sensitive
  //! for "Matches" method of SelectBasics_SensitiveEntity.
  Standard_EXPORT static Standard_Integer MaxOwnerPriority();
  
  Standard_EXPORT static Standard_Integer MinOwnerPriority();

};

#endif // _SelectBasics_HeaderFile
