// Created by: DAUTRY Philippe
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
