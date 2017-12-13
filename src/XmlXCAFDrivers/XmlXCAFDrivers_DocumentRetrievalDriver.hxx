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

#ifndef _XmlXCAFDrivers_DocumentRetrievalDriver_HeaderFile
#define _XmlXCAFDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlDrivers_DocumentRetrievalDriver.hxx>
class XmlMDF_ADriverTable;
class Message_Messenger;


class XmlXCAFDrivers_DocumentRetrievalDriver;
DEFINE_STANDARD_HANDLE(XmlXCAFDrivers_DocumentRetrievalDriver, XmlDrivers_DocumentRetrievalDriver)

//! retrieval driver of a XS document
class XmlXCAFDrivers_DocumentRetrievalDriver : public XmlDrivers_DocumentRetrievalDriver
{

public:

  
  Standard_EXPORT XmlXCAFDrivers_DocumentRetrievalDriver();
  
  Standard_EXPORT virtual Handle(XmlMDF_ADriverTable) AttributeDrivers (const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XmlXCAFDrivers_DocumentRetrievalDriver,XmlDrivers_DocumentRetrievalDriver)

protected:




private:




};







#endif // _XmlXCAFDrivers_DocumentRetrievalDriver_HeaderFile
