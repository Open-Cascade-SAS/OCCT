// Created on: 2009-06-17
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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <DNaming.hxx>
#include <DNaming_RevolutionDriver.hxx>
#include <Geom_Line.hxx>
#include <GProp_GProps.hxx>
#include <ModelDefinitions.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TDF_TagSource.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Map.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_RevolutionDriver, TFunction_Driver)

// OCAF
//=================================================================================================

DNaming_RevolutionDriver::DNaming_RevolutionDriver() {}

//=======================================================================
// function : Validate
// purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_RevolutionDriver::Validate(occ::handle<TFunction_Logbook>&) const {}

//=======================================================================
// function : MustExecute
// purpose  : Analyses in <theLog> if the loaded function must be executed
//=======================================================================
bool DNaming_RevolutionDriver::MustExecute(const occ::handle<TFunction_Logbook>&) const
{
  return true;
}

//=================================================================================================

int DNaming_RevolutionDriver::Execute(occ::handle<TFunction_Logbook>& theLog) const
{
  occ::handle<TFunction_Function> aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if (aFunction.IsNull())
    return -1;

  // Save location
  occ::handle<TNaming_NamedShape> aPrevRevol = DNaming::GetFunctionResult(aFunction);
  TopLoc_Location                 aLocation;
  if (!aPrevRevol.IsNull() && !aPrevRevol->IsEmpty())
  {
    aLocation = aPrevRevol->Get().Location();
  }

  // Basis for Revol
  occ::handle<TDataStd_UAttribute> aBasObject = DNaming::GetObjectArg(aFunction, REVOL_BASIS);
  occ::handle<TNaming_NamedShape>  aBasisNS   = DNaming::GetObjectValue(aBasObject);
  if (aBasisNS.IsNull() || aBasisNS->IsEmpty())
  {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  const TopoDS_Shape& aBasis = aBasisNS->Get();
  TopoDS_Shape        aBASIS;
  if (aBasis.ShapeType() == TopAbs_WIRE)
  {
    occ::handle<BRepCheck_Wire> aCheck = new BRepCheck_Wire(TopoDS::Wire(aBasis));
    if (aCheck->Closed(true) == BRepCheck_NoError)
    {
      BRepBuilderAPI_MakeFace aMaker(TopoDS::Wire(aBasis), true); // Makes planar face
      if (aMaker.IsDone())
        aBASIS = aMaker.Face(); // aMaker.Face();
    }
  }
  else if (aBasis.ShapeType() == TopAbs_FACE)
    aBASIS = aBasis;
  if (aBASIS.IsNull())
  {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  occ::handle<TNaming_NamedShape> aContextOfBasis;
  bool                            anIsAttachment = false;
  if (DNaming::IsAttachment(aBasObject))
  {
    aContextOfBasis = DNaming::GetAttachmentsContext(aBasObject); // a Context of Revolution basis
    if (aContextOfBasis.IsNull() || aContextOfBasis->IsEmpty())
    {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    anIsAttachment = true;
  }

  // Axis
  occ::handle<TDataStd_UAttribute> anAxObject = DNaming::GetObjectArg(aFunction, REVOL_AXIS);
  occ::handle<TNaming_NamedShape>  anAxNS     = DNaming::GetObjectValue(anAxObject);
  gp_Ax1                           anAXIS;
  TopoDS_Shape                     aTopoDSAxis;
  if (anAxNS.IsNull() || anAxNS->IsEmpty())
  {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }
  else
  {
    aTopoDSAxis   = anAxNS->Get();
    bool anAxisOK = false;
    if (!aTopoDSAxis.IsNull())
    {
      if (aTopoDSAxis.ShapeType() == TopAbs_EDGE || aTopoDSAxis.ShapeType() == TopAbs_WIRE)
      {
        if (aTopoDSAxis.ShapeType() == TopAbs_WIRE)
        {
          TopExp_Explorer anExplorer(aTopoDSAxis, TopAbs_EDGE);
          aTopoDSAxis = anExplorer.Current();
        }
        const TopoDS_Edge&      anEdge = TopoDS::Edge(aTopoDSAxis);
        double                  aFirst, aLast;
        occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
        if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
        {
          occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(aCurve);
          if (!aLine.IsNull())
          {
            anAXIS   = aLine->Position();
            anAxisOK = true;
          }
        }
      }
    }
    if (!anAxisOK)
    {
#ifdef OCCT_DEBUG
      std::cout << "RevolutionDriver:: Axis is not correct" << std::endl;
#endif
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
  }

  if (aFunction->GetDriverGUID() == FULREVOL_GUID)
  {
    BRepPrimAPI_MakeRevol aMakeRevol(aBASIS, anAXIS, true);
    aMakeRevol.Build();
    if (!aMakeRevol.IsDone())
    {
      aFunction->SetFailure(ALGO_FAILED);
      return -1;
    }
    const TopoDS_Shape aResult = aMakeRevol.Shape();
    BRepCheck_Analyzer aCheckAnalyzer(aResult);
    if (!aCheckAnalyzer.IsValid(aResult))
    {
      aFunction->SetFailure(RESULT_NOT_VALID);
      return -1;
    }
    bool aVol = false;
    if (aResult.ShapeType() == TopAbs_SOLID)
      aVol = true;
    else if (aResult.ShapeType() == TopAbs_SHELL)
    {
      occ::handle<BRepCheck_Shell> aCheck = new BRepCheck_Shell(TopoDS::Shell(aResult));
      if (aCheck->Closed() == BRepCheck_NoError)
        aVol = true;
    }
    if (aVol)
    {
      GProp_GProps aGProp;
      BRepGProp::VolumeProperties(aResult, aGProp);
      if (aGProp.Mass() <= Precision::Confusion())
      {
        aFunction->SetFailure(RESULT_NOT_VALID);
        return -1;
      }
    }
    // Naming
    if (anIsAttachment)
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aContextOfBasis->Get());
    else
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aBASIS);
  }
  else if (aFunction->GetDriverGUID() == SECREVOL_GUID)
  {
    double anANGLE = DNaming::GetReal(aFunction, REVOL_ANGLE)->Get();
    if (anANGLE <= Precision::Confusion())
    {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    // Reverse
    int aRev = DNaming::GetInteger(aFunction, REVOL_REV)->Get();
    if (aRev)
      anAXIS.Reverse();

    BRepPrimAPI_MakeRevol aMakeRevol(aBASIS, anAXIS, anANGLE, true);
    aMakeRevol.Build();
    if (!aMakeRevol.IsDone())
    {
      aFunction->SetFailure(ALGO_FAILED);
      return -1;
    }
    const TopoDS_Shape aResult = aMakeRevol.Shape();
    BRepCheck_Analyzer aCheckAnalyzer(aResult);
    if (!aCheckAnalyzer.IsValid(aResult))
    {
      aFunction->SetFailure(RESULT_NOT_VALID);
      return -1;
    }
    bool aVol = false;
    if (aResult.ShapeType() == TopAbs_SOLID)
      aVol = true;
    else if (aResult.ShapeType() == TopAbs_SHELL)
    {
      occ::handle<BRepCheck_Shell> aCheck = new BRepCheck_Shell(TopoDS::Shell(aResult));
      if (aCheck->Closed() == BRepCheck_NoError)
        aVol = true;
    }
    if (aVol)
    {
      GProp_GProps aGProp;
      BRepGProp::VolumeProperties(aResult, aGProp);
      if (aGProp.Mass() <= Precision::Confusion())
      {
        aFunction->SetFailure(RESULT_NOT_VALID);
        return -1;
      }
    }

    // Naming
    if (anIsAttachment)
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aContextOfBasis->Get());
    else
      LoadNamingDS(RESPOSITION(aFunction), aMakeRevol, aBASIS, aBASIS);
  }
  else
  {
    aFunction->SetFailure(UNSUPPORTED_FUNCTION);
    return -1;
  }

  // restore location
  if (!aLocation.IsIdentity())
    TNaming::Displace(RESPOSITION(aFunction), aLocation, true);

  theLog->SetValid(RESPOSITION(aFunction), true);
  aFunction->SetFailure(DONE);
  return 0;
}

//=======================================================================
static void LoadSeamEdge(BRepPrimAPI_MakeRevol& mkRevol,
                         const TDF_Label&       ResultLabel,
                         const TopoDS_Shape&    ShapeIn)

{
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> View;
  TopExp_Explorer                                        ShapeExplorer(ShapeIn, TopAbs_EDGE);
  bool                                                   isFound(false);
  for (; ShapeExplorer.More(); ShapeExplorer.Next())
  {
    const TopoDS_Shape& Root = ShapeExplorer.Current();
    if (!View.Add(Root))
      continue;
    const NCollection_List<TopoDS_Shape>&    Shapes = mkRevol.Generated(Root);
    NCollection_List<TopoDS_Shape>::Iterator ShapesIterator(Shapes);
    for (; ShapesIterator.More(); ShapesIterator.Next())
    {
      TopoDS_Shape newShape = ShapesIterator.Value();
      if (newShape.ShapeType() != TopAbs_FACE)
        continue;
      if (!Root.IsSame(newShape))
      {
        TopExp_Explorer exp(newShape, TopAbs_EDGE);
        for (; exp.More(); exp.Next())
        {
          if (BRep_Tool::IsClosed(TopoDS::Edge(exp.Current()), TopoDS::Face(newShape)))
          {
            TNaming_Builder Builder(ResultLabel.NewChild());
            Builder.Generated(exp.Current());
            isFound = true;
            break;
          }
        }
        if (isFound)
        {
          isFound = false;
          break;
        }
      }
    }
  }
}

//=======================================================================
static bool HasDangle(const TopoDS_Shape& ShapeIn)
{
  if (ShapeIn.ShapeType() == TopAbs_SOLID)
    return false;
  else if (ShapeIn.ShapeType() == TopAbs_SHELL)
  {
    occ::handle<BRepCheck_Shell> aCheck = new BRepCheck_Shell(TopoDS::Shell(ShapeIn));
    return aCheck->Closed() != BRepCheck_NoError;
  }
  else if (ShapeIn.ShapeType() == TopAbs_FACE || ShapeIn.ShapeType() == TopAbs_WIRE
           || ShapeIn.ShapeType() == TopAbs_EDGE || ShapeIn.ShapeType() == TopAbs_VERTEX)
    return true;
  return false;
}

//=======================================================================
static void BuildAtomicMap(const TopoDS_Shape&                                     S,
                           NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& M)
{
  if (S.ShapeType() == TopAbs_COMPOUND || S.ShapeType() == TopAbs_COMPSOLID)
  {
    TopoDS_Iterator it(S);
    for (; it.More(); it.Next())
    {
      if (it.Value().ShapeType() > TopAbs_COMPSOLID)
        M.Add(it.Value());
      else
        BuildAtomicMap(it.Value(), M);
    }
  }
  else
    M.Add(S);
}

//=================================================================================================

bool HasDangleShapes(const TopoDS_Shape& ShapeIn)
{
  if (ShapeIn.ShapeType() == TopAbs_COMPOUND || ShapeIn.ShapeType() == TopAbs_COMPSOLID)
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> M;
    BuildAtomicMap(ShapeIn, M);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator it(M);
    for (; it.More(); it.Next())
      if (HasDangle(it.Key()))
        return true;
  }
  else
    return HasDangle(ShapeIn);
  return false;
}

//=================================================================================================

void DNaming_RevolutionDriver::LoadNamingDS(const TDF_Label&       theResultLabel,
                                            BRepPrimAPI_MakeRevol& MS,
                                            const TopoDS_Shape&    Basis,
                                            const TopoDS_Shape&    Context) const
{

  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> SubShapes;
  for (TopExp_Explorer Exp(MS.Shape(), TopAbs_FACE); Exp.More(); Exp.Next())
  {
    SubShapes.Bind(Exp.Current(), Exp.Current());
  }

  occ::handle<TDF_TagSource> Tagger = TDF_TagSource::Set(theResultLabel);
  if (Tagger.IsNull())
    return;
  Tagger->Set(0);

  TNaming_Builder Builder(theResultLabel);
  if (Basis.IsEqual(Context))
    Builder.Generated(MS.Shape());
  else
    Builder.Generated(Context, MS.Shape());

  // Insert lateral face : Face from Edge
  TNaming_Builder LateralFaceBuilder(theResultLabel.NewChild());
  DNaming::LoadAndOrientGeneratedShapes(MS, Basis, TopAbs_EDGE, LateralFaceBuilder, SubShapes);

  // is full
  TopoDS_Shape StartShape = MS.FirstShape();
  TopoDS_Shape EndShape   = MS.LastShape();
  bool         isFull(false);
  if (!StartShape.IsNull() && !EndShape.IsNull())
    isFull = StartShape.IsEqual(EndShape);

  bool          hasDangle = HasDangleShapes(MS.Shape());
  bool          isBasisClosed(true);
  TopoDS_Vertex Vfirst, Vlast;
  if (Basis.ShapeType() == TopAbs_WIRE)
  {
    occ::handle<BRepCheck_Wire> aCheck = new BRepCheck_Wire(TopoDS::Wire(Basis));
    if (aCheck->Closed() != BRepCheck_NoError)
    {
      isBasisClosed = false; // open
      TopExp::Vertices(TopoDS::Wire(Basis), Vfirst, Vlast);
    }
  }
  else if (Basis.ShapeType() == TopAbs_EDGE)
  {
    BRepBuilderAPI_MakeWire aMakeWire;
    aMakeWire.Add(TopoDS::Edge(Basis));
    if (aMakeWire.IsDone())
    {
      occ::handle<BRepCheck_Wire> aCheck = new BRepCheck_Wire(aMakeWire.Wire());
      if (aCheck->Closed() != BRepCheck_NoError)
      {                        // check for circle case
        isBasisClosed = false; // open
        TopExp::Vertices(TopoDS::Edge(Basis), Vfirst, Vlast);
      }
    }
  }
  if (isFull)
  {
    // seam edge
    LoadSeamEdge(MS, theResultLabel, Basis);

    if (hasDangle)
    {
      if (!isBasisClosed)
      {
        // dangle edges
        const NCollection_List<TopoDS_Shape>&    Shapes = MS.Generated(Vfirst);
        NCollection_List<TopoDS_Shape>::Iterator it(Shapes);
        for (; it.More(); it.Next())
        {
          if (!BRep_Tool::Degenerated(TopoDS::Edge(it.Value())))
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(Vfirst, it.Value());
          }
#ifdef OCCT_DEBUG
          else
          {
            if (MS.HasDegenerated())
              std::cout << "mkRevol has degenerated" << std::endl;
            std::cout << "BRep_Tool found degenerated edge (from Vfirst) TS = "
                      << it.Value().TShape().get() << std::endl;
          }
#endif
        }

        const NCollection_List<TopoDS_Shape>& Shapes2 = MS.Generated(Vlast);
        it.Initialize(Shapes2);
        for (; it.More(); it.Next())
        {
          if (!BRep_Tool::Degenerated(TopoDS::Edge(it.Value())))
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(Vlast, it.Value());
          }
#ifdef OCCT_DEBUG
          else
          {
            if (MS.HasDegenerated())
              std::cout << "mkRevol has degenerated" << std::endl;
            std::cout << "BRep_Tool found degenerated edge (from Vlast) TS = "
                      << it.Value().TShape().get() << std::endl;
          }
#endif
        }
      }
    }
  }
  else
  { // if(!isFull)
    // Insert start shape
    if (!StartShape.IsNull())
    {
      if (StartShape.ShapeType() != TopAbs_COMPOUND)
      {
        TNaming_Builder StartBuilder(theResultLabel.NewChild());
        if (SubShapes.IsBound(StartShape))
        {
          StartShape = SubShapes(StartShape);
        }
        StartBuilder.Generated(StartShape);
        if (StartShape.ShapeType() != TopAbs_FACE)
        {
          TopoDS_Iterator it(StartShape);
          for (; it.More(); it.Next())
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(it.Value());
          }
        }
      }
      else
      {
        TopoDS_Iterator itr(StartShape);
        for (; itr.More(); itr.Next())
        {
          TNaming_Builder StartBuilder(theResultLabel.NewChild());
          StartBuilder.Generated(itr.Value());
        }
      }
    }

    // Insert end shape
    if (!EndShape.IsNull())
    {
      if (EndShape.ShapeType() != TopAbs_COMPOUND)
      {
        TNaming_Builder EndBuilder(theResultLabel.NewChild());
        if (SubShapes.IsBound(EndShape))
        {
          EndShape = SubShapes(EndShape);
        }
        EndBuilder.Generated(EndShape);
        if (EndShape.ShapeType() != TopAbs_FACE)
        {
          TopoDS_Iterator it(EndShape);
          for (; it.More(); it.Next())
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(it.Value());
          }
        }
      }
      else
      {
        TopoDS_Iterator itr(EndShape);
        for (; itr.More(); itr.Next())
        {
          TNaming_Builder EndBuilder(theResultLabel.NewChild());
          EndBuilder.Generated(itr.Value());
        }
      }
    }
    if (hasDangle)
    {
      if (!isBasisClosed)
      {
        // dangle edges
        const NCollection_List<TopoDS_Shape>&    Shapes = MS.Generated(Vfirst);
        NCollection_List<TopoDS_Shape>::Iterator it(Shapes);
        for (; it.More(); it.Next())
        {
          if (!BRep_Tool::Degenerated(TopoDS::Edge(it.Value())))
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(Vfirst, it.Value());
          }
#ifdef OCCT_DEBUG
          else
          {
            if (MS.HasDegenerated())
              std::cout << "mkRevol has degenerated" << std::endl;
            std::cout << "BRep_Tool found degenerated edge (from Vfirst) TS = "
                      << it.Value().TShape().get() << std::endl;
          }
#endif
        }

        const NCollection_List<TopoDS_Shape>& Shapes2 = MS.Generated(Vlast);
        it.Initialize(Shapes2);
        for (; it.More(); it.Next())
        {
          if (!BRep_Tool::Degenerated(TopoDS::Edge(it.Value())))
          {
            TNaming_Builder aBuilder(theResultLabel.NewChild());
            aBuilder.Generated(Vlast, it.Value());
          }
#ifdef OCCT_DEBUG
          else
          {
            if (MS.HasDegenerated())
              std::cout << "mkRevol has degenerated" << std::endl;
            std::cout << "BRep_Tool found degenerated edge (from Vlast) TS = "
                      << it.Value().TShape().get() << std::endl;
          }
#endif
        }
      }
    }
  }
}
