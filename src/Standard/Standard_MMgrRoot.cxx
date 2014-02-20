// Created on: 2005-03-23
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <Standard_MMgrRoot.hxx>
#include <stdlib.h>

//=======================================================================
//function : ~Standard_MMgrRoot
//purpose  : 
//=======================================================================

Standard_MMgrRoot::~Standard_MMgrRoot()
{
}

//=======================================================================
//function : Purge
//purpose  : 
//=======================================================================

Standard_Integer Standard_MMgrRoot::Purge(Standard_Boolean)
{
  return 0;
}
