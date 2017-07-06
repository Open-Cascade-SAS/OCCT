// Created on: 1992-10-14
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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


#include <MAT_Bisector.hxx>
#include <MAT_Edge.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Edge,Standard_Transient)

MAT_Edge::MAT_Edge()
{
}
    
void MAT_Edge::EdgeNumber(const Standard_Integer anumber)
{
  theedgenumber = anumber;
}

void MAT_Edge::FirstBisector(const Handle(MAT_Bisector)& abisector)
{
  thefirstbisector = abisector;
}

void MAT_Edge::SecondBisector(const Handle(MAT_Bisector)& abisector)
{
  thesecondbisector = abisector;
}

void MAT_Edge::Distance(const Standard_Real adistance)
{
  thedistance = adistance;
}

void MAT_Edge::IntersectionPoint(const Standard_Integer apoint)
{
  theintersectionpoint = apoint;
}

Standard_Integer MAT_Edge::EdgeNumber() const
{
  return theedgenumber;
}

Handle(MAT_Bisector) MAT_Edge::FirstBisector() const
{
  return thefirstbisector;
}

Handle(MAT_Bisector) MAT_Edge::SecondBisector() const
{
  return thesecondbisector;
}

Standard_Real MAT_Edge::Distance() const
{
  return thedistance;
}

Standard_Integer MAT_Edge::IntersectionPoint() const
{
  return theintersectionpoint;
}

void MAT_Edge::Dump(const Standard_Integer,
		     const Standard_Integer) const
{
}

