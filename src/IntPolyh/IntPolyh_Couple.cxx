// File:	IntPolyh_Couple.cxx
// Created:	Thu Apr  8 10:34:41 1999
// Author:	Fabrice SERVANT
//		<fst@cleox.paris1.matra-dtv.fr>


#include <IntPolyh_Couple.ixx>
#include <stdio.h>

IntPolyh_Couple::IntPolyh_Couple() : t1(-1),t2(-1),ia(0),angle(-2.0) {  } 

IntPolyh_Couple::IntPolyh_Couple(const Standard_Integer a,const Standard_Integer b) : t1(a),t2(b),ia(0),angle(-2.0) {  }

Standard_Integer IntPolyh_Couple::FirstValue()  const{ return(t1);}
Standard_Integer IntPolyh_Couple::SecondValue() const{ return(t2);}
Standard_Integer IntPolyh_Couple::AnalyseFlagValue() const{ return(ia);}
Standard_Real IntPolyh_Couple::AngleValue() const{ return(angle);}

void IntPolyh_Couple::SetCoupleValue(const Standard_Integer a,const Standard_Integer b) 
{ t1=a; t2=b; } 
void IntPolyh_Couple::SetAnalyseFlag(const Standard_Integer iiaa) { ia=iiaa; }    
void IntPolyh_Couple::SetAngleValue(const Standard_Real ang) { angle=ang; }    

void IntPolyh_Couple::Dump(const Standard_Integer i) const{ 
  printf("\nCouple(%3d) : %5d %5d %3d %5f\n",i,t1,t2,ia,angle);
}




