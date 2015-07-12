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

//============================================ IntAna2d_AnaIntersection_7.cxx
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







 
