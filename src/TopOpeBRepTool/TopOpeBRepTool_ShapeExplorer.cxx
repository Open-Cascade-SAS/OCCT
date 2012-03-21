// Created on: 1995-07-13
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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



#include <TopOpeBRepTool_ShapeExplorer.ixx>
#include <TopAbs.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>


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

#ifdef DEB
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
#ifdef DEB
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
#ifdef DEB
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
#ifdef DEB
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
#ifdef DEB
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
