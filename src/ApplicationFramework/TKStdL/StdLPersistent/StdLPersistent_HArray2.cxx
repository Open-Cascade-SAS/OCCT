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

#include <StdLPersistent_HArray2.hxx>

//=======================================================================
// function : Read
// purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_HArray2::base::Read(StdObjMgt_ReadData& theReadData)
{
  int aLowerRow, aLowerCol, anUpperRow, anUpperCol;
  theReadData >> aLowerRow >> aLowerCol >> anUpperRow >> anUpperCol;
  createArray(aLowerRow, aLowerCol, anUpperRow, anUpperCol);

  StdObjMgt_ReadData::ObjectSentry aSentry(theReadData);

  int aSize;
  theReadData >> aSize;

  for (int aRow = aLowerRow; aRow <= anUpperRow; aRow++)
    for (int aCol = aLowerCol; aCol <= anUpperCol; aCol++)
      readValue(theReadData, aRow, aCol);
}

//=======================================================================
// function : Read
// purpose  : Read persistent data from a file
//=======================================================================
void StdLPersistent_HArray2::base::Write(StdObjMgt_WriteData& theWriteData) const
{
  int aLowerRow, aLowerCol, anUpperRow, anUpperCol;
  lowerBound(aLowerRow, aLowerCol);
  upperBound(anUpperRow, anUpperCol);
  theWriteData << aLowerRow << aLowerCol << anUpperRow << anUpperCol;

  StdObjMgt_WriteData::ObjectSentry aSentry(theWriteData);

  int aSize = (anUpperRow - aLowerRow + 1) * (anUpperCol - aLowerCol + 1);
  theWriteData << aSize;

  for (int aRow = aLowerRow; aRow <= anUpperRow; aRow++)
    for (int aCol = aLowerCol; aCol <= anUpperCol; aCol++)
      writeValue(theWriteData, aRow, aCol);
}
