// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _OpenGlTest_HeaderFile
#define _OpenGlTest_HeaderFile

#include <Draw_Interpretor.hxx>

//! This package defines a set of Draw commands for testing of TKOpenGl library.
class OpenGlTest
{
public:

  DEFINE_STANDARD_ALLOC

  //! Adds Draw commands to the draw interpretor.
  Standard_EXPORT static void Commands (Draw_Interpretor& theDI);

  //! Plugin entry point function.
  Standard_EXPORT static void Factory (Draw_Interpretor& theDI);
};

#endif // _OpenGlTest_HeaderFile
