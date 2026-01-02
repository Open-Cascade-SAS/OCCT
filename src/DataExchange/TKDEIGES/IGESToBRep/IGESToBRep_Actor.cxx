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

#include <BRepLib.hxx>
#include <IGESData_IGESModel.hxx>
#include <IGESToBRep.hxx>
#include <IGESToBRep_Actor.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_Static.hxx>
#include <Message_ProgressScope.hxx>
#include <ShapeExtend_Explorer.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <TransferBRep_ShapeBinder.hxx>
#include <XSAlgo.hxx>
#include <XSAlgo_ShapeProcessor.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESToBRep_Actor, Transfer_ActorOfTransientProcess)

namespace
{
//=======================================================================
// function : EncodeRegul
// purpose  : INTERNAL to encode regularity on edges
//=======================================================================
static bool EncodeRegul(const TopoDS_Shape& theShape)
{
  const double aToleranceAngle = Interface_Static::RVal("read.encoderegularity.angle");
  if (theShape.IsNull())
  {
    return true;
  }
  if (aToleranceAngle <= 0.)
  {
    return true;
  }

  try
  {
    OCC_CATCH_SIGNALS
    BRepLib::EncodeRegularity(theShape, aToleranceAngle);
  }
  catch (const Standard_Failure&)
  {
    return false;
  }
  return true;
}

//=======================================================================
// function : TrimTolerances
// purpose  : Trims tolerances of the shape according to static parameters
//=======================================================================
static void TrimTolerances(const TopoDS_Shape& theShape, const double theTolerance)
{
  if (Interface_Static::IVal("read.maxprecision.mode") == 1)
  {
    ShapeFix_ShapeTolerance aSFST;
    aSFST.LimitTolerance(theShape,
                         0,
                         std::max(theTolerance, Interface_Static::RVal("read.maxprecision.val")));
  }
}
} // namespace

//=======================================================================

IGESToBRep_Actor::IGESToBRep_Actor()
    : thecontinuity(0),
      theeps(0.0001)
{
}

//=======================================================================

void IGESToBRep_Actor::SetModel(const occ::handle<Interface_InterfaceModel>& model)
{
  themodel = model;
  theeps   = occ::down_cast<IGESData_IGESModel>(themodel)->GlobalSection().Resolution();
}

//=======================================================================

void IGESToBRep_Actor::SetContinuity(const int continuity)
{
  thecontinuity = continuity;
}

//=======================================================================

int IGESToBRep_Actor::GetContinuity() const
{
  return thecontinuity;
}

//=======================================================================

bool IGESToBRep_Actor::Recognize(const occ::handle<Standard_Transient>& start)
{
  DeclareAndCast(IGESData_IGESModel, mymodel, themodel);
  DeclareAndCast(IGESData_IGESEntity, ent, start);
  if (ent.IsNull())
    return false;

  //   Cas reconnus
  int typnum = ent->TypeNumber();
  int fornum = ent->FormNumber();
  return IGESToBRep::IsCurveAndSurface(ent)
      || ((typnum == 402 && (fornum == 1 || fornum == 7 || fornum == 14 || fornum == 15))
          || (typnum == 408) || (typnum == 308));
}

//=======================================================================

occ::handle<Transfer_Binder> IGESToBRep_Actor::Transfer(
  const occ::handle<Standard_Transient>&        start,
  const occ::handle<Transfer_TransientProcess>& TP,
  const Message_ProgressRange&                  theProgress)
{
  DeclareAndCast(IGESData_IGESModel, mymodel, themodel);
  DeclareAndCast(IGESData_IGESEntity, ent, start);
  if (mymodel.IsNull() || ent.IsNull())
    return NullResult();
  int anum = mymodel->Number(start);

  if (Interface_Static::IVal("read.iges.faulty.entities") == 0 && mymodel->IsErrorEntity(anum))
    return NullResult();
  TopoDS_Shape shape;

  // Call the transfer only if type is OK.
  int    typnum = ent->TypeNumber();
  int    fornum = ent->FormNumber();
  double eps;
  if (IGESToBRep::IsCurveAndSurface(ent)
      || (typnum == 402 && (fornum == 1 || fornum == 7 || fornum == 14 || fornum == 15))
      || (typnum == 408) || (typnum == 308))
  {

    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressScope aPS(theProgress, "Transfer stage", 2);

    XSAlgo_ShapeProcessor::PrepareForTransfer();
    IGESToBRep_CurveAndSurface CAS;
    CAS.SetModel(mymodel);
    CAS.SetContinuity(thecontinuity);
    CAS.SetTransferProcess(TP);
    int Ival = Interface_Static::IVal("read.precision.mode");
    if (Ival == 0)
      eps = mymodel->GlobalSection().Resolution();
    else
      eps = Interface_Static::RVal("read.precision.val"); //: 10 ABV 11 Nov 97
    //: 10      eps = BRepAPI::Precision();
    Ival = Interface_Static::IVal("read.iges.bspline.approxd1.mode");
    CAS.SetModeApprox((Ival > 0));
    Ival = Interface_Static::IVal("read.surfacecurve.mode");
    CAS.SetSurfaceCurve(Ival);

    if (eps > 1.E-08)
    {
      CAS.SetEpsGeom(eps);
      theeps = eps * CAS.GetUnitFactor();
    }
    int nbTPitems = TP->NbMapped();
    {
      try
      {
        OCC_CATCH_SIGNALS
        shape = CAS.TransferGeometry(ent, aPS.Next());
      }
      catch (Standard_Failure const&)
      {
        shape.Nullify();
      }
    }

    // fixing shape

    // Set tolerances for shape processing.
    // These parameters are calculated inside IGESToBRep_Actor::Transfer() and cannot be set from
    // outside.
    XSAlgo_ShapeProcessor::ParameterMap aParameters = GetShapeFixParameters();
    XSAlgo_ShapeProcessor::SetParameter("FixShape.Tolerance3d", theeps, true, aParameters);
    XSAlgo_ShapeProcessor::SetParameter("FixShape.MaxTolerance3d",
                                        CAS.GetMaxTol(),
                                        true,
                                        aParameters);

    XSAlgo_ShapeProcessor aShapeProcessor(aParameters);
    shape = aShapeProcessor.ProcessShape(shape, GetProcessingFlags().first, aPS.Next());
    aShapeProcessor.MergeTransferInfo(TP, nbTPitems);
  }

  ShapeExtend_Explorer SBE;
  if (SBE.ShapeType(shape, true) != TopAbs_SHAPE)
  {
    if (!shape.IsNull())
    {
      EncodeRegul(shape);
      // #74 rln 03.03.99 S4135
      TrimTolerances(shape, UsedTolerance());
      //   Shapes().Append(shape);
    }
  }
  occ::handle<TransferBRep_ShapeBinder> binder;
  if (!shape.IsNull())
    binder = new TransferBRep_ShapeBinder(shape);
  return binder;
}

//=============================================================================

double IGESToBRep_Actor::UsedTolerance() const
{
  return theeps;
}
