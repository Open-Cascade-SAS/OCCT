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
#include <Draw.hxx>
#include <Draw_Interpretor.hxx>
#include <Image_AlienPixMap.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_FileSystem.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

#include <ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName.hxx>

extern ViewerTest_DoubleMapOfInteractiveAndName& GetMapOfAIS();

//=================================================================================================

static int VImmediateFront(Draw_Interpretor&, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the context
  Handle(AIS_InteractiveContext) aContextAIS = ViewerTest::GetAISContext();
  if (aContextAIS.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }

  Handle(Graphic3d_GraphicDriver) aDriver = aContextAIS->CurrentViewer()->Driver();
  if (aDriver.IsNull())
  {
    Message::SendFail("Error: graphic driver not available.");
    return 1;
  }

  if (theArgNb < 2)
  {
    Message::SendFail("Syntax error: wrong number of arguments.");
    return 1;
  }

  ViewerTest::CurrentView()->View()->SetImmediateModeDrawToFront(atoi(theArgVec[1]) != 0);

  return 0;
}

//! Search the info from the key.
inline TCollection_AsciiString searchInfo(const TColStd_IndexedDataMapOfStringString& theDict,
                                          const TCollection_AsciiString&              theKey)
{
  for (TColStd_IndexedDataMapOfStringString::Iterator anIter(theDict); anIter.More(); anIter.Next())
  {
    if (TCollection_AsciiString::IsSameString(anIter.Key(), theKey, Standard_False))
    {
      return anIter.Value();
    }
  }
  return TCollection_AsciiString();
}

//=================================================================================================

static int VGlInfo(Draw_Interpretor& theDI, Standard_Integer theArgNb, const char** theArgVec)
{
  // get the active view
  Handle(V3d_View) aView = ViewerTest::CurrentView();
  if (aView.IsNull())
  {
    Message::SendFail("No active viewer");
    return 1;
  }

  Graphic3d_DiagnosticInfo                      anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
  Standard_Integer                              aLineWidth  = 80;
  NCollection_Sequence<TCollection_AsciiString> aKeys;
  TColStd_IndexedDataMapOfStringString          aDict;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString aName(theArgVec[anArgIter]);
    aName.LowerCase();
    TCollection_AsciiString aValue;
    if (aName == "-short")
    {
      anInfoLevel = Graphic3d_DiagnosticInfo_Short;
    }
    else if (aName == "-basic")
    {
      anInfoLevel = Graphic3d_DiagnosticInfo_Basic;
    }
    else if (aName == "-complete" || aName == "-full")
    {
      anInfoLevel = Graphic3d_DiagnosticInfo_Complete;
    }
    else if (anArgIter + 1 < theArgNb
             && (aName == "-maxwidth" || aName == "-maxlinewidth" || aName == "-linewidth"))
    {
      aLineWidth = Draw::Atoi(theArgVec[++anArgIter]);
      if (aLineWidth < 0)
      {
        aLineWidth = IntegerLast();
      }
    }
    else if (aName.Search("vendor") != -1)
    {
      aKeys.Append("GLvendor");
    }
    else if (aName.Search("renderer") != -1)
    {
      aKeys.Append("GLdevice");
    }
    else if (aName.Search("shading_language_version") != -1 || aName.Search("glsl") != -1)
    {
      aKeys.Append("GLSLversion");
    }
    else if (aName.Search("version") != -1)
    {
      aKeys.Append("GLversion");
    }
    else if (aName.Search("extensions") != -1)
    {
      aKeys.Append("GLextensions");
    }
    else if (aName.Search("extensions") != -1)
    {
      aKeys.Append("GLextensions");
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown key '" << aName << "'";
      return 1;
    }
  }

  if (aKeys.IsEmpty())
  {
    aView->DiagnosticInformation(aDict, anInfoLevel);
    TCollection_AsciiString aText;
    for (TColStd_IndexedDataMapOfStringString::Iterator aValueIter(aDict); aValueIter.More();
         aValueIter.Next())
    {
      if (!aText.IsEmpty())
      {
        aText += "\n";
      }
      if ((aValueIter.Key().Length() + aValueIter.Value().Length() + 4) <= aLineWidth)
      {
        aText += TCollection_AsciiString("  ") + aValueIter.Key() + ": " + aValueIter.Value();
        continue;
      }

      // split into lines
      aText += TCollection_AsciiString("  ") + aValueIter.Key() + ":";
      TCollection_AsciiString aSubList;
      for (Standard_Integer aTokenIter = 1;; ++aTokenIter)
      {
        TCollection_AsciiString aToken = aValueIter.Value().Token(" ", aTokenIter);
        if (aToken.IsEmpty())
        {
          break;
        }

        if (!aSubList.IsEmpty() && (aSubList.Length() + aToken.Length() + 5) > aLineWidth)
        {
          aText += TCollection_AsciiString("\n    ") + aSubList;
          aSubList = aToken;
        }
        else
        {
          if (!aSubList.IsEmpty())
          {
            aSubList += " ";
          }
          aSubList += aToken;
        }
      }
      if (!aSubList.IsEmpty())
      {
        aText += TCollection_AsciiString("\n    ") + aSubList;
      }
    }

    theDI << "OpenGL info:\n" << aText;
    return 0;
  }

  aView->DiagnosticInformation(aDict, Graphic3d_DiagnosticInfo_Complete);
  for (NCollection_Sequence<TCollection_AsciiString>::Iterator aKeyIter(aKeys); aKeyIter.More();
       aKeyIter.Next())
  {
    TCollection_AsciiString aValue = searchInfo(aDict, aKeyIter.Value());
    if (aKeys.Length() > 1)
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
static bool parseShaderTypeArg(Graphic3d_TypeOfShaderObject&  theType,
                               const TCollection_AsciiString& theArg)
{
  if (theArg == "-vertex" || theArg == "-vert")
  {
    theType = Graphic3d_TOS_VERTEX;
  }
  else if (theArg == "-tessevaluation" || theArg == "-tesseval" || theArg == "-evaluation"
           || theArg == "-eval")
  {
    theType = Graphic3d_TOS_TESS_EVALUATION;
  }
  else if (theArg == "-tesscontrol" || theArg == "-tessctrl" || theArg == "-control"
           || theArg == "-ctrl")
  {
    theType = Graphic3d_TOS_TESS_CONTROL;
  }
  else if (theArg == "-geometry" || theArg == "-geom")
  {
    theType = Graphic3d_TOS_GEOMETRY;
  }
  else if (theArg == "-fragment" || theArg == "-frag")
  {
    theType = Graphic3d_TOS_FRAGMENT;
  }
  else if (theArg == "-compute" || theArg == "-comp")
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
// function : VShaderProg
// purpose  : Sets the pair of vertex and fragment shaders for the object
//==============================================================================
static Standard_Integer VShaderProg(Draw_Interpretor&,
                                    Standard_Integer theArgNb,
                                    const char**     theArgVec)
{
  Handle(AIS_InteractiveContext) aCtx = ViewerTest::GetAISContext();
  if (aCtx.IsNull())
  {
    Message::SendFail("Error: no active viewer");
    return 1;
  }
  else if (theArgNb < 2)
  {
    Message::SendFail("Syntax error: lack of arguments");
    return 1;
  }

  bool                                                isExplicitShaderType = false;
  Handle(Graphic3d_ShaderProgram)                     aProgram = new Graphic3d_ShaderProgram();
  NCollection_Sequence<Handle(AIS_InteractiveObject)> aPrsList;
  Graphic3d_GroupAspect                               aGroupAspect     = Graphic3d_ASPECT_FILL_AREA;
  bool                                                isSetGroupAspect = false;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    Graphic3d_TypeOfShaderObject aShaderTypeArg = Graphic3d_TypeOfShaderObject(-1);
    if (!aProgram.IsNull() && anArg == "-uniform" && anArgIter + 2 < theArgNb)
    {
      TCollection_AsciiString aName = theArgVec[++anArgIter];
      aProgram->PushVariableFloat(aName, float(Draw::Atof(theArgVec[++anArgIter])));
    }
    else if (!aProgram.IsNull() && aProgram->ShaderObjects().IsEmpty()
             && (anArg == "-off" || anArg == "off"))
    {
      aProgram.Nullify();
    }
    else if (!aProgram.IsNull() && aProgram->ShaderObjects().IsEmpty()
             && (anArg == "-phong" || anArg == "phong"))
    {
      const TCollection_AsciiString& aShadersRoot = Graphic3d_ShaderProgram::ShadersFolder();
      if (aShadersRoot.IsEmpty())
      {
        Message::SendFail(
          "Error: both environment variables CSF_ShadersDirectory and CASROOT are undefined!\n"
          "At least one should be defined to load Phong program.");
        return 1;
      }

      const TCollection_AsciiString aSrcVert = aShadersRoot + "/PhongShading.vs";
      const TCollection_AsciiString aSrcFrag = aShadersRoot + "/PhongShading.fs";
      if (!aSrcVert.IsEmpty() && !OSD_File(aSrcVert).Exists())
      {
        Message::SendFail("Error: PhongShading.vs is not found");
        return 1;
      }
      if (!aSrcFrag.IsEmpty() && !OSD_File(aSrcFrag).Exists())
      {
        Message::SendFail("Error: PhongShading.fs is not found");
        return 1;
      }

      aProgram->AttachShader(
        Graphic3d_ShaderObject::CreateFromFile(Graphic3d_TOS_VERTEX, aSrcVert));
      aProgram->AttachShader(
        Graphic3d_ShaderObject::CreateFromFile(Graphic3d_TOS_FRAGMENT, aSrcFrag));
    }
    else if (aPrsList.IsEmpty() && anArg == "*")
    {
      //
    }
    else if (!isSetGroupAspect && anArgIter + 1 < theArgNb
             && (anArg == "-primtype" || anArg == "-primitivetype" || anArg == "-groupaspect"
                 || anArg == "-aspecttype" || anArg == "-aspect"))
    {
      isSetGroupAspect = true;
      TCollection_AsciiString aPrimTypeStr(theArgVec[++anArgIter]);
      aPrimTypeStr.LowerCase();
      if (aPrimTypeStr == "line")
      {
        aGroupAspect = Graphic3d_ASPECT_LINE;
      }
      else if (aPrimTypeStr == "tris" || aPrimTypeStr == "triangles" || aPrimTypeStr == "fill"
               || aPrimTypeStr == "fillarea" || aPrimTypeStr == "shading"
               || aPrimTypeStr == "shade")
      {
        aGroupAspect = Graphic3d_ASPECT_FILL_AREA;
      }
      else if (aPrimTypeStr == "text")
      {
        aGroupAspect = Graphic3d_ASPECT_TEXT;
      }
      else if (aPrimTypeStr == "marker" || aPrimTypeStr == "point" || aPrimTypeStr == "pnt")
      {
        aGroupAspect = Graphic3d_ASPECT_MARKER;
      }
      else
      {
        Message::SendFail() << "Syntax error at '" << aPrimTypeStr << "'";
        return 1;
      }
    }
    else if (anArgIter + 1 < theArgNb && !aProgram.IsNull() && aProgram->Header().IsEmpty()
             && (anArg == "-version" || anArg == "-glslversion" || anArg == "-header"
                 || anArg == "-glslheader"))
    {
      TCollection_AsciiString aHeader(theArgVec[++anArgIter]);
      if (aHeader.IsIntegerValue())
      {
        aHeader = TCollection_AsciiString("#version ") + aHeader;
      }
      aProgram->SetHeader(aHeader);
    }
    else if (!aProgram.IsNull()
             && (anArg == "-defaultsampler" || anArg == "-defampler" || anArg == "-nodefaultsampler"
                 || anArg == "-nodefsampler"))
    {
      bool toUseDefSampler = Draw::ParseOnOffNoIterator(theArgNb, theArgVec, anArgIter);
      aProgram->SetDefaultSampler(toUseDefSampler);
    }
    else if (!anArg.StartsWith("-") && GetMapOfAIS().IsBound2(theArgVec[anArgIter]))
    {
      Handle(AIS_InteractiveObject) anIO = GetMapOfAIS().Find2(theArgVec[anArgIter]);
      if (anIO.IsNull())
      {
        Message::SendFail() << "Syntax error: " << theArgVec[anArgIter] << " is not an AIS object";
        return 1;
      }
      aPrsList.Append(anIO);
    }
    else if (!aProgram.IsNull()
             && ((anArgIter + 1 < theArgNb && parseShaderTypeArg(aShaderTypeArg, anArg))
                 || (!isExplicitShaderType && aProgram->ShaderObjects().Size() < 2)))
    {
      TCollection_AsciiString aShaderPath(theArgVec[anArgIter]);
      if (aShaderTypeArg != Graphic3d_TypeOfShaderObject(-1))
      {
        aShaderPath          = (theArgVec[++anArgIter]);
        isExplicitShaderType = true;
      }

      const bool                     isSrcFile = OSD_File(aShaderPath).Exists();
      Handle(Graphic3d_ShaderObject) aShader =
        isSrcFile ? Graphic3d_ShaderObject::CreateFromFile(Graphic3d_TOS_VERTEX, aShaderPath)
                  : Graphic3d_ShaderObject::CreateFromSource(Graphic3d_TOS_VERTEX, aShaderPath);
      const TCollection_AsciiString& aShaderSrc = aShader->Source();

      const bool hasVertPos = aShaderSrc.Search("gl_Position") != -1;
      const bool hasFragColor =
        aShaderSrc.Search("occSetFragColor") != -1 || aShaderSrc.Search("occFragColor") != -1
        || aShaderSrc.Search("gl_FragColor") != -1 || aShaderSrc.Search("gl_FragData") != -1;
      Graphic3d_TypeOfShaderObject aShaderType = aShaderTypeArg;
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        if (hasVertPos && !hasFragColor)
        {
          aShaderType = Graphic3d_TOS_VERTEX;
        }
        if (hasFragColor && !hasVertPos)
        {
          aShaderType = Graphic3d_TOS_FRAGMENT;
        }
      }
      if (aShaderType == Graphic3d_TypeOfShaderObject(-1))
      {
        Message::SendFail() << "Error: non-existing or invalid shader source";
        return 1;
      }

      aProgram->AttachShader(Graphic3d_ShaderObject::CreateFromSource(aShaderType, aShaderSrc));
    }
    else
    {
      Message::SendFail() << "Syntax error at '" << anArg << "'";
      return 1;
    }
  }

  if (!aProgram.IsNull()
      && ViewerTest::CurrentView()->RenderingParams().TransparencyMethod == Graphic3d_RTM_BLEND_OIT)
  {
    aProgram->SetNbFragmentOutputs(2);
    aProgram->SetOitOutput(Graphic3d_RTM_BLEND_OIT);
  }

  ViewerTest_DoubleMapIteratorOfDoubleMapOfInteractiveAndName   aGlobalPrsIter(GetMapOfAIS());
  NCollection_Sequence<Handle(AIS_InteractiveObject)>::Iterator aPrsIter(aPrsList);
  const bool                                                    isGlobalList = aPrsList.IsEmpty();
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

    if (anIO->Attributes()->SetShaderProgram(aProgram, aGroupAspect, true))
    {
      aCtx->Redisplay(anIO, Standard_False);
    }
    else
    {
      anIO->SynchronizeAspects();
    }
  }

  aCtx->UpdateCurrentViewer();
  return 0;
}

//! Print triplet of values.
template <class S, class T>
static S& operator<<(S& theStream, const NCollection_Vec3<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2];
  return theStream;
}

//! Print 4 values.
template <class S, class T>
static S& operator<<(S& theStream, const NCollection_Vec4<T>& theVec)
{
  theStream << theVec[0] << " " << theVec[1] << " " << theVec[2] << " " << theVec[3];
  return theStream;
}

//! Print fresnel model.
static const char* fresnelModelString(const Graphic3d_FresnelModel theModel)
{
  switch (theModel)
  {
    case Graphic3d_FM_SCHLICK:
      return "SCHLICK";
    case Graphic3d_FM_CONSTANT:
      return "CONSTANT";
    case Graphic3d_FM_CONDUCTOR:
      return "CONDUCTOR";
    case Graphic3d_FM_DIELECTRIC:
      return "DIELECTRIC";
  }
  return "N/A";
}

//! Create a colored rectangle SVG element.
static TCollection_AsciiString formatSvgColoredRect(const Quantity_Color& theColor)
{
  return TCollection_AsciiString()
         + "<svg width='20px' height='20px'><rect width='20px' height='20px' fill='"
         + Quantity_Color::ColorToHex(theColor) + "' /></svg>";
}

//=================================================================================================

static Standard_Integer VListMaterials(Draw_Interpretor& theDI,
                                       Standard_Integer  theArgNb,
                                       const char**      theArgVec)
{
  TCollection_AsciiString                        aDumpFile;
  NCollection_Sequence<Graphic3d_NameOfMaterial> aMatList;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    Graphic3d_NameOfMaterial aMat =
      Graphic3d_MaterialAspect::MaterialFromName(theArgVec[anArgIter]);
    if (aMat != Graphic3d_NameOfMaterial_DEFAULT)
    {
      aMatList.Append(aMat);
    }
    else if (anArg == "*")
    {
      for (Standard_Integer aMatIter = 0;
           aMatIter < (Standard_Integer)Graphic3d_NameOfMaterial_DEFAULT;
           ++aMatIter)
      {
        aMatList.Append((Graphic3d_NameOfMaterial)aMatIter);
      }
    }
    else if (aDumpFile.IsEmpty()
             && (anArg.EndsWith(".obj") || anArg.EndsWith(".mtl") || anArg.EndsWith(".htm")
                 || anArg.EndsWith(".html")))
    {
      aDumpFile = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aMatList.IsEmpty())
  {
    if (aDumpFile.IsEmpty())
    {
      for (Standard_Integer aMatIter = 1; aMatIter <= Graphic3d_MaterialAspect::NumberOfMaterials();
           ++aMatIter)
      {
        theDI << Graphic3d_MaterialAspect::MaterialName(aMatIter) << " ";
      }
      return 0;
    }

    for (Standard_Integer aMatIter = 0;
         aMatIter < (Standard_Integer)Graphic3d_NameOfMaterial_DEFAULT;
         ++aMatIter)
    {
      aMatList.Append((Graphic3d_NameOfMaterial)aMatIter);
    }
  }

  // geometry for dumping
  const Graphic3d_Vec3 aBoxVerts[8] = {Graphic3d_Vec3(1, -1, -1),
                                       Graphic3d_Vec3(1, -1, 1),
                                       Graphic3d_Vec3(-1, -1, 1),
                                       Graphic3d_Vec3(-1, -1, -1),
                                       Graphic3d_Vec3(1, 1, -1),
                                       Graphic3d_Vec3(1, 1, 1),
                                       Graphic3d_Vec3(-1, 1, 1),
                                       Graphic3d_Vec3(-1, 1, -1)};

  const Graphic3d_Vec4i aBoxQuads[6] = {Graphic3d_Vec4i(1, 2, 3, 4),
                                        Graphic3d_Vec4i(5, 8, 7, 6),
                                        Graphic3d_Vec4i(1, 5, 6, 2),
                                        Graphic3d_Vec4i(2, 6, 7, 3),
                                        Graphic3d_Vec4i(3, 7, 8, 4),
                                        Graphic3d_Vec4i(5, 1, 4, 8)};

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aMatFile, anObjFile, aHtmlFile;
  if (aDumpFile.EndsWith(".obj") || aDumpFile.EndsWith(".mtl"))
  {
    const TCollection_AsciiString aMatFilePath =
      aDumpFile.SubString(1, aDumpFile.Length() - 3) + "mtl";
    const TCollection_AsciiString anObjFilePath =
      aDumpFile.SubString(1, aDumpFile.Length() - 3) + "obj";

    aMatFile = aFileSystem->OpenOStream(aMatFilePath, std::ios::out | std::ios::binary);
    if (aMatFile.get() == NULL)
    {
      Message::SendFail("Error: unable creating material file");
      return 0;
    }
    if (!aDumpFile.EndsWith(".mtl"))
    {
      anObjFile = aFileSystem->OpenOStream(anObjFilePath, std::ios::out | std::ios::binary);
      if (anObjFile.get() == NULL)
      {
        Message::SendFail("Error: unable creating OBJ file");
        return 0;
      }

      TCollection_AsciiString anMtlName, aFolder;
      OSD_Path::FolderAndFileFromPath(aMatFilePath, aFolder, anMtlName);
      *anObjFile << "mtllib " << anMtlName << "\n";
    }
  }
  else if (aDumpFile.EndsWith(".htm") || aDumpFile.EndsWith(".html"))
  {
    aHtmlFile = aFileSystem->OpenOStream(aDumpFile, std::ios::out | std::ios::binary);
    if (aHtmlFile.get() == NULL)
    {
      Message::SendFail("Error: unable creating HTML file");
      return 0;
    }
    *aHtmlFile
      << "<html>\n"
         "<head><title>OCCT Material table</title></head>\n"
         "<body>\n"
         "<table border='1'><tbody>\n"
         "<tr>\n"
         "<th rowspan='2'><div title='Material name.\n"
         "See also Graphic3d_NameOfMaterial enumeration'>"
         "Name</div></th>\n"
         "<th rowspan='2'><div title='Material type: PHYSIC or ASPECT.\n"
         "ASPECT material does not define final colors, it is taken from Internal Color instead.\n"
         "See also Graphic3d_TypeOfMaterial enumeration'>"
         "Type</div></th>\n"
         "<th rowspan='2'>Transparency</th>\n"
         "<th colspan='5'><div title='PBR Metallic-Roughness'>"
         "PBR Metallic-Roughness</div></th>\n"
         "<th colspan='5'><div title='Common material definition for Phong shading model'>"
         "Common (Blinn-Phong)</div></th>\n"
         "<th colspan='10'><div title='BSDF (Bidirectional Scattering Distribution Function).\n"
         "Used for physically-based rendering (in path tracing engine).\n"
         "BSDF is represented as weighted mixture of basic BRDFs/BTDFs (Bidirectional Reflectance "
         "(Transmittance) Distribution Functions).\n"
         "See also Graphic3d_BSDF structure.'>"
         "BSDF (Bidirectional Scattering Distribution Function)</div></th>\n"
         "</tr>\n"
         "<tr>\n"
         "<th>Color</th>\n"
         "<th>Metallic</th>\n"
         "<th>Roughness</th>\n"
         "<th>Emission</th>\n"
         "<th><div title='Index of refraction'>"
         "IOR</div></th>\n"
         "<th>Ambient</th>\n"
         "<th>Diffuse</th>\n"
         "<th>Specular</th>\n"
         "<th>Emissive</th>\n"
         "<th>Shiness</th>\n"
         "<th><div title='Weight of coat specular/glossy BRDF'>"
         "Kc</div></th>\n"
         "<th><div title='Weight of base diffuse BRDF'>"
         "Kd</div></th>\n"
         "<th><div title='Weight of base specular/glossy BRDF'>"
         "Ks</div></th>\n"
         "<th><div title='Weight of base specular/glossy BTDF'>"
         "Kt</div></th>\n"
         "<th><div title='Radiance emitted by the surface'>"
         "Le</div></th>\n"
         "<th><div title='Volume scattering color/density'>"
         "Absorption</div></th>\n"
         "<th><div title='Parameters of Fresnel reflectance of coat layer'>"
         "FresnelCoat</div></th>\n"
         "<th><div title='Parameters of Fresnel reflectance of base layer'>"
         "FresnelBase</div></th>\n"
         "<th>Refraction Index</th>\n"
         "</tr>\n";
  }
  else if (!aDumpFile.IsEmpty())
  {
    Message::SendFail("Syntax error: unknown output file format");
    return 1;
  }

  Standard_Integer aMatIndex = 0, anX = 0, anY = 0;
  for (NCollection_Sequence<Graphic3d_NameOfMaterial>::Iterator aMatIter(aMatList); aMatIter.More();
       aMatIter.Next(), ++aMatIndex)
  {
    Graphic3d_MaterialAspect       aMat(aMatIter.Value());
    const TCollection_AsciiString& aMatName   = aMat.StringName();
    const Graphic3d_Vec3           anAmbient  = (Graphic3d_Vec3)aMat.AmbientColor();
    const Graphic3d_Vec3           aDiffuse   = (Graphic3d_Vec3)aMat.DiffuseColor();
    const Graphic3d_Vec3           aSpecular  = (Graphic3d_Vec3)aMat.SpecularColor();
    const Graphic3d_Vec3           anEmission = (Graphic3d_Vec3)aMat.EmissiveColor();
    const Standard_Real            aShiness   = aMat.Shininess() * 1000.0;
    if (aMatFile.get() != NULL)
    {
      *aMatFile << "newmtl " << aMatName << "\n";
      *aMatFile << "Ka " << Quantity_Color::Convert_LinearRGB_To_sRGB(anAmbient) << "\n";
      *aMatFile << "Kd " << Quantity_Color::Convert_LinearRGB_To_sRGB(aDiffuse) << "\n";
      *aMatFile << "Ks " << Quantity_Color::Convert_LinearRGB_To_sRGB(aSpecular) << "\n";
      *aMatFile << "Ns " << aShiness << "\n";
      if (aMat.Transparency() >= 0.0001)
      {
        *aMatFile << "Tr " << aMat.Transparency() << "\n";
      }
      *aMatFile << "\n";
    }
    else if (aHtmlFile.get() != NULL)
    {
      *aHtmlFile << "<tr>\n";
      *aHtmlFile << "<td>" << aMat.StringName() << "</td>\n";
      *aHtmlFile << "<td>"
                 << (aMat.MaterialType() == Graphic3d_MATERIAL_PHYSIC ? "PHYSIC" : "ASPECT")
                 << "</td>\n";
      *aHtmlFile << "<td>" << aMat.Transparency() << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(aMat.PBRMaterial().Color().GetRGB())
                 << (Graphic3d_Vec3)aMat.PBRMaterial().Color().GetRGB() << "</td>\n";
      *aHtmlFile << "<td>" << aMat.PBRMaterial().Metallic() << "</td>\n";
      *aHtmlFile << "<td>" << aMat.PBRMaterial().NormalizedRoughness() << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(Quantity_Color(aMat.PBRMaterial().Emission()))
                 << aMat.PBRMaterial().Emission() << "</td>\n";
      *aHtmlFile << "<td>" << aMat.PBRMaterial().IOR() << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(Quantity_Color(anAmbient)) << anAmbient
                 << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(Quantity_Color(aDiffuse)) << aDiffuse
                 << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(Quantity_Color(aSpecular)) << aSpecular
                 << "</td>\n";
      *aHtmlFile << "<td>" << formatSvgColoredRect(Quantity_Color(anEmission)) << anEmission
                 << "</td>\n";
      *aHtmlFile << "<td>" << aMat.Shininess() << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Kc << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Kd << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Ks << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Kt << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Le << "</td>\n";
      *aHtmlFile << "<td>" << aMat.BSDF().Absorption << "</td>\n";
      *aHtmlFile << "<td>" << fresnelModelString(aMat.BSDF().FresnelCoat.FresnelType())
                 << "</td>\n";
      *aHtmlFile << "<td>" << fresnelModelString(aMat.BSDF().FresnelBase.FresnelType())
                 << "</td>\n";
      *aHtmlFile << "<td>" << aMat.RefractionIndex() << "</td>\n";
      *aHtmlFile << "</tr>\n";
    }
    else
    {
      theDI << aMat.StringName() << "\n";
      theDI << "  Transparency:           " << aMat.Transparency() << "\n";
      theDI << "  PBR.BaseColor:          " << (Graphic3d_Vec3)aMat.PBRMaterial().Color().GetRGB()
            << "\n";
      theDI << "  PBR.Metallic:           " << aMat.PBRMaterial().Metallic() << "\n";
      theDI << "  PBR.Roughness:          " << aMat.PBRMaterial().NormalizedRoughness() << "\n";
      theDI << "  PBR.Emission:           " << aMat.PBRMaterial().Emission() << "\n";
      theDI << "  PBR.IOR:                " << aMat.PBRMaterial().IOR() << "\n";
      theDI << "  Common.Ambient:         " << anAmbient << "\n";
      theDI << "  Common.Diffuse:         " << aDiffuse << "\n";
      theDI << "  Common.Specular:        " << aSpecular << "\n";
      theDI << "  Common.Emissive:        " << anEmission << "\n";
      theDI << "  Common.Shiness:         " << aMat.Shininess() << "\n";
      theDI << "  BSDF.Kc:                " << aMat.BSDF().Kc << "\n";
      theDI << "  BSDF.Kd:                " << aMat.BSDF().Kd << "\n";
      theDI << "  BSDF.Ks:                " << aMat.BSDF().Ks << "\n";
      theDI << "  BSDF.Kt:                " << aMat.BSDF().Kt << "\n";
      theDI << "  BSDF.Le:                " << aMat.BSDF().Le << "\n";
      theDI << "  BSDF.Absorption:        " << aMat.BSDF().Absorption << "\n";
      theDI << "  BSDF.FresnelCoat:       "
            << fresnelModelString(aMat.BSDF().FresnelCoat.FresnelType()) << "\n";
      theDI << "  BSDF.FresnelBase:       "
            << fresnelModelString(aMat.BSDF().FresnelBase.FresnelType()) << "\n";
      theDI << "  RefractionIndex:        " << aMat.RefractionIndex() << "\n";
    }

    if (anObjFile.get() != NULL)
    {
      *anObjFile << "g " << aMatName << "\n";
      *anObjFile << "usemtl " << aMatName << "\n";
      for (Standard_Integer aVertIter = 0; aVertIter < 8; ++aVertIter)
      {
        *anObjFile << "v " << (aBoxVerts[aVertIter] + Graphic3d_Vec3(3.0f * anX, -3.0f * anY, 0.0f))
                   << "\n";
      }
      *anObjFile << "s off\n";
      for (Standard_Integer aFaceIter = 0; aFaceIter < 6; ++aFaceIter)
      {
        *anObjFile << "f " << (aBoxQuads[aFaceIter] + Graphic3d_Vec4i(8 * aMatIndex)) << "\n";
      }
      *anObjFile << "\n";
      if (++anX > 5)
      {
        anX = 0;
        ++anY;
      }
    }
  }

  if (aHtmlFile.get() != NULL)
  {
    *aHtmlFile << "</tbody></table>\n</body>\n</html>\n";
  }
  return 0;
}

//=================================================================================================

static Standard_Integer VListColors(Draw_Interpretor& theDI,
                                    Standard_Integer  theArgNb,
                                    const char**      theArgVec)
{
  TCollection_AsciiString                    aDumpFile;
  NCollection_Sequence<Quantity_NameOfColor> aColList;
  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();
    Quantity_NameOfColor aName;
    if (Quantity_Color::ColorFromName(theArgVec[anArgIter], aName))
    {
      aColList.Append(aName);
    }
    else if (anArg == "*")
    {
      for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer)Quantity_NOC_WHITE;
           ++aColIter)
      {
        aColList.Append((Quantity_NameOfColor)aColIter);
      }
    }
    else if (aDumpFile.IsEmpty() && (anArg.EndsWith(".htm") || anArg.EndsWith(".html")))
    {
      aDumpFile = theArgVec[anArgIter];
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument '" << theArgVec[anArgIter] << "'";
      return 1;
    }
  }
  if (aColList.IsEmpty())
  {
    if (aDumpFile.IsEmpty())
    {
      for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer)Quantity_NOC_WHITE;
           ++aColIter)
      {
        theDI << Quantity_Color::StringName(Quantity_NameOfColor(aColIter)) << " ";
      }
      return 0;
    }

    for (Standard_Integer aColIter = 0; aColIter <= (Standard_Integer)Quantity_NOC_WHITE;
         ++aColIter)
    {
      aColList.Append((Quantity_NameOfColor)aColIter);
    }
  }

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aHtmlFile;
  TCollection_AsciiString       aFileNameBase, aFolder;
  if (aDumpFile.EndsWith(".htm") || aDumpFile.EndsWith(".html"))
  {
    OSD_Path::FolderAndFileFromPath(aDumpFile, aFolder, aFileNameBase);
    aFileNameBase =
      aFileNameBase.SubString(1, aFileNameBase.Length() - (aDumpFile.EndsWith(".htm") ? 4 : 5));
  }
  else if (!aDumpFile.IsEmpty())
  {
    Message::SendFail("Syntax error: unknown output file format");
    return 1;
  }

  Standard_Integer aMaxNameLen = 1;
  for (NCollection_Sequence<Quantity_NameOfColor>::Iterator aColIter(aColList); aColIter.More();
       aColIter.Next())
  {
    aMaxNameLen =
      Max(aMaxNameLen,
          TCollection_AsciiString(Quantity_Color::StringName(aColIter.Value())).Length());
  }

  V3d_ImageDumpOptions anImgParams;
  anImgParams.Width          = 60;
  anImgParams.Height         = 30;
  anImgParams.BufferType     = Graphic3d_BT_RGB;
  anImgParams.StereoOptions  = V3d_SDO_MONO;
  anImgParams.ToAdjustAspect = Standard_True;
  Handle(V3d_View) aView;
  if (!aDumpFile.IsEmpty())
  {
    ViewerTest_VinitParams aParams;
    aParams.Size.SetValues((float)anImgParams.Width, (float)anImgParams.Height);
    aParams.ViewName = "TmpDriver/TmpViewer/TmpView";
    ViewerTest::ViewerInit(aParams);
    aView = ViewerTest::CurrentView();
    aView->SetImmediateUpdate(false);
    aView->SetBgGradientStyle(Aspect_GradientFillMethod_None, false);
  }

  if (!aDumpFile.IsEmpty())
  {
    aHtmlFile = aFileSystem->OpenOStream(aDumpFile, std::ios::out | std::ios::binary);
    if (aHtmlFile.get() == NULL)
    {
      Message::SendFail("Error: unable creating HTML file");
      return 0;
    }
    *aHtmlFile << "<html>\n"
               << "<head><title>OCCT Color table</title></head>\n"
               << "<body>\n"
               << "<table border='1'><tbody>\n"
               << "<tr>\n"
               << "<th>HTML</th>\n"
               << "<th>OCCT</th>\n"
               << "<th>Color name</th>\n"
               << "<th>sRGB hex</th>\n"
               << "<th>sRGB dec</th>\n"
               << "<th>RGB linear</th>\n"
               << "</tr>\n";
  }

  Image_AlienPixMap anImg;
  Standard_Integer  aColIndex = 0;
  for (NCollection_Sequence<Quantity_NameOfColor>::Iterator aColIter(aColList); aColIter.More();
       aColIter.Next(), ++aColIndex)
  {
    Quantity_Color                aCol(aColIter.Value());
    const TCollection_AsciiString aColName  = Quantity_Color::StringName(aColIter.Value());
    const TCollection_AsciiString anSRgbHex = Quantity_Color::ColorToHex(aCol);
    const Graphic3d_Vec3i         anSRgbInt((Graphic3d_Vec3)aCol * 255.0f);
    if (aHtmlFile.get() != NULL)
    {
      const TCollection_AsciiString anImgPath = aFileNameBase + "_" + aColName + ".png";
      if (!aView.IsNull())
      {
        aView->SetImmediateUpdate(false);
        aView->SetBackgroundColor(aCol);
        if (!aView->ToPixMap(anImg, anImgParams) || !anImg.Save(aFolder + anImgPath))
        {
          theDI << "Error: image dump failed\n";
          return 0;
        }
      }

      *aHtmlFile << "<tr>\n";
      *aHtmlFile << "<td style='background-color:" << anSRgbHex << "'><pre>       </pre></td>\n";
      *aHtmlFile << "<td><img src='" << (!aView.IsNull() ? anImgPath : "") << "'></img></td>\n";
      *aHtmlFile << "<td style='text-align:left'>" << aColName << "</td>\n";
      *aHtmlFile << "<td style='text-align:left'><pre>" << anSRgbHex << "</pre></td>\n";
      *aHtmlFile << "<td style='text-align:left'>(" << anSRgbInt.r() << " " << anSRgbInt.g() << " "
                 << anSRgbInt.b() << ")</td>\n";
      *aHtmlFile << "<td style='text-align:left'>(" << aCol.Red() << " " << aCol.Green() << " "
                 << aCol.Blue() << ")</td>\n";
      *aHtmlFile << "</tr>\n";
    }
    else
    {
      TCollection_AsciiString aColNameLong(aColName);
      aColNameLong.RightJustify(aMaxNameLen, ' ');
      theDI << aColNameLong << " [" << anSRgbHex << "]: " << aCol.Red() << " " << aCol.Green()
            << " " << aCol.Blue() << "\n";
    }
  }

  if (!aView.IsNull())
  {
    ViewerTest::RemoveView(aView);
  }

  if (aHtmlFile.get() != NULL)
  {
    *aHtmlFile << "</tbody></table>\n</body>\n</html>\n";
  }
  return 0;
}

//=================================================================================================

static std::string envLutWriteToFile(Standard_ShortReal theValue)
{
  std::stringstream aStream;
  aStream << theValue;
  if (aStream.str().length() == 1)
  {
    aStream << '.';
  }
  aStream << 'f';
  return aStream.str();
}

//=================================================================================================

static Standard_Integer VGenEnvLUT(Draw_Interpretor&,
                                   Standard_Integer theArgNb,
                                   const char**     theArgVec)
{
  Standard_Integer        aTableSize = -1;
  Standard_Integer        aNbSamples = -1;
  TCollection_AsciiString aFilePath =
    Graphic3d_TextureRoot::TexturesFolder() + "/Textures_EnvLUT.pxx";

  for (Standard_Integer anArgIter = 1; anArgIter < theArgNb; ++anArgIter)
  {
    TCollection_AsciiString anArg(theArgVec[anArgIter]);
    anArg.LowerCase();

    if (anArg == "-size" || anArg == "-s")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail("Syntax error: size of PBR environment look up table is undefined");
        return 1;
      }

      aTableSize = Draw::Atoi(theArgVec[++anArgIter]);

      if (aTableSize < 16)
      {
        Message::SendFail(
          "Error: size of PBR environment look up table must be greater or equal 16");
        return 1;
      }
    }
    else if (anArg == "-nbsamples" || anArg == "-samples")
    {
      if (anArgIter + 1 >= theArgNb)
      {
        Message::SendFail(
          "Syntax error: number of samples to generate PBR environment look up table is undefined");
        return 1;
      }

      aNbSamples = Draw::Atoi(theArgVec[++anArgIter]);

      if (aNbSamples < 1)
      {
        Message::SendFail("Syntax error: number of samples to generate PBR environment look up "
                          "table must be greater than 1");
        return 1;
      }
    }
    else
    {
      Message::SendFail() << "Syntax error: unknown argument " << anArg;
      return 1;
    }
  }

  if (aTableSize < 0)
  {
    aTableSize = 128;
  }

  if (aNbSamples < 0)
  {
    aNbSamples = 1024;
  }

  const Handle(OSD_FileSystem)& aFileSystem = OSD_FileSystem::DefaultFileSystem();
  std::shared_ptr<std::ostream> aFile =
    aFileSystem->OpenOStream(aFilePath, std::ios::out | std::ios::trunc);

  if (aFile.get() == NULL || !aFile->good())
  {
    Message::SendFail() << "Error: unable to write to " << aFilePath;
    return 1;
  }

  *aFile << "//this file has been generated by vgenenvlut draw command\n";
  *aFile << "static unsigned int Textures_EnvLUTSize = " << aTableSize << ";\n\n";
  *aFile << "static float Textures_EnvLUT[] =\n";
  *aFile << "{\n";

  Handle(Image_PixMap) aPixMap = new Image_PixMap();
  aPixMap->InitZero(Image_Format_RGF, aTableSize, aTableSize);
  Graphic3d_PBRMaterial::GenerateEnvLUT(aPixMap, aNbSamples);

  const Standard_Integer aNumbersInRow = 5;
  Standard_Integer       aCounter      = 0;

  for (int y = 0; y < aTableSize - 1; ++y)
  {
    aCounter = 0;
    for (int x = 0; x < aTableSize; ++x)
    {
      *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(aTableSize - 1 - y, x).x()) << ",";
      *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(aTableSize - 1 - y, x).y()) << ",";
      if (++aCounter % aNumbersInRow == 0)
      {
        *aFile << "\n";
      }
      else if (x != aTableSize - 1)
      {
        *aFile << " ";
      }
    }
    *aFile << "\n";
    if (aTableSize % aNumbersInRow != 0)
    {
      *aFile << "\n";
    }
  }

  aCounter = 0;
  for (int x = 0; x < aTableSize - 1; ++x)
  {
    *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(0, x).x()) << ",";
    *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(0, x).y()) << ",";
    if (++aCounter % aNumbersInRow == 0)
    {
      *aFile << "\n";
    }
    else
    {
      *aFile << " ";
    }
  }

  *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(0, aTableSize - 1).x()) << ",";
  *aFile << envLutWriteToFile(aPixMap->Value<Graphic3d_Vec3>(0, aTableSize - 1).y()) << "\n";

  *aFile << "};";

  return 0;
}

//=================================================================================================

void ViewerTest::OpenGlCommands(Draw_Interpretor& theCommands)
{
  const char* aGroup    = "AIS Viewer";
  const char* aFileName = __FILE__;
  auto        addCmd =
    [&](const char* theName, Draw_Interpretor::CommandFunction theFunc, const char* theHelp) {
      theCommands.Add(theName, theHelp, aFileName, theFunc, aGroup);
    };

  addCmd("vimmediatefront", VImmediateFront, /* [vimmediatefront] */ R"(
vimmediatefront : render immediate mode to front buffer or to back buffer
)" /* [vimmediatefront] */);

  addCmd("vglinfo", VGlInfo, /* [vglinfo] */ R"(
vglinfo [-short|-basic|-complete] [-lineWidth Value=80]
        [GL_VENDOR] [GL_RENDERER] [GL_VERSION]
        [GL_SHADING_LANGUAGE_VERSION] [GL_EXTENSIONS]
Print OpenGL info.
 -lineWidth split values longer than specified value into multiple lines;
            -1 disables splitting.
)" /* [vglinfo] */);

  addCmd("vshader", VShaderProg, /* [vshader] */ R"(
vshader name -vert VertexShader -frag FragmentShader [-geom GeometryShader]
        [-off] [-phong] [-aspect {shading|line|point|text}=shading]
        [-header VersionHeader]
        [-tessControl TessControlShader -tessEval TessEvaluationShader]
        [-uniform Name FloatValue]
        [-defaultSampler {0|1}]=1
Assign custom GLSL program to presentation aspects.
)" /* [vshader] */);

  addCmd("vshaderprog", VShaderProg, /* [vshaderprog] */ R"(
Alias for vshader
)" /* [vshaderprog] */);

  addCmd("vlistmaterials", VListMaterials, /* [vlistmaterials] */ R"(
vlistmaterials [*] [MaterialName1 [MaterialName2 [...]]] [dump.obj|dump.html]
Without arguments, command prints the list of standard materials.
Otherwise, properties of specified materials will be printed
or dumped into specified file.
* can be used to refer to complete list of standard materials.
)" /* [vlistmaterials] */);

  addCmd("vlistcolors", VListColors, /* [vlistcolors] */ R"(
vlistcolors [*] [ColorName1 [ColorName2 [...]]] [dump.html]
Without arguments, command prints the list of standard colors.
Otherwise, properties of specified colors will be printed
or dumped into specified file.
* can be used to refer to complete list of standard colors.
)" /* [vlistcolors] */);

  addCmd("vgenenvlut", VGenEnvLUT, /* [vgenenvlut] */ R"(
vgenenvlut [-size size = 128] [-nbsamples nbsamples = 1024]
Generates PBR environment look up table.
Saves it as C++ source file which is expected to be included in code.
The path where result will be located is 'Graphic3d_TextureRoot::TexturesFolder()'.
 -size size of one side of resulted square table
 -nbsamples number of samples used in Monte-Carlo integration
)" /* [vgenenvlut] */);
}
