// Created on: 1993-06-14
// Created by: Remi LEQUETTE
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



#include <MgtBRep.ixx>

#include <MgtTopoDS.hxx>
#include <MgtBRep_TranslateTool.hxx>
#include <MgtBRep_TranslateTool1.hxx>

#include <TopExp_Explorer.hxx>
#include <TopAbs.hxx>
#include <TopoDS.hxx>

#include <Geom_Surface.hxx>

#include <BRep_Tool.hxx>

//=======================================================================
//function : Translate
//purpose  : ... a Shape from BRep to PBRep.
//=======================================================================

Handle(PTopoDS_HShape) MgtBRep::Translate
(const TopoDS_Shape&              aShape,
 PTColStd_TransientPersistentMap& aMap,
 const MgtBRep_TriangleMode    aTriMode)
{
  Handle(MgtBRep_TranslateTool) TR = new MgtBRep_TranslateTool(aTriMode);
  return MgtTopoDS::Translate(aShape, TR, aMap);
}

//=======================================================================
//function : Translate
//purpose  : ... a Shape from PBRep to BRep.
//=======================================================================

void MgtBRep::Translate
(const Handle(PTopoDS_HShape)&    aShape,
 PTColStd_PersistentTransientMap& aMap,
 TopoDS_Shape&                    aResult,
 const MgtBRep_TriangleMode    aTriMode)
{
  Handle(MgtBRep_TranslateTool) TR = new MgtBRep_TranslateTool(aTriMode);
  MgtTopoDS::Translate(aShape, TR, aMap, aResult);
}
//=======================================================================
//function : Translate1
//purpose  : ... a Shape from BRep to PBRep.
//=======================================================================

void MgtBRep::Translate1
(const TopoDS_Shape& aShape,
 PTColStd_TransientPersistentMap& aMap,
 PTopoDS_Shape1& aResult,
 const MgtBRep_TriangleMode aTriMode)
{
  Handle(MgtBRep_TranslateTool1) TR = new MgtBRep_TranslateTool1(aTriMode);
  MgtTopoDS::Translate1(aShape, TR, aMap, aResult);
}


//=======================================================================
//function : Translate1
//purpose  : ... a Shape from PBRep to BRep.
//=======================================================================

void MgtBRep::Translate1
(const PTopoDS_Shape1& aShape,
 PTColStd_PersistentTransientMap& aMap,
 TopoDS_Shape& aResult,
 const MgtBRep_TriangleMode aTriMode)
{
  Handle(MgtBRep_TranslateTool1) TR = new MgtBRep_TranslateTool1(aTriMode);
  MgtTopoDS::Translate1(aShape, TR, aMap, aResult);
}
