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


#include <gce_MakeLin.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une ligne 3d de gp a partir d un Ax1 de gp.               +
//=========================================================================
gce_MakeLin::gce_MakeLin(const gp_Ax1& A1)
{
  TheLin = gp_Lin(A1);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp a partir de son origine P (Pnt de gp)  +
//   et d une direction V (Dir de gp).                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P,
			 const gp_Dir& V)
{
  TheLin = gp_Lin(P,V);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une ligne 3d de gp passant par les deux points <P1> et    +
//   <P2>.                                                                +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Pnt& P1 ,
			 const gp_Pnt& P2 ) 
{
  if (P1.Distance(P2) >= gp::Resolution()) {
    TheLin = gp_Lin(P1,gp_Dir(P2.XYZ()-P1.XYZ()));
    TheError = gce_Done;
  }
  else { TheError = gce_ConfusedPoints; }
}

//=========================================================================
//   Creation d une ligne 3d de gp parallele a une autre <Lin> et passant +
//   par le point <P>.                                                    +
//=========================================================================

gce_MakeLin::gce_MakeLin(const gp_Lin& Lin ,
			 const gp_Pnt& P   )
{
  TheLin = gp_Lin(P,Lin.Direction());
  TheError = gce_Done;
}

const gp_Lin& gce_MakeLin::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeLin::Value() - no result");
  return TheLin;
}

const gp_Lin& gce_MakeLin::Operator() const 
{
  return Value();
}

gce_MakeLin::operator gp_Lin() const
{
  return Value();
}

