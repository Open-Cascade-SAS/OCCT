// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.

#include <BRepGraphInc_Instance.hxx>

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <vector>

TEST(BRepGraphInc_InstanceTest, EqualityUsesCompleteOccurrenceIdentity)
{
  static_assert(std::is_aggregate_v<BRepGraphInc::FaceInstance>);

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const TopLoc_Location aLocation(aTranslation);

  const BRepGraphInc::FaceInstance anInstance{BRepGraph_FaceId(3), aLocation, TopAbs_REVERSED};
  const BRepGraphInc::FaceInstance anEqual{BRepGraph_FaceId(3), aLocation, TopAbs_REVERSED};
  const BRepGraphInc::FaceInstance aDifferentDefinition{BRepGraph_FaceId(4),
                                                        aLocation,
                                                        TopAbs_REVERSED};
  const BRepGraphInc::FaceInstance aDifferentLocation{BRepGraph_FaceId(3),
                                                      TopLoc_Location(),
                                                      TopAbs_REVERSED};
  const BRepGraphInc::FaceInstance aDifferentOrientation{BRepGraph_FaceId(3),
                                                         aLocation,
                                                         TopAbs_FORWARD};

  EXPECT_EQ(anInstance, anEqual);
  EXPECT_NE(anInstance, aDifferentDefinition);
  EXPECT_NE(anInstance, aDifferentLocation);
  EXPECT_NE(anInstance, aDifferentOrientation);

  std::unordered_set<BRepGraphInc::FaceInstance> anInstances;
  anInstances.insert(anInstance);
  anInstances.insert(anEqual);
  anInstances.insert(aDifferentDefinition);
  anInstances.insert(aDifferentLocation);
  anInstances.insert(aDifferentOrientation);
  EXPECT_EQ(anInstances.size(), 4);
}

TEST(BRepGraphInc_InstanceTest, ReferenceEqualityMatchesHashIdentity)
{
  static_assert(std::is_aggregate_v<BRepGraphInc::FaceRefInstance>);

  const BRepGraphInc::FaceRefInstance aFirst;
  const BRepGraphInc::FaceRefInstance aSecond;
  EXPECT_EQ(aFirst, aSecond);
  EXPECT_EQ(std::hash<BRepGraphInc::FaceRefInstance>{}(aFirst),
            std::hash<BRepGraphInc::FaceRefInstance>{}(aSecond));
}

TEST(BRepGraphInc_InstanceTest, PlacementOrderingIsGenericAndDeterministic)
{
  using Less = BRepGraphInc::InstancePlacementLess<BRepGraph_FaceId>;
  const Less aLess;

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(10.0, 0.0, 0.0));
  const BRepGraphInc::FaceInstance anIdentity{BRepGraph_FaceId(3),
                                              TopLoc_Location(),
                                              TopAbs_FORWARD};
  const BRepGraphInc::FaceInstance anEqual = anIdentity;
  const BRepGraphInc::FaceInstance aTranslated{BRepGraph_FaceId(3),
                                               TopLoc_Location(aTranslation),
                                               TopAbs_FORWARD};
  const BRepGraphInc::FaceInstance aReversed{BRepGraph_FaceId(3),
                                             TopLoc_Location(aTranslation),
                                             TopAbs_REVERSED};
  const BRepGraphInc::FaceInstance aLaterDefinition{BRepGraph_FaceId(4),
                                                    TopLoc_Location(),
                                                    TopAbs_FORWARD};

  EXPECT_FALSE(aLess(anIdentity, anEqual));
  EXPECT_FALSE(aLess(anEqual, anIdentity));
  EXPECT_TRUE(aLess(anIdentity, aTranslated));
  EXPECT_TRUE(aLess(aTranslated, aLaterDefinition));
  EXPECT_EQ(static_cast<int>(TopAbs_FORWARD) < static_cast<int>(TopAbs_REVERSED),
            aLess(aTranslated, aReversed));

  std::vector<BRepGraphInc::FaceInstance> anInstances = {aLaterDefinition,
                                                         aReversed,
                                                         anIdentity,
                                                         aTranslated};
  std::sort(anInstances.begin(), anInstances.end(), aLess);
  EXPECT_TRUE(std::is_sorted(anInstances.begin(), anInstances.end(), aLess));
  EXPECT_EQ(anIdentity, anInstances.front());
  EXPECT_EQ(aLaterDefinition, anInstances.back());
}
