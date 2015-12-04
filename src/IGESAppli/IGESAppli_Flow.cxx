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
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESAppli_Flow,IGESData_IGESEntity)

IGESAppli_Flow::IGESAppli_Flow ()    {  }


    void  IGESAppli_Flow::Init
  (const Standard_Integer nbContextFlags,
   const Standard_Integer aFlowType, const Standard_Integer aFuncFlag,
   const Handle(IGESData_HArray1OfIGESEntity)&    allFlowAssocs,
   const Handle(IGESDraw_HArray1OfConnectPoint)&  allConnectPoints,
   const Handle(IGESData_HArray1OfIGESEntity)&    allJoins,
   const Handle(Interface_HArray1OfHAsciiString)& allFlowNames,
   const Handle(IGESGraph_HArray1OfTextDisplayTemplate)& allTextDisps,
   const Handle(IGESData_HArray1OfIGESEntity)&    allContFlowAssocs)
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
  InitTypeAndForm(402,18);
}

    Standard_Boolean  IGESAppli_Flow::OwnCorrect ()
{
  if (theNbContextFlags == 2) return Standard_False;
  theNbContextFlags = 2;
  return Standard_True;
}


    Standard_Integer  IGESAppli_Flow::NbContextFlags () const
{
  return theNbContextFlags;
}

    Standard_Integer  IGESAppli_Flow::NbFlowAssociativities () const
{
  return (theFlowAssociativities.IsNull() ? 0 : theFlowAssociativities->Length());
}

    Standard_Integer  IGESAppli_Flow::NbConnectPoints () const
{
  return (theConnectPoints.IsNull() ? 0 : theConnectPoints->Length());
}

    Standard_Integer  IGESAppli_Flow::NbJoins () const
{
  return (theJoins.IsNull() ? 0 : theJoins->Length());
}

    Standard_Integer  IGESAppli_Flow::NbFlowNames () const
{
  return (theFlowNames.IsNull() ? 0 : theFlowNames->Length());
}

    Standard_Integer  IGESAppli_Flow::NbTextDisplayTemplates () const
{
  return (theTextDisplayTemplates.IsNull() ? 0 : theTextDisplayTemplates->Length());
}

    Standard_Integer  IGESAppli_Flow::NbContFlowAssociativities () const
{
  return (theContFlowAssociativities.IsNull() ? 0 : theContFlowAssociativities->Length());
}

    Standard_Integer  IGESAppli_Flow::TypeOfFlow () const
{
  return theTypeOfFlow;
}

    Standard_Integer  IGESAppli_Flow::FunctionFlag () const
{
  return theFunctionFlag;
}

    Handle(IGESData_IGESEntity)  IGESAppli_Flow::FlowAssociativity
  (const Standard_Integer Index) const
{
  return theFlowAssociativities->Value(Index);
}

    Handle(IGESDraw_ConnectPoint)  IGESAppli_Flow::ConnectPoint
  (const Standard_Integer Index) const
{
  return theConnectPoints->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESAppli_Flow::Join
  (const Standard_Integer Index) const
{
  return theJoins->Value(Index);
}

    Handle(TCollection_HAsciiString)  IGESAppli_Flow::FlowName
  (const Standard_Integer Index) const
{
  return theFlowNames->Value(Index);
}

    Handle(IGESGraph_TextDisplayTemplate)  IGESAppli_Flow::TextDisplayTemplate
  (const Standard_Integer Index) const
{
  return theTextDisplayTemplates->Value(Index);
}

    Handle(IGESData_IGESEntity)  IGESAppli_Flow::ContFlowAssociativity
  (const Standard_Integer Index) const
{
  return theContFlowAssociativities->Value(Index);
}
