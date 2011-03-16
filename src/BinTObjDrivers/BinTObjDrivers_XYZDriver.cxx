// File      : BinTObjDrivers_XYZDriver.cxx
// Created   : Wed Nov 24 11:23:54 2004
// Author    : Michael SAZONOV
// Copyright:   Open CASCADE  2007
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
