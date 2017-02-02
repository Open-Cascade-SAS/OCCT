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
#include <TDataStd_Real.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDF_Attribute.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlMDataXtd_ConstraintDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMDataXtd_ConstraintDriver,XmlMDF_ADriver)

static TDataXtd_ConstraintEnum    ConstraintTypeEnum
                                                (const XmlObjMgt_DOMString&);
static const XmlObjMgt_DOMString& ConstraintTypeString
                                                (const TDataXtd_ConstraintEnum);

IMPLEMENT_DOMSTRING (TypeString,             "contype")
IMPLEMENT_DOMSTRING (ValueString,            "valueref")
IMPLEMENT_DOMSTRING (GeometriesString,       "geometries")
IMPLEMENT_DOMSTRING (PlaneString,            "plane")
IMPLEMENT_DOMSTRING (StatusString,           "flags")
     // planar constraints
IMPLEMENT_DOMSTRING (ConRadiusString,        "radius")
IMPLEMENT_DOMSTRING (ConDiameterString,      "diameter")
IMPLEMENT_DOMSTRING (ConMinRadiusString,     "minorradius")
IMPLEMENT_DOMSTRING (ConMajRadiusString,     "majorradius")
IMPLEMENT_DOMSTRING (ConTangentString,       "tangent")
IMPLEMENT_DOMSTRING (ConParallelString,      "parallel")
IMPLEMENT_DOMSTRING (ConPerpendicularString, "perpendicular")
IMPLEMENT_DOMSTRING (ConConcentricString,    "concentric")
IMPLEMENT_DOMSTRING (ConCoincidentString,    "coincident")
IMPLEMENT_DOMSTRING (ConDistanceString,      "distance")
IMPLEMENT_DOMSTRING (ConAngleString,         "angle")
IMPLEMENT_DOMSTRING (ConEqualRadiusString,   "equalradius")
IMPLEMENT_DOMSTRING (ConSymmetryString,      "symmetry")
IMPLEMENT_DOMSTRING (ConMidPointString,      "midpoint")
IMPLEMENT_DOMSTRING (ConEqualDistanceString, "equaldist")
IMPLEMENT_DOMSTRING (ConFixString,           "fix")
IMPLEMENT_DOMSTRING (ConRigidString,         "rigid")
     // placement constraints
IMPLEMENT_DOMSTRING (ConFromString,          "from")
IMPLEMENT_DOMSTRING (ConAxisString,          "axis")
IMPLEMENT_DOMSTRING (ConMateString,          "mate")
IMPLEMENT_DOMSTRING (ConAlignFacesString,    "alignfaces")
IMPLEMENT_DOMSTRING (ConAlignAxesString,     "alignaxes")
IMPLEMENT_DOMSTRING (ConAxesAngleString,     "axesangle")
IMPLEMENT_DOMSTRING (ConFacesAngleString,    "facesangle")
IMPLEMENT_DOMSTRING (ConRoundString,         "round")
IMPLEMENT_DOMSTRING (ConOffsetString,        "offset")

//=======================================================================
//function : XmlMDataXtd_ConstraintDriver
//purpose  : Constructor
//=======================================================================
XmlMDataXtd_ConstraintDriver::XmlMDataXtd_ConstraintDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataXtd_ConstraintDriver::NewEmpty() const
{
  return (new TDataXtd_Constraint());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMDataXtd_ConstraintDriver::Paste
                        (const XmlObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC = 
    Handle(TDataXtd_Constraint)::DownCast(theTarget);
  const XmlObjMgt_Element& anElem = theSource;

  Standard_Integer aNb;
  TCollection_ExtendedString aMsgString;

  // value
  Handle(TDataStd_Real) aTValue;
  XmlObjMgt_DOMString aDOMStr = anElem.getAttribute(::ValueString());
  if (aDOMStr != NULL)
  {
    if (!aDOMStr.GetInteger(aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMDataXtd_ConstraintDriver: "
         "Cannot retrieve reference on Integer attribute from \"")
          + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    if (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        aTValue = Handle(TDataStd_Real)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aTValue = new TDataStd_Real;
        theRelocTable.Bind(aNb, aTValue);
      }
      aC->SetValue(aTValue);
    }
  }

  // geometries
  aDOMStr = anElem.getAttribute(::GeometriesString());
  if (aDOMStr != NULL)
  {
    Standard_CString aGs = Standard_CString(aDOMStr.GetString());

    // first geometry
    if (!XmlObjMgt::GetInteger(aGs, aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMDataXtd_ConstraintDriver: Cannot retrieve reference on first Geometry from \"")
          + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    Standard_Integer i = 1;
    while (aNb > 0)
    {
      Handle(TNaming_NamedShape) aG;
      if (theRelocTable.IsBound(aNb))
        aG = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aG = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, aG);
      }
      aC->SetGeometry (i, aG);

      // next geometry
      if (!XmlObjMgt::GetInteger(aGs, aNb)) aNb = 0;
      i++;
    }
  }

  // plane
  aDOMStr = anElem.getAttribute(::PlaneString());
  if (aDOMStr != NULL)
  {
    if (!aDOMStr.GetInteger(aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMDataXtd_ConstraintDriver: Cannot retrieve reference on Plane from \"")
          + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    Handle(TNaming_NamedShape) aTPlane;
    if (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        aTPlane = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        aTPlane = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, aTPlane);
      }
      aC->SetPlane(aTPlane);
    }
  }

  // constraint type
  XmlObjMgt_DOMString aType = anElem.getAttribute(::TypeString());
  aC->SetType(ConstraintTypeEnum(aType));

  // flags
  XmlObjMgt_DOMString aString = anElem.getAttribute(::StatusString());
  const char * aPtr = aString.GetString();
  aC->Verified((*aPtr) == '+');
  aPtr ++;
  aC->Inverted((*aPtr) == '+');
  aPtr ++;
  aC->Reversed((*aPtr) == '+');

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMDataXtd_ConstraintDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         XmlObjMgt_Persistent&        theTarget,
                         XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TDataXtd_Constraint) aC =
    Handle(TDataXtd_Constraint)::DownCast(theSource);
  XmlObjMgt_Element& anElem = theTarget;

  Standard_Integer aNb;

  // value
  Handle(TDataStd_Real) aValue = aC->GetValue();
  if (!aValue.IsNull())
  {
    aNb = theRelocTable.FindIndex(aValue);
    if (aNb == 0)
    {
      aNb = theRelocTable.Add(aValue);
    }
    anElem.setAttribute(::ValueString(), aNb);
  }

  // geometries
  Standard_Integer NbGeom = aC->NbGeometries();
  if (NbGeom >= 1)
  {
    TCollection_AsciiString aGsStr;
    for (Standard_Integer i = 1; i <= NbGeom; i++)
    {
      Handle(TNaming_NamedShape) aG = aC->GetGeometry(i);
      aNb = 0;
      if (!aG.IsNull())
      {
        aNb = theRelocTable.FindIndex(aG);
        if (aNb == 0)
        {
          aNb = theRelocTable.Add(aG);
        }
        aGsStr += TCollection_AsciiString(aNb) + " ";
      }
      else aGsStr += "0 ";
    }
    anElem.setAttribute(::GeometriesString(), aGsStr.ToCString());
  }

  // plane
  Handle(TNaming_NamedShape) aTPlane = aC->GetPlane();
  if (!aTPlane.IsNull())
  {
    aNb = theRelocTable.FindIndex(aTPlane);
    if (aNb == 0)
    {
      aNb = theRelocTable.Add(aTPlane);
    }
    anElem.setAttribute(::PlaneString(), aNb);
  }
  
  // constraint type
  anElem.setAttribute(::TypeString(), ConstraintTypeString(aC->GetType()));

  // flags
  TCollection_AsciiString aStatusStr;

  if (aC->Verified()) aStatusStr += "+";
  else aStatusStr += "-";

  if (aC->Inverted()) aStatusStr += "+";
  else aStatusStr += "-";

  if (aC->Reversed()) aStatusStr += "+";
  else aStatusStr += "-";

  anElem.setAttribute(::StatusString(), aStatusStr.ToCString());
}

//=======================================================================
//function : ConstraintTypeEnum
//purpose  : 
//=======================================================================
static TDataXtd_ConstraintEnum ConstraintTypeEnum (const XmlObjMgt_DOMString& theString) 
{
  TDataXtd_ConstraintEnum aResult = TDataXtd_RADIUS;
  // planar constraints
  if (!theString.equals (::ConRadiusString()))
  {
    if (theString.equals (::ConDiameterString()))
      aResult = TDataXtd_DIAMETER;
    else if (theString.equals (::ConMinRadiusString()))
      aResult = TDataXtd_MINOR_RADIUS;
    else if (theString.equals (::ConMajRadiusString()))
      aResult = TDataXtd_MAJOR_RADIUS;
    else if (theString.equals (::ConTangentString()))
      aResult = TDataXtd_TANGENT;
    else if (theString.equals (::ConParallelString()))
      aResult = TDataXtd_PARALLEL;
    else if (theString.equals (::ConPerpendicularString()))
      aResult = TDataXtd_PERPENDICULAR;
    else if (theString.equals (::ConConcentricString()))
      aResult = TDataXtd_CONCENTRIC;
    else if (theString.equals (::ConCoincidentString()))
      aResult = TDataXtd_COINCIDENT;
    else if (theString.equals (::ConDistanceString()))
      aResult = TDataXtd_DISTANCE;
    else if (theString.equals (::ConAngleString()))
      aResult = TDataXtd_ANGLE;
    else if (theString.equals (::ConEqualRadiusString()))
      aResult = TDataXtd_EQUAL_RADIUS;
    else if (theString.equals (::ConSymmetryString()))
      aResult = TDataXtd_SYMMETRY;
    else if (theString.equals (::ConMidPointString()))
      aResult = TDataXtd_MIDPOINT;
    else if (theString.equals (::ConEqualDistanceString()))
      aResult = TDataXtd_EQUAL_DISTANCE;
    else if (theString.equals (::ConFixString()))
      aResult = TDataXtd_FIX;
    else if (theString.equals (::ConRigidString()))
      aResult = TDataXtd_RIGID;
    // placement constraints
    else if (theString.equals (::ConFromString()))
      aResult = TDataXtd_FROM;
    else if (theString.equals (::ConAxisString()))
      aResult = TDataXtd_AXIS;
    else if (theString.equals (::ConMateString()))
      aResult = TDataXtd_MATE;
    else if (theString.equals (::ConAlignFacesString()))
      aResult = TDataXtd_ALIGN_FACES;
    else if (theString.equals (::ConAlignAxesString()))
      aResult = TDataXtd_ALIGN_AXES;
    else if (theString.equals (::ConAxesAngleString()))
      aResult = TDataXtd_AXES_ANGLE;
    else if (theString.equals (::ConFacesAngleString()))
      aResult = TDataXtd_FACES_ANGLE;
    else if (theString.equals (::ConRoundString()))
      aResult = TDataXtd_ROUND;
    else if (theString.equals (::ConOffsetString()))
      aResult = TDataXtd_OFFSET;
    else
      throw Standard_DomainError("TDataXtd_ConstraintEnum; string value without enum term equivalence");
  }
  return aResult;
}

//=======================================================================
//function : ConstraintTypeString
//purpose  : 
//=======================================================================
static const XmlObjMgt_DOMString& ConstraintTypeString (const TDataXtd_ConstraintEnum theE) 
{
  switch (theE)
  {
    // planar constraints
  case TDataXtd_RADIUS         : return ::ConRadiusString();
  case TDataXtd_DIAMETER       : return ::ConDiameterString();
  case TDataXtd_MINOR_RADIUS   : return ::ConMinRadiusString();
  case TDataXtd_MAJOR_RADIUS   : return ::ConMajRadiusString();
  case TDataXtd_TANGENT        : return ::ConTangentString();
  case TDataXtd_PARALLEL       : return ::ConParallelString();
  case TDataXtd_PERPENDICULAR  : return ::ConPerpendicularString();
  case TDataXtd_CONCENTRIC     : return ::ConConcentricString();
  case TDataXtd_COINCIDENT     : return ::ConCoincidentString();
  case TDataXtd_DISTANCE       : return ::ConDistanceString();
  case TDataXtd_ANGLE          : return ::ConAngleString();
  case TDataXtd_EQUAL_RADIUS   : return ::ConEqualRadiusString();
  case TDataXtd_SYMMETRY       : return ::ConSymmetryString();
  case TDataXtd_MIDPOINT       : return ::ConMidPointString();   
  case TDataXtd_EQUAL_DISTANCE : return ::ConEqualDistanceString();
  case TDataXtd_FIX            : return ::ConFixString();
  case TDataXtd_RIGID          : return ::ConRigidString();
    // placement constraints
  case TDataXtd_FROM           : return ::ConFromString(); 
  case TDataXtd_AXIS           : return ::ConAxisString(); 
  case TDataXtd_MATE           : return ::ConMateString();
  case TDataXtd_ALIGN_FACES    : return ::ConAlignFacesString();
  case TDataXtd_ALIGN_AXES     : return ::ConAlignAxesString();
  case TDataXtd_AXES_ANGLE     : return ::ConAxesAngleString();
  case TDataXtd_FACES_ANGLE    : return ::ConFacesAngleString();
  case TDataXtd_ROUND          : return ::ConRoundString();
  case TDataXtd_OFFSET         : return ::ConOffsetString();
    
  default:
    throw Standard_DomainError("TDataXtd_ConstraintEnum; enum term unknown");
  }
}
