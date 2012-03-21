// Created on: 1999-01-14
// Created by: Prestataire Xuan PHAM PHU
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TopOpeBRepTool_face.ixx>
#include <TopOpeBRepTool_define.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Precision.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>

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
  if (!IsDone()) Standard_Failure::Raise("TopOpeBRepTool_face NOT DONE");
  return myfinite;
}

//=======================================================================
//function : Ffinite
//purpose  : 
//=======================================================================

const TopoDS_Face& TopOpeBRepTool_face::Ffinite() const
{
  if (!IsDone()) Standard_Failure::Raise("TopOpeBRepTool_face NOT DONE");
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


