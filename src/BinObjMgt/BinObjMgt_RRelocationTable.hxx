// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinObjMgt_RRelocationTable_HeaderFile
#define _BinObjMgt_RRelocationTable_HeaderFile

#include <TColStd_DataMapOfIntegerTransient.hxx>
#include <Storage_HeaderData.hxx>

//! Retrieval relocation table is modeled as a child class of
//! TColStd_DataMapOfIntegerTransient that stores a handle to the file
//! header section. With that attribute drivers have access to the file header
//! section.
class BinObjMgt_RRelocationTable : public TColStd_DataMapOfIntegerTransient
{
public:

  //! Returns a handle to the header data of the file that is begin read
  Standard_EXPORT const Handle(Storage_HeaderData)& GetHeaderData() const;

  //! Sets the storage header data.
  //!
  //! @param theHeaderData header data of the file that is begin read
  Standard_EXPORT void SetHeaderData(
      const Handle(Storage_HeaderData)& theHeaderData);

  Standard_EXPORT void Clear(const Standard_Boolean doReleaseMemory = Standard_True);



protected:



private:

  Handle(Storage_HeaderData) myHeaderData;
};

#endif // _BinObjMgt_RRelocationTable_HeaderFile
