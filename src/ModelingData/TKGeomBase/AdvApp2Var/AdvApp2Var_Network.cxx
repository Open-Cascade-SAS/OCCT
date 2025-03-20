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
#include <AdvApp2Var_SequenceOfPatch.hxx>
#include <TColStd_SequenceOfReal.hxx>

//=================================================================================================

AdvApp2Var_Network::AdvApp2Var_Network() {}

//=================================================================================================

AdvApp2Var_Network::AdvApp2Var_Network(const AdvApp2Var_SequenceOfPatch& Net,
                                       const TColStd_SequenceOfReal&     TheU,
                                       const TColStd_SequenceOfReal&     TheV)
{
  myNet         = Net;
  myUParameters = TheU;
  myVParameters = TheV;
}

//==========================================================================================
// function : FirstNotApprox
// purpose  : return the first Patch not approximated
//==========================================================================================

Standard_Boolean AdvApp2Var_Network::FirstNotApprox(Standard_Integer& theIndex) const
{
  Standard_Integer anIndex = 1;
  for (AdvApp2Var_SequenceOfPatch::Iterator aPatchIter(myNet); aPatchIter.More();
       aPatchIter.Next(), ++anIndex)
  {
    const Handle(AdvApp2Var_Patch)& aPatch = aPatchIter.Value();
    if (!aPatch->IsApproximated())
    {
      theIndex = anIndex;
      return true;
    }
  }
  return false;
}

//==========================================================================================
// function : UpdateInU
// purpose  : modification and insertion of patches and parameters
//==========================================================================================

void AdvApp2Var_Network::UpdateInU(const Standard_Real CuttingValue)
{

  //  insertion du nouveau parametre de decoupe
  Standard_Integer i = 1, j;
  while (myUParameters.Value(i) < CuttingValue)
  {
    i++;
  }
  myUParameters.InsertBefore(i, CuttingValue);

  for (j = 1; j < myVParameters.Length(); j++)
  {
    //    modification des patches concernes par la decoupe
    Standard_Integer                indice = (myUParameters.Length() - 1) * (j - 1) + i - 1;
    const Handle(AdvApp2Var_Patch)& aPat   = myNet.Value(indice);
    aPat->ChangeDomain(aPat->U0(), CuttingValue, aPat->V0(), aPat->V1());
    aPat->ResetApprox();

    //    insertion des nouveaux patches
    Handle(AdvApp2Var_Patch) aNewPat = new AdvApp2Var_Patch(CuttingValue,
                                                            myUParameters.Value(i + 1),
                                                            myVParameters.Value(j),
                                                            myVParameters.Value(j + 1),
                                                            aPat->UOrder(),
                                                            aPat->VOrder());
    aNewPat->ResetApprox();
    myNet.InsertAfter(indice, aNewPat);
  }
}

//==========================================================================================
// function : UpdateInV
// purpose  : modification and insertion of patches and parameters
//==========================================================================================

void AdvApp2Var_Network::UpdateInV(const Standard_Real CuttingValue)
{

  //  insertion du nouveau parametre de decoupe
  Standard_Integer         j = 1;
  Handle(AdvApp2Var_Patch) Pat;
  while (myVParameters.Value(j) < CuttingValue)
  {
    j++;
  }
  myVParameters.InsertBefore(j, CuttingValue);

  //  modification des patches concernes par la decoupe
  for (Standard_Integer i = 1; i < myUParameters.Length(); i++)
  {
    const Standard_Integer indice = (myUParameters.Length() - 1) * (j - 2) + i;
    Pat                           = myNet.Value(indice);
    Pat->ChangeDomain(Pat->U0(), Pat->U1(), Pat->V0(), CuttingValue);
    Pat->ResetApprox();
  }

  //  insertion des nouveaux patches
  for (Standard_Integer i = 1; i < myUParameters.Length(); i++)
  {
    const Standard_Integer   indice  = (myUParameters.Length() - 1) * (j - 1) + i - 1;
    Handle(AdvApp2Var_Patch) aNewPat = new AdvApp2Var_Patch(myUParameters.Value(i),
                                                            myUParameters.Value(i + 1),
                                                            CuttingValue,
                                                            myVParameters.Value(j + 1),
                                                            Pat->UOrder(),
                                                            Pat->VOrder());
    aNewPat->ResetApprox();
    myNet.InsertAfter(indice, aNewPat);
  }
}

//=======================================================================
// function : SameDegree
// purpose  : same numbers of coefficients for all patches
//=======================================================================

void AdvApp2Var_Network::SameDegree(const Standard_Integer iu,
                                    const Standard_Integer iv,
                                    Standard_Integer&      ncfu,
                                    Standard_Integer&      ncfv)
{
  //  calcul des coeff. max avec init selon l'ordre de continuite
  ncfu = 2 * iu + 2;
  ncfv = 2 * iv + 2;
  for (AdvApp2Var_SequenceOfPatch::Iterator aPatIter(myNet); aPatIter.More(); aPatIter.Next())
  {
    const Handle(AdvApp2Var_Patch)& aPat = aPatIter.Value();
    ncfu                                 = Max(ncfu, aPat->NbCoeffInU());
    ncfv                                 = Max(ncfv, aPat->NbCoeffInV());
  }

  //  augmentation des nombres de coeff.
  for (AdvApp2Var_SequenceOfPatch::Iterator aPatIter(myNet); aPatIter.More(); aPatIter.Next())
  {
    const Handle(AdvApp2Var_Patch)& aPat = aPatIter.Value();
    aPat->ChangeNbCoeff(ncfu, ncfv);
  }
}

//=================================================================================================

Standard_Integer AdvApp2Var_Network::NbPatch() const
{
  return myNet.Length();
}

//=================================================================================================

Standard_Integer AdvApp2Var_Network::NbPatchInU() const
{
  return myUParameters.Length() - 1;
}

//=================================================================================================

Standard_Integer AdvApp2Var_Network::NbPatchInV() const
{
  return myVParameters.Length() - 1;
}

//=================================================================================================

Standard_Real AdvApp2Var_Network::UParameter(const Standard_Integer Index) const
{
  return myUParameters.Value(Index);
}

//=================================================================================================

Standard_Real AdvApp2Var_Network::VParameter(const Standard_Integer Index) const
{
  return myVParameters.Value(Index);
}
