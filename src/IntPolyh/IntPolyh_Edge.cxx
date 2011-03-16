// File:	IntPolyh_Edge.cxx
// Created:	Mon Mar  8 09:32:00 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


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



