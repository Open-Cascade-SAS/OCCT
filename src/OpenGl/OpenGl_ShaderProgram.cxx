// Created on: 2013-09-19
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <OSD_File.hxx>
#include <OSD_Protection.hxx>

#include <Standard_Assert.hxx>
#include <Standard_Atomic.hxx>
#include <TCollection_ExtendedString.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_ShaderProgram.hxx>
#include <OpenGl_ShaderManager.hxx>

IMPLEMENT_STANDARD_HANDLE (OpenGl_ShaderProgram, OpenGl_Resource)
IMPLEMENT_STANDARD_RTTIEXT(OpenGl_ShaderProgram, OpenGl_Resource)

OpenGl_VariableSetterSelector OpenGl_ShaderProgram::mySetterSelector = OpenGl_VariableSetterSelector();

// Declare OCCT-specific OpenGL/GLSL shader variables
Standard_CString OpenGl_ShaderProgram::PredefinedKeywords[] =
{
  /* OpenGl_OCC_MODEL_WORLD_MATRIX */ "occModelWorldMatrix",
  /* OpenGl_OCC_WORLD_VIEW_MATRIX */ "occWorldViewMatrix",
  /* OpenGl_OCC_PROJECTION_MATRIX */ "occProjectionMatrix",
  /* OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE */ "occModelWorldMatrixInverse",
  /* OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE */ "occWorldViewMatrixInverse",
  /* OpenGl_OCC_PROJECTION_MATRIX_INVERSE */ "occProjectionMatrixInverse",
  /* OpenGl_OCC_MODEL_WORLD_MATRIX_TRANSPOSE */ "occModelWorldMatrixTranspose",
  /* OpenGl_OCC_WORLD_VIEW_MATRIX_TRANSPOSE */ "occWorldViewMatrixTranspose",
  /* OpenGl_OCC_PROJECTION_MATRIX_TRANSPOSE */ "occProjectionMatrixTranspose",
  /* OpenGl_OCC_MODEL_WORLD_MATRIX_INVERSE_TRANSPOSE */ "occModelWorldMatrixInverseTranspose",
  /* OpenGl_OCC_WORLD_VIEW_MATRIX_INVERSE_TRANSPOSE */ "occWorldViewMatrixInverseTranspose",
  /* OpenGl_OCC_PROJECTION_MATRIX_INVERSE_TRANSPOSE */ "occProjectionMatrixInverseTranspose",

  /* OpenGl_OCC_CLIP_PLANE_0_EQUATION */ "occClipPlanes[0].Equation",
  /* OpenGl_OCC_CLIP_PLANE_1_EQUATION */ "occClipPlanes[1].Equation",
  /* OpenGl_OCC_CLIP_PLANE_2_EQUATION */ "occClipPlanes[2].Equation",
  /* OpenGl_OCC_CLIP_PLANE_3_EQUATION */ "occClipPlanes[3].Equation",
  /* OpenGl_OCC_CLIP_PLANE_4_EQUATION */ "occClipPlanes[4].Equation",
  /* OpenGl_OCC_CLIP_PLANE_5_EQUATION */ "occClipPlanes[5].Equation",
  /* OpenGl_OCC_CLIP_PLANE_6_EQUATION */ "occClipPlanes[6].Equation",
  /* OpenGl_OCC_CLIP_PLANE_7_EQUATION */ "occClipPlanes[7].Equation",

  /* OpenGl_OCC_CLIP_PLANE_0_SPACE */ "occClipPlanes[0].Space",
  /* OpenGl_OCC_CLIP_PLANE_1_SPACE */ "occClipPlanes[1].Space",
  /* OpenGl_OCC_CLIP_PLANE_2_SPACE */ "occClipPlanes[2].Space",
  /* OpenGl_OCC_CLIP_PLANE_3_SPACE */ "occClipPlanes[3].Space",
  /* OpenGl_OCC_CLIP_PLANE_4_SPACE */ "occClipPlanes[4].Space",
  /* OpenGl_OCC_CLIP_PLANE_5_SPACE */ "occClipPlanes[5].Space",
  /* OpenGl_OCC_CLIP_PLANE_6_SPACE */ "occClipPlanes[6].Space",
  /* OpenGl_OCC_CLIP_PLANE_7_SPACE */ "occClipPlanes[7].Space",

  /* OpenGl_OCC_LIGHT_SOURCE_COUNT */ "occLightSourcesCount",

  /* OpenGl_OCC_LIGHT_SOURCE_0_TYPE */ "occLightSources[0].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_1_TYPE */ "occLightSources[1].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_2_TYPE */ "occLightSources[2].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_3_TYPE */ "occLightSources[3].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_4_TYPE */ "occLightSources[4].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_5_TYPE */ "occLightSources[5].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_6_TYPE */ "occLightSources[6].Type",
  /* OpenGl_OCC_LIGHT_SOURCE_7_TYPE */ "occLightSources[7].Type",

  /* OpenGl_OCC_LIGHT_SOURCE_0_HEAD */ "occLightSources[0].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_1_HEAD */ "occLightSources[1].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_2_HEAD */ "occLightSources[2].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_3_HEAD */ "occLightSources[3].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_4_HEAD */ "occLightSources[4].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_5_HEAD */ "occLightSources[5].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_6_HEAD */ "occLightSources[6].Head",
  /* OpenGl_OCC_LIGHT_SOURCE_7_HEAD */ "occLightSources[7].Head",

  /* OpenGl_OCC_LIGHT_SOURCE_0_AMBIENT */ "occLightSources[0].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_1_AMBIENT */ "occLightSources[1].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_2_AMBIENT */ "occLightSources[2].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_3_AMBIENT */ "occLightSources[3].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_4_AMBIENT */ "occLightSources[4].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_5_AMBIENT */ "occLightSources[5].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_6_AMBIENT */ "occLightSources[6].Ambient",
  /* OpenGl_OCC_LIGHT_SOURCE_7_AMBIENT */ "occLightSources[7].Ambient",

  /* OpenGl_OCC_LIGHT_SOURCE_0_DIFFUSE */ "occLightSources[0].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_1_DIFFUSE */ "occLightSources[1].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_2_DIFFUSE */ "occLightSources[2].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_3_DIFFUSE */ "occLightSources[3].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_4_DIFFUSE */ "occLightSources[4].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_5_DIFFUSE */ "occLightSources[5].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_6_DIFFUSE */ "occLightSources[6].Diffuse",
  /* OpenGl_OCC_LIGHT_SOURCE_7_DIFFUSE */ "occLightSources[7].Diffuse",

  /* OpenGl_OCC_LIGHT_SOURCE_0_SPECULAR */ "occLightSources[0].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_1_SPECULAR */ "occLightSources[1].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_2_SPECULAR */ "occLightSources[2].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_3_SPECULAR */ "occLightSources[3].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_4_SPECULAR */ "occLightSources[4].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_5_SPECULAR */ "occLightSources[5].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_6_SPECULAR */ "occLightSources[6].Specular",
  /* OpenGl_OCC_LIGHT_SOURCE_7_SPECULAR */ "occLightSources[7].Specular",

  /* OpenGl_OCC_LIGHT_SOURCE_0_POSITION */ "occLightSources[0].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_1_POSITION */ "occLightSources[1].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_2_POSITION */ "occLightSources[2].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_3_POSITION */ "occLightSources[3].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_4_POSITION */ "occLightSources[4].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_5_POSITION */ "occLightSources[5].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_6_POSITION */ "occLightSources[6].Position",
  /* OpenGl_OCC_LIGHT_SOURCE_7_POSITION */ "occLightSources[7].Position",

  /* OpenGl_OCC_LIGHT_SOURCE_0_SPOT_CUTOFF */ "occLightSources[0].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_1_SPOT_CUTOFF */ "occLightSources[1].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_2_SPOT_CUTOFF */ "occLightSources[2].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_3_SPOT_CUTOFF */ "occLightSources[3].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_4_SPOT_CUTOFF */ "occLightSources[4].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_5_SPOT_CUTOFF */ "occLightSources[5].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_6_SPOT_CUTOFF */ "occLightSources[6].SpotCutoff",
  /* OpenGl_OCC_LIGHT_SOURCE_7_SPOT_CUTOFF */ "occLightSources[7].SpotCutoff",

  /* OpenGl_OCC_LIGHT_SOURCE_0_SPOT_EXPONENT */ "occLightSources[0].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_1_SPOT_EXPONENT */ "occLightSources[1].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_2_SPOT_EXPONENT */ "occLightSources[2].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_3_SPOT_EXPONENT */ "occLightSources[3].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_4_SPOT_EXPONENT */ "occLightSources[4].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_5_SPOT_EXPONENT */ "occLightSources[5].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_6_SPOT_EXPONENT */ "occLightSources[6].SpotExponent",
  /* OpenGl_OCC_LIGHT_SOURCE_7_SPOT_EXPONENT */ "occLightSources[7].SpotExponent",

  /* OpenGl_OCC_LIGHT_SOURCE_0_SPOT_DIRECTION */ "occLightSources[0].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_1_SPOT_DIRECTION */ "occLightSources[1].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_2_SPOT_DIRECTION */ "occLightSources[2].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_3_SPOT_DIRECTION */ "occLightSources[3].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_4_SPOT_DIRECTION */ "occLightSources[4].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_5_SPOT_DIRECTION */ "occLightSources[5].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_6_SPOT_DIRECTION */ "occLightSources[6].SpotDirection",
  /* OpenGl_OCC_LIGHT_SOURCE_7_SPOT_DIRECTION */ "occLightSources[7].SpotDirection",

  /* OpenGl_OCC_LIGHT_SOURCE_0_CONST_ATTENUATION */ "occLightSources[0].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_1_CONST_ATTENUATION */ "occLightSources[1].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_2_CONST_ATTENUATION */ "occLightSources[2].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_3_CONST_ATTENUATION */ "occLightSources[3].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_4_CONST_ATTENUATION */ "occLightSources[4].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_5_CONST_ATTENUATION */ "occLightSources[5].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_6_CONST_ATTENUATION */ "occLightSources[6].ConstAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_7_CONST_ATTENUATION */ "occLightSources[7].ConstAttenuation",

  /* OpenGl_OCC_LIGHT_SOURCE_0_LINEAR_ATTENUATION */ "occLightSources[0].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_1_LINEAR_ATTENUATION */ "occLightSources[1].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_2_LINEAR_ATTENUATION */ "occLightSources[2].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_3_LINEAR_ATTENUATION */ "occLightSources[3].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_4_LINEAR_ATTENUATION */ "occLightSources[4].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_5_LINEAR_ATTENUATION */ "occLightSources[5].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_6_LINEAR_ATTENUATION */ "occLightSources[6].LinearAttenuation",
  /* OpenGl_OCC_LIGHT_SOURCE_7_LINEAR_ATTENUATION */ "occLightSources[7].LinearAttenuation",

  /* OpenGl_OCCT_ACTIVE_SAMPLER */ "occActiveSampler",
  /* OpenGl_OCCT_TEXTURE_ENABLE */ "occTextureEnable",
  /* OpenGl_OCCT_DISTINGUISH_MODE */ "occDistinguishingMode",
  /* OpenGl_OCCT_FRONT_MATERIAL_AMBIENT */ "occFrontMaterial.Ambient",
  /* OpenGl_OCCT_BACK_MATERIAL_AMBIENT */ "occBackMaterial.Ambient",
  /* OpenGl_OCCT_FRONT_MATERIAL_DIFFUSE */ "occFrontMaterial.Diffuse",
  /* OpenGl_OCCT_BACK_MATERIAL_DIFFUSE */ "occBackMaterial.Diffuse",
  /* OpenGl_OCCT_FRONT_MATERIAL_SPECULAR */ "occFrontMaterial.Specular",
  /* OpenGl_OCCT_BACK_MATERIAL_SPECULAR */ "occBackMaterial.Specular",
  /* OpenGl_OCCT_FRONT_MATERIAL_EMISSION */ "occFrontMaterial.Emission",
  /* OpenGl_OCCT_BACK_MATERIAL_EMISSION */ "occBackMaterial.Emission",
  /* OpenGl_OCCT_FRONT_MATERIAL_SHININESS */ "occFrontMaterial.Shininess",
  /* OpenGl_OCCT_BACK_MATERIAL_SHININESS */ "occBackMaterial.Shininess",
  /* OpenGl_OCCT_FRONT_MATERIAL_TRANSPARENCY */ "occFrontMaterial.Transparency",
  /* OpenGl_OCCT_BACK_MATERIAL_TRANSPARENCY */ "occBackMaterial.Transparency"

};

// =======================================================================
// function : OpenGl_VariableSetterSelector
// purpose  : Creates new variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::OpenGl_VariableSetterSelector()
{
  // Note: Add new variable setters here
  mySetterList = OpenGl_HashMapInitializer::CreateListOf<size_t, OpenGl_SetterInterface*>
    (Graphic3d_UniformValueTypeID<int>::ID,          new OpenGl_VariableSetter<int>())
    (Graphic3d_UniformValueTypeID<float>::ID,        new OpenGl_VariableSetter<float>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec2>::ID,  new OpenGl_VariableSetter<OpenGl_Vec2>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec3>::ID,  new OpenGl_VariableSetter<OpenGl_Vec3>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec4>::ID,  new OpenGl_VariableSetter<OpenGl_Vec4>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec2i>::ID, new OpenGl_VariableSetter<OpenGl_Vec2i>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec3i>::ID, new OpenGl_VariableSetter<OpenGl_Vec3i>())
    (Graphic3d_UniformValueTypeID<OpenGl_Vec4i>::ID, new OpenGl_VariableSetter<OpenGl_Vec4i>());
}

// =======================================================================
// function : ~OpenGl_VariableSetterSelector
// purpose  : Releases memory resources of variable setter selector
// =======================================================================
OpenGl_VariableSetterSelector::~OpenGl_VariableSetterSelector()
{
  for (OpenGl_SetterList::Iterator anIt (mySetterList); anIt.More(); anIt.Next())
  {
    delete anIt.Value();
  }

  mySetterList.Clear();
}

// =======================================================================
// function : Set
// purpose  : Sets generic variable to specified shader program
// =======================================================================
void OpenGl_VariableSetterSelector::Set (const Handle(OpenGl_Context)&           theCtx,
                                         const Handle(Graphic3d_ShaderVariable)& theVariable,
                                         OpenGl_ShaderProgram*                   theProgram) const
{
  Standard_ASSERT_RETURN (mySetterList.IsBound (theVariable->Value()->TypeID()),
    "The type of user-defined uniform variable is not supported...", );

  mySetterList.Find (theVariable->Value()->TypeID())->Set (theCtx, theVariable, theProgram);
}

// =======================================================================
// function : OpenGl_ShaderProgram
// purpose  : Creates uninitialized shader program
// =======================================================================
OpenGl_ShaderProgram::OpenGl_ShaderProgram (const Handle(Graphic3d_ShaderProgram)& theProxy)
: myProgramID (NO_PROGRAM),
  myProxy     (theProxy)
{
  memset (myCurrentState, 0, sizeof (myCurrentState));
  for (GLint aVar = 0; aVar < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES; ++aVar)
  {
    myStateLocations[aVar] = INVALID_LOCATION;
  }
}

// =======================================================================
// function : Initialize
// purpose  : Initializes program object with the list of shader objects
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Initialize (const Handle(OpenGl_Context)&     theCtx,
                                                   const Graphic3d_ShaderObjectList& theShaders)
{
  if (theCtx.IsNull() || !Create (theCtx))
  {
    return Standard_False;
  }

  GLchar *aShaderDir = getenv ("CSF_ShadersDirectory");
  if (aShaderDir == NULL)
  {
    TCollection_ExtendedString aMsg = "Error! Failed to get OCCT shaders directory";

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         aMsg);

    return Standard_False;
  }

  OSD_File aDeclFile (TCollection_AsciiString (aShaderDir) + "/Declarations.glsl");
  if (!aDeclFile.Exists())
  {
    TCollection_ExtendedString aMsg = "Error! Failed to load OCCT shader declarations file";

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         aMsg);

    return Standard_False;
  }

  TCollection_AsciiString aDeclarations;

  aDeclFile.Open (OSD_ReadOnly, OSD_Protection());
  aDeclFile.Read (aDeclarations, aDeclFile.Size());
  aDeclFile.Close();

  for (Graphic3d_ShaderObjectList::Iterator anIter (theShaders);
       anIter.More(); anIter.Next())
  {
    if (!anIter.Value()->IsDone())
    {
      TCollection_ExtendedString aMsg = "Error! Failed to get shader source";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                           GL_DEBUG_TYPE_ERROR_ARB,
                           0,
                           GL_DEBUG_SEVERITY_HIGH_ARB,
                           aMsg);

      return Standard_False;
    }

    Handle(OpenGl_ShaderObject) aShader;

    // Note: Add support of other shader types here
    switch (anIter.Value()->Type())
    {
      case Graphic3d_TOS_VERTEX:
        aShader = new OpenGl_ShaderObject (GL_VERTEX_SHADER);
        break;
      case Graphic3d_TOS_FRAGMENT:
        aShader = new OpenGl_ShaderObject (GL_FRAGMENT_SHADER);
        break;
    }

    // Is unsupported shader type?
    if (aShader.IsNull())
    {
      TCollection_ExtendedString aMsg = "Error! Unsupported shader type";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                           GL_DEBUG_TYPE_ERROR_ARB,
                           0,
                           GL_DEBUG_SEVERITY_HIGH_ARB,
                           aMsg);

      return Standard_False;
    }

    if (!aShader->Create (theCtx))
    {
      return Standard_False;
    }

    TCollection_AsciiString aSource = aDeclarations + anIter.Value()->Source();
    
    if (anIter.Value()->Type() == Graphic3d_TOS_VERTEX)
    {
      aSource = TCollection_AsciiString ("#define VERTEX_SHADER\n") + aSource;
    }

    if (!aShader->LoadSource (theCtx, aSource))
    {
      TCollection_ExtendedString aMsg = "Error! Failed to set shader source";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                           GL_DEBUG_TYPE_ERROR_ARB,
                           0,
                           GL_DEBUG_SEVERITY_HIGH_ARB,
                           aMsg);

      return Standard_False;
    }

    if (!aShader->Compile (theCtx))
    {
      TCollection_ExtendedString aMsg = "Error! Failed to compile shader object";

      theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                           GL_DEBUG_TYPE_ERROR_ARB,
                           0,
                           GL_DEBUG_SEVERITY_HIGH_ARB,
                           aMsg);

      if (theCtx->caps->contextDebug)
      {
        TCollection_AsciiString aLog;
        aShader->FetchInfoLog (theCtx, aLog);
        if (!aLog.IsEmpty())
        {
          std::cout << aLog.ToCString() << std::endl << std::flush;
        }
        else
        {
          std::cout << "Information log is empty" << std::endl;
        }
      }

      return Standard_False;
    }

    if (!AttachShader (theCtx, aShader))
    {
      return Standard_False;
    }
  }

  if (!Link (theCtx))
  {
    TCollection_ExtendedString aMsg = "Error! Failed to link program object";

    theCtx->PushMessage (GL_DEBUG_SOURCE_APPLICATION_ARB,
                         GL_DEBUG_TYPE_ERROR_ARB,
                         0,
                         GL_DEBUG_SEVERITY_HIGH_ARB,
                         aMsg);

    if (theCtx->caps->contextDebug)
    {
      TCollection_AsciiString aLog;
      FetchInfoLog (theCtx, aLog);
      if (!aLog.IsEmpty())
      {
        std::cout << aLog.ToCString() << std::endl;
      }
      else
      {
        std::cout << "Information log is empty" << std::endl;
      }
    }

    return Standard_False;
  }

  return Standard_True;
}

// =======================================================================
// function : ~OpenGl_ShaderProgram
// purpose  : Releases resources of shader program
// =======================================================================
OpenGl_ShaderProgram::~OpenGl_ShaderProgram()
{
  Release (NULL);
}

// =======================================================================
// function : AttachShader
// purpose  : Attaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::AttachShader (const Handle(OpenGl_Context)&      theCtx,
                                                     const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM || theShader.IsNull())
  {
    return Standard_False;
  }

  for (OpenGl_ShaderList::Iterator anIter (myShaderObjects); anIter.More(); anIter.Next())
  {
    if (theShader == anIter.Value())
    {
      return Standard_False;
    }
  }

  myShaderObjects.Append (theShader);
  theCtx->core20->glAttachShader (myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : DetachShader
// purpose  : Detaches shader object to the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::DetachShader (const Handle(OpenGl_Context)&      theCtx,
                                                     const Handle(OpenGl_ShaderObject)& theShader)
{
  if (myProgramID == NO_PROGRAM
   || theShader.IsNull())
  {
    return Standard_False;
  }

  OpenGl_ShaderList::Iterator anIter (myShaderObjects);
  while (anIter.More())
  {
    if (theShader == anIter.Value())
    {
      myShaderObjects.Remove (anIter);
      break;
    }

    anIter.Next();
  }

  if (!anIter.More())
  {
    return Standard_False;
  }

  theCtx->core20->glDetachShader (myProgramID, theShader->myShaderID);
  return Standard_True;
}

// =======================================================================
// function : Link
// purpose  : Links the program object
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Link (const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  theCtx->core20->glLinkProgram (myProgramID);

  GLint aStatus = GL_FALSE;
  theCtx->core20->glGetProgramiv (myProgramID, GL_LINK_STATUS, &aStatus);

  for (GLint aVar = 0; aVar < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES; ++aVar)
  {
    myStateLocations[aVar] = GetUniformLocation (theCtx, PredefinedKeywords[aVar]);
  }

  return aStatus != GL_FALSE;
}

// =======================================================================
// function : FetchInfoLog
// purpose  : Fetches information log of the last link operation
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::FetchInfoLog (const Handle(OpenGl_Context)& theCtx,
                                                     TCollection_AsciiString&      theOutput)
{
  if (myProgramID == NO_PROGRAM)
  {
    return Standard_False;
  }

  GLint aLength = 0;
  theCtx->core20->glGetProgramiv (myProgramID, GL_INFO_LOG_LENGTH, &aLength);
  if (aLength > 0)
  {
    GLchar* aLog = (GLchar*) alloca (aLength);
    memset (aLog, 0, aLength);
    theCtx->core20->glGetProgramInfoLog (myProgramID, aLength, NULL, aLog);
    theOutput = aLog;
  }
  return Standard_True;
}

// =======================================================================
// function : Bind
// purpose  : Sets the program object as part of current rendering state
// =======================================================================
void OpenGl_ShaderProgram::Bind (const Handle(OpenGl_Context)& theCtx) const
{
  if (myProgramID == NO_PROGRAM)
  {
    return;
  }

  theCtx->core20->glUseProgram (myProgramID);
  theCtx->ShaderManager()->myIsPP = Standard_True;
}

// =======================================================================
// function : ApplyVariables
// purpose  : Fetches uniform variables from proxy shader program
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::ApplyVariables(const Handle(OpenGl_Context)& theCtx)
{
  if (myProxy.IsNull() || myProxy->Variables().IsEmpty())
  {
    return Standard_False;
  }

  for (Graphic3d_ShaderVariableList::Iterator anIter (myProxy->Variables()); anIter.More(); anIter.Next())
  {
    mySetterSelector.Set (theCtx, anIter.Value(), this);
  }

  myProxy->ClearVariables();
  return Standard_True;
}

// =======================================================================
// function : BindWithVariables
// purpose  : Binds the program object and applies variables
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::BindWithVariables (const Handle(OpenGl_Context)& theCtx)
{
  Bind (theCtx);
  return ApplyVariables (theCtx);
}

// =======================================================================
// function : Unbind
// purpose  : Reverts to fixed-function graphics pipeline (FFP)
// =======================================================================
void OpenGl_ShaderProgram::Unbind (const Handle(OpenGl_Context)& theCtx)
{
  if (theCtx->ShaderManager()->myIsPP)
  {
    theCtx->core20->glUseProgram (NO_PROGRAM);
    theCtx->ShaderManager()->myIsPP = Standard_False;
  }
}

// =======================================================================
// function : ActiveState
// purpose  : Returns index of last modification for specified state type
// =======================================================================
Standard_Size OpenGl_ShaderProgram::ActiveState (const OpenGl_UniformStateType theType) const
{
  if (theType < MaxStateTypes)
  {
    return myCurrentState[theType];
  }
  return 0;
}

// =======================================================================
// function : UpdateState
// purpose  : Updates index of last modification for specified state type
// =======================================================================
void OpenGl_ShaderProgram::UpdateState (const OpenGl_UniformStateType theType,
                                        const Standard_Size           theIndex)
{
  if (theType < MaxStateTypes)
  {
    myCurrentState[theType] = theIndex;
  }
}

// =======================================================================
// function : GetUniformLocation
// purpose  : Returns location (index) of the specific uniform variable
// =======================================================================
GLint OpenGl_ShaderProgram::GetUniformLocation (const Handle(OpenGl_Context)& theCtx,
                                                const GLchar*                 theName) const
{
  return myProgramID != NO_PROGRAM
       ? theCtx->core20->glGetUniformLocation (myProgramID, theName)
       : INVALID_LOCATION;
}

// =======================================================================
// function : GetAttributeLocation
// purpose  : Returns location (index) of the generic vertex attribute
// =======================================================================
GLint OpenGl_ShaderProgram::GetAttributeLocation (const Handle(OpenGl_Context)& theCtx,
                                                  const GLchar*                 theName) const
{
  return myProgramID != NO_PROGRAM
       ? theCtx->core20->glGetAttribLocation (myProgramID, theName)
       : INVALID_LOCATION;
}

// =======================================================================
// function : GetStateLocation
// purpose  : Returns location of the OCCT state uniform variable
// =======================================================================
GLint OpenGl_ShaderProgram::GetStateLocation (const GLuint theVariable) const
{
  if (theVariable < OpenGl_OCCT_NUMBER_OF_STATE_VARIABLES)
  {
    return myStateLocations[theVariable];
  }
  return INVALID_LOCATION;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   OpenGl_Vec4i&                 theValue) const
{
  return GetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glGetUniformiv (myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   OpenGl_Vec4&                  theValue) const
{
  return GetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetUniform
// purpose  : Returns the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glGetUniformfv (myProgramID, theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the integer vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     OpenGl_Vec4i&                 theValue) const
{
  return GetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the integer vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     OpenGl_Vec4i&                 theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glGetVertexAttribiv (theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the floating-point vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     const GLchar*                 theName,
                                                     OpenGl_Vec4&                  theValue) const
{
  return GetAttribute (theCtx, GetAttributeLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : GetAttribute
// purpose  : Returns the floating-point vertex attribute
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::GetAttribute (const Handle(OpenGl_Context)& theCtx,
                                                     GLint                         theIndex,
                                                     OpenGl_Vec4&                  theValue) const
{
  if (myProgramID == NO_PROGRAM || theIndex == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glGetVertexAttribfv (theIndex, GL_CURRENT_VERTEX_ATTRIB, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   GLint                         theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLint                         theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform1i (theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   GLfloat                       theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   GLfloat                       theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform1f (theLocation, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec2i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec2i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform2iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec3i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec3i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform3iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec4i&           theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the integer uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec4i&           theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform4iv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec2&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 2D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec2&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform2fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec3&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 3D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec3&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform3fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Vec4&            theValue)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4D vector
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Vec4&            theValue)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform4fv (theLocation, 1, theValue);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const OpenGl_Matrix&          theValue,
                                                   GLboolean                     theTranspose)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue, theTranspose);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const OpenGl_Matrix&          theValue,
                                                   GLboolean                     theTranspose)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniformMatrix4fv (theLocation, 1, theTranspose, *theValue.mat);
  return Standard_True;
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const Tmatrix3&               theValue,
                                                   GLboolean                     theTranspose)
{
  return SetUniform (theCtx, GetUniformLocation (theCtx, theName), theValue, theTranspose);
}

// =======================================================================
// function : SetUniform
// purpose  : Specifies the value of the floating-point uniform 4x4 matrix
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetUniform (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const Tmatrix3&               theValue,
                                                   GLboolean                     theTranspose)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniformMatrix4fv (theLocation, 1, theTranspose, *theValue);
  return Standard_True;
}

// =======================================================================
// function : SetSampler
// purpose  : Specifies the value of the sampler uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetSampler (const Handle(OpenGl_Context)& theCtx,
                                                   const GLchar*                 theName,
                                                   const GLenum                  theTextureUnit)
{
  return SetSampler (theCtx, GetUniformLocation (theCtx, theName), theTextureUnit);
}

// =======================================================================
// function : SetSampler
// purpose  : Specifies the value of the sampler uniform variable
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::SetSampler (const Handle(OpenGl_Context)& theCtx,
                                                   GLint                         theLocation,
                                                   const GLenum                  theTextureUnit)
{
  if (myProgramID == NO_PROGRAM || theLocation == INVALID_LOCATION)
  {
    return Standard_False;
  }

  theCtx->core20->glUniform1i (theLocation, theTextureUnit);
  return Standard_True;
}

// =======================================================================
// function : Create
// purpose  : Creates new empty shader program of specified type
// =======================================================================
Standard_Boolean OpenGl_ShaderProgram::Create (const Handle(OpenGl_Context)& theCtx)
{
  if (myProgramID == NO_PROGRAM
   && theCtx->core20 != NULL)
  {
    myProgramID = theCtx->core20->glCreateProgram();
  }

  return myProgramID != NO_PROGRAM;
}

// =======================================================================
// function : Release
// purpose  : Destroys shader program
// =======================================================================
void OpenGl_ShaderProgram::Release (const OpenGl_Context* theCtx)
{
  if (myProgramID == NO_PROGRAM)
  {
    return;
  }

  Standard_ASSERT_RETURN (theCtx != NULL,
    "OpenGl_ShaderProgram destroyed without GL context! Possible GPU memory leakage...",);

  for (OpenGl_ShaderList::Iterator anIter (myShaderObjects); anIter.More(); anIter.Next())
  {
    anIter.ChangeValue()->Release (theCtx);
    anIter.ChangeValue().Nullify();
  }

  if (theCtx->core20 != NULL)
  {
    theCtx->core20->glDeleteProgram (myProgramID);
  }

  myProgramID = NO_PROGRAM;
}
