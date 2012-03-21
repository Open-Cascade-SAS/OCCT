// Created on: 1992-09-02
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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


#include <gce_MakeParab.ixx>
#include <gp.hxx>
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
  StdFail_NotDone_Raise_if(!TheError == gce_Done,"");
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

