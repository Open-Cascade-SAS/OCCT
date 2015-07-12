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
#include <Standard_NoSuchObject.hxx>
#include <TColStd_SequenceOfReal.hxx>

//==========================================================================================
//function : AdvApp2Var_Network
//purpose  : 
//==========================================================================================
AdvApp2Var_Network::AdvApp2Var_Network()
{
}


//==========================================================================================
//function : AdvApp2Var_Network
//purpose  : 
//==========================================================================================

AdvApp2Var_Network::AdvApp2Var_Network(const AdvApp2Var_SequenceOfPatch& Net,
				       const TColStd_SequenceOfReal& TheU,
				       const TColStd_SequenceOfReal& TheV)
{
  myNet=Net;
  myUParameters=TheU;
  myVParameters=TheV;
}

//==========================================================================================
//function : FirstNotApprox
//purpose  : return the first Patch not approximated
//==========================================================================================

Standard_Boolean AdvApp2Var_Network::FirstNotApprox(Standard_Integer& Index) const 
{
  Standard_Boolean good = Standard_True;
  Standard_Integer i;
  for (i = 1; i <= myNet.Length() && good; i++) {
    good = myNet.Value(i).IsApproximated();
    if (!good) {Index = i;}
  }
  return !good;
}

//==========================================================================================
//function : UpdateInU
//purpose  : modification and insertion of patches and parameters
//==========================================================================================

void AdvApp2Var_Network::UpdateInU(const Standard_Real CuttingValue)
{

//  insertion du nouveau parametre de decoupe
  Standard_Integer i=1,j;
  while (myUParameters.Value(i)<CuttingValue) {
    i++;
  }
  myUParameters.InsertBefore(i,CuttingValue);

  Standard_Integer indice;
  for (j=1; j< myVParameters.Length() ; j++){

//    modification des patches concernes par la decoupe
    AdvApp2Var_Patch Pat;
    indice = (myUParameters.Length()-1) * (j-1) + i - 1;
    Pat = myNet.Value(indice);
    Pat.ChangeDomain(Pat.U0(), CuttingValue, Pat.V0(), Pat.V1());
    Pat.ResetApprox();
    myNet.SetValue(indice, Pat);

//    insertion des nouveaux patches
    AdvApp2Var_Patch NewPat(CuttingValue,myUParameters.Value(i+1),
			    myVParameters.Value(j),myVParameters.Value(j+1),
			    Pat.UOrder(),Pat.VOrder());
    NewPat.ResetApprox();
    myNet.InsertAfter(indice, NewPat);
  }

}

//==========================================================================================
//function : UpdateInV
//purpose  : modification and insertion of patches and parameters
//==========================================================================================

void AdvApp2Var_Network::UpdateInV(const Standard_Real CuttingValue)
{

//  insertion du nouveau parametre de decoupe
  Standard_Integer i,j=1;
  AdvApp2Var_Patch Pat;
  while (myVParameters.Value(j)<CuttingValue) {
    j++;
  }
  myVParameters.InsertBefore(j,CuttingValue);

//  modification des patches concernes par la decoupe
  Standard_Integer indice;
  for (i=1; i< myUParameters.Length() ; i++){
    indice = (myUParameters.Length()-1) * (j-2) + i;
    Pat = myNet.Value(indice);
    Pat.ChangeDomain(Pat.U0(), Pat.U1(), Pat.V0(), CuttingValue);
    Pat.ResetApprox();
    myNet.SetValue(indice,Pat);
  }

//  insertion des nouveaux patches
  for (i=1; i< myUParameters.Length() ; i++){
    indice = (myUParameters.Length()-1) * (j-1) + i-1;
    AdvApp2Var_Patch NewPat(myUParameters.Value(i), myUParameters.Value(i+1),
			    CuttingValue,myVParameters.Value(j+1),
			    Pat.UOrder(),Pat.VOrder());
    NewPat.ResetApprox();
    myNet.InsertAfter(indice,NewPat);
  }
}

//=======================================================================
//function : SameDegree
//purpose  : same numbers of coefficients for all patches
//=======================================================================

void AdvApp2Var_Network::SameDegree(const Standard_Integer iu,
				    const Standard_Integer iv,
				    Standard_Integer& ncfu,
				    Standard_Integer& ncfv)
{

//  calcul des coeff. max avec init selon l'ordre de continuite
  Standard_Integer IndPat;
  ncfu = 2*iu+2;
  ncfv = 2*iv+2;
  for (IndPat=1;IndPat<=myNet.Length();IndPat++) {
    ncfu = Max(ncfu,myNet.Value(IndPat).NbCoeffInU());
    ncfv = Max(ncfv,myNet.Value(IndPat).NbCoeffInV());
  }

//  augmentation des nombres de coeff.
  AdvApp2Var_Patch Pat;
  for (IndPat=1;IndPat<=myNet.Length();IndPat++) {
    Pat = myNet.Value(IndPat);
    Pat.ChangeNbCoeff(ncfu,ncfv);
    myNet.SetValue(IndPat,Pat);
  }

}

//=======================================================================
//function : NbPatch
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Network::NbPatch() const
{
  return myNet.Length();
}

//=======================================================================
//function : NbPatchInU
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Network::NbPatchInU() const
{
  return myUParameters.Length()-1;
}

//=======================================================================
//function : NbPatchInV
//purpose  : 
//=======================================================================

Standard_Integer AdvApp2Var_Network::NbPatchInV() const
{
  return myVParameters.Length()-1;
}

//=======================================================================
//function : UParameter
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Network::UParameter(const Standard_Integer Index) const 
{
  return myUParameters.Value(Index);
}

//=======================================================================
//function : VParameter
//purpose  : 
//=======================================================================

Standard_Real AdvApp2Var_Network::VParameter(const Standard_Integer Index) const 
{
  return myVParameters.Value(Index);
}

