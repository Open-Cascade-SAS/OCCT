// Created on: 1997-02-18
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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



#include <TNaming_UsedShapes.ixx>

#include <TDF_DeltaOnAddition.hxx>
#include <TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_PtrNode.hxx>
#define BUC60862

#ifdef BUC60862
#include <TopoDS_Shape.hxx>
#endif

#define BUC60921      //SRN 15/05/01 : Fixes the memory leak due to pointer to RefShape is not deleted

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
#ifdef BUC60921
  Destroy();
#else
  myMap.Clear(); 
#endif 
}


//=======================================================================
//function : AfterUndo
//purpose  : After application of a TDF_Delta.
//=======================================================================

Standard_Boolean TNaming_UsedShapes::AfterUndo
(const Handle(TDF_AttributeDelta)& anAttDelta,
 const Standard_Boolean forceIt)
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

void TNaming_UsedShapes::Restore(const Handle(TDF_Attribute)& anAttribute) 
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

void  TNaming_UsedShapes::Paste(const Handle(TDF_Attribute)&       into,
				const Handle(TDF_RelocationTable)& Tab) const
{ 
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TNaming_UsedShapes::Dump(Standard_OStream& anOS) const
{
#ifdef BUC60862
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
#endif
  return anOS;
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TNaming_UsedShapes::References(const Handle(TDF_DataSet)& aDataSet) const
{
}
