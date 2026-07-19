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

namespace
{
int findStripByBounds(
  const NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>& theConstraints,
  const double                                                                   theFirst,
  const double                                                                   theLast)
{
  int aStripIndex = 1;
  while (aStripIndex < theConstraints.Length()
         && (theConstraints.Value(aStripIndex).First()->T0() != theFirst
             || theConstraints.Value(aStripIndex).First()->T1() != theLast))
  {
    ++aStripIndex;
  }
  return aStripIndex;
}

int findIsoByConstant(const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& theStrip,
                      const int                                                theNbIso,
                      const double                                             theConst)
{
  int anIsoIndex = 1;
  while (anIsoIndex <= theNbIso && theStrip.Value(anIsoIndex)->Constante() != theConst)
  {
    ++anIsoIndex;
  }
  return anIsoIndex;
}
} // namespace

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

//=================================================================================================

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

//=================================================================================================

int AdvApp2Var_Framework::FirstNode(const GeomAbs_IsoType Type,
                                    const int             IndexIso,
                                    const int             IndexStrip) const
{
  const int aNbIsoInV = myUConstraints.Length() + 1;
  if (Type == GeomAbs_IsoU)
  {
    return aNbIsoInV * (IndexStrip - 1) + IndexIso;
  }
  return aNbIsoInV * (IndexIso - 1) + IndexStrip;
}

//=================================================================================================

int AdvApp2Var_Framework::LastNode(const GeomAbs_IsoType Type,
                                   const int             IndexIso,
                                   const int             IndexStrip) const
{
  const int aNbIsoInV = myUConstraints.Length() + 1;
  if (Type == GeomAbs_IsoU)
  {
    return aNbIsoInV * IndexStrip + IndexIso;
  }
  return aNbIsoInV * (IndexIso - 1) + IndexStrip + 1;
}

//=================================================================================================

void AdvApp2Var_Framework::ChangeIso(const int                          IndexIso,
                                     const int                          IndexStrip,
                                     const occ::handle<AdvApp2Var_Iso>& theIso)
{
  NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
    theIso->Type() == GeomAbs_IsoV ? myUConstraints.ChangeValue(IndexStrip)
                                   : myVConstraints.ChangeValue(IndexStrip);
  aStrip.SetValue(IndexIso, theIso);
}

//=================================================================================================

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

//=================================================================================================

const AdvApp2Var_Iso& AdvApp2Var_Framework::IsoU(const double U,
                                                 const double V0,
                                                 const double V1) const
{
  const int aStripIndex = findStripByBounds(myVConstraints, V0, V1);
  const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
    myVConstraints.Value(aStripIndex);
  const int aIsoIndex = findIsoByConstant(aStrip, myUConstraints.Length(), U);
  return *(aStrip.Value(aIsoIndex));
}

//=================================================================================================

const AdvApp2Var_Iso& AdvApp2Var_Framework::IsoV(const double U0,
                                                 const double U1,
                                                 const double V) const
{
  const int aStripIndex = findStripByBounds(myUConstraints, U0, U1);
  const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
    myUConstraints.Value(aStripIndex);
  const int aIsoIndex = findIsoByConstant(aStrip, myVConstraints.Length(), V);
  return *(aStrip.Value(aIsoIndex));
}

//=================================================================================================

void AdvApp2Var_Framework::UpdateInU(const double CuttingValue)
{
  int aUStripIndex = 1;
  for (NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>::Iterator
         anUConstIter(myUConstraints);
       anUConstIter.More();
       anUConstIter.Next(), ++aUStripIndex)
  {
    if (anUConstIter.Value().First()->U0() <= CuttingValue
        && anUConstIter.Value().First()->U1() >= CuttingValue)
    {
      break;
    }
  }

  {
    const NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
      myUConstraints.Value(aUStripIndex);
    const double aUFirst = aStrip.First()->U0();
    const double aULast  = aStrip.First()->U1();

    // Modify the V isos of the U strip at aUStripIndex.
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator aStripIter(aStrip);
         aStripIter.More();
         aStripIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso = aStripIter.Value();
      anIso->ChangeDomain(aUFirst, CuttingValue);
      anIso->ResetApprox();
    }

    // Insert a new U strip after aUStripIndex.
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>> aNewStrip;
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator aStripIter(aStrip);
         aStripIter.More();
         aStripIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso   = aStripIter.Value();
      occ::handle<AdvApp2Var_Iso>        aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                               anIso->Constante(),
                                                               CuttingValue,
                                                               aULast,
                                                               anIso->V0(),
                                                               anIso->V1(),
                                                               0,
                                                               anIso->UOrder(),
                                                               anIso->VOrder());
      aNewIso->ResetApprox();
      aNewStrip.Append(aNewIso);
    }
    myUConstraints.InsertAfter(aUStripIndex, aNewStrip);
  }

  //  Insert a new Iso U=U* in each V strip after aUStripIndex
  //  and restrict the domains of the adjacent Isos
  for (int aVStripIndex = 1; aVStripIndex <= myVConstraints.Length(); aVStripIndex++)
  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
      myVConstraints.ChangeValue(aVStripIndex);
    occ::handle<AdvApp2Var_Iso> anIso = aStrip.Value(aUStripIndex);
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
    aStrip.InsertAfter(aUStripIndex, aNewIso);

    anIso = aStrip.Value(aUStripIndex + 2);
    anIso->ChangeDomain(CuttingValue, anIso->U1(), anIso->V0(), anIso->V1());
  }

  //  Insert the new nodes (U*,Vj)
  occ::handle<AdvApp2Var_Node> aNext;
  occ::handle<AdvApp2Var_Node> aPrev = myNodeConstraints.First();
  for (int aNodeIndex = 1; aNodeIndex < myNodeConstraints.Length(); aNodeIndex++)
  {
    aNext = myNodeConstraints.Value(aNodeIndex + 1);
    if (aPrev->Coord().X() < CuttingValue && aNext->Coord().X() > CuttingValue
        && aPrev->Coord().Y() == aNext->Coord().Y())
    {
      gp_XY                        aNewUV(CuttingValue, aPrev->Coord().Y());
      occ::handle<AdvApp2Var_Node> aNewNode =
        new AdvApp2Var_Node(aNewUV, aPrev->UOrder(), aPrev->VOrder());
      myNodeConstraints.InsertAfter(aNodeIndex, aNewNode);
    }
    aPrev = aNext;
  }
}

//=================================================================================================

void AdvApp2Var_Framework::UpdateInV(const double CuttingValue)
{
  int aVStripIndex = 1;
  while (myVConstraints.Value(aVStripIndex).First()->V0() > CuttingValue
         || myVConstraints.Value(aVStripIndex).First()->V1() < CuttingValue)
  {
    aVStripIndex++;
  }

  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip =
      myVConstraints.ChangeValue(aVStripIndex);
    const double aVFirst = aStrip.First()->V0();
    const double aVLast  = aStrip.First()->V1();

    // Modify the U isos of the V strip at aVStripIndex.
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator anIsoIter(aStrip);
         anIsoIter.More();
         anIsoIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso = anIsoIter.Value();
      anIso->ChangeDomain(aVFirst, CuttingValue);
      anIso->ResetApprox();
    }

    // Insert a new V strip after aVStripIndex.
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>> aNewStrip;
    for (NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>::Iterator anIsoIter(aStrip);
         anIsoIter.More();
         anIsoIter.Next())
    {
      const occ::handle<AdvApp2Var_Iso>& anIso   = anIsoIter.Value();
      occ::handle<AdvApp2Var_Iso>        aNewIso = new AdvApp2Var_Iso(anIso->Type(),
                                                               anIso->Constante(),
                                                               anIso->U0(),
                                                               anIso->U1(),
                                                               CuttingValue,
                                                               aVLast,
                                                               0,
                                                               anIso->UOrder(),
                                                               anIso->VOrder());
      aNewIso->ResetApprox();
      aNewStrip.Append(aNewIso);
    }
    myVConstraints.InsertAfter(aVStripIndex, aNewStrip);
  }

  // Insert a new Iso V=V* in each U strip after aVStripIndex
  // and restrict the domains of the adjacent Isos
  for (NCollection_Sequence<NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>>::Iterator
         anUConstIter(myUConstraints);
       anUConstIter.More();
       anUConstIter.Next())
  {
    NCollection_Sequence<occ::handle<AdvApp2Var_Iso>>& aStrip = anUConstIter.ChangeValue();
    occ::handle<AdvApp2Var_Iso>                        anIso  = aStrip.Value(aVStripIndex);
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
    aStrip.InsertAfter(aVStripIndex, aNewIso);

    anIso = aStrip.Value(aVStripIndex + 2);
    anIso->ChangeDomain(anIso->U0(), anIso->U1(), CuttingValue, anIso->V1());
  }

  //  Insert the new nodes (Ui,V*)
  int aNodeStartIndex = 1;
  while (aNodeStartIndex <= myNodeConstraints.Length()
         && myNodeConstraints.Value(aNodeStartIndex)->Coord().Y() < CuttingValue)
  {
    aNodeStartIndex += myUConstraints.Length() + 1;
  }
  for (int anIsoIndex = 1; anIsoIndex <= myUConstraints.Length() + 1; anIsoIndex++)
  {
    const occ::handle<AdvApp2Var_Node>& aJNode = myNodeConstraints.Value(anIsoIndex);
    gp_XY                               NewUV(aJNode->Coord().X(), CuttingValue);
    occ::handle<AdvApp2Var_Node>        aNewNode =
      new AdvApp2Var_Node(NewUV, aJNode->UOrder(), aJNode->VOrder());
    myNodeConstraints.InsertAfter(aNodeStartIndex + anIsoIndex - 2, aNewNode);
  }
}

//=================================================================================================

const occ::handle<NCollection_HArray1<double>>& AdvApp2Var_Framework::UEquation(
  const int IndexIso,
  const int IndexStrip) const
{
  return myVConstraints.Value(IndexStrip).Value(IndexIso)->Polynom();
}

//=================================================================================================

const occ::handle<NCollection_HArray1<double>>& AdvApp2Var_Framework::VEquation(
  const int IndexIso,
  const int IndexStrip) const
{
  return myUConstraints.Value(IndexStrip).Value(IndexIso)->Polynom();
}
