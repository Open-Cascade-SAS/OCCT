// File:	GccAna_Circ2d2TanOn_2.cxx
// Created:	Thu Jan  2 15:52:15 1992
// Author:	Remi GILET
//		<reg@topsn3>
// JCT 06/07/98 cas des droites confondues (PRO14405)
// JCT 16/11/98 tri des solutions apres le calcul (PRO16384)

#include <GccAna_Circ2d2TanOn.jxx>

#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <GccAna_Lin2dBisec.hxx>
#include <gp.hxx>
#include <GccEnt_BadQualifier.hxx>

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedLin&  Qualified1 , 
                        const GccEnt_QualifiedLin&  Qualified2 , 
                        const gp_Lin2d&             OnLine     ,
                        const Standard_Real         Tolerance  ):
   cirsol(1,2)     ,
   qualifier1(1,2) ,
   qualifier2(1,2) ,
   TheSame1(1,2)   ,
   TheSame2(1,2)   ,
   pnttg1sol(1,2)  ,
   pnttg2sol(1,2)  ,
   pntcen(1,2)     ,
   par1sol(1,2)    ,
   par2sol(1,2)    ,
   pararg1(1,2)    ,
   pararg2(1,2)    ,
   parcen3(1,2)
{
  // initialisations
  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;

  if (!(Qualified1.IsEnclosed() ||
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() ||
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
  GccEnt_BadQualifier::Raise();
    return;
  }

  gp_Dir2d dirx(1.,0.);
  Standard_Real Tol = Abs(Tolerance);

  // calcul de la (des) bisectrice(s) de L1 et L2
  gp_Lin2d L1(Qualified1.Qualified());
  gp_Lin2d L2(Qualified2.Qualified());
  gp_Pnt2d originL1(L1.Location());
  gp_Pnt2d originL2(L2.Location());
  GccAna_Lin2dBisec Bis(L1,L2);
  
  if (Bis.IsDone()) {
    
    if (Bis.NbSolutions() == 1 || Bis.NbSolutions() == 2) {
      // si 1 bisectrice, L1 et L2 sont paralleles
      // si 2 bisectrices, L1 et L2 sont secantes

      for (Standard_Integer k = 1 ; k <= Bis.NbSolutions() ; k++) {
	IntAna2d_AnaIntersection Intp(Bis.ThisSolution(k),OnLine);
	if (Intp.IsDone()) {
	  WellDone = Standard_True;
	  // pour les cas degeneres, pas de solution acceptable 
	  // (OnLine et bisectrice paralleles strictement ou pas)
	  if (!Intp.IdenticalElements() 
	       && !Intp.ParallelElements() 
	       && !Intp.IsEmpty()) {
	    // au maximum 1 point d'intersection !
	    for (Standard_Integer l = 1 ; l <= Intp.NbPoints() ; l++) {
	      gp_Pnt2d pt(Intp.Point(l).Value());
	      gp_Ax2d axe(pt,dirx);
	      Standard_Real Radius = L1.Distance(pt);
	      if (!L1.Contains(pt,Tol) && Radius<1.0/Tol && NbrSol<2) {
		// solution acceptable : le rayon est correct
		NbrSol++;
		cirsol(NbrSol) = gp_Circ2d(axe,Radius);
	      }
	    }
	  }
	}
      }

    }    

  }
  
  // tri selon les qualifiers des NbrSol solutions acceptables
  
  for (Standard_Integer i=1 ; i <= NbrSol ; i++) {

    gp_Pnt2d pbid(cirsol(i).Location());
    Standard_Real Radius = cirsol(i).Radius();
    Standard_Boolean ok = Standard_False;
    
    // solution Outside ou Enclosed / L1
    gp_Dir2d dc1(originL1.XY()-pbid.XY());
    Standard_Real sign1 = dc1.Dot(gp_Dir2d(-L1.Direction().Y(),
					    L1.Direction().X()));
    if (sign1>0.0) ok = (Qualified1.IsUnqualified() 
			  || Qualified1.IsOutside());
    else ok = (Qualified1.IsUnqualified() 
	        || Qualified1.IsEnclosed());

    // solution Outside ou Enclosed / L2
    gp_Dir2d dc2(originL2.XY()-pbid.XY());
    Standard_Real sign2 = dc2.Dot(gp_Dir2d(-L2.Direction().Y(),
                                            L2.Direction().X()));
    if (sign2>0.0) ok = ok && (Qualified2.IsUnqualified() 
			        || Qualified2.IsOutside());
    else ok = ok &&(Qualified2.IsUnqualified() 
		     || Qualified2.IsEnclosed());

    if (ok) {
      // solution a garder
      dc1 = gp_Dir2d(sign1*gp_XY(-L1.Direction().Y(),
				  L1.Direction().X()));
      pnttg1sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc1.XY());
      if (sign1>0.0) qualifier1(i) = GccEnt_outside;
      else qualifier1(i) = GccEnt_enclosed;
      dc2 = gp_Dir2d(sign2*gp_XY(-L2.Direction().Y(),
				  L2.Direction().X()));
      pnttg2sol(i) = gp_Pnt2d(pbid.XY()+Radius*dc2.XY());
      if (sign2>0.0) qualifier2(i) = GccEnt_outside;
      else qualifier2(i) = GccEnt_enclosed;
      pntcen(i) = pbid;
      par1sol(i)=ElCLib::Parameter(cirsol(i),pnttg1sol(i));
      pararg1(i)=ElCLib::Parameter(L1,pnttg1sol(i));
      par2sol(i)=ElCLib::Parameter(cirsol(i),pnttg2sol(i));
      pararg2(i)=ElCLib::Parameter(L2,pnttg2sol(i));
      parcen3(i)=ElCLib::Parameter(OnLine,pntcen(i));
    }
    else {
      // solution a jeter
      if (i==NbrSol) NbrSol--;
      else {
	for (Standard_Integer k = i+1 ; k <= NbrSol ; k++) {
	  cirsol(k-1) = cirsol(k);
	}
	NbrSol--;
	i--;
      }
    }

  }

}

