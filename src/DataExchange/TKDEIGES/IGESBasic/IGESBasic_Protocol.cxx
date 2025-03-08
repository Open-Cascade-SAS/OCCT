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

#include <IGESBasic_Protocol.hxx>

#include <IGESBasic_AssocGroupType.hxx>
#include <IGESBasic_ExternalRefFile.hxx>
#include <IGESBasic_ExternalRefFileIndex.hxx>
#include <IGESBasic_ExternalRefFileName.hxx>
#include <IGESBasic_ExternalRefLibName.hxx>
#include <IGESBasic_ExternalRefName.hxx>
#include <IGESBasic_ExternalReferenceFile.hxx>
#include <IGESBasic_GroupWithoutBackP.hxx>
#include <IGESBasic_Hierarchy.hxx>
#include <IGESBasic_Name.hxx>
#include <IGESBasic_OrderedGroup.hxx>
#include <IGESBasic_OrderedGroupWithoutBackP.hxx>
#include <IGESBasic_SingleParent.hxx>
#include <IGESBasic_SingularSubfigure.hxx>
#include <IGESBasic_SubfigureDef.hxx>
#include <IGESData.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESBasic_Protocol, IGESData_Protocol)

IGESBasic_Protocol::IGESBasic_Protocol() {}

Standard_Integer IGESBasic_Protocol::NbResources() const
{
  return 1;
}

Handle(Interface_Protocol) IGESBasic_Protocol::Resource(const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESData::Protocol();
  return res;
}

Standard_Integer IGESBasic_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESBasic_AssocGroupType))
    return 1;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalRefFile))
    return 2;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalRefFileIndex))
    return 3;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalRefFileName))
    return 4;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalRefLibName))
    return 5;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalRefName))
    return 6;
  else if (atype == STANDARD_TYPE(IGESBasic_ExternalReferenceFile))
    return 7;
  else if (atype == STANDARD_TYPE(IGESBasic_Group))
    return 8;
  else if (atype == STANDARD_TYPE(IGESBasic_GroupWithoutBackP))
    return 9;
  else if (atype == STANDARD_TYPE(IGESBasic_Hierarchy))
    return 10;
  else if (atype == STANDARD_TYPE(IGESBasic_Name))
    return 11;
  else if (atype == STANDARD_TYPE(IGESBasic_OrderedGroup))
    return 12;
  else if (atype == STANDARD_TYPE(IGESBasic_OrderedGroupWithoutBackP))
    return 13;
  else if (atype == STANDARD_TYPE(IGESBasic_SingleParent))
    return 14;
  else if (atype == STANDARD_TYPE(IGESBasic_SingularSubfigure))
    return 15;
  else if (atype == STANDARD_TYPE(IGESBasic_SubfigureDef))
    return 16;
  return 0;
}
