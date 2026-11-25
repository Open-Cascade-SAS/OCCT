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
#include <TCollection_AsciiString.hxx>
#include <XSControl_Controller.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

namespace
{
class TestController : public XSControl_Controller
{
public:
  TestController(const char* const theLongName, const char* const theShortName)
      : XSControl_Controller(theLongName, theShortName)
  {
  }

  occ::handle<Interface_InterfaceModel> NewModel() const override { return nullptr; }

  DEFINE_STANDARD_RTTI_INLINE(TestController, XSControl_Controller)
};
} // namespace

// Record and Recorded work on one process-wide map, so concurrent recordings must all survive.
TEST(XSControl_ControllerTest, ConcurrentRecordKeepsEveryController)
{
  constexpr int aNbThreads   = 8;
  constexpr int aNbPerThread = 250;

  // The controller constructor initialises shared parameters, so build them up front.
  std::vector<occ::handle<TestController>> aControllers;
  std::vector<TCollection_AsciiString>     aNames;
  for (int anIdx = 0; anIdx < aNbThreads * aNbPerThread; ++anIdx)
  {
    const TCollection_AsciiString aName = TCollection_AsciiString("xsctest-") + anIdx;
    aNames.push_back(aName);
    aControllers.push_back(new TestController(aName.ToCString(), aName.ToCString()));
  }

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
        aControllers[anIdx]->Record(aNames[anIdx].ToCString());
        XSControl_Controller::Recorded(aNames[anIdx].ToCString());
      }
    });
  }
  aStart.store(true);
  for (std::thread& aThread : aThreads)
  {
    aThread.join();
  }

  int aMissing = 0;
  for (size_t anIdx = 0; anIdx < aNames.size(); ++anIdx)
  {
    if (XSControl_Controller::Recorded(aNames[anIdx].ToCString()) != aControllers[anIdx])
    {
      ++aMissing;
    }
  }
  EXPECT_EQ(aMissing, 0);
}
