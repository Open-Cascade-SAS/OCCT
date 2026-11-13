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

#include <Interface_Check.hxx>
#include <Interface_CheckIterator.hxx>
#include <Interface_CheckTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_HAsciiString.hxx>

namespace
{

//! The entity the throwing module is registered for.
class CheckToolTestEntity : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTI_INLINE(CheckToolTestEntity, Standard_Transient)
};

//! Smallest concrete model that can hold an entity.
class CheckToolTestModel : public Interface_InterfaceModel
{
public:
  void ClearLabels() override {}

  void ClearHeader() override {}

  void GetFromAnother(const occ::handle<Interface_InterfaceModel>&) override {}

  occ::handle<Interface_InterfaceModel> NewEmptyModel() const override
  {
    return new CheckToolTestModel;
  }

  void DumpHeader(Standard_OStream&, const int) const override {}

  void PrintLabel(const occ::handle<Standard_Transient>&,
                  Standard_OStream& theStream) const override
  {
    theStream << "test";
  }

  occ::handle<TCollection_HAsciiString> StringLabel(
    const occ::handle<Standard_Transient>&) const override
  {
    return new TCollection_HAsciiString("test");
  }
};

//! A module whose CheckCase always raises, so FillCheck's error-handling frame is observable:
//! guarded, the failure is caught and recorded as a check fail; unguarded, it escapes the call.
class ThrowingModule : public Interface_GeneralModule
{
public:
  void FillSharedCase(const int,
                      const occ::handle<Standard_Transient>&,
                      Interface_EntityIterator&) const override
  {
  }

  void CheckCase(const int,
                 const occ::handle<Standard_Transient>&,
                 const Interface_ShareTool&,
                 occ::handle<Interface_Check>&) const override
  {
    throw Standard_Failure("deliberate failure from ThrowingModule");
  }

  bool NewVoid(const int, occ::handle<Standard_Transient>& entto) const override
  {
    entto = new CheckToolTestEntity;
    return true;
  }

  void CopyCase(const int,
                const occ::handle<Standard_Transient>&,
                const occ::handle<Standard_Transient>&,
                Interface_CopyTool&) const override
  {
  }
};

//! Recognises CheckToolTestEntity and nothing else, so registering it globally cannot change how
//! any other protocol resolves its own types.
class TestProtocol : public Interface_Protocol
{
public:
  int NbResources() const override { return 0; }

  occ::handle<Interface_Protocol> Resource(const int) const override
  {
    return occ::handle<Interface_Protocol>();
  }

  int TypeNumber(const occ::handle<Standard_Type>& atype) const override
  {
    return (atype == STANDARD_TYPE(CheckToolTestEntity)) ? 1 : 0;
  }

  occ::handle<Interface_InterfaceModel> NewModel() const override { return new CheckToolTestModel; }

  bool IsSuitableModel(const occ::handle<Interface_InterfaceModel>& model) const override
  {
    return !occ::down_cast<CheckToolTestModel>(model).IsNull();
  }

  occ::handle<Standard_Transient> UnknownEntity() const override { return new CheckToolTestEntity; }

  bool IsUnknownEntity(const occ::handle<Standard_Transient>&) const override { return false; }
};

//! Registers the throwing module once for the whole test binary. Additive: TypeNumber above only
//! answers for CheckToolTestEntity, so no other protocol's resolution changes.
const occ::handle<TestProtocol>& testProtocol()
{
  static occ::handle<TestProtocol> THE_PROTOCOL;
  if (THE_PROTOCOL.IsNull())
  {
    THE_PROTOCOL = new TestProtocol;
    Interface_GeneralLib::SetGlobal(new ThrowingModule, THE_PROTOCOL);
  }
  return THE_PROTOCOL;
}

occ::handle<CheckToolTestModel> makeModel()
{
  occ::handle<CheckToolTestModel> aModel = new CheckToolTestModel;
  aModel->AddEntity(new CheckToolTestEntity);
  return aModel;
}

} // namespace

//! FillCheck must catch a raising CheckCase and record it, which is what its own try/catch is for.
//! Pins that the guard is real, so the cross-instance test below is not vacuous.
TEST(Interface_CheckTool_Test, FillCheckCatchesARaisingModule)
{
  occ::handle<CheckToolTestModel> aModel = makeModel();
  Interface_CheckTool             aTool(aModel, testProtocol());
  Interface_ShareTool             aShare(aModel, testProtocol());

  occ::handle<Interface_Check> aCheck = new Interface_Check(aModel->Value(1));
  ASSERT_NO_THROW(aTool.FillCheck(aModel->Value(1), aShare, aCheck));
  EXPECT_TRUE(aCheck->HasFailed()) << "the caught failure was not recorded as a check fail";
}

//! The sentinel that disables FillCheck's own try/catch must be per instance.
//!
//! The bulk list builders clear it because they wrap their whole loop, and they do not restore it.
//! While it was a file-scope flag, one tool running a bulk list left error handling off for every
//! other tool in the process, and a later direct FillCheck ran unguarded. This needs no threads,
//! and it fails before the fix with the deliberate failure escaping the call.
TEST(Interface_CheckTool_Test, ErrorHandlingSentinelIsPerInstance)
{
  occ::handle<CheckToolTestModel> aFirstModel = makeModel();
  Interface_CheckTool             aFirstTool(aFirstModel, testProtocol());

  // A bulk list builder: clears the sentinel because it guards the loop itself.
  aFirstTool.CompleteCheckList();

  // A different tool, which has never run a bulk operation and must still guard its own call.
  occ::handle<CheckToolTestModel> aSecondModel = makeModel();
  Interface_CheckTool             aSecondTool(aSecondModel, testProtocol());
  Interface_ShareTool             aSecondShare(aSecondModel, testProtocol());

  occ::handle<Interface_Check> aCheck = new Interface_Check(aSecondModel->Value(1));
  ASSERT_NO_THROW(aSecondTool.FillCheck(aSecondModel->Value(1), aSecondShare, aCheck))
    << "another tool's bulk list operation disabled this tool's error handling";
  EXPECT_TRUE(aCheck->HasFailed());
}
