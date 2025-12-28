// Created on: 1998-04-08
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#include <BRepFill_PipeShell.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <GeomFill_PipeError.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <Law_Function.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================

BRepOffsetAPI_MakePipeShell::BRepOffsetAPI_MakePipeShell(const TopoDS_Wire& Spine)
{
  myPipe = new (BRepFill_PipeShell)(Spine);
  SetTolerance();
  SetTransitionMode();
  NotDone();
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMode(const bool IsFrenet)
{
  myPipe->Set(IsFrenet);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetDiscreteMode()
{
  myPipe->SetDiscrete();
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMode(const gp_Ax2& Axe)
{
  myPipe->Set(Axe);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMode(const gp_Dir& BiNormal)
{
  myPipe->Set(BiNormal);
}

//=================================================================================================

bool BRepOffsetAPI_MakePipeShell::SetMode(const TopoDS_Shape& SpineSupport)
{
  return myPipe->Set(SpineSupport);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMode(const TopoDS_Wire&           AuxiliarySpine,
                                          const bool       CurvilinearEquivalence,
                                          const BRepFill_TypeOfContact KeepContact)
{
  myPipe->Set(AuxiliarySpine, CurvilinearEquivalence, KeepContact);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::Add(const TopoDS_Shape&    Profile,
                                      const bool WithContact,
                                      const bool WithCorrection)
{
  myPipe->Add(Profile, WithContact, WithCorrection);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::Add(const TopoDS_Shape&    Profile,
                                      const TopoDS_Vertex&   Location,
                                      const bool WithContact,
                                      const bool WithCorrection)
{
  myPipe->Add(Profile, Location, WithContact, WithCorrection);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetLaw(const TopoDS_Shape&         Profile,
                                         const occ::handle<Law_Function>& L,
                                         const bool      WithContact,
                                         const bool      WithCorrection)
{
  myPipe->SetLaw(Profile, L, WithContact, WithCorrection);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetLaw(const TopoDS_Shape&         Profile,
                                         const occ::handle<Law_Function>& L,
                                         const TopoDS_Vertex&        Location,
                                         const bool      WithContact,
                                         const bool      WithCorrection)
{
  myPipe->SetLaw(Profile, L, Location, WithContact, WithCorrection);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::Delete(const TopoDS_Shape& Profile)
{
  myPipe->DeleteProfile(Profile);
}

//=================================================================================================

bool BRepOffsetAPI_MakePipeShell::IsReady() const
{
  return myPipe->IsReady();
}

//=================================================================================================

BRepBuilderAPI_PipeError BRepOffsetAPI_MakePipeShell::GetStatus() const
{
  GeomFill_PipeError stat;
  stat = myPipe->GetStatus();
  switch (stat)
  {
    case GeomFill_PipeOk: {
      return BRepBuilderAPI_PipeDone;
    }
    case GeomFill_PlaneNotIntersectGuide: {
      return BRepBuilderAPI_PlaneNotIntersectGuide;
    }
    case GeomFill_ImpossibleContact: {
      return BRepBuilderAPI_ImpossibleContact;
    }
    default:
      return BRepBuilderAPI_PipeNotDone;
  }
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetTolerance(const double Tol3d,
                                               const double BoundTol,
                                               const double TolAngular)
{
  myPipe->SetTolerance(Tol3d, BoundTol, TolAngular);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMaxDegree(const int NewMaxDegree)
{
  myPipe->SetMaxDegree(NewMaxDegree);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetMaxSegments(const int NewMaxSegments)
{
  myPipe->SetMaxSegments(NewMaxSegments);
}

//=======================================================================
// function : SetForceApproxC1
// purpose  : Set the flag that indicates attempt to approximate
//           a C1-continuous surface if a swept surface proved
//           to be C0.
//=======================================================================
void BRepOffsetAPI_MakePipeShell::SetForceApproxC1(const bool ForceApproxC1)
{
  myPipe->SetForceApproxC1(ForceApproxC1);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::SetTransitionMode(const BRepBuilderAPI_TransitionMode Mode)
{
  myPipe->SetTransition((BRepFill_TransitionStyle)Mode);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::Simulate(const int N, NCollection_List<TopoDS_Shape>& R)
{
  myPipe->Simulate(N, R);
}

//=================================================================================================

void BRepOffsetAPI_MakePipeShell::Build(const Message_ProgressRange& /*theRange*/)
{
  bool Ok;
  Ok = myPipe->Build();
  if (Ok)
  {
    myShape = myPipe->Shape();
    Done();
  }
  else
    NotDone();
}

//=================================================================================================

bool BRepOffsetAPI_MakePipeShell::MakeSolid()
{
  if (!IsDone())
    throw StdFail_NotDone("BRepOffsetAPI_MakePipeShell::MakeSolid");
  bool Ok;
  Ok = myPipe->MakeSolid();
  if (Ok)
    myShape = myPipe->Shape();
  return Ok;
}

//=================================================================================================

TopoDS_Shape BRepOffsetAPI_MakePipeShell::FirstShape()
{
  return myPipe->FirstShape();
}

//=================================================================================================

TopoDS_Shape BRepOffsetAPI_MakePipeShell::LastShape()
{
  return myPipe->LastShape();
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& BRepOffsetAPI_MakePipeShell::Generated(const TopoDS_Shape& S)
{
  myPipe->Generated(S, myGenerated);
  return myGenerated;
}

//=================================================================================================

double BRepOffsetAPI_MakePipeShell::ErrorOnSurface() const
{
  return myPipe->ErrorOnSurface();
}
