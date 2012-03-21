// Created on: 2002-12-10
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V2.0

#include <StepElement_ElementAspect.ixx>
#include <StepElement_ElementAspectMember.hxx>
#include <TCollection_HAsciiString.hxx>

//=======================================================================
//function : StepElement_ElementAspect
//purpose  : 
//=======================================================================

StepElement_ElementAspect::StepElement_ElementAspect ()
{
}

//=======================================================================
//function : CaseNum
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::CaseNum (const Handle(Standard_Transient)& ent) const
{
  return 0;
}

//=======================================================================
//function : CaseMem
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::CaseMem (const Handle(StepData_SelectMember)& ent) const
{
 if(ent.IsNull()) return 0;
 if(ent->Matches("ELEMENT_VOLUME")) return 1;
 else if(ent->Matches("VOLUME_3D_FACE")) return 2;
 else if(ent->Matches("VOLUME_2D_FACE")) return 3;
 else if(ent->Matches("VOLUME_3D_EDGE")) return 4;
 else if(ent->Matches("VOLUME_2D_EDGE")) return 5;
 else if(ent->Matches("SURFACE_3D_FACE")) return 6;
 else if(ent->Matches("SURFACE_2D_FACE")) return 7;
 else if(ent->Matches("SURFACE_3D_EDGE")) return 8;
 else if(ent->Matches("SURFACE_2D_EDGE")) return 9;
 else if(ent->Matches("CURVE_EDGE")) return 10;
 else return 0;
}

//=======================================================================
//function : NewMember
//purpose  : 
//=======================================================================

Handle(StepData_SelectMember) StepElement_ElementAspect::NewMember() const
{
 return new StepElement_ElementAspectMember;
}

//=======================================================================
//function : SetElementVolume
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetElementVolume (const StepElement_ElementVolume val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("ELEMENT_VOLUME");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : ElementVolume
//purpose  : 
//=======================================================================

StepElement_ElementVolume StepElement_ElementAspect::ElementVolume () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_Volume;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("ELEMENT_VOLUME");
  if(name->IsDifferent(nameitem)) return StepElement_Volume;
  Standard_Integer numit = SelMem->Enum();
  StepElement_ElementVolume val;
  switch(numit) {
  case 1 : val = StepElement_Volume; break;
    default : return StepElement_Volume;break;
  }
 return val;
}

//=======================================================================
//function : SetVolume3dFace
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetVolume3dFace (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("VOLUME_3D_FACE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Volume3dFace
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Volume3dFace () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("VOLUME_3D_FACE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetVolume2dFace
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetVolume2dFace (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("VOLUME_2D_FACE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Volume2dFace
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Volume2dFace () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("VOLUME_2D_FACE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetVolume3dEdge
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetVolume3dEdge (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("VOLUME_3D_EDGE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Volume3dEdge
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Volume3dEdge () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("VOLUME_3D_EDGE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetVolume2dEdge
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetVolume2dEdge (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("VOLUME_2D_EDGE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Volume2dEdge
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Volume2dEdge () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("VOLUME_2D_EDGE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetSurface3dFace
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetSurface3dFace (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("SURFACE_3D_FACE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Surface3dFace
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Surface3dFace () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("SURFACE_3D_FACE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetSurface2dFace
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetSurface2dFace (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("SURFACE_2D_FACE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Surface2dFace
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Surface2dFace () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("SURFACE_2D_FACE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetSurface3dEdge
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetSurface3dEdge (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("SURFACE_3D_EDGE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Surface3dEdge
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Surface3dEdge () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("SURFACE_3D_EDGE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetSurface2dEdge
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetSurface2dEdge (const Standard_Integer val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("SURFACE_2D_EDGE");
 SelMem->SetName(name->ToCString());
 SelMem->SetInteger(val);
}

//=======================================================================
//function : Surface2dEdge
//purpose  : 
//=======================================================================

Standard_Integer StepElement_ElementAspect::Surface2dEdge () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return 0;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
 name->AssignCat(SelMem->Name());
 Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("SURFACE_2D_EDGE");
 if(name->IsDifferent(nameitem)) return 0;
 Standard_Integer val = SelMem->Integer();
 return val;
}

//=======================================================================
//function : SetCurveEdge
//purpose  : 
//=======================================================================

void StepElement_ElementAspect::SetCurveEdge (const StepElement_CurveEdge val)
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return;
 Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("CURVE_EDGE");
 SelMem->SetName(name->ToCString());
 SelMem->SetEnum((Standard_Integer)val);
}

//=======================================================================
//function : CurveEdge
//purpose  : 
//=======================================================================

StepElement_CurveEdge StepElement_ElementAspect::CurveEdge () const
{
  Handle(StepElement_ElementAspectMember) SelMem = Handle(StepElement_ElementAspectMember)::DownCast(Value());
  if(SelMem.IsNull()) return StepElement_ElementEdge;
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString;
  name->AssignCat(SelMem->Name());
  Handle(TCollection_HAsciiString) nameitem = new TCollection_HAsciiString("CURVE_EDGE");
  if(name->IsDifferent(nameitem)) return StepElement_ElementEdge;
  Standard_Integer numit = SelMem->Enum();
  StepElement_CurveEdge val;
  switch(numit) {
  case 1 : val = StepElement_ElementEdge; break;
    default : return StepElement_ElementEdge;break;
  }
 return val;
}
