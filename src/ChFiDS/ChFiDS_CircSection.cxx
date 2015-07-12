// Created on: 1996-03-06
// Created by: Laurent BOURESCHE
// Copyright (c) 1996-1999 Matra Datavision
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


#include <ChFiDS_CircSection.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>

//=======================================================================
//function : ChFiDS_CircSection
//purpose  : 
//=======================================================================
ChFiDS_CircSection::ChFiDS_CircSection(){}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Circ&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myCirc = C;
  myF    = F;
  myL    = L;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Lin&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myLin  = C;
  myF    = F;
  myL    = L;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Circ&       C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myCirc;
  F = myF;
  L = myL;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Lin&        C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myLin;
  F = myF;
  L = myL;
}
