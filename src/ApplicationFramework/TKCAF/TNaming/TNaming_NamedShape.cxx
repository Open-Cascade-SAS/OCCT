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
#include <Standard.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_NoMoreObject.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_Type.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_CopyShape.hxx>
#include <TNaming_DeltaOnModification.hxx>
#include <TNaming_DeltaOnRemoval.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_PtrNode.hxx>
#include <TNaming_RefShape.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_UsedShapes.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TNaming_NamedShape, TDF_Attribute)

// Defines the nodes classes
// #define MDTV_DEB_HASL
//=================================================================================================

const Standard_GUID& TNaming_NamedShape::GetID()
{
  static Standard_GUID TNaming_NamedShapeID("c4ef4200-568f-11d1-8940-080009dc3333");
  return TNaming_NamedShapeID;
}

//=======================================================================
// class: TNaming_Node
//=======================================================================

class TNaming_Node
{
public:
  TNaming_Node(TNaming_PtrRefShape Old, TNaming_PtrRefShape New)
      : myOld(Old),
        myNew(New),
        myAtt(nullptr),
        nextSameAttribute(nullptr),
        nextSameOld(nullptr),
        nextSameNew(nullptr)
  {
  }

  // Label : Donne le Label
  TDF_Label Label();

  // NextSameShape
  TNaming_Node* NextSameShape(TNaming_RefShape* prs);

  // Test si l evolution est valide dans la transaction Trans
  // ie : Trans n est pas anterieure a sa creation
  //      et Trans n est pas posterieure a son BackUp
  bool IsValidInTrans(int Trans);

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  // Memory management
  DEFINE_STANDARD_ALLOC

  TNaming_PtrRefShape myOld;
  TNaming_PtrRefShape myNew;
  TNaming_NamedShape* myAtt;
  TNaming_PtrNode     nextSameAttribute;
  TNaming_PtrNode     nextSameOld;
  TNaming_PtrNode     nextSameNew;
};

//=================================================================================================

TNaming_Node* TNaming_Node::NextSameShape(TNaming_RefShape* prs)
{
  if (myOld == prs)
    return nextSameOld;
  if (myNew == prs)
    return nextSameNew;
  return nextSameNew;
}

//=================================================================================================

TDF_Label TNaming_Node::Label()
{
  return myAtt->Label();
}

//=================================================================================================

bool TNaming_Node::IsValidInTrans(int Trans)
{
  return myAtt->Transaction() <= Trans && Trans <= myAtt->UntilTransaction();
}

//=================================================================================================

void TNaming_Node::DumpJson(Standard_OStream& theOStream, int theDepth) const {
  OCCT_DUMP_CLASS_BEGIN(theOStream, TNaming_Node)

    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myOld)
      OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myNew)
        OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myAtt)

          OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, nextSameAttribute)
            OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, nextSameOld)
              OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, nextSameNew)}

//**********************************************************************
// Methods of TNaming_NamedShape
//**********************************************************************

//=================================================================================================

TNaming_NamedShape::TNaming_NamedShape()
{
  myNode      = nullptr;
  myVersion   = 0;
  myEvolution = TNaming_PRIMITIVE;
}

//=================================================================================================

bool TNaming_NamedShape::IsEmpty() const
{
  TNaming_Iterator it(this);
  return !it.More();
}

//=================================================================================================

TopoDS_Shape TNaming_NamedShape::Get() const
{
  return TNaming_Tool::GetShape(this);
}

//=================================================================================================

static void RemoveNode(
  bool                                                                             MapExist,
  NCollection_DataMap<TopoDS_Shape, TNaming_PtrRefShape, TopTools_ShapeMapHasher>& M,
  TNaming_Node*&                                                                   N)
{
  TNaming_RefShape* pos = N->myOld;
  if (pos != nullptr)
  {
    if (pos->FirstUse() == N)
    {
      TNaming_Node* nextOld = N->nextSameOld;
      if (nextOld != nullptr)
        pos->FirstUse(nextOld);
      else
      {
        // le shape disparait
        if (MapExist)
          M.UnBind(pos->Shape());
        N->myOld = nullptr;
        if (pos != N->myNew)
        {
          delete pos;
          pos = nullptr;
        }
      }
    }
    else
    {
      TNaming_Node* pdn = pos->FirstUse();
      while (pdn != nullptr)
      {

        if (pdn->NextSameShape(pos) == N)
        {
          if (pdn->myOld == pos)
            pdn->nextSameOld = N->nextSameOld;
          else
            pdn->nextSameNew = N->nextSameOld;
          break;
        }
        pdn = pdn->NextSameShape(pos);
      }
    }
  }

  TNaming_RefShape* pns = N->myNew;
  if (pns != nullptr)
  {
    if (pns->FirstUse() == N)
    {
      TNaming_Node* nextNew = N->nextSameNew;
      if (nextNew != nullptr)
        pns->FirstUse(nextNew);
      else
      {
        // le shape disparait
        if (MapExist)
          M.UnBind(pns->Shape());

        pns->FirstUse(nullptr);
        delete pns;
        pns = nullptr;

        N->myNew = nullptr;
      }
    }
    else
    {
      TNaming_Node* pdn = pns->FirstUse();
      while (pdn != nullptr)
      {
        if (pdn->NextSameShape(pns) == N)
        {
          if (pdn->myOld == pns)
            pdn->nextSameOld = N->nextSameNew;
          else
            pdn->nextSameNew = N->nextSameNew;
          break;
        }
        pdn = pdn->NextSameShape(pns);
      }
    }
  }
}

//=================================================================================================

void TNaming_NamedShape::Clear()
{
  if (Label().IsNull())
  {
#ifdef OCCT_DEBUG_BUILDER
    std::cout << "attention etat fantomatique" << std::endl;
#endif
    return;
  }

  occ::handle<TNaming_UsedShapes> US;

  NCollection_DataMap<TopoDS_Shape, TNaming_PtrRefShape, TopTools_ShapeMapHasher>* M = nullptr;

  // Recuperation de la map si celle-ci n est pas deja detruite.
  // bool MapExist = Ins.FindInRoot(TNaming_UsedShapes::GetID(),US);

  bool MapExist = Label().Root().FindAttribute(TNaming_UsedShapes::GetID(), US);
  if (MapExist)
    M = &(US->Map());

  TNaming_Node* p = myNode;
  while (p != nullptr)
  {
    RemoveNode(MapExist, *M, p);
    p = p->nextSameAttribute;
  }

  p = myNode;
  TNaming_Node* q;
  while (p != nullptr)
  {
    q = p;
    p = p->nextSameAttribute;
    if (q != nullptr)
    {
      delete q;
      q = nullptr;
    }
  }

  myNode = nullptr;
}

//=================================================================================================

void TNaming_NamedShape::BeforeRemoval()
{
  Clear();
}

//=======================================================================
// function : BeforeUndo
// purpose  : before application of a TDF_Delta.
//=======================================================================

bool TNaming_NamedShape::BeforeUndo(const occ::handle<TDF_AttributeDelta>& /*anAttDelta*/,
                                    const bool /*forceIt*/)
{
  //  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition))) {
  //    anAttDelta->Attribute()->BeforeRemoval();
  //  }
  return true;
}

//=======================================================================
// function : AfterUndo
// purpose  : After application of a TDF_Delta.
//=======================================================================

bool TNaming_NamedShape::AfterUndo(const occ::handle<TDF_AttributeDelta>& anAttDelta,
                                   const bool /*forceIt*/)
{
  if (anAttDelta->IsKind(STANDARD_TYPE(TDF_DeltaOnAddition)))
  {
    occ::handle<TNaming_UsedShapes> US;

    NCollection_DataMap<TopoDS_Shape, TNaming_PtrRefShape, TopTools_ShapeMapHasher>* M = nullptr;

    // Recuperation de la map si celle-ci n est pas deja detruite.
    // bool MapExist = Ins.FindInRoot(TNaming_UsedShapes::GetID(),US);

    bool MapExist = anAttDelta->Label().Root().FindAttribute(TNaming_UsedShapes::GetID(), US);

    if (MapExist)
      M = &(US->Map());

    TNaming_Node* p = myNode;
    while (p != nullptr)
    {
      RemoveNode(MapExist, *M, p);
      p = p->nextSameAttribute;
    }

    p = myNode;
    TNaming_Node* q;
    while (p != nullptr)
    {
      q = p;
      p = p->nextSameAttribute;
      if (q != nullptr)
      {
        delete q;
        q = nullptr;
      }
    }

    myNode = nullptr;
  }
  return true;
}

//=================================================================================================

occ::handle<TDF_Attribute> TNaming_NamedShape::BackupCopy() const
{
  // Note: in the copy, it is important to copy the attribute node
  // to not break the nextSameShape path.

  occ::handle<TNaming_NamedShape> Cop = new TNaming_NamedShape();
  Cop->myNode                         = myNode;
  Cop->myEvolution                    = myEvolution;
  Cop->myVersion                      = myVersion;

  // Update myAtt on the nodes in the attribute.
  TNaming_Node* CN = Cop->myNode;

  occ::handle<TNaming_NamedShape> A = this;
  A->myNode                         = nullptr;

  while (CN != nullptr)
  {
    CN->myAtt = Cop.operator->();
    CN        = CN->nextSameAttribute;
  }
  return Cop;
}

//=================================================================================================

void TNaming_NamedShape::Restore(const occ::handle<TDF_Attribute>& anAttribute)
{
  Clear();

  TNaming_NamedShape* PAtt = (TNaming_NamedShape*)anAttribute.operator->();
  myNode                   = PAtt->myNode;
  myEvolution              = PAtt->myEvolution;
  myVersion                = PAtt->myVersion;

  // Mise a jour de myAtt sur les noeuds dans l attribut.
  TNaming_Node* CN = myNode;
  while (CN != nullptr)
  {
    CN->myAtt = this;
    CN        = CN->nextSameAttribute;
  }
  PAtt->myNode = nullptr; // un noeud est dans un seul attribut.
}

//=================================================================================================

occ::handle<TDF_DeltaOnModification> TNaming_NamedShape::DeltaOnModification(
  const occ::handle<TDF_Attribute>& anOldAttribute) const
{

  return new TNaming_DeltaOnModification(occ::down_cast<TNaming_NamedShape>(anOldAttribute));
}

//=================================================================================================

void TNaming_NamedShape::DeltaOnModification(const occ::handle<TDF_DeltaOnModification>& aDelta)
{
  aDelta->Apply();
}

//=================================================================================================

occ::handle<TDF_DeltaOnRemoval> TNaming_NamedShape::DeltaOnRemoval() const
{
  return new TNaming_DeltaOnRemoval(this);
}

//=================================================================================================

occ::handle<TDF_Attribute> TNaming_NamedShape::NewEmpty() const
{
  return new TNaming_NamedShape();
}

//=================================================================================================

void TNaming_NamedShape::Paste(const occ::handle<TDF_Attribute>&       into,
                               const occ::handle<TDF_RelocationTable>& Tab) const
{
  TDF_Label Lab = into->Label();
  if (Lab.IsNull())
  {
    throw Standard_NullObject("TNaming_NamedShape::Paste");
  }
  TNaming_Builder B(Lab);

  TNaming_Iterator It(this);
  for (; It.More(); It.Next())
  {
    const TopoDS_Shape& OS      = It.OldShape();
    const TopoDS_Shape& NS      = It.NewShape();
    TNaming_Evolution   aStatus = It.Evolution();

    // Modification_1 24.06.99 (szy)
    TopoDS_Shape copOS, copNS;
    if (aStatus != TNaming_PRIMITIVE)
      TNaming_CopyShape::CopyTool(OS, Tab->TransientTable(), copOS);
    else
      copOS.Nullify();
    if (aStatus != TNaming_DELETE)
      TNaming_CopyShape::CopyTool(NS, Tab->TransientTable(), copNS);
    else
      copNS.Nullify();

    switch (aStatus)
    {
      case TNaming_PRIMITIVE: {
        B.Generated(copNS);
        break;
      }
      case TNaming_GENERATED: {
        B.Generated(copOS, copNS);
        break;
      }
      case TNaming_MODIFY: {
        B.Modify(copOS, copNS);
        break;
      }
      case TNaming_DELETE: {
        B.Delete(copOS);
        break;
      }
      case TNaming_SELECTED: {
        B.Select(copNS, copOS);
        break;
      }

      default:
        break;
    }
  }
}

//=================================================================================================

void TNaming_NamedShape::References(const occ::handle<TDF_DataSet>& aDataSet) const
{
  // Recherche des dependances.
  // Pour chaque OldShape de l attribut on ajoute au dataSet son label d origine.
  TNaming_Node* Current = myNode;
  while (Current != nullptr)
  {
    if (Current->myOld != nullptr)
    {
      TNaming_RefShape* prs = Current->myOld;
      TNaming_Node*     pdn = prs->FirstUse();

      while (pdn != nullptr)
      {
        if (pdn->myNew == prs && pdn->myAtt->Evolution() != TNaming_SELECTED)
        {
          aDataSet->AddLabel(pdn->Label());
        }
        pdn = pdn->NextSameShape(prs);
      }
    }
    Current = Current->nextSameAttribute;
  }
}

//=================================================================================================

void TNaming_NamedShape::Add(TNaming_Node*& pdn)
{
  pdn->myAtt = this;
  if (myNode != nullptr)
  {
    pdn->nextSameAttribute = myNode;
  }
  myNode = pdn;
}

//=================================================================================================

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

///=================================================================================================

TNaming_Builder::TNaming_Builder(const TDF_Label& L)
{
  // Find or Build Map;
  const TDF_Label& root = L.Root();
  if (!root.FindAttribute(TNaming_UsedShapes::GetID(), myShapes))
  {
    myShapes = new TNaming_UsedShapes();
    root.AddAttribute(myShapes);
  }

  // Find Or Build Attribute in LIns.
  if (!L.FindAttribute(TNaming_NamedShape::GetID(), myAtt))
  {
    myAtt = new TNaming_NamedShape();
    L.AddAttribute(myAtt);
  }
  else
  {
    myAtt->Backup();
    myAtt->Clear();
    myAtt->myVersion++;
  }
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_Builder::NamedShape() const
{
  return myAtt;
}

//=================================================================================================

static void UpdateFirstUseOrNextSameShape(TNaming_RefShape*& prs, TNaming_Node*& pdn)
{
  TNaming_Node* ldn = prs->FirstUse();
  if (ldn == nullptr)
  {
    prs->FirstUse(pdn);
  }
  else
  {
    TNaming_Node* cdn = ldn;
    while (cdn != nullptr)
    {
      ldn = cdn;
      cdn = cdn->NextSameShape(prs);
      if (ldn == cdn)
      {
        throw Standard_ConstructionError("UpdateFirstUseOrNextSameShape");
        break;
      }
    }
    // forbidden and useless loop.
    if (ldn != pdn)
    {
      if (ldn->myOld == prs)
        ldn->nextSameOld = pdn;
      if (ldn->myNew == prs)
        ldn->nextSameNew = pdn;
    }
  }
}

//=================================================================================================

void TNaming_Builder::Generated(const TopoDS_Shape& newShape)
{
  if (myAtt->myNode == nullptr)
    myAtt->myEvolution = TNaming_PRIMITIVE;
  else
  {
    if (myAtt->myEvolution != TNaming_PRIMITIVE)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape* pos = nullptr;
  TNaming_RefShape* pns;

  TNaming_RefShape** ppns = myShapes->myMap.ChangeSeek(newShape);
  if (ppns)
  {
#ifdef OCCT_DEBUG_BUILDER
    std::cout << "TNaming_Builder::Generate : the shape is already in the attribute" << std::endl;
#endif
    pns = *ppns;
    if (pns->FirstUse()->myAtt == myAtt.operator->())
    {
      throw Standard_ConstructionError("TNaming_Builder::Generate");
    }
    TNaming_Node* pdn = new TNaming_Node(pos, pns);
    myAtt->Add(pdn);
    UpdateFirstUseOrNextSameShape(pns, pdn);
  }
  else
  {
    pns               = new TNaming_RefShape(newShape);
    TNaming_Node* pdn = new TNaming_Node(pos, pns);
    pns->FirstUse(pdn);
    myShapes->myMap.Bind(newShape, pns);
    myAtt->Add(pdn);
  }
}

//=================================================================================================

void TNaming_Builder::Delete(const TopoDS_Shape& oldShape)
{
  if (myAtt->myNode == nullptr)
    myAtt->myEvolution = TNaming_DELETE;
  else
  {
    if (myAtt->myEvolution != TNaming_DELETE)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape* pns;
  TNaming_RefShape* pos;

  TNaming_RefShape** ppos = myShapes->myMap.ChangeSeek(oldShape);
  if (ppos)
    pos = *ppos;
  else
  {
#ifdef OCCT_DEBUG_BUILDER
    std::cout << "TNaming_Builder::Delete : the shape is not in the data" << std::endl;
#endif
    pos = new TNaming_RefShape(oldShape);
    myShapes->myMap.Bind(oldShape, pos);
  }

  TopoDS_Shape nullShape;
  pns = new TNaming_RefShape(nullShape);
  myShapes->myMap.Bind(nullShape, pns);

  TNaming_Node* pdn = new TNaming_Node(pos, pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape(pos, pdn);
  UpdateFirstUseOrNextSameShape(pns, pdn);
}

//=================================================================================================

void TNaming_Builder::Generated(const TopoDS_Shape& oldShape, const TopoDS_Shape& newShape)
{
  if (myAtt->myNode == nullptr)
    myAtt->myEvolution = TNaming_GENERATED;
  else
  {
    if (myAtt->myEvolution != TNaming_GENERATED)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  if (oldShape.IsSame(newShape))
  {
#ifdef OCCT_DEBUG_BUILDER
    std::cout << "TNaming_Builder::Generate : oldShape IsSame newShape" << std::endl;
#endif
    return;
  }
  TNaming_RefShape*  pos;
  TNaming_RefShape** ppos = myShapes->myMap.ChangeSeek(oldShape);
  if (!ppos)
  {
    pos = new TNaming_RefShape(oldShape);
    myShapes->myMap.Bind(oldShape, pos);
  }
  else
    pos = *ppos;

  TNaming_RefShape*  pns;
  TNaming_RefShape** ppns = myShapes->myMap.ChangeSeek(newShape);
  if (!ppns)
  {
    pns = new TNaming_RefShape(newShape);
    myShapes->myMap.Bind(newShape, pns);
  }
  else
    pns = *ppns;

  TNaming_Node* pdn = new TNaming_Node(pos, pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape(pos, pdn);
  UpdateFirstUseOrNextSameShape(pns, pdn);
}

//=================================================================================================

void TNaming_Builder::Modify(const TopoDS_Shape& oldShape, const TopoDS_Shape& newShape)
{
  if (myAtt->myNode == nullptr)
    myAtt->myEvolution = TNaming_MODIFY;
  else
  {
    if (myAtt->myEvolution != TNaming_MODIFY)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  if (oldShape.IsSame(newShape))
  {
#ifdef OCCT_DEBUG_BUILDER
    std::cout << "TNaming_Builder::Modify : oldShape IsSame newShape" << std::endl;
#endif
    return;
  }
  TNaming_RefShape*  pos;
  TNaming_RefShape** ppos = myShapes->myMap.ChangeSeek(oldShape);
  if (!ppos)
  {
    pos = new TNaming_RefShape(oldShape);
    myShapes->myMap.Bind(oldShape, pos);
  }
  else
    pos = *ppos;

  TNaming_RefShape*  pns;
  TNaming_RefShape** ppns = myShapes->myMap.ChangeSeek(newShape);
  if (!ppns)
  {
    pns = new TNaming_RefShape(newShape);
    myShapes->myMap.Bind(newShape, pns);
  }
  else
    pns = *ppns;

  TNaming_Node* pdn = new TNaming_Node(pos, pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape(pos, pdn);
  UpdateFirstUseOrNextSameShape(pns, pdn);
}

//=================================================================================================

void TNaming_Builder::Select(const TopoDS_Shape& S, const TopoDS_Shape& InS)
{
  if (myAtt->myNode == nullptr)
    myAtt->myEvolution = TNaming_SELECTED;
  else
  {
    if (myAtt->myEvolution != TNaming_SELECTED)
      throw Standard_ConstructionError("TNaming_Builder : not same evolution");
  }

  TNaming_RefShape*  pos;
  TNaming_RefShape** ppos = myShapes->myMap.ChangeSeek(InS);
  if (!ppos)
  {
    pos = new TNaming_RefShape(InS);
    myShapes->myMap.Bind(InS, pos);
  }
  else
    pos = *ppos;

  TNaming_RefShape*  pns;
  TNaming_RefShape** ppns = myShapes->myMap.ChangeSeek(S);
  if (!ppns)
  {
    pns = new TNaming_RefShape(S);
    myShapes->myMap.Bind(S, pns);
  }
  else
    pns = *ppns;

  TNaming_Node* pdn = new TNaming_Node(pos, pns);
  myAtt->Add(pdn);
  UpdateFirstUseOrNextSameShape(pos, pdn);
  UpdateFirstUseOrNextSameShape(pns, pdn);
}

//**********************************************************************
// Methods of the TNaming_Iterator class
//**********************************************************************

//=================================================================================================

TNaming_Iterator::TNaming_Iterator(const occ::handle<TNaming_NamedShape>& Att)
    : myTrans(-1)
{
  myNode = Att->myNode;
}

//=================================================================================================

TNaming_Iterator::TNaming_Iterator(const TDF_Label& Lab)
    : myTrans(-1)
{
  occ::handle<TNaming_NamedShape> Att;
  if (Lab.FindAttribute(TNaming_NamedShape::GetID(), Att))
  {
    myNode = Att->myNode;
  }
  else
  {
    myNode = nullptr;
  }
}

//=================================================================================================

TNaming_Iterator::TNaming_Iterator(const TDF_Label& Lab, const int Trans)
    : myTrans(Trans)
{
  occ::handle<TDF_Attribute> Att;
  if (Lab.FindAttribute(TNaming_NamedShape::GetID(), Trans, Att))
  {
    myNode = occ::down_cast<TNaming_NamedShape>(Att)->myNode;
  }
  else
  {
    myNode = nullptr;
#ifdef OCCT_DEBUG
    std::cout << "TNaming_Iterator : No Shape for this label" << std::endl;
#endif
  }
}

//=================================================================================================

void TNaming_Iterator::Next()
{
  Standard_NoMoreObject_Raise_if(myNode == nullptr, "TNaming_Iterator::Next");
  myNode = myNode->nextSameAttribute;
}

//=================================================================================================

const TopoDS_Shape& TNaming_Iterator::OldShape() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_Iterator::OldShape");
  if (myNode->myOld == nullptr)
  {
    static TopoDS_Shape NullShape;
    return NullShape;
  }
  return myNode->myOld->Shape();
}

//=================================================================================================

const TopoDS_Shape& TNaming_Iterator::NewShape() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_Iterator::NewShape");
  if (myNode->myNew == nullptr)
  {
    static TopoDS_Shape NullShape;
    return NullShape;
  }
  return myNode->myNew->Shape();
}

//=================================================================================================

bool TNaming_Iterator::IsModification() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_Iterator::IsModification");
  return (myNode->myAtt->myEvolution == TNaming_MODIFY
          || myNode->myAtt->myEvolution == TNaming_DELETE);
}

//=================================================================================================

TNaming_Evolution TNaming_Iterator::Evolution() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_Iterator::IsModification");
  return myNode->myAtt->myEvolution;
}

//**********************************************************************
// Methods of the TNaming_NewShapeIterator class
//**********************************************************************

//=======================================================================
// function : SelectSameShape
// purpose  : Selectionne le prochain noeud ou le shape est le meme que celui
//           de RS. Old = 0 si il doit etre new dans le noeud a chercher.
//           selection dans la transaction valide.
//           On saute aussi les noeud ou OS = NS;
//=======================================================================

static void SelectSameShape(TNaming_Node*&     myNode,
                            bool               Old,
                            TNaming_RefShape*& RS,
                            const int&         Trans)
{
  TNaming_Node* pdn = myNode;

  while (pdn != nullptr)
  {
    bool Valid;
    if (Trans < 0)
      Valid = pdn->myAtt->IsValid();
    else
      Valid = pdn->IsValidInTrans(Trans);

    if (Valid)
    {
      if (Old)
      {
        if (pdn->myOld == RS && pdn->myNew != nullptr && pdn->myNew != RS)
        {
          break;
        }
      }
      else
      {
        if (pdn->myNew == RS && pdn->myOld != nullptr && pdn->myOld != RS)
        {
          break;
        }
      }
    }
    pdn = pdn->NextSameShape(RS);
  }
  myNode = pdn;
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TopoDS_Shape&                    aShape,
                                                   const int                              Trans,
                                                   const occ::handle<TNaming_UsedShapes>& Shapes)
    : myTrans(Trans)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
  myNode               = RS->FirstUse();
  bool Old(true);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TopoDS_Shape& aShape,
                                                   const int           Trans,
                                                   const TDF_Label&    access)
    : myTrans(Trans)
{
  occ::handle<TNaming_UsedShapes> Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), Shapes))
  {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                   "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
    TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
    myNode               = RS->FirstUse();
    bool Old(true);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TNaming_Iterator& anIterator)
    : myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == nullptr,
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator");
  myNode               = anIterator.myNode;
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == nullptr)
    myNode = nullptr; // No descendant
  else
  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
    bool Old(true);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TopoDS_Shape&                    aShape,
                                                   const occ::handle<TNaming_UsedShapes>& Shapes)
    : myTrans(-1)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
  myNode               = RS->FirstUse();
  bool Old(true);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TopoDS_Shape& aShape,
                                                   const TDF_Label&    access)
    : myTrans(-1)
{
  occ::handle<TNaming_UsedShapes> Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), Shapes))
  {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                   "TNaming_NewShapeIterator::TNaming_NewShapeIterator aShape");
    bool              Old(true);
    TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
    myNode               = RS->FirstUse();
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_NewShapeIterator::TNaming_NewShapeIterator(const TNaming_NewShapeIterator& anIterator)
    : myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == nullptr,
                                 "TNaming_NewShapeIterator::TNaming_NewShapeIterator");
  myNode               = anIterator.myNode;
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == nullptr)
    myNode = nullptr; // No descendant
  else
  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
    bool Old(true);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

void TNaming_NewShapeIterator::Next()
{
  TNaming_RefShape* RS = myNode->myOld;
  myNode               = myNode->NextSameShape(RS);
  bool Old(true);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TDF_Label TNaming_NewShapeIterator::Label() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_NewShapeIterator::Label");
  return myNode->Label();
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_NewShapeIterator::NamedShape() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_NewShapeIterator::Label");
  return myNode->myAtt;
}

//=================================================================================================

const TopoDS_Shape& TNaming_NewShapeIterator::Shape() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_NewShapeIterator::Shape");
  return myNode->myNew->Shape();
}

//=================================================================================================

bool TNaming_NewShapeIterator::IsModification() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_NewShapeIterator::IsModification");

  return (myNode->myAtt->myEvolution == TNaming_MODIFY
          || myNode->myAtt->myEvolution == TNaming_DELETE);
}

//**********************************************************************
// Methods of the TNaming_OldShapeIterator class
//**********************************************************************
//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TopoDS_Shape&                    aShape,
                                                   const int                              Trans,
                                                   const occ::handle<TNaming_UsedShapes>& Shapes)
    : myTrans(Trans)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
  myNode               = RS->FirstUse();
  bool Old(false);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TopoDS_Shape& aShape,
                                                   const int           Trans,
                                                   const TDF_Label&    access)
    : myTrans(Trans)
{
  occ::handle<TNaming_UsedShapes> Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), Shapes))
  {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                   "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");
    TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
    myNode               = RS->FirstUse();
    bool Old(false);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TopoDS_Shape&                    aShape,
                                                   const occ::handle<TNaming_UsedShapes>& Shapes)
    : myTrans(-1)
{
  Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
  myNode               = RS->FirstUse();
  bool Old(false);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TopoDS_Shape& aShape,
                                                   const TDF_Label&    access)
    : myTrans(-1)
{
  occ::handle<TNaming_UsedShapes> Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), Shapes))
  {
    Standard_NoSuchObject_Raise_if(!Shapes->Map().IsBound(aShape),
                                   "TNaming_OldShapeIterator::TNaming_OldShapeIterator aShape");
    TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
    myNode               = RS->FirstUse();
    bool Old(false);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TNaming_Iterator& anIterator)
    : myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == nullptr,
                                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator");
  myNode               = anIterator.myNode;
  TNaming_RefShape* RS = myNode->myNew;
  if (RS == nullptr)
    myNode = nullptr; // No descendant
  else
  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
    bool Old(false);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

TNaming_OldShapeIterator::TNaming_OldShapeIterator(const TNaming_OldShapeIterator& anIterator)
    : myTrans(anIterator.myTrans)
{
  Standard_NoSuchObject_Raise_if(anIterator.myNode == nullptr,
                                 "TNaming_OldShapeIterator::TNaming_OldShapeIterator");
  myNode               = anIterator.myNode;
  TNaming_RefShape* RS = myNode->myOld;
  if (RS == nullptr)
    myNode = nullptr; // No descendant
  else
  {
    // il faut repartir de la premiere utilisation.
    myNode = RS->FirstUse();
    bool Old(false);
    SelectSameShape(myNode, Old, RS, myTrans);
  }
}

//=================================================================================================

void TNaming_OldShapeIterator::Next()
{
  bool              Old = false;
  TNaming_RefShape* RS  = myNode->myNew;
  myNode                = myNode->NextSameShape(RS);
  SelectSameShape(myNode, Old, RS, myTrans);
}

//=================================================================================================

TDF_Label TNaming_OldShapeIterator::Label() const
{
  if (myNode == nullptr)
    throw Standard_NoSuchObject("TNaming_OldShapeIterator::Label");
  return myNode->Label();
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_OldShapeIterator::NamedShape() const
{
  if (myNode == nullptr)
    throw Standard_NoSuchObject("TNaming_OldShapeIterator::Label");
  return myNode->myAtt;
}

//=================================================================================================

const TopoDS_Shape& TNaming_OldShapeIterator::Shape() const
{
  if (myNode == nullptr)
    throw Standard_NoSuchObject("TNaming_OldShapeIterator::Shape");
  return myNode->myOld->Shape();
}

//=================================================================================================

bool TNaming_OldShapeIterator::IsModification() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_OldShapeIterator::IsModification");
  return (myNode->myAtt->myEvolution == TNaming_MODIFY
          || myNode->myAtt->myEvolution == TNaming_DELETE);
}

//**********************************************************************
// Methods of the SameShapeIterator
//**********************************************************************

//=================================================================================================

TNaming_SameShapeIterator::TNaming_SameShapeIterator(const TopoDS_Shape&                    aShape,
                                                     const occ::handle<TNaming_UsedShapes>& Shapes)
{
  TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
  myNode               = RS->FirstUse();
  myIsNew              = (myNode->myNew == RS);
}

//=================================================================================================

TNaming_SameShapeIterator::TNaming_SameShapeIterator(const TopoDS_Shape& aShape,
                                                     const TDF_Label&    access)
{
  occ::handle<TNaming_UsedShapes> Shapes;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), Shapes))
  {
    TNaming_RefShape* RS = Shapes->Map().ChangeFind(aShape);
    myNode               = RS->FirstUse();
    myIsNew              = (myNode->myNew == RS);
  }
}

//=================================================================================================

void TNaming_SameShapeIterator::Next()
{
  TNaming_RefShape* prs;
  if (myIsNew)
    prs = myNode->myNew;
  else
    prs = myNode->myOld;

  myNode = myNode->NextSameShape(prs);
  if (myNode != nullptr)
    myIsNew = (myNode->myNew == prs);
}

//=================================================================================================

TDF_Label TNaming_SameShapeIterator::Label() const
{
  Standard_NoSuchObject_Raise_if(myNode == nullptr, "TNaming_SameShapeIterator::Label");
  return myNode->Label();
}

//**********************************************************************
// Methods of the TNaming_RefShape
//**********************************************************************
//=================================================================================================

TDF_Label TNaming_RefShape::Label() const
{
  return myFirstUse->myAtt->Label();
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_RefShape::NamedShape() const
{
  return myFirstUse->myAtt;
}

//=================================================================================================

void TNaming_RefShape::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, TNaming_NamedShape);

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myShape);
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myFirstUse);
}

//**********************************************************************
// Methods of the TNaming_Tool class
//**********************************************************************

//=================================================================================================

bool TNaming_Tool::HasLabel(const TDF_Label& access, const TopoDS_Shape& S)
{
  occ::handle<TNaming_UsedShapes> US;
  if (access.Root().FindAttribute(TNaming_UsedShapes::GetID(), US))
  {
    return (US->Map().IsBound(S));
  }
#ifdef OCCT_DEBUG_HASL
  std::cout << "##==> Sub-Shape has no Label!" << std::endl;
#endif
  return false;
}

//=================================================================================================

TDF_Label TNaming_Tool::Label(const TDF_Label& access, const TopoDS_Shape& S, int& Trans)
{
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(access, S), "TNaming_Tool::Label");
  occ::handle<TNaming_UsedShapes> US;
  access.Root().FindAttribute(TNaming_UsedShapes::GetID(), US);
  return TNaming_Tool::Label(US, S, Trans);
}

//=======================================================================
// function : IsValidInTrans
// purpose  : un shape est valid tant que l attribut ou il est cree est valid
//=======================================================================

int TNaming_Tool::ValidUntil(const TDF_Label& access, const TopoDS_Shape& S)
{
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(access, S), "TNaming_Tool::ValidUntil");
  occ::handle<TNaming_UsedShapes> US;
  access.Root().FindAttribute(TNaming_UsedShapes::GetID(), US);
  return TNaming_Tool::ValidUntil(S, US);
}

//=================================================================================================

bool TNaming_Tool::HasLabel(const occ::handle<TNaming_UsedShapes>& Shapes, const TopoDS_Shape& S)
{
  return (Shapes->Map().IsBound(S));
}

//=================================================================================================

TDF_Label TNaming_Tool::Label(const occ::handle<TNaming_UsedShapes>& Shapes,
                              const TopoDS_Shape&                    S,
                              int&                                   Trans)
{
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(Shapes, S), "TNaming_Tool::Label");
  TNaming_RefShape* prs = Shapes->Map().Find(S);
  TNaming_Node*     pdn = prs->FirstUse();

  while (pdn != nullptr && (pdn->myNew != prs || pdn->myAtt->Evolution() == TNaming_SELECTED))
  {
    pdn = pdn->NextSameShape(prs);
  }
  if (pdn == nullptr)
    pdn = prs->FirstUse();

  TDF_Label L = pdn->Label();
  Trans       = pdn->myAtt->Transaction();
  return L;
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_Tool::NamedShape(const TopoDS_Shape& S,
                                                         const TDF_Label&    Acces)
{
  occ::handle<TNaming_NamedShape> NS;
  occ::handle<TNaming_UsedShapes> US;
  if (!Acces.Root().FindAttribute(TNaming_UsedShapes::GetID(), US))
  {
    return NS;
  }

  if (!TNaming_Tool::HasLabel(US, S))
  {
    return NS;
  }

  TNaming_RefShape* prs = US->Map().Find(S);
  TNaming_Node*     pdn = prs->FirstUse();
  TNaming_Node*     res = nullptr;

  while (pdn != nullptr)
  {
    if (pdn->myNew == prs && pdn->myAtt->Evolution() != TNaming_SELECTED)
    {
      res = pdn;
      if (pdn->myAtt->Evolution() != TNaming_GENERATED)
      {
        // Les modifications sont privilegiees par rapport au generation.
        // Dans le cas des shapes qui sont a la fois des modifications et des generations
        // faces tangentes.
        break;
      }
    }
    pdn = pdn->NextSameShape(prs);
  }

  if (res == nullptr)
    return NS;

  // VERUE EN ATTENDANT DE REVOIR ABORT 03/11/98
  // Protection pour eviter de renvoyer un attribut backuped
  TDF_Label Lab = res->Label();
  Lab.FindAttribute(TNaming_NamedShape::GetID(), NS);
  return NS;
  //  return res->myAtt;
}

//=======================================================================
// function : IsValidInTrans
// purpose  : un shape est valid tant que l attribut ou il est cree est valid
//=======================================================================

int TNaming_Tool::ValidUntil(const TopoDS_Shape& S, const occ::handle<TNaming_UsedShapes>& US)
{
  Standard_NoSuchObject_Raise_if(!TNaming_Tool::HasLabel(US, S), "TNaming_Tool::ValidUntil");

  TNaming_RefShape* RS = US->Map().ChangeFind(S);
  int               Cur;
  int               Until = 0;
  TNaming_Node*     Node  = RS->FirstUse();

  while (Node != nullptr)
  {
    if (Node->myNew != nullptr && Node->myNew == RS)
    {
      Cur = Node->myAtt->UntilTransaction();
      if (Cur > Until)
        Until = Cur;
    }
    Node = Node->NextSameShape(RS);
  }
  return Until;
}

//=================================================================================================

void TNaming_NamedShape::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  TNaming_Node* p = myNode;
  if (p != nullptr)
  {
    TCollection_AsciiString aLabel;
    TDF_Tool::Entry(myNode->Label(), aLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLabel)
  }
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myEvolution)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myVersion)
}
