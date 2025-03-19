// Created on: 2000-08-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_Driver_HeaderFile
#define _XCAFPrs_Driver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TPrsStd_Driver.hxx>
class TDF_Label;
class AIS_InteractiveObject;
class Standard_GUID;

class XCAFPrs_Driver;
DEFINE_STANDARD_HANDLE(XCAFPrs_Driver, TPrsStd_Driver)

//! Implements a driver for presentation of shapes in DECAF
//! document. Its the only purpose is to initialize and return
//! XCAFPrs_AISObject object on request
class XCAFPrs_Driver : public TPrsStd_Driver
{

public:
  Standard_EXPORT virtual Standard_Boolean Update(const TDF_Label&               L,
                                                  Handle(AIS_InteractiveObject)& ais)
    Standard_OVERRIDE;

  //! returns GUID of the driver
  Standard_EXPORT static const Standard_GUID& GetID();

  DEFINE_STANDARD_RTTIEXT(XCAFPrs_Driver, TPrsStd_Driver)

protected:
private:
};

#endif // _XCAFPrs_Driver_HeaderFile
