// File GccAna_Lin2d2Tan.cxx, REG 08/07/91

//=========================================================================
//   Droite tangente a deux cercles ou tangente a un cercle et passant    +
//   par un point.                                                        +
//=========================================================================

#include <GccAna_Lin2d2Tan.ixx>

#include <ElCLib.hxx>
#include <gp_XY.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Circ2d.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <GccEnt_BadQualifier.hxx>

//=========================================================================
//   Droite passant par deux points.                                      +
//   ===============================                                      +
//=========================================================================

GccAna_Lin2d2Tan::
   GccAna_Lin2d2Tan (const gp_Pnt2d&             ThePoint1,
                     const gp_Pnt2d&             ThePoint2 ,
                     const Standard_Real         Tolerance ):
   linsol(1,1),
   qualifier1(1,1),
   qualifier2(1,1) ,
   pnttg1sol(1,1),
   pnttg2sol(1,1),
   par1sol(1,1),
   par2sol(1,1),
   pararg1(1,1),
   pararg2(1,1)
{

   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   Standard_Real dist = ThePoint1.Distance(ThePoint2);
   qualifier1(1) = GccEnt_noqualifier;
   qualifier2(1) = GccEnt_noqualifier;
   if (dist >= Tol) {
     gp_Dir2d dir(ThePoint2.X()-ThePoint1.X(),ThePoint2.Y()-ThePoint1.Y());
     linsol(1) = gp_Lin2d(ThePoint1,dir);
//   ===================================
     WellDone = Standard_True;
     NbrSol = 1;
     pnttg1sol(1) = ThePoint1;
     pnttg2sol(1) = ThePoint2;
     par1sol(NbrSol)=ElCLib::Parameter(linsol(NbrSol),pnttg1sol(NbrSol));
     par2sol(NbrSol)=ElCLib::Parameter(linsol(NbrSol),pnttg2sol(NbrSol));
     pararg1(1)   = 0.0;
     pararg2(1)   = 0.0;
   }
 }

//=========================================================================
//   Droite tangente a un cercle passant par un point.                    +
//   =================================================                    +
//   Suivant le qualifieur attache au cercle Qualified1 (C1) on definit   +
//   la direction de la tangente a calculer.                              +
//   Cette tangente aura comme point d attache le point P1 (point de tan- +
//   gence avec le cercle.                                                +
//   Elle fera un angle A (de sinus R1/dist ou -R1/dist) avec la droite   +
//   passant par le centre du cercle et ThePoint.                         +
//=========================================================================

GccAna_Lin2d2Tan::
   GccAna_Lin2d2Tan (const GccEnt_QualifiedCirc& Qualified1,
                     const gp_Pnt2d&             ThePoint  ,
                     const Standard_Real         Tolerance ):
   linsol(1,2),
   qualifier1(1,2),
   qualifier2(1,2),
   pnttg1sol(1,2),
   pnttg2sol(1,2),
   par1sol(1,2),
   par2sol(1,2),
   pararg1(1,2),
   pararg2(1,2) 
{

   Standard_Real Tol = Abs(Tolerance);
   WellDone = Standard_False;
   NbrSol = 0;
   if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	 Qualified1.IsOutside() || Qualified1.IsUnqualified())) {
     GccEnt_BadQualifier::Raise();
     return;
   }
   gp_Circ2d C1 = Qualified1.Qualified();
   Standard_Real R1 = C1.Radius();

   if (Qualified1.IsEnclosed()) { GccEnt_BadQualifier::Raise(); }
// ============================
   else if (Tol < R1-ThePoint.Distance(C1.Location())) { 
     WellDone = Standard_True; 
   }
   else if (Abs(ThePoint.Distance(C1.Location())-R1) <= Tol) {
     gp_Dir2d dir(gp_Vec2d(C1.Location(),ThePoint));
     linsol(1) = gp_Lin2d(ThePoint,gp_Dir2d(Standard_Real(-dir.Y()),
					    Standard_Real(dir.X())));
//   =====================================================================
     qualifier1(1) = Qualified1.Qualifier();
     qualifier2(1) = GccEnt_noqualifier;
     WellDone = Standard_True;
     NbrSol = 1;
     pnttg1sol(1) = ThePoint;
     pnttg2sol(1) = ThePoint;
   }
   else {
     Standard_Real signe = 1;
     Standard_Real dist = ThePoint.Distance(C1.Location());
     Standard_Real d = dist - Sqrt(dist*dist - R1*R1);
     if (Qualified1.IsEnclosing()) {
//   =============================
       signe = 1;
       NbrSol = 1;
     }
     else if (Qualified1.IsOutside()) {
       signe = -1;
       NbrSol = 1;
     }
     else if (Qualified1.IsUnqualified()) {
//   ====================================
       signe = 1;
       NbrSol = 2;
     }
     for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
       gp_Pnt2d P1(C1.Location().Rotated(ThePoint,ASin(signe*R1/dist)));
       gp_Dir2d D1(gp_Vec2d(P1,ThePoint));
       P1=gp_Pnt2d(P1.XY() + d*D1.XY());
       linsol(i) = gp_Lin2d(P1,gp_Dir2d(gp_Vec2d(P1,ThePoint)));
//     ========================================================
       qualifier1(i) = Qualified1.Qualifier();
       qualifier2(i) = GccEnt_noqualifier;
	pnttg1sol(i) = P1;
	pnttg2sol(i) = ThePoint;
	signe = -signe;
      }
      WellDone = Standard_True;
    }
    for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
      par1sol(i)=ElCLib::Parameter(linsol(i),pnttg1sol(i));
      par2sol(i)=ElCLib::Parameter(linsol(i),pnttg2sol(i));
      pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(i));
      pararg2(i) = 0.;
    }
  }

 //=========================================================================
 //   Droite tangente a deux cercles.                                      +
 //   ===============================                                      +
 //   Dans le cas limite (les deux cercles tangents interieurs l un a      +
 //   l autre) on prend la droite orthogonale a la droite reliant les deux +
 //   cercles.                                                             +
 //   Dans les autres cas on fait subir au centre de C1 (Qualified1) ou de +
 //   C2 (Qualified2), suivant que R1 est plus grand ou non que R2, une    +
 //   rotation d angle A avec sinus(A) = (R1+R2)/dist ou                   +
 //                           sinus(A) = (R1-R2)/dist ou                   +
 //                           sinus(A) = (R2-R1)/dist ou                   +
 //                           sinus(A) = (-R1-R2)/dist                     +
 //   Le point ainsi determine est P1 ou P2.                               +
 //   la direction de la droite a calculer est celle de la droite passant  +
 //   par le centre de la rotation (centre de C1 ou de C2) et P1 ou P2.    +
 //   On translate ensuite la droite pour la rendre tangente a C1.         +
 //=========================================================================

 GccAna_Lin2d2Tan::
    GccAna_Lin2d2Tan (const GccEnt_QualifiedCirc& Qualified1,
		      const GccEnt_QualifiedCirc& Qualified2,
		      const Standard_Real         Tolerance ):
    linsol(1,4),
    qualifier1(1,4),
    qualifier2(1,4) ,
    pnttg1sol(1,4),
    pnttg2sol(1,4),
    par1sol(1,4),
    par2sol(1,4),
    pararg1(1,4),
    pararg2(1,4)
{

    Standard_Real Tol = Abs(Tolerance);
    WellDone = Standard_False;
    NbrSol = 0;
    if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	  Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
	!(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	  Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
      GccEnt_BadQualifier::Raise();
      return;
    }
    gp_Circ2d C1 = Qualified1.Qualified();
    gp_Circ2d C2 = Qualified2.Qualified();

    if (Qualified1.IsEnclosed() || Qualified2.IsEnclosed()) {
 // =======================================================
      GccEnt_BadQualifier::Raise();
    }
    else {
      Standard_Real R1 = C1.Radius();
      Standard_Real R2 = C2.Radius();
      gp_Dir2d D1;
      Standard_Integer signe = 1;
      Standard_Real dist = C1.Location().Distance(C2.Location());
      if (Tol < Max(R1,R2)-dist-Min(R1,R2) ) { WellDone = Standard_True; }
      else if (!Qualified1.IsUnqualified() || !Qualified2.IsUnqualified()) {
 //   ====================================================================
	if (Qualified1.IsEnclosing() && Qualified2.IsEnclosing()) {
 //     =========================================================
	  if (Abs(dist+Min(R1,R2)-Max(R1,R2)) <= Tol && dist >= Tol) {
	    if (R1<R2) { D1 = gp_Dir2d(gp_Vec2d(C2.Location(),C1.Location()));}
	    else { D1 = gp_Dir2d(gp_Vec2d(C1.Location(),C2.Location())); }
	    gp_Pnt2d P1(C1.Location().XY()+R1*D1.XY());
	    linsol(1) = gp_Lin2d(P1,gp_Dir2d(-D1.Y(),D1.X()));
 //         =================================================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified2.Qualifier();
	    pnttg1sol(1) = P1;
	    pnttg2sol(1) = P1;
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	  else {
	    gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
					      ASin((R1-R2)/dist)));
	    D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	    P1=gp_Pnt2d((C1.Location().XY()+gp_XY(R1*D1.Y(),-R1*D1.X())));
	    linsol(1) = gp_Lin2d(P1,D1);
 //         ===========================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified1.Qualifier();
	    pnttg1sol(1) = P1;
	    pnttg2sol(1) = gp_Pnt2d(C2.Location().XY()+
				    gp_XY(R2*D1.Y(),-R2*D1.X()));
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	}
	else if ((Qualified1.IsEnclosing() && Qualified2.IsOutside()) ||
 //     ================================================================
		 (Qualified2.IsEnclosing() && Qualified1.IsOutside())) {
 //              =====================================================
	  if (Qualified1.IsEnclosing() && Qualified2.IsOutside()) {
	    signe = 1;
	  }
	  else {
	    signe = -1;
	  }
	  if (R1+R2-dist > Tol) { WellDone = Standard_True; }
	  else if (Abs(dist-R1-R2)<Tol && dist>Tol) {
	    D1 = gp_Dir2d(gp_Vec2d(C1.Location(),C2.Location()));
	    gp_Pnt2d P1(C1.Location().XY()+R1*D1.XY());
	    linsol(1) = gp_Lin2d(P1,gp_Dir2d(-D1.Y(),D1.X()));
//          =================================================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified1.Qualifier();
	    pnttg1sol(1) = P1;
	    pnttg2sol(1) = P1;
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	  else {
	    gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
					      ASin(signe*(R1+R2)/dist)));
	    D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	    P1=gp_Pnt2d(C1.Location().XY()+signe*(gp_XY(R1*D1.Y(),
							-R1*D1.X())));
	    linsol(1) = gp_Lin2d(P1,D1);
 //         ===========================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified1.Qualifier();
	    pnttg1sol(1) = P1;
	    pnttg2sol(1) = gp_Pnt2d(C2.Location().XY()+
				    signe*(gp_XY(-R2*D1.Y(),R2*D1.X())));
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	}
	else if (Qualified1.IsOutside() && Qualified2.IsOutside()) {
 //     =========================================================
	  if (Abs(dist+Min(R1,R2)-Max(R1,R2)) < Tol && dist > Tol) {
	    if (R1<R2) { D1 = gp_Dir2d(gp_Vec2d(C2.Location(),C1.Location()));}
	    else { D1 = gp_Dir2d(gp_Vec2d(C1.Location(),C2.Location())); }
	    linsol(1) = gp_Lin2d(gp_Pnt2d(C1.Location().XY()+R1*D1.XY()),
 //         =============================================================
				 gp_Dir2d(D1.Y(),-D1.X()));
 //                              =========================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified1.Qualifier();
	    pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*D1.XY());
	    pnttg2sol(1) = pnttg1sol(1);
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	  else {
	    gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
					      ASin((R2-R1)/dist)));
	    D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	    P1 = gp_Pnt2d(C1.Location().XY()+gp_XY(-R1*D1.Y(),R1*D1.X()));
	    linsol(1) = gp_Lin2d(P1,D1);
 //         ===========================
	    qualifier1(1) = Qualified1.Qualifier();
	    qualifier2(1) = Qualified1.Qualifier();
	    pnttg1sol(1) = P1;
	    pnttg2sol(1) = gp_Pnt2d(C2.Location().XY()+
				    (gp_XY(-R2*D1.Y(),R2*D1.X())));
	    WellDone = Standard_True;
	    NbrSol = 1;
	  }
	}
	else {
	  if ((Qualified1.IsUnqualified() && Qualified2.IsEnclosing()) ||
 //       ===============================================================
	      (Qualified2.IsUnqualified() && Qualified1.IsEnclosing())) {
 //           =========================================================
	    if (Qualified2.IsUnqualified()) { signe = 1; }
	    else { signe = -1; }
	    if (Abs(dist+Min(R1,R2)-Max(R1,R2)) < Tol && dist > Tol) {
	      if (R1<R2) { D1=gp_Dir2d(gp_Vec2d(C2.Location(),C1.Location()));}
	      else { D1 = gp_Dir2d(gp_Vec2d(C1.Location(),C2.Location())); }
	      linsol(1) = gp_Lin2d(gp_Pnt2d(C1.Location().XY()+R1*D1.XY()),
 //           =============================================================
				   gp_Dir2d(-D1.Y(),D1.X()));
 //                                =========================
	      qualifier1(1) = Qualified1.Qualifier();
	      qualifier2(1) = Qualified1.Qualifier();
	      pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*D1.XY());
	      pnttg2sol(1) = pnttg1sol(1);
	      WellDone = Standard_True;
	      NbrSol = 1;
	    }
	    else {
	      gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
						ASin((R1-R2)/dist)));
	      D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	      P1 = gp_Pnt2d(C1.Location().XY()+gp_XY(R1*D1.Y(),-R1*D1.X()));
	      linsol(1) = gp_Lin2d(P1,D1);
 //           ===========================
	      qualifier1(1) = Qualified1.Qualifier();
	      qualifier2(1) = Qualified1.Qualifier();
	      pnttg1sol(1) = P1;
	      pnttg2sol(1) = gp_Pnt2d(C2.Location().XY()+
				      signe*(gp_XY(R2*D1.Y(),-R2*D1.X())));
	      WellDone = Standard_True;
	      NbrSol = 1;
	      if (Min(R1,R2)+Max(R1,R2)<dist) {
		gp_Pnt2d P2(C2.Location().Rotated(C1.Location(),
						  ASin(signe*(R1+R2)/dist)));
		gp_Dir2d D2(gp_Vec2d(C1.Location(),P2));
		P2=gp_Pnt2d(C1.Location().XY()+
			    signe*(gp_XY(R1*D2.Y(),-R1*D2.X())));
		linsol(2) = gp_Lin2d(P2,D2);
 //             ===========================
		qualifier1(1) = Qualified1.Qualifier();
		qualifier2(1) = Qualified1.Qualifier();
		pnttg1sol(2) = P1;
		pnttg2sol(2) = gp_Pnt2d(C2.Location().XY()+
					(gp_XY(-R2*D2.Y(),R2*D2.X())));
		NbrSol = 2;
	      }
	    }
	  }
	  else if ((Qualified1.IsUnqualified() && Qualified2.IsOutside()) ||
 //       ==================================================================
	      (Qualified2.IsUnqualified() && Qualified1.IsOutside())) {
 //           =======================================================
	    if (Qualified2.IsUnqualified()) { signe = 1; }
	    else { signe = -1; }
	    if (Abs(dist+Min(R1,R2)-Max(R1,R2)) <= Tol && dist >= Tol) {
	      if (R1<R2) { D1=gp_Dir2d(gp_Vec2d(C2.Location(),C1.Location()));}
	      else { D1 = gp_Dir2d(gp_Vec2d(C1.Location(),C2.Location())); }
	      linsol(1) = gp_Lin2d(gp_Pnt2d(C1.Location().XY()+R1*D1.XY()),
 //           =============================================================
				   gp_Dir2d(D1.Y(),-D1.X()));
 //                                =========================
	      qualifier1(1) = Qualified1.Qualifier();
	      qualifier2(1) = Qualified1.Qualifier();
	      pnttg1sol(1) = gp_Pnt2d(C1.Location().XY()+R1*D1.XY());
	      pnttg2sol(1) = pnttg1sol(1);
	      WellDone = Standard_True;
	      NbrSol = 1;
	    }
	    else {
	      gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
						ASin(signe*(R2-R1)/dist)));
	      D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	      P1 = gp_Pnt2d(C1.Location().XY()+gp_XY(-R1*D1.Y(),R1*D1.X()));
	      linsol(1) = gp_Lin2d(P1,D1);
 //           ===========================
	      qualifier1(1) = Qualified1.Qualifier();
	      qualifier2(1) = Qualified1.Qualifier();
	      pnttg1sol(1) = P1;
	      pnttg2sol(1) = gp_Pnt2d(C2.Location().XY()+
				      signe*(gp_XY(-R2*D1.Y(),R2*D1.X())));
	      WellDone = Standard_True;
	      NbrSol = 1;
	      if (Min(R1,R2)+Max(R1,R2)<dist) {
		gp_Pnt2d P2(C2.Location().Rotated(C1.Location(),
						  ASin(signe*(-R2-R1)/dist)));
		gp_Dir2d D2(gp_Vec2d(C1.Location(),P2));
		P2=gp_Pnt2d(C1.Location().XY()+
			    signe*(gp_XY(-R1*D2.Y(),R1*D2.X())));
		linsol(2) = gp_Lin2d(P2,D2);
 //             ===========================
		qualifier1(1) = Qualified1.Qualifier();
		qualifier2(1) = Qualified1.Qualifier();
		pnttg1sol(2) = P2;
		pnttg2sol(2) = gp_Pnt2d(C2.Location().XY()+
					signe*(gp_XY(R2*D2.Y(),-R2*D2.X())));
		NbrSol = 2;
	      }
	    }
	  }
	}
      }
      else {
	for (Standard_Integer i = 1 ; i <= 2 ; i++) {
	  signe = -signe;
	  NbrSol++;
	  gp_Pnt2d P1(C2.Location().Rotated(C1.Location(),
					    ASin(signe*(R2-R1)/dist)));
	  D1 = gp_Dir2d(gp_Vec2d(C1.Location(),P1));
	  P1 = gp_Pnt2d(C1.Location().XY()+signe*gp_XY(-R1*D1.Y(),R1*D1.X()));
	  linsol(NbrSol) = gp_Lin2d(P1,D1);
 //       ===========================
	  qualifier1(NbrSol) = Qualified1.Qualifier();
	  qualifier2(NbrSol) = Qualified1.Qualifier();
	  pnttg1sol(NbrSol) = P1;
	  pnttg2sol(NbrSol) = gp_Pnt2d(C2.Location().XY()+
				       signe*(gp_XY(-R2*D1.Y(),R2*D1.X())));
	  WellDone = Standard_True;
	  if (Min(R1,R2)+Max(R1,R2)<dist) {
	    gp_Pnt2d P2(C2.Location().Rotated(C1.Location(),
					      ASin(signe*(R2+R1)/dist)));
	    gp_Dir2d D2(gp_Vec2d(C1.Location(),P2));
	    P2=gp_Pnt2d(C1.Location().XY()+
			signe*(gp_XY(R1*D2.Y(),-R1*D2.X())));
	    NbrSol++;
	    linsol(NbrSol) = gp_Lin2d(P2,D2);
 //         ================================
	    qualifier1(NbrSol) = Qualified1.Qualifier();
	    qualifier2(NbrSol) = Qualified1.Qualifier();
	    pnttg1sol(NbrSol) = P2;
	    pnttg2sol(NbrSol) = gp_Pnt2d(C2.Location().XY()+
					 signe*(gp_XY(-R2*D2.Y(),R2*D2.X())));
	 }
       }
     }
   }
   for (Standard_Integer i = 1 ; i <= NbrSol ; i++) {
     par1sol(i)=ElCLib::Parameter(linsol(i),pnttg1sol(i));
     par2sol(i)=ElCLib::Parameter(linsol(i),pnttg2sol(i));
     pararg1(i)=ElCLib::Parameter(C1,pnttg1sol(i));
     pararg2(i)=ElCLib::Parameter(C2,pnttg2sol(i));
   }
 }

Standard_Boolean GccAna_Lin2d2Tan::
   IsDone () const { return WellDone; }

Standard_Integer GccAna_Lin2d2Tan::
   NbSolutions () const { return NbrSol; }

gp_Lin2d GccAna_Lin2d2Tan::
   ThisSolution (const Standard_Integer Index) const {

   if (Index > NbrSol || Index <= 0) { Standard_OutOfRange::Raise(); }
   return linsol(Index);
 }

void GccAna_Lin2d2Tan::
  WhichQualifier(const Standard_Integer Index   ,
		       GccEnt_Position& Qualif1 ,
		       GccEnt_Position& Qualif2 ) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     Qualif1 = qualifier1(Index);
     Qualif2 = qualifier2(Index);
   }
}

void GccAna_Lin2d2Tan::
   Tangency1 (const Standard_Integer Index,
              Standard_Real& ParSol,
              Standard_Real& ParArg,
              gp_Pnt2d& PntSol) const {
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par1sol(Index);
     ParArg = pararg1(Index);
     PntSol = gp_Pnt2d(pnttg1sol(Index));
   }
 }

void GccAna_Lin2d2Tan::
   Tangency2 (const Standard_Integer   Index  ,
                    Standard_Real&     ParSol ,
                    Standard_Real&     ParArg ,
                    gp_Pnt2d& PntSol ) const {
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParSol = par2sol(Index);
     ParArg = pararg2(Index);
     PntSol = gp_Pnt2d(pnttg2sol(Index));
   }
 }
