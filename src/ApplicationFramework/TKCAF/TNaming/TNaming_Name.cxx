// Created on: 1997-03-21
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

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepTools.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Name.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>
#include <TNaming_NamingTool.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_ShapesSet.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_CompSolid.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Wire.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_HArray2.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>

// mpv modifications 08.04.2002
// end of mpv modifications 08.04.2002
#ifdef OCCT_DEBUG
  #include <TCollection_AsciiString.hxx>
#endif
#ifdef OCCT_DEBUG
  #include <TCollection_AsciiString.hxx>
  #include <TDF_ChildIterator.hxx>

//=======================================================================
void PrintEntry(const TDF_Label& label)
{
  TCollection_AsciiString entry;
  TDF_Tool::Entry(label, entry);
  std::cout << "LabelEntry = " << entry << std::endl;
}

//=======================================================================
void PrintEntries(const NCollection_Map<TDF_Label>& map)
{
  std::cout << "=== Labels Map ===" << std::endl;
  TCollection_AsciiString              entry;
  NCollection_Map<TDF_Label>::Iterator it(map);
  for (; it.More(); it.Next())
  {
    TDF_Tool::Entry(it.Key(), entry);
    std::cout << "LabelEntry = " << entry << std::endl;
  }
}
#endif
//
//====================================================================
static bool ValidArgs(const NCollection_List<occ::handle<TNaming_NamedShape>>& Args)
{
  NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(Args);
  for (; it.More(); it.Next())
  {
    const occ::handle<TNaming_NamedShape>& aNS = it.Value();
    if (aNS.IsNull())
    {
      return false;
    }
    else if (aNS->IsEmpty())
    {
      return false;
    }
    else if (!aNS->IsValid())
    {
      return false;
    }
  }
  return true;
}

//=================================================================================================

TNaming_Name::TNaming_Name()
    : myType(TNaming_UNKNOWN),
      myIndex(-1)
{
}

//=================================================================================================

void TNaming_Name::Type(const TNaming_NameType aType)
{
  myType = aType;
}

//=================================================================================================

void TNaming_Name::ShapeType(const TopAbs_ShapeEnum T)
{
  myShapeType = T;
}

//=================================================================================================

void TNaming_Name::Shape(const TopoDS_Shape& theShape)
{
  myShape = theShape;
}

//=================================================================================================

TopoDS_Shape TNaming_Name::Shape() const
{
  return myShape;
}

//=================================================================================================

void TNaming_Name::Append(const occ::handle<TNaming_NamedShape>& arg)
{
  myArgs.Append(arg);
}

//=================================================================================================

void TNaming_Name::StopNamedShape(const occ::handle<TNaming_NamedShape>& arg)
{
  myStop = arg;
}

//=================================================================================================

void TNaming_Name::Index(const int I)
{
  myIndex = I;
}

//=================================================================================================

TNaming_NameType TNaming_Name::Type() const
{
  return myType;
}

//=================================================================================================

TopAbs_ShapeEnum TNaming_Name::ShapeType() const
{
  return myShapeType;
}

//=================================================================================================

void TNaming_Name::Paste(TNaming_Name& into, const occ::handle<TDF_RelocationTable>& RT) const
{
  into.myType      = myType;
  into.myShapeType = myShapeType;
  into.myShape     = myShape;
  into.myIndex     = myIndex;
  into.myArgs.Clear();
  //  into.myOrientation = myOrientation;
  occ::handle<TNaming_NamedShape> NS;

  for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(myArgs); it.More(); it.Next())
  {
    RT->HasRelocation(it.Value(), NS);
    into.myArgs.Append(NS);
  }
  if (!myStop.IsNull())
  {
    RT->HasRelocation(myStop, NS);
    into.myStop = NS;
  }
  if (!myContextLabel.IsNull())
  {
    RT->HasRelocation(myContextLabel, into.myContextLabel);
  }
}

//=================================================================================================

const NCollection_List<occ::handle<TNaming_NamedShape>>& TNaming_Name::Arguments() const
{
  return myArgs;
}

//=================================================================================================

occ::handle<TNaming_NamedShape> TNaming_Name::StopNamedShape() const
{
  return myStop;
}

//=================================================================================================

int TNaming_Name::Index() const
{
  return myIndex;
}

//=================================================================================================

static TopoDS_Shape MakeShape(
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS)
{
  if (!MS.IsEmpty())
  {
    if (MS.Extent() == 1)
    {
      return MS(1);
    }
    else
    {
      TopoDS_Compound C;
      BRep_Builder    B;
      B.MakeCompound(C);
      for (int anIt = 1; anIt <= MS.Extent(); ++anIt)
      {
        B.Add(C, MS(anIt));
      }
      return C;
    }
  }
  return TopoDS_Shape();
}

//=================================================================================================

static TopoDS_Shape ShapeWithType(const TopoDS_Shape& theShape, const TopAbs_ShapeEnum theType)
{
  if (theShape.IsNull() || theType == TopAbs_SHAPE)
  {
    return theShape;
  }
  int aType = theShape.ShapeType();
  if (aType == theType)
  {
    return theShape;
  }

  NCollection_List<TopoDS_Shape> aShapes;
  if (aType == TopAbs_COMPOUND)
  {
    TopoDS_Iterator anIter(theShape);
    if (anIter.More())
    {
      aType = anIter.Value().ShapeType();
    }
    for (; anIter.More(); anIter.Next())
    {
      aShapes.Append(anIter.Value());
    }
    if (aType == theType)
    {
      if (aShapes.Extent() == 1)
      {
        return aShapes.First();
      }
      else
      {
        return theShape;
      }
    }
  }
  else
  {
    aShapes.Append(theShape);
  }

  TopoDS_Shape                             aResult;
  NCollection_List<TopoDS_Shape>::Iterator aListIter(aShapes);

  if (aType < theType)
  {
    int aCount;
    for (aCount = 0; aListIter.More(); aListIter.Next())
    {
      TopExp_Explorer anExp(aListIter.Value(), theType);
      if (anExp.More())
      {
        if (!anExp.Current().IsNull())
        {
          aResult = anExp.Current();
          aCount++;
          if (aCount > 1)
          {
            return theShape;
          }
        }
      }
    }
    if (aCount == 1)
    {
      return aResult;
    }
  }
  else
  { // if the shape type more complex than shapes from aShapes list, try make it
    switch (aType)
    {
      case TopAbs_VERTEX: // can't do something from vertex
        break;
      case TopAbs_EDGE: {
        // make wire from edges
        if (theType <= TopAbs_SOLID)
        {
          break;
        }
        BRepBuilderAPI_MakeWire aMakeWire;
        aMakeWire.Add(aShapes);
        if (!aMakeWire.IsDone())
        {
          return theShape;
        }
        if (theType == TopAbs_WIRE)
        {
          return aMakeWire.Wire();
        }
        aShapes.Clear(); // don't break: we can do something more of it
        aShapes.Append(aMakeWire.Wire());
        aListIter.Initialize(aShapes);
      }
        [[fallthrough]];
      case TopAbs_WIRE: {
        // make faceS from wires (one per one)
        if (theType < TopAbs_SOLID)
        {
          break;
        }
        NCollection_List<TopoDS_Shape> aFaces;
        for (; aListIter.More(); aListIter.Next())
        {
          BRepBuilderAPI_MakeFace aMakeFace(TopoDS::Wire(aListIter.Value()));
          if (!aMakeFace.IsDone())
          {
            aFaces.Append(aMakeFace.Face());
          }
        }
        if (theType == TopAbs_FACE)
        {
          if (aFaces.Extent() == 1)
          {
            return aFaces.First();
          }
          return theShape;
        }
        aShapes.Assign(aFaces); // don't break: we can do something more of it
        aListIter.Initialize(aShapes);
      }
        [[fallthrough]];
      case TopAbs_FACE: {
        // make shell from faces
        if (theType < TopAbs_SOLID)
        {
          break;
        }
        BRep_Builder aShellBuilder;
        TopoDS_Shell aShell;
        aShellBuilder.MakeShell(aShell);
        for (; aListIter.More(); aListIter.Next())
        {
          aShellBuilder.Add(aShell, TopoDS::Face(aListIter.Value()));
        }
        aShell.Closed(BRep_Tool::IsClosed(aShell));
        if (theType == TopAbs_SHELL)
        {
          return aShell;
        }
        aShapes.Clear(); // don't break: we can do something more of it
        aShapes.Append(aShell);
        aListIter.Initialize(aShapes);
      }
        [[fallthrough]];
      case TopAbs_SHELL: {
        // make solids from shells (one per one)
        NCollection_List<TopoDS_Shape> aSolids;
        for (; aListIter.More(); aListIter.Next())
        {
          BRepBuilderAPI_MakeSolid aMakeSolid(TopoDS::Shell(aListIter.Value()));
          if (aMakeSolid.IsDone())
          {
            aSolids.Append(aMakeSolid.Solid());
          }
        }
        if (theType == TopAbs_SOLID)
        {
          if (aSolids.Extent() == 1)
          {
            return aSolids.First();
          }
          return theShape;
        }
        aShapes.Assign(aSolids); // don't break: we can do something more of it
        aListIter.Initialize(aShapes);
      }
        [[fallthrough]];
      case TopAbs_SOLID: {
        // make compsolid from solids
        BRep_Builder     aCompBuilder;
        TopoDS_CompSolid aCompSolid;
        aCompBuilder.MakeCompSolid(aCompSolid);
        for (; aListIter.More(); aListIter.Next())
        {
          aCompBuilder.Add(aCompSolid, TopoDS::Solid(aListIter.Value()));
        }
        if (theType == TopAbs_COMPSOLID)
        {
          return aCompSolid;
        }
      }
    }
  }
  return theShape;
}

//=================================================================================================

static bool FindModifUntil(TNaming_NewShapeIterator&                                      it,
                           NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& MS,
                           const TopoDS_Shape&                                            S,
                           const occ::handle<TNaming_NamedShape>&                         Context)
{
  bool found = false;
  for (; it.More(); it.Next())
  {
    if (!it.Shape().IsNull())
    {
      if (it.NamedShape() == Context)
      {
        MS.Add(S);
        found = true;
      }
      else
      { // New shape != Context
        TNaming_NewShapeIterator it2(it);
        found = FindModifUntil(it2, MS, it.Shape(), Context);
      }
    }
  }
  return found;
}

//=================================================================================================

static void SearchModifUntil(
  const NCollection_Map<TDF_Label>& /*Valid*/,
  const occ::handle<TNaming_NamedShape>&                         Target,
  const NCollection_List<occ::handle<TNaming_NamedShape>>&       theListOfGenerators,
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMS)
{
  // Test si S apparait comme oldshape dans Context. : Test if S appears as oldshape in Context.
  bool found = false;
  for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(theListOfGenerators);
       it.More();
       it.Next())
  {
    const occ::handle<TNaming_NamedShape>& aNS = it.Value();
    for (TNaming_Iterator itL(aNS); itL.More(); itL.Next())
    { // <- generators
      const TopoDS_Shape& S = itL.NewShape();
      found                 = false;
      TNaming_Iterator itC(Target);
      for (; itC.More(); itC.Next())
      { // <- generated
        const TopoDS_Shape& OS = itC.OldShape();
        if (OS.IsSame(S))
        {
          theMS.Add(S);
          found = true;
          break;
        }
      }
      if (!found)
      {
        TNaming_NewShapeIterator it1(itL);
        found = FindModifUntil(it1, theMS, S, Target);
      }
    }
  }
}

//=================================================================================================

// NamedShape for this type is assembled from all last modifications of the
// last argument shapes (see method  TNaming_NamingTool::CurrentShape),
// which are not descendants (see method TNaming_NamingTool::BuildDescendants)
// of the stop shape. This type of naming is used for identification shapes,
// which has only one parent with evolution PRIMITIVE (or itself), which
// uniquely identifies it. In most cases stop shape is empty and this algorithm
// is equal to the algorithm for IDENTITY.
//=======================================================================
static bool ModifUntil(const TDF_Label&                                         L,
                       const NCollection_Map<TDF_Label>&                        Valid,
                       const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                       const occ::handle<TNaming_NamedShape>&                   Stop)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;
  if (!ValidArgs(Args))
  {
    return false;
  }
  TNaming_NamingTool::BuildDescendants(Stop, Forbiden); // fills Forbidden from Stop
  // all last modifications of the last argument
  TNaming_NamingTool::CurrentShape(Valid, Forbiden, Args.Last(), MS);
  TNaming_Builder B(L);
  for (int anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    const TopoDS_Shape& S = MS(anItMS);
    B.Select(S, S);
  }
  return true;
}

//=================================================================================================

// from the NS of the first argument TNaming_Iterator is started, shape "S"
// is the NewShape from Iterator with index "myIndex" of the Name, this
// shape and all last modifications (except NamedShapes - descendants of
// the stop shape) are the parts of resulting NamedShape.
//=======================================================================
static bool ConstShape(const TDF_Label&                                         L,
                       const NCollection_Map<TDF_Label>&                        Valid,
                       const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                       const occ::handle<TNaming_NamedShape>&                   Stop,
                       const int                                                Index)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;
  if (!ValidArgs(Args))
  {
    return false;
  }
  TNaming_NamingTool::BuildDescendants(Stop, Forbiden);

  TopoDS_Shape S;
  int          i = 1;
  for (TNaming_Iterator it(Args.First()); it.More(); it.Next(), i++)
  {
    if (Index == i)
    {
      S = it.NewShape();
      break;
    }
  }
  if (S.IsNull())
  {
    return false;
  }

  TNaming_NamingTool::CurrentShapeFromShape(Valid, Forbiden, L, S, MS);

  TNaming_Builder B(L);
  for (int anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    const TopoDS_Shape& SS = MS(anItMS);
    B.Select(SS, SS);
  }
  return true;
}

//=================================================================================================

// algorithm does next steps:
// 1. Sets to the "Forbiden" map  all shapes, which are descendants of stop
//    shape. Named shapes at these labels can't be used.
// 2. Takes first argument (with method CurrentShape) and sets map "S" of
//    ancestors (shapes, which belong to this one) of its shape with type
//    "ShapeType" of Name.
// 3. Takes next argument of Name (with method CurrentShape) and removes
//    from the map "S" all ancestors, which not belongs to the shape of
//    this argument. This step is repeated for all arguments of this Name.
// 4. Adds to the result NamedShape all rest of shapes from the map "S".
//=======================================================================
static bool Intersection(const TDF_Label&                                         L,
                         const NCollection_Map<TDF_Label>&                        Valid,
                         const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                         const occ::handle<TNaming_NamedShape>&                   Stop,
                         const TopAbs_ShapeEnum                                   ShapeType,
                         const int                                                Index)
{
  if (Args.IsEmpty())
  {
    return false;
  }
  if (!ValidArgs(Args))
  {
    return false;
  }
  NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator   it(Args);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;

  TNaming_NamingTool::BuildDescendants(Stop, Forbiden); // <==<1>

  NCollection_List<TopoDS_Shape> aListOfAnc;
  TNaming_NamingTool::CurrentShape(Valid, Forbiden, it.Value(), MS); // first argument
  TopoDS_Shape      CS = MakeShape(MS);
  TNaming_ShapesSet S(CS, ShapeType); // <==<2>
  aListOfAnc.Append(CS);
  it.Next(); // <<===<3.1>
  for (; it.More(); it.Next())
  {
    MS.Clear();
    TNaming_NamingTool::CurrentShape(Valid, Forbiden, it.Value(), MS);
    CS = MakeShape(MS);
    aListOfAnc.Append(CS);

    TNaming_ShapesSet OS(CS, ShapeType);
    S.Filter(OS); //<<===<3.2>
  }

  TNaming_Builder B(L); //<<===<4>
  bool            isOK(false);
  if (S.Map().Extent() > 1 && Index > 0 && ShapeType == TopAbs_EDGE)
  {
    int indxE(0), nbE(0), indxW(0), nbW(0), indxF(0);
    indxE = Index & 0x000000FF;
    nbE   = (Index & 0x0000FF00) >> 8;
    indxW = (Index & 0x000F0000) >> 16;
    nbW   = (Index & 0x00F00000) >> 20;
    indxF = (Index & 0x0F000000) >> 24;
    int                                      k(1);
    TopoDS_Shape                             aS;
    NCollection_List<TopoDS_Shape>::Iterator itl(aListOfAnc);
    for (; itl.More(); itl.Next(), k++)
    {
      if (indxF == k)
      {
        aS = itl.Value();
        break;
      }
    }
    int aCaseW(0);
    int aNbW = aS.NbChildren();
    if (aNbW == nbW)
    {
      aCaseW = 1; // exact solution for wire (nb of wires is kept)
    }
    else
    {
      aCaseW = 2; // indefinite description ==> compound which can include expected wire
    }
    if (aCaseW == 1)
    {
      TopoDS_Shape aWire;
      int          i = 1;
      for (TopoDS_Iterator it2(aS); it2.More(); it2.Next(), i++)
      {
        if (indxW == i)
        {
          aWire = it2.Value();
          break;
        }
      }
      int aCaseE(0);
      int aNbE = aWire.NbChildren();
      if (aNbE == nbE)
      {
        aCaseE = 1; // exact solution for edge
      }
      else
      {
        aCaseE = 2;
      }
      if (aCaseE == 1)
      {
        i = 1;
        TopoDS_Shape anEdge;
        for (TopoDS_Iterator it2(aWire); it2.More(); it2.Next(), i++)
        {
          if (indxE == i)
          {
            anEdge = it2.Value();
            break;
          }
        }
        if (!anEdge.IsNull())
        {
          B.Select(anEdge, anEdge);
          isOK = true;
        }
      }
    }
  }
  if (!isOK)
  {
    for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itM(S.Map()); itM.More();
         itM.Next())
    {
      const TopoDS_Shape& S1 = itM.Key();
      B.Select(S1, S1);
      isOK = true;
    }
  }
  return isOK;
}

//=======================================================================
static void KeepInList(const TopoDS_Shape&             CS,
                       const TopAbs_ShapeEnum          Type,
                       NCollection_List<TopoDS_Shape>& aList)
{
  if (CS.IsNull())
  {
    return;
  }

  if (Type == TopAbs_SHAPE)
  {
    if (CS.ShapeType() == TopAbs_SOLID || CS.ShapeType() == TopAbs_FACE
        || CS.ShapeType() == TopAbs_EDGE || CS.ShapeType() == TopAbs_VERTEX)
    {
      aList.Append(CS);
    }
    else
    {
      for (TopoDS_Iterator it(CS); it.More(); it.Next())
      {
        aList.Append(it.Value());
      }
    }
  }
  else
  {
    if (Type > CS.ShapeType())
    {
      for (TopExp_Explorer exp(CS, Type); exp.More(); exp.Next())
      {
        aList.Append(exp.Current());
      }
    }
    else
    {
      aList.Append(CS);
    }
  }
}

//=================================================================================================

// Resulting NamedShape contains compound of next shapes:
// compound of last modifications of each argument (see CurrentShape method)
// without descendants of the stop shape.
//=======================================================================
static bool Union(const TDF_Label&                                         L,
                  const NCollection_Map<TDF_Label>&                        Valid,
                  const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                  const occ::handle<TNaming_NamedShape>&                   Stop,
                  const TopAbs_ShapeEnum                                   ShapeType,
                  const TDF_Label&                                         ContextLabel)
{
  if (Args.IsEmpty())
  {
    return false;
  }
  if (!ValidArgs(Args))
  {
    return false;
  }
  // temporary solution for Orientation name
  bool isOr(true);
  /* not completed
    const TDF_Label& aLabel = L.Father();
    if(!aLabel.IsNull()) {
    PrintEntry(L);
    PrintEntry(aLabel);
      Handle (TNaming_Naming)  Naming;
      if(aLabel.FindAttribute(TNaming_Naming::GetID(), Naming)) {
        const TNaming_Name& aName = Naming->GetName();
        if(aName.Type() == TNaming_ORIENTATION) {
      const NCollection_List<occ::handle<TNaming_NamedShape>>&   Args = aName.Arguments();
      if(Args.Extent() > 2) {
        const occ::handle<TNaming_NamedShape>& A = Args.First();
        if(!A.IsNull()) {
          PrintEntry(A->Label());
          if (A->Label() == L)
            isOr = true;
        }
      } else
        if(!Args.Extent())
          isOr = true;
        }
      }
    }
  */
  // end of temp. sol.

  NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator   it(Args);
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;

  TNaming_NamingTool::BuildDescendants(Stop, Forbiden); // fill Forbidden
  // clang-format off
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,it.Value(),MS); // fill MS with last modifications of the first argument
  TopoDS_Shape  CS = MakeShape(MS);

  NCollection_List<TopoDS_Shape> aListS;
  if(isOr) {
    KeepInList(CS,ShapeType,aListS);
}
  TNaming_ShapesSet S(CS,ShapeType);//fill internal map of shapeset by shapes of the specified type
  // clang-format on
  it.Next();
  for (; it.More(); it.Next())
  {
    MS.Clear();
    // clang-format off
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,it.Value(),MS);// fill MS with last modifications of the it.Value()
    // clang-format on
    CS = MakeShape(MS);
    if (isOr)
    {
      KeepInList(CS, ShapeType, aListS);
    }
    TNaming_ShapesSet OS(CS, ShapeType);
    S.Add(OS); // concatenate both shapesets
  }

  // start szy 27.05.08
  TopoDS_Shape aCand;
  bool         found = false;
  if (!ContextLabel.IsNull())
  {
    occ::handle<TNaming_NamedShape> CNS;
    ContextLabel.FindAttribute(TNaming_NamedShape::GetID(), CNS);
    TopoDS_Shape aContext;
    if (!CNS.IsNull())
    {
      MS.Clear();
      TNaming_NamingTool::CurrentShape(Valid, Forbiden, CNS, MS);
      aContext = MakeShape(MS);
    }
    NCollection_List<TopoDS_Shape> aList;
    for (TopExp_Explorer anExpl(aContext, ShapeType); anExpl.More(); anExpl.Next())
    {
      aList.Append(anExpl.Current());
    }
    NCollection_List<TopoDS_Shape>::Iterator itl(aList);
    for (; itl.More(); itl.Next())
    {
      aCand   = itl.Value();
      int num = S.Map().Extent();
      for (TopExp_Explorer anExpl(aCand, (ShapeType == TopAbs_WIRE) ? TopAbs_EDGE : TopAbs_FACE);
           anExpl.More();
           anExpl.Next())
      {
        if (S.Map().Contains(anExpl.Current()))
        {
          num--;
        }
      }
      if (num == 0)
      {
        found = true;
        break;
      }
    }
    // end szy 27.05.08
  }

  TNaming_Builder B(L);
  if (found)
  {
    B.Select(aCand, aCand);
  }
  else
  {
    BRep_Builder    aCompoundBuilder;
    TopoDS_Compound aCompound;
    aCompoundBuilder.MakeCompound(aCompound);
    if (!isOr)
    {
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itM(S.Map());
           itM.More();
           itM.Next())
      {
        aCompoundBuilder.Add(aCompound, itM.Key());
      }
    }
    else
    {
      for (NCollection_List<TopoDS_Shape>::Iterator itL(aListS); itL.More(); itL.Next())
      {
        aCompoundBuilder.Add(aCompound, itL.Value());
      }
    }
    TopoDS_Shape aShape = ShapeWithType(aCompound, ShapeType);
    B.Select(aShape, aShape);
  }
  return true;
}

//=======================================================================

static TopoDS_Shape FindShape(
  const NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>>& DM)
{
  TopoDS_Shape aResult;
  int          aNum = DM.Extent();
  if (aNum < 1)
  {
    return aResult;
  }
  NCollection_List<TopoDS_Shape>                                             List;
  NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>>::Iterator it(DM);
  if (it.More())
  {
    const TopoDS_Shape&                  aKey1 = it.Key();
    const NCollection_Map<TopoDS_Shape>& aMap  = it.Value();

    NCollection_Map<TopoDS_Shape>::Iterator itm(aMap); // iterate first map
    for (; itm.More(); itm.Next())
    {
      const TopoDS_Shape& aS = itm.Key(); // element of the first map
      bool                isCand(true);   // aS is a Candidate
      NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>>::Iterator it2(DM);
      for (; it2.More(); it2.Next())
      {
        const TopoDS_Shape& aKey2 = it2.Key();
        if (aKey2 == aKey1)
        {
          continue;
        }
        const NCollection_Map<TopoDS_Shape>& aMap2 = it2.Value();
        if (!aMap2.Contains(aS))
        {
          isCand = false;
        }
      }
      if (isCand)
      {
        List.Append(aS);
      }
    }
  }
  if (List.IsEmpty())
  {
    return aResult;
  }
  if (List.Extent() == 1)
  {
    return List.First();
  }
  NCollection_List<TopoDS_Shape>::Iterator itl(List);
  TopoDS_Compound                          Compound;
  BRep_Builder                             B;
  B.MakeCompound(Compound);
  for (; itl.More(); itl.Next())
  {
    B.Add(Compound, itl.Value());
  }
  return Compound;
}

//=================================================================================================

//! Resolves GENERATION naming.
//! The first argument is a generated target, following arguments are generators.
//! The method rebuilds resulting selections using the last valid modifications.
static bool Generated(const TDF_Label&                                         L,
                      const NCollection_Map<TDF_Label>&                        Valid,
                      const NCollection_List<occ::handle<TNaming_NamedShape>>& Args)
{
  if (Args.Extent() < 2)
  {
    throw Standard_ConstructionError("TNaming_Name::Solve: => Generated");
  }
  // First argument : label of generation
  // Next arguments : generators.

  if (!ValidArgs(Args))
  {
    return false;
  }

  TDF_Label LabelOfGeneration = Args.First()->Label();
  // Nouvell valeurs des generateurs dans l attribut de generation
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMS;
  NCollection_List<occ::handle<TNaming_NamedShape>>             aGenerators;
  aGenerators.Assign(Args);
  aGenerators.RemoveFirst();
  SearchModifUntil(Valid, Args.First(), aGenerators, aMS);
  occ::handle<TNaming_Naming> aNaming;
  TopoDS_Shape                aSelection;
  L.FindAttribute(TNaming_Naming::GetID(), aNaming);
  if (!aNaming.IsNull())
  {
    aSelection = aNaming->GetName().Shape();
  }
  occ::handle<TNaming_NamedShape> anOldNS;
  int                             aVer = -1; // Initial build of name
  L.FindAttribute(TNaming_NamedShape::GetID(), anOldNS);
  if (!anOldNS.IsNull())
  {
    aVer = anOldNS->Version();
  }
  TNaming_Builder                                                  B(L); // NS
  NCollection_List<TopoDS_Shape>                                   aList;
  NCollection_DataMap<TopoDS_Shape, NCollection_Map<TopoDS_Shape>> aDM;
  for (int anItMS = 1; anItMS <= aMS.Extent(); ++anItMS)
  {
    const TopoDS_Shape&           OS = aMS(anItMS);
    NCollection_Map<TopoDS_Shape> aMapDM;
    for (TNaming_NewShapeIterator itNew(OS, L); itNew.More(); itNew.Next())
    {
      if (itNew.Label() == LabelOfGeneration)
      {
        aMapDM.Add(itNew.Shape());
        aList.Append(itNew.Shape()); // szy 21.10.03
      }
    }
    if (aMapDM.Extent())
    {
      aDM.Bind(OS, aMapDM);
    }
  }

  if (aVer == -1)
  { // initial
    int                                      i     = 1;
    TNaming_Name&                            aName = aNaming->ChangeName();
    NCollection_List<TopoDS_Shape>::Iterator it(aList);
    if (!aSelection.IsNull())
    {
      for (; it.More(); it.Next(), i++)
      {
        if (it.Value().IsSame(aSelection))
        {
          B.Select(it.Value(), it.Value());
          aName.Index(i); // for debug - index of selection in the list
          break;
        }
      }
    }
    else
    { // Selection == Null
      for (; it.More(); it.Next())
      {
        B.Select(it.Value(), it.Value());
      }
    }
  }
  else
  {
    // *** Regeneration *** //
    if (aList.Extent() == 1)
    { // trivial case
      B.Select(aList.Last(), aList.Last());
    }
    else
    {
      TNaming_Name&                            aName = aNaming->ChangeName();
      const TopAbs_ShapeEnum                   aType = aName.ShapeType();
      NCollection_List<TopoDS_Shape>           aList2;
      NCollection_List<TopoDS_Shape>::Iterator it(aList);
      for (; it.More(); it.Next())
      {
        if (it.Value().ShapeType() == aType)
        { // collect only the same type
          aList2.Append(it.Value());
        }
      }
      if (!aList2.Extent())
      {
        return false; // Empty
      }

      bool         found  = false;
      TopoDS_Shape aShape = FindShape(aDM);
      if (!aShape.IsNull())
      {
        found = true;
      }
      if (found)
      {
        NCollection_List<TopoDS_Shape> aLM;
        bool                           aHas = false;
        bool                           a1NB = false;
        if (aGenerators.Extent() != aMS.Extent())
        {              // missed generators
          aHas = true; // has lost generatos
          for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator itg(aGenerators);
               itg.More();
               itg.Next())
          {
            if (!aMS.Contains(itg.Value()->Get()))
            {
              aLM.Append(itg.Value()->Get());
            }
          }
          if (aLM.Extent() == 1)
          { // lost 1
            NCollection_List<TopoDS_Shape>::Iterator itm(aLM);
            TopoDS_Shape                             aSM = itm.Value(); // Missed
            for (int anItMS1 = 1; anItMS1 <= aMS.Extent(); ++anItMS1)
            {
              const TopoDS_Shape& aS = aMS(anItMS1);
              if (aSM.ShapeType() == aS.ShapeType())
              {
                if (aS.ShapeType() == TopAbs_EDGE)
                {
                  TopoDS_Vertex aVCom;
                  if (TopExp::CommonVertex(TopoDS::Edge(aS), TopoDS::Edge(aSM), aVCom))
                  {
                    a1NB = true;
                    break;
                  } // lost only 1 neighbour
                }
                else if (aS.ShapeType() == TopAbs_FACE)
                {
                  TopExp_Explorer expl1(aS, TopAbs_EDGE);
                  for (; expl1.More(); expl1.Next())
                  {
                    TopExp_Explorer expl2(aSM, TopAbs_EDGE);
                    for (; expl2.More(); expl2.Next())
                    {
                      if (expl1.Current().IsSame(expl2.Current()))
                      {
                        a1NB = true;
                        break;
                      } // lost only 1 neighbour
                    }
                  }
                }
              }
            }
            // 	    if(aShape.ShapeType() == TopAbs_VERTEX && a1NBE) {
            // 	//if atleast 1 Gen was missed and the Gen is Edge
            // 	      NCollection_List<TopoDS_Shape>::Iterator it(aList);
            // 	      for(;it.More();it.Next()) {
            // 		if(it.Value().ShapeType() == TopAbs_EDGE) {
            // 		  const TopoDS_Shape& aV1 = TopExp::FirstVertex(TopoDS::Edge(it.Value()));
            // 		  const TopoDS_Shape& aV2 = TopExp::LastVertex(TopoDS::Edge(it.Value()));
            // 		  if(aShape.IsSame(aV1)) {aShape2 =  aV2; 	std::cout << "##### => V2 "
            // <<std::endl;break;} 		  else 		    if(aShape.IsSame(aV2)) {aShape2 =  aV1;
            // std::cout << "#####
            // => V1 " <<std::endl;break;}
            // 		}
            // 	      }
            // 	    }
          }
        }
        if (!aHas)
        {                           // all arguments were kept
          B.Select(aShape, aShape); // only this case is correct on 100%
        }
        else
        {
          if (a1NB)
          { // Has, but may be ...
            B.Select(aShape, aShape);
          }
          else
          {
            // put Compound, may be if possible processed later in Sel. Driver
            NCollection_List<TopoDS_Shape>::Iterator it1(aList2);
            for (; it1.More(); it1.Next())
            {
              B.Select(it1.Value(), it1.Value());
            }
          }
        }
      }
      else
      { // not found
        NCollection_List<TopoDS_Shape>::Iterator it2(aList2);
        for (; it2.More(); it2.Next())
        {
          B.Select(it2.Value(), it2.Value());
        }
      }
    }
  }
  return true;
}

//=================================================================================================

//! Resolves IDENTITY naming from a single argument.
//! The resulting selection is built from current shapes filtered by valid labels.
static bool Identity(const TDF_Label&                                         L,
                     const NCollection_Map<TDF_Label>&                        Valid,
                     const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                     const TopAbs_ShapeEnum                                   ShapeType)
{
  if (Args.Extent() > 2)
  {
    throw Standard_ConstructionError("TNaming_Name::Solve");
  }
  if (!ValidArgs(Args))
  {
    return false;
  }
  const occ::handle<TNaming_NamedShape>&                        A = Args.First();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;
  TNaming_NamingTool::CurrentShape(Valid, Forbiden, A, MS);
  TNaming_Builder B(L);
  for (int anItMS = 1; anItMS <= MS.Extent(); ++anItMS)
  {
    const TopoDS_Shape& S = ShapeWithType(MS(anItMS), ShapeType);
    B.Select(S, S);
  }
  return true;
}

//=================================================================================================

//! Resolves FILTERBYNEIGHBOURS naming.
//! The result is a candidate sub-shape sharing required boundaries with all neighbours.
static bool FilterByNeighbourgs(const TDF_Label&                                         L,
                                const NCollection_Map<TDF_Label>&                        Valid,
                                const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                                const occ::handle<TNaming_NamedShape>&                   Stop,
                                const TopAbs_ShapeEnum                                   ShapeType)
{

  TNaming_Builder B(L);

  NCollection_Map<TDF_Label> Forbiden;
  if (!ValidArgs(Args))
  {
    return false;
  }
  // clang-format off
  TNaming_NamingTool::BuildDescendants (Stop, Forbiden); //all descendants of Stop (New shapes) are forbidden
  // clang-format on
  if (!Stop.IsNull())
  {
    Forbiden.Remove(Stop->Label());
  }
  //----------------------------------------
  // First argument: collection has to be filtered.
  //----------------------------------------
  const occ::handle<TNaming_NamedShape>& Cand = Args.First(); // collection of candidates

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> SCand;
  // clang-format off
  TNaming_NamingTool::CurrentShape  (Valid, Forbiden,Cand,SCand);//fills SCand with last modifications of Cand. CandNS should be at the same level (before) as NS of FilterByNBS
  // clang-format on

  //------------------------------------------------------------
  // Autres arguments : contiennent les voisins du bon candidat.
  // Other arguments  : contains the neighbors of the good candidate.
  //------------------------------------------------------------
  TopAbs_ShapeEnum TC = TopAbs_EDGE;
  if (ShapeType == TopAbs_EDGE)
  {
    TC = TopAbs_VERTEX;
  }
  // clang-format off
  if (ShapeType == TopAbs_VERTEX) { TC = TopAbs_VERTEX; // szy 31.03.10 - to process case when Candidate is of type Vertex
}
  // clang-format on
  bool isDone = false;
  if (SCand.Extent() == 1)
  { // check if a collection is inside
    TopoDS_Shape aS = SCand(1);
    if (!aS.IsNull())
    {
      if (aS.ShapeType() == TopAbs_COMPOUND && aS.ShapeType() != ShapeType)
      {
        SCand.Clear();
        TopoDS_Iterator itt(aS);
        for (; itt.More(); itt.Next())
        {
          SCand.Add(itt.Value());
        }
      }
    }
  }
  for (int anItSCand = 1; anItSCand <= SCand.Extent(); ++anItSCand)
  { // 1
    const TopoDS_Shape&                                    S = SCand(anItSCand);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> Boundaries;
    if (S.ShapeType() == TopAbs_VERTEX)
    {                    // # szy 31.03.10
      Boundaries.Add(S); // #
    }
    else
    { // #
      // clang-format off
      for (TopExp_Explorer exp(S,TC); exp.More(); exp.Next()) { //put boundaries of each candidate (from SCand) to the Boundaries map
        // clang-format on
        Boundaries.Add(exp.Current());
      }
    }

    NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(Args);
    it.Next();
    bool Keep = true;
    for (; it.More(); it.Next())
    { // 2 ==> for each Arg
      bool Connected = false;
      // Le candidat doit etre  connexe a au moins un shape de
      // chaque NamedShape des voisins.
      // The candidate should be connectedand and have at least one shape of NamedShape
      // of each neighbor.
      const occ::handle<TNaming_NamedShape>& NSVois = it.Value(); // neighbor

      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> SVois;
      // clang-format off
      TNaming_NamingTool::CurrentShape  (Valid, Forbiden,NSVois,SVois); // fills SVois with last modifications of NSVois
      // clang-format on

      for (int anItSVois = 1; anItSVois <= SVois.Extent(); ++anItSVois)
      { // 6
        const TopoDS_Shape& Vois = SVois(anItSVois);
        for (TopExp_Explorer exp1(Vois, TC); exp1.More(); exp1.Next())
        { // 7
          if (Boundaries.Contains(exp1.Current()))
          {
            Connected = true; // has common boundaries with candidate shape
            break;
          }
        } // 7
        if (Connected)
        {
          break;
        }
      } // 6
      if (!Connected)
      {
        Keep = false;
        break;
      }
    } // 2
    if (Keep)
    {
      B.Select(S, S);
      isDone = true;
    }
  } // 1
  return isDone;
}

//=======================================================================
static const TopoDS_Shape FindSubShapeInAncestor(const TopoDS_Shape& Selection,
                                                 const TopoDS_Shape& Context)
{
  if (Selection.ShapeType() != TopAbs_COMPOUND)
  {
    for (TopExp_Explorer anExpl(Context, Selection.ShapeType()); anExpl.More(); anExpl.Next())
    {
      if (anExpl.Current().IsSame(Selection))
      {
        return anExpl.Current();
      }
    }
  }

  return TopoDS_Shape();
}

//=======================================================================

static int Aggregation(const TopoDS_Shape& S, const TopoDS_Shape& AS, TNaming_Builder& B)
{
  int             N(0);
  TopoDS_Iterator it(S);
  for (; it.More(); it.Next())
  {
    const TopoDS_Shape& sel = it.Value();
    if (sel.ShapeType() > TopAbs_COMPSOLID)
    {
      const TopoDS_Shape& CS = FindSubShapeInAncestor(sel, AS);
      if (!CS.IsNull())
      {
        B.Select(CS, CS);
        N++;
      }
    }
    else
    {
      N += Aggregation(sel, AS, B);
    }
  }
  return N;
}

//=================================================================================================

//! Resolves ORIENTATION naming.
//! Applies orientation from context to the selected shape; Index is used for seam-edge cases.
static bool ORientation(const TDF_Label&                                         L,
                        const NCollection_Map<TDF_Label>&                        Valid,
                        const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                        const occ::handle<TNaming_NamedShape>&                   Stop,
                        const int                                                Index)
{

  if (!ValidArgs(Args))
  {
    return false;
  }

  const occ::handle<TNaming_NamedShape>&                        A = Args.First();
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
  NCollection_Map<TDF_Label>                                    Forbiden;
  TNaming_NamingTool::BuildDescendants(Stop, Forbiden);
  TNaming_NamingTool::CurrentShape(Valid, Forbiden, A, MS);

  TopoDS_Shape aShape;
  bool         isSplit(false);
  if (!MS.IsEmpty())
  {
    if (MS.Extent() == 1)
    {
      aShape = MS(1);
    }
    else
    {
      isSplit = true;
      aShape  = MakeShape(MS);
    }
  }

  TNaming_Builder B(L);
  if (aShape.IsNull())
  {
    return false;
  }

  NCollection_List<TopoDS_Shape> aSList;
  // tmp. solution
  if (aShape.ShapeType() == TopAbs_COMPOUND && !isSplit)
  {
    TopoDS_Iterator it(aShape);
    for (; it.More(); it.Next())
    {
      aSList.Append(it.Value());
    }
  } //

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MSC;
  if (aSList.Extent() == 0)
  {
    const occ::handle<TNaming_NamedShape>& Anc = Args.Last();
    MSC.Clear();
    TNaming_NamingTool::CurrentShape(Valid, Forbiden, Anc, MSC);
    if (MSC.Extent() == 1)
    {
      for (int anItMSC = 1; anItMSC <= MSC.Extent(); ++anItMSC)
      {
        const TopoDS_Shape& AS = MSC(anItMSC);
        // <=== start 21.10.2009
        TopoDS_Shape CS;
        if (Index > 0)
        { // only for seam edge
          TopoDS_Iterator itw(AS);
          for (; itw.More(); itw.Next())
          {
            bool            found(false);
            TopoDS_Iterator it(itw.Value());
            for (int i = 1; it.More(); it.Next(), i++)
            {
              if (i == Index && it.Value().IsSame(aShape))
              {
                CS    = it.Value();
                found = true;
                break;
              }
            }
            if (found)
            {
              break;
            }
          }
        }
        else
        {
          CS = FindSubShapeInAncestor(aShape, AS);
        }
        if (!CS.IsNull())
        {
          B.Select(CS, CS);
        }
        else
        {
          if (!Aggregation(aShape, AS, B))
          {
            return false;
          }
        }
      }
    }
    else
    {
      const TopoDS_Shape  AS = MakeShape(MSC);
      const TopoDS_Shape& CS = FindSubShapeInAncestor(aShape, AS);
      if (!CS.IsNull())
      {
        B.Select(CS, CS);
      }
      else
      {
        if (!Aggregation(aShape, AS, B))
        {
          return false;
        }
      }
    }
  }
  else
  {
    NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator it(Args);
    it.Next(); // skip first

    // temporary solution. To be optimized (+ has connection with Union name)
    occ::handle<NCollection_HArray2<TopoDS_Shape>>
      Arr; // Arr(1,1) - selection; Arr(1,2) - Context shape
    Arr = new NCollection_HArray2<TopoDS_Shape>(1, aSList.Extent(), 1, 2);
    NCollection_List<TopoDS_Shape>::Iterator it1(aSList);
    int                                      i = 1;
    for (; it1.More(); it1.Next(), it.Next(), i++)
    {
      Arr->SetValue(i, 1, it1.Value());
      MSC.Clear();
      TNaming_NamingTool::CurrentShape(Valid, Forbiden, it.Value(), MSC);
      if (MSC.Extent() == 1)
      {
        Arr->SetValue(i, 2, MSC(1));
      }
      else
      {
        const TopoDS_Shape AS = MakeShape(MSC);
        Arr->SetValue(i, 2, AS);
      }
    }

    if (aSList.Extent() == 1)
    {
      const TopoDS_Shape& S = Arr->Value(1, 1);
      if (S.ShapeType() != TopAbs_COMPOUND)
      {
        const TopoDS_Shape& CS = FindSubShapeInAncestor(S, Arr->Value(1, 2));
        if (!CS.IsNull())
        {
          B.Select(CS, CS);
        }
        else
        {
          return false;
        }
      }
      else
      {
        if (!Aggregation(S, Arr->Value(1, 2), B))
        {
          return false;
        }
      }
    }
    else
    { // > 1
      for (int k = Arr->LowerRow(); k <= Arr->UpperRow(); k++)
      {
        const TopoDS_Shape& S  = Arr->Value(k, 1);
        const TopoDS_Shape& AC = Arr->Value(k, 2);
        if (S.ShapeType() != TopAbs_COMPOUND)
        {
          const TopoDS_Shape& CS = FindSubShapeInAncestor(S, AC);
          if (!CS.IsNull())
          {
            B.Select(CS, CS);
          }
          else
          {
            return false;
          }
        }
        else
        {
          if (!Aggregation(S, AC, B))
          {
            return false;
          }
        }
      }
    }
  } // end of tmp. solution

  return true;
}

//=================================================================================================

static bool WireIN(const TDF_Label&                                         L,
                   const NCollection_Map<TDF_Label>&                        Valid,
                   const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                   const occ::handle<TNaming_NamedShape>&                   Stop,
                   int                                                      Index)
{
  bool aResult(false);
  if (!ValidArgs(Args))
  {
    return aResult;
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSh;
  NCollection_Map<TDF_Label>                                    aForbiden;
  if (Args.Extent() < 1)
  {
    throw Standard_ConstructionError("TNaming_Name::Solve");
  }
  const occ::handle<TNaming_NamedShape>& A = Args.First();
  TNaming_NamingTool::CurrentShape(Valid, aForbiden, A, aMapOfSh);
  if (aMapOfSh.Extent() != 1)
  {
    return aResult;
  }
  const TopoDS_Shape& aCF = aMapOfSh(1);
  TNaming_Builder     B(L);
  if (Index == 1)
  { // Outer wire case
    TopoDS_Wire anOuterWire;
    TNaming::OuterWire(TopoDS::Face(aCF), anOuterWire);
    if (!anOuterWire.IsNull())
    {
      B.Select(anOuterWire, anOuterWire);
      aResult = true;
    }
  }
  else
  { // has internal wires
    NCollection_List<occ::handle<TNaming_NamedShape>> ArgsE;
    ArgsE.Assign(Args);
    ArgsE.RemoveFirst();
    // fill Map with edges
    NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator   anIter(ArgsE);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
    NCollection_Map<TDF_Label>                                    Forbiden;

    TNaming_NamingTool::BuildDescendants(Stop, Forbiden); // fill Forbidden
                                                          // clang-format off
    TNaming_NamingTool::CurrentShape  (Valid, Forbiden, anIter.Value(),MS); // fill MS with last modifications of the first additional argument
    TopoDS_Shape  CS = MakeShape(MS);

    TNaming_ShapesSet aSet(CS,TopAbs_EDGE);//fill internal map of shapeset by shapes of the specified type
                                                          // clang-format on
    anIter.Next();
    for (; anIter.More(); anIter.Next())
    {
      MS.Clear();
      // clang-format off
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,anIter.Value(),MS);// fill MS with last modifications of the it.Value()
      // clang-format on
      CS = MakeShape(MS);
      TNaming_ShapesSet OS(CS, TopAbs_EDGE);
      aSet.Add(OS); // concatenate both shapesets
    }
    // end for edges

    for (TopoDS_Iterator itF(aCF); itF.More(); itF.Next())
    {                                      // find the expected wire in the face
      const TopoDS_Shape& S = itF.Value(); // wire
      if (!S.IsNull())
      {
        if (S.ShapeType() == TopAbs_WIRE)
        {
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aView;
          int                                                    aNum(0x7FFFFFFF);
          for (TopoDS_Iterator it(S); it.More(); it.Next())
          {
            aView.Add(it.Value()); // edges of wire of the face in map
          }

          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(aSet.Map());
          aNum = aView.Extent();
          if (aNum == aSet.Map().Extent())
          {
            for (; it.More(); it.Next())
            {
              if (aView.Contains(it.Key()))
              {
                aNum--;
              }
            }
          }
          if (aNum == 0)
          {
            B.Select(S, S);
            aResult = true;
            break;
          }
        }
      }
    } //

    if (!aResult)
    {
      TopoDS_Wire anOuterWire;
      TNaming::OuterWire(TopoDS::Face(aCF), anOuterWire);
      if (!anOuterWire.IsNull())
      {
        for (TopoDS_Iterator itF(aCF); itF.More(); itF.Next())
        {
          const TopoDS_Shape& S = itF.Value(); // wire
          if (!S.IsNull() && S.ShapeType() == TopAbs_WIRE)
          {
            if (S.IsEqual(anOuterWire))
            {
              continue;
            }
            B.Select(S, S);
          }
        }
      }
    }
  }
  return aResult;
}

//=================================================================================================

//! Resolves SHELLIN naming.
//! Builds shell selection from shape arguments within the current naming scope.
static bool ShellIN(const TDF_Label&                                         L,
                    const NCollection_Map<TDF_Label>&                        Valid,
                    const NCollection_List<occ::handle<TNaming_NamedShape>>& Args,
                    const occ::handle<TNaming_NamedShape>&                   Stop,
                    int                                                      Index)
{
  bool aResult(false);
  if (!ValidArgs(Args))
  {
    return aResult;
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapOfSh;
  NCollection_Map<TDF_Label>                                    aForbiden;
  if (Args.Extent() < 1)
  {
    throw Standard_ConstructionError("TNaming_Name::Solve");
  }
  const occ::handle<TNaming_NamedShape>& A = Args.First();
  TNaming_NamingTool::CurrentShape(Valid, aForbiden, A, aMapOfSh);
  if (aMapOfSh.Extent() != 1)
  {
    return aResult;
  }
  const TopoDS_Shape& aCSO = aMapOfSh(1);
  TNaming_Builder     B(L);
  if (Index == 1)
  { // Outer Shell case
    TopoDS_Shell anOuterShell;
    TNaming::OuterShell(TopoDS::Solid(aCSO), anOuterShell);
    if (!anOuterShell.IsNull())
    {
      B.Select(anOuterShell, anOuterShell);
      aResult = true;
    }
  }
  else
  { // has internal Shells
    NCollection_List<occ::handle<TNaming_NamedShape>> ArgsF;
    ArgsF.Assign(Args);
    ArgsF.RemoveFirst();
    // fill Map with faces
    NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator   anIter(ArgsF);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MS;
    NCollection_Map<TDF_Label>                                    Forbiden;

    TNaming_NamingTool::BuildDescendants(Stop, Forbiden); // fill Forbidden
                                                          // clang-format off
    TNaming_NamingTool::CurrentShape  (Valid, Forbiden, anIter.Value(),MS); // fill MS with last modifications of the first additional argument
    TopoDS_Shape  CS = MakeShape(MS);

    TNaming_ShapesSet aSet(CS,TopAbs_FACE);//fill internal map of shapeset by shapes of the specified type
                                                          // clang-format on
    anIter.Next();
    for (; anIter.More(); anIter.Next())
    {
      MS.Clear();
      // clang-format off
      TNaming_NamingTool::CurrentShape (Valid, Forbiden,anIter.Value(),MS);// fill MS with last modifications of the it.Value()
      // clang-format on
      CS = MakeShape(MS);
      TNaming_ShapesSet OS(CS, TopAbs_FACE);
      aSet.Add(OS); // concatenate both shapesets
    }
    // end for faces

    for (TopoDS_Iterator itS(aCSO); itS.More(); itS.Next())
    {                                      // find the expected shell in the solid
      const TopoDS_Shape& S = itS.Value(); // shell
      if (!S.IsNull())
      {
        if (S.ShapeType() == TopAbs_SHELL)
        {
          NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aView;
          int                                                    aNum(0x7FFFFFFF);
          for (TopoDS_Iterator it(S); it.More(); it.Next())
          {
            aView.Add(it.Value()); // faces of shell of the solid in map
          }

          aNum = aView.Extent();
          if (aNum == aSet.Map().Extent())
          {
            NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(aSet.Map());
            for (; it.More(); it.Next())
            {
              if (aView.Contains(it.Key()))
              {
                aNum--;
              }
            }
          }
          if (aNum == 0)
          {
            B.Select(S, S);
            aResult = true;
            break;
          }
        }
      }
    } //

    if (!aResult)
    {
      TopoDS_Shell anOuterShell;
      TNaming::OuterShell(TopoDS::Solid(aCSO), anOuterShell);
      if (!anOuterShell.IsNull())
      {
        for (TopoDS_Iterator itS(aCSO); itS.More(); itS.Next())
        {
          const TopoDS_Shape& S = itS.Value(); // shell
          if (!S.IsNull() && S.ShapeType() == TopAbs_SHELL)
          {
            if (S.IsEqual(anOuterShell))
            {
              continue;
            }
            B.Select(S, S);
          }
        }
      }
    }
  }
  return aResult;
}
#ifdef OCCT_DEBUG
//=======================================================================
static const char* NameTypeToString(const TNaming_NameType Type)
{
  switch (Type)
  {
    case TNaming_UNKNOWN:
      return "UNKNOWN";
    case TNaming_IDENTITY:
      return "IDENTITY";
    case TNaming_MODIFUNTIL:
      return "MODIFUNTIL";
    case TNaming_GENERATION:
      return "GENERATION";
    case TNaming_INTERSECTION:
      return "INTERSECTION";
    case TNaming_UNION:
      return "UNION";
    case TNaming_SUBSTRACTION:
      return "SUBSTRACTION";
    case TNaming_CONSTSHAPE:
      return "CONSTSHAPE";
    case TNaming_FILTERBYNEIGHBOURGS:
      return "FILTERBYNEIGHBOURGS";
    case TNaming_ORIENTATION:
      return "ORIENTATION";
    case TNaming_WIREIN:
      return "WIREIN";
    default:
      throw Standard_DomainError("TNaming_NameType; enum term unknown ");
  }
}
#endif
//=================================================================================================

bool TNaming_Name::Solve(const TDF_Label& aLab, const NCollection_Map<TDF_Label>& Valid) const
{
  bool Done = false;
  try
  {
    OCC_CATCH_SIGNALS
    switch (myType)
    {
      case TNaming_UNKNOWN: {
        break;
      }
      case TNaming_IDENTITY: {
        Done = Identity(aLab, Valid, myArgs, myShapeType);
        break;
      }
      case TNaming_MODIFUNTIL: {
        Done = ModifUntil(aLab, Valid, myArgs, myStop);
        break;
      }
      case TNaming_GENERATION: {
        Done = Generated(aLab, Valid, myArgs);
        break;
      }
      case TNaming_INTERSECTION: {
        Done = Intersection(aLab, Valid, myArgs, myStop, myShapeType, myIndex);
        break;
      }
      case TNaming_UNION: {
        Done = Union(aLab, Valid, myArgs, myStop, myShapeType, myContextLabel);
        break;
      }
      case TNaming_SUBSTRACTION: {
        throw Standard_NotImplemented();
        //      Done = Substraction (aLab,Valid,myArgs);
        break;
      }
      case TNaming_CONSTSHAPE: {
        Done = ConstShape(aLab, Valid, myArgs, myStop, myIndex);
        break;
      }
      case TNaming_FILTERBYNEIGHBOURGS: {
        Done = FilterByNeighbourgs(aLab, Valid, myArgs, myStop, myShapeType);
        break;
      }
      case TNaming_ORIENTATION: {
        Done = ORientation(aLab, Valid, myArgs, myStop, myIndex);
        break;
      }
      case TNaming_WIREIN: {
        Done = WireIN(aLab, Valid, myArgs, myStop, myIndex);
        break;
      }
      case TNaming_SHELLIN: {
        Done = ShellIN(aLab, Valid, myArgs, myStop, myIndex);
        break;
      }
    }
  }
  catch (Standard_Failure const&)
  {
#ifdef OCCT_DEBUG
    std::cout << "Name::Solve: EXCEPTION==> NameType = " << NameTypeToString(myType) << "  ";
    PrintEntry(aLab);
#endif
  }
  return Done;
}

//=================================================================================================

void TNaming_Name::ContextLabel(const TDF_Label& theLabel)
{
  myContextLabel = theLabel;
}

//=================================================================================================

const TDF_Label& TNaming_Name::ContextLabel() const
{
  return myContextLabel;
}

//=================================================================================================

void TNaming_Name::Orientation(const TopAbs_Orientation theOrientation)
{
  myOrientation = theOrientation;
}

//=================================================================================================

void TNaming_Name::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN(theOStream, TNaming_Name)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myType)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myShapeType)

  for (NCollection_List<occ::handle<TNaming_NamedShape>>::Iterator anArgsIt(myArgs);
       anArgsIt.More();
       anArgsIt.Next())
  {
    const occ::handle<TNaming_NamedShape>& anArg = anArgsIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, anArg.get())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIndex)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myShape)

  TCollection_AsciiString aLabel;
  TDF_Tool::Entry(myContextLabel, aLabel);
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLabel)
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, myOrientation)
}
