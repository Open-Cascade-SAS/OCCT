// File:	TopOpeBRepBuild_ShellFaceSet.cxx
// Created:	Thu Jun 17 17:21:05 1993
// Author:	Jean Yves LEBEY
//		<jyl@zerox>

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
Standard_IMPORT Standard_Boolean TopOpeBRepBuild_GettraceCHK();
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
