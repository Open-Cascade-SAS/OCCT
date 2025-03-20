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

#ifndef _StdObjMgt_TransientPersistentMap_HeaderFile
#define _StdObjMgt_TransientPersistentMap_HeaderFile

#include <NCollection_DataMap.hxx>

class Standard_Transient;
class StdObjMgt_Persistent;

typedef NCollection_DataMap<Handle(Standard_Transient), Handle(StdObjMgt_Persistent)>
  StdObjMgt_TransientPersistentMap;

#endif // _StdObjMgt_TransientPersistentMap_HeaderFile
