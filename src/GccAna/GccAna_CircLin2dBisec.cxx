// File:	GccAna_CircLin2dBisec.cxx
// Created:	Fri Oct 11 08:19:14 1991
// Author:	Remi GILET
//		<reg@topsn3>

//=========================================================================
//   CREATION DE LA BISSECTICE ENTRE UN CERCLE ET UNE DROITE.             +
//=========================================================================

#include <GccAna_CircLin2dBisec.ixx>

#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <GccInt_BParab.hxx>
#include <GccInt_BLine.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================

GccAna_CircLin2dBisec::
   GccAna_CircLin2dBisec (const gp_Circ2d& Circle ,
		          const gp_Lin2d&  Line   ):

   circle(Circle),
   line(Line)
{

//=========================================================================
//  Initialisation des champs :                                           +
//            - circle   (Le cercle.)                                     +
//            - line     (la droite.)                                     +
//            - NbrSol   (nombre de solution.)                            +
//            - WellDone (Booleen indiquant le succes ou non de l algo.). +
//=========================================================================

   NbrSol = 2;
   WellDone = Standard_True;
 }

//=========================================================================
//  Traitement.                                                           +
//  On recupere les coordonees des origines de la droite (xloc,yloc) et   +
//  du cercle (xcencir, ycencir).                                         +
//  On recupere aussi les coordonees dela direction de la droite (xdir,   +
//  ydir) et le rayon du cercle R1.                                       +
//  On regarde de quel cote de la droite se trouve le centre du cercle    +
//  pour orienter la parabole (signe).                                    +
//  On cree l axe de chacune des paraboles (axeparab1, axeparb2), puis    +
//  les deux paraboles (biscirlin1, biscirlin1).                          +
//=========================================================================

Handle(GccInt_Bisec) GccAna_CircLin2dBisec::
   ThisSolution (const Standard_Integer Index) const 
{
  
  if (!WellDone) StdFail_NotDone::Raise();
  
  if ((Index <=0) || (Index > NbrSol)) Standard_OutOfRange::Raise();
  
  Handle(GccInt_Bisec) bissol;
  Standard_Real xdir = line.Direction().X();
  Standard_Real ydir = line.Direction().Y();
  Standard_Real xloc = line.Location().X();
  Standard_Real yloc = line.Location().Y();
  Standard_Real xcencir = circle.Location().X();
  Standard_Real ycencir = circle.Location().Y();
  Standard_Real R1 = circle.Radius();
  Standard_Real dist = line.Distance(circle.Location());
  if ((Abs(line.Distance(circle.Location())-circle.Radius()) 
       <= gp::Resolution()) && (Index == 1)) {
    gp_Lin2d biscirlin1(circle.Location(),gp_Dir2d(-ydir,xdir));
    bissol = new GccInt_BLine(biscirlin1);
    //       ==========================================================
  }
  else {
    Standard_Integer signe;
    if ((-ydir*(xcencir-xloc)+xdir*(ycencir-yloc)) > 0.0) {
      signe = 1;
    }
    else {
      signe = -1;
    }
    gp_Ax2d axeparab1;
//    gp_Ax2d axeparab2;
    gp_Parab2d biscirlin;
    if (dist != R1) {
      if (Index == 1) {
	axeparab1=gp_Ax2d(gp_Pnt2d(gp_XY(xcencir+signe*ydir*(dist+R1)/2,
					 ycencir-signe*xdir*(dist+R1)/2.)),
			  gp_Dir2d(-signe*ydir,signe*xdir));
	biscirlin = gp_Parab2d(axeparab1,(dist+R1)/2.0);
      }
      else {
	if (dist < R1) {
	  axeparab1=gp_Ax2d(gp_Pnt2d(gp_XY(xcencir+signe*ydir*(dist-R1)/2,
					   ycencir-signe*xdir*(dist-R1)/2.)),
			    gp_Dir2d(signe*ydir,-signe*xdir));
	}
	else {
	  axeparab1=gp_Ax2d(gp_Pnt2d(gp_XY(xcencir+signe*ydir*(dist-R1)/2,
					   ycencir-signe*xdir*(dist-R1)/2.)),
			    gp_Dir2d(-signe*ydir,signe*xdir));
	}
	biscirlin = gp_Parab2d(axeparab1,Abs(dist-R1)/2.0);
      }
      bissol = new GccInt_BParab(biscirlin);
      //         ==========================================================
    }
    else {
      axeparab1 = gp_Ax2d(gp_Pnt2d(gp_XY(xcencir+signe*ydir*(dist+R1)/2.,
					 ycencir-signe*xdir*(dist+R1)/2.)),
			  gp_Dir2d(signe*(-ydir),signe*xdir));
      biscirlin = gp_Parab2d(axeparab1,R1);
      bissol = new GccInt_BParab(biscirlin);
      //         ==========================================================
    }
  }

  return bissol;
}

//=========================================================================

Standard_Boolean GccAna_CircLin2dBisec::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_CircLin2dBisec::
   NbSolutions () const { return NbrSol; }


