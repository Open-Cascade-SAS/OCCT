// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMDataXtd_GeometryDriver.ixx>
#include <TDataXtd_Geometry.hxx>

static const XmlObjMgt_DOMString& GeometryTypeString
                                        (const TDataXtd_GeometryEnum);
static Standard_Boolean           GeometryTypeEnum
                                        (const XmlObjMgt_DOMString& theString,
                                         TDataXtd_GeometryEnum&     theResult);

IMPLEMENT_DOMSTRING (TypeString,        "geomtype")

IMPLEMENT_DOMSTRING (GeomAnyString,     "any")
IMPLEMENT_DOMSTRING (GeomPointString,   "point")
IMPLEMENT_DOMSTRING (GeomLineString,    "line")
IMPLEMENT_DOMSTRING (GeomCircleString,  "circle")
IMPLEMENT_DOMSTRING (GeomEllipseString, "ellipse")

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
    
  default:
    Standard_DomainError::Raise("TDataXtd_GeometryEnum; enum term unknown");
  }
  static XmlObjMgt_DOMString aNullString;
  return aNullString; // To avoid compilation error message.
}
