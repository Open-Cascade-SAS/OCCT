// File GccAna_Circ2d2TanOn.cxx, REG 08/07/91

#include <GccAna_Circ2d2TanOn.ixx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <Standard_OutOfRange.hxx>
#include <ElCLib.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2d.hxx>
#include <GccInt_IType.hxx>
#include <GccInt_BCirc.hxx>
#include <GccInt_BLine.hxx>
#include <IntAna2d_Conic.hxx>
#include <StdFail_NotDone.hxx>
#include <GccEnt_BadQualifier.hxx>
#include <GccAna_Circ2dBisec.hxx>

//=========================================================================
//  Cercles tangents a deux cercles C1 et C2 et centres sur une droite.   +
//  Nous commencons par distinguer les differents cas limites que nous    +
//  allons traiter separement.                                            +
//  Pour le cas general:                                                  +
//  ====================                                                  +
//  Nous calculons les bissectrices aux deux cercles qui nous donnent     +
//  l ensemble des lieux possibles des centres de tous les cercles        +
//  tangents a C1 et C2.                                                  +
//  Nous intersectons ces bissectrices avec la droite OnLine ce qui nous  +
//  donne les points parmis lesquels nous allons choisir les solutions.   +
//  Les choix s effectuent a partir des Qualifieurs qualifiant C1 et C2.  +
//=========================================================================

GccAna_Circ2d2TanOn::
   GccAna_Circ2d2TanOn (const GccEnt_QualifiedCirc&  Qualified1 , 
                        const GccEnt_QualifiedCirc&  Qualified2 , 
                        const gp_Lin2d&              OnLine     ,
                        const Standard_Real          Tolerance  ):
   cirsol(1,4)   ,
   qualifier1(1,4),
   qualifier2(1,4) ,
   TheSame1(1,4) ,
   TheSame2(1,4) ,
   pnttg1sol(1,4),
   pnttg2sol(1,4),
   pntcen(1,4)   ,
   par1sol(1,4)  ,
   par2sol(1,4)  ,
   pararg1(1,4)  ,
   pararg2(1,4)  ,
   parcen3(1,4)  
{

  TheSame1.Init(0);
  TheSame2.Init(0);
  WellDone = Standard_False;
  NbrSol = 0;
  Standard_Integer nbsol = 0;
  Standard_Real Tol = Abs(Tolerance);
  if (!(Qualified1.IsEnclosed() || Qualified1.IsEnclosing() || 
	Qualified1.IsOutside() || Qualified1.IsUnqualified()) ||
      !(Qualified2.IsEnclosed() || Qualified2.IsEnclosing() || 
	Qualified2.IsOutside() || Qualified2.IsUnqualified())) {
   GccEnt_BadQualifier::Raise();
     return;
   }
  gp_Circ2d C1 = Qualified1.Qualified();
  gp_Circ2d C2 = Qualified2.Qualified();
  Standard_Real R1 = C1.Radius();
  Standard_Real R2 = C2.Radius();
  gp_Dir2d dirx(1.,0.);
  gp_Pnt2d center1(C1.Location());
  gp_Pnt2d center2(C2.Location());
  TColStd_Array1OfReal Radius(1,2);
#ifdef DEB
  Standard_Real distance = center1.Distance(center2);
#else
  center1.Distance(center2);
#endif
  Standard_Real dist1 = OnLine.Distance(center1);
  Standard_Real dist2 = OnLine.Distance(center2);
  Standard_Real d1 = dist1+R1;
  Standard_Real d2 = dist2+R2;
  Standard_Real d3 = dist1-R1;
  Standard_Real d4 = dist2-R2;

//=========================================================================
//  Traitement des cas limites.                                           +
//=========================================================================
   
  if (Abs(d3-d4)<Tol && 
      (Qualified1.IsEnclosed() || Qualified1.IsOutside() ||
       Qualified1.IsUnqualified()) && 
      (Qualified2.IsEnclosed() || Qualified2.IsOutside() ||
       Qualified2.IsUnqualified())) {
    nbsol++;
    Radius(nbsol) = Abs(d3);
    WellDone = Standard_True;
  }
  if (Abs(d1-d2)<Tol &&
      (Qualified1.IsEnclosing() || Qualified1.IsUnqualified()) &&
      (Qualified2.IsEnclosing() || Qualified2.IsUnqualified())){
    nbsol++;
    Radius(nbsol) = Abs(d1);
    WellDone = Standard_True;
  }
  if (Abs(d1-d4)<Tol &&
      (Qualified1.IsEnclosing() || Qualified1.IsUnqualified()) &&
      (Qualified2.IsEnclosed() || Qualified2.IsOutside() ||
       Qualified2.IsUnqualified())){
    nbsol++;
    Radius(nbsol) = Abs(d1);
    WellDone = Standard_True;
  }
  if (Abs(d3-d2)<Tol &&
      (Qualified2.IsEnclosing() || Qualified2.IsUnqualified()) &&
      (Qualified1.IsEnclosed() || Qualified1.IsOutside() ||
       Qualified1.IsUnqualified())){
    nbsol++;
    Radius(nbsol) = Abs(d3);
    WellDone = Standard_True;
  }
  gp_Lin2d L(center1,gp_Dir2d(center2.XY()-center1.XY()));
  IntAna2d_AnaIntersection Intp(OnLine,L);
  if (Intp.IsDone()) {
    if (!Intp.IsEmpty()){
      for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	gp_Pnt2d Center(Intp.Point(j).Value());
	for (Standard_Integer i = 1 ; i <= nbsol ; i++) {
	  NbrSol++;
	  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(i));
//        ==========================================================
	  WellDone = Standard_True;
	  Standard_Real distcc1 = Center.Distance(center1);
	  Standard_Real distcc2 = Center.Distance(center2);
	  if (!Qualified1.IsUnqualified()) { 
	    qualifier1(NbrSol) = Qualified1.Qualifier();
	  }
	  else if (Abs(distcc1+Radius(i)-R1) < Tol) {
	    qualifier1(NbrSol) = GccEnt_enclosed;
	  }
	  else if (Abs(distcc1-R1-Radius(i)) < Tol) {
	    qualifier1(NbrSol) = GccEnt_outside;
	  }
	  else { qualifier1(NbrSol) = GccEnt_enclosing; }
	  if (!Qualified2.IsUnqualified()) { 
	    qualifier2(NbrSol) = Qualified2.Qualifier();
	  }
	  else if (Abs(distcc2+Radius(i)-R2) < Tol) {
	    qualifier2(NbrSol) = GccEnt_enclosed;
	  }
	  else if (Abs(distcc2-R2-Radius(i)) < Tol) {
	    qualifier2(NbrSol) = GccEnt_outside;
	  }
	  else { qualifier2(NbrSol) = GccEnt_enclosing; }
	  gp_Dir2d dc1(center1.XY()-Center.XY());
	  gp_Dir2d dc2(center2.XY()-Center.XY());
	  pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(i)*dc1.XY());
	  pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+Radius(i)*dc2.XY());
	  par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					    pnttg1sol(NbrSol));
	  pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg1sol(NbrSol));
	  par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
					    pnttg2sol(NbrSol));
	  pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
	  pntcen(NbrSol) = cirsol(NbrSol).Location();
	  parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
	}
      }
    }
  }
  
//=========================================================================
//   Cas general.                                                         +
//=========================================================================

  if (!WellDone) {
    GccAna_Circ2dBisec Bis(C1,C2);
    if (Bis.IsDone()) {
      TColStd_Array1OfReal Rbid(1,2);
      TColStd_Array1OfReal RBid(1,2);
      Standard_Integer nbsolution = Bis.NbSolutions();
      for (Standard_Integer i = 1 ; i <=  nbsolution ; i++) {
	Handle(GccInt_Bisec) Sol = Bis.ThisSolution(i);
	GccInt_IType typ = Sol->ArcType();

	if (typ == GccInt_Cir) {
	  Intp.Perform(OnLine,Sol->Circle());
	}
	else if (typ == GccInt_Lin) {
	  Intp.Perform(OnLine,Sol->Line());
	}
	else if (typ == GccInt_Hpr) {
	  Intp.Perform(OnLine,IntAna2d_Conic(Sol->Hyperbola()));
	}
	else if (typ == GccInt_Ell) {
	  Intp.Perform(OnLine,IntAna2d_Conic(Sol->Ellipse()));
	}
	if (Intp.IsDone()) {
	  if (!Intp.IsEmpty()){
	    for (Standard_Integer j = 1 ; j <= Intp.NbPoints() ; j++) {
	      gp_Pnt2d Center(Intp.Point(j).Value());
	      dist1 = Center.Distance(center1);
	      dist2 = Center.Distance(center2);
	      nbsol = 0;
	      Standard_Integer nsol = 0;
	      Standard_Integer nnsol = 0;
	      R1 = C1.Radius();
	      R2 = C2.Radius();
	      if (Qualified1.IsEnclosed()) {
		if (dist1-R1 < Tol) { 
		  nbsol = 1;
		  Rbid(1) = Abs(R1-dist1);
		}
	      }
	      else if (Qualified1.IsOutside()) {
		if (R1-dist1 < Tol) { 
		  nbsol = 1;
		  Rbid(1) = Abs(dist1-R1);
		}
	      }
	      else if (Qualified1.IsEnclosing()) {
		nbsol = 1;
		Rbid(1) = dist1+R1;
	      }
	      else if (Qualified1.IsUnqualified()) {
		nbsol = 2;
		Rbid(1) = dist1+R1;
		Rbid(1) = Abs(dist1-R1);
	      }
	      if (Qualified2.IsEnclosed() && nbsol != 0) {
		if (dist2-R2 < Tol) {
		  nsol = 1;
		  RBid(1) = Abs(R2-dist2);
		}
	      }
	      else if (Qualified2.IsOutside() && nbsol != 0) {
		if (R2-dist2 < Tol) {
		  nsol = 1;
		  RBid(1) = Abs(R2-dist2);
		}
	      }
	      else if (Qualified2.IsEnclosing() && nbsol != 0) {
		nsol = 1;
		RBid(1) = dist2+R2;
	      }
	      else if (Qualified2.IsUnqualified() && nbsol != 0) {
		nsol = 2;
		RBid(1) = dist2+R2;
		RBid(2) = Abs(R2-dist2);
	      }
	      for (Standard_Integer isol = 1; isol <= nbsol ; isol++) {
		for (Standard_Integer jsol = 1; jsol <= nsol ; jsol++) {
		  if (Abs(Rbid(isol)-RBid(jsol)) <= Tol) {
		    nnsol++;
		    Radius(nnsol) = (RBid(jsol)+Rbid(isol))/2.;
		  }
		}
	      }
	      if (nnsol > 0) {
		for (Standard_Integer k = 1 ; k <= nnsol ; k++) {
		  NbrSol++;
		  cirsol(NbrSol) = gp_Circ2d(gp_Ax2d(Center,dirx),Radius(k));
//                ==========================================================
		  Standard_Real distcc1 = Center.Distance(center1);
		  Standard_Real distcc2 = Center.Distance(center2);
		  if (!Qualified1.IsUnqualified()) { 
		    qualifier1(NbrSol) = Qualified1.Qualifier();
		  }
		  else if (Abs(distcc1+Radius(i)-R1) < Tol) {
		    qualifier1(NbrSol) = GccEnt_enclosed;
		  }
		  else if (Abs(distcc1-R1-Radius(i)) < Tol) {
		    qualifier1(NbrSol) = GccEnt_outside;
		  }
		  else { qualifier1(NbrSol) = GccEnt_enclosing; }
		  if (!Qualified2.IsUnqualified()) { 
		    qualifier2(NbrSol) = Qualified2.Qualifier();
		  }
		  else if (Abs(distcc2+Radius(i)-R2) < Tol) {
		    qualifier2(NbrSol) = GccEnt_enclosed;
		  }
		  else if (Abs(distcc2-R2-Radius(i)) < Tol) {
		    qualifier2(NbrSol) = GccEnt_outside;
		  }
		  else { qualifier2(NbrSol) = GccEnt_enclosing; }
		  if (Center.Distance(center1) <= Tol &&
		      Abs(Radius(k)-C1.Radius()) <= Tol) {TheSame1(NbrSol)=1;}
		  else {
		    TheSame1(NbrSol) = 0;
		    gp_Dir2d dc1(center1.XY()-Center.XY());
		    pnttg1sol(NbrSol) = gp_Pnt2d(Center.XY()+
						 Radius(k)*dc1.XY());
		    par1sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg1sol(NbrSol));
		    pararg1(NbrSol)=ElCLib::Parameter(C1,pnttg2sol(NbrSol));
		  }
		  if (Center.Distance(center2) <= Tol &&
		      Abs(Radius(k)-C2.Radius()) <= Tol) {TheSame2(NbrSol)=1;}
		  else {
		    TheSame2(NbrSol) = 0;
		    gp_Dir2d dc2(center2.XY()-Center.XY());
		    pnttg2sol(NbrSol) = gp_Pnt2d(Center.XY()+
						 Radius(k)*dc2.XY());
		    par2sol(NbrSol)=ElCLib::Parameter(cirsol(NbrSol),
						      pnttg2sol(NbrSol));
		    pararg2(NbrSol)=ElCLib::Parameter(C2,pnttg2sol(NbrSol));
		  }
		  pntcen(NbrSol) = Center;
		  parcen3(NbrSol)=ElCLib::Parameter(OnLine,pntcen(NbrSol));
		}
	      }
	    }
	  }
	  WellDone = Standard_True;
	}
      }
    }
  }
}

//========================================================================

Standard_Boolean GccAna_Circ2d2TanOn::
   IsDone () const{ return WellDone; }

Standard_Integer GccAna_Circ2d2TanOn::
   NbSolutions () const{ return NbrSol; }

gp_Circ2d GccAna_Circ2d2TanOn::
   ThisSolution (const Standard_Integer Index) const{ return cirsol(Index); }

void GccAna_Circ2d2TanOn::
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

void GccAna_Circ2d2TanOn:: 
   Tangency1 (const Standard_Integer    Index  , 
                    Standard_Real&      ParSol ,
                    Standard_Real&      ParArg ,
                    gp_Pnt2d&  PntSol ) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     if (TheSame1(Index) == 0) {
       ParSol = par1sol(Index);
       ParArg = pararg1(Index);
       PntSol = gp_Pnt2d(pnttg1sol(Index));
     }
     else { StdFail_NotDone::Raise(); }
   }
 }

void GccAna_Circ2d2TanOn:: 
   Tangency2 (const Standard_Integer    Index  , 
                    Standard_Real&      ParSol ,
                    Standard_Real&      ParArg ,
                    gp_Pnt2d&  PntSol ) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     if (TheSame2(Index) == 0) {
       ParSol = par2sol(Index);
       ParArg = pararg2(Index);
       PntSol = gp_Pnt2d(pnttg2sol(Index));
     }
     else { StdFail_NotDone::Raise(); }
   }
 }

void GccAna_Circ2d2TanOn::
   CenterOn3 (const Standard_Integer    Index  ,
                    Standard_Real&      ParArg ,
                    gp_Pnt2d&  PntSol ) const{
   if (!WellDone) { StdFail_NotDone::Raise(); }
   else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }
   else {
     ParArg = parcen3(Index);
     PntSol = pnttg1sol(Index);
   }
 }
 
Standard_Boolean GccAna_Circ2d2TanOn::
   IsTheSame1 (const Standard_Integer Index) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }

  if (TheSame1(Index) == 0) { return Standard_False; }
  return Standard_True;
}

Standard_Boolean GccAna_Circ2d2TanOn::
   IsTheSame2 (const Standard_Integer Index) const
{
  if (!WellDone) { StdFail_NotDone::Raise(); }
  else if (Index <= 0 ||Index > NbrSol) { Standard_OutOfRange::Raise(); }

  if (TheSame2(Index) == 0) { return Standard_False; }
  return Standard_True;
}
