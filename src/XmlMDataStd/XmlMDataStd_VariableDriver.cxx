// File:        XmlMDataStd_VariableDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <XmlMDataStd_VariableDriver.ixx>
#include <TDataStd_Variable.hxx>

IMPLEMENT_DOMSTRING (IsConstString, "isconst")
IMPLEMENT_DOMSTRING (UnitString,    "unit")
IMPLEMENT_DOMSTRING (ConstString,   "true")

//=======================================================================
//function : XmlMDataStd_VariableDriver
//purpose  : Constructor
//=======================================================================
XmlMDataStd_VariableDriver::XmlMDataStd_VariableDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : XmlMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMDataStd_VariableDriver::NewEmpty() const
{
  return (new TDataStd_Variable());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean XmlMDataStd_VariableDriver::Paste
                                (const XmlObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 XmlObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theTarget);
  
  XmlObjMgt_DOMString aStr =
    theSource.Element().getAttribute(::IsConstString());
  aV->Constant (aStr != NULL);

  aStr = theSource.Element().getAttribute(::UnitString());
  aV->Unit(aStr);
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XmlMDataStd_VariableDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        XmlObjMgt_Persistent&        theTarget,
                                        XmlObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theSource);
  if (aV->IsConstant())
    theTarget.Element().setAttribute (::IsConstString(), ::ConstString());
  theTarget.Element().setAttribute(::UnitString(), aV->Unit().ToCString());
}
