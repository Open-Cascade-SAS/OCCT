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

#include <StlAPI.hxx>

#include <StlAPI_Reader.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Shape.hxx>

//=============================================================================
//function : Write
//purpose  :
//=============================================================================
Standard_Boolean StlAPI::Write (const TopoDS_Shape&    theShape,
                                const Standard_CString theFile,
                                const Standard_Boolean theAsciiMode)
{
  StlAPI_Writer aWriter;
  aWriter.ASCIIMode() = theAsciiMode;
  return aWriter.Write (theShape, theFile);
}

//=============================================================================
//function : Read
//purpose  :
//=============================================================================
Standard_Boolean StlAPI::Read (TopoDS_Shape&          theShape,
                               const Standard_CString theFile)
{
  StlAPI_Reader aReader;
  return aReader.Read(theShape, theFile);
}
