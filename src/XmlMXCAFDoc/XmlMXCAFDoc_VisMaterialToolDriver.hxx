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

DEFINE_STANDARD_HANDLE(XmlMXCAFDoc_VisMaterialToolDriver, XmlMDF_ADriver)

//! XML persistence driver for XCAFDoc_VisMaterialTool.
class XmlMXCAFDoc_VisMaterialToolDriver : public XmlMDF_ADriver
{
  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_VisMaterialToolDriver, XmlMDF_ADriver)
public:

  //! Main constructor.
  Standard_EXPORT XmlMXCAFDoc_VisMaterialToolDriver (const Handle(Message_Messenger)& theMsgDriver);

  //! Create new instance of XCAFDoc_VisMaterialTool.
  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Paste attribute from persistence into document.
  Standard_EXPORT virtual Standard_Boolean Paste (const XmlObjMgt_Persistent&  theSource,
                                                  const Handle(TDF_Attribute)& theTarget,
                                                  XmlObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  //! Paste attribute from document into persistence.
  Standard_EXPORT virtual void Paste (const Handle(TDF_Attribute)& theSource,
                                      XmlObjMgt_Persistent& theTarget,
                                      XmlObjMgt_SRelocationTable& theRelocTable) const Standard_OVERRIDE;

};

#endif // _XmlMXCAFDoc_VisMaterialToolDriver_HeaderFile
