// Created on: 2017-02-16
// Created by: Eugeny NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <BinMXCAFDoc_AssemblyItemRefDriver.hxx>
#include <XCAFDoc_AssemblyItemRef.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_AssemblyItemRefDriver, BinMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_AssemblyItemRefDriver::BinMXCAFDoc_AssemblyItemRefDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_AssemblyItemRef)->Name())
{

}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_AssemblyItemRefDriver::NewEmpty() const
{
  return new XCAFDoc_AssemblyItemRef();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_AssemblyItemRefDriver::Paste(const BinObjMgt_Persistent&  theSource,
                                                          const Handle(TDF_Attribute)& theTarget,
                                                          BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  Handle(XCAFDoc_AssemblyItemRef) aThis = Handle(XCAFDoc_AssemblyItemRef)::DownCast(theTarget);
  if (aThis.IsNull())
    return Standard_False;
  
  TCollection_AsciiString aPathStr;
  if (!(theSource >> aPathStr))
    return Standard_False;

  aThis->SetItem(aPathStr);

  Standard_Integer anExtraRef = 0;
  if (!(theSource >> anExtraRef))
    return Standard_False;

  if (anExtraRef == 1)
  {
    Standard_GUID aGUID;
    if (!(theSource >> aGUID))
      return Standard_False;

    aThis->SetGUID(aGUID);
  }
  else if (anExtraRef == 2)
  {
    Standard_Integer aSubshapeIndex;
    if (!(theSource >> aSubshapeIndex))
      return Standard_False;

    aThis->SetSubshapeIndex(aSubshapeIndex);
  }

  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_AssemblyItemRefDriver::Paste(const Handle(TDF_Attribute)& theSource,
					                                    BinObjMgt_Persistent&        theTarget,
					                                    BinObjMgt_SRelocationTable&  /*theRelocTable*/) const
{
  Handle(XCAFDoc_AssemblyItemRef) aThis = Handle(XCAFDoc_AssemblyItemRef)::DownCast(theSource);
  if (!aThis.IsNull())
  {
    theTarget << aThis->GetItem().ToString();
    if (aThis->IsGUID())
    {
      theTarget << Standard_Integer(1);
      theTarget << aThis->GetGUID();
    }
    else if (aThis->IsSubshapeIndex())
    {
      theTarget << Standard_Integer(2);
      theTarget << aThis->GetSubshapeIndex();
    }
    else
      theTarget << Standard_Integer(0);
  }
}
