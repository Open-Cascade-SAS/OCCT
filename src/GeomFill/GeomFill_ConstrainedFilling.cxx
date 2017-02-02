// Created on: 1995-10-26
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

//  Modified by skv - Fri Jun 18 12:52:54 2004 OCC6129

#include <AdvApprox_ApproxAFunction.hxx>
#include <BSplCLib.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_Boundary.hxx>
#include <GeomFill_BoundWithSurf.hxx>
#include <GeomFill_ConstrainedFilling.hxx>
#include <GeomFill_CoonsAlgPatch.hxx>
#include <GeomFill_DegeneratedBound.hxx>
#include <GeomFill_TgtField.hxx>
#include <GeomFill_TgtOnCoons.hxx>
#include <gp_XYZ.hxx>
#include <Law.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_BSpline.hxx>
#include <Law_Linear.hxx>
#include <PLib.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NotImplemented.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_HArray1OfReal.hxx>

#ifdef DRAW
// Pour le dessin.
#include <Draw_Appli.hxx>
#include <Draw_Display.hxx>
#include <Draw.hxx>
#include <Draw_Segment3D.hxx>
#include <Draw_Segment2D.hxx>
#include <Draw_Marker2D.hxx>
#include <Draw_ColorKind.hxx>
#include <Draw_MarkerShape.hxx>
static Standard_Boolean dodraw = 0;
static Standard_Real drawfac = 0.1;
#endif
#ifdef OCCT_DEBUG
Standard_IMPORT void Law_draw1dcurve(const TColStd_Array1OfReal&    pol,
			    const TColStd_Array1OfReal&    knots,
			    const TColStd_Array1OfInteger& mults,
			    const Standard_Integer         deg, 
			    const gp_Vec2d&                tra,
			    const Standard_Real            scal);
Standard_IMPORT void Law_draw1dcurve(const Handle(Law_BSpline)&     bs,
			    const gp_Vec2d&                tra,
			    const Standard_Real            scal);


// Pour les mesures.
#include <OSD_Chronometer.hxx>
static OSD_Chronometer totclock, parclock, appclock, cstclock;
#endif

static Standard_Integer inqadd(const Standard_Real    d1,
			       const Standard_Real    d2,
			       Standard_Real*         k,
			       Standard_Integer*      m,
			       const Standard_Integer deg,
			       const Standard_Real    tolk)
{
  Standard_Integer nbadd = 0;
  m[0] = m[1] = deg - 2;
  if (d1 != 1. && d2 != 1.){
    if(Abs(d1+d2-1.) < tolk) {
      k[0] = 0.5 * (d1 + 1. - d2);
      nbadd = 1;
    }
    else{
      nbadd = 2;
      k[0] = Min(d1,1. - d2);
      k[1] = Max(d1,1. - d2);
    }
  }
  else if (d1 != 1.) {
    k[0] = d1;
    nbadd = 1;
  }
  else if (d2 != 1.) {
    k[0] = d2;
    nbadd = 1;
  }
  return nbadd;
}

static Handle(Law_Linear) mklin(const Handle(Law_Function)& func)
{
  Handle(Law_Linear) fu = Handle(Law_Linear)::DownCast(func);
  if(fu.IsNull()) {
    fu = new Law_Linear();
    Standard_Real d,f;
    func->Bounds(d,f);
    fu->Set(d,func->Value(d),f,func->Value(f));
  }
  return fu;
}

static void sortbounds(const Standard_Integer     nb,
		       Handle(GeomFill_Boundary)* bound,
		       Standard_Boolean*          rev,
		       GeomFill_CornerState*      stat)
{
  // trier les bords (facon bourinos),
  // flaguer ceux a renverser,
  // flaguer les baillements au coins.
  Standard_Integer i,j;
  Handle(GeomFill_Boundary) temp;
  rev[0] = 0;
  gp_Pnt pf,pl;
  gp_Pnt qf,ql;
  for (i = 0; i < nb-1; i++){
    if(!rev[i]) bound[i]->Points(pf,pl);
    else bound[i]->Points(pl,pf);
    for (j = i+1; j <= nb-1; j++){
      bound[j]->Points(qf,ql);
      //  Modified by skv - Fri Jun 18 12:52:54 2004 OCC6129 Begin
      Standard_Real df = qf.Distance(pl);
      Standard_Real dl = ql.Distance(pl);
      if (df<dl) {
	if(df < stat[i+1].Gap()){
	  temp = bound[i+1];
	  bound[i+1] = bound[j];
	  bound[j] = temp;
	  stat[i+1].Gap(df);
	  rev[i+1] = Standard_False;
	}
      } else {
	if(dl < stat[i+1].Gap()){
	  temp = bound[i+1];
	  bound[i+1] = bound[j];
	  bound[j] = temp;
	  stat[i+1].Gap(dl);
	  rev[i+1] = Standard_True;
	}
      }
      //  Modified by skv - Fri Jun 18 12:52:54 2004 OCC6129 End
    }
  }
  if(!rev[nb-1]) bound[nb-1]->Points(pf,pl);
  else bound[nb-1]->Points(pl,pf);
  bound[0]->Points(qf,ql);
  stat[0].Gap(pl.Distance(qf));

  // flaguer les angles entre tangentes au coins et entre les normales au
  // coins pour les bords contraints.
  gp_Pnt pbid;
  gp_Vec tgi, nori, tgn, norn;
  Standard_Real fi, fn, li, ln;
  for (i = 0; i < nb; i++){
    Standard_Integer next = (i+1)%nb;
    if(!rev[i]) bound[i]->Bounds(fi,li);
    else bound[i]->Bounds(li,fi);
    bound[i]->D1(li,pbid,tgi);
    if(rev[i]) tgi.Reverse();
    if(!rev[next]) bound[next]->Bounds(fn,ln);
    else bound[next]->Bounds(ln,fn);
    bound[next]->D1(fn,pbid,tgn);
    if(rev[next]) tgn.Reverse();
    Standard_Real ang = M_PI - tgi.Angle(tgn);
    stat[next].TgtAng(ang);
    if(bound[i]->HasNormals() && bound[next]->HasNormals()){
      stat[next].Constraint();
      nori = bound[i]->Norm(li);
      norn = bound[next]->Norm(fn);
      ang = nori.Angle(norn);
      stat[next].NorAng(ang);
    }
  }
}
static void coonscnd(const Standard_Integer     nb,
		     Handle(GeomFill_Boundary)* bound,
		     Standard_Boolean*          rev,
		     GeomFill_CornerState*      stat,
//		     Handle(GeomFill_TgtField)* tga,
		     Handle(GeomFill_TgtField)* ,
		     Standard_Real*             mintg)
{
  Standard_Real fact_normalization = 100.;  
  Standard_Integer i;
  // Pour chaque coin contraint, on controle les bounds adjascents.
  for(i = 0; i < nb; i++){
    if(stat[i].HasConstraint()){
      Standard_Integer ip = (i-1+nb)%nb;
      Standard_Real tolang = Min(bound[ip]->Tolang(),bound[i]->Tolang());
      Standard_Real an = stat[i].NorAng();
      Standard_Boolean twist = Standard_False;
      if(an >= 0.5*M_PI) { twist = Standard_True; an = M_PI-an; }
      if(an > tolang) stat[i].DoKill(0.);
      else{
	Standard_Real fact = 0.5*27./4;
	tolang *= (Min(mintg[ip],mintg[i])*fact*fact_normalization);
	gp_Vec tgp, dnorp, tgi, dnori, vbid;
	gp_Pnt pbid;
	Standard_Real fp,lp,fi,li;
	if(!rev[ip]) bound[ip]->Bounds(fp,lp);
	else bound[ip]->Bounds(lp,fp);
	bound[ip]->D1(lp,pbid,tgp);
	bound[ip]->D1Norm(lp,vbid,dnorp);
	if(!rev[i]) bound[i]->Bounds(fi,li);
	else bound[i]->Bounds(li,fi);
	bound[i]->D1(fi,pbid,tgi);
	bound[i]->D1Norm(fi,vbid,dnori);
	Standard_Real scal1 = tgp.Dot(dnori);
	Standard_Real scal2 = tgi.Dot(dnorp);
	if(!twist) scal2 *= -1.;
	scal1 = Abs(scal1+scal2);
	if(scal1 > tolang) {
	  Standard_Real killfactor = tolang/scal1;
	  stat[i].DoKill(killfactor);
#ifdef OCCT_DEBUG
	  cout<<"pb coons cnd coin : "<<i<<" fact = "<<killfactor<<endl; 
#endif
	}
      }
    }
  }
}
static void killcorners(const Standard_Integer     nb,
			Handle(GeomFill_Boundary)* bound,
			Standard_Boolean*          rev,
			Standard_Boolean*          nrev,
			GeomFill_CornerState*      stat,
			Handle(GeomFill_TgtField)* tga)
{
  Standard_Integer i;
  // Pour chaque  bound, on  controle l etat  des extremites  et on flingue
  // eventuellement le champ tangent et les derivees du bound.
  for(i = 0; i < nb; i++){
    Standard_Integer inext = (i+1)%nb;
    Standard_Boolean fnul, lnul;
    Standard_Real fscal, lscal;
    if(!nrev[i]){
      fnul = stat[i].IsToKill(fscal);
      lnul = stat[inext].IsToKill(lscal);
    }
    else{
      lnul = stat[i].IsToKill(lscal);
      fnul = stat[inext].IsToKill(fscal);
    }
    if(fnul || lnul){
#ifdef OCCT_DEBUG
      parclock.Start();
#endif
      bound[i]->Reparametrize(0.,1.,fnul,lnul,fscal,lscal,rev[i]);
#ifdef OCCT_DEBUG
      parclock.Stop();
#endif
      if(bound[i]->HasNormals() && tga[i]->IsScalable()) {
	Handle(Law_BSpline) bs = Law::ScaleCub(0.,1.,fnul,lnul,fscal,lscal);
	tga[i]->Scale(bs);
#ifdef DRAW
	if(dodraw) Law_draw1dcurve(bs,gp_Vec2d(1.,0.),1.);
#endif
      }
    }
  }
}

//=======================================================================
//class : GeomFill_ConstrainedFilling_Eval
//purpose: The evaluator for curve approximation
//=======================================================================

class GeomFill_ConstrainedFilling_Eval : public AdvApprox_EvaluatorFunction
{
 public:
  GeomFill_ConstrainedFilling_Eval (GeomFill_ConstrainedFilling& theTool)
    : curfil(theTool) {}
  
  virtual void Evaluate (Standard_Integer *Dimension,
		         Standard_Real     StartEnd[2],
                         Standard_Real    *Parameter,
                         Standard_Integer *DerivativeRequest,
                         Standard_Real    *Result, // [Dimension]
                         Standard_Integer *ErrorCode);
  
 private:
  GeomFill_ConstrainedFilling& curfil;
};

void GeomFill_ConstrainedFilling_Eval::Evaluate (Standard_Integer *,/*Dimension*/
                                                 Standard_Real     /*StartEnd*/[2],
                                                 Standard_Real    *Parameter,
                                                 Standard_Integer *DerivativeRequest,
                                                 Standard_Real    *Result,// [Dimension]
                                                 Standard_Integer *ErrorCode)
{
  *ErrorCode = curfil.Eval(*Parameter, *DerivativeRequest, Result[0]);
}

//=======================================================================
//function : GeomFill_ConstrainedFilling
//purpose  : 
//=======================================================================

GeomFill_ConstrainedFilling::GeomFill_ConstrainedFilling
(const Standard_Integer MaxDeg,
 const Standard_Integer MaxSeg) :
 degmax(MaxDeg),segmax(MaxSeg),appdone(Standard_False)
{
  dom[0] = dom[1] = dom[2] = dom[3] = 1.;
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::Init(const Handle(GeomFill_Boundary)& B1,
				       const Handle(GeomFill_Boundary)& B2,
				       const Handle(GeomFill_Boundary)& B3,
				       const Standard_Boolean           NoCheck)
{
#ifdef OCCT_DEBUG
  totclock.Reset();
  parclock.Reset();
  appclock.Reset();
  cstclock.Reset();
  totclock.Start();
#endif
  Standard_Boolean rev[3];
  rev[0] = rev[1] = rev[2] = Standard_False;
  Handle(GeomFill_Boundary) bound[3];
  bound[0] = B1; bound[1] = B2; bound[2] = B3;
  Standard_Integer i;
  sortbounds(3,bound,rev,stcor);

  // on reoriente.
  rev[2] = !rev[2];
  
  // on reparamettre tout le monde entre 0. et 1.
#ifdef OCCT_DEBUG
  parclock.Start();
#endif
  for (i = 0; i <= 2; i++){
    bound[i]->Reparametrize(0.,1.,0,0,1.,1.,rev[i]);
  }
#ifdef OCCT_DEBUG
  parclock.Stop();
#endif

  // On cree le carreau algorithmique (u,(1-u)) et les champs tangents
  // 1er jus.
  // On cree donc le bord manquant.
  gp_Pnt p1 = bound[1]->Value(1.);
  gp_Pnt p2 = bound[2]->Value(1.);
  gp_Pnt ppp(0.5*(p1.XYZ()+p2.XYZ()));
  Standard_Real t3 = Max(bound[1]->Tol3d(),bound[2]->Tol3d());
  Handle(GeomFill_DegeneratedBound) 
    DB = new GeomFill_DegeneratedBound(ppp,0.,1.,t3,10.);

  ptch = new GeomFill_CoonsAlgPatch(bound[0],bound[1],DB,bound[2]);

  Handle(GeomFill_TgtField) ttgalg[3];
  if(bound[0]->HasNormals()) 
    ttgalg[0] = tgalg[0] = new GeomFill_TgtOnCoons(ptch,0);
  if(bound[1]->HasNormals()) 
    ttgalg[1] = tgalg[1] = new GeomFill_TgtOnCoons(ptch,1);
  if(bound[2]->HasNormals()) 
    ttgalg[2] = tgalg[3] = new GeomFill_TgtOnCoons(ptch,3);

  for (i = 0; i <= 3; i++){ 
    mig[i] = 1.;
    if(!tgalg[i].IsNull()) MinTgte(i); 
  }

  if(!NoCheck){
    // On  verifie enfin les conditions  de compatibilites sur les derivees
    // aux coins maintenant qu on a quelque chose a quoi les comparer.
    Standard_Boolean nrev[3];
    nrev[0] = nrev[1] = 0;
    nrev[2] = 1;
    mig[2] = mig[3];
    coonscnd(3,bound,nrev,stcor,ttgalg,mig);
    killcorners(3,bound,rev,nrev,stcor,ttgalg);
  }
  // on remet les coins en place (on duplique la pointe).
  stcor[3] = stcor[2];
  
  for (i = 0; i <= 3; i++){ 
    mig[i] = 1.;
    if(!tgalg[i].IsNull()) {
      if(!CheckTgte(i)) {
	Handle(Law_Function) fu1,fu2;
	ptch->Func(fu1,fu2);
	fu1 = Law::MixBnd(Handle(Law_Linear)::DownCast (fu1));
	fu2 = Law::MixBnd(Handle(Law_Linear)::DownCast (fu2));
	ptch->Func(fu1,fu2);
	break;
      } 
    }
  }

  Build();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::Init(const Handle(GeomFill_Boundary)& B1,
				       const Handle(GeomFill_Boundary)& B2,
				       const Handle(GeomFill_Boundary)& B3,
				       const Handle(GeomFill_Boundary)& B4,
				       const Standard_Boolean           NoCheck)
{
#ifdef OCCT_DEBUG
  totclock.Reset();
  parclock.Reset();
  appclock.Reset();
  cstclock.Reset();
  totclock.Start();
#endif
  Standard_Boolean rev[4];
  rev[0] = rev[1] = rev[2] = rev[3] = Standard_False;
  Handle(GeomFill_Boundary) bound[4];
  bound[0] = B1; bound[1] = B2; bound[2] = B3; bound[3] = B4;
  Standard_Integer i;
  sortbounds(4,bound,rev,stcor);

  // on reoriente.
  rev[2] = !rev[2];
  rev[3] = !rev[3];
  
  // on reparamettre tout le monde entre 0. et 1.
#ifdef OCCT_DEBUG
  parclock.Start();
#endif
  for (i = 0; i <= 3; i++){
    bound[i]->Reparametrize(0.,1.,0,0,1.,1.,rev[i]);
  }
#ifdef OCCT_DEBUG
  parclock.Stop();
#endif

  // On cree le carreau algorithmique (u,(1-u)) et les champs tangents
  // 1er jus.
  ptch = new GeomFill_CoonsAlgPatch(bound[0],bound[1],bound[2],bound[3]);
  for (i = 0; i <= 3; i++){
    if(bound[i]->HasNormals()) tgalg[i] = new GeomFill_TgtOnCoons(ptch,i);
  }
  // on calcule le min de chacun des champs tangents pour l evaluation 
  // des tolerances.
  for (i = 0; i <= 3; i++){ 
    mig[i] = 1.;
    if(!tgalg[i].IsNull()) MinTgte(i); 
  }

  if(!NoCheck){
    // On  verifie enfin les conditions  de compatibilites sur les derivees
    // aux coins maintenant qu on a quelque chose a quoi les comparer.
    Standard_Boolean nrev[4];
    nrev[0] = nrev[1] = 0;
    nrev[2] = nrev[3] = 1;
    coonscnd(4,bound,nrev,stcor,tgalg,mig);
    killcorners(4,bound,rev,nrev,stcor,tgalg);
  }
  // On verifie les champs tangents ne changent pas de direction.
  for (i = 0; i <= 3; i++){ 
    mig[i] = 1.;
    if(!tgalg[i].IsNull()) {
      if(!CheckTgte(i)) {
	Handle(Law_Function) fu1,fu2;
	ptch->Func(fu1,fu2);
	Handle(Law_Function) ffu1 = Law::MixBnd(Handle(Law_Linear)::DownCast (fu1));
	Handle(Law_Function) ffu2 = Law::MixBnd(Handle(Law_Linear)::DownCast (fu2));
	ptch->SetFunc(ffu1,ffu2);
	break;
      } 
    }
  }

  Build();
}


//=======================================================================
//function : SetDomain
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::SetDomain
(const Standard_Real l, const Handle(GeomFill_BoundWithSurf)& B)
{
  if(B == ptch->Bound(0)) dom[0] = Min(1.,Abs(l));
  else if(B == ptch->Bound(1)) dom[1] = Min(1.,Abs(l));
  else if(B == ptch->Bound(2)) dom[2] = Min(1.,Abs(l));
  else if(B == ptch->Bound(3)) dom[3] = Min(1.,Abs(l));
}


//=======================================================================
//function : ReBuild
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::ReBuild()
{
  if(!appdone) throw Standard_Failure("GeomFill_ConstrainedFilling::ReBuild Approx non faite");
  MatchKnots();
  PerformS0();
  PerformS1();
  PerformSurface();
}


//=======================================================================
//function : Boundary
//purpose  : 
//=======================================================================

Handle(GeomFill_Boundary) GeomFill_ConstrainedFilling::Boundary
(const Standard_Integer I) const 
{
  return ptch->Bound(I);
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) GeomFill_ConstrainedFilling::Surface() const
{
  return surf;
}


//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::Build()
{
  for (Standard_Integer count = 0; count < 2; count++){
    ibound[0] = count; ibound[1] = count+2;
    ctr[0] = ctr[1] = nbd3 = 0;
    Standard_Integer ii ;
    for ( ii = 0; ii < 2; ii++){
      if (ptch->Bound(ibound[ii])->HasNormals()) { 
	ctr[ii] = 2;
      }
      else if (!ptch->Bound(ibound[ii])->IsDegenerated()){
	ctr[ii] = 1;
      }
      nbd3 += ctr[ii];
    }
#ifdef OCCT_DEBUG
    appclock.Start();
#endif
    if(nbd3) PerformApprox();
#ifdef OCCT_DEBUG
    appclock.Stop();
#endif
  }
  appdone = Standard_True;
#ifdef OCCT_DEBUG
  cstclock.Start();
#endif
  MatchKnots();
  PerformS0();
  PerformS1();
  PerformSurface();
#ifdef OCCT_DEBUG
  cstclock.Stop();
  totclock.Stop();
  Standard_Real tottime, apptime, partime, csttime;
  totclock.Show(tottime);
  parclock.Show(partime);
  appclock.Show(apptime);
  cstclock.Show(csttime);
  cout<<"temp total : "<<tottime<<" secondes"<<endl;
  cout<<endl;
  cout<<"dont"<<endl;
  cout<<endl;
  cout<<"reparametrage         : "<<partime<<" secondes"<<endl;
  cout<<"approximation         : "<<apptime<<" secondes"<<endl;
  cout<<"construction formelle : "<<csttime<<" secondes"<<endl;
  cout<<endl;
#endif
}


//=======================================================================
//function : PerformApprox
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::PerformApprox()
{
  Standard_Integer ii ;
  Handle(TColStd_HArray1OfReal) tol3d, tol2d, tol1d;
  if(nbd3) tol3d = new TColStd_HArray1OfReal(1,nbd3);
  Standard_Integer i3d = 0;
  for( ii = 0; ii <= 1; ii++){
    if (ctr[ii]) {tol3d->SetValue((++i3d),ptch->Bound(ibound[ii])->Tol3d());}
    if(ctr[ii] == 2){
      tol3d->SetValue(++i3d,0.5* mig[ibound[ii]] * ptch->Bound(ibound[ii])->Tolang());
    }
  }
  Standard_Real f,l;
  ptch->Bound(ibound[0])->Bounds(f,l);

  GeomFill_ConstrainedFilling_Eval ev (*this);
  AdvApprox_ApproxAFunction  app(0,
				 0,
				 nbd3,
				 tol1d,
				 tol2d,
				 tol3d,
				 f,
				 l,
				 GeomAbs_C1,
				 degmax,
				 segmax,
				 ev);

  if (app.IsDone() || app.HasResult()){
    Standard_Integer imk = Min(ibound[0],ibound[1]);
    Standard_Integer nbpol = app.NbPoles();
    degree[imk] = app.Degree();
    mults[imk] = app.Multiplicities();
    knots[imk] = app.Knots();
    i3d = 0;
    for(ii = 0; ii <= 1; ii++){
      curvpol[ibound[ii]] = new TColgp_HArray1OfPnt(1,nbpol);
      TColgp_Array1OfPnt& cp = curvpol[ibound[ii]]->ChangeArray1();
      if (ctr[ii]){
	app.Poles(++i3d,cp);
      }
      else{
	gp_Pnt ppp =  ptch->Bound(ibound[ii])->Value(0.5*(f+l));
	for(Standard_Integer ij = 1; ij <= nbpol; ij++){
	  cp(ij) = ppp;
	}
      }
      if(ctr[ii] == 2){
	tgtepol[ibound[ii]] = new TColgp_HArray1OfPnt(1,nbpol);
	app.Poles(++i3d,tgtepol[ibound[ii]]->ChangeArray1());
      }
    }
  }
}


//=======================================================================
//function : MatchKnots
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::MatchKnots()
{
  // on n insere rien si les domaines valent 1.
  Standard_Integer i, j, l;
  Standard_Integer ind[4];
  nm[0] = mults[0]; nm[1] = mults[1];
  nk[0] = knots[0]; nk[1] = knots[1];
  ind[0] = nk[1]->Length(); ind[2] = 1;
  ind[1] = 1; ind[3] = nk[0]->Length();
  ncpol[0] = curvpol[0]; ncpol[1] = curvpol[1]; 
  ncpol[2] = curvpol[2]; ncpol[3] = curvpol[3];
  ntpol[0] = tgtepol[0]; ntpol[1] = tgtepol[1];
  ntpol[2] = tgtepol[2]; ntpol[3] = tgtepol[3];
  Standard_Real kadd[2];
  Standard_Integer madd[2];
  Standard_Real tolk = 1./Max(10,2*knots[1]->Array1().Length());
  Standard_Integer nbadd = inqadd(dom[0],dom[2],kadd,madd,degree[1],tolk);
  if(nbadd){
    TColStd_Array1OfReal addk(kadd[0],1,nbadd);
    TColStd_Array1OfInteger addm(madd[0],1,nbadd);
    Standard_Integer nbnp, nbnk;
    if(BSplCLib::PrepareInsertKnots(degree[1],0,
				    knots[1]->Array1(),
				    mults[1]->Array1(),
				    addk,&addm,nbnp,nbnk,tolk,0)){
      nm[1] = new TColStd_HArray1OfInteger(1,nbnk);
      nk[1] = new TColStd_HArray1OfReal(1,nbnk);
      ncpol[1] = new TColgp_HArray1OfPnt(1,nbnp);
      ncpol[3] = new TColgp_HArray1OfPnt(1,nbnp);
      BSplCLib::InsertKnots(degree[1],0,
			    curvpol[1]->Array1(),BSplCLib::NoWeights(),
			    knots[1]->Array1(),mults[1]->Array1(),
			    addk,&addm,
			    ncpol[1]->ChangeArray1(),BSplCLib::NoWeights(),
			    nk[1]->ChangeArray1(),nm[1]->ChangeArray1(),
			    tolk,0);

      BSplCLib::InsertKnots(degree[1],0,
			    curvpol[3]->Array1(),BSplCLib::NoWeights(),
			    knots[1]->Array1(),mults[1]->Array1(),
			    addk,&addm,
			    ncpol[3]->ChangeArray1(),BSplCLib::NoWeights(),
			    nk[1]->ChangeArray1(),nm[1]->ChangeArray1(),
			    tolk,0);
      if(!tgtepol[1].IsNull()){
	ntpol[1] = new TColgp_HArray1OfPnt(1,nbnp);
	BSplCLib::InsertKnots(degree[1],0,
			      tgtepol[1]->Array1(),BSplCLib::NoWeights(),
			      knots[1]->Array1(),mults[1]->Array1(),
			      addk,&addm,
			      ntpol[1]->ChangeArray1(),BSplCLib::NoWeights(),
			      nk[1]->ChangeArray1(),nm[1]->ChangeArray1(),
			      tolk,0);
      }
      if(!tgtepol[3].IsNull()){
	ntpol[3] = new TColgp_HArray1OfPnt(1,nbnp);
	BSplCLib::InsertKnots(degree[1],0,
			      tgtepol[3]->Array1(),BSplCLib::NoWeights(),
			      knots[1]->Array1(),mults[1]->Array1(),
			      addk,&addm,
			      ntpol[3]->ChangeArray1(),BSplCLib::NoWeights(),
			      nk[1]->ChangeArray1(),nm[1]->ChangeArray1(),
			      tolk,0);
      }
    }
    if(dom[0] != 1.) {
      for(i = 2; i <= nbnk; i++){
	if(Abs(dom[0]-nm[1]->Value(i)) < tolk){
	  ind[0] = i;
	  break;
	}
      }
    } 
    if(dom[2] != 1.) {
      for(i = 1; i < nbnk; i++){
	if(Abs(1.-dom[2]-nm[1]->Value(i)) < tolk){
	  ind[2] = i;
	  break;
	}
      }
    } 
  }
  tolk = 1./Max(10.,2.*knots[0]->Array1().Length());
  nbadd = inqadd(dom[1],dom[3],kadd,madd,degree[0],tolk);
  if(nbadd){
    TColStd_Array1OfReal addk(kadd[0],1,nbadd);
    TColStd_Array1OfInteger addm(madd[0],1,nbadd);
    Standard_Integer nbnp, nbnk;
    if(BSplCLib::PrepareInsertKnots(degree[0],0,
				    knots[0]->Array1(),
				    mults[0]->Array1(),
				    addk,&addm,nbnp,nbnk,tolk,0)){
      nm[0] = new TColStd_HArray1OfInteger(1,nbnk);
      nk[0] = new TColStd_HArray1OfReal(1,nbnk);
      ncpol[0] = new TColgp_HArray1OfPnt(1,nbnp);
      ncpol[2] = new TColgp_HArray1OfPnt(1,nbnp);
      BSplCLib::InsertKnots(degree[0],0,
			    curvpol[0]->Array1(),BSplCLib::NoWeights(),
			    knots[0]->Array1(),mults[0]->Array1(),
			    addk,&addm,
			    ncpol[0]->ChangeArray1(),BSplCLib::NoWeights(),
			    nk[0]->ChangeArray1(),nm[0]->ChangeArray1(),
			    tolk,0);

      BSplCLib::InsertKnots(degree[0],0,
			    curvpol[2]->Array1(),BSplCLib::NoWeights(),
			    knots[0]->Array1(),mults[0]->Array1(),
			    addk,&addm,
			    ncpol[2]->ChangeArray1(),BSplCLib::NoWeights(),
			    nk[0]->ChangeArray1(),nm[0]->ChangeArray1(),
			    tolk,0);
      if(!tgtepol[0].IsNull()){
	ntpol[0] = new TColgp_HArray1OfPnt(1,nbnp);
	BSplCLib::InsertKnots(degree[0],0,
			      tgtepol[0]->Array1(),BSplCLib::NoWeights(),
			      knots[0]->Array1(),mults[0]->Array1(),
			      addk,&addm,
			      ntpol[0]->ChangeArray1(),BSplCLib::NoWeights(),
			      nk[0]->ChangeArray1(),nm[0]->ChangeArray1(),
			      tolk,0);
      }
      if(!tgtepol[2].IsNull()){
	ntpol[2] = new TColgp_HArray1OfPnt(1,nbnp);
	BSplCLib::InsertKnots(degree[0],0,
			      tgtepol[2]->Array1(),BSplCLib::NoWeights(),
			      knots[0]->Array1(),mults[0]->Array1(),
			      addk,&addm,
			      ntpol[2]->ChangeArray1(),BSplCLib::NoWeights(),
			      nk[0]->ChangeArray1(),nm[0]->ChangeArray1(),
			      tolk,0);
      }
    }
    if(dom[1] != 1.) {
      for(i = 2; i <= nbnk; i++){
	if(Abs(dom[1]-nm[0]->Value(i)) < tolk){
	  ind[1] = i;
	  break;
	}
      }
    } 
    if(dom[3] != 1.) {
      for(i = 1; i < nbnk; i++){
	if(Abs(1.-dom[3]-nm[0]->Value(i)) < tolk){
	  ind[3] = i;
	  break;
	}
      }
    } 
  }
  Handle(Law_Linear) fu = mklin(ptch->Func(0));
  ab[0] = Law::MixBnd(degree[1],nk[1]->Array1(),nm[1]->Array1(),fu);
  fu = mklin(ptch->Func(1));
  ab[1] = Law::MixBnd(degree[0],nk[0]->Array1(),nm[0]->Array1(),fu);

  for(i = 0; i<2; i++){
    l = ab[i]->Length();
    ab[i+2] = new TColStd_HArray1OfReal(1,l);
    for(j = 1; j <= l; j++){
      ab[i+2]->SetValue(j,1.-ab[i]->Value(j));
    }
  }
  pq[0] = Law::MixTgt(degree[1],nk[1]->Array1(),nm[1]->Array1(),1,ind[0]);
  pq[2] = Law::MixTgt(degree[1],nk[1]->Array1(),nm[1]->Array1(),0,ind[2]);

  pq[1] = Law::MixTgt(degree[0],nk[0]->Array1(),nm[0]->Array1(),0,ind[1]);
  pq[3] = Law::MixTgt(degree[0],nk[0]->Array1(),nm[0]->Array1(),1,ind[3]);

#ifdef DRAW
  if(dodraw){
    gp_Vec2d tra(0.,0.);
    Standard_Real scal = 1.;
    Law_draw1dcurve(ab[0]->Array1(),nk[1]->Array1(),nm[1]->Array1(),degree[1],tra,scal);
    tra.SetCoord(1.,0.);
    Law_draw1dcurve(ab[1]->Array1(),nk[0]->Array1(),nm[0]->Array1(),degree[0],tra,scal);
    tra.SetCoord(0.,1.);
    Law_draw1dcurve(ab[2]->Array1(),nk[1]->Array1(),nm[1]->Array1(),degree[1],tra,scal);
    tra.SetCoord(1.,1.);
    Law_draw1dcurve(ab[3]->Array1(),nk[0]->Array1(),nm[0]->Array1(),degree[0],tra,scal);
    tra.SetCoord(0.,0.);
    Law_draw1dcurve(pq[0]->Array1(),nk[1]->Array1(),nm[1]->Array1(),degree[1],tra,scal);
    tra.SetCoord(0.,1.);
    Law_draw1dcurve(pq[2]->Array1(),nk[1]->Array1(),nm[1]->Array1(),degree[1],tra,scal);
    tra.SetCoord(1.,0.);
    Law_draw1dcurve(pq[1]->Array1(),nk[0]->Array1(),nm[0]->Array1(),degree[0],tra,scal);
    tra.SetCoord(1.,1.);
    Law_draw1dcurve(pq[3]->Array1(),nk[0]->Array1(),nm[0]->Array1(),degree[0],tra,scal);
  }
#endif
}


//=======================================================================
//function : PerformS0
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::PerformS0()
{
  // On construit les poles de S0 par combinaison des poles des bords,
  // des poles des fonctions ab, des points c selon la formule :
  // S0(i,j) = ab[0](j)*ncpol[0](i) + ab[1](i)*ncpol[1](j) 
  //         + ab[2](j)*ncpol[2](i) + ab[3](i)*ncpol[3](j) 
  //         - ab[3](i)*ab[0](j)*c[0] - ab[0](j)*ab[1](i)*c[1]
  //         - ab[1](i)*ab[2](j)*c[2] - ab[2](j)*ab[3](i)*c[3]

  Standard_Integer i, j;
  Standard_Integer ni = ncpol[0]->Length();
  Standard_Integer nj = ncpol[1]->Length();
  S0 = new TColgp_HArray2OfPnt(1,ni,1,nj);
  TColgp_Array2OfPnt& ss0 = S0->ChangeArray2();
  const gp_XYZ& c0 = ptch->Corner(0).Coord();
  const gp_XYZ& c1 = ptch->Corner(1).Coord();
  const gp_XYZ& c2 = ptch->Corner(2).Coord();
  const gp_XYZ& c3 = ptch->Corner(3).Coord();
  for (i = 1; i <= ni; i++){
    Standard_Real ab1 = ab[1]->Value(i);
    Standard_Real ab3 = ab[3]->Value(i);
    const gp_XYZ& b0 = ncpol[0]->Value(i).Coord();
    const gp_XYZ& b2 = ncpol[2]->Value(i).Coord();
    for (j = 1; j <= nj; j++){
      Standard_Real ab0 = ab[0]->Value(j);
      Standard_Real ab2 = ab[2]->Value(j);
      const gp_XYZ& b1 = ncpol[1]->Value(j).Coord();
      const gp_XYZ& b3 = ncpol[3]->Value(j).Coord();
      gp_XYZ polij = b0.Multiplied(ab0);
      gp_XYZ temp = b1.Multiplied(ab1);
      polij.Add(temp);
      temp = b2.Multiplied(ab2);
      polij.Add(temp);
      temp = b3.Multiplied(ab3);
      polij.Add(temp);
      temp = c0.Multiplied(-ab3*ab0);
      polij.Add(temp);
      temp = c1.Multiplied(-ab0*ab1);
      polij.Add(temp);
      temp = c2.Multiplied(-ab1*ab2);
      polij.Add(temp);
      temp = c3.Multiplied(-ab2*ab3);
      polij.Add(temp);
      ss0(i,j).SetXYZ(polij);
    }
  }
}


//=======================================================================
//function : PerformS1
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::PerformS1()
{
  // on construit en temporaire les poles des champs tangents
  // definis par :
  // tgte[ibound](u) - d/dv (S0(u,vbound)) pour ibound = 0 ou 2
  // tgte[ibound](v) - d/du (S0(ubound,v)) pour ibound = 1 ou 3
  // sur les bords ou tgte est defini.
  gp_XYZ* nt[4];
  const TColgp_Array2OfPnt& ss0 = S0->Array2();
  Standard_Integer l, i, j, k;
  Standard_Integer ni = ss0.ColLength();
  Standard_Integer nj = ss0.RowLength();
  for(i = 0; i <= 3; i++){
    if(ntpol[i].IsNull()) nt[i] = 0;
    else {
      Standard_Real z=0;
      Standard_Integer nbp = ntpol[i]->Length();
      Standard_Integer i1=0,i2=0,j1=0,j2=0;
      Standard_Boolean inci=0;
      nt[i] = new gp_XYZ[nbp];
      switch(i){
      case 0 : 
	z = - degree[1]/(nk[1]->Value(2) - nk[1]->Value(1));
	inci = Standard_True;
	i1 = 1; i2 = 1; j1 = 1; j2 = 2;
	break;
      case 1 : 
	l = nk[0]->Length();
	z = - degree[0]/(nk[0]->Value(l) - nk[0]->Value(l-1));
	inci = Standard_False;
	i1 = ni-1; i2 = ni; j1 = 1; j2 = 1;
	break;
      case 2 : 
	l = nk[1]->Length();
	z = - degree[1]/(nk[1]->Value(l) - nk[1]->Value(l-1));
	inci = Standard_True;
	i1 = 1; i2 = 1; j1 = nj-1; j2 = nj;
	break;
      case 3 : 
	z = - degree[0]/(nk[0]->Value(2) - nk[0]->Value(1));
	inci = Standard_False;
	i1 = 1; i2 = 2; j1 = 1; j2 = 1;
	break;
      }
      for(k = 0; k < nbp; k++){
	nt[i][k] = S0->Value(i1,j1).XYZ();
	nt[i][k].Multiply(-1.);
	nt[i][k].Add(S0->Value(i2,j2).XYZ());
	nt[i][k].Multiply(z);
	nt[i][k].Add(ntpol[i]->Value(k+1).XYZ());
	if(inci) { i1++; i2++; }
	else { j1++; j2++; }
      }
    }
  }
  // on calcul les termes correctifs pour le melange.
  Standard_Real coef0 = degree[0]/(nk[0]->Value(2) - nk[0]->Value(1));
  Standard_Real coef1 = degree[1]/(nk[1]->Value(2) - nk[1]->Value(1));
  gp_XYZ vtemp, vtemp0, vtemp1;
  if(nt[0] && nt[3]){
    vtemp0 = nt[0][0].Multiplied(-1.);
    vtemp0.Add(nt[0][1]);
    vtemp0.Multiply(coef0);
    vtemp1 = nt[3][0].Multiplied(-1.);
    vtemp1.Add(nt[3][1]);
    vtemp1.Multiply(coef1);
    vtemp = vtemp0.Added(vtemp1);
    vtemp.Multiply(0.5);
    v[0].SetXYZ(vtemp);
  }

  Standard_Integer ln0 = nk[0]->Length(), lp0 = ncpol[0]->Length();
  coef0 = degree[0]/(nk[0]->Value(ln0) - nk[0]->Value(ln0 - 1));
  coef1 = degree[1]/(nk[1]->Value(2) - nk[1]->Value(1));
  if(nt[0] && nt[1]){
    vtemp0 = nt[0][lp0 - 2].Multiplied(-1.);
    vtemp0.Add(nt[0][lp0 - 1]);
    vtemp0.Multiply(coef0);
    vtemp1 = nt[1][0].Multiplied(-1.);
    vtemp1.Add(nt[1][1]);
    vtemp1.Multiply(coef1);
    vtemp = vtemp0.Added(vtemp1);
    vtemp.Multiply(0.5);
    v[1].SetXYZ(vtemp);
  }
  ln0 = nk[0]->Length(); lp0 = ncpol[0]->Length();
  Standard_Integer ln1 = nk[1]->Length(), lp1 = ncpol[1]->Length();
  coef0 = degree[0]/(nk[0]->Value(ln0) - nk[0]->Value(ln0 - 1));
  coef1 = degree[1]/(nk[1]->Value(ln1) - nk[1]->Value(ln1 - 1));
  if(nt[1] && nt[2]){
    vtemp0 = nt[2][lp0 - 2].Multiplied(-1.);
    vtemp0.Add(nt[2][lp0 - 1]);
    vtemp0.Multiply(coef0);
    vtemp1 = nt[1][lp1 - 2].Multiplied(-1.);
    vtemp1.Add(nt[1][lp1 - 1]);
    vtemp1.Multiply(coef1);
    vtemp = vtemp0.Added(vtemp1);
    vtemp.Multiply(0.5);
    v[2].SetXYZ(vtemp);
  }
  ln1 = nk[1]->Length(); lp1 = ncpol[1]->Length();
  coef0 = degree[0]/(nk[0]->Value(2) - nk[0]->Value(1));
  coef1 = degree[1]/(nk[1]->Value(ln1) - nk[1]->Value(ln1 - 1));
  if(nt[2] && nt[3]){
    vtemp0 = nt[2][0].Multiplied(-1.);
    vtemp0.Add(nt[2][1]);
    vtemp0.Multiply(coef0);
    vtemp1 = nt[3][lp1 - 2].Multiplied(-1.);
    vtemp1.Add(nt[3][lp1 - 1]);
    vtemp1.Multiply(coef1);
    vtemp = vtemp0.Added(vtemp1);
    vtemp.Multiply(0.5);
    v[3].SetXYZ(vtemp);
  }

  // On construit les poles de S1 par combinaison des poles des 
  // champs tangents, des poles des fonctions pq, des duv au coins
  // selon la formule :
  // S1(i,j) = pq[0](j)*ntpol[0](i) + pq[1](i)*ntpol[1](j) 
  //         + pq[2](j)*ntpol[2](i) + pq[3](i)*ntpol[3](j) 
  //         - pq[3](i)*pq[0](j)*v[0] - pq[0](j)*pq[1](i)*v[1]
  //         - pq[1](i)*pq[2](j)*v[2] - pq[2](j)*pq[3](i)*v[3]
  S1 = new TColgp_HArray2OfPnt(1,ni,1,nj);
  TColgp_Array2OfPnt& ss1 = S1->ChangeArray2();
  const gp_XYZ& v0 = v[0].XYZ();
  const gp_XYZ& v1 = v[1].XYZ();
  const gp_XYZ& v2 = v[2].XYZ();
  const gp_XYZ& v3 = v[3].XYZ();

  for (i = 1; i <= ni; i++){
    Standard_Real pq1=0, pq3=0;
    if(nt[1]) pq1 = -pq[1]->Value(i);
    if(nt[3]) pq3 = pq[3]->Value(i);
    gp_XYZ t0, t2;
    if(nt[0]) t0 = nt[0][i-1];
    if(nt[2]) t2 = nt[2][i-1];
    for (j = 1; j <= nj; j++){
      Standard_Real pq0=0, pq2=0;
      if(nt[0]) pq0 = pq[0]->Value(j);
      if(nt[2]) pq2 = -pq[2]->Value(j);
      gp_XYZ t1, t3;
      if(nt[1]) t1 = nt[1][j-1];
      if(nt[3]) t3 = nt[3][j-1];

      gp_XYZ tpolij(0.,0.,0.), temp;
      if(nt[0]) {
	temp = t0.Multiplied(pq0);
	tpolij.Add(temp);
      }
      if(nt[1]) {
	temp = t1.Multiplied(pq1);
	tpolij.Add(temp);
      }
      if(nt[2]){
	temp = t2.Multiplied(pq2);
	tpolij.Add(temp);
      }
      if(nt[3]){
	temp = t3.Multiplied(pq3);
	tpolij.Add(temp);
      }
      if(nt[3] && nt[0]){
	temp = v0.Multiplied(-pq3*pq0);
	tpolij.Add(temp);
      }
      if(nt[0] && nt[1]){
	temp = v1.Multiplied(-pq0*pq1);
	tpolij.Add(temp);
      }
      if(nt[1] && nt[2]){
	temp = v2.Multiplied(-pq1*pq2);
	tpolij.Add(temp);
      }
      if(nt[2] && nt[3]){
	temp = v3.Multiplied(-pq2*pq3);
	tpolij.Add(temp);
      }
      ss1(i,j).SetXYZ(tpolij);
    }
  }

  // Un petit menage
  for(i = 0; i <= 3; i++){
    if(nt[i]){
      delete[] nt[i];
    }
  }
}


//=======================================================================
//function : PerformSurface
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::PerformSurface()
{
  Standard_Integer ni = S0->ColLength(), nj = S0->RowLength(),i,j;
  TColgp_Array2OfPnt temp(1,ni,1,nj);
  const TColgp_Array2OfPnt& t0 = S0->Array2();
  const TColgp_Array2OfPnt& t1 = S1->Array2();
  for(i = 1; i <= ni; i++){
    for(j = 1; j <= nj; j++){
      temp(i,j).SetXYZ(t0(i,j).XYZ().Added(t1(i,j).XYZ()));
    }
  }
  surf = new Geom_BSplineSurface(temp,
				 nk[0]->Array1(),nk[1]->Array1(),
				 nm[0]->Array1(),nm[1]->Array1(),
				 degree[0],degree[1]);
}

//=======================================================================
//function : CheckTgte
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_ConstrainedFilling::CheckTgte(const Standard_Integer I) 
{
  Handle(GeomFill_Boundary) bou = ptch->Bound(I);
  if(!bou->HasNormals()) return Standard_True;
  // On prend 13 points le long du bord et on verifie que le triedre 
  // forme par la tangente a la courbe la normale et la tangente du
  // peigne ne change pas d orientation.
  Standard_Real ll = 1./12., pmix=0;
  for (Standard_Integer iu = 0; iu < 13; iu++){
    Standard_Real uu = iu * ll;
    gp_Pnt pbid;
    gp_Vec tgte;
    bou->D1(uu,pbid,tgte);
    gp_Vec norm = bou->Norm(uu);
    gp_Vec vfield = tgalg[I]->Value(uu);
    if(iu == 0) pmix = vfield.Dot(tgte.Crossed(norm));
    else {
      Standard_Real pmixcur = vfield.Dot(tgte.Crossed(norm));
      if(pmix*pmixcur < 0.) return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : MinTgte
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::MinTgte(const Standard_Integer I) 
{
  if(!ptch->Bound(I)->HasNormals()) return;
  Standard_Real minmag = RealLast();
  Standard_Real ll = 0.02;
  for (Standard_Integer iu = 0; iu <= 30; iu++){
    Standard_Real uu = 0.2 + iu * ll;
    gp_Vec vv = tgalg[I]->Value(uu);
    Standard_Real temp = vv.SquareMagnitude();
    if(temp < minmag) minmag = temp;
  }
  mig[I] = sqrt(minmag);
}

//=======================================================================
//function : Eval
//purpose  : 
//=======================================================================

Standard_Integer GeomFill_ConstrainedFilling::Eval(const Standard_Real W,
						   const Standard_Integer Ord,
						   Standard_Real& Result)const 
{
  Standard_Real* res = &Result;
  Standard_Integer jmp = (3 * ctr[0]);
  switch(Ord){
  case 0 :
    if(ctr[0]){
      ptch->Bound(ibound[0])->Value(W).Coord(res[0],res[1],res[2]);
    }
    if(ctr[0] == 2){
      tgalg[ibound[0]]->Value(W).Coord(res[3],res[4],res[5]);
    }
    if(ctr[1]){
      ptch->Bound(ibound[1])->Value(W).Coord(res[jmp],res[jmp+1],res[jmp+2]);
    }
    if(ctr[1] == 2){
      tgalg[ibound[1]]->Value(W).Coord(res[jmp+3],res[jmp+4],res[jmp+5]);
    }
    break;
  case 1 :
    gp_Pnt pt;
    gp_Vec vt;
    if(ctr[0]){
      ptch->Bound(ibound[0])->D1(W,pt,vt);
      vt.Coord(res[0],res[1],res[2]);
    }
    if(ctr[0] == 2){
      tgalg[ibound[0]]->D1(W).Coord(res[3],res[4],res[5]);
    }
    if(ctr[1]){
      ptch->Bound(ibound[1])->D1(W,pt,vt);
      vt.Coord(res[jmp],res[jmp+1],res[jmp+2]);
    }
    if(ctr[1] == 2){
      tgalg[ibound[1]]->D1(W).Coord(res[jmp+3],res[jmp+4],res[jmp+5]);
    }
    break;
  }
  return 0;
}

//=======================================================================
//function : CheckCoonsAlgPatch
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::CheckCoonsAlgPatch(const Standard_Integer I) 
{
  Standard_Integer nbp = 30;  
  Standard_Real uu=0,duu=0,vv=0,dvv=0,ww=0,dww=0,u1,u2,v1,v2;
  surf->Bounds(u1,u2,v1,v2);
  Standard_Boolean enu = Standard_False;
  switch(I){
  case 0:
    uu = ww = u1; 
    vv = v1; 
    duu = dww = (u2 - u1)/nbp;
    dvv = 0.;
    break;
  case 1:
    vv = ww = v1; 
    uu = u2; 
    dvv = dww = (v2 - v1)/nbp;
    duu = 0.;
    enu = Standard_True;
    break;
  case 2:
    uu = ww = u1; 
    vv = v2; 
    duu = dww = (u2 - u1)/nbp;
    dvv = 0.;
    break;
  case 3:
    vv = ww = v1; 
    uu = u1; 
    dvv = dww = (v2 - v1)/nbp;
    duu = 0.;
    enu = Standard_True;
    break;
  }
  gp_Pnt pbound;
  gp_Vec vptch;
  Handle(GeomFill_Boundary) bou = ptch->Bound(I);
  for (Standard_Integer k = 0; k <= nbp; k++){
    pbound = bou->Value(ww);
    if(enu) vptch = ptch->D1U(uu,vv);
    else vptch = ptch->D1V(uu,vv);
#ifdef DRAW
    gp_Pnt pp;
    Handle(Draw_Segment3D) seg;
    pp = pbound.Translated(vptch);
    seg = new Draw_Segment3D(pbound,pp,Draw_jaune);
    dout << seg;
#endif
    uu += duu;
    vv += dvv;
    ww += dww;
  }
}

//=======================================================================
//function : CheckTgteField
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::CheckTgteField(const Standard_Integer I) 
{
  if(tgalg[I].IsNull()) return;
#ifdef DRAW
  gp_Pnt p1,p2;
#else
  gp_Pnt p1;
#endif
  gp_Vec d1;
  Standard_Boolean caplisse = 0;
  Standard_Real maxang = 0.,pmix=0,pmixcur;
  Handle(GeomFill_Boundary) bou =  ptch->Bound(I);
  for (Standard_Integer iu = 0; iu <= 30; iu++){
    Standard_Real uu = iu/30.;
    bou->D1(uu,p1,d1);
    gp_Vec vtg = tgalg[I]->Value(uu);
    gp_Vec vnor = bou->Norm(uu);
    gp_Vec vcros = d1.Crossed(vnor);
    vcros.Normalize();
    if(iu == 0) pmix = vtg.Dot(vcros);
    else {
      pmixcur = vtg.Dot(vcros);
      if(pmix*pmixcur < 0.) caplisse = 1;
    }
#ifdef DRAW
    Handle(Draw_Segment3D) seg;
    p2 = p1.Translated(vtg);
    seg = new Draw_Segment3D(p1,p2,Draw_blanc);
    dout << seg;
    p2 = p1.Translated(vnor);
    seg = new Draw_Segment3D(p1,p2,Draw_rouge);
    dout << seg;
    p2 = p1.Translated(vcros);
    seg = new Draw_Segment3D(p1,p2,Draw_jaune);
    dout << seg;
#endif
    if(vnor.Magnitude() > 1.e-15 && vtg.Magnitude() > 1.e-15){
      Standard_Real alpha = Abs(M_PI/2.-Abs(vnor.Angle(vtg)));
      if(Abs(alpha) > maxang) maxang = Abs(alpha);
    }
  }
  cout<<"KAlgo angle max sur bord "<<I<<" : "<<maxang<<endl;
  if(caplisse) cout<<"sur bord "<<I<<" le champ tangent change de cote!"<<endl;
}


//=======================================================================
//function : CheckApprox
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::CheckApprox(const Standard_Integer I) 
{
  Standard_Boolean donor = !tgalg[I].IsNull();
  Standard_Integer nbp = 30;  
  Standard_Real maxang = 0., maxdist = 0.;
  gp_Pnt pbound, papp, pbid;
  gp_Vec vbound, vapp;
  Handle(GeomFill_Boundary) bou = ptch->Bound(I);
  for (Standard_Integer iu = 0; iu <= nbp; iu++){
    Standard_Real uu = iu;
    uu /= nbp;
    pbound = bou->Value(uu);
    BSplCLib::D0(uu,0,degree[I%2],0,ncpol[I]->Array1(),BSplCLib::NoWeights(),
		 nk[I%2]->Array1(),&nm[I%2]->Array1(),papp);
    if(donor) {
      BSplCLib::D0(uu,0,degree[I%2],0,ntpol[I]->Array1(),BSplCLib::NoWeights(),
		   nk[I%2]->Array1(),&nm[I%2]->Array1(),pbid);
      vapp.SetXYZ(pbid.XYZ());
      vbound = bou->Norm(uu);
      if(vapp.Magnitude() > 1.e-15 && vbound.Magnitude() > 1.e-15){
	Standard_Real alpha = Abs(M_PI/2.-Abs(vbound.Angle(vapp)));
	if(Abs(alpha) > maxang) maxang = Abs(alpha);
      }
#ifdef DRAW
      Handle(Draw_Segment3D) seg;
      gp_Pnt pp;
      pp = pbound.Translated(vbound);
      seg = new Draw_Segment3D(pbound,pp,Draw_blanc);
      dout << seg;
      pp = papp.Translated(vapp);
      seg = new Draw_Segment3D(papp,pp,Draw_rouge);
      dout << seg;
#endif
    }
    if(papp.Distance(pbound) > maxdist) maxdist = papp.Distance(pbound);
  }
  cout<<"Controle approx/contrainte sur bord "<<I<<" : "<<endl;
  cout<<"Distance max : "<<maxdist<<endl;
  if (donor) {
    maxang = maxang*180./M_PI;
    cout<<"Angle max    : "<<maxang<<" deg"<<endl;
  }
}


//=======================================================================
//function : CheckResult
//purpose  : 
//=======================================================================

void GeomFill_ConstrainedFilling::CheckResult(const Standard_Integer I) 
{
  Standard_Boolean donor = !tgalg[I].IsNull();
  Standard_Real maxang = 0., maxdist = 0.;
  Standard_Real uu=0,duu=0,vv=0,dvv=0,ww=0,dww=0,u1,u2,v1,v2;
  surf->Bounds(u1,u2,v1,v2);
  switch(I){
  case 0:
    uu = ww = u1; 
    vv = v1; 
    duu = dww = (u2 - u1)/30;
    dvv = 0.;
    break;
  case 1:
    vv = ww = v1; 
    uu = u2; 
    dvv = dww = (v2 - v1)/30;
    duu = 0.;
    break;
  case 2:
    uu = ww = u1; 
    vv = v2; 
    duu = dww = (u2 - u1)/30;
    dvv = 0.;
    break;
  case 3:
    vv = ww = v1; 
    uu = u1; 
    dvv = dww = (v2 - v1)/30;
    duu = 0.;
    break;
  }
  gp_Pnt pbound[31],pres[31];
  gp_Vec vbound[31],vres[31];
#ifdef DRAW
  Standard_Real ang[31];
  Standard_Boolean hasang[31];
#endif
  Handle(GeomFill_Boundary) bou = ptch->Bound(I);
  Standard_Integer k ;
  for ( k = 0; k <= 30; k++){
    pbound[k] = bou->Value(ww);
    if(!donor) surf->D0(uu,vv,pres[k]); 
    else{ 
      vbound[k] = bou->Norm(ww);
      gp_Vec V1,V2;
      surf->D1(uu,vv,pres[k],V1,V2);
      vres[k] = V1.Crossed(V2);
      if(vres[k].Magnitude() > 1.e-15 && vbound[k].Magnitude() > 1.e-15){
	Standard_Real alpha = Abs(vres[k].Angle(vbound[k]));
	alpha = Min(alpha,Abs(M_PI-alpha));
	if(alpha > maxang) maxang = alpha;
#ifdef DRAW
	ang[k] = alpha;
	hasang[k] = 1;
#endif
      }
#ifdef DRAW
      else hasang[k] = 0;
#endif
    }
    if(pres[k].Distance(pbound[k]) > maxdist) maxdist = pres[k].Distance(pbound[k]);
    uu += duu;
    vv += dvv;
    ww += dww;
  }
  cout<<"Controle resultat/contrainte sur bord "<<I<<" : "<<endl;
  cout<<"Distance max : "<<maxdist<<endl;
  if (donor) {
    Standard_Real angdeg = maxang*180./M_PI;
    cout<<"Angle max    : "<<angdeg<<" deg"<<endl;
  }
#ifdef DRAW
  Standard_Boolean scale = maxang>1.e-10;
  for (k = 0; k <= 30; k++){
    if(hasang[k]){
      gp_Pnt pp;
      Handle(Draw_Segment3D) seg;
      vbound[k].Normalize();
      if(scale) vbound[k].Multiply(1.+3.*ang[k]/maxang);
      vbound[k].Multiply(drawfac);
      pp = pbound[k].Translated(vbound[k]);
      seg = new Draw_Segment3D(pbound[k],pp,Draw_blanc);
      dout << seg;
      vres[k].Normalize();
      if(scale) vres[k].Multiply(1.+3.*ang[k]/maxang);
      vres[k].Multiply(drawfac);
      pp = pres[k].Translated(vres[k]);
      seg = new Draw_Segment3D(pres[k],pp,Draw_rouge);
      dout << seg;
    }
  }
#endif
}

