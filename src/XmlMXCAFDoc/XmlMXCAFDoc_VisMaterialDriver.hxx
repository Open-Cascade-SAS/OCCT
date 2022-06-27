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

#ifndef _XmlMXCAFDoc_VisMaterialDriver_HeaderFile
#define _XmlMXCAFDoc_VisMaterialDriver_HeaderFile

#include <Standard.hxx>

#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>

DEFINE_STANDARD_HANDLE(XmlMXCAFDoc_VisMaterialDriver, XmlMDF_ADriver)

//! Attribute Driver.
class XmlMXCAFDoc_VisMaterialDriver : public XmlMDF_ADriver
{
  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_VisMaterialDriver, XmlMDF_ADriver)
public:

  //! Main constructor.
  Standard_EXPORT XmlMXCAFDoc_VisMaterialDriver (const Handle(Message_Messenger)& theMessageDriver);

  //! Create new instance of XCAFDoc_VisMaterial.
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  //! Paste attribute from persistence into document.
  Standard_EXPORT Standard_Boolean Paste (const XmlObjMgt_Persistent&  theSource,
                                          const Handle(TDF_Attribute)& theTarget,
                                          XmlObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  //! Paste attribute from document into persistence.
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& theSource,
                              XmlObjMgt_Persistent& theTarget,
                              XmlObjMgt_SRelocationTable& theRelocTable) const Standard_OVERRIDE;

};

#endif // _XmlMXCAFDoc_VisMaterialDriver_HeaderFile
