// File:        BinMDataStd_UAttributeDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <BinMDataStd_UAttributeDriver.ixx>
#include <TDataStd_UAttribute.hxx>

//=======================================================================
//function : BinMDataStd_UAttributeDriver
//purpose  : Constructor
//=======================================================================

BinMDataStd_UAttributeDriver::BinMDataStd_UAttributeDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_UAttributeDriver::NewEmpty() const
{
  return (new TDataStd_UAttribute());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_UAttributeDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_UAttribute) anUAttr =
    Handle(TDataStd_UAttribute)::DownCast (theTarget);
  Standard_GUID aGUID;
  Standard_Boolean ok = theSource >> aGUID;
  if (ok)
    anUAttr -> SetID (aGUID);
  return ok;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_UAttributeDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                         BinObjMgt_Persistent&        theTarget,
                                         BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_UAttribute) anUAttr =
    Handle(TDataStd_UAttribute)::DownCast(theSource);
  theTarget << anUAttr->ID();
}
