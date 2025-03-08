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

#include <IGESDefs_Protocol.hxx>

#include <IGESDefs_AssociativityDef.hxx>
#include <IGESDefs_AttributeDef.hxx>
#include <IGESDefs_AttributeTable.hxx>
#include <IGESDefs_GenericData.hxx>
#include <IGESDefs_MacroDef.hxx>
#include <IGESDefs_TabularData.hxx>
#include <IGESDefs_UnitsData.hxx>
#include <IGESGraph.hxx>
#include <IGESGraph_Protocol.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDefs_Protocol, IGESData_Protocol)

IGESDefs_Protocol::IGESDefs_Protocol() {}

Standard_Integer IGESDefs_Protocol::NbResources() const
{
  return 1;
}

Handle(Interface_Protocol) IGESDefs_Protocol::Resource(const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESGraph::Protocol();
  return res;
}

Standard_Integer IGESDefs_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESDefs_AssociativityDef))
    return 1;
  else if (atype == STANDARD_TYPE(IGESDefs_AttributeDef))
    return 2;
  else if (atype == STANDARD_TYPE(IGESDefs_AttributeTable))
    return 3;
  else if (atype == STANDARD_TYPE(IGESDefs_GenericData))
    return 4;
  else if (atype == STANDARD_TYPE(IGESDefs_MacroDef))
    return 5;
  else if (atype == STANDARD_TYPE(IGESDefs_TabularData))
    return 6;
  else if (atype == STANDARD_TYPE(IGESDefs_UnitsData))
    return 7;
  return 0;
}
