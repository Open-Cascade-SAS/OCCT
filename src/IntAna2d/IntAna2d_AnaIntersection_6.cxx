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

//============================================ IntAna2d_AnaIntersection_6.cxx
//============================================================================
#include <IntAna2d_AnaIntersection.jxx>

#include <IntAna2d_Outils.hxx>

void IntAna2d_AnaIntersection::Perform(const gp_Elips2d& Elips,
				  const IntAna2d_Conic& Conic)
{
  Standard_Boolean EIsDirect = Elips.IsDirect();
  Standard_Real A,B,C,D,E,F;
  Standard_Real pcte,ps,pc,p2sc,pcc,pss;
  Standard_Real minor_radius=Elips.MinorRadius();
  Standard_Real major_radius=Elips.MajorRadius();
  Standard_Integer i;
  Standard_Real tx,ty,S;
  
  done = Standard_False;
  nbp = 0;
  para = Standard_False;
  iden = Standard_False; 
  empt = Standard_False;
  
  
  gp_Ax2d Axe_rep(Elips.XAxis());
  
  Conic.Coefficients(A,B,C,D,E,F);
  Conic.NewCoefficients(A,B,C,D,E,F,Axe_rep);   

  // Parametre : a avec x=MajorRadius Cos(a)  et y=MinorRadius Sin(a)
  
  pss= B*minor_radius*minor_radius;                   // SIN ^2
  pcc= A*major_radius*major_radius-pss;               // COS ^2
  p2sc=C*major_radius*minor_radius;                   // 2 SIN COS
  pc= 2.0*D*major_radius;                             // COS
  ps= 2.0*E*minor_radius;                             // SIN
  pcte=F+pss;                                         // 1
  
  math_TrigonometricFunctionRoots Sol(pcc,p2sc,pc,ps,pcte,0.0,2.0*M_PI);

  if (!Sol.IsDone()) {
    done=Standard_False;
    return;
  }
  else { 
    if(Sol.InfiniteRoots()) {
      iden=Standard_True;
      done=Standard_True;
      return;
    }
    nbp=Sol.NbSolutions();
    for(i=1;i<=nbp;i++) {
      S = Sol.Value(i);
      tx=major_radius*Cos(S); 
      ty=minor_radius*Sin(S);
      Coord_Ancien_Repere(tx,ty,Axe_rep);
      if(!EIsDirect) 
	S = M_PI+M_PI-S;
      lpnt[i-1].SetValue(tx,ty,S);
    }
    Traitement_Points_Confondus(nbp,lpnt);
  }
  done = Standard_True;
}

