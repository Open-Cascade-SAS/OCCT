// Created on: 1996-02-23
// Created by: Jacques GOUSSARD
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

#include <BRepTest.hxx>
#include <BRepAlgo.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>
#include <DBRep_DrawableShape.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Edge.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>
#include <Draw.hxx>
#include <DBRep.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <DrawTrSurf.hxx>
#include <GeomAbs_Shape.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Map.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <TopOpeBRepTool_PurgeInternalEdges.hxx>
// #include <TopOpeBRepTool_FuseEdges.hxx>
#include <BRepLib.hxx>
#include <BRepLib_FuseEdges.hxx>

#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>

#include <stdio.h>

// Number of BRepCheck_Statuses in BRepCheck_Status.hxx file
//(BRepCheck_NoError is not considered, i.e. general status
// is smaller by one specified in file)
static const int NumberOfStatus = 36;

static char* checkfaultyname = nullptr;

Standard_EXPORT void BRepTest_CheckCommands_SetFaultyName(const char* name)
{
  if (checkfaultyname != nullptr)
  {
    free(checkfaultyname);
    checkfaultyname = nullptr;
  }
  if (name == nullptr)
  {
    checkfaultyname = (char*)malloc(strlen("faulty_") + 1);
    strcpy(checkfaultyname, "faulty_");
  }
  else
  {
    checkfaultyname = (char*)malloc(strlen(name) + 1);
    strcpy(checkfaultyname, name);
  }
}

static NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
                                                          theMap;
static int                                                nbfaulty = 0;
static NCollection_Sequence<occ::handle<Draw_Drawable3D>> lfaulty;

Standard_IMPORT int BRepCheck_Trace(const int phase);

//=======================================================================
// function : FindNamed
//=======================================================================
static bool FindNamed(const TopoDS_Shape& S, char*& Name)
{
  for (int i = 1; i <= lfaulty.Length(); i++)
  {
    occ::handle<DBRep_DrawableShape> DS = occ::down_cast<DBRep_DrawableShape>(lfaulty(i));
    if (DS->Shape().IsSame(S))
    {
      Name = (char*)DS->Name();
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : Contains
//=======================================================================
static bool Contains(const NCollection_List<TopoDS_Shape>& L, const TopoDS_Shape& S)
{
  NCollection_List<TopoDS_Shape>::Iterator it;
  for (it.Initialize(L); it.More(); it.Next())
  {
    if (it.Value().IsSame(S))
    {
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : PrintSub
//=======================================================================
static void PrintSub(Standard_OStream&         OS,
                     const BRepCheck_Analyzer& Ana,
                     const TopoDS_Shape&       S,
                     const TopAbs_ShapeEnum    Subtype)

{
  char*                                        Name;
  NCollection_List<BRepCheck_Status>::Iterator itl;
  TopExp_Explorer                              exp;
  for (exp.Init(S, Subtype); exp.More(); exp.Next())
  {
    const occ::handle<BRepCheck_Result>& res = Ana.Result(exp.Current());
    const TopoDS_Shape&                  sub = exp.Current();
    for (res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext())
    {
      if (res->ContextualShape().IsSame(S) && !Contains(theMap(sub), S))
      {
        theMap(sub).Append(S);
        itl.Initialize(res->StatusOnShape());
        if (itl.Value() != BRepCheck_NoError)
        {
          if (!FindNamed(sub, Name))
          {
            nbfaulty++;
            Name = (char*)malloc(18 * sizeof(char));
            Sprintf(Name, "%s%d", checkfaultyname, nbfaulty);
            DBRep::Set(Name, sub);
            lfaulty.Append(Draw::Get((const char*&)Name));
          }
          OS << "Shape " << Name << " ";
          if (!FindNamed(S, Name))
          {
            nbfaulty++;
            Name = (char*)malloc(18 * sizeof(char));
            Sprintf(Name, "%s%d", checkfaultyname, nbfaulty);
            DBRep::Set(Name, S);
            lfaulty.Append(Draw::Get((const char*&)Name));
          }
          OS << " on shape " << Name << " :\n";
          for (; itl.More(); itl.Next())
          {
            BRepCheck::Print(itl.Value(), OS);
          }
        }
        break;
      }
    }
  }
}

//=======================================================================
// function : Print
//=======================================================================
static void Print(Standard_OStream& OS, const BRepCheck_Analyzer& Ana, const TopoDS_Shape& S)
{
  for (TopoDS_Iterator iter(S); iter.More(); iter.Next())
  {
    Print(OS, Ana, iter.Value());
  }

  char*                                        Name;
  TopAbs_ShapeEnum                             styp = S.ShapeType();
  NCollection_List<BRepCheck_Status>::Iterator itl;
  if (!Ana.Result(S).IsNull() && !theMap.IsBound(S))
  {
    itl.Initialize(Ana.Result(S)->Status());
    if (itl.Value() != BRepCheck_NoError)
    {
      if (!FindNamed(S, Name))
      {
        nbfaulty++;
        Name = (char*)malloc(18 * sizeof(char));
        Sprintf(Name, "%s%d", checkfaultyname, nbfaulty);
        DBRep::Set(Name, S);
        lfaulty.Append(Draw::Get((const char*&)Name));
      }
      OS << "On Shape " << Name << " :\n";

      for (; itl.More(); itl.Next())
      {
        if (itl.Value() != BRepCheck_NoError)
          BRepCheck::Print(itl.Value(), OS);
      }
    }
  }
  if (!theMap.IsBound(S))
  {
    NCollection_List<TopoDS_Shape> thelist;
    theMap.Bind(S, thelist);
  }

  switch (styp)
  {
    case TopAbs_EDGE:
      PrintSub(OS, Ana, S, TopAbs_VERTEX);
      break;
    case TopAbs_WIRE:
      PrintSub(OS, Ana, S, TopAbs_EDGE);
      PrintSub(OS, Ana, S, TopAbs_VERTEX);
      break;
    case TopAbs_FACE:
      PrintSub(OS, Ana, S, TopAbs_WIRE);
      PrintSub(OS, Ana, S, TopAbs_EDGE);
      PrintSub(OS, Ana, S, TopAbs_VERTEX);
      break;
    case TopAbs_SHELL:
      //    PrintSub(OS,Ana,S,TopAbs_FACE);
      break;
    case TopAbs_SOLID:
      //    PrintSub(OS,Ana,S,TopAbs_EDGE);
      PrintSub(OS, Ana, S, TopAbs_SHELL);
      break;
    default:
      break;
  }
}

//=================================================================================================

static int computetolerance(Draw_Interpretor& di, int narg, const char** a)
{
  if (narg < 2)
  {
    // std::cout << "Usage: computetolerance shape" << std::endl;
    di << "Usage: computetolerance shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);
  double       tol;
  if (S.ShapeType() == TopAbs_EDGE)
  {
    BRepCheck_Edge bce(TopoDS::Edge(S));
    tol = bce.Tolerance();
    // std::cout<< "Tolerance de " << (void*) &(*S.TShape()) << " : " << tol << std::endl;
    Standard_SStream aSStream1;
    aSStream1 << "Tolerance de " << (void*)&(*S.TShape()) << " : " << tol << "\n";
    di << aSStream1;
  }
  else
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theEdges;
    TopExp_Explorer                                        exp;
    for (exp.Init(S, TopAbs_EDGE); exp.More(); exp.Next())
    {
      if (theEdges.Add(exp.Current()))
      {
        BRepCheck_Edge bce(TopoDS::Edge(exp.Current()));
        tol = bce.Tolerance();
        // std::cout<< "Tolerance de " << (void*) &(*exp.Current().TShape()) << " : " << tol <<
        // "\n";
        Standard_SStream aSStream2;
        aSStream2 << "Tolerance de " << (void*)&(*exp.Current().TShape()) << " : " << tol << "\n";
        di << aSStream2;
      }
    }
    // std::cout << std::endl;
    di << "\n";
  }
  return 0;
}

//=======================================================================
// function : checksection
// purpose  : Checks the closure of a section line
//=======================================================================
static int checksection(Draw_Interpretor& di, int narg, const char** a)
{
  if (narg < 2)
  {
    di << a[0] << " shape [-r <ref_val>]\n";
    return 1;
  }

  int          aCompareValue = -1;
  TopoDS_Shape S             = DBRep::Get(a[1]);

  for (int anAI = 2; anAI < narg; anAI++)
  {
    if (!strcmp(a[anAI], "-r"))
    {
      aCompareValue = Draw::Atoi(a[++anAI]);
    }
    else
    {
      di << "Error: Wrong option" << a[anAI] << "\n";
    }
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> theVertices;
  TopExp_Explorer                                        exp;
  for (exp.Init(S, TopAbs_VERTEX); exp.More(); exp.Next())
  {
    if (!theVertices.Add(exp.Current()))
      theVertices.Remove(exp.Current());
  }
  // std::cout << " nb alone Vertices : " << theVertices.Extent() << std::endl;
  di << " nb alone Vertices : " << theVertices.Extent() << "\n";

  if (aCompareValue >= 0)
  {
    if (theVertices.Extent() == aCompareValue)
    {
      di << "Section is OK\n";
    }
    else
    {
      di << "Error: " << aCompareValue << " vertices are expected but " << theVertices.Extent()
         << " are found.\n";
    }
  }

  char                                                             Name[32];
  int                                                              ipp = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator itvx;
  for (itvx.Initialize(theVertices); itvx.More(); itvx.Next())
  {
    ipp++;
    Sprintf(Name, "alone_%d", ipp);
    DBRep::Set(Name, itvx.Key());
    // std::cout << Name << " " ;
    di << Name << " ";
  }
  // std::cout << std::endl;
  di << "\n";
  return 0;
}

//=======================================================================

//=======================================================================
// function : checkdiff
// purpose  : Checks the differences between a result and his arguments
//=======================================================================
static int checkdiff(Draw_Interpretor& di, int narg, const char** a)
{
  const char* syntaxe = "checkdiff arg1 [arg2..argn] result [closedSolid (0/1)] [geomCtrl (1/0)]";
  if (narg < 3)
  {
    if (narg == 2)
    {
      int bcrtrace = Draw::Atoi(a[narg - 1]);
      bcrtrace     = BRepCheck_Trace(bcrtrace);
      // std::cout << "BRepCheck_Trace : " << bcrtrace << std::endl;
      di << "BRepCheck_Trace : " << bcrtrace << "\n";
      // std::cout << syntaxe << std::endl;
      di << syntaxe << "\n";
      return 0;
    }
    // std::cout << syntaxe << std::endl;
    di << syntaxe << "\n";
    return 1;
  }

  int          lastArg     = narg - 2;
  bool         closedSolid = false;
  bool         geomCtrl    = true;
  TopoDS_Shape resu        = DBRep::Get(a[narg - 1]);
  if (resu.IsNull())
  {
    if (narg < 4)
    {
      // std::cout << syntaxe << std::endl;
      di << syntaxe << "\n";
      return 1;
    }
    closedSolid = Draw::Atoi(a[narg - 1]) != 0;
    resu        = DBRep::Get(a[narg - 2]);
    lastArg     = narg - 3;
    if (resu.IsNull())
    {
      if (narg < 5)
      {
        // std::cout << syntaxe << std::endl;
        di << syntaxe << "\n";
        return 1;
      }
      geomCtrl    = closedSolid;
      closedSolid = Draw::Atoi(a[narg - 2]) != 0;
      resu        = DBRep::Get(a[narg - 3]);
      lastArg     = narg - 4;
      if (resu.IsNull())
      {
        // std::cout << syntaxe << std::endl;
        di << syntaxe << "\n";
        return 1;
      }
    }
  }

  NCollection_List<TopoDS_Shape> lesArgs;
  for (int id = 1; id <= lastArg; id++)
  {
    lesArgs.Append(DBRep::Get(a[id]));
  }

  if (BRepAlgo::IsValid(lesArgs, resu, closedSolid, geomCtrl))
  {
    // std::cout << "Difference is Valid." << std::endl;
    di << "Difference is Valid.\n";
  }
  else
  {
    // std::cout << "error : Difference is Not Valid !" << std::endl;
    di << "error : Difference is Not Valid !\n";
  }

  return 0;
}

//=======================================================================
// function : ContextualDump
// purpose  : Contextual (modeling) style of output.
//=======================================================================
void ContextualDump(Draw_Interpretor&         theCommands,
                    const BRepCheck_Analyzer& theAna,
                    const TopoDS_Shape&       theShape)
{
  theMap.Clear();
  nbfaulty = 0;
  lfaulty.Clear();

  Standard_SStream aSStream;
  Print(aSStream, theAna, theShape);
  theCommands << aSStream;

  theCommands << "\n";
  theMap.Clear();

  if (nbfaulty != 0)
    theCommands << "Faulty shapes in variables " << checkfaultyname << "1 to " << checkfaultyname
                << nbfaulty << " \n";

  theCommands << "\n";
}

//=======================================================================
// function : FillProblems
// purpose : auxiliary for StructuralDump
//=======================================================================
static void FillProblems(const BRepCheck_Status                 stat,
                         occ::handle<NCollection_HArray1<int>>& NbProblems)
{

  const int anID = static_cast<int>(stat);

  if ((NbProblems->Upper() < anID) || (NbProblems->Lower() > anID))
    return;

  NbProblems->SetValue(anID, NbProblems->Value(anID) + 1);
}

//=======================================================================
// function : GetProblemSub
// purpose : auxiliary for StructuralDump
//=======================================================================
static void GetProblemSub(const BRepCheck_Analyzer&                         Ana,
                          const TopoDS_Shape&                               Shape,
                          occ::handle<NCollection_HSequence<TopoDS_Shape>>& sl,
                          occ::handle<NCollection_HArray1<int>>&            NbProblems,
                          const TopAbs_ShapeEnum                            Subtype)
{
  NCollection_List<BRepCheck_Status>::Iterator itl;
  TopExp_Explorer                              exp;
  for (exp.Init(Shape, Subtype); exp.More(); exp.Next())
  {
    const occ::handle<BRepCheck_Result>& res = Ana.Result(exp.Current());

    const TopoDS_Shape& sub = exp.Current();
    for (res->InitContextIterator(); res->MoreShapeInContext(); res->NextShapeInContext())
    {
      if (res->ContextualShape().IsSame(Shape) && !Contains(theMap(sub), Shape))
      {
        theMap(sub).Append(Shape);
        itl.Initialize(res->StatusOnShape());

        if (itl.Value() != BRepCheck_NoError)
        {
          int ii = 0;

          for (ii = 1; ii <= sl->Length(); ii++)
            if (sl->Value(ii).IsSame(sub))
              break;

          if (ii > sl->Length())
          {
            sl->Append(sub);
            FillProblems(itl.Value(), NbProblems);
          }
          for (ii = 1; ii <= sl->Length(); ii++)
            if (sl->Value(ii).IsSame(Shape))
              break;
          if (ii > sl->Length())
          {
            sl->Append(Shape);
            FillProblems(itl.Value(), NbProblems);
          }
        }
        break;
      }
    }
  }
}

//=======================================================================
// function : GetProblemShapes
// purpose : auxiliary for StructuralDump
//=======================================================================
static void GetProblemShapes(const BRepCheck_Analyzer&                         Ana,
                             const TopoDS_Shape&                               Shape,
                             occ::handle<NCollection_HSequence<TopoDS_Shape>>& sl,
                             occ::handle<NCollection_HArray1<int>>&            NbProblems)
{
  for (TopoDS_Iterator iter(Shape); iter.More(); iter.Next())
  {
    GetProblemShapes(Ana, iter.Value(), sl, NbProblems);
  }
  TopAbs_ShapeEnum                             styp = Shape.ShapeType();
  NCollection_List<BRepCheck_Status>::Iterator itl;
  if (!Ana.Result(Shape).IsNull() && !theMap.IsBound(Shape))
  {
    itl.Initialize(Ana.Result(Shape)->Status());

    if (itl.Value() != BRepCheck_NoError)
    {
      sl->Append(Shape);
      FillProblems(itl.Value(), NbProblems);
    }
  }
  if (!theMap.IsBound(Shape))
  {
    NCollection_List<TopoDS_Shape> thelist;
    theMap.Bind(Shape, thelist);
  }

  switch (styp)
  {
    case TopAbs_EDGE:
      GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX);
      break;
    case TopAbs_FACE:
      GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_WIRE);
      GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_EDGE);
      GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_VERTEX);
      break;
    case TopAbs_SHELL:
      break;
    case TopAbs_SOLID:
      GetProblemSub(Ana, Shape, sl, NbProblems, TopAbs_SHELL);
      break;
    default:
      break;
  }
}

//=======================================================================
// function : StructuralDump
// purpose  : Structural (data exchange) style of output.
//=======================================================================
void StructuralDump(Draw_Interpretor&         theCommands,
                    const BRepCheck_Analyzer& theAna,
                    const char*               ShName,
                    const char*               Pref,
                    const TopoDS_Shape&       theShape)
{
  int i;
  theCommands << " -- The Shape " << ShName << " has problems :\n";
  theCommands << "  Check                                    Count\n";
  theCommands << " ------------------------------------------------\n";

  occ::handle<NCollection_HArray1<int>> NbProblems =
    new NCollection_HArray1<int>(1, NumberOfStatus);
  for (i = 1; i <= NumberOfStatus; i++)
    NbProblems->SetValue(i, 0);
  occ::handle<NCollection_HSequence<TopoDS_Shape>> sl, slv, sle, slw, slf, sls, slo;
  sl = new NCollection_HSequence<TopoDS_Shape>();
  theMap.Clear();
  GetProblemShapes(theAna, theShape, sl, NbProblems);
  theMap.Clear();

  int aProblemID = static_cast<int>(BRepCheck_InvalidPointOnCurve);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Point on Curve ................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidPointOnCurveOnSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Point on CurveOnSurface .......... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidPointOnSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Point on Surface ................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_No3DCurve);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  No 3D Curve .............................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_Multiple3DCurve);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Multiple 3D Curve ........................ " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_Invalid3DCurve);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid 3D Curve ......................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_NoCurveOnSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  No Curve on Surface ...................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidCurveOnSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Curve on Surface ................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidCurveOnClosedSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Curve on closed Surface .......... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidSameRangeFlag);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid SameRange Flag ................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidSameParameterFlag);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid SameParameter Flag ............... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidDegeneratedFlag);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Degenerated Flag ................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_FreeEdge);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Free Edge ................................ " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidMultiConnexity);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid MultiConnexity ................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidRange);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Range ............................ " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_EmptyWire);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Empty Wire ............................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_RedundantEdge);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Redundant Edge ........................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_SelfIntersectingWire);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Self Intersecting Wire ................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_NoSurface);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  No Surface ............................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidWire);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Wire ............................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_RedundantWire);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Redundant Wire ........................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_IntersectingWires);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Intersecting Wires ....................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidImbricationOfWires);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Imbrication of Wires ............. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_EmptyShell);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Empty Shell .............................. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_RedundantFace);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Redundant Face ........................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_UnorientableShape);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Unorientable Shape ....................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_NotClosed);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Not Closed ............................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_NotConnected);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Not Connected ............................ " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_SubshapeNotInShape);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Subshape not in Shape .................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_BadOrientation);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Bad Orientation .......................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_BadOrientationOfSubshape);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Bad Orientation of Subshape .............. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidToleranceValue);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid tolerance value................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidPolygonOnTriangulation);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid polygon on triangulation.......... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_InvalidImbricationOfShells);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Invalid Imbrication of Shells............. " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_EnclosedRegion);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  Enclosed Region........................... " << NbProblems->Value(aProblemID)
                << "\n";

  aProblemID = static_cast<int>(BRepCheck_CheckFail);
  if (NbProblems->Value(aProblemID) > 0)
    theCommands << "  checkshape failure........................ " << NbProblems->Value(aProblemID)
                << "\n";

  theCommands << " ------------------------------------------------\n";
  theCommands << "*** Shapes with problems : " << sl->Length() << "\n";

  slv = new NCollection_HSequence<TopoDS_Shape>();
  sle = new NCollection_HSequence<TopoDS_Shape>();
  slw = new NCollection_HSequence<TopoDS_Shape>();
  slf = new NCollection_HSequence<TopoDS_Shape>();
  sls = new NCollection_HSequence<TopoDS_Shape>();
  slo = new NCollection_HSequence<TopoDS_Shape>();

  for (i = 1; i <= sl->Length(); i++)
  {
    TopoDS_Shape     shi = sl->Value(i);
    TopAbs_ShapeEnum sti = shi.ShapeType();
    switch (sti)
    {
      case TopAbs_VERTEX:
        slv->Append(shi);
        break;
      case TopAbs_EDGE:
        sle->Append(shi);
        break;
      case TopAbs_WIRE:
        slw->Append(shi);
        break;
      case TopAbs_FACE:
        slf->Append(shi);
        break;
      case TopAbs_SHELL:
        sls->Append(shi);
        break;
      case TopAbs_SOLID:
        slo->Append(shi);
        break;
      default:
        break;
    }
  }

  BRep_Builder B;
  if (slv->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = slv->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, slv->Value(i));
    char aName[20];
    Sprintf(aName, "%s_v", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "VERTEX	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "VERTEX	:  " << nb << " Items -> compound named " << aName << "\n";
  }
  if (sle->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = sle->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, sle->Value(i));
    char aName[20];
    Sprintf(aName, "%s_e", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "EDGE	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "EDGE	:  " << nb << " Items -> compound named " << aName << "\n";
  }
  if (slw->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = slw->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, slw->Value(i));
    char aName[20];
    Sprintf(aName, "%s_w", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "WIRE	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "WIRE	:  " << nb << " Items -> compound named " << aName << "\n";
  }
  if (slf->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = slf->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, slf->Value(i));
    char aName[20];
    Sprintf(aName, "%s_f", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "FACE	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "FACE	:  " << nb << " Items -> compound named " << aName << "\n";
  }
  if (sls->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = sls->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, sls->Value(i));
    char aName[20];
    Sprintf(aName, "%s_s", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "SHELL	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "SHELL	:  " << nb << " Items -> compound named " << aName << "\n";
  }
  if (slo->Length() > 0)
  {
    TopoDS_Compound comp;
    B.MakeCompound(comp);
    int nb = slo->Length();
    for (i = 1; i <= nb; i++)
      B.Add(comp, slo->Value(i));
    char aName[20];
    Sprintf(aName, "%s_o", Pref);
    DBRep::Set(aName, comp);
    if (nb > 9)
      theCommands << "SOLID	: " << nb << " Items -> compound named " << aName << "\n";
    else
      theCommands << "SOLID	:  " << nb << " Items -> compound named " << aName << "\n";
  }
}

//=================================================================================================

static int checkshape(Draw_Interpretor& theCommands, int narg, const char** a)
{
  if (narg == 1)
  {
    theCommands << "\n";
    theCommands << "Usage : checkshape [-top] shape [result] [-short] [-parallel] [-exact]\n";
    theCommands << "\n";
    theCommands << "Where :\n";
    theCommands << "   -top      - check topology only.\n";
    theCommands << "   shape     - the name of the shape to test.\n";
    theCommands
      << "   result    - the prefix of the output shape names. If it is used, structural\n";
    theCommands << "               output style will be used. Otherwise - contextual one.\n";
    theCommands << "   -short    - short description of check.\n";
    theCommands << "   -parallel - run check in parallel.\n";
    theCommands << "   -exact    - run check using exact method.\n";
    return 0;
  }

  if (narg > 7)
  {
    theCommands << "Invalid number of args!!!\n";
    theCommands << "No args to have help.\n";
    return 1;
  }

  bool aGeomCtrl = true;
  int  aCurInd   = 1;
  if (!strcmp(a[1], "-top"))
  {
    aGeomCtrl = false;
    aCurInd++;
  }

  if (aCurInd > narg - 1)
  {
    theCommands << "Invalid number of args!!!\n";
    theCommands << "No args to have help.\n";
    return 1;
  }

  const char*  aShapeName = a[aCurInd];
  TopoDS_Shape aShape     = DBRep::Get(aShapeName);
  if (aShape.IsNull())
  {
    theCommands << a[aCurInd] << " is not a topological shape!!!\n";
    return 1;
  }
  aCurInd++;

  bool        IsShortDump   = false;
  bool        IsContextDump = true;
  bool        IsParallel    = false;
  bool        IsExact       = false;
  const char* aPref(nullptr);
  if (aCurInd < narg && strncmp(a[aCurInd], "-", 1))
  {
    IsContextDump = false;
    aPref         = a[aCurInd];
    aCurInd++;
  }

  for (int anAI = aCurInd; anAI < narg; anAI++)
  {
    TCollection_AsciiString anArg(a[anAI]);
    anArg.LowerCase();
    if (anArg == "-short")
    {
      IsShortDump = true;
    }
    else if (anArg == "-parallel")
    {
      IsParallel = true;
    }
    else if (anArg == "-exact")
    {
      IsExact = true;
    }
    else
    {
      theCommands << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  try
  {
    OCC_CATCH_SIGNALS
    BRepCheck_Analyzer anAna(aShape, aGeomCtrl, IsParallel, IsExact);
    bool               isValid = anAna.IsValid();

    if (isValid)
    {
      if (IsContextDump)
      {
        theCommands << "This shape seems to be valid";
      }
      else
      {
        theCommands << " -- The Shape " << aShapeName << " looks OK";
      }
    }
    else
    {
      if (IsShortDump)
      {
        theCommands << "This shape has faulty shapes";
      }
      else
      {
        if (IsContextDump)
        {
          ContextualDump(theCommands, anAna, aShape);
        }
        else
        {
          StructuralDump(theCommands, anAna, aShapeName, aPref, aShape);
        }
      }
    }
  }
  catch (Standard_Failure const& anException)
  {
    theCommands << "checkshape exception : ";
    theCommands << anException.GetMessageString();
    theCommands << "\n";
    return 1;
  }

  return 0;
}

/***************************************************************/
static void InitEpsSurf(double& epsnl,
                        double& epsdis,
                        double& epsangk1,
                        double& epsangk2,
                        double& epsangn1,
                        double& perce,
                        double& maxlen)
{
  epsnl    = 0.001;
  epsdis   = 0.001;
  epsangk1 = 0.001;
  epsangk2 = 0.001;
  epsangn1 = 0.001;
  perce    = 0.01;
  maxlen   = 10000;
}

static int shapeG1continuity(Draw_Interpretor& di, int n, const char** a)

{
  double epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen;
  int    nbeval;
  InitEpsSurf(epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen);
  bool ISG1 = true;
  if (n < 4)
    return 1;
  TopoDS_Face  face1, face2;
  double       f1, f2, l1, l2;
  TopoDS_Shape shape = DBRep::Get(a[1], TopAbs_SHAPE);
  if (shape.IsNull())
    return 1;
  TopoDS_Shape edge = DBRep::Get(a[2], TopAbs_EDGE);
  if (edge.IsNull())
    return 1;
  // calcul des deux faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    lface;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, lface);
  const NCollection_List<TopoDS_Shape>& lfac = lface.FindFromKey(edge);

  int nelem = lfac.Extent();
  if (nelem != 2)
    return 1;
  NCollection_List<TopoDS_Shape>::Iterator It;
  It.Initialize(lfac);
  face1 = TopoDS::Face(It.Value());
  It.Next();
  face2 = TopoDS::Face(It.Value());

  bool IsSeam = face1.IsEqual(face2);

  // calcul des deux pcurves
  const occ::handle<Geom2d_Curve> c1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face1, f1, l1);
  if (c1.IsNull())
    return 1;

  if (IsSeam)
    edge.Reverse();
  const occ::handle<Geom2d_Curve> c2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face2, f2, l2);
  if (c2.IsNull())
    return 1;

  occ::handle<Geom2d_Curve> curv1 = new Geom2d_TrimmedCurve(c1, f1, l1);

  occ::handle<Geom2d_Curve> curv2 = new Geom2d_TrimmedCurve(c2, f2, l2);

  // calcul dees deux surfaces
  TopLoc_Location                  L1, L2;
  TopoDS_Face                      aLocalFace = face1;
  const occ::handle<Geom_Surface>& s1         = BRep_Tool::Surface(aLocalFace, L1);
  //  const occ::handle<Geom_Surface>& s1 =
  //    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull())
    return 1;
  aLocalFace                          = face2;
  const occ::handle<Geom_Surface>& s2 = BRep_Tool::Surface(aLocalFace, L2);
  //  const occ::handle<Geom_Surface>& s2 =
  //    BRep_Tool::Surface(TopoDS::Face(face2),L2);
  if (s2.IsNull())
    return 1;

  occ::handle<Geom_Surface> surf1 =
    occ::down_cast<Geom_Surface>(s1->Transformed(L1.Transformation()));
  if (surf1.IsNull())
    return 1;
  occ::handle<Geom_Surface> surf2 =
    occ::down_cast<Geom_Surface>(s2->Transformed(L2.Transformation()));
  if (surf2.IsNull())
    return 1;

  nbeval = (int)Draw::Atof(a[3]);

  switch (n)
  {
    case 7:
      epsG1 = Draw::Atof(a[6]);
      [[fallthrough]];
    case 6:
      epsC0 = Draw::Atof(a[5]);
      [[fallthrough]];
    case 5:
      epsnl = Draw::Atof(a[4]);
      [[fallthrough]];
    case 4:
      break;
    default:
      return 1;
  }

  double pard1, parf1, U, Uf, deltaU, nb = 0;
  bool   isconti    = true;
  bool   isdone     = true;
  pard1             = curv1->FirstParameter();
  parf1             = curv1->LastParameter();
  double MaxG0Value = 0, MaxG1Angle = 0;
  U  = std::min(pard1, parf1);
  Uf = std::max(pard1, parf1);

  deltaU = std::abs(parf1 - pard1) / nbeval;

  do
  {
    if (nb == nbeval)
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          Uf,
                                          surf1,
                                          surf2,
                                          GeomAbs_G1,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsG1();
        if (isconti)
        {
          if (res.C0Value() > MaxG0Value)
            MaxG0Value = res.C0Value();
          if (res.G1Angle() > MaxG1Angle)
            MaxG1Angle = res.G1Angle();
        }
      }
      else
        isconti = false;
    }

    else
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          (U + nb * deltaU),
                                          surf1,
                                          surf2,
                                          GeomAbs_G1,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsG1();
        if (nb == 0)
        {
          MaxG0Value = res.C0Value();
          MaxG1Angle = res.G1Angle();
        }
        if (res.C0Value() > MaxG0Value)
          MaxG0Value = res.C0Value();
        if (res.G1Angle() > MaxG1Angle)
          MaxG1Angle = res.G1Angle();
        ;
      }

      else
        isconti = false;
    }
    if (!isconti)
      ISG1 = false;
    nb++;

  } while ((nb < nbeval) && isdone);

  // if (!isdone)  { std::cout<<" Problem in computation "<<std::endl; return 1;}
  // if (ISG1)
  //     {std::cout<<" the continuity is G1 "<<std::endl;}
  // else { std::cout<<" the continuity is not G1  "<<std::endl;}
  // std::cout<<"MaxG0Value :"<< MaxG0Value << std::endl;
  // std::cout<<"MaxG1Angle:"<<  MaxG1Angle << std::endl;
  if (!isdone)
  {
    di << " Problem in computation \n";
    return 1;
  }
  if (ISG1)
  {
    di << " the continuity is G1 \n";
  }
  else
  {
    di << " the continuity is not G1  \n";
  }
  di << "MaxG0Value :" << MaxG0Value << "\n";
  di << "MaxG1Angle:" << MaxG1Angle << "\n";
  return 0;
}

/*****************************************************************************/
static int shapeG0continuity(Draw_Interpretor& di, int n, const char** a)

{
  double epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen;
  int    nbeval;
  bool   ISG0;
  InitEpsSurf(epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen);

  if (n < 4)
    return 1;
  TopoDS_Face  face1, face2;
  double       f1, f2, l1, l2;
  TopoDS_Shape shape = DBRep::Get(a[1], TopAbs_SHAPE);
  if (shape.IsNull())
    return 1;
  TopoDS_Shape edge = DBRep::Get(a[2], TopAbs_EDGE);
  if (edge.IsNull())
    return 1;
  // calcul des deux faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    lface;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, lface);
  const NCollection_List<TopoDS_Shape>& lfac = lface.FindFromKey(edge);

  int nelem = lfac.Extent();
  if (nelem != 2)
    return 1;
  NCollection_List<TopoDS_Shape>::Iterator It;
  It.Initialize(lfac);
  face1 = TopoDS::Face(It.Value());
  It.Next();
  face2 = TopoDS::Face(It.Value());

  bool IsSeam = face1.IsEqual(face2);

  // calcul des deux pcurves
  const occ::handle<Geom2d_Curve> c1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face1, f1, l1);
  if (c1.IsNull())
    return 1;

  if (IsSeam)
    edge.Reverse();
  const occ::handle<Geom2d_Curve> c2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face2, f2, l2);
  if (c2.IsNull())
    return 1;

  occ::handle<Geom2d_Curve> curv1 = new Geom2d_TrimmedCurve(c1, f1, l1);

  occ::handle<Geom2d_Curve> curv2 = new Geom2d_TrimmedCurve(c2, f2, l2);

  // calcul des deux surfaces
  TopLoc_Location                  L1, L2;
  TopoDS_Face                      aLocalFace = face1;
  const occ::handle<Geom_Surface>& s1         = BRep_Tool::Surface(aLocalFace, L1);
  //  const occ::handle<Geom_Surface>& s1 =
  //    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull())
    return 1;
  aLocalFace                          = face2;
  const occ::handle<Geom_Surface>& s2 = BRep_Tool::Surface(aLocalFace, L2);
  //  const occ::handle<Geom_Surface>& s2 =
  //    BRep_Tool::Surface(TopoDS::Face(face2),L2);
  if (s2.IsNull())
    return 1;

  occ::handle<Geom_Surface> surf1 =
    occ::down_cast<Geom_Surface>(s1->Transformed(L1.Transformation()));
  if (surf1.IsNull())
    return 1;
  occ::handle<Geom_Surface> surf2 =
    occ::down_cast<Geom_Surface>(s2->Transformed(L2.Transformation()));
  if (surf2.IsNull())
    return 1;

  nbeval = (int)Draw::Atof(a[3]);

  switch (n)
  {
    case 6:
      epsC0 = Draw::Atof(a[5]);
      [[fallthrough]];
    case 5:
      epsnl = Draw::Atof(a[4]);
      [[fallthrough]];
    case 4:
      break;
    default:
      return 1;
  }

  double pard1, parf1, U, Uf, deltaU, nb = 0;
  bool   isconti    = true;
  bool   isdone     = true;
  pard1             = curv1->FirstParameter();
  parf1             = curv1->LastParameter();
  double MaxG0Value = 0;
  U                 = std::min(pard1, parf1);
  Uf                = std::max(pard1, parf1);

  deltaU = std::abs(parf1 - pard1) / nbeval;
  ISG0   = true;
  do
  {
    if (nb == nbeval)
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          Uf,
                                          surf1,
                                          surf2,
                                          GeomAbs_C0,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsC0();
        if (isconti)
          if (res.C0Value() > MaxG0Value)
            MaxG0Value = res.C0Value();
      }
      else
        isconti = false;
    }

    else
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          (U + nb * deltaU),
                                          surf1,
                                          surf2,
                                          GeomAbs_C0,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsC0();
        if (nb == 0)
        {
          MaxG0Value = res.C0Value();
        }
        if (res.C0Value() > MaxG0Value)
          MaxG0Value = res.C0Value();
      }

      else
        isconti = false;
    }

    nb++;
    if (!isconti)
      ISG0 = false;

  } while ((nb < nbeval) && isdone);

  // f (!isdone)  { std::cout<<" Problem in computation "<<std::endl; return 1;}
  // if (ISG0)
  //     {std::cout<<" the continuity is G0 "<<std::endl;}

  // else { std::cout<<" the continuity is not G0  "<<std::endl;}
  // std::cout<<"MaxG0Value :"<< MaxG0Value << std::endl;
  if (!isdone)
  {
    di << " Problem in computation \n";
    return 1;
  }
  if (ISG0)
  {
    di << " the continuity is G0 \n";
  }

  else
  {
    di << " the continuity is not G0  \n";
  }
  di << "MaxG0Value :" << MaxG0Value << "\n";
  return 0;
}

/*****************************************************************************************/
static int shapeG2continuity(Draw_Interpretor& di, int n, const char** a)

{
  double epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen;
  bool   ISG2 = true;
  int    nbeval;
  double MaxG0Value = 0, MaxG1Angle = 0, MaxG2Curvature = 0;
  InitEpsSurf(epsnl, epsC0, epsC1, epsC2, epsG1, percent, maxlen);

  if (n < 4)
    return 1;
  TopoDS_Face  face1, face2;
  double       f1, f2, l1, l2;
  TopoDS_Shape shape = DBRep::Get(a[1], TopAbs_SHAPE);
  if (shape.IsNull())
    return 1;
  TopoDS_Shape edge = DBRep::Get(a[2], TopAbs_EDGE);
  if (edge.IsNull())
    return 1;
  // calcul des deux faces
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    lface;
  TopExp::MapShapesAndAncestors(shape, TopAbs_EDGE, TopAbs_FACE, lface);
  const NCollection_List<TopoDS_Shape>& lfac = lface.FindFromKey(edge);

  int nelem = lfac.Extent();
  if (nelem != 2)
    return 1;
  NCollection_List<TopoDS_Shape>::Iterator It;
  It.Initialize(lfac);
  face1 = TopoDS::Face(It.Value());
  It.Next();
  face2 = TopoDS::Face(It.Value());

  bool IsSeam = face1.IsEqual(face2);

  // calcul des deux pcurves
  const occ::handle<Geom2d_Curve> c1 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face1, f1, l1);
  if (c1.IsNull())
    return 1;

  if (IsSeam)
    edge.Reverse();
  const occ::handle<Geom2d_Curve> c2 = BRep_Tool::CurveOnSurface(TopoDS::Edge(edge), face2, f2, l2);
  if (c2.IsNull())
    return 1;

  occ::handle<Geom2d_Curve> curv1 = new Geom2d_TrimmedCurve(c1, f1, l1);

  occ::handle<Geom2d_Curve> curv2 = new Geom2d_TrimmedCurve(c2, f2, l2);

  // calcul des deux surfaces
  TopLoc_Location                  L1, L2;
  TopoDS_Face                      aLocalFace = face1;
  const occ::handle<Geom_Surface>& s1         = BRep_Tool::Surface(aLocalFace, L1);
  //  const occ::handle<Geom_Surface>& s1 =
  //    BRep_Tool::Surface(TopoDS::Face(face1),L1);
  if (s1.IsNull())
    return 1;
  aLocalFace                          = face2;
  const occ::handle<Geom_Surface>& s2 = BRep_Tool::Surface(aLocalFace, L2);
  //  const occ::handle<Geom_Surface>& s2 =
  //    BRep_Tool::Surface(TopoDS::Face(face2),L2);
  if (s2.IsNull())
    return 1;

  occ::handle<Geom_Surface> surf1 =
    occ::down_cast<Geom_Surface>(s1->Transformed(L1.Transformation()));
  if (surf1.IsNull())
    return 1;
  occ::handle<Geom_Surface> surf2 =
    occ::down_cast<Geom_Surface>(s2->Transformed(L2.Transformation()));
  if (surf2.IsNull())
    return 1;

  nbeval = (int)Draw::Atof(a[3]);

  switch (n)
  {
    case 9:
      maxlen = Draw::Atof(a[8]);
      [[fallthrough]];
    case 8:
      percent = Draw::Atof(a[7]);
      [[fallthrough]];
    case 7:
      epsG1 = Draw::Atof(a[6]);
      [[fallthrough]];
    case 6:
      epsC0 = Draw::Atof(a[5]);
      [[fallthrough]];
    case 5:
      epsnl = Draw::Atof(a[4]);
      [[fallthrough]];
    case 4:
      break;
    default:
      return 1;
  }

  double pard1, parf1, U, Uf, deltaU, nb = 0;
  bool   isconti = true;
  bool   isdone  = true;
  pard1          = curv1->FirstParameter();
  parf1          = curv1->LastParameter();
  U              = std::min(pard1, parf1);
  Uf             = std::max(pard1, parf1);

  deltaU = std::abs(parf1 - pard1) / nbeval;

  do
  {
    if (nb == nbeval)
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          Uf,
                                          surf1,
                                          surf2,
                                          GeomAbs_G2,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsG2();
        if (isconti)
        {
          if (res.C0Value() > MaxG0Value)
            MaxG0Value = res.C0Value();
          if (res.G1Angle() > MaxG1Angle)
            MaxG1Angle = res.G1Angle();
          if (res.G2CurvatureGap() > MaxG2Curvature)
            MaxG2Curvature = res.G2CurvatureGap();
        }
      }
      else
        isconti = false;
    }

    else
    {
      LocalAnalysis_SurfaceContinuity res(curv1,
                                          curv2,
                                          (U + nb * deltaU),
                                          surf1,
                                          surf2,
                                          GeomAbs_G2,
                                          epsnl,
                                          epsC0,
                                          epsC1,
                                          epsC2,
                                          epsG1,
                                          percent,
                                          maxlen);
      isdone = res.IsDone();
      if (isdone)
      {
        isconti = res.IsG2();
        if (nb == 0)
        {
          MaxG0Value     = res.C0Value();
          MaxG1Angle     = res.G1Angle();
          MaxG2Curvature = res.G2CurvatureGap();
        }
        if (res.C0Value() > MaxG0Value)
          MaxG0Value = res.C0Value();
        if (res.G1Angle() > MaxG1Angle)
          MaxG1Angle = res.G1Angle();
        if (res.G2CurvatureGap() > MaxG2Curvature)
          MaxG2Curvature = res.G2CurvatureGap();
      }
      else
        isconti = false;
    }

    nb++;
    if (!isconti)
      ISG2 = false;

  } while ((nb < nbeval) && isdone);

  // if (!isdone)  { std::cout<<" Problem in computation "<<std::endl; return 1;}
  // if (ISG2)
  // std::cout<<" the continuity is G2 "<<std::endl;
  // else std::cout<<" the continuity is not G2  "<<std::endl;
  // std::cout<<"MaxG0Value :"<< MaxG0Value << std::endl;
  // std::cout<<"MaxG1Angle:"<<  MaxG1Angle << std::endl;
  // std::cout<<"MaxG2Curvature:"<<MaxG2Curvature<<std::endl;
  if (!isdone)
  {
    di << " Problem in computation \n";
    return 1;
  }
  if (ISG2)
    di << " the continuity is G2 \n";
  else
    di << " the continuity is not G2  \n";
  di << "MaxG0Value :" << MaxG0Value << "\n";
  di << "MaxG1Angle:" << MaxG1Angle << "\n";
  di << "MaxG2Curvature:" << MaxG2Curvature << "\n";
  return 0;
}

//=================================================================================================

static int clintedge(Draw_Interpretor& di, int narg, const char** a)
{
  char newname[255];

  if (narg < 2)
  {
    // std::cout << "Usage: clintedge shape" << std::endl;
    di << "Usage: clintedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mymap;
  TopOpeBRepTool_PurgeInternalEdges mypurgealgo(S);
  int                               nbedges = mypurgealgo.NbEdges();
  if (nbedges > 0)
  {
    // std::cout<<nbedges<<" internal (or external) edges to be removed"<<std::endl;
    di << nbedges << " internal (or external) edges to be removed\n";

    int   i    = 1;
    char* temp = newname;

    Sprintf(newname, "%s_%d", a[1], i);
    DBRep::Set(temp, mypurgealgo.Shape());
    // std::cout<<newname<<" ";
    di << newname << " ";

    // std::cout<<std::endl;
    di << "\n";
  }
  else
    di << "no internal (or external) edges\n";
  // std::cout << "no internal (or external) edges"<<std::endl;

  return 0;
}

//=================================================================================================

static int facintedge(Draw_Interpretor& di, int narg, const char** a)
{
  char newname[255];

  if (narg < 2)
  {
    // std::cout << "Usage: facintedge shape" << std::endl;
    di << "Usage: facintedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> mymap;
  TopOpeBRepTool_PurgeInternalEdges mypurgealgo(S);
  mypurgealgo.Faces(mymap);

  int   i    = 1;
  char* temp = newname;

  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itFacEdg;
  for (itFacEdg.Initialize(mymap); itFacEdg.More(); itFacEdg.Next())
  {
    Sprintf(newname, "%s_%d", a[1], i);
    DBRep::Set(temp, itFacEdg.Key());
    // std::cout<<newname<<" ";
    di << newname << " ";
    i++;
  }

  // std::cout<<std::endl;
  di << "\n";

  return 0;
}

//=================================================================================================

static int fuseedge(Draw_Interpretor& di, int narg, const char** a)
{
  char newname[255];

  if (narg < 2)
  {
    // std::cout << "Usage: fuseedge shape" << std::endl;
    di << "Usage: fuseedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>> mymap;
  // TopOpeBRepTool_FuseEdges myfusealgo(S);
  BRepLib_FuseEdges myfusealgo(S);
  myfusealgo.SetConcatBSpl();
  int nbvertices;
  nbvertices = myfusealgo.NbVertices();

  if (nbvertices > 0)
  {

    // std::cout<<nbvertices<<" vertices to be removed"<<std::endl;
    di << nbvertices << " vertices to be removed\n";

    int   i    = 1;
    char* temp = newname;

    Sprintf(newname, "%s_%d", a[1], i);
    DBRep::Set(temp, myfusealgo.Shape());
    // std::cout<<newname<<" ";
    di << newname << " ";

    // std::cout<<std::endl;
    di << "\n";
  }
  else
    di << "no vertices to remove\n";
  // std::cout << "no vertices to remove"<<std::endl;

  return 0;
}

//=================================================================================================

static int listfuseedge(Draw_Interpretor& di, int narg, const char** a)
{
  char newname[255];

  if (narg < 2)
  {
    // std::cout << "Usage: listfuseedge shape" << std::endl;
    di << "Usage: listfuseedge shape\n";
    return 1;
  }
  TopoDS_Shape S = DBRep::Get(a[1]);

  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>> mymap;
  BRepLib_FuseEdges                                        myfusealgo(S);
  myfusealgo.Edges(mymap);

  int   i;
  char* temp = newname;

  NCollection_DataMap<int, NCollection_List<TopoDS_Shape>>::Iterator itLstEdg;
  for (itLstEdg.Initialize(mymap); itLstEdg.More(); itLstEdg.Next())
  {
    const int&                               iLst    = itLstEdg.Key();
    const NCollection_List<TopoDS_Shape>&    LmapEdg = mymap.Find(iLst);
    NCollection_List<TopoDS_Shape>::Iterator itEdg;
    i = 1;
    for (itEdg.Initialize(LmapEdg); itEdg.More(); itEdg.Next())
    {
      Sprintf(newname, "%s_%d_%d", a[1], iLst, i);
      DBRep::Set(temp, itEdg.Value());
      // std::cout<<newname<<" ";
      di << newname << " ";
      i++;
    }
  }

  // std::cout<<std::endl;
  di << "\n";

  return 0;
}

//=================================================================================================

static int tolsphere(Draw_Interpretor& di, int n, const char** a)
{
  if (n != 2)
  {
    di << "use toolsphere shape\n";
    return 1;
  }

  TopoDS_Shape aS = DBRep::Get(a[1]);
  if (aS.IsNull())
  {
    di << "No such shape " << a[1] << "\n";
    return 1;
  }

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapV;
  TopExp::MapShapes(aS, TopAbs_VERTEX, aMapV);
  for (int i = 1; i <= aMapV.Extent(); i++)
  {
    const TopoDS_Vertex&      aV      = TopoDS::Vertex(aMapV.FindKey(i));
    double                    aRadius = BRep_Tool::Tolerance(aV);
    gp_Pnt                    aCenter = BRep_Tool::Pnt(aV);
    occ::handle<Geom_Surface> aSph = new Geom_SphericalSurface(gp_Ax2(aCenter, gp::DZ()), aRadius);
    TCollection_AsciiString   aName(a[1]);
    aName = aName + "_v" + i;
    DrawTrSurf::Set(aName.ToCString(), aSph);
    di << aName << " ";
  }
  return 0;
}

//=================================================================================================

static int validrange(Draw_Interpretor& di, int narg, const char** a)
{
  if (narg < 2)
  {
    di << "usage: validrange edge [(out) u1 u2]";
    return 1;
  }

  TopoDS_Edge aE = TopoDS::Edge(DBRep::Get(a[1], TopAbs_EDGE, true));
  if (aE.IsNull())
    return 1;

  double u1, u2;
  if (BRepLib::FindValidRange(aE, u1, u2))
  {
    if (narg > 3)
    {
      Draw::Set(a[2], u1);
      Draw::Set(a[3], u2);
    }
    else
    {
      di << u1 << " " << u2;
    }
  }
  else
    di << "edge has no valid range";
  return 0;
}

//=================================================================================================

void BRepTest::CheckCommands(Draw_Interpretor& theCommands)
{
  static bool done = false;
  if (done)
    return;
  done = true;

  BRepTest_CheckCommands_SetFaultyName("faulty_");
  DBRep::BasicCommands(theCommands);

  const char* g = "TOPOLOGY Check commands";

  theCommands.Add("checkshape", "checkshape : no args to have help", __FILE__, checkshape, g);

  theCommands.Add("checksection",
                  "checks the closure of a section : checksection name [-r <RefVal>]\n"
                  "\"-r\" - allowed number of alone vertices.",
                  __FILE__,
                  checksection,
                  g);

  theCommands.Add("checkdiff",
                  "checks the validity of the diff between the shapes arg1..argn and result :\n "
                  "checkdiff arg1 [arg2..argn] result [closedSolid (1/0)] [geomCtrl (1/0)]",
                  __FILE__,
                  checkdiff,
                  g);

  g = "TOPOLOGY Analysis of shapes ";

  theCommands.Add("shapeG0continuity",
                  "shapeG0continuity  shape  edge nbeval [epsnul [epsG0]]",
                  __FILE__,
                  shapeG0continuity,
                  g);

  theCommands.Add("shapeG1continuity",
                  "shapeG1continuity  shape  edge nbeval [epsnul [epsG0 [epsG1]]]",
                  __FILE__,
                  shapeG1continuity,
                  g);
  theCommands.Add("shapeG2continuity",
                  "shapeG2continuity shape  edge  nbeval [epsnul [epsG0 [epsG1 [maxlen [perce]]]]]",
                  __FILE__,
                  shapeG2continuity,
                  g);

  theCommands.Add("computetolerance", "computetolerance shape", __FILE__, computetolerance, g);

  theCommands.Add("clintedge", "clintedge shape", __FILE__, clintedge, g);

  theCommands.Add("facintedge", "facintedge shape", __FILE__, facintedge, g);

  theCommands.Add("fuseedge", "fuseedge shape", __FILE__, fuseedge, g);

  theCommands.Add("listfuseedge", "listfuseedge shape", __FILE__, listfuseedge, g);
  theCommands.Add("tolsphere",
                  "toolsphere shape\n"
                  "\t\tshows vertex tolerances by drawing spheres",
                  __FILE__,
                  tolsphere,
                  g);
  theCommands.Add("validrange",
                  "validrange edge [(out) u1 u2]\n"
                  "\t\tcomputes valid range of the edge, and\n"
                  "\t\tprints first and last values or sets the variables u1 and u2",
                  __FILE__,
                  validrange,
                  g);
}
