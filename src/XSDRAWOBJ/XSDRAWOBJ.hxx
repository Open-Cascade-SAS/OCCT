// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _XSDRAWOBJ_HeaderFile
#define _XSDRAWOBJ_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Draw_Interpretor.hxx>

class XSDRAWOBJ
{
  DEFINE_STANDARD_ALLOC
public:
  //! Loads all Draw commands of XSDRAWOBJ. Used for plugin.
  Standard_EXPORT static void Factory(Draw_Interpretor& theDI);
};

#endif // _XSDRAWOBJ_HeaderFile
