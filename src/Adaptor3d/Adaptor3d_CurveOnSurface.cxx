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

#define No_Standard_OutOfRange


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_InterFunc.hxx>
#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <math_FunctionRoots.hxx>
#include <Precision.hxx>
#include <Standard_Assert.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HSequenceOfReal.hxx>

static gp_Pnt to3d(const gp_Pln& Pl, const gp_Pnt2d& P)
{
  return ElSLib::Value(P.X(),P.Y(),Pl);
}

static gp_Vec to3d(const gp_Pln& Pl, const gp_Vec2d& V)
{
  gp_Vec Vx = Pl.XAxis().Direction();
  gp_Vec Vy = Pl.YAxis().Direction();
  Vx.Multiply(V.X());
  Vy.Multiply(V.Y());
  Vx.Add(Vy);
  return Vx;
}

static gp_Ax2 to3d(const gp_Pln& Pl, const gp_Ax22d& A)
{
  gp_Pnt P = to3d(Pl,A.Location());
  gp_Vec VX = to3d(Pl,A.XAxis().Direction());
  gp_Vec VY = to3d(Pl,A.YAxis().Direction());
  return gp_Ax2(P,VX.Crossed(VY),VX);
}

static gp_Circ to3d(const gp_Pln& Pl, const gp_Circ2d& C)
{
  return gp_Circ(to3d(Pl,C.Axis()),C.Radius());
}

static gp_Elips to3d(const gp_Pln& Pl, const gp_Elips2d& E)
{
  return gp_Elips(to3d(Pl,E.Axis()),E.MajorRadius(),E.MinorRadius());
}

static gp_Hypr to3d(const gp_Pln& Pl, const gp_Hypr2d& H)
{
  return gp_Hypr(to3d(Pl,H.Axis()),H.MajorRadius(),H.MinorRadius());
}

static gp_Parab to3d(const gp_Pln& Pl, const gp_Parab2d& P)
{
  return gp_Parab(to3d(Pl,P.Axis()),P.Focal());
}

static gp_Vec SetLinearForm(const gp_Vec2d DW, const gp_Vec2d D2W,const gp_Vec2d D3W,
			    const gp_Vec D1U,  const gp_Vec D1V,  const gp_Vec D2U,
			    const gp_Vec D2V,  const gp_Vec D2UV, const gp_Vec D3U,
			    const gp_Vec D3V,  const gp_Vec D3UUV,const gp_Vec D3UVV)
{gp_Vec V31, V32, V33, V34,V3 ;
 V31.SetLinearForm(DW.X(),D1U,
		   D2W.X()*DW.X(),D2U,
		   D2W.X()*DW.Y(),D2UV);
 V31.SetLinearForm(D3W.Y(),D1V,
		   D2W.Y()*DW.X(),D2UV,
		   D2W.Y()*DW.Y(),D2V,
		   V31);
 V32.SetLinearForm(DW.X()*DW.X()*DW.Y(),D3UUV,
		   DW.X()*DW.Y()*DW.Y(),D3UVV);
 V32.SetLinearForm(D2W.X()*DW.Y()+DW.X()*D2W.Y(),D2UV,
		   DW.X()*DW.Y()*DW.Y(),D3UVV,
		   V32);
 V33.SetLinearForm(2*D2W.X()*DW.X(),D2U,
		   DW.X()*DW.X()*DW.X(),D3U,
		   DW.X()*DW.X()*DW.Y(),D3UUV);
 
 V34.SetLinearForm(2*D2W.Y()*DW.Y(),D2V,
		   DW.Y()*DW.Y()*DW.X(),D3UVV,
		   DW.Y()*DW.Y()*DW.Y(),D3V);
 V3.SetLinearForm(1,V31,2,V32,1,V33,V34);
 return V3;
}

//=======================================================================
static void CompareBounds(gp_Pnt2d& P1,
			  gp_Pnt2d& P2)//SVV
{
   Standard_Real Lx = P1.X(),Ly = P1.Y();
   Standard_Real Rx = P2.X(),Ry = P2.Y();
   
   if (Lx > Rx) { P1.SetX(Rx); P2.SetX(Lx);}
   if (Ly > Ry) { P1.SetY(Ry); P2.SetY(Ly);}
}

//=======================================================================
//function :Hunt
//purpose  : 
//=======================================================================
static void Hunt(const TColStd_Array1OfReal& Arr,
		 const Standard_Real Coord,
		 Standard_Integer& Iloc)
{//Warning: Hunt is used to find number of knot which equals co-ordinate component,
  //        when co-ordinate component definitly equals a knot only.
  Standard_Real Tol=Precision::PConfusion()/10;
  Standard_Integer i=1; 
  while((i <= Arr.Upper()) && (Abs(Coord - Arr(i)) > Tol)){
    i++;}
 
  if(Abs(Coord - Arr(i)) < Tol)
    Iloc = i;
  else
    if(Abs(Coord - Arr(i)) > Tol) 
      throw Standard_NotImplemented("Adaptor3d_CurveOnSurface:Hunt");
}

//=======================================================================
//function :ReverseParam
//purpose  : 
//=======================================================================

static void ReverseParam(const Standard_Real In1,
			 const Standard_Real In2,
			 Standard_Real& Out1,
			 Standard_Real& Out2 )
{

  if(In1>In2)  {Out1=In2;
		Out2=In1;}
  else         {Out1=In1;
		Out2=In2;}
}
//=======================================================================
//function :ReverseParam
//purpose  : 
//=======================================================================

static void ReverseParam(const Standard_Integer In1,
			 const Standard_Integer In2,
			 Standard_Integer& Out1,
			 Standard_Integer& Out2 )
{
  if(In1>In2)  {Out1=In2;
		Out2=In1;}
  else         {Out1=In1;
		Out2=In2;}
}

//=======================================================================
//function :FindBounds
//purpose  : 
//=======================================================================
static void FindBounds(const TColStd_Array1OfReal& Arr,
		       const Standard_Real Coord,
		       const Standard_Real Der,
		       Standard_Integer& Bound1,
		       Standard_Integer& Bound2,
		       Standard_Boolean& DerNull)

{
  Standard_Integer N=0;
  Standard_Real Tol=Precision::PConfusion()/10;
  Hunt(Arr,Coord,N);
  DerNull=Standard_False;
  
  if(N==Bound1){ if(Abs(Der) > Tol) DerNull = Standard_False;
		      if(Abs(Der)<= Tol) DerNull = Standard_True;
		      Bound1=N;Bound2=N+1; return;
		    }
  if(N==Bound2){ if( Abs(Der) > Tol ) DerNull = Standard_False;
		      if( Abs(Der)<= Tol ) DerNull = Standard_True;
		      Bound1=N-1;Bound2=N; return;
		    }
  if((N!=Bound1)&&(N!=Bound2))  { 
    if(Abs(Der) > Tol ) { 
      if(Der>0) {Bound1=N;Bound2= N+1;}
      else
      if(Der<0){Bound1=N-1;Bound2=N;}
      DerNull = Standard_False;
    }
    if(Abs(Der) <=Tol ) {
      DerNull = Standard_True;
      Bound1=N-1;
      Bound2=N+1;
    }
  } 
}

//=======================================================================
//function :Locate1Coord
//purpose  : along BSpline curve
//=======================================================================

static void Locate1Coord(const Standard_Integer Index,
			 const gp_Pnt2d& UV, 
			 const gp_Vec2d& DUV,
			 const Handle(Geom_BSplineCurve)& BSplC,
			 gp_Pnt2d& LeftBot, gp_Pnt2d& RightTop)
{
  Standard_Real Comp1=0, DComp1=0, cur, f = 0.0, l = 0.0;
  Standard_Real Tol = Precision::PConfusion()/10;
  Standard_Integer i = 1, Bnd1, Bnd2;
  Standard_Boolean DIsNull= Standard_False; 
  TColStd_Array1OfReal Arr(1,BSplC->NbKnots());  BSplC->Knots(Arr);
  
  if(Index==1) {    Comp1=UV.X();  DComp1=DUV.X(); } 
  if(Index==2) {    Comp1=UV.Y();  DComp1=DUV.Y(); } 

  Standard_Integer Lo = BSplC->FirstUKnotIndex(), Up = BSplC->LastUKnotIndex();

  i = Lo;
  while ( ( Abs(BSplC->Knot(i)-Comp1)>Tol )&& (i!=Up ) ) i++;
  cur=BSplC->Knot(i);

  if( Abs(Comp1-cur)<=Tol) {
 
    Bnd1 = Lo; Bnd2 = Up;
    FindBounds(Arr,cur,DComp1,Bnd1,Bnd2,DIsNull); 
    ReverseParam(Bnd1,Bnd2,Bnd1,Bnd2);
    
    if(DIsNull==Standard_False){
      if(Index==1)  {LeftBot.SetX(BSplC->Knot(Bnd1));
		     RightTop.SetX(BSplC->Knot(Bnd2));}
      else 
	if(Index==2){ LeftBot.SetY(BSplC->Knot(Bnd1));
		      RightTop.SetY(BSplC->Knot(Bnd2)); }
    }
    else
      if(DIsNull==Standard_True){
	if( Abs( Comp1-(f=BSplC->Knot(Lo))) <= Tol)
	  {	  
	    if(Index==1)   { LeftBot.SetX(BSplC->Knot(Lo));
			     RightTop.SetX(BSplC->Knot(Lo+1));}
	    else if(Index==2)  { LeftBot.SetY(BSplC->Knot(Lo));
				 RightTop.SetY(BSplC->Knot(Lo+1));}
	   } else
	     if( Abs( Comp1-(l=BSplC->Knot(Up))) <= Tol)
	       {
		 if(Index==1)   {  LeftBot.SetX(BSplC->Knot(Up-1));
				   RightTop.SetX(BSplC->Knot(Up));}
		 else if(Index==2)  {LeftBot.SetY(BSplC->Knot(Up-1));
				     RightTop.SetY(BSplC->Knot(Up));}
	       }else
		 if(Index==1)   {  LeftBot.SetX(BSplC->Knot(Bnd1));
				   RightTop.SetX(BSplC->Knot(Bnd2));}
		 else if(Index==2)  {LeftBot.SetY(BSplC->Knot(Bnd1));
				     RightTop.SetY(BSplC->Knot(Bnd2));}
      }
  } 
  else//*********if Coord != Knot
    {
      i=Lo;
      while (i < Up) {
	//if((f=BSplC->Knot(i))<Comp1 && (l=BSplC->Knot(i+1))>Comp1) break;
        //skl 28.03.2002 for OCC233
	f=BSplC->Knot(i);
        l=BSplC->Knot(i+1);
	if(f<Comp1 && l>Comp1) break;
	i++; 
      }
      ReverseParam(f,l,f,l);
     
      if(i!=Up)   {
	if(Abs(DComp1)<Tol) 
	  { if(Index==1) {LeftBot.SetX(f); RightTop.SetX(l);}else
	      if(Index==2) {LeftBot.SetY(f); RightTop.SetY(l); }
	  }else
	    if(Abs(DComp1)>Tol) 
	      { 
		if(Index==1) {
		  if(DComp1>0) {LeftBot.SetX(Comp1); RightTop.SetX(l);}   else
		    if(DComp1<0) {LeftBot.SetX(f); RightTop.SetX(Comp1);}
		}
		else
		  if(Index==2) {
		    if(DComp1>0) {LeftBot.SetY(Comp1); RightTop.SetY(l);} else
		      if(DComp1<0) {LeftBot.SetY(f); RightTop.SetY(Comp1);};
		  }
	      }
      }else
	if(i==Up)   {
	  if(Index==1) {LeftBot.SetX(Comp1); RightTop.SetX(BSplC->Knot(i));}else
	    if(Index==2) {LeftBot.SetY(Comp1); RightTop.SetY(BSplC->Knot(i)); }
	}
    }
}


//=======================================================================
//function :Locate1Coord
//purpose  : 
//=======================================================================

static void Locate1Coord(const Standard_Integer Index,
        const gp_Pnt2d& UV, 
        const gp_Vec2d& DUV,
        const Handle(Geom_BSplineSurface)& BSplS,
        Standard_Boolean& DIsNull,
        gp_Pnt2d& LeftBot, 
        gp_Pnt2d& RightTop)
{
  Standard_Real Comp1=0,DComp1=0; 
  Standard_Real Tol = Precision::PConfusion()/10;
  Standard_Integer i=1, Up=0, Up1, Up2, Down=0, Down1, Down2;
  Standard_Real cur = 0.;

  DIsNull= Standard_False; 

  Up1 =  BSplS->LastUKnotIndex();
  Down1 = BSplS->FirstUKnotIndex();
  Up2 =  BSplS->LastVKnotIndex();
  Down2 = BSplS->FirstVKnotIndex();

  if(Index==1)
  { 
    i = Down1; 
    Comp1 =  UV.X(); 
    DComp1= DUV.X(); 
    Up=Up1; 
    Down=Down1;

    while ( ( Abs(BSplS->UKnot(i)-Comp1)>Tol )&&(i!=Up1 ) )
    {
      i++;
    }

    cur = BSplS->UKnot(i); 
  }
  else if(Index==2)
  {
    i = Down2;
    Comp1 = UV.Y();
    DComp1=DUV.Y();
    Up=Up2;
    Down=Down2;

    while ( ( Abs(BSplS->VKnot(i)-Comp1)>Tol )&&(i!=Up2 ) )
    {
      i++;
    }

    cur = BSplS->VKnot(i);
  }
  
  if( Abs(Comp1-cur)<=Tol ) 
  { 
    Standard_Integer Bnd1 = Down, Bnd2 = Up;
    if(Index==1)
    {
      TColStd_Array1OfReal Arr1(1,BSplS->NbUKnots());
      BSplS->UKnots(Arr1); //   Up1=Arr1.Upper(); Down1=Arr1.Lower();
      FindBounds(Arr1,cur,DUV.X(),Bnd1,Bnd2,DIsNull); 
    }
    else if(Index==2)
    {
      TColStd_Array1OfReal Arr2(1,BSplS->NbVKnots());
      BSplS->VKnots(Arr2); //   Up2=Arr2.Upper(); Down2=Arr2.Lower();
      FindBounds(Arr2,cur,DUV.Y(),Bnd1,Bnd2,DIsNull); 
    }
    
    ReverseParam(Bnd1,Bnd2,Bnd1,Bnd2);

    if(DIsNull==Standard_False)
    {
      if(Index==1)
      {
        LeftBot.SetX(BSplS->UKnot(Bnd1));
        RightTop.SetX(BSplS->UKnot(Bnd2));
      }
      else if(Index==2)
      { 
        LeftBot.SetY(BSplS->VKnot(Bnd1));
        RightTop.SetY(BSplS->VKnot(Bnd2));
      }
    }      
  }
  else//*********if Coord != Knot
  {
    if( (Index==1)&&(Comp1 < BSplS->UKnot(Down)) )
    { 
      LeftBot.SetX(BSplS->UKnot(Down)); 
      RightTop.SetX( BSplS->UKnot(Down + 1) );
      return;
    }
    else if( (Index==2)&&(Comp1 < BSplS->VKnot(Down)) )
    { 
      LeftBot.SetY(BSplS->VKnot(Down)); 
      RightTop.SetY( BSplS->VKnot(Down + 1) );
      return;
    }
    else if( (Index==1)&&(Comp1 > BSplS->UKnot(Up)) )
    { 
      RightTop.SetX(BSplS->UKnot(Up - 1)); 
      LeftBot.SetX( BSplS->UKnot(Up) );
      return;
    }
    else if( (Index==2)&&(Comp1 > BSplS->VKnot(Up)) )
    { 
      RightTop.SetY(BSplS->VKnot(Up - 1)); 
      LeftBot.SetY( BSplS->VKnot(Up) );
      return;
    } 
    else
    {
      Standard_Real f = 0., l = 1.;
      if (Index==1)
      {
        f=BSplS->UKnot(Down);
        l=BSplS->UKnot(Up);
      }
      else if (Index == 2)
      {
        f=BSplS->VKnot(Down);
        l=BSplS->VKnot(Up);
      }

      i = Down;
      if ((!(Comp1 < f))&&(!(Comp1 > l)))
      {
        if (Index==1)
        {
          while (!(((f=BSplS->UKnot(i)) < Comp1)&&((l=BSplS->UKnot(i+1)) > Comp1)) && (i<Up)) 
          {
            i++;
          }
        }
        else if (Index==2)
        {
          while (!(((f=BSplS->VKnot(i)) < Comp1)&&((l=BSplS->VKnot(i+1)) > Comp1)) && (i<Up)) 
          {
            i++;
          }
        }
      }
      else
        ReverseParam(f,l,f,l);

      if(i!=Up)
      {
        if(Abs(DComp1)>Tol)
        {
          if(Index==1) 
          {  
            if(DComp1>0)
            { 
              LeftBot.SetX(Comp1); 
              RightTop.SetX(l);
            }
            else if(DComp1<0)
            {
              LeftBot.SetX(f); 
              RightTop.SetX(Comp1);
            }
          }
          else if(Index==2)
          {
            if(DComp1>0)
            { 
              LeftBot.SetY(Comp1); 
              RightTop.SetY(l);
            }
            else if(DComp1<0)
            { 
              LeftBot.SetY(f); 
              RightTop.SetY(Comp1);
            }
          }
        }
        else
        {
          if(Abs(DComp1)<Tol)
          {
            if(Index==1)
            {
              LeftBot.SetX(f); 
              RightTop.SetX(l);
            }
            else if(Index==2) 
            { 
              LeftBot.SetY(f); 
              RightTop.SetY(l);
            }
          }
        }
      } 
      else if(i==Up)
      {
        if(Index==1)
        {
          LeftBot.SetX(Comp1);
          RightTop.SetX(BSplS->UKnot(i));
        }
        else if(Index==2)
        { 
          LeftBot.SetY(Comp1); 
          RightTop.SetY(BSplS->VKnot(i));
        }
      }
    }
  }
}
//=======================================================================
//function :Locate2Coord
//purpose  : along non-BSpline curve
//=======================================================================


static void Locate2Coord(const Standard_Integer Index,
			 const gp_Pnt2d& UV, const gp_Vec2d& DUV, 
			 const Standard_Real I1, 
			 const Standard_Real I2,
			 gp_Pnt2d& LeftBot, gp_Pnt2d& RightTop)
{
  Standard_Real Tol=Precision::PConfusion()/10;
  Standard_Real Comp1=0,DComp1=0;
  if(Index==1)   {   Comp1=UV.X();
		     DComp1=DUV.X();} 
  else
    if(Index==2)   {Comp1=UV.Y();
		    DComp1=DUV.Y();}
  
  if((Comp1!=I1)&&(Comp1!=I2))
    { if(Abs(DComp1) > Tol)
	{  if(DComp1 < 0) 
	     {  if(Index==1) {  LeftBot.SetX(I1);
				RightTop.SetX(Comp1);}
		if(Index==2) {  LeftBot.SetY(I1);
				RightTop.SetY(Comp1);}
	      }
	else
	  if(DComp1 > 0) 
	    {   if(Index==1) {  LeftBot.SetX(Comp1);
				RightTop.SetX(I2);}
		if(Index==2) {  LeftBot.SetY(Comp1);
				RightTop.SetY(I2);}
	      }
	  else { if(Index==1) { LeftBot.SetX(I1); 
				RightTop.SetX(I2);}
		 if(Index==2) { LeftBot.SetY(I1);
				RightTop.SetY(I2);}
	       }
	 }
    else 
      if(Abs(DComp1)<=Tol) {
			      if(Index==1) { LeftBot.SetX(I1) ;
					     RightTop.SetX(I2);}
			      if(Index==2) { LeftBot.SetY(I1) ;
					     RightTop.SetY(I2);}
			    }
    }else 
      if(Abs(Comp1-I1)<Tol)
	{ if(Index==1) { LeftBot.SetX(I1) ;
			 RightTop.SetX(I2);}
	  if(Index==2) { LeftBot.SetY(I1) ;
			 RightTop.SetY(I2);}
	}
      else
	if(Abs(Comp1-I2)<Tol)
	  {	   if(Index==1) { LeftBot.SetX(I1); 
				  RightTop.SetX(I2);}
		   if(Index==2) { LeftBot.SetY(I1);
				  RightTop.SetY(I2);}
		 } 
}

//=======================================================================
//function :Locate2Coord
//purpose  : 
//=======================================================================

static void Locate2Coord(const Standard_Integer Index,
			 const gp_Pnt2d& UV, const gp_Vec2d& DUV, 
			 const Handle(Geom_BSplineSurface)& BSplS,
			 const TColStd_Array1OfReal& Arr,
			 gp_Pnt2d& LeftBot, gp_Pnt2d& RightTop)
{
  Standard_Real Comp=0,DComp=0,Tmp1=0.0,Tmp2=0.0;
  Standard_Real Tol=Precision::PConfusion()/10;
  Standard_Integer N=0, NUp=0, NLo=0;
  if(Index==1)
    { Comp=UV.X();
      DComp=DUV.Y();
      NUp = BSplS->LastUKnotIndex();
      NLo = BSplS->FirstUKnotIndex();
    }
  if(Index==2)
     { Comp=UV.Y();
       DComp=DUV.X(); 
       NUp = BSplS->LastVKnotIndex();
       NLo = BSplS->FirstVKnotIndex();
     }
  
  if((DComp > 0)&&(Abs(DComp)>Tol)) {  
    Hunt(Arr,Comp,N);
    if (N >= NUp){
      //limit case: Hunt() cought upper knot. Take the last span. 
      N = NUp - 1;
    }
    if(Index==1) {  Tmp1=BSplS->UKnot(N);
		    Tmp2=BSplS->UKnot(N+1);}
    else
      if(Index==2) {	  Tmp1=BSplS->VKnot(N);
			  Tmp2=BSplS->VKnot(N+1);}
    
    ReverseParam(Tmp1,Tmp2,Tmp1,Tmp2);
    
    if(Index==1) {	LeftBot.SetX(Tmp1);
			RightTop.SetX(Tmp2);}
    else
      if(Index==2) {	  LeftBot.SetY(Tmp1);
			  RightTop.SetY(Tmp2);}
    }
  else
    if((DComp < 0)&&(Abs(DComp)>Tol)){      
     Hunt(Arr,Comp,N);
     if (N <= NLo) {
       //limit case: Hunt() cought lower knot. Take the first span.
       N = NLo + 1;
     }
     if(Index==1) {	  Tmp1=BSplS->UKnot(N-1);
			  Tmp2=BSplS->UKnot(N);}
	else
	  if(Index==2) { Tmp1=BSplS->VKnot(N-1);
			 Tmp2=BSplS->VKnot(N);}
     
	ReverseParam(Tmp1,Tmp2,Tmp1,Tmp2);
     
	if(Index==1) {	  LeftBot.SetX(Tmp1);
			  RightTop.SetX(Tmp2);}
	else
	  if(Index==2) {	    LeftBot.SetY(Tmp1);
				    RightTop.SetY(Tmp2);}
   } 
}



//=======================================================================
//function : Adaptor3d_CurveOnSurface
//purpose  : 
//=======================================================================

Adaptor3d_CurveOnSurface::Adaptor3d_CurveOnSurface()
     : myType(GeomAbs_OtherCurve), myIntCont(GeomAbs_CN)
{}

//=======================================================================
//function : Adaptor3d_CurveOnSurface
//purpose  : 
//=======================================================================

Adaptor3d_CurveOnSurface::Adaptor3d_CurveOnSurface
(const Handle(Adaptor3d_HSurface)& S) 
     : myType(GeomAbs_OtherCurve), myIntCont(GeomAbs_CN)
{
  Load(S);
}

//=======================================================================
//function : Adaptor3d_CurveOnSurface
//purpose  : 
//=======================================================================

Adaptor3d_CurveOnSurface::Adaptor3d_CurveOnSurface
(const Handle(Adaptor2d_HCurve2d)& C,
 const Handle(Adaptor3d_HSurface)& S)
     : myType(GeomAbs_OtherCurve), myIntCont(GeomAbs_CN)
{
  Load(S);
  Load(C);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::Load(const Handle(Adaptor3d_HSurface)& S) 
{
  mySurface = S;
  if (!myCurve.IsNull()) EvalKPart();
}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================

void Adaptor3d_CurveOnSurface::Load(const Handle(Adaptor2d_HCurve2d)& C)
{
  myCurve = C;
  if (mySurface.IsNull())
  {
    return;
  }

  EvalKPart();

  GeomAbs_SurfaceType SType = mySurface->GetType();
  if (SType == GeomAbs_OffsetSurface)
  {
    SType = mySurface->BasisSurface()->GetType();
  }

  if (SType == GeomAbs_BSplineSurface ||  
      SType == GeomAbs_SurfaceOfExtrusion ||
      SType == GeomAbs_SurfaceOfRevolution)
  {
    EvalFirstLastSurf();
  }
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::Load (const Handle(Adaptor2d_HCurve2d)& C,
                                     const Handle(Adaptor3d_HSurface)& S) 
{
  Load (C);
  Load (S);
}

//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_CurveOnSurface::FirstParameter() const 
{
  return myCurve->FirstParameter();
}

//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_CurveOnSurface::LastParameter() const 
{
  return myCurve->LastParameter();
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Adaptor3d_CurveOnSurface::Continuity() const
{
  GeomAbs_Shape ContC  = myCurve->Continuity();
  GeomAbs_Shape ContSu = mySurface->UContinuity();
  if ( ContSu < ContC) ContC = ContSu;
  GeomAbs_Shape ContSv = mySurface->VContinuity();
  if ( ContSv < ContC) ContC = ContSv;

  return ContC;
}

// Auxiliary: adds roots of equation to sorted sequence of parameters
// along curve, keeping it sorted and avoiding repetitions (within tolerance Tol)
static void AddIntervals (const Handle(TColStd_HSequenceOfReal)& theParameters, 
                          const math_FunctionRoots& theRoots, Standard_Real theTol)
{
  if (! theRoots.IsDone() || theRoots.IsAllNull())
    return;

  Standard_Integer nsol = theRoots.NbSolutions();
  for (Standard_Integer i = 1; i <= nsol; i++)
  {
    Standard_Real param = theRoots.Value(i);
    if (param - theParameters->Value(1) < theTol) // skip param if equal to or less than theParameters(1)
      continue;
    for (Standard_Integer j=2; j <= theParameters->Length(); ++j)
    {
      Standard_Real aDelta = theParameters->Value(j) - param;
      if (aDelta > theTol)
      {
        theParameters->InsertBefore (j, param);
        break;
      }
      else if (aDelta >= -theTol) // param == theParameters(j) within Tol
        break;
    }
  }
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_CurveOnSurface::NbIntervals (const GeomAbs_Shape S) const
{
  if(S == myIntCont && !myIntervals.IsNull())
    return myIntervals->Length()-1;
  
  Standard_Integer nu,nv,nc;
  nu=mySurface->NbUIntervals(S);
  nv=mySurface->NbVIntervals(S);
  nc=myCurve->NbIntervals(S);

  // Allocate the memory for arrays TabU, TabV, TabC only once using the buffer TabBuf.
  TColStd_Array1OfReal TabBuf(1, nu + nv + nc + 3);
  TColStd_Array1OfReal TabU(TabBuf(1), 1, nu+1);
  TColStd_Array1OfReal TabV(TabBuf(nu + 2), 1, nv+1);
  TColStd_Array1OfReal TabC(TabBuf(nu + nv + 3), 1, nc+1);

  Standard_Integer NbSample = 20;
  Standard_Real U,V,Tdeb,Tfin;
  Tdeb=myCurve->FirstParameter();
  Tfin=myCurve->LastParameter();

  myCurve->Intervals(TabC,S);

  Standard_Real Tol= Precision::PConfusion()/10;

  // sorted sequence of parameters defining continuity intervals;
  // started with own intervals of curve and completed by 
  // additional points coming from surface discontinuities
  Handle(TColStd_HSequenceOfReal) aIntervals = new TColStd_HSequenceOfReal;
  for (Standard_Integer i = 1; i <= nc + 1; i++)
  {
    aIntervals->Append(TabC(i));
  }
 
  if (nu>1)
  {
    mySurface->UIntervals(TabU,S);
    for(Standard_Integer iu = 2;iu <= nu; iu++)
    {
      U = TabU.Value(iu);
      Adaptor3d_InterFunc Func(myCurve,U,1);
      math_FunctionRoots Resol(Func,Tdeb,Tfin,NbSample,Tol,Tol,Tol,0.);
      AddIntervals (aIntervals, Resol, Tol);
    }
  }
  if (nv>1)
  {
    mySurface->VIntervals(TabV,S);
    for(Standard_Integer iv = 2;iv <= nv; iv++)
    {
      V = TabV.Value(iv);
      Adaptor3d_InterFunc Func(myCurve,V,2);
      math_FunctionRoots Resol(Func,Tdeb,Tfin,NbSample,Tol,Tol,Tol,0.);
      AddIntervals (aIntervals, Resol, Tol);
    }
  }

  // for case intervals==1 and first point == last point SequenceOfReal
  // contains only one value, therefore it is necessary to add second
  // value into aIntervals which will be equal first value.
  if (aIntervals->Length() == 1)
    aIntervals->Append (aIntervals->Value(1));

  const_cast<Adaptor3d_CurveOnSurface*>(this)->myIntervals = aIntervals;
  const_cast<Adaptor3d_CurveOnSurface*>(this)->myIntCont = S;
  return myIntervals->Length() - 1;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::Intervals(TColStd_Array1OfReal& T,
                                         const GeomAbs_Shape S) const
{
  NbIntervals(S);
  Standard_ASSERT_RAISE (T.Length() == myIntervals->Length(), "Error: Wrong size of array buffer in call to Adaptor3d_CurveOnSurface::Intervals");
  for(Standard_Integer i=1; i<=myIntervals->Length(); i++) {
    T(i) = myIntervals->Value(i);
  }
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) Adaptor3d_CurveOnSurface::Trim
(const Standard_Real First, 
 const Standard_Real Last, 
 const Standard_Real Tol) const 
{
  Handle(Adaptor3d_HCurveOnSurface) HCS = new Adaptor3d_HCurveOnSurface();
  HCS->ChangeCurve().Load(mySurface);
  HCS->ChangeCurve().Load(myCurve->Trim(First,Last,Tol));
  return HCS;
}

//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_CurveOnSurface::IsClosed() const
{
  return myCurve->IsClosed();
}

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_CurveOnSurface::IsPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_CurveOnSurface::Period() const
{
  return myCurve->Period();
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt Adaptor3d_CurveOnSurface::Value(const Standard_Real U ) const
{
  gp_Pnt P;
  gp_Pnt2d Puv;
  
  if      (myType == GeomAbs_Line  ) P = ElCLib::Value(U,myLin );
  else if (myType == GeomAbs_Circle) P = ElCLib::Value(U,myCirc);
  else {
    myCurve->D0(U,Puv);
    mySurface->D0(Puv.X(),Puv.Y(),P);
  }

  return P;
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::D0(const Standard_Real U ,
				  gp_Pnt& P) const 
{
  gp_Pnt2d Puv;
  
  if      (myType == GeomAbs_Line  ) P = ElCLib::Value(U,myLin );
  else if (myType == GeomAbs_Circle) P = ElCLib::Value(U,myCirc);
  else {
    myCurve->D0(U,Puv);
    mySurface->D0(Puv.X(),Puv.Y(),P);
  }

}


//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::D1(const Standard_Real U ,
				gp_Pnt& P,
				gp_Vec& V) const 
{
  gp_Pnt2d Puv;
  gp_Vec2d Duv;
  gp_Vec D1U,D1V;
  
  Standard_Real FP = myCurve->FirstParameter();
  Standard_Real LP = myCurve->LastParameter();
  
  Standard_Real Tol= Precision::PConfusion()/10; 
  if( ( Abs(U-FP)<Tol)&&(!myFirstSurf.IsNull()) )
    {
      myCurve->D1(U,Puv,Duv);
      myFirstSurf->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
      V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
    }
  else
    if( (Abs(U-LP)<Tol)&&(!myLastSurf.IsNull()) )
      {
	myCurve->D1(U,Puv,Duv);
	myLastSurf->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
	V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
      }
    else
      if      (myType == GeomAbs_Line  ) ElCLib::D1(U,myLin ,P,V);
      else if (myType == GeomAbs_Circle) ElCLib::D1(U,myCirc,P,V);
      else {
	myCurve->D1(U,Puv,Duv);
	mySurface->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
	V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V);
      }
}
//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::D2(const Standard_Real U,
				gp_Pnt& P,
				gp_Vec& V1,
				gp_Vec& V2) const
{ 
  gp_Pnt2d UV;
  gp_Vec2d DW,D2W; 
  gp_Vec D1U,D1V,D2U,D2V,D2UV;
  
  Standard_Real FP = myCurve->FirstParameter();
  Standard_Real LP = myCurve->LastParameter();
  
  Standard_Real Tol= Precision::PConfusion()/10; 
  if( (Abs(U-FP)<Tol)&&(!myFirstSurf.IsNull()) )
    {
      myCurve->D2(U,UV,DW,D2W);
      myFirstSurf->D2(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV);
      
      V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
      V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
      V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2);  
    }
  else  
    if( (Abs(U-LP)<Tol)&&(!myLastSurf.IsNull()) )
      {
	myCurve->D2(U,UV,DW,D2W);
	myLastSurf->D2(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV);
	
	V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
	V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
	V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2);  
      }
    else
      if (myType == GeomAbs_Line  ) {
	ElCLib::D1(U,myLin,P,V1);
	  V2.SetCoord(0.,0.,0.);
	}
      else if (myType == GeomAbs_Circle) ElCLib::D2(U,myCirc,P,V1,V2);
      else {
	myCurve->D2(U,UV,DW,D2W);
	mySurface->D2(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV);
	
	V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
	V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
	V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2);  
      }
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::D3
  (const Standard_Real U,
   gp_Pnt& P,
   gp_Vec& V1,
   gp_Vec& V2,
   gp_Vec& V3) const
{ 
  
  Standard_Real Tol= Precision::PConfusion()/10; 
  gp_Pnt2d UV;
  gp_Vec2d DW,D2W,D3W;
  gp_Vec D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV;
  
  Standard_Real FP = myCurve->FirstParameter();
  Standard_Real LP = myCurve->LastParameter();
  
  if( (Abs(U-FP)<Tol)&&(!myFirstSurf.IsNull()) )
    { myCurve->D3(U,UV,DW,D2W,D3W);
      myFirstSurf->D3(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
      V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
      V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
      V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2); 
      V3=SetLinearForm( DW, D2W, D3W, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
    }else
      
      if( (Abs(U-LP)<Tol)&&(!myLastSurf.IsNull()) )
	{ myCurve->D3(U,UV,DW,D2W,D3W);
	  myLastSurf->D3(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
	  V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
	  
	  V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
	  V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2);  
	  V3=SetLinearForm( DW, D2W, D3W, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);  
	}
      else
	if (myType == GeomAbs_Line  ) {
	  ElCLib::D1(U,myLin,P,V1);
	    V2.SetCoord(0.,0.,0.);
	    V3.SetCoord(0.,0.,0.);
	  }
	else if (myType == GeomAbs_Circle) ElCLib::D3(U,myCirc,P,V1,V2,V3);
	else {
	  myCurve->D3(U,UV,DW,D2W,D3W);
	  mySurface->D3(UV.X(),UV.Y(),P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
	  V1.SetLinearForm(DW.X(),D1U,DW.Y(),D1V);
	  
	  V2.SetLinearForm(D2W.X(), D1U, D2W.Y(), D1V, 2.*DW.X()*DW.Y(),D2UV);
	  V2.SetLinearForm(DW.X()*DW.X(), D2U, DW.Y()*DW.Y(), D2V, V2);  
	  V3=SetLinearForm( DW, D2W, D3W, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);  
	}
}


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec Adaptor3d_CurveOnSurface::DN
  (const Standard_Real U, 
   const Standard_Integer N) const 
{
  gp_Pnt P;
  gp_Vec V1, V2, V;
  switch (N) {
  case 1:
    D1(U,P,V);
    break ;
  case 2: 
    D2(U,P,V1,V);
    break ;
  case 3: 
    D3(U,P,V1,V2,V);
    break ;
  default:
    throw Standard_NotImplemented("Adaptor3d_CurveOnSurface:DN");
    break;
  }
  return V;
}


//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

Standard_Real Adaptor3d_CurveOnSurface::Resolution
  (const Standard_Real R3d) const
{
  Standard_Real ru,rv;
  ru = mySurface->UResolution(R3d);
  rv = mySurface->VResolution(R3d);
  return myCurve->Resolution(Min(ru,rv)); 
}


//=======================================================================
//function : GetType
//purpose  : 
//=======================================================================

GeomAbs_CurveType Adaptor3d_CurveOnSurface::GetType() const 
{
  return myType;
} 


//=======================================================================
//function : Line
//purpose  : 
//=======================================================================

gp_Lin Adaptor3d_CurveOnSurface::Line() const
{
  Standard_NoSuchObject_Raise_if(myType != GeomAbs_Line, "Adaptor3d_CurveOnSurface::Line(): curve is not a line")
  return myLin;
}

//=======================================================================
//function : Circle
//purpose  : 
//=======================================================================

gp_Circ Adaptor3d_CurveOnSurface::Circle() const
{
  Standard_NoSuchObject_Raise_if(myType != GeomAbs_Circle, "Adaptor3d_CurveOnSurface::Line(): curve is not a circle")
  return myCirc;
}

//=======================================================================
//function : Ellipse
//purpose  : 
//=======================================================================

gp_Elips Adaptor3d_CurveOnSurface::Ellipse() const
{
  return to3d(mySurface->Plane(),myCurve->Ellipse());
}

//=======================================================================
//function : Hyperbola
//purpose  : 
//=======================================================================

gp_Hypr Adaptor3d_CurveOnSurface::Hyperbola() const
{
  return to3d(mySurface->Plane(),myCurve->Hyperbola());
}

//=======================================================================
//function : Parabola
//purpose  : 
//=======================================================================

gp_Parab Adaptor3d_CurveOnSurface::Parabola() const
{
  return to3d(mySurface->Plane(),myCurve->Parabola());
}

Standard_Integer  Adaptor3d_CurveOnSurface::Degree() const
{
  
  // on a parametric surface should multiply
  // return TheCurve2dTool::Degree(myCurve);

  return myCurve->Degree();
}

//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_CurveOnSurface::IsRational() const
{
  return ( myCurve->IsRational() ||
	  mySurface->IsURational() ||
	  mySurface->IsVRational()   );
}

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer  Adaptor3d_CurveOnSurface::NbPoles() const
{
  // on a parametric surface should multiply
  return myCurve->NbPoles();
}

//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Adaptor3d_CurveOnSurface::NbKnots() const {
  if (mySurface->GetType()==GeomAbs_Plane)
    return myCurve->NbKnots();
  else {
    throw Standard_NoSuchObject();
  }
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierCurve) Adaptor3d_CurveOnSurface::Bezier() const  
{
  Standard_NoSuchObject_Raise_if
    ( mySurface->GetType() != GeomAbs_Plane,
     "Adaptor3d_CurveOnSurface : Bezier");

  Handle(Geom2d_BezierCurve) Bez2d = myCurve->Bezier();
  Standard_Integer NbPoles = Bez2d->NbPoles();

  const gp_Pln& Plane = mySurface->Plane();

  TColgp_Array1OfPnt Poles(1,NbPoles);
  for ( Standard_Integer i=1; i<= NbPoles; i++) {
    Poles(i) = to3d( Plane, Bez2d->Pole(i));
  }
  Handle(Geom_BezierCurve) Bez;

  if (Bez2d->IsRational()) {
    TColStd_Array1OfReal Weights(1,NbPoles);
    Bez2d->Weights(Weights);
    Bez = new Geom_BezierCurve(Poles,Weights);
  }
  else {
    Bez = new Geom_BezierCurve(Poles);
  }
  return Bez;
}

//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) Adaptor3d_CurveOnSurface::BSpline() const  
{
  Standard_NoSuchObject_Raise_if
    ( mySurface->GetType() != GeomAbs_Plane,
     "Adaptor3d_CurveOnSurface : BSpline");

  Handle(Geom2d_BSplineCurve) Bsp2d = myCurve->BSpline();
  Standard_Integer NbPoles = Bsp2d->NbPoles();

  const gp_Pln& Plane = mySurface->Plane();

  TColgp_Array1OfPnt Poles(1,NbPoles);
  for ( Standard_Integer i=1; i<= NbPoles; i++) {
    Poles(i) = to3d( Plane, Bsp2d->Pole(i));
  }

  TColStd_Array1OfReal    Knots(1,Bsp2d->NbKnots());
  TColStd_Array1OfInteger Mults(1,Bsp2d->NbKnots());
  Bsp2d->Knots(Knots);
  Bsp2d->Multiplicities(Mults);

  Handle(Geom_BSplineCurve) Bsp;

  if (Bsp2d->IsRational()) {
    TColStd_Array1OfReal Weights(1,NbPoles);
    Bsp2d->Weights(Weights);
    Bsp = new Geom_BSplineCurve(Poles,Weights,Knots,Mults,
				Bsp2d->Degree(),
				Bsp2d->IsPeriodic());
  }
  else {
    Bsp = new Geom_BSplineCurve(Poles,Knots,Mults,
				Bsp2d->Degree(),
				Bsp2d->IsPeriodic());
  }
  return Bsp;
}

//=======================================================================
//function : GetCurve
//purpose  : 
//=======================================================================

const Handle(Adaptor2d_HCurve2d)& Adaptor3d_CurveOnSurface::GetCurve() const
{
  return myCurve;
}

//=======================================================================
//function : GetSurface
//purpose  : 
//=======================================================================

const Handle(Adaptor3d_HSurface)& Adaptor3d_CurveOnSurface::GetSurface() const 
{
  return mySurface;
}

//=======================================================================
//function : ChangeCurve
//purpose  : 
//=======================================================================

Handle(Adaptor2d_HCurve2d)& Adaptor3d_CurveOnSurface::ChangeCurve() 
{
  return myCurve;
}

//=======================================================================
//function : ChangeSurface
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface)& Adaptor3d_CurveOnSurface::ChangeSurface() {
  return mySurface;
}

//=======================================================================
//function : EvalKPart
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::EvalKPart()
{
  myType = GeomAbs_OtherCurve;

  GeomAbs_SurfaceType STy = mySurface->GetType();
  GeomAbs_CurveType   CTy = myCurve->GetType();
  if (STy == GeomAbs_Plane) {
    myType =  CTy;
    if (myType == GeomAbs_Circle) 
      myCirc = to3d(mySurface->Plane(),myCurve->Circle());
    else if (myType == GeomAbs_Line) {
      gp_Pnt P;
      gp_Vec V;
      gp_Pnt2d Puv;
      gp_Vec2d Duv;
      myCurve->D1(0.,Puv,Duv);
      gp_Vec D1U,D1V;
      mySurface->D1(Puv.X(),Puv.Y(),P,D1U,D1V);
      V.SetLinearForm(Duv.X(),D1U,Duv.Y(),D1V); 
      myLin = gp_Lin(P,V);
    }
  }
  else {
    if ( CTy == GeomAbs_Line) {
      gp_Dir2d D = myCurve->Line().Direction();
      if ( D.IsParallel(gp::DX2d(),Precision::Angular())) { // Iso V.
	if ( STy == GeomAbs_Sphere) {
	  gp_Pnt2d  P    = myCurve->Line().Location();
	  if ( Abs( Abs(P.Y()) -M_PI/2. ) >= Precision::PConfusion()) {
	    myType = GeomAbs_Circle;
	    gp_Sphere Sph  =  mySurface->Sphere();
	    gp_Ax3    Axis = Sph.Position();
	    myCirc   = ElSLib::SphereVIso(Axis,
					  Sph.Radius(),
					  P.Y());
	    gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	    gp_Ax1 AxeRev(Axis.Location(), DRev);
	    myCirc.Rotate(AxeRev, P.X());
	    if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) {
	      gp_Ax2 Ax = myCirc.Position();
	      Ax.SetDirection(Ax.Direction().Reversed());
	      myCirc.SetPosition(Ax);
	    }
	  }
	}
	else if ( STy == GeomAbs_Cylinder) {
	  myType = GeomAbs_Circle;
	  gp_Cylinder Cyl  = mySurface->Cylinder();
	  gp_Pnt2d    P    = myCurve->Line().Location();
	  gp_Ax3      Axis = Cyl.Position();
	  myCirc           = ElSLib::CylinderVIso(Axis,
						  Cyl.Radius(),
						  P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  myCirc.Rotate(AxeRev, P.X());
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) {
	    gp_Ax2 Ax = myCirc.Position();
	    Ax.SetDirection(Ax.Direction().Reversed());
	    myCirc.SetPosition(Ax);
	  }
	}
	else if ( STy == GeomAbs_Cone) {
	  myType = GeomAbs_Circle;
	  gp_Cone  Cone = mySurface->Cone();
	  gp_Pnt2d P    = myCurve->Line().Location();
	  gp_Ax3   Axis = Cone.Position();
	  myCirc        = ElSLib::ConeVIso(Axis,
					   Cone.RefRadius(),
					   Cone.SemiAngle(),
					   P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  myCirc.Rotate(AxeRev, P.X());
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) {
	    gp_Ax2 Ax = myCirc.Position();
	    Ax.SetDirection(Ax.Direction().Reversed());
	    myCirc.SetPosition(Ax);
	  }
	}
	else if ( STy == GeomAbs_Torus) {
	  myType = GeomAbs_Circle;
	  gp_Torus Tore = mySurface->Torus();
	  gp_Pnt2d P    = myCurve->Line().Location();
	  gp_Ax3   Axis = Tore.Position();
	  myCirc        = ElSLib::TorusVIso(Axis,
					    Tore.MajorRadius(),
					    Tore.MinorRadius(),
					    P.Y());
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  gp_Ax1 AxeRev(Axis.Location(), DRev);
	  myCirc.Rotate(AxeRev, P.X());
	  if ( D.IsOpposite(gp::DX2d(),Precision::Angular())) {
	    gp_Ax2 Ax = myCirc.Position();
	    Ax.SetDirection(Ax.Direction().Reversed());
	    myCirc.SetPosition(Ax);
	  }
	}
      }
      else if ( D.IsParallel(gp::DY2d(),Precision::Angular())) { // Iso U.
	if ( STy == GeomAbs_Sphere) {
	  myType = GeomAbs_Circle;
	  gp_Sphere Sph  = mySurface->Sphere();
	  gp_Pnt2d  P    = myCurve->Line().Location();
	  gp_Ax3    Axis = Sph.Position();
	  // calcul de l'iso 0.
	  myCirc         = ElSLib::SphereUIso(Axis, Sph.Radius(),0.);

	  // mise a sameparameter (rotation du cercle - decalage du Y)
	  gp_Dir DRev = Axis.XDirection().Crossed(Axis. Direction());
	  gp_Ax1 AxeRev(Axis.Location(),DRev);
	  myCirc.Rotate(AxeRev, P.Y());

	  // transformation en iso U ( = P.X())
	  DRev = Axis.XDirection().Crossed(Axis.YDirection());
	  AxeRev = gp_Ax1(Axis.Location(), DRev);
	  myCirc.Rotate(AxeRev, P.X());
	  
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular())) {
	    gp_Ax2 Ax = myCirc.Position();
	    Ax.SetDirection(Ax.Direction().Reversed());
	    myCirc.SetPosition(Ax);
	  }
	}
	else if ( STy == GeomAbs_Cylinder) {
	  myType = GeomAbs_Line;
	  gp_Cylinder Cyl = mySurface->Cylinder();
	  gp_Pnt2d    P   = myCurve->Line().Location();
	  myLin           = ElSLib::CylinderUIso(Cyl.Position(),
						 Cyl.Radius(),
						 P.X());
	  gp_Vec Tr(myLin.Direction());
	  Tr.Multiply(P.Y());
	  myLin.Translate(Tr);
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular()))
	    myLin.Reverse();
	}
	else if ( STy == GeomAbs_Cone) {
	  myType = GeomAbs_Line;
	  gp_Cone  Cone = mySurface->Cone();
	  gp_Pnt2d P    = myCurve->Line().Location();
	  myLin         = ElSLib::ConeUIso(Cone.Position(),
					   Cone.RefRadius(),
					   Cone.SemiAngle(),
					   P.X());
	  gp_Vec Tr(myLin.Direction());
	  Tr.Multiply(P.Y());
	  myLin.Translate(Tr);	  
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular()))
	    myLin.Reverse();
	}
	else if ( STy == GeomAbs_Torus) {
	  myType = GeomAbs_Circle;
	  gp_Torus Tore = mySurface->Torus();
	  gp_Pnt2d P    = myCurve->Line().Location();
	  gp_Ax3   Axis = Tore.Position();
	  myCirc        = ElSLib::TorusUIso(Axis,
					    Tore.MajorRadius(),
					    Tore.MinorRadius(),
					    P.X());
	  myCirc.Rotate(myCirc.Axis(),P.Y());
	  
	  if ( D.IsOpposite(gp::DY2d(),Precision::Angular())) {
	    gp_Ax2 Ax = myCirc.Position();
	    Ax.SetDirection(Ax.Direction().Reversed());
	    myCirc.SetPosition(Ax);
	  }
	}
      }
    }
  }
}
//=======================================================================
//function :EvalFirstLastSurf 
//purpose  : 
//=======================================================================

void Adaptor3d_CurveOnSurface::EvalFirstLastSurf()
{ 
  Standard_Real FirstPar,LastPar;
  gp_Pnt2d UV, LeftBot, RightTop;
  gp_Vec2d DUV;
  Standard_Real Tol= Precision::PConfusion()/10;
  Standard_Boolean Ok = Standard_True;
  
  
  FirstPar=myCurve->FirstParameter();
  myCurve->D1(FirstPar,UV,DUV);

  if(DUV.Magnitude() <= Tol) Ok = Standard_False;

  if(Ok) {

    switch(mySurface->GetType()) {
    case GeomAbs_BSplineSurface :
      LocatePart(UV,DUV,mySurface,LeftBot,RightTop);
      break;
    case GeomAbs_SurfaceOfRevolution :
    case  GeomAbs_SurfaceOfExtrusion :  
      Ok = LocatePart_RevExt(UV,DUV,mySurface,LeftBot,RightTop);
      break;
    case GeomAbs_OffsetSurface :
      Ok = LocatePart_Offset(UV,DUV,mySurface,LeftBot,RightTop);
      break;
      default :
	throw Standard_NotImplemented("Adaptor3d_CurveOnSurface::EvalFirstLastSurf");
      break;
    }
  }

  if (Ok) {

    CompareBounds(LeftBot,RightTop); //SVV
    
    myFirstSurf = mySurface->UTrim(LeftBot.X(),RightTop.X(),Tol);
    myFirstSurf = myFirstSurf->VTrim(LeftBot.Y(),RightTop.Y(),Tol);

  }
  else {
    myFirstSurf = mySurface;
  }
  
  LastPar=myCurve->LastParameter();
  Ok = Standard_True;
  myCurve->D1(LastPar,UV,DUV);
  DUV.Reverse(); //We want the other part

  if(DUV.Magnitude() <= Tol) Ok = Standard_False;

  if(Ok) {

    switch(mySurface->GetType()) {
    case GeomAbs_BSplineSurface :
      LocatePart(UV,DUV,mySurface,LeftBot,RightTop);
      break;
    case GeomAbs_SurfaceOfRevolution :
    case  GeomAbs_SurfaceOfExtrusion :  
      Ok = LocatePart_RevExt(UV,DUV,mySurface,LeftBot,RightTop);
      break;
    case GeomAbs_OffsetSurface :
      Ok = LocatePart_Offset(UV,DUV,mySurface,LeftBot,RightTop);
      break;
      default :
	throw Standard_NotImplemented("Adaptor3d_CurveOnSurface::EvalFirstLastSurf");
      break;
    }
  }

  if (Ok) {

    CompareBounds(LeftBot, RightTop); //SVV

    myLastSurf = mySurface->UTrim(LeftBot.X(),RightTop.X(),Tol);
    myLastSurf = myLastSurf->VTrim(LeftBot.Y(),RightTop.Y(),Tol);

  }
  else {
    myLastSurf = mySurface;
  }
}

//=======================================================================
//function :LocatePart_RevExt
//purpose  : processes Knots 
//=======================================================================

Standard_Boolean Adaptor3d_CurveOnSurface::LocatePart_RevExt(const gp_Pnt2d& UV, 
							   const gp_Vec2d& DUV,
							   const Handle(Adaptor3d_HSurface)& S,
							   gp_Pnt2d& LeftBot, 
							   gp_Pnt2d& RightTop) const
{ 
  Handle(Adaptor3d_HCurve) AHC = S->BasisCurve();

  if (AHC->GetType() == GeomAbs_BSplineCurve) {
    Handle( Geom_BSplineCurve) BSplC;
    BSplC = AHC->BSpline();
 
    if((S->GetType())==GeomAbs_SurfaceOfExtrusion) { 
      Locate1Coord(1,UV,DUV,BSplC,LeftBot,RightTop);
      Locate2Coord(2,UV,DUV,S->FirstVParameter(),S->LastVParameter(),LeftBot,RightTop); 
    }
    else if((S->GetType())==GeomAbs_SurfaceOfRevolution)  { 
      Locate1Coord(2,UV,DUV,BSplC,LeftBot,RightTop);
      Locate2Coord(1,UV,DUV,S->FirstUParameter(),S->LastUParameter(),LeftBot,RightTop);
    } 

    Standard_Real u1,u2,v1,v2;
    ReverseParam(LeftBot.X(),RightTop.X(),u1,u2);
    LeftBot.SetX(u1);
    RightTop.SetX(u2);
    ReverseParam(LeftBot.Y(),RightTop.Y(),v1,v2);
    LeftBot.SetY(v1);
    RightTop.SetY(v2);
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function :LocatePart_OffsetSurface
//purpose  : 
//=======================================================================

Standard_Boolean Adaptor3d_CurveOnSurface::
   LocatePart_Offset(const gp_Pnt2d& UV, const gp_Vec2d& DUV,
		     const Handle(Adaptor3d_HSurface)& S,
		     gp_Pnt2d& LeftBot, gp_Pnt2d& RightTop) const
{
  Standard_Boolean Ok = Standard_True;
  Handle( Adaptor3d_HSurface) AHS;
  Handle( Geom_BSplineSurface) BSplS;
  AHS = S->BasisSurface();
  GeomAbs_SurfaceType  BasisSType = AHS->GetType();
  switch(BasisSType) {
  case GeomAbs_SurfaceOfRevolution:
  case GeomAbs_SurfaceOfExtrusion :
    Ok = LocatePart_RevExt(UV,DUV,AHS,LeftBot,RightTop);
    break;
      
  case GeomAbs_BSplineSurface:
    LocatePart(UV,DUV,AHS,LeftBot,RightTop);
    break;

    default : 
      Ok=Standard_False;
  }
  return Ok;
}

//=======================================================================
//function :LocatePart
//purpose  : for BSplineSurface
//=======================================================================

void Adaptor3d_CurveOnSurface::LocatePart(const gp_Pnt2d& UV, const gp_Vec2d& DUV,
					const Handle(Adaptor3d_HSurface)& S,
					gp_Pnt2d& LeftBot, gp_Pnt2d& RightTop) const
{
  Handle( Geom_BSplineSurface) BSplS;
  BSplS = S->BSpline();
  Standard_Boolean  DUIsNull=Standard_False,
  DVIsNull=Standard_False;
      
  Locate1Coord(1,UV,DUV,BSplS,DUIsNull,LeftBot,RightTop);
  Locate1Coord(2,UV,DUV,BSplS,DVIsNull,LeftBot,RightTop); 
  
  if((DUIsNull==Standard_True)&&(DVIsNull==Standard_False)) {
    TColStd_Array1OfReal ArrU(1,BSplS->NbUKnots());
    BSplS->UKnots(ArrU);
    Locate2Coord(1,UV,DUV,BSplS,ArrU,LeftBot,RightTop); 
  }
  else if((DVIsNull==Standard_True)&&(DUIsNull==Standard_False)) {
    TColStd_Array1OfReal ArrV(1,BSplS->NbVKnots());
    BSplS->VKnots(ArrV);
    Locate2Coord(2,UV,DUV,BSplS,ArrV,LeftBot,RightTop);
    }
}



