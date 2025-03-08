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
#include <BinObjMgt_SRelocationTable.hxx>

DEFINE_STANDARD_HANDLE(BinMXCAFDoc_VisMaterialDriver, BinMDF_ADriver)

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
  Standard_EXPORT BinMXCAFDoc_VisMaterialDriver(const Handle(Message_Messenger)& theMsgDriver);

  //! Create new instance of XCAFDoc_VisMaterial.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Paste attribute from persistence into document.
  Standard_EXPORT virtual Standard_Boolean Paste(const BinObjMgt_Persistent&  theSource,
                                                 const Handle(TDF_Attribute)& theTarget,
                                                 BinObjMgt_RRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;

  //! Paste attribute from document into persistence.
  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& theSource,
                                     BinObjMgt_Persistent&        theTarget,
                                     BinObjMgt_SRelocationTable&  theRelocTable) const
    Standard_OVERRIDE;
};

#endif // _BinMXCAFDoc_VisMaterialDriver_HeaderFile
