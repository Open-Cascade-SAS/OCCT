// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

//#ifndef DEB
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#define No_Standard_DimensionError
//#endif

#include <math_FunctionAllRoots.ixx>

#include <Standard_NumericError.hxx>
#include <Standard_OutOfRange.hxx>
#include <math_FunctionRoots.hxx>
#include <math_FunctionWithDerivative.hxx>
#include <math_FunctionSample.hxx>

math_FunctionAllRoots::math_FunctionAllRoots (
                           math_FunctionWithDerivative& F,
			   const math_FunctionSample& S,
			   const Standard_Real EpsX, const Standard_Real EpsF,
			   const Standard_Real EpsNul) {

    done=Standard_False;



    Standard_Boolean Nul, PNul, InterNul, Nuld, Nulf;
    Standard_Real DebNul,FinNul;
    Standard_Integer Indd,Indf;
    Standard_Real cst,val,valsav=0,valbid;
    Standard_Boolean bid,fini;
    Standard_Integer Nbp,i;

    Nbp=S.NbPoints();
    bid=F.Value(S.GetParameter(1),val);
    PNul=Abs(val)<=EpsNul;
    if (!PNul) {valsav=val;}
    InterNul=Standard_False;
    Nuld=Standard_False;
    Nulf=Standard_False;

    i=2;
    fini=(i>Nbp);

    while (!fini) {

      bid=F.Value(S.GetParameter(i),val);
      Nul=Abs(val)<=EpsNul;
      if (!Nul) {
	valsav=val;
      }
      if (InterNul && (!Nul)) {
	InterNul=Standard_False;
	pdeb.Append(DebNul);
	ideb.Append(Indd);
	if (val>0.0) {
	  cst=EpsNul;
	}
	else {
	  cst=-EpsNul;
	}
	math_FunctionRoots Res1(F,S.GetParameter(i-1),S.GetParameter(i),10,
				EpsX,EpsF,0.0,cst);
	Standard_NumericError_Raise_if((!Res1.IsDone())   ||
				  (Res1.IsAllNull()) ||
				  (Res1.NbSolutions()==0), " ");

	FinNul=Res1.Value(1);
	Indf=Res1.StateNumber(1);

	cst=-cst;
	math_FunctionRoots Res2(F,S.GetParameter(i-1),S.GetParameter(i),10,
				EpsX,EpsF,0.0,cst);
	Standard_NumericError_Raise_if((!Res2.IsDone())   ||
				       (Res2.IsAllNull())," ");
	  
				   //-- || (Res2.NbSolutions()!=0), " ");  lbr le 13 mai 87 (!=0 -> ==0)
	if (Res2.NbSolutions()!=0) {
	  if (Res2.Value(1) < FinNul) {
	    FinNul = Res2.Value(1);
	    Indf = Res2.StateNumber(1);
	  }
	}
	pfin.Append(FinNul);
	ifin.Append(Indf);
      }
      else if ((!InterNul) && PNul && Nul) {
	InterNul=Standard_True;
	if (i==2) {
	  DebNul=S.GetParameter(1);
	  bid = F.Value(DebNul,valbid);
	  Indd = F.GetStateNumber();
	  Nuld=Standard_True;
	}
	else {
	  if (valsav>0.0) {
	    cst=EpsNul;
	  }
	  else {
	    cst=-EpsNul;
	  }
	  math_FunctionRoots Res1(F,S.GetParameter(i-2),S.GetParameter(i-1),10,
				  EpsX,EpsF,0.0,cst);
	  Standard_NumericError_Raise_if((!Res1.IsDone())   ||
				  (Res1.IsAllNull()) ||
				  (Res1.NbSolutions()==0), " ");
	  DebNul=Res1.Value(Res1.NbSolutions());
	  Indd = Res1.StateNumber(Res1.NbSolutions());

	  cst=-cst;
	  math_FunctionRoots Res3(F,S.GetParameter(i-2),S.GetParameter(i-1),10,
				  EpsX,EpsF,0.0,cst);
	  Standard_NumericError_Raise_if((!Res3.IsDone()) ||
				    (Res3.IsAllNull()), " ");

	  if (Res3.NbSolutions()!=0) {
	    if (Res3.Value(Res3.NbSolutions()) > DebNul) {
	      DebNul = Res3.Value(Res3.NbSolutions());
	      Indd = Res3.StateNumber(Res3.NbSolutions());
	    }
	  }
	}
      }
      i=i+1;
      PNul=Nul;
      fini=(i>Nbp);
    }

    if (InterNul) {            // rajouter l intervalle finissant au dernier pt
      pdeb.Append(DebNul);
      ideb.Append(Indd);
      FinNul = S.GetParameter(Nbp);
      bid = F.Value(FinNul,valbid);
      Indf = F.GetStateNumber();
      pfin.Append(FinNul);
      ifin.Append(Indf);
      Nulf=Standard_True;
    }

    if (pdeb.Length()==0) {  // Pas d intervalle nul

      math_FunctionRoots Res(F,S.GetParameter(1),S.GetParameter(Nbp),Nbp,
			     EpsX,EpsF,0.0);
      Standard_NumericError_Raise_if((!Res.IsDone()) ||
				(Res.IsAllNull()), " ");

      for (Standard_Integer j=1; j<=Res.NbSolutions(); j++) {
	piso.Append(Res.Value(j));
	iiso.Append(Res.StateNumber(j));
      }
    }
    else {
      Standard_Integer NbpMin = 3;
      Standard_Integer Nbrpt;
      if (!Nuld) {          // Recherche des solutions entre S.GetParameter(1) 
                            // et le debut du 1er intervalle nul

	Nbrpt=(Standard_Integer ) IntegerPart(
                              Abs((pdeb.Value(1)-S.GetParameter(1))/
			      (S.GetParameter(Nbp)-S.GetParameter(1)))*Nbp);
        math_FunctionRoots Res(F,S.GetParameter(1),pdeb.Value(1),
			       Max(Nbrpt, NbpMin), EpsX,EpsF,0.0);
	Standard_NumericError_Raise_if((!Res.IsDone()) ||
				  (Res.IsAllNull()), " ");

	for (Standard_Integer j=1; j<=Res.NbSolutions(); j++) {
	  piso.Append(Res.Value(j));
	  iiso.Append(Res.StateNumber(j));
	}
      }
      for (Standard_Integer k=2; k<=pdeb.Length(); k++) {
	Nbrpt=(Standard_Integer )
                              IntegerPart(Abs((pdeb.Value(k)-pfin.Value(k-1))/
			      (S.GetParameter(Nbp)-S.GetParameter(1)))*Nbp);
        math_FunctionRoots Res(F,pfin.Value(k-1),pdeb.Value(k),
			       Max(Nbrpt, NbpMin), EpsX,EpsF,0.0);
	Standard_NumericError_Raise_if((!Res.IsDone()) ||
				  (Res.IsAllNull()), " ");

	for (Standard_Integer j=1; j<=Res.NbSolutions(); j++) {
	  piso.Append(Res.Value(j));
	  iiso.Append(Res.StateNumber(j));
	}
      }
      if (!Nulf) {            // Recherche des solutions entre la fin du
                              // dernier intervalle nul et Value(Nbp).

	Nbrpt=(Standard_Integer )
                              IntegerPart(Abs((S.GetParameter(Nbp)-
			       pfin.Value(pdeb.Length()))/
			      (S.GetParameter(Nbp)-S.GetParameter(1)))*Nbp);
        math_FunctionRoots Res(F,pfin.Value(pdeb.Length()),
			       S.GetParameter(Nbp), Max(Nbrpt, NbpMin),
			       EpsX,EpsF,0.0);
	Standard_NumericError_Raise_if((!Res.IsDone()) ||
				  (Res.IsAllNull()), " ");

	for (Standard_Integer j=1; j<=Res.NbSolutions(); j++) {
	  piso.Append(Res.Value(j));
	  iiso.Append(Res.StateNumber(j));
	}
      }
    }
    done=Standard_True;
  }


void math_FunctionAllRoots::Dump(Standard_OStream& o) const {
  
  o<< "math_FunctionAllRoots ";
  if(done) {
    o<< " Status = Done \n";
    o << " Number of null intervals = " << pdeb.Length() << endl;
    o << " Number of points where the function is null: " << piso.Length() << endl;
  }
  else {
    o<< " Status = not Done \n";
  }
}
