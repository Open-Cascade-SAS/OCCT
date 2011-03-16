// File:	gce_MakeParab.cxx
// Created:	Wed Sep  2 12:35:36 1992
// Author:	Remi GILET
//		<reg@sdsun1>

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

