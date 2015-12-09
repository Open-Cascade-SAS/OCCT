// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObjMgt_Persistent.hxx>

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//!          (to be overriden by attribute classes;
//!          returns a null handle by default for non-attribute classes)
//=======================================================================
Handle(TDF_Attribute) StdObjMgt_Persistent::ImportAttribute() const
{
  return Handle(TDF_Attribute)();
}
