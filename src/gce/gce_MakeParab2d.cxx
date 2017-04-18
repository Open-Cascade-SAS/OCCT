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


#include <gce_MakeParab2d.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax22d&     A     ,
				 const Standard_Real Focal ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(A,Focal);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&         MirrorAxis ,
				 const Standard_Real    Focal      ,
				 const Standard_Boolean Sense      ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab2d = gp_Parab2d(MirrorAxis,Focal,Sense);
    TheError = gce_Done;
  }
}

gce_MakeParab2d::gce_MakeParab2d(const gp_Ax2d&  D            ,
				 const gp_Pnt2d& F            ,
				 const Standard_Boolean Sense )
{
  TheParab2d = gp_Parab2d(D,F,Sense);
  TheError = gce_Done;
}

//=========================================================================
//   Creation d une Parabole 2d de gp de centre <Center> et de sommet     +
//   <S1> .                                                               +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne la focale.                                                +
//=========================================================================

gce_MakeParab2d::gce_MakeParab2d(const gp_Pnt2d&        S      ,
				 const gp_Pnt2d&        Center ,
				 const Standard_Boolean Sense  ) 
{
  if (S.Distance(Center) >= gp::Resolution()) {
    gp_Dir2d XAxis(gp_XY(S.XY()-Center.XY()));
    TheParab2d = gp_Parab2d(gp_Ax2d(Center,XAxis),S.Distance(Center),Sense);
    TheError = gce_Done;
  }
  else { TheError = gce_NullAxis; }
}

const gp_Parab2d& gce_MakeParab2d::Value () const
{
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeParab2d::Value() - no result");
  return TheParab2d;
}

const gp_Parab2d& gce_MakeParab2d::Operator() const 
{
  return Value();
}

gce_MakeParab2d::operator gp_Parab2d() const
{
  return Value();
}

