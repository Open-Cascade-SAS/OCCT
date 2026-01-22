// Created on: 1997-01-09
// Created by: VAUTHIER Jean-Claude & Fricaud Yves
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

#include <DNaming.hxx>

#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <BRepLib_FindSurface.hxx>
#include <BRepTools.hxx>
#include <DDF.hxx>
#include <Draw.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pln.hxx>
#include <ModelDefinitions.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_TagSource.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_Function.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>

//=================================================================================================

// bool DNaming_DFandUS(char* a,
// 				 occ::handle<TDF_Data>&           ND,
// 				 occ::handle<TNaming_UsedShapes>& US)
// {
//   occ::handle<DDF_Data> DND = Handle(DDF_Data)::DownCast (Draw::Get(a));
//   if (DND.IsNull ()) return 0;
//   ND = DND->DataFramework ();
//   ND->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);
//   return 1;
// }
//=================================================================================================

void DNaming::GetShape(const char* const               LabelName,
                       const occ::handle<TDF_Data>&    DF,
                       NCollection_List<TopoDS_Shape>& L)
{
  L.Clear();
  TDF_Label Label;
  bool      Found = DDF::AddLabel(DF, LabelName, Label);
  if (Found)
  {
    TNaming_Iterator it(Label, DF->Transaction());
    for (; it.More(); it.Next())
    {
      L.Append(it.NewShape());
    }
  }
}

//=================================================================================================

void DNaming_BuildMap(NCollection_Map<TDF_Label>& Updated, const TDF_Label& Lab)
{
  TDF_ChildIterator it(Lab);
  for (; it.More(); it.Next())
  {
    Updated.Add(it.Value());
    DNaming_BuildMap(Updated, it.Value());
  }
}

//=================================================================================================

TopoDS_Shape DNaming::CurrentShape(const char* const LabelName, const occ::handle<TDF_Data>& DF)
{
  TopoDS_Shape S;
  TDF_Label    Label;
  bool         Found = DDF::AddLabel(DF, LabelName, Label);
  if (!Found)
  {
#ifdef OCCT_DEBUG
    std::cout << "no labels" << std::endl;
#endif
    return S;
  }
  if (Found)
  {
    occ::handle<TNaming_NamedShape> NS;
    Label.FindAttribute(TNaming_NamedShape::GetID(), NS);
    S = TNaming_Tool::CurrentShape(NS);
    if (S.IsNull())
#ifdef OCCT_DEBUG
      std::cout << "current shape from " << LabelName << " is deleted" << std::endl;
#endif
    return S;
  }
  return S;
}

//=================================================================================================

TCollection_AsciiString DNaming::GetEntry(const TopoDS_Shape&          Shape,
                                          const occ::handle<TDF_Data>& DF,
                                          int&                         theStatus)
{
  theStatus = 0;
  // occ::handle<TNaming_UsedShapes> US;
  // DF->Root().FindAttribute(TNaming_UsedShapes::GetID(),US);

  if (!TNaming_Tool::HasLabel(DF->Root(), Shape))
  {
    return TCollection_AsciiString();
  }
  int                     Transdef;
  TDF_Label               Lab = TNaming_Tool::Label(DF->Root(), Shape, Transdef);
  TCollection_AsciiString entry;
  TDF_Tool::Entry(Lab, entry);
  // Update theStatus;
  TNaming_Iterator it(Lab, DF->Transaction());
  for (; it.More(); it.Next())
  {
    theStatus++;
    if (theStatus == 2)
      break;
  }
  return entry;
}

//=================================================================================================

void DNaming::AllCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  DNaming::BasicCommands(theCommands);
  DNaming::ToolsCommands(theCommands);
  DNaming::SelectionCommands(theCommands);
  DNaming::ModelingCommands(theCommands);
  // define the TCL variable Draw_NamingData
  const char* com = "set Draw_NamingData 1";
  theCommands.Eval(com);
}

//=======================================================================
//=======================================================================
// function : LoadC0Vertices
// purpose  : Method for internal use. It is used by Load() method.
//=======================================================================

static void LoadC0Vertices(const TopoDS_Shape& S, const occ::handle<TDF_TagSource>& Tagger)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                 vertexNaborFaces;
  NCollection_List<TopoDS_Shape> empty;
  TopExp_Explorer                explF(S, TopAbs_FACE);
  for (; explF.More(); explF.Next())
  {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer     explV(aFace, TopAbs_VERTEX);
    for (; explV.More(); explV.Next())
    {
      const TopoDS_Shape&             aVertex   = explV.Current();
      NCollection_List<TopoDS_Shape>* pList     = vertexNaborFaces.TryBound(aVertex, empty);
      bool                            faceIsNew = true;
      for (NCollection_List<TopoDS_Shape>::Iterator itrF(*pList); itrF.More(); itrF.Next())
      {
        if (itrF.Value().IsSame(aFace))
        {
          faceIsNew = false;
          break;
        }
      }
      if (faceIsNew)
      {
        pList->Append(aFace);
      }
    }
  }

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itr(vertexNaborFaces);
  for (; itr.More(); itr.Next())
  {
    const NCollection_List<TopoDS_Shape>& naborFaces = itr.Value();
    if (naborFaces.Extent() < 3)
    {
      TNaming_Builder bC0Vertex(Tagger->NewChild());
      bC0Vertex.Generated(itr.Key());
    }
  }
}

//=======================================================================
// function : LoadC0Edges
// purpose  : Method for internal use. It is used by Load() method.
//=======================================================================

static void LoadC0Edges(const TopoDS_Shape& S, const occ::handle<TDF_TagSource>& Tagger)
{
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                 edgeNaborFaces;
  NCollection_List<TopoDS_Shape> empty;
  TopExp_Explorer                explF(S, TopAbs_FACE);
  for (; explF.More(); explF.Next())
  {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer     explV(aFace, TopAbs_EDGE);
    for (; explV.More(); explV.Next())
    {
      const TopoDS_Shape&             anEdge    = explV.Current();
      NCollection_List<TopoDS_Shape>* pList     = edgeNaborFaces.TryBound(anEdge, empty);
      bool                            faceIsNew = true;
      for (NCollection_List<TopoDS_Shape>::Iterator itrF(*pList); itrF.More(); itrF.Next())
      {
        if (itrF.Value().IsSame(aFace))
        {
          faceIsNew = false;
          break;
        }
      }
      if (faceIsNew)
      {
        pList->Append(aFace);
      }
    }
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> anEdgesToDelete;
  // clang-format off
  TopExp_Explorer anEx(S,TopAbs_EDGE); // mpv: new explorer iterator because we need keep edges order
  // clang-format on
  for (; anEx.More(); anEx.Next())
  {
    bool                                  aC0     = false;
    const TopoDS_Shape&                   anEdge1 = anEx.Current();
    const NCollection_List<TopoDS_Shape>* pList1  = edgeNaborFaces.Seek(anEdge1);
    if (pList1)
    {
      const NCollection_List<TopoDS_Shape>& aList1 = *pList1;
      if (aList1.Extent() < 2)
        continue; // mpv (06.09.2002): these edges already was loaded
      NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
        Iterator itr(edgeNaborFaces);
      for (; itr.More(); itr.Next())
      {
        const TopoDS_Shape& anEdge2 = itr.Key();
        if (anEdgesToDelete.Contains(anEdge2))
          continue;
        if (anEdge1.IsSame(anEdge2))
          continue;
        const NCollection_List<TopoDS_Shape>& aList2 = itr.Value();
        // compare lists of the neighbour faces of edge1 and edge2
        if (aList1.Extent() == aList2.Extent())
        {
          int aMatches = 0;
          for (NCollection_List<TopoDS_Shape>::Iterator aLIter1(aList1); aLIter1.More();
               aLIter1.Next())
            for (NCollection_List<TopoDS_Shape>::Iterator aLIter2(aList2); aLIter2.More();
                 aLIter2.Next())
              if (aLIter1.Value().IsSame(aLIter2.Value()))
                aMatches++;
          if (aMatches == aList1.Extent())
          {
            aC0 = true;
            TNaming_Builder bC0Edge(Tagger->NewChild());
            bC0Edge.Generated(anEdge2);
            // edgeNaborFaces.UnBind(anEdge2);
            anEdgesToDelete.Add(anEdge2);
          }
        }
      }
      // VUN (10/2/2005) avoid UnBind during iterating -^
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itDelete(anEdgesToDelete);
      for (; itDelete.More(); itDelete.Next())
      {
        edgeNaborFaces.UnBind(itDelete.Key());
      }
      edgeNaborFaces.UnBind(anEdge1);
    }
    if (aC0)
    {
      TNaming_Builder bC0Edge(Tagger->NewChild());
      bC0Edge.Generated(anEdge1);
    }
  }
}

//
//=======================================================================
// function : GetDangleShapes
// purpose  : Returns dangle sub shapes Generator - Dangle.
//=======================================================================

static bool GetDangleShapes(
  const TopoDS_Shape&                                                       ShapeIn,
  const TopAbs_ShapeEnum                                                    GeneratedFrom,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& Dangles)
{
  Dangles.Clear();
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                   subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (GeneratedFrom == TopAbs_FACE)
    GeneratedTo = TopAbs_EDGE;
  else if (GeneratedFrom == TopAbs_EDGE)
    GeneratedTo = TopAbs_VERTEX;
  else
    return false;
  TopExp::MapShapesAndAncestors(ShapeIn, GeneratedTo, GeneratedFrom, subShapeAndAncestors);
  for (int i = 1; i <= subShapeAndAncestors.Extent(); i++)
  {
    const TopoDS_Shape&                   mayBeDangle = subShapeAndAncestors.FindKey(i);
    const NCollection_List<TopoDS_Shape>& ancestors   = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1)
      Dangles.Bind(ancestors.First(), mayBeDangle);
  }
  return !Dangles.IsEmpty();
}

//=================================================================================================

static void LoadGeneratedDangleShapes(const TopoDS_Shape&    ShapeIn,
                                      const TopAbs_ShapeEnum GeneratedFrom,
                                      TNaming_Builder&       Builder)
{
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> dangles;
  if (!GetDangleShapes(ShapeIn, GeneratedFrom, dangles))
    return;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itr(dangles);
  for (; itr.More(); itr.Next())
    Builder.Generated(itr.Key(), itr.Value());
}

//=======================================================================
// function : LoadNextLevels
// purpose  : Method for internal use. Is used by LoadFirstLevel()
//=======================================================================

static void LoadNextLevels(const TopoDS_Shape& S, const occ::handle<TDF_TagSource>& Tagger)
{

  if (S.ShapeType() == TopAbs_SOLID)
  {
    TopExp_Explorer aExp(S, TopAbs_FACE);
    for (; aExp.More(); aExp.Next())
    {
      TNaming_Builder bFace(Tagger->NewChild());
      bFace.Generated(aExp.Current());
    }
  }
  else if (S.ShapeType() == TopAbs_SHELL || S.ShapeType() == TopAbs_FACE)
  {
    // load faces and all the free edges
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Faces;
    TopExp::MapShapes(S, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (S.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1))
    {
      TopExp_Explorer aExp(S, TopAbs_FACE);
      for (; aExp.More(); aExp.Next())
      {
        TNaming_Builder bFace(Tagger->NewChild());
        bFace.Generated(aExp.Current());
      }
    }
    NCollection_IndexedDataMap<TopoDS_Shape,
                               NCollection_List<TopoDS_Shape>,
                               TopTools_ShapeMapHasher>
      anEdgeAndNeighbourFaces;
    TopExp::MapShapesAndAncestors(S, TopAbs_EDGE, TopAbs_FACE, anEdgeAndNeighbourFaces);
    for (int i = 1; i <= anEdgeAndNeighbourFaces.Extent(); i++)
    {
      const NCollection_List<TopoDS_Shape>& aLL = anEdgeAndNeighbourFaces.FindFromIndex(i);
      if (aLL.Extent() < 2)
      {
        TNaming_Builder bFreeEdges(Tagger->NewChild());
        bFreeEdges.Generated(anEdgeAndNeighbourFaces.FindKey(i));
      }
      else
      {
        NCollection_List<TopoDS_Shape>::Iterator anIter(aLL);
        const TopoDS_Face&                       aFace = TopoDS::Face(anIter.Value());
        anIter.Next();
        if (aFace.IsEqual(anIter.Value()))
        {
          TNaming_Builder bFreeEdges(Tagger->NewChild());
          bFreeEdges.Generated(anEdgeAndNeighbourFaces.FindKey(i));
        }
      }
    }
  }
  else if (S.ShapeType() == TopAbs_WIRE)
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> Edges;
    BRepTools::Map3DEdges(S, Edges);
    if (Edges.Extent() == 1)
    {
      TNaming_Builder bEdge(Tagger->NewChild());
      bEdge.Generated(Edges.FindKey(1));
      TopExp_Explorer aExp(S, TopAbs_VERTEX);
      for (; aExp.More(); aExp.Next())
      {
        TNaming_Builder bVertex(Tagger->NewChild());
        bVertex.Generated(aExp.Current());
      }
    }
    else
    {
      TopExp_Explorer aExp(S, TopAbs_EDGE);
      for (; aExp.More(); aExp.Next())
      {
        TNaming_Builder bEdge(Tagger->NewChild());
        bEdge.Generated(aExp.Current());
      }
      // and load generated vertices.
      NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> generated;
      if (GetDangleShapes(S, TopAbs_EDGE, generated))
      {
        TNaming_Builder bGenVertices(Tagger->NewChild());
        LoadGeneratedDangleShapes(S, TopAbs_EDGE, bGenVertices);
      }
    }
  }
  else if (S.ShapeType() == TopAbs_EDGE)
  {
    TopExp_Explorer aExp(S, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next())
    {
      TNaming_Builder bVertex(Tagger->NewChild());
      bVertex.Generated(aExp.Current());
    }
  }
}

//=======================================================================
// function : LoadFirstLevel
// purpose  : Method for internal use. Is used by Load()
//=======================================================================

static void LoadFirstLevel(const TopoDS_Shape& S, const occ::handle<TDF_TagSource>& Tagger)
{
  if (S.ShapeType() == TopAbs_COMPOUND || S.ShapeType() == TopAbs_COMPSOLID)
  {
    TopoDS_Iterator itr(S);
    for (; itr.More(); itr.Next())
    {
      TNaming_Builder bIndependentShapes(Tagger->NewChild());
      bIndependentShapes.Generated(itr.Value());
      if (itr.Value().ShapeType() == TopAbs_COMPOUND || itr.Value().ShapeType() == TopAbs_COMPSOLID)
      {
        LoadFirstLevel(itr.Value(), Tagger);
      }
      else
        LoadNextLevels(itr.Value(), Tagger);
    }
  }
  else
    LoadNextLevels(S, Tagger);
}

//=======================================================================
// function : Load
// purpose  : To load an ImportShape
//           Use this method for a topological naming of an imported shape
//=======================================================================

void DNaming::LoadImportedShape(const TDF_Label& theResultLabel, const TopoDS_Shape& theShape)
{
  theResultLabel.ForgetAllAttributes();
  TNaming_Builder aBuilder(theResultLabel);
  aBuilder.Generated(theShape);

  occ::handle<TDF_TagSource> aTagger = TDF_TagSource::Set(theResultLabel);
  if (aTagger.IsNull())
    return;
  aTagger->Set(0);

  LoadFirstLevel(theShape, aTagger);
  LoadC0Edges(theShape, aTagger);
  LoadC0Vertices(theShape, aTagger);
}

//=================================================================================================

void DNaming::LoadPrime(const TDF_Label& theResultLabel, const TopoDS_Shape& theShape)
{

  occ::handle<TDF_TagSource> aTagger = TDF_TagSource::Set(theResultLabel);
  if (aTagger.IsNull())
    return;
  aTagger->Set(0);

  LoadFirstLevel(theShape, aTagger);
  LoadC0Edges(theShape, aTagger);
  LoadC0Vertices(theShape, aTagger);
}

//
//=======================================================================
// function : Real
// purpose  : Gives the access to a real argument
//=======================================================================
occ::handle<TDataStd_Real> DNaming::GetReal(const occ::handle<TFunction_Function>& theFunction,
                                            const int                              thePosition)
{
  occ::handle<TDataStd_Real> aReal;
  if (!POSITION(theFunction, thePosition).FindAttribute(TDataStd_Real::GetID(), aReal))
    aReal = TDataStd_Real::Set(POSITION(theFunction, thePosition), 0.0);
  return aReal;
}

//=======================================================================
// function : Integer
// purpose  : Give an access to integer attribute
//=======================================================================
occ::handle<TDataStd_Integer> DNaming::GetInteger(
  const occ::handle<TFunction_Function>& theFunction,
  const int                              thePosition)
{
  occ::handle<TDataStd_Integer> anInteger;
  if (!POSITION(theFunction, thePosition).FindAttribute(TDataStd_Integer::GetID(), anInteger))
    anInteger = TDataStd_Integer::Set(POSITION(theFunction, thePosition), 0);
  return anInteger;
}

//=======================================================================
// function : String
// purpose  : Returns Name attribute
//=======================================================================
occ::handle<TDataStd_Name> DNaming::GetString(const occ::handle<TFunction_Function>& theFunction,
                                              const int                              thePosition)
{
  occ::handle<TDataStd_Name> aString;
  if (!POSITION(theFunction, thePosition).FindAttribute(TDataStd_Name::GetID(), aString))
    aString = TDataStd_Name::Set(POSITION(theFunction, thePosition), "");
  return aString;
}

//=======================================================================
// function : GetResult
// purpose  : Returns a result of a function, which is stored on a second label
//=======================================================================
occ::handle<TNaming_NamedShape> DNaming::GetFunctionResult(
  const occ::handle<TFunction_Function>& theFunction)
{
  occ::handle<TNaming_NamedShape> aNShape;
  theFunction->Label()
    .FindChild(FUNCTION_RESULT_LABEL)
    .FindAttribute(TNaming_NamedShape::GetID(), aNShape);
  return aNShape;
}

//=======================================================================
// function : Object
// purpose  : Returns UAttribute associated with Object
//=======================================================================
occ::handle<TDataStd_UAttribute> DNaming::GetObjectArg(
  const occ::handle<TFunction_Function>& theFunction,
  const int                              thePosition)
{
  occ::handle<TDataStd_UAttribute> anObject;
  occ::handle<TDF_Reference>       aReference;
  if (POSITION(theFunction, thePosition).FindAttribute(TDF_Reference::GetID(), aReference))
    aReference->Get().FindAttribute(GEOMOBJECT_GUID, anObject);
  return anObject;
}

//=======================================================================
// function : SetObject
// purpose  : Replace the argument by new value.
//=======================================================================
void DNaming::SetObjectArg(const occ::handle<TFunction_Function>&  theFunction,
                           const int                               thePosition,
                           const occ::handle<TDataStd_UAttribute>& theNewValue)
{

  if (theNewValue.IsNull())
    return;
  TDF_Reference::Set(POSITION(theFunction, thePosition), theNewValue->Label());
}

//=======================================================================
// function : GetObjectValue
// purpose  : Returns NamedShape of the Object
//=======================================================================
occ::handle<TNaming_NamedShape> DNaming::GetObjectValue(
  const occ::handle<TDataStd_UAttribute>& theObject)
{
  occ::handle<TNaming_NamedShape> aNS;

  if (!theObject.IsNull() && theObject->ID() == GEOMOBJECT_GUID)
  {

    occ::handle<TDF_Reference> aReference;
    if (theObject->FindAttribute(TDF_Reference::GetID(), aReference))
      aReference->Get().FindAttribute(TNaming_NamedShape::GetID(), aNS);
  }
  return aNS;

  /*
    occ::handle<TFunction_Function> aFun;
    occ::handle<TDataStd_TreeNode> aNode;
    objLabel.FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    if(aNode.IsNull()) return aFun;
    if(!aNode->HasFirst()) return aFun;
    else
      aNode = aNode->First();
    while(!aNode.IsNull()) {
      if(aNode->FindAttribute(TFunction_Function::GetID(), aFun)) {
        const Standard_GUID& aGUID = aFun->GetDriverGUID();
        if(aGUID == funGUID) break;
        else aFun.Nullify();
      }
      aNode = aNode->Next();
    }
  */
}

//=======================================================================
// function : GetPrevFunction
// purpose  : Returns previous function
//=======================================================================
occ::handle<TFunction_Function> DNaming::GetPrevFunction(
  const occ::handle<TFunction_Function>& theFunction)
{
  occ::handle<TFunction_Function> aPrevFun;
  if (!theFunction.IsNull())
  {
    occ::handle<TDataStd_TreeNode> aNode;
    theFunction->FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    while (!aNode.IsNull())
    {
      if (!aNode->HasPrevious())
        return aPrevFun;
      else
        aNode = aNode->Previous();
      aNode->FindAttribute(TFunction_Function::GetID(), aPrevFun);
      if (!aPrevFun.IsNull())
        break;
    }
  }
  return aPrevFun;
  /*
      while(!aNode.IsNull()) {
      if(aNode->FindAttribute(TFunction_Function::GetID(), aFun)) {
        const Standard_GUID& aGUID = aFun->GetDriverGUID();
        if(aGUID == funGUID) break;
        else aFun.Nullify();
      }
      aNode = aNode->Next();
    }
  */
}

//=======================================================================
// function : GetFirstFunction
// purpose  : Returns first function
//=======================================================================
occ::handle<TFunction_Function> DNaming::GetFirstFunction(
  const occ::handle<TDataStd_UAttribute>& theObject)
{
  occ::handle<TFunction_Function> aFirstFun;
  if (!theObject.IsNull())
  {
    occ::handle<TDataStd_TreeNode> aNode;
    theObject->FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    if (aNode.IsNull())
      return aFirstFun;
    if (!aNode->HasFirst())
      return aFirstFun;
    else
      aNode = aNode->First();

    while (!aNode.IsNull())
    {
      aNode->FindAttribute(TFunction_Function::GetID(), aFirstFun);
      if (!aFirstFun.IsNull())
        break;
      aNode = aNode->Next();
    }
  }
  return aFirstFun;
}

//=================================================================================================

occ::handle<TFunction_Function> DNaming::GetLastFunction(
  const occ::handle<TDataStd_UAttribute>& theObject)
{
  occ::handle<TFunction_Function> aLastFun;
  if (!theObject.IsNull())
  {
    occ::handle<TDataStd_TreeNode> aNode;
    theObject->FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    if (aNode.IsNull())
      return aLastFun;
    if (!aNode->HasFirst())
      return aLastFun;
    else
      aNode = aNode->First();

    while (!aNode.IsNull())
    {
      if (aNode->IsAttribute(TFunction_Function::GetID()))
        aNode->FindAttribute(TFunction_Function::GetID(), aLastFun);
      aNode = aNode->Next();
    }
  }
  return aLastFun;
}

//=================================================================================================

occ::handle<TDataStd_UAttribute> DNaming::GetObjectFromFunction(
  const occ::handle<TFunction_Function>& theFunction)
{
  occ::handle<TDataStd_UAttribute> anObject;
  if (!theFunction.IsNull())
  {
    occ::handle<TDataStd_TreeNode> aNode;
    theFunction->FindAttribute(TDataStd_TreeNode::GetDefaultTreeID(), aNode);
    if (!aNode.IsNull())
    {
      if (!aNode->HasFather())
        return anObject;
      else
        aNode = aNode->Father();
      aNode->FindAttribute(GEOMOBJECT_GUID, anObject);
    }
  }
  return anObject;
  /*
      while(!aNode.IsNull()) {
      if(aNode->FindAttribute(TFunction_Function::GetID(), aFun)) {
        const Standard_GUID& aGUID = aFun->GetDriverGUID();
        if(aGUID == funGUID) break;
        else aFun.Nullify();
      }
      aNode = aNode->Next();
    }
  */
}

//=================================================================================================

void DNaming::LoadResult(const TDF_Label& ResultLabel, BRepAlgoAPI_BooleanOperation& MS)
{
  occ::handle<TDF_TagSource> Tagger = TDF_TagSource::Set(ResultLabel);
  if (Tagger.IsNull())
    return;
  Tagger->Set(0);
  TNaming_Builder Builder(ResultLabel);
  TopoDS_Shape    aResult = MS.Shape();
  if (aResult.ShapeType() == TopAbs_COMPOUND)
  {
    if (aResult.NbChildren() == 1)
    {
      TopoDS_Iterator itr(aResult);
      if (itr.More())
        aResult = itr.Value();
    }
  }
  if (MS.Shape1().IsNull())
    Builder.Generated(aResult);
  else
  {
    Builder.Modify(MS.Shape1(), aResult);
  }
}

//=================================================================================================

void DNaming::LoadAndOrientModifiedShapes(
  BRepBuilderAPI_MakeShape&                                                       MS,
  const TopoDS_Shape&                                                             ShapeIn,
  const TopAbs_ShapeEnum                                                          KindOfShape,
  TNaming_Builder&                                                                Builder,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& SubShapes)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  TopExp_Explorer                                        ShapeExplorer(ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next())
  {
    const TopoDS_Shape& Root = ShapeExplorer.Current();
    if (!View.Add(Root))
      continue;
    const NCollection_List<TopoDS_Shape>&    Shapes = MS.Modified(Root);
    NCollection_List<TopoDS_Shape>::Iterator ShapesIterator(Shapes);
    for (; ShapesIterator.More(); ShapesIterator.Next())
    {
      TopoDS_Shape newShape = ShapesIterator.Value();
      if (SubShapes.IsBound(newShape))
      {
        newShape.Orientation((SubShapes(newShape)).Orientation());
      }
      if (!Root.IsSame(newShape))
        Builder.Modify(Root, newShape);
    }
  }
}

//=================================================================================================

void DNaming::LoadDeletedShapes(BRepBuilderAPI_MakeShape& MS,
                                const TopoDS_Shape&       ShapeIn,
                                const TopAbs_ShapeEnum    KindOfShape,
                                TNaming_Builder&          Builder)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  TopExp_Explorer                                        ShapeExplorer(ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next())
  {
    const TopoDS_Shape& Root = ShapeExplorer.Current();
    if (!View.Add(Root))
      continue;
    if (MS.IsDeleted(Root))
    {
      Builder.Delete(Root);
    }
  }
}

//=================================================================================================

void DNaming::LoadAndOrientGeneratedShapes(
  BRepBuilderAPI_MakeShape&                                                       MS,
  const TopoDS_Shape&                                                             ShapeIn,
  const TopAbs_ShapeEnum                                                          KindOfShape,
  TNaming_Builder&                                                                Builder,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& SubShapes)
{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  TopExp_Explorer                                        ShapeExplorer(ShapeIn, KindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next())
  {
    const TopoDS_Shape& Root = ShapeExplorer.Current();
    if (!View.Add(Root))
      continue;
    const NCollection_List<TopoDS_Shape>&    Shapes = MS.Generated(Root);
    NCollection_List<TopoDS_Shape>::Iterator ShapesIterator(Shapes);
    for (; ShapesIterator.More(); ShapesIterator.Next())
    {
      TopoDS_Shape newShape = ShapesIterator.Value();
      if (SubShapes.IsBound(newShape))
      {
        newShape.Orientation((SubShapes(newShape)).Orientation());
      }
      if (!Root.IsSame(newShape))
        Builder.Generated(Root, newShape);
    }
  }
}

//=======================================================================
// function : ComputeNormalizedVector
// purpose  : Computes normalized vector from shape if it is possible
//=======================================================================
bool DNaming::ComputeAxis(const occ::handle<TNaming_NamedShape>& theNS, gp_Ax1& theAx1)
{
  if (theNS.IsNull() || theNS->IsEmpty())
    return false;
  TopoDS_Shape aShape = theNS->Get();
  if (aShape.IsNull())
    return false;
  if (aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE)
  {
    if (aShape.ShapeType() == TopAbs_WIRE)
    {
      TopExp_Explorer anExplorer(aShape, TopAbs_EDGE);
      aShape = anExplorer.Current();
    }
    const TopoDS_Edge&      anEdge = TopoDS::Edge(aShape);
    double                  aFirst, aLast;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
    if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    {
      occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(aCurve);
      if (!aLine.IsNull())
      {
        theAx1 = aLine->Position();
        return true;
      }
    }
  }
  return false;
}

//=================================================================================================

bool DNaming::IsAttachment(const occ::handle<TDataStd_UAttribute>& anObj)
{

  occ::handle<TFunction_Function> aFun = GetFirstFunction(anObj);
  if (!aFun.IsNull())
  {
    const Standard_GUID& aGUID = aFun->GetDriverGUID();
    if (aGUID == ATTCH_GUID || aGUID == XTTCH_GUID)
    {
      return aFun->Label()
        .FindChild(FUNCTION_ARGUMENTS_LABEL)
        .FindChild(ATTACH_ARG)
        .IsAttribute(TDF_Reference::GetID());
    }
  }
  return false;
}

//=================================================================================================

occ::handle<TNaming_NamedShape> DNaming::GetAttachmentsContext(
  const occ::handle<TDataStd_UAttribute>& anObj)
{
  occ::handle<TNaming_NamedShape> aNS;
  occ::handle<TFunction_Function> aFun = GetFirstFunction(anObj);
  if (!aFun.IsNull())
  {
    const Standard_GUID& aGUID = aFun->GetDriverGUID();
    if (aGUID == ATTCH_GUID)
    {
      const TDF_Label& aLabel =
        aFun->Label().FindChild(FUNCTION_ARGUMENTS_LABEL).FindChild(ATTACH_ARG);
      occ::handle<TDF_Reference>      aRef;
      occ::handle<TFunction_Function> aFunCnt;
      if (aLabel.FindAttribute(TDF_Reference::GetID(), aRef))
      {
        if (aRef->Get().FindAttribute(TFunction_Function::GetID(), aFunCnt))
        {
          const TDF_Label& aResultLabel = aFunCnt->Label().FindChild(FUNCTION_RESULT_LABEL, true);
          aResultLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS);
        }
      }
    }
  }
  return aNS;
}

//=======================================================================
// function : ComputeSweepDir
// purpose  : Computes direction for extrusion
//=======================================================================
bool DNaming::ComputeSweepDir(const TopoDS_Shape& theShape, gp_Ax1& theAxis)
{
  // Find surface
  occ::handle<Geom_Plane> aPlane;

  if (theShape.ShapeType() == TopAbs_FACE)
  {
    occ::handle<Geom_Surface> aSurf = BRep_Tool::Surface(TopoDS::Face(theShape));
#ifdef OCCT_DEBUG
    const char* s = aSurf->DynamicType()->Name();
    std::cout << "Surface Dynamic TYPE = " << s << std::endl;
#endif
    if (aSurf->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
      aSurf = occ::down_cast<Geom_RectangularTrimmedSurface>(aSurf)->BasisSurface();
    aPlane = occ::down_cast<Geom_Plane>(aSurf);
  }

  if (aPlane.IsNull())
  {
    BRepLib_FindSurface aFinder(theShape, 0., true);
    if (!aFinder.Found())
      return false;
    aPlane = occ::down_cast<Geom_Plane>(aFinder.Surface());
  }

  if (aPlane.IsNull())
    return false;

  theAxis = aPlane->Pln().Axis();
  if (!aPlane->Pln().Direct())
    theAxis.Reverse();

  if (theShape.Orientation() == TopAbs_REVERSED)
    theAxis.Reverse();

  return true;
}
