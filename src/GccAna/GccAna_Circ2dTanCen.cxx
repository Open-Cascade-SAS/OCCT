// File GccAna_Circ2dTanCen.cxx, REG 08/07/91


//========================================================================
//    Creation d'un cercle tangent a un element et centre en un point    +
//========================================================================

#include <GccAna_Circ2dTanCen.ixx>

#include <ElCLib.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp.hxx>
#include <gp_Dir2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <GccEnt_BadQualifier.hxx>

//========================================================================
//     Creation d'un cercle tangent a un cercle centre en un point.      +
//   - On calcule la distance entre le centre du cercle et le point de   +
//     centre : dist                                                     +
//   - On verifie que cette distance est compatible avec le qualifieur   +
//     du cercle.                                                        +
//       Si oui le rayon de la solution sera :                           +
//          C1.Radius()-dist si le qualifieur est Enclosed.              +
//          C1.Radius()+dist si le qualifieur est Enclosing.             +
//          dist-C1.Radius() si le qualifieur est Outside.               +
//          un melange de ces valeurs si le qualifieur est Unqualified.  +
//========================================================================


GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const GccEnt_QualifiedCirc& Qualified1,
                        const gp_Pnt2d&             Pcenter   ,
			const Standard_Real                  Tolerance ):

//========================================================================
//   Initialisation des champs.                                          +
//========================================================================

   cirsol(1,2)   ,
   qualifier1(1,2) ,
   TheSame1(1,2) ,
   pnttg1sol(1,2),
   par1sol(1,2),
   pararg1(1,2)
{

   NbrSol = 0;
   Standard_Real Radius = 0.0;
   WellDone = Standard_False;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   gp_Dir2d dirx(1.0,0.0);
   Standard_Real Tol = Abs(Tolerance);
   gp_Circ2d C1 = Qualified1.Qualified();
   Standard_Real R1 = C1.Radius();
   gp_Pnt2d center1(C1.Location());
   Standard_Real dist;
   Standard_Integer signe = 0;
   Standard_Integer signe1 = 0;

   if (!Qualified1.IsUnqualified()) {
     dist = Pcenter.Distance(center1);
     if (Qualified1.IsEnclosed()) {
//   ============================
       if (dist-R1 <= Tol) {
	 Radius = Abs(R1-dist);
	 signe = 1;
       }
       else { WellDone = Standard_True; }
     }
     else if (Qualified1.IsEnclosing()) {
//   =================================
       Radius = R1+dist;
       signe = -1;
     }
     else if (Qualified1.IsOutside()) {
//   ===============================
       if (dist < R1-Tol) { WellDone = Standard_True; }
       else {
	 Radius = Abs(R1-dist);
	 signe = -1;
       }
     }
     if (signe != 0) {
       NbrSol++;
       cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),Radius);
//     ========================================================
       qualifier1(NbrSol) = Qualified1.Qualifier();
       if (dist <= gp::Resolution()) { TheSame1(NbrSol) = 1; }
       else {
	 TheSame1(NbrSol) = 0;
	 gp_Dir2d d(Pcenter.X()-center1.X(),Pcenter.Y()-center1.Y());
	 pnttg1sol(NbrSol) = gp_Pnt2d(Pcenter.XY()+signe*Radius*d.XY());
	 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
       }
       WellDone = Standard_True;
     }
   }
   else {
// ====
     dist = Pcenter.Distance(center1);
     if (dist >= gp::Resolution()) {
       signe = 1;
       for (Standard_Integer i = 1; i <= 2 ; i++) {
	 signe = -signe;
	 if (R1-dist <= 0.) {
	   signe1 = -1;
	 }
	 else {
	   signe1 = -signe;
	 }
	 Radius = Abs(R1+signe*dist);
	 NbrSol++;
	 cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),Radius);
//       ========================================================
	 Standard_Real distcc1 = Pcenter.Distance(center1);
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
	 TheSame1(NbrSol) = 0;
	 WellDone = Standard_True;
	 gp_Dir2d d(Pcenter.X()-center1.X(),Pcenter.Y()-center1.Y());
	 pnttg1sol(NbrSol) = gp_Pnt2d(Pcenter.XY()+signe1*Radius*d.XY());
	 par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),pnttg1sol(NbrSol));
	 pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
       }
     }
     else {
       NbrSol++;
       cirsol(NbrSol) = gp_Circ2d(C1);
//     ==============================
       qualifier1(1) = Qualified1.Qualifier();
       TheSame1(NbrSol) = 1;
       WellDone = Standard_True;
     }
   }
 }

//=========================================================================
//   Cercle tangent a une ligne Linetan et centre en un point Pcenter.    +
//   On calcule la distance du point a la ligne ==> Rayon.                +
//   On cree le cercle de centre Pcenter de rayon Rayon.                  +
//=========================================================================

GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const gp_Lin2d&            Linetan    ,
                        const gp_Pnt2d&            Pcenter    ):

//=========================================================================
//   Initialisation des champs.                                           +
//=========================================================================

   cirsol(1,1)   ,
   qualifier1(1,1),
   TheSame1(1,1) ,
   pnttg1sol(1,1),
   par1sol(1,1),
   pararg1(1,1) 
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real rayon = Linetan.Distance(Pcenter);
   cirsol(1) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),rayon);
// ==================================================
   qualifier1(1) = GccEnt_noqualifier;
   TheSame1(1) = 0;
   Standard_Real xloc = Linetan.Location().X();
   Standard_Real yloc = Linetan.Location().Y();
   Standard_Real xdir = Linetan.Direction().X();
   Standard_Real ydir = Linetan.Direction().Y();

   if (gp_Dir2d(xloc-Pcenter.X(),yloc-Pcenter.Y())
               .Dot(gp_Dir2d(-ydir,xdir)) > 0.0) {
     pnttg1sol(1) = gp_Pnt2d(Pcenter.XY()+rayon*gp_XY(-ydir,xdir));
     par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
     pararg1(1)=ElCLib::Parameter(Linetan,pnttg1sol(1));
   }
   else {
     pnttg1sol(1) = gp_Pnt2d(Pcenter.XY()+rayon*gp_XY(ydir,-xdir));
     par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
     pararg1(1)=ElCLib::Parameter(Linetan,pnttg1sol(1));
   }
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Cercle tangent a un point Point1 et centre en un point Pcenter.      +
//   On calcule la distance de Pcenter a Point1 ==> Rayon.                +
//   On cree le cercle de centre Pcenter de rayon Rayon.                  +
//=========================================================================

GccAna_Circ2dTanCen::
   GccAna_Circ2dTanCen (const gp_Pnt2d&            Point1     ,
                        const gp_Pnt2d&            Pcenter    ):

//=========================================================================
//   Initialisation des champs.                                           +
//=========================================================================

   cirsol(1,1)   ,
   qualifier1(1,1),
   TheSame1(1,1) ,
   pnttg1sol(1,1),
   par1sol(1,1)  ,
   pararg1(1,1)  
{

   gp_Dir2d dirx(1.0,0.0);
   Standard_Real rayon = Point1.Distance(Pcenter);
   cirsol(1) = gp_Circ2d(gp_Ax2d(Pcenter,dirx),rayon);
// =================================================
   qualifier1(1) = GccEnt_noqualifier;
   TheSame1(1) = 0;
   pnttg1sol(1) = Point1;
   par1sol(1)=ElCLib::Parameter(cirsol(1),pnttg1sol(1));
   pararg1(1) = 0.0;
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================

Standard_Boolean GccAna_Circ2dTanCen::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Circ2dTanCen::
   NbSolutions () const { return NbrSol; }

gp_Circ2d GccAna_Circ2dTanCen::
   ThisSolution (const Standard_Integer Index) const 
{
  if (Index > NbrSol || Index <= 0) { Standard_OutOfRange::Raise(); }
  return cirsol(Index);
}

void GccAna_Circ2dTanCen::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     Qualif1 = qualifier1(Index);
   }
}

void GccAna_Circ2dTanCen::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     if (TheSame1(Index) == 0) {
       PntSol = gp_Pnt2d(pnttg1sol(Index));
       ParSol = par1sol(Index);
       ParArg = pararg1(Index);
     }
     else { StdFail_NotDone::Raise(); }
   }
 }

Standard_Boolean GccAna_Circ2dTanCen::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) 
    StdFail_NotDone::Raise();
  if (Index <= 0 ||Index > NbrSol) 
    Standard_OutOfRange::Raise();
 
  if (TheSame1(Index) == 0) 
    return Standard_False;

  return Standard_True;
}
