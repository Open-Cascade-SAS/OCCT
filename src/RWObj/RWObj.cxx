// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#include <RWObj.hxx>

#include <RWObj_TriangulationReader.hxx>

//=============================================================================
//function : Read
//purpose  :
//=============================================================================
Handle(Poly_Triangulation) RWObj::ReadFile (const Standard_CString theFile,
                                            const Message_ProgressRange& theProgress)
{
  RWObj_TriangulationReader aReader;
  aReader.SetCreateShapes (Standard_False);
  aReader.Read (theFile, theProgress);
  // note that returned bool value is ignored intentionally -- even if something went wrong,
  // but some data have been read, we at least will return these data
  return aReader.GetTriangulation();
}
