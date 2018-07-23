// Created on: 2012-04-09
// Created by: Sergey ANIKIN
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#include <ViewerTest.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ShaderObject.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <OpenGl_AspectFace.hxx>
#include <OpenGl_AspectLine.hxx>
#include <OpenGl_AspectMarker.hxx>
#include <OpenGl_AspectText.hxx>
#include <OpenGl_Context.hxx>
#include <OpenGl_Element.hxx>
#include <OpenGl_GlCore20.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OpenGl_ShaderManager.hxx>
#include <OpenGl_Workspace.hxx>
#include <OSD_Environment.hxx>
#include <OSD_File.hxx>
#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Select3D_SensitiveCurve.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Selection.hxx>
#include <TCollection_AsciiString.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <ViewerTest_DoubleMapOfInteractiveAndName.hxx>
#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>
#include <OpenGl_Group.hxx>

extern Standard_Boolean VDisplayAISObject (const TCollection_AsciiString& theName,
                                           const Handle(AIS_InteractiveObject)& theAISObj,
                                           Standard_Boolean theReplaceIfExists = Standard_True);
extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

namespace {

//=======================================================================
//function : VUserDraw
//purpose  : Checks availability and operation of UserDraw feature
//=======================================================================

class VUserDrawObj : public AIS_InteractiveObject
{
public:
    // CASCADE RTTI
    DEFINE_STANDARD_RTTI_INLINE(VUserDrawObj,AIS_InteractiveObject);

    VUserDrawObj()
    {
      myCoords[0] = -10.;
      myCoords[1] = -20.;
      myCoords[2] = -30.;
      myCoords[3] =  10.;
      myCoords[4] =  20.;
      myCoords[5] =  30.;
    }

public:
  class Element : public OpenGl_Element
  {
  private:
    Handle(VUserDrawObj) myIObj;

  public:
    Element (const Handle(VUserDrawObj)& theIObj) : myIObj (theIObj) {}

    virtual ~Element() {}

    virtual void Render (const Handle(OpenGl_Workspace)& theWorkspace) const
    {
      if (!myIObj.IsNull())
        myIObj->Render(theWorkspace);
    }

    virtual void Release (OpenGl_Context*)
    {
      //
    }

  public:
    DEFINE_STANDARD_ALLOC
  };

private:
    // Virtual methods implementation
    void Compute (const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
                  const Handle(Prs3d_Presentation)& thePresentation,
                  const Standard_Integer theMode) Standard_OVERRIDE;

    void ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                           const Standard_Integer theMode) Standard_OVERRIDE;

    // Called by VUserDrawElement
    void Render(const Handle(OpenGl_Workspace)& theWorkspace) const;

private:
    GLfloat myCoords[6];
    friend class Element;
};

void VUserDrawObj::Compute(const Handle(PrsMgr_PresentationManager3d)& thePrsMgr,
                           const Handle(Prs3d_Presentation)& thePrs,
                           const Standard_Integer /*theMode*/)
{
  thePrs->Clear();

  Graphic3d_Vec4 aBndMin (myCoords[0], myCoords[1], myCoords[2], 1.0f);
  Graphic3d_Vec4 aBndMax (myCoords[3], myCoords[4], myCoords[5], 1.0f);
  Handle(OpenGl_Group) aGroup = Handle(OpenGl_Group)::DownCast (thePrs->NewGroup());
  aGroup->SetMinMaxValues (aBndMin.x(), aBndMin.y(), aBndMin.z(),
                           aBndMax.x(), aBndMax.y(), aBndMax.z());
  aGroup->SetGroupPrimitivesAspect (myDrawer->LineAspect()->Aspect());
  VUserDrawObj::Element* anElem = new VUserDrawObj::Element (this);
  aGroup->AddElement(anElem);

  // invalidate bounding box of the scene
  thePrsMgr->StructureManager()->Update();
}

void VUserDrawObj::ComputeSelection (const Handle(SelectMgr_Selection)& theSelection,
                                     const Standard_Integer /*theMode*/)
{
  Handle(SelectMgr_EntityOwner) anEntityOwner = new SelectMgr_EntityOwner(this);
  Handle(TColgp_HArray1OfPnt) aPnts = new TColgp_HArray1OfPnt(1, 5);
  aPnts->SetValue(1, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  aPnts->SetValue(2, gp_Pnt(myCoords[3], myCoords[4], myCoords[2]));
  aPnts->SetValue(3, gp_Pnt(myCoords[3], myCoords[4], myCoords[5]));
  aPnts->SetValue(4, gp_Pnt(myCoords[0], myCoords[1], myCoords[5]));
  aPnts->SetValue(5, gp_Pnt(myCoords[0], myCoords[1], myCoords[2]));
  Handle(Select3D_SensitiveCurve) aSensitive = new Select3D_SensitiveCurve(anEntityOwner, aPnts);
  theSelection->Add(aSensitive);
}

void VUserDrawObj::Render(const Handle(OpenGl_Workspace)& theWorkspace) const
{
  const Handle(OpenGl_Context)& aCtx = theWorkspace->GetGlContext();

  // To test linking against OpenGl_Workspace and all aspect classes
  const OpenGl_AspectMarker* aMA = theWorkspace->AspectMarker();
  aMA->Aspect()->Type();
  const OpenGl_AspectText* aTA = theWorkspace->AspectText();
  aTA->Aspect()->Font();
  OpenGl_Vec4 aColor = theWorkspace->LineColor();

  aCtx->ShaderManager()->BindLineProgram (Handle(OpenGl_TextureSet)(), Aspect_TOL_SOLID,
                                          Graphic3d_TOSM_UNLIT, Graphic3d_AlphaMode_Opaque, false,
                                          Handle(OpenGl_ShaderProgram)());
  aCtx->SetColor4fv (aColor);

  const OpenGl_Vec3 aVertArray[4] =
  {
    OpenGl_Vec3(myCoords[0], myCoords[1], myCoords[2]),
    OpenGl_Vec3(myCoords[3], myCoords[4], myCoords[2]),
    OpenGl_Vec3(myCoords[3], myCoords[4], myCoords[5]),
    OpenGl_Vec3(myCoords[0], myCoords[1], myCoords[5]),
  };
  Handle(OpenGl_VertexBuffer) aVertBuffer = new OpenGl_VertexBuffer();
  aVertBuffer->Init (aCtx, 3, 4, aVertArray[0].GetData());

  // Finally draw something to make sure UserDraw really works
  aVertBuffer->BindAttribute  (aCtx, Graphic3d_TOA_POS);
  glDrawArrays(GL_LINE_LOOP, 0, aVertBuffer->GetElemsNb());
  aVertBuffer->UnbindAttribute(aCtx, Graphic3d_TOA_POS);
  aVertBuffer->Release (aCtx.get());
}

} // end of anonymous namespace

static Standard_Integer VUserDraw (Draw_Interpretor& di,
                                    Standard_Integer argc,
                                    const char ** argv)
{
  Handle(AIS_InteractiveContext) aContext = ViewerTest::GetAISContext();
  if (aContext.IsNull())
  {
    di << argv[0] << "Call 'vinit' before!\n";
    return 1;
  }

  Handle(OpenGl_GraphicDriver) aDriver = Handle(OpenGl_GraphicDriver)::DownCast (aContext->CurrentViewer()->Driver());
  if (aDriver.IsNull())
  {
    std::cerr << "Graphic driver not available.\n";
    return 1;
  }

  if (argc > 2)
  {
    di << argv[0] << "Wrong number of arguments, only the object name expected\n";
    return 1;
  }

  TCollection_AsciiString aName (argv[1]);
  VDisplayAISObject(aName, Handle(AIS_InteractiveObject)());

  Handle(VUserDrawObj) anIObj = new VUserDrawObj();
  VDisplayAISObject(aName, anIObj);

  return 0;
}

//==============================================================================
//function : VFeedback
//purpose  :
//==============================================================================

static int VFeedback (Draw_Interpretor& theDI,
                      Standard_Integer  /*theArgNb*/,
                      const char**      /*theArgVec*/)
{
#if !defined(GL_ES_VERSION_2_0)
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  unsigned int aBufferSize = 1024 * 1024;
  for (;;)
  {
    size_t aBytes = (size_t )aBufferSize * sizeof(GLfloat);
    if (aBytes / sizeof(GLfloat) != (size_t )aBufferSize)
    {
      // finito la commedia
      std::cerr << "Can not allocate buffer - requested size ("
                << (double(aBufferSize / (1024 * 1024)) * double(sizeof(GLfloat)))
                << " MiB) is out of address space\n";
      return 1;
    }

    GLfloat* aBuffer = (GLfloat* )Standard::Allocate (aBytes);
    if (aBuffer == NULL)
    {
      // finito la commedia
      std::cerr << "Can not allocate buffer with size ("
                << (double(aBufferSize / (1024 * 1024)) * double(sizeof(GLfloat)))
                << " MiB)\n";
      return 1;
    }

    glFeedbackBuffer ((GLsizei )aBufferSize, GL_2D, aBuffer);
    glRenderMode (GL_FEEDBACK);

    aView->Redraw();

    GLint aResult = glRenderMode (GL_RENDER);
    if (aResult < 0)
    {
      aBufferSize *= 2;

      void* aPtr = aBuffer;
      Standard::Free (aPtr);
      aBuffer = NULL;
      continue;
    }

    std::cout << "FeedBack result= " << aResult << "\n";
    GLint aPntNb     = 0;
    GLint aTriNb     = 0;
    GLint aQuadsNb   = 0;
    GLint aPolyNb    = 0;
    GLint aNodesNb   = 0;
    GLint aLinesNb   = 0;
    GLint aBitmapsNb = 0;
    GLint aPassThrNb = 0;
    GLint aUnknownNb = 0;
    const GLint NODE_VALUES = 2; // GL_2D
    for (GLint anIter = 0; anIter < aResult;)
    {
        const GLfloat aPos = aBuffer[anIter];
        switch ((GLint )aPos)
        {
          case GL_POINT_TOKEN:
          {
            ++aPntNb;
            ++aNodesNb;
            anIter += 1 + NODE_VALUES;
            break;
          }
          case GL_LINE_RESET_TOKEN:
          case GL_LINE_TOKEN:
          {
            ++aLinesNb;
            aNodesNb += 2;
            anIter += 1 + 2 * NODE_VALUES;
            break;
          }
          case GL_POLYGON_TOKEN:
          {
            const GLint aCount = (GLint )aBuffer[++anIter];
            aNodesNb += aCount;
            anIter += aCount * NODE_VALUES + 1;
            if (aCount == 3)
            {
              ++aTriNb;
            }
            else if (aCount == 4)
            {
              ++aQuadsNb;
            }
            else
            {
              ++aPolyNb;
            }
            break;
          }
          case GL_BITMAP_TOKEN:
          case GL_DRAW_PIXEL_TOKEN:
          case GL_COPY_PIXEL_TOKEN:
          {
            ++aBitmapsNb;
            anIter += 1 + NODE_VALUES;
            break;
          }
          case GL_PASS_THROUGH_TOKEN:
          {
            ++aPassThrNb;
            anIter += 2; // header + value
            break;
          }
          default:
          {
            ++anIter;
            ++aUnknownNb;
            break;
          }
       }
    }
    void* aPtr = aBuffer;
    Standard::Free (aPtr);

    // return statistics
    theDI << "Total nodes:   " << aNodesNb   << "\n"
          << "Points:        " << aPntNb     << "\n"
          << "Line segments: " << aLinesNb   << "\n"
          << "Triangles:     " << aTriNb     << "\n"
          << "Quads:         " << aQuadsNb   << "\n"
          << "Polygons:      " << aPolyNb    << "\n"
          << "Bitmap tokens: " << aBitmapsNb << "\n"
          << "Pass through:  " << aPassThrNb << "\n"
          << "UNKNOWN:       " << aUnknownNb << "\n";

    double aLen2D      = double(aNodesNb * 2 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    double aLen3D      = double(aNodesNb * 3 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    double aLen3D_rgba = double(aNodesNb * 7 + aPntNb + aLinesNb * 2 + (aTriNb + aQuadsNb + aPolyNb) * 2 + aBitmapsNb + aPassThrNb);
    theDI << "Buffer size GL_2D:       " << aLen2D      * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n"
          << "Buffer size GL_3D:       " << aLen3D      * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n"
          << "Buffer size GL_3D_COLOR: " << aLen3D_rgba * double(sizeof(GLfloat)) / double(1024 * 1024) << " MiB\n";
    return 0;
  }
#else
  (void )theDI;
  std::cout << "Command is unsupported on current platform.\n";
  return 1;
#endif
}

//==============================================================================
//function : VImmediateFront
//purpose  :
//==============================================================================

static int VImmediateFront (Draw_Interpretor& /*theDI*/,
                            Standard_Integer  theArgNb,
                            const char**      theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();

  if (aDriver.IsNull())
  {
    std::cerr << "Graphic driver not available.\n";
    return 1;
  }

  if (theArgNb < 2)
  {
    std::cerr << "Wrong number of arguments.\n";
    return 1;
  }

  ViewerTest::CurrentView()->View()->SetImmediateModeDrawToFront (atoi(theArgVec[1]) != 0);

  return 0;
}

//! Search the info from the key.
inline TCollection_AsciiString searchInfo (const TColStd_IndexedDataMapOfStringString& theDict,
                                           const TCollection_AsciiString& theKey)
{
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter (theDict); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString (anIter.Key(), theKey, Standard_False))
    {
      return anIter.Value();
    }
  }
  return TCollection_AsciiString();
}

//==============================================================================
//function : VGlInfo
//purpose  :
//==============================================================================

static int VGlInfo (Draw_Interpretor& theDI,
                    Standard_Integer  theArgNb,
                    const char**      theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    std::cerr << "No active view. Please call vinit.\n";
    return 1;
  }

  Standard_Integer anArgIter = 1;
  Graphic3d_DiagnosticInfo anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
  if (theArgNb == 2)
  {
    TCollection_AsciiString aName (theArgVec[1]);
    aName.LowerCase();
    if (aName == "-short")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Short;
    }
    else if (aName == "-basic")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
    }
    else if (aName == "-complete"
          || aName == "-full")
    {
      ++anArgIter;
      anInfoLevel = Graphic3d_DiagnosticInfo_Complete;
    }
  }

  TColStd_IndexedDataMapOfStringString aDict;
  if (anArgIter >= theArgNb)
  {
    aView->DiagnosticInformation (aDict, anInfoLevel);
    TCollection_AsciiString aText;
    for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter (aDict); aValueIter.More(); aValueIter.Next())
    {
      if (!aText.IsEmpty())
      {
        aText += "\n";
      }
      aText += TCollection_AsciiString("  ") + aValueIter.Key() + ": " + aValueIter.Value();
    }

    theDI << "OpenGL info:\n"
          << aText;
    return 0;
  }

  const Standard_Boolean isList = theArgNb >= 3;
  aView->DiagnosticInformation (aDict, Graphic3d_DiagnosticInfo_Complete);
  for (; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString aName (theArgVec[anArgIter]);
    aName.UpperCase();
    TCollection_AsciiString aValue;
    if (aName.Search ("VENDOR") != -1)
    {
      aValue = searchInfo (aDict, "GLvendor");
    }
    else if (aName.Search ("RENDERER") != -1)
    {
      aValue = searchInfo (aDict, "GLdevice");
    }
    else if (aName.Search ("SHADING_LANGUAGE_VERSION") != -1
          || aName.Search ("GLSL") != -1)
    {
      aValue = searchInfo (aDict, "GLSLversion");
    }
    else if (aName.Search ("VERSION") != -1)
    {
      aValue = searchInfo (aDict, "GLversion");
    }
    else if (aName.Search ("EXTENSIONS") != -1)
    {
      aValue = searchInfo (aDict, "GLextensions");
    }
    else
    {
      std::cerr << "Unknown key '" << aName.ToCString() << "'\n";
      return 1;
    }

    if (isList)
    {
      theDI << "{" << aValue << "} ";
    }
    else
    {
      theDI << aValue;
    }
  }

  return 0;
}

//! Parse shader type argument.
static bool parseShaderTypeArg (Graphic3d_TypeOfShaderObject& theType,
                                const TCollection_AsciiString& theArg)
{
  if (theArg == "-vertex"
   || theArg == "-vert")
  {
    theType = Graphic3d_TOS_VERTEX;
  }
  else if (theArg == "-tessevaluation"
        || theArg == "-tesseval"
        || theArg == "-evaluation"
        || theArg == "-eval")
  {
    theType = Graphic3d_TOS_TESS_EVALUATION;
  }
  else if (theArg == "-tesscontrol"
        || theArg == "-tessctrl"
        || theArg == "-control"
        || theArg == "-ctrl")
  {
    theType = Graphic3d_TOS_TESS_CONTROL;
  }
  else if (theArg == "-geometry"
        || theArg == "-geom")
  {
    theType = Graphic3d_TOS_GEOMETRY;
  }
  else if (theArg == "-fragment"
        || theArg == "-frag")
  {
    theType = Graphic3d_TOS_FRAGMENT;
  }
  else if (theArg == "-compute"
        || theArg == "-comp")
  {
    theType = Graphic3d_TOS_COMPUTE;
  }
  else
  {
    return false;
  }
  return true;
}

//==============================================================================
//function : VShaderProg
//purpose  : Sets the pair of vertex and fragment shaders for the object
//==============================================================================
static Standard_Integer VShaderProg (Draw_Interpretor& /*theDI*/,
                                     Standard_Integer  theArgNb,
                                     const char**      theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    std::cout << "Error: no active view.\n";
    return 1;
  }
  else if (theArgNb < 2)
  {
    std::cout << "Syntax error: lack of arguments\n";
    return 1;
  }

  bool isExplicitShaderType = false;
  Handle(Graphic3d_ShaderProgram) aProgram = new Graphic3d_ShaderProgram();
  NCollection_Sequence<Handle(AIS_InteractiveObject)> aPrsList;
  Graphic3d_GroupAspect aGroupAspect = Graphic3d_ASPECT_FILL_AREA;
  bool isSetGroupAspect = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg (theArgVec[anArgIter]);
    anArg.LowerCase();
    Graphic3d_TypeOfShaderObject aShaderTypeArg = Graphic3d_TypeOfShaderObject(-1);
    if (!aProgram.IsNull()
     &&  aProgram->ShaderObjects().IsEmpty()
     && (anArg == "-off"
      || anArg ==  "off"))
    {
      aProgram.Nullify();
    }
    else if (!aProgram.IsNull()
          &&  aProgram->ShaderObjects().IsEmpty()
          && (anArg == "-phong"
           || anArg ==  "phong"))
    {
      const TCollection_AsciiString& aShadersRoot = Graphic3d_ShaderProgram::ShadersFolder();
      if (aShadersRoot.IsEmpty())
      {
        std::cout << "Error: both environment variables CSF_ShadersDirectory and CASROOT are undefined!\n"
                     "At least one should be defined to load Phong program.\n";
        return 1;
      }

      const TCollection_AsciiString aSrcVert = aShadersRoot + "/PhongShading.vs";
      const TCollection_AsciiString aSrcFrag = aShadersRoot + "/PhongShading.fs";
      if (!aSrcVert.IsEmpty()
       && !OSD_File (aSrcVert).Exists())
      {
        std::cout << "Error: PhongShading.vs is not found\n";
        return 1;
      }
      if (!aSrcFrag.IsEmpty()
       && !OSD_File (aSrcFrag).Exists())
      {
        std::cout << "Error: PhongShading.fs is not found\n";
        return 1;
      }

      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile (Graphic3d_TOS_VERTEX,   aSrcVert));
      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromFile (Graphic3d_TOS_FRAGMENT, aSrcFrag));
    }
    else if (aPrsList.IsEmpty()
          && anArg == "*")
    {
      //
    }
    else if (!isSetGroupAspect
          &&  anArgIter + 1 < theArgNb
          && (anArg == "-primtype"
           || anArg == "-primitivetype"
           || anArg == "-groupaspect"
           || anArg == "-aspecttype"
           || anArg == "-aspect"))
    {
      isSetGroupAspect = true;
      TCollection_AsciiString aPrimTypeStr (theArgVec[++anArgIter]);
      aPrimTypeStr.LowerCase();
      if (aPrimTypeStr == "line")
      {
        aGroupAspect = Graphic3d_ASPECT_LINE;
      }
      else if (aPrimTypeStr == "tris"
            || aPrimTypeStr == "triangles"
            || aPrimTypeStr == "fill"
            || aPrimTypeStr == "fillarea"
            || aPrimTypeStr == "shading"
            || aPrimTypeStr == "shade")
      {
        aGroupAspect = Graphic3d_ASPECT_FILL_AREA;
      }
      else if (aPrimTypeStr == "text")
      {
        aGroupAspect = Graphic3d_ASPECT_TEXT;
      }
      else if (aPrimTypeStr == "marker"
            || aPrimTypeStr == "point"
            || aPrimTypeStr == "pnt")
      {
        aGroupAspect = Graphic3d_ASPECT_MARKER;
      }
      else
      {
        std::cerr << "Syntax error at '" << aPrimTypeStr << "'\n";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb
         && !aProgram.IsNull()
         &&  aProgram->Header().IsEmpty()
         &&  (anArg == "-version"
           || anArg == "-glslversion"
           || anArg == "-header"
           || anArg == "-glslheader"))
    {
      TCollection_AsciiString aHeader (theArgVec[++anArgIter]);
      if (aHeader.IsIntegerValue())
      {
        aHeader = TCollection_AsciiString ("#version ") + aHeader;
      }
      aProgram->SetHeader (aHeader);
    }
    else if (!anArg.StartsWith ("-")
          && GetMapOfAIS().IsBound2 (theArgVec[anArgIter]))
    {
      Handle(AIS_InteractiveObject) anIO = GetMapOfAIS().Find2 (theArgVec[anArgIter]);
      if (anIO.IsNull())
      {
        std::cerr << "Syntax error: " << theArgVec[anArgIter] << " is not an AIS object\n";
        return 1;
      }
      aPrsList.Append (anIO);
    }
    else if (!aProgram.IsNull()
           && ((anArgIter + 1 < theArgNb && parseShaderTypeArg (aShaderTypeArg, anArg))
            || (!isExplicitShaderType && aProgram->ShaderObjects().Size() < 2)))
    {
      TCollection_AsciiString aShaderPath (theArgVec[anArgIter]);
      if (aShaderTypeArg != Graphic3d_TypeOfShaderObject(-1))
      {
        aShaderPath = (theArgVec[++anArgIter]);
        isExplicitShaderType = true;
      }

      const bool isSrcFile = OSD_File (aShaderPath).Exists();
      Handle(Graphic3d_ShaderObject) aShader = isSrcFile
                                             ? Graphic3d_ShaderObject::CreateFromFile  (Graphic3d_TOS_VERTEX, aShaderPath)
                                             : Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, aShaderPath);
      const TCollection_AsciiString& aShaderSrc = aShader->Source();

      const bool hasVertPos   = aShaderSrc.Search ("gl_Position")  != -1;
      const bool hasFragColor = aShaderSrc.Search ("occSetFragColor") != -1
                             || aShaderSrc.Search ("occFragColor") != -1
                             || aShaderSrc.Search ("gl_FragColor") != -1
                             || aShaderSrc.Search ("gl_FragData")  != -1;
      Graphic3d_TypeOfShaderObject aShaderType = aShaderTypeArg;
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        if (hasVertPos
        && !hasFragColor)
        {
          aShaderType = Graphic3d_TOS_VERTEX;
        }
        if (hasFragColor
        && !hasVertPos)
        {
          aShaderType = Graphic3d_TOS_FRAGMENT;
        }
      }
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        std::cerr << "Error: non-existing or invalid shader source\n";
        return 1;
      }

      aProgram->AttachShader (Graphic3d_ShaderObject::CreateFromSource (aShaderType, aShaderSrc));
    }
    else
    {
      std::cerr << "Syntax error at '" << anArg << "'\n";
      return 1;
    }
  }

  if (!aProgram.IsNull()
    && ViewerTest::CurrentView()->RenderingParams().TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
  {
    aProgram->SetNbFragmentOutputs (2);
    aProgram->SetWeightOitOutput (true);
  }

  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName aGlobalPrsIter (GetMapOfAIS());
  NCollection_Sequence<Handle(AIS_InteractiveObject)>::Iterator aPrsIter (aPrsList);
  const bool isGlobalList = aPrsList.IsEmpty();
  for (;;)
  {
    Handle(AIS_InteractiveObject) anIO;
    if (isGlobalList)
    {
      if (!aGlobalPrsIter.More())
      {
        break;
      }
      anIO = aGlobalPrsIter.Key1();
      aGlobalPrsIter.Next();
      if (anIO.IsNull())
      {
        continue;
      }
    }
    else
    {
      if (!aPrsIter.More())
      {
        break;
      }
      anIO = aPrsIter.Value();
      aPrsIter.Next();
    }

    if (anIO->Attributes()->SetShaderProgram (aProgram, aGroupAspect, true))
    {
      aCtx->Redisplay (anIO, Standard_False);
    }
    else
    {
      anIO->SynchronizeAspects();
    }
  }

  aCtx->UpdateCurrentViewer();
  return 0;
}

//=======================================================================
//function : OpenGlCommands
//purpose  :
//=======================================================================

void ViewerTest::OpenGlCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup ="Commands for low-level TKOpenGl features";

  theCommands.Add("vuserdraw",
    "vuserdraw : name - simulates drawing with help of UserDraw",
    __FILE__, VUserDraw, aGroup);
  theCommands.Add("vfeedback",
    "vfeedback       : perform test GL feedback rendering",
    __FILE__, VFeedback, aGroup);
  theCommands.Add("vimmediatefront",
    "vimmediatefront : render immediate mode to front buffer or to back buffer",
    __FILE__, VImmediateFront, aGroup);
  theCommands.Add("vglinfo",
                "vglinfo [-short|-basic|-complete]"
        "\n\t\t:         [GL_VENDOR] [GL_RENDERER] [GL_VERSION]"
        "\n\t\t:         [GL_SHADING_LANGUAGE_VERSION] [GL_EXTENSIONS]"
        "\n\t\t: print OpenGL info",
    __FILE__, VGlInfo, aGroup);
  theCommands.Add("vshader",
                  "vshader name -vert VertexShader -frag FragmentShader [-geom GeometryShader]"
                  "\n\t\t:   [-off] [-phong] [-aspect {shading|line|point|text}=shading]"
                  "\n\t\t:   [-header VersionHeader]"
                  "\n\t\t:   [-tessControl TessControlShader -tesseval TessEvaluationShader]"
                  "\n\t\t: Assign custom GLSL program to presentation aspects.",
    __FILE__, VShaderProg, aGroup);
  theCommands.Add("vshaderprog", "Alias for vshader", __FILE__, VShaderProg, aGroup);
}
