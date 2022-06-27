// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMXCAFDoc_ColorDriver_HeaderFile
#define _XmlMXCAFDoc_ColorDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;


class XmlMXCAFDoc_ColorDriver;
DEFINE_STANDARD_HANDLE(XmlMXCAFDoc_ColorDriver, XmlMDF_ADriver)

//! Attribute Driver.
class XmlMXCAFDoc_ColorDriver : public XmlMDF_ADriver
{

public:

  
  Standard_EXPORT XmlMXCAFDoc_ColorDriver(const Handle(Message_Messenger)& theMessageDriver);
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean Paste (const XmlObjMgt_Persistent& Source, const Handle(TDF_Attribute)& Target, XmlObjMgt_RRelocationTable& RelocTable) const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Source, XmlObjMgt_Persistent& Target, XmlObjMgt_SRelocationTable& RelocTable) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_ColorDriver,XmlMDF_ADriver)

protected:




private:




};







#endif // _XmlMXCAFDoc_ColorDriver_HeaderFile
