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

#include <Graphic3d_LightSet.hxx>
#include <Graphic3d_ShaderManager.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderProgram.hxx>

namespace
{
class Graphic3d_ShaderManagerTestProxy : public Graphic3d_ShaderManager
{
public:
  Graphic3d_ShaderManagerTestProxy()
      : Graphic3d_ShaderManager(Aspect_GraphicsLibrary_OpenGL)
  {
    SetGapiVersion(3, 2);
  }

  occ::handle<Graphic3d_ShaderProgram> StdProgramUnlit(int theBits) const
  {
    return getStdProgramUnlit(theBits, false);
  }

  occ::handle<Graphic3d_ShaderProgram> StdProgramGouraud(
    const occ::handle<Graphic3d_LightSet>& theLights,
    int                                    theBits) const
  {
    return getStdProgramGouraud(theLights, theBits);
  }

  occ::handle<Graphic3d_ShaderProgram> StdProgramPhong(
    const occ::handle<Graphic3d_LightSet>& theLights,
    int                                    theBits) const
  {
    return getStdProgramPhong(theLights, theBits, false, false, 0);
  }
};

bool hasShaderSource(const occ::handle<Graphic3d_ShaderProgram>& theProgram,
                     Graphic3d_TypeOfShaderObject                theType,
                     const TCollection_AsciiString&              theSnippet)
{
  for (NCollection_Sequence<occ::handle<Graphic3d_ShaderObject>>::Iterator anIter(
         theProgram->ShaderObjects());
       anIter.More();
       anIter.Next())
  {
    const occ::handle<Graphic3d_ShaderObject>& aShader = anIter.Value();
    if (!aShader.IsNull() && aShader->Type() == theType && aShader->Source().Search(theSnippet) > 0)
    {
      return true;
    }
  }
  return false;
}
} // namespace

TEST(Graphic3d_ShaderManagerTest, UnlitFrontOnlyVertexColor_UsesBackColor)
{
  Graphic3d_ShaderManagerTestProxy aManager;

  const occ::handle<Graphic3d_ShaderProgram> aDefaultProgram =
    aManager.StdProgramUnlit(Graphic3d_ShaderFlags_VertColor);
  const occ::handle<Graphic3d_ShaderProgram> aFrontOnlyProgram = aManager.StdProgramUnlit(
    Graphic3d_ShaderFlags_VertColor | Graphic3d_ShaderFlags_VertColorFrontOnly);

  ASSERT_FALSE(aDefaultProgram.IsNull());
  ASSERT_FALSE(aFrontOnlyProgram.IsNull());
  EXPECT_NE(aDefaultProgram->GetId(), aFrontOnlyProgram->GetId());
  EXPECT_TRUE(hasShaderSource(aFrontOnlyProgram,
                              Graphic3d_TOS_FRAGMENT,
                              "gl_FrontFacing ? VertColor : getBackColor()"));
  EXPECT_TRUE(hasShaderSource(aFrontOnlyProgram, Graphic3d_TOS_FRAGMENT, "occBackColor"));
}

TEST(Graphic3d_ShaderManagerTest, GouraudFrontOnlyVertexColor_UsesSideAwareHelper)
{
  Graphic3d_ShaderManagerTestProxy           aManager;
  occ::handle<Graphic3d_LightSet>            aLights  = new Graphic3d_LightSet();
  const occ::handle<Graphic3d_ShaderProgram> aProgram = aManager.StdProgramGouraud(
    aLights,
    Graphic3d_ShaderFlags_VertColor | Graphic3d_ShaderFlags_VertColorFrontOnly);

  ASSERT_FALSE(aProgram.IsNull());
  EXPECT_TRUE(
    hasShaderSource(aProgram, Graphic3d_TOS_VERTEX, "theIsFront ? occVertColor : vec4(1.0)"));
  EXPECT_TRUE(
    hasShaderSource(aProgram,
                    Graphic3d_TOS_VERTEX,
                    "BackColor   = computeLighting (aNormal, aView, aPositionWorld, false)"));
}

TEST(Graphic3d_ShaderManagerTest, PhongFrontOnlyVertexColor_UsesSideAwareHelper)
{
  Graphic3d_ShaderManagerTestProxy           aManager;
  occ::handle<Graphic3d_LightSet>            aLights  = new Graphic3d_LightSet();
  const occ::handle<Graphic3d_ShaderProgram> aProgram = aManager.StdProgramPhong(
    aLights,
    Graphic3d_ShaderFlags_VertColor | Graphic3d_ShaderFlags_VertColorFrontOnly);

  ASSERT_FALSE(aProgram.IsNull());
  EXPECT_TRUE(
    hasShaderSource(aProgram, Graphic3d_TOS_FRAGMENT, "theIsFront ? VertColor : vec4(1.0)"));
  EXPECT_TRUE(hasShaderSource(aProgram, Graphic3d_TOS_FRAGMENT, "getVertColor(theIsFront)"));
}
