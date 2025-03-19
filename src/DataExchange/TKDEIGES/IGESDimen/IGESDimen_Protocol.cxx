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

#include <IGESDimen_Protocol.hxx>

#include <IGESDimen_AngularDimension.hxx>
#include <IGESDimen_BasicDimension.hxx>
#include <IGESDimen_CenterLine.hxx>
#include <IGESDimen_CurveDimension.hxx>
#include <IGESDimen_DiameterDimension.hxx>
#include <IGESDimen_DimensionDisplayData.hxx>
#include <IGESDimen_DimensionTolerance.hxx>
#include <IGESDimen_DimensionUnits.hxx>
#include <IGESDimen_DimensionedGeometry.hxx>
#include <IGESDimen_FlagNote.hxx>
#include <IGESDimen_GeneralLabel.hxx>
#include <IGESDimen_GeneralNote.hxx>
#include <IGESDimen_GeneralSymbol.hxx>
#include <IGESDimen_LeaderArrow.hxx>
#include <IGESDimen_LinearDimension.hxx>
#include <IGESDimen_NewDimensionedGeometry.hxx>
#include <IGESDimen_NewGeneralNote.hxx>
#include <IGESDimen_OrdinateDimension.hxx>
#include <IGESDimen_PointDimension.hxx>
#include <IGESDimen_RadiusDimension.hxx>
#include <IGESDimen_Section.hxx>
#include <IGESDimen_SectionedArea.hxx>
#include <IGESDimen_WitnessLine.hxx>
#include <IGESGeom.hxx>
#include <IGESGeom_Protocol.hxx>
#include <IGESGraph.hxx>
#include <IGESGraph_Protocol.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDimen_Protocol, IGESData_Protocol)

IGESDimen_Protocol::IGESDimen_Protocol() {}

Standard_Integer IGESDimen_Protocol::NbResources() const
{
  return 2;
}

Handle(Interface_Protocol) IGESDimen_Protocol::Resource(const Standard_Integer num) const
{
  Handle(Interface_Protocol) res;
  if (num == 1)
    res = IGESGraph::Protocol();
  if (num == 2)
    res = IGESGeom::Protocol();
  return res;
}

Standard_Integer IGESDimen_Protocol::TypeNumber(const Handle(Standard_Type)& atype) const
{
  if (atype == STANDARD_TYPE(IGESDimen_AngularDimension))
    return 1;
  else if (atype == STANDARD_TYPE(IGESDimen_BasicDimension))
    return 2;
  else if (atype == STANDARD_TYPE(IGESDimen_CenterLine))
    return 3;
  else if (atype == STANDARD_TYPE(IGESDimen_CurveDimension))
    return 4;
  else if (atype == STANDARD_TYPE(IGESDimen_DiameterDimension))
    return 5;
  else if (atype == STANDARD_TYPE(IGESDimen_DimensionDisplayData))
    return 6;
  else if (atype == STANDARD_TYPE(IGESDimen_DimensionTolerance))
    return 7;
  else if (atype == STANDARD_TYPE(IGESDimen_DimensionUnits))
    return 8;
  else if (atype == STANDARD_TYPE(IGESDimen_DimensionedGeometry))
    return 9;
  else if (atype == STANDARD_TYPE(IGESDimen_FlagNote))
    return 10;
  else if (atype == STANDARD_TYPE(IGESDimen_GeneralLabel))
    return 11;
  else if (atype == STANDARD_TYPE(IGESDimen_GeneralNote))
    return 12;
  else if (atype == STANDARD_TYPE(IGESDimen_GeneralSymbol))
    return 13;
  else if (atype == STANDARD_TYPE(IGESDimen_LeaderArrow))
    return 14;
  else if (atype == STANDARD_TYPE(IGESDimen_LinearDimension))
    return 15;
  else if (atype == STANDARD_TYPE(IGESDimen_NewDimensionedGeometry))
    return 16;
  else if (atype == STANDARD_TYPE(IGESDimen_NewGeneralNote))
    return 17;
  else if (atype == STANDARD_TYPE(IGESDimen_OrdinateDimension))
    return 18;
  else if (atype == STANDARD_TYPE(IGESDimen_PointDimension))
    return 19;
  else if (atype == STANDARD_TYPE(IGESDimen_RadiusDimension))
    return 20;
  else if (atype == STANDARD_TYPE(IGESDimen_Section))
    return 21;
  else if (atype == STANDARD_TYPE(IGESDimen_SectionedArea))
    return 22;
  else if (atype == STANDARD_TYPE(IGESDimen_WitnessLine))
    return 23;
  return 0;
}
