// Created on: 1999-03-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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



#include <IntPolyh_Edge.ixx>
#include <stdio.h>

IntPolyh_Edge::IntPolyh_Edge() : p1(-1),p2(-1),t1(-1),t2(-1),ia(-1) { } 

IntPolyh_Edge::IntPolyh_Edge(const Standard_Integer a, const Standard_Integer b, 
			     const Standard_Integer c, const Standard_Integer d) : p1(a),p2(b),t1(c),t2(d),ia(-1) { 
}

Standard_Integer IntPolyh_Edge::FirstPoint()  const{ return(p1);} 
Standard_Integer IntPolyh_Edge::SecondPoint() const{ return(p2);}
Standard_Integer IntPolyh_Edge::FirstTriangle()  const{ return(t1);} 
Standard_Integer IntPolyh_Edge::SecondTriangle() const{ return(t2);}
Standard_Integer IntPolyh_Edge::AnalysisFlag() const{ return(ia);}

void IntPolyh_Edge::SetFirstPoint(const Standard_Integer a) { p1=a; } 
void IntPolyh_Edge::SetSecondPoint(const Standard_Integer b) { p2=b; }    
void IntPolyh_Edge::SetFirstTriangle(const Standard_Integer c) { t1=c; } 
void IntPolyh_Edge::SetSecondTriangle(const Standard_Integer d) { t2=d; }    
void IntPolyh_Edge::SetAnalysisFlag(const Standard_Integer e) { ia=e; }    

void IntPolyh_Edge::Dump(const Standard_Integer i) const{ 
  printf("\nEdge(%3d) : P1:%5d   P2:%5d   T1:%5d   T2:%5d    AnalysisFlag:%5d\n",i,p1,p2,t1,t2,ia);
}



