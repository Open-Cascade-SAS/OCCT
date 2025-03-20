// Created on: 2018-10-02
// Created by: Sergey ZARITCHNY
// Copyright (c) 2018 OPEN CASCADE SAS
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

#include <XmlObjMgt_RRelocationTable.hxx>

//=======================================================================
// function : GetHeaderData
// purpose  : getter for the file header data
//=======================================================================

const Handle(Storage_HeaderData)& XmlObjMgt_RRelocationTable::GetHeaderData() const
{
  return myHeaderData;
}

//=======================================================================
// function : SetHeaderData
// purpose  : setter for the file header data
//=======================================================================

void XmlObjMgt_RRelocationTable::SetHeaderData(const Handle(Storage_HeaderData)& theHeaderData)
{
  myHeaderData = theHeaderData;
}

//=======================================================================
// function : Clear
// purpose  : The relocation table is cleared before/after reading in a document.
//         : In this case the reference to the file header data should also be
//         : cleared, because it is specific to the document.
//=======================================================================
void XmlObjMgt_RRelocationTable::Clear(const Standard_Boolean doReleaseMemory)
{
  myHeaderData.Nullify();
  TColStd_DataMapOfIntegerTransient::Clear(doReleaseMemory);
}