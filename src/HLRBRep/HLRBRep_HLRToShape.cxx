// Created on: 1993-10-11
// Created by: Christophe MARION
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


#include <HLRBRep_HLRToShape.ixx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <BRep_Builder.hxx>
#include <HLRBRep.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <HLRAlgo_EdgeIterator.hxx>

//=======================================================================
//function : HLRBRep_HLRToShape
//purpose  : 
//=======================================================================

HLRBRep_HLRToShape::HLRBRep_HLRToShape (const Handle(HLRBRep_Algo)& A) :
myAlgo(A)
{}

//=======================================================================
//function : InternalCompound
//purpose  : 
//=======================================================================

TopoDS_Shape 
HLRBRep_HLRToShape::InternalCompound (const Standard_Integer typ,
				      const Standard_Boolean visible,
				      const TopoDS_Shape& S)
{
  Standard_Boolean added = Standard_False;
  TopoDS_Shape Result;
  Handle(HLRBRep_Data) DS = myAlgo->DataStructure();

  if (!DS.IsNull()) {
    DS->Projector().Scaled(Standard_True);
    Standard_Integer e1 = 1;
    Standard_Integer e2 = DS->NbEdges();
    Standard_Integer f1 = 1;
    Standard_Integer f2 = DS->NbFaces();
    Standard_Boolean explor = Standard_False;
//    Standard_Boolean todraw;
    if (!S.IsNull()) {
      Standard_Integer v1,v2;
      Standard_Integer index = myAlgo->Index(S);
      if (index == 0) explor = Standard_True;
      else            myAlgo->ShapeBounds(index).Bounds(v1,v2,e1,e2,f1,f2);
    }
    BRep_Builder B;
    B.MakeCompound(TopoDS::Compound(Result));
    HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(e1 - 1));
    
    for (Standard_Integer ie = e1; ie <= e2; ie++) {
      ed++;
      if (ed->Selected() && !ed->Vertical()) {
	ed->Used(Standard_False);
	ed->HideCount(0);
      }
      else ed->Used(Standard_True);
    }
    if (explor) {
      TopTools_IndexedMapOfShape& Edges = DS->EdgeMap();
      TopTools_IndexedMapOfShape& Faces = DS->FaceMap();
      TopExp_Explorer Exp;
      
      for (Exp.Init (S, TopAbs_FACE);
	   Exp.More();
	   Exp.Next()) {
	Standard_Integer iface = Faces.FindIndex(Exp.Current());
	if (iface != 0) DrawFace(visible,typ,iface,DS,Result,added);
      }
      if (typ >= 3) {

	for (Exp.Init (S, TopAbs_EDGE, TopAbs_FACE);
	     Exp.More();
	     Exp.Next()) {
	  Standard_Integer ie = Edges.FindIndex(Exp.Current());
	  if (ie != 0) {
	    HLRBRep_EdgeData& ed = DS->EDataArray().ChangeValue(ie);
	    if (!ed.Used()) {
	      DrawEdge(visible,Standard_False,typ,ed,Result,added);
	      ed.Used(Standard_True);
	    }
	  }
	}
      }
    }
    else {

      for (Standard_Integer iface = f1; iface <= f2; iface++)
	DrawFace(visible,typ,iface,DS,Result,added);

      if (typ >= 3) {
	HLRBRep_EdgeData* ed = &(DS->EDataArray().ChangeValue(e1 - 1));
	
	for (Standard_Integer ie = e1; ie <= e2; ie++) {
	  ed++;
	  if (!ed->Used()) {
	    DrawEdge(visible,Standard_False,typ,*ed,Result,added);
	    ed->Used(Standard_True);
	  }
	}
      }
    }
    DS->Projector().Scaled(Standard_False);
  }
  if (!added) Result = TopoDS_Shape();
  return Result;
}

//=======================================================================
//function : DrawFace
//purpose  : 
//=======================================================================

void 
HLRBRep_HLRToShape::DrawFace (const Standard_Boolean visible,
			      const Standard_Integer typ,
			      const Standard_Integer iface,
			      Handle(HLRBRep_Data)& DS,
			      TopoDS_Shape& Result,
			      Standard_Boolean& added) const
{
  HLRBRep_FaceIterator Itf;

  for (Itf.InitEdge(DS->FDataArray().ChangeValue(iface));
       Itf.MoreEdge();
       Itf.NextEdge()) {               
    Standard_Integer ie = Itf.Edge();
    HLRBRep_EdgeData& edf = DS->EDataArray().ChangeValue(ie);
    if (!edf.Used()) {
      Standard_Boolean todraw;
      if      (typ == 1) todraw =  Itf.IsoLine();
      else if (typ == 2) todraw =  Itf.Internal();
      else if (typ == 3) todraw =  edf.Rg1Line() &&
	!edf.RgNLine() && !Itf.OutLine();
      else if (typ == 4) todraw =  edf.RgNLine() && !Itf.OutLine();
      else               todraw =
	!(Itf.IsoLine()  ||
	  Itf.Internal() ||
	  (edf.Rg1Line() && !Itf.OutLine()));

       if (todraw) {
	DrawEdge(visible,Standard_True,typ,edf,Result,added);
	edf.Used(Standard_True);
      }
      else {
	if(typ > 4 && (edf.Rg1Line() && !Itf.OutLine())) {
	  Standard_Integer hc = edf.HideCount();
	  if(hc > 0) {
	    edf.Used(Standard_True);
	  }
	  else {
	    ++hc;
	    edf.HideCount(hc); //to try with another face
	  }
	}
	else {
	  edf.Used(Standard_True);
	}
      }
    }
  }
}

//=======================================================================
//function : DrawEdge
//purpose  : 
//=======================================================================

void 
HLRBRep_HLRToShape::DrawEdge (const Standard_Boolean visible,
			      const Standard_Boolean inFace,
			      const Standard_Integer typ,
			      HLRBRep_EdgeData& ed,
			      TopoDS_Shape& Result,
			      Standard_Boolean& added) const
{
  Standard_Boolean todraw = Standard_False;
  if      (inFace)   todraw = Standard_True;
  else if (typ == 3) todraw = ed.Rg1Line() && !ed.RgNLine();
  else if (typ == 4) todraw = ed.RgNLine();
  else               todraw =!ed.Rg1Line();

  if (todraw) {
    Standard_Real sta,end;
    Standard_ShortReal tolsta,tolend;
    BRep_Builder B;
    TopoDS_Edge E;
    HLRAlgo_EdgeIterator It;
    if (visible)
    {
      for (It.InitVisible(ed.Status()); It.MoreVisible(); It.NextVisible()) {
        It.Visible(sta,tolsta,end,tolend);
        E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        if (!E.IsNull())
        {
          B.Add(Result,E);
          added = Standard_True;
        }
      }
    }
    else
    {
      for (It.InitHidden(ed.Status()); It.MoreHidden(); It.NextHidden()) {
        It.Hidden(sta,tolsta,end,tolend);
        E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        if (!E.IsNull())
        {
          B.Add(Result,E);
          added = Standard_True;
        }
      }
    }
  }
}
