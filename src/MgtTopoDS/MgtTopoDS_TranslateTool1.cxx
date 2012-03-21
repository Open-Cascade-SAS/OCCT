// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
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

//      	---------------------------

// Version:	0.0
//Version	Date		Purpose
//		0.0	Feb  3 1998	Creation



#include <MgtTopoDS_TranslateTool1.ixx>

#include <TopoDS_TShape.hxx>
#include <PTopoDS_TShape1.hxx>


//=======================================================================
//function : UpdateShape
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateShape
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2) const 
{
  // Transfert the flags
  // S2.TShape()->Free(S1.TShape()->Free());
  S2.TShape()->Modified(S1.TShape()->Modified());
  S2.TShape()->Checked(S1.TShape()->Checked());
  S2.TShape()->Orientable(S1.TShape()->Orientable());
  S2.TShape()->Closed(S1.TShape()->Closed());
  S2.TShape()->Infinite(S1.TShape()->Infinite());
  S2.TShape()->Convex(S1.TShape()->Convex());
}

//=======================================================================
//function : UpdateShape
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateShape
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2) const 
{
  // Transfert the flags
  S2.TShape()->Free(Standard_False); // Always frozen when coming from D.B.
  // S2.TShape()->Free(S1.TShape()->Free());
  S2.TShape()->Modified(S1.TShape()->Modified());
  S2.TShape()->Checked(S1.TShape()->Checked());
  S2.TShape()->Orientable(S1.TShape()->Orientable());
  S2.TShape()->Closed(S1.TShape()->Closed());
  S2.TShape()->Infinite(S1.TShape()->Infinite());
  S2.TShape()->Convex(S1.TShape()->Convex());
}

//=======================================================================
//function : UpdateVertex
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateVertex
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2,
// PTColStd_TransientPersistentMap& aMap) const 
 PTColStd_TransientPersistentMap& ) const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateVertex
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateVertex
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2,
// PTColStd_PersistentTransientMap& aMap)  const 
 PTColStd_PersistentTransientMap& )  const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateEdge
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateEdge
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2,
// PTColStd_TransientPersistentMap& aMap) const 
 PTColStd_TransientPersistentMap& ) const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateEdge
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateEdge
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2,
// PTColStd_PersistentTransientMap& aMap) const 
 PTColStd_PersistentTransientMap& ) const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateWire
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateWire
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateWire
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateWire
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateFace
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateFace
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2,
// PTColStd_TransientPersistentMap& aMap) const 
 PTColStd_TransientPersistentMap& ) const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateFace
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateFace
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2,
// PTColStd_PersistentTransientMap& aMap) const 
 PTColStd_PersistentTransientMap& ) const 
{ UpdateShape(S1,S2); }

//=======================================================================
//function : UpdateShell
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateShell
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateShell
//purpose  : Persistent->Transient
//=======================================================================

void  MgtTopoDS_TranslateTool1::UpdateShell
(const PTopoDS_Shape1& S1,
 TopoDS_Shape& S2)  const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateSolid
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateSolid
(const TopoDS_Shape& S1,
 PTopoDS_Shape1& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateSolid
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateSolid
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateCompSolid
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateCompSolid
(const TopoDS_Shape& S1, 
 PTopoDS_Shape1& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateCompSolid
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateCompSolid
(const PTopoDS_Shape1& S1, 
 TopoDS_Shape& S2) const 
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateCompound
//purpose  : Transient->Persistent
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateCompound
(const TopoDS_Shape& S1, 
 PTopoDS_Shape1& S2) const
{ UpdateShape(S1,S2); }


//=======================================================================
//function : UpdateCompound
//purpose  : Persistent->Transient
//=======================================================================

void MgtTopoDS_TranslateTool1::UpdateCompound
(const PTopoDS_Shape1& S1,
 TopoDS_Shape& S2) const 
{ UpdateShape(S1,S2); }
