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

#include <IGESGraph_Protocol.hxx>

#include <IGESBasic.hxx>
#include <IGESBasic_Protocol.hxx>
#include <IGESGraph_Color.hxx>
#include <IGESGraph_DefinitionLevel.hxx>
#include <IGESGraph_DrawingSize.hxx>
#include <IGESGraph_DrawingUnits.hxx>
#include <IGESGraph_HighLight.hxx>
#include <IGESGraph_IntercharacterSpacing.hxx>
#include <IGESGraph_LineFontDefPattern.hxx>
#include <IGESGraph_LineFontDefTemplate.hxx>
#include <IGESGraph_LineFontPredefined.hxx>
#include <IGESGraph_NominalSize.hxx>
#include <IGESGraph_Pick.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <IGESGraph_TextFontDef.hxx>
#include <IGESGraph_UniformRectGrid.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGraph_Protocol, IGESData_Protocol)

IGESGraph_Protocol::IGESGraph_Protocol() {}

Standard_Integer IGESGraph_Protocol::NbResources() const
{
  return 1;
}

Handle(Interface_Protocol) IGESGraph_Protocol::Resource(const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESBasic::Protocol();
  return res;
}

Standard_Integer IGESGraph_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESGraph_Color))
    return 1;
  else if (atype == STANDARD_TYPE(IGESGraph_DefinitionLevel))
    return 2;
  else if (atype == STANDARD_TYPE(IGESGraph_DrawingSize))
    return 3;
  else if (atype == STANDARD_TYPE(IGESGraph_DrawingUnits))
    return 4;
  else if (atype == STANDARD_TYPE(IGESGraph_HighLight))
    return 5;
  else if (atype == STANDARD_TYPE(IGESGraph_IntercharacterSpacing))
    return 6;
  else if (atype == STANDARD_TYPE(IGESGraph_LineFontDefPattern))
    return 7;
  else if (atype == STANDARD_TYPE(IGESGraph_LineFontPredefined))
    return 8;
  else if (atype == STANDARD_TYPE(IGESGraph_LineFontDefTemplate))
    return 9;
  else if (atype == STANDARD_TYPE(IGESGraph_NominalSize))
    return 10;
  else if (atype == STANDARD_TYPE(IGESGraph_Pick))
    return 11;
  else if (atype == STANDARD_TYPE(IGESGraph_TextDisplayTemplate))
    return 12;
  else if (atype == STANDARD_TYPE(IGESGraph_TextFontDef))
    return 13;
  else if (atype == STANDARD_TYPE(IGESGraph_UniformRectGrid))
    return 14;
  return 0;
}
