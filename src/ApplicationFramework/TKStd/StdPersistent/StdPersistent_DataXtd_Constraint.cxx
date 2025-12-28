// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdPersistent_DataXtd_Constraint.hxx>

#include <TNaming_NamedShape.hxx>
#include <TDataStd_Real.hxx>

//=======================================================================
// function : Import
// purpose  : Import transient attribute from the persistent data
//=======================================================================
void StdPersistent_DataXtd_Constraint::Import(const occ::handle<TDataXtd_Constraint>& theAttribute) const
{
  theAttribute->SetType(static_cast<TDataXtd_ConstraintEnum>(myType));
  theAttribute->Reversed(myIsReversed);
  theAttribute->Inverted(myIsInverted);
  theAttribute->Verified(myIsVerified);

  if (myGeometries)
  {
    occ::handle<NCollection_HArray1<occ::handle<StdObjMgt_Persistent>>> aGeometries = myGeometries->Array();
    for (int i = 1; i <= aGeometries->Length(); i++)
    {
      occ::handle<StdObjMgt_Persistent> aPGeometry = aGeometries->Value(i);
      if (aPGeometry)
      {
        occ::handle<TDF_Attribute> aTGeometry = aPGeometry->GetAttribute();
        theAttribute->SetGeometry(i, occ::down_cast<TNaming_NamedShape>(aTGeometry));
      }
    }
  }

  if (myValue)
  {
    occ::handle<TDF_Attribute> aValue = myValue->GetAttribute();
    theAttribute->SetValue(occ::down_cast<TDataStd_Real>(aValue));
  }

  if (myPlane)
  {
    occ::handle<TDF_Attribute> aPlane = myPlane->GetAttribute();
    theAttribute->SetPlane(occ::down_cast<TNaming_NamedShape>(aPlane));
  }
}
