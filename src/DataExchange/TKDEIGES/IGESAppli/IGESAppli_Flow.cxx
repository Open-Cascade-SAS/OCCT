// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESAppli_Flow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Flow, IGESData_IGESEntity)

IGESAppli_Flow::IGESAppli_Flow() {}

void IGESAppli_Flow::Init(
  const int                                                                      nbContextFlags,
  const int                                                                      aFlowType,
  const int                                                                      aFuncFlag,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allFlowAssocs,
  const occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>>&    allConnectPoints,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allJoins,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allFlowNames,
  const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextDisplayTemplate>>>& allTextDisps,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allContFlowAssocs)
{
  theNbContextFlags          = nbContextFlags;
  theTypeOfFlow              = aFlowType;
  theFunctionFlag            = aFuncFlag;
  theFlowAssociativities     = allFlowAssocs;
  theConnectPoints           = allConnectPoints;
  theJoins                   = allJoins;
  theFlowNames               = allFlowNames;
  theTextDisplayTemplates    = allTextDisps;
  theContFlowAssociativities = allContFlowAssocs;
  InitTypeAndForm(402, 18);
}

bool IGESAppli_Flow::OwnCorrect()
{
  if (theNbContextFlags == 2)
    return false;
  theNbContextFlags = 2;
  return true;
}

int IGESAppli_Flow::NbContextFlags() const
{
  return theNbContextFlags;
}

int IGESAppli_Flow::NbFlowAssociativities() const
{
  return (theFlowAssociativities.IsNull() ? 0 : theFlowAssociativities->Length());
}

int IGESAppli_Flow::NbConnectPoints() const
{
  return (theConnectPoints.IsNull() ? 0 : theConnectPoints->Length());
}

int IGESAppli_Flow::NbJoins() const
{
  return (theJoins.IsNull() ? 0 : theJoins->Length());
}

int IGESAppli_Flow::NbFlowNames() const
{
  return (theFlowNames.IsNull() ? 0 : theFlowNames->Length());
}

int IGESAppli_Flow::NbTextDisplayTemplates() const
{
  return (theTextDisplayTemplates.IsNull() ? 0 : theTextDisplayTemplates->Length());
}

int IGESAppli_Flow::NbContFlowAssociativities() const
{
  return (theContFlowAssociativities.IsNull() ? 0 : theContFlowAssociativities->Length());
}

int IGESAppli_Flow::TypeOfFlow() const
{
  return theTypeOfFlow;
}

int IGESAppli_Flow::FunctionFlag() const
{
  return theFunctionFlag;
}

occ::handle<IGESData_IGESEntity> IGESAppli_Flow::FlowAssociativity(const int Index) const
{
  return theFlowAssociativities->Value(Index);
}

occ::handle<IGESDraw_ConnectPoint> IGESAppli_Flow::ConnectPoint(const int Index) const
{
  return theConnectPoints->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESAppli_Flow::Join(const int Index) const
{
  return theJoins->Value(Index);
}

occ::handle<TCollection_HAsciiString> IGESAppli_Flow::FlowName(const int Index) const
{
  return theFlowNames->Value(Index);
}

occ::handle<IGESGraph_TextDisplayTemplate> IGESAppli_Flow::TextDisplayTemplate(
  const int Index) const
{
  return theTextDisplayTemplates->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESAppli_Flow::ContFlowAssociativity(const int Index) const
{
  return theContFlowAssociativities->Value(Index);
}
