// File:	TNaming_UsedShapes.cxx
// Created:	Tue Feb 18 15:33:00 1997
// Author:	Yves FRICAUD
//		<yfr@claquox.paris1.matra-dtv.fr>


#include <TNaming_UsedShapes.ixx>

#include <TDF_DeltaOnAddition.hxx>
#include <TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape.hxx>
#include <TNaming_RefShape.hxx>

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
  TNaming_DataMapIteratorOfDataMapOfShapePtrRefShape itr(myMap);
  for (; itr.More(); itr.Next()) {
   if(itr.Value() != NULL) delete itr.Value(); // <== szy: removed "//"
  }
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
