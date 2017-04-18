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


#include <gce_MakeHypr2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <StdFail_NotDone.hxx>

//=========================================================================
//   Creation d une Hyperbola 2d de gp de centre <Center> et de sommets   +
//   <S1> et <S2>.                                                        +
//   <CenterS1> donne le grand axe .                                      +
//   <S1> donne le grand rayon et <S2> le petit rayon.                    +
//=========================================================================
gce_MakeHypr2d::gce_MakeHypr2d(const gp_Pnt2d&   S1     ,
			       const gp_Pnt2d&   S2     ,
			       const gp_Pnt2d&   Center )
{
  gp_Dir2d XAxis(gp_XY(S1.XY()-Center.XY()));
  gp_Dir2d YAxis(gp_XY(S2.XY()-Center.XY()));
  gp_Ax22d Axis(Center,XAxis,YAxis);
  gp_Lin2d L(Center,XAxis);
  Standard_Real D1 = S1.Distance(Center);
  Standard_Real D2 = L.Distance(S2);
  if (D1 >= D2) {
    TheHypr2d = gp_Hypr2d(Axis,D1,D2);
    TheError = gce_Done;
  }
  else { TheError = gce_InvertAxis; }
}

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax2d&         MajorAxis   ,
			       const Standard_Real    MajorRadius ,
			       const Standard_Real    MinorRadius ,
			       const Standard_Boolean Sense       )
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0) { TheError = gce_NegativeRadius;}
  else {
    TheHypr2d = gp_Hypr2d(MajorAxis,MajorRadius,MinorRadius,Sense);
    TheError = gce_Done;
  }
}

gce_MakeHypr2d::gce_MakeHypr2d(const gp_Ax22d&     A           ,
			       const Standard_Real MajorRadius ,
			       const Standard_Real MinorRadius )
{
  if (MajorRadius < 0.0 || MinorRadius < 0.0) { TheError = gce_NegativeRadius;}
  else {
    TheHypr2d = gp_Hypr2d(A,MajorRadius,MinorRadius);
    TheError = gce_Done;
  }
}

const gp_Hypr2d& gce_MakeHypr2d::Value() const
{ 
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeHypr2d::Value() - no result");
  return TheHypr2d;
}

const gp_Hypr2d& gce_MakeHypr2d::Operator() const 
{
  return Value();
}

gce_MakeHypr2d::operator gp_Hypr2d() const
{
  return Value();
}

