// Created on: 1992-08-06
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

//----------------------------------------------------------------------
//-- Purposse: Geometric Intersection between two Natural Quadric 
//--          If the intersection is not a conic, 
//--          analytical methods must be called.
//----------------------------------------------------------------------
#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif

#include <IntAna_QuadQuadGeo.ixx>

#include <IntAna_IntConicQuad.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <math_DirectPolynomialRoots.hxx>

#include <gp.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <ElSLib.hxx>
#include <ElCLib.hxx>

#include <gp_Dir.hxx>
#include <gp_XYZ.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>


static
  gp_Ax2 DirToAx2(const gp_Pnt& P,const gp_Dir& D);
static
  void RefineDir(gp_Dir& aDir);

//=======================================================================
//class :  
//purpose  : O p e r a t i o n s   D i v e r s e s  s u r   d e s   A x 1 
//=======================================================================
class AxeOperator {
 public:
  AxeOperator(const gp_Ax1& A1,const gp_Ax1& A2);

  void Distance(Standard_Real& dist,
		Standard_Real& Param1,
		Standard_Real& Param2);
  
  gp_Pnt PtIntersect()              { 
    return ptintersect;
  }
  Standard_Boolean Coplanar(void)   { 
    return thecoplanar;
  }
  Standard_Boolean Same(void)       {
    return theparallel && (thedistance<myEPSILON_DISTANCE); 
  }
  Standard_Real Distance(void)      { 
    return thedistance ;
  }
  Standard_Boolean Intersect(void)  { 
    return (thecoplanar && (!theparallel));
  }
  Standard_Boolean Parallel(void)   { 
    return theparallel; 
  }
  Standard_Boolean Normal(void)     { 
    return thenormal;
  }

 protected:
  Standard_Real Det33(const Standard_Real a11,
		      const Standard_Real a12,
		      const Standard_Real a13,
		      const Standard_Real a21,
		      const Standard_Real a22,
		      const Standard_Real a23,
		      const Standard_Real a31,
		      const Standard_Real a32,
		      const Standard_Real a33) {
    Standard_Real theReturn =  
      a11*(a22*a33-a32*a23) - a21*(a12*a33-a32*a13) + a31*(a12*a23-a22*a13) ;   
    return theReturn ;
  }

 private:
  gp_Pnt ptintersect;
  gp_Ax1 Axe1;
  gp_Ax1 Axe2;
  Standard_Real thedistance;
  Standard_Boolean theparallel;
  Standard_Boolean thecoplanar;
  Standard_Boolean thenormal;
  //
  Standard_Real myEPSILON_DISTANCE;
  Standard_Real myEPSILON_AXES_PARA;
};

//=======================================================================
//function : AxeOperator::AxeOperator
//purpose  : 
//=======================================================================
  AxeOperator::AxeOperator(const gp_Ax1& A1,const gp_Ax1& A2) 
{
  myEPSILON_DISTANCE=0.00000000000001;
  myEPSILON_AXES_PARA=0.000000000001;
  Axe1=A1; 
  Axe2=A2;
  //---------------------------------------------------------------------
  gp_Dir V1=Axe1.Direction();
  gp_Dir V2=Axe2.Direction();
  gp_Pnt P1=Axe1.Location();
  gp_Pnt P2=Axe2.Location();
  //
  RefineDir(V1);
  RefineDir(V2);
  thecoplanar= Standard_False;
  thenormal  = Standard_False;

  //--- check if the two axis are parallel
  theparallel=V1.IsParallel(V2, myEPSILON_AXES_PARA);  
  //--- Distance between the two axis
  gp_XYZ perp(A1.Direction().XYZ().Crossed(A2.Direction().XYZ()));
  if (theparallel) { 
    gp_Lin L1(A1);
    thedistance = L1.Distance(A2.Location());
  }
  else {   
    thedistance = Abs(gp_Vec(perp.Normalized()).Dot(gp_Vec(Axe1.Location(),
							   Axe2.Location())));
  }
  //--- check if Axis are Coplanar
  Standard_Real D33;
  if(thedistance<myEPSILON_DISTANCE) {
    D33=Det33(V1.X(),V1.Y(),V1.Z()
	      ,V2.X(),V2.Y(),V2.Z()
	      ,P1.X()-P2.X(),P1.Y()-P2.Y(),P1.Z()-P2.Z());
    if(Abs(D33)<=myEPSILON_DISTANCE) { 
      thecoplanar=Standard_True;
    }
  }
  else {
    thecoplanar=Standard_True;
    thenormal=(V1.Dot(V2)==0.0)? Standard_True : Standard_False;
  }
  //--- check if the two axis are concurrent
  if(thecoplanar && (!theparallel)) {
    Standard_Real smx=P2.X() - P1.X();
    Standard_Real smy=P2.Y() - P1.Y();
    Standard_Real smz=P2.Z() - P1.Z();
    Standard_Real Det1,Det2,Det3,A;
    Det1=V1.Y() * V2.X() - V1.X() * V2.Y();
    Det2=V1.Z() * V2.Y() - V1.Y() * V2.Z();
    Det3=V1.Z() * V2.X() - V1.X() * V2.Z();
    
    if((Det1!=0.0) && ((Abs(Det1) >= Abs(Det2))&&(Abs(Det1) >= Abs(Det3)))) {
      A=(smy * V2.X() - smx * V2.Y())/Det1;
    }
    else if((Det2!=0.0) 
	    && ((Abs(Det2) >= Abs(Det1))
		&&(Abs(Det2) >= Abs(Det3)))) {
      A=(smz * V2.Y() - smy * V2.Z())/Det2;
    }
    else {
      A=(smz * V2.X() - smx * V2.Z())/Det3;
    }
    ptintersect.SetCoord( P1.X() + A * V1.X()
			 ,P1.Y() + A * V1.Y()
			 ,P1.Z() + A * V1.Z());
  }
  else { 
    ptintersect.SetCoord(0,0,0);  //-- Pour eviter des FPE
  }
}
//=======================================================================
//function : Distance
//purpose  : 
//=======================================================================
  void AxeOperator::Distance(Standard_Real& dist,Standard_Real& Param1,Standard_Real& Param2)
 {
  gp_Vec O1O2(Axe1.Location(),Axe2.Location());   //-----------------------------
  gp_Dir U1 = Axe1.Direction();   //-- juste pour voir. 
  gp_Dir U2 = Axe2.Direction();
  
  gp_Dir N  = U1.Crossed(U2);
  Standard_Real D = Det33(U1.X(),U2.X(),N.X(),
			  U1.Y(),U2.Y(),N.Y(),
			  U1.Z(),U2.Z(),N.Z());
  if(D) { 
    dist = Det33(U1.X(),U2.X(),O1O2.X(),
		 U1.Y(),U2.Y(),O1O2.Y(),
		 U1.Z(),U2.Z(),O1O2.Z()) / D;
    Param1 = Det33(O1O2.X(),U2.X(),N.X(),
		   O1O2.Y(),U2.Y(),N.Y(),
		   O1O2.Z(),U2.Z(),N.Z()) / (-D);
    //------------------------------------------------------------
    //-- On resout P1 * Dir1 + P2 * Dir2 + d * N = O1O2
    //-- soit : Segment perpendiculaire : O1+P1 D1
    //--                                  O2-P2 D2
    Param2 = Det33(U1.X(),O1O2.X(),N.X(),
		   U1.Y(),O1O2.Y(),N.Y(),
		   U1.Z(),O1O2.Z(),N.Z()) / (D);
  }
}
//=======================================================================
//function : DirToAx2
//purpose  : returns a gp_Ax2 where D is the main direction
//=======================================================================
gp_Ax2 DirToAx2(const gp_Pnt& P,const gp_Dir& D) 
{
  Standard_Real x=D.X(); Standard_Real ax=Abs(x);
  Standard_Real y=D.Y(); Standard_Real ay=Abs(y);
  Standard_Real z=D.Z(); Standard_Real az=Abs(z);
  if( (ax==0.0) || ((ax<ay) && (ax<az)) ) {
    return(gp_Ax2(P,D,gp_Dir(gp_Vec(0.0,-z,y))));
  }
  else if( (ay==0.0) || ((ay<ax) && (ay<az)) ) {
    return(gp_Ax2(P,D,gp_Dir(gp_Vec(-z,0.0,x))));
  }
  else {
    return(gp_Ax2(P,D,gp_Dir(gp_Vec(-y,x,0.0))));
  }
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Empty constructor
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(void)
    : done(Standard_False),
      nbint(0),
      typeres(IntAna_Empty),
      pt1(0,0,0),
      pt2(0,0,0),
      param1(0),
      param2(0),
      param1bis(0),
      param2bis(0),
      myCommonGen(Standard_False),
      myPChar(0,0,0)
{
  InitTolerances();
}
//=======================================================================
//function : InitTolerances
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::InitTolerances()
{
  myEPSILON_DISTANCE               = 0.00000000000001;
  myEPSILON_ANGLE_CONE             = 0.000000000001;
  myEPSILON_MINI_CIRCLE_RADIUS     = 0.000000001;
  myEPSILON_CYLINDER_DELTA_RADIUS  = 0.0000000000001;
  myEPSILON_CYLINDER_DELTA_DISTANCE= 0.0000001;
  myEPSILON_AXES_PARA              = 0.000000000001;
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Pln  Pln 
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Pln& P1, 
					 const gp_Pln& P2,
					 const Standard_Real TolAng,
					 const Standard_Real Tol)
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(P1,P2,TolAng,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform (const gp_Pln& P1, 
				    const gp_Pln& P2,
				    const Standard_Real TolAng,
				    const Standard_Real Tol)
{
  done=Standard_False;
  //
  param2bis=0.0;

  Standard_Real A1 = 0., B1 = 0., C1 = 0., D1 = 0., A2 = 0., B2 = 0., C2 = 0., D2 = 0.;
  P1.Coefficients(A1,B1,C1,D1);
  P2.Coefficients(A2,B2,C2,D2);
  
  gp_Vec vd(gp_Vec(A1,B1,C1).Crossed(gp_Vec(A2,B2,C2)));
  Standard_Real dist1= A2*P1.Location().X() + B2*P1.Location().Y() + C2*P1.Location().Z() + D2;
  Standard_Real dist2= A1*P2.Location().X() + B1*P2.Location().Y() + C1*P2.Location().Z() + D1;

  if(vd.Magnitude() <=TolAng) {
    // normalles are collinear - planes are same or parallel
    typeres = (Abs(dist1) <= Tol && Abs(dist2) <= Tol) ? IntAna_Same : IntAna_Empty;
  }
  else {
    Standard_Real denom=A1*A2 + B1*B2 + C1*C2;

    Standard_Real denom2 = denom*denom;
    Standard_Real ddenom = 1. - denom2;
    denom = ( Abs(ddenom) <= 1.e-9 ) ? 1.e-9 : ddenom;
      
    Standard_Real par1 = dist1/denom;
    Standard_Real par2 = -dist2/denom;
      
    gp_Vec inter1(gp_Vec(A1,B1,C1).Crossed(vd));
    gp_Vec inter2(gp_Vec(A2,B2,C2).Crossed(vd));
      
    Standard_Real X1=P1.Location().X() + par1*inter1.X();
    Standard_Real Y1=P1.Location().Y() + par1*inter1.Y();
    Standard_Real Z1=P1.Location().Z() + par1*inter1.Z();
    Standard_Real X2=P2.Location().X() + par2*inter2.X();
    Standard_Real Y2=P2.Location().Y() + par2*inter2.Y();
    Standard_Real Z2=P2.Location().Z() + par2*inter2.Z();
      
    pt1=gp_Pnt((X1+X2)*0.5, (Y1+Y2)*0.5, (Z1+Z2)*0.5);
    dir1 = gp_Dir(vd);
    typeres = IntAna_Line;
    nbint = 1;
 
  }
  done=Standard_True;
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Pln Cylinder
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo( const gp_Pln& P
       ,const gp_Cylinder& Cl
       ,const Standard_Real Tolang
       ,const Standard_Real Tol)
    : done(Standard_False),
      nbint(0),
      typeres(IntAna_Empty),
      pt1(0,0,0),
      pt2(0,0,0),
      param1(0),
      param2(0),
      param1bis(0),
      param2bis(0),
      myCommonGen(Standard_False),
      myPChar(0,0,0)
{
  InitTolerances();
  Perform(P,Cl,Tolang,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform( const gp_Pln& P
				   ,const gp_Cylinder& Cl
				   ,const Standard_Real Tolang
				   ,const Standard_Real Tol) 
{
  done = Standard_False;
  Standard_Real dist,radius;
  Standard_Real A,B,C,D;
  Standard_Real X,Y,Z;
  Standard_Real sint,cost,h;
  gp_XYZ axex,axey,omega;

  
  param2bis=0.0;
  radius = Cl.Radius();

  gp_Lin axec(Cl.Axis());
  gp_XYZ normp(P.Axis().Direction().XYZ());

  P.Coefficients(A,B,C,D);
  axec.Location().Coord(X,Y,Z);
  dist = A*X + B*Y + C*Z + D; // la distance axe/plan est evaluee a l origine de l axe.

  Standard_Real tolang = Tolang;
  Standard_Boolean newparams = Standard_False;

  gp_Vec ldv( axec.Direction() );
  gp_Vec npv( normp );
  Standard_Real dA = Abs( ldv.Angle( npv ) );
  if( dA > (M_PI/4.) )
    {
      Standard_Real dang = Abs( ldv.Angle( npv ) ) - M_PI/2.;
      Standard_Real dangle = Abs( dang );
      if( dangle > Tolang )
	{
	  Standard_Real sinda = Abs( Sin( dangle ) );
	  Standard_Real dif = Abs( sinda - Tol );
	  if( dif < Tol )
	    {
	      tolang = sinda * 2.;
	      newparams = Standard_True;
	    }
	}
    }

  nbint = 0;
  IntAna_IntConicQuad inter(axec,P,tolang);

  if (inter.IsParallel()) {
    // Le resultat de l intersection Plan-Cylindre est de type droite.
    // il y a 1 ou 2 droites

    typeres = IntAna_Line;
    omega.SetCoord(X-dist*A,Y-dist*B,Z-dist*C);

    if (Abs(Abs(dist)-radius) < Tol)
      {
	nbint = 1;
	pt1.SetXYZ(omega);

	if( newparams )
	  {
	    gp_XYZ omegaXYZ(X,Y,Z);
	    gp_XYZ omegaXYZtrnsl( omegaXYZ + 100.*axec.Direction().XYZ() );
	    Standard_Real Xt,Yt,Zt,distt;
	    omegaXYZtrnsl.Coord(Xt,Yt,Zt);
	    distt = A*Xt + B*Yt + C*Zt + D;
	    gp_XYZ omega1( omegaXYZtrnsl.X()-distt*A, omegaXYZtrnsl.Y()-distt*B, omegaXYZtrnsl.Z()-distt*C );
	    gp_Pnt ppt1;
	    ppt1.SetXYZ( omega1 );
	    gp_Vec vv1(pt1,ppt1);
	    gp_Dir dd1( vv1 );
	    dir1 = dd1;
	  }
	else
	  dir1 = axec.Direction();
    }
    else if (Abs(dist) < radius)
      {
	nbint = 2;
	h = Sqrt(radius*radius - dist*dist);
	axey = axec.Direction().XYZ().Crossed(normp); // axey est normalise

	pt1.SetXYZ(omega - h*axey);
	pt2.SetXYZ(omega + h*axey);

	if( newparams )
	  { 
	    gp_XYZ omegaXYZ(X,Y,Z);
	    gp_XYZ omegaXYZtrnsl( omegaXYZ + 100.*axec.Direction().XYZ() );
	    Standard_Real Xt,Yt,Zt,distt,ht;
	    omegaXYZtrnsl.Coord(Xt,Yt,Zt);
	    distt = A*Xt + B*Yt + C*Zt + D;
	    // 	    ht = Sqrt(radius*radius - distt*distt);
	    Standard_Real anSqrtArg = radius*radius - distt*distt;
	    ht = (anSqrtArg > 0.) ? Sqrt(anSqrtArg) : 0.;

	    gp_XYZ omega1( omegaXYZtrnsl.X()-distt*A, omegaXYZtrnsl.Y()-distt*B, omegaXYZtrnsl.Z()-distt*C );
	    gp_Pnt ppt1,ppt2;
	    ppt1.SetXYZ( omega1 - ht*axey);
	    ppt2.SetXYZ( omega1 + ht*axey);
	    gp_Vec vv1(pt1,ppt1);
	    gp_Vec vv2(pt2,ppt2);
	    gp_Dir dd1( vv1 );
	    gp_Dir dd2( vv2 );
	    dir1 = dd1;
	    dir2 = dd2;
	  }
	else
	  {
	    dir1 = axec.Direction();
	    dir2 = axec.Direction();
	  }
    }
    //  else nbint = 0

    // debug JAG : le nbint = 0 doit etre remplace par typeres = IntAna_Empty
    // et ne pas etre seulement supprime...

    else {
      typeres = IntAna_Empty;
    }
  }
  else {     // Il y a un point d intersection. C est le centre du cercle
             // ou de l ellipse solution.

    nbint = 1;
    axey = normp.Crossed(axec.Direction().XYZ());
    sint = axey.Modulus();

    pt1 = inter.Point(1);
    
    if (sint < Tol/radius) {

      // on construit un cercle avec comme axes X et Y ceux du cylindre
      typeres = IntAna_Circle;

      dir1 = axec.Direction(); // axe Z
      dir2 = Cl.Position().XDirection();
      param1 = radius;
    }
    else {

      // on construit un ellipse
      typeres = IntAna_Ellipse;
      cost = Abs(axec.Direction().XYZ().Dot(normp));
      axex = axey.Crossed(normp);

      dir1.SetXYZ(normp);         //Modif ds ce bloc 
      dir2.SetXYZ(axex);

      param1    = radius/cost;
      param1bis = radius;
    }
  }
  if(typeres == IntAna_Ellipse) { 
    if(   param1>100000.0*param1bis 
       || param1bis>100000.0*param1) { 
      done = Standard_False;
      return;
    }
  }
  done = Standard_True;
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Pln Cone
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Pln& P,
					 const gp_Cone& Co,
					 const Standard_Real Tolang,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(P,Co,Tolang,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Pln& P,
				   const gp_Cone& Co,
				   const Standard_Real Tolang,
				   const Standard_Real Tol) 
{

  done = Standard_False;
  nbint = 0;

  Standard_Real A,B,C,D;
  Standard_Real X,Y,Z;
  Standard_Real dist,sint,cost,sina,cosa,angl,costa;
  Standard_Real dh;
  gp_XYZ axex,axey;

  gp_Lin axec(Co.Axis());
  P.Coefficients(A,B,C,D);
  gp_Pnt apex(Co.Apex());

  apex.Coord(X,Y,Z);
  dist = A*X + B*Y + C*Z + D; // distance signee sommet du cone/ Plan

  gp_XYZ normp = P.Axis().Direction().XYZ();
  if(P.Direct()==Standard_False) {  //-- lbr le 14 jan 97
    normp.Reverse();
  }

  axey = normp.Crossed(Co.Axis().Direction().XYZ());
  axex = axey.Crossed(normp);


  angl = Co.SemiAngle();

  cosa = Cos(angl);
  sina = Abs(Sin(angl));


  // Angle entre la normale au plan et l axe du cone, ramene entre 0. et PI/2.

  sint = axey.Modulus();
  cost = Abs(Co.Axis().Direction().XYZ().Dot(normp));

  // Le calcul de costa permet de determiner si le plan contient
  // un generatrice du cone : on calcul Sin((PI/2. - t) - angl)

  costa = cost*cosa - sint*sina;  // sin((PI/2 -t)-angl)=cos(t+angl)
                                  // avec  t ramene entre 0 et pi/2.

  if (Abs(dist) < Tol) {
    // on considere que le plan contient le sommet du cone.
    // les solutions possibles sont donc : 1 point, 1 droite, 2 droites
    // selon l inclinaison du plan.

    if (Abs(costa) < Tolang) {          // plan parallele a la generatrice
      typeres = IntAna_Line;
      nbint = 1;
      gp_XYZ ptonaxe(apex.XYZ() + 10.*(Co.Axis().Direction().XYZ()));
      // point sur l axe du cone cote z positif

      dist = A*ptonaxe.X() + B*ptonaxe.Y() + C*ptonaxe.Z() + D;
      ptonaxe = ptonaxe - dist*normp;
      pt1 = apex;
      dir1.SetXYZ(ptonaxe - pt1.XYZ());
    }
    else if (cost < sina) {   // plan "interieur" au cone
      typeres = IntAna_Line;
      nbint = 2;
      pt1 = apex;
      pt2 = apex;
      dh = Sqrt(sina*sina-cost*cost)/cosa;
      dir1.SetXYZ(axex + dh*axey);
      dir2.SetXYZ(axex - dh*axey);
    }
    else {                              // plan "exterieur" au cone
      typeres = IntAna_Point;
      nbint = 1;
      pt1 = apex;
    }
  }
  else {
    // Solutions possibles : cercle, ellipse, parabole, hyperbole selon
    // l inclinaison du plan.
    Standard_Real deltacenter, distance;

    if (cost < Tolang) {
      // Le plan contient la direction de l axe du cone. La solution est
      // l hyperbole
      typeres = IntAna_Hyperbola;
      nbint = 2;
      pt1.SetXYZ(apex.XYZ()-dist*normp);
      pt2 = pt1;
      dir1=normp;
      dir2.SetXYZ(axex);
      param1     = param2 = Abs(dist/Tan(angl));
      param1bis  = param2bis = Abs(dist);
    }
    else {

      IntAna_IntConicQuad inter(axec,P,Tolang); // on a necessairement 1 point.
      
      gp_Pnt center(inter.Point(1));

      // En fonction de la position de l intersection par rapport au sommet
      // du cone, on change l axe x en -x et y en -y. Le parametre du sommet
      // sur axec est negatif (voir definition du cone)

      distance = apex.Distance(center);

      if (inter.ParamOnConic(1) + Co.RefRadius()/Tan(angl) < 0.) {
	axex.Reverse();
	axey.Reverse();
      }

      if (Abs(costa) < Tolang) {  // plan parallele a une generatrice
	typeres = IntAna_Parabola;
	nbint = 1;
	deltacenter = distance/2./cosa;
	axex.Normalize();
	pt1.SetXYZ(center.XYZ()-deltacenter*axex);
	dir1 = normp;
	dir2.SetXYZ(axex);
	param1 = deltacenter*sina*sina;
      }
      else if (sint  < Tolang) {            // plan perpendiculaire a l axe
	typeres = IntAna_Circle;
	nbint = 1;
	pt1 = center;
	dir1 = Co.Position().Direction();
	dir2 = Co.Position().XDirection();
	param1 = apex.Distance(center)*Abs(Tan(angl));
      }
      else if (cost < sina ) {
	typeres = IntAna_Hyperbola;
	nbint = 2;
	axex.Normalize();

	deltacenter = sint*sina*sina*distance/(sina*sina - cost*cost);
	pt1.SetXYZ(center.XYZ() - deltacenter*axex);
	pt2 = pt1;
	dir1 = normp;
	dir2.SetXYZ(axex);
	param1    = param2 = cost*sina*cosa*distance /(sina*sina-cost*cost);
	param1bis = param2bis = cost*sina*distance / Sqrt(sina*sina-cost*cost);

      }
      else {                    // on a alors cost > sina
	typeres = IntAna_Ellipse;
	nbint = 1;
	Standard_Real radius = cost*sina*cosa*distance/(cost*cost-sina*sina);
	deltacenter = sint*sina*sina*distance/(cost*cost-sina*sina);
	axex.Normalize();
	pt1.SetXYZ(center.XYZ() + deltacenter*axex);
	dir1 = normp;
	dir2.SetXYZ(axex);
	param1    = radius;
	param1bis = cost*sina*distance/ Sqrt(cost*cost - sina*sina);
      }
    }
  }
  
  //-- On a du mal a gerer plus loin (Value ProjLib, Params ... )
  //-- des hyperboles trop bizarres
  //-- On retourne False -> Traitement par biparametree
  static Standard_Real EllipseLimit   = 1.0E+9; //OCC513(apo) 1000000
  static Standard_Real HyperbolaLimit = 2.0E+6; //OCC537(apo) 50000
  if(typeres==IntAna_Ellipse && nbint>=1) { 
    if(Abs(param1) > EllipseLimit || Abs(param1bis) > EllipseLimit)  { 
      done=Standard_False; 
      return;
    }
  }
  if(typeres==IntAna_Hyperbola && nbint>=2) { 
    if(Abs(param2) > HyperbolaLimit || Abs(param2bis) > HyperbolaLimit)  { 
      done = Standard_False; 
      return;
    }
  }
  if(typeres==IntAna_Hyperbola && nbint>=1) { 
    if(Abs(param1) > HyperbolaLimit || Abs(param1bis) > HyperbolaLimit)  {
      done=Standard_False;
      return;
    }
  }

  done = Standard_True;
}

//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Pln Sphere
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Pln& P,
					 const gp_Sphere& S)
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(P,S);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform( const gp_Pln& P
				   ,const gp_Sphere& S) 
{
  
  done = Standard_False;
  Standard_Real A,B,C,D,dist, radius;
  Standard_Real X,Y,Z;

  nbint = 0;
// debug JAG : on met typeres = IntAna_Empty par defaut...
  typeres = IntAna_Empty;
  
  P.Coefficients(A,B,C,D);
  S.Location().Coord(X,Y,Z);
  radius = S.Radius();
  
  dist = A * X + B * Y + C * Z + D;
  
  if (Abs( Abs(dist) - radius) < Epsilon(radius)) {
    // on a une seule solution : le point projection du centre de la sphere
    // sur le plan
    nbint = 1;
    typeres = IntAna_Point;
    pt1.SetCoord(X - dist*A, Y - dist*B, Z - dist*C);
  }
  else if (Abs(dist) < radius) {
    // on a un cercle solution
    nbint = 1;
    typeres = IntAna_Circle;
    pt1.SetCoord(X - dist*A, Y - dist*B, Z - dist*C);
    dir1 = P.Axis().Direction();
    if(P.Direct()==Standard_False) dir1.Reverse();
    dir2 = P.Position().XDirection();
    param1 = Sqrt(radius*radius - dist*dist);
  }
  param2bis=0.0; //-- pour eviter param2bis not used .... 
  done = Standard_True;
}

//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Cylinder - Cylinder
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Cylinder& Cyl1,
					 const gp_Cylinder& Cyl2,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Cyl1,Cyl2,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Cylinder& Cyl1,
		     const gp_Cylinder& Cyl2,
		     const Standard_Real Tol) 
{
  done=Standard_True;
  //---------------------------- Parallel axes -------------------------
  AxeOperator A1A2(Cyl1.Axis(),Cyl2.Axis());
  Standard_Real R1=Cyl1.Radius();
  Standard_Real R2=Cyl2.Radius();
  Standard_Real RmR, RmR_Relative;
  RmR=(R1>R2)? (R1-R2) : (R2-R1);
  {
    Standard_Real Rmax, Rmin;
    Rmax=(R1>R2)? R1 : R2;
    Rmin=(R1>R2)? R2 : R1;
    RmR_Relative=RmR/Rmax;
  }

  Standard_Real DistA1A2=A1A2.Distance();
  
  if(A1A2.Parallel()) {
    if(DistA1A2<=Tol) {
      if(RmR<=Tol) {
	typeres=IntAna_Same;
      }
      else {
	typeres=IntAna_Empty;
      }
    }
    else {  //-- DistA1A2 > Tol
      gp_Pnt P1=Cyl1.Location();
      gp_Pnt P2t=Cyl2.Location();
      gp_Pnt P2;
      //-- P2t is projected on the plane (P1,DirCylX,DirCylY)
      gp_Dir DirCyl = Cyl1.Position().Direction();
      Standard_Real ProjP2OnDirCyl1=gp_Vec(DirCyl).Dot(gp_Vec(P1,P2t));
      
      P2.SetCoord( P2t.X() - ProjP2OnDirCyl1*DirCyl.X()
		  ,P2t.Y() - ProjP2OnDirCyl1*DirCyl.Y()
		  ,P2t.Z() - ProjP2OnDirCyl1*DirCyl.Z());
      //-- 
      Standard_Real R1pR2=R1+R2;
      if(DistA1A2>(R1pR2+Tol)) { 
	typeres=IntAna_Empty;
	nbint=0;
      }
      else if(DistA1A2>(R1pR2)) {
	//-- 1 Tangent line -------------------------------------OK
	typeres=IntAna_Line;

	nbint=1;
	dir1=DirCyl;
	Standard_Real R1_R1pR2=R1/R1pR2;
	pt1.SetCoord( P1.X() + R1_R1pR2 * (P2.X()-P1.X())
		     ,P1.Y() + R1_R1pR2 * (P2.Y()-P1.Y())
		     ,P1.Z() + R1_R1pR2 * (P2.Z()-P1.Z()));
	
      }
      else if(DistA1A2>RmR) {
	//-- 2 lines ---------------------------------------------OK
	typeres=IntAna_Line;
	nbint=2;
	dir1=DirCyl;
	gp_Vec P1P2(P1,P2);
	gp_Dir DirA1A2=gp_Dir(P1P2);
	gp_Dir Ortho_dir1_P1P2 = dir1.Crossed(DirA1A2);
	dir2=dir1;
	Standard_Real Alpha=0.5*(R1*R1-R2*R2+DistA1A2*DistA1A2)/(DistA1A2);       

// 	Standard_Real Beta = Sqrt(R1*R1-Alpha*Alpha);
	Standard_Real anSqrtArg = R1*R1-Alpha*Alpha;
	Standard_Real Beta = (anSqrtArg > 0.) ? Sqrt(anSqrtArg) : 0.;
	
	if((Beta+Beta)<Tol) { 
	  nbint=1;
	  pt1.SetCoord( P1.X() + Alpha*DirA1A2.X()
		       ,P1.Y() + Alpha*DirA1A2.Y()
		       ,P1.Z() + Alpha*DirA1A2.Z());
	}
	else { 
	  pt1.SetCoord( P1.X() + Alpha*DirA1A2.X() + Beta*Ortho_dir1_P1P2.X()
		       ,P1.Y() + Alpha*DirA1A2.Y() + Beta*Ortho_dir1_P1P2.Y()
		       ,P1.Z() + Alpha*DirA1A2.Z() + Beta*Ortho_dir1_P1P2.Z() );
	  
	  pt2.SetCoord( P1.X() + Alpha*DirA1A2.X() - Beta*Ortho_dir1_P1P2.X()
		       ,P1.Y() + Alpha*DirA1A2.Y() - Beta*Ortho_dir1_P1P2.Y()
		       ,P1.Z() + Alpha*DirA1A2.Z() - Beta*Ortho_dir1_P1P2.Z());
	}
      }
      else if(DistA1A2>(RmR-Tol)) {
	//-- 1 Tangent ------------------------------------------OK
	typeres=IntAna_Line;
	nbint=1;
	dir1=DirCyl;
	Standard_Real R1_RmR=R1/RmR;

	if(R1 < R2) R1_RmR = -R1_RmR;

	pt1.SetCoord( P1.X() + R1_RmR * (P2.X()-P1.X())
		     ,P1.Y() + R1_RmR * (P2.Y()-P1.Y())
		     ,P1.Z() + R1_RmR * (P2.Z()-P1.Z()));
      }
      else {
	nbint=0;
	typeres=IntAna_Empty;
      }
    }
  }
  else { //-- No Parallel Axis ---------------------------------OK
    if((RmR_Relative<=myEPSILON_CYLINDER_DELTA_RADIUS) 
       && (DistA1A2 <= myEPSILON_CYLINDER_DELTA_DISTANCE)) {
      //-- PI/2 between the two axis   and   Intersection  
      //-- and identical radius
      typeres=IntAna_Ellipse;
      nbint=2;
      gp_Dir DirCyl1=Cyl1.Position().Direction();
      gp_Dir DirCyl2=Cyl2.Position().Direction();
      pt1=pt2=A1A2.PtIntersect();
      
      Standard_Real A=DirCyl1.Angle(DirCyl2);
      Standard_Real B;
      B=Abs(Sin(0.5*(M_PI-A)));
      A=Abs(Sin(0.5*A));
      
      if(A==0.0 || B==0.0) {
	typeres=IntAna_Same;
	return;
      }
      
      
      gp_Vec dircyl1(DirCyl1);gp_Vec dircyl2(DirCyl2);
      dir1 = gp_Dir(dircyl1.Added(dircyl2));
      dir2 = gp_Dir(dircyl1.Subtracted(dircyl2));
	
      param2   = Cyl1.Radius() / A;
      param1   = Cyl1.Radius() / B;
      param2bis= param1bis = Cyl1.Radius();
      if(param1 < param1bis) {
	A=param1; param1=param1bis; param1bis=A;
      }
      if(param2 < param2bis) {
	A=param2; param2=param2bis; param2bis=A;
      }
    }
    else {
      if(Abs(DistA1A2-Cyl1.Radius()-Cyl2.Radius())<Tol) { 
	typeres = IntAna_Point;
	Standard_Real d,p1,p2;

	gp_Dir D1 = Cyl1.Axis().Direction();
	gp_Dir D2 = Cyl2.Axis().Direction();
	A1A2.Distance(d,p1,p2);
	gp_Pnt P = Cyl1.Axis().Location();
	gp_Pnt P1(P.X() - p1*D1.X(),
		  P.Y() - p1*D1.Y(),
		  P.Z() - p1*D1.Z());
	P = Cyl2.Axis().Location();
	gp_Pnt P2(P.X() - p2*D2.X(),
		  P.Y() - p2*D2.Y(),
		  P.Z() - p2*D2.Z());
	gp_Vec P1P2(P1,P2);
	D1=gp_Dir(P1P2);
	p1=Cyl1.Radius();
	pt1.SetCoord(P1.X() + p1*D1.X(),
		     P1.Y() + p1*D1.Y(),
		     P1.Z() + p1*D1.Z());
	nbint = 1;
      }
      else {
	typeres=IntAna_NoGeometricSolution;
      }
    }
  }
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Cylinder - Cone
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Cylinder& Cyl,
					 const gp_Cone& Con,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Cyl,Con,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Cylinder& Cyl,
				   const gp_Cone& Con,
				   const Standard_Real ) 
{
  done=Standard_True;
  AxeOperator A1A2(Cyl.Axis(),Con.Axis());
  if(A1A2.Same()) {
    gp_Pnt Pt=Con.Apex();
    Standard_Real dist=Cyl.Radius()/(Tan(Con.SemiAngle()));
    gp_Dir dir=Cyl.Position().Direction();
    pt1.SetCoord( Pt.X() + dist*dir.X()
		 ,Pt.Y() + dist*dir.Y()
		 ,Pt.Z() + dist*dir.Z());
    pt2.SetCoord( Pt.X() - dist*dir.X()
		 ,Pt.Y() - dist*dir.Y()
		 ,Pt.Z() - dist*dir.Z());
    dir1=dir2=dir;
    param1=param2=Cyl.Radius();
    nbint=2;
    typeres=IntAna_Circle;

  }
  else {
    typeres=IntAna_NoGeometricSolution;
  }
}
//=======================================================================
//function : 
//purpose  : Cylinder - Sphere
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Cylinder& Cyl,
					 const gp_Sphere& Sph,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Cyl,Sph,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform( const gp_Cylinder& Cyl
				   ,const gp_Sphere& Sph
				   ,const Standard_Real)
{
  done=Standard_True;
  gp_Pnt Pt=Sph.Location();
  AxeOperator A1A2(Cyl.Axis(),Sph.Position().Axis());
  if((A1A2.Intersect()  && Pt.Distance(A1A2.PtIntersect())==0.0 )
     || (A1A2.Same()))      {
    if(Sph.Radius() < Cyl.Radius()) { 
      typeres = IntAna_Empty;
    }
    else { 
      Standard_Real dist=Sqrt( Sph.Radius() * Sph.Radius() - Cyl.Radius() * Cyl.Radius() );
      gp_Dir dir=Cyl.Position().Direction();
      dir1 = dir2 = dir;
      typeres=IntAna_Circle;
      pt1.SetCoord( Pt.X() + dist*dir.X()
		   ,Pt.Y() + dist*dir.Y()
		   ,Pt.Z() + dist*dir.Z());
      nbint=1;
      param1 = Cyl.Radius();
      if(dist>RealEpsilon()) {
	pt2.SetCoord( Pt.X() - dist*dir.X()
		     ,Pt.Y() - dist*dir.Y()
		     ,Pt.Z() - dist*dir.Z());
	param2=Cyl.Radius();
	nbint=2;
      }
    }
  }
  else {
    typeres=IntAna_NoGeometricSolution; 
  }
}

//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Cone - Cone
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Cone& Con1,
					 const gp_Cone& Con2,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Con1,Con2,Tol);
}
//
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Cone& Con1,
				   const gp_Cone& Con2,
				   const Standard_Real Tol) 
{
  done=Standard_True;
  //
  Standard_Real tg1, tg2, aDA1A2, aTol2;
  gp_Pnt aPApex1, aPApex2;

  Standard_Real TOL_APEX_CONF = 1.e-10;
  
  //
  tg1=Tan(Con1.SemiAngle());
  tg2=Tan(Con2.SemiAngle());

  if((tg1 * tg2) < 0.) {
    tg2 = -tg2;
  }
  //
  aTol2=Tol*Tol;
  aPApex1=Con1.Apex();
  aPApex2=Con2.Apex();
  aDA1A2=aPApex1.SquareDistance(aPApex2);
  //
  AxeOperator A1A2(Con1.Axis(),Con2.Axis());
  //
  // 1
  if(A1A2.Same()) {
    //-- two circles 
    Standard_Real x;
    gp_Pnt P=Con1.Apex();
    gp_Dir D=Con1.Position().Direction();
    Standard_Real d=gp_Vec(D).Dot(gp_Vec(P,Con2.Apex()));
    
    if(Abs(tg1-tg2)>myEPSILON_ANGLE_CONE) { 
      if (fabs(d) < TOL_APEX_CONF) {
	typeres = IntAna_Point;
	nbint = 1;
	pt1 = P;
	return;
      }
      x=(d*tg2)/(tg1+tg2);
      pt1.SetCoord( P.X() + x*D.X()
		   ,P.Y() + x*D.Y()
		   ,P.Z() + x*D.Z());
      param1=Abs(x*tg1);

      x=(d*tg2)/(tg2-tg1);
      pt2.SetCoord( P.X() + x*D.X()
		   ,P.Y() + x*D.Y()
		   ,P.Z() + x*D.Z());
      param2=Abs(x*tg1);
      dir1 = dir2 = D;
      nbint=2;
      typeres=IntAna_Circle;
    }
    else {
      if (fabs(d) < TOL_APEX_CONF) { 
	typeres=IntAna_Same;
      }
      else {
	typeres=IntAna_Circle;
	nbint=1;
	x=d*0.5;
	pt1.SetCoord( P.X() + x*D.X()
		     ,P.Y() + x*D.Y()
		     ,P.Z() + x*D.Z());
	param1 = Abs(x * tg1);
	dir1 = D;
      }
    }
  } //-- fin A1A2.Same
  // 2
  else if((Abs(tg1-tg2)<myEPSILON_ANGLE_CONE) && (A1A2.Parallel())) {
    //-- voir AnVer12mai98
    Standard_Real DistA1A2=A1A2.Distance();
    gp_Dir DA1=Con1.Position().Direction();
    gp_Vec O1O2(Con1.Apex(),Con2.Apex());
    Standard_Real O1O2_DA1=gp_Vec(DA1).Dot(O1O2);
    
    gp_Vec O1_Proj_A2(O1O2.X()-O1O2_DA1*DA1.X(),
		      O1O2.Y()-O1O2_DA1*DA1.Y(),
		      O1O2.Z()-O1O2_DA1*DA1.Z());
    gp_Dir DB1=gp_Dir(O1_Proj_A2);
    
    Standard_Real yO1O2=O1O2.Dot(gp_Vec(DA1));
    Standard_Real ABSTG1 = Abs(tg1);
    Standard_Real X2 = (DistA1A2/ABSTG1 - yO1O2)*0.5;
    Standard_Real X1 = X2+yO1O2;
    
    gp_Pnt P1(Con1.Apex().X() + X1*( DA1.X() + ABSTG1*DB1.X()),
	      Con1.Apex().Y() + X1*( DA1.Y() + ABSTG1*DB1.Y()), 
	      Con1.Apex().Z() + X1*( DA1.Z() + ABSTG1*DB1.Z()));

    gp_Pnt MO1O2(0.5*(Con1.Apex().X()+Con2.Apex().X()),
		 0.5*(Con1.Apex().Y()+Con2.Apex().Y()),
		 0.5*(Con1.Apex().Z()+Con2.Apex().Z()));
    gp_Vec P1MO1O2(P1,MO1O2);
    
    gp_Dir DA1_X_DB1=DA1.Crossed(DB1);
    gp_Dir OrthoPln =  DA1_X_DB1.Crossed(gp_Dir(P1MO1O2));
    
    IntAna_QuadQuadGeo INTER_QUAD_PLN(gp_Pln(P1,OrthoPln),Con1,Tol,Tol);
      if(INTER_QUAD_PLN.IsDone()) {
      switch(INTER_QUAD_PLN.TypeInter()) {
      case IntAna_Ellipse: 	{
	typeres=IntAna_Ellipse;
	gp_Elips E=INTER_QUAD_PLN.Ellipse(1);
	pt1 = E.Location();
	dir1 = E.Position().Direction();
	dir2 = E.Position().XDirection();
	param1 = E.MajorRadius();
	param1bis = E.MinorRadius();
	nbint = 1;
	break; 
      }
      case IntAna_Circle: {
	typeres=IntAna_Circle;
	gp_Circ C=INTER_QUAD_PLN.Circle(1);
	pt1 = C.Location();
	dir1 = C.Position().XDirection();
	dir2 = C.Position().YDirection();
	param1 = C.Radius();
	nbint = 1;
	break;
      }
      case IntAna_Hyperbola: {
	typeres=IntAna_Hyperbola;
	gp_Hypr H=INTER_QUAD_PLN.Hyperbola(1);
	pt1 = pt2 = H.Location();
	dir1 = H.Position().Direction();
	dir2 = H.Position().XDirection();
	param1 = param2 = H.MajorRadius();
	param1bis = param2bis = H.MinorRadius();
	nbint = 2;
	break;
      }
      case IntAna_Line: {
	typeres=IntAna_Line;
	gp_Lin H=INTER_QUAD_PLN.Line(1);
	pt1 = pt2 = H.Location();
	dir1 = dir2 = H.Position().Direction();
	param1 = param2 = 0.0;
	param1bis = param2bis = 0.0;
	nbint = 2;
	break;
      }
      default:
	typeres=IntAna_NoGeometricSolution; 
      }
    }
  }// else if((Abs(tg1-tg2)<EPSILON_ANGLE_CONE) && (A1A2.Parallel()))
  // 3
  else if (aDA1A2<aTol2) {
    //
    // When apices are coinsided there can be 3 possible cases
    // 3.1 - empty solution (iRet=0)
    // 3.2 - one line  when cone1 touches cone2 (iRet=1)
    // 3.3 - two lines when cone1 intersects cone2 (iRet=2)
    //
    Standard_Integer iRet;
    Standard_Real aGamma, aBeta1, aBeta2;
    Standard_Real aD1, aR1, aTgBeta1, aTgBeta2, aHalfPI;
    Standard_Real aCosGamma, aSinGamma, aDx, aR2, aRD2, aD2;
    gp_Pnt2d aP0, aPA1, aP1, aPA2;
    gp_Vec2d aVAx2;
    gp_Ax1 aAx1, aAx2;
    //
    // Preliminary analysis. Determination of iRet
    //
    iRet=0;
    aHalfPI=0.5*M_PI;
    aD1=1.;
    aPA1.SetCoord(aD1, 0.);
    aP0.SetCoord(0., 0.);
    //
    aAx1=Con1.Axis();
    aAx2=Con2.Axis();
    aGamma=aAx1.Angle(aAx2);
    if (aGamma>aHalfPI){
      aGamma=M_PI-aGamma;
    }
    aCosGamma=Cos(aGamma);
    aSinGamma=Sin(aGamma);
    //
    aBeta1=Con1.SemiAngle();
    aTgBeta1=Tan(aBeta1);
    aTgBeta1=Abs(aTgBeta1);
    //
    aBeta2=Con2.SemiAngle();
    aTgBeta2=Tan(aBeta2);
    aTgBeta2=Abs(aTgBeta2);
    //
    aR1=aD1*aTgBeta1;
    aP1.SetCoord(aD1, aR1);
    //
    // PA2
    aVAx2.SetCoord(aCosGamma, aSinGamma);
    gp_Dir2d aDAx2(aVAx2);
    gp_Lin2d aLAx2(aP0, aDAx2);
    //
    gp_Vec2d aV(aP0, aP1);
    aDx=aVAx2.Dot(aV);
    aPA2=aP0.Translated(aDx*aDAx2);
    //
    // aR2
    aDx=aPA2.Distance(aP0);
    aR2=aDx*aTgBeta2;
    //
    // aRD2
    aRD2=aPA2.Distance(aP1);
    //
    if (aRD2>(aR2+Tol)) {
      iRet=0;
      typeres=IntAna_Empty; //nothing
      return;
    }
    //
    iRet=1; //touch case => 1 line
    if (aRD2<(aR2-Tol)) {
      iRet=2;//intersection => couple of lines
    }
    //
    // Finding the solution in 3D
    //
    Standard_Real aDa;
    gp_Pnt aQApex1, aQA1, aQA2, aQX, aQX1, aQX2;
    gp_Dir aD3Ax1, aD3Ax2;
    gp_Lin aLin;
    IntAna_QuadQuadGeo aIntr;
    //
    aQApex1=Con1.Apex();
    aD3Ax1=aAx1.Direction(); 
    aQA1.SetCoord(aQApex1.X()+aD1*aD3Ax1.X(),
		  aQApex1.Y()+aD1*aD3Ax1.Y(),
		  aQApex1.Z()+aD1*aD3Ax1.Z());
    //
    aDx=aD3Ax1.Dot(aAx2.Direction());
    if (aDx<0.) {
      aAx2.Reverse();
    }
    aD3Ax2=aAx2.Direction();
    //
    aD2=aD1*sqrt((1.+aTgBeta1*aTgBeta1)/(1.+aTgBeta2*aTgBeta2));
    //
    aQA2.SetCoord(aQApex1.X()+aD2*aD3Ax2.X(),
		  aQApex1.Y()+aD2*aD3Ax2.Y(),
		  aQApex1.Z()+aD2*aD3Ax2.Z());
    //
    gp_Pln aPln1(aQA1, aD3Ax1);
    gp_Pln aPln2(aQA2, aD3Ax2);
    //
    aIntr.Perform(aPln1, aPln2, Tol, Tol);
    if (!aIntr.IsDone()) {
      iRet=-1; // just in case. it must not be so
      typeres=IntAna_NoGeometricSolution; 
      return;
    }
    //
    aLin=aIntr.Line(1);
    const gp_Dir& aDLin=aLin.Direction();
    gp_Vec aVLin(aDLin);
    gp_Pnt aOrig=aLin.Location();
    gp_Vec aVr(aQA1, aOrig);
    aDx=aVLin.Dot(aVr);
    aQX=aOrig.Translated(aDx*aVLin);
    //
    // Final part
    //
    typeres=IntAna_Line; 
    //
    param1=0.;
    param2 =0.;
    param1bis=0.;
    param2bis=0.;
    //
    if (iRet==1) {
      // one line
      nbint=1;
      pt1=aQApex1;
      gp_Vec aVX(aQApex1, aQX);
      dir1=gp_Dir(aVX);
    }
    
    else {//iRet=2 
      // two lines
      nbint=2;
      aDa=aQA1.Distance(aQX);
      aDx=sqrt(aR1*aR1-aDa*aDa);
      aQX1=aQX.Translated(aDx*aVLin);
      aQX2=aQX.Translated(-aDx*aVLin);
      //
      pt1=aQApex1;
      pt2=aQApex1;
      gp_Vec aVX1(aQApex1, aQX1);
      dir1=gp_Dir(aVX1);
      gp_Vec aVX2(aQApex1, aQX2);
      dir2=gp_Dir(aVX2);
    }
  } //else if (aDA1A2<aTol2) {
  //Case when cones have common generatrix
  else if(A1A2.Intersect()) {
    //Check if apex of one cone belongs another one
    Standard_Real u, v, tol2 = Tol*Tol;
    ElSLib::Parameters(Con2, aPApex1, u, v);
    gp_Pnt p = ElSLib::Value(u, v, Con2);
    if(aPApex1.SquareDistance(p) > tol2) {
      typeres=IntAna_NoGeometricSolution; 
      return;
    }
    //
    ElSLib::Parameters(Con1, aPApex2, u, v);
    p = ElSLib::Value(u, v, Con1);
    if(aPApex2.SquareDistance(p) > tol2) {
      typeres=IntAna_NoGeometricSolution; 
      return;
    }

    //Cones have a common generatrix passing through apexes
    myCommonGen = Standard_True;

    //common generatrix of cones
    gp_Lin aGen(aPApex1, gp_Dir(gp_Vec(aPApex1, aPApex2)));

    //Intersection point of axes
    gp_Pnt aPAxeInt = A1A2.PtIntersect();

    //Characteristic point of intersection curve
    u = ElCLib::Parameter(aGen, aPAxeInt);
    myPChar = ElCLib::Value(u, aGen);


    //Other generatrixes of cones laying in maximal plane
    gp_Lin aGen1 = aGen.Rotated(Con1.Axis(), M_PI); 
    gp_Lin aGen2 = aGen.Rotated(Con2.Axis(), M_PI); 
    //
    //Intersection point of generatrixes
    gp_Dir aN; //solution plane normal
    gp_Dir aD1 = aGen1.Direction();

    gp_Dir aD2(aD1.Crossed(aGen.Direction()));

    if(aD1.IsParallel(aGen2.Direction(), Precision::Angular())) {
      aN = aD1.Crossed(aD2);
    }
    else if(aGen1.SquareDistance(aGen2) > tol2) {
      //Something wrong ???
      typeres=IntAna_NoGeometricSolution; 
      return;
    }
    else {
      gp_Dir D1 = aGen1.Position().Direction();
      gp_Dir D2 = aGen2.Position().Direction();
      gp_Pnt O1 = aGen1.Location();
      gp_Pnt O2 = aGen2.Location();
      Standard_Real D1DotD2 = D1.Dot(D2);
      Standard_Real aSin = 1.-D1DotD2*D1DotD2;
      gp_Vec O1O2 (O1,O2);
      Standard_Real U2 = (D1.XYZ()*(O1O2.Dot(D1))-(O1O2.XYZ())).Dot(D2.XYZ());
      U2 /= aSin;
      gp_Pnt aPGint(ElCLib::Value(U2, aGen2));
    
      aD1 = gp_Dir(gp_Vec(aPGint, myPChar));
      aN = aD1.Crossed(aD2);
    }
    //Plane that must contain intersection curves
    gp_Pln anIntPln(myPChar, aN);

    IntAna_QuadQuadGeo INTER_QUAD_PLN(anIntPln,Con1,Tol,Tol);

      if(INTER_QUAD_PLN.IsDone()) {
      switch(INTER_QUAD_PLN.TypeInter()) {
      case IntAna_Ellipse: 	{
	typeres=IntAna_Ellipse;
	gp_Elips E=INTER_QUAD_PLN.Ellipse(1);
	pt1 = E.Location();
	dir1 = E.Position().Direction();
	dir2 = E.Position().XDirection();
	param1 = E.MajorRadius();
	param1bis = E.MinorRadius();
	nbint = 1;
	break; 
      }
      case IntAna_Circle: {
	typeres=IntAna_Circle;
	gp_Circ C=INTER_QUAD_PLN.Circle(1);
	pt1 = C.Location();
	dir1 = C.Position().XDirection();
	dir2 = C.Position().YDirection();
	param1 = C.Radius();
	nbint = 1;
	break;
      }
      case IntAna_Parabola: {
	typeres=IntAna_Parabola;
	gp_Parab Prb=INTER_QUAD_PLN.Parabola(1);
	pt1 = Prb.Location();
	dir1 = Prb.Position().Direction();
	dir2 = Prb.Position().XDirection();
	param1 = Prb.Focal();
	nbint = 1;
	break;
      }
      case IntAna_Hyperbola: {
	typeres=IntAna_Hyperbola;
	gp_Hypr H=INTER_QUAD_PLN.Hyperbola(1);
	pt1 = pt2 = H.Location();
	dir1 = H.Position().Direction();
	dir2 = H.Position().XDirection();
	param1 = param2 = H.MajorRadius();
	param1bis = param2bis = H.MinorRadius();
	nbint = 2;
	break;
      }
      default:
	typeres=IntAna_NoGeometricSolution; 
      }
    }
  }
  
  else {
    typeres=IntAna_NoGeometricSolution; 
  }
}
//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Sphere - Cone
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo(const gp_Sphere& Sph,
					 const gp_Cone& Con,
					 const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Sph,Con,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Sphere& Sph,
				   const gp_Cone& Con,
				   const Standard_Real)
{
  
  //
  done=Standard_True;
  //
  AxeOperator A1A2(Con.Axis(),Sph.Position().Axis());
  gp_Pnt Pt=Sph.Location();
  //
  if((A1A2.Intersect() && (Pt.Distance(A1A2.PtIntersect())==0.0))
     || A1A2.Same()) {
    gp_Pnt ConApex= Con.Apex();
    Standard_Real dApexSphCenter=Pt.Distance(ConApex); 
    gp_Dir ConDir;
    if(dApexSphCenter>RealEpsilon()) { 
      ConDir = gp_Dir(gp_Vec(ConApex,Pt));
    }
    else { 
      ConDir = Con.Position().Direction();
    }
    
    Standard_Real Rad=Sph.Radius();
    Standard_Real tga=Tan(Con.SemiAngle());


    //-- 2 circles
    //-- x: Roots of    (x**2 + y**2 = Rad**2)
    //--                tga = y / (x+dApexSphCenter)
    Standard_Real tgatga = tga * tga;
    math_DirectPolynomialRoots Eq( 1.0+tgatga
				  ,2.0*tgatga*dApexSphCenter
				  ,-Rad*Rad + dApexSphCenter*dApexSphCenter*tgatga);
    if(Eq.IsDone()) {
      Standard_Integer nbsol=Eq.NbSolutions();
      if(nbsol==0) {
	typeres=IntAna_Empty;
      }
      else { 
	typeres=IntAna_Circle;
	if(nbsol>=1) {
	  Standard_Real x = Eq.Value(1);
	  Standard_Real dApexSphCenterpx = dApexSphCenter+x;
	  nbint=1;
	  pt1.SetCoord( ConApex.X() + (dApexSphCenterpx) * ConDir.X()
		       ,ConApex.Y() + (dApexSphCenterpx) * ConDir.Y()
		       ,ConApex.Z() + (dApexSphCenterpx) * ConDir.Z());
	  param1 = tga * dApexSphCenterpx;
	  param1 = Abs(param1);
	  dir1 = ConDir;
	  if(param1<=myEPSILON_MINI_CIRCLE_RADIUS) {
	    typeres=IntAna_PointAndCircle;
	    param1=0.0;
	  }
	}
	if(nbsol>=2) {
	  Standard_Real x=Eq.Value(2);
	  Standard_Real dApexSphCenterpx = dApexSphCenter+x;
	  nbint=2;
	  pt2.SetCoord( ConApex.X() + (dApexSphCenterpx) * ConDir.X()
		       ,ConApex.Y() + (dApexSphCenterpx) * ConDir.Y()
		       ,ConApex.Z() + (dApexSphCenterpx) * ConDir.Z());
	  param2 = tga * dApexSphCenterpx;
	  param2 = Abs(param2);
	  dir2=ConDir;
	  if(param2<=myEPSILON_MINI_CIRCLE_RADIUS) {
	    typeres=IntAna_PointAndCircle;
	    param2=0.0;
	  }
	}
      }
    }
    else {
      done=Standard_False;
    }
  }
  else {
    typeres=IntAna_NoGeometricSolution; 
  }
}

//=======================================================================
//function : IntAna_QuadQuadGeo
//purpose  : Sphere - Sphere
//=======================================================================
  IntAna_QuadQuadGeo::IntAna_QuadQuadGeo( const gp_Sphere& Sph1
					 ,const gp_Sphere& Sph2
					 ,const Standard_Real Tol) 
: done(Standard_False),
  nbint(0),
  typeres(IntAna_Empty),
  pt1(0,0,0),
  pt2(0,0,0),
  param1(0),
  param2(0),
  param1bis(0),
  param2bis(0),
  myCommonGen(Standard_False),
  myPChar(0,0,0)
{
  InitTolerances();
  Perform(Sph1,Sph2,Tol);
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
  void IntAna_QuadQuadGeo::Perform(const gp_Sphere& Sph1,
				   const gp_Sphere& Sph2,
				   const Standard_Real Tol)   
{
  done=Standard_True;
  gp_Pnt O1=Sph1.Location();
  gp_Pnt O2=Sph2.Location();
  Standard_Real dO1O2=O1.Distance(O2);
  Standard_Real R1=Sph1.Radius();
  Standard_Real R2=Sph2.Radius();
  Standard_Real Rmin,Rmax;
  typeres=IntAna_Empty;
  param2bis=0.0; //-- pour eviter param2bis not used .... 

  if(R1>R2) { Rmin=R2; Rmax=R1; } else { Rmin=R1; Rmax=R2; }
  
  if(dO1O2<=Tol && (Abs(R1-R2) <= Tol)) {
    typeres = IntAna_Same;
  }
  else { 
    if(dO1O2<=Tol) { return; } 
    gp_Dir Dir=gp_Dir(gp_Vec(O1,O2));
    Standard_Real t = Rmax - dO1O2 - Rmin;

    //----------------------------------------------------------------------
    //--        |----------------- R1 --------------------|
    //--        |----dO1O2-----|-----------R2----------|
    //--                                            --->--<-- t
    //--
    //--        |------ R1 ------|---------dO1O2----------|
    //--     |-------------------R2-----------------------|
    //--  --->--<-- t
    //----------------------------------------------------------------------
    if(t >= 0.0  && t <=Tol) { 
      typeres = IntAna_Point;
      nbint = 1;
      Standard_Real t2;
      if(R1==Rmax) t2=(R1 + (R2 + dO1O2)) * 0.5;
      else         t2=(-R1+(dO1O2-R2))*0.5;
	
      pt1.SetCoord( O1.X() + t2*Dir.X()
		   ,O1.Y() + t2*Dir.Y()
		   ,O1.Z() + t2*Dir.Z());
    }
    else  {
      //-----------------------------------------------------------------
      //--        |----------------- dO1O2 --------------------|
      //--        |----R1-----|-----------R2----------|-Tol-|
      //--                                            
      //--        |----------------- Rmax --------------------|
      //--        |----Rmin----|-------dO1O2-------|-Tol-|
      //--                                            
      //-----------------------------------------------------------------
      if((dO1O2 > (R1+R2+Tol)) || (Rmax > (dO1O2+Rmin+Tol))) {
	typeres=IntAna_Empty;
      }
      else {
	//---------------------------------------------------------------
	//--     
	//--
	//---------------------------------------------------------------
	Standard_Real Alpha=0.5*(R1*R1-R2*R2+dO1O2*dO1O2)/(dO1O2);       
	Standard_Real Beta = R1*R1-Alpha*Alpha;
	Beta = (Beta>0.0)? Sqrt(Beta) : 0.0;
	
	if(Beta<= myEPSILON_MINI_CIRCLE_RADIUS) { 
	  typeres = IntAna_Point;
	  Alpha = (R1 + (dO1O2 - R2)) * 0.5;
	}
	else { 
	  typeres = IntAna_Circle;
	  dir1 = Dir;
	  param1 = Beta;
	}	  
	pt1.SetCoord( O1.X() + Alpha*Dir.X()
		     ,O1.Y() + Alpha*Dir.Y()
		     ,O1.Z() + Alpha*Dir.Z());
	
	nbint=1;
      }
    }
  }
}
//=======================================================================
//function : Point
//purpose  : Returns a Point
//=======================================================================
  gp_Pnt IntAna_QuadQuadGeo::Point(const Standard_Integer n) const 
{
  if(!done)          {    StdFail_NotDone::Raise();        }
  if(n>nbint || n<1) {    Standard_DomainError::Raise();   }
  if(typeres==IntAna_PointAndCircle) {
    if(n!=1) { Standard_DomainError::Raise();  }
    if(param1==0.0) return(pt1);
    return(pt2);
  }
  else if(typeres==IntAna_Point) {
    if(n==1) return(pt1);
    return(pt2);
  }

  // WNT (what can you expect from MicroSoft ?)
  return gp_Pnt(0,0,0);
}
//=======================================================================
//function : Line
//purpose  : Returns a Line
//=======================================================================
  gp_Lin IntAna_QuadQuadGeo::Line(const Standard_Integer n) const 
{
  if(!done)        {   StdFail_NotDone::Raise();   }
  if((n>nbint) || (n<1) || (typeres!=IntAna_Line)) {
    Standard_DomainError::Raise();
    }
  if(n==1) {  return(gp_Lin(pt1,dir1));   }
  else {      return(gp_Lin(pt2,dir2));   }
}
//=======================================================================
//function : Circle
//purpose  : Returns a Circle
//=======================================================================
  gp_Circ IntAna_QuadQuadGeo::Circle(const Standard_Integer n) const 
{
  if(!done) {    StdFail_NotDone::Raise();     }
  if(typeres==IntAna_PointAndCircle) {
    if(n!=1) { Standard_DomainError::Raise();  }
    if(param2==0.0) return(gp_Circ(DirToAx2(pt1,dir1),param1));
    return(gp_Circ(DirToAx2(pt2,dir2),param2));
  }
  else if((n>nbint) || (n<1) || (typeres!=IntAna_Circle)) {
    Standard_DomainError::Raise();
    }
  if(n==1) { return(gp_Circ(DirToAx2(pt1,dir1),param1));   }
  else {     return(gp_Circ(DirToAx2(pt2,dir2),param2));   }
}

//=======================================================================
//function : Ellipse
//purpose  : Returns a Elips  
//=======================================================================
  gp_Elips IntAna_QuadQuadGeo::Ellipse(const Standard_Integer n) const
{
  if(!done) {     StdFail_NotDone::Raise();     }
  if((n>nbint) || (n<1) || (typeres!=IntAna_Ellipse)) {
    Standard_DomainError::Raise();
  }

  if(n==1) {
    Standard_Real R1=param1, R2=param1bis, aTmp;
    if (R1<R2) {
      aTmp=R1; R1=R2; R2=aTmp;
    }
    gp_Ax2 anAx2(pt1, dir1 ,dir2);
    gp_Elips anElips (anAx2, R1, R2);
    return anElips;
  }
  else {
    Standard_Real R1=param2, R2=param2bis, aTmp;
    if (R1<R2) {
      aTmp=R1; R1=R2; R2=aTmp;
    }
    gp_Ax2 anAx2(pt2, dir2 ,dir1);
    gp_Elips anElips (anAx2, R1, R2);
    return anElips;
  }
}
//=======================================================================
//function : Parabola
//purpose  : Returns a Parabola 
//=======================================================================
  gp_Parab IntAna_QuadQuadGeo::Parabola(const Standard_Integer n) const 
{
  if(!done) {
    StdFail_NotDone::Raise();
    }
  if (typeres!=IntAna_Parabola) {
    Standard_DomainError::Raise();
  }
  if((n>nbint) || (n!=1)) {
    Standard_OutOfRange::Raise();
  }
  return(gp_Parab(gp_Ax2( pt1
			 ,dir1
			 ,dir2)
		  ,param1));
}
//=======================================================================
//function : Hyperbola
//purpose  : Returns a Hyperbola  
//=======================================================================
  gp_Hypr IntAna_QuadQuadGeo::Hyperbola(const Standard_Integer n) const 
{
  if(!done) {
    StdFail_NotDone::Raise();
    }
  if((n>nbint) || (n<1) || (typeres!=IntAna_Hyperbola)) {
    Standard_DomainError::Raise();
    }
  if(n==1) {
    return(gp_Hypr(gp_Ax2( pt1
			  ,dir1
			  ,dir2)
		   ,param1,param1bis));
  }
  else {
    return(gp_Hypr(gp_Ax2( pt2
			  ,dir1
			  ,dir2.Reversed())
		   ,param2,param2bis));
  }
}
//=======================================================================
//function : HasCommonGen
//purpose  : 
//=======================================================================
Standard_Boolean IntAna_QuadQuadGeo::HasCommonGen() const
{
  return myCommonGen;
}
//=======================================================================
//function : PChar
//purpose  : 
//=======================================================================
const gp_Pnt& IntAna_QuadQuadGeo::PChar() const
{
  return myPChar;
}
//=======================================================================
//function : RefineDir
//purpose  : 
//=======================================================================
void RefineDir(gp_Dir& aDir)
{
  Standard_Integer k, m, n;
  Standard_Real aC[3];
  //
  aDir.Coord(aC[0], aC[1], aC[2]);
  //
  m=0;
  n=0;
  for (k=0; k<3; ++k) {
    if (aC[k]==1. || aC[k]==-1.) {
      ++m;
    }
    else if (aC[k]!=0.) {
      ++n;
    }
  }
  //
  if (m && n) {
    Standard_Real aEps, aR1, aR2, aNum;
    //
    aEps=RealEpsilon();
    aR1=1.-aEps;
    aR2=1.+aEps;
    //
    for (k=0; k<3; ++k) {
      m=(aC[k]>0.);
      aNum=(m)? aC[k] : -aC[k];
      if (aNum>aR1 && aNum<aR2) {
	if (m) {
	  aC[k]=1.;
	}	  
	else {
	  aC[k]=-1.;
	}
	//
	aC[(k+1)%3]=0.;
	aC[(k+2)%3]=0.;
	break;
      }
    }
    aDir.SetCoord(aC[0], aC[1], aC[2]);
  }
}



