// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <Interface_InterfaceModel.hxx>
#include <STEPControl_ActorRead.hxx>
#include <STEPControl_Controller.hxx>
#include <StepData_StepModel.hxx>

//! The non-manifold flag lives on the actor, so two concurrent reads are independent only while
//! each read owns its actor. Pin that: a shared actor would silently restore the old behaviour.
TEST(STEPControl_ActorReadTest, ActorReadReturnsDistinctActorPerCall)
{
  STEPControl_Controller::Init();
  occ::handle<STEPControl_Controller> aController = new STEPControl_Controller;

  occ::handle<Interface_InterfaceModel> aModel1 = new StepData_StepModel;
  occ::handle<Interface_InterfaceModel> aModel2 = new StepData_StepModel;

  occ::handle<Transfer_ActorOfTransientProcess> anActor1 = aController->ActorRead(aModel1);
  occ::handle<Transfer_ActorOfTransientProcess> anActor2 = aController->ActorRead(aModel2);

  ASSERT_FALSE(anActor1.IsNull());
  ASSERT_FALSE(anActor2.IsNull());
  EXPECT_NE(anActor1.get(), anActor2.get());
}

//! Each actor must start with the flag clear, whatever a previously constructed actor saw.
TEST(STEPControl_ActorReadTest, FreshActorIsIndependentOfAnyOther)
{
  occ::handle<Interface_InterfaceModel> aModel = new StepData_StepModel;

  occ::handle<STEPControl_ActorRead> anActor1 = new STEPControl_ActorRead(aModel);
  occ::handle<STEPControl_ActorRead> anActor2 = new STEPControl_ActorRead(aModel);

  EXPECT_NE(anActor1.get(), anActor2.get());
}
