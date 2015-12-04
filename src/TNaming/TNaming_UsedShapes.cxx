// Created on: 1997-02-18
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape.hxx>
#include <TNaming_PtrNode.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TNaming_UsedShapes,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TNaming_UsedShapes::GetID() 
{
  static Standard_GUID TNaming_UsedShapesID("c4ef4201-568f-11d1-8940-080009dc3333");
  return TNaming_UsedShapesID;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

TNaming_UsedShapes::TNaming_UsedShapes() 
{
}


//=======================================================================
//function : Destroy
//purpose  : 
//=======================================================================

void TNaming_UsedShapes::Destroy() 
{
  myMap.Clear();
}

//=======================================================================
//function : BackupCopy
//purpose  : No Backup
//=======================================================================

Handle(TDF_Attribute) TNaming_UsedShapes::BackupCopy() const
{
  Handle(TNaming_UsedShapes) Att;
  return Att;
}

//=======================================================================
//function : BeforeRemoval
//purpose  : 
//=======================================================================

void TNaming_UsedShapes::BeforeRemoval() 
{
  Destroy();
}


//=======================================================================
//function : AfterUndo
//purpose  : After application of a TDF_Delta.
//=======================================================================

Standard_Boolean TNaming_UsedShapes::AfterUndo
(const Handle(TDF_AttributeDelta)& anAttDelta,
 const Standard_Boolean /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) {
    anAttDelta->Attribute()->BeforeRemoval();
  }
  return Standard_True;
}

//=======================================================================
//function : DeltaOnAddition
//purpose  : no delta
//=======================================================================

Handle(TDF_DeltaOnAddition) TNaming_UsedShapes::DeltaOnAddition() const
{ 
  Handle(TDF_DeltaOnAddition) aDelta;
  return aDelta; 
}
//=======================================================================
//function : DeltaOnRemoval
//purpose  : no delta
//=======================================================================

Handle(TDF_DeltaOnRemoval) TNaming_UsedShapes::DeltaOnRemoval() const
{  
  Handle(TDF_DeltaOnRemoval) aDelta;
  return aDelta;
} 

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TNaming_UsedShapes::Restore(const Handle(TDF_Attribute)& /*anAttribute*/) 
{
}

//=======================================================================
//function : NewEmpty 
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TNaming_UsedShapes::NewEmpty () const
{
  return new TNaming_UsedShapes();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void  TNaming_UsedShapes::Paste(const Handle(TDF_Attribute)&,
				const Handle(TDF_RelocationTable)&) const
{ 
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TNaming_UsedShapes::Dump(Standard_OStream& anOS) const
{
  anOS<<"The content of UsedShapes attribute:"<<endl;
  TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape itr(myMap);
  for (; itr.More(); itr.Next()) {
    anOS<<"  ";
    TopAbs::Print(itr.Key().ShapeType(),anOS);
    anOS<<"  ";
    itr.Value()->Label().EntryDump(anOS);
    anOS << " Key_TShape   = " <<itr.Key().TShape()->This();
    anOS << " Value_TShape = " <<itr.Value()->Shape().TShape()->This();    
    anOS<<endl;
  }
  return anOS;
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TNaming_UsedShapes::References(const Handle(TDF_DataSet)& ) const
{
}
