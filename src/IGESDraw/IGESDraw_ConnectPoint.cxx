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

#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESGraph_TextDisplayTemplate.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_ConnectPoint,IGESData_IGESEntity)

IGESDraw_ConnectPoint::IGESDraw_ConnectPoint ()    {  }


    void IGESDraw_ConnectPoint::Init
  (const gp_XYZ&                                aPoint,
   const Handle(IGESData_IGESEntity)&           aDisplaySymbol,
   const Standard_Integer                       aTypeFlag,
   const Standard_Integer                       aFunctionFlag,
   const Handle(TCollection_HAsciiString)&      aFunctionIdentifier,
   const Handle(IGESGraph_TextDisplayTemplate)& anIdentifierTemplate,
   const Handle(TCollection_HAsciiString)&      aFunctionName,
   const Handle(IGESGraph_TextDisplayTemplate)& aFunctionTemplate,
   const Standard_Integer                       aPointIdentifier,
   const Standard_Integer                       aFunctionCode,
   const Standard_Integer                       aSwapFlag,
   const Handle(IGESData_IGESEntity)&           anOwnerSubfigure)
{
  thePoint              = aPoint;
  theDisplaySymbol      = aDisplaySymbol;
  theTypeFlag           = aTypeFlag;
  theFunctionFlag       = aFunctionFlag;
  theFunctionIdentifier = aFunctionIdentifier;
  theIdentifierTemplate = anIdentifierTemplate;
  theFunctionName       = aFunctionName;
  theFunctionTemplate   = aFunctionTemplate;
  thePointIdentifier    = aPointIdentifier;
  theFunctionCode       = aFunctionCode;
  theSwapFlag           = aSwapFlag != 0;
  theOwnerSubfigure     = anOwnerSubfigure;
  InitTypeAndForm(132,0);
}

    gp_Pnt IGESDraw_ConnectPoint::Point () const
{
  gp_Pnt tempPoint(thePoint);
  return tempPoint;
}

    gp_Pnt IGESDraw_ConnectPoint::TransformedPoint () const
{
  gp_XYZ tempPoint = thePoint;
  if (HasTransf()) Location().Transforms(tempPoint);
  gp_Pnt tempRes(tempPoint);

  return (tempRes);
}

    Standard_Boolean IGESDraw_ConnectPoint::HasDisplaySymbol () const
{
  return (! theDisplaySymbol.IsNull());
}

    Handle(IGESData_IGESEntity) IGESDraw_ConnectPoint::DisplaySymbol () const
{
  return theDisplaySymbol;
}

    Standard_Integer IGESDraw_ConnectPoint::TypeFlag () const
{
  return theTypeFlag;
}

    Standard_Integer IGESDraw_ConnectPoint::FunctionFlag () const
{
  return theFunctionFlag;
}

    Handle(TCollection_HAsciiString) IGESDraw_ConnectPoint::FunctionIdentifier
  () const
{
  return theFunctionIdentifier;
}

    Standard_Boolean IGESDraw_ConnectPoint::HasIdentifierTemplate () const
{
  return (! theIdentifierTemplate.IsNull());
}

    Handle(IGESGraph_TextDisplayTemplate)
    IGESDraw_ConnectPoint::IdentifierTemplate () const
{
  return theIdentifierTemplate;
}

    Handle(TCollection_HAsciiString) IGESDraw_ConnectPoint::FunctionName () const
{
  return theFunctionName;
}

    Standard_Boolean IGESDraw_ConnectPoint::HasFunctionTemplate () const
{
  return (! theFunctionTemplate.IsNull());
}

    Handle(IGESGraph_TextDisplayTemplate)
    IGESDraw_ConnectPoint::FunctionTemplate () const
{
  return theFunctionTemplate;
}

    Standard_Integer IGESDraw_ConnectPoint::PointIdentifier () const
{
  return thePointIdentifier;
}

    Standard_Integer IGESDraw_ConnectPoint::FunctionCode () const
{
  return theFunctionCode;
}

    Standard_Boolean IGESDraw_ConnectPoint::SwapFlag () const
{
  return theSwapFlag;
}

    Standard_Boolean IGESDraw_ConnectPoint::HasOwnerSubfigure () const
{
  return (! theOwnerSubfigure.IsNull());
}

    Handle(IGESData_IGESEntity)  IGESDraw_ConnectPoint::OwnerSubfigure () const
{
  return theOwnerSubfigure;
}
