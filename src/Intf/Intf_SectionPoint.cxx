// Created on: 1991-06-24
// Created by: Didier PIFFAULT
// Copyright (c) 1991-1999 Matra Datavision
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


#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <Intf_SectionPoint.hxx>

#define DEBUG_INTFSECTIONPOINT 0


//=======================================================================
//function : Pnt
//purpose  : 
//=======================================================================

const gp_Pnt& Intf_SectionPoint::Pnt () const {return myPnt;}


//=======================================================================
//function : GetElemO
//purpose  : 
//=======================================================================

void Intf_SectionPoint::InfoFirst (Intf_PIType& Dim,
				   Standard_Integer&     Add1,
				   Standard_Integer&     Add2,
				   Standard_Real&        Param) const
{
  Dim =DimenObje;
  Add1=IndexO1;
  Add2=IndexO2;
  Param=ParamObje;
}

//=======================================================================
//function : GetElemO
//purpose  : 
//=======================================================================

void Intf_SectionPoint::InfoFirst (Intf_PIType& Dim,
				   Standard_Integer&     Add,
				   Standard_Real&        Param) const
{
  Dim =DimenObje;
  Add=IndexO2;
  Param=ParamObje;
}


//=======================================================================
//function : GetElemT
//purpose  : 
//=======================================================================

void Intf_SectionPoint::InfoSecond (Intf_PIType& Dim,
				    Standard_Integer&     Add1,
				    Standard_Integer&     Add2,
				    Standard_Real&        Param) const 
{
  Dim =DimenTool;
  Add1=IndexT1;
  Add2=IndexT2;
  Param=ParamTool;
}

//=======================================================================
//function : GetElemT
//purpose  : 
//=======================================================================

void Intf_SectionPoint::InfoSecond (Intf_PIType& Dim,
				    Standard_Integer&     Add,
				    Standard_Real&        Param) const 
{
  Dim =DimenTool;
  Add=IndexT2;
  Param=ParamTool;
}

//=======================================================================
//function : GetIncidence
//purpose  : 
//=======================================================================

Standard_Real Intf_SectionPoint::Incidence () const 
{return Incide;}


//=======================================================================
//function : IsOnSameEdge
//purpose  : 
//=======================================================================

Standard_Boolean Intf_SectionPoint::IsOnSameEdge
  (const Intf_SectionPoint& Other) const
{
  Standard_Boolean isOn=Standard_False;
  if (DimenObje==Intf_EDGE) {
    if (Other.DimenObje==Intf_EDGE) {
      isOn=(IndexO1==Other.IndexO1 && IndexO2==Other.IndexO2);
    }
    else if (Other.DimenObje==Intf_VERTEX) {
      isOn=(IndexO1==Other.IndexO1 || IndexO2==Other.IndexO1);
    }
  }
  else if (DimenObje == Intf_VERTEX) {
    if (Other.DimenObje==Intf_EDGE) {
      isOn=(IndexO1==Other.IndexO1 || IndexO1==Other.IndexO2);
    }
    else if (Other.DimenObje==Intf_VERTEX) {
#if DEBUG_INTFSECTIONPOINT
      cout << " IsOnSameEdge on Intersection VERTEX VERTEX Obje !" << endl;
#endif
	isOn=(IndexT1==Other.IndexT1);
    }
  }
  if (!isOn) {
    if (DimenTool==Intf_EDGE) {
      if (Other.DimenTool==Intf_EDGE) {
	isOn=(IndexT1==Other.IndexT1 && IndexT2==Other.IndexT2);
      }
      else if (Other.DimenTool==Intf_VERTEX) {
	isOn=(IndexT1==Other.IndexT1 || IndexT2==Other.IndexT1);
      }
    }
    else if (DimenTool == Intf_VERTEX) {
      if (Other.DimenTool==Intf_EDGE) {
	isOn=(IndexT1==Other.IndexT1 || IndexT1==Other.IndexT2);
      }
      else if (Other.DimenTool==Intf_VERTEX) {
#if DEBUG_INTFSECTIONPOINT
	cout << " IsOnSameEdge on Intersection VERTEX VERTEX Tool !" << endl;
#endif
	isOn=(IndexT1==Other.IndexT1);
      }
    }
  }
  return isOn;
}

//=======================================================================
//function : Intf_SectionPoint
//purpose  : 
//=======================================================================

Intf_SectionPoint::Intf_SectionPoint  ()
     : myPnt(0.,0.,0.),
       DimenObje(Intf_EXTERNAL), IndexO1(0), IndexO2(0), ParamObje(0.),
       DimenTool(Intf_EXTERNAL), IndexT1(0), IndexT2(0), ParamTool(0.),
       Incide(0.)
{
}

//=======================================================================
//function : Intf_SectionPoint
//purpose  : 
//=======================================================================

Intf_SectionPoint::Intf_SectionPoint  (const gp_Pnt& Where,
				       const Intf_PIType Dim1,
				       const Standard_Integer Addr1,
				       const Standard_Integer Addr2,
				       const Standard_Real Param1,
				       const Intf_PIType Dim2,
				       const Standard_Integer Addr3,
				       const Standard_Integer Addr4,
				       const Standard_Real Param2,
				       const Standard_Real Incid)
     : myPnt(Where),
       DimenObje(Dim1), IndexO1(Addr1), IndexO2(Addr2), ParamObje(Param1),
       DimenTool(Dim2), IndexT1(Addr3), IndexT2(Addr4), ParamTool(Param2),
       Incide(Incid)
{
}

//=======================================================================
//function : Intf_SectionPoint
//purpose  : 
//=======================================================================

Intf_SectionPoint::Intf_SectionPoint  (const gp_Pnt2d& Where,
				       const Intf_PIType Dim1,
				       const Standard_Integer Addr1,
				       const Standard_Real Param1,
				       const Intf_PIType Dim2,
				       const Standard_Integer Addr2,
				       const Standard_Real Param2,
				       const Standard_Real Incid)
     : myPnt(Where.X(),Where.Y(),0.),
       DimenObje(Dim1), IndexO1(0), IndexO2(Addr1), ParamObje(Param1),
       DimenTool(Dim2), IndexT1(0), IndexT2(Addr2), ParamTool(Param2),
       Incide(Incid)
{
}


//=======================================================================
//function : Merge
//purpose  : 
//=======================================================================

void Intf_SectionPoint::Merge  (Intf_SectionPoint& Other) {
  Other.myPnt=myPnt;
  if (DimenObje >= Other.DimenObje) {
    Other.DimenObje=DimenObje;
    Other.IndexO1=IndexO1;
    Other.IndexO2=IndexO2;
    Other.ParamObje=ParamObje;
  }
  else {
    DimenObje=Other.DimenObje;
    IndexO1=Other.IndexO1;
    IndexO2=Other.IndexO2;
    ParamObje=Other.ParamObje;
  }
  if (DimenTool >= Other.DimenTool) {
    Other.DimenTool=DimenTool;
    Other.IndexT1=IndexT1;
    Other.IndexT2=IndexT2;
    Other.ParamTool=ParamTool;
  }
  else {
    DimenTool=Other.DimenTool;
    IndexT1=Other.IndexT1;
    IndexT2=Other.IndexT2;
    ParamTool=Other.ParamTool;
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Intf_SectionPoint::Dump (const Standard_Integer
#if DEBUG_INTFSECTIONPOINT 
                                                     Indent
#endif
                             ) const
{
#if DEBUG_INTFSECTIONPOINT 
  for (Standard_Integer id=0; id<Indent; id++) cout << " ";

  cout << "PIType(" << DimenObje << "," << DimenTool << ")  entre(" 
        << IndexO1 << "," << IndexO2 << ") par(" << ParamObje 
	  << ") et ("
	<< IndexT1 << "," << IndexT2 << ") par(" << ParamTool  
	  << ")" << endl;

  for (id=0; id<Indent; id++) cout << " ";

  cout << "  Lieu(" << myPnt.X() << ","
                    << myPnt.Y() << "," 
                    << myPnt.Z() << 
	 ")  Incidence(" << Incide << ")" << endl;
#endif
}
