// Created on: 1994-12-16
// Created by: Laurent BUCHARD
// Copyright (c) 1994-1999 Matra Datavision
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

//  Modified by skv - Fri Jul 14 17:03:47 2006 OCC12627

#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dHatch_Element.hxx>
#include <Geom2dHatch_Elements.hxx>
#include <gp.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Integer.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <TopAbs_Orientation.hxx>

Geom2dHatch_Elements::Geom2dHatch_Elements(const Geom2dHatch_Elements& )
{
#ifdef OCCT_DEBUG
  cout<<" Magic Constructor in Geom2dHatch_Elements:: "<<endl;
#endif
}

Geom2dHatch_Elements::Geom2dHatch_Elements()
{
  NumWire = 0;
  NumEdge = 0;
  myCurEdge = 1;
}

void Geom2dHatch_Elements::Clear()
{
  myMap.Clear();
}

Standard_Boolean Geom2dHatch_Elements::IsBound(const Standard_Integer K) const
{
  return(myMap.IsBound(K));
}

Standard_Boolean Geom2dHatch_Elements::UnBind(const Standard_Integer K)
{
  return(myMap.UnBind(K));
}

Standard_Boolean Geom2dHatch_Elements::Bind(const Standard_Integer K,const Geom2dHatch_Element& I)
{
  return(myMap.Bind(K,I));
}

const Geom2dHatch_Element& Geom2dHatch_Elements::Find(const Standard_Integer K) const
{
  return(myMap.Find(K));
}

Geom2dHatch_Element& Geom2dHatch_Elements::ChangeFind(const Standard_Integer K)
{
  return(myMap.ChangeFind(K));
}

//=======================================================================
//function : CheckPoint
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::CheckPoint(gp_Pnt2d&)
{
  return Standard_True;
}

//=======================================================================
//function : Reject
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::Reject(const gp_Pnt2d&) const  {
  return Standard_False;
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dHatch_Elements::Segment(const gp_Pnt2d& P, 
					             gp_Lin2d& L, 
					             Standard_Real& Par)
{
  myCurEdge = 1;

  return OtherSegment(P, L, Par);
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dHatch_Elements::OtherSegment(const gp_Pnt2d& P, 
						          gp_Lin2d& L, 
						          Standard_Real& Par)
{
  Geom2dHatch_DataMapIteratorOfMapOfElements Itertemp;
  Standard_Integer                        i;
  
  for(  Itertemp.Initialize(myMap), i = 1; Itertemp.More(); Itertemp.Next(), i++) { 
    if (i < myCurEdge)
      continue;

    void *ptrmyMap = (void *)(&myMap);
    Geom2dHatch_Element& Item=((Geom2dHatch_MapOfElements*)ptrmyMap)->ChangeFind(Itertemp.Key());
    Geom2dAdaptor_Curve& E = Item.ChangeCurve();
    TopAbs_Orientation Or= Item.Orientation();
    gp_Pnt2d P2 = E.Value
      ((E.FirstParameter() + E.LastParameter()) *0.5);
    if ((Or == TopAbs_FORWARD) ||
	(Or == TopAbs_REVERSED)) {
      gp_Vec2d V(P,P2);
      Par = V.Magnitude();
      if (Par >= gp::Resolution()) {
	L = gp_Lin2d(P,V);
	myCurEdge++;
	return Standard_True;
      }
    }
  }

  if (i == myCurEdge + 1) {
    Par = RealLast();
    L = gp_Lin2d(P,gp_Dir2d(1,0));
    myCurEdge++;

    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
//function : InitWires
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::InitWires()  {
  NumWire = 0;
}

//=======================================================================
//function : RejectWire NYI
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dHatch_Elements::RejectWire(const gp_Lin2d& , 
						   const Standard_Real) const 
{
  return Standard_False;
}

//=======================================================================
//function : InitEdges
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::InitEdges()  {
  NumEdge = 0;
  Iter.Initialize(myMap);
}

//=======================================================================
//function : RejectEdge NYI
//purpose  : 
//=======================================================================

Standard_Boolean Geom2dHatch_Elements::RejectEdge(const gp_Lin2d& , 
						  const Standard_Real ) const 
{
  return Standard_False;
}


//=======================================================================
//function : CurrentEdge
//purpose  : 
//=======================================================================

void  Geom2dHatch_Elements::CurrentEdge(Geom2dAdaptor_Curve& E, 
					TopAbs_Orientation& Or) const 
{
  void *ptrmyMap = (void *)(&myMap);
  Geom2dHatch_Element& Item=((Geom2dHatch_MapOfElements*)ptrmyMap)->ChangeFind(Iter.Key());

  E = Item.ChangeCurve();
  Or= Item.Orientation();
#if 0 
  E.Edge() = TopoDS::Edge(myEExplorer.Current());
  E.Face() = myFace;
  Or = E.Edge().Orientation();
#endif
}


//=======================================================================
//function : MoreWires
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::MoreWires() const 
{
  return (NumWire == 0);
}

//=======================================================================
//function : NextWire
//purpose  : 
//=======================================================================

void Geom2dHatch_Elements::NextWire()  {
  NumWire++;
}

//=======================================================================
//function : MoreEdges
//purpose  : 
//=======================================================================

Standard_Boolean  Geom2dHatch_Elements::MoreEdges() const  {
  return(Iter.More());
}

//=======================================================================
//function : NextEdge
//purpose  : 
//=======================================================================

void Geom2dHatch_Elements::NextEdge()  {
  Iter.Next();
}



