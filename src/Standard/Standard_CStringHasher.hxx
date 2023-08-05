// Copyright (c) 2023 OPEN CASCADE SAS
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

#ifndef _Standard_CStringHasher_HeaderFile
#define _Standard_CStringHasher_HeaderFile

#include <Standard_CString.hxx>
#include <Standard_HashUtils.hxx>
#include <Standard_TypeDef.hxx>

#include <functional>

class Standard_CStringHasher
{
public:

  size_t operator()(const Standard_CString& theString) const noexcept
  {
    const int aLen = static_cast<int>(strlen(theString));
    if (aLen < 4)
    {
      return opencascade::FNVHash::hash_combine(*theString, aLen);
    }
    return opencascade::hashBytes(theString, aLen);
  }

  bool operator()(const Standard_CString& theString1,
                  const Standard_CString& theString2) const noexcept
  {
    return strcmp(theString1, theString2) == 0;
  }

};

#endif
