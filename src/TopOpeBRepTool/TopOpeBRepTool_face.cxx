// Created on: 1999-01-14
// Created by: Prestataire Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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
#include <BRep_Tool.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopOpeBRepTool_define.hxx>
#include <TopOpeBRepTool_face.hxx>

//=======================================================================
//function : TopOpeBRepTool_face
//purpose  : 
//=======================================================================
TopOpeBRepTool_face::TopOpeBRepTool_face()
{
}

static void FUN_reverse(const TopoDS_Face& f, TopoDS_Face& frev)
{
  BRep_Builder B; 
  TopoDS_Shape aLocalShape = f.EmptyCopied();
  frev = TopoDS::Face(aLocalShape);
//  frev = TopoDS::Face(f.EmptyCopied());
  TopoDS_Iterator it(f);
  while (it.More()) {
    B.Add(frev,it.Value().Reversed());
    it.Next();
  }    
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_face::Init(const TopoDS_Wire& W, const TopoDS_Face& Fref)
{
  myFfinite.Nullify();
  myW = W;

  // fres : 
//  TopoDS_Face fres;
//  Handle(Geom_Surface) su = BRep_Tool::Surface(Fref);  
//  BRep_Builder B; B.MakeFace(fres,su,Precision::Confusion());
  TopoDS_Shape aLocalShape = Fref.EmptyCopied();
  TopoDS_Face fres = TopoDS::Face(aLocalShape);
//  TopoDS_Face fres = TopoDS::Face(Fref.EmptyCopied());
  BRep_Builder B; B.Add(fres,W);
  B.NaturalRestriction(fres,Standard_True);

  // <myfinite> :
  BRepTopAdaptor_FClass2d FClass(fres,0.);
  Standard_Boolean infinite = ( FClass.PerformInfinitePoint() == TopAbs_IN);
  myfinite = !infinite;

  // <myFfinite> : 
  if (myfinite) myFfinite = fres;
  else          FUN_reverse(fres,myFfinite);
  return Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_face::IsDone() const
{
  return (!myFfinite.IsNull());
}

//=======================================================================
//function : Finite
//purpose  : 
//=======================================================================

Standard_Boolean TopOpeBRepTool_face::Finite() const
{
  if (!IsDone()) throw Standard_Failure("TopOpeBRepTool_face NOT DONE");
  return myfinite;
}

//=======================================================================
//function : Ffinite
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_face::Ffinite() const
{
  if (!IsDone()) throw Standard_Failure("TopOpeBRepTool_face NOT DONE");
  return myFfinite;
}

//=======================================================================
//function : W
//purpose  : 
//=======================================================================

const TopoDS_Wire& TopOpeBRepTool_face::W() const
{
  return myW;
}

//=======================================================================
//function : TopoDS_Face&
//purpose  : 
//=======================================================================

TopoDS_Face TopOpeBRepTool_face::RealF() const
{
  if (myfinite) return myFfinite;
  TopoDS_Face realf; FUN_reverse(myFfinite,realf);
  return realf;
}


