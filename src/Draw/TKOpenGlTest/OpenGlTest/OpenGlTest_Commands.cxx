// Created on: 2012-04-09
// Created by: Sergey ANIKIN
// Copyright (c) 2012-2021 OPEN CASCADE SAS
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

#include <OpenGlTest.hxx>

#include <Draw.hxx>
#include <Draw_Interpretor.hxx>

#include <OpenGl_GlCore20.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_GraphicDriverFactory.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>

#include <OSD_OpenFile.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <TCollection_AsciiString.hxx>

#include <ViewerTest.hxx>
#include <ViewerTest_AutoUpdater.hxx>

static occ::handle<OpenGl_Caps> getDefaultCaps()
{
  occ::handle<OpenGl_GraphicDriverFactory> aFactory = occ::down_cast<OpenGl_GraphicDriverFactory>(
    Graphic3d_GraphicDriverFactory::DefaultDriverFactory());
  if (aFactory.IsNull())
  {
    for (NCollection_List<occ::handle<Graphic3d_GraphicDriverFactory>>::Iterator aFactoryIter(
           Graphic3d_GraphicDriverFactory::DriverFactories());
         aFactoryIter.More();
         aFactoryIter.Next())
    {
      aFactory = occ::down_cast<OpenGl_GraphicDriverFactory>(aFactoryIter.Value());
      if (!aFactory.IsNull())
      {
        break;
      }
    }
  }
  if (aFactory.IsNull())
  {
    throw Standard_ProgramError("Error: no OpenGl_GraphicDriverFactory registered");
  }
  return aFactory->DefaultOptions();
}

namespace
{

//=======================================================================
// function : VUserDraw
// purpose  : Checks availability and operation of UserDraw feature
//=======================================================================
class VUserDrawObj : public AIS_InteractiveObject
{
public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(VUserDrawObj, AIS_InteractiveObject);

  VUserDrawObj()
  {
    myCoords[0] = -10.;
    myCoords[1] = -20.;
    myCoords[2] = -30.;
    myCoords[3] = 10.;
    myCoords[4] = 20.;
    myCoords[5] = 30.;
  }

public:
  class Element : public OpenGl_Element
  {
  private:
    occ::handle<VUserDrawObj> myIObj;

  public:
    Element(const occ::handle<VUserDrawObj>& theIObj)
        : myIObj(theIObj)
    {
    }

    virtual ~Element() {}

    virtual void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
    {
      if (!myIObj.IsNull())
        myIObj->Render(theWorkspace);
    }

    virtual void Release(OpenGl_Context*)
    {
      //
    }

  public:
    DEFINE_STANDARD_ALLOC
  };

private:
  // Virtual methods implementation
  virtual void Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                       const occ::handle<Prs3d_Presentation>&         thePrs,
                       const int                                      theMode) override;

  virtual void ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                const int                               theMode) override;

  // Called by VUserDrawElement
  void Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const;

private:
  GLfloat myCoords[6];
  friend class Element;
};

void VUserDrawObj::Compute(const occ::handle<PrsMgr_PresentationManager>& thePrsMgr,
                           const occ::handle<Prs3d_Presentation>&         thePrs,
                           const int                                      theMode)
{
  if (theMode != 0)
  {
    return;
  }
  thePrs->Clear();

  NCollection_Vec4<float>   aBndMin(myCoords[0], myCoords[1], myCoords[2], 1.0f);
  NCollection_Vec4<float>   aBndMax(myCoords[3], myCoords[4], myCoords[5], 1.0f);
  occ::handle<OpenGl_Group> aGroup = occ::down_cast<OpenGl_Group>(thePrs->NewGroup());
  aGroup
    ->SetMinMaxValues(aBndMin.x(), aBndMin.y(), aBndMin.z(), aBndMax.x(), aBndMax.y(), aBndMax.z());
  aGroup->SetGroupPrimitivesAspect(myDrawer->LineAspect()->Aspect());
  VUserDrawObj::Element* anElem = new VUserDrawObj::Element(this);
  aGroup->AddElement(anElem);

  // invalidate bounding box of the scene
  thePrsMgr->StructureManager()->Update();
}

void VUserDrawObj::ComputeSelection(const occ::handle<SelectMgr_Selection>& theSelection,
                                    const int                               theMode)
{
  if (theMode != 0)
  {
    return;
  }
  occ::handle<SelectMgr_EntityOwner>       anEntityOwner = new SelectMgr_EntityOwner(this);
  occ::handle<NCollection_HArray1<gp_Pnt>> aPnts         = new NCollection_HArray1<gp_Pnt>(1, 5);
  aPnts->SetValue(1, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  aPnts->SetValue(2, gp_Pnt(myCoords[3], myCoords[4], myCoords[2]));
  aPnts->SetValue(3, gp_Pnt(myCoords[3], myCoords[4], myCoords[5]));
  aPnts->SetValue(4, gp_Pnt(myCoords[0], myCoords[1], myCoords[5]));
  aPnts->SetValue(5, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  occ::handle<Select3D_SensitiveCurve> aSensitive =
    new Select3D_SensitiveCurve(anEntityOwner, aPnts);
  theSelection->Add(aSensitive);
}

void VUserDrawObj::Render(const occ::handle<OpenGl_Workspace>& theWorkspace) const
{
  const occ::handle<OpenGl_Context>& aCtx = theWorkspace->GetGlContext();

  // To test linking against OpenGl_Workspace and all aspect classes
  const OpenGl_Aspects* aMA = theWorkspace->Aspects();
  aMA->Aspect()->MarkerType();
  NCollection_Vec4<float> aColor = theWorkspace->InteriorColor();

  aCtx->ShaderManager()->BindLineProgram(occ::handle<OpenGl_TextureSet>(),
                                         Aspect_TOL_SOLID,
                                         Graphic3d_TypeOfShadingModel_Unlit,
                                         Graphic3d_AlphaMode_Opaque,
                                         false,
                                         occ::handle<OpenGl_ShaderProgram>());
  aCtx->SetColor4fv(aColor);

  const NCollection_Vec3<float> aVertArray[4] = {
    NCollection_Vec3<float>(myCoords[0], myCoords[1], myCoords[2]),
    NCollection_Vec3<float>(myCoords[3], myCoords[4], myCoords[2]),
    NCollection_Vec3<float>(myCoords[3], myCoords[4], myCoords[5]),
    NCollection_Vec3<float>(myCoords[0], myCoords[1], myCoords[5]),
  };
  occ::handle<OpenGl_VertexBuffer> aVertBuffer = new OpenGl_VertexBuffer();
  aVertBuffer->Init(aCtx, 3, 4, aVertArray[0].GetData());

  // Finally draw something to make sure UserDraw really works
  aVertBuffer->BindAttribute(aCtx, Graphic3d_TOA_POS);
  aCtx->core11fwd->glDrawArrays(GL_LINE_LOOP, 0, aVertBuffer->GetElemsNb());
  aVertBuffer->UnbindAttribute(aCtx, Graphic3d_TOA_POS);
  aVertBuffer->Release(aCtx.get());
}

} // end of anonymous namespace

static int VUserDraw(Draw_Interpretor&, int argc, const char** argv)
{
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  occ::handle<OpenGl_GraphicDriver> aDriver =
    occ::down_cast<OpenGl_GraphicDriver>(aContext->CurrentViewer()->Driver());
  if (aDriver.IsNull())
  {
    Message::SendFail("Error: Graphic driver not available.");
    return 1;
  }

  if (argc > 2)
  {
    Message::SendFail("Syntax error: wrong number of arguments");
    return 1;
  }

  TCollection_AsciiString aName(argv[1]);
  ViewerTest::Display(aName, occ::handle<AIS_InteractiveObject>());

  occ::handle<VUserDrawObj> anIObj = new VUserDrawObj();
  ViewerTest::Display(aName, anIObj);

  return 0;
}

//=================================================================================================

static int VGlShaders(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  occ::handle<AIS_InteractiveContext> aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  occ::handle<OpenGl_Context> aGlCtx;
  if (occ::handle<OpenGl_GraphicDriver> aDriver =
        occ::down_cast<OpenGl_GraphicDriver>(aCtx->CurrentViewer()->Driver()))
  {
    aGlCtx = aDriver->GetSharedContext();
  }
  if (aGlCtx.IsNull())
  {
    Message::SendFail("Error: no OpenGl_Context");
    return 1;
  }

  bool toList = theArgNb < 2;
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    if (anArg == "-list")
    {
      toList = true;
    }
    else if ((anArg == "-update" || anArg == "-dump" || anArg == "-debug" || anArg == "-reload"
              || anArg == "-load")
             && anArgIter + 1 < theArgNb)
    {
      TCollection_AsciiString           aShaderName = theArgVec[++anArgIter];
      occ::handle<OpenGl_ShaderProgram> aResProg;
      if (!aGlCtx->GetResource(aShaderName, aResProg))
      {
        Message::SendFail() << "Syntax error: shader resource '" << aShaderName << "' is not found";
        return 1;
      }
      if (aResProg->UpdateDebugDump(aGlCtx, "", false, anArg == "-dump"))
      {
        aCtx->UpdateCurrentViewer();
      }
      return 0;
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }
  if (toList)
  {
    for (OpenGl_Context::OpenGl_ResourcesMap::Iterator aResIter(aGlCtx->SharedResources());
         aResIter.More();
         aResIter.Next())
    {
      if (occ::handle<OpenGl_ShaderProgram> aResProg =
            occ::down_cast<OpenGl_ShaderProgram>(aResIter.Value()))
      {
        theDI << aResProg->ResourceId() << " ";
      }
    }
  }

  return 0;
}

//! Auxiliary function for parsing glsl dump level argument.
static bool parseGlslSourceFlag(const char* theArg, OpenGl_ShaderProgramDumpLevel& theGlslDumpLevel)
{
  TCollection_AsciiString aTypeStr(theArg);
  aTypeStr.LowerCase();
  if (aTypeStr == "off" || aTypeStr == "0")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
  }
  else if (aTypeStr == "short")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Short;
  }
  else if (aTypeStr == "full" || aTypeStr == "1")
  {
    theGlslDumpLevel = OpenGl_ShaderProgramDumpLevel_Full;
  }
  else
  {
    return false;
  }
  return true;
}

//=================================================================================================

static int VGlDebug(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  occ::handle<OpenGl_GraphicDriver> aDriver;
  occ::handle<OpenGl_Context>       aGlCtx;
  occ::handle<V3d_View>             aView = ViewerTest::CurrentView();
  if (!aView.IsNull())
  {
    aDriver = occ::down_cast<OpenGl_GraphicDriver>(aView->Viewer()->Driver());
    if (!aDriver.IsNull())
    {
      aGlCtx = aDriver->GetSharedContext();
    }
  }
  OpenGl_Caps* aDefCaps = getDefaultCaps().get();
  OpenGl_Caps* aCaps    = !aDriver.IsNull() ? &aDriver->ChangeOptions() : NULL;

  if (theArgNb < 2)
  {
    TCollection_AsciiString aDebActive, aSyncActive;
    if (aCaps == NULL)
    {
      aCaps = aDefCaps;
    }
    else if (!aGlCtx.IsNull())
    {
      bool isActive =
        OpenGl_Context::CheckExtension((const char*)aGlCtx->core11fwd->glGetString(GL_EXTENSIONS),
                                       "GL_ARB_debug_output");
      aDebActive = isActive ? " (active)" : " (inactive)";
      if (isActive)
      {
        // GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB
        aSyncActive =
          aGlCtx->core11fwd->glIsEnabled(0x8242) == GL_TRUE ? " (active)" : " (inactive)";
      }
    }

    TCollection_AsciiString aGlslCodeDebugStatus =
      TCollection_AsciiString() + "glslSourceCode: "
      + (aCaps->glslDumpLevel == OpenGl_ShaderProgramDumpLevel_Off     ? "Off"
         : aCaps->glslDumpLevel == OpenGl_ShaderProgramDumpLevel_Short ? "Short"
                                                                       : "Full")
      + "\n";
    theDI << "debug:          " << (aCaps->contextDebug ? "1" : "0") << aDebActive << "\n"
          << "sync:           " << (aCaps->contextSyncDebug ? "1" : "0") << aSyncActive << "\n"
          << "glslWarn:       " << (aCaps->glslWarnings ? "1" : "0") << "\n"
          << aGlslCodeDebugStatus << "extraMsg:       " << (aCaps->suppressExtraMsg ? "0" : "1")
          << "\n";
    return 0;
  }

  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const char*             anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    bool toEnableDebug = true;
    if (anArgCase == "-glsl" || anArgCase == "-glslwarn" || anArgCase == "-glslwarns"
        || anArgCase == "-glslwarnings")
    {
      bool toShowWarns = true;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toShowWarns))
      {
        --anArgIter;
      }
      aDefCaps->glslWarnings = toShowWarns;
      if (aCaps != NULL)
      {
        aCaps->glslWarnings = toShowWarns;
      }
    }
    else if (anArgCase == "-extra" || anArgCase == "-extramsg" || anArgCase == "-extramessages")
    {
      bool toShow = true;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toShow))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = !toShow;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = !toShow;
      }
    }
    else if (anArgCase == "-noextra" || anArgCase == "-noextramsg"
             || anArgCase == "-noextramessages")
    {
      bool toSuppress = true;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toSuppress))
      {
        --anArgIter;
      }
      aDefCaps->suppressExtraMsg = toSuppress;
      if (aCaps != NULL)
      {
        aCaps->suppressExtraMsg = toSuppress;
      }
    }
    else if (anArgCase == "-sync")
    {
      bool toSync = true;
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toSync))
      {
        --anArgIter;
      }
      aDefCaps->contextSyncDebug = toSync;
      if (toSync)
      {
        aDefCaps->contextDebug = true;
      }
    }
    else if (anArgCase == "-glslsourcecode" || anArgCase == "-glslcode")
    {
      OpenGl_ShaderProgramDumpLevel aGslsDumpLevel = OpenGl_ShaderProgramDumpLevel_Full;
      if (++anArgIter < theArgNb && !parseGlslSourceFlag(theArgVec[anArgIter], aGslsDumpLevel))
      {
        --anArgIter;
      }
      aDefCaps->glslDumpLevel = aGslsDumpLevel;
      if (aCaps != NULL)
      {
        aCaps->glslDumpLevel = aGslsDumpLevel;
      }
    }
    else if (anArgCase == "-debug")
    {
      if (++anArgIter < theArgNb && !Draw::ParseOnOff(theArgVec[anArgIter], toEnableDebug))
      {
        --anArgIter;
      }
      aDefCaps->contextDebug = toEnableDebug;
    }
    else if (Draw::ParseOnOff(anArg, toEnableDebug) && (anArgIter + 1 == theArgNb))
    {
      // simple alias to turn on almost everything
      aDefCaps->contextDebug     = toEnableDebug;
      aDefCaps->contextSyncDebug = toEnableDebug;
      aDefCaps->glslWarnings     = toEnableDebug;
      if (!toEnableDebug)
      {
        aDefCaps->glslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
      }
      aDefCaps->suppressExtraMsg = !toEnableDebug;
      if (aCaps != NULL)
      {
        aCaps->contextDebug     = toEnableDebug;
        aCaps->contextSyncDebug = toEnableDebug;
        aCaps->glslWarnings     = toEnableDebug;
        if (!toEnableDebug)
        {
          aCaps->glslDumpLevel = OpenGl_ShaderProgramDumpLevel_Off;
        }
        aCaps->suppressExtraMsg = !toEnableDebug;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  return 0;
}

//=================================================================================================

static int VVbo(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  const bool toSet    = (theArgNb > 1);
  const bool toUseVbo = toSet ? (Draw::Atoi(theArgVec[1]) == 0) : 1;
  if (toSet)
  {
    getDefaultCaps()->vboDisable = toUseVbo;
  }

  // get the context
  occ::handle<AIS_InteractiveContext> aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    if (!toSet)
    {
      Message::SendFail("Error: no active viewer");
    }
    return 1;
  }
  occ::handle<OpenGl_GraphicDriver> aDriver =
    occ::down_cast<OpenGl_GraphicDriver>(aContextAIS->CurrentViewer()->Driver());
  if (!aDriver.IsNull())
  {
    if (!toSet)
    {
      theDI << (aDriver->Options().vboDisable ? "0" : "1") << "\n";
    }
    else
    {
      aDriver->ChangeOptions().vboDisable = toUseVbo;
    }
  }

  return 0;
}

//=================================================================================================

static int VCaps(Draw_Interpretor& theDI, int theArgNb, const char** theArgVec)
{
  OpenGl_Caps*                        aCaps = getDefaultCaps().get();
  occ::handle<OpenGl_GraphicDriver>   aDriver;
  occ::handle<AIS_InteractiveContext> aContext = ViewerTest::GetAISContext();
  if (!aContext.IsNull())
  {
    aDriver = occ::down_cast<OpenGl_GraphicDriver>(aContext->CurrentViewer()->Driver());
    aCaps   = &aDriver->ChangeOptions();
  }

  if (theArgNb < 2)
  {
    theDI << "sRGB:    " << (aCaps->sRGBDisable ? "0" : "1") << "\n";
    theDI << "VBO:     " << (aCaps->vboDisable ? "0" : "1") << "\n";
    theDI << "Sprites: " << (aCaps->pntSpritesDisable ? "0" : "1") << "\n";
    theDI << "SoftMode:" << (aCaps->contextNoAccel ? "1" : "0") << "\n";
    theDI << "FFP:     " << (aCaps->ffpEnable ? "1" : "0") << "\n";
    theDI << "PolygonMode: " << (aCaps->usePolygonMode ? "1" : "0") << "\n";
    theDI << "DepthZeroToOne: " << (aCaps->useZeroToOneDepth ? "1" : "0") << "\n";
    theDI << "VSync:   " << aCaps->swapInterval << "\n";
    theDI << "Compatible:" << (aCaps->contextCompatible ? "1" : "0") << "\n";
    theDI << "Stereo:  " << (aCaps->contextStereo ? "1" : "0") << "\n";
    theDI << "WinBuffer: " << (aCaps->useSystemBuffer ? "1" : "0") << "\n";
    theDI << "OpaqueAlpha: " << (aCaps->buffersOpaqueAlpha ? "1" : "0") << "\n";
    theDI << "DeepColor: " << (aCaps->buffersDeepColor ? "1" : "0") << "\n";
    theDI << "NoExt:" << (aCaps->contextNoExtensions ? "1" : "0") << "\n";
    theDI << "MaxVersion:" << aCaps->contextMajorVersionUpper << "."
          << aCaps->contextMinorVersionUpper << "\n";
    theDI << "CompressTextures: " << (aCaps->compressedTexturesDisable ? "0" : "1") << "\n";
    return 0;
  }

  ViewerTest_AutoUpdater anUpdateTool(aContext, ViewerTest::CurrentView());
  for (int anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    const char*             anArg = theArgVec[anArgIter];
    TCollection_AsciiString anArgCase(anArg);
    anArgCase.LowerCase();
    if (anUpdateTool.parseRedrawMode(anArg))
    {
      continue;
    }
    else if (anArgCase == "-vsync" || anArgCase == "-novsync" || anArgCase == "-swapinterval")
    {
      aCaps->swapInterval = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-ffp" || anArgCase == "-noffp")
    {
      aCaps->ffpEnable = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-polygonmode" || anArgCase == "-nopolygonmode")
    {
      aCaps->usePolygonMode = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-srgb" || anArgCase == "-nosrgb")
    {
      aCaps->sRGBDisable = !Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-compressedtextures" || anArgCase == "-nocompressedtextures")
    {
      aCaps->compressedTexturesDisable =
        !Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-vbo" || anArgCase == "-novbo")
    {
      aCaps->vboDisable = !Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-sprite" || anArgCase == "-sprites")
    {
      aCaps->pntSpritesDisable = !Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-depthzerotoone" || anArgCase == "-zerotoonedepth"
             || anArgCase == "-usezerotoonedepth" || anArgCase == "-iszerotoonedepth")
    {
      aCaps->useZeroToOneDepth = Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-softmode" || anArgCase == "-contextnoaccel")
    {
      aCaps->contextNoAccel = Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-opaquealpha" || anArgCase == "-noopaquealpha"
             || anArgCase == "-buffersopaquealpha" || anArgCase == "-nobuffersopaquealpha")
    {
      aCaps->buffersOpaqueAlpha = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-deepcolor" || anArgCase == "-nodeepcolor"
             || anArgCase == "-buffersdeepcolor" || anArgCase == "-nobuffersdeepcolor")
    {
      aCaps->buffersDeepColor = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-winbuffer" || anArgCase == "-windowbuffer"
             || anArgCase == "-nowinbuffer" || anArgCase == "-nowindowbuffer"
             || anArgCase == "-usewinbuffer" || anArgCase == "-usewindowbuffer"
             || anArgCase == "-usesystembuffer")
    {
      aCaps->useSystemBuffer = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-accel" || anArgCase == "-acceleration" || anArgCase == "-noaccel"
             || anArgCase == "-noacceleration")
    {
      aCaps->contextNoAccel = !Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-compat" || anArgCase == "-compatprofile" || anArgCase == "-compatible"
             || anArgCase == "-compatibleprofile")
    {
      aCaps->contextCompatible = Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = false;
      }
    }
    else if (anArgCase == "-core" || anArgCase == "-coreprofile")
    {
      aCaps->contextCompatible = !Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
      if (!aCaps->contextCompatible)
      {
        aCaps->ffpEnable = false;
      }
    }
    else if (anArgCase == "-stereo" || anArgCase == "-quadbuffer" || anArgCase == "-nostereo"
             || anArgCase == "-noquadbuffer")
    {
      aCaps->contextStereo = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-noext" || anArgCase == "-noextensions" || anArgCase == "-noextension")
    {
      aCaps->contextNoExtensions = Draw::ParseOnOffIterator(theArgNb, theArgVec, anArgIter);
    }
    else if (anArgCase == "-maxversion" || anArgCase == "-upperversion"
             || anArgCase == "-limitversion")
    {
      int aVer[2] = {-2, -1};
      for (int aValIter = 0; aValIter < 2; ++aValIter)
      {
        if (anArgIter + 1 < theArgNb)
        {
          const TCollection_AsciiString aStr(theArgVec[anArgIter + 1]);
          if (aStr.IsIntegerValue())
          {
            aVer[aValIter] = aStr.IntegerValue();
            ++anArgIter;
          }
        }
      }
      if (aVer[0] < -1 || aVer[1] < -1)
      {
        Message::SendFail() << "Syntax error at '" << anArgCase << "'";
        return 1;
      }
      aCaps->contextMajorVersionUpper = aVer[0];
      aCaps->contextMinorVersionUpper = aVer[1];
    }
    else
    {
      Message::SendFail() << "Error: unknown argument '" << anArg << "'";
      return 1;
    }
  }
  if (aCaps != getDefaultCaps().get())
  {
    *getDefaultCaps() = *aCaps;
  }
  return 0;
}

//=================================================================================================

void OpenGlTest::Commands(Draw_Interpretor& theCommands)
{
  const char* aGroup = "Commands for low-level TKOpenGl features";

  theCommands.Add("vuserdraw",
                  "vuserdraw : name - simulates drawing with help of UserDraw",
                  __FILE__,
                  VUserDraw,
                  aGroup);
  theCommands.Add("vglshaders",
                  "vglshaders [-list] [-dump] [-reload] ShaderId"
                  "\n\t\t:  -list   prints the list of registered GLSL programs"
                  "\n\t\t:  -dump   dumps specified GLSL program (for debugging)"
                  "\n\t\t:  -reload restores dump of specified GLSL program",
                  __FILE__,
                  VGlShaders,
                  aGroup);
  theCommands.Add(
    "vcaps",
    "vcaps [-sRGB {0|1}] [-vbo {0|1}] [-sprites {0|1}] [-ffp {0|1}] [-polygonMode {0|1}]"
    "\n\t\t:       [-compatibleProfile {0|1}] [-compressedTextures {0|1}]"
    "\n\t\t:       [-vsync {0|1}] [-useWinBuffer {0|1}] [-opaqueAlpha {0|1}]"
    "\n\t\t:       [-deepColor {0|1}] [-quadBuffer {0|1}] [-stereo {0|1}]"
    "\n\t\t:       [-softMode {0|1}] [-noupdate|-update]"
    "\n\t\t:       [-zeroToOneDepth {0|1}]"
    "\n\t\t:       [-noExtensions {0|1}] [-maxVersion Major Minor]"
    "\n\t\t: Modify particular graphic driver options:"
    "\n\t\t:  sRGB     - enable/disable sRGB rendering"
    "\n\t\t:  FFP      - use fixed-function pipeline instead of"
    "\n\t\t:             built-in GLSL programs"
    "\n\t\t:            (requires compatible profile)"
    "\n\t\t:  polygonMode - use Polygon Mode instead of built-in GLSL programs"
    "\n\t\t:  compressedTexture - allow uploading of GPU-supported compressed texture formats"
    "\n\t\t:  VBO      - use Vertex Buffer Object (copy vertex"
    "\n\t\t:             arrays to GPU memory)"
    "\n\t\t:  sprite   - use textured sprites instead of bitmaps"
    "\n\t\t:  vsync    - switch VSync on or off"
    "\n\t\t:  opaqueAlpha - disable writes in alpha component of color buffer"
    "\n\t\t:  winBuffer - allow using window buffer for rendering"
    "\n\t\t:  zeroToOneDepth - use [0,1] depth range instead of [-1,1] range"
    "\n\t\t: Window buffer creation options:"
    "\n\t\t:  quadbuffer  - QuadBuffer for stereoscopic displays"
    "\n\t\t:  deepColor   - window buffer with higher color precision (30bit instead of 24bit RGB)"
    "\n\t\t: Context creation options:"
    "\n\t\t:  softMode          - software OpenGL implementation"
    "\n\t\t:  compatibleProfile - backward-compatible profile"
    "\n\t\t:  noExtensions      - disallow usage of extensions"
    "\n\t\t:  maxVersion        - force upper OpenGL version to be used"
    "\n\t\t: These parameters control alternative"
    "\n\t\t: rendering paths producing the same visual result when possible.",
    __FILE__,
    VCaps,
    aGroup);
  theCommands.Add("vgldebug",
                  "vgldebug [-sync {0|1}] [-debug {0|1}] [-glslWarn {0|1}]"
                  "\n\t\t:          [-glslCode {off|short|full}] [-extraMsg {0|1}] [{0|1}]"
                  "\n\t\t: Request debug GL context. Should be called BEFORE vinit."
                  "\n\t\t: Debug context can be requested only on Windows"
                  "\n\t\t: with GL_ARB_debug_output extension implemented by GL driver!"
                  "\n\t\t:  -sync     - request synchronized debug GL context"
                  "\n\t\t:  -glslWarn - log GLSL compiler/linker warnings,"
                  "\n\t\t:              which are suppressed by default,"
                  "\n\t\t:  -glslCode - log GLSL program source code,"
                  "\n\t\t:              which are suppressed by default,"
                  "\n\t\t:  -extraMsg - log extra diagnostic messages from GL context,"
                  "\n\t\t:              which are suppressed by default",
                  __FILE__,
                  VGlDebug,
                  aGroup);
  theCommands.Add("vvbo",
                  "vvbo [{0|1}] : turn VBO usage On/Off; affects only newly displayed objects",
                  __FILE__,
                  VVbo,
                  aGroup);
}
