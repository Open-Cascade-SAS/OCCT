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


#include <gp_GTrsf2d.ixx>
#include <Standard_ConstructionError.hxx>
#include <Precision.hxx>

void gp_GTrsf2d::SetAffinity (const gp_Ax2d& A,
			      const Standard_Real Ratio)
{
  shape = gp_Other;
  scale = 0.0;
  Standard_Real a = A.Direction().X();
  Standard_Real b = A.Direction().Y();
  matrix.SetValue (1, 1, (1.0 - Ratio)*a*a + Ratio);
  matrix.SetValue (2, 2, (1.0 - Ratio)*b*b + Ratio);
  matrix.SetValue (1, 2, (1.0 - Ratio)*a*b);
  matrix.SetValue (2, 1, matrix.Value (1, 2));
  loc = A.Location().XY();
  loc.Reverse();
  loc.Multiply (matrix);
  loc.Add (A.Location().XY());
}

void gp_GTrsf2d::SetTranslationPart (const gp_XY& Coord)
{
  loc = Coord;
  if (Form() == gp_CompoundTrsf || Form() == gp_Other || 
      Form() == gp_Translation) { }
  else if (Form() == gp_Identity) { shape = gp_Translation; }
  else { shape = gp_CompoundTrsf; }
}

void gp_GTrsf2d::Invert ()
{
  if (shape == gp_Other) {
    matrix.Invert();
    loc.Multiply (matrix);
    loc.Reverse();
  }
  else {
    gp_Trsf2d T = Trsf2d ();
    T.Invert ();
    SetTrsf2d (T);
  }
}

void gp_GTrsf2d::Multiply (const gp_GTrsf2d& T)
{
  if (Form() == gp_Other || T.Form() == gp_Other) {
    shape = gp_Other;
    loc.Add (T.loc.Multiplied (matrix));
    matrix.Multiply(T.matrix);
  }
  else {
    gp_Trsf2d T1 = Trsf2d();
    gp_Trsf2d T2 = T.Trsf2d();
    T1.Multiply(T2);
    matrix = T1.matrix;
    loc = T1.loc;
    scale = T1.scale;
    shape = T1.shape;
  }
}

void gp_GTrsf2d::Power (const Standard_Integer N)
{
  if (N == 0)  {
    scale = 1.0;
    shape = gp_Identity;
    matrix.SetIdentity();
    loc = gp_XY (0., 0.);
  }
  else if (N == 1) { }
  else if (N == -1) { Invert(); }
  else {
    if (N < 0) { Invert(); }
    if (shape == gp_Other) {
      Standard_Integer Npower = N;
      if (Npower < 0) Npower = - Npower;
      Npower--;
      gp_XY Temploc = loc;
//      Standard_Real Tempscale = scale;
      gp_Mat2d Tempmatrix (matrix);
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
      gp_Trsf2d T = Trsf2d ();
      T.Power (N);
      SetTrsf2d (T);
    }
  }
}

void gp_GTrsf2d::PreMultiply (const gp_GTrsf2d& T)
{
  if (Form() == gp_Other || T.Form() == gp_Other) {
    shape = gp_Other;
    loc.Multiply (T.matrix);
    loc.Add (T.loc);
    matrix.PreMultiply(T.matrix);
  }
  else {
    gp_Trsf2d T1 = Trsf2d();
    gp_Trsf2d T2 = T.Trsf2d();
    T1.PreMultiply(T2);
    matrix = T1.matrix;
    loc = T1.loc;
    scale = T1.scale;
    shape = T1.shape;
  }
}

gp_Trsf2d gp_GTrsf2d::Trsf2d () const
{
  gp_Trsf2d T;
  Standard_Real value;
#ifndef No_Exception
  Standard_Real tolerance = Precision::Angular() ;
  Standard_Real tol2 = 2. * tolerance;
#endif

  Standard_ConstructionError_Raise_if
    (Form() == gp_Other," gp_GTrsf2d::Trsf2d() - non-orthogonal GTrsf2d (0)");


//Test of orthogonality

  value = (matrix.Value(1,1) * matrix.Value(1,1) +
	   matrix.Value(2,1) * matrix.Value(2,1)) ;
  Standard_ConstructionError_Raise_if
    (Abs(value - 1.) > tol2," gp_GTrsf2d::Trsf2d() - non-orthogonal GTrsf2d (1)");

  value = (matrix.Value(1,2) * matrix.Value(1,2) +
	   matrix.Value(2,2) * matrix.Value(2,2));
  Standard_ConstructionError_Raise_if
    (Abs(value - 1.) > tol2," gp_GTrsf2d::Trsf2d() - non-orthogonal GTrsf2d (2)");

  value = (matrix.Value(1,1) * matrix.Value(1,2) +
	   matrix.Value(2,1) * matrix.Value(2,2));
  Standard_ConstructionError_Raise_if
    (Abs(value) > tolerance," gp_GTrsf2d::Trsf2d() - non-orthogonal GTrsf2d (3)");
//

  T.matrix = matrix ;
  T.shape = shape;
  T.scale = scale ;
  T.loc = loc;

  return T;
}

