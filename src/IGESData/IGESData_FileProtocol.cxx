// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <IGESData_FileProtocol.hxx>
#include <IGESData_Protocol.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESData_FileProtocol,IGESData_Protocol)

IGESData_FileProtocol::IGESData_FileProtocol ()    {  }

    void  IGESData_FileProtocol::Add (const Handle(IGESData_Protocol)& protocol)
{
  if      (theresource.IsNull()) theresource = protocol;
  else if (theresource->IsInstance(protocol->DynamicType())) return; // passer
  else if (!thenext.IsNull()) thenext->Add(protocol);
  else {
    thenext = new IGESData_FileProtocol;
    thenext->Add(protocol);
  }
}

    Standard_Integer  IGESData_FileProtocol::NbResources () const
{
  Standard_Integer nb = (theresource.IsNull() ? 0 : 1);
  if (!thenext.IsNull()) nb += thenext->NbResources();
  return nb;
}

    Handle(Interface_Protocol) IGESData_FileProtocol::Resource
  (const Standard_Integer num) const
{
  Handle(IGESData_Protocol) res;
  if (num == 1) return Handle(Interface_Protocol) (theresource);
  else if (!thenext.IsNull()) return thenext->Resource(num-1);
  return res;  // Null
}
