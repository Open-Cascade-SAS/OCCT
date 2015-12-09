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

#include <StdLPersistent_PDataStd_Integer.hxx>

#include <StdObjMgt_ReadData.hxx>

#include <TDataStd_Integer.hxx>

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_PDataStd_Integer::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myValue;
}

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
Handle(TDF_Attribute) StdLPersistent_PDataStd_Integer::ImportAttribute() const
{
  Handle(TDataStd_Integer) aValue = new TDataStd_Integer;
  aValue->Set (myValue);
  return aValue;
}
