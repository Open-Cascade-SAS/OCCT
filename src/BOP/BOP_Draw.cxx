// Created on: 2001-05-31
// Created by: Peter KURNEV
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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



#include <BOP_Draw.ixx>

#ifdef BOPDRAW

#include <stdio.h>

#include <TCollection_AsciiString.hxx>

#include <BRep_Tool.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <DBRep.hxx>

#include <DrawTrSurf.hxx>
#include <Draw_Color.hxx>
#include <Draw_Interpretor.hxx>


Standard_IMPORT 
  Draw_Color DrawTrSurf_CurveColor(const Draw_Color col);

Standard_IMPORT 
  Draw_Color DBRep_ColorOrientation (const TopAbs_Orientation Or);

Standard_EXPORT 
  Draw_Interpretor theCommands;
//=======================================================================
// function: DrawListOfShape
// purpose: 
//=======================================================================
  void BOP_Draw::DrawListOfShape (const TopTools_ListOfShape& aListOfShape,
				  const Standard_CString aName)
				  
{
  Standard_Integer j;
  TCollection_AsciiString aNm(aName);

  j=aListOfShape.Extent();
  if (!j) {
    cout << " No shapes to draw" << flush;
    return;
  }
  
  TopTools_ListIteratorOfListOfShape anIt(aListOfShape);
  for (j=1; anIt.More(); anIt.Next(), j++) {
    const TopoDS_Shape& aEx=anIt.Value();
    
    TCollection_AsciiString aN, jIndex(j);
    aN=aNm+jIndex;
    Standard_CString aTxt=aN.ToCString();
    cout << aTxt << " " << flush;
    DBRep::Set(aTxt, aEx);
  }
  cout << endl << flush;
}
//=======================================================================
// function: DrawListOfEdgesWithPC
// purpose: 
//=======================================================================
  void BOP_Draw::DrawListOfEdgesWithPC (const TopoDS_Face& aFace,
					const TopTools_ListOfShape& aListOfShape,
					const Standard_Integer i,
					const Standard_CString cName)
				  
{
  theCommands.Eval("2dclear");
  
  TCollection_AsciiString aName, aIndex(i), aUndscr("_"), aNm(cName);
  aName=aName+aNm;
  aName=aName+aIndex;
  aName=aName+aUndscr;
  Standard_CString aCString=aName.ToCString();
  
  DrawListOfEdgesWithPC(aFace, aListOfShape, aCString);
}

//=======================================================================
// function: DrawListOfEdgesWithPC
// purpose: 
//=======================================================================
  void BOP_Draw::DrawListOfEdgesWithPC (const TopoDS_Face& aFace,
					const TopTools_ListOfShape& aListOfShape,
					const Standard_CString aName)
				  
{
  theCommands.Eval("2dclear");
  
  Standard_Integer j;
  TCollection_AsciiString aNm(aName);
  
  j=aListOfShape.Extent();
  if (!j) {
    cout << " No shapes to draw" << flush;
    return;
  }

  TopTools_ListIteratorOfListOfShape anIt(aListOfShape);
    for (j=1; anIt.More(); anIt.Next(), j++) {
    const TopoDS_Edge& aEx=TopoDS::Edge(anIt.Value());
    
    TCollection_AsciiString aN, jIndex(j);
    aN=aNm+jIndex;
    Standard_CString aTxt=aN.ToCString();
    BOP_Draw::DrawEdgeWithPC(aFace, aEx, aTxt);
    cout << " " << aTxt << flush;
  }

  theCommands.Eval("2dfit");
}

//=======================================================================
// function: DrawEdgeWithPC
// purpose: 
//=======================================================================
  void BOP_Draw::DrawEdgeWithPC (const TopoDS_Face& aFace,
				 const TopoDS_Edge& aEdge,
				 const Standard_CString aName)
				  
{
  Standard_Real f,l;
  
  // Edge 3D
  DBRep::Set(aName, aEdge);
  // PC
  Draw_Color col, savecol;
  savecol = DrawTrSurf_CurveColor(Draw_rouge);

  const Handle(Geom2d_Curve) c = 
    BRep_Tool::CurveOnSurface (aEdge, aFace, f, l);

  col = DBRep_ColorOrientation(aEdge.Orientation());
  DrawTrSurf_CurveColor(col);
  
  TCollection_AsciiString aNm(aName), aAdd("_p");
  aNm=aNm+aAdd;
  Standard_CString aTxtPC=aNm.ToCString();
  
  DrawTrSurf::Set(aTxtPC, new Geom2d_TrimmedCurve(c, f, l));
  
  DrawTrSurf_CurveColor(savecol);
}

//=======================================================================
// function: Wait
// purpose: 
//=======================================================================
  void BOP_Draw::Wait()
{
  char xx; 
  scanf ("%c", &xx);
}

#endif 

#ifndef BOPDRAW

void BOP_Draw::DrawListOfShape       (const TopTools_ListOfShape& , 
				      const Standard_CString ){}
void BOP_Draw::DrawListOfEdgesWithPC (const TopoDS_Face& ,
				      const TopTools_ListOfShape& ,
				      const Standard_Integer ,
				      const Standard_CString ){}
void BOP_Draw::DrawListOfEdgesWithPC (const TopoDS_Face& ,
				      const TopTools_ListOfShape& ,
				      const Standard_CString ){}
void BOP_Draw::DrawEdgeWithPC        (const TopoDS_Face& ,
				      const TopoDS_Edge& ,
				      const Standard_CString ){} 
void BOP_Draw::Wait(){}

#endif

