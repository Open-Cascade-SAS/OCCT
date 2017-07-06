// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
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


#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopOpeBRepDS.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_Transition.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopOpeBRepDS_Interference,Standard_Transient)

//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================
TopOpeBRepDS_Interference::TopOpeBRepDS_Interference()
{
}

//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference
    (const TopOpeBRepDS_Transition& T, 
     const TopOpeBRepDS_Kind ST,
     const Standard_Integer S, 
     const TopOpeBRepDS_Kind GT, 
     const Standard_Integer G) :
     myTransition(T),
     mySupport(S),
     myGeometry(G),
     mySupportType(ST),
     myGeometryType(GT)
{
}

//=======================================================================
//function : TopOpeBRepDS_Interference
//purpose  : 
//=======================================================================

TopOpeBRepDS_Interference::TopOpeBRepDS_Interference
    (const Handle(TopOpeBRepDS_Interference)& I ) :
    myTransition(I->Transition()),
    mySupport(I->Support()),
    myGeometry(I->Geometry()),
    mySupportType(I->SupportType()),
    myGeometryType(I->GeometryType())
{
}


//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

const TopOpeBRepDS_Transition& TopOpeBRepDS_Interference::Transition() const 
{
  return myTransition;
}


//=======================================================================
//function : ChangeTransition
//purpose  : 
//=======================================================================

TopOpeBRepDS_Transition&  TopOpeBRepDS_Interference::ChangeTransition()
{
  return myTransition;
}

//=======================================================================
//function : Transition
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Transition(const TopOpeBRepDS_Transition& T)
{
  myTransition = T;
}

//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Interference::GKGSKS(TopOpeBRepDS_Kind& GK,
				       Standard_Integer& G,
				       TopOpeBRepDS_Kind& SK,
				       Standard_Integer& S) const
{
  GK = myGeometryType;
  G = myGeometry;
  SK = mySupportType;
  S = mySupport;
}

//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind  TopOpeBRepDS_Interference::SupportType()const 
{
  return mySupportType;
}


//=======================================================================
//function : Support
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_Interference::Support()const 
{
  return mySupport;
}


//=======================================================================
//function : GeometryType
//purpose  : 
//=======================================================================

TopOpeBRepDS_Kind  TopOpeBRepDS_Interference::GeometryType()const 
{
  return myGeometryType;
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

Standard_Integer  TopOpeBRepDS_Interference::Geometry()const 
{
  return myGeometry;
}

//=======================================================================
//function : SetGeometry
//purpose  : 
//=======================================================================

void TopOpeBRepDS_Interference::SetGeometry(const Standard_Integer GI)
{
  myGeometry = GI;
}


//=======================================================================
//function : SupportType
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::SupportType(const TopOpeBRepDS_Kind ST)
{
  mySupportType = ST;
}


//=======================================================================
//function : Support
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Support(const Standard_Integer S)
{
  mySupport = S;
}


//=======================================================================
//function : GeometryType
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::GeometryType(const TopOpeBRepDS_Kind GT)
{
  myGeometryType = GT;
}


//=======================================================================
//function : Geometry
//purpose  : 
//=======================================================================

void  TopOpeBRepDS_Interference::Geometry(const Standard_Integer G)
{
  myGeometry = G;
}

//=======================================================================
//function : HasSameSupport 
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepDS_Interference::HasSameSupport(const Handle(TopOpeBRepDS_Interference)& I)
const
{
  return (mySupportType == I->mySupportType && mySupport == I->mySupport);
}

//=======================================================================
//function : HasSameGeometry
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepDS_Interference::HasSameGeometry(const Handle(TopOpeBRepDS_Interference)& I)
const
{
  return (myGeometryType == I->myGeometryType && myGeometry == I->myGeometry);
}
