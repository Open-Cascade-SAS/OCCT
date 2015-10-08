// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Graphic3d_CStructure.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_GroupDefinitionError.hxx>
#include <Graphic3d_ShaderProgram.hxx>
#include <Graphic3d_Structure.hxx>
#include "Graphic3d_Structure.pxx"
#include <Graphic3d_StructureManager.hxx>
#include <Graphic3d_TextureMap.hxx>
#include <Graphic3d_TransModeFlags.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <NCollection_String.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#define MyContainsFacet myCBitFields.bool2

// =======================================================================
// function : Graphic3d_Group
// purpose  :
// =======================================================================
Graphic3d_Group::Graphic3d_Group (const Handle(Graphic3d_Structure)& theStruct)
: myIsClosed (Standard_False)
{
  // A small commentary on the usage of This!
  //
  // Graphic3d_Group is created in a structure. Graphic3d_Structure is a
  // manager of Graphic3d_Group. In the constructor of Graphic3d_Group
  // a method Add of Graphic3d_Structure is called. It allows adding
  // the instance of Graphic3d_Group in its manager. So there are 2 references
  // to <me> and everything works well.
  //
  // This () is the instance of the class, the current group
  //Handle(Graphic3d_Group) me  = Handle(Graphic3d_Group)::DownCast (This());

  myStructure = theStruct.operator->();

  MyContainsFacet = Standard_False,

  ContextLine.IsDef     = 0;
  ContextText.IsDef     = 0;
  ContextMarker.IsDef   = 0;
  ContextFillArea.IsDef = 0;

  ContextLine.IsSet     = 0;
  ContextText.IsSet     = 0;
  ContextMarker.IsSet   = 0;
  ContextFillArea.IsSet = 0;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Graphic3d_Group::Clear (Standard_Boolean theUpdateStructureMgr)
{
  if (IsDeleted())
  {
    return;
  }

  ContextLine.IsSet     = 0,
  ContextText.IsSet     = 0,
  ContextMarker.IsSet   = 0,
  ContextFillArea.IsSet = 0;

  ContextLine.IsDef     = 0,
  ContextText.IsDef     = 0,
  ContextMarker.IsDef   = 0,
  ContextFillArea.IsDef = 0;

  myBounds.Clear();

  if (MyContainsFacet)
  {
    myStructure->GroupsWithFacet (-1);
    MyContainsFacet = Standard_False;
  }

  // clear method could be used on Graphic3d_Structure destruction,
  // and its structure manager could be already destroyed, in that
  // case we don't need to update it;
  if (theUpdateStructureMgr)
  {
    Update();
  }
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Graphic3d_Group::Destroy()
{
  // tell graphics driver to clear internal resources of the group
  Clear (Standard_False);
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void Graphic3d_Group::Remove()
{
  if (IsDeleted())
  {
    return;
  }

  if (MyContainsFacet)
  {
    myStructure->GroupsWithFacet (-1);
    MyContainsFacet = Standard_False;
  }
  myStructure->Remove (this);

  Update();

  myBounds.Clear();
}

// =======================================================================
// function : IsDeleted
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsDeleted() const
{
  return myStructure == NULL
      || myStructure->IsDeleted();
}

// =======================================================================
// function : ContainsFacet
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::ContainsFacet() const
{
  return MyContainsFacet;
}

// =======================================================================
// function : IsEmpty
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsEmpty() const
{
  if (IsDeleted())
  {
    return Standard_True;
  }

  return !myStructure->IsInfinite()
      && !myBounds.IsValid();
}

// =======================================================================
// function : SetMinMaxValues
// purpose  :
// =======================================================================
void Graphic3d_Group::SetMinMaxValues (const Standard_Real theXMin, const Standard_Real theYMin, const Standard_Real theZMin,
                                       const Standard_Real theXMax, const Standard_Real theYMax, const Standard_Real theZMax)
{
  myBounds = Graphic3d_BndBox4f (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theXMin),
                                                 static_cast<Standard_ShortReal> (theYMin),
                                                 static_cast<Standard_ShortReal> (theZMin),
                                                 1.0f),
                                 Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theXMax),
                                                 static_cast<Standard_ShortReal> (theYMax),
                                                 static_cast<Standard_ShortReal> (theZMax),
                                                 1.0f));
}

// =======================================================================
// function : MinMaxValues
// purpose  :
// =======================================================================
void Graphic3d_Group::MinMaxValues (Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                    Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax) const
{
  MinMaxCoord (theXMin, theYMin, theZMin,
               theXMax, theYMax, theZMax);
}

// =======================================================================
// function : Structure
// purpose  :
// =======================================================================
Handle(Graphic3d_Structure) Graphic3d_Group::Structure() const
{
  return myStructure;
}

// =======================================================================
// function : MinMaxCoord
// purpose  :
// =======================================================================
void Graphic3d_Group::MinMaxCoord (Standard_Real& theXMin, Standard_Real& theYMin, Standard_Real& theZMin,
                                   Standard_Real& theXMax, Standard_Real& theYMax, Standard_Real& theZMax) const
{
  if (IsEmpty())
  {
    // Empty Group
    theXMin = theYMin = theZMin = ShortRealFirst();
    theXMax = theYMax = theZMax = ShortRealLast();
  }
  else if (myBounds.IsValid())
  {
    const Graphic3d_Vec4& aMinPt = myBounds.CornerMin();
    const Graphic3d_Vec4& aMaxPt = myBounds.CornerMax();
    theXMin = Standard_Real (aMinPt.x());
    theYMin = Standard_Real (aMinPt.y());
    theZMin = Standard_Real (aMinPt.z());
    theXMax = Standard_Real (aMaxPt.x());
    theYMax = Standard_Real (aMaxPt.y());
    theZMax = Standard_Real (aMaxPt.z());
  }
  else
  {
    // for consistency with old API
    theXMin = theYMin = theZMin = ShortRealLast();
    theXMax = theYMax = theZMax = ShortRealFirst();
  }
}

// =======================================================================
// function : Update
// purpose  :
// =======================================================================
void Graphic3d_Group::Update() const
{
  if (IsDeleted())
  {
    return;
  }

  myStructure->StructureManager()->Update (myStructure->StructureManager()->UpdateMode());
}

// =======================================================================
// function : SetGroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& theAspLine)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real aWidth;
  Quantity_Color aColor;
  Aspect_TypeOfLine aLType;
  theAspLine->Values (aColor, aLType, aWidth);

  ContextLine.IsDef         = 1;
  ContextLine.Color.r       = float (aColor.Red());
  ContextLine.Color.g       = float (aColor.Green());
  ContextLine.Color.b       = float (aColor.Blue());
  ContextLine.LineType      = aLType;
  ContextLine.Width         = float (aWidth);
  ContextLine.ShaderProgram = theAspLine->ShaderProgram();

  UpdateAspectLine (Standard_True);

  ContextLine.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetGroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspFill)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real        anRGB[3];
  Standard_Real        aWidth;
  Quantity_Color       anIntColor;
  Quantity_Color       aBackIntColor;
  Quantity_Color       anEdgeColor;
  Aspect_TypeOfLine    aLType;
  Aspect_InteriorStyle aStyle;
  theAspFill->Values (aStyle, anIntColor, aBackIntColor, anEdgeColor, aLType, aWidth);

  anIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  ContextFillArea.Style      = aStyle;
  ContextFillArea.IntColor.r = float (anRGB[0]);
  ContextFillArea.IntColor.g = float (anRGB[1]);
  ContextFillArea.IntColor.b = float (anRGB[2]);

  if (theAspFill->Distinguish())
  {
    aBackIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }
  ContextFillArea.BackIntColor.r = float (anRGB[0]);
  ContextFillArea.BackIntColor.g = float (anRGB[1]);
  ContextFillArea.BackIntColor.b = float (anRGB[2]);

  // Edges
  ContextFillArea.Edge        = theAspFill->Edge() ? 1 : 0;
  ContextFillArea.EdgeColor.r = float (anEdgeColor.Red());
  ContextFillArea.EdgeColor.g = float (anEdgeColor.Green());
  ContextFillArea.EdgeColor.b = float (anEdgeColor.Blue());
  ContextFillArea.LineType    = aLType;
  ContextFillArea.Width       = float (aWidth);
  ContextFillArea.Hatch       = theAspFill->HatchStyle();

  // Front and Back face
  ContextFillArea.Distinguish = theAspFill->Distinguish() ? 1 : 0;
  ContextFillArea.BackFace    = theAspFill->BackFace()    ? 1 : 0;

  // Back Material
  const Graphic3d_MaterialAspect& aBack = theAspFill->BackMaterial();

  // Material properties
  ContextFillArea.Back.Shininess       = float (aBack.Shininess());
  ContextFillArea.Back.Ambient         = float (aBack.Ambient());
  ContextFillArea.Back.Diffuse         = float (aBack.Diffuse());
  ContextFillArea.Back.Specular        = float (aBack.Specular());
  ContextFillArea.Back.Transparency    = float (aBack.Transparency());
  ContextFillArea.Back.Emission        = float (aBack.Emissive());
  ContextFillArea.Back.RefractionIndex = float (aBack.RefractionIndex());
  ContextFillArea.Back.BSDF            = aBack.BSDF();

  // Reflection mode
  ContextFillArea.Back.IsAmbient    = aBack.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0;
  ContextFillArea.Back.IsDiffuse    = aBack.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0;
  ContextFillArea.Back.IsSpecular   = aBack.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0;
  ContextFillArea.Back.IsEmission   = aBack.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0;

  // Material type
  ContextFillArea.Back.IsPhysic = aBack.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0;

  // Specular color
  ContextFillArea.Back.ColorSpec.r  = float (aBack.SpecularColor().Red());
  ContextFillArea.Back.ColorSpec.g  = float (aBack.SpecularColor().Green());
  ContextFillArea.Back.ColorSpec.b  = float (aBack.SpecularColor().Blue());

  // Ambient color
  ContextFillArea.Back.ColorAmb.r   = float (aBack.AmbientColor().Red());
  ContextFillArea.Back.ColorAmb.g   = float (aBack.AmbientColor().Green());
  ContextFillArea.Back.ColorAmb.b   = float (aBack.AmbientColor().Blue());

  // Diffuse color
  ContextFillArea.Back.ColorDif.r   = float (aBack.DiffuseColor().Red());
  ContextFillArea.Back.ColorDif.g   = float (aBack.DiffuseColor().Green());
  ContextFillArea.Back.ColorDif.b   = float (aBack.DiffuseColor().Blue());

  // Emissive color
  ContextFillArea.Back.ColorEms.r   = float (aBack.EmissiveColor().Red());
  ContextFillArea.Back.ColorEms.g   = float (aBack.EmissiveColor().Green());
  ContextFillArea.Back.ColorEms.b   = float (aBack.EmissiveColor().Blue());

  ContextFillArea.Back.EnvReflexion = float (aBack.EnvReflexion());

  // Front Material
  const Graphic3d_MaterialAspect& aFront = theAspFill->FrontMaterial();

  // Material properties
  ContextFillArea.Front.Shininess       = float (aFront.Shininess());
  ContextFillArea.Front.Ambient         = float (aFront.Ambient());
  ContextFillArea.Front.Diffuse         = float (aFront.Diffuse());
  ContextFillArea.Front.Specular        = float (aFront.Specular());
  ContextFillArea.Front.Transparency    = float (aFront.Transparency());
  ContextFillArea.Front.Emission        = float (aFront.Emissive());
  ContextFillArea.Front.RefractionIndex = float (aFront.RefractionIndex());
  ContextFillArea.Front.BSDF            = aFront.BSDF();

  // Reflection mode
  ContextFillArea.Front.IsAmbient     = aFront.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0;
  ContextFillArea.Front.IsDiffuse     = aFront.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0;
  ContextFillArea.Front.IsSpecular    = aFront.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0;
  ContextFillArea.Front.IsEmission    = aFront.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0;

  // Material type
  ContextFillArea.Front.IsPhysic = aFront.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0;

  // Specular color
  ContextFillArea.Front.ColorSpec.r   = float (aFront.SpecularColor().Red());
  ContextFillArea.Front.ColorSpec.g   = float (aFront.SpecularColor().Green());
  ContextFillArea.Front.ColorSpec.b   = float (aFront.SpecularColor().Blue());

  // Ambient color
  ContextFillArea.Front.ColorAmb.r    = float (aFront.AmbientColor().Red());
  ContextFillArea.Front.ColorAmb.g    = float (aFront.AmbientColor().Green());
  ContextFillArea.Front.ColorAmb.b    = float (aFront.AmbientColor().Blue());

  // Diffuse color
  ContextFillArea.Front.ColorDif.r    = float (aFront.DiffuseColor().Red());
  ContextFillArea.Front.ColorDif.g    = float (aFront.DiffuseColor().Green());
  ContextFillArea.Front.ColorDif.b    = float (aFront.DiffuseColor().Blue());

  // Emissive color
  ContextFillArea.Front.ColorEms.r    = float (aFront.EmissiveColor().Red());
  ContextFillArea.Front.ColorEms.g    = float (aFront.EmissiveColor().Green());
  ContextFillArea.Front.ColorEms.b    = float (aFront.EmissiveColor().Blue());

  ContextFillArea.Front.EnvReflexion  = float (aFront.EnvReflexion());

  ContextFillArea.IsDef  = 1; // Definition material ok

  // Texture map
  ContextFillArea.Texture.TextureMap   = theAspFill->TextureMap();
  ContextFillArea.Texture.doTextureMap = theAspFill->TextureMapState() ? 1 : 0;

  Standard_Integer   aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspFill->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  ContextFillArea.PolygonOffsetMode   = aPolyMode;
  ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  ContextFillArea.PolygonOffsetUnits  = aPolyUnits;

  ContextFillArea.ShaderProgram = theAspFill->ShaderProgram();

  UpdateAspectFace (Standard_True);

  ContextFillArea.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetGroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& theAspMarker)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real       aScale;
  Quantity_Color      aColor;
  Aspect_TypeOfMarker aMarkerType;
  theAspMarker->Values (aColor, aMarkerType, aScale);

  ContextMarker.IsDef         = 1;
  ContextMarker.Color.r       = Standard_ShortReal (aColor.Red());
  ContextMarker.Color.g       = Standard_ShortReal (aColor.Green());
  ContextMarker.Color.b       = Standard_ShortReal (aColor.Blue());
  ContextMarker.MarkerType    = aMarkerType;
  ContextMarker.Scale         = Standard_ShortReal (aScale);
  ContextMarker.MarkerImage   = theAspMarker->GetMarkerImage();
  ContextMarker.ShaderProgram = theAspMarker->ShaderProgram();

  UpdateAspectMarker (Standard_True);

  ContextMarker.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetGroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetGroupPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& theAspText)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real  anExpansion, aSpace, aTextAngle;
  Quantity_Color aColor, aColorSubTitle;
  Standard_CString         aFont;
  Aspect_TypeOfStyleText   aStyle;
  Aspect_TypeOfDisplayText aDisplayType;
  Standard_Boolean         isTextZoomable;
  Font_FontAspect          aTextFontAspect;
  theAspText->Values (aColor, aFont, anExpansion, aSpace, aStyle, aDisplayType,
                      aColorSubTitle, isTextZoomable, aTextAngle, aTextFontAspect);

  ContextText.IsDef           = 1;
  ContextText.Color.r         = float (aColor.Red());
  ContextText.Color.g         = float (aColor.Green());
  ContextText.Color.b         = float (aColor.Blue());
  ContextText.Font            = (char* )aFont;
  ContextText.Expan           = float (anExpansion);
  ContextText.Space           = float (aSpace);
  ContextText.Style           = aStyle;
  ContextText.DisplayType     = aDisplayType;
  ContextText.ColorSubTitle.r = float (aColorSubTitle.Red());
  ContextText.ColorSubTitle.g = float (aColorSubTitle.Green());
  ContextText.ColorSubTitle.b = float (aColorSubTitle.Blue());
  ContextText.TextZoomable    = isTextZoomable;
  ContextText.TextAngle       = float (aTextAngle);
  ContextText.TextFontAspect  = aTextFontAspect;
  ContextText.ShaderProgram   = theAspText->ShaderProgram();

  UpdateAspectText (Standard_True);

  ContextText.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)& theAspLine)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real     aWidth;
  Quantity_Color    aColor;
  Aspect_TypeOfLine aLType;
  theAspLine->Values (aColor, aLType, aWidth);

  ContextLine.IsDef         = 1;
  ContextLine.Color.r       = float (aColor.Red());
  ContextLine.Color.g       = float (aColor.Green());
  ContextLine.Color.b       = float (aColor.Blue());
  ContextLine.LineType      = aLType;
  ContextLine.Width         = float (aWidth);
  ContextLine.ShaderProgram = theAspLine->ShaderProgram();

  UpdateAspectLine (Standard_False);

  ContextLine.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectFillArea3d)& theAspFill)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real        anRGB[3];
  Standard_Real        aWidth;
  Quantity_Color       anIntColor;
  Quantity_Color       aBackIntColor;
  Quantity_Color       anEdgeColor;
  Aspect_TypeOfLine    aLType;
  Aspect_InteriorStyle aStyle;
  theAspFill->Values (aStyle, anIntColor, aBackIntColor, anEdgeColor, aLType, aWidth);

  anIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  ContextFillArea.Style      = aStyle;
  ContextFillArea.IntColor.r = float (anRGB[0]);
  ContextFillArea.IntColor.g = float (anRGB[1]);
  ContextFillArea.IntColor.b = float (anRGB[2]);

  if (theAspFill->Distinguish())
  {
    aBackIntColor.Values (anRGB[0], anRGB[1], anRGB[2], Quantity_TOC_RGB);
  }
  ContextFillArea.BackIntColor.r = float(anRGB[0]);
  ContextFillArea.BackIntColor.g = float(anRGB[1]);
  ContextFillArea.BackIntColor.b = float(anRGB[2]);

  // Edges
  ContextFillArea.Edge        = theAspFill->Edge() ? 1 : 0;
  ContextFillArea.EdgeColor.r = float (anEdgeColor.Red());
  ContextFillArea.EdgeColor.g = float (anEdgeColor.Green());
  ContextFillArea.EdgeColor.b = float (anEdgeColor.Blue());
  ContextFillArea.LineType    = aLType;
  ContextFillArea.Width       = float (aWidth);
  ContextFillArea.Hatch       = theAspFill->HatchStyle();

  // Front and Back face
  ContextFillArea.Distinguish = theAspFill->Distinguish() ? 1 : 0;
  ContextFillArea.BackFace    = theAspFill->BackFace()    ? 1 : 0;

  // Back Material
  const Graphic3d_MaterialAspect& aBack = theAspFill->BackMaterial();
  // Material state
  ContextFillArea.Back.Shininess    = float (aBack.Shininess());
  ContextFillArea.Back.Ambient      = float (aBack.Ambient());
  ContextFillArea.Back.Diffuse      = float (aBack.Diffuse());
  ContextFillArea.Back.Specular     = float (aBack.Specular());
  ContextFillArea.Back.Transparency = float (aBack.Transparency());
  ContextFillArea.Back.Emission     = float (aBack.Emissive());

  // Reflection mode
  ContextFillArea.Back.IsAmbient    = aBack.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0;
  ContextFillArea.Back.IsDiffuse    = aBack.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0;
  ContextFillArea.Back.IsSpecular   = aBack.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0;
  ContextFillArea.Back.IsEmission   = aBack.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0;

  // Material type
  ContextFillArea.Back.IsPhysic = aBack.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0;

  // Specular color
  ContextFillArea.Back.ColorSpec.r  = float (aBack.SpecularColor().Red());
  ContextFillArea.Back.ColorSpec.g  = float (aBack.SpecularColor().Green());
  ContextFillArea.Back.ColorSpec.b  = float (aBack.SpecularColor().Blue());

  // Ambient color
  ContextFillArea.Back.ColorAmb.r   = float (aBack.AmbientColor().Red());
  ContextFillArea.Back.ColorAmb.g   = float (aBack.AmbientColor().Green());
  ContextFillArea.Back.ColorAmb.b   = float (aBack.AmbientColor().Blue());

  // Diffuse color
  ContextFillArea.Back.ColorDif.r   = float (aBack.DiffuseColor().Red());
  ContextFillArea.Back.ColorDif.g   = float (aBack.DiffuseColor().Green());
  ContextFillArea.Back.ColorDif.b   = float (aBack.DiffuseColor().Blue());

  // Emissive color
  ContextFillArea.Back.ColorEms.r   = float (aBack.EmissiveColor().Red());
  ContextFillArea.Back.ColorEms.g   = float (aBack.EmissiveColor().Green());
  ContextFillArea.Back.ColorEms.b   = float (aBack.EmissiveColor().Blue());

  ContextFillArea.Back.EnvReflexion = float (aBack.EnvReflexion());

  ContextFillArea.Back.RefractionIndex = float (aBack.RefractionIndex());
  ContextFillArea.Back.BSDF = aBack.BSDF();

  // Front Material
  const Graphic3d_MaterialAspect& aFront = theAspFill->FrontMaterial();
  // Light specificity
  ContextFillArea.Front.Shininess     = float (aFront.Shininess());
  ContextFillArea.Front.Ambient       = float (aFront.Ambient());
  ContextFillArea.Front.Diffuse       = float (aFront.Diffuse());
  ContextFillArea.Front.Specular      = float (aFront.Specular());
  ContextFillArea.Front.Transparency  = float (aFront.Transparency());
  ContextFillArea.Front.Emission      = float (aFront.Emissive());

  // Reflection mode
  ContextFillArea.Front.IsAmbient     = aFront.ReflectionMode (Graphic3d_TOR_AMBIENT)  ? 1 : 0;
  ContextFillArea.Front.IsDiffuse     = aFront.ReflectionMode (Graphic3d_TOR_DIFFUSE)  ? 1 : 0;
  ContextFillArea.Front.IsSpecular    = aFront.ReflectionMode (Graphic3d_TOR_SPECULAR) ? 1 : 0;
  ContextFillArea.Front.IsEmission    = aFront.ReflectionMode (Graphic3d_TOR_EMISSION) ? 1 : 0;

  // Material type
  ContextFillArea.Front.IsPhysic = aFront.MaterialType (Graphic3d_MATERIAL_PHYSIC) ? 1 : 0;

  // Specular color
  ContextFillArea.Front.ColorSpec.r   = float (aFront.SpecularColor().Red());
  ContextFillArea.Front.ColorSpec.g   = float (aFront.SpecularColor().Green());
  ContextFillArea.Front.ColorSpec.b   = float (aFront.SpecularColor().Blue());

  // Ambient color
  ContextFillArea.Front.ColorAmb.r    = float (aFront.AmbientColor().Red());
  ContextFillArea.Front.ColorAmb.g    = float (aFront.AmbientColor().Green());
  ContextFillArea.Front.ColorAmb.b    = float (aFront.AmbientColor().Blue());

  // Diffuse color
  ContextFillArea.Front.ColorDif.r    = float (aFront.DiffuseColor().Red());
  ContextFillArea.Front.ColorDif.g    = float (aFront.DiffuseColor().Green());
  ContextFillArea.Front.ColorDif.b    = float (aFront.DiffuseColor().Blue());

  // Emissive color
  ContextFillArea.Front.ColorEms.r    = float (aFront.EmissiveColor().Red());
  ContextFillArea.Front.ColorEms.g    = float (aFront.EmissiveColor().Green());
  ContextFillArea.Front.ColorEms.b    = float (aFront.EmissiveColor().Blue());

  ContextFillArea.Front.EnvReflexion  = float (aFront.EnvReflexion());

  ContextFillArea.Front.RefractionIndex = float (aFront.RefractionIndex());
  ContextFillArea.Front.BSDF = aFront.BSDF();

  ContextFillArea.IsDef = 1; // Material definition ok

  ContextFillArea.Texture.TextureMap   = theAspFill->TextureMap();
  ContextFillArea.Texture.doTextureMap = theAspFill->TextureMapState() ? 1 : 0;

  Standard_Integer   aPolyMode;
  Standard_ShortReal aPolyFactor, aPolyUnits;
  theAspFill->PolygonOffsets (aPolyMode, aPolyFactor, aPolyUnits);
  ContextFillArea.PolygonOffsetMode   = aPolyMode;
  ContextFillArea.PolygonOffsetFactor = aPolyFactor;
  ContextFillArea.PolygonOffsetUnits  = aPolyUnits;

  ContextFillArea.ShaderProgram = theAspFill->ShaderProgram();

  UpdateAspectFace (Standard_False);

  ContextFillArea.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectMarker3d)& theAspMarker)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_Real       aScale;
  Quantity_Color      aColor;
  Aspect_TypeOfMarker aMarkerType;
  theAspMarker->Values (aColor, aMarkerType, aScale);

  ContextMarker.IsDef         = 1;
  ContextMarker.Color.r       = Standard_ShortReal (aColor.Red());
  ContextMarker.Color.g       = Standard_ShortReal (aColor.Green());
  ContextMarker.Color.b       = Standard_ShortReal (aColor.Blue());
  ContextMarker.MarkerType    = aMarkerType;
  ContextMarker.Scale         = Standard_ShortReal (aScale);
  ContextMarker.MarkerImage   = theAspMarker->GetMarkerImage();

  ContextMarker.ShaderProgram = theAspMarker->ShaderProgram();

  UpdateAspectMarker (Standard_False);

  ContextMarker.IsSet = 1;

  Update();
}

// =======================================================================
// function : SetPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::SetPrimitivesAspect (const Handle(Graphic3d_AspectText3d)& theAspText)
{
  if (IsDeleted())
  {
    return;
  }

  Standard_CString aFont;
  Standard_Real aSpace, anExpansion, aTextAngle;
  Quantity_Color aColor, aColorSubTitle;
  Aspect_TypeOfStyleText   aStyle;
  Aspect_TypeOfDisplayText aDisplayType;
  Standard_Boolean aTextZoomable;
  Font_FontAspect  aTextFontAspect;
  theAspText->Values (aColor, aFont, anExpansion, aSpace, aStyle, aDisplayType,
                      aColorSubTitle, aTextZoomable, aTextAngle, aTextFontAspect);

  ContextText.IsDef           = 1;
  ContextText.Color.r         = float (aColor.Red());
  ContextText.Color.g         = float (aColor.Green());
  ContextText.Color.b         = float (aColor.Blue());
  ContextText.Font            = (char* )aFont;
  ContextText.Expan           = float (anExpansion);
  ContextText.Space           = float (aSpace);
  ContextText.Style           = aStyle;
  ContextText.DisplayType     = aDisplayType;
  ContextText.ColorSubTitle.r = float (aColorSubTitle.Red());
  ContextText.ColorSubTitle.g = float (aColorSubTitle.Green());
  ContextText.ColorSubTitle.b = float (aColorSubTitle.Blue());
  ContextText.TextZoomable    = aTextZoomable;
  ContextText.TextAngle       = float (aTextAngle);
  ContextText.TextFontAspect  = aTextFontAspect;
  ContextText.ShaderProgram   = theAspText->ShaderProgram();

  UpdateAspectText (Standard_False);

  ContextText.IsSet = 1;

  Update();
}

// =======================================================================
// function : IsGroupPrimitivesAspectSet
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsGroupPrimitivesAspectSet (const Graphic3d_GroupAspect theAspect) const
{
  switch (theAspect)
  {
    case Graphic3d_ASPECT_LINE:      return ContextLine.IsSet;
    case Graphic3d_ASPECT_TEXT:      return ContextText.IsSet;
    case Graphic3d_ASPECT_MARKER:    return ContextMarker.IsSet;
    case Graphic3d_ASPECT_FILL_AREA: return ContextFillArea.IsSet;
    default:                         return Standard_False;
  }
}

// =======================================================================
// function : GroupPrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::GroupPrimitivesAspect (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                             const Handle(Graphic3d_AspectText3d)&     theAspText,
                                             const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                             const Handle(Graphic3d_AspectFillArea3d)& theAspFill) const
{
  Quantity_Color aColor;
  Graphic3d_MaterialAspect aFront, aBack;

  const CALL_DEF_CONTEXTLINE& anAspLine = ContextLine.IsSet ? ContextLine : myStructure->CStructure()->ContextLine;
  aColor.SetValues (Standard_Real (anAspLine.Color.r),
                    Standard_Real (anAspLine.Color.g),
                    Standard_Real (anAspLine.Color.b), Quantity_TOC_RGB);
  theAspLine->SetColor         (aColor);
  theAspLine->SetType          (Aspect_TypeOfLine (anAspLine.LineType));
  theAspLine->SetWidth         (Standard_Real     (anAspLine.Width));
  theAspLine->SetShaderProgram (anAspLine.ShaderProgram);

  const CALL_DEF_CONTEXTTEXT& anAspText = ContextText.IsSet ? ContextText : myStructure->CStructure()->ContextText;
  aColor.SetValues (Standard_Real (anAspText.Color.r),
                    Standard_Real (anAspText.Color.g),
                    Standard_Real (anAspText.Color.b), Quantity_TOC_RGB);
  theAspText->SetColor (aColor);
  aColor.SetValues (Standard_Real (anAspText.ColorSubTitle.r),
                    Standard_Real (anAspText.ColorSubTitle.g),
                    Standard_Real (anAspText.ColorSubTitle.b), Quantity_TOC_RGB);
  theAspText->SetColorSubTitle   (aColor);
  theAspText->SetFont            (anAspText.Font);
  theAspText->SetSpace           (Standard_Real            (anAspText.Space));
  theAspText->SetExpansionFactor (Standard_Real            (anAspText.Expan));
  theAspText->SetStyle           (Aspect_TypeOfStyleText   (anAspText.Style));
  theAspText->SetDisplayType     (Aspect_TypeOfDisplayText (anAspText.DisplayType));
  theAspText->SetShaderProgram   (anAspText.ShaderProgram);

  const CALL_DEF_CONTEXTMARKER& anAspMarker = ContextMarker.IsSet ? ContextMarker : myStructure->CStructure()->ContextMarker;
  aColor.SetValues (Standard_Real (anAspMarker.Color.r),
                    Standard_Real (anAspMarker.Color.g),
                    Standard_Real (anAspMarker.Color.b), Quantity_TOC_RGB);
  theAspMarker->SetColor (aColor);
  theAspMarker->SetType  (anAspMarker.MarkerType);
  theAspMarker->SetScale (Standard_Real (anAspMarker.Scale));
  theAspMarker->SetShaderProgram (anAspMarker.ShaderProgram);
  if (anAspMarker.MarkerType == Aspect_TOM_USERDEFINED)
  {
    theAspMarker->SetMarkerImage (ContextMarker.MarkerImage);
  }

  const CALL_DEF_CONTEXTFILLAREA& anAspFill = ContextFillArea.IsSet ? ContextFillArea : myStructure->CStructure()->ContextFillArea;
  // Interior
  theAspFill->SetInteriorStyle (Aspect_InteriorStyle (anAspFill.Style));
  aColor.SetValues (Standard_Real (anAspFill.IntColor.r),
                    Standard_Real (anAspFill.IntColor.g),
                    Standard_Real (anAspFill.IntColor.b), Quantity_TOC_RGB);
  theAspFill->SetInteriorColor (aColor);

  // Edges
  aColor.SetValues (Standard_Real (anAspFill.EdgeColor.r),
                    Standard_Real (anAspFill.EdgeColor.g),
                    Standard_Real (anAspFill.EdgeColor.b), Quantity_TOC_RGB);
  theAspFill->SetEdgeColor    (aColor);
  theAspFill->SetEdgeLineType (Aspect_TypeOfLine (anAspFill.LineType));
  theAspFill->SetEdgeWidth    (Standard_Real     (anAspFill.Width));

  // Back Material
  aBack.SetShininess    (Standard_Real (anAspFill.Back.Shininess));
  aBack.SetAmbient      (Standard_Real (anAspFill.Back.Ambient));
  aBack.SetDiffuse      (Standard_Real (anAspFill.Back.Diffuse));
  aBack.SetSpecular     (Standard_Real (anAspFill.Back.Specular));
  aBack.SetTransparency (Standard_Real (anAspFill.Back.Transparency));
  aBack.SetEmissive     (Standard_Real (anAspFill.Back.Emission));
  anAspFill.Back.IsAmbient  == 1 ? aBack.SetReflectionModeOn (Graphic3d_TOR_AMBIENT)  : aBack.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  anAspFill.Back.IsDiffuse  == 1 ? aBack.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE)  : aBack.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  anAspFill.Back.IsSpecular == 1 ? aBack.SetReflectionModeOn (Graphic3d_TOR_SPECULAR) : aBack.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  anAspFill.Back.IsEmission == 1 ? aBack.SetReflectionModeOn (Graphic3d_TOR_EMISSION) : aBack.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  aColor.SetValues (Standard_Real (anAspFill.Back.ColorSpec.r),
                    Standard_Real (anAspFill.Back.ColorSpec.g),
                    Standard_Real (anAspFill.Back.ColorSpec.b), Quantity_TOC_RGB);
  aBack.SetSpecularColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Back.ColorAmb.r),
                    Standard_Real (anAspFill.Back.ColorAmb.g),
                    Standard_Real (anAspFill.Back.ColorAmb.b), Quantity_TOC_RGB);
  aBack.SetAmbientColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Back.ColorDif.r),
                    Standard_Real (anAspFill.Back.ColorDif.g),
                    Standard_Real (anAspFill.Back.ColorDif.b), Quantity_TOC_RGB);
  aBack.SetDiffuseColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Back.ColorEms.r),
                    Standard_Real (anAspFill.Back.ColorEms.g),
                    Standard_Real (anAspFill.Back.ColorEms.b), Quantity_TOC_RGB);
  aBack.SetEmissiveColor (aColor);

  aBack.SetEnvReflexion (anAspFill.Back.EnvReflexion);

  aBack.SetRefractionIndex (Standard_Real (anAspFill.Back.RefractionIndex));
  aBack.SetBSDF (anAspFill.Back.BSDF);

  // Front Material
  aFront.SetShininess    (Standard_Real (anAspFill.Front.Shininess));
  aFront.SetAmbient      (Standard_Real (anAspFill.Front.Ambient));
  aFront.SetDiffuse      (Standard_Real (anAspFill.Front.Diffuse));
  aFront.SetSpecular     (Standard_Real (anAspFill.Front.Specular));
  aFront.SetTransparency (Standard_Real (anAspFill.Front.Transparency));
  aFront.SetEmissive     (Standard_Real (anAspFill.Back.Emission));
  anAspFill.Front.IsAmbient  == 1 ? aFront.SetReflectionModeOn (Graphic3d_TOR_AMBIENT)  : aFront.SetReflectionModeOff (Graphic3d_TOR_AMBIENT);
  anAspFill.Front.IsDiffuse  == 1 ? aFront.SetReflectionModeOn (Graphic3d_TOR_DIFFUSE)  : aFront.SetReflectionModeOff (Graphic3d_TOR_DIFFUSE);
  anAspFill.Front.IsSpecular == 1 ? aFront.SetReflectionModeOn (Graphic3d_TOR_SPECULAR) : aFront.SetReflectionModeOff (Graphic3d_TOR_SPECULAR);
  anAspFill.Front.IsEmission == 1 ? aFront.SetReflectionModeOn (Graphic3d_TOR_EMISSION) : aFront.SetReflectionModeOff (Graphic3d_TOR_EMISSION);

  aColor.SetValues (Standard_Real (anAspFill.Front.ColorSpec.r),
                    Standard_Real (anAspFill.Front.ColorSpec.g),
                    Standard_Real (anAspFill.Front.ColorSpec.b), Quantity_TOC_RGB);
  aFront.SetSpecularColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Front.ColorAmb.r),
                    Standard_Real (anAspFill.Front.ColorAmb.g),
                    Standard_Real (anAspFill.Front.ColorAmb.b), Quantity_TOC_RGB);
  aFront.SetAmbientColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Front.ColorDif.r),
                    Standard_Real (anAspFill.Front.ColorDif.g),
                    Standard_Real (anAspFill.Front.ColorDif.b), Quantity_TOC_RGB);
  aFront.SetDiffuseColor (aColor);

  aColor.SetValues (Standard_Real (anAspFill.Front.ColorEms.r),
                    Standard_Real (anAspFill.Front.ColorEms.g),
                    Standard_Real (anAspFill.Front.ColorEms.b), Quantity_TOC_RGB);
  aFront.SetEmissiveColor (aColor);

  aFront.SetEnvReflexion (anAspFill.Front.EnvReflexion);

  aFront.SetRefractionIndex (Standard_Real (anAspFill.Front.RefractionIndex));
  aFront.SetBSDF (anAspFill.Front.BSDF);

  // Edges
  anAspFill.Edge == 1 ? theAspFill->SetEdgeOn() : theAspFill->SetEdgeOff();
  // Hatch
  theAspFill->SetHatchStyle (Aspect_HatchStyle (anAspFill.Hatch));
  // Materials
  // Front and Back face
  anAspFill.Distinguish == 1 ? theAspFill->SetDistinguishOn() : theAspFill->SetDistinguishOff();
  anAspFill.BackFace    == 1 ? theAspFill->SuppressBackFace() : theAspFill->AllowBackFace();
  // Texture
  theAspFill->SetTextureMap (anAspFill.Texture.TextureMap);
  anAspFill.Texture.doTextureMap == 1 ? theAspFill->SetTextureMapOn() : theAspFill->SetTextureMapOff();
  theAspFill->SetShaderProgram  (anAspFill.ShaderProgram);
  theAspFill->SetPolygonOffsets (anAspFill.PolygonOffsetMode,
                                 anAspFill.PolygonOffsetFactor,
                                 anAspFill.PolygonOffsetUnits);
  theAspFill->SetBackMaterial  (aBack);
  theAspFill->SetFrontMaterial (aFront);
}

// =======================================================================
// function : PrimitivesAspect
// purpose  :
// =======================================================================
void Graphic3d_Group::PrimitivesAspect (const Handle(Graphic3d_AspectLine3d)&     theAspLine,
                                        const Handle(Graphic3d_AspectText3d)&     theAspText,
                                        const Handle(Graphic3d_AspectMarker3d)&   theAspMarker,
                                        const Handle(Graphic3d_AspectFillArea3d)& theAspFill) const
{
  GroupPrimitivesAspect (theAspLine, theAspText, theAspMarker, theAspFill);
}

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================
void Graphic3d_Group::AddPrimitiveArray (const Handle(Graphic3d_ArrayOfPrimitives)& thePrim,
                                         const Standard_Boolean                     theToEvalMinMax)
{
  if (IsDeleted()
  || !thePrim->IsValid())
  {
    return;
  }

  AddPrimitiveArray (thePrim->Type(), thePrim->Indices(), thePrim->Attributes(), thePrim->Bounds(), theToEvalMinMax);
}

// =======================================================================
// function : AddPrimitiveArray
// purpose  :
// =======================================================================
void Graphic3d_Group::AddPrimitiveArray (const Graphic3d_TypeOfPrimitiveArray theType,
                                         const Handle(Graphic3d_IndexBuffer)& ,
                                         const Handle(Graphic3d_Buffer)&      theAttribs,
                                         const Handle(Graphic3d_BoundBuffer)& ,
                                         const Standard_Boolean               theToEvalMinMax)
{
  if (IsDeleted()
   || theAttribs.IsNull())
  {
    return;
  }

  if (!MyContainsFacet
    && theType != Graphic3d_TOPA_POLYLINES
    && theType != Graphic3d_TOPA_SEGMENTS
    && theType != Graphic3d_TOPA_POINTS)
  {
    myStructure->GroupsWithFacet (1);
    MyContainsFacet = Standard_True;
  }

  if (theToEvalMinMax)
  {
    const Standard_Integer aNbVerts = theAttribs->NbElements;
    for (Standard_Integer anAttribIter = 0; anAttribIter < theAttribs->NbAttributes; ++anAttribIter)
    {
      const Graphic3d_Attribute& anAttrib = theAttribs->Attribute (anAttribIter);
      if (anAttrib.Id != Graphic3d_TOA_POS)
      {
        continue;
      }

      const size_t anOffset = theAttribs->AttributeOffset (anAttribIter);
      switch (anAttrib.DataType)
      {
        case Graphic3d_TOD_VEC2:
        {
          for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
          {
            const Graphic3d_Vec2& aVert = *reinterpret_cast<const Graphic3d_Vec2* >(theAttribs->value (aVertIter) + anOffset);
            myBounds.Add (Graphic3d_Vec4 (aVert.x(), aVert.y(), 0.0f, 1.0f));
          }
          break;
        }
        case Graphic3d_TOD_VEC3:
        case Graphic3d_TOD_VEC4:
        {
          for (Standard_Integer aVertIter = 0; aVertIter < aNbVerts; ++aVertIter)
          {
            const Graphic3d_Vec3& aVert = *reinterpret_cast<const Graphic3d_Vec3* >(theAttribs->value (aVertIter) + anOffset);
            myBounds.Add (Graphic3d_Vec4 (aVert.x(), aVert.y(), aVert.z(), 1.0f));
          }
          break;
        }
        default: break;
      }
      break;
    }
  }

  Update();
}

// =======================================================================
// function : Marker
// purpose  :
// =======================================================================
void Graphic3d_Group::Marker (const Graphic3d_Vertex& thePoint,
                              const Standard_Boolean  theToEvalMinMax)
{
  Handle(Graphic3d_ArrayOfPoints) aPoints = new Graphic3d_ArrayOfPoints (1);
  aPoints->AddVertex (thePoint.X(), thePoint.Y(), thePoint.Z());
  AddPrimitiveArray (aPoints, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString                  /*theText*/,
                            const Graphic3d_Vertex&                 thePoint,
                            const Standard_Real                     /*theHeight*/,
                            const Quantity_PlaneAngle               /*theAngle*/,
                            const Graphic3d_TextPath                /*theTp*/,
                            const Graphic3d_HorizontalTextAlignment /*theHta*/,
                            const Graphic3d_VerticalTextAlignment   /*theVta*/,
                            const Standard_Boolean                  theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theToEvalMinMax)
  {
    Standard_ShortReal x, y, z;
    thePoint.Coord (x, y, z);
    myStructure->CStructure()->Is2dText = Standard_True;
    myBounds.Add (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (x),
                                  static_cast<Standard_ShortReal> (y),
                                  static_cast<Standard_ShortReal> (z),
                                  1.0f));
  }
  Update();
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString  theText,
                            const Graphic3d_Vertex& thePoint,
                            const Standard_Real     theHeight,
                            const Standard_Boolean  theToEvalMinMax)
{
  Text (theText, thePoint, theHeight, 0.0,
        Graphic3d_TP_RIGHT, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString&       theText,
                            const Graphic3d_Vertex&                 thePoint,
                            const Standard_Real                     theHeight,
                            const Quantity_PlaneAngle               theAngle,
                            const Graphic3d_TextPath                theTp,
                            const Graphic3d_HorizontalTextAlignment theHta,
                            const Graphic3d_VerticalTextAlignment   theVta,
                            const Standard_Boolean                  theToEvalMinMax)
{
  const NCollection_String aText ((Standard_Utf16Char* )theText.ToExtString());
  Text (aText.ToCString(), thePoint, theHeight, theAngle,
        theTp, theHta, theVta, theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString&       theText,
                            const gp_Ax2&                           theOrientation,
                            const Standard_Real                     theHeight,
                            const Quantity_PlaneAngle               theAngle,
                            const Graphic3d_TextPath                theTP,
                            const Graphic3d_HorizontalTextAlignment theHTA,
                            const Graphic3d_VerticalTextAlignment   theVTA,
                            const Standard_Boolean                  theToEvalMinMax)
{
  const NCollection_String aText ((Standard_Utf16Char*)(theText.ToExtString()));
  Text (aText.ToCString(),
        theOrientation,
        theHeight,
        theAngle,
        theTP,
        theHTA,
        theVTA,
        theToEvalMinMax);
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const Standard_CString                  /*theText*/,
                            const gp_Ax2&                           theOrientation,
                            const Standard_Real                     /*theHeight*/,
                            const Quantity_PlaneAngle               /*theAngle*/,
                            const Graphic3d_TextPath                /*theTp*/,
                            const Graphic3d_HorizontalTextAlignment /*theHta*/,
                            const Graphic3d_VerticalTextAlignment   /*theVta*/,
                            const Standard_Boolean                  theToEvalMinMax)
{
  if (IsDeleted())
  {
    return;
  }

  if (theToEvalMinMax)
  {
    myStructure->CStructure()->Is2dText = Standard_False;
    myBounds.Add (Graphic3d_Vec4 (static_cast<Standard_ShortReal> (theOrientation.Location().X()),
                                  static_cast<Standard_ShortReal> (theOrientation.Location().Y()),
                                  static_cast<Standard_ShortReal> (theOrientation.Location().Z()),
                                  1.0f));
  }
  Update();
}

// =======================================================================
// function : Text
// purpose  :
// =======================================================================
void Graphic3d_Group::Text (const TCollection_ExtendedString& theText,
                            const Graphic3d_Vertex&           thePoint,
                            const Standard_Real               theHeight,
                            const Standard_Boolean            theToEvalMinMax)
{
  const NCollection_String aText ((Standard_Utf16Char* )theText.ToExtString());
  Text (aText.ToCString(), thePoint, theHeight, 0.0,
        Graphic3d_TP_RIGHT, Graphic3d_HTA_LEFT, Graphic3d_VTA_BOTTOM, theToEvalMinMax);
}

// =======================================================================
// function : SetClosed
// purpose  :
// =======================================================================
void Graphic3d_Group::SetClosed (const Standard_Boolean theIsClosed)
{
  myIsClosed = theIsClosed;
}

// =======================================================================
// function : IsClosed
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_Group::IsClosed() const
{
  return myIsClosed;
}

//=======================================================================
//function : BoundingBox
//purpose  :
//=======================================================================
const Graphic3d_BndBox4f& Graphic3d_Group::BoundingBox() const
{
  return myBounds;
}

//=======================================================================
//function : ChangeBoundingBox
//purpose  :
//=======================================================================
Graphic3d_BndBox4f& Graphic3d_Group::ChangeBoundingBox()
{
  return myBounds;
}
