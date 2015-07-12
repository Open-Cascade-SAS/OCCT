// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

//============================================ IntAna2d_AnaIntersection_6.cxx
//============================================================================

#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <IntAna2d_Conic.hxx>
#include <IntAna2d_IntPoint.hxx>
#include <IntAna2d_Outils.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>

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

