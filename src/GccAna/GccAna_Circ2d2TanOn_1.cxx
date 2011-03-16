// File:	GccAna_Circ2d2TanOn_1.cxx
// Created:	Thu Jan  2 15:50:43 1992
// Author:	Remi GILET
//		<reg@topsn3>

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <GccAna_CircLin2dBisec.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_BCirc.hxx>
#include <IntAna2d_Conic.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Creation d un cercle tangent a un Cercle C1 et a une Droite L2.      +
//                        centre sur une Droite.                          +
//  Nous commencons par distinguer les differents cas limites que nous    +
//  allons traiter separement.                                            +
//  Pour le cas general:                                                  +
//  ====================                                                  +
//  Nous calculons les bissectrices a C1 et L2 qui nous donnent           +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents a C1 et L2.                                                  +
//  Nous intersectons ces bissectrices avec la droite OnLine ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1 et L2.  +
//=========================================================================

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc& Qualified1  , 
                        const GccEnt_QualifiedLin&  Qualified2  , 
                        const gp_Lin2d&             OnLine      ,
                        const Standard_Real         Tolerance   ):
   cirsol(1,4)     ,
   qualifier1(1,4) ,
   qualifier2(1,4),
   TheSame1(1,4)   ,
   TheSame2(1,4)   ,
   pnttg1sol(1,4)  ,
   pnttg2sol(1,4)  ,
   pntcen(1,4)     ,
   par1sol(1,4)    ,
   par2sol(1,4)    ,
   pararg1(1,4)    ,
   pararg2(1,4)    ,
   parcen3(1,4)
{

   TheSame1.Init(0);
   TheSame2.Init(0);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
       !(Qualified2.IsEnclosed() ||
	 Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   Standard_Real Tol = Abs(Tolerance);
   Standard_Real Radius=0;
   Standard_Boolean ok = Standard_False;
   gp_Dir2d dirx(1.,0.);
   gp_Circ2d C1 = Qualified1.Qualified();
   gp_Lin2d  L2 = Qualified2.Qualified();
   Standard_Real R1 = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   gp_Pnt2d origin2(L2.Location());
   gp_Dir2d dirL2(L2.Direction());
   gp_Dir2d normL2(-dirL2.Y(),dirL2.X());

//=========================================================================
//   Traitement des cas limites.                                          +
//=========================================================================

   Standard_Real distcl = OnLine.Distance(center1);
   gp_Pnt2d pinterm(center1.XY()+distcl*
		    gp_XY(-OnLine.Direction().Y(),OnLine.Direction().X()));
   if (OnLine.Distance(pinterm) > Tolerance) {
     pinterm = gp_Pnt2d(center1.XY()+distcl*
			gp_XY(-OnLine.Direction().Y(),OnLine.Direction().X()));
   }
   Standard_Real dist2 = L2.Distance(pinterm);
   if (Qualified1.IsEnclosed() || Qualified1.IsOutside()) {
     if (Abs(distcl-R1-dist2) <= Tol) { ok = Standard_True; }
   }
   else if (Qualified1.IsEnclosing()) {
     if (Abs(dist2-distcl-R1) <= Tol) { ok = Standard_True; }
   }
   else if (Qualified1.IsUnqualified()) { ok = Standard_True; }
   else { 
     GccEnt_BadQualifier::Raise();
     return;
   }
   if (ok) {
     if (Qualified2.IsOutside()) {
       gp_Pnt2d pbid(pinterm.XY()+dist2*gp_XY(-dirL2.Y(),dirL2.X()));
       if (L2.Distance(pbid) <= Tol) { WellDone = Standard_True; }
     }
     else if (Qualified2.IsEnclosed()) {
       gp_Pnt2d pbid(pinterm.XY()-dist2*gp_XY(-dirL2.Y(),dirL2.X()));
       if (L2.Distance(pbid) <= Tol) { WellDone = Standard_True; }
     }
     else if (Qualified2.IsUnqualified()) { WellDone = Standard_False; }
     else { 
       GccEnt_BadQualifier::Raise();
       return;
     }
   }
   if (WellDone) {
     NbrSol++;
     cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(pinterm,dirx),dist2);
//   =======================================================
     gp_Dir2d dc1(center1.XY()-pinterm.XY());
     gp_Dir2d dc2(origin2.XY()-pinterm.XY());
     Standard_Real distcc1 = pinterm.Distance(center1);
     if (!Qualified1.IsUnqualified()) { 
       qualifier1(NbrSol) = Qualified1.Qualifier();
     }
     else if (Abs(distcc1+dist2-R1) < Tol) {
       qualifier1(NbrSol) = GccEnt_enclosed;
     }
     else if (Abs(distcc1-R1-dist2) < Tol) {
       qualifier1(NbrSol) = GccEnt_outside;
     }
     else { qualifier1(NbrSol) = GccEnt_enclosing; }
     if (!Qualified2.IsUnqualified()) { 
       qualifier2(NbrSol) = Qualified2.Qualifier();
     }
     else if (dc2.Dot(normL2) > 0.0) {
       qualifier2(NbrSol) = GccEnt_outside;
     }
     else { qualifier2(NbrSol) = GccEnt_enclosed; }

     Standard_Real sign = dc2.Dot(gp_Dir2d(-dirL2.Y(),dirL2.X()));
     dc2 = gp_Dir2d(sign*gp_XY(-dirL2.Y(),dirL2.X()));
     pnttg1sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dist2*dc1.XY());
     pnttg2sol(NbrSol) = gp_Pnt2d(pinterm.XY()+dist2*dc2.XY());
     par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
     pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg2sol(NbrSol));
     pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
     pntcen(NbrSol) = cirsol(NbrSol).Location();
     parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
     return;
   }

//=========================================================================
//   Cas general.                                                         +
//=========================================================================

   GccAna_CircLin2dBisec Bis(C1,L2);
   if (Bis.IsDone()) {
     Standard_Integer nbsolution = Bis.NbSolutions();
     for (Standard_Integer i = 1 ; i <=  nbsolution; i++) {
       Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
       GccInt_IType type = Sol->ArcType();
       IntAna2d_AnaIntersection Intp;
       if (type == GccInt_Lin) {
	 Intp.Perform(OnLine,Sol->Line());
       }
       else if (type == GccInt_Par) {
	 Intp.Perform(OnLine,IntAna2d_Conic(Sol->Parabola()));
       }
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	     gp_Pnt2d Center(Intp.Point(j).Value());
	     Standard_Real dist1 = Center.Distance(center1);
	     dist2 = L2.Distance(Center);
//	     Standard_Integer nbsol = 1;
	     ok = Standard_False;
	     if (Qualified1.IsEnclosed()) {
	       if (dist1-R1 < Tolerance) {
		 if (Abs(Abs(R1-dist1)-dist2)<Tolerance) { ok=Standard_True; }
	       }
	     }
	     else if (Qualified1.IsOutside()) {
	       if (R1-dist1 < Tolerance) {
		 if (Abs(Abs(R1-dist1)-dist2)<Tolerance) { ok=Standard_True; }
	       }
	     }
	     else if (Qualified1.IsEnclosing() || Qualified1.IsUnqualified()) {
	       ok = Standard_True;
	     }
	     if (Qualified2.IsEnclosed() && ok) {
	       if ((((origin2.X()-Center.X())*(-dirL2.Y()))+
		    ((origin2.Y()-Center.Y())*(dirL2.X())))<=0){
		 ok = Standard_True;
		 Radius = dist2;
	       }
	     }
	     else if (Qualified2.IsOutside() && ok) {
	       if ((((origin2.X()-Center.X())*(-dirL2.Y()))+
		    ((origin2.Y()-Center.Y())*(dirL2.X())))>=0){
		 ok = Standard_True;
		 Radius = dist2;
	       }
	     }
	     else if (Qualified2.IsUnqualified() && ok) {
	       ok = Standard_True;
	       Radius = dist2;
	     }
	     if (ok) {
	       NbrSol++;
	       cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius);
//             =======================================================
	       gp_Dir2d dc1(center1.XY()-Center.XY());
	       gp_Dir2d dc2(origin2.XY()-Center.XY());
	       Standard_Real distcc1 = Center.Distance(center1);
	       if (!Qualified1.IsUnqualified()) { 
		 qualifier1(NbrSol) = Qualified1.Qualifier();
	       }
	       else if (Abs(distcc1+Radius-R1) < Tol) {
		 qualifier1(NbrSol) = GccEnt_enclosed;
	       }
	       else if (Abs(distcc1-R1-Radius) < Tol) {
		 qualifier1(NbrSol) = GccEnt_outside;
	       }
	       else { qualifier1(NbrSol) = GccEnt_enclosing; }
	       if (!Qualified2.IsUnqualified()) { 
		 qualifier2(NbrSol) = Qualified2.Qualifier();
	       }
	       else if (dc2.Dot(normL2) > 0.0) {
		 qualifier2(NbrSol) = GccEnt_outside;
	       }
	       else { qualifier2(NbrSol) = GccEnt_enclosed; }
	       if (Center.Distance(center1) <= Tolerance &&
		   Abs(Radius-C1.Radius()) <= Tolerance) {
		 TheSame1(NbrSol) = 1;
		 }
	       else {
		 TheSame1(NbrSol) = 0;
		 pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc1.XY());
		 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
							pnttg1sol(NbrSol));
		 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	       }
	       TheSame2(NbrSol) = 0;
	       Standard_Real sign = dc2.Dot(gp_Dir2d(-dirL2.Y(),dirL2.X()));
	       dc2 = gp_Dir2d(sign*gp_XY(-dirL2.Y(),dirL2.X()));
	       pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius*dc2.XY());
	       par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg2sol(NbrSol));
	       pararg2(NbrSol)=ElCLib::Parameter(L2,pnttg2sol(NbrSol));
	       pntcen(NbrSol) = Center;
	       parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	     }
	   }
	 }
	 WellDone = Standard_True;
       }
     }
   }
 }






