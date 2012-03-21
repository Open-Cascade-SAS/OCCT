// Created on: 2001-08-24
// Created by: Alexnder GRIGORIEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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

// modified     13.04.2009 Sergey Zaritchny

#include <BinMDataXtd_GeometryDriver.ixx>
#include <TDataXtd_Geometry.hxx>

//=======================================================================
//function : BinMDataXtd_GeometryDriver
//purpose  : Constructor
//=======================================================================
BinMDataXtd_GeometryDriver::BinMDataXtd_GeometryDriver
                        (const Handle(CDM_MessageDriver)& theMsgDriver)
      : BinMDF_ADriver (theMsgDriver, NULL)
{}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMDataXtd_GeometryDriver::NewEmpty() const
{
  return (new TDataXtd_Geometry());
}

//=======================================================================
//function : Paste
//purpose  : P -> T
//=======================================================================
Standard_Boolean BinMDataXtd_GeometryDriver::Paste
                                        (const BinObjMgt_Persistent&  theSource,
                                         const Handle(TDF_Attribute)& theTarget,
                                         BinObjMgt_RRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aT = 
    Handle(TDataXtd_Geometry)::DownCast (theTarget);

  Standard_Integer aType;
  Standard_Boolean ok = theSource >> aType;
  if (ok)
    aT->SetType ((TDataXtd_GeometryEnum) aType);

  return ok;
}

//=======================================================================
//function : Paste
//purpose  : T -> P
//=======================================================================
void BinMDataXtd_GeometryDriver::Paste (const Handle(TDF_Attribute)& theSource,
                                        BinObjMgt_Persistent&        theTarget,
                                        BinObjMgt_SRelocationTable&  ) const
{
  Handle(TDataXtd_Geometry) aG = Handle(TDataXtd_Geometry)::DownCast(theSource);
  theTarget << (Standard_Integer) aG->GetType();
}
