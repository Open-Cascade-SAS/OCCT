// Created on: 1992-04-27
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

// Modified:    OFV Thu Nov  6 17:03:52 2003

#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntCurve_IConicTool.hxx>
#include <IntCurve_IntConicConic.hxx>
#include <IntCurve_PConic.hxx>
#include <IntRes2d_Domain.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
// Perform() for
//              Line      - Parabola
//              Line      - Hyperbola
//              Circle    - Parabola
//              Circle    - Elipse
//              Circle    - Hyperbola
//              Parabola  - Parabola
//              Elipse    - Parabola
//              Parabola  - Hyperbola
//              Elipse    - Elipse
//              Elipse    - Hyperbola
//              Hyperbola - Hyperbola
//=======================================================================
static const Standard_Real PARAM_MAX_ON_PARABOLA  = 100000000.0;
static const Standard_Real PARAM_MAX_ON_HYPERBOLA = 10000.0;
static const Standard_Real TOL_EXACT_INTER        = 1.e-7;

static inline Standard_Boolean BOUNDED_DOMAIN(const IntRes2d_Domain& domain)
{
  return (domain.HasFirstPoint() && domain.HasLastPoint());
}

static Standard_Boolean SET_BOUNDED_DOMAIN(const IntRes2d_Domain domain,
					   Standard_Real&        binf,
					   Standard_Real&        tolinf,
					   gp_Pnt2d&             Pntinf,
					   Standard_Real&        bsup,
					   Standard_Real&        tolsup,
					   gp_Pnt2d&             Pntsup)
{ 
  if(domain.HasFirstPoint()) { 
    if(binf<domain.FirstParameter()) {
      Pntinf = domain.FirstPoint();
      binf   = domain.FirstParameter();
      tolinf = domain.FirstTolerance(); 
    }
  }
  if(domain.HasLastPoint()) { 
    if(bsup>domain.FirstParameter()) {
      Pntsup = domain.LastPoint();
      bsup   = domain.LastParameter();
      tolsup = domain.LastTolerance(); 
    }
  }
  Standard_Boolean result = (bsup <= binf) ? Standard_False : Standard_True;
//   if(bsup<=binf) return(Standard_False);
//   return(Standard_True);
  return result;
}

void SetBinfBsupFromIntAna2d(const IntAna2d_AnaIntersection& theIntAna2d,
			     Standard_Real&                  binf,
			     gp_Pnt2d&                       pntinf,
			     Standard_Real& bsup,gp_Pnt2d& pntsup,
			     const gp_Hypr2d& H,
			     const Standard_Real maxtol,
			     const Standard_Real LIMITE);

void SetBinfBsupFromIntAna2d(const IntAna2d_AnaIntersection &theIntAna2d,
			     Standard_Real& binf,gp_Pnt2d& pntinf,
			     Standard_Real& bsup,gp_Pnt2d& pntsup,
			     const gp_Parab2d& PR,
			     const Standard_Real maxtol,
			     const Standard_Real LIMITE);
   
//=======================================================================
//function : Perform
//purpose  : Line - Parabola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Lin2d&        L,
				     const IntRes2d_Domain& DL,
				     const gp_Parab2d&      P,
				     const IntRes2d_Domain& DP,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  
  this->ResetFields(); 
  IntCurve_IConicTool ITool(L);
  IntCurve_PConic PCurve(P);
  
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());
  
  Standard_Real binf   = Precision::Infinite();
  Standard_Real bsup   = -Precision::Infinite();
  Standard_Real maxtol = (Tol > TolConf) ? Tol : TolConf;
  if( maxtol< 1.e-7 ) maxtol = 1.e-7;
  Standard_Boolean wasSet = Standard_False;

  gp_Pnt2d Pntinf,Pntsup;
  IntAna2d_AnaIntersection theIntAna2d;

  maxtol*=100.0;
//  if(maxtol<0.000001) maxtol = 0.000001;

  gp_Vec2d Offset(maxtol*L.Direction().Y(),maxtol*L.Direction().X());
  gp_Lin2d Lp(L.Translated(Offset));
  theIntAna2d.Perform(P,Lp);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);

  Offset.Reverse();
  gp_Lin2d Lm(L.Translated(Offset));
  theIntAna2d.Perform(P,Lm);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);

  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DP)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DP,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) { 
	IntRes2d_Domain DPModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DL,PCurve,DPModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DP.FirstParameter())  { 
	binf   =  DP.FirstParameter();
	Pntinf =  DP.FirstPoint();
	ft     =  DP.FirstTolerance();
	if(bsup < DP.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DP.LastParameter()) { 
	bsup   =  DP.LastParameter();
	Pntsup =  DP.LastPoint();
	lt     =  DP.LastTolerance();
	if(binf>DP.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      IntRes2d_Domain DPModif(Pntinf,binf,ft,Pntsup,bsup,lt);
//      Inter.Perform(ITool,DL,PCurve,DPModif,TolConf,Tol);
      Inter.Perform(ITool,DL,PCurve,DPModif,TOL_EXACT_INTER,TOL_EXACT_INTER);
      this->SetValues(Inter);
      wasSet = Standard_True;
      if( this->IsDone() && this->NbPoints() == 0 ) {
	this->ResetFields(); 
	Inter.Perform(ITool,DL,PCurve,DPModif,TolConf,Tol);
	wasSet = Standard_False;
      }
    }
    if( !wasSet ) this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}


//=======================================================================
//function : Perform
//purpose  : Line - Hyperbola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Lin2d&        L,
				     const IntRes2d_Domain& DL,
				     const gp_Hypr2d&       H,
				     const IntRes2d_Domain& DH,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(L);
  IntCurve_PConic PCurve(H);
  PCurve.SetAccuracy(20);
  

  Inter.SetReversedParameters(ReversedParameters());

  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  if(Tol > TolConf) maxtol = Tol; else maxtol = TolConf;
  maxtol*=100.0;
  if(maxtol<0.000001) maxtol = 0.000001;
  gp_Vec2d Offset(maxtol*H.XAxis().Direction().X(),
		  maxtol*H.XAxis().Direction().Y());
  gp_Hypr2d Hp(H.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Hp,L);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  Offset.Reverse();
  gp_Hypr2d Hm(H.Translated(Offset));
  theIntAna2d.Perform(Hm,L);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DH)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DH,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) { 
	IntRes2d_Domain DHModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DL,PCurve,DHModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DH.FirstParameter())  { 
	binf   =  DH.FirstParameter();
	Pntinf =  DH.FirstPoint();
	ft     =  DH.FirstTolerance();
	if(bsup < DH.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DH.LastParameter()) { 
	bsup   =  DH.LastParameter();
	Pntsup =  DH.LastPoint();
	lt     =  DH.LastTolerance();
	if(binf>DH.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      IntRes2d_Domain DHModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,DL,PCurve,DHModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}

//=======================================================================
//function : Perform
//purpose  : Circle - Parabola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Circ2d&       C,
				     const IntRes2d_Domain& DC,
				     const gp_Parab2d&      P,
				     const IntRes2d_Domain& DP,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(C);
  IntCurve_PConic PCurve(P);
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());

  IntRes2d_Domain D(DC);
  if(! DC.IsClosed()) {
    D.SetEquivalentParameters(DC.FirstParameter(),DC.FirstParameter()+M_PI+M_PI);
  }
  
  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  maxtol = C.Radius() / 10.0;
  gp_Circ2d Cp(C);
  Cp.SetRadius(C.Radius()+maxtol);
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(P,Cp);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);
  if(C.Radius() > maxtol) { 
    Cp.SetRadius(C.Radius() - maxtol);
    theIntAna2d.Perform(P,Cp);
    SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);
  }
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DP)) { 
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DP,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) { 
	IntRes2d_Domain DPModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,D,PCurve,DPModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return; 
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DP.FirstParameter())  { 
	binf   =  DP.FirstParameter();
	Pntinf =  DP.FirstPoint();
	ft     =  DP.FirstTolerance();
	if(bsup < DP.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DP.LastParameter()) { 
	bsup   =  DP.LastParameter();
	Pntsup =  DP.LastPoint();
	lt     =  DP.LastTolerance();
	if(binf>DP.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      IntRes2d_Domain DPModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,D,PCurve,DPModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
} 
          
//=======================================================================
//function : Perform
//purpose  : Circle - Elipse
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Circ2d&       C,
				     const IntRes2d_Domain& DC,
				     const gp_Elips2d&      E,
				     const IntRes2d_Domain& DE,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(C);
  IntCurve_PConic PCurve(E);
  PCurve.SetAccuracy(20);
  
  Inter.SetReversedParameters(ReversedParameters());

  if(! DC.IsClosed()) {
    IntRes2d_Domain D1(DC);
    D1.SetEquivalentParameters(DC.FirstParameter(),DC.FirstParameter()+M_PI+M_PI);
    if(! DE.IsClosed()) {
      IntRes2d_Domain D2(DE);
      D2.SetEquivalentParameters(DE.FirstParameter(),DE.FirstParameter()+M_PI+M_PI);
      Inter.Perform(ITool,D1,PCurve,D2,TolConf,Tol);
    }
    else { 
      Inter.Perform(ITool,D1,PCurve,DE,TolConf,Tol);
    }
  }
  else { 
    if(! DE.IsClosed()) {
      IntRes2d_Domain D2(DE);
      D2.SetEquivalentParameters(DE.FirstParameter(),DE.FirstParameter()+M_PI+M_PI);
      Inter.Perform(ITool,DC,PCurve,D2,TolConf,Tol);
    }
    else { 
      Inter.Perform(ITool,DC,PCurve,DE,TolConf,Tol);
    }
  }
  this->SetValues(Inter);
}

//=======================================================================
//function : Perform
//purpose  : Circle - Hyperbola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Circ2d&       C,
				     const IntRes2d_Domain& DC,
				     const gp_Hypr2d&       H,
				     const IntRes2d_Domain& DH,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(C);
  IntCurve_PConic PCurve(H);
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());
  IntRes2d_Domain D(DC);

  if(! DC.IsClosed()) {
    D.SetEquivalentParameters(DC.FirstParameter(),DC.FirstParameter()+M_PI+M_PI); 
  }
  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  maxtol = C.Radius() / 10.0;
  gp_Vec2d Offset(maxtol*H.XAxis().Direction().X(),
		  maxtol*H.XAxis().Direction().Y());
  gp_Hypr2d Hp(H.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Hp,C);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  Offset.Reverse();
  gp_Hypr2d Hm(H.Translated(Offset));
  theIntAna2d.Perform(Hm,C);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DH)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DH,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DHModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,D,PCurve,DHModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else {
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DH.FirstParameter())  { 
	binf   =  DH.FirstParameter();
	Pntinf =  DH.FirstPoint();
	ft     =  DH.FirstTolerance();
	if(bsup < DH.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DH.LastParameter()) { 
	bsup   =  DH.LastParameter();
	Pntsup =  DH.LastPoint();
	lt     =  DH.LastTolerance();
	if(binf>DH.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      IntRes2d_Domain DHModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,D,PCurve,DHModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}
           
//=======================================================================
//function : Perform
//purpose  : Parabola - Parabola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Parab2d& P1,
				     const IntRes2d_Domain& DP1,
				     const gp_Parab2d& P2,
				     const IntRes2d_Domain& DP2,
				     const Standard_Real TolConf,
				     const Standard_Real Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(P1);
  IntCurve_PConic PCurve(P2);
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());
  
  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  if(Tol > TolConf) maxtol = Tol; else maxtol = TolConf;
  maxtol*=100.0;
  if(maxtol<0.000001) maxtol = 0.000001;
  gp_Vec2d Offset(maxtol*P2.MirrorAxis().Direction().X(),
		  maxtol*P2.MirrorAxis().Direction().Y());
  gp_Parab2d Pp(P2.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Pp,P1);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P2,maxtol,PARAM_MAX_ON_PARABOLA);
  Offset.Reverse();
  gp_Parab2d Pm(P2.Translated(Offset));
  theIntAna2d.Perform(Pm,P1);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P2,maxtol,PARAM_MAX_ON_PARABOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DP2)) { 
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DP2,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DPModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DP1,PCurve,DPModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DP2.FirstParameter())  { 
	binf   =  DP2.FirstParameter();
	Pntinf =  DP2.FirstPoint();
	ft     =  DP2.FirstTolerance();
	if(bsup < DP2.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DP2.LastParameter()) { 
	bsup   =  DP2.LastParameter();
	Pntsup =  DP2.LastPoint();
	lt     =  DP2.LastTolerance();
	if(binf>DP2.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      IntRes2d_Domain DPModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,DP1,PCurve,DPModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}

//=======================================================================
//function : Perform
//purpose  : Elipse - Parabola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Elips2d&      E,
				     const IntRes2d_Domain& DE,
				     const gp_Parab2d&      P,
				     const IntRes2d_Domain& DP,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(E);
  IntCurve_PConic PCurve(P);
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());

  IntRes2d_Domain D(DE);
  if(! DE.IsClosed()) {
    D.SetEquivalentParameters(DE.FirstParameter(),DE.FirstParameter()+M_PI+M_PI);
  }  

  //-- cout<<" Parab Elips "<<endl;

  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  if(Tol > TolConf) maxtol = Tol; else maxtol = TolConf;

  maxtol = E.MinorRadius() / 10.0;
  gp_Elips2d Ep(E);
  Ep.SetMajorRadius(E.MajorRadius()+maxtol);
  Ep.SetMinorRadius(E.MinorRadius()+maxtol);
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(P,Ep);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);
  if(E.MinorRadius() > maxtol) {
    Ep.SetMinorRadius(E.MinorRadius() - maxtol); 
    Ep.SetMajorRadius(E.MajorRadius() - maxtol);
    theIntAna2d.Perform(P,Ep);
    SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,P,maxtol,PARAM_MAX_ON_PARABOLA);
  }
  //-- cout<<" binf,bsup "<<binf<<" "<<bsup<<endl;
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DP)) { 
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DP,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DPModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,D,PCurve,DPModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return; 
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DP.FirstParameter())  { 
	binf   =  DP.FirstParameter();
	Pntinf =  DP.FirstPoint();
	ft     =  DP.FirstTolerance();
	if(bsup < DP.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DP.LastParameter()) { 
	bsup   =  DP.LastParameter();
	Pntsup =  DP.LastPoint();
	lt     =  DP.LastTolerance();
	if(binf>DP.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      //-- cout<<"BOUNDED  binf,bsup "<<binf<<" "<<bsup<<endl;
      //-- cout<<"Domain   "<<DP.FirstParameter()<<" "<<DP.LastParameter()<<endl;
      IntRes2d_Domain DPModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,D,PCurve,DPModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}           

//=======================================================================
//function : Perform
//purpose  : Parabola - Hyperbola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Parab2d&      P,
				     const IntRes2d_Domain& DP,
				     const gp_Hypr2d&       H,
				     const IntRes2d_Domain& DH,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(P);
  IntCurve_PConic PCurve(H);
  Inter.SetReversedParameters(ReversedParameters());

  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  if(Tol > TolConf) maxtol = Tol; else maxtol = TolConf;
  maxtol*=100.0;
  if(maxtol<0.000001) maxtol = 0.000001;
  gp_Vec2d Offset(maxtol*H.XAxis().Direction().X(),
		  maxtol*H.XAxis().Direction().Y());
  gp_Hypr2d Hp(H.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Hp,P);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  Offset.Reverse();
  gp_Hypr2d Hm(H.Translated(Offset));
  theIntAna2d.Perform(Hm,P);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DH)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DH,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DHModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DP,PCurve,DHModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DH.FirstParameter())  { 
	binf   =  DH.FirstParameter();
	Pntinf =  DH.FirstPoint();
	ft     =  DH.FirstTolerance();
	if(bsup < DH.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DH.LastParameter()) { 
	bsup   =  DH.LastParameter();
	Pntsup =  DH.LastPoint();
	lt     =  DH.LastTolerance();
	if(binf>DH.LastParameter()) { 
	  done=Standard_True; return;
	}
      } 
      IntRes2d_Domain DHModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,DP,PCurve,DHModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}

//=======================================================================
//function : Perform
//purpose  : Elipse - Elipse
//=======================================================================

void IntCurve_IntConicConic::Perform(const gp_Elips2d&      E1,
				     const IntRes2d_Domain& DE1,
				     const gp_Elips2d&      E2,
				     const IntRes2d_Domain& DE2,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(E1);
  IntCurve_PConic PCurve(E2);
  PCurve.SetAccuracy(20);
  

  Inter.SetReversedParameters(ReversedParameters());

  if(! DE1.IsClosed()) {
    IntRes2d_Domain D1(DE1);
    D1.SetEquivalentParameters(DE1.FirstParameter(),DE1.FirstParameter()+M_PI+M_PI);
    if(! DE2.IsClosed()) {
      IntRes2d_Domain D2(DE2);
      D2.SetEquivalentParameters(DE2.FirstParameter(),DE2.FirstParameter()+M_PI+M_PI);
      Inter.Perform(ITool,D1,PCurve,D2,TolConf,Tol);
    }
    else { 
      Inter.Perform(ITool,D1,PCurve,DE2,TolConf,Tol);
    }
  }
  else { 
    if(! DE2.IsClosed()) {
      IntRes2d_Domain D2(DE2);
      D2.SetEquivalentParameters(DE2.FirstParameter(),DE2.FirstParameter()+M_PI+M_PI);
      Inter.Perform(ITool,DE1,PCurve,D2,TolConf,Tol);
    }
    else { 
      Inter.Perform(ITool,DE1,PCurve,DE2,TolConf,Tol);
    }
  }
  this->SetValues(Inter);
}

//=======================================================================
//function : Perform
//purpose  : Elipse - Hyperbola
//=======================================================================
void IntCurve_IntConicConic::Perform(const gp_Elips2d&      E,
				     const IntRes2d_Domain& DE,
				     const gp_Hypr2d&       H,
				     const IntRes2d_Domain& DH,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(E);
  IntCurve_PConic PCurve(H);
  PCurve.SetAccuracy(20);
  Inter.SetReversedParameters(ReversedParameters());
  IntRes2d_Domain DEModif(DE);

  if(! DE.IsClosed()) {
    DEModif.SetEquivalentParameters(DE.FirstParameter(),DE.FirstParameter()+M_PI+M_PI);
  }
  
  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  maxtol = E.MinorRadius() / 10.0;
  gp_Vec2d Offset(maxtol*H.XAxis().Direction().X(),
		  maxtol*H.XAxis().Direction().Y());
  gp_Hypr2d Hp(H.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Hp,E);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  Offset.Reverse();
  gp_Hypr2d Hm(H.Translated(Offset));
  theIntAna2d.Perform(Hm,E);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H,maxtol,PARAM_MAX_ON_HYPERBOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DH)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DH,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DHModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DEModif,PCurve,DHModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DH.FirstParameter())  { 
	binf   =  DH.FirstParameter();
	Pntinf =  DH.FirstPoint();
	ft     =  DH.FirstTolerance();
	if(bsup < DH.FirstParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(bsup> DH.LastParameter()) { 
	bsup   =  DH.LastParameter();
	Pntsup =  DH.LastPoint();
	lt     =  DH.LastTolerance();
	if(binf>DH.LastParameter()) { 
	  done=Standard_True; return;
	}
      }
      if(binf>=bsup) {  
	done = Standard_True; return;
      }
      IntRes2d_Domain DHModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,DEModif,PCurve,DHModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}

//=======================================================================
//function : Perform
//purpose  : Hyperbola - Hyperbola
//=======================================================================

void IntCurve_IntConicConic::Perform(const gp_Hypr2d&       H1,
				     const IntRes2d_Domain& DH1,
				     const gp_Hypr2d&       H2,
				     const IntRes2d_Domain& DH2,
				     const Standard_Real    TolConf,
				     const Standard_Real    Tol)
{
  this->ResetFields();
  IntCurve_IConicTool ITool(H1);
  IntCurve_PConic PCurve(H2);
  PCurve.SetAccuracy(20);
  
  Inter.SetReversedParameters(ReversedParameters());

  Standard_Real binf = Precision::Infinite(),bsup = -Precision::Infinite(),maxtol;
  gp_Pnt2d Pntinf,Pntsup;
  if(Tol > TolConf) maxtol = Tol; else maxtol = TolConf;
  maxtol*=100.0;
  if(maxtol<0.000001) maxtol = 0.000001;
  gp_Vec2d Offset(maxtol*H2.XAxis().Direction().X(),
		  maxtol*H2.XAxis().Direction().Y());
  gp_Hypr2d Hp(H2.Translated(Offset));
  IntAna2d_AnaIntersection theIntAna2d;
  theIntAna2d.Perform(Hp,H1);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H2,maxtol,PARAM_MAX_ON_HYPERBOLA);
  Offset.Reverse();
  gp_Hypr2d Hm(H2.Translated(Offset));
  theIntAna2d.Perform(Hm,H1);
  SetBinfBsupFromIntAna2d(theIntAna2d,binf,Pntinf,bsup,Pntsup,H2,maxtol,PARAM_MAX_ON_HYPERBOLA);
  if(binf<=bsup) { 
    if(!BOUNDED_DOMAIN(DH2)) {
      Standard_Real tolinf = 0.0;
      Standard_Real tolsup = 0.0;
      if(SET_BOUNDED_DOMAIN(DH2,binf,tolinf,Pntinf,bsup,tolsup,Pntsup)) {       
	IntRes2d_Domain DHModif(Pntinf,binf,tolinf,Pntsup,bsup,tolsup);
	Inter.Perform(ITool,DH1,PCurve,DHModif,TolConf,Tol);
      }
      else { 
	done = Standard_True; return;
      }
    }
    else { 
      Standard_Real ft=0.0,lt=0.0;
      if(binf< DH2.FirstParameter())  { 
	binf   =  DH2.FirstParameter();
	Pntinf =  DH2.FirstPoint();
	ft     =  DH2.FirstTolerance();
      }
      if(bsup> DH2.LastParameter()) { 
	bsup   =  DH2.LastParameter();
	Pntsup =  DH2.LastPoint();
	lt     =  DH2.LastTolerance();
      }
      
      //-- lbr le 22 sept 97 
      //-- binf=-5.6 bsup=-5.3 DH2=(5.6,5.7) -> binf=+5.6 bsup=-5.3 
      if(binf>=bsup) {  
	done = Standard_True; return;
      }
      IntRes2d_Domain DHModif(Pntinf,binf,ft,Pntsup,bsup,lt);
      Inter.Perform(ITool,DH1,PCurve,DHModif,TolConf,Tol);
    }
    this->SetValues(Inter);
  }
  else { 
    done = Standard_True;
  }
}
//----------------------------------------------------------------------


void SetBinfBsupFromIntAna2d(const IntAna2d_AnaIntersection &theIntAna2d,
			     Standard_Real& binf,gp_Pnt2d& Pntinf,
			     Standard_Real& bsup,gp_Pnt2d& Pntsup,
			     const gp_Parab2d& PR,
			     const Standard_Real maxtol,
			     const Standard_Real LIMITE) { 


  if(theIntAna2d.IsDone()) { 
    if(!theIntAna2d.IsEmpty()) { 
      for(Standard_Integer p=1; p<=theIntAna2d.NbPoints(); p++) { 
	Standard_Real param = theIntAna2d.Point(p).ParamOnFirst();
	
	if(Abs(param) < LIMITE) { 
	  gp_Vec2d V;
	  gp_Pnt2d P;
	  ElCLib::D1(param,PR,P,V);
	  Standard_Real NormeD1 = V.Magnitude();
	  Standard_Real dparam  = 100.0*maxtol/NormeD1;
	  if(dparam<1e-3) dparam = 1e-3;
	  param-=dparam;
	  
	  if(param < binf) { 
	    binf = param; 
	    Pntinf=ElCLib::Value(param,PR);
	  }
	  param+=dparam+dparam;
	  if(param > bsup) {
	    bsup = param; 
	    Pntsup=ElCLib::Value(param,PR);
	  } 
	}
      }
    }
  }
}
void SetBinfBsupFromIntAna2d(const IntAna2d_AnaIntersection &theIntAna2d,
			     Standard_Real& binf,gp_Pnt2d& Pntinf,
			     Standard_Real& bsup,gp_Pnt2d& Pntsup,
			     const gp_Hypr2d& H,
			     const Standard_Real maxtol,
			     const Standard_Real LIMITE) { 


  if(theIntAna2d.IsDone()) { 
    if(!theIntAna2d.IsEmpty()) { 
      for(Standard_Integer p=1; p<=theIntAna2d.NbPoints(); p++) { 
	Standard_Real param = theIntAna2d.Point(p).ParamOnFirst();
	
	if(Abs(param) < LIMITE) { 
	  gp_Vec2d V;
	  gp_Pnt2d P;
	  ElCLib::D1(param,H,P,V);
	  Standard_Real NormeD1 = V.Magnitude();
	  Standard_Real dparam  = 100.0*maxtol/NormeD1;
	  if(dparam<1e-3) dparam = 1e-3;
	  param-=dparam;
	  
	  if(param < binf) { 
	    binf = param; 
	    Pntinf=ElCLib::Value(param,H);
	  }
	  param+=dparam+dparam;
	  if(param > bsup) {
	    bsup = param; 
	    Pntsup=ElCLib::Value(param,H);
	  } 
	}
      }
    }
  }
}



