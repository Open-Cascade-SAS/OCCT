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

#include <Graphic3d_Aspects.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Aspects, Standard_Transient)

// =======================================================================
// function : Graphic3d_Aspects
// purpose  :
// =======================================================================
Graphic3d_Aspects::Graphic3d_Aspects()
: myInteriorColor       (Quantity_NOC_CYAN1),
  myBackInteriorColor   (Quantity_NOC_CYAN1),
  myEdgeColor           (Quantity_NOC_WHITE),
  myInteriorStyle       (Aspect_IS_SOLID),
  myShadingModel        (Graphic3d_TypeOfShadingModel_DEFAULT),
  myFaceCulling         (Graphic3d_TypeOfBackfacingModel_Auto),
  myAlphaMode           (Graphic3d_AlphaMode_BlendAuto),
  myAlphaCutoff         (0.5f),
  myLineType            (Aspect_TOL_SOLID),
  myLineWidth           (1.0f),
  myLineFactor          (1),
  myLinePattern         (0xFFFF),
  myMarkerType          (Aspect_TOM_POINT),
  myMarkerScale         (1.0f),
  myTextStyle           (Aspect_TOST_NORMAL),
  myTextDisplayType     (Aspect_TODT_NORMAL),
  myTextFontAspect      (Font_FontAspect_Regular),
  myTextAngle           (0.0f),
  myToSkipFirstEdge     (false),
  myToDistinguishMaterials (false),
  myToDrawEdges         (false),
  myToDrawSilhouette    (false),
  myToMapTexture        (false),
  myIsTextZoomable      (false)
{
  //
}

// =======================================================================
// function : SetTextureMap
// purpose  :
// =======================================================================
void Graphic3d_Aspects::SetTextureMap (const Handle(Graphic3d_TextureMap)& theTexture)
{
  if (theTexture.IsNull())
  {
    myTextureSet.Nullify();
    return;
  }

  myTextureSet = new Graphic3d_TextureSet (theTexture);
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void Graphic3d_Aspects::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myInteriorColor)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myBackInteriorColor)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myEdgeColor)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myPolygonOffset)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToSkipFirstEdge)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDistinguishMaterials)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDrawEdges)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDrawSilhouette)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myFaceCulling)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToMapTexture)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsTextZoomable)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAlphaMode)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myAlphaCutoff)

  if (!myTextFont.IsNull())
  {
    OCCT_DUMP_FIELD_VALUE_STRING (theOStream, myTextFont->String())
  }

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextStyle)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextDisplayType)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextFontAspect)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextAngle)
}
