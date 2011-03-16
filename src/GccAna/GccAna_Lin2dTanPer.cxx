// File GccAna_Lin2dTanPer.cxx, REG 08/07/91

//=========================================================================
//   CREATION D UNE DROITE TANGENTE A UN CERCLE OU PASSANT PAR UN POINT   +
//                      ET ORTHOGONALE A UNE DROITE.                      +
//=========================================================================

#include <GccAna_Lin2dTanPer.ixx>

#include <ElCLib.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Circ2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>

//=========================================================================
//   Droite passant par un point     : ThePoint et                        +
//          orthogonale a une droite : TheLin.                            +
//   On cree la droite d origine     : ThePoint                           +
//                  et de direction  : TheLin.Direction() tournee de 90   +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const gp_Pnt2d& ThePnt    ,
                       const gp_Lin2d& TheLin    ):
   linsol(1,1),
   qualifier1(1,1) ,
   pnttg1sol(1,1),
   pntint2sol(1,1),
   par1sol(1,1),
   par2sol(1,1),
   pararg1(1,1),
   pararg2(1,1)
{

   linsol(1) = gp_Lin2d(ThePnt,gp_Dir2d(-(TheLin.Direction().Y()),
// ===============================================================
					TheLin.Direction().X()));
//                                      ========================
   pnttg1sol(1) = ThePnt;
   IntAna2d_AnaIntersection Intp(linsol(1),TheLin);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 pntint2sol(1) = Intp.Point(i).Value();
       }
     }
   }
   par1sol(1) = ElCLib::Parameter(linsol(1),pnttg1sol(1));
   par2sol(1) = ElCLib::Parameter(linsol(1),pntint2sol(1));
   pararg1(1) = 0.;
   pararg2(1) = ElCLib::Parameter(TheLin,pntint2sol(1));
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Droite passant par un point     : ThePnt                             +
//       et orthogonale a un cercle  : TheCircle.                         +
//   On cree la droite d origine    : ThePoint                            +
//                  et de direction : (TheCircle.Location(),ThePnt).      +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const gp_Pnt2d&  ThePnt    ,
                       const gp_Circ2d& TheCircle ):
   linsol(1,1),
   qualifier1(1,1) ,
   pnttg1sol(1,1),
   pntint2sol(1,1),
   par1sol(1,1),
   par2sol(1,1),
   pararg1(1,1),
   pararg2(1,1)
{

   linsol(1) = gp_Lin2d(ThePnt,
// ============================
                        gp_Dir2d(TheCircle.Location().XY()-ThePnt.XY()));
//                      ================================================
   pnttg1sol(1) = ThePnt;
   IntAna2d_AnaIntersection Intp(linsol(1),TheCircle);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       Standard_Real maxdist = RealLast();
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 if (Intp.Point(i).Value().Distance(ThePnt) < maxdist) {
	   pntint2sol(1) = Intp.Point(i).Value();
	 }
       }
     }
   }
   par1sol(1) = ElCLib::Parameter(linsol(1),pnttg1sol(1));
   par2sol(1) = ElCLib::Parameter(linsol(1),pntint2sol(1));
   pararg1(1) = 0.;
   pararg2(1) = ElCLib::Parameter(TheCircle,pntint2sol(1));
   NbrSol = 1;
   WellDone = Standard_True;
 }

//=========================================================================
//   Droite tangente a un cercle     : Qualified1 (C1)                    +
//       et orthogonale a une droite : TheLin.                            +
//   On cree la droite d origine    : P1 (sur C1)                         +
//                  et de direction : TheLin.Direction() tournee de 90`   +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const GccEnt_QualifiedCirc& Qualified1,
                       const gp_Lin2d&             TheLin    ):
   linsol(1,2),
   qualifier1(1,2) ,
   pnttg1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2)
{

   WellDone = Standard_False;
   Standard_Integer nbsol = 0;
   Standard_Integer signe = 0;
   NbrSol = 0;
   gp_Circ2d C1 = Qualified1.Qualified();

   if (Qualified1.IsEnclosed()) {
// ============================
     GccEnt_BadQualifier::Raise();
   }
   else if (Qualified1.IsEnclosing()) {
// ==================================
     nbsol = 1;
     signe = -1;
   }
   else if (Qualified1.IsOutside()) {
// ================================
     nbsol = 1;
     signe = 1;
   }
   else {
     nbsol = 2;
     signe = -1;
   }
   gp_XY xy(C1.Radius()*TheLin.Direction().XY());
   for (Standard_Integer j = 1 ; j <= nbsol ; j++) {
     signe = -signe;
     NbrSol++;
     linsol(NbrSol)=gp_Lin2d(gp_Pnt2d((C1.Location().XY()).Added(signe*xy)),
//   =======================================================================
			     gp_Dir2d(-TheLin.Direction().Y(),
//                           =================================
				      TheLin.Direction().X()));
//                                    ========================
     pnttg1sol(NbrSol) = gp_Pnt2d((C1.Location().XY()).Added(signe*xy));
     IntAna2d_AnaIntersection Intp(linsol(NbrSol),TheLin);
     if (Intp.IsDone()) {
       if (!Intp.IsEmpty()) {
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   pntint2sol(NbrSol) = Intp.Point(i).Value();
	 }
       }
     }
     par1sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pnttg1sol(NbrSol));
     par2sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pntint2sol(NbrSol));
     pararg1(NbrSol) = ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     pararg2(NbrSol) = ElCLib::Parameter(TheLin,pntint2sol(NbrSol));
     WellDone = Standard_True;
   }
 }

//=========================================================================
//   Droite tangente a un cercle     : Qualified1 (C1)                    +
//       et orthogonale a un cercle  : TheCircle.                         +
//   On cree la droite d origine    : P1 (sur C1)                         +
//                  et de direction : TheLin.Direction() tournee de 90`   +
//=========================================================================

GccAna_Lin2dTanPer::
   GccAna_Lin2dTanPer (const GccEnt_QualifiedCirc& Qualified1,
                       const gp_Circ2d&            TheCircle ):
   linsol(1,2),
   qualifier1(1,2) ,
   pnttg1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2)
{

   WellDone = Standard_False;
   NbrSol = 0;
   Standard_Integer nbsol = 0;
   Standard_Integer signe = 0;
   gp_Circ2d C1 = Qualified1.Qualified();

   if (Qualified1.IsEnclosed()) {
// ============================
     GccEnt_BadQualifier::Raise();
   }
   else if (Qualified1.IsEnclosing()) {
// ==================================
     nbsol = 1;
     signe = -1;
     qualifier1(1) = GccEnt_enclosing;
   }
   else if (Qualified1.IsOutside()) {
// ================================
     nbsol = 1;
     signe = 1;
     qualifier1(1) = GccEnt_outside;
   }
   else if (Qualified1.IsUnqualified()) {
// ====================================
     nbsol = 2;
     signe = -1;
     qualifier1(1) = GccEnt_enclosing;
     qualifier1(2) = GccEnt_outside;
   }
   for (Standard_Integer j = 1 ; j <= 2 ; j++) {
     NbrSol++;
     signe = -signe;
     gp_Dir2d D1(TheCircle.Location().XY()-C1.Location().XY());
     linsol(NbrSol) = gp_Lin2d(gp_Pnt2d((C1.Location().XY())+
//   ===================================================
		       signe*(D1.XY()*C1.Radius())),gp_Dir2d(-D1.Y(),D1.X()));
//                     ======================================================
     pnttg1sol(NbrSol) = gp_Pnt2d((C1.Location().XY())+
				  signe*(D1.XY()*C1.Radius()));
     IntAna2d_AnaIntersection Intp(linsol(NbrSol),TheCircle);
     if (Intp.IsDone()) {
       if (!Intp.IsEmpty()) {
	 Standard_Real maxdist = RealLast();
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   if (Intp.Point(i).Value().Distance(pnttg1sol(NbrSol)) < maxdist) {
	     pntint2sol(NbrSol) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     par1sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pnttg1sol(NbrSol));
     par2sol(NbrSol) = ElCLib::Parameter(linsol(NbrSol),pntint2sol(NbrSol));
     pararg1(NbrSol) = ElCLib::Parameter(C1,pnttg1sol(NbrSol));
     pararg2(NbrSol) = ElCLib::Parameter(TheCircle,pntint2sol(NbrSol));
     WellDone = Standard_True;
   }
 }

Standard_Boolean GccAna_Lin2dTanPer::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2dTanPer::
   NbSolutions () const 
{
  if (!WellDone) { StdFail_NotDone::Raise(); } 
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dTanPer::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) { StdFail_NotDone::Raise(); } 
  if (Index <= 0 || Index > NbrSol) { Standard_RangeError::Raise(); }
  return linsol(Index);
}

void GccAna_Lin2dTanPer::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
  else {
    Qualif1 = qualifier1(Index);
  }
}

void GccAna_Lin2dTanPer::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& Pnt) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     Pnt    = gp_Pnt2d(pnttg1sol(Index));
   }
 }

void GccAna_Lin2dTanPer::
   Intersection2 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const {
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = gp_Pnt2d(pntint2sol(Index));
   }
 }

