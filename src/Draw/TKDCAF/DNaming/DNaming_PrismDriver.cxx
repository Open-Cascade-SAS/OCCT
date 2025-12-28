// Created on: 2009-06-16
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
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Wire.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <DNaming.hxx>
#include <DNaming_PrismDriver.hxx>
#include <Geom_Line.hxx>
#include <gp_Vec.hxx>
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
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_PrismDriver, TFunction_Driver)

// OCC
// OCAF
//=================================================================================================

DNaming_PrismDriver::DNaming_PrismDriver() = default;

//=======================================================================
// function : Validate
// purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_PrismDriver::Validate(occ::handle<TFunction_Logbook>&) const {}

//=======================================================================
// function : MustExecute
// purpose  : Analyses in <theLog> if the loaded function must be executed
//=======================================================================
bool DNaming_PrismDriver::MustExecute(const occ::handle<TFunction_Logbook>&) const
{
  return true;
}

#ifdef OCCT_DEBUG
  #include <BRepTools.hxx>

static void Write(const TopoDS_Shape& shape, const char* filename)
{
  std::ofstream save;
  save.open(filename);
  save << "DBRep_DrawableShape" << std::endl << std::endl;
  if (!shape.IsNull())
    BRepTools::Write(shape, save);
  save.close();
}
#endif

//=================================================================================================

int DNaming_PrismDriver::Execute(occ::handle<TFunction_Logbook>& theLog) const
{
  occ::handle<TFunction_Function> aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if (aFunction.IsNull())
    return -1;

  // Save location
  occ::handle<TNaming_NamedShape> aPrevPrism = DNaming::GetFunctionResult(aFunction);
  TopLoc_Location                 aLocation;
  if (!aPrevPrism.IsNull() && !aPrevPrism->IsEmpty())
  {
    aLocation = aPrevPrism->Get().Location();
  }

  // Basis for IPrism
  occ::handle<TDataStd_UAttribute> aBasObject = DNaming::GetObjectArg(aFunction, PRISM_BASIS);
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
    aContextOfBasis = DNaming::GetAttachmentsContext(aBasObject); // a Context of Prism basis
    if (aContextOfBasis.IsNull() || aContextOfBasis->IsEmpty())
    {
      aFunction->SetFailure(WRONG_ARGUMENT);
      return -1;
    }
    anIsAttachment = true;
  }

  // Height
  double aHeight = DNaming::GetReal(aFunction, PRISM_HEIGHT)->Get();
  if (aHeight <= Precision::Confusion())
  {
    aFunction->SetFailure(WRONG_ARGUMENT);
    return -1;
  }

  // Direction
  gp_Ax1 anAxis;
  DNaming::ComputeSweepDir(aBasis, anAxis);

  // Reverse
  int aRev = DNaming::GetInteger(aFunction, PRISM_DIR)->Get();
  if (aRev)
    anAxis.Reverse();

  // Calculate Vec - direction of extrusion
  gp_Vec aVEC(anAxis.Direction());
  aVEC = aVEC * aHeight;

  BRepPrimAPI_MakePrism aMakePrism(aBASIS, aVEC, true);
  aMakePrism.Build();
  if (!aMakePrism.IsDone())
  {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }

  const TopoDS_Shape& aResult = aMakePrism.Shape();
  BRepCheck_Analyzer  aCheckAnalyzer(aResult);
  if (!aCheckAnalyzer.IsValid(aResult))
  {
    aFunction->SetFailure(RESULT_NOT_VALID);
#ifdef OCCT_DEBUG
    const char* aFileName = "PrismResult.brep";
    Write(aResult, aFileName);
#endif
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
    LoadNamingDS(RESPOSITION(aFunction), aMakePrism, aBASIS, aContextOfBasis->Get());
  else
    LoadNamingDS(RESPOSITION(aFunction), aMakePrism, aBASIS, aBASIS);

  // restore location
  if (!aLocation.IsIdentity())
    TNaming::Displace(RESPOSITION(aFunction), aLocation, true);

  theLog->SetValid(RESPOSITION(aFunction), true);
  aFunction->SetFailure(DONE);
  return 0;
}

//=================================================================================================

void DNaming_PrismDriver::LoadNamingDS(const TDF_Label&       theResultLabel,
                                       BRepPrimAPI_MakePrism& MS,
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

  bool makeTopBottom = true;
  if (Basis.ShapeType() == TopAbs_COMPOUND)
  {
    TopoDS_Iterator itr(Basis);
    if (itr.More() && itr.Value().ShapeType() == TopAbs_WIRE)
      makeTopBottom = false;
  }
  else if (Basis.ShapeType() == TopAbs_WIRE)
  {
    makeTopBottom = false;
  }
  if (makeTopBottom)
  {
    // Insert bottom face
    TopoDS_Shape BottomFace = MS.FirstShape();
    if (!BottomFace.IsNull())
    {
      if (BottomFace.ShapeType() != TopAbs_COMPOUND)
      {
        TNaming_Builder BottomBuilder(theResultLabel.NewChild()); // 2
        if (SubShapes.IsBound(BottomFace))
        {
          BottomFace = SubShapes(BottomFace);
        }
        BottomBuilder.Generated(BottomFace);
      }
      else
      {
        TopoDS_Iterator itr(BottomFace);
        for (; itr.More(); itr.Next())
        {
          TNaming_Builder BottomBuilder(theResultLabel.NewChild());
          BottomBuilder.Generated(itr.Value());
        }
      }
    }

    // Insert top face
    TopoDS_Shape TopFace = MS.LastShape();
    if (!TopFace.IsNull())
    {
      if (TopFace.ShapeType() != TopAbs_COMPOUND)
      {
        TNaming_Builder TopBuilder(theResultLabel.NewChild()); // 3
        if (SubShapes.IsBound(TopFace))
        {
          TopFace = SubShapes(TopFace);
        }
        TopBuilder.Generated(TopFace);
      }
      else
      {
        TopoDS_Iterator itr(TopFace);
        for (; itr.More(); itr.Next())
        {
          TNaming_Builder TopBuilder(theResultLabel.NewChild());
          TopBuilder.Generated(itr.Value());
        }
      }
    }
  }
}
