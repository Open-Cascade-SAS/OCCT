// Created on: 2001-07-25
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

#ifndef _XmlDrivers_DocumentRetrievalDriver_HeaderFile
#define _XmlDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlLDrivers_DocumentRetrievalDriver.hxx>
#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
class XmlMDF_ADriverTable;
class CDM_MessageDriver;
class XmlMDF_ADriver;


class XmlDrivers_DocumentRetrievalDriver;
DEFINE_STANDARD_HANDLE(XmlDrivers_DocumentRetrievalDriver, XmlLDrivers_DocumentRetrievalDriver)


class XmlDrivers_DocumentRetrievalDriver : public XmlLDrivers_DocumentRetrievalDriver
{

public:

  
  Standard_EXPORT XmlDrivers_DocumentRetrievalDriver();
  
  Standard_EXPORT virtual Handle(XmlMDF_ADriverTable) AttributeDrivers (const Handle(CDM_MessageDriver)& theMsgDriver) Standard_OVERRIDE;
  
  Standard_EXPORT virtual Handle(XmlMDF_ADriver) ReadShapeSection (const XmlObjMgt_Element& thePDoc, const Handle(CDM_MessageDriver)& theMsgDriver) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void ShapeSetCleaning (const Handle(XmlMDF_ADriver)& theDriver) Standard_OVERRIDE;
  
  Standard_EXPORT virtual void PropagateDocumentVersion (const Standard_Integer theDocVersion) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(XmlDrivers_DocumentRetrievalDriver,XmlLDrivers_DocumentRetrievalDriver)

protected:




private:




};







#endif // _XmlDrivers_DocumentRetrievalDriver_HeaderFile
