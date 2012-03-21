// Created on: 2004-05-17
// Created by: Sergey ZARITCHNY
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
