// File:	TopOpeBRep_Bipoint.cxx
// Created:	Thu Jan  9 15:46:51 1997
// Author:	Jean Yves LEBEY
//		<jyl@bistrox.paris1.matra-dtv.fr>


#include <TopOpeBRep_Bipoint.ixx>
#include <Standard_ProgramError.hxx>

TopOpeBRep_Bipoint::TopOpeBRep_Bipoint():myI1(0),myI2(0){}
TopOpeBRep_Bipoint::TopOpeBRep_Bipoint
(const Standard_Integer I1,const Standard_Integer I2):myI1(I1),myI2(I2){}
Standard_Integer TopOpeBRep_Bipoint::I1() const {
if(myI1<=0)Standard_ProgramError::Raise("TopOpeBRep_Bipoint I1=0");
return myI1;
}
Standard_Integer TopOpeBRep_Bipoint::I2() const {
if(myI2<=0)Standard_ProgramError::Raise("TopOpeBRep_Bipoint I2=0");
return myI2;
}
