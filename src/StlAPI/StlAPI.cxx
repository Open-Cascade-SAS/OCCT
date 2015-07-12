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

StlAPI_ErrorStatus StlAPI::Write(const TopoDS_Shape& aShape,
		   const Standard_CString aFile,
		   const Standard_Boolean aAsciiMode) 
{
  StlAPI_Writer writer;
  writer.ASCIIMode() = aAsciiMode;
  return writer.Write (aShape, aFile);
}


void StlAPI::Read(TopoDS_Shape& aShape,const Standard_CString aFile)
{
  StlAPI_Reader reader;
  reader.Read (aShape, aFile);
}
