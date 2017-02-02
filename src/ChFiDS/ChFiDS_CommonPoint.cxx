// Created on: 1993-11-30
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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


#include <ChFiDS_CommonPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_TypeMismatch.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : ChFiDS_CommonPoint
//purpose  : 
//=======================================================================
ChFiDS_CommonPoint::ChFiDS_CommonPoint() : 
tol(0.),
isonarc(Standard_False),
isvtx(Standard_False),
hasvector(Standard_False)
{
}


//=======================================================================
//function : Reset
//purpose  : 
//=======================================================================

void ChFiDS_CommonPoint::Reset()
{
  tol = 0.;
  isvtx  = Standard_False;
  isonarc = Standard_False;
  hasvector = Standard_False;
}


//=======================================================================
//function : SetArc
//purpose  : 
// 30/09/1997 : PMN On n'ecrabouille plus la tolerance
//=======================================================================

void  ChFiDS_CommonPoint::SetArc(const Standard_Real Tol, 
				const TopoDS_Edge& A, 
				const Standard_Real Param, 
				const TopAbs_Orientation TArc)
{
  isonarc = Standard_True;
  if (Tol > tol) tol = Tol;
  arc     = A;
  prmarc     = Param;
  traarc  = TArc;
}

//=======================================================================
//function : SetParameter
//purpose  : 
//=======================================================================

void  ChFiDS_CommonPoint::SetParameter(const Standard_Real Param)
{
  prmtg     = Param;
}

//=======================================================================
//function : Arc
//purpose  : 
//=======================================================================

const TopoDS_Edge&  ChFiDS_CommonPoint::Arc()const 
{
  if (!isonarc){
    throw Standard_DomainError("CommonPoint not on Arc");
  }
  return arc;
}


//=======================================================================
//function : TransitionOnArc
//purpose  : 
//=======================================================================

TopAbs_Orientation  ChFiDS_CommonPoint::TransitionOnArc()const 
{
  if (!isonarc){
    throw Standard_DomainError("CommonPoint not on Arc");
  }
  return traarc;
}


//=======================================================================
//function : ParameterOnArc
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_CommonPoint::ParameterOnArc()const 
{
  if (!isonarc){
    throw Standard_DomainError("CommonPoint not on Arc");
  }
  return prmarc;
}

//=======================================================================
//function : Parameter
//purpose  : 
//=======================================================================

Standard_Real  ChFiDS_CommonPoint::Parameter()const 
{
  return prmtg;
}


