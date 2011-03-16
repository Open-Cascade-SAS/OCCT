// File:        BinMDataStd_VariableDriver.cxx
// Created:     Fri Aug 24 20:46:58 2001
// Author:      Alexnder GRIGORIEV
// Copyright:   Open Cascade 2001
// History:

#include <BinMDataStd_VariableDriver.ixx>
#include <TDataStd_Variable.hxx>
#include <TCollection_AsciiString.hxx>

//=======================================================================
//function : BinMDataStd_VariableDriver
//purpose  : Constructor
//=======================================================================
BinMDataStd_VariableDriver::BinMDataStd_VariableDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataStd_VariableDriver::NewEmpty() const
{
  return (new TDataStd_Variable());
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMDataStd_VariableDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theTarget);

  Standard_Boolean isConstant;
  if (! (theSource >> isConstant))
    return Standard_False;
  aV->Constant (isConstant);

  TCollection_AsciiString anStr;
  if (! (theSource >> anStr))
    return Standard_False;
  aV->Unit(anStr);
  
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMDataStd_VariableDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataStd_Variable) aV = Handle(TDataStd_Variable)::DownCast(theSource);
  theTarget << aV->IsConstant() << aV->Unit();
}
