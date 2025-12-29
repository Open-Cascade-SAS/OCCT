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
#include <NCollection_Sequence.hxx>
#include <AdvApp2Var_Node.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <gp_XY.hxx>

//=================================================================================================

AdvApp2Var_Framework::AdvApp2Var_Framework() = default;

//=================================================================================================

AdvApp2Var_Framework::AdvApp2Var_Framework(
  const NCollection_Sequence<occ::handle<AdvApp2Var_Node>>&                      Frame,
  const NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>& UFrontier,
  const NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>& VFrontier)
{
  myNodeConstraints = Frame;
  myUConstraints    = UFrontier;
  myVConstraints    = VFrontier;
}

//==========================================================================================
// function : FirstNotApprox
// purpose  : return the first Iso not approximated
//==========================================================================================

occ::handle<AdvApp2Var_Iso> AdvApp2Var_Framework::FirstNotApprox(int& IndexIso,
                                                                 int& IndexStrip) const
{
  for (int anUVIter = 0; anUVIter < 2; ++anUVIter)
  {
    const NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>& aSeq =
      anUVIter == 0 ? myUConstraints : myVConstraints;
    int i = 1;
    for (NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>::Iterator
           aConstIter(aSeq);
         aConstIter.More();
         aConstIter.Next(), i++)
    {
      const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S = aConstIter.Value();
      int                                                      j = 1;
      for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator anIsoIter(S);
           anIsoIter.More();
           anIsoIter.Next(), j++)
      {
        const occ::handle<AdvApp2Var_Iso>& anIso = anIsoIter.Value();
        if (!anIso->IsApproximated())
        {
          IndexIso   = j;
          IndexStrip = i;
          return anIso;
        }
      }
    }
  }
  return occ::handle<AdvApp2Var_Iso>();
}

//==========================================================================================
// function : FirstNode
// purpose  : return the first node of an iso
//==========================================================================================

int AdvApp2Var_Framework::FirstNode(const GeomAbs_IsoType Type,
                                    const int             IndexIso,
                                    const int             IndexStrip) const
{
  int NbIso, Index;
  NbIso = myUConstraints.Length() + 1;
  if (Type == GeomAbs_IsoU)
  {
    Index = NbIso * (IndexStrip - 1) + IndexIso;
    return Index;
  }
  else
  {
    Index = NbIso * (IndexIso - 1) + IndexStrip;
    return Index;
  }
}

//==========================================================================================
// function : LastNode
// purpose  : return the last node of an iso
//==========================================================================================

int AdvApp2Var_Framework::LastNode(const GeomAbs_IsoType Type,
                                   const int             IndexIso,
                                   const int             IndexStrip) const
{
  int NbIso, Index;
  NbIso = myUConstraints.Length() + 1;
  if (Type == GeomAbs_IsoU)
  {
    Index = NbIso * IndexStrip + IndexIso;
    return Index;
  }
  else
  {
    Index = NbIso * (IndexIso - 1) + IndexStrip + 1;
    return Index;
  }
}

//==========================================================================================
// function : ChangeIso
// purpose  : replace the iso IndexIso of the strip IndexStrip by anIso
//==========================================================================================

void AdvApp2Var_Framework::ChangeIso(const int                          IndexIso,
                                     const int                          IndexStrip,
                                     const occ::handle<AdvApp2Var_Iso>& theIso)
{
  NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S0 =
    theIso->Type() == GeomAbs_IsoV ? myUConstraints.ChangeValue(IndexStrip)
                                   : myVConstraints.ChangeValue(IndexStrip);
  S0.SetValue(IndexIso, theIso);
}

//==========================================================================================
// function : Node
// purpose  : return the node of coordinates (U,V)
//==========================================================================================

const occ::handle<AdvApp2Var_Node>& AdvApp2Var_Framework::Node(const double U, const double V) const
{
  for (NCollection_Sequence<occ::handle<AdvApp2Var_Node>>::Iterator aNodeIter(myNodeConstraints);
       aNodeIter.More();
       aNodeIter.Next())
  {
    const occ::handle<AdvApp2Var_Node>& aNode = aNodeIter.Value();
    if (aNode->Coord().X() == U && aNode->Coord().Y() == V)
    {
      return aNode;
    }
  }
  return myNodeConstraints.Last();
}

//==========================================================================================
// function : IsoU
// purpose  : return the Iso U=U with V0<=V<=V1
//==========================================================================================

const AdvApp2Var_Iso& AdvApp2Var_Framework::IsoU(const double U,
                                                 const double V0,
                                                 const double V1) const
{
  int IndexStrip = 1;
  while (IndexStrip < myVConstraints.Length()
         && (myVConstraints.Value(IndexStrip).First()->T0() != V0
             || myVConstraints.Value(IndexStrip).First()->T1() != V1))
  {
    IndexStrip++;
  }
  int IndexIso = 1;
  while (IndexIso <= myUConstraints.Length()
         && myVConstraints.Value(IndexStrip).Value(IndexIso)->Constante() != U)
  {
    IndexIso++;
  }
  return *(myVConstraints.Value(IndexStrip).Value(IndexIso));
}

//==========================================================================================
// function : IsoV
// purpose  : return the Iso V=V with U0<=U<=U1
//==========================================================================================

const AdvApp2Var_Iso& AdvApp2Var_Framework::IsoV(const double U0,
                                                 const double U1,
                                                 const double V) const
{
  int IndexStrip = 1;
  while (IndexStrip < myUConstraints.Length()
         && (myUConstraints.Value(IndexStrip).First()->T0() != U0
             || myUConstraints.Value(IndexStrip).First()->T1() != U1))
  {
    IndexStrip++;
  }
  int IndexIso = 1;
  while (IndexIso <= myVConstraints.Length()
         && myUConstraints.Value(IndexStrip).Value(IndexIso)->Constante() != V)
  {
    IndexIso++;
  }
  return *(myUConstraints.Value(IndexStrip).Value(IndexIso));
}

//==========================================================================================
// function : UpdateInU
// purpose  : modification and insertion of nodes and isos
//==========================================================================================

void AdvApp2Var_Framework::UpdateInU(const double CuttingValue)
{
  int i = 1;
  for (NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>::Iterator
         anUConstIter(myUConstraints);
       anUConstIter.More();
       anUConstIter.Next(), ++i)
  {
    if (anUConstIter.Value().First()->U0() <= CuttingValue
        && anUConstIter.Value().First()->U1() >= CuttingValue)
    {
      break;
    }
  }

  {
    const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S0 = myUConstraints.Value(i);
    const double Udeb = S0.First()->U0(), Ufin = S0.First()->U1();

    // modification des Isos V de la bande en U d'indice i
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator aStripIter(S0);
         aStripIter.More();
         aStripIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso = aStripIter.Value();
      anIso->ChangeDomain(Udeb, CuttingValue);
      anIso->ResetApprox();
    }

    // insertion d'une nouvelle bande en U apres l'indice i
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>> aNewStrip;
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator aStripIter(S0);
         aStripIter.More();
         aStripIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso   = aStripIter.Value();
      occ::handle<AdvApp2Var_Iso>        aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                               anIso->Constante(),
                                                               CuttingValue,
                                                               Ufin,
                                                               anIso->V0(),
                                                               anIso->V1(),
                                                               0,
                                                               anIso->UOrder(),
                                                               anIso->VOrder());
      aNewIso->ResetApprox();
      aNewStrip.Append(aNewIso);
    }
    myUConstraints.InsertAfter(i, aNewStrip);
  }

  //  insertion d'une nouvelle Iso U=U* dans chaque bande en V apres l'indice i
  //  et restriction des paves des Isos adjacentes
  for (int j = 1; j <= myVConstraints.Length(); j++)
  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S0    = myVConstraints.ChangeValue(j);
    occ::handle<AdvApp2Var_Iso>                        anIso = S0.Value(i);
    anIso->ChangeDomain(anIso->U0(), CuttingValue, anIso->V0(), anIso->V1());

    occ::handle<AdvApp2Var_Iso> aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                             CuttingValue,
                                                             anIso->U0(),
                                                             CuttingValue,
                                                             anIso->V0(),
                                                             anIso->V1(),
                                                             0,
                                                             anIso->UOrder(),
                                                             anIso->VOrder());
    aNewIso->ResetApprox();
    S0.InsertAfter(i, aNewIso);

    anIso = S0.Value(i + 2);
    anIso->ChangeDomain(CuttingValue, anIso->U1(), anIso->V0(), anIso->V1());
  }

  //  insertion des nouveaux noeuds (U*,Vj)
  occ::handle<AdvApp2Var_Node> aNext;
  occ::handle<AdvApp2Var_Node> aPrev = myNodeConstraints.First();
  for (int j = 1; j < myNodeConstraints.Length(); j++)
  {
    aNext = myNodeConstraints.Value(j + 1);
    if (aPrev->Coord().X() < CuttingValue && aNext->Coord().X() > CuttingValue
        && aPrev->Coord().Y() == aNext->Coord().Y())
    {
      gp_XY                        aNewUV(CuttingValue, aPrev->Coord().Y());
      occ::handle<AdvApp2Var_Node> aNewNode =
        new AdvApp2Var_Node(aNewUV, aPrev->UOrder(), aPrev->VOrder());
      myNodeConstraints.InsertAfter(j, aNewNode);
    }
    aPrev = aNext;
  }
}

//==========================================================================================
// function : UpdateInV
// purpose  : modification and insertion of nodes and isos
//==========================================================================================

void AdvApp2Var_Framework::UpdateInV(const double CuttingValue)
{
  int j = 1;
  while (myVConstraints.Value(j).First()->V0() > CuttingValue
         || myVConstraints.Value(j).First()->V1() < CuttingValue)
  {
    j++;
  }

  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S0 = myVConstraints.ChangeValue(j);
    const double Vdeb = S0.First()->V0(), Vfin = S0.First()->V1();

    // modification des Isos U de la bande en V d'indice j
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator anIsoIter(S0);
         anIsoIter.More();
         anIsoIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso = anIsoIter.Value();
      anIso->ChangeDomain(Vdeb, CuttingValue);
      anIso->ResetApprox();
    }

    // insertion d'une nouvelle bande en V apres l'indice j
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>> aNewStrip;
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator anIsoIter(S0);
         anIsoIter.More();
         anIsoIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso   = anIsoIter.Value();
      occ::handle<AdvApp2Var_Iso>        aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                               anIso->Constante(),
                                                               anIso->U0(),
                                                               anIso->U1(),
                                                               CuttingValue,
                                                               Vfin,
                                                               0,
                                                               anIso->UOrder(),
                                                               anIso->VOrder());
      aNewIso->ResetApprox();
      aNewStrip.Append(aNewIso);
    }
    myVConstraints.InsertAfter(j, aNewStrip);
  }

  // insertion d'une nouvelle Iso V=V* dans chaque bande en U apres l'indice j
  // et restriction des paves des Isos adjacentes
  for (NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>::Iterator
         anUConstIter(myUConstraints);
       anUConstIter.More();
       anUConstIter.Next())
  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& S0    = anUConstIter.ChangeValue();
    occ::handle<AdvApp2Var_Iso>                        anIso = S0.Value(j);
    anIso->ChangeDomain(anIso->U0(), anIso->U1(), anIso->V0(), CuttingValue);

    occ::handle<AdvApp2Var_Iso> aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                             CuttingValue,
                                                             anIso->U0(),
                                                             anIso->U1(),
                                                             anIso->V0(),
                                                             CuttingValue,
                                                             0,
                                                             anIso->UOrder(),
                                                             anIso->VOrder());
    aNewIso->ResetApprox();
    S0.InsertAfter(j, aNewIso);

    anIso = S0.Value(j + 2);
    anIso->ChangeDomain(anIso->U0(), anIso->U1(), CuttingValue, anIso->V1());
  }

  //  insertion des nouveaux noeuds (Ui,V*)
  int i = 1;
  while (i <= myNodeConstraints.Length() && myNodeConstraints.Value(i)->Coord().Y() < CuttingValue)
  {
    i += myUConstraints.Length() + 1;
  }
  for (j = 1; j <= myUConstraints.Length() + 1; j++)
  {
    const occ::handle<AdvApp2Var_Node>& aJNode = myNodeConstraints.Value(j);
    gp_XY                               NewUV(aJNode->Coord().X(), CuttingValue);
    occ::handle<AdvApp2Var_Node>        aNewNode =
      new AdvApp2Var_Node(NewUV, aJNode->UOrder(), aJNode->VOrder());
    myNodeConstraints.InsertAfter(i + j - 2, aNewNode);
  }
}

//==========================================================================================
// function : UEquation
// purpose  : return the coefficients of the polynomial equation which approximates an iso U
//==========================================================================================

const occ::handle<NCollection_HArray1<double>>& AdvApp2Var_Framework::UEquation(
  const int IndexIso,
  const int IndexStrip) const
{
  return myVConstraints.Value(IndexStrip).Value(IndexIso)->Polynom();
}

//==========================================================================================
// function : VEquation
// purpose  : return the coefficients of the polynomial equation which approximates an iso V
//==========================================================================================

const occ::handle<NCollection_HArray1<double>>& AdvApp2Var_Framework::VEquation(
  const int IndexIso,
  const int IndexStrip) const
{
  return myUConstraints.Value(IndexStrip).Value(IndexIso)->Polynom();
}
