// Created on: 1991-06-21
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

#include <Intf_SectionLine.hxx>
#include <Intf_SectionPoint.hxx>

//=================================================================================================

Intf_SectionLine::Intf_SectionLine()
    : closed(Standard_False)
{
}

//=================================================================================================

Intf_SectionLine::Intf_SectionLine(const Intf_SectionLine& Other)
    : closed(Standard_False)
{
  myPoints = Other.myPoints;
}

//=================================================================================================

void Intf_SectionLine::Append(const Intf_SectionPoint& Pi)
{
  myPoints.Append(Pi);
}

//=================================================================================================

void Intf_SectionLine::Append(Intf_SectionLine& LS)
{
  myPoints.Append(LS.myPoints);
}

//=================================================================================================

void Intf_SectionLine::Prepend(const Intf_SectionPoint& Pi)
{
  myPoints.Prepend(Pi);
}

//=================================================================================================

void Intf_SectionLine::Prepend(Intf_SectionLine& LS)
{
  myPoints.Prepend(LS.myPoints);
}

//=================================================================================================

void Intf_SectionLine::Reverse()
{
  myPoints.Reverse();
}

//=================================================================================================

void Intf_SectionLine::Close()
{
  closed = Standard_True;
}

//=================================================================================================

const Intf_SectionPoint& Intf_SectionLine::GetPoint(const Standard_Integer index) const
{
  return myPoints.Value(index);
}

//=================================================================================================

Standard_Boolean Intf_SectionLine::IsClosed() const
{
  // return closed;
  //  On ne peut fermer une ligne de section inseree dans une liste car
  //  la fonction Value() copie l'element avant d'appeler la fonction.
  //  C'est donc la copie qui est modifiee.
  //  Pour la sequence myPoints on a un pointeur donc le pb ne se pose pas.

  return (myPoints.First() == myPoints.Last());
}

//=================================================================================================

Standard_Boolean Intf_SectionLine::Contains(const Intf_SectionPoint& ThePI) const
{
  for (Standard_Integer i = 1; i <= myPoints.Length(); i++)
    if (ThePI.IsEqual(myPoints(i)))
      return Standard_True;
  return Standard_False;
}

//=================================================================================================

Standard_Integer Intf_SectionLine::IsEnd(const Intf_SectionPoint& ThePI) const
{
  if (myPoints.First().IsEqual(ThePI))
    return 1;
  if (myPoints.Last().IsEqual(ThePI))
    return myPoints.Length();
  return 0;
}

//=================================================================================================

Standard_Boolean Intf_SectionLine::IsEqual(const Intf_SectionLine& Other) const
{
  if (myPoints.Length() != Other.myPoints.Length())
    return Standard_False;
  for (Standard_Integer i = 1; i <= myPoints.Length(); i++)
    if (!myPoints(i).IsEqual(Other.myPoints(i)))
      return Standard_False;
  return Standard_True;
}

//=================================================================================================

void Intf_SectionLine::Dump(const Standard_Integer Indent) const
{
  for (Standard_Integer id = 0; id < Indent; id++)
    std::cout << " ";
  std::cout << "LS ";
  if (IsClosed())
    std::cout << "Closed :" << std::endl;
  else
    std::cout << "Open :" << std::endl;
  for (Standard_Integer p = 1; p <= myPoints.Length(); p++)
  {
    myPoints.Value(p).Dump(Indent + 2);
  }
}
