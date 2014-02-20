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


#include <AdvApp2Var_Framework.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_Strip.hxx>
#include <AdvApp2Var_SequenceOfStrip.hxx>
#include <AdvApp2Var_Node.hxx>
#include <AdvApp2Var_SequenceOfNode.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <gp_XY.hxx>

//==========================================================================================
//function : AdvApp2Var_Framework
//purpose  :
//==========================================================================================

AdvApp2Var_Framework::AdvApp2Var_Framework()
{
}


//==========================================================================================
//function : AdvApp2Var_Framework
//purpose  :
//==========================================================================================

AdvApp2Var_Framework::AdvApp2Var_Framework(const AdvApp2Var_SequenceOfNode& Frame,
					   const AdvApp2Var_SequenceOfStrip& UFrontier,
					   const AdvApp2Var_SequenceOfStrip& VFrontier)
{
  myNodeConstraints = Frame;
  myUConstraints = UFrontier;
  myVConstraints = VFrontier;
}

//==========================================================================================
//function : FirstNotApprox
//purpose  : return the first Iso not approximated
//==========================================================================================

Standard_Boolean  AdvApp2Var_Framework::FirstNotApprox(Standard_Integer& IndexIso,
                                                       Standard_Integer& IndexStrip,
						       AdvApp2Var_Iso& anIso) const
{
  Standard_Boolean good = Standard_True;
  Standard_Integer i,j;
  AdvApp2Var_Strip S;

  for (i = 1; i <= myUConstraints.Length() && good ; i++) {
    S = myUConstraints.Value(i);
    for (j = 1; j <= S.Length() && good ; j++) {
      good = (S.Value(j)).IsApproximated();
      if (!good) {
	IndexIso = j;
	IndexStrip = i;
	anIso = S.Value(j);
      }
    }
  }
  if (!good)  {
    goto FINISH;
  }

  for (i = 1; i <= myVConstraints.Length() && good; i++) {
    S = myVConstraints.Value(i);
    for (j = 1; j <= S.Length() && good ; j++) {
      good = (S.Value(j)).IsApproximated();
      if (!good) {
	IndexIso = j;
	IndexStrip = i;
	anIso = S.Value(j);
      }
    }
  }

  FINISH:
  return !good;
}

//==========================================================================================
//function : FirstNode 
//purpose  : return the first node of an iso
//==========================================================================================

Standard_Integer AdvApp2Var_Framework::FirstNode(const GeomAbs_IsoType Type,
						 const Standard_Integer IndexIso,
						 const Standard_Integer IndexStrip) const 
{
  Standard_Integer NbIso,Index;
  NbIso = myUConstraints.Length()+1;
  if (Type==GeomAbs_IsoU) {
    Index = NbIso * (IndexStrip-1) + IndexIso;
    return Index;
  }
  else {
    Index = NbIso * (IndexIso-1) + IndexStrip;
    return Index;
  }
}

//==========================================================================================
//function : LastNode 
//purpose  : return the last node of an iso
//==========================================================================================

Standard_Integer AdvApp2Var_Framework::LastNode(const GeomAbs_IsoType Type,
					       const Standard_Integer IndexIso,
					       const Standard_Integer IndexStrip) const 
{
  Standard_Integer NbIso,Index;
  NbIso = myUConstraints.Length()+1;
  if (Type==GeomAbs_IsoU) {
    Index = NbIso * IndexStrip + IndexIso;
    return Index;
  }
  else {
    Index = NbIso * (IndexIso-1) + IndexStrip + 1;
    return Index;
  }
}

//==========================================================================================
//function : ChangeIso
//purpose  : replace the iso IndexIso of the strip IndexStrip by anIso
//==========================================================================================

void AdvApp2Var_Framework::ChangeIso(const Standard_Integer IndexIso,
				     const Standard_Integer IndexStrip,
				     const AdvApp2Var_Iso& anIso)
{
  AdvApp2Var_Strip S0;
  if (anIso.Type()==GeomAbs_IsoV) {
    S0 = myUConstraints.Value(IndexStrip); 
    S0.SetValue(IndexIso,anIso);
    myUConstraints.SetValue(IndexStrip,S0);
  }
  else {
    S0 = myVConstraints.Value(IndexStrip);
    S0.SetValue(IndexIso,anIso);
    myVConstraints.SetValue(IndexStrip,S0);
  }
}

//==========================================================================================
//function : Node
//purpose  : return the node of coordinates (U,V)
//==========================================================================================

const AdvApp2Var_Node& AdvApp2Var_Framework::Node(const Standard_Real U,
						  const Standard_Real V) const
{
  Standard_Integer Index=1;
  while ( ( ((myNodeConstraints.Value(Index)).Coord()).X() != U
	 || ((myNodeConstraints.Value(Index)).Coord()).Y() != V )
         && (Index<myNodeConstraints.Length()) ) {
    Index++;
  }
  return myNodeConstraints.Value(Index);
}

//==========================================================================================
//function : IsoU
//purpose  : return the Iso U=U with V0<=V<=V1
//==========================================================================================

const AdvApp2Var_Iso& 
AdvApp2Var_Framework::IsoU(const Standard_Real U,
			      const Standard_Real V0,
			      const Standard_Real V1) const 
{
  Standard_Integer IndexStrip=1,IndexIso=1;
  while ( ( ((myVConstraints.Value(IndexStrip)).Value(1)).T0() != V0
	 || ((myVConstraints.Value(IndexStrip)).Value(1)).T1() != V1 )
         && (IndexStrip<myVConstraints.Length()) ) {
    IndexStrip++;
  }
  while ( ( ((myVConstraints.Value(IndexStrip)).Value(IndexIso)).Constante() != U)
         && (IndexIso<=myUConstraints.Length()) ) {
    IndexIso++;
  }
  return (myVConstraints.Value(IndexStrip)).Value(IndexIso);
}

//==========================================================================================
//function : IsoV
//purpose  : return the Iso V=V with U0<=U<=U1
//==========================================================================================

const AdvApp2Var_Iso& 
AdvApp2Var_Framework::IsoV(const Standard_Real U0,
			      const Standard_Real U1,
			      const Standard_Real V) const
{
  Standard_Integer IndexStrip=1,IndexIso=1;
  while ( ( ((myUConstraints.Value(IndexStrip)).Value(1)).T0() != U0
	 || ((myUConstraints.Value(IndexStrip)).Value(1)).T1() != U1 )
         && (IndexStrip<myUConstraints.Length()) ) {
    IndexStrip++;
  }
  while ( ( ((myUConstraints.Value(IndexStrip)).Value(IndexIso)).Constante() != V)
         && (IndexIso<=myVConstraints.Length()) ) {
    IndexIso++;
  }
  return (myUConstraints.Value(IndexStrip)).Value(IndexIso);
}

//==========================================================================================
//function : UpdateInU
//purpose  : modification and insertion of nodes and isos
//==========================================================================================

void AdvApp2Var_Framework::UpdateInU(const Standard_Real CuttingValue)
{
  Standard_Integer i=1,j;
  while (((myUConstraints.Value(i)).Value(1)).U0()>CuttingValue
	 || ((myUConstraints.Value(i)).Value(1)).U1()<CuttingValue) {
    i++;
  }
  AdvApp2Var_Strip S0;
  AdvApp2Var_Iso Is;
  S0 = myUConstraints.Value(i);
  Standard_Real Udeb = (S0.Value(1)).U0(), Ufin = (S0.Value(1)).U1(); 

//  modification des Isos V de la bande en U d'indice i
  for (j=1;j<=S0.Length();j++) {
    Is = S0.Value(j);
    Is.ChangeDomain(Udeb,CuttingValue);
    Is.ResetApprox();
    S0.SetValue(j,Is);
  }
  myUConstraints.SetValue(i,S0);

//  insertion d'une nouvelle bande en U apres l'indice i
  AdvApp2Var_Strip NewStrip;
  for (j=1;j<=S0.Length();j++) {
    AdvApp2Var_Iso NewIso((S0.Value(j)).Type(),(S0.Value(j)).Constante(),
                          CuttingValue,Ufin,(S0.Value(j)).V0(),(S0.Value(j)).V1(),
			  0,(S0.Value(j)).UOrder(),(S0.Value(j)).VOrder());
    NewIso.ResetApprox();
    NewStrip.Append(NewIso);
  }
  myUConstraints.InsertAfter(i,NewStrip);

//  insertion d'une nouvelle Iso U=U* dans chaque bande en V apres l'indice i
//  et restriction des paves des Isos adjacentes
  for (j=1;j<=myVConstraints.Length();j++) {
    S0 = myVConstraints.Value(j);
    Is = S0.Value(i);
    Is.ChangeDomain(Is.U0(),CuttingValue,Is.V0(),Is.V1());
    S0.SetValue(i,Is);
    AdvApp2Var_Iso NewIso(Is.Type(),CuttingValue,Is.U0(),CuttingValue,Is.V0(),Is.V1(),
			  0,Is.UOrder(),Is.VOrder());
    NewIso.ResetApprox();
    S0.InsertAfter(i,NewIso);
    Is = S0.Value(i+2);
    Is.ChangeDomain(CuttingValue,Is.U1(),Is.V0(),Is.V1());
    S0.SetValue(i+2,Is);
    myVConstraints.SetValue(j,S0);
  }

//  insertion des nouveaux noeuds (U*,Vj)
  AdvApp2Var_Node Prev, Next;
  Prev=myNodeConstraints.Value(1);
  for (j=1;j<myNodeConstraints.Length();j++) {
    Next=myNodeConstraints.Value(j+1);
    if ((Prev.Coord()).X()<CuttingValue && ((Next.Coord()).X()>CuttingValue)
	&& ((Prev.Coord()).Y()==(Next.Coord()).Y())) {
      gp_XY NewUV(CuttingValue,(Prev.Coord()).Y());
      AdvApp2Var_Node NewNode(NewUV,Prev.UOrder(),Prev.VOrder());
      myNodeConstraints.InsertAfter(j,NewNode);
    }
    Prev=Next;
  }
}

//==========================================================================================
//function : UpdateInV
//purpose  : modification and insertion of nodes and isos
//==========================================================================================

void AdvApp2Var_Framework::UpdateInV(const Standard_Real CuttingValue)
{
  Standard_Integer i,j=1;
  while (((myVConstraints.Value(j)).Value(1)).V0()>CuttingValue
	 || ((myVConstraints.Value(j)).Value(1)).V1()<CuttingValue) {
    j++;
  }
  AdvApp2Var_Strip S0;
  AdvApp2Var_Iso Is;
  S0 = myVConstraints.Value(j);
  Standard_Real Vdeb = (S0.Value(1)).V0(), Vfin = (S0.Value(1)).V1(); 

//  modification des Isos U de la bande en V d'indice j
  for (i=1;i<=S0.Length();i++) {
    Is = S0.Value(i);
    Is.ChangeDomain(Vdeb,CuttingValue);
    Is.ResetApprox();
    S0.SetValue(i,Is);
  }
  myVConstraints.SetValue(j,S0);

//  insertion d'une nouvelle bande en V apres l'indice j
  AdvApp2Var_Strip NewStrip;
  for (i=1;i<=S0.Length();i++) {
    AdvApp2Var_Iso NewIso((S0.Value(i)).Type(),(S0.Value(i)).Constante(),
                          (S0.Value(i)).U0(),(S0.Value(i)).U1(),CuttingValue,Vfin,
			  0,(S0.Value(i)).UOrder(),(S0.Value(i)).VOrder());
    NewIso.ResetApprox();
    NewStrip.Append(NewIso);
  }
  myVConstraints.InsertAfter(j,NewStrip);

//  insertion d'une nouvelle Iso V=V* dans chaque bande en U apres l'indice j
//  et restriction des paves des Isos adjacentes
  for (i=1;i<=myUConstraints.Length();i++) {
    S0 = myUConstraints.Value(i);
    Is = S0.Value(j);
    Is.ChangeDomain(Is.U0(),Is.U1(),Is.V0(),CuttingValue);
    S0.SetValue(j,Is);
    AdvApp2Var_Iso NewIso(Is.Type(),CuttingValue,Is.U0(),Is.U1(),Is.V0(),CuttingValue,
			  0,Is.UOrder(),Is.VOrder());
    NewIso.ResetApprox();
    S0.InsertAfter(j,NewIso);
    Is = S0.Value(j+2);
    Is.ChangeDomain(Is.U0(),Is.U1(),CuttingValue,Is.V1());
    S0.SetValue(j+2,Is);
    myUConstraints.SetValue(i,S0);
  }

//  insertion des nouveaux noeuds (Ui,V*)
  i = 1;
  while ( i<=myNodeConstraints.Length() 
	 && ( ((myNodeConstraints.Value(i)).Coord()).Y()) < CuttingValue) {
    i+=myUConstraints.Length()+1;
  }
  for (j=1;j<=myUConstraints.Length()+1;j++) {
    gp_XY NewUV(((myNodeConstraints.Value(j)).Coord()).X(),CuttingValue);
    AdvApp2Var_Node NewNode(NewUV,
			    (myNodeConstraints.Value(j)).UOrder(),
			    (myNodeConstraints.Value(j)).VOrder());
    myNodeConstraints.InsertAfter(i+j-2,NewNode);
  }
}

//==========================================================================================
//function : UEquation
//purpose  : return the coefficients of the polynomial equation which approximates an iso U
//==========================================================================================

const Handle(TColStd_HArray1OfReal)& 
AdvApp2Var_Framework::UEquation(const Standard_Integer IndexIso,
				const Standard_Integer IndexStrip) const
{
  return ((myVConstraints.Value(IndexStrip)).Value(IndexIso)).Polynom();
}

//==========================================================================================
//function : VEquation
//purpose  : return the coefficients of the polynomial equation which approximates an iso V
//==========================================================================================

const Handle(TColStd_HArray1OfReal)& 
AdvApp2Var_Framework::VEquation(const Standard_Integer IndexIso,
				const Standard_Integer IndexStrip) const
{  
  return myUConstraints.Value(IndexStrip).Value(IndexIso).Polynom();
}

