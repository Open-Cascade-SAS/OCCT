// Created on: 2013-09-20
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Graphic3d_ShaderObject.hxx>

#include <Graphic3d_GraphicDriver.hxx>
#include <OSD_File.hxx>
#include <OSD_Protection.hxx>
#include <Standard_Atomic.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_ShaderObject,Standard_Transient)

namespace
{
  static volatile Standard_Integer THE_SHADER_OBJECT_COUNTER = 0;
}

// =======================================================================
// function : Graphic3d_ShaderObject
// purpose  : Creates a shader object from specified file
// =======================================================================
Graphic3d_ShaderObject::Graphic3d_ShaderObject (const Graphic3d_TypeOfShaderObject theType)
: myType (theType)
{
  myID = TCollection_AsciiString ("Graphic3d_ShaderObject_")
       + TCollection_AsciiString (Standard_Atomic_Increment (&THE_SHADER_OBJECT_COUNTER));
}

// =======================================================================
// function : CreatFromFile
// purpose  : Creates new shader object from specified file
// =======================================================================
Handle(Graphic3d_ShaderObject) Graphic3d_ShaderObject::CreateFromFile (const Graphic3d_TypeOfShaderObject theType,
                                                                       const TCollection_AsciiString&     thePath)
{
  Handle(Graphic3d_ShaderObject) aShader = new Graphic3d_ShaderObject (theType);
  aShader->myPath = thePath;

  OSD_File aFile (thePath);
  if (!aFile.Exists())
  {
    return NULL;
  }

  aFile.Open (OSD_ReadOnly, OSD_Protection());
  aFile.Read (aShader->mySource, (int)aFile.Size());
  aFile.Close();

  return aShader;
}

// =======================================================================
// function : CreatFromSource
// purpose  : Creates new shader object from specified source
// =======================================================================
Handle(Graphic3d_ShaderObject) Graphic3d_ShaderObject::CreateFromSource (const Graphic3d_TypeOfShaderObject theType,
                                                                         const TCollection_AsciiString&     theSource)
{
  Handle(Graphic3d_ShaderObject) aShader = new Graphic3d_ShaderObject (theType);
  aShader->mySource = theSource;
  return aShader;
}

// =======================================================================
// function : ~Graphic3d_ShaderObject
// purpose  : Releases resources of shader object
// =======================================================================
Graphic3d_ShaderObject::~Graphic3d_ShaderObject()
{
  //
}

// =======================================================================
// function : IsDone
// purpose  : Checks if the shader object is valid or not
// =======================================================================
Standard_Boolean Graphic3d_ShaderObject::IsDone() const
{
  return !mySource.IsEmpty();
}

// =======================================================================
// function : CreateFromSource
// purpose  :
// =======================================================================
Handle(Graphic3d_ShaderObject) Graphic3d_ShaderObject::CreateFromSource (TCollection_AsciiString& theSource,
                                                                         Graphic3d_TypeOfShaderObject theType,
                                                                         const ShaderVariableList& theUniforms,
                                                                         const ShaderVariableList& theStageInOuts,
                                                                         const TCollection_AsciiString& theInName,
                                                                         const TCollection_AsciiString& theOutName,
                                                                         Standard_Integer theNbGeomInputVerts)
{
  if (theSource.IsEmpty())
  {
    return Handle(Graphic3d_ShaderObject)();
  }

  TCollection_AsciiString aSrcUniforms, aSrcInOuts, aSrcInStructs, aSrcOutStructs;
  for (ShaderVariableList::Iterator anUniformIter (theUniforms); anUniformIter.More(); anUniformIter.Next())
  {
    const ShaderVariable& aVar = anUniformIter.Value();
    if ((aVar.Stages & theType) != 0)
    {
      aSrcUniforms += TCollection_AsciiString("\nuniform ") + aVar.Name + ";";
    }
  }
  for (ShaderVariableList::Iterator aVarListIter (theStageInOuts); aVarListIter.More(); aVarListIter.Next())
  {
    const ShaderVariable& aVar = aVarListIter.Value();
    Standard_Integer aStageLower = IntegerLast(), aStageUpper = IntegerFirst();
    for (Standard_Integer aStageIter = Graphic3d_TOS_VERTEX; aStageIter <= (Standard_Integer )Graphic3d_TOS_COMPUTE; aStageIter = aStageIter << 1)
    {
      if ((aVar.Stages & aStageIter) != 0)
      {
        aStageLower = Min (aStageLower, aStageIter);
        aStageUpper = Max (aStageUpper, aStageIter);
      }
    }
    if ((Standard_Integer )theType < aStageLower
     || (Standard_Integer )theType > aStageUpper)
    {
      continue;
    }

    const Standard_Boolean hasGeomStage = theNbGeomInputVerts > 0
                                       && aStageLower <  Graphic3d_TOS_GEOMETRY
                                       && aStageUpper >= Graphic3d_TOS_GEOMETRY;
    const Standard_Boolean isAllStagesVar = aStageLower == Graphic3d_TOS_VERTEX
                                         && aStageUpper == Graphic3d_TOS_FRAGMENT;
    if (hasGeomStage
    || !theInName.IsEmpty()
    || !theOutName.IsEmpty())
    {
      if (aSrcInStructs.IsEmpty()
       && aSrcOutStructs.IsEmpty()
       && isAllStagesVar)
      {
        if (theType == aStageLower)
        {
          aSrcOutStructs = "\nout VertexData\n{";
        }
        else if (theType == aStageUpper)
        {
          aSrcInStructs = "\nin VertexData\n{";
        }
        else // requires theInName/theOutName
        {
          aSrcInStructs  = "\nin  VertexData\n{";
          aSrcOutStructs = "\nout VertexData\n{";
        }
      }
    }

    if (isAllStagesVar
     && (!aSrcInStructs.IsEmpty()
      || !aSrcOutStructs.IsEmpty()))
    {
      if (!aSrcInStructs.IsEmpty())
      {
        aSrcInStructs  += TCollection_AsciiString("\n  ") + aVar.Name + ";";
      }
      if (!aSrcOutStructs.IsEmpty())
      {
        aSrcOutStructs += TCollection_AsciiString("\n  ") + aVar.Name + ";";
      }
    }
    else
    {
      if (theType == aStageLower)
      {
        aSrcInOuts += TCollection_AsciiString("\nTHE_SHADER_OUT ") + aVar.Name + ";";
      }
      else if (theType == aStageUpper)
      {
        aSrcInOuts += TCollection_AsciiString("\nTHE_SHADER_IN ") + aVar.Name + ";";
      }
    }
  }

  if (theType == Graphic3d_TOS_GEOMETRY)
  {
    aSrcUniforms.Prepend (TCollection_AsciiString()
                        + "\nlayout (triangles) in;"
                          "\nlayout (triangle_strip, max_vertices = " + theNbGeomInputVerts + ") out;");
  }
  if (!aSrcInStructs.IsEmpty()
   && theType == Graphic3d_TOS_GEOMETRY)
  {
    aSrcInStructs  += TCollection_AsciiString ("\n} ") + theInName  + "[" + theNbGeomInputVerts + "];";
  }
  else if (!aSrcInStructs.IsEmpty())
  {
    aSrcInStructs += "\n}";
    if (!theInName.IsEmpty())
    {
      aSrcInStructs += " ";
      aSrcInStructs += theInName;
    }
    aSrcInStructs += ";";
  }
  if (!aSrcOutStructs.IsEmpty())
  {
    aSrcOutStructs += "\n}";
    if (!theOutName.IsEmpty())
    {
      aSrcOutStructs += " ";
      aSrcOutStructs += theOutName;
    }
    aSrcOutStructs += ";";
  }

  theSource.Prepend (aSrcUniforms + aSrcInStructs + aSrcOutStructs + aSrcInOuts);
  return Graphic3d_ShaderObject::CreateFromSource (theType, theSource);
}
