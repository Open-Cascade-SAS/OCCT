// Created on: 1996-07-02
// Created by: Joelle CHAUVET
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// Modified:	Mon Dec  9 11:39:13 1996
//   by:	Joelle CHAUVET
//		G1135 : empty constructor

#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Patch.hxx>
#include <NCollection_Sequence.hxx>

//=================================================================================================

AdvApp2Var_Network::AdvApp2Var_Network() = default;

//=================================================================================================

AdvApp2Var_Network::AdvApp2Var_Network(
  const NCollection_Sequence<occ::handle<AdvApp2Var_Patch>>& Net,
  const NCollection_Sequence<double>&                        TheU,
  const NCollection_Sequence<double>&                        TheV)
{
  myNet         = Net;
  myUParameters = TheU;
  myVParameters = TheV;
}

//=================================================================================================

bool AdvApp2Var_Network::FirstNotApprox(int& theIndex) const
{
  int anIndex = 1;
  for (NCollection_Sequence<occ::handle<AdvApp2Var_Patch>>::Iterator aPatchIter(myNet);
       aPatchIter.More();
       aPatchIter.Next(), ++anIndex)
  {
    const occ::handle<AdvApp2Var_Patch>& aPatch = aPatchIter.Value();
    if (!aPatch->IsApproximated())
    {
      theIndex = anIndex;
      return true;
    }
  }
  return false;
}

//=================================================================================================

void AdvApp2Var_Network::UpdateInU(const double CuttingValue)
{
  // Insert the new cutting parameter.
  int i = 1;
  while (myUParameters.Value(i) < CuttingValue)
  {
    i++;
  }
  myUParameters.InsertBefore(i, CuttingValue);

  const int aNbPatchInU = myUParameters.Length() - 1;
  for (int j = 1; j < myVParameters.Length(); j++)
  {
    // Modify the patch impacted by the cut.
    const int                            aPatchIndex = aNbPatchInU * (j - 1) + i - 1;
    const occ::handle<AdvApp2Var_Patch>& aPat        = myNet.Value(aPatchIndex);
    aPat->ChangeDomain(aPat->U0(), CuttingValue, aPat->V0(), aPat->V1());
    aPat->ResetApprox();

    // Insert the right-side patch.
    occ::handle<AdvApp2Var_Patch> aNewPat = new AdvApp2Var_Patch(CuttingValue,
                                                                 myUParameters.Value(i + 1),
                                                                 myVParameters.Value(j),
                                                                 myVParameters.Value(j + 1),
                                                                 aPat->UOrder(),
                                                                 aPat->VOrder());
    aNewPat->ResetApprox();
    myNet.InsertAfter(aPatchIndex, aNewPat);
  }
}

//=================================================================================================

void AdvApp2Var_Network::UpdateInV(const double CuttingValue)
{
  // Insert the new cutting parameter.
  int j = 1;
  while (myVParameters.Value(j) < CuttingValue)
  {
    j++;
  }
  myVParameters.InsertBefore(j, CuttingValue);

  const int aNbPatchInU = myUParameters.Length() - 1;

  // Modify the patches affected by the cut.
  for (int i = 1; i <= aNbPatchInU; i++)
  {
    const int                            aPatchIndex = aNbPatchInU * (j - 2) + i;
    const occ::handle<AdvApp2Var_Patch>& aPatch      = myNet.Value(aPatchIndex);
    aPatch->ChangeDomain(aPatch->U0(), aPatch->U1(), aPatch->V0(), CuttingValue);
    aPatch->ResetApprox();
  }

  // Insert the top patches.
  for (int i = 1; i <= aNbPatchInU; i++)
  {
    const int                            aPatchIndex  = aNbPatchInU * (j - 1) + i - 1;
    const occ::handle<AdvApp2Var_Patch>& aSourcePatch = myNet.Value(aNbPatchInU * (j - 2) + i);
    occ::handle<AdvApp2Var_Patch>        aNewPat      = new AdvApp2Var_Patch(myUParameters.Value(i),
                                                                 myUParameters.Value(i + 1),
                                                                 CuttingValue,
                                                                 myVParameters.Value(j + 1),
                                                                 aSourcePatch->UOrder(),
                                                                 aSourcePatch->VOrder());
    aNewPat->ResetApprox();
    myNet.InsertAfter(aPatchIndex, aNewPat);
  }
}

//=================================================================================================

void AdvApp2Var_Network::SameDegree(const int iu, const int iv, int& ncfu, int& ncfv)
{
  //  Compute the max coefficients, initialized according to the continuity order
  ncfu = 2 * iu + 2;
  ncfv = 2 * iv + 2;
  for (NCollection_Sequence<occ::handle<AdvApp2Var_Patch>>::Iterator aPatIter(myNet);
       aPatIter.More();
       aPatIter.Next())
  {
    const occ::handle<AdvApp2Var_Patch>& aPat = aPatIter.Value();
    ncfu                                      = std::max(ncfu, aPat->NbCoeffInU());
    ncfv                                      = std::max(ncfv, aPat->NbCoeffInV());
  }

  //  Increase the number of coefficients
  for (NCollection_Sequence<occ::handle<AdvApp2Var_Patch>>::Iterator aPatIter(myNet);
       aPatIter.More();
       aPatIter.Next())
  {
    const occ::handle<AdvApp2Var_Patch>& aPat = aPatIter.Value();
    aPat->ChangeNbCoeff(ncfu, ncfv);
  }
}

//=================================================================================================

int AdvApp2Var_Network::NbPatch() const
{
  return myNet.Length();
}

//=================================================================================================

int AdvApp2Var_Network::NbPatchInU() const
{
  return myUParameters.Length() - 1;
}

//=================================================================================================

int AdvApp2Var_Network::NbPatchInV() const
{
  return myVParameters.Length() - 1;
}

//=================================================================================================

double AdvApp2Var_Network::UParameter(const int Index) const
{
  return myUParameters.Value(Index);
}

//=================================================================================================

double AdvApp2Var_Network::VParameter(const int Index) const
{
  return myVParameters.Value(Index);
}
