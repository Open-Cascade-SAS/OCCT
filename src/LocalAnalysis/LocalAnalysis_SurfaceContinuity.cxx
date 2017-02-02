// Created on: 1996-09-09
// Created by: Herve LOUESSARD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <GeomLProp_SLProps.hxx>
#include <gp.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <StdFail_NotDone.hxx>

/*********************************************************************************/
/*********************************************************************************/
void LocalAnalysis_SurfaceContinuity::SurfC0 (const GeomLProp_SLProps& Surf1, 
                                               const GeomLProp_SLProps& Surf2
					       )  
{  myContC0=(Surf1.Value()).Distance(Surf2.Value());
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC1(  GeomLProp_SLProps& Surf1, 
                                       GeomLProp_SLProps & Surf2) 
{ gp_Vec V1u, V2u, V1v, V2v;
  Standard_Real norm1u, norm2u, norm1v, norm2v,angu,angv; 
  
  V1u = Surf1.D1U();
  V2u = Surf2.D1U();

  V1v = Surf1.D1V();
  V2v = Surf2.D1V();

  norm1u = V1u.Magnitude();
  norm2u = V2u.Magnitude();
  norm1v = V1v.Magnitude();
  norm2v = V2v.Magnitude();

  if ((norm1u>myepsnul )&&(norm2u>myepsnul )&&(norm1v>myepsnul)
       &&(norm2v>myepsnul))
    { if (norm1u >= norm2u ) 
         myLambda1U= norm2u / norm1u;
      else myLambda1U = norm1u / norm2u;
      if (norm1v >= norm2v ) 
         myLambda1V= norm2v / norm1v;
      else myLambda1V = norm1v / norm2v;
      angu= V1u.Angle(V2u);
      if (angu>M_PI/2) myContC1U=M_PI-angu;
      else myContC1U=angu;
      angv= V1v.Angle(V2v);
      if (angv>M_PI/2) myContC1V=M_PI-angv;
       else myContC1V=angv;   }
  else {myIsDone = Standard_False;  
        myErrorStatus=LocalAnalysis_NullFirstDerivative;}
}

/*********************************************************************************/

void LocalAnalysis_SurfaceContinuity::SurfC2(  GeomLProp_SLProps& Surf1, 
                                               GeomLProp_SLProps& Surf2)

{ gp_Vec V11u, V12u, V21u, V22u, V11v, V12v, V21v, V22v;
  Standard_Real norm11u, norm12u, norm21u, norm22u, norm11v, norm12v, norm21v, norm22v;
  Standard_Real ang;
  V11u = Surf1.D1U();
  V12u = Surf2.D1U();
  V21u = Surf1.D2U();
  V22u = Surf2.D2U();
  norm11u = V11u.Magnitude();
  norm12u = V12u.Magnitude();
  norm21u = V21u.Magnitude();
  norm22u = V22u.Magnitude();

  if ((norm11u>myepsnul)&&(norm12u>myepsnul))
     { if( (norm21u>myepsnul)&&(norm22u>myepsnul))
          { if (norm11u >= norm12u ) 
	        {myLambda1U= norm12u / norm11u;
	         myLambda2U = norm22u /norm21u;}
            else {myLambda1U = norm11u / norm12u;
	          myLambda2U = norm21u / norm22u;}
            ang=V21u.Angle(V22u);
            if(ang>M_PI/2) myContC2U=M_PI-ang;
            else myContC2U=ang; }
       else
	  {  myIsDone=Standard_False;
            myErrorStatus=LocalAnalysis_NullSecondDerivative;}
     }
   
   else { myIsDone=Standard_False; 
          myErrorStatus=LocalAnalysis_NullFirstDerivative;}

  V11v = Surf1.D1V();
  V12v = Surf2.D1V();
  V21v = Surf1.D2V();
  V22v = Surf2.D2V();
  norm11v = V11v.Magnitude();
  norm12v = V12v.Magnitude();
  norm21v = V21v.Magnitude();
  norm22v = V22v.Magnitude();

  if ((norm11v>myepsnul)&&(norm12v>myepsnul))
     {if ((norm21v>myepsnul)&&(norm22v>myepsnul))
         {if ( norm11v >= norm12v ) 
	     {myLambda1V= norm12v / norm11v;
	      myLambda2V= norm22v / norm21v;}       
          else{ myLambda1V = norm11v / norm12v;
		myLambda2V = norm21v / norm22v;}
          ang= V21v.Angle(V22v);
          if (ang>M_PI/2)  myContC2V=M_PI-ang;
          else myContC2V=ang;
         }
      else{ myIsDone= Standard_False;
            myErrorStatus=LocalAnalysis_NullSecondDerivative; }
    }
   else{ myIsDone=Standard_False;
         myErrorStatus=LocalAnalysis_NullFirstDerivative;}
}

/*********************************************************************************/
void LocalAnalysis_SurfaceContinuity::SurfG1(  GeomLProp_SLProps& Surf1, 
                                                GeomLProp_SLProps& Surf2) 
{ if (Surf1.IsNormalDefined()&&Surf2.IsNormalDefined()) 
  { gp_Dir D1 = Surf1.Normal();
    gp_Dir D2 = Surf2.Normal();
    Standard_Real ang=D1.Angle(D2);
    if (ang>M_PI/2)  myContG1= M_PI-ang;
    else  myContG1=ang; 
  }
  else{ myIsDone=Standard_False;
        myErrorStatus=LocalAnalysis_NormalNotDefined;} 
}

/*********************************************************************************/

void   LocalAnalysis_SurfaceContinuity::SurfG2 ( GeomLProp_SLProps& Surf1, 
                                          GeomLProp_SLProps & Surf2)
{ gp_Dir DMIN1, DMIN2, DMAX1, DMAX2;
  Standard_Real RMIN1,RMIN2,RMAX1,RMAX2;
  Standard_Real  x1, x2, y1, y2, z1, z2;
 
  if ( Surf1.IsCurvatureDefined() && Surf2.IsCurvatureDefined())
    { 
      Surf1.CurvatureDirections(DMIN1, DMAX1);
      Surf2.CurvatureDirections(DMIN2, DMAX2);
      DMIN1.Coord(x1, y1, z1);
      DMAX1.Coord(x2, y2, z2);
      gp_Dir MCD1( (Abs(x1)+Abs(x2))/2, (Abs(y1)+Abs(y2))/2, (Abs(z1)+Abs(z2))/2 );
      DMIN2.Coord(x1, y1, z1);
      DMAX2.Coord(x2, y2, z2);
      gp_Dir MCD2( (Abs(x1)+Abs(x2))/2, (Abs(y1)+Abs(y2))/2, (Abs(z1)+Abs(z2))/2 );
  
      myAlpha = MCD1.Angle( MCD2 );
      RMIN1 = Surf1.MinCurvature();  
      RMAX1 = Surf1.MaxCurvature();  
      RMIN2 = Surf2.MinCurvature();  
      RMAX2 = Surf2.MaxCurvature();
      myETA1 = (RMIN1+RMAX1)/2;
      myETA2 = (RMIN2+RMAX2)/2;
      myETA  = (myETA1+myETA2)/2;
      myZETA1 = (RMAX1-RMIN1)/2;
      myZETA2 = (RMAX2-RMIN2)/2;
      myZETA = (myZETA1+myZETA2)/2;
      Standard_Real DETA,DZETA; 
      DETA = ( myETA1- myETA2)/2;
      DZETA =( myZETA1- myZETA2)/2;
       myGap= Abs(DETA) + sqrt( DZETA*DZETA*Cos(myAlpha)*Cos(myAlpha)
                     + myZETA*myZETA*Sin(myAlpha)*Sin(myAlpha));
       }   
   else {myIsDone = Standard_False;
         myErrorStatus=LocalAnalysis_CurvatureNotDefined;}
          
}
LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity(const Standard_Real EpsNul,
								 const Standard_Real EpsC0,
								 const Standard_Real EpsC1,
								 const Standard_Real EpsC2,
								 const Standard_Real EpsG1,
								 const Standard_Real Percent,
								 const Standard_Real Maxlen)
{ myepsnul=EpsNul;
  myepsC0= EpsC0;
  myepsC1= EpsC1;
  myepsC2= EpsC2;
  myepsG1= EpsG1;
  myperce= Percent;
  mymaxlen= Maxlen;
  myIsDone = Standard_True;
}
 void LocalAnalysis_SurfaceContinuity::ComputeAnalysis(GeomLProp_SLProps& Surf1,
						       GeomLProp_SLProps& Surf2,
						       const GeomAbs_Shape Order)
{  myTypeCont = Order;
   switch ( Order )
	{ case GeomAbs_C0 : {
			        SurfC0(Surf1, Surf2);
			    }
			    break;
 	  case GeomAbs_C1 : {
			       SurfC0(Surf1, Surf2);
 			       SurfC1(Surf1, Surf2);}
			    break;
 	  case GeomAbs_C2 : {
			      SurfC0(Surf1, Surf2);
			      SurfC1(Surf1, Surf2);
 	 		      SurfC2(Surf1, Surf2);}
			    break;
 	  case GeomAbs_G1 : {
			      SurfC0(Surf1, Surf2);
 			      SurfG1(Surf1, Surf2);}
			    break;
 	  case GeomAbs_G2 : {
			      SurfC0(Surf1, Surf2);
 			      SurfG1(Surf1, Surf2);
 			      SurfG2(Surf1, Surf2);}
			    break;
	  default         : {}
	}

}

/*********************************************************************************/

LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity( const Handle(Geom_Surface)& Surf1, 
                         const Standard_Real u1, const Standard_Real v1, 
                         const Handle(Geom_Surface)& Surf2, const Standard_Real u2, 
                         const Standard_Real v2, const GeomAbs_Shape Ordre, 
                         const Standard_Real EpsNul,
	                 const Standard_Real EpsC0 ,
	                 const Standard_Real EpsC1 ,
                         const Standard_Real EpsC2,
                         const Standard_Real EpsG1,
                         const Standard_Real Percent,   	    
                         const Standard_Real  Maxlen )
{ myTypeCont = Ordre;
  myepsnul=EpsNul;
  myepsC0= EpsC0;
  myepsC1= EpsC1;
  myepsC2= EpsC2;
  myepsG1= EpsG1;
  myperce= Percent;
  mymaxlen= Maxlen;
  myIsDone = Standard_True;
  switch ( Ordre )
	{ case GeomAbs_C0 : {
			       GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 0, myepsnul);
			       GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 0, myepsnul);
			        SurfC0(Surfa1, Surfa2);
			    }
			    break;
 	  case GeomAbs_C1 : {
			       GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 1, myepsnul); 
			       GeomLProp_SLProps   Surfa2 ( Surf2, u2, v2, 1, myepsnul);
			       SurfC0(Surfa1, Surfa2);
 			       SurfC1(Surfa1, Surfa2);}
			    break;
 	  case GeomAbs_C2 : {
			       GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 2, myepsnul); 
			        GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 2, myepsnul);
			      SurfC0(Surfa1, Surfa2);
			      SurfC1(Surfa1, Surfa2);
 	 		      SurfC2(Surfa1, Surfa2);}
			    break;
 	  case GeomAbs_G1 : {
			       GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 1, myepsnul); 
			        GeomLProp_SLProps  Surfa2 ( Surf2, u2, v2, 1, myepsnul);
			      SurfC0(Surfa1, Surfa2);
 			      SurfG1(Surfa1, Surfa2);}
			    break;
 	  case GeomAbs_G2 : {
			      GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 2, myepsnul); 
			      GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 2, myepsnul);
			      SurfC0(Surfa1, Surfa2);
 			      SurfG1(Surfa1, Surfa2);
 			      SurfG2(Surfa1, Surfa2);}
			    break;
	  default         : {}
	}
}

/*********************************************************************************/

LocalAnalysis_SurfaceContinuity::LocalAnalysis_SurfaceContinuity(const Handle(Geom2d_Curve)& curv1, 
                         const Handle(Geom2d_Curve)& curv2, const Standard_Real U, 
                         const Handle(Geom_Surface)& Surf1 , 
                         const Handle(Geom_Surface)& Surf2, 
                         const GeomAbs_Shape Ordre,
                         const Standard_Real EpsNul,
	                 const Standard_Real EpsC0 ,
	                 const Standard_Real EpsC1 ,
                         const Standard_Real EpsC2,
                         const Standard_Real EpsG1,
                         const Standard_Real Percent,   	    
                         const Standard_Real  Maxlen )
{ Standard_Real pard1, parf1, pard2, parf2, u1, v1, u2, v2;
  
  myTypeCont = Ordre;
  myepsnul=EpsNul;
  myepsC0= EpsC0;
  myepsC1= EpsC1;
  myepsC2= EpsC2;
  myepsG1= EpsG1;
  myperce= Percent;
  mymaxlen= Maxlen;
  myIsDone = Standard_True;

  pard1 = curv1->FirstParameter();
  pard2 = curv2->FirstParameter();
  parf1 = curv1->LastParameter();
  parf2 = curv2->LastParameter();


  if (!(((U <= parf1) && (U >= pard1)) &&((U <= parf2) && (U >= pard2))) )    myIsDone = Standard_False; 
    else 
    { gp_Pnt2d pt1 = curv1->Value(U);
      gp_Pnt2d pt2 = curv2->Value(U);
       
      pt1.Coord(u1, v1);
      pt2.Coord(u2, v2);
      switch ( Ordre )
	  { case GeomAbs_C0 : {
	                         GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 0, myepsnul);
				 GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 0, myepsnul);
				 SurfC0(Surfa1, Surfa2);
			      }
			      break;
	    case GeomAbs_C1 : {
			         GeomLProp_SLProps Surfa1 ( Surf1, u1, v1, 1, myepsnul);
				  GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 1, myepsnul);
				SurfC0(Surfa1, Surfa2);
				SurfC1(Surfa1, Surfa2);}
			      break;
 	    case GeomAbs_C2 : {
			         GeomLProp_SLProps  Surfa1 ( Surf1, u1, v1, 2, myepsnul); 
			         GeomLProp_SLProps   Surfa2 ( Surf2, u2, v2, 2, myepsnul);
			        SurfC0(Surfa1, Surfa2);
			        SurfC1(Surfa1, Surfa2);
 	 		        SurfC2(Surfa1, Surfa2);}
			      break;
            case GeomAbs_G1 : {
		   	         GeomLProp_SLProps  Surfa1 ( Surf1, u1, v1, 1, myepsnul); 
				 GeomLProp_SLProps Surfa2 ( Surf2, u2, v2, 1, myepsnul);
			         SurfC0(Surfa1, Surfa2);
				 SurfG1(Surfa1, Surfa2);}
			      break;
 	    case GeomAbs_G2 : {
			        GeomLProp_SLProps   Surfa1 ( Surf1, u1, v1, 2, myepsnul); 
				 GeomLProp_SLProps  Surfa2 ( Surf2, u2, v2, 2, myepsnul);
				SurfC0(Surfa1, Surfa2);
				SurfG1(Surfa1, Surfa2);
				SurfG2(Surfa1, Surfa2);}
			      break;
	    default         : {}
	  }
    }
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsC0() const 
{ if (!myIsDone) { throw StdFail_NotDone();}
  if ( myContC0 <= myepsC0 ) 
     return Standard_True;
     else return Standard_False;
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsC1() const 
{ if (!myIsDone) { throw StdFail_NotDone();}
  if ( IsC0 () && (myContC1U <= myepsC1) && (myContC1V <= myepsC1))
   return Standard_True;
  else return Standard_False;
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsC2() const 
{ Standard_Real eps1u, eps1v, eps2u, eps2v;

  if (!myIsDone) { throw StdFail_NotDone();}
  if ( IsC1())
     { eps1u = 0.5*myepsC1*myepsC1*myLambda1U;
       eps1v = 0.5*myepsC1*myepsC1*myLambda1V;
       eps2u = 0.5*myepsC2*myepsC2*myLambda2U;
       eps2v = 0.5*myepsC2*myepsC2*myLambda2V;
       if ((myContC2U < myepsC2) && (myContC2V < myepsC2))
          { if (Abs(myLambda1U*myLambda1U-myLambda2U) <= (eps1u*eps1u+eps2u))
   	       if (Abs(myLambda1V*myLambda1V-myLambda2V) <= (eps1v*eps1v+eps2v))
		  return Standard_True;
	       else return  Standard_False;
	    else return  Standard_False;
	  }
	  else return  Standard_False;        
     }
     else return Standard_False;
    
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsG1() const 
{
  if (!myIsDone) { throw StdFail_NotDone();} 
  if ( IsC0 () &&( myContG1 <= myepsG1))
       return Standard_True;
      else return Standard_False;
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsG2()const 
{ Standard_Real EPSNL;
  Standard_Integer itype;
 
  if (!myIsDone) { throw StdFail_NotDone();} 
  itype =0;
  EPSNL= 8*myepsC0/(mymaxlen*mymaxlen);
  if ( IsG1())
     { if ( ( Abs(myETA)< EPSNL) && ( Abs(myZETA)< EPSNL))
	  return Standard_True;
 	if ( ( Abs(myZETA1)< EPSNL) && ( Abs(myZETA2)< EPSNL))
	   itype =1;
           else if ( ( Abs(myETA1)< EPSNL) && ( Abs(myETA2)< EPSNL))
	           itype =1;
		   else if ((Abs(Abs(myZETA)-Abs(myETA))) < EPSNL)
			   itype =1;
			   else if ((myETA1<myZETA1)&&(myETA2<myZETA2))
				   itype =1; 
			   	   else if ((myETA1>myZETA1)&&(myETA2>myZETA2))
				   	   itype =1; 
       if (itype == 1)
          { 

	    if (( myETA >= (2*myZETA))&&(myGap<=(myperce*(myETA-myZETA)))) return Standard_True;
	    if (( myZETA>= myETA) && ( myGap<= (myperce*myZETA))) return Standard_True;
	    if (( myZETA<= myETA) && ( myETA <= (2*myZETA)) && (myGap <= (myperce * myETA))) 
               return Standard_True;
               else  return Standard_False;
                    
          }
          else  return Standard_False;
     }
     else return Standard_False;

}

  
/*********************************************************************************/

GeomAbs_Shape  LocalAnalysis_SurfaceContinuity::ContinuityStatus() const 
{
 if (!myIsDone) { throw StdFail_NotDone();}
 return (myTypeCont);
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C0Value() const 
{ 
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContC0    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C1UAngle() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContC1U    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C1VAngle() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContC1V    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C2UAngle() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContC2U    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C2VAngle() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContC2V    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::G1Angle() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myContG1    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C1URatio() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myLambda1U    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C2URatio() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myLambda2U    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C1VRatio() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myLambda1V    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::C2VRatio() const 
{ 
  if (!myIsDone) { throw StdFail_NotDone();}
 return ( myLambda2V    );
}

/*********************************************************************************/

Standard_Real LocalAnalysis_SurfaceContinuity::G2CurvatureGap() const 
{
  if (!myIsDone) { throw StdFail_NotDone();}
  return ( myGap    );
}

/*********************************************************************************/

Standard_Boolean LocalAnalysis_SurfaceContinuity::IsDone() const
{ return ( myIsDone );
}

/*********************************************************************************/
LocalAnalysis_StatusErrorType  LocalAnalysis_SurfaceContinuity::StatusError() const
{
return  myErrorStatus;
}
