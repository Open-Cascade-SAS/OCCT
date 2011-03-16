// File:	XmlMDataStd_AsciiStringDriver.cxx
// Created:	Tue Aug 21 16:35:28 2007
// Author:	Sergey ZARITCHNY
//		<sergey.zaritchny@opencascade.com>
// Copyright:   Open CASCADE SA 2007

#include <XmlMDataStd_AsciiStringDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <TDataStd_AsciiString.hxx>

//=======================================================================
//function : XmlMDataStd_AsciiStringDriver
//purpose  : Constructor
//=======================================================================

XmlMDataStd_AsciiStringDriver::XmlMDataStd_AsciiStringDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty()
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_AsciiStringDriver::NewEmpty () const
{
  return (new TDataStd_AsciiString());
}

//=======================================================================
//function : Paste()
//purpose  : retrieve
//=======================================================================
Standard_Boolean XmlMDataStd_AsciiStringDriver::Paste
                               (const XmlObjMgt_Persistent&  theSource,
                                const Handle(TDF_Attribute)& theTarget,
                                XmlObjMgt_RRelocationTable&  ) const
{
  if(!theTarget.IsNull()) {
    const TCollection_AsciiString aString = XmlObjMgt::GetStringValue (theSource);
    Handle(TDataStd_AsciiString)::DownCast(theTarget) -> Set (aString);
    return Standard_True;
  }
  WriteMessage("error retrieving AsciiString for type TDataStd_AsciiString");
  return Standard_False;
}

//=======================================================================
//function : Paste()
//purpose  : store
//=======================================================================
void XmlMDataStd_AsciiStringDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_AsciiString) aS = Handle(TDataStd_AsciiString)::DownCast(theSource);
  if (aS.IsNull()) return;
  XmlObjMgt_DOMString aString = aS->Get().ToCString();
  XmlObjMgt::SetStringValue (theTarget, aString);
}
