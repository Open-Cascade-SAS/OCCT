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

#include <IGESAppli_Protocol.hxx>

#include <IGESAppli_DrilledHole.hxx>
#include <IGESAppli_ElementResults.hxx>
#include <IGESAppli_FiniteElement.hxx>
#include <IGESAppli_Flow.hxx>
#include <IGESAppli_FlowLineSpec.hxx>
#include <IGESAppli_LevelFunction.hxx>
#include <IGESAppli_LevelToPWBLayerMap.hxx>
#include <IGESAppli_LineWidening.hxx>
#include <IGESAppli_NodalConstraint.hxx>
#include <IGESAppli_NodalDisplAndRot.hxx>
#include <IGESAppli_NodalResults.hxx>
#include <IGESAppli_Node.hxx>
#include <IGESAppli_PWBArtworkStackup.hxx>
#include <IGESAppli_PWBDrilledHole.hxx>
#include <IGESAppli_PartNumber.hxx>
#include <IGESAppli_PinNumber.hxx>
#include <IGESAppli_PipingFlow.hxx>
#include <IGESAppli_ReferenceDesignator.hxx>
#include <IGESAppli_RegionRestriction.hxx>
#include <IGESDefs.hxx>
#include <IGESDefs_Protocol.hxx>
#include <IGESDraw.hxx>
#include <IGESDraw_Protocol.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Protocol, IGESData_Protocol)

IGESAppli_Protocol::IGESAppli_Protocol() {}

Standard_Integer IGESAppli_Protocol::NbResources() const
{
  return 2;
}

Handle(Interface_Protocol) IGESAppli_Protocol::Resource(const Standard_Integer num) const
{
  Handle(Interface_Protocol) res;
  if (num == 1)
    res = IGESDefs::Protocol();
  if (num == 2)
    res = IGESDraw::Protocol();
  return res;
}

Standard_Integer IGESAppli_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESAppli_DrilledHole))
    return 1;
  else if (atype == STANDARD_TYPE(IGESAppli_ElementResults))
    return 2;
  else if (atype == STANDARD_TYPE(IGESAppli_FiniteElement))
    return 3;
  else if (atype == STANDARD_TYPE(IGESAppli_Flow))
    return 4;
  else if (atype == STANDARD_TYPE(IGESAppli_FlowLineSpec))
    return 5;
  else if (atype == STANDARD_TYPE(IGESAppli_LevelFunction))
    return 6;
  else if (atype == STANDARD_TYPE(IGESAppli_LevelToPWBLayerMap))
    return 7;
  else if (atype == STANDARD_TYPE(IGESAppli_LineWidening))
    return 8;
  else if (atype == STANDARD_TYPE(IGESAppli_NodalConstraint))
    return 9;
  else if (atype == STANDARD_TYPE(IGESAppli_NodalDisplAndRot))
    return 10;
  else if (atype == STANDARD_TYPE(IGESAppli_NodalResults))
    return 11;
  else if (atype == STANDARD_TYPE(IGESAppli_Node))
    return 12;
  else if (atype == STANDARD_TYPE(IGESAppli_PWBArtworkStackup))
    return 13;
  else if (atype == STANDARD_TYPE(IGESAppli_PWBDrilledHole))
    return 14;
  else if (atype == STANDARD_TYPE(IGESAppli_PartNumber))
    return 15;
  else if (atype == STANDARD_TYPE(IGESAppli_PinNumber))
    return 16;
  else if (atype == STANDARD_TYPE(IGESAppli_PipingFlow))
    return 17;
  else if (atype == STANDARD_TYPE(IGESAppli_ReferenceDesignator))
    return 18;
  else if (atype == STANDARD_TYPE(IGESAppli_RegionRestriction))
    return 19;
  return 0;
}
