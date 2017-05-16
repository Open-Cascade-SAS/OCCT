// Created on: 1993-06-17
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


#include <BRepCheck_Analyzer.hxx>
#include <Standard_ProgramError.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopAbs.hxx>
#include <TopoDS_Shape.hxx>
#include <TopOpeBRepBuild_define.hxx>
#include <TopOpeBRepBuild_ShapeSet.hxx>

#ifdef DRAW
#include <DBRep.hxx>
#include <DBRep_DrawableShape.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
static TCollection_AsciiString PRODINS("dins ");

static void ShapeEnumToString(const TopAbs_ShapeEnum T, TCollection_AsciiString& N)
{
  if      (T == TopAbs_SHAPE)  N = "s"; 
  else if (T == TopAbs_COMPOUND)  N = "co";
  else if (T == TopAbs_COMPSOLID)  N = "cs";
  else if (T == TopAbs_SOLID)  N = "so";
  else if (T == TopAbs_SHELL)  N = "sh";  
  else if (T == TopAbs_FACE)   N = "f"; 
  else if (T == TopAbs_WIRE)   N = "w"; 
  else if (T == TopAbs_EDGE)   N = "e"; 
  else if (T == TopAbs_VERTEX) N = "v"; 
}
#endif

#ifdef OCCT_DEBUG
extern Standard_Boolean TopOpeBRepBuild_GettraceCHK();
extern Standard_Boolean TopOpeBRepBuild_GettraceCHKOK();
extern Standard_Boolean TopOpeBRepBuild_GettraceCHKNOK();
Standard_EXPORT void debaddss(){}
Standard_IMPORT TopOpeBRepBuild_Builder* LOCAL_PBUILDER_DEB;
#endif

//=======================================================================
//function : TopOpeBRepBuild_ShapeSet
//purpose  : 
//=======================================================================
TopOpeBRepBuild_ShapeSet::TopOpeBRepBuild_ShapeSet(const TopAbs_ShapeEnum SubShapeType,const Standard_Boolean checkshape) : 
 mySubShapeType(SubShapeType), myCheckShape(checkshape) 
{
  if      (SubShapeType == TopAbs_EDGE)   myShapeType = TopAbs_FACE;
  else if (SubShapeType == TopAbs_VERTEX) myShapeType = TopAbs_EDGE;
  else throw Standard_ProgramError("ShapeSet : bad ShapeType");
  myDEBNumber = 0;

  myCheckShape = Standard_False; // temporary NYI
}

TopOpeBRepBuild_ShapeSet::~TopOpeBRepBuild_ShapeSet()
{}

//=======================================================================
//function : AddShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::AddShape(const TopoDS_Shape& S)
{
  Standard_Boolean chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(cout," AddShape",S,chk);
#endif

  if (!chk) return;
  ProcessAddShape(S);
}

//=======================================================================
//function : AddStartElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::AddStartElement(const TopoDS_Shape& S)
{
  Standard_Boolean chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(cout," AddStartElement",S,chk);
#endif

  if (!chk) return;
  ProcessAddStartElement(S);
}

//=======================================================================
//function : AddElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::AddElement(const TopoDS_Shape& S)
{
  Standard_Boolean chk = CheckShape(S);
#ifdef OCCT_DEBUG
  DumpCheck(cout," AddElement",S,chk);
#endif

  if (!chk) return;
  ProcessAddElement(S);
}

//=======================================================================
//function : ProcessAddShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::ProcessAddShape(const TopoDS_Shape& S)
{
  if ( !myOMSH.Contains(S) ) {
    myOMSH.Add(S);
    myShapes.Append(S);
  }
}

//=======================================================================
//function : ProcessStartElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::ProcessAddStartElement(const TopoDS_Shape& S)
{
  if ( !myOMSS.Contains(S) ) {
    myOMSS.Add(S);
    myStartShapes.Append(S);
    ProcessAddElement(S);
  }
}

//=======================================================================
//function : ProcessAddElement
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::ProcessAddElement(const TopoDS_Shape& S)
{
  if ( !myOMES.Contains(S) ) {
    myOMES.Add(S);
    TopTools_ListOfShape  Lemp;
    TopOpeBRepTool_ShapeExplorer Ex(S,mySubShapeType);
    for (; Ex.More(); Ex.Next()) {
      const TopoDS_Shape& subshape = Ex.Current();
      Standard_Boolean b = ( ! mySubShapeMap.Contains(subshape) );
      if ( b ) mySubShapeMap.Add(subshape,Lemp);
      mySubShapeMap.ChangeFromKey(subshape).Append(S);
    }
  }
}

//=======================================================================
//function : StartElements
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& TopOpeBRepBuild_ShapeSet::StartElements()const 
{
  return myStartShapes;
}

//=======================================================================
//function : InitShapes
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_ShapeSet::InitShapes()
{
  myShapesIter.Initialize(myShapes);
}

//=======================================================================
//function : MoreShapes
//purpose  : 
//=======================================================================
Standard_Boolean  TopOpeBRepBuild_ShapeSet::MoreShapes()const 
{
  Standard_Boolean b = myShapesIter.More();
  return b;
}

//=======================================================================
//function : NextShape
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_ShapeSet::NextShape()
{
  myShapesIter.Next();
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================
const TopoDS_Shape&  TopOpeBRepBuild_ShapeSet::Shape()const 
{
  const TopoDS_Shape& S = myShapesIter.Value();
  return S;
}

//=======================================================================
//function : InitStartElements
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_ShapeSet::InitStartElements()
{
  myStartShapesIter.Initialize(myStartShapes);
}

//=======================================================================
//function : MoreStartElements
//purpose  : 
//=======================================================================
Standard_Boolean  TopOpeBRepBuild_ShapeSet::MoreStartElements()const 
{
  Standard_Boolean b = myStartShapesIter.More();
  return b;
}

//=======================================================================
//function : NextStartElement
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_ShapeSet::NextStartElement()
{
  myStartShapesIter.Next();
}

//=======================================================================
//function : StartElement
//purpose  : 
//=======================================================================
const TopoDS_Shape& TopOpeBRepBuild_ShapeSet::StartElement()const 
{
  const TopoDS_Shape& S = myStartShapesIter.Value();
  return S;
}

//=======================================================================
//function : InitNeighbours
//purpose  : 
//=======================================================================
void  TopOpeBRepBuild_ShapeSet::InitNeighbours(const TopoDS_Shape& S)
{
  mySubShapeExplorer.Init(S,mySubShapeType);
  myCurrentShape = S;
  FindNeighbours();
}

//=======================================================================
//function : MoreNeighbours
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_ShapeSet::MoreNeighbours()
{
  Standard_Boolean b = myIncidentShapesIter.More();
  return b;
}

//=======================================================================
//function : NextNeighbour
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::NextNeighbour()
{
  myIncidentShapesIter.Next();
  Standard_Boolean noisimore = ! myIncidentShapesIter.More();
  if ( noisimore ) {
    Standard_Boolean ssemore = mySubShapeExplorer.More();
    if ( ssemore ) {
      mySubShapeExplorer.Next();
      FindNeighbours();
    }
  }
}

//=======================================================================
//function : Neighbour
//purpose  : 
//=======================================================================
const TopoDS_Shape&  TopOpeBRepBuild_ShapeSet::Neighbour()const 
{
  const TopoDS_Shape& S = myIncidentShapesIter.Value();
  return S;
}

//=======================================================================
//function : ChangeStartShapes
//purpose  : 
//=======================================================================
TopTools_ListOfShape& TopOpeBRepBuild_ShapeSet::ChangeStartShapes()
{
  return myStartShapes;
}

//=======================================================================
//function : FindNeighbours
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::FindNeighbours()
{
  while (mySubShapeExplorer.More()) {
    
    // l = list of edges neighbour of edge myCurrentShape trough
    // the vertex mySubShapeExplorer.Current(), which is a vertex of the
    // edge myCurrentShape.
    const TopoDS_Shape& V = mySubShapeExplorer.Current();
    const TopTools_ListOfShape & l = MakeNeighboursList(myCurrentShape,V);
    
    // myIncidentShapesIter iterates on the neighbour edges of the edge
    // given as InitNeighbours() argument (this edge has been stored 
    // in the field myCurrentShape).
    
    myIncidentShapesIter.Initialize(l);
    if (myIncidentShapesIter.More()) break;
    else mySubShapeExplorer.Next();
  }
}

//=======================================================================
//function : MakeNeighboursList
//purpose  : // (Earg = Edge, Varg = Vertex) to find connected to Earg by Varg 
//=======================================================================
const TopTools_ListOfShape & TopOpeBRepBuild_ShapeSet::MakeNeighboursList(const TopoDS_Shape& /*Earg*/, const TopoDS_Shape& Varg)
{
  const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(Varg);
  return l;
}

//=======================================================================
//function : MaxNumberSubShape
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_ShapeSet::MaxNumberSubShape(const TopoDS_Shape& Shape)
{
  Standard_Integer i, m = 0;
  TopOpeBRepTool_ShapeExplorer SE(Shape, mySubShapeType);
  TopTools_ListIteratorOfListOfShape LI;
  while(SE.More()) {
    const TopoDS_Shape& SubShape = SE.Current();
    if(!mySubShapeMap.Contains(SubShape)) {
      SE.Next();
      continue;
    }
    const TopTools_ListOfShape& l = mySubShapeMap.FindFromKey(SubShape);
    LI.Initialize(l);
    for(i = 0;LI.More();LI.Next(), i++) {}
    m = Max(m, i);
    SE.Next();
  }
  return m;
}

//=======================================================================
//function : CheckShape
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::CheckShape(const Standard_Boolean checkshape)
{
  myCheckShape = checkshape;

#ifdef OCCT_DEBUG
  if (TopOpeBRepBuild_GettraceCHK() && !myCheckShape) {
    DumpName(cout,"no checkshape set on ");cout<<endl;
  }
#endif
}

//=======================================================================
//function : CheckShape
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_ShapeSet::CheckShape() const
{
  return myCheckShape;
}

//=======================================================================
//function : CheckShape
//purpose  : 
//=======================================================================
Standard_Boolean TopOpeBRepBuild_ShapeSet::CheckShape(const TopoDS_Shape& S,const Standard_Boolean checkgeom)
{
  if (!myCheckShape) return Standard_True;

  BRepCheck_Analyzer ana(S,checkgeom);    
  Standard_Boolean val = ana.IsValid();
  if ( val ) {
    return Standard_True;
  }
  else {
    return Standard_False;
  }
}

//=======================================================================
//function : DumpName
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::DumpName(Standard_OStream& OS,const TCollection_AsciiString& str) const
{
  OS<<str<<"("<<myDEBName<<","<<myDEBNumber<<")";
}

//=======================================================================
//function : DumpCheck
//purpose  : 
//=======================================================================

#ifdef OCCT_DEBUG
void TopOpeBRepBuild_ShapeSet::DumpCheck(Standard_OStream& OS,
                                         const TCollection_AsciiString& str,
                                         const TopoDS_Shape& S, 
                                         const Standard_Boolean chk
#else
void TopOpeBRepBuild_ShapeSet::DumpCheck(Standard_OStream&,
                                         const TCollection_AsciiString&,
                                         const TopoDS_Shape&,
                                         const Standard_Boolean
#endif
                                                                    ) const
{ 
  if (!myCheckShape) return;

#ifdef OCCT_DEBUG
  TopAbs_ShapeEnum t = S.ShapeType();
  if (!chk) {
    if (TopOpeBRepBuild_GettraceCHK() ||
	TopOpeBRepBuild_GettraceCHKNOK() ) {
      DumpName(OS,"*********************** ");
      OS<<str<<" ";TopAbs::Print(t,OS);OS<<" : incorrect"<<endl;
    }
  }
  else {
    if (TopOpeBRepBuild_GettraceCHK() ||
	TopOpeBRepBuild_GettraceCHKOK() ) {
      DumpName(OS,"");
      OS<<str<<" ";TopAbs::Print(t,OS);OS<<" : correct"<<endl;
    }
  }
  if (!chk) debaddss();
#endif
}

//=======================================================================
//function : DumpSS
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::DumpSS()
{
#ifdef DRAW
  DumpName(cout,"\nDumpSS start ");
  TopTools_ListIteratorOfListOfShape it;
  Standard_Integer i,j,ne;
  TCollection_AsciiString s1("   ");
  InitShapes();
  cout<<endl<<"#Shapes : ";if (!MoreShapes()) cout<<0;cout<<endl;
  for (i=1;MoreShapes();NextShape(),i++) {
    TCollection_AsciiString ns = SNameori(Shape());
    cout<<PRODINS<<ns<<"; # "<<i<<" draw"<<endl;
    DBRep::Set(ns.ToCString(),Shape());
  }

  InitStartElements();
  cout<<"#StartElements : ";if (!MoreStartElements()) cout<<0;cout<<endl;
  for (i=1;MoreStartElements();NextStartElement(),i++) {
    cout<<PRODINS<<SNameori(StartElement())<<"; # "<<i<<" draw"<<endl;
  }

  InitStartElements();
  cout<<"#Neighbours of StartElements : ";if (!MoreStartElements()) cout<<0;cout<<endl;
  for (i=1;MoreStartElements();NextStartElement(),i++) {
    const TopoDS_Shape& e = StartElement();
    TCollection_AsciiString enam = SNameori(e);
    InitNeighbours(e);
    if (MoreNeighbours()) {
      TCollection_AsciiString sne("clear; ");sne=sne+PRODINS+enam;
      for (ne=1; MoreNeighbours(); NextNeighbour(),ne++) {
	const TopoDS_Shape& N = Neighbour();
	sne=sne+" "+SNameori(N);
      }
      sne=sne+"; wclick; #draw";
      cout<<sne<<endl;
    }
  }

  Standard_Integer ism,nsm=mySubShapeMap.Extent();
  cout<<"#Incident shapes : ";if (!nsm) cout<<0;cout<<endl;
  for (i=1,ism=1;ism<=nsm;ism++,i++) {
    const TopoDS_Shape& v = mySubShapeMap.FindKey(ism);
    const TopTools_ListOfShape& lsv = mySubShapeMap.FindFromIndex(ism);
    TopTools_ListIteratorOfListOfShape itle(lsv);
    if (itle.More()) {
      TCollection_AsciiString vnam = SName(v);
      TCollection_AsciiString sle("clear; ");sle=sle+PRODINS+vnam;
      for (j=1;itle.More();itle.Next(),j++) {
	const TopoDS_Shape& e = itle.Value();
	sle=sle+" "+SNameori(e);
      }
      sle=sle+"; wclick; #draw";
      cout<<sle<<endl;
    } 
  }
  DumpName(cout,"DumpSS end ");
#endif
}

//=======================================================================
//function : DumpBB
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::DumpBB()
{
#ifdef DRAW
  DumpName(cout,"\nDumpBB ");
  TopTools_ListIteratorOfListOfShape it;
  Standard_Integer i,j,ne = 0, nb = 1, curr, currloc;
  TCollection_AsciiString s1("   "),stt,enam,nnam,vnam;
  InitShapes();

  cout<<endl<<"#Shapes : (block old) ";if (!MoreShapes()) cout<<0;cout<<endl;
  for (i=1;MoreShapes();NextShape(),i++, nb++) {
    cout<<"Block number"<<nb<<" (old)."<<endl;
    const TopoDS_Shape& e = Shape();
    TopoDS_Iterator ShapIter(e);
    for(ne = 1;ShapIter.More();ShapIter.Next(), ne++) {
      const TopoDS_Shape& subsha = ShapIter.Value();
      ShapeEnumToString(subsha.ShapeType(),stt);
      enam = stt+ne+"ShaB"+nb;
      DBRep::Set(enam.ToCString(),subsha); 
      cout<<"clear; "<<PRODINS<<enam<<"; #draw"<<endl;
    }
  }

  InitStartElements();
  TopTools_IndexedMapOfShape mos;
  cout<<"#Elements : (block new) : ";if (!MoreStartElements()) cout<<0;cout<<endl;
  mos.Clear();
  for (;MoreStartElements();NextStartElement()) {
    const TopoDS_Shape& e = StartElement();
    curr = mos.Extent();
    if(mos.Add(e) > curr) {
      cout<<"#Block number"<<nb<<" (new)."<<endl;
      nb++;ne++;
      enam = "";
      enam = enam+"ste"+ne+"newB"+nb;
      DBRep::Set(enam.ToCString(),e); 

      while(curr < mos.Extent()) {
	curr = mos.Extent();
	currloc = curr;
	InitNeighbours(mos.FindKey(curr));
	for (; MoreNeighbours(); NextNeighbour()) {
	  const TopoDS_Shape& N = Neighbour();
	  if(mos.Add(N) > currloc) {
	    currloc++;ne++;
	    // to know if ste or ele is displayed; start
	    const TopTools_ListOfShape& LSE = StartElements();
	    it.Initialize(LSE);
	    while(it.More()) 
	      if(it.Value() == N)
		break;
	      else
		it.Next();
	    enam = "";
	    if(it.More()) {
	      enam = enam+"ste"+ne+"newB"+nb;
	      DBRep::Set(enam.ToCString(),N); 
	    } else {
	      enam = enam+"ele"+ne+"newB"+nb;
	      DBRep::Set(enam.ToCString(),N); 
	      cout<<PRODINS<<enam<<"; #draw"<<endl;
	    }	  
	  }
	}
      }
    }
  }
#endif
}

//=======================================================================
//function : DEBName
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::DEBName(const TCollection_AsciiString& N)
{ 
  myDEBName = N; 
}

//=======================================================================
//function : DEBName
//purpose  : 
//=======================================================================
const TCollection_AsciiString& TopOpeBRepBuild_ShapeSet::DEBName() const
{ 
  return myDEBName; 
}

//=======================================================================
//function : DEBNumber
//purpose  : 
//=======================================================================
void TopOpeBRepBuild_ShapeSet::DEBNumber(const Standard_Integer I)
{ 
  myDEBNumber = I; 
}

//=======================================================================
//function : DEBNumber
//purpose  : 
//=======================================================================
Standard_Integer TopOpeBRepBuild_ShapeSet::DEBNumber() const
{ 
  return myDEBNumber; 
}

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const TopoDS_Shape& /*S*/,
                                                        const TCollection_AsciiString& sb,
                                                        const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str;

  str=sb;
  TCollection_AsciiString WESi=myDEBName.SubString(1,1)+myDEBNumber;
  str=str+WESi;
  TopAbs_ShapeEnum t = S.ShapeType();
  TCollection_AsciiString sts;ShapeEnumToString(t,sts);
  sts.UpperCase();str=str+sts.SubString(1,1);
  Standard_Integer isub = mySubShapeMap.FindIndex(S);
  Standard_Integer ista = myOMSS.FindIndex(S);
  Standard_Integer iele = myOMES.FindIndex(S);
  Standard_Integer isha = myOMSH.FindIndex(S);
  if      (isub) str=str+"sub"+isub;
  if      (ista) str=str+"sta"+ista;
  else if (iele) str=str+"ele"+iele;
  if      (isha) str=str+"sha"+isha;
  str=str+sa;

  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const TopoDS_Shape&,
                                                        const TCollection_AsciiString&,
                                                        const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}
#endif

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SNameori(const TopoDS_Shape& S,
                                                           const TCollection_AsciiString& sb,
                                                           const TCollection_AsciiString& sa) const
{
  TCollection_AsciiString str;
  str=sb+SName(S);
  TCollection_AsciiString sto = TopAbs::ShapeOrientationToString (S.Orientation());
  str=str+sto.SubString(1,1);
  str=str+sa;
  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SNameori(const TopoDS_Shape&,
                                                           const TCollection_AsciiString&,
                                                           const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}
#endif

//=======================================================================
//function : SName
//purpose  : 
//=======================================================================
#ifdef DRAW
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const TopTools_ListOfShape& L,
                                                        const TCollection_AsciiString& sb,
                                                        const TCollection_AsciiString& /*sa*/) const
{
  TCollection_AsciiString str;
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next()) str=str+sb+SName(it.Value())+sa+" ";
  return str;
}
#else
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SName(const TopTools_ListOfShape&,
                                                        const TCollection_AsciiString&,
                                                        const TCollection_AsciiString&) const
{
  TCollection_AsciiString str;
  return str;
}
#endif

//=======================================================================
//function : SNameori
//purpose  : 
//=======================================================================
TCollection_AsciiString TopOpeBRepBuild_ShapeSet::SNameori(const TopTools_ListOfShape& /*L*/,
                                                           const TCollection_AsciiString& /*sb*/,
                                                           const TCollection_AsciiString& /*sa*/) const
{
  TCollection_AsciiString str;
#ifdef DRAW
  for (TopTools_ListIteratorOfListOfShape it(L);it.More();it.Next()) str=str+sb+SNameori(it.Value())+sa+" ";
#endif
  return str;
}
