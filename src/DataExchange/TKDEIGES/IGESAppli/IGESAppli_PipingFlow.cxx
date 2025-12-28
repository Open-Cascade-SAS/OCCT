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

#include <IGESAppli_PipingFlow.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_PipingFlow, IGESData_IGESEntity)

IGESAppli_PipingFlow::IGESAppli_PipingFlow() {}

void IGESAppli_PipingFlow::Init(
  const int                                                                      nbContextFlags,
  const int                                                                      aFlowType,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allFlowAssocs,
  const occ::handle<NCollection_HArray1<occ::handle<IGESDraw_ConnectPoint>>>&    allConnectPoints,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>&      allJoins,
  const occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& allFlowNames,
  const occ::handle<NCollection_HArray1<occ::handle<IGESGraph_TextDisplayTemplate>>>& allTextDisps,
  const occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& allContFlowAssocs)
{
  int num = allFlowAssocs->Length();
  if (allFlowAssocs->Lower() != 1 || allConnectPoints->Lower() != 1
      || allConnectPoints->Length() != num || allJoins->Lower() != 1 || allJoins->Length() != num
      || allFlowNames->Lower() != 1 || allFlowNames->Length() != num
      || allContFlowAssocs->Lower() != 1 || allContFlowAssocs->Length() != num)
    throw Standard_DimensionMismatch("IGESAppli_PipingFlow : Init");
  theNbContextFlags          = nbContextFlags;
  theTypeOfFlow              = aFlowType;
  theFlowAssociativities     = allFlowAssocs;
  theConnectPoints           = allConnectPoints;
  theJoins                   = allJoins;
  theFlowNames               = allFlowNames;
  theTextDisplayTemplates    = allTextDisps;
  theContFlowAssociativities = allContFlowAssocs;
  InitTypeAndForm(402, 20);
}

bool IGESAppli_PipingFlow::OwnCorrect()
{
  if (theNbContextFlags == 1)
    return false;
  theNbContextFlags = 1;
  return true;
}

int IGESAppli_PipingFlow::NbContextFlags() const
{
  return theNbContextFlags;
}

int IGESAppli_PipingFlow::NbFlowAssociativities() const
{
  return theFlowAssociativities->Length();
}

int IGESAppli_PipingFlow::NbConnectPoints() const
{
  return theConnectPoints->Length();
}

int IGESAppli_PipingFlow::NbJoins() const
{
  return theJoins->Length();
}

int IGESAppli_PipingFlow::NbFlowNames() const
{
  return theFlowNames->Length();
}

int IGESAppli_PipingFlow::NbTextDisplayTemplates() const
{
  return theTextDisplayTemplates->Length();
}

int IGESAppli_PipingFlow::NbContFlowAssociativities() const
{
  return theContFlowAssociativities->Length();
}

int IGESAppli_PipingFlow::TypeOfFlow() const
{
  return theTypeOfFlow;
}

occ::handle<IGESData_IGESEntity> IGESAppli_PipingFlow::FlowAssociativity(const int Index) const
{
  return theFlowAssociativities->Value(Index);
}

occ::handle<IGESDraw_ConnectPoint> IGESAppli_PipingFlow::ConnectPoint(const int Index) const
{
  return theConnectPoints->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESAppli_PipingFlow::Join(const int Index) const
{
  return theJoins->Value(Index);
}

occ::handle<TCollection_HAsciiString> IGESAppli_PipingFlow::FlowName(const int Index) const
{
  return theFlowNames->Value(Index);
}

occ::handle<IGESGraph_TextDisplayTemplate> IGESAppli_PipingFlow::TextDisplayTemplate(
  const int Index) const
{
  return theTextDisplayTemplates->Value(Index);
}

occ::handle<IGESData_IGESEntity> IGESAppli_PipingFlow::ContFlowAssociativity(const int Index) const
{
  return theContFlowAssociativities->Value(Index);
}
