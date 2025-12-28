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

#ifndef _XmlMXCAFDoc_VisMaterialToolDriver_HeaderFile
#define _XmlMXCAFDoc_VisMaterialToolDriver_HeaderFile

#include <XmlMDF_ADriver.hxx>

//! XML persistence driver for XCAFDoc_VisMaterialTool.
class XmlMXCAFDoc_VisMaterialToolDriver : public XmlMDF_ADriver
{
  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_VisMaterialToolDriver, XmlMDF_ADriver)
public:
  //! Main constructor.
  Standard_EXPORT XmlMXCAFDoc_VisMaterialToolDriver(const occ::handle<Message_Messenger>& theMsgDriver);

  //! Create new instance of XCAFDoc_VisMaterialTool.
  Standard_EXPORT virtual occ::handle<TDF_Attribute> NewEmpty() const override;

  //! Paste attribute from persistence into document.
  Standard_EXPORT virtual bool Paste(const XmlObjMgt_Persistent&  theSource,
                                                 const occ::handle<TDF_Attribute>& theTarget,
                                                 XmlObjMgt_RRelocationTable&  theRelocTable) const
    override;

  //! Paste attribute from document into persistence.
  Standard_EXPORT virtual void Paste(const occ::handle<TDF_Attribute>& theSource,
                                     XmlObjMgt_Persistent&        theTarget,
                                     XmlObjMgt_SRelocationTable&  theRelocTable) const
    override;
};

#endif // _XmlMXCAFDoc_VisMaterialToolDriver_HeaderFile
