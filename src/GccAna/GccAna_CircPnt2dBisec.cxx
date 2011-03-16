// File:	GccAna_CircPnt2dBisec.cxx
// Created:	Fri Oct 11 08:19:14 1991
// Author:	Remi GILET
//		<reg@topsn3>

//=========================================================================
//   CREATION DE LA BISSECTICE ENTRE UN CERCLE ET UN POINT.               +
//=========================================================================

#include <GccAna_CircPnt2dBisec.ixx>

#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <GccInt_BHyper.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BElips.hxx>
#include <GccInt_BLine.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <gp.hxx>

//=========================================================================

GccAna_CircPnt2dBisec::
   GccAna_CircPnt2dBisec (const gp_Circ2d& Circle ,
		          const gp_Pnt2d&  Point  ):

   circle(Circle),
   point(Point)  {

//=========================================================================
//  Initialisation des champs :                                           +
//            - circle   (Cercle : premier argument.)                     +
//            - line     (Ligne  : deuxieme argument.)                    +
//            - theposition (Entier indiquant la position de Point par    +
//                           rapport a Circle.)                           +
//            - NbrSol   (Entier indiquant le nombre de solutions.)       +
//            - WellDone (Booleen indiquant le succes ou non de l algo.). +
//=========================================================================

   Standard_Real dist = Circle.Radius()-Point.Distance(Circle.Location());
//   if (Abs(dist) < gp::Resolution())
   if (Abs(dist) < 1.E-10)
     {
       theposition = 0;
       NbrSol = 1;
     }
   else if (dist > 0.0)
     {
       theposition = -1;
       NbrSol = 1;
     }
   else {
     theposition = 1;
     NbrSol = 2;
   }
   WellDone = Standard_True;
 }

//=========================================================================
//  Traitement.                                                           +
//  On recupere les coordonees des origines de la droite (xloc,yloc) et   +
//  du cercle (xcencirc, ycencirc).                                       +
//  On recupere aussi les coordonees dela direction de la droite (xdir,   +
//  ydir) et le rayon du cercle R1.                                       +
//  On regarde de quel cote de la droite se trouve le centre du cercle    +
//  pour orienter la parabole (signe).                                    +
//  On cree l axe de chacune des paraboles (axeparab1, axeparb2), puis    +
//  les deux paraboles (biscirPnt1, biscirPnt1).                          +
//=========================================================================

Handle(GccInt_Bisec) GccAna_CircPnt2dBisec::
   ThisSolution (const Standard_Integer Index) const 
{
  
  if (!WellDone)
    StdFail_NotDone::Raise();
  
  if ((Index <=0) || (Index > NbrSol))
    Standard_OutOfRange::Raise();

  Handle(GccInt_Bisec) bissol;  
  Standard_Real xpoint = point.X();
  Standard_Real ypoint = point.Y();
  Standard_Real xcencir = circle.Location().X();
  Standard_Real ycencir = circle.Location().Y();
  Standard_Real R1 = circle.Radius();
  Standard_Real dist = point.Distance(circle.Location());
  //       if (dist < gp::Resolution())
  if (dist < 1.E-10)
    {
      gp_Circ2d biscirpnt1(gp_Ax2d(point,gp_Dir2d(1.0,0.0)),R1/2.);
      bissol = new GccInt_BCirc(biscirpnt1);
      //       ==========================================================
    }
  else {
    gp_Pnt2d center((xpoint+xcencir)/2.,(ypoint+ycencir)/2.);
    gp_Ax2d majax(center,gp_Dir2d(xpoint-xcencir,ypoint-ycencir));
    
    //=========================================================================
    //   Le point est a l interieur du cercle.                                +
    //=========================================================================
    
    if (theposition == -1) {
      gp_Elips2d biscirpnt(majax,R1/2.,Sqrt(R1*R1-dist*dist)/2.);
      bissol = new GccInt_BElips(biscirpnt);
      //         ===========================================================
    }
    
    //=========================================================================
    //   Le point est sur le cercle.                                          +
    //   Il y a une seule solution : la droite passant par point et le centre +
    //   du cercle.                                                           +
    //=========================================================================
    
    else if (theposition == 0) {
      gp_Dir2d dirsol;
      if (circle.IsDirect()) 
	dirsol=gp_Dir2d(xcencir-xpoint,ycencir-ypoint);
      else dirsol = gp_Dir2d(xpoint-xcencir,ypoint-ycencir);
      gp_Lin2d biscirpnt(point,dirsol);
      bissol = new GccInt_BLine(biscirpnt);
      //         =========================================================
    }
    
    //=========================================================================
    //   Le point est a l exterieur du cercle.                                +
    //   Il y a deux solutions : les deux branches principales de l hyperbole.+
    //=========================================================================
    
    else {
      //	   Standard_Real d1 = sqrt(dist*R1-R1*R1);
      Standard_Real d1 = sqrt(dist*dist-R1*R1)/2.0;
      Standard_Real d2 = R1/2.;
      if (Index == 1) {
	gp_Hypr2d biscirpnt1(majax,d2,d1);
	bissol = new GccInt_BHyper(biscirpnt1);
	//           =========================================
      }
      else {
	gp_Hypr2d biscirpnt1(majax,d2,d1);
	gp_Hypr2d biscirpnt2 = biscirpnt1.OtherBranch();
	bissol = new GccInt_BHyper(biscirpnt2);
	//           =========================================
      }
    }
  }
  return bissol;
}


//=========================================================================

Standard_Boolean GccAna_CircPnt2dBisec::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_CircPnt2dBisec::
   NbSolutions () const { return NbrSol; }


