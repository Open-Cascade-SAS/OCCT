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

#ifndef _StdObjMgt_MapOfInstantiators_HeaderFile
#define _StdObjMgt_MapOfInstantiators_HeaderFile

#include <StdObjMgt_Persistent.hxx>
#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>


class StdObjMgt_MapOfInstantiators
  : public NCollection_DataMap<TCollection_AsciiString,
                               StdObjMgt_Persistent::Instantiator,
                               TCollection_AsciiString>
{
public:
  template <class Persistent>
  void Bind (const TCollection_AsciiString& theTypeName)
  {
    NCollection_DataMap<TCollection_AsciiString,
                        StdObjMgt_Persistent::Instantiator,
                        TCollection_AsciiString>
      ::Bind (theTypeName, Persistent::template Instantiate<Persistent>);
  }

  DEFINE_STANDARD_ALLOC
};

#endif // _StdObjMgt_MapOfInstantiators_HeaderFile
