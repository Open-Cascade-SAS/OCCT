// File:        XmlMXCAFDoc_VolumeDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMXCAFDoc_VolumeDriver.ixx>
#include <XCAFDoc_Volume.hxx>
#include <XmlObjMgt.hxx>

//=======================================================================
//function : XmlMXCAFDoc_VolumeDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_VolumeDriver::XmlMXCAFDoc_VolumeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Volume")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_VolumeDriver::NewEmpty() const
{
  return (new XCAFDoc_Volume());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_VolumeDriver::Paste
                                        (const XmlObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         XmlObjMgt_RRelocationTable&  ) const
{
  Standard_Real aValue;
  XmlObjMgt_DOMString aRealStr = XmlObjMgt::GetStringValue (theSource);

  if (XmlObjMgt::GetReal(aRealStr, aValue) == Standard_False) {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString("Cannot retrieve Volume attribute from \"")
        + aRealStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }

  Handle(XCAFDoc_Volume) anInt = Handle(XCAFDoc_Volume)::DownCast(theTarget);
  anInt->Set(aValue);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_VolumeDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                    XmlObjMgt_Persistent&        theTarget,
                                    XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Volume) anInt = Handle(XCAFDoc_Volume)::DownCast(theSource);
  TCollection_AsciiString aValueStr (anInt->Get());
  XmlObjMgt::SetStringValue (theTarget, aValueStr.ToCString());
}
