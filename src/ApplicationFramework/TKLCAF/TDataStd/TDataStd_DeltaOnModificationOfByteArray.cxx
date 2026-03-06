// Created on: 2007-12-05
// Created by: Sergey ZARITCHNY
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <TDataStd_DeltaOnModificationOfByteArray.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfArray.hxx>
#include <TDataStd_ByteArray.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfByteArray, TDF_DeltaOnModification)

//=================================================================================================

TDataStd_DeltaOnModificationOfByteArray::TDataStd_DeltaOnModificationOfByteArray(
  const occ::handle<TDataStd_ByteArray>& OldAtt)
    : TDF_DeltaOnModification(OldAtt),
      myUp1(0),
      myUp2(0)
{
  occ::handle<TDataStd_ByteArray> CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(), CurrAtt))
  {
    TDataStd_DeltaOnModificationOfArray::ComputeDelta(OldAtt->InternalArray(),
                                                      CurrAtt->InternalArray(),
                                                      myIndxes,
                                                      myValues,
                                                      myUp1,
                                                      myUp2);
    OldAtt->RemoveArray();
  }
}

//=================================================================================================

void TDataStd_DeltaOnModificationOfByteArray::Apply()
{
  occ::handle<TDF_Attribute>      TDFAttribute = Attribute();
  occ::handle<TDataStd_ByteArray> BackAtt      = occ::down_cast<TDataStd_ByteArray>(TDFAttribute);
  if (BackAtt.IsNull())
    return;

  occ::handle<TDataStd_ByteArray> aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(), aCurAtt))
    Label().AddAttribute(BackAtt);

  if (aCurAtt.IsNull())
    return;

  aCurAtt->Backup();

  occ::handle<NCollection_HArray1<uint8_t>> aNewArr =
    TDataStd_DeltaOnModificationOfArray::ApplyDelta(aCurAtt->InternalArray(),
                                                    myIndxes,
                                                    myValues,
                                                    myUp1,
                                                    myUp2);
  if (!aNewArr.IsNull())
    aCurAtt->myValue = aNewArr;
}
