// File:        XmlMXCAFDoc_AreaDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMXCAFDoc_AreaDriver.ixx>
#include <XCAFDoc_Area.hxx>
#include <XmlObjMgt.hxx>

//=======================================================================
//function : XmlMXCAFDoc_AreaDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_AreaDriver::XmlMXCAFDoc_AreaDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Area")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_AreaDriver::NewEmpty() const
{
  return (new XCAFDoc_Area());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_AreaDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aValue;
  XmlObjMgt_DOMString aRealStr = XmlObjMgt::GetStringValue (theSource);

  if (XmlObjMgt::GetReal(aRealStr, aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Area attribute from \"")
        + aRealStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(XCAFDoc_Area) anInt = Handle(XCAFDoc_Area)::DownCast(theTarget);
  anInt->Set(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_AreaDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Area) anInt = Handle(XCAFDoc_Area)::DownCast(theSource);
  TCollection_AsciiString aValueStr (anInt->Get());
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString());
}
