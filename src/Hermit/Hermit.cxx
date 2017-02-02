// Created on: 1997-01-15
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt2d.hxx>
#include <Hermit.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_Real.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

#include <algorithm>
//=======================================================================
//function : HermiteCoeff
//purpose  : calculate  the Hermite coefficients of degree 3 from BS and
//    	     store them in TAB(4 coefficients)
//=======================================================================
static void HermiteCoeff(const Handle(Geom_BSplineCurve)& BS,
			 TColStd_Array1OfReal&            TAB)
     
{
  TColStd_Array1OfReal    Knots(1,BS->NbKnots());         
  TColStd_Array1OfReal    Weights(1,BS->NbPoles()); 
  TColStd_Array1OfInteger Mults(1,BS->NbKnots());
  Standard_Integer        Degree,Index0,Index1;                     // denominateur value for u=0 & u=1
  Standard_Real           Denom0,Denom1,                            // denominator value for u=0 & u=1
                          Deriv0,Deriv1 ;                           // derivative denominator value for u=0 & 1
  Standard_Boolean        Periodic;
  
  BS->Knots(Knots);
  BSplCLib::Reparametrize(0.0,1.0,Knots);                           //affinity on the nodal vector
  BS->Weights(Weights);
  BS->Multiplicities(Mults);
  Degree   = BS->Degree();
  Periodic = BS->IsPeriodic();
  Index0   = BS->FirstUKnotIndex();
  Index1   = BS->LastUKnotIndex()-1;

  BSplCLib::D1(0.0,Index0,Degree,Periodic,Weights,BSplCLib::NoWeights(),Knots,&Mults,Denom0,Deriv0);
  BSplCLib::D1(1.0,Index1,Degree,Periodic,Weights,BSplCLib::NoWeights(),Knots,&Mults,Denom1,Deriv1);
  TAB(0) = 1/Denom0;                                                //Hermit coefficients
  TAB(1) = -Deriv0/(Denom0*Denom0);
  TAB(2) = -Deriv1/(Denom1*Denom1);
  TAB(3) = 1/Denom1;

}

//=======================================================================
//function : HermiteCoeff
//purpose  : calculate  the Hermite coefficients of degree 3 from BS and
//    	     store them in TAB(4 coefficients)
//=======================================================================

static void HermiteCoeff(const Handle(Geom2d_BSplineCurve)& BS,
			 TColStd_Array1OfReal&              TAB)
     
{
  TColStd_Array1OfReal    Knots(1,BS->NbKnots());         
  TColStd_Array1OfReal    Weights(1,BS->NbPoles()); 
  TColStd_Array1OfInteger Mults(1,BS->NbKnots());
  Standard_Integer        Degree,Index0,Index1;
  Standard_Real           Denom0,Denom1,                            // denominateur value for u=0 & u=1
                          Deriv0,Deriv1 ;                           // denominator value for u=0 & u=1
  Standard_Boolean        Periodic;                                 // derivative denominatur value for u=0 & 1
  
  BS->Knots(Knots);
  BSplCLib::Reparametrize(0.0,1.0,Knots);                           //affinity on the nodal vector
  BS->Weights(Weights);
  BS->Multiplicities(Mults);
  Degree   = BS->Degree();
  Periodic = BS->IsPeriodic();
  Index0   = BS->FirstUKnotIndex();
  Index1   = BS->LastUKnotIndex()-1;

  BSplCLib::D1(0.0,Index0,Degree,Periodic,Weights,BSplCLib::NoWeights(),Knots,&Mults,Denom0,Deriv0);
  BSplCLib::D1(1.0,Index1,Degree,Periodic,Weights,BSplCLib::NoWeights(),Knots,&Mults,Denom1,Deriv1);
  TAB(0) = 1/Denom0;                                                //Hermit coefficients
  TAB(1) = -Deriv0/(Denom0*Denom0);
  TAB(2) = -Deriv1/(Denom1*Denom1);
  TAB(3) = 1/Denom1;

}

//=======================================================================
//function : SignDenom
//purpose  : give the sign of Herm(0) True=Positive
//=======================================================================

static Standard_Boolean SignDenom(const TColgp_Array1OfPnt2d& Poles)

{
  Standard_Boolean Result;
  
  if (Poles(0).Y()<0)                                             
    Result=Standard_False;                                        
  else  Result=Standard_True;
  return Result;
}

//=======================================================================
//function : Polemax
//purpose  : give the max and the min of the Poles (by their index) 
//=======================================================================


static void Polemax(const TColgp_Array1OfPnt2d& Poles,
		    Standard_Integer&           min,
		    Standard_Integer&           max)

{
//  Standard_Integer i,index=0;
  Standard_Integer i;
  Standard_Real Max,Min;                                         //intermediate value of max and min ordinates
  min=0;max=0;                                                   //initialisation of the indices
  
  Min=Poles(0).Y();                                              //initialisation of the intermediate value
  Max=Poles(0).Y();
  for (i=1;i<=(Poles.Length()-1);i++){
    if (Poles(i).Y()<Min){
      Min=Poles(i).Y();
      min=i;
    }
    if (Poles(i).Y()>Max){
      Max=Poles(i).Y();
      max=i;
    } 
  }
}

//=======================================================================
//function : PolyTest
//purpose  : give the knots U4 and U5 to insert to a(u)
//=======================================================================
   

static void PolyTest(const TColStd_Array1OfReal&         Herm,
		     const Handle(Geom_BSplineCurve)&    BS,
		     Standard_Real&                      U4,
		     Standard_Real&                      U5,
		     Standard_Integer&                   boucle, 
		     const Standard_Real                 TolPoles,
//		     const Standard_Real                 TolKnots,
		     const Standard_Real                 ,
		     const Standard_Real                 Ux, 
		     const Standard_Real                 Uy)

{
  Standard_Integer               index,i,                
                                 I1=0,I2=0,I3=0,I4=0;    //knots index
  TColgp_Array1OfPnt2d           Polesinit(0,3) ;        
  Handle(TColStd_HArray1OfReal)  Knots;                  //array of the BSpline knots + the ones inserted
  Standard_Integer               cas=0,mark=0,dercas=0,  //loop marks
                                 min,max;                //Pole min and max indices
  Standard_Real                  Us1,Us2,a;              //bounderies value of the knots to be inserted
  
  U4=0.0;U5=1.0;                                         //default value
  if (Ux!=1.0){
    BS->LocateU(Ux,0.0,I1,I2);                          //localization of the inserted knots 
    if (Uy!=0.0)
      BS->LocateU(Uy,0.0,I3,I4);
  }

  if (I1==I2)                                            //definition and filling of the 
    if((I3==I4)||(I3==0)){                                //array of knots
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots());
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
    }
    else{
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+1);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Uy);
    }
  else{
    if((I3==I4)||(I3==0)){
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+1);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Ux);
    }
    else{
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+2);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Ux);
      Knots->SetValue(BS->NbKnots()+2,Uy);
    }
  }

  TColStd_Array1OfReal knots(1,Knots->Length());
  knots=Knots->ChangeArray1();

  //sort of the array of knots
  std::sort (knots.begin(), knots.end());

  Polesinit(0).SetCoord(0.0,Herm(0));                 //poles of the Hermite polynome in the BSpline form
  Polesinit(1).SetCoord(0.0,Herm(0)+Herm(1)/3.0);
  Polesinit(2).SetCoord(0.0,Herm(3)-Herm(2)/3.0);
  Polesinit(3).SetCoord(0.0,Herm(3));

                              //loop to check the tolerances on poles
  if (TolPoles!=0.0){
    Polemax(Polesinit,min,max);
    Standard_Real  Polemin=Polesinit(min).Y();
    Standard_Real  Polemax=Polesinit(max).Y();
    if (((Polemax)>=((1/TolPoles)*Polemin))||((Polemin==0.0)&&(Polemax>=(1/TolPoles)))){
      if (Polesinit(0).Y()>=(1/TolPoles)*Polesinit(3).Y()||Polesinit(0).Y()<=TolPoles*Polesinit(3).Y())
        throw Standard_DimensionError("Hermit Impossible Tolerance");
      if ((max==0)||(max==3))
      {
        for (i=0;i<=3;i++)
          Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-TolPoles*Polemax));
      }
      else if ((max==1)||(max==2)) {
        if ((min==0)||(min==3))
        {
          for (i=0;i<=3;i++)
            Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-(1/TolPoles)*Polemin));
        }
        else{                                                                  
          if ((TolPoles*Polemax<Polesinit(0).Y())&&(TolPoles*Polemax<Polesinit(3).Y())){
            for (i=0;i<=3;i++)                                             
              Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-TolPoles*Polemax));
            mark=1;
          }
          if ((1/TolPoles*Polemin>Polesinit(0).Y())&&(1/TolPoles*Polemin>Polesinit(3).Y())&&(mark==0)){
            for (i=0;i<=3;i++)                                             
              Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-1/TolPoles*Polemin));
            mark=1;
          }
          if (mark==0){
            Standard_Real Pole0,Pole3;
            Pole0=Polesinit(0).Y();
            Pole3=Polesinit(3).Y();
            if (Pole0<3){                         
              a=Log10(Pole3/Pole0);
              if (boucle==2)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole3*(Pow(10.0,(-0.5*Log10(TolPoles)-a/2.0))))); 
              }
              if (boucle==1)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole0*(Pow(10.0,(a/2.0+0.5*Log10(TolPoles)))))); 
                dercas=1;
              }
            }
            if (Pole0>Pole3)
            {
              a=Log10(Pole0/Pole3);                           
              if (boucle==2)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole0*(Pow(10.0,(-0.5*Log10(TolPoles)-a/2.0)))));
              }
              if (boucle==1)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole3*(Pow(10.0,(a/2.0+0.5*Log10(TolPoles)))))); 
                dercas=1;
              }
            }
          }
        }
      }
    }
  } // end of the loop
  
  if (!SignDenom(Polesinit)) //invertion of the polynome sign
  {
    for (index=0;index<=3;index++)
      Polesinit(index).SetCoord(0.0,-Polesinit(index).Y());
  }

  // loop of positivity
  if ((Polesinit(1).Y()<0.0)&&(Polesinit(2).Y()>=0.0))
  {
    Us1=Polesinit(0).Y()/(Polesinit(0).Y()-Polesinit(1).Y());
    if (boucle==2)
      Us1=Us1*knots(2);
    if (boucle==1)
      if (Ux!=0.0)
	Us1=Us1*Ux;
    BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
    if (I1<2)
      U4=Us1;
    else
      U4=knots(I1);
  }
  
  if ((Polesinit(1).Y()>=0.0)&&(Polesinit(2).Y()<0.0))
  {
    Us2=Polesinit(2).Y()/(Polesinit(2).Y()-Polesinit(3).Y());
    if (boucle==2)
      Us2=knots(knots.Length()-1)+Us2*(1-knots(knots.Length()-1));
    if (boucle==1)
      if (Ux!=0.0)
	Us2=Uy+Us2*(1-Uy);
    BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I1,Us2);
    if (I1>=(knots.Length()-1))
      U5=Us2;
    else
      U5=knots(I1+1);
  }
  
  if (dercas==1)
    boucle++;
  
  if ((Polesinit(1).Y()<0.0)&&(Polesinit(2).Y()<0.0)){
    Us1=Polesinit(0).Y()/(Polesinit(0).Y()-Polesinit(1).Y());
    Us2=Polesinit(2).Y()/(Polesinit(2).Y()-Polesinit(3).Y());
    if (boucle!=0)
      if (Ux!=0.0){
	Us1=Us1*Ux;
	Us2=Uy+Us2*(1-Uy);
      }
    if (Us2<=Us1){                                   
      BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
      if (knots(I1)>=Us2)                                    //insertion of one knot for the two poles
	U4=knots(I1);
      else{
	if (I1>=2){                                          //insertion to the left and
	  U4=knots(I1);                                      //to the right without a new knot
	  BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I3,Us2);
	  if (I3<(BS->NbKnots()-1)){
	    U5=knots(I3+1);
	    cas=1;
	  }
	}
	if(cas==0)                                          //insertion of only one new knot
	  U4=(Us1+Us2)/2;
      }
    }
    else{                                                      //insertion of two knots
      BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
      if (I1>=2)                                                 
	U4=knots(I1);
      else
	U4=Us1;
      BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I3,Us2);
      if (I3<(BS->NbKnots()-1))
	U5=knots(I3+1);
      else
	U5=Us2;
    }
  }
} 

//=======================================================================
//function : PolyTest
//purpose  : give the knots U4 and U5 to insert to a(u)
//=======================================================================
   

static void PolyTest(const TColStd_Array1OfReal&        Herm,
		     const Handle(Geom2d_BSplineCurve)& BS,
		     Standard_Real&                     U4,
		     Standard_Real&                     U5,
		     Standard_Integer&                  boucle,
		     const Standard_Real                TolPoles,
//		     const Standard_Real                TolKnots,
		     const Standard_Real                ,
		     const Standard_Real                Ux, 
		     const Standard_Real                Uy)

{
  Standard_Integer               index,i,
                                 I1=0,I2=0,I3=0,I4=0;    //knots index
  TColgp_Array1OfPnt2d           Polesinit(0,3) ;
  Handle(TColStd_HArray1OfReal)  Knots;                  //array of the BSpline knots + the ones inserted
  Standard_Integer               cas=0,mark=0,dercas=0,  //loop marks
                                 min,max;                //Pole min and max indices
  Standard_Real                  Us1,Us2,a;              //bounderies value of the knots to be inserted
  
  U4=0.0;U5=1.0;                                         //default value
  if (Ux!=1.0){
    BS->LocateU(Ux,0.0,I1,I2);                          //localization of the inserted knots
    if (Uy!=0.0)
      BS->LocateU(Uy,0.0,I3,I4);
  }

  if (I1==I2)                                            //definition and filling of the 
  {
    if((I3==I4)||(I3==0)){                               //array of knots
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots());
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
    }
    else{
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+1);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Uy);
    }
  }
  else
  {
    if((I3==I4)||(I3==0)){
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+1);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Ux);
    }
    else{
      Knots=new TColStd_HArray1OfReal(1,BS->NbKnots()+2);
      for (i=1;i<=BS->NbKnots();i++)
	Knots->SetValue(i,BS->Knot(i));
      Knots->SetValue(BS->NbKnots()+1,Ux);
      Knots->SetValue(BS->NbKnots()+2,Uy);
    }
  }

  TColStd_Array1OfReal knots(1,Knots->Length());
  knots=Knots->ChangeArray1();

  //sort of the array of knots
  std::sort (knots.begin(), knots.end());

  Polesinit(0).SetCoord(0.0,Herm(0));              //poles of the Hermite polynome in the BSpline form
  Polesinit(1).SetCoord(0.0,Herm(0)+Herm(1)/3.0);
  Polesinit(2).SetCoord(0.0,Herm(3)-Herm(2)/3.0);
  Polesinit(3).SetCoord(0.0,Herm(3));

  // loop to check the tolerances on poles
  if (TolPoles!=0.0)
  {
    Polemax(Polesinit,min,max);
    Standard_Real  Polemin=Polesinit(min).Y();
    Standard_Real  Polemax=Polesinit(max).Y();
    if (((Polemax)>=((1/TolPoles)*Polemin))||((Polemin==0.0)&&(Polemax>=(1/TolPoles))))
    {
      if (Polesinit(0).Y()>=(1/TolPoles)*Polesinit(3).Y()||Polesinit(0).Y()<=TolPoles*Polesinit(3).Y())
        throw Standard_DimensionError("Hermit Impossible Tolerance");
      if ((max==0)||(max==3))
      {
        for (i=0;i<=3;i++)
          Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-TolPoles*Polemax));
      }
      else if ((max==1)||(max==2))
      {
        if ((min==0)||(min==3))
        {
          for (i=0;i<=3;i++)
            Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-(1/TolPoles)*Polemin));
        }
        else
        {
          if ((TolPoles*Polemax<Polesinit(0).Y())&&(TolPoles*Polemax<Polesinit(3).Y()))
          {
            for (i=0;i<=3;i++)                                             
              Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-TolPoles*Polemax));
            mark=1;
          }

          if ((1/TolPoles*Polemin>Polesinit(0).Y())&&(1/TolPoles*Polemin>Polesinit(3).Y())&&(mark==0))
          {
            for (i=0;i<=3;i++)                                             
              Polesinit(i).SetCoord(0.0,(Polesinit(i).Y()-1/TolPoles*Polemin));
            mark=1;
          }
          if (mark==0)
          {
            Standard_Real Pole0,Pole3;
            Pole0=Polesinit(0).Y();
            Pole3=Polesinit(3).Y();
            if (Pole0<3)
            {
              a=Log10(Pole3/Pole0);
              if (boucle==2)
              {
                for (i=0;i<=3;i++)
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole3*(Pow(10.0,(-0.5*Log10(TolPoles)-a/2.0)))));
              }
              if (boucle==1)
              {
                for (i=0;i<=3;i++)
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole0*(Pow(10.0,(a/2.0+0.5*Log10(TolPoles)))))); 
                dercas=1;
              }
            }
            if (Pole0>Pole3)
            {
              a=Log10(Pole0/Pole3);                           
              if (boucle==2)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole0*(Pow(10.0,(-0.5*Log10(TolPoles)-a/2.0))))); 
              }
              else if (boucle==1)
              {
                for (i=0;i<=3;i++)                                                    
                  Polesinit(i).SetCoord(0.0, Polesinit(i).Y()-(Pole3*(Pow(10.0,(a/2.0+0.5*Log10(TolPoles)))))); 
                dercas=1;
              }
            }
          }
        }
      }
    }
  } // end of the loop
  
  if (!SignDenom(Polesinit)) // invertion of the polynome sign
  {
    for (index=0;index<=3;index++)
      Polesinit(index).SetCoord(0.0,-Polesinit(index).Y());
  }

  // boucle de positivite
  if ((Polesinit(1).Y()<0.0)&&(Polesinit(2).Y()>=0.0))
  {
    Us1=Polesinit(0).Y()/(Polesinit(0).Y()-Polesinit(1).Y());
    if (boucle==2)
      Us1=Us1*knots(2);
    if (boucle==1)
      if (Ux!=0.0)
	Us1=Us1*Ux;
    BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
    if (I1<2)
      U4=Us1;
    else
      U4=knots(I1);
  }
  
  if ((Polesinit(1).Y()>=0.0)&&(Polesinit(2).Y()<0.0))
  {
    Us2=Polesinit(2).Y()/(Polesinit(2).Y()-Polesinit(3).Y());
    if (boucle==2)
      Us2=knots(knots.Length()-1)+Us2*(1-knots(knots.Length()-1));
    if (boucle==1)
      if (Ux!=0.0)
	Us2=Uy+Us2*(1-Uy);
    BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I1,Us2);
    if (I1>=(knots.Length()-1))
      U5=Us2;
    else
      U5=knots(I1+1);
  }
  
  if (dercas==1)
    boucle++;
  
  if ((Polesinit(1).Y()<0.0)&&(Polesinit(2).Y()<0.0)){
    Us1=Polesinit(0).Y()/(Polesinit(0).Y()-Polesinit(1).Y());
    Us2=Polesinit(2).Y()/(Polesinit(2).Y()-Polesinit(3).Y());
    if (boucle!=0)
      if (Ux!=0.0){
	Us1=Us1*Ux;
	Us2=Uy+Us2*(1-Uy);
      }
    if (Us2<=Us1){                                   
      BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
      if (knots(I1)>=Us2)                                    //insertion of one knot for the two poles
	U4=knots(I1);
      else{
	if (I1>=2){                                          //insertion to the left and
	  U4=knots(I1);                                    //to the right without a new knot
	  BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I3,Us2);
	  if (I3<(BS->NbKnots()-1)){
	    U5=knots(I3+1);
	    cas=1;
	  }
	}
	if(cas==0)                                          //insertion of only one new knot
	  U4=(Us1+Us2)/2;
      }
    }
    else{                                                      //insertion of two knots
      BSplCLib::LocateParameter(3,knots,Us1,Standard_False,1,knots.Length(),I1,Us1);
      if (I1>=2)                                                 
	U4=knots(I1);
      else
	U4=Us1;
      BSplCLib::LocateParameter(3,knots,Us2,Standard_False,1,knots.Length(),I3,Us2);
      if (I3<(BS->NbKnots()-1))
	U5=knots(I3+1);
      else
	U5=Us2;
    }
  }
} 
                               
//=======================================================================
//function : InsertKnots
//purpose  : insert the knots in BS knot sequence if they are not null. 
//======================================================================= 
 
static void InsertKnots(Handle(Geom2d_BSplineCurve)& BS,
			const Standard_Real          U4,
			const Standard_Real          U5)

{
  if (U4!=0.0)                                  //insertion of :0 knot if U4=0
    BS->InsertKnot(U4);	                        //              1 knot if U4=U5 
  if ((U5!=1.0)&&(U5!=U4))                      //              2 knots otherwise
    BS->InsertKnot(U5);
  
}

//=======================================================================
//function : MovePoles
//purpose  : move the poles above the x axis
//======================================================================= 

static void MovePoles(Handle(Geom2d_BSplineCurve)& BS)

{
  gp_Pnt2d  P ;
//  Standard_Integer i,index; 
  Standard_Integer i; 

  for (i=3;i<=(BS->NbPoles()-2);i++){
    P.SetCoord(1,(BS->Pole(i).Coord(1)));           //raising of the no constrained poles to
    P.SetCoord(2,(BS->Pole(1).Coord(2)));           //the first pole level          
    BS->SetPole(i,P);
  }
}

//=======================================================================
//function : Solution
//purpose  :
//======================================================================= 

Handle(Geom2d_BSplineCurve) Hermit::Solution(const Handle(Geom_BSplineCurve)& BS,
					     const Standard_Real              TolPoles,
					     const Standard_Real              TolKnots)
     
{
  TColStd_Array1OfReal       Herm(0,3);
  Standard_Real              Upos1=0.0, Upos2=1.0,   //positivity knots
                             Ux=0.0,    Uy=1.0,
                             Utol1=0.0, Utol2=1.0,   //tolerance knots
                             Uint1=0.0, Uint2=1.0;   //tolerance knots for the first loop
  Standard_Integer           boucle=1;               //loop mark
  TColStd_Array1OfReal       Knots(1,2);             
  TColStd_Array1OfInteger    Multiplicities(1,2);
  TColgp_Array1OfPnt2d       Poles(1,4);
  Standard_Integer           zeroboucle = 0 ;
  HermiteCoeff(BS,Herm);                             //computation of the Hermite coefficient

  Poles(1).SetCoord(0.0,Herm(0));                    //poles of the Hermite polynome in the BSpline form
  Poles(2).SetCoord(0.0,Herm(0)+Herm(1)/3.0);
  Poles(3).SetCoord(0.0,Herm(3)-Herm(2)/3.0);
  Poles(4).SetCoord(0.0,Herm(3));
  Knots(1)=0.0;
  Knots(2)=1.0;
  Multiplicities(1)=4;
  Multiplicities(2)=4;

  Handle(Geom2d_BSplineCurve)  BS1=new Geom2d_BSplineCurve(Poles,Knots,Multiplicities,3);//creation of the basic
  Handle(Geom2d_BSplineCurve)  BS2=new Geom2d_BSplineCurve(Poles,Knots,Multiplicities,3);//BSpline without modif

  PolyTest(Herm,BS,Upos1,Upos2,zeroboucle,Precision::Confusion(),Precision::Confusion(),1.0,0.0);//computation of the positivity knots
  InsertKnots(BS2,Upos1,Upos2);                      //and insertion
    
  if (Upos1!=0.0)
    if (Upos2!=1.0){
      Ux=Min(Upos1,Upos2);
      Uy=Max(Upos1,Upos2);
    }
    else{
      Ux=Upos1;
      Uy=Upos1;
    }
  else{
    Ux=Upos2;
    Uy=Upos2;
  }

  Herm(0)=BS2->Pole(1).Y();                           //computation of the Hermite coefficient on the
  Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());      //positive BSpline
  Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
  Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
  
  PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Ux,Uy);          //computation of the tolerance knots
  InsertKnots(BS2,Utol1,Utol2);                                          //and insertion
  
  if (boucle==2){                                     //insertion of two knots
    Herm(0)=BS2->Pole(1).Y();
    Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());
    Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
    Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
    if (Utol1==0.0){
      Uint2=Utol2;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint2,0.0);
    }
    else{
      Uint1=Utol1;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint1,0.0);
    }
    InsertKnots(BS2,Utol1,Utol2);
  }
  if ((BS2->Knot(2)<TolKnots)||(BS2->Knot(BS2->NbKnots()-1)>(1-TolKnots))) //checking of the knots tolerance
    throw Standard_DimensionError("Hermit Impossible Tolerance");
  else{
    if ((Upos2==1.0)&&(Utol2==1.0)&&(Uint2==1.0))    //test on the final inserted knots
      InsertKnots(BS1,BS2->Knot(2),1.0);
    else{
      if ((Upos1==0.0)&&(Utol1==0.0)&&(Uint1==0.0))
	InsertKnots(BS1,BS2->Knot(BS2->NbKnots()-1),1.0);
      else
	InsertKnots(BS1,BS2->Knot(BS2->NbKnots()-1),BS2->Knot(2));
    }
    MovePoles(BS1);                                  //relocation of the no-contrained knots
  }
  return BS1;
}


//=======================================================================
// Solution
//======================================================================= 

Handle(Geom2d_BSplineCurve) Hermit::Solution(const Handle(Geom2d_BSplineCurve)& BS,
					     const Standard_Real                TolPoles,
					     const Standard_Real                TolKnots)

{
  TColStd_Array1OfReal       Herm(0,3);
  Standard_Real              Upos1=0.0, Upos2=1.0,   //positivity knots
                             Ux=0.0,    Uy=1.0, 
                             Utol1=0.0, Utol2=1.0,   //tolerance knots
                             Uint1=0.0, Uint2=1.0;   //tolerance knots for the first loop
  Standard_Integer           boucle=1;               //loop mark
  TColStd_Array1OfReal       Knots(1,2);             
  TColStd_Array1OfInteger    Multiplicities(1,2);
  TColgp_Array1OfPnt2d       Poles(1,4);
  Standard_Integer           zeroboucle = 0 ;
  HermiteCoeff(BS,Herm);                             //computation of the Hermite coefficient

  Poles(1).SetCoord(0.0,Herm(0));                    //poles of the Hermite polynome in the BSpline form
  Poles(2).SetCoord(0.0,Herm(0)+Herm(1)/3.0);
  Poles(3).SetCoord(0.0,Herm(3)-Herm(2)/3.0);
  Poles(4).SetCoord(0.0,Herm(3));
  Knots(1)=0.0;
  Knots(2)=1.0;
  Multiplicities(1)=4;
  Multiplicities(2)=4;

  Handle(Geom2d_BSplineCurve)  BS1=new Geom2d_BSplineCurve(Poles,Knots,Multiplicities,3);//creation of the basic
  Handle(Geom2d_BSplineCurve)  BS2=new Geom2d_BSplineCurve(Poles,Knots,Multiplicities,3);//BSpline without modif

  PolyTest(Herm,BS,Upos1,Upos2,zeroboucle,Precision::Confusion(),Precision::Confusion(),1.0,0.0);//computation of the positivity knots
  InsertKnots(BS2,Upos1,Upos2);                      //and insertion
    
  if (Upos1!=0.0)
    if (Upos2!=1.0){
      Ux=Min(Upos1,Upos2);
      Uy=Max(Upos1,Upos2);
    }
    else{
      Ux=Upos1;
      Uy=Upos1;
    }
  else{
    Ux=Upos2;
    Uy=Upos2;
  }
  
  Herm(0)=BS2->Pole(1).Y();                           //computation of the Hermite coefficient on the
  Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());      //positive BSpline
  Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
  Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
  
  PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Ux,Uy);          //computation of the tolerance knots
  InsertKnots(BS2,Utol1,Utol2);                                          //and insertion
  
  if (boucle==2){                                      //insertion of two knots
    Herm(0)=BS2->Pole(1).Y();
    Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());
    Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
    Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
    if (Utol1==0.0){
      Uint2=Utol2;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint2,0.0);
    }
    else{
      Uint1=Utol1;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint1,0.0);
    }
    InsertKnots(BS2,Utol1,Utol2);
  }
  if ((BS2->Knot(2)<TolKnots)||(BS2->Knot(BS2->NbKnots()-1)>(1-TolKnots))) //checking of the knots tolerance
    throw Standard_DimensionError("Hermit Impossible Tolerance");
  else{
    if ((Upos2==1.0)&&(Utol2==1.0)&&(Uint2==1.0))    //test on the final inserted knots
      InsertKnots(BS1,BS2->Knot(2),1.0);
    else{
      if ((Upos1==0.0)&&(Utol1==0.0)&&(Uint1==0.0))
	InsertKnots(BS1,BS2->Knot(BS2->NbKnots()-1),1.0);
      else
	InsertKnots(BS1,BS2->Knot(BS2->NbKnots()-1),BS2->Knot(2));
    }
    MovePoles(BS1);                          //relocation of the no-contrained knots
  }
  return BS1;
}

//=======================================================================
//function : Solutionbis
//purpose  :
//======================================================================= 

void Hermit::Solutionbis(const Handle(Geom_BSplineCurve)& BS,
			 Standard_Real &                  Knotmin,
			 Standard_Real &                  Knotmax,
			 const Standard_Real              TolPoles,
			 const Standard_Real              TolKnots)
     
{
  TColStd_Array1OfReal       Herm(0,3);
  Standard_Real              Upos1=0.0, Upos2=1.0,   //positivity knots
                             Ux=0.0,    Uy=1.0, 
                             Utol1=0.0, Utol2=1.0,   //tolerance knots
                             Uint1=0.0, Uint2=1.0;   //tolerance knots for the first loop
  Standard_Integer           boucle=1;               //loop mark
  TColStd_Array1OfReal       Knots(1,2);             
  TColStd_Array1OfInteger    Multiplicities(1,2);
  TColgp_Array1OfPnt2d       Poles(1,4);
  Standard_Integer           zeroboucle = 0 ;
  HermiteCoeff(BS,Herm);                             //computation of the Hermite coefficient
  
  Poles(1).SetCoord(0.0,Herm(0));                    //poles of the Hermite polynome in the BSpline form
  Poles(2).SetCoord(0.0,Herm(0)+Herm(1)/3.0);
  Poles(3).SetCoord(0.0,Herm(3)-Herm(2)/3.0);
  Poles(4).SetCoord(0.0,Herm(3));
  Knots(1)=0.0;
  Knots(2)=1.0;
  Multiplicities(1)=4;
  Multiplicities(2)=4;
  
  Handle(Geom2d_BSplineCurve)  BS2=new Geom2d_BSplineCurve(Poles,Knots,Multiplicities,3);//creation of the basic
                                                                                         //BSpline without modif
  
  PolyTest(Herm,BS,Upos1,Upos2,zeroboucle,Precision::Confusion(),Precision::Confusion(),1.0,0.0);//computation of the positivity knots
  InsertKnots(BS2,Upos1,Upos2);                      //and insertion
  
  if (Upos1!=0.0)
    if (Upos2!=1.0){
      Ux=Min(Upos1,Upos2);
      Uy=Max(Upos1,Upos2);
    }
    else{
      Ux=Upos1;
      Uy=Upos1;
    }
  else{
    Ux=Upos2;
    Uy=Upos2;
  }
  
  Herm(0)=BS2->Pole(1).Y();                           //computation of the Hermite coefficient on the
  Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());      //positive BSpline
  Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
  Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
  
  PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Ux,Uy);          //computation of the tolerance knots
  InsertKnots(BS2,Utol1,Utol2);                                          //and insertion
  
  if (boucle==2){                                      //insertion of two knots
    Herm(0)=BS2->Pole(1).Y();
    Herm(1)=3*(BS2->Pole(2).Y()-BS2->Pole(1).Y());
    Herm(2)=3*(BS2->Pole(BS2->NbPoles()).Y()-BS2->Pole(BS2->NbPoles()-1).Y());
    Herm(3)=BS2->Pole(BS2->NbPoles()).Y();
    if (Utol1==0.0){
      Uint2=Utol2;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint2,0.0);
    }
    else{
      Uint1=Utol1;
      PolyTest(Herm,BS,Utol1,Utol2,boucle,TolPoles,TolKnots,Uint1,0.0);
    }
    InsertKnots(BS2,Utol1,Utol2);
  }
  if ((BS2->Knot(2)<TolKnots)||(BS2->Knot(BS2->NbKnots()-1)>(1-TolKnots))) //checking of the knots tolerance
    throw Standard_DimensionError("Hermit Impossible Tolerance");
  else{
    if ((Upos2==1.0)&&(Utol2==1.0)&&(Uint2==1.0))    //test on the final inserted knots
      Knotmin=BS2->Knot(2);
    else{
      if ((Upos1==0.0)&&(Utol1==0.0)&&(Uint1==0.0))
	Knotmax=BS2->Knot(BS2->NbKnots()-1);
      else{
	Knotmin=BS2->Knot(2);
	Knotmax=BS2->Knot(BS2->NbKnots()-1);
      }
    }  
  }
}



