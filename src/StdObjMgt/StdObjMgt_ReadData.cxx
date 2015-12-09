// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdObjMgt_ReadData.hxx>


StdObjMgt_ReadData::StdObjMgt_ReadData (Storage_BaseDriver&    theDriver,
                                        const Standard_Integer theNumberOfObjects)
  : myDriver (&theDriver)
  , myPersistentObjects (1, theNumberOfObjects) {}

void StdObjMgt_ReadData::ReadReference (Handle(StdObjMgt_Persistent)& theTarget)
{
  Standard_Integer aRef;
  myDriver->GetReference (aRef);

  if (aRef)
    theTarget = Object (aRef);
  else
    theTarget.Nullify();
}
