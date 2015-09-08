// Created on: 1993-10-11
// Created by: Christophe MARION
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


#include <BRep_Builder.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_Data.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <HLRBRep_ShapeBounds.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

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
				      const TopoDS_Shape& S,
                                      const Standard_Boolean In3d)
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
	if (iface != 0)
          DrawFace(visible,typ,iface,DS,Result,added,In3d);
      }
      if (typ >= 3) {

	for (Exp.Init (S, TopAbs_EDGE, TopAbs_FACE);
	     Exp.More();
	     Exp.Next()) {
	  Standard_Integer ie = Edges.FindIndex(Exp.Current());
	  if (ie != 0) {
	    HLRBRep_EdgeData& EDataIE = DS->EDataArray().ChangeValue(ie);
	    if (!EDataIE.Used()) {
	      DrawEdge(visible,Standard_False,typ, EDataIE,Result,added,In3d);
	      EDataIE.Used(Standard_True);
	    }
	  }
	}
      }
    }
    else {

      for (Standard_Integer iface = f1; iface <= f2; iface++)
	DrawFace(visible,typ,iface,DS,Result,added,In3d);

      if (typ >= 3) {
	HLRBRep_EdgeData* EDataE11 = &(DS->EDataArray().ChangeValue(e1 - 1));
	
	for (Standard_Integer ie = e1; ie <= e2; ie++) {
          EDataE11++;
	  if (!EDataE11->Used()) {
	    DrawEdge(visible,Standard_False,typ,*EDataE11,Result,added,In3d);
	    EDataE11->Used(Standard_True);
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
			      Standard_Boolean& added,
                              const Standard_Boolean In3d) const
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
      else if (typ == 2) //outlines
      {
        if (In3d)
          todraw = Itf.Internal() || Itf.OutLine();
        else
          todraw =  Itf.Internal();
      }
      else if (typ == 3) todraw =  edf.Rg1Line() &&
	!edf.RgNLine() && !Itf.OutLine();
      else if (typ == 4) todraw =  edf.RgNLine() && !Itf.OutLine();
      else               todraw =
	!(Itf.IsoLine()  ||
	  Itf.Internal() ||
	  (edf.Rg1Line() && !Itf.OutLine()));

       if (todraw) {
         DrawEdge(visible,Standard_True,typ,edf,Result,added,In3d);
	edf.Used(Standard_True);
      }
      else {
	if((typ > 4 || typ == 2) && //sharp or outlines
           (edf.Rg1Line() && !Itf.OutLine()))
        {
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
			      Standard_Boolean& added,
                              const Standard_Boolean In3d) const
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
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(),sta,end);
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
        if (!In3d)
          E = HLRBRep::MakeEdge(ed.Geometry(),sta,end);
        else
          E = HLRBRep::MakeEdge3d(ed.Geometry(),sta,end);
        if (!E.IsNull())
        {
          B.Add(Result,E);
          added = Standard_True;
        }
      }
    }
  }
}
