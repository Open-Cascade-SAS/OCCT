// Copyright (c) 1995-1999 Matra Datavision
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

//============================================ IntAna2d_AnaIntersection_7.cxx
//============================================================================
#include <IntAna2d_AnaIntersection.jxx>

#include <IntAna2d_Outils.hxx>

void IntAna2d_AnaIntersection::Perform(const gp_Parab2d& P, 
				       const IntAna2d_Conic& Conic)
  {
    Standard_Boolean PIsDirect = P.IsDirect();
    Standard_Real A,B,C,D,E,F;
    Standard_Real px4,px3,px2,px1,px0;
    Standard_Integer i;
    Standard_Real tx,ty,S;
    Standard_Real un_sur_2p=0.5/(P.Parameter());
    gp_Ax2d Axe_rep(P.MirrorAxis());

    done = Standard_False;
    nbp = 0;
    para = Standard_False;
    empt = Standard_False;
    iden = Standard_False; 

    Conic.Coefficients(A,B,C,D,E,F);
    Conic.NewCoefficients(A,B,C,D,E,F,Axe_rep); 

    //-------- 'Parametre'  y avec y=y  x=y^2/(2 p)
    
    px0=F;
    px1=E+E;
    px2=B + un_sur_2p*(D+D);
    px3=(C+C)*un_sur_2p;
    px4=A*(un_sur_2p*un_sur_2p);
    
    MyDirectPolynomialRoots Sol(px4,px3,px2,px1,px0);

    if(!Sol.IsDone()) {
      done=Standard_False;
    }
    else {   
      if(Sol.InfiniteRoots()) {
	iden=Standard_True;
	done=Standard_True;
      }
      nbp=Sol.NbSolutions();
      for(i=1;i<=nbp;i++) {
	S = Sol.Value(i);
	tx=un_sur_2p*S*S;
	ty=S;
	Coord_Ancien_Repere(tx,ty,Axe_rep);
	if(!PIsDirect) 
	  S =-S;
	lpnt[i-1].SetValue(tx,ty,S);
      }
      Traitement_Points_Confondus(nbp,lpnt);
    }
    done=Standard_True;
  }







 
