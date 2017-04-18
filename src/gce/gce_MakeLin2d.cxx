// Created on: 1992-09-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gce_MakeLin2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une ligne 2d de gp a partir d un Ax2d de gp.              +
//=========================================================================
gce_MakeLin2d::gce_MakeLin2d(const gp_Ax2d& A)
{
  TheLin2d = gp_Lin2d(A);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp a partir de son origine P (Pnt2d de gp)+
//   et d une direction V (Dir2d de gp).                                  +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Pnt2d& P,
			     const gp_Dir2d& V)
{
  TheLin2d = gp_Lin2d(P,V);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp a partir des parametres de son         +
//    equation.                                                           +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const Standard_Real A,
			     const Standard_Real B,
			     const Standard_Real C)
{
  if (A*A + B*B <= gp::Resolution()) {
    TheError = gce_NullAxis;
  }
  else {
    TheLin2d = gp_Lin2d(A,B,C);
    TheError = gce_Done;
  }
}

//=========================================================================
//   Creation d une ligne 2d de gp passant par les deux points <P1> et    +
//   <P2>.                                                                +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Pnt2d& P1,
			     const gp_Pnt2d& P2)
{
  if (P1.Distance(P2) >= gp::Resolution()) {
    TheLin2d = gp_Lin2d(P1,gp_Dir2d(P2.XY()-P1.XY()));
    TheError = gce_Done;
  }
  else { 
    TheError = gce_ConfusedPoints;
  }
}

//=========================================================================
//   Creation d une ligne 2d de gp <TheLine> parallele a une autre ligne  +
//   <Line1> passant par le point <Point1>.                               +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Lin2d& Line,
			     const gp_Pnt2d& Point)
{
  TheLin2d = gp_Lin2d(Point,Line.Direction());
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 2d de gp <TheLine> parallele a une autre ligne  +
//   <Line1> a une distance <Dist1>.                                      +
//=========================================================================

gce_MakeLin2d::gce_MakeLin2d(const gp_Lin2d&     Line,
			     const Standard_Real Dist)
{
  gp_Pnt2d Point(Line.Location().XY()+
		 Dist*gp_XY(-Line.Direction().Y(),Line.Direction().X()));
  TheLin2d = gp_Lin2d(Point,Line.Direction());
  TheError = gce_Done;
}

gp_Lin2d gce_MakeLin2d::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeLin2d::Value() - no result");
  return TheLin2d;
}

gp_Lin2d gce_MakeLin2d::Operator() const 
{
  return Value();
}

gce_MakeLin2d::operator gp_Lin2d () const
{
  return Value();
}

