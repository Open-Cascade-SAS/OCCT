// File:        XmlMXCAFDoc_ColorDriver.cxx
// Created:     11.09.01 09:29:57
// Author:      Julia DOROVSKIKH
// Copyright:   Open Cascade 2001
// History:

#include <XmlMXCAFDoc_ColorDriver.ixx>
#include <XmlObjMgt.hxx>
#include <XCAFDoc_Color.hxx>

//=======================================================================
//function : XmlMXCAFDoc_ColorDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_ColorDriver::XmlMXCAFDoc_ColorDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Color")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_ColorDriver::NewEmpty() const
{
  return (new XCAFDoc_Color());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_ColorDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Integer aValue;
  XmlObjMgt_DOMString anIntStr = XmlObjMgt::GetStringValue(theSource);

  if (anIntStr.GetInteger(aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Color attribute from \"")
        + anIntStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(XCAFDoc_Color) anInt = Handle(XCAFDoc_Color)::DownCast(theTarget);
  anInt->Set((Quantity_NameOfColor)aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_ColorDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       XmlObjMgt_Persistent&        theTarget,
                                       XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Color) anInt = Handle(XCAFDoc_Color)::DownCast(theSource);
  XmlObjMgt::SetStringValue (theTarget, (Standard_Integer)anInt->GetNOC());
}
