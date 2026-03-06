// Created on: 2007-10-30
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

#include <TDataStd_DeltaOnModificationOfRealArray.hxx>

#include <Standard_Type.hxx>
#include <TDataStd_DeltaOnModificationOfArray.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_DeltaOnModificationOfRealArray, TDF_DeltaOnModification)

//=================================================================================================

TDataStd_DeltaOnModificationOfRealArray::TDataStd_DeltaOnModificationOfRealArray(
  const occ::handle<TDataStd_RealArray>& OldAtt)
    : TDF_DeltaOnModification(OldAtt),
      myUp1(0),
      myUp2(0)
{
  occ::handle<TDataStd_RealArray> CurrAtt;
  if (Label().FindAttribute(OldAtt->ID(), CurrAtt))
  {
    TDataStd_DeltaOnModificationOfArray::ComputeDelta(OldAtt->Array(),
                                                      CurrAtt->Array(),
                                                      myIndxes,
                                                      myValues,
                                                      myUp1,
                                                      myUp2);
    OldAtt->RemoveArray();
  }
}

//=================================================================================================

void TDataStd_DeltaOnModificationOfRealArray::Apply()
{
  occ::handle<TDF_Attribute>      TDFAttribute = Attribute();
  occ::handle<TDataStd_RealArray> BackAtt      = occ::down_cast<TDataStd_RealArray>(TDFAttribute);
  if (BackAtt.IsNull())
    return;

  occ::handle<TDataStd_RealArray> aCurAtt;
  if (!Label().FindAttribute(BackAtt->ID(), aCurAtt))
    Label().AddAttribute(BackAtt);

  if (aCurAtt.IsNull())
    return;

  aCurAtt->Backup();

  occ::handle<NCollection_HArray1<double>> aNewArr =
    TDataStd_DeltaOnModificationOfArray::ApplyDelta(aCurAtt->Array(),
                                                    myIndxes,
                                                    myValues,
                                                    myUp1,
                                                    myUp2);
  if (!aNewArr.IsNull())
    aCurAtt->myValue = aNewArr;
}
