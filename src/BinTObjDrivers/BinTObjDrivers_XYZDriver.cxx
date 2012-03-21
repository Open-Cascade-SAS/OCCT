// Created on: 2004-11-24
// Created by: Michael SAZONOV
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

// The original implementation Copyright: (C) RINA S.p.A


#include <BinTObjDrivers_XYZDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <TDF_Attribute.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <TObj_TXYZ.hxx>

IMPLEMENT_STANDARD_HANDLE(BinTObjDrivers_XYZDriver,BinMDF_ADriver)
IMPLEMENT_STANDARD_RTTIEXT(BinTObjDrivers_XYZDriver,BinMDF_ADriver)

//=======================================================================
//function : BinTObjDrivers_XYZDriver
//purpose  : constructor
//=======================================================================

BinTObjDrivers_XYZDriver::BinTObjDrivers_XYZDriver
                         (const Handle(CDM_MessageDriver)& theMessageDriver)
: BinMDF_ADriver( theMessageDriver, NULL)
{
}

//=======================================================================
//function : NewEmpty
//purpose  : Creates a new attribute
//=======================================================================

Handle(TDF_Attribute) BinTObjDrivers_XYZDriver::NewEmpty() const
{
  return new TObj_TXYZ;
}

//=======================================================================
//function : Paste
//purpose  : Retrieve. Translate the contents of <theSource> and put it
//           into <theTarget>.
//=======================================================================

Standard_Boolean BinTObjDrivers_XYZDriver::Paste
                         (const BinObjMgt_Persistent&  theSource,
                          const Handle(TDF_Attribute)& theTarget,
                          BinObjMgt_RRelocationTable&) const
{
  Handle(TObj_TXYZ) aTarget = Handle(TObj_TXYZ)::DownCast(theTarget);
  Standard_Real aX, aY, aZ;
  if (! (theSource >> aX >> aY >> aZ))
    return Standard_False;
  aTarget->Set (gp_XYZ (aX, aY, aZ));
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : Store. Translate the contents of <theSource> and put it
//           into <theTarget>
//=======================================================================

void BinTObjDrivers_XYZDriver::Paste
                         (const Handle(TDF_Attribute)& theSource,
                          BinObjMgt_Persistent&        theTarget,
                          BinObjMgt_SRelocationTable&) const
{
  Handle(TObj_TXYZ) aSource = Handle(TObj_TXYZ)::DownCast (theSource);
  gp_XYZ aXYZ = aSource->Get();
  theTarget << aXYZ.X() << aXYZ.Y() << aXYZ.Z();
}
