// Created on: 1999-03-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
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


#include <IntPolyh_Edge.hxx>

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



