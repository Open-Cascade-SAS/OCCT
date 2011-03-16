// File:	GeomFill_LocFunction.cxx
// Created:	Mon Feb  2 18:19:56 1998
// Author:	Philippe MANGIN
//		<pmn@sgi29>


#include <GeomFill_LocFunction.ixx>

#include <gp_Mat.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_Array1OfVec2d.hxx>

GeomFill_LocFunction::GeomFill_LocFunction(const Handle(GeomFill_LocationLaw)& Law) 
                                          :V(1,4), DV(1,4), D2V(1,4)

{
  myLaw = Law;
}

 Standard_Boolean GeomFill_LocFunction::D0(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  gp_Mat M;
  Standard_Boolean B;
  B = myLaw->D0(Param, M, V.ChangeValue(1));
  V(2).SetXYZ(M.Column(1));
  V(3).SetXYZ(M.Column(2));
  V(4).SetXYZ(M.Column(3));  
  return B;
}

 Standard_Boolean GeomFill_LocFunction::D1(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  TColgp_Array1OfPnt2d T1(1,1);
  TColgp_Array1OfVec2d T2(1,1);
  gp_Mat M, DM;
  Standard_Boolean B;
  B = myLaw->D1(Param, M, V.ChangeValue(1),
		DM, DV.ChangeValue(1), 
		T1, T2);

  V(2).SetXYZ(M.Column(1));
  V(3).SetXYZ(M.Column(2));
  V(4).SetXYZ(M.Column(3));

  DV(2).SetXYZ(DM.Column(1));
  DV(3).SetXYZ(DM.Column(2));
  DV(4).SetXYZ(DM.Column(3)); 
  return B;
}

 Standard_Boolean GeomFill_LocFunction::D2(const Standard_Real Param,
//					   const Standard_Real First,
					   const Standard_Real ,
//					   const Standard_Real Last) 
					   const Standard_Real ) 
{
  TColgp_Array1OfPnt2d T1(1,1);
  TColgp_Array1OfVec2d T2(1,1), T3(1,1);
  gp_Mat M, DM, D2M;
  Standard_Boolean B;
  B = myLaw->D2(Param, M, V.ChangeValue(1),
		DM, DV.ChangeValue(1),
		D2M, D2V.ChangeValue(1), 
		T1, T2, T3);
  V(2).SetXYZ(M.Column(1));
  V(3).SetXYZ(M.Column(2));
  V(4).SetXYZ(M.Column(3));

  DV(2).SetXYZ(DM.Column(1));
  DV(3).SetXYZ(DM.Column(2));
  DV(4).SetXYZ(DM.Column(3)); 


  D2V(2).SetXYZ(D2M.Column(1));
  D2V(3).SetXYZ(D2M.Column(2));
  D2V(4).SetXYZ(D2M.Column(3)); 

  return B;  
}

 void GeomFill_LocFunction::DN(const Standard_Real Param,
					   const Standard_Real First,
					   const Standard_Real Last,
					   const Standard_Integer Order,
					   Standard_Real& Result,
					   Standard_Integer& Ier) 
{
  Standard_Boolean B;
  Standard_Real * AddrResult =  &Result;
  const Standard_Real * LocalResult=NULL;
    
  Ier = 0;
  switch (Order) {
  case 0:
    {
      B = D0(Param, First, Last);
      LocalResult = (Standard_Real*)(&V(1));
      break;
    }
  case 1:
    {
      B = D1(Param, First, Last);
      LocalResult = (Standard_Real*)(&DV(1));
      break;
    }
  case 2:
    {
      B = D2(Param, First, Last);
      LocalResult = (Standard_Real*)(&D2V(1));
      break;
    }
    default :
      {
	B = Standard_False;
      }
  }
  if (!B) {
    Ier = Order+1;
  }
  for (Standard_Integer ii=0; ii<=11; ii++) {
    AddrResult[ii] =  LocalResult[ii]; 
  }
}
