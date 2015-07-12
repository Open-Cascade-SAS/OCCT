// Created on: 1995-07-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepTool_ShapeExplorer.hxx>

//=======================================================================
//function : TopOpeBRepTool_ShapeExplorer
//purpose  : 
//=======================================================================
TopOpeBRepTool_ShapeExplorer::TopOpeBRepTool_ShapeExplorer() : 
   myIndex(0),
   myNbShapes(0)
{
}


//=======================================================================
//function : TopOpeBRepTool_ShapeExplorer
//purpose  : 
//=======================================================================

TopOpeBRepTool_ShapeExplorer::TopOpeBRepTool_ShapeExplorer
   (const TopoDS_Shape& S, 
    const TopAbs_ShapeEnum ToFind, 
    const TopAbs_ShapeEnum ToAvoid)
{
  Init(S,ToFind,ToAvoid);
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void  TopOpeBRepTool_ShapeExplorer::Init(const TopoDS_Shape& S, 
					 const TopAbs_ShapeEnum ToFind, 
					 const TopAbs_ShapeEnum ToAvoid)
{
  myExplorer.Init(S,ToFind,ToAvoid);

#ifdef OCCT_DEBUG
  myIndex = myNbShapes = 0;
  for (;More();Next()) myNbShapes++;
  myExplorer.ReInit();
  if (More()) myIndex = 1;
#endif

}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================

Standard_Boolean  TopOpeBRepTool_ShapeExplorer::More()const 
{
  Standard_Boolean b = myExplorer.More(); 
  return b;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void  TopOpeBRepTool_ShapeExplorer::Next()
{
  myExplorer.Next();
#ifdef OCCT_DEBUG
  myIndex++;
#endif
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================

const TopoDS_Shape&  TopOpeBRepTool_ShapeExplorer::Current()const 
{
  const TopoDS_Shape& S = myExplorer.Current();
  return S;
}


//=======================================================================
//function : NbShapes
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_ShapeExplorer::NbShapes()const 
{
  Standard_Integer n = 0;
#ifdef OCCT_DEBUG
  n = myNbShapes;
#endif
  return n;
}


//=======================================================================
//function : Index
//purpose  : 
//=======================================================================

Standard_Integer TopOpeBRepTool_ShapeExplorer::Index()const 
{
  Standard_Integer n = 0;
#ifdef OCCT_DEBUG
  n = myIndex;
#endif
  return n;
}
 
//=======================================================================
//function : DumpCurrent
//purpose  : 
//=======================================================================

Standard_OStream& TopOpeBRepTool_ShapeExplorer::DumpCurrent
(Standard_OStream& OS)const 
{
#ifdef OCCT_DEBUG
  if ( More() ) { 
    const TopoDS_Shape& S = Current();
    TopAbs_ShapeEnum    T = S.ShapeType();
    TopAbs_Orientation  O = S.Orientation();
    Standard_Integer    I = Index();
    TopAbs::Print(T,cout);
    cout<<"("<<I<<","; TopAbs::Print(O,cout); cout<<") ";
  }
#endif
  return OS;
}
