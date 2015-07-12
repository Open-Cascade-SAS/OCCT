// Created on: 1995-10-19
// Created by: Andre LIEUTIER
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

// 26-Mar-96 : xab : inclusion des inlines trop gros 
// 15-Oct-96 : alr : extraction des inlines (pas tous ceux inclus par xab)
// 19-Fev-97 : jct : ajout des methodes UVBox et UVConstraints (G1134)
// 10-Dec-97 : jag : Gros debug sur delete, et sur la methode Copy...
// 13-Jan-98 : alr : ajout des derivees pour contraintes G3 et approx. C2
// 28-Avr-98 : alr : Prise en compte des Linear*Constraint, methodes SolveTI1,SolveTI2,SolveTI3

#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <math_Gauss.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Plate_FreeGtoCConstraint.hxx>
#include <Plate_GlobalTranslationConstraint.hxx>
#include <Plate_GtoCConstraint.hxx>
#include <Plate_LinearScalarConstraint.hxx>
#include <Plate_LinearXYZConstraint.hxx>
#include <Plate_LineConstraint.hxx>
#include <Plate_PinpointConstraint.hxx>
#include <Plate_PlaneConstraint.hxx>
#include <Plate_Plate.hxx>
#include <Plate_SampledCurveConstraint.hxx>
#include <Standard_ErrorHandler.hxx>

//=======================================================================
//function : Plate_Plate
//purpose  : 
//=======================================================================
Plate_Plate::Plate_Plate()
: order(0), n_el(0), n_dim(0),
  solution(0),points(0),deru(0),derv(0),
  OK(Standard_False),maxConstraintOrder(0),
  Uold (1.e20),
  Vold (1.e20),
  U2 (0.0),
  R (0.0),
  L (0.0)
{
  PolynomialPartOnly = Standard_False;
}

//=======================================================================
//function : Plate_Plate
//purpose  : 
//=======================================================================

Plate_Plate::Plate_Plate(const Plate_Plate& Ref)
: order(Ref.order),n_el(Ref.n_el),n_dim(Ref.n_dim),
  solution(0),points(0),deru(0),derv(0),
  OK (Ref.OK),
  Uold (1.e20),
  Vold (1.e20),
  U2 (0.0),
  R (0.0),
  L (0.0)
{
  Standard_Integer i;
  if (Ref.OK) {
    if (n_dim >0 && Ref.solution != 0) {
      solution = new gp_XYZ[n_dim];
      for(i=0; i<n_dim ;i++) {
	Solution(i) = Ref.Solution(i);
      }
    }
    
    if (n_el >0) {
      if (Ref.points != 0) {
	points = new gp_XY[n_el];
	for(i=0; i<n_el;i++) {
	  Points(i) = Ref.Points(i);
	}
      }
      
      if (Ref.deru != 0) {
	deru = new Standard_Integer[n_el] ;
	for (i = 0 ; i < n_el ; i++) {
	  Deru(i) = Ref.Deru(i);
	}
      }
      
      if (Ref.derv != 0) {
	derv = new Standard_Integer[n_el] ;
	for (i = 0 ; i < n_el ; i++) {
	  Derv(i) = Ref.Derv(i);
	}
      }
    }
  }

  myConstraints = Ref.myConstraints;
  myLXYZConstraints = Ref.myLXYZConstraints;
  myLScalarConstraints = Ref.myLScalarConstraints;
  maxConstraintOrder = Ref.maxConstraintOrder;
  PolynomialPartOnly = Ref.PolynomialPartOnly;
  for (i=0; i<10;i++) {
    ddu[i]=Ref.ddu[i];
    ddv[i]=Ref.ddv[i];
  }
}
//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================
 Plate_Plate& Plate_Plate::Copy(const Plate_Plate& Ref) 
{
  Init();
   order = Ref.order;
   n_el = Ref.n_el;
   n_dim = Ref.n_dim;
   OK = Ref.OK;
  Standard_Integer i;
  if (Ref.OK) {
    if (n_dim >0 && Ref.solution != 0) {
      solution = new gp_XYZ[n_dim];
      for(i=0; i<n_dim ;i++) {
	Solution(i) = Ref.Solution(i);
      }
    }
    
    if (n_el >0) {
      if (Ref.points != 0) {
	points = new gp_XY[n_el];
	for(i=0; i<n_el;i++) {
	  Points(i) = Ref.Points(i);
	}
      }
      
      if (Ref.deru != 0) {
	deru = new Standard_Integer[n_el] ;
	for (i = 0 ; i < n_el ; i++) {
	  Deru(i) = Ref.Deru(i);
	}
      }
      
      if (Ref.derv != 0) {
	derv = new Standard_Integer[n_el] ;
	for (i = 0 ; i < n_el ; i++) {
	  Derv(i) = Ref.Derv(i);
	}
      }
    }
  }

  myConstraints = Ref.myConstraints;
  myLXYZConstraints = Ref.myLXYZConstraints;
  myLScalarConstraints = Ref.myLScalarConstraints;
  maxConstraintOrder = Ref.maxConstraintOrder;
  PolynomialPartOnly = Ref.PolynomialPartOnly;

   for (i=0; i<10;i++) {
    ddu[i]=Ref.ddu[i];
    ddv[i]=Ref.ddv[i];
  }
  return *this;
}
//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Plate_Plate::Load(const Plate_PinpointConstraint& PConst)
{
  OK = Standard_False;
  n_el++;
  myConstraints.Append(PConst);
  Standard_Integer OrdreConst = PConst.Idu() + PConst.Idv();
  if(maxConstraintOrder<OrdreConst) maxConstraintOrder = OrdreConst;
}

 void Plate_Plate::Load(const Plate_LinearXYZConstraint& LXYZConst) 
{
  OK = Standard_False;
  n_el += LXYZConst.Coeff().RowLength();

  myLXYZConstraints.Append(LXYZConst);
  for(Standard_Integer j=1;j <= LXYZConst.GetPPC().Length() ; j++)
    {
      Standard_Integer OrdreConst = LXYZConst.GetPPC()(j).Idu() + LXYZConst.GetPPC()(j).Idv();
      if(maxConstraintOrder<OrdreConst) maxConstraintOrder = OrdreConst;
  }
}

 void Plate_Plate::Load(const Plate_LinearScalarConstraint& LScalarConst) 
{
  OK = Standard_False;
  n_el += LScalarConst.Coeff().RowLength();
  myLScalarConstraints.Append(LScalarConst);
  for(Standard_Integer j=1;j <= LScalarConst.GetPPC().Length() ; j++)
    {
      Standard_Integer OrdreConst = LScalarConst.GetPPC()(j).Idu() + LScalarConst.GetPPC()(j).Idv();
      if(maxConstraintOrder<OrdreConst) maxConstraintOrder = OrdreConst;
  }
}

void Plate_Plate::Load(const Plate_LineConstraint& LConst)
{
  Load(LConst.LSC());
}

void Plate_Plate::Load(const Plate_PlaneConstraint& PConst)
{
  Load(PConst.LSC());
}

void Plate_Plate::Load(const Plate_SampledCurveConstraint& SCConst)
{
  Load(SCConst.LXYZC());
}

void Plate_Plate::Load(const Plate_GtoCConstraint& GtoCConst)
{
  for(Standard_Integer i=0;i< GtoCConst.nb_PPC();i++) 
    Load(GtoCConst.GetPPC(i));
}

void Plate_Plate::Load(const Plate_FreeGtoCConstraint& FGtoCConst) 
{
  Standard_Integer i ;
  for( i=0;i< FGtoCConst.nb_PPC();i++) 
    Load(FGtoCConst.GetPPC(i));
  for(i=0;i< FGtoCConst.nb_LSC();i++) 
    Load(FGtoCConst.LSC(i));
}

void Plate_Plate::Load(const Plate_GlobalTranslationConstraint& GTConst) 
{
  Load(GTConst.LXYZC());
}

//=======================================================================
//function : SolveTI
//purpose  : to solve the set of constraints
//=======================================================================

void Plate_Plate::SolveTI(const Standard_Integer ord, 
			  const Standard_Real anisotropie)
{
  Standard_Integer IterationNumber=0;
  OK = Standard_False;
  order = ord;
  if(ord <=1) return;
  if(ord > 9) return;
  if(n_el<1) return;
  if(anisotropie < 1.e-6) return;
  if(anisotropie > 1.e+6) return;

// computation of the bounding box of the 2d PPconstraints
  Standard_Real xmin,xmax,ymin,ymax;
  UVBox(xmin,xmax,ymin,ymax);

  Standard_Real du = 0.5*(xmax - xmin);
  if(anisotropie >1.) du *= anisotropie;
  if(du < 1.e-10) return;
  ddu[0] = 1;
  Standard_Integer i ;
  for( i=1;i<=9;i++) ddu[i] = ddu[i-1] / du;

  Standard_Real dv = 0.5*(ymax - ymin);
  if(anisotropie <1.) dv /= anisotropie;
  if(dv < 1.e-10) return;
  ddv[0] = 1;
  for(i=1;i<=9;i++) ddv[i] = ddv[i-1] / dv;


  if(myLScalarConstraints.IsEmpty())
    {
      if(myLXYZConstraints.IsEmpty())
	SolveTI1(IterationNumber);
      else
	SolveTI2(IterationNumber);
    }
  else
    SolveTI3(IterationNumber);

}

//=======================================================================
//function : SolveTI1
//purpose  : to solve the set of constraints in the easiest case,
//           only PinPointConstraints are loaded
//=======================================================================

void Plate_Plate::SolveTI1(const Standard_Integer IterationNumber)
{
// computation of square matrix members


  n_dim = n_el + order*(order+1)/2;
  math_Matrix mat(0, n_dim-1, 0, n_dim-1, 0.);
  
  delete [] (gp_XY*)points;
  points = new gp_XY[n_el];
  Standard_Integer i ;
  for( i=0; i<n_el;i++)  Points(i) = myConstraints(i+1).Pnt2d();

  delete [] (Standard_Integer*)deru;
  deru = new Standard_Integer[n_el];
  for(i=0; i<n_el;i++)  Deru(i) = myConstraints(i+1).Idu();

  delete [] (Standard_Integer*)derv;
  derv = new Standard_Integer[n_el];
  for(i=0; i<n_el;i++)  Derv(i) = myConstraints(i+1).Idv();

  for(i=0; i<n_el;i++) {
    for(Standard_Integer j=0;j<i;j++) {
      Standard_Real signe = 1;
      if ( ((Deru(j)+Derv(j))%2) == 1) signe = -1;
      Standard_Integer iu =  Deru(i) + Deru(j);
      Standard_Integer iv =  Derv(i) + Derv(j);
      mat(i,j) = signe * SolEm(Points(i) - Points(j),iu,iv);
    }
  }
  
  i = n_el;
  for(Standard_Integer iu = 0; iu< order; iu++) {
    for(Standard_Integer iv =0; iu+iv < order; iv++) {
      for(Standard_Integer j=0;j<n_el;j++) {
	Standard_Integer idu = Deru(j);
	Standard_Integer idv = Derv(j);
	mat(i,j) = Polm (Points(j), iu, iv, idu, idv);
      }
      i++;
    }
  }

  for(i=0;i<n_dim;i++) {
    for(Standard_Integer j = i+1; j<n_dim;j++) {
      mat(i,j) = mat(j,i);
    }
  }

// initialisation of the Gauss algorithm
  Standard_Real pivot_max = 1.e-12;
  OK = Standard_True;     

  math_Gauss algo_gauss(mat,pivot_max);
  if(!algo_gauss.IsDone()) {
    Standard_Integer nbm = order*(order+1)/2;
    for(i=n_el;i<n_el+nbm;i++) {
      mat(i,i) = 1.e-8;
    }
    pivot_max = 1.e-18;
    math_Gauss thealgo(mat,pivot_max);
    algo_gauss = thealgo;
    OK = algo_gauss.IsDone();
  }

  if (OK) {
//   computation of the linear system solution for the X, Y and Z coordinates
    math_Vector sec_member( 0, n_dim-1, 0.);
    math_Vector sol(0,n_dim-1);

    delete [] (gp_XYZ*) solution;
    solution = new gp_XYZ[n_dim];
    
    for(Standard_Integer icoor=1; icoor<=3;icoor++) {
      for(i=0;i<n_el;i++) {
	sec_member(i) = myConstraints(i+1).Value().Coord(icoor);
      }
      algo_gauss.Solve(sec_member, sol);
      //alr iteration pour affiner la solution
      {
	math_Vector sol1(0,n_dim-1);
	math_Vector sec_member1(0,n_dim-1);
	for(i=1;i<=IterationNumber;i++)
	  {
	    sec_member1 = sec_member - mat*sol;
	    algo_gauss.Solve(sec_member1, sol1);
	    sol += sol1;
	  }
      }
      //finalr
 
      for(i=0;i<n_dim;i++) {
	Solution(i).SetCoord (icoor, sol(i));
      }
    }
  }
}

//=======================================================================
//function : SolveTI2
//purpose  : to solve the set of constraints in the medium case,
//           LinearXYZ constraints are provided but no LinearScalar one
//=======================================================================

void Plate_Plate::SolveTI2(const Standard_Integer IterationNumber)
{
// computation of square matrix members

  Standard_Integer nCC1 = myConstraints.Length();
  Standard_Integer nCC2 = 0;
  Standard_Integer i ;
  for( i = 1; i<= myLXYZConstraints.Length(); i++)
    nCC2 += myLXYZConstraints(i).Coeff().ColLength();

  Standard_Integer n_dimat = nCC1 + nCC2 + order*(order+1)/2;

  
  delete [] (gp_XY*)points;
  points = new gp_XY[n_el];
  delete [] (Standard_Integer*)deru;
  deru = new Standard_Integer[n_el];
  delete [] (Standard_Integer*)derv;
  derv = new Standard_Integer[n_el];


  for(i=0; i< nCC1;i++)
    {
      Points(i) = myConstraints(i+1).Pnt2d();
      Deru(i) = myConstraints(i+1).Idu();
      Derv(i) = myConstraints(i+1).Idv();
    }

  Standard_Integer k = nCC1;
  for( i = 1; i<= myLXYZConstraints.Length(); i++)
    for(Standard_Integer j=1;j <= myLXYZConstraints(i).GetPPC().Length() ; j++)
      {
	Points(k) = myLXYZConstraints(i).GetPPC()(j).Pnt2d();
	Deru(k) =  myLXYZConstraints(i).GetPPC()(j).Idu();
	Derv(k) =  myLXYZConstraints(i).GetPPC()(j).Idv();
	k++;
      }

  math_Matrix mat(0, n_dimat-1, 0, n_dimat-1, 0.);

  fillXYZmatrix(mat,0,0,nCC1,nCC2);


// initialisation of the Gauss algorithm
  Standard_Real pivot_max = 1.e-12;
  OK = Standard_True;      // ************ JHH

  math_Gauss algo_gauss(mat,pivot_max);
  if(!algo_gauss.IsDone()) {
    for(i=nCC1+nCC2;i<n_dimat;i++) {
      mat(i,i) = 1.e-8;
    }
    pivot_max = 1.e-18;
    math_Gauss thealgo1(mat,pivot_max);
    algo_gauss = thealgo1; 
    OK = algo_gauss.IsDone();
  }

  if (OK) {
//   computation of the linear system solution for the X, Y and Z coordinates
    math_Vector sec_member( 0, n_dimat-1, 0.);
    math_Vector sol(0,n_dimat-1);

    delete [] (gp_XYZ*) solution;
    n_dim = n_el+order*(order+1)/2;
    solution = new gp_XYZ[n_dim];
    
    for(Standard_Integer icoor=1; icoor<=3;icoor++) {
      for(i=0;i<nCC1;i++) {
	sec_member(i) = myConstraints(i+1).Value().Coord(icoor);
      }

      k = nCC1;
      for(i = 1; i<= myLXYZConstraints.Length(); i++) {
	for(Standard_Integer irow =1; irow <= myLXYZConstraints(i).Coeff().ColLength(); irow++) {
	  for(Standard_Integer icol=1; icol<=myLXYZConstraints(i).Coeff().RowLength();icol++)
	    sec_member(k) += myLXYZConstraints(i).Coeff()(irow,icol) 
	      *  myLXYZConstraints(i).GetPPC()(icol).Value().Coord(icoor);
	  k++;
	}
      }

      algo_gauss.Solve(sec_member, sol);
      //alr iteration pour affiner la solution
      {
	math_Vector sol1(0,n_dimat-1);
	math_Vector sec_member1(0,n_dimat-1);
	for(i=1;i<=IterationNumber;i++)
	  {
	    sec_member1 = sec_member - mat*sol;
	    algo_gauss.Solve(sec_member1, sol1);
	    sol += sol1;
	}
      }
      //finalr

      for(i=0;i<nCC1;i++) Solution(i).SetCoord (icoor, sol(i));

      Standard_Integer kSolution = nCC1;
      Standard_Integer ksol = nCC1;

      for(i = 1; i<= myLXYZConstraints.Length(); i++) {
	for(Standard_Integer icol=1; icol<=myLXYZConstraints(i).Coeff().RowLength();icol++){
	  Standard_Real vsol = 0;
	  for(Standard_Integer irow =1; irow <= myLXYZConstraints(i).Coeff().ColLength(); irow++)
	    vsol += myLXYZConstraints(i).Coeff()(irow,icol)*sol(ksol+irow-1);
	  Solution(kSolution).SetCoord (icoor, vsol);
	  kSolution++;
	}
	ksol += myLXYZConstraints(i).Coeff().ColLength();
      }

      for(i=0;i<order*(order+1)/2; i++) {
	Solution(n_el+i).SetCoord (icoor, sol(ksol+i));
      }
    }
  }
}

//=======================================================================
//function : SolveTI3
//purpose  : to solve the set of constraints in the most general situation
//=======================================================================

void Plate_Plate::SolveTI3(const Standard_Integer IterationNumber)
{
// computation of square matrix members

  Standard_Integer nCC1 = myConstraints.Length();

  Standard_Integer nCC2 = 0;
  Standard_Integer i ;
  for( i = 1; i<= myLXYZConstraints.Length(); i++)
    nCC2 += myLXYZConstraints(i).Coeff().ColLength();

  Standard_Integer nCC3 = 0;
  for(i = 1; i<= myLScalarConstraints.Length(); i++)
    nCC3 += myLScalarConstraints(i).Coeff().ColLength();

  Standard_Integer nbm = order*(order+1)/2;
  Standard_Integer n_dimsousmat = nCC1 + nCC2 + nbm ;
  Standard_Integer n_dimat =3*n_dimsousmat + nCC3;

  
  delete [] (gp_XY*)points;
  points = new gp_XY[n_el];
  delete [] (Standard_Integer*)deru;
  deru = new Standard_Integer[n_el];
  delete [] (Standard_Integer*)derv;
  derv = new Standard_Integer[n_el];


  for(i=0; i< nCC1;i++)
    {
      Points(i) = myConstraints(i+1).Pnt2d();
      Deru(i) = myConstraints(i+1).Idu();
      Derv(i) = myConstraints(i+1).Idv();
    }

  Standard_Integer k = nCC1;
  for(i = 1; i<= myLXYZConstraints.Length(); i++)
    for(Standard_Integer j=1;j <= myLXYZConstraints(i).GetPPC().Length() ; j++)
      {
	Points(k) = myLXYZConstraints(i).GetPPC()(j).Pnt2d();
	Deru(k) =  myLXYZConstraints(i).GetPPC()(j).Idu();
	Derv(k) =  myLXYZConstraints(i).GetPPC()(j).Idv();
	k++;
      }
  Standard_Integer nPPC2 = k;
  for(i = 1; i<= myLScalarConstraints.Length(); i++)
    for(Standard_Integer j=1;j <= myLScalarConstraints(i).GetPPC().Length() ; j++)
      {
	Points(k) = myLScalarConstraints(i).GetPPC()(j).Pnt2d();
	Deru(k) =  myLScalarConstraints(i).GetPPC()(j).Idu();
	Derv(k) =  myLScalarConstraints(i).GetPPC()(j).Idv();
	k++;
      }

  math_Matrix mat(0, n_dimat-1, 0, n_dimat-1, 0.);

  fillXYZmatrix(mat,0,0,nCC1,nCC2);
  fillXYZmatrix(mat,n_dimsousmat,n_dimsousmat,nCC1,nCC2);
  fillXYZmatrix(mat,2*n_dimsousmat,2*n_dimsousmat,nCC1,nCC2);

  k = 3*n_dimsousmat;
  Standard_Integer kppc = nPPC2;
  Standard_Integer j ;
  for(i = 1; i<= myLScalarConstraints.Length(); i++) {
    for( j=0;j<nCC1;j++){

      math_Vector vmat(1,myLScalarConstraints(i).GetPPC().Length());

      for(Standard_Integer ippc=1;ippc <= myLScalarConstraints(i).GetPPC().Length() ; ippc++) {
	Standard_Real signe = 1;
	if ( ((Deru(j)+Derv(j))%2) == 1) signe = -1;
	Standard_Integer iu =  Deru(kppc+ippc-1) + Deru(j);
	Standard_Integer iv =  Derv(kppc+ippc-1) + Derv(j);
	vmat(ippc) =  signe * SolEm(Points(kppc+ippc-1) - Points(j),iu,iv);
      }

      for(Standard_Integer irow=1;irow <= myLScalarConstraints(i).Coeff().ColLength() ; irow++)
	for(Standard_Integer icol=1;icol <= myLScalarConstraints(i).Coeff().RowLength() ; icol++){
	  mat(k+irow-1,j) += myLScalarConstraints(i).Coeff()(irow,icol).X()*vmat(icol);
	  mat(k+irow-1,n_dimsousmat+j) += myLScalarConstraints(i).Coeff()(irow,icol).Y()*vmat(icol);
	  mat(k+irow-1,2*n_dimsousmat+j) += myLScalarConstraints(i).Coeff()(irow,icol).Z()*vmat(icol);
	}
    }

    Standard_Integer k2 = nCC1;
    Standard_Integer kppc2 = nCC1;
    Standard_Integer i2 ;
    for( i2 = 1; i2<=myLXYZConstraints.Length() ; i2++){

      math_Matrix tmpmat(1,myLScalarConstraints(i).GetPPC().Length(),1,myLXYZConstraints(i2).GetPPC().Length() );

      for(Standard_Integer ippc=1;ippc <= myLScalarConstraints(i).GetPPC().Length() ; ippc++)
	for(Standard_Integer ippc2=1;ippc2 <= myLXYZConstraints(i2).GetPPC().Length() ; ippc2++){
	  Standard_Real signe = 1;
	  if ( ((Deru(kppc2+ippc2-1)+Derv(kppc2+ippc2-1))%2) == 1) signe = -1;
	  Standard_Integer iu =  Deru(kppc+ippc-1) + Deru(kppc2+ippc2-1);
	  Standard_Integer iv =  Derv(kppc+ippc-1) + Derv(kppc2+ippc2-1);
	  tmpmat(ippc,ippc2) = signe * SolEm(Points(kppc+ippc-1) - Points(kppc2+ippc2-1),iu,iv);
	}

      for(Standard_Integer irow=1;irow <= myLScalarConstraints(i).Coeff().ColLength() ; irow++)
	for(Standard_Integer irow2=1;irow2 <= myLXYZConstraints(i2).Coeff().ColLength() ; irow2++)
	  for(Standard_Integer icol=1;icol <= myLScalarConstraints(i).Coeff().RowLength() ; icol++)
	    for(Standard_Integer icol2=1;icol2 <= myLXYZConstraints(i2).Coeff().RowLength() ; icol2++){
	      mat(k+irow-1,k2+irow2-1) += 
		myLScalarConstraints(i).Coeff()(irow,icol).X()*myLXYZConstraints(i2).Coeff()(irow2,icol2)*tmpmat(icol,icol2);
	      mat(k+irow-1,n_dimsousmat+k2+irow2-1) += 
		myLScalarConstraints(i).Coeff()(irow,icol).Y()*myLXYZConstraints(i2).Coeff()(irow2,icol2)*tmpmat(icol,icol2);
	      mat(k+irow-1,2*n_dimsousmat+k2+irow2-1) += 
		myLScalarConstraints(i).Coeff()(irow,icol).Z()*myLXYZConstraints(i2).Coeff()(irow2,icol2)*tmpmat(icol,icol2);
	    }

      k2 += myLXYZConstraints(i2).Coeff().ColLength();
      kppc2 += myLXYZConstraints(i2).Coeff().RowLength();
    }



    j = nCC1+nCC2;
    for(Standard_Integer iu = 0; iu< order; iu++)
      for(Standard_Integer iv =0; iu+iv < order; iv++) {

	math_Vector vmat(1,myLScalarConstraints(i).GetPPC().Length());
	for(Standard_Integer ippc=1;ippc <= myLScalarConstraints(i).GetPPC().Length() ; ippc++){
	  Standard_Integer idu = Deru(kppc+ippc-1);
	  Standard_Integer idv = Derv(kppc+ippc-1);
	  vmat(ippc) = Polm (Points(kppc+ippc-1),iu,iv,idu,idv);
	}

	for(Standard_Integer irow=1;irow <= myLScalarConstraints(i).Coeff().ColLength() ; irow++)
	  for(Standard_Integer icol=1;icol <= myLScalarConstraints(i).Coeff().RowLength() ; icol++){
	    mat(k+irow-1,j) += myLScalarConstraints(i).Coeff()(irow,icol).X()*vmat(icol);
	    mat(k+irow-1,n_dimsousmat+j) += myLScalarConstraints(i).Coeff()(irow,icol).Y()*vmat(icol);
	    mat(k+irow-1,2*n_dimsousmat+j) += myLScalarConstraints(i).Coeff()(irow,icol).Z()*vmat(icol);
	  }

      j++;
    }


    k2 = 3*n_dimsousmat;
    kppc2 = nPPC2;
    for(i2 = 1; i2<=i ; i2++){

      math_Matrix tmpmat(1,myLScalarConstraints(i).GetPPC().Length(),1,myLScalarConstraints(i2).GetPPC().Length() );

      for(Standard_Integer ippc=1;ippc <= myLScalarConstraints(i).GetPPC().Length() ; ippc++)
	for(Standard_Integer ippc2=1;ippc2 <= myLScalarConstraints(i2).GetPPC().Length() ; ippc2++){
	  Standard_Real signe = 1;
	  if ( ((Deru(kppc2+ippc2-1)+Derv(kppc2+ippc2-1))%2) == 1) signe = -1;
	  Standard_Integer a_iu =  Deru(kppc+ippc-1) + Deru(kppc2+ippc2-1);
	  Standard_Integer iv =  Derv(kppc+ippc-1) + Derv(kppc2+ippc2-1);
	  tmpmat(ippc,ippc2) = signe * SolEm(Points(kppc+ippc-1) - Points(kppc2+ippc2-1),a_iu,iv);
	}

      for(Standard_Integer irow=1;irow <= myLScalarConstraints(i).Coeff().ColLength() ; irow++)
	for(Standard_Integer irow2=1;irow2 <= myLScalarConstraints(i2).Coeff().ColLength() ; irow2++)
	  for(Standard_Integer icol=1;icol <= myLScalarConstraints(i).Coeff().RowLength() ; icol++)
	    for(Standard_Integer icol2=1;icol2 <= myLScalarConstraints(i2).Coeff().RowLength() ; icol2++){
	      mat(k+irow-1,k2+irow2-1) += 
		myLScalarConstraints(i).Coeff()(irow,icol)*myLScalarConstraints(i2).Coeff()(irow2,icol2)*tmpmat(icol,icol2);
	    }

      k2 += myLScalarConstraints(i2).Coeff().ColLength();
      kppc2 += myLScalarConstraints(i2).Coeff().RowLength();
    }

    k += myLScalarConstraints(i).Coeff().ColLength();
    kppc += myLScalarConstraints(i).Coeff().RowLength();
  }

  for( j=3*n_dimsousmat;j<n_dimat;j++)
    for(i=0;i<j;i++)
      mat(i,j)= mat(j,i);



// initialisation of the Gauss algorithm
  Standard_Real pivot_max = 1.e-12;
  OK = Standard_True;      // ************ JHH

  math_Gauss algo_gauss(mat,pivot_max);
  if(!algo_gauss.IsDone()) {
    for(i=nCC1+nCC2;i<nCC1+nCC2+nbm;i++) {
      mat(i,i) = 1.e-8;
      mat(n_dimsousmat+i,n_dimsousmat+i) = 1.e-8;
      mat(2*n_dimsousmat+i,2*n_dimsousmat+i) = 1.e-8;
    }
    pivot_max = 1.e-18;
    math_Gauss thealgo2(mat,pivot_max);
    algo_gauss = thealgo2;
    OK = algo_gauss.IsDone();
  }

  if (OK) {
//   computation of the linear system solution for the X, Y and Z coordinates
    math_Vector sec_member( 0, n_dimat-1, 0.);
    math_Vector sol(0,n_dimat-1);

    delete [] (gp_XYZ*) solution;
    n_dim = n_el+order*(order+1)/2;
    solution = new gp_XYZ[n_dim];
    
    Standard_Integer icoor ;
    for( icoor=1; icoor<=3;icoor++){
      for(i=0;i<nCC1;i++)
	sec_member((icoor-1)*n_dimsousmat+i) = myConstraints(i+1).Value().Coord(icoor);
   

      k = nCC1;
      for(i = 1; i<= myLXYZConstraints.Length(); i++)
	for(Standard_Integer irow =1; irow <= myLXYZConstraints(i).Coeff().ColLength(); irow++) {
	  for(Standard_Integer icol=1; icol<=myLXYZConstraints(i).Coeff().RowLength();icol++)
	    sec_member((icoor-1)*n_dimsousmat+k) += myLXYZConstraints(i).Coeff()(irow,icol) 
	      *  myLXYZConstraints(i).GetPPC()(icol).Value().Coord(icoor);
	  k++;
	}
    }
    k = 3*n_dimsousmat;
    for(i = 1; i<= myLScalarConstraints.Length(); i++)
      for(Standard_Integer irow =1; irow <= myLScalarConstraints(i).Coeff().ColLength(); irow++) {
	for(Standard_Integer icol=1; icol<=myLScalarConstraints(i).Coeff().RowLength();icol++)
	  sec_member(k) += myLScalarConstraints(i).Coeff()(irow,icol) 
	    *  myLScalarConstraints(i).GetPPC()(icol).Value();
	k++;
      }
    
    algo_gauss.Solve(sec_member, sol);
    //alr iteration pour affiner la solution
    {
      math_Vector sol1(0,n_dimat-1);
      math_Vector sec_member1(0,n_dimat-1);
      for(i=1;i<=IterationNumber;i++)
	{
	  sec_member1 = sec_member - mat*sol;
	  algo_gauss.Solve(sec_member1, sol1);
	  sol += sol1;
	}
    }
    //finalr
    
    for(icoor=1; icoor<=3;icoor++){
      for(i=0;i<nCC1;i++) Solution(i).SetCoord (icoor, sol((icoor-1)*n_dimsousmat+i));

      Standard_Integer kSolution = nCC1;
      Standard_Integer ksol = nCC1;

      for(i = 1; i<= myLXYZConstraints.Length(); i++) {
	for(Standard_Integer icol=1; icol<=myLXYZConstraints(i).Coeff().RowLength();icol++){
	  Standard_Real vsol = 0;
	  for(Standard_Integer irow =1; irow <= myLXYZConstraints(i).Coeff().ColLength(); irow++)
	    vsol += myLXYZConstraints(i).Coeff()(irow,icol)*sol((icoor-1)*n_dimsousmat+ksol+irow-1);
	  Solution(kSolution).SetCoord (icoor, vsol);
	  kSolution++;
	}
	ksol += myLXYZConstraints(i).Coeff().ColLength();
      }

      ksol = nCC1+nCC2;
      for(i=0;i<order*(order+1)/2; i++) {
	Solution(n_el+i).SetCoord (icoor, sol((icoor-1)*n_dimsousmat+ksol+i));
      }
    }

    Standard_Integer ksol = 3*n_dimsousmat;
    Standard_Integer kSolution = nPPC2;
    for(i = 1; i<= myLScalarConstraints.Length(); i++) {
      for(Standard_Integer icol=1; icol<=myLScalarConstraints(i).Coeff().RowLength();icol++){
	gp_XYZ Vsol(0.,0.,0.);
	for(Standard_Integer irow =1; irow <= myLScalarConstraints(i).Coeff().ColLength(); irow++)
	  Vsol += myLScalarConstraints(i).Coeff()(irow,icol)*sol(ksol+irow-1);
	Solution(kSolution) = Vsol;
	kSolution++;
      }
      ksol += myLScalarConstraints(i).Coeff().ColLength();
    }
  }
}

//=======================================================================
//function : fillXYZmatrix
//purpose  : 
//=======================================================================
void Plate_Plate::fillXYZmatrix(math_Matrix &mat, 
				const Standard_Integer i0,
				const Standard_Integer j0,
				const Standard_Integer ncc1,
				const Standard_Integer ncc2) const
{
  Standard_Integer i,j ;
  for( i=0; i<ncc1;i++) {
    for( j=0;j<i;j++) {
      Standard_Real signe = 1;
      if ( ((Deru(j)+Derv(j))%2) == 1) signe = -1;
      Standard_Integer iu =  Deru(i) + Deru(j);
      Standard_Integer iv =  Derv(i) + Derv(j);
      mat(i0+i,j0+j) = signe * SolEm(Points(i) - Points(j),iu,iv);
    }
  }
  
  Standard_Integer k = ncc1;
  Standard_Integer kppc = ncc1;
  for( i = 1; i<= myLXYZConstraints.Length(); i++){

    for(Standard_Integer a_j=0; a_j < ncc1; a_j++){

      math_Vector vmat(1,myLXYZConstraints(i).GetPPC().Length());

      for(Standard_Integer ippc=1;ippc <= myLXYZConstraints(i).GetPPC().Length() ; ippc++) {
	Standard_Real signe = 1;
	if ( ((Deru(a_j)+Derv(a_j))%2) == 1) signe = -1;
	Standard_Integer iu =  Deru(kppc+ippc-1) + Deru(a_j);
	Standard_Integer iv =  Derv(kppc+ippc-1) + Derv(a_j);
	vmat(ippc) =  signe * SolEm(Points(kppc+ippc-1) - Points(a_j),iu,iv);
      }

      for(Standard_Integer irow=1;irow <= myLXYZConstraints(i).Coeff().ColLength() ; irow++)
	for(Standard_Integer icol=1;icol <= myLXYZConstraints(i).Coeff().RowLength() ; icol++)
	  mat(i0+k+irow-1,j0+a_j) += myLXYZConstraints(i).Coeff()(irow,icol)*vmat(icol);
    }

    Standard_Integer k2 = ncc1;
    Standard_Integer kppc2 = ncc1;
    for(Standard_Integer i2 = 1; i2<= i; i2++){

      math_Matrix tmpmat(1,myLXYZConstraints(i).GetPPC().Length(),1,myLXYZConstraints(i2).GetPPC().Length() );

      for(Standard_Integer ippc=1;ippc <= myLXYZConstraints(i).GetPPC().Length() ; ippc++)
	for(Standard_Integer ippc2=1;ippc2 <= myLXYZConstraints(i2).GetPPC().Length() ; ippc2++){
	  Standard_Real signe = 1;
	  if ( ((Deru(kppc2+ippc2-1)+Derv(kppc2+ippc2-1))%2) == 1) signe = -1;
	  Standard_Integer iu =  Deru(kppc+ippc-1) + Deru(kppc2+ippc2-1);
	  Standard_Integer iv =  Derv(kppc+ippc-1) + Derv(kppc2+ippc2-1);
	  tmpmat(ippc,ippc2) = signe * SolEm(Points(kppc+ippc-1) - Points(kppc2+ippc2-1),iu,iv);
	}

      for(Standard_Integer irow=1;irow <= myLXYZConstraints(i).Coeff().ColLength() ; irow++)
	for(Standard_Integer irow2=1;irow2 <= myLXYZConstraints(i2).Coeff().ColLength() ; irow2++)
	  for(Standard_Integer icol=1;icol <= myLXYZConstraints(i).Coeff().RowLength() ; icol++)
	    for(Standard_Integer icol2=1;icol2 <= myLXYZConstraints(i2).Coeff().RowLength() ; icol2++)
	      mat(i0+k+irow-1,j0+k2+irow2-1) += 
		myLXYZConstraints(i).Coeff()(irow,icol)*myLXYZConstraints(i2).Coeff()(irow2,icol2)*tmpmat(icol,icol2);
      

      k2 += myLXYZConstraints(i2).Coeff().ColLength();
      kppc2 += myLXYZConstraints(i2).Coeff().RowLength();
    }

    k += myLXYZConstraints(i).Coeff().ColLength();
    kppc += myLXYZConstraints(i).Coeff().RowLength();
  }




  i = ncc1+ncc2;
  for(Standard_Integer iu = 0; iu< order; iu++)
    for(Standard_Integer iv =0; iu+iv < order; iv++) {
      for(Standard_Integer a_j=0; a_j < ncc1; a_j++) {
	Standard_Integer idu = Deru(a_j);
	Standard_Integer idv = Derv(a_j);
	mat(i0+i,j0+a_j) = Polm (Points(a_j), iu, iv, idu, idv);
      }

      Standard_Integer k2 = ncc1;
      Standard_Integer kppc2 = ncc1;
      for(Standard_Integer i2 = 1; i2<= myLXYZConstraints.Length(); i2++){
	math_Vector vmat(1,myLXYZConstraints(i2).GetPPC().Length());
	for(Standard_Integer ippc2=1;ippc2 <= myLXYZConstraints(i2).GetPPC().Length() ; ippc2++){
	  Standard_Integer idu = Deru(kppc2+ippc2-1);
	  Standard_Integer idv = Derv(kppc2+ippc2-1);
	  vmat(ippc2) = Polm (Points(kppc2+ippc2-1),iu,iv,idu,idv);
	}

	for(Standard_Integer irow2=1;irow2 <= myLXYZConstraints(i2).Coeff().ColLength() ; irow2++)
	  for(Standard_Integer icol2=1;icol2 <= myLXYZConstraints(i2).Coeff().RowLength() ; icol2++)
	    mat(i0+i,j0+k2+irow2-1) += myLXYZConstraints(i2).Coeff()(irow2,icol2)*vmat(icol2);

	k2 += myLXYZConstraints(i2).Coeff().ColLength();
	kppc2 += myLXYZConstraints(i2).Coeff().RowLength();
	}

      i++;
    }

  Standard_Integer n_dimat = ncc1 + ncc2 + order*(order+1)/2;

  for(i=0;i<n_dimat;i++) {
    for(Standard_Integer a_j = i+1; a_j < n_dimat; a_j++) {
      mat(i0+i,j0+a_j) = mat(i0+a_j,j0+i);
    }
  }

}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Plate_Plate::IsDone() const 
{
  return OK;
}


//=======================================================================
//function : destroy
//purpose  : 
//=======================================================================

void Plate_Plate::destroy()
{
  Init();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void Plate_Plate::Init()
{ 
  myConstraints.Clear();
  myLXYZConstraints.Clear();
  myLScalarConstraints.Clear();


  delete [] (gp_XYZ*)solution;
  solution = 0;

  delete [] (gp_XY*)points;
  points = 0;

  delete [] (Standard_Integer*)deru;
  deru = 0;

  delete [] (Standard_Integer*)derv;
  derv = 0;

  order = 0;
  n_el = 0;
  n_dim = 0;
  OK = Standard_True;
  maxConstraintOrder=0;
}

//=======================================================================
//function : Evaluate
//purpose  : 
//=======================================================================

gp_XYZ Plate_Plate::Evaluate(const gp_XY& point2d) const 
{
  if(solution == 0) return gp_XYZ(0,0,0);
  if(!OK) return gp_XYZ(0,0,0);

  gp_XYZ valeur(0,0,0);

  if(!PolynomialPartOnly)
    {
      for(Standard_Integer i=0; i<n_el;i++)
	{
	  Standard_Real signe = 1;
	  if ( ((Deru(i)+Derv(i))%2) == 1) signe = -1;
	  valeur += Solution(i) *  (signe*SolEm(point2d - Points(i), Deru(i), Derv(i))) ;
	}
    }
  Standard_Integer i = n_el;
  for(Standard_Integer idu = 0; idu< order; idu++)
  for(Standard_Integer idv =0; idu+idv < order; idv++)
  {
    valeur += Solution(i) * Polm( point2d, idu,idv,0,0);
    i++;
  }
  return valeur;
}

//=======================================================================
//function : EvaluateDerivative
//purpose  : 
//=======================================================================

gp_XYZ Plate_Plate::EvaluateDerivative(const gp_XY& point2d, const Standard_Integer iu, const Standard_Integer iv) const 
{
  if(solution == 0) return gp_XYZ(0,0,0);
  if(!OK) return gp_XYZ(0,0,0);

  gp_XYZ valeur(0,0,0);
  if(!PolynomialPartOnly)
    {
      for(Standard_Integer i=0; i<n_el;i++)
	{
	  Standard_Real signe = 1;
	  if ( ((Deru(i)+Derv(i))%2) == 1) signe = -1;
	  valeur += Solution(i) *  (signe*SolEm(point2d - Points(i), Deru(i)+iu, Derv(i)+iv)) ;
	}
    }
  Standard_Integer i = n_el;
  for(Standard_Integer idu = 0; idu< order; idu++)
  for(Standard_Integer idv =0; idu+idv < order; idv++)
  {
    valeur += Solution(i) * Polm( point2d, idu,idv, iu,iv);
    i++;
  }
  return valeur;
}
//=======================================================================
//function : Plate_Plate::CoefPol
//purpose  :give back the array of power basis coefficient of
// the polynomial part of the Plate function
//=======================================================================

 void Plate_Plate::CoefPol(Handle(TColgp_HArray2OfXYZ)& Coefs) const
{
  Coefs = new TColgp_HArray2OfXYZ(0,order-1,0,order-1,gp_XYZ(0.,0.,0.));
  Standard_Integer i = n_el;
  for(Standard_Integer iu = 0; iu< order; iu++)
  for(Standard_Integer iv =0; iu+iv < order; iv++)
  {
    Coefs->ChangeValue(iu,iv) = Solution(i)*ddu[iu]*ddv[iv];
    //Coefs->ChangeValue(idu,idv) = Solution(i);
    // il faut remettre cette ligne si on enleve ls facteurs dans
    // la methode Polm.
    i++;
  }
  
}
//=======================================================================
//function : Plate_Plate::Continuity
//purpose  :give back the continuity order of the Plate function
//=======================================================================

 Standard_Integer Plate_Plate::Continuity() const
{
  return 2*order - 3 - maxConstraintOrder;
}

//=======================================================================
//function : Plate_Plate::SolEm
//purpose  : compute the (iu,iv)th derivative of the fondamental solution
// of Laplcian at the power order
//=======================================================================


Standard_Real Plate_Plate::SolEm(const gp_XY& point2d, const Standard_Integer iu, const Standard_Integer iv) const 
{
  Plate_Plate* aThis = const_cast<Plate_Plate*>(this);
  Standard_Real U,V;
  Standard_Integer IU,IV;

  if(iv>iu)
  {
    // SolEm is symetric in (u<->v) : we swap u and v if iv>iu
    // to avoid some code 
    IU = iv;
    IV = iu;
    U = point2d.Y() *ddv[1];
    V = point2d.X() *ddu[1];
  }
  else
  {
    IU = iu;
    IV = iv;
    U = point2d.X() *ddu[1];
    V = point2d.Y() *ddv[1];
  }
  
   if((U==Uold)&&(V==Vold) )
     {
       if (R<1.e-20) return 0;
     }
  else
    {
        aThis->Uold = U;
 	aThis->Vold = V;
  	aThis->U2 = U*U;
  	aThis->R = U2+V*V;
  	if (R<1.e-20) return 0;
   	aThis->L = log(R);
      }
  Standard_Real DUV = 0;

  Standard_Integer m = order;
  Standard_Integer mm1 = m-1;
  Standard_Real &r = aThis->R;


  //Standard_Real pr = pow(R, mm1 - IU - IV);
  // cette expression prend beaucoup  de temps 
  //(ne tient pas compte de la petite valeur entiere de l'exposant)
  //

  Standard_Integer expo =  mm1 - IU - IV;
  Standard_Real pr;
  if(expo<0)
	{
        pr = R;
        for(Standard_Integer i=1;i<-expo;i++) pr *= R;
        pr = 1./pr;
	}
  else if(expo>0)
  	{
        pr = R;
        for(Standard_Integer i=1;i<expo;i++) pr *= R;
	}
  else pr = 1.;
	

  switch (IU)
  {
  case 0:
    switch (IV)
    {
    case 0:
      {
      DUV = pr*L;
      }
      break;

    default:
      break;
    }
  break;

  case 1:
    switch (IV)
    {
    case 0:
      {
      DUV = 2*pr*U*(1+L*mm1);
      }
      break;

    case 1:
      {
      Standard_Real m2 = m*m;
      //DUV = 4*pr*U*V*(-3+2*L+2*m-3*L*m+L*m2);
      DUV = 4*pr*U*V*((2*m-3)+(m2-3*m+2)*L);
      }
      break;

    default:
      break;
    }
    break;

  case 2:
    switch (IV)
    {
    case 0:
      {
      Standard_Real m2 = m*m;
      DUV = 2*pr*(R-L*R+L*m*R-6*U2+4*L*U2+4*m*U2-6*L*m*U2+2*L*m2*U2);
      }
      break;

    case 1:
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      DUV = -3*R+2*L*R+2*m*R-3*L*m*R+L*m2*R+22*U2-12*L*U2-24*m*U2+22*L*m*U2+6*m2*U2-12*L*m2*U2+2*L*m3*U2;
      DUV = DUV * 4* pr*V;
      }
      break;

    case 2:
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real V2 = V*V;
      Standard_Real R2 = R*R;
      DUV = -3*R2+2*L*R2+2*m*R2-3*L*m*R2+L*m2*R2+22*R*U2-12*L*R*U2-24*m*R*U2+22*L*m*R*U2+6*m2*R*U2-12*L*m2*R*U2;
      DUV += 2*L*m3*R*U2+22*R*V2-12*L*R*V2-24*m*R*V2+22*L*m*R*V2+6*m2*R*V2-12*L*m2*R*V2+2*L*m3*R*V2-200*U2*V2+96*L*U2*V2;
      DUV += 280*m*U2*V2-200*L*m*U2*V2-120*m2*U2*V2+140*L*m2*U2*V2+16*m3*U2*V2-40*L*m3*U2*V2+4*L*m4*U2*V2;
      DUV = 4*pr*DUV;
      }
      break;

    default:
      break;
    }
    break;

  case 3:
    switch (IV)
    {
    case 0:
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      DUV = -9*R+6*L*R+6*m*R-9*L*m*R+3*L*m2*R+22*U2-12*L*U2-24*m*U2+22*L*m*U2+6*m2*U2-12*L*m2*U2+2*L*m3*U2;
      DUV = DUV * 4* pr*U;
      }
      break;

    case 1:      
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      DUV = 33*R-18*L*R-36*m*R+33*L*m*R+9*m2*R-18*L*m2*R+3*L*m3*R-100*U2+48*L*U2+140*m*U2-100*L*m*U2-60*m2*U2+70*L*m2*U2;
      DUV += 8*m3*U2-20*L*m3*U2+2*L*m4*U2;
      DUV = 8*pr*U*V*DUV;
      }
      break;

    case 2:      
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m4*m;
      Standard_Real ru2 = R*U2;
      Standard_Real v2 = V*V;
      Standard_Real rv2 = R*v2;
      Standard_Real u2v2 = v2*U2;
      Standard_Real r2 = r*r;

      // copier-coller de mathematica 
	  DUV = 
     -100*ru2 + 48*L*ru2 + 140*m*ru2 - 100*L*m*ru2 - 60*m2*ru2 + 70*L*m2*ru2 + 8*m3*ru2 - 
     20*L*m3*ru2 + 2*L*m4*ru2 - 300*rv2 + 144*L*rv2 + 420*m*rv2 - 300*L*m*rv2 - 180*m2*rv2 + 210*L*m2*rv2 + 
     24*m3*rv2 - 60*L*m3*rv2 + 6*L*m4*rv2 + 33*r2 - 18*L*r2 - 36*m*r2 + 33*L*m*r2 + 9*m2*r2 - 18*L*m2*r2 + 
     3*L*m3*r2 + 1096*u2v2 - 480*L*u2v2 - 1800*m*u2v2 + 1096*L*m*u2v2 + 1020*m2*u2v2 - 900*L*m2*u2v2 - 
     240*m3*u2v2 + 340*L*m3*u2v2 + 20*m4*u2v2 - 60*L*m4*u2v2 + 4*L*m5*u2v2;

      DUV = 8*pr*U*DUV;
      }
      break;

    case 3:      
      {
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m3*m2;
      Standard_Real m6 = m3*m3;
      Standard_Real ru2 = r*U2;
      Standard_Real v2 = V*V;
      Standard_Real rv2 = R*v2;
      Standard_Real u2v2 = v2*U2;
      Standard_Real r2 = r*r;

     // copier-coller de mathematica 
      DUV = 
		1644*ru2 - 720*L*ru2 - 2700*m*ru2 + 1644*L*m*ru2 + 1530*m2*ru2 - 1350*L*m2*ru2 - 
     360*m3*ru2 + 510*L*m3*ru2 + 30*m4*ru2 - 90*L*m4*ru2 + 6*L*m5*ru2 + 1644*rv2 - 720*L*rv2 - 2700*m*rv2 + 
     1644*L*m*rv2 + 1530*m2*rv2 - 1350*L*m2*rv2 - 360*m3*rv2 + 510*L*m3*rv2 + 30*m4*rv2 - 90*L*m4*rv2 + 
     6*L*m5*rv2 - 450*r2 + 216*L*r2 + 630*m*r2 - 450*L*m*r2 - 270*m2*r2 + 315*L*m2*r2 + 36*m3*r2 - 90*L*m3*r2 + 
     9*L*m4*r2 - 7056*u2v2 + 2880*L*u2v2 + 12992*m*u2v2 - 7056*L*m*u2v2 - 8820*m2*u2v2 + 6496*L*m2*u2v2 + 
     2800*m3*u2v2 - 2940*L*m3*u2v2 - 420*m4*u2v2 + 700*L*m4*u2v2 + 24*m5*u2v2 - 84*L*m5*u2v2 + 4*L*m6*u2v2;
	 
      DUV = 16*pr*U*V*DUV;
      }
      break;

    default:
      break;
    }
  break;

  case 4:
    switch (IV)
    {
    case 0:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real U4 = U2*U2;
      Standard_Real R2 = R*R;
      DUV = -9*R2+6*L*R2+6*m*R2-9*L*m*R2+3*L*m2*R2+132*R*U2-72*L*R*U2-144*m*R*U2+132*L*m*R*U2+36*m2*R*U2-72*L*m2*R*U2;
      DUV += 12*L*m3*R*U2-200*U4+96*L*U4+280*m*U4-200*L*m*U4-120*m2*U4+140*L*m2*U4+16*m3*U4-40*L*m3*U4+4*L*m4*U4;
      DUV = 4*pr*DUV;
      }
      break;

   case 1:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real u4 = U2*U2;
      Standard_Real ru2 = R*U2;
      Standard_Real r2 = R*R;
 
	  // copier-coller de mathematica 
	  DUV = 
		-600*ru2 + 288*L*ru2 + 840*m*ru2 - 600*L*m*ru2 - 360*m2*ru2 + 420*L*m2*ru2 + 48*m3*ru2 - 
     120*L*m3*ru2 + 12*L*m4*ru2 + 33*r2 - 18*L*r2 - 36*m*r2 + 33*L*m*r2 + 9*m2*r2 - 18*L*m2*r2 + 3*L*m3*r2 + 
     1096*u4 - 480*L*u4 - 1800*m*u4 + 1096*L*m*u4 + 1020*m2*u4 - 900*L*m2*u4 - 240*m3*u4 + 340*L*m3*u4 + 20*m4*u4 - 
     60*L*m4*u4 + 4*L*m5*u4;  

      DUV = 8*pr*V*DUV;
      }
      break;

  case 2:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real m6 = m3*m3;
      Standard_Real u4 = U2*U2;
      Standard_Real r2 = r*r;
      Standard_Real r3 = r2*r;
      Standard_Real v2 = V*V;
      Standard_Real u2v2 = v2*U2;
      Standard_Real ru2v2 = R*u2v2;
      Standard_Real u4v2 = u4*v2;
      Standard_Real r2u2 = r2*U2;
      Standard_Real ru4 = r*u4;
      Standard_Real r2v2 = r2*v2;

	  // copier-coller de mathematica 
	  DUV = 
		  6576*ru2v2 - 2880*L*ru2v2 - 10800*m*ru2v2 + 6576*L*m*ru2v2 + 6120*m2*ru2v2 - 5400*L*m2*ru2v2 - 
     1440*m3*ru2v2 + 2040*L*m3*ru2v2 + 120*m4*ru2v2 - 360*L*m4*ru2v2 + 24*L*m5*ru2v2 + 1096*ru4 - 480*L*ru4 - 
     1800*m*ru4 + 1096*L*m*ru4 + 1020*m2*ru4 - 900*L*m2*ru4 - 240*m3*ru4 + 340*L*m3*ru4 + 20*m4*ru4 - 60*L*m4*ru4 + 
     4*L*m5*ru4 - 600*r2u2 + 288*L*r2u2 + 840*m*r2u2 - 600*L*m*r2u2 - 360*m2*r2u2 + 420*L*m2*r2u2 + 48*m3*r2u2 - 
     120*L*m3*r2u2 + 12*L*m4*r2u2 - 300*r2v2 + 144*L*r2v2 + 420*m*r2v2 - 300*L*m*r2v2 - 180*m2*r2v2 + 210*L*m2*r2v2 + 
     24*m3*r2v2 - 60*L*m3*r2v2 + 6*L*m4*r2v2 + 33*r3 - 18*L*r3 - 36*m*r3 + 33*L*m*r3 + 9*m2*r3 - 18*L*m2*r3 + 
     3*L*m3*r3 - 14112*u4v2 + 5760*L*u4v2 + 25984*m*u4v2 - 14112*L*m*u4v2 - 17640*m2*u4v2 + 12992*L*m2*u4v2 + 
     5600*m3*u4v2 - 5880*L*m3*u4v2 - 840*m4*u4v2 + 1400*L*m4*u4v2 + 48*m5*u4v2 - 168*L*m5*u4v2 + 8*L*m6*u4v2;

      DUV = 8*pr*DUV;
      }
      break;

 case 3:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real m6 = m3*m3;
      Standard_Real m7 = m3*m4;
      Standard_Real u4 = U2*U2;
      Standard_Real r2 = r*r;
      Standard_Real r3 = r2*r;
      Standard_Real v2 = V*V;
      Standard_Real u2v2 = v2*U2;
      Standard_Real ru2v2 = R*u2v2;
      Standard_Real u4v2 = u4*v2;
      Standard_Real r2u2 = r2*U2;
      Standard_Real r2v2 = r2*v2;
      Standard_Real ru4 = r*u4;

	  // copier-coller de mathematica 
      DUV = 
		  -42336*ru2v2 + 17280*L*ru2v2 + 77952*m*ru2v2 - 42336*L*m*ru2v2 - 52920*m2*ru2v2 + 
     38976*L*m2*ru2v2 + 16800*m3*ru2v2 - 17640*L*m3*ru2v2 - 2520*m4*ru2v2 + 4200*L*m4*ru2v2 + 144*m5*ru2v2 - 
     504*L*m5*ru2v2 + 24*L*m6*ru2v2 - 21168*ru4 + 8640*L*ru4 + 38976*m*ru4 - 21168*L*m*ru4 - 26460*m2*ru4 + 
     19488*L*m2*ru4 + 8400*m3*ru4 - 8820*L*m3*ru4 - 1260*m4*ru4 + 2100*L*m4*ru4 + 72*m5*ru4 - 252*L*m5*ru4 + 
     12*L*m6*ru4 + 9864*r2u2 - 4320*L*r2u2 - 16200*m*r2u2 + 9864*L*m*r2u2 + 9180*m2*r2u2 - 8100*L*m2*r2u2 - 
     2160*m3*r2u2 + 3060*L*m3*r2u2 + 180*m4*r2u2 - 540*L*m4*r2u2 + 36*L*m5*r2u2 + 1644*r2v2 - 720*L*r2v2 - 
     2700*m*r2v2 + 1644*L*m*r2v2 + 1530*m2*r2v2 - 1350*L*m2*r2v2 - 360*m3*r2v2 + 510*L*m3*r2v2 + 30*m4*r2v2 - 
     90*L*m4*r2v2 + 6*L*m5*r2v2 - 450*r3 + 216*L*r3 + 630*m*r3 - 450*L*m*r3 - 270*m2*r3 + 315*L*m2*r3 + 36*m3*r3 - 
     90*L*m3*r3 + 9*L*m4*r3 + 104544*u4v2 - 40320*L*u4v2 - 210112*m*u4v2 + 104544*L*m*u4v2 + 162456*m2*u4v2 - 
     105056*L*m2*u4v2 - 62720*m3*u4v2 + 54152*L*m3*u4v2 + 12880*m4*u4v2 - 15680*L*m4*u4v2 - 1344*m5*u4v2 + 
     2576*L*m5*u4v2 + 56*m6*u4v2 - 224*L*m6*u4v2 + 8*L*m7*u4v2;

      DUV = 16*pr*V*DUV;
      }
      break;

   default:
      break;
    }
  break;

  case 5:
    switch (IV)
    {
    case 0:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real u4 = U2*U2;
      Standard_Real r2 = R*R;
      Standard_Real ru2 = R*U2;

	  // copier-coller de mathematica 
      DUV = 
     -1000*ru2 + 480*L*ru2 + 1400*m*ru2 - 1000*L*m*ru2 - 600*m2*ru2 + 700*L*m2*ru2 + 80*m3*ru2 - 
     200*L*m3*ru2 + 20*L*m4*ru2 + 165*r2 - 90*L*r2 - 180*m*r2 + 165*L*m*r2 + 45*m2*r2 - 90*L*m2*r2 + 15*L*m3*r2 + 
     1096*u4 - 480*L*u4 - 1800*m*u4 + 1096*L*m*u4 + 1020*m2*u4 - 900*L*m2*u4 - 240*m3*u4 + 340*L*m3*u4 + 20*m4*u4 - 
     60*L*m4*u4 + 4*L*m5*u4;
	  
	  DUV = 8*pr*U*DUV;
      }
      break;

   case 1:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real m6 = m3*m3;
      Standard_Real u4 = U2*U2;
      Standard_Real ru2 = r*U2;
      Standard_Real r2 = r*r;


	  // copier-coller de mathematica 
     DUV = 
     5480*ru2 - 2400*L*ru2 - 9000*m*ru2 + 5480*L*m*ru2 + 5100*m2*ru2 - 4500*L*m2*ru2 - 1200*m3*ru2 + 
     1700*L*m3*ru2 + 100*m4*ru2 - 300*L*m4*ru2 + 20*L*m5*ru2 - 750*r2 + 360*L*r2 + 1050*m*r2 - 750*L*m*r2 - 
     450*m2*r2 + 525*L*m2*r2 + 60*m3*r2 - 150*L*m3*r2 + 15*L*m4*r2 - 7056*u4 + 2880*L*u4 + 12992*m*u4 - 7056*L*m*u4 - 
     8820*m2*u4 + 6496*L*m2*u4 + 2800*m3*u4 - 2940*L*m3*u4 - 420*m4*u4 + 700*L*m4*u4 + 24*m5*u4 - 84*L*m5*u4 + 
     4*L*m6*u4;		 

      DUV = 16*pr*U*V*DUV;
      }
      break;

  case 2:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real m6 = m3*m3;
      Standard_Real m7 = m3*m4;
      Standard_Real u4 = U2*U2;
      Standard_Real r2 = r*r;
      Standard_Real r3 = r2*r;
      Standard_Real v2 = V*V;
      Standard_Real u2v2 = v2*U2;
      Standard_Real ru2v2 = R*u2v2;
      Standard_Real u4v2 = u4*v2;
      Standard_Real r2u2 = r2*U2;
      Standard_Real r2v2 = r2*v2;
      Standard_Real ru4 = r*u4;

	  // copier-coller de mathematica 
	  DUV = 
		  
     -70560*ru2v2 + 28800*L*ru2v2 + 129920*m*ru2v2 - 70560*L*m*ru2v2 - 88200*m2*ru2v2 + 
     64960*L*m2*ru2v2 + 28000*m3*ru2v2 - 29400*L*m3*ru2v2 - 4200*m4*ru2v2 + 7000*L*m4*ru2v2 + 240*m5*ru2v2 - 
     840*L*m5*ru2v2 + 40*L*m6*ru2v2 - 7056*ru4 + 2880*L*ru4 + 12992*m*ru4 - 7056*L*m*ru4 - 8820*m2*ru4 + 
     6496*L*m2*ru4 + 2800*m3*ru4 - 2940*L*m3*ru4 - 420*m4*ru4 + 700*L*m4*ru4 + 24*m5*ru4 - 84*L*m5*ru4 + 4*L*m6*ru4 + 
     5480*r2u2 - 2400*L*r2u2 - 9000*m*r2u2 + 5480*L*m*r2u2 + 5100*m2*r2u2 - 4500*L*m2*r2u2 - 1200*m3*r2u2 + 
     1700*L*m3*r2u2 + 100*m4*r2u2 - 300*L*m4*r2u2 + 20*L*m5*r2u2 + 8220*r2v2 - 3600*L*r2v2 - 13500*m*r2v2 + 
     8220*L*m*r2v2 + 7650*m2*r2v2 - 6750*L*m2*r2v2 - 1800*m3*r2v2 + 2550*L*m3*r2v2 + 150*m4*r2v2 - 450*L*m4*r2v2 + 
     30*L*m5*r2v2 - 750*r3 + 360*L*r3 + 1050*m*r3 - 750*L*m*r3 - 450*m2*r3 + 525*L*m2*r3 + 60*m3*r3 - 150*L*m3*r3 + 
     15*L*m4*r3 + 104544*u4v2 - 40320*L*u4v2 - 210112*m*u4v2 + 104544*L*m*u4v2 + 162456*m2*u4v2 - 105056*L*m2*u4v2 - 
     62720*m3*u4v2 + 54152*L*m3*u4v2 + 12880*m4*u4v2 - 15680*L*m4*u4v2 - 1344*m5*u4v2 + 2576*L*m5*u4v2 + 56*m6*u4v2 - 
     224*L*m6*u4v2 + 8*L*m7*u4v2;
	  
     DUV = 16*pr*U*DUV;
      }
      break;

   default:
      break;
    }
  break;

  case 6:
    switch (IV)
    {
    case 0:     
      { 
      Standard_Real m2 = m*m;
      Standard_Real m3 = m2*m;
      Standard_Real m4 = m2*m2;
      Standard_Real m5 = m2*m3;
      Standard_Real m6 = m3*m3;
      Standard_Real u4 = U2*U2;
      Standard_Real u6 = U2*u4;
      Standard_Real r2 = r*r;
      Standard_Real r3 = r2*r;
      Standard_Real r2u2 = r2*U2;
      Standard_Real ru4 = r*u4;
 
	  // copier-coller de mathematica 
      DUV = 
		16440*ru4 - 7200*L*ru4 - 27000*m*ru4 + 16440*L*m*ru4 + 15300*m2*ru4 - 13500*L*m2*ru4 - 
     3600*m3*ru4 + 5100*L*m3*ru4 + 300*m4*ru4 - 900*L*m4*ru4 + 60*L*m5*ru4 - 4500*r2u2 + 2160*L*r2u2 + 6300*m*r2u2 - 
     4500*L*m*r2u2 - 2700*m2*r2u2 + 3150*L*m2*r2u2 + 360*m3*r2u2 - 900*L*m3*r2u2 + 90*L*m4*r2u2 + 165*r3 - 90*L*r3 - 
     180*m*r3 + 165*L*m*r3 + 45*m2*r3 - 90*L*m2*r3 + 15*L*m3*r3 - 14112*u6 + 5760*L*u6 + 25984*m*u6 - 14112*L*m*u6 - 
     17640*m2*u6 + 12992*L*m2*u6 + 5600*m3*u6 - 5880*L*m3*u6 - 840*m4*u6 + 1400*L*m4*u6 + 48*m5*u6 - 168*L*m5*u6 + 
     8*L*m6*u6;

      DUV = 8*pr*DUV;
      }
      break;

   default:
      break;
    }
  break;

  default:
  break;
  }
  
  return DUV * ddu[iu]*ddv[iv];

}


//=======================================================================
//function : UVBox
//purpose  : 
//=======================================================================

void Plate_Plate::UVBox(Standard_Real& UMin, Standard_Real& UMax,
			Standard_Real& VMin, Standard_Real& VMax) const 
{
  Standard_Integer i ;
  const Standard_Real Bmin = 1.e-3;
  UMin =  myConstraints(1).Pnt2d().X();
  UMax =  UMin;
  VMin =  myConstraints(1).Pnt2d().Y();
  VMax =  VMin;

  for( i=2; i<=myConstraints.Length();i++)
  {
    Standard_Real x = myConstraints(i).Pnt2d().X();
    if(x<UMin) UMin = x;
    if(x>UMax) UMax = x;
    Standard_Real y = myConstraints(i).Pnt2d().Y();
    if(y<VMin) VMin = y;
    if(y>VMax) VMax = y; 
  }

  for(i=1; i<=myLXYZConstraints.Length();i++)
    for(Standard_Integer j=1;j<= myLXYZConstraints(i).GetPPC().Length(); j++)
  {
    Standard_Real x = myLXYZConstraints(i).GetPPC()(j).Pnt2d().X();
    if(x<UMin) UMin = x;
    if(x>UMax) UMax = x;
    Standard_Real y = myLXYZConstraints(i).GetPPC()(j).Pnt2d().Y();
    if(y<VMin) VMin = y;
    if(y>VMax) VMax = y; 
  }

  for(i=1; i<=myLScalarConstraints.Length();i++)
    for(Standard_Integer j=1;j<= myLScalarConstraints(i).GetPPC().Length(); j++)
  {
    Standard_Real x = myLScalarConstraints(i).GetPPC()(j).Pnt2d().X();
    if(x<UMin) UMin = x;
    if(x>UMax) UMax = x;
    Standard_Real y = myLScalarConstraints(i).GetPPC()(j).Pnt2d().Y();
    if(y<VMin) VMin = y;
    if(y>VMax) VMax = y; 
  }
 

  if(UMax-UMin < Bmin)
    {
      Standard_Real UM = 0.5*(UMin+UMax);
      UMin = UM - 0.5*Bmin;
      UMax = UM + 0.5*Bmin;
    }
  if(VMax-VMin < Bmin)
    {
      Standard_Real VM = 0.5*(VMin+VMax);
      VMin = VM - 0.5*Bmin;
      VMax = VM + 0.5*Bmin;
    }
}

//=======================================================================
//function : UVConstraints
//purpose  : 
//=======================================================================

void Plate_Plate::UVConstraints(TColgp_SequenceOfXY& Seq) const 
{
  for (Standard_Integer i=1;i<=myConstraints.Length();i++) {
    if ((myConstraints.Value(i).Idu()==0) && (myConstraints.Value(i).Idv()==0))
      Seq.Append((myConstraints.Value(i)).Pnt2d());
  }
}
//=======================================================================

void Plate_Plate::SetPolynomialPartOnly(const Standard_Boolean PPOnly)
{
  PolynomialPartOnly = PPOnly;
}
