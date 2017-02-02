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

#define No_Standard_OutOfRange


#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>

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
      for(;;) {
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

  if ( Abs(s) < gp::Resolution() )
    throw Standard_ConstructionError("gp_GTrsf::SetForm, null determinant");

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

  for (Standard_Integer i = 1; i <= 3; i++)
    for (Standard_Integer j = 1; j <= 3; j++)
      if ( Abs( TM.Value(i, j) ) > tol )
      {
        shape = gp_Other;
        return;
      }
}
