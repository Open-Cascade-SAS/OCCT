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
#include <OpenGl_Workspace.hxx>
#include <OpenGl_Window.hxx>
#include <OpenGl_Aspects.hxx>
#include <OpenGl_Element.hxx>
#include <array>
#include <OpenGl_Flipper.hxx>
#include <OpenGl_StencilTest.hxx>
#include <OpenGl_RenderFilter.hxx>
#include <Graphic3d_Aspects.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <gp_Ax2.hxx>

namespace
{
// Exposes the stencil command's protected destructor for stack allocation.
class OpenGl_TestStencil : public OpenGl_StencilTest
{
public:
  ~OpenGl_TestStencil() override;
};

// Geometry command with a selectable fill mode and no GPU resources.
class OpenGl_TestGeometry : public OpenGl_Element
{
public:
  explicit OpenGl_TestGeometry(bool theFilled);

  bool IsFillDrawMode() const override;

  void Render(const occ::handle<OpenGl_Workspace>&) const override;

  void Release(OpenGl_Context*) override;

private:
  const bool myFilled; // Whether the command draws filled geometry.
};

//==================================================================================================

OpenGl_TestStencil::~OpenGl_TestStencil() = default;

//==================================================================================================

OpenGl_TestGeometry::OpenGl_TestGeometry(bool theFilled)
    : myFilled(theFilled)
{
}

//==================================================================================================

bool OpenGl_TestGeometry::IsFillDrawMode() const
{
  return myFilled;
}

//==================================================================================================

void OpenGl_TestGeometry::Render(const occ::handle<OpenGl_Workspace>&) const {}

//==================================================================================================

void OpenGl_TestGeometry::Release(OpenGl_Context*) {}
} // namespace

TEST(OpenGl_WorkspaceTest, TransparencyPassPreservesAllStateCommands)
{
  OpenGl_Workspace    aWorkspace(nullptr, nullptr);
  OpenGl_Aspects      anAspects;
  OpenGl_Flipper      aFlipper{gp_Ax2()};
  OpenGl_TestStencil  aStencil;
  OpenGl_TestGeometry aLine(false);
  OpenGl_TestGeometry aFace(true);
  aWorkspace.SetRenderFilter(OpenGl_RenderFilter_TransparentOnly);
  for (const OpenGl_Element* anElement :
       std::array<const OpenGl_Element*, 3>{&anAspects, &aFlipper, &aStencil})
  {
    EXPECT_TRUE(aWorkspace.ShouldRender(anElement, nullptr));
  }
  EXPECT_FALSE(aWorkspace.ShouldRender(&aLine, nullptr));
  EXPECT_FALSE(aWorkspace.ShouldRender(&aFace, nullptr));
  anAspects.Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  Graphic3d_MaterialAspect aMaterial = anAspects.Aspect()->FrontMaterial();
  aMaterial.SetTransparency(0.5f);
  anAspects.Aspect()->SetFrontMaterial(aMaterial);
  aWorkspace.SetAspects(&anAspects);
  EXPECT_TRUE(aWorkspace.ShouldRender(&aFace, nullptr));
}

TEST(OpenGl_WorkspaceTest, OpaqueAndUnfilteredPassesKeepTheirGeometryRules)
{
  OpenGl_Workspace    aWorkspace(nullptr, nullptr);
  OpenGl_Aspects      anAspects;
  OpenGl_Flipper      aFlipper{gp_Ax2()};
  OpenGl_TestGeometry aLine(false);
  OpenGl_TestGeometry aFace(true);
  aWorkspace.SetRenderFilter(OpenGl_RenderFilter_OpaqueOnly);
  EXPECT_TRUE(aWorkspace.ShouldRender(&aFlipper, nullptr));
  EXPECT_TRUE(aWorkspace.ShouldRender(&aLine, nullptr));
  EXPECT_TRUE(aWorkspace.ShouldRender(&aFace, nullptr));
  anAspects.Aspect()->SetAlphaMode(Graphic3d_AlphaMode_Blend);
  Graphic3d_MaterialAspect aMaterial = anAspects.Aspect()->FrontMaterial();
  aMaterial.SetTransparency(0.5f);
  anAspects.Aspect()->SetFrontMaterial(aMaterial);
  aWorkspace.SetAspects(&anAspects);
  EXPECT_FALSE(aWorkspace.ShouldRender(&aFace, nullptr));
  EXPECT_EQ(aWorkspace.NbSkippedTransparentElements(), 1);
  aWorkspace.SetRenderFilter(OpenGl_RenderFilter_Empty);
  EXPECT_TRUE(aWorkspace.ShouldRender(&aFace, nullptr));
  EXPECT_TRUE(aWorkspace.ShouldRender(&aLine, nullptr));
}
