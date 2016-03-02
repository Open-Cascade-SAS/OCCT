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


#ifndef _StdLObject_GUID_HeaderFile
#define _StdLObject_GUID_HeaderFile

#include <StdObjMgt_Attribute.hxx>

#include <Standard_GUID.hxx>


class StdLObject_GUID : public Standard_GUID, private StdObjMgt_ContentTypes
{
public:
  void Read (StdObjMgt_ReadData& theReadData);
};

#endif
