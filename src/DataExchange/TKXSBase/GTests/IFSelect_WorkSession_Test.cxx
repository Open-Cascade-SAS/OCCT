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

#include <IFSelect_Selection.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Check.hxx>
#include <Interface_CopyTool.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_GeneralLib.hxx>
#include <Interface_GeneralModule.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Protocol.hxx>
#include <Interface_ShareTool.hxx>
#include <Standard_Failure.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

namespace
{

//! Smallest concrete model the work session will accept, so a graph can be computed.
class TestModel : public Interface_InterfaceModel
{
public:
  void ClearLabels() override {}

  void ClearHeader() override {}

  void GetFromAnother(const occ::handle<Interface_InterfaceModel>&) override {}

  occ::handle<Interface_InterfaceModel> NewEmptyModel() const override { return new TestModel; }

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

  DEFINE_STANDARD_RTTI_INLINE(TestModel, Interface_InterfaceModel)
};

//! The smallest protocol that lets the session compute a graph. The session only evaluates a
//! selection once a graph exists, and it only builds one when it has a protocol and a model with at
//! least one entity. Entities are shared through a general module registered for the protocol: with
//! no module registered, the library has an empty node list and the graph build dereferences it, so
//! a protocol alone is not enough. `TypeNumber` answers 1 for every type so the module is selected.
class TestProtocol : public Interface_Protocol
{
public:
  int NbResources() const override { return 0; }

  occ::handle<Interface_Protocol> Resource(const int) const override
  {
    return occ::handle<Interface_Protocol>();
  }

  int TypeNumber(const occ::handle<Standard_Type>&) const override { return 1; }

  occ::handle<Interface_InterfaceModel> NewModel() const override { return new TestModel; }

  bool IsSuitableModel(const occ::handle<Interface_InterfaceModel>& theModel) const override
  {
    return !occ::down_cast<TestModel>(theModel).IsNull();
  }

  occ::handle<Standard_Transient> UnknownEntity() const override { return new Standard_Transient; }

  bool IsUnknownEntity(const occ::handle<Standard_Transient>&) const override { return false; }

  DEFINE_STANDARD_RTTI_INLINE(TestProtocol, Interface_Protocol)
};

//! The module the protocol selects. Nothing is shared and nothing is checked: the test entity has
//! no references, so the graph has one node and no edges.
class TestGeneralModule : public Interface_GeneralModule
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
  }

  bool NewVoid(const int, occ::handle<Standard_Transient>& theEntity) const override
  {
    theEntity = new Standard_Transient;
    return true;
  }

  void CopyCase(const int,
                const occ::handle<Standard_Transient>&,
                const occ::handle<Standard_Transient>&,
                Interface_CopyTool&) const override
  {
  }

  DEFINE_STANDARD_RTTI_INLINE(TestGeneralModule, Interface_GeneralModule)
};

//! A selection whose evaluation always raises, so the caller's error-handling frame is observable:
//! guarded, the failure is caught and reported; unguarded, it escapes.
class ThrowingSelection : public IFSelect_Selection
{
public:
  Interface_EntityIterator RootResult(const Interface_Graph&) const override
  {
    throw Standard_Failure("deliberate failure from ThrowingSelection");
  }

  Interface_EntityIterator CompleteResult(const Interface_Graph& theGraph) const override
  {
    return RootResult(theGraph);
  }

  TCollection_AsciiString Label() const override
  {
    return TCollection_AsciiString("throwing selection");
  }

  void FillIterator(IFSelect_SelectionIterator&) const override {}
};

//! A session whose graph is really computed. Without that, EvalSelection returns an empty result
//! before it ever reaches the selection, and a test of its error handling would observe nothing.
occ::handle<IFSelect_WorkSession> makeSessionWithGraph()
{
  // The library reads its modules from a process-wide registry when the protocol is set, so the
  // module has to be registered first, once.
  static const bool THE_IS_REGISTERED = []() {
    Interface_GeneralLib::SetGlobal(new TestGeneralModule, new TestProtocol);
    return true;
  }();
  (void)THE_IS_REGISTERED;

  occ::handle<IFSelect_WorkSession> aSession = new IFSelect_WorkSession;
  aSession->SetProtocol(new TestProtocol);
  occ::handle<TestModel> aModel = new TestModel;
  aModel->AddEntity(new Standard_Transient);
  aSession->SetModel(aModel);
  return aSession;
}

} // namespace

// The error-handling frame is per session, not per process.
//
// IFSelect_WorkSession used a file-scope flag as the sentinel that makes each error-handled
// operation wrap itself in a try exactly once. Because it was shared, disabling error handling on
// one session cleared the sentinel for every other session in the process, so an unrelated
// session's failure escaped instead of being caught and reported.
TEST(IFSelect_WorkSessionTest, ErrorHandleIsPerSession)
{
  occ::handle<IFSelect_WorkSession> aDisabled = makeSessionWithGraph();
  occ::handle<IFSelect_WorkSession> aEnabled  = makeSessionWithGraph();

  ASSERT_TRUE(aDisabled->ComputeGraph());
  ASSERT_TRUE(aEnabled->ComputeGraph());
  ASSERT_TRUE(aEnabled->ErrorHandle());
  aDisabled->SetErrorHandle(false);
  ASSERT_FALSE(aDisabled->ErrorHandle());
  ASSERT_TRUE(aEnabled->ErrorHandle());

  // The second session still handles its own errors, despite the first having turned its own off.
  occ::handle<ThrowingSelection> aSelection = new ThrowingSelection;
  EXPECT_NO_THROW(aEnabled->EvalSelection(aSelection));
}

// Turning error handling off really does let the failure through, so the test above is asserting a
// working guard rather than an absent one.
TEST(IFSelect_WorkSessionTest, ErrorHandleOffLetsFailureEscape)
{
  occ::handle<IFSelect_WorkSession> aSession = makeSessionWithGraph();
  ASSERT_TRUE(aSession->ComputeGraph());
  aSession->SetErrorHandle(false);

  occ::handle<ThrowingSelection> aSelection = new ThrowingSelection;
  EXPECT_THROW(aSession->EvalSelection(aSelection), Standard_Failure);
}
