// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_VisMaterialDriver_HeaderFile
#define _BinMXCAFDoc_VisMaterialDriver_HeaderFile

#include <BinMDF_ADriver.hxx>
#include <Standard_Boolean.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>

//! Binary persistence driver for XCAFDoc_VisMaterial attribute.
class BinMXCAFDoc_VisMaterialDriver : public BinMDF_ADriver
{
  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_VisMaterialDriver, BinMDF_ADriver)

  //! Persistence version (major for breaking changes, minor for adding new fields at end).
  enum
  {
    MaterialVersionMajor_1 = 1,
    MaterialVersionMinor_0 = 0,
    MaterialVersionMinor_1 = 1, //!< added IOR

    MaterialVersionMajor = MaterialVersionMajor_1,
    MaterialVersionMinor = MaterialVersionMinor_1
  };

public:
  //! Main constructor.
  Standard_EXPORT BinMXCAFDoc_VisMaterialDriver(const occ::handle<Message_Messenger>& theMsgDriver);

  //! Create new instance of XCAFDoc_VisMaterial.
  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  //! Paste attribute from persistence into document.
  Standard_EXPORT bool Paste(const BinObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     BinObjMgt_RRelocationTable& theRelocTable) const override;

  //! Paste attribute from document into persistence.
  Standard_EXPORT void Paste(
    const occ::handle<TDF_Attribute>&                        theSource,
    BinObjMgt_Persistent&                                    theTarget,
    NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const override;
};

#endif // _BinMXCAFDoc_VisMaterialDriver_HeaderFile
