// Created on: 2001-09-10
// Created by: Sergey KUUL
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

#ifndef _Transfer_MapContainer_HeaderFile
#define _Transfer_MapContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_DataMapOfTransientTransient.hxx>
#include <Standard_Transient.hxx>

class Transfer_MapContainer;
DEFINE_STANDARD_HANDLE(Transfer_MapContainer, Standard_Transient)

class Transfer_MapContainer : public Standard_Transient
{

public:
  Standard_EXPORT Transfer_MapContainer();

  //! Set map already translated geometry objects.
  Standard_EXPORT void SetMapObjects(TColStd_DataMapOfTransientTransient& theMapObjects);

  //! Get map already translated geometry objects.
  Standard_EXPORT TColStd_DataMapOfTransientTransient& GetMapObjects();

  DEFINE_STANDARD_RTTIEXT(Transfer_MapContainer, Standard_Transient)

protected:
private:
  TColStd_DataMapOfTransientTransient myMapObj;
};

#endif // _Transfer_MapContainer_HeaderFile
