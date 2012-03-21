// Created on: 2011-10-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2012 OPEN CASCADE SAS
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


#include <OpenGl_GraphicDriver.hxx>

#include <Aspect_TypeOfMarker.hxx>
#include <OpenGl_Group.hxx>
#include <OpenGl_MarkerSet.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

static void OpenGl_CreateMarkers(const Graphic3d_CGroup& ACGroup, const Standard_Integer NbPoints, const Graphic3d_Vertex *Points)
{
  OpenGl_MarkerSet *amarkerset = new OpenGl_MarkerSet( NbPoints, Points );
  ((OpenGl_Group *)ACGroup.ptrGroup)->AddElement( TelMarkerSet, amarkerset );
}

void OpenGl_GraphicDriver::Marker (const Graphic3d_CGroup& ACGroup, const Graphic3d_Vertex& APoint, const Standard_Boolean )
{
  if (!ACGroup.ptrGroup)
    return;

  const Aspect_TypeOfMarker AType = Aspect_TypeOfMarker (ACGroup.ContextMarker.MarkerType);

  switch (AType) 
  {
    case Aspect_TOM_POINT :
    case Aspect_TOM_PLUS :
    case Aspect_TOM_STAR :
    case Aspect_TOM_O :
    case Aspect_TOM_X :
    case Aspect_TOM_O_POINT :
    case Aspect_TOM_O_PLUS :
    case Aspect_TOM_O_STAR :
    case Aspect_TOM_O_X :
    case Aspect_TOM_RING1 :
    case Aspect_TOM_RING2 :
    case Aspect_TOM_RING3 :
    case Aspect_TOM_USERDEFINED:
      OpenGl_CreateMarkers(ACGroup, 1, &APoint);
      break;

    case Aspect_TOM_BALL :
    {
      const Standard_Real Delta = 0.1;

      Standard_Real R, G, B, H, L, S, AScale;
      R = Standard_Real (ACGroup.ContextMarker.Color.r);
      G = Standard_Real (ACGroup.ContextMarker.Color.g);
      B = Standard_Real (ACGroup.ContextMarker.Color.b);
      AScale = Standard_Real (ACGroup.ContextMarker.Scale);

      // we draw a set of circles
	  CALL_DEF_CONTEXTMARKER AContext;
      AContext.MarkerType = int (Aspect_TOM_O);
      while (AScale >= 1.0)
      {
        Quantity_Color::RgbHls (R, G, B, H, L, S);
        // Modification de la saturation de 5 %
        S *= 0.95;
        Quantity_Color::HlsRgb (H, L, S, R, G, B);
        AContext.Color.r  = float (R);
        AContext.Color.g  = float (G);
        AContext.Color.b  = float (B);
        AContext.Scale  = float (AScale);
        ((OpenGl_Group *)ACGroup.ptrGroup)->SetAspectMarker(AContext, Standard_False);
        OpenGl_CreateMarkers(ACGroup, 1, &APoint);
        AScale -= Delta;
      }
      break;
    }
    default:
      break;
  }
}

void OpenGl_GraphicDriver::MarkerSet (const Graphic3d_CGroup& ACGroup,
                                     const Graphic3d_Array1OfVertex& ListVertex,
                                     const Standard_Boolean )
{
  if (!ACGroup.ptrGroup)
    return;

  const Standard_Integer nb = ListVertex.Length();
  const Graphic3d_Vertex *arr = &ListVertex( ListVertex.Lower() );

  const Aspect_TypeOfMarker AType = Aspect_TypeOfMarker (ACGroup.ContextMarker.MarkerType);

  switch (AType)
  {
    case Aspect_TOM_POINT :
    case Aspect_TOM_PLUS :
    case Aspect_TOM_STAR :
    case Aspect_TOM_O :
    case Aspect_TOM_X :
    case Aspect_TOM_O_POINT :
    case Aspect_TOM_O_PLUS :
    case Aspect_TOM_O_STAR :
    case Aspect_TOM_O_X :
    case Aspect_TOM_RING1 :
    case Aspect_TOM_RING2 :
    case Aspect_TOM_RING3 :
    case Aspect_TOM_USERDEFINED:
      OpenGl_CreateMarkers(ACGroup, nb, arr);
      break;

    case Aspect_TOM_BALL :
    {
      const Standard_Real Delta = 0.1;

      Standard_Real R, G, B, H, L, S, AScale;
      R = Standard_Real (ACGroup.ContextMarker.Color.r);
      G = Standard_Real (ACGroup.ContextMarker.Color.g);
      B = Standard_Real (ACGroup.ContextMarker.Color.b);
      AScale = Standard_Real (ACGroup.ContextMarker.Scale);

      // we draw a set of circles
	  CALL_DEF_CONTEXTMARKER AContext;
      AContext.MarkerType = int (Aspect_TOM_O);
      while (AScale >= 1.0)
      {
        Quantity_Color::RgbHls (R, G, B, H, L, S);
        // Modification de la saturation de 5 %
        S *= 0.95;
        Quantity_Color::HlsRgb (H, L, S, R, G, B);
        AContext.Color.r  = float (R);
        AContext.Color.g  = float (G);
        AContext.Color.b  = float (B);
        AContext.Scale  = float (AScale);
        ((OpenGl_Group *)ACGroup.ptrGroup)->SetAspectMarker(AContext, Standard_False);
        OpenGl_CreateMarkers(ACGroup, nb, arr);
        AScale -= Delta;
      }
      break;
    }
    default:
      break;
  }
}
