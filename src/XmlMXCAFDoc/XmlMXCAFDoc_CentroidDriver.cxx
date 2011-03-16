// File:      XmlMXCAFDoc_CentroidDriver.cxx
// Created:   04.09.01 14:47:31
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlMXCAFDoc_CentroidDriver.ixx>

#include <XmlObjMgt.hxx>
#include <XCAFDoc_Centroid.hxx>
#include <gp_XYZ.hxx>

#include <stdio.h>

//=======================================================================
//function : XmlMXCAFDoc_CentroidDriver
//purpose  : Constructor
//=======================================================================
XmlMXCAFDoc_CentroidDriver::XmlMXCAFDoc_CentroidDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, "xcaf", "Centroid")
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_CentroidDriver::NewEmpty() const
{
  return (new XCAFDoc_Centroid());
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================
Standard_Boolean XmlMXCAFDoc_CentroidDriver::Paste
                (const XmlObjMgt_Persistent&  theSource,
                 const Handle(TDF_Attribute)& theTarget,
                 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(XCAFDoc_Centroid) aTPos = Handle(XCAFDoc_Centroid)::DownCast(theTarget);

  // position
  XmlObjMgt_DOMString aPosStr = XmlObjMgt::GetStringValue(theSource.Element());
  if (aPosStr == NULL)
  {
    WriteMessage ("Cannot retrieve position string from element");
    return Standard_False;
  }

  gp_Pnt aPos;
  Standard_Real aValue;
  Standard_CString aValueStr = Standard_CString(aPosStr.GetString());

  // X
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve X coordinate for XCAFDoc_Centroid attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetX(aValue);

  // Y
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve Y coordinate for XCAFDoc_Centroid attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetY(aValue);

  // Z
  if (!XmlObjMgt::GetReal(aValueStr, aValue))
  {
    TCollection_ExtendedString aMessageString =
      TCollection_ExtendedString
        ("Cannot retrieve Z coordinate for XCAFDoc_Centroid attribute as \"")
          + aValueStr + "\"";
    WriteMessage (aMessageString);
    return Standard_False;
  }
  aPos.SetZ(aValue);

  aTPos->Set(aPos);

  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================
void XmlMXCAFDoc_CentroidDriver::Paste
                (const Handle(TDF_Attribute)& theSource,
                 XmlObjMgt_Persistent&        theTarget,
                 XmlObjMgt_SRelocationTable&  ) const
{
  Handle(XCAFDoc_Centroid) aTPos = Handle(XCAFDoc_Centroid)::DownCast(theSource);
  if (!aTPos.IsNull())
  {
    gp_Pnt aPos = aTPos->Get();
    char buf [64];
    sprintf (buf, "%.17g %.17g %.17g", aPos.X(), aPos.Y(), aPos.Z());
    XmlObjMgt::SetStringValue(theTarget.Element(), buf);
  }
}
