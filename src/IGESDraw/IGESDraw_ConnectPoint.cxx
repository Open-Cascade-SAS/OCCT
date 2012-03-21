// Created by: CKY / Contract Toubro-Larsen
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESDraw_ConnectPoint.ixx>
#include <gp_GTrsf.hxx>


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
  theSwapFlag           = aSwapFlag;
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
