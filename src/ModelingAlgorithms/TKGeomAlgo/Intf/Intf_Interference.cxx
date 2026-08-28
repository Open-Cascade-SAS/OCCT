// Created on: 1991-06-24
// Created by: Didier PIFFAULT
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Intf_Interference.hxx>
#include <Intf_SectionLine.hxx>
#include <Intf_SectionPoint.hxx>
#include <Intf_TangentZone.hxx>
#include <Message_ProgressScope.hxx>
#include <Standard_Failure.hxx>

//=======================================================================
// function : Intf_Interference
// purpose  : Initialize for a deferred interference.
//=======================================================================
Intf_Interference::Intf_Interference(const bool Self)
    : SelfIntf(Self),
      Tolerance(0.0)
{
}

//=================================================================================================

namespace
{
// thread_local: safe by construction even if a future caller runs this on
// a worker thread of its own.
thread_local const Message_ProgressScope* t_intfBreaker = nullptr;
} // namespace

void Intf_Interference::SetBreaker(const Message_ProgressScope* theScope)
{
  t_intfBreaker = theScope;
}

//=======================================================================
// function : SelfInterference
// purpose  : Reset interference before perform with a new...
//=======================================================================

void Intf_Interference::SelfInterference(const bool Self)
{
  SelfIntf = Self;
  mySPoins.Clear();
  mySLines.Clear();
  myTZones.Clear();
}

//=======================================================================
// function : Insert
// purpose  : Insert a tangent zone in the list of the interference
//=======================================================================

bool Intf_Interference::Insert(const Intf_TangentZone& LaZone)
{
  // Poll every 256 calls: this function can run tens of thousands of times
  // on a degenerate surface, so polling every call would add its own tax.
  static thread_local int t_pollCounter = 0;
  if (t_intfBreaker != nullptr && (++t_pollCounter & 0xFF) == 0 && t_intfBreaker->UserBreak())
  {
    throw Standard_Failure("Intf_Interference::Insert: aborted by breaker");
  }

  if (myTZones.Length() <= 0)
  {
    return false;
  }
  int  lzin     = 0;                       // Index in the list of the zone of interest.
  int  lunp     = 0;                       // Index of the 1st stop point in this zone.
  int  lotp     = 0;                       // Index of the 2nd stop point in this zone.
  int  lunl     = 0;                       // Index of the 1st point of the new zone.
  int  lotl     = 0;                       // Index of the 2nd point of the new zone.
  bool same     = false;                   // Search direction of the stop of the new zone.
  bool Inserted = true;                    // Has the insertion succeeded ?
  int  npcz     = -1;                      // Number of points in the current zone
  int  nplz     = LaZone.NumberOfPoints(); // in the new zone

  // GetPoint(Index) is O(n) per call (NCollection_Sequence has no O(1)
  // indexed access), so calling it inside the nested loop below made every
  // comparison pay that cost again. Points() caches a random-access array
  // per zone instead: same comparisons, same result, O(1) lookup.
  const NCollection_Array1<Intf_SectionPoint>& laZonePts = LaZone.Points();

  // Loop on TangentZone :
  for (int Iz = 1; Iz <= myTZones.Length(); Iz++)
  {

    // Loop on edges of the TangentZone :
    npcz                                                    = myTZones(Iz).NumberOfPoints();
    const NCollection_Array1<Intf_SectionPoint>& curZonePts = myTZones(Iz).Points();
    int                                          Ipz0, Ipz1, Ipz2;
    for (Ipz1 = 1; Ipz1 <= npcz; Ipz1++)
    {
      Ipz0 = Ipz1 - 1;
      if (Ipz0 <= 0)
      {
        Ipz0 = npcz;
      }
      Ipz2 = (Ipz1 % npcz) + 1;

      // Loop on edges of the new TangentZone and search of the
      // corresponding point or edge:
      int Ilz1, Ilz2;
      for (Ilz1 = 1; Ilz1 <= nplz; Ilz1++)
      {
        Ilz2 = (Ilz1 % nplz) + 1;

        if (curZonePts(Ipz1).IsEqual(laZonePts(Ilz1)))
        {
          if (curZonePts(Ipz0).IsEqual(laZonePts(Ilz2)))
          {
            lzin = Iz;
            lunp = Ipz0;
            lotp = Ipz1;
            lunl = Ilz1;
            lotl = Ilz2;
            same = false;
            break;
          }
          else if (curZonePts(Ipz2).IsEqual(laZonePts(Ilz2)))
          {
            lzin = Iz;
            lunp = Ipz1;
            lotp = Ipz2;
            lunl = Ilz1;
            lotl = Ilz2;
            same = true;
            break;
          }
          else
          {
            lzin = Iz;
            lunp = Ipz1;
            lunl = Ilz1;
          }
        }
      }
      if (lotp != 0)
      {
        break;
      }
    }
    if (lotp != 0)
    {
      break;
    }
  }

  int Ilc;
  if (lotp != 0)
  {
    for (Ilc = lotl + 1; (((Ilc - 1) % nplz) + 1) != lunl; Ilc++)
    {
      myTZones(lzin).InsertBefore(lotp, laZonePts(((Ilc - 1) % nplz) + 1));
      if (!same)
      {
        lotp++;
      }
    }
  }

  else if (lunp > 0)
  {
    bool loop = false;
    for (Ilc = lunl;; Ilc++)
    {
      myTZones(lzin).InsertBefore(lunp, laZonePts((((Ilc - 1) % nplz) + 1)));
      lunp++;
      if (loop && (((Ilc - 1) % nplz) + 1) == lunl)
      {
        break;
      }
      loop = true;
    }
  }

  else
  {
    Inserted = false;
  }

  if (Inserted)
  {
    Intf_TangentZone theNew = myTZones(lzin);
    myTZones.Remove(lzin);
    if (!Insert(theNew))
    {
      myTZones.Append(theNew);
    }
  }
  return Inserted;
}

//=================================================================================================

void Intf_Interference::Insert(const Intf_SectionPoint& pdeb, const Intf_SectionPoint& pfin)
{
  bool              Inserted = false;
  int               TheLS    = 0;
  bool              Begin    = false;
  Intf_SectionPoint TheBout(pfin);
  int               ils, nd, nf;

  for (ils = 1; ils <= mySLines.Length(); ils++)
  {
    Intf_SectionLine& SL = mySLines(ils);
    nd                   = SL.IsEnd(pdeb);
    nf                   = SL.IsEnd(pfin);
    if (nd == 1)
    {
      if (nf > 1)
      {
        SL.Close();
      }
      Inserted = true;
      TheLS    = ils;
      Begin    = true;
      break;
    }
    else if (nd > 1)
    {
      if (nf == 1)
      {
        SL.Close();
      }
      Inserted = true;
      TheLS    = ils;
      Begin    = false;
      break;
    }
    else if (nf == 1)
    {
      Inserted = true;
      TheLS    = ils;
      Begin    = true;
      TheBout  = pdeb;
      break;
    }
    else if (nf > 1)
    {
      Inserted = true;
      TheLS    = ils;
      Begin    = false;
      TheBout  = pdeb;
      break;
    }
  }

  if (!Inserted)
  {
    Intf_SectionLine LaLS;
    LaLS.Append(pdeb);
    LaLS.Append(pfin);
    mySLines.Append(LaLS);
  }
  else
  {
    nd = 0;
    for (ils = 1; ils <= mySLines.Length(); ils++)
    {
      if (ils != TheLS)
      {
        nd = mySLines(ils).IsEnd(TheBout);
        if (nd == 1)
        {
          if (Begin)
          {
            mySLines(TheLS).Reverse();
          }
          mySLines(ils).Prepend(mySLines(TheLS));
          break;
        }
        else if (nd > 1)
        {
          if (!Begin)
          {
            mySLines(TheLS).Reverse();
          }
          mySLines(ils).Append(mySLines(TheLS));
          break;
        }
      }
    }
    if (nd > 0)
    {
      mySLines.Remove(TheLS);
    }
    else
    {
      if (Begin)
      {
        mySLines(TheLS).Prepend(TheBout);
      }
      else
      {
        mySLines(TheLS).Append(TheBout);
      }
    }
  }
}

//----------------------------------------------------
//
//----------------------------------------------------
bool Intf_Interference::Contains(const Intf_SectionPoint& LePnt) const
{
  //-- LePnt.Dump(0);
  for (int l = 1; l <= mySLines.Length(); l++)
  {
    if (mySLines(l).Contains(LePnt))
    {
      return true;
    }
  }
  for (int t = 1; t <= myTZones.Length(); t++)
  {
    //-- myTZones(t).Dump(2);
    if (myTZones(t).Contains(LePnt))
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------
//
//----------------------------------------------------
void Intf_Interference::Dump() const
{
  std::cout << "Mes SectionPoint :" << '\n';
  for (int p = 1; p <= mySPoins.Length(); p++)
  {
    mySPoins(p).Dump(2);
  }
  std::cout << "Mes SectionLine :" << '\n';
  for (int l = 1; l <= mySLines.Length(); l++)
  {
    mySLines(l).Dump(2);
  }
  std::cout << "Mes TangentZone :" << '\n';
  for (int t = 1; t <= myTZones.Length(); t++)
  {
    myTZones(t).Dump(2);
  }
}

//=================================================================================================

Intf_InterferenceBreakerScope::Intf_InterferenceBreakerScope(const Message_ProgressScope* theScope)
{
  myPrev = t_intfBreaker;
  Intf_Interference::SetBreaker(theScope);
}

//=================================================================================================

Intf_InterferenceBreakerScope::~Intf_InterferenceBreakerScope()
{
  Intf_Interference::SetBreaker(myPrev);
}
