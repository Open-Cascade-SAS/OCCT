// Created on: 1993-06-17
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


#ifdef DRAW
#include <DBRep.hxx>
#include <TestTopOpeDraw_TTOT.hxx>
static TCollection_AsciiString PRODINS("dins ");
#endif

#include <TopOpeBRepBuild_ShellFaceSet.ixx>
#include <TopOpeBRepBuild_Builder.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopoDS.hxx>

#ifdef DEB
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
#endif

//=======================================================================
//function : TopOpeBRepBuild_ShellFaceSet
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShellFaceSet::TopOpeBRepBuild_ShellFaceSet() :
TopOpeBRepBuild_ShapeSet(TopAbs_EDGE)
{
#ifdef DEB
  myDEBName = "SFS";
#endif
}

//=======================================================================
//function : TopOpeBRepBuild_ShellFaceSet
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShellFaceSet::TopOpeBRepBuild_ShellFaceSet
(const TopoDS_Shape& S,const Standard_Address A) : // DEB
TopOpeBRepBuild_ShapeSet(TopAbs_EDGE)
{
  mySolid = TopoDS::Solid(S);

#ifdef DEB
  myDEBName = "SFS";
  if (A != NULL) {
    TopOpeBRepBuild_Builder* pB = ((TopOpeBRepBuild_Builder*)((void*)A));
    myDEBNumber = pB->GdumpSHASETindex();
    Standard_Integer iS; Standard_Boolean tSPS = pB->GtraceSPS(S,iS);
    if(tSPS){cout<<"creation SFS "<<myDEBNumber<<" on ";}
    if(tSPS){pB->GdumpSHA(S,NULL);cout<<endl;}
  }

  if (TopOpeBRepBuild_GettraceCHK() && !myCheckShape) {
    DumpName(cout,"no checkshape in creation of ");cout<<endl;
  }
#endif
}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShellFaceSet::AddShape(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::AddShape(S);
}

//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShellFaceSet::AddStartElement(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::ProcessAddStartElement(S);
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShellFaceSet::AddElement(const TopoDS_Shape& S)
{
  TopOpeBRepBuild_ShapeSet::AddElement(S);
}

//=======================================================================
//function : Solid
//purpose  : 
//=======================================================================

const TopoDS_Solid& TopOpeBRepBuild_ShellFaceSet::Solid() const 
{
  return mySolid;
}

//=======================================================================
//function : DumpSS
//purpose  : 
//=======================================================================

void TopOpeBRepBuild_ShellFaceSet::DumpSS()
{
#ifdef DEB
  TopOpeBRepBuild_ShapeSet::DumpSS();
#endif
}

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopoDS_Shape& S,const TCollection_AsciiString& sb,const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str=sb;
#ifdef DRAW
  str=str+TopOpeBRepBuild_ShapeSet::SName(S);
  str=str+sa;
  DBRep::Set(str.ToCString(),S);
#endif
  return str;
}

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(const TopoDS_Shape& S,const TCollection_AsciiString& sb,const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str=sb;
#ifdef DRAW
  str=str+TopOpeBRepBuild_ShapeSet::SNameori(S);
  if ( S.ShapeType() == TopAbs_FACE ) {
    const TopoDS_Shape& F = TopoDS::Face(S);
    DBRep::Set(str.ToCString(),S);
  }
#endif
  return str;
}

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SName(const TopTools_ListOfShape& L,const TCollection_AsciiString& sb,const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next()) str=str+sb+SName(it.Value())+sa+" ";
#endif
  return str;
}

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
TCollection_AsciiString TopOpeBRepBuild_ShellFaceSet::SNameori(const TopTools_ListOfShape& L,const TCollection_AsciiString& sb,const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next()) str=str+sb+SNameori(it.Value())+sa+" ";
#endif
  return str;
}
