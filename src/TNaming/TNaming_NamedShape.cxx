// Created on: 1996-12-18
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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


#include <BRepBuilderAPI_MakeVertex.hxx>
#include <gp_Pnt.hxx>
#include <Standard.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_DeltaOnModification.hxx>
#include <TDF_DeltaOnRemoval.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_CopyShape.hxx>
#include <TNaming_DeltaOnModification.hxx>
#include <TNaming_DeltaOnRemoval.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_PtrNode.hxx>
#include <TNaming_PtrRefShape.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TNaming_NamedShape,TDF_Attribute)

// Defines the nodes classes
//#define MDTV_DEB_HASL
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TNaming_NamedShape::GetID() 
{
  static Standard_GUID TNaming_NamedShapeID("c4ef4200-568f-11d1-8940-080009dc3333");
  return TNaming_NamedShapeID;
}

//=======================================================================
//class: TNaming_Node
//=======================================================================

class TNaming_Node {
public:
  TNaming_Node(TNaming_PtrRefShape        Old,
	       TNaming_PtrRefShape        New)
    : myOld(Old),myNew(New),
  myAtt(0L),
  nextSameAttribute(0L), nextSameOld(0L),nextSameNew(0L)
  {}
  
  //Label : Donne le Label
  TDF_Label Label();

  // NextSameShape
  TNaming_Node* NextSameShape(TNaming_RefShape* prs);

  // Test si l evolution est valide dans la transaction Trans
  // ie : Trans n est pas anterieure a sa creation
  //      et Trans n est pas posterieure a son BackUp
  Standard_Boolean IsValidInTrans(Standard_Integer Trans);

  // Memory management
  DEFINE_STANDARD_ALLOC
  
  TNaming_PtrRefShape  myOld;
  TNaming_PtrRefShape  myNew;
  TNaming_NamedShape*  myAtt;
  TNaming_PtrNode      nextSameAttribute;
  TNaming_PtrNode      nextSameOld;
  TNaming_PtrNode      nextSameNew;
};

//=======================================================================
//function : NextSameShape
//purpose  : 
//=======================================================================

TNaming_Node* TNaming_Node::NextSameShape(TNaming_RefShape* prs)
{
  if (myOld == prs) return nextSameOld;
  if (myNew == prs) return nextSameNew;
  return nextSameNew;
}  

//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_Node::Label() 
{ 
  return myAtt->Label();
}

//=======================================================================
//function : IsValidInTrans
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Node::IsValidInTrans(Standard_Integer Trans)
{
  if (myAtt->Transaction() <= Trans && Trans <= myAtt->UntilTransaction()) {
    return 1;
  }
  return 0;
}

//**********************************************************************
// Methods of TNaming_NamedShape
//**********************************************************************

//=======================================================================
//function : TNaming_NamedShape
//purpose  : 
//=======================================================================

TNaming_NamedShape::TNaming_NamedShape()
{ 
  myNode    = 0L;
  myVersion = 0;
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean  TNaming_NamedShape::IsEmpty () const
{  
  TNaming_Iterator it (this);
  return !it.More();
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

TopoDS_Shape TNaming_NamedShape::Get () const
{  
  return TNaming_Tool::GetShape (this);
}




//=======================================================================
//function : RemoveNode
//purpose  : 
//=======================================================================

static void RemoveNode(Standard_Boolean                   MapExist ,
		       TNaming_DataMapOfShapePtrRefShape& M, 
		       TNaming_Node*&                     N) 
{
  TNaming_RefShape* pos  = N->myOld;  
  if (pos != 0L) {
    if (pos->FirstUse() == N) {
      TNaming_Node*  nextOld = N->nextSameOld;
      if (nextOld != 0L) 
        pos->FirstUse(nextOld);
      else {
        // le shape disparait
        if (MapExist)
          M.UnBind(pos->Shape());
        N->myOld = 0L;
        if(pos != N->myNew)
        {
          delete pos;
          pos = 0L;
        }
      }
    }
    else {
      TNaming_Node* pdn = pos->FirstUse();
      while (pdn != 0L) {
        
        if (pdn->NextSameShape(pos) == N) {
          if (pdn->myOld == pos) pdn->nextSameOld =  N->nextSameOld;
          else                   pdn->nextSameNew =  N->nextSameOld;
          break;
        }
        pdn = pdn->NextSameShape(pos);
      }
    }
  }

  TNaming_RefShape* pns  = N->myNew;  
  if (pns != 0L) {
    if (pns->FirstUse() == N) {
      TNaming_Node*  nextNew = N->nextSameNew;
      if (nextNew != 0L) 
        pns->FirstUse(nextNew);
      else 
      {
        // le shape disparait
        if (MapExist) 
          M.UnBind(pns->Shape());
      
        pns->FirstUse(0L);
        delete pns;
        pns = 0L;

        N->myNew = 0L;
      
      }
    }
    else {
      TNaming_Node* pdn = pns->FirstUse();
      while (pdn != 0L) {
        if (pdn->NextSameShape(pns) == N) {
          if (pdn->myOld == pns) pdn->nextSameOld =  N->nextSameNew;
          else                   pdn->nextSameNew =  N->nextSameNew;
          break;
        }
        pdn = pdn->NextSameShape(pns);
      }
    }
  }
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TNaming_NamedShape::Clear()
{
  if (Label().IsNull()) {
#ifdef OCCT_DEBUG_BUILDER
    cout << "attention etat fantomatique" << endl;
#endif
    return;
  }

  Handle(TNaming_UsedShapes) US;
    
  TNaming_DataMapOfShapePtrRefShape* M=NULL;
    
  // Recuperation de la map si celle-ci n est pas deja detruite.
  //Standard_Boolean MapExist = Ins.FindInRoot(TNaming_UsedShapes::GetID(),US);

  Standard_Boolean MapExist = Label().Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  if (MapExist) M = &(US->Map());
    
  TNaming_Node* p = myNode;
  while (p != 0L) {
    RemoveNode (MapExist,*M,p);
    p = p->nextSameAttribute;
  }
    
  p = myNode;
  TNaming_Node* q;
  while (p != 0L) {
    q = p;
    p = p->nextSameAttribute;
    if( q !=0L)
    {
      delete q;
      q = 0L;
    }
  }
    
  myNode = 0L;
}

//=======================================================================
//function : BeforeRemoval
//purpose  : 
//=======================================================================

void TNaming_NamedShape::BeforeRemoval()
{
  Clear();
}


//=======================================================================
//function : BeforeUndo
//purpose  : before application of a TDF_Delta.
//=======================================================================

Standard_Boolean TNaming_NamedShape::BeforeUndo
  (const Handle(TDF_AttributeDelta)& /*anAttDelta*/,
   const Standard_Boolean /*forceIt*/)
{
//  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) {
//    anAttDelta->Attribute()->BeforeRemoval();
//  }
  return Standard_True;
}

//=======================================================================
//function : AfterUndo 
//purpose  : After application of a TDF_Delta.
//=======================================================================

Standard_Boolean TNaming_NamedShape::AfterUndo
  (const Handle(TDF_AttributeDelta)& anAttDelta,
   const Standard_Boolean /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) {
    Handle(TNaming_UsedShapes) US;

    TNaming_DataMapOfShapePtrRefShape* M=NULL;

  // Recuperation de la map si celle-ci n est pas deja detruite.
    //Standard_Boolean MapExist = Ins.FindInRoot(TNaming_UsedShapes::GetID(),US);
    
    Standard_Boolean MapExist = anAttDelta->Label().Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

    if (MapExist) M = &(US->Map());
    
    TNaming_Node* p = myNode;
    while (p != 0L) {
      RemoveNode (MapExist,*M,p);
      p = p->nextSameAttribute;
    }
    
    p = myNode;
    TNaming_Node* q;
    while (p != 0L) {
      q = p;
      p = p->nextSameAttribute;
      if(q != 0L)
      {
        delete q;
        q = 0L;
      }
    }

    myNode = 0L;
  }
  return Standard_True;
}


//=======================================================================
//function : BackupCopy
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TNaming_NamedShape::BackupCopy() const
{
  // Remarque dans le copy il est important de reporter le noeud de l attribut
  // pour ne pas casser le chemin nextSameShape.

  Handle(TNaming_NamedShape) Cop = new TNaming_NamedShape();
  Cop->myNode      = myNode;
  Cop->myEvolution = myEvolution;
  Cop->myVersion   = myVersion;

  // Mise a jour de myAtt sur les noeuds dans l attribut.
  TNaming_Node*  CN = Cop->myNode;

  Handle(TNaming_NamedShape) A = this;
  A->myNode = 0L;

  while (CN != 0L) {
    CN->myAtt = Cop.operator->();
    CN        = CN->nextSameAttribute;
  }
  return Cop;
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TNaming_NamedShape::Restore(const Handle(TDF_Attribute)& anAttribute) 
{
  Clear();

  TNaming_NamedShape* PAtt  = (TNaming_NamedShape*)anAttribute.operator->();
  myNode                   = PAtt->myNode;
  myEvolution              = PAtt->myEvolution;
  myVersion                = PAtt->myVersion;
  
  // Mise a jour de myAtt sur les noeuds dans l attribut.
  TNaming_Node*  CN = myNode;
  while (CN != 0L) {
    CN->myAtt = this;
    CN        = CN->nextSameAttribute;
  }
  PAtt->myNode = 0L; //un noeud est dans un seul attribut.
}


//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnModification) TNaming_NamedShape::DeltaOnModification
(const Handle(TDF_Attribute)& anOldAttribute) const
{
  
  return new TNaming_DeltaOnModification(Handle(TNaming_NamedShape)::DownCast (anOldAttribute));
}

//=======================================================================
//function : DeltaOnModification
//purpose  : 
//=======================================================================

void TNaming_NamedShape::DeltaOnModification(const Handle(TDF_DeltaOnModification)& aDelta) 
{
  aDelta->Apply();
}

//=======================================================================
//function : DeltaOnRemoval
//purpose  : 
//=======================================================================

Handle(TDF_DeltaOnRemoval) TNaming_NamedShape::DeltaOnRemoval() const
{
  return new TNaming_DeltaOnRemoval(this);
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TNaming_NamedShape::NewEmpty () const
{
  return new TNaming_NamedShape();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TNaming_NamedShape::Paste(const Handle(TDF_Attribute)&       into,
			       const Handle(TDF_RelocationTable)& Tab) 
const
{ 
  TDF_Label        Lab = into->Label();
  if (Lab.IsNull()) {
    throw Standard_NullObject("TNaming_NamedShape::Paste");
  }
  TNaming_Builder B(Lab);

  TNaming_Iterator It (this);
  for ( ;It.More() ; It.Next()) {
    const TopoDS_Shape& OS     = It.OldShape();
    const TopoDS_Shape& NS     = It.NewShape();
    TNaming_Evolution   aStatus = It.Evolution();

// Modification_1 24.06.99 (szy)  
    TopoDS_Shape copOS, copNS;
    if(aStatus != TNaming_PRIMITIVE)
      TNaming_CopyShape::CopyTool(OS, Tab->TransientTable(), copOS);
    else copOS.Nullify();    
    if(aStatus != TNaming_DELETE )
      TNaming_CopyShape::CopyTool(NS, Tab->TransientTable(), copNS);
    else copNS.Nullify();
    
    switch (aStatus) {
    case TNaming_PRIMITIVE :
      {
	B.Generated(copNS);
	break;
      }
    case TNaming_GENERATED :
      {
	B.Generated(copOS, copNS);
	break;
      }
    case TNaming_MODIFY : 
      {
	B.Modify(copOS, copNS);
	break;
      }
    case TNaming_DELETE : 
      {
	B.Delete (copOS);
	break;
      }
    case TNaming_SELECTED :
      {
	B.Select(copNS,copOS);
	break;
      }

    default:
	break;
    }
  }
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TNaming_NamedShape::References(const Handle(TDF_DataSet)& aDataSet) const
{
  // Recherche des dependances.
  // Pour chaque OldShape de l attribut on ajoute au dataSet son label d origine.
  TNaming_Node* Current = myNode;
  while (Current != NULL) {
    if (Current->myOld != NULL) {
      TNaming_RefShape* prs = Current->myOld;
      TNaming_Node*     pdn = prs->FirstUse();
      
      while (pdn != NULL) {
	if (pdn->myNew == prs && pdn->myAtt->Evolution() != TNaming_SELECTED) {
	  aDataSet->AddLabel(pdn->Label());
	}
	pdn = pdn->NextSameShape(prs);
      }
    }
    Current = Current->nextSameAttribute;
  }
}


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void TNaming_NamedShape::Add(TNaming_Node*& pdn )
{ 
  pdn->myAtt             = this;
  if (myNode != 0L){
    pdn->nextSameAttribute = myNode;
  }
  myNode = pdn;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TNaming_NamedShape::Dump(Standard_OStream& anOS) const
{
  return anOS;
}

//***************************************
//       Fin Class Named_Shape.
//***************************************


//**********************************************************************
// Methods of the TNaming_Builder class
//**********************************************************************

///=======================================================================
//function : TNaming_Builder
//purpose  : 
//=======================================================================

TNaming_Builder::TNaming_Builder (const TDF_Label& L)
{
  // Find or Build Map;
  const TDF_Label& root = L.Root();
  if (!root.FindAttribute(TNaming_UsedShapes::GetID(),myShapes)) {
    myShapes = new TNaming_UsedShapes();
    root.AddAttribute (myShapes);
  }
  
  //Find Or Build Attribute in LIns.
  if (!L.FindAttribute(TNaming_NamedShape::GetID(),myAtt)) {
    myAtt = new TNaming_NamedShape();  
    L.AddAttribute(myAtt);
  }
  else {
    myAtt->Backup();
    myAtt->Clear();  
    myAtt->myVersion++;
  }
}

//=======================================================================
//function : TNaming_Builder
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_Builder::NamedShape() const
{
  return myAtt;
}

//=======================================================================
//function : UpdateNextSameShape
//purpose  : 
//=======================================================================

static void UpdateFirstUseOrNextSameShape(TNaming_RefShape*& prs,
					  TNaming_Node*& pdn)
{  
  TNaming_Node* ldn = prs->FirstUse();
  if (ldn == 0L) {
    prs->FirstUse(pdn);
  }
  else {
    TNaming_Node* cdn = ldn;
    while (cdn != 0L) {
      ldn = cdn;
      cdn = cdn->NextSameShape(prs);
      if (ldn == cdn) {
	throw Standard_ConstructionError("UpdateFirstUseOrNextSameShape");
	break;
      }
    }
    // boucle interdite et inutile.
    if (ldn != pdn) {
      if (ldn->myOld == prs) ldn->nextSameOld = pdn;
      if (ldn->myNew == prs) ldn->nextSameNew = pdn;
    }
  }
}

//=======================================================================
//function : Generate
//purpose  : 
//=======================================================================

void TNaming_Builder::Generated(const TopoDS_Shape& newShape) 
{
  if (myAtt->myNode == 0L) myAtt->myEvolution = TNaming_PRIMITIVE;
  else {
    if (myAtt->myEvolution != TNaming_PRIMITIVE)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape* pos = 0L;
  TNaming_RefShape* pns;
  
  if (myShapes->myMap.IsBound(newShape)) {
#ifdef OCCT_DEBUG_BUILDER
    cout <<"TNaming_Builder::Generate : the shape is already in the attribute"<<endl;
#endif
    pns = myShapes->myMap.ChangeFind(newShape);
    if (pns->FirstUse()->myAtt  == myAtt.operator->()) {
      throw Standard_ConstructionError("TNaming_Builder::Generate");
    }
    TNaming_Node* pdn = new TNaming_Node(pos,pns);
    myAtt->Add(pdn);
    UpdateFirstUseOrNextSameShape (pns,pdn);
  }
  else {
    pns = new TNaming_RefShape(newShape);    
    TNaming_Node*     pdn = new TNaming_Node(pos,pns); 
    pns  ->FirstUse(pdn);
    myShapes->myMap.Bind (newShape , pns);
    myAtt->Add(pdn);
  }
}



//=======================================================================
//function : Delete
//purpose  : 
//=======================================================================

void TNaming_Builder::Delete(const TopoDS_Shape& oldShape) 
{  
  if (myAtt->myNode == 0L) myAtt->myEvolution = TNaming_DELETE;
  else {
    if (myAtt->myEvolution != TNaming_DELETE)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape* pns;
  TNaming_RefShape* pos;

  if (myShapes->myMap.IsBound(oldShape)) 
    pos = myShapes->myMap.ChangeFind(oldShape); 
  else {
#ifdef OCCT_DEBUG_BUILDER
    cout <<"TNaming_Builder::Delete : the shape is not in the data"<<endl;
#endif
    pos = new TNaming_RefShape(oldShape);  
    myShapes->myMap.Bind(oldShape, pos);
  }

  TopoDS_Shape nullShape;
  pns = new TNaming_RefShape(nullShape);  
  myShapes->myMap.Bind(nullShape, pns);

  TNaming_Node*     pdn = new TNaming_Node(pos,pns);   
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape (pos,pdn);
}

//=======================================================================
//function : Generate
//purpose  : 
//=======================================================================

void TNaming_Builder::Generated(const TopoDS_Shape& oldShape,
				const TopoDS_Shape& newShape) 
{  
  if (myAtt->myNode == 0L) myAtt->myEvolution = TNaming_GENERATED;
  else {
    if (myAtt->myEvolution != TNaming_GENERATED)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  if (oldShape.IsSame(newShape)) {
#ifdef OCCT_DEBUG_BUILDER
    cout <<"TNaming_Builder::Generate : oldShape IsSame newShape"<<endl;
#endif
    return;
  }
  TNaming_RefShape* pos;
  if (!myShapes->myMap.IsBound(oldShape)) {
    pos = new TNaming_RefShape(oldShape);
    myShapes->myMap.Bind(oldShape,pos);
  }
  else
    pos = myShapes->myMap.ChangeFind(oldShape);
  
  TNaming_RefShape* pns;
  if (!myShapes->myMap.IsBound(newShape)) {
    pns = new TNaming_RefShape(newShape);
    myShapes->myMap.Bind(newShape,pns);
  }
  else
    pns = myShapes->myMap.ChangeFind(newShape);
  
  TNaming_Node* pdn = new TNaming_Node(pos,pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape (pos,pdn);
  UpdateFirstUseOrNextSameShape (pns,pdn);
}


//=======================================================================
//function : Modify
//purpose  : 
//=======================================================================

void TNaming_Builder::Modify(const TopoDS_Shape& oldShape,
			      const TopoDS_Shape& newShape) 
{ 
  if (myAtt->myNode == 0L) myAtt->myEvolution = TNaming_MODIFY;
  else {
    if (myAtt->myEvolution != TNaming_MODIFY)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  if (oldShape.IsSame(newShape)) {
#ifdef OCCT_DEBUG_BUILDER
    cout <<"TNaming_Builder::Modify : oldShape IsSame newShape"<<endl;
#endif
    return;
  }
  TNaming_RefShape* pos;
  if (!myShapes->myMap.IsBound(oldShape)) {
    pos = new TNaming_RefShape(oldShape);
    myShapes->myMap.Bind(oldShape,pos);
  }
  else
    pos = myShapes->myMap.ChangeFind(oldShape);
  
  TNaming_RefShape* pns;
  if (!myShapes->myMap.IsBound(newShape)) {
    pns = new TNaming_RefShape(newShape);
    myShapes->myMap.Bind(newShape,pns);
  }
  else
    pns = myShapes->myMap.ChangeFind(newShape);
  
  TNaming_Node* pdn = new TNaming_Node(pos,pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape (pos,pdn);
  UpdateFirstUseOrNextSameShape (pns,pdn);
  
}

//=======================================================================
//function : Select
//purpose  : 
//=======================================================================
void TNaming_Builder::Select (const TopoDS_Shape& S,
			      const TopoDS_Shape& InS)
{  
  if (myAtt->myNode == 0L) myAtt->myEvolution = TNaming_SELECTED;
  else {
    if (myAtt->myEvolution != TNaming_SELECTED)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape* pos;  
  if (!myShapes->myMap.IsBound(InS)) {
    pos = new TNaming_RefShape(InS);
    myShapes->myMap.Bind(InS,pos);
  }
  else
    pos = myShapes->myMap.ChangeFind(InS);

  TNaming_RefShape* pns;
  if (!myShapes->myMap.IsBound(S)) {
    pns = new TNaming_RefShape(S);
    myShapes->myMap.Bind(S,pns);
  }
  else
    pns = myShapes->myMap.ChangeFind(S);  

  TNaming_Node* pdn = new TNaming_Node(pos,pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape (pos,pdn);
  UpdateFirstUseOrNextSameShape (pns,pdn);
}

//**********************************************************************
//Methods of the TNaming_Iterator class
//**********************************************************************

//=======================================================================
//function : TNaming_Iterator
//purpose  : 
//=======================================================================

TNaming_Iterator::TNaming_Iterator(const Handle(TNaming_NamedShape)& Att)
:myTrans(-1)
{
  myNode  = Att->myNode; 
}

//=======================================================================
//function : TNaming_Iterator
//purpose  : 
//=======================================================================

TNaming_Iterator::TNaming_Iterator(const TDF_Label& Lab)
:myTrans(-1)
{
  Handle(TNaming_NamedShape) Att;
  if (Lab.FindAttribute(TNaming_NamedShape::GetID(),Att)) {
    myNode = Att->myNode;
  }
  else {
    myNode = 0L; 
  }
}

//=====================================================================
//function : TNaming_Iterator
//purpose  : 
//=======================================================================

TNaming_Iterator::TNaming_Iterator(const TDF_Label&       Lab,
				   const Standard_Integer Trans)
:myTrans(Trans)
{
  Handle(TDF_Attribute) Att;
  if (Lab.FindAttribute(TNaming_NamedShape::GetID(),Trans,Att)) {
    myNode = Handle(TNaming_NamedShape)::DownCast (Att)->myNode;
  }
  else {
    myNode = 0L;
#ifdef OCCT_DEBUG
    cout <<"TNaming_Iterator : No Shape for this label"<<endl;
#endif
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TNaming_Iterator::Next() 
{
  Standard_NoMoreObject_Raise_if(myNode == 0L,
				 "TNaming_Iterator::Next");
  myNode = myNode->nextSameAttribute;   
}

//=======================================================================
//function : OldShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TNaming_Iterator::OldShape() const 
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_Iterator::OldShape");
  if (myNode->myOld == 0L) {
    static TopoDS_Shape NullShape;
    return NullShape;
  }
  return myNode->myOld->Shape();
}

//=======================================================================
//function : NewShape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TNaming_Iterator::NewShape() const 
{
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_Iterator::NewShape");
  if (myNode->myNew == 0L) {
    static TopoDS_Shape NullShape;
    return NullShape;
  }
  return myNode->myNew->Shape();
}


//=======================================================================
//function : IsModification
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Iterator::IsModification() const
{
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_Iterator::IsModification");
  return (myNode->myAtt->myEvolution == TNaming_MODIFY || 
	  myNode->myAtt->myEvolution == TNaming_DELETE );
}

//=======================================================================
//function : Evolution
//purpose  : 
//=======================================================================

TNaming_Evolution  TNaming_Iterator::Evolution() const
{
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_Iterator::IsModification");
  return myNode->myAtt->myEvolution;
}



//**********************************************************************
//Methods of the TNaming_NewShapeIterator class
//**********************************************************************

//=======================================================================
//function : SelectSameShape
//purpose  : Selectionne le prochain noeud ou le shape est le meme que celui
//           de RS. Old = 0 si il doit etre new dans le noeud a chercher.
//           selection dans la transaction valide.
//           On saute aussi les noeud ou OS = NS;
//=======================================================================

static void SelectSameShape (TNaming_Node*&          myNode,
			     Standard_Boolean        Old,
			     TNaming_RefShape*&      RS,
			     const Standard_Integer& Trans)
{
  TNaming_Node* pdn = myNode;
  
  while (pdn != 0L) {
    Standard_Boolean Valid;
    if (Trans < 0) Valid = pdn->myAtt->IsValid(); 
    else Valid = pdn->IsValidInTrans(Trans);

    if (Valid)
    {
      if (Old) {
        if( pdn->myOld == RS && pdn->myNew != 0L && pdn->myNew != RS) {
          break;
        }  
      }
      else {
        if( pdn->myNew == RS && pdn->myOld != 0L && pdn->myOld != RS) {
          break;
        }
      }
    }
    pdn = pdn->NextSameShape(RS);
  }
  myNode = pdn;
}  

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator
(const TopoDS_Shape&               aShape,
 const Standard_Integer            Trans,
 const Handle(TNaming_UsedShapes)& Shapes)
:myTrans(Trans)
{ 
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
	                             "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");  
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);  
  myNode = RS->FirstUse();
  Standard_Boolean  Old(Standard_True); 
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator
(const TopoDS_Shape&               aShape,
 const Standard_Integer            Trans,
 const TDF_Label&                  access)
:myTrans(Trans)
{  
  Handle(TNaming_UsedShapes) Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),Shapes)) {    
	Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                           "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
    TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);	
    myNode = RS->FirstUse();
	Standard_Boolean  Old(Standard_True);
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator (const TNaming_Iterator& anIterator)
:myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == 0L,
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator");
  myNode = anIterator.myNode;  
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == 0L) 
    myNode = 0L;  // No descendant
  else {    
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
	Standard_Boolean Old(Standard_True);  
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator
(const TopoDS_Shape&               aShape,
 const Handle(TNaming_UsedShapes)& Shapes)
:myTrans(-1)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                         "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
  TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
  myNode = RS->FirstUse();
   Standard_Boolean  Old(Standard_True);  
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator
(const TopoDS_Shape& aShape,
 const TDF_Label&    access)
:myTrans(-1)
{
  Handle(TNaming_UsedShapes) Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),Shapes)) {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                           "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
    Standard_Boolean  Old(Standard_True);  
    TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);	
    myNode = RS->FirstUse();
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : TNaming_NewShapeIterator
//purpose  : 
//=======================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TNaming_NewShapeIterator& anIterator)
:myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == 0L,
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator");
  myNode = anIterator.myNode;
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == 0L) 
    myNode = 0L;  // No descendant
  else  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
	Standard_Boolean Old(Standard_True);
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TNaming_NewShapeIterator::Next() 
{
  TNaming_RefShape* RS  = myNode->myOld;
  myNode = myNode->NextSameShape(RS);
  Standard_Boolean  Old(Standard_True);
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_NewShapeIterator::Label() const
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_NewShapeIterator::Label");
  return myNode->Label();
}

//=======================================================================
//function : NamedShape
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_NewShapeIterator::NamedShape() const
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_NewShapeIterator::Label");
  return myNode->myAtt;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TNaming_NewShapeIterator::Shape() const
{
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_NewShapeIterator::Shape"); 
  return myNode->myNew->Shape();
}

//=======================================================================
//function : IsModification
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_NewShapeIterator::IsModification() const
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_NewShapeIterator::IsModification");

  return (myNode->myAtt->myEvolution == TNaming_MODIFY || 
	  myNode->myAtt->myEvolution == TNaming_DELETE );
}

//**********************************************************************
//Methods of the TNaming_OldShapeIterator class
//**********************************************************************
//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator
(const TopoDS_Shape&               aShape,
 const Standard_Integer            Trans,
 const Handle(TNaming_UsedShapes)& Shapes)
:myTrans(Trans)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                         "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");
  TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
  myNode = RS->FirstUse();
  Standard_Boolean  Old(Standard_False);
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator 
(const TopoDS_Shape& aShape,
 const Standard_Integer Trans,
 const TDF_Label& access)
:myTrans(Trans)
{
  Handle(TNaming_UsedShapes) Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),Shapes)) {
	Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                           "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");    
    TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
    myNode = RS->FirstUse();
	Standard_Boolean  Old(Standard_False);
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}
//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator
(const TopoDS_Shape&               aShape,
 const Handle(TNaming_UsedShapes)& Shapes)
:myTrans(-1)
{  
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                         "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape"); 
  TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
  myNode = RS->FirstUse();
  Standard_Boolean  Old(Standard_False);
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator
(const TopoDS_Shape& aShape,
 const TDF_Label& access)
:myTrans(-1)
{  
  Handle(TNaming_UsedShapes) Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),Shapes)) {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape), 
		                           "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape"); 
    TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
    myNode = RS->FirstUse();
	Standard_Boolean  Old(Standard_False);
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator (const TNaming_Iterator& anIterator)
:myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == 0L,
				                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator");
  myNode = anIterator.myNode; 
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == 0L) 
    myNode = 0L;  // No descendant
  else {    
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
	Standard_Boolean Old(Standard_False);  
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : TNaming_OldShapeIterator
//purpose  : 
//=======================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TNaming_OldShapeIterator& anIterator)
:myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == 0L,
				                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator");
  myNode = anIterator.myNode;  
  TNaming_RefShape* RS = myNode->myOld;
  if (RS == 0L) 
    myNode = 0L;  // No descendant
  else  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
	Standard_Boolean Old(Standard_False);  
    SelectSameShape(myNode,Old,RS,myTrans);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TNaming_OldShapeIterator::Next() 
{
  Standard_Boolean  Old = Standard_False;  
  TNaming_RefShape* RS  = myNode->myNew;
  myNode = myNode->NextSameShape(RS);
  SelectSameShape(myNode,Old,RS,myTrans);
}

//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_OldShapeIterator::Label() const
{  
  if (myNode == 0L) throw Standard_NoSuchObject("TNaming_OldShapeIterator::Label");
  return myNode->Label();
  
}

//=======================================================================
//function : NamedShape
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_OldShapeIterator::NamedShape() const
{  
  if (myNode == 0L) throw Standard_NoSuchObject("TNaming_OldShapeIterator::Label");
  return myNode->myAtt;
}
//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TNaming_OldShapeIterator::Shape() const
{   
  if(myNode == 0L) throw Standard_NoSuchObject("TNaming_OldShapeIterator::Shape");
  return myNode->myOld->Shape();
}

//=======================================================================
//function : IsModification
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_OldShapeIterator::IsModification() const
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_OldShapeIterator::IsModification");  
  return (myNode->myAtt->myEvolution == TNaming_MODIFY || 
	  myNode->myAtt->myEvolution == TNaming_DELETE );
}


//**********************************************************************
//Methods of the SameShapeIterator
//**********************************************************************

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

TNaming_SameShapeIterator::TNaming_SameShapeIterator
(const TopoDS_Shape& aShape,
 const Handle(TNaming_UsedShapes)& Shapes)
{
  TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
  myNode  = RS->FirstUse();
  myIsNew = (myNode->myNew == RS);
}


//=======================================================================
//function : TNaming_SameShapeIterator
//purpose  : 
//=======================================================================

TNaming_SameShapeIterator::TNaming_SameShapeIterator
(const TopoDS_Shape& aShape,
 const TDF_Label& access)
{  
  Handle(TNaming_UsedShapes) Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),Shapes)) { 
    TNaming_RefShape* RS  = Shapes->Map().ChangeFind(aShape);
    myNode  = RS->FirstUse();
    myIsNew = (myNode->myNew == RS);
  }
}

//=======================================================================
//function : Next
//purpose  : 
//=======================================================================

void TNaming_SameShapeIterator::Next() 
{
  TNaming_RefShape* prs;
  if (myIsNew) prs = myNode->myNew; else prs = myNode->myOld;
  
  myNode = myNode->NextSameShape(prs);
  if (myNode != 0L) myIsNew = (myNode->myNew == prs);
}

//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_SameShapeIterator::Label() const
{  
  Standard_NoSuchObject_Raise_if(myNode == 0L,
				 "TNaming_SameShapeIterator::Label");
  return myNode->Label();
}


//**********************************************************************
//Methods of the TNaming_RefShape
//**********************************************************************
//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_RefShape::Label() const
{
  return myFirstUse->myAtt->Label();
}

//=======================================================================
//function : NamedShape
//purpose  : 
//=======================================================================

Handle(TNaming_NamedShape) TNaming_RefShape::NamedShape() const
{
  return myFirstUse->myAtt;
}


//**********************************************************************
//Methods of the TNaming_Tool class
//**********************************************************************

//=======================================================================
//function : HasLabel
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Tool::HasLabel (const TDF_Label&    access,
					 const TopoDS_Shape& S)
{
  Handle(TNaming_UsedShapes) US;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(),US)) {
    return (US->Map().IsBound(S));
  }
#ifdef OCCT_DEBUG_HASL
  cout << "##==> Sub-Shape has no Label!" <<endl;
#endif
  return Standard_False;
}


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_Tool::Label(const TDF_Label&    access,
			      const TopoDS_Shape& S,
			      Standard_Integer&   Trans)
{ 
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(access,S),"TNaming_Tool::Label"); 
  Handle(TNaming_UsedShapes) US;
  access.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  return TNaming_Tool::Label(US,S,Trans);
}


//=======================================================================
//function : IsValidInTrans
//purpose  : un shape est valid tant que l attribut ou il est cree est valid 
//=======================================================================

Standard_Integer TNaming_Tool::ValidUntil (const TDF_Label& access, const TopoDS_Shape& S)
{  
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(access,S),"TNaming_Tool::ValidUntil"); 
  Handle(TNaming_UsedShapes) US;
  access.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  return TNaming_Tool::ValidUntil(S,US);
}
 

//=======================================================================
//function : HasLabel
//purpose  : 
//=======================================================================

Standard_Boolean TNaming_Tool::HasLabel(const Handle(TNaming_UsedShapes)& Shapes,
					const TopoDS_Shape&               S)
{
  return (Shapes->Map().IsBound(S));
}


//=======================================================================
//function : Label
//purpose  : 
//=======================================================================

TDF_Label TNaming_Tool::Label(const Handle(TNaming_UsedShapes)& Shapes,
			      const TopoDS_Shape&               S,
			      Standard_Integer&                 Trans)
{
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(Shapes,S),"TNaming_Tool::Label");
  TNaming_RefShape* prs = Shapes->Map().Find(S);
  TNaming_Node*     pdn = prs->FirstUse();

  while (pdn != 0L && !(pdn->myNew == prs && pdn->myAtt->Evolution() != TNaming_SELECTED)){
    pdn = pdn->NextSameShape(prs);
  }
  if (pdn == 0L) pdn = prs->FirstUse();

  TDF_Label      L   = pdn->Label();
  Trans = pdn->myAtt->Transaction();
  return L;
}
//=======================================================================
//function : NamedShape
//purpose  : 
//=======================================================================
Handle(TNaming_NamedShape) TNaming_Tool::NamedShape(const TopoDS_Shape& S,
						    const TDF_Label&    Acces) 
{ 
  Handle(TNaming_UsedShapes) US;
  Acces.Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
  Handle(TNaming_NamedShape) NS;
  
  if(!TNaming_Tool::HasLabel(US,S)) {
    return NS;
  }
  
  TNaming_RefShape* prs = US->Map().Find(S);
  TNaming_Node*     pdn = prs->FirstUse();
  TNaming_Node*     res = 0L;

  while (pdn != 0L) {
    if (pdn->myNew == prs && pdn->myAtt->Evolution() != TNaming_SELECTED) {
      res = pdn;
      if (pdn->myAtt->Evolution() != TNaming_GENERATED) {
	// Les modifications sont privilegiees par rapport au generation.
	// Dans le cas des shapes qui sont a la fois des modifications et des generations
	// faces tangentes.
	break;
      }
    }
    pdn = pdn->NextSameShape(prs);
  }
  
  if (res == 0L) return NS;

  // VERUE EN ATTENDANT DE REVOIR ABORT 03/11/98
  // Protection pour eviter de renvoyer un attribut backuped
  TDF_Label Lab = res->Label();
  Lab.FindAttribute(TNaming_NamedShape::GetID(),NS);
  return NS;
  //  return res->myAtt;
} 

//=======================================================================
//function : IsValidInTrans
//purpose  : un shape est valid tant que l attribut ou il est cree est valid 
//=======================================================================

Standard_Integer TNaming_Tool::ValidUntil (const TopoDS_Shape&               S,
					   const Handle(TNaming_UsedShapes)& US)
{  
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(US,S),"TNaming_Tool::ValidUntil"); 
  
  TNaming_RefShape* RS  = US->Map().ChangeFind(S);
  Standard_Integer  Cur;
  Standard_Integer  Until  = 0;  
  TNaming_Node*     Node   = RS->FirstUse();

  while (Node != 0L) {
    if (Node->myNew != 0L && Node->myNew == RS) {
      Cur    = Node->myAtt->UntilTransaction();
      if (Cur > Until) Until = Cur;
    }
    Node = Node->NextSameShape(RS);
  }
  return Until;
}
