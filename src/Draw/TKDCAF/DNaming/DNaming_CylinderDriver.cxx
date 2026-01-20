// Created on: 2009-05-04
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

#include <BRepAdaptor_Curve.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <DNaming.hxx>
#include <DNaming_CylinderDriver.hxx>
#include <gp_Lin.hxx>
#include <ModelDefinitions.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_CylinderDriver, TFunction_Driver)

//=================================================================================================

DNaming_CylinderDriver::DNaming_CylinderDriver() {}

//=======================================================================
// function : Validate
// purpose  : Validates labels of a function in <log>.
//=======================================================================
void DNaming_CylinderDriver::Validate(occ::handle<TFunction_Logbook>&) const {}

//=======================================================================
// function : MustExecute
// purpose  : Analyse in <log> if the loaded function must be executed
//=======================================================================
bool DNaming_CylinderDriver::MustExecute(const occ::handle<TFunction_Logbook>&) const
{
  return true;
}

//=======================================================================
// function : Execute
// purpose  : Execute the function and push in <log> the impacted labels
//=======================================================================
int DNaming_CylinderDriver::Execute(occ::handle<TFunction_Logbook>& theLog) const
{
  occ::handle<TFunction_Function> aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if (aFunction.IsNull())
    return -1;

  double               aRadius  = DNaming::GetReal(aFunction, CYL_RADIUS)->Get();
  double               aHeight  = DNaming::GetReal(aFunction, CYL_HEIGHT)->Get();
  occ::handle<TDataStd_UAttribute> anObject = DNaming::GetObjectArg(aFunction, CYL_AXIS);
  occ::handle<TNaming_NamedShape>  aNSAxis  = DNaming::GetObjectValue(anObject);
  if (aNSAxis->IsEmpty())
  {
#ifdef OCCT_DEBUG
    std::cout << "CylinderDriver:: Axis is empty" << std::endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  TopoDS_Shape aTopoDSAxis = aNSAxis->Get();
  if (aTopoDSAxis.IsNull())
  {
#ifdef OCCT_DEBUG
    std::cout << "CylinderDriver:: Axis is null" << std::endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }
  // Creation of gp axis (gp_Ax2):
  if (aTopoDSAxis.ShapeType() != TopAbs_EDGE && aTopoDSAxis.ShapeType() != TopAbs_WIRE)
  {
#ifdef OCCT_DEBUG
    std::cout << "CylinderDriver:: Wrong axis, ShapeType = " << aTopoDSAxis.ShapeType()
              << std::endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }

  gp_Ax2 anAxis;
  if (aTopoDSAxis.ShapeType() == TopAbs_WIRE)
  {
    TopExp_Explorer anExplorer(aTopoDSAxis, TopAbs_EDGE);
    aTopoDSAxis = anExplorer.Current();
  }

  BRepAdaptor_Curve aCurveAda(TopoDS::Edge(aTopoDSAxis));
  if (aCurveAda.GetType() == GeomAbs_Line)
  {
    gp_Lin aLin = aCurveAda.Line();
    anAxis      = gp_Ax2(aLin.Location(), aLin.Direction());
    if (!aTopoDSAxis.Infinite())
    {
      TopoDS_Vertex V1, V2;
      TopExp::Vertices(TopoDS::Edge(aTopoDSAxis), V1, V2);
      gp_Pnt aP1 = BRep_Tool::Pnt(V1);
      anAxis.SetLocation(aP1);
    }
  }
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "CylinderDriver:: I don't support wires for a while" << std::endl;
#endif
    aFunction->SetFailure(WRONG_AXIS);
    return -1;
  }

  occ::handle<TNaming_NamedShape> aPrevCyl = DNaming::GetFunctionResult(aFunction);
  // Save location
  TopLoc_Location aLocation;
  if (!aPrevCyl.IsNull() && !aPrevCyl->IsEmpty())
  {
    aLocation = aPrevCyl->Get().Location();
  }

  BRepPrimAPI_MakeCylinder aMakeCylinder(anAxis, aRadius, aHeight);
  aMakeCylinder.Build();
  if (!aMakeCylinder.IsDone())
  {
    aFunction->SetFailure(ALGO_FAILED);
    return -1;
  }

  TopoDS_Shape       aResult = aMakeCylinder.Solid();
  BRepCheck_Analyzer aCheck(aResult);
  if (!aCheck.IsValid(aResult))
  {
    aFunction->SetFailure(RESULT_NOT_VALID);
    return -1;
  }

  // Naming
  LoadNamingDS(RESPOSITION(aFunction), aMakeCylinder);

  // restore location
  if (!aLocation.IsIdentity())
    TNaming::Displace(RESPOSITION(aFunction), aLocation, true);

  theLog->SetValid(RESPOSITION(aFunction), true);
  aFunction->SetFailure(DONE);
  return 0;
}

//=================================================================================================

void DNaming_CylinderDriver::LoadNamingDS(const TDF_Label&          theResultLabel,
                                          BRepPrimAPI_MakeCylinder& MS) const
{
  TNaming_Builder Builder(theResultLabel);
  Builder.Generated(MS.Solid());

  BRepPrim_Cylinder& S = MS.Cylinder();

  // Load faces of the Cyl :
  if (S.HasBottom())
  {
    TopoDS_Face     BottomFace = S.BottomFace();
    TNaming_Builder BOF(theResultLabel.FindChild(1, true));
    BOF.Generated(BottomFace);
  }

  if (S.HasTop())
  {
    TopoDS_Face     TopFace = S.TopFace();
    TNaming_Builder TOF(theResultLabel.FindChild(2, true));
    TOF.Generated(TopFace);
  }

  TopoDS_Face     LateralFace = S.LateralFace();
  TNaming_Builder LOF(theResultLabel.FindChild(3, true));
  LOF.Generated(LateralFace);

  if (S.HasSides())
  {
    TopoDS_Face     StartFace = S.StartFace();
    TNaming_Builder SF(theResultLabel.FindChild(4, true));
    SF.Generated(StartFace);
    TopoDS_Face     EndFace = S.EndFace();
    TNaming_Builder EF(theResultLabel.FindChild(5, true));
    EF.Generated(EndFace);
  }
}
