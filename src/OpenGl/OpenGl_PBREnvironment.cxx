// Author: Ilya Khramov
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <OpenGl_PBREnvironment.hxx>

#include <Graphic3d_PBRMaterial.hxx>
#include <OpenGl_ArbFBO.hxx>
#include <OpenGl_FrameBuffer.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OSD_Timer.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_PBREnvironment, OpenGl_NamedResource)

//! Constructor of this class saves necessary OpenGL states components which can be changed by OpenGl_PBREnvironment.
//! Destructor restores state back.
class OpenGl_PBREnvironmentSentry
{
public:

  OpenGl_PBREnvironmentSentry (const Handle(OpenGl_Context)& theCtx)
  : myContext (theCtx)
  {
    backup();
    prepare();
  }

  ~OpenGl_PBREnvironmentSentry()
  {
    restore();
  }

private:

  void backup()
  {
    myContext->core11fwd->glGetIntegerv (GL_FRAMEBUFFER_BINDING, &myFBO);
    myShaderProgram = myContext->ActiveProgram();
    for (unsigned int i = 0; i < 4; ++i)
    {
      myViewport[i] = myContext->Viewport()[i];
    }
    myContext->core11fwd->glGetFloatv (GL_COLOR_CLEAR_VALUE, myClearColor);

    GLboolean aStatus = GL_TRUE;
    myContext->core11fwd->glGetBooleanv (GL_DEPTH_TEST, &aStatus);
    myDepthTestWasEnabled = aStatus ? Standard_True : Standard_False;
    myContext->core11fwd->glGetBooleanv (GL_DEPTH_WRITEMASK, &aStatus);
    myDepthWrirtingWasEnablig = aStatus ? Standard_True : Standard_False;
    myContext->core11fwd->glGetBooleanv (GL_SCISSOR_TEST, &aStatus);
    myScissorTestWasEnabled = aStatus ? Standard_True : Standard_False;
    myContext->core11fwd->glGetIntegerv (GL_SCISSOR_BOX, myScissorBox);
  }

  void prepare()
  {
    myContext->BindDefaultVao();
    myContext->core11fwd->glDisable (GL_DEPTH_TEST);
    myContext->core11fwd->glDepthMask (GL_FALSE);
    myContext->core11fwd->glDisable (GL_BLEND);
    myContext->core11fwd->glDisable (GL_SCISSOR_TEST);
  }

  void restore()
  {
    myContext->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, myFBO);
    myContext->BindProgram (myShaderProgram);
    myContext->ResizeViewport (myViewport);
    myContext->core11fwd->glClearColor (myClearColor.r(), myClearColor.g(), myClearColor.b(), myClearColor.a());
    if (myDepthTestWasEnabled)
    {
      myContext->core11fwd->glEnable (GL_DEPTH_TEST);
    }
    else
    {
      myContext->core11fwd->glDisable (GL_DEPTH_TEST);
    }
    myContext->core11fwd->glDepthMask (myDepthWrirtingWasEnablig ? GL_TRUE : GL_FALSE);
    if (myScissorTestWasEnabled)
    {
      myContext->core11fwd->glEnable (GL_SCISSOR_TEST);
    }
    else
    {
      myContext->core11fwd->glDisable (GL_SCISSOR_TEST);
    }
    myContext->core11fwd->glScissor (myScissorBox[0], myScissorBox[1], myScissorBox[2], myScissorBox[3]);
  }

private:

  OpenGl_PBREnvironmentSentry            (const OpenGl_PBREnvironmentSentry& );
  OpenGl_PBREnvironmentSentry& operator= (const OpenGl_PBREnvironmentSentry& );

private:

  const Handle(OpenGl_Context) myContext;
  GLint                        myFBO;
  Handle(OpenGl_ShaderProgram) myShaderProgram;
  Standard_Boolean             myDepthTestWasEnabled;
  Standard_Boolean             myDepthWrirtingWasEnablig;
  Standard_Boolean             myScissorTestWasEnabled;
  Standard_Integer             myScissorBox[4];
  Standard_Integer             myViewport[4];
  Graphic3d_Vec4               myClearColor;

};

// =======================================================================
// function : Create
// purpose  :
// =======================================================================
Handle(OpenGl_PBREnvironment) OpenGl_PBREnvironment::Create (const Handle(OpenGl_Context)&  theCtx,
                                                             unsigned int                   thePow2Size,
                                                             unsigned int                   theLevelsNumber,
                                                             const TCollection_AsciiString& theId)
{
  if (theCtx->arbFBO == NULL)
  {
    return Handle(OpenGl_PBREnvironment)();
  }

  Handle(OpenGl_PBREnvironment) anEnvironment = new OpenGl_PBREnvironment (theCtx, thePow2Size, theLevelsNumber, theId);
  if (!anEnvironment->IsComplete())
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PORTABILITY, 0, GL_DEBUG_SEVERITY_MEDIUM,
                         "Warning: PBR environment is not created. PBR material system will be ignored.");
    anEnvironment->Release (theCtx.get());
    anEnvironment.Nullify();
  }

  return anEnvironment;
}

// =======================================================================
// function : OpenGl_PBREnvironment
// purpose  :
// =======================================================================
OpenGl_PBREnvironment::OpenGl_PBREnvironment (const Handle(OpenGl_Context)&  theCtx,
                                              unsigned int                   thePowOf2Size,
                                              unsigned int                   theSpecMapLevelsNumber,
                                              const TCollection_AsciiString& theId)
: OpenGl_NamedResource (theId),
  myPow2Size (std::max (1u, thePowOf2Size)),
  mySpecMapLevelsNumber (std::max (2u, std::min (theSpecMapLevelsNumber, std::max (1u, thePowOf2Size) + 1))),
  myFBO (OpenGl_FrameBuffer::NO_FRAMEBUFFER),
  myIsComplete (Standard_False),
  myIsNeededToBeBound (Standard_True)
{
  OpenGl_PBREnvironmentSentry aSentry (theCtx);

  myIsComplete = initVAO (theCtx)
              && initTextures (theCtx)
              && initFBO (theCtx);

  if (myIsComplete)
  {
    clear (theCtx);
  }
}

// =======================================================================
// function : Bind
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::Bind (const Handle(OpenGl_Context)& theCtx)
{
  myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Bind (theCtx);
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Bind (theCtx);
  myIsNeededToBeBound = Standard_False;
}

// =======================================================================
// function : Unbind
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::Unbind (const Handle(OpenGl_Context)& theCtx)
{
  myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Unbind (theCtx);
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Unbind (theCtx);
  myIsNeededToBeBound = Standard_True;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::Clear (const Handle(OpenGl_Context)& theCtx,
                                   const Graphic3d_Vec3&         theColor)
{
  OpenGl_PBREnvironmentSentry aSentry (theCtx);
  clear (theCtx, theColor);
}

// =======================================================================
// function : Bake
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::Bake (const Handle(OpenGl_Context)& theCtx,
                                  const Handle(OpenGl_Texture)& theEnvMap,
                                  Standard_Boolean              theZIsInverted,
                                  Standard_Boolean              theIsTopDown,
                                  Standard_Size                 theDiffMapNbSamples,
                                  Standard_Size                 theSpecMapNbSamples,
                                  Standard_ShortReal            theProbability)
{
  Standard_ProgramError_Raise_if (theEnvMap.IsNull(), "'Bake' function of 'OpenGl_PBREnvironment' can't work without source environment map")
  Standard_RangeError_Raise_if (theProbability > 1.f || theProbability < 0.f, "'probability' parameter in 'Bake' function of 'OpenGl_PBREnvironment' must be in range [0, 1]")
  Unbind (theCtx);
  OpenGl_PBREnvironmentSentry aSentry (theCtx);
  bake (theCtx, theEnvMap, theZIsInverted, theIsTopDown, theDiffMapNbSamples, theSpecMapNbSamples, theProbability);
}

// =======================================================================
// function : SizesAreDifferent
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::SizesAreDifferent (unsigned int thePow2Size,
                                               unsigned int theSpecMapLevelsNumber) const
{
  thePow2Size = std::max (1u, thePow2Size);
  theSpecMapLevelsNumber = std::max (2u, std::min (theSpecMapLevelsNumber, std::max (1u, thePow2Size) + 1));
  return myPow2Size != thePow2Size
      || mySpecMapLevelsNumber != theSpecMapLevelsNumber;
}

// =======================================================================
// function : Release
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::Release (OpenGl_Context* theCtx)
{
  if (myFBO != OpenGl_FrameBuffer::NO_FRAMEBUFFER)
  {
    if (theCtx != NULL
     && theCtx->IsValid())
    {
      theCtx->arbFBO->glDeleteFramebuffers (1, &myFBO);
    }
    myFBO = OpenGl_FrameBuffer::NO_FRAMEBUFFER;
  }
  myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Release(theCtx);
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Release (theCtx);
  myVBO.Release (theCtx);
}

// =======================================================================
// function : ~OpenGl_PBREnvironment
// purpose  :
// =======================================================================
OpenGl_PBREnvironment::~OpenGl_PBREnvironment()
{
  Release (NULL);
}

// =======================================================================
// function : initTextures
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::initTextures (const Handle(OpenGl_Context)& theCtx)
{
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Sampler()->Parameters()->SetTextureUnit (theCtx->PBRSpecIBLMapTexUnit());
  myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Sampler()->Parameters()->SetTextureUnit (theCtx->PBRDiffIBLMapSHTexUnit());
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Sampler()->Parameters()->SetFilter (Graphic3d_TOTF_TRILINEAR);
  myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Sampler()->Parameters()->SetFilter(Graphic3d_TOTF_NEAREST);
  myIBLMaps[OpenGl_TypeOfIBLMap_Specular] .Sampler()->Parameters()->SetLevelsRange (mySpecMapLevelsNumber - 1);

  // NVIDIA's driver didn't work properly with 3 channel texture for diffuse SH coefficients so that alpha channel has been added
  return myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].Init (theCtx,
                                                        OpenGl_TextureFormat::FindFormat (theCtx, Image_Format_RGBAF, false),
                                                        Graphic3d_Vec2i (9, 1),
                                                        Graphic3d_TOT_2D)
      && myIBLMaps[OpenGl_TypeOfIBLMap_Specular].InitCubeMap (theCtx, Handle(Graphic3d_CubeMap)(),
                                                              Standard_Size(1) << myPow2Size, Image_Format_RGB, true, false);
}

// =======================================================================
// function : initVAO
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::initVAO (const Handle(OpenGl_Context)& theCtx)
{
  const float aVertexPos[] =
  {
    -1.f, -1.f, 0.f, 0.f,
     1.f, -1.f, 0.f, 0.f,
    -1.f,  1.f, 0.f, 0.f,
     1.f,  1.f, 0.f, 0.f
  };
  return myVBO.Init (theCtx, 4, 4, aVertexPos);
}

// =======================================================================
// function : initFBO
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::initFBO (const Handle(OpenGl_Context)& theCtx)
{
  theCtx->arbFBO->glGenFramebuffers (1, &myFBO);
  return checkFBOComplentess (theCtx);
}

// =======================================================================
// function : processDiffIBLMap
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::processDiffIBLMap (const Handle(OpenGl_Context)& theCtx,
                                               Standard_Boolean              theIsDrawAction,
                                               Standard_Size                 theNbSamples)
{
  theCtx->arbFBO->glFramebufferTexture2D (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                          myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].TextureId(), 0);
  const Standard_Integer aViewport[4] = { 0, 0, 9, 1 };
  theCtx->ResizeViewport(aViewport);
  if (theIsDrawAction)
  {
    theCtx->ActiveProgram()->SetUniform(theCtx, "occNbSpecIBLLevels", 0);
    theCtx->ActiveProgram()->SetUniform(theCtx, "uSamplesNum", static_cast<Standard_Integer>(theNbSamples));

    theCtx->core11fwd->glDrawArrays (GL_TRIANGLE_STRIP, 0, 4);
  }
  else
  {
    theCtx->core11fwd->glClear (GL_COLOR_BUFFER_BIT);

    theCtx->core11fwd->glEnable (GL_SCISSOR_TEST);
    theCtx->core11fwd->glClearColor (0.f, 0.f, 0.f, 1.f);
    theCtx->core11fwd->glScissor (1, 0, 8, 1);
    theCtx->core11fwd->glClear (GL_COLOR_BUFFER_BIT);
  }

  return true;
}

// =======================================================================
// function : processSpecIBLMap
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::processSpecIBLMap (const Handle(OpenGl_Context)& theCtx,
                                               Standard_Boolean              theIsDrawAction,
                                               Standard_Integer              theEnvMapSize,
                                               Standard_Size                 theNbSamples,
                                               Standard_ShortReal            theProbability)
{
  if (theIsDrawAction)
  {
    theCtx->ActiveProgram()->SetUniform (theCtx, "occNbSpecIBLLevels", Standard_Integer(mySpecMapLevelsNumber));
    theCtx->ActiveProgram()->SetUniform (theCtx, "uEnvMapSize", theEnvMapSize);
  }

  for (int aLevelIter = mySpecMapLevelsNumber - 1;; --aLevelIter)
  {
    const Standard_Integer aSize = 1 << (myPow2Size - aLevelIter);
    const Standard_Integer aViewport[4] = { 0, 0, aSize, aSize };
    theCtx->ResizeViewport (aViewport);
    if (theIsDrawAction)
    {
      Standard_Integer aNbSamples = static_cast<Standard_Integer>(Graphic3d_PBRMaterial::SpecIBLMapSamplesFactor (theProbability, aLevelIter / float (mySpecMapLevelsNumber - 1)) * theNbSamples);
      theCtx->ActiveProgram()->SetUniform (theCtx, "uSamplesNum", static_cast<Standard_Integer>(aNbSamples));
      theCtx->ActiveProgram()->SetUniform (theCtx, "uCurrentLevel", aLevelIter);
    }

    for (Standard_Integer aSideIter = 0; aSideIter < 6; ++aSideIter)
    {
      theCtx->arbFBO->glFramebufferTexture2D (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + aSideIter,
                                              myIBLMaps[OpenGl_TypeOfIBLMap_Specular].TextureId(), aLevelIter);
      if (theIsDrawAction)
      {
        theCtx->ActiveProgram()->SetUniform(theCtx, "uCurrentSide", aSideIter);
        theCtx->core11fwd->glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      }
      else
      {
        theCtx->core11fwd->glClear(GL_COLOR_BUFFER_BIT);
      }
    }

    if (aLevelIter == 0)
    {
      break;
    }
  }

  return true;
}

// =======================================================================
// function : checkFBOCompletness
// purpose  :
// =======================================================================
bool OpenGl_PBREnvironment::checkFBOComplentess (const Handle(OpenGl_Context)& theCtx) const
{
  theCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, myFBO);
  theCtx->arbFBO->glFramebufferTexture2D (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                          myIBLMaps[OpenGl_TypeOfIBLMap_DiffuseSH].TextureId(), 0);
  if (theCtx->arbFBO->glCheckFramebufferStatus (GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    return false;
  }
  for (Standard_Integer aSideIter = 0; aSideIter < 6; ++aSideIter)
  {
    for (unsigned int aLevel = 0; aLevel < mySpecMapLevelsNumber; ++aLevel)
    {
      theCtx->arbFBO->glFramebufferTexture2D (GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + aSideIter,
                                              myIBLMaps[OpenGl_TypeOfIBLMap_Specular].TextureId(), aLevel);
      if (theCtx->arbFBO->glCheckFramebufferStatus (GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      {
        return false;
      }
    }
  }
  return true;
}

// =======================================================================
// function : bake
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::bake (const Handle(OpenGl_Context)& theCtx,
                                  const Handle(OpenGl_Texture)& theEnvMap,
                                  Standard_Boolean              theZIsInverted,
                                  Standard_Boolean              theIsTopDown,
                                  Standard_Size                 theDiffNbSamples,
                                  Standard_Size                 theSpecNbSamples,
                                  Standard_ShortReal            theProbability)
{
  myIsNeededToBeBound = Standard_True;
  if (!theCtx->ShaderManager()->BindPBREnvBakingProgram())
  {
    return;
  }
  theEnvMap->Bind (theCtx, theCtx->PBRSpecIBLMapTexUnit());
  theCtx->ActiveProgram()->SetSampler (theCtx, "uEnvMap", theCtx->PBRSpecIBLMapTexUnit());
  theCtx->ActiveProgram()->SetUniform (theCtx, "uZCoeff", theZIsInverted ? -1 : 1);
  theCtx->ActiveProgram()->SetUniform (theCtx, "uYCoeff", theIsTopDown ? 1 : -1);
  theCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, myFBO);
  myVBO.BindAttribute (theCtx, Graphic3d_TOA_POS);

  OSD_Timer aTimer;
  aTimer.Start();
  if (processSpecIBLMap (theCtx, true, theEnvMap->SizeX(), theSpecNbSamples, theProbability)
   && processDiffIBLMap (theCtx, true, theDiffNbSamples))
  {
    Message::SendTrace(TCollection_AsciiString()
      + "IBL " + myIBLMaps[OpenGl_TypeOfIBLMap_Specular].SizeX() + "x" + myIBLMaps[OpenGl_TypeOfIBLMap_Specular].SizeY()
      + " is baked in " + aTimer.ElapsedTime() + " s");
  }
  else
  {
    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_PERFORMANCE, 0, GL_DEBUG_SEVERITY_HIGH,
      TCollection_AsciiString("Error: baking PBR environment ") + myIBLMaps[OpenGl_TypeOfIBLMap_Specular].SizeX()
      + "x" + myIBLMaps[OpenGl_TypeOfIBLMap_Specular].SizeY() + " takes too much time!.");
    clear (theCtx, Graphic3d_Vec3(1.0f));
  }

  myVBO.UnbindAttribute (theCtx, Graphic3d_TOA_POS);
  theEnvMap->Unbind (theCtx, theCtx->PBREnvLUTTexUnit());
}

// =======================================================================
// function : clear
// purpose  :
// =======================================================================
void OpenGl_PBREnvironment::clear (const Handle(OpenGl_Context)& theCtx,
                                   const Graphic3d_Vec3&         theColor)
{
  myIsNeededToBeBound = Standard_True;
  theCtx->arbFBO->glBindFramebuffer (GL_DRAW_FRAMEBUFFER, myFBO);
  theCtx->core11fwd->glClearColor (theColor.r(), theColor.g(), theColor.b(), 1.f);

  processSpecIBLMap (theCtx, false);
  processDiffIBLMap (theCtx, false);
}
