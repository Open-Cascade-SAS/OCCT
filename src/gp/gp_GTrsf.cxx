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


#define No_Standard_OutOfRange

#include <gp_GTrsf.ixx>
#include <gp_Mat.hxx>

void gp_GTrsf::SetTranslationPart (const gp_XYZ& Coord)
{
  loc = Coord;
  if (Form() == gp_CompoundTrsf || Form() == gp_Other || 
      Form() == gp_Translation)   { }
  else if (Form() == gp_Identity) { shape = gp_Translation; }
  else                            { shape = gp_CompoundTrsf; }
}

void gp_GTrsf::Invert ()
{
  if (shape == gp_Other) {
    matrix.Invert() ;
    loc.Multiply (matrix);
    loc.Reverse();
  }
  else {
    gp_Trsf T = Trsf();
    T.Invert ();
    SetTrsf (T);       
  }
}

void gp_GTrsf::Multiply (const gp_GTrsf& T)
{
  if (Form() == gp_Other || T.Form() == gp_Other) {
    shape = gp_Other;
    loc.Add (T.loc.Multiplied (matrix));
    matrix.Multiply(T.matrix);
  }
  else {
    gp_Trsf T1 = Trsf();
    gp_Trsf T2 = T.Trsf();
    T1.Multiply(T2);
    matrix = T1.matrix;
    loc = T1.loc;
    scale = T1.scale;
    shape = T1.shape;
  }
}

void gp_GTrsf::Power (const Standard_Integer N)
{
  if (N == 0)  {
    scale = 1.;
    shape = gp_Identity;
    matrix.SetIdentity();
    loc = gp_XYZ (0.,0.,0.);
  }
  else if (N == 1) { }
  else if (N == -1) { Invert(); }
  else {
    if (shape == gp_Other) {
      Standard_Integer Npower = N;
      if (Npower < 0) Npower = - Npower;
      Npower--;
      gp_XYZ Temploc = loc;
//      Standard_Real Tempscale = scale;
      gp_Mat Tempmatrix (matrix);
      while (1) {
	if (IsOdd(Npower)) {
	  loc.Add (Temploc.Multiplied (matrix));
	  matrix.Multiply (Tempmatrix);
	}
	if (Npower == 1) { break; }
	Temploc.Add (Temploc.Multiplied (Tempmatrix));
	Tempmatrix.Multiply (Tempmatrix);
	Npower = Npower/2;
      }
    }
    else {
      gp_Trsf T = Trsf ();
      T.Power (N);
      SetTrsf (T);
    }
  }
}

void gp_GTrsf::PreMultiply (const gp_GTrsf& T)
{
  if (Form() == gp_Other || T.Form() == gp_Other) {
    shape = gp_Other;
    loc.Multiply (T.matrix);
    loc.Add (T.loc);
    matrix.PreMultiply(T.matrix);
  }
  else {
    gp_Trsf T1 = Trsf();
    gp_Trsf T2 = T.Trsf();
    T1.PreMultiply(T2);
    matrix = T1.matrix;
    loc = T1.loc;
    scale = T1.scale;
    shape = T1.shape;
  }
}

void gp_GTrsf::SetForm()
{
  Standard_Real tol = 1.e-12; // Precision::Angular();
  //
  // don t trust the initial values !
  //
  gp_Mat M(matrix);
  Standard_Real s = M.Determinant();
  Standard_Real As = s;
  if (As < 0) As = - As;
  Standard_ConstructionError_Raise_if
    (As < gp::Resolution(),"gp_GTrsf::SetForm, null determinant");
  if (s > 0)
    s = Pow(s,1./3.);
  else
    s = -Pow(-s,1./3.);
  M.Divide(s);
  
  // check if the matrix is an uniform matrix
  // the transposition should be the invert.
  gp_Mat TM(M);
  TM.Transpose();
  TM.Multiply(M);
  gp_Mat anIdentity ;
  anIdentity.SetIdentity() ;
  TM.Subtract(anIdentity);
  if (shape==gp_Other) shape = gp_CompoundTrsf;

  Standard_Integer i, j;
  for (i=1; i<=3; i++) {
    for (j=1; j<=3; j++) {
      As = TM.Value(i,j);
      if (As < 0) As = - As;
      if (As > tol) {
	shape = gp_Other;
	return;
      }
    }
  }
}
