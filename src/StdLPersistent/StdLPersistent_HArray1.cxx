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

#include <StdLPersistent_HArray1.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_HArray1::base::Read (StdObjMgt_ReadData& theReadData)
{
  Standard_Integer aLowerBound, anUpperBound;
  theReadData >> aLowerBound >> anUpperBound;
  createArray (aLowerBound, anUpperBound);

  StdObjMgt_ReadData::Object anObjectData (theReadData);

  Standard_Integer aSize;
  anObjectData >> aSize;

  for (Standard_Integer i = aLowerBound; i <= anUpperBound; i++)
    readValue (anObjectData, i);
}

//=======================================================================
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
void StdLPersistent_HArray1::base::Write (StdObjMgt_WriteData& theWriteData) const
{
  Standard_Integer aLowerBound = lowerBound(), anUpperBound = upperBound();
  theWriteData << aLowerBound << anUpperBound;

  StdObjMgt_WriteData::Object anObjectData(theWriteData);

  Standard_Integer aSize = anUpperBound - aLowerBound + 1;
  anObjectData << aSize;

  for (Standard_Integer i = aLowerBound; i <= anUpperBound; i++)
    writeValue(theWriteData, i);
}
