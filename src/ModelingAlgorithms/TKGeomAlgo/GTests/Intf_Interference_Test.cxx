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

#include <Intf_Interference.hxx>
#include <Intf_SectionPoint.hxx>
#include <Intf_TangentZone.hxx>
#include <Message_ProgressIndicator.hxx>
#include <Message_ProgressScope.hxx>
#include <Standard_Failure.hxx>

#include <gtest/gtest.h>

namespace
{
// Intf_Interference's constructor is protected; expose it for the test.
class TestInterference : public Intf_Interference
{
public:
  TestInterference()
      : Intf_Interference(true)
  {
  }
};

// Always signals UserBreak(), regardless of Show()/position.
class AlwaysBreakIndicator : public Message_ProgressIndicator
{
public:
  bool UserBreak() override { return true; }

  void Show(const Message_ProgressScope&, const bool) override {}
  DEFINE_STANDARD_RTTI_INLINE(AlwaysBreakIndicator, Message_ProgressIndicator)
};

// Never signals UserBreak().
class NeverBreakIndicator : public Message_ProgressIndicator
{
public:
  bool UserBreak() override { return false; }

  void Show(const Message_ProgressScope&, const bool) override {}
  DEFINE_STANDARD_RTTI_INLINE(NeverBreakIndicator, Message_ProgressIndicator)
};

// theOffset keeps distinct calls geometrically disjoint, so they accumulate
// as separate zones instead of merging into one.
Intf_TangentZone MakeZoneOfSize(int theN, double theOffset)
{
  Intf_TangentZone aZone;
  for (int i = 0; i < theN; ++i)
  {
    aZone.Append(Intf_SectionPoint(gp_Pnt(theOffset + i, 0.0, 0.0),
                                   Intf_VERTEX,
                                   1,
                                   0,
                                   0.0,
                                   Intf_VERTEX,
                                   1,
                                   0,
                                   0.0,
                                   0.0));
  }
  return aZone;
}

// Enough zones that Insert()'s poll counter (every 256 calls) has a
// realistic chance to fire within the test, without depending on timing.
void SeedManyZones(TestInterference& theInterference, int theCount)
{
  for (int i = 0; i < theCount; ++i)
  {
    theInterference.Insert(MakeZoneOfSize(3, i * 10.0));
  }
}
} // namespace

// With no breaker set (the default), Insert() behaves exactly as before:
// no exception, regardless of call volume.
TEST(Intf_Interference, Insert_NoBreaker_DoesNotThrow)
{
  TestInterference anInterference;
  EXPECT_NO_THROW(SeedManyZones(anInterference, 600));
}

// A breaker that never signals UserBreak() must not affect Insert().
TEST(Intf_Interference, Insert_NonTrippingBreaker_DoesNotThrow)
{
  Handle(NeverBreakIndicator)   anIndicator = new NeverBreakIndicator;
  Message_ProgressRange         aRange      = anIndicator->Start();
  Message_ProgressScope         aScope(aRange, nullptr, 1);
  Intf_InterferenceBreakerScope aGuard(&aScope);

  TestInterference anInterference;
  EXPECT_NO_THROW(SeedManyZones(anInterference, 600));
}

// A breaker that always signals UserBreak() must abort Insert() by
// throwing Standard_Failure, within the polling grain (some multiple of
// 256 calls), not run all 600 insertions to completion.
TEST(Intf_Interference, Insert_TrippedBreaker_Throws)
{
  Handle(AlwaysBreakIndicator)  anIndicator = new AlwaysBreakIndicator;
  Message_ProgressRange         aRange      = anIndicator->Start();
  Message_ProgressScope         aScope(aRange, nullptr, 1);
  Intf_InterferenceBreakerScope aGuard(&aScope);

  TestInterference anInterference;
  EXPECT_THROW(SeedManyZones(anInterference, 600), Standard_Failure);
}

// The breaker scope restores the previous (null) breaker on destruction,
// so a later, unguarded Insert() call is unaffected.
TEST(Intf_Interference, BreakerScope_RestoresPreviousOnDestruction)
{
  {
    Handle(AlwaysBreakIndicator)  anIndicator = new AlwaysBreakIndicator;
    Message_ProgressRange         aRange      = anIndicator->Start();
    Message_ProgressScope         aScope(aRange, nullptr, 1);
    Intf_InterferenceBreakerScope aGuard(&aScope);
    // Guard is live and tripped here; not exercised further in this test.
  }

  TestInterference anInterference;
  EXPECT_NO_THROW(SeedManyZones(anInterference, 600));
}
