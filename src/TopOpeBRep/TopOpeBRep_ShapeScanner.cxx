// Created on: 1993-07-07
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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


#include <TopOpeBRep_ShapeScanner.ixx>
#include <TopOpeBRepTool_define.hxx>
#include <TopAbs.hxx>

//=======================================================================
//function : TopOpeBRep_ShapeScanner
//purpose  : 
//=======================================================================
TopOpeBRep_ShapeScanner::TopOpeBRep_ShapeScanner()
{
}    

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeScanner::Clear()
{
  myBoxSort.Clear();
}    

//=======================================================================
//function : AddBoxesMakeCOB
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeScanner::AddBoxesMakeCOB(const TopoDS_Shape& S,const TopAbs_ShapeEnum TS,const TopAbs_ShapeEnum TA)
{
  myBoxSort.AddBoxesMakeCOB(S,TS,TA);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeScanner::Init(TopOpeBRepTool_ShapeExplorer &E)
{
  TColStd_ListOfInteger anEmptyList;

  myListIterator.Initialize(anEmptyList);

  for (; E.More(); E.Next() )  {
    const TopoDS_Shape& cur = E.Current();
//    TopAbs_ShapeEnum t = cur.ShapeType();
    Init(cur);
    Standard_Boolean b = More();
    if ( b ) break;
  }
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeScanner::Init(const TopoDS_Shape &E)
{
  myListIterator = myBoxSort.Compare(E);
}

//=======================================================================
//function : More
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRep_ShapeScanner::More() const
{
  Standard_Boolean b = myListIterator.More();
  return b;
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================
void TopOpeBRep_ShapeScanner::Next()
{
  myListIterator.Next();
}

//=======================================================================
//function : Current
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRep_ShapeScanner::Current() const
{
  const TopoDS_Shape& E = myBoxSort.TouchedShape(myListIterator);
  return E;
}

//=======================================================================
//function : BoxSort
//purpose  : 
//=======================================================================
const TopOpeBRepTool_BoxSort& TopOpeBRep_ShapeScanner::BoxSort() const
{
  return myBoxSort;
}

//=======================================================================
//function : BoxSort
//purpose  : 
//=======================================================================
TopOpeBRepTool_BoxSort& TopOpeBRep_ShapeScanner::ChangeBoxSort()
{
  return myBoxSort;
}

//=======================================================================
//function : Index
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRep_ShapeScanner::Index()const 
{
  Standard_Integer n = 0;
  if ( myListIterator.More() ) n = myListIterator.Value();
  return n;
}

//=======================================================================
//function : DumpCurrent
//purpose  : 
//=======================================================================
Standard_OStream& TopOpeBRep_ShapeScanner::DumpCurrent(Standard_OStream& OS)const 
{
#ifdef DEB
  if ( More() ) { 
    const TopoDS_Shape&     S = Current();
    TopAbs_ShapeEnum    T = S.ShapeType();
    TopAbs_Orientation  O = S.Orientation();
    Standard_Integer    I = Index();
    TopAbs::Print(T,cout);
    cout<<"("<<I<<","; TopAbs::Print(O,cout); cout<<") ";
  }
#endif
  return OS;
}
