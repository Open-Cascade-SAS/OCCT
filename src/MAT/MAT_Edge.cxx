// Created on: 1992-10-14
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <MAT_Edge.ixx>

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

