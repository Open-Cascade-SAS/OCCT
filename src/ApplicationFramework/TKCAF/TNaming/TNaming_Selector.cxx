// Created on: 1999-09-30
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDF_ChildIterator.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Identifier.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_NameType.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_Selector.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>

// #define MDTV_DEB_SEL
#ifdef OCCT_DEBUG_SEL
  // #define MDTV_DEB_BNP
  #include <TopExp_Explorer.hxx>
  #include <TCollection_AsciiString.hxx>
  #include <TNaming_Tool.hxx>
  #include <BRep_Tool.hxx>
  #include <TopoDS.hxx>
  #include <TNaming_UsedShapes.hxx>

void PrintEntry(const TDF_Label& label, const bool allLevels)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  std::cout << "LabelEntry = " << entry << std::endl;
  if (allLevels)
  {
    TDF_ChildIterator it(label, allLevels);
    for (; it.More(); it.Next())
    {
      TDF_Tool::Entry(it.Value(), entry);
      std::cout << "ChildLabelEntry = " << entry << std::endl;
    }
  }
}

  #include <BRepTools.hxx>

static void Write(const TopoDS_Shape& shape, const char* const filename)
{
  char buf[256];
  if (strlen(filename) > 255)
    return;
  #if defined _MSC_VER
  strcpy_s(buf, filename);
  #else
  strcpy(buf, filename);
  #endif
  char* p = buf;
  while (*p)
  {
    if (*p == ':')
      *p = '-';
    p++;
  }
  std::ofstream save(buf);
  if (!save)
    std::cout << "File " << buf << " was not created: rdstate = " << save.rdstate() << std::endl;
  save << "DBRep_DrawableShape" << std::endl << std::endl;
  if (!shape.IsNull())
    BRepTools::Write(shape, save);
  save.close();
}
#endif

#define ORIENTATION_DSOPT
#ifdef ORIENTATION_DSOPT
  #include <NCollection_Map.hxx>
  #include <TDF_ChildIDIterator.hxx>
  #include <TNaming_Tool.hxx>
  #include <TNaming_Iterator.hxx>

//==========================================================================================
inline static void MapOfOrientedShapes(const TopoDS_Shape& S, NCollection_Map<TopoDS_Shape>& M)
{
  M.Add(S);
  TopoDS_Iterator It(S, true, true);
  while (It.More())
  {
    MapOfOrientedShapes(It.Value(), M);
    It.Next();
  }
}

//=======================================================================
static void BuildAtomicMap(const TopoDS_Shape& S, NCollection_Map<TopoDS_Shape>& M)
{
  if (S.ShapeType() > TopAbs_COMPSOLID)
    return;
  TopoDS_Iterator it(S);
  for (; it.More(); it.Next())
  {
    if (it.Value().ShapeType() > TopAbs_COMPSOLID)
      M.Add(it.Value());
    else
      BuildAtomicMap(it.Value(), M);
  }
}

//==========================================================================================
static const occ::handle<TNaming_NamedShape> FindPrevNDS(const occ::handle<TNaming_NamedShape>& CNS)
{
  occ::handle<TNaming_NamedShape> aNS;
  TNaming_Iterator                it(CNS);
  if (it.More())
  {
    if (!it.OldShape().IsNull())
    {
      aNS = TNaming_Tool::NamedShape(it.OldShape(), CNS->Label());
      return aNS;
    }
  }
  return aNS;
}

//==========================================================================================
// Purpose: checks correspondens between orientation of sub-shapes of Context and orientation
//          of sub-shapes registered in DF and put under result label
//==========================================================================================
static bool IsSpecificCase(const TDF_Label& F, const TopoDS_Shape& Context)
{
  bool                          isFound(false);
  NCollection_Map<TopoDS_Shape> shapesOfContext;
  MapOfOrientedShapes(Context, shapesOfContext);
  occ::handle<TNaming_NamedShape> CNS = TNaming_Tool::NamedShape(Context, F);
  #ifdef OCCT_DEBUG_BNP
  PrintEntry(CNS->Label(), 0);
  #endif
  if (!CNS.IsNull())
  {
    NCollection_List<occ::handle<TNaming_NamedShape>> aLNS;
    TDF_ChildIDIterator cit(CNS->Label(), TNaming_NamedShape::GetID(), false);
    if (!cit.More())
    {
      // Naming data structure is empty - no sub-shapes under resulting shape
      // clang-format off
      const occ::handle<TNaming_NamedShape> aNS = FindPrevNDS(CNS); //look to old shape data structure if exist
      // clang-format on
      if (!aNS.IsNull())
      {
  #ifdef OCCT_DEBUG_BNP
        PrintEntry(aNS->Label(), 0);
  #endif
        cit.Initialize(aNS->Label(), TNaming_NamedShape::GetID(), false);
      }
      else
        return true;
    }

    for (; cit.More(); cit.Next())
    {
      occ::handle<TNaming_NamedShape> NS(occ::down_cast<TNaming_NamedShape>(cit.Value()));
      if (!NS.IsNull())
      {
        TopoDS_Shape aS = TNaming_Tool::CurrentShape(NS);
        if (aS.IsNull())
          continue;
  #ifdef OCCT_DEBUG_BNP
        PrintEntry(NS->Label(), 0);
        std::cout << "ShapeType =" << aS.ShapeType() << std::endl;
        Write(aS, "BNProblem.brep");
  #endif
        if (aS.ShapeType() != TopAbs_COMPOUND)
        { // single shape at the child label
          if (!shapesOfContext.Contains(aS))
          {
            isFound = true;
            break;
          }
        }
        else
        {
          NCollection_Map<TopoDS_Shape> M;
          BuildAtomicMap(aS, M);
          NCollection_Map<TopoDS_Shape>::Iterator it(M);
          for (; it.More(); it.Next())
          {
            if (!shapesOfContext.Contains(it.Key()))
            {
  #ifdef OCCT_DEBUG_BNP
              std::cout << "BNProblem: ShapeType in AtomicMap = " << it.Key().ShapeType()
                        << " TShape = " << it.Key().TShape() << " OR = " << it.Key().Orientation()
                        << std::endl;
              Write(it.Key(), "BNProblem_AtomicMap_Item.brep");
              NCollection_Map<TopoDS_Shape>::Iterator itC(shapesOfContext);
              for (; itC.More(); itC.Next())
                std::cout << " ShapeType = " << itC.Key().ShapeType()
                          << " TShape = " << itC.Key().TShape()
                          << " OR = " << itC.Key().Orientation() << std::endl;

  #endif
              isFound = true;
              break;
            }
            if (isFound)
              break;
          }
        }
      }
    }
  }
  return isFound;
}

//==========================================================================================
static bool IsSpecificCase2(const TDF_Label& F, const TopoDS_Shape& Selection)
{
  bool isTheCase(false);
  if (Selection.ShapeType() == TopAbs_EDGE)
  {
    occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(Selection, F);
    if (!aNS.IsNull())
    { // presented in DF
  #ifdef OCCT_DEBUG_BNP
      PrintEntry(aNS->Label(), 0);
  #endif
      const TopoDS_Shape& aS = TNaming_Tool::CurrentShape(aNS);
      if (!aS.IsNull() && aS.ShapeType() == Selection.ShapeType())
      {
        if (Selection.Orientation() != aS.Orientation())
        {
          isTheCase = true;
        }
      }
    }
  }
  return isTheCase;
}
#endif
//=======================================================================
// function : FindGenerated
// purpose  : Finds all generated from the <S>
//=======================================================================

static void FindGenerated(const occ::handle<TNaming_NamedShape>& NS,
                          const TopoDS_Shape&                    S,
                          NCollection_List<TopoDS_Shape>&        theList)

{
  const TDF_Label& LabNS = NS->Label();
  for (TNaming_NewShapeIterator it(S, LabNS); it.More(); it.Next())
  {
    if (it.Label() == LabNS)
    {
      theList.Append(it.Shape());
    }
  }
}

//=================================================================================================

bool TNaming_Selector::IsIdentified(const TDF_Label&                 L,
                                    const TopoDS_Shape&              Selection,
                                    occ::handle<TNaming_NamedShape>& NS,
                                    const bool                       Geometry)
{
  TopoDS_Shape       Context;
  bool               OnlyOne = !Geometry;
  TNaming_Identifier Ident(L, Selection, Context, OnlyOne);
  if (Ident.IsFeature())
  {
    if (!OnlyOne)
      return false;
    else
    {
      NS = Ident.FeatureArg();

      // mpv : external condition
      NCollection_Map<TDF_Label>                                    Forbiden, Valid;
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
      TNaming_NamingTool::CurrentShape(Valid, Forbiden, NS, MS);
      return (MS.Contains(Selection) && MS.Extent() == 1);
    }
  }
  else if (Ident.Type() == TNaming_GENERATION)
  {
    NS = Ident.NamedShapeOfGeneration();
    if (!NS.IsNull())
    {
      NCollection_Map<TDF_Label>                                    Forbiden, Valid;
      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
      TNaming_NamingTool::CurrentShape(Valid, Forbiden, NS, MS);
      if (MS.Contains(Selection) && MS.Extent() == 1)
      {
        const TopoDS_Shape&            aS = Ident.ShapeArg();
        NCollection_List<TopoDS_Shape> aList;
        FindGenerated(NS, aS, aList);
        Ident.NextArg();
        while (Ident.MoreArgs())
        {
          const TopoDS_Shape& aShape = Ident.ShapeArg();
          FindGenerated(NS, aShape, aList);
          Ident.NextArg();
        }
        const TopoDS_Shape&                      aC = MS(1);
        bool                                     isEq(false);
        NCollection_List<TopoDS_Shape>::Iterator itl(aList);
        for (; itl.More(); itl.Next())
        {
          if (itl.Value() == aC)
            isEq = true;
          else
          {
            isEq = false;
            break;
          }
        }
        return isEq;
      }
    }
    else
      return false;
  }
  return false;
}

//=================================================================================================

TNaming_Selector::TNaming_Selector(const TDF_Label& L)
{
  myLabel = L;
}

//=================================================================================================

bool TNaming_Selector::Select(const TopoDS_Shape& Selection,
                              const TopoDS_Shape& Context,
                              const bool          Geometry,
                              const bool          KeepOrientation) const
{
  myLabel.ForgetAllAttributes();
  occ::handle<TNaming_NamedShape> NS;
  bool aKeepOrientation((Selection.ShapeType() == TopAbs_VERTEX) ? false : KeepOrientation);
  if (Selection.ShapeType() == TopAbs_COMPOUND)
  {
    bool            isVertex(true);
    TopoDS_Iterator it(Selection);
    for (; it.More(); it.Next())
      if (it.Value().ShapeType() != TopAbs_VERTEX)
      {
        isVertex = false;
        break;
      }
    if (isVertex)
      aKeepOrientation = false;
  }
  /*
 // for debug opposite orientation
 TopoDS_Shape selection;
 bool found(false);
 TopExp_Explorer exp(Context,TopAbs_EDGE);
 for(;exp.More();exp.Next()) {
   TopoDS_Shape E = exp.Current();
   if(E.IsSame(Selection) && E.Orientation() != Selection.Orientation()) {
     selection = E;
   found = true;
   std::cout <<" FOUND: Entity orientation = " << selection.Orientation() <<std::endl;
   }
 }
 if (!found)
   selection = Selection;
  */

#ifdef OCCT_DEBUG_SEL
  std::cout << "SELECTION ORIENTATION = " << Selection.Orientation()
            << ", TShape = " << Selection.TShape() << std::endl;
  // std::cout << "SELECTION ORIENTATION = " << selection.Orientation() <<", TShape = " <<
  // selection.TShape() <<std::endl;
  PrintEntry(myLabel, 0);
  TNaming::Print(myLabel, std::cout);
#endif

  if (aKeepOrientation)
  {
#ifdef ORIENTATION_DSOPT
    const bool aBNproblem = IsSpecificCase(myLabel, Context) || IsSpecificCase2(myLabel, Selection);

    NS = TNaming_Naming::Name(myLabel, Selection, Context, Geometry, aKeepOrientation, aBNproblem);
#else
    NS = TNaming_Naming::Name(myLabel, Selection, Context, Geometry, aKeepOrientation);
#endif
  }
  else if (!IsIdentified(myLabel, Selection, NS, Geometry))
  {
    NS = TNaming_Naming::Name(myLabel, Selection, Context, Geometry, aKeepOrientation);
  }
  if (NS.IsNull())
    return false;
  //
  // namedshape with SELECTED Evolution
  //
  TNaming_Builder B(myLabel);
  // mpv: if oldShape for selection is some shape from used map of shapes,
  //      then naming structure becomes more complex, can be cycles
  const TopoDS_Shape& aSelection = TNaming_Tool::CurrentShape(NS); // szy
#ifdef OCCT_DEBUG_CHECK_TYPE
  if (!Selection.IsSame(aSelection) && Selection.ShapeType() != TopAbs_COMPOUND)
  {
    TCollection_AsciiString entry;
    TDF_Tool::Entry(NS->Label(), entry);
    std::cout << "Selection is Not Same (NSLabel = " << entry
              << "): TShape1 = " << Selection.TShape()->This()
              << " TShape2 = " << aSelection.TShape()->This() << std::endl;
  }
#endif
  if (aSelection.ShapeType() == TopAbs_COMPOUND && aSelection.ShapeType() != Selection.ShapeType())
    B.Select(aSelection, aSelection); // type migration
  else
    B.Select(Selection, Selection);
  //
  // naming with IDENTITY NameType
  //
  occ::handle<TNaming_Naming> N = new TNaming_Naming();
  N->ChangeName().Type(TNaming_IDENTITY);
  N->ChangeName().Append(NS);
  N->ChangeName().Orientation(Selection.Orientation());
  // inserted by vro 06.09.00:
  N->ChangeName().ShapeType(Selection.ShapeType());

  myLabel.AddAttribute(N);
  return true;
}

//=================================================================================================

bool TNaming_Selector::Select(const TopoDS_Shape& Selection,
                              const bool          Geometry,
                              const bool          KeepOrientation) const
{
  // we give a Null shape. How to guess what is the good context ?
  TopoDS_Shape Context;
  //  return Select (Selection,Context,Geometry);
  // temporary!!!
  return Select(Selection, Selection, Geometry, KeepOrientation);
}

//=================================================================================================

bool TNaming_Selector::Solve(NCollection_Map<TDF_Label>& Valid) const
{
  occ::handle<TNaming_Naming> name;
#ifdef OCCT_DEBUG_SEL
  std::cout << "TNaming_Selector::Solve==> ";
  PrintEntry(myLabel, 0);
#endif
  if (myLabel.FindAttribute(TNaming_Naming::GetID(), name))
  {
    return name->Solve(Valid);
  }
  return false;
}

//=================================================================================================

void TNaming_Selector::Arguments(NCollection_Map<occ::handle<TDF_Attribute>>& args) const
{
  TDF_Tool::OutReferences(myLabel, args);
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_Selector::NamedShape() const
{
  occ::handle<TNaming_NamedShape> NS;
  myLabel.FindAttribute(TNaming_NamedShape::GetID(), NS);
  return NS;
}
