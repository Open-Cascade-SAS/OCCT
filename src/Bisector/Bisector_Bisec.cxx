// Created on: 1994-07-04
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Bisector.hxx>
#include <Bisector_Bisec.hxx>
#include <Bisector_BisecAna.hxx>
#include <Bisector_BisecCC.hxx>
#include <Bisector_BisecPC.hxx>
#include <Bisector_Curve.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_CartesianPoint.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Point.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_NotImplemented.hxx>
#include <StdFail_NotDone.hxx>

#ifdef OCCT_DEBUG
//#define DRAW
#ifdef DRAW
#include <DrawTrSurf.hxx>
#pragma comment(lib, "TKDraw.lib")
static char name[100];
static Standard_Integer nbb  = 0;
static Standard_Boolean Affich = Standard_False;
#endif
#endif


static Standard_Boolean IsMaxRC (const Handle(Geom2d_Curve)& C,
  Standard_Real         U,
  Standard_Real&        R);

static void ReplaceByLineIfIsToSmall (Handle(Bisector_Curve)& Bis,
  Standard_Real&        UFirst,
  Standard_Real&        ULast);					
//=============================================================================
//function : Empty Constructor                                                
//=============================================================================
Bisector_Bisec::Bisector_Bisec()
{
}

//===========================================================================
//    calculate the bissectrice between two curves coming from a point.         
//                                                                          
//   afirstcurve   : \ curves between which the          
//   asecondcurve  : / bissectrice is calculated.                                         
//   apoint        :   point through which the bissectrice should pass.         
//   afirstvector  : \ vectors to determine the sector where       
//   asecondvector : / the bissectrice should be located.                      
//   adirection    :   shows the the side of the bissectrice to be preserved.       
//   tolerance     :   threshold starting from which the bisectrices are degenerated
//===========================================================================

void Bisector_Bisec::Perform(const Handle(Geom2d_Curve)& afirstcurve   ,
			     const Handle(Geom2d_Curve)& asecondcurve  ,
			     const gp_Pnt2d&             apoint        ,
			     const gp_Vec2d&             afirstvector  ,
			     const gp_Vec2d&             asecondvector ,
			     const Standard_Real         adirection    ,
                             const GeomAbs_JoinType      ajointype     ,
			     const Standard_Real         tolerance     ,
			     const Standard_Boolean      oncurve       )
{
  Handle(Standard_Type)  Type1 = afirstcurve ->DynamicType();
  Handle(Standard_Type)  Type2 = asecondcurve->DynamicType();
  Handle(Bisector_Curve) Bis;
  Standard_Real          UFirst,ULast;

  if (Type1 == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Type1 = Handle(Geom2d_TrimmedCurve)::DownCast(afirstcurve)
      ->BasisCurve()->DynamicType();
  }
  if (Type2 == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Type2 = Handle(Geom2d_TrimmedCurve)::DownCast(asecondcurve)
      ->BasisCurve()->DynamicType();
  }

  Handle(Geom2d_Curve) afirstcurve1 = afirstcurve;
  Handle(Geom2d_Curve) asecondcurve1 = asecondcurve;

  if(Type1 == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    Handle(Geom2d_BSplineCurve) aBS;
    if(afirstcurve->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      aBS = Handle(Geom2d_BSplineCurve)::DownCast(Handle(Geom2d_TrimmedCurve)::DownCast(afirstcurve)
                                                                               ->BasisCurve());
    }
    else
    {
      aBS = Handle(Geom2d_BSplineCurve)::DownCast(afirstcurve);
    }
    if(aBS->Degree() == 1 && aBS->NbPoles() == 2)
    {
      if(aBS->Pole(1).Distance(aBS->Pole(2)) < 1.e-4)
      {
        afirstcurve1 = GCE2d_MakeSegment(aBS->Pole(1), aBS->Pole(2)).Value();
        Type1 = STANDARD_TYPE(Geom2d_Line);
      }
    }
  }


  if(Type2 == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    Handle(Geom2d_BSplineCurve) aBS;
    if(asecondcurve->DynamicType() == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      aBS = Handle(Geom2d_BSplineCurve)::DownCast(Handle(Geom2d_TrimmedCurve)::DownCast(asecondcurve)
                                                                               ->BasisCurve());
    }
    else
    {
      aBS = Handle(Geom2d_BSplineCurve)::DownCast(asecondcurve);
    }
    if(aBS->Degree() == 1 && aBS->NbPoles() == 2)
    {
      if(aBS->Pole(1).Distance(aBS->Pole(2)) < 1.e-4)
      {
        asecondcurve1 = GCE2d_MakeSegment(aBS->Pole(1), aBS->Pole(2)).Value();
        Type2 = STANDARD_TYPE(Geom2d_Line);
      }
    }
  }

  if ( (Type1 == STANDARD_TYPE(Geom2d_Circle) || Type1 == STANDARD_TYPE(Geom2d_Line)) &&
    (Type2 == STANDARD_TYPE(Geom2d_Circle) || Type2 == STANDARD_TYPE(Geom2d_Line))   )
  {    
    //------------------------------------------------------------------
    // Analytic Bissectrice.
    //------------------------------------------------------------------
     Handle(Bisector_BisecAna) BisAna = new Bisector_BisecAna();
     BisAna->Perform(afirstcurve1  ,
		     asecondcurve1 ,
		     apoint        ,
		     afirstvector  ,
		     asecondvector ,
		     adirection    ,
                     ajointype     ,
		     tolerance     ,
		     oncurve       );
     UFirst = BisAna->ParameterOfStartPoint();
     ULast  = BisAna->ParameterOfEndPoint();    
     Bis = BisAna;   
  }
  else {  
    Standard_Boolean IsLine = Standard_False;

    if (oncurve) {
      gp_Dir2d Fd(afirstvector);
      gp_Dir2d Sd(asecondvector);
      //if (Fd.Dot(Sd) < Precision::Angular() - 1.) { 
      //if (Fd.Dot(Sd) < 10*Precision::Angular() - 1.) //patch
      if (Fd.Dot(Sd) < Sqrt(2.*Precision::Angular()) - 1.)
        IsLine = Standard_True;
    }
    if (IsLine) {     
      //------------------------------------------------------------------
      // Half-Staight.
      //------------------------------------------------------------------
      gp_Dir2d N ( - adirection*afirstvector.Y(), adirection*afirstvector.X());
      Handle (Geom2d_CartesianPoint) PG     = new Geom2d_CartesianPoint(apoint);
      Handle (Geom2d_Line)           L      = new Geom2d_Line (apoint,N);
      Handle (Geom2d_TrimmedCurve)   
        BisL   = new Geom2d_TrimmedCurve (L,0,Precision::Infinite());
      Handle(Bisector_BisecAna)      BisAna = new Bisector_BisecAna ();
      BisAna->Init(BisL);
      UFirst = BisAna->ParameterOfStartPoint();
      ULast  = BisAna->ParameterOfEndPoint();
      Bis    = BisAna;
    }
    else {
      //-------------------------------------------------------------------
      // Bissectrice algo
      //-------------------------------------------------------------------
      Handle(Bisector_BisecCC) BisCC = new Bisector_BisecCC();
      BisCC -> Perform(asecondcurve1, 
        afirstcurve1 ,
        adirection  , 
        adirection  , 
        apoint);

      if (BisCC -> IsEmpty()) {
        // bissectrice is empty. a point is projected at the end of the guide curve. 
        // Construction of a false bissectrice.
        //  modified by NIZHNY-EAP Mon Feb 21 12:00:13 2000 ___BEGIN___
        gp_Pnt2d aP1 = afirstcurve1->Value(afirstcurve1->LastParameter());
        gp_Pnt2d aP2 = asecondcurve1->Value(asecondcurve1->FirstParameter());
        gp_Pnt2d aPm(.5*(aP1.XY()+aP2.XY()));
        Standard_Real Nx, Ny;
        if(aPm.Distance(apoint) > 10.*Precision::Confusion())
        {
          Nx = apoint.X() - aPm.X();
          Ny = apoint.Y() - aPm.Y();
          if(adirection < 0)
          {
            Nx = -Nx;
            Ny = -Ny;
          }
        }
        else
        {
          gp_Dir2d dir1(afirstvector), dir2(asecondvector);
          Nx = - dir1.X() - dir2.X(),
          Ny = - dir1.Y() - dir2.Y();
          if (Abs(Nx) <= gp::Resolution() && Abs(Ny) <= gp::Resolution()) {
            Nx = -afirstvector.Y();
            Ny = afirstvector.X();
          }
        }
        gp_Dir2d N ( adirection*Nx, adirection*Ny);
        //  modified by NIZHNY-EAP Mon Feb 21 12:00:19 2000 ___END___

        Handle (Geom2d_CartesianPoint) PG     = new Geom2d_CartesianPoint(apoint);
        Handle (Geom2d_Line)           L      = new Geom2d_Line (apoint,N);
        Handle (Geom2d_TrimmedCurve)   
          BisL   = new Geom2d_TrimmedCurve (L,0,Precision::Infinite());
        Handle(Bisector_BisecAna)      BisAna = new Bisector_BisecAna ();
        BisAna->Init(BisL);
        UFirst = BisAna->ParameterOfStartPoint();
        ULast  = BisAna->ParameterOfEndPoint();
        Bis    = BisAna;
      }
      else {
        UFirst = BisCC->FirstParameter();
        ULast  = BisCC->LastParameter ();
        Bis    = BisCC;
        ReplaceByLineIfIsToSmall(Bis,UFirst,ULast);
      }
    }
  }
  UFirst = Max(UFirst, Bis->FirstParameter());
  ULast = Min(ULast, Bis->LastParameter());
  thebisector = new Geom2d_TrimmedCurve(Bis,UFirst,ULast);
#ifdef DRAW  
  if(Affich) 
  {
    sprintf( name, "c1_%d", ++nbb );
    DrawTrSurf::Set( name, afirstcurve );
    sprintf( name, "c2_%d", nbb );
    DrawTrSurf::Set( name, asecondcurve );
    sprintf( name, "p%d", nbb );
    DrawTrSurf::Set( name, apoint );
    sprintf( name, "b%d", nbb );
    DrawTrSurf::Set( name, thebisector );
  }
#endif
  
}

//===========================================================================
//  calculate the bissectrice between a curve and a point starting in a point. 
//                                                                          
//   afirstcurve   : \ curve and point the bissectrice between which is calculated.
//   asecondpoint  : /                                          
//   apoint        :   point through which the bissectrice should pass.         
//   afirstvector  : \ vectors to find the sector where       
//   asecondvector : / the bissectrice should be located.                      
//   adirection    :   shows the side of the bissectrice to be preserved.       
//   tolerance     :   threshold starting from which the bisectrices are degenerated
//===========================================================================

void Bisector_Bisec::Perform(const Handle(Geom2d_Curve)& afirstcurve  ,
  const Handle(Geom2d_Point)& asecondpoint ,
  const gp_Pnt2d&             apoint       ,
  const gp_Vec2d&             afirstvector ,
  const gp_Vec2d&             asecondvector,
  const Standard_Real         adirection   ,
  const Standard_Real         tolerance    ,
  const Standard_Boolean      oncurve       )
{  
  //gp_Pnt2d SecondPnt = asecondpoint->Pnt2d();

  Handle(Bisector_Curve) Bis;
  Handle(Standard_Type)  Type1 = afirstcurve ->DynamicType();
  Standard_Real          UFirst,ULast;

  if (Type1 == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Type1 = Handle(Geom2d_TrimmedCurve)::DownCast(afirstcurve)
      ->BasisCurve()->DynamicType();
  }

  if ( Type1 == STANDARD_TYPE(Geom2d_Circle) || Type1 == STANDARD_TYPE(Geom2d_Line)) {
    //------------------------------------------------------------------
    // Analytic Bissectrice.
    //------------------------------------------------------------------
    Handle(Bisector_BisecAna) BisAna = new Bisector_BisecAna();
    BisAna -> Perform (afirstcurve   ,
      asecondpoint  ,
      apoint        ,
      afirstvector  ,
      asecondvector ,
      adirection    ,
      tolerance     ,
      oncurve       );
    UFirst = BisAna->ParameterOfStartPoint();
    ULast  = BisAna->ParameterOfEndPoint();
    Bis    = BisAna;
  }
  else {  
    Standard_Boolean IsLine    = Standard_False;
    Standard_Real    RC        = Precision::Infinite();

    if (oncurve) {
      if (Bisector::IsConvex(afirstcurve,adirection) || 
        IsMaxRC(afirstcurve,afirstcurve->LastParameter(),RC)) { 
          IsLine = Standard_True; 
      }
    }
    if (IsLine) {     
      //------------------------------------------------------------------
      // Half-Right.
      //------------------------------------------------------------------
      gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
      Handle (Geom2d_Line)         L      = new Geom2d_Line (apoint,N);
      Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve(L,0,RC);
      Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna ();
      BisAna->Init(BisL);
      UFirst = BisAna->ParameterOfStartPoint();
      ULast  = BisAna->ParameterOfEndPoint();
      Bis    = BisAna;
    }
    else {
      //-------------------------------------------------------------------
      // Bissectrice algo
      //-------------------------------------------------------------------
      Handle(Bisector_BisecPC) BisPC = new Bisector_BisecPC();
      Handle(Geom2d_Curve) afirstcurvereverse = afirstcurve->Reversed();

      BisPC -> Perform(afirstcurvereverse   ,
        asecondpoint->Pnt2d(),
        - adirection         );
      //  Modified by Sergey KHROMOV - Thu Feb 21 16:49:54 2002 Begin
      if (BisPC -> IsEmpty()) {
        gp_Dir2d dir1(afirstvector), dir2(asecondvector);
        Standard_Real
          Nx = - dir1.X() - dir2.X(),
          Ny = - dir1.Y() - dir2.Y();
        if (Abs(Nx) <= gp::Resolution() && Abs(Ny) <= gp::Resolution()) {
          Nx = - afirstvector.Y();
          Ny = afirstvector.X();
        }
        // 	gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
        gp_Dir2d N ( adirection*Nx, adirection*Ny);
        Handle (Geom2d_Line)         L      = new Geom2d_Line (apoint,N);
        Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve(L,0,RC);
        Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna ();
        BisAna->Init(BisL);
        UFirst = BisAna->ParameterOfStartPoint();
        ULast  = BisAna->ParameterOfEndPoint();
        Bis    = BisAna;
      } else {
        //  Modified by Sergey KHROMOV - Wed Mar  6 17:01:08 2002 End
        UFirst = BisPC->Parameter(apoint);
        ULast  = BisPC->LastParameter();
        if(UFirst >= ULast)
        {
          //Standard_Real t = .9;
          //UFirst = (1. - t) * BisPC->FirstParameter() + t * ULast;
          //Extrapolate by line
          //gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
          gp_Vec2d V( BisPC->Value(BisPC->FirstParameter()), BisPC->Value(ULast) );
          gp_Dir2d N( V );
          Handle (Geom2d_Line)         L      = new Geom2d_Line         (apoint,N);
          Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve (L,0,RC);
          Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna   ();
          BisAna->Init(BisL);
          UFirst = BisAna->ParameterOfStartPoint();
          ULast  = BisAna->ParameterOfEndPoint();
          Bis    = BisAna;
        }
        else
          Bis    = BisPC;
      }
    }
  }
  if(UFirst < Bis->FirstParameter())
    UFirst = Bis->FirstParameter();
  if(ULast > Bis->LastParameter())
    ULast = Bis->LastParameter();
  thebisector = new Geom2d_TrimmedCurve(Bis,UFirst,ULast);

#ifdef DRAW
  if(Affich)
  {
  sprintf( name, "c1_%d", ++nbb );
  DrawTrSurf::Set( name, afirstcurve );
  sprintf( name, "c2_%d", nbb );
  DrawTrSurf::Set( name, asecondpoint->Pnt2d() );
  sprintf( name, "p%d", nbb );
  DrawTrSurf::Set( name, apoint );
  sprintf( name, "b%d", nbb );
  DrawTrSurf::Set( name, thebisector );
  }
#endif
}

//===========================================================================
//   calculate the bissectrice between a curve and a point starting in a point. 
//                                                                          
//   afirstpoint   : \ curve and point the bissectrice between which is calculated.         
//   asecondcurve  : /                                          
//   apoint        :   point through which the bissectrice should pass.         
//   afirstvector  : \ vectors to find the sector where       
//   asecondvector : / the bissectrice should be located.                      
//   adirection    :   shows the side of the bissectrice to be preserved.       
//   tolerance     :   threshold starting from which the bisectrices are degenerated
//===========================================================================

void Bisector_Bisec::Perform(const Handle(Geom2d_Point)& afirstpoint  ,
  const Handle(Geom2d_Curve)& asecondcurve ,
  const gp_Pnt2d&             apoint       ,
  const gp_Vec2d&             afirstvector ,
  const gp_Vec2d&             asecondvector,
  const Standard_Real         adirection   ,
  const Standard_Real         tolerance    ,
  const Standard_Boolean      oncurve       )

{  
  //gp_Pnt2d FirstPnt = afirstpoint->Pnt2d();

  Handle(Bisector_Curve) Bis;
  Handle(Standard_Type)  Type1 = asecondcurve ->DynamicType();
  Standard_Real          UFirst,ULast;

  if (Type1 == STANDARD_TYPE(Geom2d_TrimmedCurve)) {
    Type1 = Handle(Geom2d_TrimmedCurve)::DownCast(asecondcurve)
      ->BasisCurve()->DynamicType();
  }

  if ( Type1 == STANDARD_TYPE(Geom2d_Circle) || Type1 == STANDARD_TYPE(Geom2d_Line)) {
    //------------------------------------------------------------------
    // Analytic Bissectrice.
    //------------------------------------------------------------------
    Handle(Bisector_BisecAna) BisAna = new Bisector_BisecAna();
    BisAna -> Perform (afirstpoint   ,
      asecondcurve  ,
      apoint        ,
      afirstvector  ,
      asecondvector ,
      adirection    ,
      tolerance     ,
      oncurve       );
    UFirst = BisAna->ParameterOfStartPoint();
    ULast  = BisAna->ParameterOfEndPoint();
    Bis    = BisAna;
  }
  else {   
    //  Standard_Real    UPoint    = 0.;
    Standard_Boolean IsLine    = Standard_False;
    Standard_Real    RC        = Precision::Infinite();

    if (oncurve) {
      if (Bisector::IsConvex(asecondcurve, adirection) || 
        IsMaxRC(asecondcurve,asecondcurve->FirstParameter(),RC)) {
          IsLine = Standard_True;
      }
    }    
    if (IsLine) {     
      //------------------------------------------------------------------
      // Half-Staight.
      //------------------------------------------------------------------
      gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
      Handle (Geom2d_Line)         L      = new Geom2d_Line         (apoint,N);
      Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve (L,0,RC);
      Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna   ();
      BisAna->Init(BisL);
      UFirst = BisAna->ParameterOfStartPoint();
      ULast  = BisAna->ParameterOfEndPoint();
      Bis    = BisAna;
    }
    else {
      //-------------------------------------------------------------------
      // Bissectrice algo
      //-------------------------------------------------------------------
      Handle(Bisector_BisecPC) BisPC = new Bisector_BisecPC();
      BisPC -> Perform(asecondcurve        ,
        afirstpoint->Pnt2d(),
        adirection          );
      //  Modified by Sergey KHROMOV - Thu Feb 21 16:49:54 2002 Begin
      if (BisPC -> IsEmpty()) {
        gp_Dir2d dir1(afirstvector), dir2(asecondvector);
        Standard_Real
          Nx = - dir1.X() - dir2.X(),
          Ny = - dir1.Y() - dir2.Y();
        if (Abs(Nx) <= gp::Resolution() && Abs(Ny) <= gp::Resolution()) {
          Nx = - afirstvector.Y();
          Ny = afirstvector.X();
        }
        // 	gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
        gp_Dir2d N ( adirection*Nx, adirection*Ny);
        Handle (Geom2d_Line)         L      = new Geom2d_Line (apoint,N);
        Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve(L,0,RC);
        Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna ();
        BisAna->Init(BisL);
        UFirst = BisAna->ParameterOfStartPoint();
        ULast  = BisAna->ParameterOfEndPoint();
        Bis    = BisAna;
      } else {
        //  Modified by Sergey KHROMOV - Thu Feb 21 16:49:58 2002 End
        UFirst = BisPC->Parameter(apoint);
        ULast  = BisPC->LastParameter();
        if(UFirst >= ULast)
        {
          //Extrapolate by line
          //gp_Dir2d N ( -adirection*afirstvector.Y(), adirection*afirstvector.X());
          gp_Vec2d V( BisPC->Value(BisPC->FirstParameter()), BisPC->Value(ULast) );
          gp_Dir2d N( V );
          Handle (Geom2d_Line)         L      = new Geom2d_Line         (apoint,N);
          Handle (Geom2d_TrimmedCurve) BisL   = new Geom2d_TrimmedCurve (L,0,RC);
          Handle(Bisector_BisecAna)    BisAna = new Bisector_BisecAna   ();
          BisAna->Init(BisL);
          UFirst = BisAna->ParameterOfStartPoint();
          ULast  = BisAna->ParameterOfEndPoint();
          Bis    = BisAna;
        }
        else
          Bis    = BisPC;
      }
    }
  }
 
  UFirst = Max(UFirst, Bis->FirstParameter());
  ULast = Min(ULast, Bis->LastParameter());
  thebisector = new Geom2d_TrimmedCurve(Bis,UFirst,ULast);

#ifdef DRAW
  if(Affich)
  {
  sprintf( name, "c1_%d", ++nbb );
  DrawTrSurf::Set( name, afirstpoint->Pnt2d() );
  sprintf( name, "c2_%d", nbb );
  DrawTrSurf::Set( name, asecondcurve );
  sprintf( name, "p%d", nbb );
  DrawTrSurf::Set( name, apoint );
  sprintf( name, "b%d", nbb );
  DrawTrSurf::Set( name, thebisector );
  }
#endif

}

//===========================================================================
//        calculate the bissectrice between two points starting in a point.      
//                                                                          
//   afirstpoint   : \ curves the bissectrice between which should be          
//   asecondpoint  : / calculated.                                         
//   apoint        :   point through which the bissectrice should pass.         
//   afirstvector  : \ vectors to find the sector where       
//   asecondvector : / the bissectrice should be located.                      
//   adirection    :   shows the side of the bissectrice to be preserved.       
//===========================================================================

void Bisector_Bisec::Perform(const Handle(Geom2d_Point)& afirstpoint  ,
  const Handle(Geom2d_Point)& asecondpoint ,
  const gp_Pnt2d&             apoint       ,
  const gp_Vec2d&             afirstvector ,
  const gp_Vec2d&             asecondvector,
  const Standard_Real         adirection   ,
  const Standard_Real         tolerance    ,
  const Standard_Boolean      oncurve      )
{
  Handle(Bisector_BisecAna) Bis = new Bisector_BisecAna();

  Bis -> Perform (afirstpoint   ,
    asecondpoint  ,
    apoint        ,
    afirstvector  ,
    asecondvector ,
    adirection    ,
    tolerance     ,
    oncurve       ); 
  thebisector = new Geom2d_TrimmedCurve(Bis,
    Bis->ParameterOfStartPoint(),
    Bis->ParameterOfEndPoint());

#ifdef DRAW
  if(Affich)
  {
  sprintf( name, "c1_%d", ++nbb );
  DrawTrSurf::Set( name, afirstpoint->Pnt2d() );
  sprintf( name, "c2_%d", nbb );
  DrawTrSurf::Set( name, asecondpoint->Pnt2d() );
  sprintf( name, "p%d", nbb );
  DrawTrSurf::Set( name, apoint );
  sprintf( name, "b%d", nbb );
  DrawTrSurf::Set( name, thebisector );
  }
#endif
}

//=============================================================================
//function : Value
//purpose  :
//=============================================================================
const Handle(Geom2d_TrimmedCurve)&  Bisector_Bisec::Value() const
{
  return thebisector;
}

//=============================================================================
//function : ChangeValue
//purpose  :
//=============================================================================
const Handle(Geom2d_TrimmedCurve)&  Bisector_Bisec::ChangeValue()
{
  return thebisector;
}

//=============================================================================
//function : ReplaceByLineIfIsToSmall 
//purpose  : If the size of an algorithmic bissectrice is negligeable it is
//           replaced by a half-straight.
//=============================================================================
static void ReplaceByLineIfIsToSmall (Handle(Bisector_Curve)& Bis,
  Standard_Real&        UFirst,
  Standard_Real&        ULast )

{
  if (Abs(ULast - UFirst) > 2.*Precision::PConfusion()*10.) return; //patch

  gp_Pnt2d PF = Bis->Value(UFirst);
  gp_Pnt2d PL = Bis->Value(ULast);

  if (PF.Distance(PL) > Precision::Confusion()*10.) return;

  gp_Vec2d T1 = Bis->DN(UFirst,1);

  Handle (Geom2d_CartesianPoint) PG     = new Geom2d_CartesianPoint(PF);
  Handle (Geom2d_Line)           L      = new Geom2d_Line (PF,T1);
  Handle (Geom2d_TrimmedCurve)   
    BisL   = new Geom2d_TrimmedCurve (L,0,Precision::Infinite());
  Handle(Bisector_BisecAna)      BisAna = new Bisector_BisecAna ();
  BisAna->Init(BisL);
  UFirst = BisAna->ParameterOfStartPoint();
  ULast  = BisAna->ParameterOfEndPoint();
  Bis    = BisAna;
}

//=============================================================================
//function : IsMaxRC
//purpose  :
//=============================================================================
static Standard_Boolean  IsMaxRC (const Handle(Geom2d_Curve)& C,
  Standard_Real         U,
  Standard_Real&        R)
{  
  Standard_Real KF,KL;
  Standard_Real US = C->FirstParameter();
  Standard_Real UL = C->LastParameter();

  gp_Vec2d      D1,D2;
  gp_Pnt2d      P;
  Standard_Real Norm2;

  C->D2(US,P,D1,D2);
  Norm2 = D1.SquareMagnitude();;
  if (Norm2 < gp::Resolution()) { KF = 0.0;}
  else                          { KF = Abs(D1^D2)/(Norm2*sqrt(Norm2));}

  C->D2(UL,P,D1,D2);
  Norm2 = D1.SquareMagnitude();;
  if (Norm2 < gp::Resolution()) { KL = 0.0;}
  else                          { KL = Abs(D1^D2)/(Norm2*sqrt(Norm2));}

  Standard_Boolean IsMax = Standard_False;

  if (U == UL) {
    if (KL < KF) {
      if (KL == 0.0) R = Precision::Infinite(); else R = 1/KL;
      IsMax = Standard_True;
    }
  }
  else {   
    if (KF < KL) {
      if (KF == 0.0) R = Precision::Infinite(); else R = 1/KF;
      IsMax = Standard_True;
    }
  }
  return IsMax;
}
