// File:	GccAna_Circ2dBisec.cxx
// Created:	Mon Oct  7 16:17:54 1991
// Author:	Remi GILET
//		<reg@phobox>


//=========================================================================
//   CREATION DE LA BISSECTICE ENTRE DEUX CERCLES.                        +
//=========================================================================

#include <GccAna_Circ2dBisec.ixx>

#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <GccInt_BParab.hxx>
#include <GccInt_BLine.hxx>
#include <GccInt_BElips.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BHyper.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <StdFail_NotDone.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <Precision.hxx>
//=========================================================================

GccAna_Circ2dBisec::
   GccAna_Circ2dBisec (const gp_Circ2d& Circ1    ,
		       const gp_Circ2d& Circ2    ) {

//=========================================================================
//  Initialisation des champs :                                           +
//            - circle1   (Cercle : premier argument.)                    +
//            - circle2  (Ligne  : deuxieme argument.)                    +
//            - intersection (Entier indiquant la position du plus petit  +
//                            des deux cercles par rapport a l autre.)    +
//            - sameradius   (Booleen indiquant si les deux cercles ont   +
//                            meme rayon ou non.)                         +
//            - NbrSol   (Entier indiquant le nombre de solutions.)       +
//            - WellDone (Booleen indiquant le succes ou non de l algo.). +
//=========================================================================

   WellDone = Standard_False;
   Standard_Real Tol=Precision::Confusion();

   Standard_Real R1 = Circ1.Radius();
   Standard_Real R2 = Circ2.Radius();
   if (Abs(R1-R2) <= Tol) { sameradius = Standard_True; }
   else { sameradius = Standard_False; }
   if (R1 < R2) {
     circle1 = gp_Circ2d(Circ2);
     circle2 = gp_Circ2d(Circ1);
     R1 = circle1.Radius();
     R2 = circle2.Radius();
   }
   else {
     circle1 = gp_Circ2d(Circ1);
     circle2 = gp_Circ2d(Circ2);
   }
   Standard_Real dist = circle2.Location().Distance(circle1.Location());
   if (R1-dist-R2 > Tol) {
     intersection = 0;
     NbrSol = 2;
     WellDone = Standard_True;
   }
   else if (Abs(R1-dist-R2) <= Tol) {
     intersection = 1;
     if (sameradius) {
       NbrSol = 0;
       WellDone = Standard_True;
     }
     else {
       NbrSol = 2;
       WellDone = Standard_True;
     }
   }
   else if ((dist+R2-R1 > Tol) && (R1-dist+R2 > Tol)) {
     intersection = 2;
     if (sameradius) {
       NbrSol = 2;
       WellDone = Standard_True;
     }
     else {
       NbrSol = 3;
       WellDone = Standard_True;
     }
   }
   else if (Abs(R1-dist+R2) <= Tol) {
     intersection = 3;
     if (sameradius) {
       NbrSol = 2;
       WellDone = Standard_True;
     }
     else {
       NbrSol = 3;
       WellDone = Standard_True;
     }
   }
   else {
     intersection = 4;
     if (sameradius) {
       NbrSol = 3;
       WellDone = Standard_True;
     }
     else {
       NbrSol = 4;
       WellDone = Standard_True;
     }
   }
 }

//=========================================================================
//  Traitement.                                                           +
//  On recupere les coordonees des centres des cercles circle1 et circle2 +
//  (xcencir1, ycencir1, xcencir2, ycencir2).                             +
//  On recupere aussi les rayons des deux cercles R1 et R2.               +
//=========================================================================

Handle(GccInt_Bisec) GccAna_Circ2dBisec::
   ThisSolution (const Standard_Integer Index) const {

   Standard_Real Tol = 1.e-14;
   Handle(GccInt_Bisec) bissol;

   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 || Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     Standard_Real xcencir1 = circle1.Location().X();
     Standard_Real ycencir1 = circle1.Location().Y();
     Standard_Real xcencir2 = circle2.Location().X();
     Standard_Real ycencir2 = circle2.Location().Y();
     Standard_Real dist     = circle1.Location().Distance(circle2.Location());

     gp_Pnt2d pcen((xcencir1+xcencir2)/2.0,(ycencir1+ycencir2)/2.0);
     gp_Dir2d dircen,medcen;
     if (dist > Tol) {
       dircen.SetCoord(xcencir2-xcencir1,ycencir2-ycencir1);
       medcen.SetCoord(ycencir2-ycencir1,xcencir1-xcencir2);
     }
     gp_Dir2d dirx(1.0,0.0);
     gp_Ax2d  acenx(pcen,dirx);
     gp_Ax2d  acencen(pcen,dircen);
     
     Standard_Real R1 = circle1.Radius();
     Standard_Real R2 = circle2.Radius();

     if ((NbrSol == 1) && (intersection == 0)) {
       Standard_Real R;
       if (Index == 1) 
	 R = (R1+R2)/2.0;
       else 
	 R = (R1-R2)/2.0;
       gp_Circ2d C(acenx,R);
       bissol = new GccInt_BCirc(C);
//     =============================
     }
     else if ((NbrSol == 2) && (intersection == 1)) {
       if (Index == 1) {
	 gp_Elips2d E(acencen,
		      (R1+R2)/2.0,Sqrt((R1*R1+R2*R2-dist*dist)/4.+R1*R2/2.));
	 bissol = new GccInt_BElips(E);
//       =============================
       }
       else if (Index == 2) {
	 gp_Lin2d L(circle1.Location(),
		    dircen);
	 bissol = new GccInt_BLine(L);
//       =============================
       }
     }
     else if ((NbrSol == 2) && (intersection == 0)) {
       if (Index == 1) {
	 if (Abs(xcencir2-xcencir1)<Tol && Abs(ycencir2-ycencir1)< Tol) {
	   gp_Circ2d C(acenx,(R1+R2)/2.0);
	   bissol = new GccInt_BCirc(C);
//         =============================
	 }
	 else {
	   gp_Elips2d E(acencen,
			(R1+R2)/2.0,Sqrt((R1*R1+R2*R2-dist*dist)/4.+R1*R2/2.));
	   bissol = new GccInt_BElips(E);
//         ==============================
	 }
       }
       else if (Index == 2) {
	 if (Abs(xcencir2-xcencir1)< Tol && Abs(ycencir2-ycencir1)< Tol) {
	   gp_Circ2d C(acencen,(R1-R2)/2.);
	   bissol = new GccInt_BCirc(C);
//         =============================
	 }
	 else {
	   gp_Elips2d E(acencen,
			(R1-R2)/2.0,Sqrt((R1*R1+R2*R2-dist*dist)/4.-R1*R2/2.));
	   bissol = new GccInt_BElips(E);
//         ==============================
	 }
       }
     }
     else if (intersection == 2) {
       if (sameradius) {
	 if (Index == 1) {
	   gp_Lin2d L(pcen,medcen);
	   bissol = new GccInt_BLine(L);
//         =============================
	 }
	 else if (Index == 2) {
	   gp_Elips2d E(acencen,
			R1,Sqrt(R1*R1-dist*dist/4.0));
	   bissol = new GccInt_BElips(E);
//         ==============================
	 }
       }
       else {
	 if (Index == 1) {
	   gp_Hypr2d H1;
	   H1 = gp_Hypr2d(acencen,
			  (R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1);
//         ===============================
	 }
	 else if (Index == 2) {
	   gp_Hypr2d H1(acencen,
			(R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1.OtherBranch());
//         ===============================
	 }
	 else if (Index == 3) {
	   gp_Elips2d E(acencen,
			(R1+R2)/2.0,Sqrt((R1*R1+R2*R2-dist*dist)/4.+R1*R2/2.));
	   bissol = new GccInt_BElips(E);
//         ==============================
	 }
       }
     }
     else if (intersection == 3) {
       if (sameradius) {
	 if (Index == 1) {
	   gp_Lin2d L(pcen, dircen);
	   bissol = new GccInt_BLine(L);
//         =============================
	 }
	 else if (Index == 2) {
	   gp_Lin2d L(pcen, medcen);
	   bissol = new GccInt_BLine(L);
//         =============================
	 }
       }
       else {
	 if (Index == 1) {
	   gp_Lin2d L(pcen, dircen);
	   bissol = new GccInt_BLine(L);
//         =============================
	 }
	 else if (Index == 2) {
	   gp_Hypr2d H1(acencen,
			(R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1);
//         ===============================
	 }
	 else if (Index == 3) {
	   gp_Hypr2d H1(acencen,
			(R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1.OtherBranch());
//         ===============================
	 }
       }
     }
     else if (intersection == 4) {
       if (sameradius) {
	 if (Index == 1) {
	   gp_Lin2d L(pcen,medcen);
	   bissol = new GccInt_BLine(L);
//         =============================
	 }
	 else if (Index == 2) {
	   gp_Hypr2d H1(acencen,R1,Sqrt(dist*dist-4*R1*R1)/2.0);
	   bissol = new GccInt_BHyper(H1);
//         ===============================
	 }
	 else if (Index == 3) {
	   gp_Hypr2d H1(acencen,R1,Sqrt(dist*dist-4*R1*R1)/2.0);
	   bissol = new GccInt_BHyper(H1.OtherBranch());
//         ===============================
	 }
       }
       else {
	 if (Index == 1) {
	   gp_Hypr2d H1(acencen,
			(R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1);
//         ===============================
	 }
	 else if (Index == 2) {
	   gp_Hypr2d H1(acencen,
			(R1-R2)/2.0,Sqrt(dist*dist-(R1-R2)*(R1-R2))/2.0);
	   bissol = new GccInt_BHyper(H1.OtherBranch());
//         ===============================
	 }
	 else if (Index == 3) {
	   gp_Hypr2d H1(acencen,
			(R1+R2)/2.0,Sqrt(dist*dist-(R1+R2)*(R1+R2))/2.0);
	   bissol = new GccInt_BHyper(H1);
//         ===============================
	 }
	 else if (Index == 4) {
	   gp_Hypr2d H1(acencen,
			(R1+R2)/2.0,Sqrt(dist*dist-(R1+R2)*(R1+R2))/2.0);
	   bissol = new GccInt_BHyper(H1.OtherBranch());
//         ===============================
	 }
       }
     }
   }
   return bissol;
 }

//=========================================================================

Standard_Boolean GccAna_Circ2dBisec::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Circ2dBisec::NbSolutions () const 
{
  if (!WellDone) StdFail_NotDone::Raise();

  return NbrSol;
}
