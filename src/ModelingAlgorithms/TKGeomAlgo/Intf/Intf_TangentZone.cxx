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

#include <Intf_TangentZone.hxx>

#define DEBUG_TANGENTZONE 0

//=======================================================================
// function : Intf_TangentZone
// purpose  : Constructor of an empty tangent zone.
//=======================================================================

Intf_TangentZone::Intf_TangentZone()
{
  ParamOnFirstMin = ParamOnSecondMin = RealLast();
  ParamOnFirstMax = ParamOnSecondMax = RealFirst();
}

//=======================================================================
// function : Append
// purpose  : Append the section point to the tangent zone.
//=======================================================================

void Intf_TangentZone::Append(const Intf_SectionPoint& Pi)
{
  Result.Append(Pi);
  if (ParamOnFirstMin > Pi.ParamOnFirst())
    ParamOnFirstMin = Pi.ParamOnFirst();
  if (ParamOnSecondMin > Pi.ParamOnSecond())
    ParamOnSecondMin = Pi.ParamOnSecond();

  if (ParamOnFirstMax < Pi.ParamOnFirst())
    ParamOnFirstMax = Pi.ParamOnFirst();
  if (ParamOnSecondMax < Pi.ParamOnSecond())
    ParamOnSecondMax = Pi.ParamOnSecond();
}

//=======================================================================
// function : Append
// purpose  : Merge the TangentZone to the tangent zone.
//=======================================================================

void Intf_TangentZone::Append(const Intf_TangentZone& Tzi)
{
  int Tzi_NumberOfPoints = Tzi.NumberOfPoints();
  for (int ipi = 1; ipi <= Tzi_NumberOfPoints; ipi++)
  {
    PolygonInsert(Tzi.GetPoint(ipi));
  }
}

//=======================================================================
// function : Insert
// purpose  : Insert the section point at his place in the tangent zone.
//=======================================================================

// bool Intf_TangentZone::Insert (const Intf_SectionPoint& Pi)
bool Intf_TangentZone::Insert(const Intf_SectionPoint&)
{
#if DEBUG_TANGENTZONE
  std::cout << " bool Intf_TangentZone::Insert (const Intf_SectionPoint& Pi) ???? " << std::endl;
#endif
  bool Inserted = false;
  /*
    int lpon=0;

    if (Result.Length()<1) {
      Append(Pi);
      Inserted=true;
    }
    else if (Result.Length()==1) {
      if (Pi.IsOnSameEdge(Result(1))) {
        InsertAfter(1, Pi);
        Inserted=true;
      }
    }
    else {
      int lp1, lp2;
      int nbptz=NumberOfPoints();
      for (lp1=1; lp1<=nbptz; lp1++) {
        lp2=(lp1%nbptz)+1;
        if (Pi.IsOnSameEdge(Result(lp1))) {
      lpon=lp1;
      if (Pi.IsOnSameEdge(Result(lp2))) {
        InsertAfter(lp1, Pi);
        Inserted=true;
        break;
      }
        }
      }
    }
    if (!Inserted && lpon>0) {
      InsertAfter(lpon, Pi);
      Inserted=true;
    }
  */
  return Inserted;
}

//=======================================================================
// function : PolygonInsert
// purpose  : Insert the point at his place in the polygonal tangent zone.
//=======================================================================

void Intf_TangentZone::PolygonInsert(const Intf_SectionPoint& Pi)
{
  //  bool  Inserted=false;
  int nbpTz = NumberOfPoints();
  //  int lpi;
  if (nbpTz == 0)
  {
    Append(Pi);
    return;
  }
  if (Pi.ParamOnFirst() >= ParamOnFirstMax)
  {
    Append(Pi);
  }
  else if (Pi.ParamOnFirst() >= ParamOnFirstMin)
  {
    InsertBefore(1, Pi);
  }
  else
  {
    /*----- Trop Long lbr le 13 mai 97
        double PiParamOnFirst  = Pi.ParamOnFirst();
        double PiParamOnSecond = Pi.ParamOnSecond();
        for (lpi=1; lpi<=nbpTz; lpi++) {
          const Intf_SectionPoint& Resultlpi = Result(lpi);
          if (PiParamOnFirst<Resultlpi.ParamOnFirst()) {
        InsertBefore(lpi, Pi);
        Inserted=true;
        break;
          }
          else if (PiParamOnFirst==Resultlpi.ParamOnFirst()) {
        if (PiParamOnSecond==Resultlpi.ParamOnSecond()) {
          Inserted=true;
          break;
        }
          }
        }
        if (!Inserted) {
          Append(Pi);
        }
    ------ */
    Append(Pi); //-- On met les points sans les classer
                //-- si on veut on pourra les reclasser
                //-- ensuite avec un TRI.
  }
}

//=================================================================================================

void Intf_TangentZone::InsertAfter(const int Index, const Intf_SectionPoint& Pi)
{
  Result.InsertAfter(Index, Pi);
  if (ParamOnFirstMin > Pi.ParamOnFirst())
    ParamOnFirstMin = Pi.ParamOnFirst();
  if (ParamOnSecondMin > Pi.ParamOnSecond())
    ParamOnSecondMin = Pi.ParamOnSecond();

  if (ParamOnFirstMax < Pi.ParamOnFirst())
    ParamOnFirstMax = Pi.ParamOnFirst();
  if (ParamOnSecondMax < Pi.ParamOnSecond())
    ParamOnSecondMax = Pi.ParamOnSecond();
}

//=================================================================================================

void Intf_TangentZone::InsertBefore(const int Index, const Intf_SectionPoint& Pi)
{
  Result.InsertBefore(Index, Pi);
  if (ParamOnFirstMin > Pi.ParamOnFirst())
    ParamOnFirstMin = Pi.ParamOnFirst();
  if (ParamOnSecondMin > Pi.ParamOnSecond())
    ParamOnSecondMin = Pi.ParamOnSecond();

  if (ParamOnFirstMax < Pi.ParamOnFirst())
    ParamOnFirstMax = Pi.ParamOnFirst();
  if (ParamOnSecondMax < Pi.ParamOnSecond())
    ParamOnSecondMax = Pi.ParamOnSecond();
}

//=======================================================================
// function : GetPoint
// purpose  : Return the section point of range index in the tangent zone.
//=======================================================================

const Intf_SectionPoint& Intf_TangentZone::GetPoint(const int Index) const
{
  return Result(Index);
}

//=======================================================================
// function : IsEqual
// purpose  : Compare two tangent zone.
//=======================================================================

bool Intf_TangentZone::IsEqual(const Intf_TangentZone& Other) const
{
  if (Result.Length() != Other.Result.Length())
    return false;
  int i;
  for (i = 1; i <= Result.Length(); i++)
  {
    if (!Result(i).IsEqual(Other.Result(i)))
      return false;
  }
  return true;
}

//=================================================================================================

bool Intf_TangentZone::Contains(const Intf_SectionPoint& ThePI) const
{
  int i;
  for (i = 1; i <= Result.Length(); i++)
    if (ThePI.IsEqual(Result(i)))
      return true;
  return false;
}

//=================================================================================================

void Intf_TangentZone::InfoFirst(int& segMin, double& paraMin, int& segMax, double& paraMax) const
{
  ParamOnFirst(paraMin, paraMax);
  segMin  = (int)(std::trunc(paraMin));
  paraMin = paraMin - (double)segMin;
  segMax  = (int)(std::trunc(paraMax));
  paraMax = paraMax - (double)segMax;
}

//=================================================================================================

void Intf_TangentZone::InfoSecond(int& segMin, double& paraMin, int& segMax, double& paraMax) const
{
  ParamOnSecond(paraMin, paraMax);
  segMin  = (int)(std::trunc(paraMin));
  paraMin = paraMin - (double)segMin;
  segMax  = (int)(std::trunc(paraMax));
  paraMax = paraMax - (double)segMax;
}

//=================================================================================================

bool Intf_TangentZone::RangeContains(const Intf_SectionPoint& ThePI) const
{
  double a, b, c, d;
  ParamOnFirst(a, b);
  ParamOnSecond(c, d);
  return a <= ThePI.ParamOnFirst() && ThePI.ParamOnFirst() <= b && c <= ThePI.ParamOnSecond()
         && ThePI.ParamOnSecond() <= d;
}

//=================================================================================================

bool Intf_TangentZone::HasCommonRange(const Intf_TangentZone& Other) const
{
  double a1, b1, c1, d1;
  double a2, b2, c2, d2;
  ParamOnFirst(a1, b1);
  ParamOnSecond(a2, b2);
  Other.ParamOnFirst(c1, d1);
  Other.ParamOnSecond(c2, d2);

  return ((c1 <= a1 && a1 <= d1) || (c1 <= b1 && b1 <= d1) || (a1 <= c1 && c1 <= b1))
         && ((c2 <= a2 && a2 <= d2) || (c2 <= b2 && b2 <= d2) || (a2 <= c2 && c2 <= b2));
}

//=================================================================================================

void Intf_TangentZone::Dump(const int /*Indent*/) const
{
#if DEBUG_TANGENTZONE
  for (int id = 0; id < Indent; id++)
    std::cout << " ";
  std::cout << "TZ \n";
  std::cout << "  ParamOnFirstMin Max    : " << ParamOnFirstMin << " " << ParamOnFirstMax
            << std::endl;
  std::cout << "  ParamOnSecondMin Max   : " << ParamOnSecondMin << " " << ParamOnSecondMax
            << std::endl;
  for (int p = 1; p <= Result.Length(); p++)
  {
    Result(p).Dump(Indent + 2);
  }
#endif
}
