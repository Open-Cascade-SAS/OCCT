// File:	BinMPrsStd_PositionDriver.cxx
// Created:	Mon May 17 11:36:11 2004
// Author:	Sergey ZARITCHNY <szy@opencascade.com>
// Copyright:	Open CasCade S.A. 2004
// modified     13.04.2009 Sergey ZARITCHNY

#include <BinMPrsStd_PositionDriver.ixx>
#include <gp_Pnt.hxx>
#include <TDataXtd_Position.hxx>
#include <CDM_MessageDriver.hxx>

//=======================================================================
//function : BinMDataStd_PositionDriver
//purpose  : Constructor
//=======================================================================

BinMPrsStd_PositionDriver::BinMPrsStd_PositionDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
     : BinMDF_ADriver (theMsgDriver, STANDARD_TYPE(TDataXtd_Position)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) BinMPrsStd_PositionDriver::NewEmpty() const
{
  return new TDataXtd_Position();
}

//=======================================================================
//function : Paste
//purpose  : persistent -> transient (retrieve)
//=======================================================================

Standard_Boolean BinMPrsStd_PositionDriver::Paste
                                (const BinObjMgt_Persistent&  theSource,
                                 const Handle(TDF_Attribute)& theTarget,
                                 BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Position) anAtt = Handle(TDataXtd_Position)::DownCast(theTarget);
  Standard_Real aValue;
  Standard_Boolean ok = theSource >> aValue;
  if (!ok) return ok;
  gp_Pnt aPosition(0., 0., 0.);
  aPosition.SetX(aValue);

  ok = theSource >> aValue;
  if (!ok) return ok;
  aPosition.SetY(aValue);

  ok = theSource >> aValue;
  if (!ok) return ok;
  aPosition.SetZ(aValue);

  anAtt->SetPosition(aPosition);

  return ok;
}

//=======================================================================
//function : Paste
//purpose  : transient -> persistent (store)
//=======================================================================

void BinMPrsStd_PositionDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                       BinObjMgt_Persistent&        theTarget,
                                       BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Position) anAtt = Handle(TDataXtd_Position)::DownCast(theSource);
  
  theTarget << anAtt->GetPosition().X();
  theTarget << anAtt->GetPosition().Y();
  theTarget << anAtt->GetPosition().Z();
}
