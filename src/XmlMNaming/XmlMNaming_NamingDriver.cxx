// Created on: 2001-08-31
// Created by: Julia DOROVSKIKH
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
#include <TDF_Attribute.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_ListIteratorOfListOfNamedShape.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <XmlMNaming.hxx>
#include <XmlMNaming_NamingDriver.hxx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMNaming_NamingDriver,XmlMDF_ADriver)

//=======================================================================
static TopAbs_ShapeEnum ShapeEnumFromString (const XmlObjMgt_DOMString&);
static TNaming_NameType NameTypeFromString (const XmlObjMgt_DOMString&);

static const XmlObjMgt_DOMString& ShapeEnumToString (const TopAbs_ShapeEnum);
static const XmlObjMgt_DOMString& NameTypeToString (const TNaming_NameType);

IMPLEMENT_DOMSTRING (IndexString,               "index")
IMPLEMENT_DOMSTRING (StopNamedShapeString,      "stopNS")
IMPLEMENT_DOMSTRING (TypeString,                "nametype")
IMPLEMENT_DOMSTRING (ShapeTypeString,           "shapetype")
IMPLEMENT_DOMSTRING (ArgumentsString,           "arguments")
IMPLEMENT_DOMSTRING (ContextLabelString,        "contextlabel")
IMPLEMENT_DOMSTRING (OrientString,              "orientation")

IMPLEMENT_DOMSTRING (NTUnknownString,           "unknown")
IMPLEMENT_DOMSTRING (NTIdentityString,          "identity")
IMPLEMENT_DOMSTRING (NTModifUntilString,        "modifuntil")
IMPLEMENT_DOMSTRING (NTGenerationString,        "generation")
IMPLEMENT_DOMSTRING (NTIntersectionString,      "intersection")
IMPLEMENT_DOMSTRING (NTUnionString,             "union")
IMPLEMENT_DOMSTRING (NTSubtractionString,       "subtraction")
IMPLEMENT_DOMSTRING (NTConstShapeString,        "constshape")
IMPLEMENT_DOMSTRING (NTFilterByNeighString,     "filterbyneigh")
IMPLEMENT_DOMSTRING (NTOrientationString,       "orientation")
IMPLEMENT_DOMSTRING (NTWireInString,            "wirein")
IMPLEMENT_DOMSTRING (NTShellInString,            "shellin")

IMPLEMENT_DOMSTRING (ShCompoundString,          "compound")
IMPLEMENT_DOMSTRING (ShCompsolidString,         "compsolid")
IMPLEMENT_DOMSTRING (ShSolidString,             "solid")
IMPLEMENT_DOMSTRING (ShShellString,             "shell")
IMPLEMENT_DOMSTRING (ShFaceString,              "face")
IMPLEMENT_DOMSTRING (ShWireString,              "wire")
IMPLEMENT_DOMSTRING (ShEdgeString,              "edge")
IMPLEMENT_DOMSTRING (ShVertexString,            "vertex")
IMPLEMENT_DOMSTRING (ShShapeString,             "shape")

//=======================================================================
//function : XmlMNaming_NamingDriver
//purpose  : Constructor
//=======================================================================
XmlMNaming_NamingDriver::XmlMNaming_NamingDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMNaming_NamingDriver::NewEmpty() const
{
  return (new TNaming_Naming());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMNaming_NamingDriver::Paste
                        (const XmlObjMgt_Persistent&  theSource,
                         const Handle(TDF_Attribute)& theTarget,
                         XmlObjMgt_RRelocationTable&  theRelocTable) const
{
  const XmlObjMgt_Element& anElem = theSource;
  Handle(TNaming_Naming) aNg = Handle(TNaming_Naming)::DownCast(theTarget);  

  TNaming_Name& aNgName = aNg->ChangeName();

  // type and shape type
  aNgName.Type     (NameTypeFromString (anElem.getAttribute(::TypeString())));
  aNgName.ShapeType(ShapeEnumFromString(anElem.getAttribute(::ShapeTypeString())));

  Standard_Integer aNb;
  Handle(TNaming_NamedShape) NS;
  TCollection_ExtendedString aMsgString;

  XmlObjMgt_DOMString aDOMStr = anElem.getAttribute(::ArgumentsString());
  if (aDOMStr != NULL)
  {
    Standard_CString aGs = Standard_CString(aDOMStr.GetString());

    // first argument
    if (!XmlObjMgt::GetInteger(aGs, aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMNaming_NamingDriver: Cannot retrieve reference "
         "on first Argument from \"") + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    while (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        NS = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        NS = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, NS);
      }
      aNgName.Append(NS);

      // next argument
      if (!XmlObjMgt::GetInteger(aGs, aNb)) aNb = 0;
    }
  }

  // stop named shape
  aDOMStr = anElem.getAttribute(::StopNamedShapeString());
  if (aDOMStr != NULL)
  {
    if (!aDOMStr.GetInteger(aNb))
    {
      aMsgString = TCollection_ExtendedString
        ("XmlMNaming_NamingDriver: Cannot retrieve reference "
         "on StopNamedShape from \"") + aDOMStr + "\"";
      WriteMessage (aMsgString);
      return Standard_False;
    }
    if (aNb > 0)
    {
      if (theRelocTable.IsBound(aNb))
        NS = Handle(TNaming_NamedShape)::DownCast(theRelocTable.Find(aNb));
      else
      {
        NS = new TNaming_NamedShape;
        theRelocTable.Bind(aNb, NS);
      }
      aNgName.StopNamedShape(NS);  
    }
  }

  // index
  aDOMStr = anElem.getAttribute(::IndexString());
  if (!aDOMStr.GetInteger(aNb))
  {
    aMsgString = TCollection_ExtendedString
      ("XmlMNaming_NamingDriver: Cannot retrieve "
       "integer value of Index from \"") + aDOMStr + "\"";
    WriteMessage (aMsgString);
    return Standard_False;
  }
  aNgName.Index(aNb);
//
  if(XmlMNaming::DocumentVersion() > 3) {
    XmlObjMgt_DOMString aDomEntry = anElem.getAttribute(::ContextLabelString());
    if (aDomEntry != NULL)
      {	
	TCollection_AsciiString anEntry;
	if (XmlObjMgt::GetTagEntryString (aDomEntry, anEntry) == Standard_False)
	  {
	    TCollection_ExtendedString aMessage =
	      TCollection_ExtendedString ("Cannot retrieve Entry from \"")
		+ aDomEntry + '\"';
	    WriteMessage (aMessage);
	    return Standard_False;
	  }
    
	// find label by entry
	TDF_Label tLab; // Null label.
	if (anEntry.Length() > 0) {
	  TDF_Tool::Label(aNg->Label().Data(), anEntry, tLab, Standard_True);
	    aNgName.ContextLabel(tLab);
#ifdef OCCT_DEBUG
	    cout << "Retrieving Context Label = " << anEntry.ToCString() <<endl;
#endif
	  }
      }
#ifdef OCCT_DEBUG
    else
      cout << "Retrieving Context Label is NULL" <<endl;
#endif

    if(XmlMNaming::DocumentVersion() > 4 && XmlMNaming::DocumentVersion() < 7) {
          // Orientation processing - converting from old format
          Handle(TNaming_NamedShape) aNS;
          if (aNg->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
            //const TDF_Label& aLab = aNS->Label();
            TNaming_Iterator itL (aNS);
            for (; itL.More(); itL.Next()) {
              const TopoDS_Shape& S = itL.NewShape();
              if (S.IsNull()) continue;
              if(aNS->Evolution() == TNaming_SELECTED) {
                if (itL.More() && itL.NewShape().ShapeType() != TopAbs_VERTEX &&
                    !itL.OldShape().IsNull() && itL.OldShape().ShapeType() == TopAbs_VERTEX ) {//OR-N
                  TopAbs_Orientation OrientationToApply = itL.OldShape().Orientation();
                  aNgName.Orientation(OrientationToApply);
                }
              }
            }
          }         
        }
    if(XmlMNaming::DocumentVersion() > 6) {
       aDOMStr = anElem.getAttribute(::OrientString());
       if (!aDOMStr.GetInteger(aNb))
       {
         aMsgString = TCollection_ExtendedString
           ("XmlMNaming_NamingDriver: Cannot retrieve "
            "integer value of orientation from \"") + aDOMStr + "\"";
         WriteMessage (aMsgString);
         return Standard_False;
       }
       aNgName.Orientation((TopAbs_Orientation)aNb);
    }
    // or. end
  }
#ifdef OCCT_DEBUG
  else if(XmlMNaming::DocumentVersion() == -1)
    cout << "Current DocVersion field is not initialized. "  <<endl;
  else 
    cout << "Current DocVersion = "  << XmlMNaming::DocumentVersion() <<endl;
#endif
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMNaming_NamingDriver::Paste
                        (const Handle(TDF_Attribute)& theSource,
                         XmlObjMgt_Persistent&        theTarget,
                         XmlObjMgt_SRelocationTable&  theRelocTable) const
{
  Handle(TNaming_Naming) aNg = Handle(TNaming_Naming)::DownCast(theSource);
  XmlObjMgt_Element& anElem = theTarget;
  const TNaming_Name& aNgName = aNg->GetName();

  // type and shape type
  anElem.setAttribute(::TypeString(), NameTypeToString(aNgName.Type()));
  anElem.setAttribute(::ShapeTypeString(), ShapeEnumToString(aNgName.ShapeType()));
  
  Standard_Integer aNb;

  // arguments
  Standard_Integer NbArgs = aNgName.Arguments().Extent();
  if (NbArgs > 0)
  {
    TCollection_AsciiString anArgsStr;
    for (TNaming_ListIteratorOfListOfNamedShape it (aNgName.Arguments()); it.More(); it.Next())
    {
      Handle(TNaming_NamedShape) anArg = it.Value();
      aNb = 0;
      if (!anArg.IsNull())
      {
        aNb = theRelocTable.FindIndex(anArg);
        if (aNb == 0)
        {
          aNb = theRelocTable.Add(anArg);
        }
        anArgsStr += TCollection_AsciiString(aNb) + " ";
      }
      else anArgsStr += "0 ";
    }
    anElem.setAttribute(::ArgumentsString(), anArgsStr.ToCString());
  }

  // stop named shape
  Handle(TNaming_NamedShape) aSNS = aNgName.StopNamedShape();
  if (!aSNS.IsNull())
  {
    aNb = theRelocTable.FindIndex(aSNS);
    if (aNb == 0)
    {
      aNb = theRelocTable.Add(aSNS);
    }
    anElem.setAttribute(::StopNamedShapeString(), aNb);
  }

  // index
  anElem.setAttribute(::IndexString(), aNgName.Index());

  // context label
  TCollection_AsciiString anEntry;
  if(!aNgName.ContextLabel().IsNull())
    TDF_Tool::Entry(aNgName.ContextLabel(), anEntry);  
  XmlObjMgt_DOMString aDOMString;
  XmlObjMgt::SetTagEntryString (aDOMString, anEntry);
  anElem.setAttribute(::ContextLabelString(), aDOMString);
#ifdef OCCT_DEBUG
  cout << "XmlMNaming_NamingDriver::Store: ContextLabel Entry = " << anEntry << endl;
  if (aDOMString != NULL)
  {
    Standard_CString aStr = Standard_CString(aDOMString.GetString());
    cout << "XmlMNaming_NamingDriver::Store: ContextLabel DOMString = " << aStr << endl;
  } else
    cout << "XmlMNaming_NamingDriver::Store: aDOMString is NULL" <<endl;
#endif

  // orientation
  anElem.setAttribute(::OrientString(), (Standard_Integer)aNgName.Orientation());

}

//=======================================================================
//function : ShapeEnumToString
//purpose  : 
//=======================================================================
static const XmlObjMgt_DOMString& ShapeEnumToString (const TopAbs_ShapeEnum theE)
{
  switch (theE)
  {
  case TopAbs_COMPOUND  : return ::ShCompoundString();
  case TopAbs_COMPSOLID : return ::ShCompsolidString();
  case TopAbs_SOLID     : return ::ShSolidString();
  case TopAbs_SHELL     : return ::ShShellString();
  case TopAbs_FACE      : return ::ShFaceString();
  case TopAbs_WIRE      : return ::ShWireString();
  case TopAbs_EDGE      : return ::ShEdgeString();
  case TopAbs_VERTEX    : return ::ShVertexString();
  case TopAbs_SHAPE     : return ::ShShapeString();  
  }
  static XmlObjMgt_DOMString aNullString;
  return aNullString; // To avoid compilation error message.
}

//=======================================================================
//function : NameTypeToString
//purpose  : 
//=======================================================================
static const XmlObjMgt_DOMString& NameTypeToString (const TNaming_NameType theE) 
{
  switch (theE)
  { 
  case TNaming_UNKNOWN             : return ::NTUnknownString();  
  case TNaming_IDENTITY            : return ::NTIdentityString();
  case TNaming_MODIFUNTIL          : return ::NTModifUntilString();
  case TNaming_GENERATION          : return ::NTGenerationString();
  case TNaming_INTERSECTION        : return ::NTIntersectionString();
  case TNaming_UNION               : return ::NTUnionString();
  case TNaming_SUBSTRACTION        : return ::NTSubtractionString();
  case TNaming_CONSTSHAPE          : return ::NTConstShapeString();
  case TNaming_FILTERBYNEIGHBOURGS : return ::NTFilterByNeighString();
  case TNaming_ORIENTATION         : return ::NTOrientationString();
  case TNaming_WIREIN              : return ::NTWireInString();
  case TNaming_SHELLIN             : return ::NTShellInString();
  default:
    throw Standard_DomainError("TNaming_NameType; enum term unknown ");
  }
}

//=======================================================================
//function : ShapeEnumFromString
//purpose  : 
//=======================================================================
static TopAbs_ShapeEnum ShapeEnumFromString (const XmlObjMgt_DOMString& theString)
{
  TopAbs_ShapeEnum aResult = TopAbs_SHAPE;
  if (!theString.equals (::ShShapeString()))
  {
    if (theString.equals (::ShCompoundString()))
      aResult = TopAbs_COMPOUND;
    else if (theString.equals (::ShCompsolidString()))
      aResult = TopAbs_COMPSOLID;
    else if (theString.equals (::ShSolidString()))
      aResult = TopAbs_SOLID;
    else if (theString.equals (::ShShellString()))
      aResult = TopAbs_SHELL;
    else if (theString.equals (::ShFaceString()))
      aResult = TopAbs_FACE;
    else if (theString.equals (::ShWireString()))
      aResult = TopAbs_WIRE;
    else if (theString.equals (::ShEdgeString()))
      aResult = TopAbs_EDGE;
    else if (theString.equals (::ShVertexString()))
      aResult = TopAbs_VERTEX;
    else
      throw Standard_DomainError("TopAbs_ShapeEnum; string value without enum term equivalence");
  }
  return aResult;
}

//=======================================================================
//function : NameTypeFromString
//purpose  : 
//=======================================================================
static TNaming_NameType NameTypeFromString (const XmlObjMgt_DOMString& theString) 
{
  TNaming_NameType aResult = TNaming_UNKNOWN;
  if (!theString.equals (::NTUnknownString()))
  {
    if (theString.equals (::NTIdentityString()))
      aResult = TNaming_IDENTITY;
    else if (theString.equals (::NTModifUntilString()))
      aResult = TNaming_MODIFUNTIL;
    else if (theString.equals (::NTGenerationString()))
      aResult = TNaming_GENERATION;
    else if (theString.equals (::NTIntersectionString()))
      aResult = TNaming_INTERSECTION;
    else if (theString.equals (::NTUnionString()))
      aResult = TNaming_UNION;
    else if (theString.equals (::NTSubtractionString()))
      aResult = TNaming_SUBSTRACTION;
    else if (theString.equals (::NTConstShapeString()))
      aResult = TNaming_CONSTSHAPE;
    else if (theString.equals (::NTFilterByNeighString()))
      aResult = TNaming_FILTERBYNEIGHBOURGS;
    else if (theString.equals (::NTOrientationString()))
      aResult = TNaming_ORIENTATION;
    else if (theString.equals (::NTWireInString()))
      aResult = TNaming_WIREIN;
    else if (theString.equals (::NTShellInString()))
      aResult = TNaming_SHELLIN;
    else
      throw Standard_DomainError("TNaming_NameType; string value without enum term equivalence");
  }
  return aResult;
}
