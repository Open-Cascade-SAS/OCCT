// Created on: 1997-09-03
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

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Map.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_IDFilter.hxx>
#include <TDF_Label.hxx>
#include <NCollection_List.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_TagSource.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Identifier.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Localizer.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_OldShapeIterator.hxx>
#include <TNaming_Scope.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TNaming_Naming, TDF_Attribute)

// #include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
// The bug concerns access to a null object in the method Filter():

#define ALLOW_CHILD_NBS
// #define MDTV_DEB_CC
// #define MDTV_DEB_OR
// #define MDTV_DEB_MOD
// #define MDTV_OR
// #define MDTV_DEB_INNS
// #define MDTV_DEB_NBS
// #define MDTV_DEB_71
// #define MDTV_DEB_WIN
#ifdef OCCT_DEBUG
  #include <TCollection_AsciiString.hxx>
  #include <TDF_Tool.hxx>

void Print_Entry(const TDF_Label& label)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  std::cout << "LabelEntry = " << entry << std::endl;
}

static void Write(const TopoDS_Shape& shape, const char* filename)
{
  char buf[256];
  if (strlen(filename) > 256)
    return;
  strcpy(buf, filename);
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

void WriteNSOnLabel(const occ::handle<TNaming_NamedShape>& NS, const TCollection_AsciiString& Nam)
{
  if (!NS.IsNull())
  {
    TCollection_AsciiString entry;
    TDF_Tool::Entry(NS->Label(), entry);
    TopoDS_Shape Sh = TNaming_Tool::GetShape(NS);
    if (!Sh.IsNull())
    {
      TCollection_AsciiString Entry = Nam + entry + ".brep";
      Write(Sh, Entry.ToCString());
    }
    else
      std::cout << "WriteNSOnLabel>>> TopoDS_Shape IS NULL on Entry = " << entry << std::endl;
  }
  else
    std::cout << "WriteNSOnLabel >>>  NamedShape IS NULL" << std::endl;
}
#endif

//==========================================================================================
static occ::handle<TNaming_NamedShape> BuildName(const TDF_Label&                       F,
                                                 TNaming_Scope&                         MDF,
                                                 const TopoDS_Shape&                    S,
                                                 const TopoDS_Shape&                    Context,
                                                 const occ::handle<TNaming_NamedShape>& Stop,
                                                 const bool                             Geometry);

//=======================================================================
static int RepeatabilityInContext(const TopoDS_Shape& Selection, const TopoDS_Shape& Context);

//=======================================================================
// function : Solve
// purpose  : voir avec YFR comment retrouver le bon resulat et le mettre
//         : dans le NamedShape de L
//=======================================================================
bool TNaming_Naming::Solve(NCollection_Map<TDF_Label>& Valid)
{
  occ::handle<TNaming_Naming> subname;
  for (TDF_ChildIterator it(Label(), false); it.More(); it.Next())
  {
#ifdef OCCT_DEBUG_NBS
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(it.Value(), anEntry);
    std::cout << "TNaming_Naming::Solve: Label to be solved = " << anEntry << std::endl;
#endif
    if (it.Value().FindAttribute(TNaming_Naming::GetID(), subname))
    {
      if (!subname->Solve(Valid))
      {
        return false; // not necessary to continue
      }
    }
  }
#ifdef OCCT_DEBUG_CC
  NCollection_Map<TDF_Label>::Iterator anItr(Valid);
  std::cout << "TNaming_Naming::Solve:: Valid label Map" << std::endl;
  for (; anItr.More(); anItr.Next())
  {
    const TDF_Label&        aLabel = anItr.Key();
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    std::cout << "Label = " << anEntry << std::endl;
  }
#endif
  if (Regenerate(Valid))
  {
    if (!Valid.IsEmpty())
      Valid.Add(Label());
    return true;
  }
  return false;
}

//=================================================================================================

const Standard_GUID& TNaming_Naming::GetID()
{
  static Standard_GUID TNaming_NamingID("c0a19201-5b78-11d1-8940-080009dc3333");
  return TNaming_NamingID;
}

//=================================================================================================

occ::handle<TNaming_Naming> TNaming_Naming::Insert(const TDF_Label& under)
{
  occ::handle<TNaming_Naming> N;
  TDF_Label                   child = TDF_TagSource::NewChild(under);
  N                                 = new TNaming_Naming();
  child.AddAttribute(N);
  return N;
}

//=======================================================================
// function : BuildNS
// purpose  : returns a new NamedShape, which is built as selection:
//         : TNaming_Builder::Select("S","S") at the new child label of the label <F>.
//=======================================================================

static occ::handle<TNaming_NamedShape> BuildNS(const TDF_Label&        F,
                                               const TopoDS_Shape&     S,
                                               const TNaming_NameType& Name)
{
  occ::handle<TNaming_Naming> Naming = TNaming_Naming::Insert(F);

  TNaming_Name& theName = Naming->ChangeName();
  theName.ShapeType(S.ShapeType());
  theName.Shape(S);
  theName.Orientation(S.Orientation());
  theName.Type(Name);
  TNaming_Builder B(Naming->Label());
  B.Select(S, S);
  return B.NamedShape();
}

//=================================================================================================

static int FindIndex(const occ::handle<TNaming_NamedShape>& NS, const TopoDS_Shape& S)
{
  NCollection_List<TDF_Label> Labels;
  TopoDS_Shape                IS    = TNaming_Tool::InitialShape(S, NS->Label(), Labels);
  int                         Index = 1;
  for (TNaming_Iterator itNS(NS); itNS.More(); itNS.Next(), Index++)
  {
    if (IS.IsSame(itNS.NewShape()))
      break;
  }
  return Index;
}

//=======================================================================
// function : CompareInGeneration
// purpose  : returns true, only if the specified NS contains only <S> in
//         : the "new shapes" set.
//=======================================================================

static bool CompareInGeneration(const occ::handle<TNaming_NamedShape>& NS, const TopoDS_Shape& S)
{
  for (TNaming_Iterator it(NS); it.More(); it.Next())
  {
    if (!it.NewShape().IsSame(S))
      return false;
  }
  return true;
}

//=======================================================================
// function : GetShapeEvolutions
// purpose  : returns true, if target has parent in source; list contains inheritance chain
//=======================================================================
static bool GetShapeEvolutions(
  const TopoDS_Shape&                    theTarget, // this is changed in recursion
  const occ::handle<TNaming_NamedShape>& theSource,
  NCollection_List<TopoDS_Shape>&        aList) // returns list in the backward order
{
  occ::handle<TNaming_NamedShape> aTarget = TNaming_Tool::NamedShape(theTarget, theSource->Label());
  if (!aTarget.IsNull())
  {
#ifdef OCCT_DEBUG_71
    std::cout << "GetShapeEvolutions: target NS = ";
    Print_Entry(aTarget->Label());
    std::cout << "GetShapeEvolutions: Source NS = ";
    Print_Entry(theSource->Label());
    TCollection_AsciiString aNam("GetShapeEvolutions");
    WriteNSOnLabel(aTarget, aNam);
#endif
    // clang-format off
    if (aTarget->Label() == theSource->Label()) return true; // check if target is in the source
    // clang-format on
  }
  else
    return false;

  TNaming_Iterator anIter(aTarget);
  for (; anIter.More(); anIter.Next())
  { // check all appropriate old shapes of target
#ifdef OCCT_DEBUG_71
    if (!anIter.OldShape().IsNull())
    {
      Write(anIter.OldShape(), "Target_OldS.brep");
      std::cout << "Target OldS TS =" << anIter.OldShape().TShape()->This() << std::endl;
    }
    if (!anIter.NewShape().IsNull())
    {
      Write(anIter.NewShape(), "Target_NewS.brep");
      std::cout << "Target NewS TS =" << anIter.NewShape().TShape()->This() << std::endl;
    }
#endif
    if (anIter.OldShape().IsNull() || anIter.NewShape().IsNull())
      continue;
    if (!anIter.NewShape().IsSame(theTarget))
      continue;
    if (GetShapeEvolutions(anIter.OldShape(), theSource, aList))
    { // recursion: now target is old shape
      // clang-format off
      aList.Append(theTarget); // if oldshape has the source as parent (or belongs to it) , fill the list
      // clang-format on
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : CompareInModification
// purpose  : returns empty named shape if naming is already done
//=======================================================================

static occ::handle<TNaming_NamedShape> CompareInModification(
  const occ::handle<TNaming_NamedShape>& NS, // parent
  const TopoDS_Shape&                    S)                     // target
{
  occ::handle<TNaming_NamedShape> aResult;
  if (S.IsNull() || NS.IsNull())
    return aResult;
#ifdef OCCT_DEBUG_71
  std::cout << "CompareInModification: parent NS = ";
  Print_Entry(NS->Label());
  Write(S, "CompareInM_S.brep");
  TCollection_AsciiString aNam("CompareInM");
  WriteNSOnLabel(NS, aNam);
#endif
  occ::handle<TNaming_NamedShape> aSource; // parent NamedShape, which can be found by TopoDS shape
  TNaming_Iterator                anIt(NS);
  for (; anIt.More() && aSource.IsNull(); anIt.Next())
  {
    if (!anIt.NewShape().IsNull())
    {
      aSource = TNaming_Tool::NamedShape(anIt.NewShape(), NS->Label());
#ifdef OCCT_DEBUG_71
      TCollection_AsciiString aNam("CompareInM_Source");
      WriteNSOnLabel(aSource, aNam);
#endif
    }
  }
  // searching for 1:n to the same label modifications (in this case current naming is insufficient)
  NCollection_List<TopoDS_Shape> aList;
  if (GetShapeEvolutions(S, aSource, aList) && aList.Extent() > 0)
  {
    NCollection_List<TopoDS_Shape>::Iterator anIter(aList);
    for (; anIter.More(); anIter.Next())
    {
      aResult = TNaming_Tool::NamedShape(anIter.Value(), NS->Label());
      if (aResult->Evolution() != TNaming_MODIFY)
      { // evolution must be modify, otherwise everything is OK
        aResult.Nullify();
        return aResult;
      }
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>
                       aMap; // collection of the old shapes of the shape from list
      TNaming_Iterator aNIter1(aResult);

      for (; aNIter1.More(); aNIter1.Next())
      {
        if (aNIter1.NewShape().IsSame(anIter.Value()))
        { // if isSame
          aMap.Add(aNIter1.OldShape());
        }
      }
      // clang-format off
      TNaming_Iterator aNIter2(aResult); // if some another shapes has oldshape from map, return namedshape with this oldshape
      // clang-format on

      for (; aNIter2.More(); aNIter2.Next())
      {
        if (aNIter2.NewShape().IsSame(anIter.Value()))
          continue;
        if (aMap.Contains(aNIter2.OldShape()))
        { // if one shape was modified to the two at the shared label, return this one
          aResult = TNaming_Tool::NamedShape(aNIter2.OldShape(), NS->Label());
          if (!aResult.IsNull())
            return aResult;
        }
      }
    }
    aResult.Nullify();
  }
  return aResult;
}

//=======================================================================
static bool FillSMap(const TopoDS_Shape&                                     S,
                     NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& MS)
{
  if (S.IsNull())
    return false;
  bool             isHomogen(true);
  TopAbs_ShapeEnum aPrevType(TopAbs_SHAPE);
  TopoDS_Iterator  it(S);
  for (; it.More(); it.Next())
  {
    const TopAbs_ShapeEnum aType = it.Value().ShapeType();
#ifdef OCCT_DEBUG_CC
    std::cout << "TestSolution_FillMap: S_Type = :" << it.Value().ShapeType()
              << " TShape = " << it.Value().TShape()->This() << std::endl;
#endif
    if (aType > TopAbs_COMPSOLID)
    {
      MS.Add(it.Value());
      if (aPrevType == TopAbs_SHAPE)
        aPrevType = aType;
      else if (aPrevType != aType)
        isHomogen = false;
    }
    else if (!FillSMap(it.Value(), MS))
      isHomogen = false;
  }
  return isHomogen;
}

//=======================================================================
// function : Compare
// purpose  : checks naming of the shape <S> in the NamedShape <NS>.
//         : Returns true, if it's correct. Details ==>
//         : The method takes all modifications of the "NS" (see CurrentShape method),
//         : which are in the "MDF" (if it's not empty) before <Stop> shape and check them.
//         : whether these modifications contain only "S". If yes then the method
//         : returns true, otherwise it returns false.
//=======================================================================

static bool Compare(const occ::handle<TNaming_NamedShape>& NS,
                    const TNaming_Scope&                   MDF,
                    const occ::handle<TNaming_NamedShape>& Stop,
                    const TopoDS_Shape&                    S)
{
  NCollection_Map<TDF_Label>                                    Forbiden;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  if (!Stop.IsNull())
    TNaming_NamingTool::BuildDescendants(Stop, Forbiden);
  TNaming_NamingTool::CurrentShape(MDF.GetValid(), Forbiden, NS, MS);
#ifdef OCCT_DEBUG_NBS
  Write(S, "Compare_S.brep");
  std::cout << "S: TShape = " << S.TShape()->This() << std::endl;
  TCollection_AsciiString aNam("Compare_MS_");
  TCollection_AsciiString ext(".brep");
  for (int anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    TCollection_AsciiString aName = aNam + anItMS + ext;
    Write(MS(anItMS), aName.ToCString());
    std::cout << aName.ToCString() << ": TShape = " << MS(anItMS).TShape()->This() << std::endl;
  }
#endif
  return (MS.Contains(S) && MS.Extent() == 1);
}

//=======================================================================
// function : TestSolution
// purpose  : returns true, if last modification of shape from "NS" is equal to "S":
//         : If shape "S" has atomic type (TopAbs_FACE, TopAbs_EDGE, TopAbs_VERTEX),
//         : then returns S.IsSame(shape from "NS").
//         : Otherwise the result of exploration of these shapes must be same.
//=======================================================================

static bool TestSolution(const TNaming_Scope&                   MDF,
                         const occ::handle<TNaming_NamedShape>& NS,
                         const TopoDS_Shape&                    S)
{

  if (NS.IsNull())
    return false;
  TopoDS_Shape Res = MDF.CurrentShape(NS); // last modification of NS taken into account Valid map
  if (S.IsNull() || Res.IsNull())
    return false;
#ifdef OCCT_DEBUG_CC
  Write(S, "TSol_S.brep");
  Write(Res, "TSol_Res.brep");
#endif

  if ((S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_EDGE
       || S.ShapeType() == TopAbs_VERTEX)
      && Res.ShapeType() != TopAbs_COMPOUND)
  {
    return (Res.IsSame(S));
  }
  else if (S.ShapeType() == TopAbs_SOLID || S.ShapeType() == TopAbs_COMPSOLID)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMS;
    TopExp_Explorer                                        exp;
    for (exp.Init(S, TopAbs_FACE); exp.More(); exp.Next())
    {
      aMS.Add(exp.Current());
    }
    // clang-format off
    for (exp.Init(Res,TopAbs_FACE) ; exp.More(); exp.Next()) { //content of MS and Res should be the same
      // clang-format on
      if (aMS.Contains(exp.Current()))
      {
        aMS.Remove(exp.Current());
      }
      else
        return false;
    }
    return aMS.IsEmpty();
  }
  else
  {

    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>           MS;
    bool                                                             isHom = FillSMap(S, MS);
    TopAbs_ShapeEnum                                                 aType(TopAbs_SHAPE);
    NCollection_Map<int>                                             aView;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itm(MS);
    for (; itm.More(); itm.Next())
    {
      aType = itm.Key().ShapeType();
      if (isHom)
        break;
      else
        aView.Add(itm.Key().ShapeType());
    }

    if (MS.Contains(Res))
    {
      MS.Remove(Res);
      if (MS.IsEmpty())
        return true;
    }
    if (Res.ShapeType() == TopAbs_SOLID || Res.ShapeType() == TopAbs_COMPSOLID
        || Res.ShapeType() == TopAbs_COMPOUND)
    {
      TopExp_Explorer exp;
      if (isHom)
        for (exp.Init(Res, aType); exp.More(); exp.Next())
        {
          if (MS.Contains(exp.Current()))
          {
            MS.Remove(exp.Current());
          }
        }
      else
      {
        NCollection_Map<int>::Iterator aMapIter(aView);
        for (; aMapIter.More(); aMapIter.Next())
        {
          TopAbs_ShapeEnum aCurType = (TopAbs_ShapeEnum)aMapIter.Key();
          for (exp.Init(Res, aCurType); exp.More(); exp.Next())
          {
            if (MS.Contains(exp.Current()))
            {
              MS.Remove(exp.Current());
            }
          }
        }
      }
    }
    else
    {
      if (S.IsSame(Res))
        return true;
      TopoDS_Iterator it(Res);
      for (; it.More(); it.Next())
      { // content of MS and Res should be the same
        if (MS.Contains(it.Value()))
        {
          MS.Remove(it.Value());
        }
        else
          return false;
      }
    }
    return MS.IsEmpty();
  }
}

//=================================================================================================

static void FindNewShapeInFather(const occ::handle<TNaming_NamedShape>& NS, TopoDS_Shape& SC)
{
  const TDF_Label& Father = NS->Label().Father();
  TNaming_Iterator itLab(Father);
  if (itLab.More())
    SC = itLab.NewShape();
}

//=================================================================================================

static occ::handle<TNaming_NamedShape> NextModif(const occ::handle<TNaming_NamedShape>& NS)
{
  occ::handle<TNaming_NamedShape> Next;
  if (!NS.IsNull())
  {
    TNaming_NewShapeIterator it(NS);
    if (it.More() && it.IsModification())
      Next = it.NamedShape();
  }
  return Next;
}

//=======================================================================
// C1 - cand shape of the father, C2 - shape of rebuild child Naming attr.
//      (to be Compound of elementary shapes)
//=======================================================================
static bool IsContSame(const TopoDS_Shape& C1, const TopoDS_Shape& C2)
{
  bool aRes(false);
  if (!C1.IsNull() && !C2.IsNull())
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMap;
    if (FillSMap(C1, aMap))
    {
      aRes = true;
      TopoDS_Iterator it(C2);
      for (; it.More(); it.Next())
      {
        if (!aMap.Contains(it.Value()))
        {
          aRes = false;
          break;
        }
      }
    }
  }
  return aRes;
}

//=======================================================================
// Identifies the case when Filter haven't sense because of multiplicity
//=======================================================================
static bool IsMultipleCase(
  const TopoDS_Shape&                                           S,
  const TopoDS_Shape&                                           Context,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& Neighbourgs)
{

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                         aDM;
  NCollection_Map<TopoDS_Shape>                          aM;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aNbs;
  aNbs.Assign(Neighbourgs);
  aNbs.Add(S);
  NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>> aDMM;
  TopExp::MapShapesAndAncestors(Context, TopAbs_EDGE, TopAbs_FACE, aDM);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(aNbs);
  for (; it.More(); it.Next())
  {
    if (aDM.Contains(it.Key()))
    {
      NCollection_Map<TopoDS_Shape>            aMS;
      const NCollection_List<TopoDS_Shape>&    aL = aDM.FindFromKey(it.Key());
      NCollection_List<TopoDS_Shape>::Iterator lit(aL);
      for (; lit.More(); lit.Next())
      {
        aM.Add(lit.Value());
        aMS.Add(lit.Value());
      }
      if (aMS.Extent())
        aDMM.Bind(it.Key(), aMS);
    }
    else
    {
#ifdef OCCT_DEBUG
      std::cout << "Key is not BOUND!" << std::endl;
#endif
      return false;
    }
  }

  bool                                    isCommon(true);
  NCollection_Map<TopoDS_Shape>::Iterator itm(aM);
  for (; itm.More(); itm.Next())
  {
    isCommon = true; // statement: this shape (itm.Key()) is common (to be checked below)
    NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>>::Iterator itdm(aDMM);
    for (; itdm.More(); itdm.Next())
    {
      const NCollection_Map<TopoDS_Shape>& aMap = itdm.Value();
      if (!aMap.Contains(itm.Key()))
      {
        isCommon = false;
        break;
      }
    }
    if (isCommon)
      break; // common single face found
  }

  // number of unique faces (to have single solution)
  // should be at least no less than (Nb of Neighbourgs) +1
  return isCommon && aM.Extent() < aNbs.Extent();
}

//=======================================================================
// function : Filter
// purpose  : sets the name with type "FILTERBYNEIGHBOURGS" and returns true,
//         : if it was correctly done.
//         :
//         :
//=======================================================================

static bool Filter(const TDF_Label&                 F,
                   TNaming_Scope&                   MDF,
                   const TopoDS_Shape&              S,
                   const TopoDS_Shape&              Context,
                   TNaming_Localizer&               Localizer,
                   occ::handle<TNaming_NamedShape>& NS,
                   const int                        Lev)
{
  // 1. Localizer.FindNeighbourg("Context", "S", "Neighbourg") - looks for neighbourgs of  "S" with
  // the same shape type in the "Context"
  //    shape and adds them to the "Neighbourg" map;
  // 2. If "Neighbourg" map is empty, tries to do the same with the new context shape: shape from
  // the father label of the "S" named shape;
  // 3. If "Neighbourg" map is still empty, returns false;
  // 4. Adds to the new child of label "L" new TNaming_Naming attribute with the next properties
  // name: shape type is shape type of the "S",
  //     type is TNaming_FILTERBYNEIGHBOURGS, stop shape ( "Until" ) is "Context" of corresponding
  //     NamedShape; first argument of name is "NS" NamedShape.
  // 5. Adds to the Name all shapes from "Neighbourg" map: build it with the BuildName( new
  // generated TNaming_Naming attribute, MDF,
  //    argument shape from "Neighbourg", "Context", NextModif( "Until" ), true ) method.
  // 6. Creates resulting NamedShape with the "Regenerate" ( it just calls TName::Solve method )
  // method from TNaming_Naming class.
  // 7. If Compare( result NamedShape, MDF, stop, "S" ) returns true, "Filter" method returns true,
  // else returns false.

  //------------------------------
  // Construction des voisins ==> of neighbors.
  //------------------------------
  int                                                    aLev(Lev);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Neighbourg;
  Localizer.FindNeighbourg(Context, S, Neighbourg);
#ifdef OCCT_DEBUG_NBS
  // DbgTools::DisplayShape(Context, F, Quantity_NOC_GREEN);
  // DbgTools::DisplayShape(S, F, Quantity_NOC_BLUE1);
  Write(Context, "FNBS_Context.brep");
  Write(S, "FNBS_S.brep");
  Write(Neighbourg, "NBS");
#endif
  // mpv : NS and shape must be the same
  bool             isIn = false;
  TNaming_Iterator anIter(NS);
  for (; anIter.More(); anIter.Next())
  {
#ifdef OCCT_DEBUG
    // DbgTools::DisplayShape(anIter.NewShape(), F, Quantity_NOC_RED);
#endif
    if (anIter.NewShape().IsSame(S))
    {
      isIn = true;
      break;
    }
  }
  if (!isIn)
    if (!TNaming_Tool::NamedShape(S, F).IsNull())
      NS = TNaming_Tool::NamedShape(S, F);
  //  if (!TNaming_Tool::NamedShape(S,F).IsNull()) NS = TNaming_Tool::NamedShape(S,F);

  if (Neighbourg.IsEmpty())
  {
    // Recherche du vrai context. (Research of context truth)
    occ::handle<TNaming_NamedShape> GenS = TNaming_Tool::NamedShape(S, NS->Label());
    if (GenS.IsNull())
      return false;
    TDF_Label Father = (GenS->Label()).Father();
    Father.FindAttribute(TNaming_NamedShape::GetID(), GenS);
    TopoDS_Shape GoodContext = TNaming_Tool::GetShape(GenS);
    Localizer.FindNeighbourg(GoodContext, S, Neighbourg);
  }

  if (Neighbourg.IsEmpty())
  {
#ifdef OCCT_DEBUG
    std::cout << "FindNeighbourg: impossible" << std::endl;
#endif
    return false;
  }
  else
  {
#ifdef OCCT_DEBUG_NBS
    Write(Neighbourg, "Neighbourgs");
#endif
    aLev++;
    // std::cout <<"Filter: Lev = " << aLev << std::endl;
  }
  if (aLev > 3)
    return false;
#ifdef ALLOW_CHILD_NBS
  occ::handle<TNaming_Naming> aFNaming;
  TopoDS_Shape                aFatherCandSh;
  F.FindAttribute(TNaming_Naming::GetID(), aFNaming);
  if (!aFNaming.IsNull())
  {
    const TNaming_Name& aName = aFNaming->GetName();
    if (aName.Type() == TNaming_FILTERBYNEIGHBOURGS)
    {
      aFatherCandSh = aName.Arguments().First()->Get();
    }
  }
  if (S.ShapeType() == TopAbs_EDGE && aFatherCandSh.IsNull())
  {
    // check the applicability
    if (!NS.IsNull() && !NS->Get().IsNull() && NS->Get().ShapeType() == TopAbs_COMPOUND)
      if (IsMultipleCase(S, Context, Neighbourg))
      {
        // std::cout << "Filter: ==> MultipleCase!" << std::endl;
        NS->Label().FindAttribute(TNaming_Naming::GetID(), aFNaming);
        if (!aFNaming.IsNull())
        {
          TNaming_Name& aName = aFNaming->ChangeName();
          if (aName.Type() == TNaming_INTERSECTION)
          {
            int                                                         ij(1);
            NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator itA(aName.Arguments());
            for (; itA.More(); itA.Next(), ij++)
            {
              const TopoDS_Shape& aFace = TNaming_Tool::CurrentShape(itA.Value());
  #ifdef OCCT_DEBUG_MOD
              Write(aFace, "First_Face.brep");
              std::cout << "Selection TS = " << S.TShape()->This() << std::endl;
  #endif
              int             i(1), indxW(0), indxE(0), nbW(0), nbE(0), indxF(0);
              bool            isFound(false);
              TopoDS_Iterator it(aFace);
              for (; it.More(); it.Next(), i++)
              {
                nbW++;
  #ifdef OCCT_DEBUG_MOD
                Write(it.Value(), "First_Wire.brep");
  #endif
                if (!isFound)
                {
                  int             j(1);
                  TopoDS_Iterator it2(it.Value());
                  for (; it2.More(); it2.Next(), j++)
                  {
                    nbE++;
  #ifdef OCCT_DEBUG_MOD
                    Write(it2.Value(), "First_Wire.brep");
                    std::cout << "Edge TS = " << it2.Value().TShape()->This() << std::endl;
  #endif
                    if (S.IsEqual(it2.Value()))
                    {
                      indxE   = j;
                      indxW   = i;
                      indxF   = ij;
                      isFound = true;
                    }
                  }
                }
              }
              if (isFound)
              {
                int Index = indxE & 0x000000FF;
                Index     = Index | (nbE << 8);
                Index     = Index | (indxW << 16);
                Index     = Index | (nbW << 20);
                Index     = Index | (indxF << 24);
                aName.Index(Index);
                //------------------------------
                // Compute the TNaming_NamedShape
                //------------------------------
                aFNaming->Regenerate(MDF.ChangeValid());
                aFNaming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
                occ::handle<TNaming_NamedShape> Until =
                  TNaming_Tool::NamedShape(Context, NS->Label());
                occ::handle<TNaming_NamedShape> Stop = NextModif(Until);
                if (Compare(NS, MDF, Stop, S))
                  return true;
                break;
              }
            }
          }
        }
        return false;
      }
  }
#endif

  //-----------------------------------------------------
  // Construction function de naming. et insertion sous F
  //-----------------------------------------------------
  occ::handle<TNaming_Naming> NF = TNaming_Naming::Insert(F);

  occ::handle<TNaming_NamedShape> Until   = TNaming_Tool::NamedShape(Context, NS->Label());
  occ::handle<TNaming_NamedShape> Stop    = NextModif(Until);
  TNaming_Name&                   theName = NF->ChangeName();
  theName.ShapeType(S.ShapeType());
  theName.Shape(S);
  theName.Orientation(S.Orientation());
  theName.Type(TNaming_FILTERBYNEIGHBOURGS);
  theName.Append(NS);
  theName.StopNamedShape(Until);
#ifdef OCCT_DEBUG_NBS
  std::cout << "FilterByNBS: ";
  Print_Entry(NF->Label());
  std::cout << "AppendNS = ";
  Print_Entry(NS->Label());
#endif
  //---------------------
  // Naming des voisins.
  //---------------------

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itN(Neighbourg);
  for (; itN.More(); itN.Next())
  {
#ifdef ALLOW_CHILD_NBS
    const TopoDS_Shape&             aS  = itN.Key();
    occ::handle<TNaming_NamedShape> aNS = BuildName(NF->Label(), MDF, aS, Context, Stop, true);
  #ifdef OCCT_DEBUG_NBS
    const TopoDS_Shape& aS2 = aNS->Get();
    if (!aS.IsNull())
      std::cout << "Shape arg type = " << aS.ShapeType() << " TSH = " << aS.TShape()->This()
                << std::endl;
    if (!aS2.IsNull())
    {
      std::cout << "Build shape type = " << aS2.ShapeType() << " TSH = " << aS2.TShape()->This()
                << std::endl;
      Write(aS2, "NBS_BuildShape.brep");
    }
    if (aNS.IsNull())
    {
      std::cout << "AppendNS = ";
      Print_Entry(aNS->Label());
    }
  #endif

    const TopoDS_Shape aSNS = aNS->Get(); // allow child level
    bool               allowChild(true);
    if (!aSNS.IsNull() && aSNS.ShapeType() == TopAbs_COMPOUND && !aFatherCandSh.IsNull())
      allowChild = !IsContSame(aFatherCandSh, aSNS);
    if (allowChild && !aSNS.IsNull() && aS.ShapeType() != aSNS.ShapeType()
        && aSNS.ShapeType() == TopAbs_COMPOUND)
    { // aLev < 3
  #ifdef OCCT_DEBUG_NBS
      std::cout << "Father label = ";
      Print_Entry(aNS->Label().Father());
      Write(aS, "SelectionS.brep");
      Write(aSNS, "SelectionSNS.brep");
  #endif
      occ::handle<TNaming_Naming> aNaming;
      bool                        StandardFilter(true);
      aNS->FindAttribute(TNaming_Naming::GetID(), aNaming);
      if (!aNaming.IsNull())
      {
        const TNaming_Name& aName = aNaming->GetName();
        if (aName.Type() == TNaming_GENERATION)
          StandardFilter = false;
        if (StandardFilter)
          if (!Compare(aNS, MDF, Stop, aS))
          {
            TNaming_Localizer aLocalizer;
            Filter(NF->Label(), MDF, aS, Context, aLocalizer, aNS, aLev);
          }
      }
    }
    theName.Append(aNS);
#else
    theName.Append(BuildName(NF->Label(), MDF, itN.Key(), Context, Stop, 1));
#endif
  }
  //------------------------------
  // Compute the TNaming_NamedShape
  //------------------------------
  NF->Regenerate(MDF.ChangeValid());
  NF->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);

  //-----------------
  // Check du filtre.
  //-----------------
  return Compare(NS, MDF, Stop, S);
}

//=======================================================================
// function : BuildNameInNS
// purpose  : Calls BuildName method, but with new context and new stop shape.
//         : Context is searched at the father label of the "Context" label :
//         : old shapes from the NamedShape at the defined father label.
//         : If it's impossible, then "S" set as context.
//         : If "S" is in new context, then stop shape is named shape,
//         : which belongs to the father label of the "Context" named shape.
//         : For example, face (F2) of prism (P) is generated from the edge (E)
//         : of primitive face (F1) of box (B). F2 is named as GENERATION from E.
//         : Naming of E is done with help BuildNameInNS function:
//         : with context B and stop shape P.
//=======================================================================
static occ::handle<TNaming_NamedShape> BuildNameInNS(const TDF_Label&                       F,
                                                     TNaming_Scope&                         MDF,
                                                     const TopoDS_Shape&                    S,
                                                     const occ::handle<TNaming_NamedShape>& Context,
                                                     const occ::handle<TNaming_NamedShape>& Stop,
                                                     const bool Geometry)

{
  // il faut determiner un nouveau context et un nouveau Stop.
  // it is necessary to determine a new context and a new Stop
  TopoDS_Shape                    SC;
  occ::handle<TNaming_NamedShape> NewStop = Stop;

  TNaming_Localizer::FindShapeContext(Context, S, SC);

  if (!SC.IsNull())
  {
    // <Context> is Ident.NamedShapeOfGeneration() ==
    TDF_Label Father = Context->Label().Father();
    Father.FindAttribute(TNaming_NamedShape::GetID(), NewStop);
#ifdef OCCT_DEBUG_INNS
    if (!Stop.IsNull())
    {
      std::cout << " Stop NS : ";
      Print_Entry(Stop->Label());
    }
    if (!NewStop.IsNull())
    {
      std::cout << " NewStop : ";
      Print_Entry(NewStop->Label());
    }
    std::cout << "ContextLabel: ";
    Print_Entry(Context->Label());
    std::cout << "Father      : ";
    Print_Entry(Father);
#endif
  }
#ifdef OCCT_DEBUG_INNS
  if (NewStop.IsNull())
    std::cout << "BuildNameInNS:: NewStop shape is  NULL" << std::endl;
#endif
  return BuildName(F, MDF, S, SC, NewStop, Geometry);
}

//=================================================================================================

static occ::handle<TNaming_NamedShape> BuildName(const TDF_Label&                       F,
                                                 TNaming_Scope&                         MDF,
                                                 const TopoDS_Shape&                    Selection,
                                                 const TopoDS_Shape&                    Context,
                                                 const occ::handle<TNaming_NamedShape>& Stop,
                                                 const bool                             Geom)
{

  // Create an identifier
  bool OnlyOne      = !Geom;
  bool IsGeneration = false;
#ifdef OCCT_DEBUG_MOD
  std::cout << "BuildName: F =>  ";
  Print_Entry(F);
  std::cout << " Selection type = " << Selection.ShapeType()
            << " TS = " << Selection.TShape()->This() << std::endl;
  Write(Selection, "BName_Selection.brep");
  Write(Context, "BName_Context.brep");
#endif
  TNaming_Identifier Ident(F, Selection, Context, OnlyOne);

  occ::handle<TNaming_Naming>     Naming;
  occ::handle<TNaming_NamedShape> NS;

  if (!Ident.IsDone())
  {
    return BuildNS(F, Selection, TNaming_UNKNOWN);
  }
  if (Ident.IsFeature() && Stop.IsNull())
  {
    //-------------
    // Deja Nomme
    //-------------
    if (!OnlyOne)
      return Ident.FeatureArg();
    else
      NS = Ident.FeatureArg();
  }
  else
  {
    //---------------------------------------------
    // Construction de la fonction d identification.
    //---------------------------------------------
    // bool NotOnlyOne = 0;

    Naming = TNaming_Naming::Insert(F);

    TNaming_Name& theName = Naming->ChangeName();
    theName.ShapeType(Selection.ShapeType());
    theName.Shape(Selection);
    theName.Orientation(Selection.Orientation());
    theName.Type(Ident.Type());
#ifdef OCCT_DEBUG_MOD
    std::cout << "BuildName: Inserted Naming Att at ";
    Print_Entry(Naming->Label());
    std::cout << " NameType = " << theName.Type() << std::endl;
#endif
    if (Ident.IsFeature())
    {
      theName.Append(Ident.FeatureArg());
    }
    if (theName.Type() == TNaming_GENERATION)
    {
      theName.Append(Ident.NamedShapeOfGeneration());
      IsGeneration = true;
    }
    if (theName.Type() == TNaming_CONSTSHAPE)
    {
      theName.Index(FindIndex(Ident.FeatureArg(), Selection));
    }
    //------------------------------------
    // Renseignement du NamedShape d arret.
    //------------------------------------
    theName.StopNamedShape(Stop);
#ifdef OCCT_DEBUG_MOD
    if (!Stop.IsNull())
    {
      TCollection_AsciiString Es;
      TDF_Tool::Entry(Stop->Label(), Es);
      std::cout << "StopNS at Label = " << Es << std::endl;
    }
#endif
    //---------------------------------
    // Identification des arguments.
    //---------------------------------

    for (Ident.InitArgs(); Ident.MoreArgs(); Ident.NextArg())
    {
      if (Ident.ArgIsFeature())
      {
        theName.Append(Ident.FeatureArg());
#ifdef OCCT_DEBUG_MOD
        if (!Ident.FeatureArg().IsNull())
        {
          TCollection_AsciiString E;
          TDF_Tool::Entry(Ident.FeatureArg()->Label(), E);
          std::cout << "Added argument NS from Label = " << E << std::endl;
        }
#endif
      }
      else
      {
#ifdef OCCT_DEBUG_MOD
        std::cout << "BuildName: NameType = " << theName.Type() << " NS ";
        Print_Entry(Naming->Label());
        std::cout << "Ident.ShapeArg() type = " << Ident.ShapeArg().ShapeType()
                  << " TS = " << Ident.ShapeArg().TShape()->This() << std::endl;
        Write(Ident.ShapeArg(), "BName_ShapeArg.brep");
#endif
        if (theName.Type() == TNaming_GENERATION)
          theName.Append(BuildNameInNS(Naming->Label(),
                                       MDF,
                                       Ident.ShapeArg(),
                                       Ident.NamedShapeOfGeneration(),
                                       Stop,
                                       Geom));
        else
          theName.Append(BuildName(Naming->Label(), MDF, Ident.ShapeArg(), Context, Stop, Geom));
      }
    }

    //------------------------
    // Reconstruction of Name
    //------------------------
    Naming->Regenerate(MDF.ChangeValid());
#ifdef OCCT_DEBUG_MOD
    TCollection_AsciiString E2;
    TDF_Tool::Entry(Naming->Label(), E2);
    std::cout << "Regenerated Naming Att at Label = " << E2 << std::endl;
#endif
    Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
    if (NS.IsNull())
      return NS;
    if (MDF.WithValid())
      MDF.Valid(NS->Label());
#ifdef OCCT_DEBUG_MOD
    if (!NS.IsNull())
    {
      TCollection_AsciiString E;
      TDF_Tool::Entry(NS->Label(), E);
      std::cout << "Regenerated NS at Label = " << E << std::endl;
    }
#endif
  }

  if (OnlyOne)
  {
    //-------------------------------------------------
    // Filter by neighbors
    // to build the name corresponding to S.
    //-------------------------------------------------

    if (NS.IsNull())
      return NS;

    TNaming_Localizer Localizer;
    TNaming_Iterator  itNS(NS);
    if (itNS.More())
    {
      //----------------
      // Check + Filtre
      //----------------

      bool StandardFilter = !IsGeneration;
      if (IsGeneration)
      {
        if (!CompareInGeneration(NS, Selection))
        {

          TopoDS_Shape                    NewContext;
          occ::handle<TNaming_NamedShape> NewStop;
          FindNewShapeInFather(Ident.NamedShapeOfGeneration(), NewContext);
          Filter(F, MDF, Selection, NewContext, Localizer, NS, 0);
        }
      }
      else if (Ident.Type() == TNaming_MODIFUNTIL
               || (Ident.Type() == TNaming_INTERSECTION
                   && Naming->ChangeName().Arguments().Extent() == 1))
      {
#ifdef OCCT_DEBUG_MOD
        std::cout << "BuildName(CompareInModification): NameType = " << Ident.Type() << " NS ";
        Print_Entry(Ident.Type() == TNaming_MODIFUNTIL
                      ? NS->Label()
                      : Naming->ChangeName().Arguments().First()->Label());
        std::cout << "Selection type = " << Selection.ShapeType()
                  << " TS = " << Selection.TShape()->This() << std::endl;
#endif
        occ::handle<TNaming_NamedShape> NewNS = CompareInModification(
          Ident.Type() == TNaming_MODIFUNTIL ? NS : Naming->ChangeName().Arguments().First(),
          Selection);
        if (!NewNS.IsNull())
        { // there is need to describe name in detail: modification with type 1:n in the same label
          StandardFilter = false;
          if (Ident.IsFeature())
          { // for MODIFUNTIL: change it to the GENERATION
            Naming                = TNaming_Naming::Insert(F);
            TNaming_Name& theName = Naming->ChangeName();
            theName.ShapeType(Selection.ShapeType());
            theName.Shape(Selection);
            theName.Orientation(Selection.Orientation());
            theName.Type(TNaming_GENERATION);
            theName.Append(TNaming_Tool::NamedShape(Selection, F));
            theName.Append(NewNS);
            Naming->Regenerate(MDF.ChangeValid());
            Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
          }
          Filter(F, MDF, Selection, Context, Localizer, NS, 0);
        }
      }
      if (StandardFilter)
        if (!Compare(NS, MDF, Stop, Selection))
        {
          Filter(F, MDF, Selection, Context, Localizer, NS, 0);
        }
    }
  }
  if (MDF.WithValid())
    MDF.Valid(NS->Label());
#ifdef OCCT_DEBUG_MOD
  if (!NS.IsNull())
  {
    TCollection_AsciiString E;
    TDF_Tool::Entry(NS->Label(), E);
    std::cout << "Returned NS from Label = " << E << std::endl;
  }
#endif
  return NS;
}

//=================================================================================================

static void Validate(TNaming_Scope& MDF, TNaming_OldShapeIterator& it)
{
  MDF.Valid(it.Label());
  MDF.ValidChildren(it.Label());

  TNaming_OldShapeIterator it2(it);
  for (; it2.More(); it2.Next())
  {
    Validate(MDF, it2);
  }
}

//=================================================================================================

static void UnValidate(TNaming_Scope& MDF, TNaming_NewShapeIterator& it)
{
  MDF.Unvalid(it.Label());
  MDF.UnvalidChildren(it.Label());

  TNaming_NewShapeIterator it2(it);
  for (; it2.More(); it2.Next())
  {
    UnValidate(MDF, it2);
  }
}

//=======================================================================
// function : BuildScope
// purpose  : adds to the MDF the label of <Context> NamedShape,
//         : its children, all its oldShapes and its children.
//         : unvalidates all newShapes and it's children.
//         : If <Context> is null or next modification has an empty newShape
//         : ( this shape was deleted ), then MDF.WithValid(false)
//         : and nothing is added to the scope.
//=======================================================================

static void BuildScope(TNaming_Scope& MDF, const TopoDS_Shape& Context, const TDF_Label& Acces)
{
  if (Context.IsNull())
  {
    MDF.WithValid(false);
    return;
  }

  //----------------------------------------------------
  // Is context the current state
  //----------------------------------------------------
  occ::handle<TNaming_NamedShape> NS = TNaming_Tool::NamedShape(Context, Acces);
  // clang-format off
  occ::handle<TNaming_NamedShape> Next = NextModif(NS); // if NS has subsequent evolution = MODIFY, return it
  // clang-format on
  if (Next.IsNull())
  {
    MDF.WithValid(false);
    return;
  }
  //-----------------------------
  // a posteriori naming
  //-----------------------------
  MDF.WithValid(true);
  MDF.Valid(NS->Label());
  MDF.ValidChildren(NS->Label());
  TNaming_OldShapeIterator it(Context, Acces);

  for (; it.More(); it.Next())
  {
    Validate(MDF, it);
  }

  TNaming_NewShapeIterator it2(Context, Acces);
  for (; it2.More(); it2.Next())
  {
    UnValidate(MDF, it2);
  }
}

//=======================================================================
// function : HasAncFace
// purpose  : Returns True & <Face> if ancestor face is found
//=======================================================================
static bool HasAncFace(const TopoDS_Shape& Context,
                       const TopoDS_Shape& W,
                       TopoDS_Shape&       Face,
                       bool&               isOuter)
{
  bool hasFace(false);
  if (W.ShapeType() != TopAbs_WIRE)
    return hasFace;
  TopExp_Explorer exp(Context, TopAbs_FACE);
  for (; exp.More(); exp.Next())
  {
    for (TopoDS_Iterator it(exp.Current()); it.More(); it.Next())
    {
      if (it.Value().IsEqual(W))
      { // is the Wire ?
        Face = exp.Current();
        if (!Face.IsNull())
        {
          hasFace = true;
          //	  std::cout << "HasAncFace: TS = " <<theFace.TShape()->This() <<std::endl;
          const TopoDS_Face aFace(TopoDS::Face(Face));
          TopoDS_Wire       anOuterW;
          if (TNaming::OuterWire(aFace, anOuterW))
          {
            isOuter = !anOuterW.IsNull() && anOuterW.IsEqual(W);
          }
          break;
        }
      }
    }
    if (hasFace)
      break;
  }
  return hasFace;
}

//=================================================================================================

static occ::handle<TNaming_NamedShape> BuildNameWire(const TDF_Label&    F,
                                                     TNaming_Scope&      MDF,
                                                     const TopoDS_Shape& Selection,
                                                     const TopoDS_Shape& Context,
                                                     const occ::handle<TNaming_NamedShape>& Stop,
                                                     const bool                             Geom)
{
  occ::handle<TNaming_NamedShape> NS;
  bool                            found(false);
  occ::handle<TNaming_Naming>     Naming;
  if (!F.FindAttribute(TNaming_Naming::GetID(), Naming))
  {
    Naming = new TNaming_Naming();
    F.AddAttribute(Naming);
    TNaming_Name& theName = Naming->ChangeName();
    theName.ShapeType(Selection.ShapeType());
    theName.Shape(Selection);
    theName.Orientation(Selection.Orientation());
  }

  TNaming_Name& theName = Naming->ChangeName();
  TopoDS_Shape  aFace;
  bool          isOuter(false);
  bool          hasFace = HasAncFace(Context, Selection, aFace, isOuter);
  if (hasFace && Selection.ShapeType() > Context.ShapeType())
  {
    theName.Type(TNaming_WIREIN);
    if (Context.ShapeType() == TopAbs_FACE)
    {
      for (TopoDS_Iterator it(Context); it.More(); it.Next())
      {
        if (it.Value().IsEqual(Selection))
        {
          if (TNaming_Selector::IsIdentified(F, Context, NS, Geom))
          {
            theName.Append(NS);
            found = true;
            break;
          }
        }
      }
      if (found)
      {
        theName.Append(BuildName(Naming->Label(), MDF, aFace, Context, Stop, Geom));
        if (isOuter)
        {
          theName.Index(1);
        }
        else
        {
          theName.Index(-1);
          for (TopExp_Explorer exp(Selection, TopAbs_EDGE); exp.More(); exp.Next())
          {
            if (exp.Current().IsNull())
              continue;
            if (BRep_Tool::Degenerated(TopoDS::Edge(exp.Current())))
              continue;
            theName.Append(
              TNaming_Naming::Name(Naming->Label(), exp.Current(), Context, Geom, true, false));
          }
        }
      }
      else
        return BuildNS(F, Selection, TNaming_UNKNOWN);
    }
    else
    { // context is not Face
      theName.Append(BuildName(Naming->Label(), MDF, aFace, Context, Stop, Geom));
      if (isOuter)
      {
        theName.Index(1);
      }
      else
      {
        for (TopExp_Explorer exp(Selection, TopAbs_EDGE); exp.More(); exp.Next())
        {
          if (exp.Current().IsNull())
            continue;
          if (BRep_Tool::Degenerated(TopoDS::Edge(exp.Current())))
            continue;
          theName.Append(
            TNaming_Naming::Name(Naming->Label(), exp.Current(), Context, Geom, true, false));
        }
      }
    } //
  }
  else
  { // => no Face
    theName.Type(TNaming_UNION);
    for (TopExp_Explorer exp(Selection, TopAbs_EDGE); exp.More(); exp.Next())
    {
      if (exp.Current().IsNull())
        continue;
      if (BRep_Tool::Degenerated(TopoDS::Edge(exp.Current())))
        continue;
      theName.Append(BuildName(Naming->Label(), MDF, exp.Current(), Context, Stop, Geom));
    }
  }
  // Naming->GetName().Solve(Naming->Label(),MDF.GetValid());
  Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
  return NS;
}

//=======================================================================
static bool IsOneIn(const TopoDS_Shape& S, const TopoDS_Shape& Context)
{
  bool found(false);
  if (S.IsNull() || Context.IsNull())
    return found;
  for (TopExp_Explorer exp(Context, S.ShapeType()); exp.More(); exp.Next())
  {
    if (exp.Current().IsEqual(S))
    {
      found = true;
      break;
    }
  }
  return found;
}

//=======================================================================
static bool IsAllIn(const TopoDS_Shape& S, const TopoDS_Shape& Context)
{
#ifdef OCCT_DEBUG_CC
  Write(S, "IsAllIn_Sel.brep");
#endif
  bool found(false);
  if (S.IsNull() || Context.IsNull())
    return found;
  int num1(0), num2(0);
  for (TopoDS_Iterator it(S); it.More(); it.Next(), num1++)
  {
#ifdef OCCT_DEBUG_CC
    std::cout << "S sub-shape type = " << it.Value().ShapeType() << std::endl;
    Write(it.Value(), "Sel_ItValue.brep");
#endif
    if (it.Value().ShapeType() != TopAbs_COMPOUND)
      for (TopExp_Explorer exp(Context, it.Value().ShapeType()); exp.More(); exp.Next())
      {
#ifdef OCCT_DEBUG_CC
        std::cout << "Context sub-shape type = " << exp.Current().ShapeType() << std::endl;
        Write(exp.Current(), "Contex_Curnt.brep");
#endif
        if (exp.Current().IsEqual(it.Value()))
        {
          num2++;
          break;
        }
      }
    else
    {
      bool isAll = IsAllIn(it.Value(), Context);
      if (isAll)
        num2++;
    }
  }
  if (num1 == num2)
    found = true;
#ifdef OCCT_DEBUG_CC
  else
    std::cout << "Compound case : selected num1 = " << num1 << " context contains num2 = " << num2
              << std::endl;
#endif
  return found;
}

//=================================================================================================

static int RepeatabilityInContext(const TopoDS_Shape& Selection, const TopoDS_Shape& Context)
{
  int aNum(0);
  if (!Context.IsNull() && !Selection.IsNull())
  {
    //    Write(Selection, "Repeat_Selection.brep");
    //    Write(Context, "Repeat_Context.brep");
    if (Context.ShapeType() < Selection.ShapeType())
    {
      if (Selection.ShapeType() != TopAbs_SHELL)
      {
        for (TopExp_Explorer exp(Context, Selection.ShapeType()); exp.More(); exp.Next())
        {
          if (exp.Current().IsSame(Selection))
            aNum++;
        }
      }
    }
    else if (Selection.ShapeType() == TopAbs_COMPOUND)
    {
      TopoDS_Iterator it(Selection);
      for (; it.More(); it.Next())
      {
        int n(0);
        for (TopExp_Explorer exp(Context, it.Value().ShapeType()); exp.More(); exp.Next())
        {
          if (exp.Current().IsSame(it.Value()))
          {
            n++;
          }
        }
        if (n > aNum)
          aNum = n;
      }
    }
  }
#ifdef OCCT_DEBUG_OR
  std::cout << "RepeatabilityInContext: = " << aNum << std::endl;
#endif
  return aNum;
}

//=======================================================================
// function : HasAncSolid
// purpose  : Returns true if Sh has ancestor solid in this context
//=======================================================================
static bool HasAncSolid(const TopoDS_Shape& Context,
                        const TopoDS_Shape& Sh,
                        TopoDS_Shape&       Solid,
                        bool&               isOuter)
{
  bool hasSolid(false);
  if (Sh.ShapeType() != TopAbs_SHELL)
    return hasSolid;
  TopExp_Explorer exp(Context, TopAbs_SOLID);
  for (; exp.More(); exp.Next())
  {
    for (TopoDS_Iterator it(exp.Current()); it.More(); it.Next())
    {
      if (it.Value().IsEqual(Sh))
      { // is the Solid ?
        Solid = exp.Current();
        if (!Solid.IsNull())
        {
          hasSolid = true;
          TopoDS_Shell anOuterShell;
          if (TNaming::OuterShell(TopoDS::Solid(Solid), anOuterShell))
          {
#ifdef OCCT_DEBUG_TSOL
            Write(anOuterShell, "OuterShell.brep");
#endif
            isOuter = !anOuterShell.IsNull() && anOuterShell.IsEqual(Sh);
          }
          break;
        }
      }
    }
    if (hasSolid)
      break;
  }
  return hasSolid;
}

//=================================================================================================

static occ::handle<TNaming_NamedShape> BuildNameShell(const TDF_Label&    F,
                                                      TNaming_Scope&      MDF,
                                                      const TopoDS_Shape& Selection,
                                                      const TopoDS_Shape& Context,
                                                      const occ::handle<TNaming_NamedShape>& Stop,
                                                      const bool                             Geom)
{
  occ::handle<TNaming_NamedShape> NS;
  bool                            found(false);
  occ::handle<TNaming_Naming>     Naming;
  if (!F.FindAttribute(TNaming_Naming::GetID(), Naming))
  {
    Naming = new TNaming_Naming();
    F.AddAttribute(Naming);
    TNaming_Name& theName = Naming->ChangeName();
    theName.ShapeType(Selection.ShapeType());
    theName.Shape(Selection);
    theName.Orientation(Selection.Orientation());
  }

  TNaming_Name& theName = Naming->ChangeName();
  TopoDS_Shape  aSolid;
  bool          isOuter(false);
  bool          hasSolid = HasAncSolid(Context, Selection, aSolid, isOuter);
  if (hasSolid && Selection.ShapeType() > Context.ShapeType())
  {
    theName.Type(TNaming_SHELLIN); // SHELLIN

    if (Context.ShapeType() == TopAbs_SOLID)
    {
      for (TopoDS_Iterator it(Context); it.More(); it.Next())
      {
#ifdef OCCT_DEBUG_TSOL
        Write(it.Value(), "Shell_inSo.brep");
#endif
        if (it.Value().IsEqual(Selection))
        {
          found = true;
          break;
        }
      }
      if (found)
      {
        // solid => aSolid which is also a context
        occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(Context, F);
        if (!aNS.IsNull())
          theName.ContextLabel(aNS->Label());
        theName.Append(aNS);
        if (isOuter)
        {
          theName.Index(1);
        }
        else
        { // not OuterShell
          theName.Index(-1);
          for (TopExp_Explorer exp(Selection, TopAbs_FACE); exp.More(); exp.Next())
          {
            if (exp.Current().IsNull())
              continue;
            theName.Append(BuildName(Naming->Label(), MDF, exp.Current(), Context, Stop, Geom));
          }
        }
      }
      else
        return BuildNS(F, Selection, TNaming_UNKNOWN);
    }
    else
    {
      // context is not SOLID
      // theName.Append(BuildName (Naming->Label(),MDF,aSolid,Context,Stop,Geom));//###########
      if (isOuter)
      {
#ifdef OCCT_DEBUG_TSOL
        Write(aSolid, "foundSolid.brep");
#endif
        theName.Index(1);
        occ::handle<TNaming_Naming> NamingSo  = TNaming_Naming::Insert(F);
        TNaming_Name&               theNameSo = NamingSo->ChangeName();
        theNameSo.ShapeType(aSolid.ShapeType());
        theNameSo.Shape(aSolid);
        theNameSo.Type(TNaming_UNION);
        occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(Context, F);
        if (!aNS.IsNull())
          theNameSo.ContextLabel(aNS->Label());
        for (TopExp_Explorer exp(aSolid, TopAbs_FACE); exp.More(); exp.Next())
          theNameSo.Append(BuildName(NamingSo->Label(), MDF, exp.Current(), Context, Stop, Geom));
        NamingSo->GetName().Solve(NamingSo->Label(), MDF.GetValid());
        aNS.Nullify();
        NamingSo->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS);
        theName.Append(aNS);
      }
      else
      {
        theName.Index(-1);
        // - name Solid: theName.Append(BuildName (Naming->Label(),MDF, aSolid,Context,Stop,Geom));
        occ::handle<TNaming_Naming> NamingSo  = TNaming_Naming::Insert(F);
        TNaming_Name&               theNameSo = NamingSo->ChangeName();
        theNameSo.ShapeType(aSolid.ShapeType());
        theNameSo.Shape(aSolid);
        theNameSo.Type(TNaming_UNION);
        occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(Context, F);
        if (!aNS.IsNull())
          theNameSo.ContextLabel(aNS->Label());
        for (TopExp_Explorer exp(aSolid, TopAbs_FACE); exp.More(); exp.Next())
          theNameSo.Append(BuildName(NamingSo->Label(), MDF, exp.Current(), Context, Stop, Geom));
        NamingSo->GetName().Solve(NamingSo->Label(), MDF.GetValid());
        aNS.Nullify();
        NamingSo->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS);
        theName.Append(aNS);

        for (TopExp_Explorer exp(Selection, TopAbs_FACE); exp.More(); exp.Next())
        {
          if (exp.Current().IsNull())
            continue;
          theName.Append(BuildName(Naming->Label(), MDF, exp.Current(), Context, Stop, Geom));
        }
      }
    } //
  }
  else
  { // => no Solid
    theName.Type(TNaming_UNION);
    occ::handle<TNaming_NamedShape> aNS = TNaming_Tool::NamedShape(Context, F);
    if (!aNS.IsNull())
      theName.ContextLabel(aNS->Label());
    for (TopExp_Explorer exp(Selection, TopAbs_FACE); exp.More(); exp.Next())
    {
      if (exp.Current().IsNull())
        continue;
      theName.Append(BuildName(Naming->Label(), MDF, exp.Current(), Context, Stop, Geom));
    }
  }
  // Naming->GetName().Solve(Naming->Label(),MDF.GetValid());
  Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
  return NS;
}

//=================================================================================================

static void BuildAggregationName(const TDF_Label&                       F,
                                 TNaming_Scope&                         MDF,
                                 const TopoDS_Shape&                    S,
                                 const TopoDS_Shape&                    Context,
                                 const occ::handle<TNaming_NamedShape>& Stop,
                                 const bool                             Geom)
{
  const bool                  found2 = IsAllIn(S, Context);
  occ::handle<TNaming_Naming> Naming;
  if (!F.FindAttribute(TNaming_Naming::GetID(), Naming))
  {
    Naming = new TNaming_Naming();
    F.AddAttribute(Naming);
    TNaming_Name& theName = Naming->ChangeName();
    theName.ShapeType(S.ShapeType());
    theName.Shape(S);
    theName.Orientation(S.Orientation());
  }
#ifdef OCCT_DEBUG_CC
  std::cout << "BuildAggregationName ==> ";
  Print_Entry(Naming->Label());
#endif
  TNaming_Name& theName = Naming->ChangeName();
  for (TopoDS_Iterator itc(S); itc.More(); itc.Next())
  {
    const TopoDS_Shape& aS = itc.Value();
    if ((aS.ShapeType() == TopAbs_SOLID && !TNaming_Tool::NamedShape(aS, Naming->Label()).IsNull())
        || aS.ShapeType() == TopAbs_FACE || aS.ShapeType() == TopAbs_EDGE
        || aS.ShapeType() == TopAbs_VERTEX)
    {
      theName.Append(BuildName(F, MDF, aS, Context, Stop, Geom));
    }
    else
    { // ==> union of union || union of wires
      TopAbs_ShapeEnum atomTyp;
      switch (aS.ShapeType())
      {
        case TopAbs_SOLID:
        case TopAbs_SHELL:
          atomTyp = TopAbs_FACE;
          break;
        case TopAbs_WIRE:
          atomTyp = TopAbs_EDGE;
          break;
        default:
          atomTyp = TopAbs_SHAPE;
      }

      occ::handle<TNaming_NamedShape> aNS;
      occ::handle<TNaming_Naming>     aNaming = TNaming_Naming::Insert(F);
      TNaming_Name&                   aName   = aNaming->ChangeName();
      aName.ShapeType(aS.ShapeType());
      aName.Shape(aS);
      theName.Orientation(aS.Orientation());
      aName.Type(TNaming_UNION);

      if (atomTyp != TopAbs_SHAPE)
      {
        if (aS.ShapeType() == TopAbs_WIRE)
        {
          aNS = BuildNameWire(aNaming->Label(), MDF, aS, Context, Stop, Geom);
        }
        else if (aS.ShapeType() == TopAbs_SHELL)
          aNS = BuildNameShell(aNaming->Label(), MDF, aS, Context, Stop, Geom);
        else
        {
          for (TopExp_Explorer exp(aS, atomTyp); exp.More(); exp.Next())
          {
            aName.Append(BuildName(aNaming->Label(), MDF, exp.Current(), Context, Stop, Geom));
          }
        }
      }
      else
      {
#ifdef OCCT_DEBUG_CC
        std::cout << "atomic type is NOT defined ... ==> Aggregation" << std::endl;
#endif
        BuildAggregationName(aNaming->Label(), MDF, aS, Context, Stop, Geom);
      }
      if (found2)
      {
        aNS = TNaming_Tool::NamedShape(Context, F);
        if (!aNS.IsNull())
          aNaming->ChangeName().ContextLabel(aNS->Label());
      }

      aNaming->GetName().Solve(aNaming->Label(), MDF.GetValid());
      if (aNaming->Label().FindAttribute(TNaming_NamedShape::GetID(), aNS))
        if (!Geom && TestSolution(MDF, aNS, aS))
        {
          theName.Append(aNS);
        }
    }
  }
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_Naming::Name(const TDF_Label&    F,
                                                     const TopoDS_Shape& S,
                                                     const TopoDS_Shape& Context,
                                                     const bool          Geom,
                                                     const bool          KeepOrientation,
                                                     const bool          BNProblem)

{
  occ::handle<TNaming_NamedShape> aNamedShape;
  if (KeepOrientation)
  {
#ifdef OCCT_DEBUG_INNS
    std::cout << "KeepOR = 1: ";
    Print_Entry(F);
#endif
    int aNum = RepeatabilityInContext(S, Context);

    bool aBNproblem = (BNProblem) ? (aNum /*== 1*/ && S != Context) : false;

    if (aNum > 1 || aBNproblem)
    {
      TopoDS_Shape                                   UC = TNaming::FindUniqueContext(S, Context);
      occ::handle<NCollection_HArray1<TopoDS_Shape>> Arr;
      if (UC.IsNull() && S.ShapeType() == TopAbs_COMPOUND)
      {
        UC = TNaming::FindUniqueContextSet(S, Context, Arr);
#ifdef OCCT_DEBUG_CC
        Write(UC, "UniqueContextSet.brep");
        Write(S, "InitialSelection.brep");
        if (S.ShapeType() == TopAbs_COMPOUND)
        {
          TCollection_AsciiString aNam("S_");
          TopoDS_Iterator         it(S);
          for (int i = 1; it.More(); it.Next(), i++)
          {
            TCollection_AsciiString aName = aNam + i + ".brep";
            Write(it.Value(), aName.ToCString());
          }
        }
#endif
      }
      if (!UC.IsNull())
      {
        occ::handle<TNaming_Naming> Naming  = TNaming_Naming::Insert(F);
        TNaming_Name&               theName = Naming->ChangeName();
        theName.ShapeType(S.ShapeType());
        theName.Shape(S);
        theName.Type(TNaming_ORIENTATION);
        theName.Orientation(S.Orientation());

        if (!TNaming_Selector::IsIdentified(F, S, aNamedShape, Geom))
          aNamedShape = TNaming_Naming::Name(Naming->Label(), S, Context, Geom, false);
        theName.Append(aNamedShape);
#ifdef MDTV_OR
        std::cout << " Sel Label ==> ";
        Print_Entry(NS->Label());
#endif
        // szy 21.10.2009
        if (S.ShapeType() == TopAbs_EDGE && UC.ShapeType() == TopAbs_FACE)
        {
          if (RepeatabilityInContext(S, UC) == 2)
          { // sim. edge
            TopoDS_Iterator itw(UC);
            for (; itw.More(); itw.Next())
            {
              bool            found(false);
              TopoDS_Iterator it(itw.Value());
              for (int i = 1; it.More(); it.Next(), i++)
              {
                if (it.Value().IsEqual(S))
                {
                  // clang-format off
		  theName.Index(i);//We use this field to save a Seam Shape Index; Before this field was used for GENERATED only
                  // clang-format on
                  found = true;
#ifdef MDTV_OR
                  std::cout << "ORDER = " << i << std::endl;
#endif
                  break;
                }
              }
              if (found)
                break;
            }
          }
        }
        //
        if (S.ShapeType() == TopAbs_COMPOUND && Arr->Length() > 1)
        {
          // N arguments: to be optimized to avoid duplication of the same Context shape
          for (int i = Arr->Lower(); i <= Arr->Upper(); i++)
          {
            aNamedShape =
              TNaming_Naming::Name(Naming->Label(), Arr->Value(i), Context, Geom, true, aBNproblem);
            theName.Append(aNamedShape);
          }
        }
        else
        {
          aNamedShape = TNaming_Naming::Name(Naming->Label(), UC, Context, Geom, true, aBNproblem);
          theName.Append(aNamedShape);
#ifdef MDTV_OR
          std::cout << " Cont Label ==> ";
          Print_Entry(NS->Label());
#endif
        }
        // Naming->Update();
        TNaming_Scope MDF;
        BuildScope(MDF, Context, F);
        Naming->GetName().Solve(Naming->Label(), MDF.GetValid());
        Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), aNamedShape);
        theName.ContextLabel(aNamedShape->Label());
        if (Geom)
          return aNamedShape;
        if (aNamedShape.IsNull())
        {
          std::cout << " %%% WARNING: TNaming_Naming::Name:  FAILED" << std::endl;
          return BuildNS(F, S, TNaming_UNKNOWN);
        }

        if (!Geom && TestSolution(MDF, aNamedShape, S))
          return aNamedShape;
        std::cout << " %%% WARNING: TNaming_Naming::Name:  FAILED" << std::endl;

        // Naming n is  unsatisfactory
        return BuildNS(F, S, TNaming_UNKNOWN);
      }
    }
    else if (TNaming_Selector::IsIdentified(F, S, aNamedShape, Geom))
      return aNamedShape;
  }

  //------------------------------------------------------------
  // Construction du MDF tel que <Context> soit le dernier etat
  // valide,
  // Ceci pour les localisation a posteriori par exemple.
  //------------------------------------------------------------

  TNaming_Scope MDF;
  BuildScope(MDF, Context, F);
  occ::handle<TNaming_NamedShape> Stop;

  if ((S.ShapeType() == TopAbs_SOLID && !TNaming_Tool::NamedShape(S, F).IsNull()) ||

      S.ShapeType() == TopAbs_FACE || S.ShapeType() == TopAbs_EDGE
      || S.ShapeType() == TopAbs_VERTEX)
  {
    //---------------------------------------
    // Localisation de S comme element simple.
    //---------------------------------------
    occ::handle<TNaming_NamedShape> NS = BuildName(F, MDF, S, Context, Stop, Geom);
    if (Geom)
      return NS;
    if (!Geom && TestSolution(MDF, NS, S))
      return NS;
  }
  else
  {
    //----------------------------------------------------
    // Localisation de S comme ensemble d elements simples.
    //-----------------------------------------------------
    occ::handle<TNaming_NamedShape> NS;
    occ::handle<TNaming_Naming>     Naming  = TNaming_Naming::Insert(F);
    TNaming_Name&                   theName = Naming->ChangeName();

    theName.ShapeType(S.ShapeType()); // modified by vro 05.09.00
    theName.Shape(S);
    theName.Orientation(S.Orientation());
    if (S.ShapeType() != TopAbs_WIRE)
      theName.Type(TNaming_UNION);

    TopAbs_ShapeEnum atomType;
    switch (S.ShapeType())
    {
      case TopAbs_COMPSOLID:
      case TopAbs_SOLID:
      case TopAbs_SHELL:
        atomType = TopAbs_FACE;
        break;
      case TopAbs_WIRE:
        atomType = TopAbs_EDGE;
        break;
      default:
        atomType = TopAbs_SHAPE;
    }
    bool found(false);
    if (!Context.IsNull())
    {
      if (Context.ShapeType() < S.ShapeType())
        found = IsOneIn(S, Context);
      if (found)
      {
        NS = TNaming_Tool::NamedShape(Context, F);
        if (!NS.IsNull())
          theName.ContextLabel(NS->Label());
      }
    }
    if (atomType == TopAbs_SHAPE)
    {
      if (S.ShapeType() == TopAbs_COMPOUND)
      {
        BuildAggregationName(Naming->Label(), MDF, S, Context, Stop, Geom);
      }
      else
      {
        for (TopoDS_Iterator it(S); it.More(); it.Next())
        {
          theName.Append(BuildName(Naming->Label(), MDF, it.Value(), Context, Stop, Geom));
        }
      }
    }
    else
    {
      if (S.ShapeType() == TopAbs_WIRE)
        NS = BuildNameWire(Naming->Label(), MDF, S, Context, Stop, Geom);
      else if (S.ShapeType() == TopAbs_SHELL)
      {
        NS = BuildNameShell(Naming->Label(), MDF, S, Context, Stop, Geom);
      }
      else
      {
        theName.Type(TNaming_UNION);
        for (TopExp_Explorer exp(S, atomType); exp.More(); exp.Next())
        {
          theName.Append(BuildName(Naming->Label(), MDF, exp.Current(), Context, Stop, Geom));
        }
      }
    }

    // Naming->Update();
    Naming->GetName().Solve(Naming->Label(), MDF.GetValid());
    Naming->Label().FindAttribute(TNaming_NamedShape::GetID(), NS);
    if (Geom)
      return NS;

    if (NS.IsNull())
      return BuildNS(F, S, TNaming_UNKNOWN);

    if (!Geom && TestSolution(MDF, NS, S))
      return NS;
  }

  std::cout << " %%% WARNING: TNaming_Naming::Name:  FAILED" << std::endl;

  // Naming n is not satisfactory
  return BuildNS(F, S, TNaming_UNKNOWN);
}

//=================================================================================================

TNaming_Naming::TNaming_Naming() = default;

//=================================================================================================

const Standard_GUID& TNaming_Naming::ID() const
{
  return GetID();
}

//=================================================================================================

bool TNaming_Naming::IsDefined() const
{
  return (myName.Type() != TNaming_UNKNOWN);
}

//=================================================================================================

const TNaming_Name& TNaming_Naming::GetName() const
{
  return myName;
}

//=================================================================================================

TNaming_Name& TNaming_Naming::ChangeName()
{
  return myName;
}

//=================================================================================================

bool TNaming_Naming::Regenerate(NCollection_Map<TDF_Label>& MDF)

{
  return myName.Solve(Label(), MDF);
}

//=================================================================================================

occ::handle<TDF_Attribute> TNaming_Naming::NewEmpty() const
{
  return new TNaming_Naming();
}

//=================================================================================================

void TNaming_Naming::Restore(const occ::handle<TDF_Attribute>& other)
{
  occ::handle<TNaming_Naming> OtherNaming = occ::down_cast<TNaming_Naming>(other);
  myName                                  = OtherNaming->ChangeName();
}

//=================================================================================================

void TNaming_Naming::Paste(const occ::handle<TDF_Attribute>&       into,
                           const occ::handle<TDF_RelocationTable>& RT) const
{
  occ::handle<TNaming_Naming> NewNaming = occ::down_cast<TNaming_Naming>(into);
  myName.Paste(NewNaming->ChangeName(), RT);
}

//=======================================================================
// function : References
// purpose  : Redefined from TDF_Attribute
//=======================================================================

void TNaming_Naming::References(const occ::handle<TDF_DataSet>& DataSet) const
{
  // Iteration on NamedShape of the name
  NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(myName.Arguments());
  for (; it.More(); it.Next())
    DataSet->AddAttribute(it.Value());
  if (!myName.StopNamedShape().IsNull())
    DataSet->AddAttribute(myName.StopNamedShape());
}

//=================================================================================================

Standard_OStream& TNaming_Naming::Dump(Standard_OStream& anOS) const
{
  anOS << "TNaming_Naming";
  return anOS;
}

//=================================================================================================

void TNaming_Naming::ExtendedDump(
  Standard_OStream& anOS,
  const TDF_IDFilter& /*aFilter*/,
  NCollection_IndexedMap<occ::handle<TDF_Attribute>>& /*aMap*/) const
{
  anOS << "TNaming_Naming ExtendedDump  ";
  // anOS<<"myContext: #" <<aMap.Add(myContext)<<std::endl;
}

//=================================================================================================

void TNaming_Naming::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myName)
}
