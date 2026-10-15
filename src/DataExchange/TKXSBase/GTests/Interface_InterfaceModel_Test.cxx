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

#include <Interface_InterfaceModel.hxx>
#include <NCollection_HSequence.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{
class TestModel : public Interface_InterfaceModel
{
public:
  void ClearLabels() override {}

  void ClearHeader() override {}

  void GetFromAnother(const occ::handle<Interface_InterfaceModel>&) override {}

  occ::handle<Interface_InterfaceModel> NewEmptyModel() const override { return new TestModel; }

  void DumpHeader(Standard_OStream&, const int) const override {}

  void PrintLabel(const occ::handle<Standard_Transient>&, Standard_OStream&) const override {}

  occ::handle<TCollection_HAsciiString> StringLabel(
    const occ::handle<Standard_Transient>&) const override
  {
    return new TCollection_HAsciiString("");
  }

  DEFINE_STANDARD_RTTI_INLINE(TestModel, Interface_InterfaceModel)
};
} // namespace

// SetTemplate, HasTemplate and Template work on one process-wide map, so concurrent registrations
// must all survive.
TEST(Interface_InterfaceModelTest, ConcurrentTemplateRegistrationKeepsEveryTemplate)
{
  constexpr int aNbThreads   = 8;
  constexpr int aNbPerThread = 250;

  std::vector<TCollection_AsciiString> aNames;
  for (int anIdx = 0; anIdx < aNbThreads * aNbPerThread; ++anIdx)
  {
    aNames.push_back(TCollection_AsciiString("imtest-") + anIdx);
  }
  const occ::handle<Interface_InterfaceModel> aModel = new TestModel;

  std::atomic<bool>        aStart(false);
  std::vector<std::thread> aThreads;
  for (int aThread = 0; aThread < aNbThreads; ++aThread)
  {
    aThreads.emplace_back([&, aThread]() {
      while (!aStart.load())
      {
      }
      for (int anIdx = aThread * aNbPerThread; anIdx < (aThread + 1) * aNbPerThread; ++anIdx)
      {
        Interface_InterfaceModel::SetTemplate(aNames[anIdx].ToCString(), aModel);
        Interface_InterfaceModel::Template(aNames[anIdx].ToCString());
      }
    });
  }
  aStart.store(true);
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  int aMissing = 0;
  for (const TCollection_AsciiString& aName : aNames)
  {
    if (!Interface_InterfaceModel::HasTemplate(aName.ToCString()))
    {
      ++aMissing;
    }
  }
  EXPECT_EQ(aMissing, 0);
  EXPECT_GE(Interface_InterfaceModel::ListTemplates()->Length(), aNbThreads * aNbPerThread);
}
