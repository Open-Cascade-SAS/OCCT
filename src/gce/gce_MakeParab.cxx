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


#include <gce_MakeParab.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <StdFail_NotDone.hxx>

gce_MakeParab::gce_MakeParab(const gp_Ax2&       A2    ,
			     const Standard_Real Focal ) 
{
  if (Focal < 0.0) { TheError = gce_NullFocusLength; }
  else {
    TheParab = gp_Parab(A2,Focal);
    TheError = gce_Done;
  }
}

gce_MakeParab::gce_MakeParab(const gp_Ax1& D ,
			     const gp_Pnt& F )
{
  TheParab = gp_Parab(D,F);
  TheError = gce_Done;
}

const gp_Parab& gce_MakeParab::Value () const
{
  StdFail_NotDone_Raise_if (TheError != gce_Done,
                            "gce_MakeParab::Value() - no result");
  return TheParab;
}

const gp_Parab& gce_MakeParab::Operator() const 
{
  return Value();
}

gce_MakeParab::operator gp_Parab() const
{
  return Value();
}

