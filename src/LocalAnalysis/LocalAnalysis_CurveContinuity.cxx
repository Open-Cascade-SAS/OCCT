// Copyright (c) 1996-1999 Matra Datavision
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

#include <LocalAnalysis_CurveContinuity.ixx>
#include <LocalAnalysis_StatusErrorType.hxx>
#include <GeomLProp_CLProps.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>
#include <StdFail_NotDone.hxx>


/***********************************************************************/
void LocalAnalysis_CurveContinuity::CurvC0( GeomLProp_CLProps& Curv1, 
				      GeomLProp_CLProps& Curv2)
{ myContC0= (Curv1.Value()).Distance(Curv2.Value());  
}

/****************************************************************************/
void LocalAnalysis_CurveContinuity::CurvC1(  GeomLProp_CLProps& Curv1, 
					        GeomLProp_CLProps& Curv2)
{ gp_Vec V1, V2;
  Standard_Real ang; 
  V1 = Curv1.D1();
  V2 = Curv2.D1();
  Standard_Real norm1,norm2; 
  norm1 = V1.Magnitude();
  norm2 = V2.Magnitude();
 
  if ((norm1>myepsnul)&&(norm2>myepsnul))
   { if ( norm1 >= norm2 ) 
       { myLambda1 = norm2 / norm1;}   
       else { myLambda1  = norm1 / norm2;} 
       ang= V1.Angle(V2);
       if (ang>M_PI/2)   myContC1=M_PI-ang;
       else  myContC1=ang;
   }
   else  {myIsDone= Standard_False;
          myErrorStatus=LocalAnalysis_NullFirstDerivative;} 
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvC2(GeomLProp_CLProps& Curv1,  
				     GeomLProp_CLProps& Curv2)
{ gp_Vec V1, V2, V12, V22;
//  gp_Dir D1, D2;
  Standard_Real norm1, norm2, norm12, norm22,ang;
  V1 = Curv1.D1();
  V2 = Curv2.D1();
  V12 = Curv1.D2();
  V22 = Curv2.D2();
  norm1 = V1.Magnitude();
  norm2 = V2.Magnitude();
  norm12 = V12.Magnitude();
  norm22 = V22.Magnitude();

  if ((norm1>myepsnul)&&(norm2>myepsnul))
       {if((norm12>myepsnul)&&(norm22>myepsnul)) 
            {if (norm1 >= norm2 ) 
                { myLambda1 = norm2 / norm1; 
	          myLambda2 = norm22 / norm12;}
       
            else {myLambda1  = norm1 / norm2; 
	          myLambda2 = norm12 / norm22;}
            ang=V12.Angle(V22);
            if (ang>M_PI/2)  myContC2=M_PI-ang;
	    else myContC2=ang; }
    
        else{myIsDone= Standard_False ;
	     myErrorStatus=LocalAnalysis_NullSecondDerivative;} }  
     
  else  {myIsDone= Standard_False ;
	 myErrorStatus=LocalAnalysis_NullFirstDerivative;}
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvG1 (   GeomLProp_CLProps& Curv1, 
				        GeomLProp_CLProps & Curv2) 
{ gp_Dir Tang1,Tang2;
  Standard_Real ang;
   if (Curv1.IsTangentDefined() && Curv2.IsTangentDefined ()) 
     {   Curv1.Tangent(Tang1);
         Curv2.Tangent(Tang2);
         ang=Tang1.Angle(Tang2);
         if (ang>M_PI/2)  myContG1=M_PI-ang;
         else myContG1=ang;
     } 
   else {myIsDone= Standard_False ;
	 myErrorStatus=LocalAnalysis_TangentNotDefined;}  
}

/*********************************************************************************/

void LocalAnalysis_CurveContinuity::CurvG2( GeomLProp_CLProps& Curv1, 
				      GeomLProp_CLProps & Curv2 ) 
{ gp_Vec V1, V2;
  gp_Dir D1, D2;
  Standard_Real ang;
  Standard_Real epscrb= 8*myepsC0/(myMaxLon*myMaxLon);	
 
  if (Curv1.IsTangentDefined() && Curv2.IsTangentDefined())
    {  myCourbC1= Curv1.Curvature();
       myCourbC2 = Curv2.Curvature();
       if( (Abs(myCourbC1)>epscrb)&& (Abs(myCourbC2)>epscrb)) 
          { V1 = Curv1.D1();
            V2 = Curv2.D1();
            Curv1.Normal(D1);
            Curv2.Normal(D2);
            ang =D1.Angle(D2);
           if (ang>M_PI/2)  myContG2=M_PI-ang;
           else myContG2=ang;
            myCourbC1= Curv1.Curvature();
            myCourbC2 = Curv2.Curvature();
            myG2Variation= Abs(myCourbC1-myCourbC2) /  sqrt (myCourbC1* myCourbC2);}
       else {myIsDone = Standard_False ; 
             myErrorStatus=LocalAnalysis_NormalNotDefined; }
     }
    else  { myIsDone = Standard_False ;
           myErrorStatus=LocalAnalysis_TangentNotDefined;}
}
 
/*********************************************************************************/

LocalAnalysis_CurveContinuity::LocalAnalysis_CurveContinuity(const Handle(Geom_Curve)& Curv1, 
 const Standard_Real u1, const Handle(Geom_Curve)& Curv2, const Standard_Real u2, 
 const GeomAbs_Shape Order, 
 const Standard_Real Epsnul,
 const Standard_Real EpsC0,
 const Standard_Real EpsC1,
 const Standard_Real EpsC2,
 const Standard_Real EpsG1,
 const Standard_Real EpsG2,
 const Standard_Real Percent,
 const Standard_Real Maxlen )
{ myTypeCont = Order;
  myepsnul= Epsnul; 
  myMaxLon=Maxlen;
  myepsC0= EpsC0;
  myepsC1= EpsC1;
  myepsC2= EpsC2;
  myepsG1= EpsG1;
  myepsG2= EpsG2;
  myperce=Percent;

  myIsDone = Standard_True;
 
  switch ( Order)
	{ case GeomAbs_C0 : { //TypeCont=GeomAbs_C0;
			       GeomLProp_CLProps Curve1 ( Curv1, u1, 0, myepsnul); 
			       GeomLProp_CLProps Curve2 ( Curv2, u2, 0, myepsnul);	     
			       CurvC0(Curve1, Curve2);}
			    break;
 	  case GeomAbs_C1 : { //TypeCont=GeomAbs_C1;
 			    
			       GeomLProp_CLProps  Curve1 ( Curv1, u1, 1, myepsnul); 
			       GeomLProp_CLProps  Curve2 ( Curv2, u2, 1, myepsnul );
			       CurvC0(Curve1, Curve2);
 			       CurvC1(Curve1, Curve2);}
			    break;
 	  case GeomAbs_C2 : { //TypeCont=GeomAbs_C2;
			
			      GeomLProp_CLProps  Curve1 ( Curv1, u1, 2, myepsnul); 
			      GeomLProp_CLProps Curve2 ( Curv2, u2, 2, myepsnul);
			      CurvC0(Curve1, Curve2);
 			      CurvC1(Curve1, Curve2);
			      CurvC2(Curve1, Curve2);}
			    break;		
 	  case GeomAbs_G1 : { //TypeCont=GeomAbs_G1; 
			       GeomLProp_CLProps Curve1 ( Curv1, u1, 1, myepsnul); 
			        GeomLProp_CLProps Curve2 ( Curv2, u2, 1, myepsnul);
			       CurvC0(Curve1, Curve2);
 			       CurvG1(Curve1, Curve2);}
			    break;
 	  case GeomAbs_G2 : { //TypeCont=GeomAbs_G2; 
			       GeomLProp_CLProps  Curve1 ( Curv1, u1, 2, myepsnul); 
			       GeomLProp_CLProps Curve2 ( Curv2, u2, 2, myepsnul);
			       CurvC0(Curve1, Curve2);
 			       CurvG1(Curve1, Curve2);
			       CurvG2(Curve1, Curve2);}
			    break;
	  default         : {}
	}
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsC0() const 
{ 
  if (!myIsDone) { StdFail_NotDone::Raise();}
  if (myContC0 <= myepsC0 )
     return Standard_True;
     else return Standard_False;
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsC1() const  
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
  if ( IsC0() && ( (myContC1 <= myepsC1)||(Abs(myContC1-M_PI)<=myepsC1)))
     return Standard_True;
     else return Standard_False;	
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsC2() const  
{ Standard_Real epsil1, epsil2;

  if (!myIsDone) { StdFail_NotDone::Raise();}
  if ( IsC1())
    { if ((myContC2 <= myepsC2)||(Abs(myContC2-M_PI)<=myepsC2))
      { epsil1 = 0.5*myepsC1*myepsC1*myLambda1;
        epsil2 = 0.5*myepsC2*myepsC2*myLambda2;
       if ( (Abs(myLambda1*myLambda1-myLambda2)) <= (epsil1*epsil1+epsil2))
	return Standard_True;}
      else  return Standard_False;
    }
  return Standard_False;    	

}


/*********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsG1() const  
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
 if ( IsC0() && (( myContG1 <= myepsG1||(Abs(myContG1-M_PI)<=myepsG1))))
     return Standard_True;
     else return Standard_False;	
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsG2()const 
{ Standard_Real CRBINF, CRBNUL;
  Standard_Integer IETA1, IETA2;
        // etat des coubures IETA. -> 0 Crbure nulle 
	//			   -> 1 Crbure finie
	//			   -> 2 Crbure infinie

  if (!myIsDone) { StdFail_NotDone::Raise();}
  if ( IsG1 ())
     { CRBINF = 1/myepsC0;
       CRBNUL = 8*myepsC0/(myMaxLon*myMaxLon);

       if (myCourbC1 > CRBINF) IETA1=2;
     	  else if (myCourbC1 < CRBNUL) IETA1=0;	
  	     	  else IETA1=1;
       if (myCourbC2 > CRBINF) IETA2=2;
     	  else if (myCourbC2 < CRBNUL) IETA2=0;	
  	          else IETA2=1;
       if (IETA1 == IETA2)
     	  { if (IETA1 == 1)
               { Standard_Real eps= RealPart( (myContG2+myepsG2)/M_PI) * M_PI;
                 if (Abs( eps - myepsG2) < myepsG2)
                    {if (myG2Variation < myperce )
		       return Standard_True;
                    else return Standard_False;}
                 else return Standard_False;
               }
	       else return  Standard_True;
     	  }	
     	  else return  Standard_False;    
     }
     else return  Standard_False; 
}

/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::C0Value() const 
{ 
  if (!myIsDone) {StdFail_NotDone::Raise();}
  return ( myContC0 );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::C1Angle() const 
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
  return ( myContC1 );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::C2Angle() const 
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
  return ( myContC2 );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::G1Angle() const 
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
  return ( myContG1    );
}
/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::G2Angle() const 
{
  if (!myIsDone) { StdFail_NotDone::Raise();}
  return ( myContG2    );
}



/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::C1Ratio() const 
{
  if (!myIsDone) {StdFail_NotDone::Raise();}
  return ( myLambda1    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_CurveContinuity::C2Ratio() const 
{
  if (!myIsDone) {StdFail_NotDone::Raise();}
  return ( myLambda2    );
}

/********************************************************************************/
Standard_Real LocalAnalysis_CurveContinuity::G2CurvatureVariation() const 
{
  if (!myIsDone) {StdFail_NotDone::Raise();}
  return ( myG2Variation);
}


/********************************************************************************/

Standard_Boolean LocalAnalysis_CurveContinuity::IsDone() const
{ return ( myIsDone );
}

/*********************************************************************************/

LocalAnalysis_StatusErrorType  LocalAnalysis_CurveContinuity::StatusError() const
{
return  myErrorStatus;
}
/*************************************************************************/
GeomAbs_Shape  LocalAnalysis_CurveContinuity::ContinuityStatus() const 
{
 if (!myIsDone) { StdFail_NotDone::Raise();}
 return (myTypeCont);
}
/*********************************************************************************/










