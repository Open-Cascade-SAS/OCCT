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

//============================================================================
//======================================================= IntAna2d_Outils.hxx
//============================================================================
#ifndef IntAna2d_Outil_HeaderFile
#define IntAna2d_Outil_HeaderFile

#ifndef math_DirectPolynomialRoots_HeaderFile
#include <math_DirectPolynomialRoots.hxx>
#endif

#ifndef math_TrigonometricFunctionRoots_HeaderFile
#include <math_TrigonometricFunctionRoots.hxx>
#endif

#ifndef IntAna2d_IntPoint_HeaderFile
#include <IntAna2d_IntPoint.hxx>
#endif

#ifndef gp_Ax2d_HeaderFile
#include <gp_Ax2d.hxx>
#endif

class MyDirectPolynomialRoots { 
public:
  MyDirectPolynomialRoots(const Standard_Real A4,
			  const Standard_Real A3,
			  const Standard_Real A2,
			  const Standard_Real A1,
			  const Standard_Real A0);

  MyDirectPolynomialRoots(const Standard_Real A2,
			  const Standard_Real A1,
			  const Standard_Real A0); 
  
  Standard_Integer NbSolutions() const { return(nbsol); } 
  Standard_Real    Value(const Standard_Integer i) const { return(sol[i-1]); }
  Standard_Real    IsDone() const { return(nbsol>-1); }
  Standard_Boolean InfiniteRoots() const { return(same); } 
private:
  Standard_Real      sol[16];
  Standard_Real      val[16];
  Standard_Integer   nbsol;
  Standard_Boolean   same;
}; 
						     

Standard_Boolean Points_Confondus(const Standard_Real xa,const Standard_Real ya,
				  const Standard_Real xb,const Standard_Real yb);



void Traitement_Points_Confondus(Standard_Integer& nb_pts
				 ,IntAna2d_IntPoint *pts);

void Coord_Ancien_Repere(Standard_Real& Ancien_X,Standard_Real& Ancien_Y
                        ,const gp_Ax2d Axe_Nouveau_Repere);


#endif




