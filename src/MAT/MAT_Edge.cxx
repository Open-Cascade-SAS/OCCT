// File:	MAT_Edge.cxx
// Created:	Wed Oct 14 10:30:09 1992
// Author:	Gilles DEBARBOUILLE
//		<gde@bravox>


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

