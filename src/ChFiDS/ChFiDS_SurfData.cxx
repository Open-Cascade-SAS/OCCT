// Created on: 1993-11-29
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
#include <ChFiDS_FaceInterference.hxx>
#include <ChFiDS_SurfData.hxx>
#include <gp_Pnt2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ChFiDS_SurfData,Standard_Transient)

ChFiDS_SurfData::ChFiDS_SurfData () : 
indexOfS1(0),indexOfS2(0),indexOfConge(0),
isoncurv1(0),isoncurv2(0),twistons1(0),twistons2(0)
{}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::Copy(const Handle(ChFiDS_SurfData)& Other)
{
indexOfS1    = Other->indexOfS1;
indexOfS2    = Other->indexOfS2;
indexOfConge = Other->indexOfConge;
orientation  = Other->orientation;
intf1        = Other->intf1;
intf2        = Other->intf2;

pfirstOnS1   = Other->pfirstOnS1;
plastOnS1    = Other->plastOnS1; 
pfirstOnS2   = Other->pfirstOnS2;   
plastOnS2    = Other->plastOnS2;  

ufspine      = Other->ufspine;
ulspine      = Other->ulspine;

simul        = Other->simul; 
p2df1        = Other->p2df1;
p2dl1        = Other->p2dl1;
p2df2        = Other->p2df2; 
p2dl2        = Other->p2dl2;

myfirstextend = Other->myfirstextend;
mylastextend  = Other->mylastextend;

twistons1 = Other->twistons1;
twistons2 = Other->twistons2;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_SurfData::Index(const Standard_Integer OfS) const
{
  if(OfS == 1) return indexOfS1;
  else return indexOfS2;
}

//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

const ChFiDS_FaceInterference& ChFiDS_SurfData::Interference
(const Standard_Integer OnS) const
{
  if(OnS == 1) return intf1;
  else return intf2;
}


//=======================================================================
//function : Interference
//purpose  : 
//=======================================================================

ChFiDS_FaceInterference& ChFiDS_SurfData::ChangeInterference
(const Standard_Integer OnS)
{
  if(OnS == 1) return intf1;
  else return intf2;
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const ChFiDS_CommonPoint&  ChFiDS_SurfData::Vertex
(const Standard_Boolean First,const Standard_Integer OnS) const
{
  if(First && OnS == 1) return pfirstOnS1;
  else if(First && OnS == 2) return pfirstOnS2;
  else if(!First && OnS == 1) return plastOnS1;
  else return plastOnS2;
}


//=======================================================================
//function : ChangeVertex
//purpose  : 
//=======================================================================

ChFiDS_CommonPoint&  ChFiDS_SurfData::ChangeVertex
(const Standard_Boolean First,const Standard_Integer OnS)
{
  if(First && OnS == 1) return pfirstOnS1;
  else if(First && OnS == 2) return pfirstOnS2;
  else if(!First && OnS == 1) return plastOnS1;
  else return plastOnS2;
}


//=======================================================================
//function : FirstSpineParam
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_SurfData::FirstSpineParam()const
{
  return ufspine;
}

//=======================================================================
//function : LastSpineParam
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_SurfData::LastSpineParam()const
{
  return ulspine;
}

//=======================================================================
//function : FirstSpineParam
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::FirstSpineParam(const Standard_Real Par)
{
  ufspine = Par;
}

//=======================================================================
//function : LastSpineParam
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::LastSpineParam(const Standard_Real Par)
{
  ulspine = Par;
}

//=======================================================================
//function : FirstExtensionValue
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_SurfData::FirstExtensionValue()const
{
  return myfirstextend;
}

//=======================================================================
//function : LastExtensionValue
//purpose  : 
//=======================================================================

Standard_Real ChFiDS_SurfData::LastExtensionValue()const
{
  return mylastextend;
}

//=======================================================================
//function : FirstExtensionValue
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::FirstExtensionValue(const Standard_Real Extend)
{
  myfirstextend=Extend;
}

//=======================================================================
//function : LastExtensionValue
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::LastExtensionValue(const Standard_Real Extend)
{
  mylastextend=Extend;
}

//=======================================================================
//function : Simul
//purpose  : 
//=======================================================================

Handle(Standard_Transient) ChFiDS_SurfData::Simul() const 
{
  return simul;
}


//=======================================================================
//function : SetSimul
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::SetSimul(const Handle(Standard_Transient)& S)
{
  simul = S;
}

//=======================================================================
//function : ResetSimul
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::ResetSimul()
{
  simul.Nullify();
}


//=======================================================================
//function : Get2dPoints
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::Get2dPoints(gp_Pnt2d& P2df1,
				  gp_Pnt2d& P2dl1,
				  gp_Pnt2d& P2df2,
				  gp_Pnt2d& P2dl2) const 
{
  P2df1 = p2df1;
  P2dl1 = p2dl1;
  P2df2 = p2df2;
  P2dl2 = p2dl2;
}

//=======================================================================
//function : Get2dPoints
//purpose  : 
//=======================================================================

gp_Pnt2d ChFiDS_SurfData::Get2dPoints(const Standard_Boolean First,
				      const Standard_Integer OnS) const 

{
  if(First && OnS == 1) return p2df1;
  else if(!First && OnS == 1) return p2dl1;
  else if(First && OnS == 2) return p2df2;
  return p2dl2;
}

//=======================================================================
//function : Set2dPoints
//purpose  : 
//=======================================================================

void ChFiDS_SurfData::Set2dPoints(const gp_Pnt2d& P2df1,
				  const gp_Pnt2d& P2dl1,
				  const gp_Pnt2d& P2df2,
				  const gp_Pnt2d& P2dl2)
{
  p2df1 = P2df1;
  p2dl1 = P2dl1;
  p2df2 = P2df2;
  p2dl2 = P2dl2;
}

