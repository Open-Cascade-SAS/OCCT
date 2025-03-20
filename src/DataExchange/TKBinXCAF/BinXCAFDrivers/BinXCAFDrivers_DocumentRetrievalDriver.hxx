// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV <eugeny.napalkov@opencascade.com>
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _BinXCAFDrivers_DocumentRetrievalDriver_HeaderFile
#define _BinXCAFDrivers_DocumentRetrievalDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinDrivers_DocumentRetrievalDriver.hxx>
class BinMDF_ADriverTable;
class Message_Messenger;

class BinXCAFDrivers_DocumentRetrievalDriver;
DEFINE_STANDARD_HANDLE(BinXCAFDrivers_DocumentRetrievalDriver, BinDrivers_DocumentRetrievalDriver)

class BinXCAFDrivers_DocumentRetrievalDriver : public BinDrivers_DocumentRetrievalDriver
{

public:
  //! Constructor
  Standard_EXPORT BinXCAFDrivers_DocumentRetrievalDriver();

  Standard_EXPORT virtual Handle(BinMDF_ADriverTable) AttributeDrivers(
    const Handle(Message_Messenger)& theMsgDriver) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinXCAFDrivers_DocumentRetrievalDriver,
                          BinDrivers_DocumentRetrievalDriver)

protected:
private:
};

#endif // _BinXCAFDrivers_DocumentRetrievalDriver_HeaderFile
