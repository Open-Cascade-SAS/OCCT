//File GccAna_Lin2dTanObl.cxx, REG 08/07/91

//=========================================================================
//   CREATION D UNE DROITE TANGENTE A UN CERCLE OU PASSANT PAR UN POINT   +
//                ET FAISANT UN ANGLE A AVEC UNE DROITE.                  +
//=========================================================================

#include <GccAna_Lin2dTanObl.ixx>

#include <ElCLib.hxx>
#include <StdFail_NotDone.hxx>
#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Circ2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Creation d une droite passant par un point : ThePoint                +
//                         faisant un angle     : TheAngle                +
//                         avec une droite      : TheLine.                +
//   On fait subir a la droite (ThePoint,TheLine.Location()) une rotation +
//   d angle TheAngle ==> D1.                                             +
//   on cree la droite passant par ThePoint de direction D1.              +
//=========================================================================

GccAna_Lin2dTanObl::
   GccAna_Lin2dTanObl (const gp_Pnt2d&          ThePoint  ,
                       const gp_Lin2d&          TheLine   ,
                       const Standard_Real      TheAngle  ):
   linsol(1,1)    ,
   qualifier1(1,1),
   pnttg1sol(1,1) ,
   pntint2sol(1,1),
   par1sol(1,1)   ,
   par2sol(1,1)   ,
   pararg1(1,1)   ,
   pararg2(1,1)   
{

   Standard_Real Cosa = TheLine.Direction().X();
   Standard_Real Sina = TheLine.Direction().Y();
   linsol(1) = gp_Lin2d(ThePoint,
// ==============================
                        gp_Dir2d(Cosa*Cos(TheAngle)-Sina*Sin(TheAngle),
//                      ===============================================
				 Sina*Cos(TheAngle)+Sin(TheAngle)*Cosa));
//                               =======================================
   qualifier1(1) = GccEnt_noqualifier;
   pnttg1sol(1) = ThePoint;
   IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
   if (Intp.IsDone()) {
     if (!Intp.IsEmpty()) {
       for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	 pntint2sol(1) = Intp.Point(i).Value();
       }
     }
     par1sol(1)=ElCLib::Parameter(linsol(1),pnttg1sol(1));
     par2sol(1)=ElCLib::Parameter(linsol(1),pntint2sol(1));
     pararg1(1)=0.;
     pararg2(1)=ElCLib::Parameter(TheLine,pntint2sol(1));
     NbrSol = 1;
     WellDone = Standard_True;
   }
   else { 
     WellDone = Standard_False; 
     NbrSol=0;
   }
 }

//=========================================================================
//   Creation d une droite tangent a un cercle  : Qualified1 (C1)         +
//                         faisant un angle     : TheAngle                +
//                         avec une droite      : TheLine.                +
//   On fait subir a la droite (C1.Location,TheLine.Location()) une       +
//   rotation d angle TheAngle ou -TheAngle ==> D1.                       +
//   on cree la droite passant par C1 de direction D1.                    +
//=========================================================================

GccAna_Lin2dTanObl::
   GccAna_Lin2dTanObl (const GccEnt_QualifiedCirc& Qualified1 ,
                       const gp_Lin2d&             TheLine    ,
                       const Standard_Real         TheAngle   ):
   linsol(1,2)   ,
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
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   Standard_Real Cosa = TheLine.Direction().X();
   Standard_Real Sina = TheLine.Direction().Y();
   if (Qualified1.IsEnclosed()) {
// ============================
     GccEnt_BadQualifier::Raise();
   }
   else {
     gp_Circ2d C1 = Qualified1.Qualified();
     Standard_Real R1 = C1.Radius();
     if (Qualified1.IsEnclosing()) {
//   =============================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
	        Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(xy.Y(),-xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = Qualified1.Qualifier();
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       NbrSol = 1;
       WellDone = Standard_True;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     else if (Qualified1.IsOutside()) {
//   ================================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
		Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(-xy.Y(),xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = Qualified1.Qualifier();
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       WellDone = Standard_True;
       NbrSol = 1;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     else if (Qualified1.IsUnqualified()) {
//   ====================================
       gp_XY xy(Cos(TheAngle)*Cosa-Sin(TheAngle)*Sina,
		Cos(TheAngle)*Sina+Sin(TheAngle)*Cosa);
       pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(xy.Y(),-xy.X()));
       linsol(1) = gp_Lin2d(pnttg1sol(1),gp_Dir2d(xy));
//     ===============================================
       qualifier1(1) = GccEnt_enclosing;
       IntAna2d_AnaIntersection Intp(linsol(1),TheLine);
       WellDone = Standard_True;
       NbrSol=1;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(1) = Intp.Point(i).Value();
	   }
	 }
       }
       pnttg1sol(2) = gp_Pnt2d(C1.Location().XY()+R1*gp_XY(-xy.Y(),xy.X()));
       linsol(2) = gp_Lin2d(pnttg1sol(2),gp_Dir2d(xy));
//     ===============================================
       qualifier1(2) = GccEnt_outside;
       Intp = IntAna2d_AnaIntersection(linsol(1),TheLine);
       NbrSol++;
       WellDone = Standard_True;
       if (Intp.IsDone()) {
	 if (!Intp.IsEmpty()) {
	   for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	     pntint2sol(2) = Intp.Point(i).Value();
	   }
	 }
       }
     }
     for (Standard_Integer index = 1; index <= NbrSol; index++) {
       par1sol(index)=ElCLib::Parameter(linsol(index),pnttg1sol(index));
       pararg1(index)=ElCLib::Parameter(C1,pnttg1sol(index));
       par2sol(index)=ElCLib::Parameter(linsol(index),pntint2sol(index));
       pararg2(index)=ElCLib::Parameter(TheLine,pntint2sol(index));
     }
   }
 }

Standard_Boolean GccAna_Lin2dTanObl::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2dTanObl::
   NbSolutions () const 
{ 
  if (!WellDone) StdFail_NotDone::Raise();
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dTanObl::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) 
    StdFail_NotDone::Raise();
  if (Index <= 0 || Index > NbrSol) 
    Standard_OutOfRange::Raise();

  return linsol(Index);
}

void GccAna_Lin2dTanObl::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     Qualif1 = qualifier1(Index);
   }
}

void GccAna_Lin2dTanObl::
   Tangency1 (const Standard_Integer   Index,
                    Standard_Real&     ParSol,
                    Standard_Real&     ParArg,
                    gp_Pnt2d& PntSol) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = gp_Pnt2d(pnttg1sol(Index));
   }
 }

void GccAna_Lin2dTanObl::
   Intersection2 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = gp_Pnt2d(pntint2sol(Index));
   }
 }


