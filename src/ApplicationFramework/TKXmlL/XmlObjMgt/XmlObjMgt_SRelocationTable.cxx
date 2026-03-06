// Created on: 2018-10-04
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

#include <XmlObjMgt_SRelocationTable.hxx>

//=================================================================================================

const occ::handle<Storage_HeaderData>& XmlObjMgt_SRelocationTable::GetHeaderData() const
{
  return myHeaderData;
}

//=================================================================================================

void XmlObjMgt_SRelocationTable::SetHeaderData(const occ::handle<Storage_HeaderData>& theHeaderData)
{
  myHeaderData = theHeaderData;
}

//=================================================================================================

void XmlObjMgt_SRelocationTable::Clear(const bool doReleaseMemory)
{
  myHeaderData.Nullify();
  NCollection_IndexedMap<occ::handle<Standard_Transient>>::Clear(doReleaseMemory);
}