// File GccAna_Lin2dBisec.cxx, REG 08/07/91
// JCT 06/07/98 on se fie a IntAna2d_AnaIntersection pour savoir si
//              les droites sont paralleles (PRO14405)
//=========================================================================
//   CREATION DE LA BISSECTICE ENTRE DEUX DROITES.                        +
//=========================================================================

#include <GccAna_Lin2dBisec.ixx>

#include <ElCLib.hxx>
#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp.hxx>
#include <Standard_OutOfRange.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <StdFail_NotDone.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>

//=========================================================================
//   La premiere bissectrice calculee est la bisectrice interieure, la    +
//   seconde est la bissectrice exterieure.                               +
//   la direction de la premiere bissectrice est telle que son produit    +
//   scalaire avec la direction de Lin1 est toujours positif.             +
//   La seconde bissectrice est tournee dans le sens positif.             +
//=========================================================================

GccAna_Lin2dBisec::
   GccAna_Lin2dBisec (const gp_Lin2d& Lin1,
		      const gp_Lin2d& Lin2):
   linsol(1,2)    ,
   pntint1sol(1,2),
   pntint2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2) {

   WellDone = Standard_False;
   NbrSol = 0;

   IntAna2d_AnaIntersection Intp(Lin1,Lin2);
   if (Intp.IsDone()) {
     
     if (Intp.ParallelElements()) {
       if (Intp.IdenticalElements()) {
	 NbrSol = 1;
	 WellDone = Standard_True;
	 linsol(NbrSol) = gp_Lin2d(Lin1);
       }
       else {
	 // Attention : ne pas utiliser dist = Lin1.Distance(Lin2);
	 // car les droites sont peut-etre concourrentes pour gp_Lin2d
	 // d'ou dist = 0.0 (test sur l'angle trop severe ?)
	 Standard_Real dist = Lin1.Distance(Lin2.Location())/2.0;
	 Standard_Real cross = 
	   gp_Vec2d ( -Lin2.Direction().Y() , Lin2.Direction().X() )
	   .Dot ( gp_Vec2d ( Lin2.Location() , Lin1.Location() ) );
	 if (cross < 0) dist = -dist;
	 NbrSol++;
	 WellDone = Standard_True;
	 linsol(NbrSol) = gp_Lin2d(gp_Pnt2d(Lin2.Location().XY()+
//       ========================================================
		 gp_XY(-Lin2.Direction().Y()*dist,Lin2.Direction().X()*dist)),
//               =============================================================
		 Lin2.Direction());
//               =================
       }
     }
     
     else {
       if (!Intp.IsEmpty()) {
	 for (Standard_Integer i = 1 ; i <= Intp.NbPoints() ; i++) {
	   NbrSol++;
	   linsol(NbrSol) = gp_Lin2d(Intp.Point(i).Value(),
//	   ================================================
		       gp_Dir2d(Lin1.Direction().XY()+Lin2.Direction().XY()));
//	               ======================================================
	   NbrSol++;
	   linsol(NbrSol) = gp_Lin2d(Intp.Point(i).Value(),
//	   ===============================================
		       gp_Dir2d(Lin1.Direction().XY()-Lin2.Direction().XY()));
//	               ======================================================
	   if (Lin1.Angle(Lin2) >= 0.) { linsol(NbrSol).Reverse(); }
	   WellDone = Standard_True;
	 }
       }
     }
   }

   for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
     pntint1sol(i) = linsol(i).Location();
     pntint2sol(i) = pntint1sol(i);
     par1sol(i)=ElCLib::Parameter(linsol(i),pntint1sol(i));
     par2sol(i)=ElCLib::Parameter(linsol(i),pntint2sol(i));
     pararg1(i)=ElCLib::Parameter(Lin1,pntint1sol(i));
     pararg2(i)=ElCLib::Parameter(Lin2,pntint2sol(i));
   }

 }

//=========================================================================

Standard_Boolean GccAna_Lin2dBisec::
   IsDone () const { return WellDone; }


Standard_Integer GccAna_Lin2dBisec::
   NbSolutions () const 
{
  if (!WellDone) 
    StdFail_NotDone::Raise();
  return NbrSol;
}

gp_Lin2d GccAna_Lin2dBisec::
   ThisSolution (const Standard_Integer Index) const 
{
  if (!WellDone) 
    StdFail_NotDone::Raise();
  if (Index <= 0 || Index > NbrSol) 
    Standard_OutOfRange::Raise();
  return linsol(Index);
}

void GccAna_Lin2dBisec::
   Intersection1 (const Standard_Integer Index,
                  Standard_Real& ParSol,
                  Standard_Real& ParArg,
                  gp_Pnt2d& PntSol) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = gp_Pnt2d(pntint1sol(Index));
   }
 }


void GccAna_Lin2dBisec::
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

