// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDF_Attribute.hxx>
#include <XmlMDataXtd_GeometryDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataXtd_GeometryDriver,XmlMDF_ADriver)

static const XmlObjMgt_DOMString& GeometryTypeString
                                        (const TDataXtd_GeometryEnum);
static Standard_Boolean           GeometryTypeEnum
                                        (const XmlObjMgt_DOMString& theString,
                                         TDataXtd_GeometryEnum&     theResult);

IMPLEMENT_DOMSTRING (TypeString,        "geomtype")

IMPLEMENT_DOMSTRING (GeomAnyString,      "any")
IMPLEMENT_DOMSTRING (GeomPointString,    "point")
IMPLEMENT_DOMSTRING (GeomLineString,     "line")
IMPLEMENT_DOMSTRING (GeomCircleString,   "circle")
IMPLEMENT_DOMSTRING (GeomEllipseString,  "ellipse")
IMPLEMENT_DOMSTRING (GeomSplineString,   "slpine")
IMPLEMENT_DOMSTRING (GeomPlaneString,    "plane")
IMPLEMENT_DOMSTRING (GeomCylinderString, "cylinder")

//=======================================================================
//function : XmlMDataXtd_GeometryDriver
//purpose  : Constructor
//=======================================================================
XmlMDataXtd_GeometryDriver::XmlMDataXtd_GeometryDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataXtd_GeometryDriver::NewEmpty() const
{
  return (new TDataXtd_Geometry());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataXtd_GeometryDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aT = 
    Handle(TDataXtd_Geometry)::DownCast (theTarget);

  XmlObjMgt_DOMString aType = theSource.Element().getAttribute(::TypeString());
  TDataXtd_GeometryEnum aTypeEnum;
  if (GeometryTypeEnum (aType, aTypeEnum) == Standard_False) {
    WriteMessage ("TDataXtd_GeometryEnum; "
                  "string value without enum term equivalence");
    return Standard_False;
  }

  aT->SetType (aTypeEnum);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataXtd_GeometryDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aG = Handle(TDataXtd_Geometry)::DownCast(theSource);
  theTarget.Element().setAttribute (::TypeString(),
                                    GeometryTypeString (aG->GetType()));
}

//=======================================================================
//function : GeometryTypeEnum
//purpose  : 
//=======================================================================
static Standard_Boolean GeometryTypeEnum (const XmlObjMgt_DOMString& theString,
                                          TDataXtd_GeometryEnum&     theResult) 
{
  TDataXtd_GeometryEnum aResult = TDataXtd_ANY_GEOM;
  if (!theString.equals (::GeomAnyString()))
  {
    if (theString.equals (::GeomPointString()))
      aResult = TDataXtd_POINT;
    else if (theString.equals (::GeomLineString()))
      aResult = TDataXtd_LINE;
    else if (theString.equals (::GeomCircleString()))
      aResult = TDataXtd_CIRCLE;
    else if (theString.equals (::GeomEllipseString()))
      aResult = TDataXtd_ELLIPSE;
    else if (theString.equals(::GeomSplineString()))
      aResult = TDataXtd_SPLINE;
    else if (theString.equals(::GeomPlaneString()))
      aResult = TDataXtd_PLANE;
    else if (theString.equals(::GeomCylinderString()))
      aResult = TDataXtd_CYLINDER;
    else
      return Standard_False;
  }
  theResult = aResult;
  return Standard_True;
}

//=======================================================================
//function : GeometryTypeString
//purpose  : 
//=======================================================================
static const XmlObjMgt_DOMString& GeometryTypeString
                                        (const TDataXtd_GeometryEnum theE) 
{
  switch (theE)
  {
  case TDataXtd_ANY_GEOM : return ::GeomAnyString();
  case TDataXtd_POINT    : return ::GeomPointString();
  case TDataXtd_LINE     : return ::GeomLineString();
  case TDataXtd_CIRCLE   : return ::GeomCircleString();
  case TDataXtd_ELLIPSE  : return ::GeomEllipseString();
  case TDataXtd_SPLINE   : return ::GeomSplineString();
  case TDataXtd_PLANE    : return ::GeomPlaneString();
  case TDataXtd_CYLINDER : return ::GeomCylinderString();

  default:
    throw Standard_DomainError("TDataXtd_GeometryEnum; enum term unknown");
  }
}
