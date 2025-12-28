// Created on: 2009-05-05
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Section.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Status.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Shared.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepLib.hxx>
#include <DNaming.hxx>
#include <DNaming_BooleanOperationDriver.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <ModelDefinitions.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_BooleanOperationDriver, TFunction_Driver)

static bool FixSameParameter(const TopoDS_Shape& theShape,
                             BRepCheck_Analyzer& theAnalyzer,
                             const bool          bIgnoreNotSPErrors = false);
static void FindSPErrorEdges(const TopoDS_Shape&       theShape,
                             const BRepCheck_Analyzer& theAnalyzer,
                             NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap);

static bool FindOtherErrors(
  const TopoDS_Shape&                                                  theShape,
  const BRepCheck_Analyzer&                                            theAnalyzer,
  const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap);

//=================================================================================================

DNaming_BooleanOperationDriver::DNaming_BooleanOperationDriver() {}

//=======================================================================
// function : Validate
// purpose  : Validates labels of a function in <log>.
//=======================================================================
void DNaming_BooleanOperationDriver::Validate(occ::handle<TFunction_Logbook>&) const {}

//=======================================================================
// function : MustExecute
// purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
bool DNaming_BooleanOperationDriver::MustExecute(const occ::handle<TFunction_Logbook>&) const
{
  return true;
}

//=======================================================================
// function : Execute
// purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
int DNaming_BooleanOperationDriver::Execute(occ::handle<TFunction_Logbook>& theLog) const
{
  occ::handle<TFunction_Function> aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if (aFunction.IsNull())
    return -1;

  //  occ::handle<TDataStd_UAttribute> anObject = DNaming::GetObjectFromFunction(aFunction);
  //  if(anObject.IsNull()) return -1;
  //  occ::handle<TNaming_NamedShape> anObjectNS = DNaming::GetObjectValue(anObject);
  occ::handle<TFunction_Function> aPrevFun = DNaming::GetPrevFunction(aFunction);
  if (aPrevFun.IsNull())
    return -1;
  const TDF_Label&                aLab = RESPOSITION(aPrevFun);
  occ::handle<TNaming_NamedShape> anObjectNS;
  aLab.FindAttribute(TNaming_NamedShape::GetID(), anObjectNS);
  if (anObjectNS.IsNull() || anObjectNS->IsEmpty())
  {
#ifdef OCCT_DEBUG
    std::cout << "BooleanOperationDriver:: Object is empty" << std::endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  occ::handle<TDataStd_UAttribute> aToolObj = DNaming::GetObjectArg(aFunction, BOOL_TOOL);
  occ::handle<TNaming_NamedShape>  aToolNS  = DNaming::GetObjectValue(aToolObj);

  if (aToolNS.IsNull() || aToolNS->IsEmpty())
  {
#ifdef OCCT_DEBUG
    std::cout << "BooleanOperationDriver:: Tool is empty" << std::endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  TopoDS_Shape aTOOL    = aToolNS->Get();
  TopoDS_Shape anOBJECT = anObjectNS->Get();
  if (aTOOL.IsNull() || anOBJECT.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "BooleanOperationDriver:: Tool is null" << std::endl;
#endif
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  bool anIsDone = false;

  // case FUSE
  if (aFunction->GetDriverGUID() == FUSE_GUID)
  {
    BRepAlgoAPI_Fuse aMkFuse(anOBJECT, aTOOL);
    anIsDone = CheckAndLoad(aMkFuse, aFunction);
  }
  // case CUT
  else if (aFunction->GetDriverGUID() == CUT_GUID)
  {
    BRepAlgoAPI_Cut aMkCut(anOBJECT, aTOOL);
    anIsDone = CheckAndLoad(aMkCut, aFunction);
  }
  // case COMMON
  else if (aFunction->GetDriverGUID() == COMMON_GUID)
  {
    BRepAlgoAPI_Common aMkCom(anOBJECT, aTOOL);
    anIsDone = CheckAndLoad(aMkCom, aFunction);
  }
  // case SECTION
  else if (aFunction->GetDriverGUID() == SECTION_GUID)
  {
    BRepAlgoAPI_Section aMkSect(anOBJECT, aTOOL);
    anIsDone = CheckAndLoad(aMkSect, aFunction);
  }
  else
  {
    aFunction->SetFailure(UNSUPPORTED_FUNCTION);
    return -1;
  }
  if (!anIsDone)
    return -1;
  else
  {
    theLog->SetValid(RESPOSITION(aFunction), true);
    aFunction->SetFailure(DONE);
    return 0;
  }
}

//===================================================================
//=================================================================================================

static TopAbs_ShapeEnum ShapeType(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum TypeSh = theShape.ShapeType();
  if (TypeSh == TopAbs_COMPOUND || TypeSh == TopAbs_COMPSOLID)
  {
    TopoDS_Iterator itr(theShape);
    if (!itr.More())
      return TypeSh;
    TypeSh = ShapeType(itr.Value());
    if (TypeSh == TopAbs_COMPOUND)
      return TypeSh;
    itr.Next();
    for (; itr.More(); itr.Next())
      if (ShapeType(itr.Value()) != TypeSh)
        return TopAbs_COMPOUND;
  }
  return TypeSh;
}

//=====================================================================
static bool IsValidSurfType(const TopoDS_Face& theFace)
{
  BRepAdaptor_Surface          anAdapt(theFace);
  occ::handle<Adaptor3d_Curve> aBasisCurve;
  const GeomAbs_SurfaceType&   aType = anAdapt.GetType();
  if (aType == GeomAbs_Sphere)
    return true;
/*  if(aType == GeomAbs_Cylinder || aType == GeomAbs_Cone || Type == GeomAbs_Sphere)
    return true;
  else if(aType == GeomAbs_SurfaceOfRevolution){
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Line)
      return true;
  }
  else if(aType == GeomAbs_SurfaceOfExtrusion) {
    aBasisCurve = anAdapt.BasisCurve();
    if (aBasisCurve->GetType() == GeomAbs_Circle || aBasisCurve->GetType() == GeomAbs_Ellipse)
      return true;
  }
*/
#ifdef OCCT_DEBUG
  // ModDbgTools_Write(theFace, "Surf");
#endif
  return false;
}

//=================================================================================================

static bool IsWRCase(const BRepAlgoAPI_BooleanOperation& MS)
{

  const TopoDS_Shape&     ObjSh  = MS.Shape1();
  const TopoDS_Shape&     ToolSh = MS.Shape2();
  const TopAbs_ShapeEnum& Type1  = ShapeType(ObjSh);
  if (Type1 == TopAbs_COMPOUND || Type1 > TopAbs_FACE)
    return false;
  const TopAbs_ShapeEnum& Type2 = ShapeType(ToolSh);
  if (Type2 == TopAbs_COMPOUND || Type2 > TopAbs_FACE)
    return false;
  NCollection_List<TopoDS_Shape> aList;

  if (Type1 != TopAbs_FACE)
  {
    TopExp_Explorer anExp(ObjSh, TopAbs_FACE);
    for (; anExp.More(); anExp.Next())
    {
      if (IsValidSurfType(TopoDS::Face(anExp.Current())))
        aList.Append(anExp.Current());
    }
  }
  else if (IsValidSurfType(TopoDS::Face(ObjSh)))
    aList.Append(ObjSh);

  if (aList.Extent() == 0)
  {
    if (Type2 != TopAbs_FACE)
    {
      TopExp_Explorer anExp(ToolSh, TopAbs_FACE);
      for (; anExp.More(); anExp.Next())
      {
        if (IsValidSurfType(TopoDS::Face(anExp.Current())))
          aList.Append(anExp.Current());
      }
    }
    else if (IsValidSurfType(TopoDS::Face(ToolSh)))
      aList.Append(ToolSh);
  }
  if (aList.Extent() > 0)
    return true;
  return false;
}

//=================================================================================================

void DNaming_BooleanOperationDriver::LoadNamingDS(const TDF_Label&              theResultLabel,
                                                  BRepAlgoAPI_BooleanOperation& MS) const
{

  const TopoDS_Shape& ResSh  = MS.Shape();
  const TopoDS_Shape& ObjSh  = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();
  if (ResSh.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "LoadFuseNamingDS: The result of the boolean operation is null" << std::endl;
#endif
    return;
  }

  // LoadResult
  DNaming::LoadResult(theResultLabel, MS);

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> SubShapes;
  TopExp_Explorer                                                          Exp(ResSh, TopAbs_FACE);
  for (; Exp.More(); Exp.Next())
  {
    SubShapes.Bind(Exp.Current(), Exp.Current());
  }

  // Naming of modified faces:
  TNaming_Builder modFB(theResultLabel.NewChild()); // FindChild(1,true));
  DNaming::LoadAndOrientModifiedShapes(MS, ObjSh, TopAbs_FACE, modFB, SubShapes);
  DNaming::LoadAndOrientModifiedShapes(MS, ToolSh, TopAbs_FACE, modFB, SubShapes);

  // Naming of deleted faces:
  if (MS.HasDeleted())
  {
    TNaming_Builder delB(theResultLabel.NewChild()); // FindChild(2,true));
    DNaming::LoadDeletedShapes(MS, ObjSh, TopAbs_FACE, delB);
    DNaming::LoadDeletedShapes(MS, ToolSh, TopAbs_FACE, delB);
  }

  if (IsWRCase(MS))
  {
    // Edges
    Exp.Init(ResSh, TopAbs_EDGE);
    for (; Exp.More(); Exp.Next())
    {
      SubShapes.Bind(Exp.Current(), Exp.Current());
    }

    const NCollection_List<TopoDS_Shape>&                  aList = MS.SectionEdges();
    bool                                                   theCase(false);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aView;
    if (aList.Extent() > 0 && aList.Extent() < 3)
      theCase = true;

    NCollection_List<TopoDS_Shape>::Iterator it(aList);
    for (; it.More(); it.Next())
    {
      TopoDS_Shape newShape = it.Value();
      if (SubShapes.IsBound(newShape))
        newShape.Orientation((SubShapes(newShape)).Orientation());
      TNaming_Builder secED(theResultLabel.NewChild());
      secED.Generated(newShape);
      if (theCase)
      {
        TopoDS_Vertex Vfirst, Vlast;
        TopExp::Vertices(TopoDS::Edge(newShape), Vfirst, Vlast, true);
        if (aView.Add(Vfirst))
        {
          TNaming_Builder secV(theResultLabel.NewChild());
          secV.Generated(Vfirst);
        }
        if (aView.Add(Vlast))
        {
          TNaming_Builder secV(theResultLabel.NewChild());
          secV.Generated(Vlast);
        }
      }
    }
  }
}

//=================================================================================================

void DNaming_BooleanOperationDriver::LoadSectionNDS(const TDF_Label&              theResultLabel,
                                                    BRepAlgoAPI_BooleanOperation& MS) const
{

  const TopoDS_Shape& ResSh  = MS.Shape();
  const TopoDS_Shape& ObjSh  = MS.Shape1();
  const TopoDS_Shape& ToolSh = MS.Shape2();
  if (ResSh.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "LoadSectionNamingDS: The result of the boolean operation is null" << std::endl;
#endif
    return;
  }

  // LoadResult
  DNaming::LoadResult(theResultLabel, MS);

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> SubShapes;
  TopExp_Explorer                                                          Exp(ResSh, TopAbs_EDGE);
  for (; Exp.More(); Exp.Next())
  {
    SubShapes.Bind(Exp.Current(), Exp.Current());
  }

  // Naming of modified faces:
  TNaming_Builder genEdB(theResultLabel.NewChild()); // FindChild(1,true));
  DNaming::LoadAndOrientGeneratedShapes(MS, ObjSh, TopAbs_FACE, genEdB, SubShapes);
  DNaming::LoadAndOrientGeneratedShapes(MS, ToolSh, TopAbs_FACE, genEdB, SubShapes);
}

//=======================================================================
// function : CheckAndLoad
// purpose  : checks result of operation and performs Topological Naming
//=======================================================================
bool DNaming_BooleanOperationDriver::CheckAndLoad(
  BRepAlgoAPI_BooleanOperation&          theMkOpe,
  const occ::handle<TFunction_Function>& theFunction) const
{

  if (theMkOpe.IsDone() && !theMkOpe.Shape().IsNull())
  {
    if (theMkOpe.Shape().ShapeType() == TopAbs_COMPOUND)
    {
      if (theMkOpe.Shape().NbChildren() == 0)
      {
        theFunction->SetFailure(NULL_RESULT);
        return false;
      }
    }
    BRepCheck_Analyzer aCheck(theMkOpe.Shape());
    bool               aResIsValid = true;
    if (!aCheck.IsValid(theMkOpe.Shape()))
      aResIsValid = FixSameParameter(theMkOpe.Shape(), aCheck);
    if (aResIsValid)
    {
      if (theFunction->GetDriverGUID() == FUSE_GUID)
      {
        LoadNamingDS(RESPOSITION(theFunction), theMkOpe);
      }
      else if (theFunction->GetDriverGUID() == CUT_GUID)
      {
        LoadNamingDS(RESPOSITION(theFunction), theMkOpe); // the same naming only for case of solids
      }
      else if (theFunction->GetDriverGUID() == COMMON_GUID)
      {
        LoadNamingDS(RESPOSITION(theFunction), theMkOpe);
      }
      else if (theFunction->GetDriverGUID() == SECTION_GUID)
      {
        LoadSectionNDS(RESPOSITION(theFunction), theMkOpe);
      }

      theFunction->SetFailure(DONE);
      return true;
    }
    else
    {
      theFunction->SetFailure(RESULT_NOT_VALID);
      return false;
    }
  }
  theFunction->SetFailure(ALGO_FAILED);
  return false;
}

// ------------------------------------------------------------------------
// static function: FixSameParameter
// purpose:
// ------------------------------------------------------------------------
bool FixSameParameter(const TopoDS_Shape& theShape,
                      BRepCheck_Analyzer& theAnalyzer,
                      const bool          bIgnoreNotSPErrors)
{

  int                                                           bDoFix = true;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aMapE;

  FindSPErrorEdges(theShape, theAnalyzer, aMapE);

  if (!bIgnoreNotSPErrors)
  {
    if (FindOtherErrors(theShape, theAnalyzer, aMapE))
    {
      bDoFix = false;
    }
  }

  if (bDoFix)
  {
    int i = 0;

    for (i = 1; i <= aMapE.Extent(); i++)
    {
      const TopoDS_Shape& aE = aMapE(i);
      BRepLib::SameParameter(aE, Precision::Confusion(), true);
    }

    if (!aMapE.IsEmpty())
    {
      theAnalyzer.Init(theShape);
      return theAnalyzer.IsValid();
    }
  }
  return false;
}

// ------------------------------------------------------------------------
// static function: FindSPErrorEdges
// purpose:
// ------------------------------------------------------------------------
void FindSPErrorEdges(const TopoDS_Shape&                                            theShape,
                      const BRepCheck_Analyzer&                                      theAnalyzer,
                      NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{
  NCollection_List<BRepCheck_Status>::Iterator itl;

  TopoDS_Iterator anIt(theShape);

  for (; anIt.More(); anIt.Next())
  {
    FindSPErrorEdges(anIt.Value(), theAnalyzer, theMap);
  }

  if (theShape.ShapeType() == TopAbs_FACE)
  {
    TopExp_Explorer anExpE(theShape, TopAbs_EDGE);

    for (; anExpE.More(); anExpE.Next())
    {
      const occ::handle<BRepCheck_Result>& aResult = theAnalyzer.Result(anExpE.Current());

      if (aResult.IsNull() || theMap.Contains(anExpE.Current()))
        continue;

      for (aResult->InitContextIterator(); aResult->MoreShapeInContext();
           aResult->NextShapeInContext())
      {
        if (aResult->ContextualShape().IsSame(theShape))
        {
          itl.Initialize(aResult->StatusOnShape());

          for (; itl.More(); itl.Next())
          {
            if ((itl.Value() == BRepCheck_InvalidSameParameterFlag)
                || (itl.Value() == BRepCheck_InvalidCurveOnSurface))
            {
              theMap.Add(anExpE.Current());
              break;
            }
          }
        }
      }
    }
  }
  else if (theShape.ShapeType() == TopAbs_EDGE)
  {
    const occ::handle<BRepCheck_Result>& aResult = theAnalyzer.Result(theShape);
    itl.Initialize(aResult->Status());

    for (; itl.More(); itl.Next())
    {
      if ((itl.Value() == BRepCheck_InvalidSameParameterFlag)
          || (itl.Value() == BRepCheck_InvalidCurveOnSurface))
      {
        theMap.Add(theShape);
        break;
      }
    }
  }
}

// ------------------------------------------------------------------------
// static function: FindOtherErrors
// purpose:
// ------------------------------------------------------------------------
bool FindOtherErrors(const TopoDS_Shape&       theShape,
                     const BRepCheck_Analyzer& theAnalyzer,
                     const NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& theMap)
{

  bool                                         bOtherFound = false;
  NCollection_List<BRepCheck_Status>::Iterator itl;
  TopoDS_Iterator                              anIt(theShape);

  for (; anIt.More(); anIt.Next())
  {
    if (FindOtherErrors(anIt.Value(), theAnalyzer, theMap))
      return true;
  }
  const occ::handle<BRepCheck_Result>& aResult = theAnalyzer.Result(theShape);

  if (!aResult.IsNull())
  {

    if (!theMap.Contains(theShape) && !aResult->Status().IsEmpty())
    {
      if (aResult->Status().First() != BRepCheck_NoError)
      {
        bOtherFound = true;

        //
        TopExp_Explorer anExpF(theShape, TopAbs_FACE);

        for (; anExpF.More(); anExpF.Next())
        {
          TopExp_Explorer anExpE(anExpF.Current(), TopAbs_EDGE);

          for (; anExpE.More(); anExpE.Next())
          {
            const occ::handle<BRepCheck_Result>& aResultE = theAnalyzer.Result(anExpE.Current());

            if (aResultE.IsNull())
              continue;
            bOtherFound = false;

            for (aResultE->InitContextIterator(); aResultE->MoreShapeInContext();
                 aResultE->NextShapeInContext())
            {

              if (aResultE->ContextualShape().IsSame(anExpF.Current())
                  || aResultE->ContextualShape().IsSame(theShape))
              {
                itl.Initialize(aResultE->StatusOnShape());

                if (!itl.More())
                  continue;

                if (itl.Value() != BRepCheck_NoError)
                {
                  if (theMap.Contains(anExpE.Current()))
                  {
                    for (; itl.More(); itl.Next())
                    {

                      if ((itl.Value() != BRepCheck_InvalidSameParameterFlag)
                          && (itl.Value() != BRepCheck_InvalidCurveOnSurface)
                          && (itl.Value() != BRepCheck_NoError))
                      {
                        return true;
                      }
                    }
                  }
                  else
                  {
                    return true;
                  }
                }
              }
            }
          }
        }
        //

        if (!bOtherFound)
        {
          for (aResult->InitContextIterator(); !bOtherFound && aResult->MoreShapeInContext();
               aResult->NextShapeInContext())
          {
            if (!aResult->StatusOnShape().IsEmpty())
            {
              bOtherFound = (aResult->StatusOnShape().First() != BRepCheck_NoError);
            }
          }
        }
      }
      else
      {
        TopAbs_ShapeEnum aType = theShape.ShapeType();

        if ((aType == TopAbs_VERTEX) || (aType == TopAbs_EDGE) || (aType == TopAbs_WIRE)
            || (aType == TopAbs_FACE) || (aType == TopAbs_SHELL))
        {
          for (aResult->InitContextIterator(); aResult->MoreShapeInContext();
               aResult->NextShapeInContext())
          {
            if (!aResult->StatusOnShape().IsEmpty())
            {
              if (aResult->StatusOnShape().First() != BRepCheck_NoError)
              {
                return true;
              }
            }
          }
        }
      }
    }
    else
    {
      itl.Initialize(aResult->Status());

      for (; itl.More(); itl.Next())
      {
        if ((itl.Value() != BRepCheck_InvalidSameParameterFlag)
            && (itl.Value() != BRepCheck_InvalidCurveOnSurface)
            && (itl.Value() != BRepCheck_NoError))
        {
          return true;
        }
      }
    }
  }

  return bOtherFound;
}
