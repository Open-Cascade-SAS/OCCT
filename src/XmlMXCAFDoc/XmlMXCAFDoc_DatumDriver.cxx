// File:      XmlMXCAFDoc_DatumDriver.cxx
// Created:   10.12.08 15:04:13
// Author:    Pavel TELKOV
// Copyright: Open CASCADE 2008

#include <XmlMXCAFDoc_DatumDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XCAFDoc_Datum.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfReal.hxx>

IMPLEMENT_DOMSTRING (DescrIndexString, "descr")
IMPLEMENT_DOMSTRING (IdIndexString,    "ident")

//=======================================================================
//function : XmlMXCAFDoc_DatumDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_DatumDriver::XmlMXCAFDoc_DatumDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "Datum")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_DatumDriver::NewEmpty() const
{
  return (new XCAFDoc_Datum());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_DatumDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  XmlObjMgt_DOMString aNameStr = XmlObjMgt::GetStringValue(theSource);

  if (aNameStr == NULL) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Datum attribute");
    WriteMessage (aMessageString);
    return Standard_False;
  }
  
  const XmlObjMgt_Element& anElement = theSource;

  XmlObjMgt_DOMString aDescrStr = anElement.getAttribute(::DescrIndexString());
  XmlObjMgt_DOMString anIdStr = anElement.getAttribute(::IdIndexString());
  if ( aDescrStr == NULL || anIdStr == NULL ) {
    TCollection_ExtendedString aMessageString
      ("Cannot retrieve Datum attribute description or identification");
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(TCollection_HAsciiString) aName =
    new TCollection_HAsciiString( aNameStr.GetString() );
  Handle(TCollection_HAsciiString) aDescr =
    new TCollection_HAsciiString( aDescrStr.GetString() );
  Handle(TCollection_HAsciiString) anId =
    new TCollection_HAsciiString( anIdStr.GetString() );
  
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theTarget);
  anAtt->Set(aName, aDescr, anId);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_DatumDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Datum) anAtt = Handle(XCAFDoc_Datum)::DownCast(theSource);
  
  XmlObjMgt_DOMString aNameString, aDescrString, anIdString;
  if ( !anAtt->GetName().IsNull() )
    aNameString = anAtt->GetName()->String().ToCString();
  if ( !anAtt->GetDescription().IsNull() )
    aDescrString = anAtt->GetDescription()->String().ToCString();
  if ( !anAtt->GetIdentification().IsNull() )
    anIdString = anAtt->GetIdentification()->String().ToCString();

  XmlObjMgt::SetStringValue (theTarget, aNameString);
  theTarget.Element().setAttribute(::DescrIndexString(),aDescrString);
  theTarget.Element().setAttribute(::IdIndexString(), anIdString);
}
