// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

/***********************************************************************

FONCTION :
----------
File OpenGl_tsm :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
17-07-96 : FMN ; Suppression de TelHLHSRMode
05-08-97 : PCT ; support texture mapping
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
et TelBackInteriorShadingMethod
30-11-98 : FMN ; S3819 : Textes toujours visibles
21-09-99 : EUG : G003 : Degeneration management
16-06-00 : ATS,SPK : G005 : Group of new primitives: TelParray
22-03-04 : SAN : OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

#ifndef  OPENGL_TSM_H
#define  OPENGL_TSM_H

#define OCC1188 /* SAV 23/12/02 Added structure to control background texture
+ enum to control texture fill method
*/

typedef  enum
{
  TelNil = 0,
  TelLabel,
  TelAntiAliasing,
  TelBlink,
  TelExecuteStructure,
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  TelTransformPersistence,
  /* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
  TelPolylineColour,
  TelPolylineType,
  TelPolylineWidth,
  TelInteriorStyle,
  TelInteriorColour,
  TelBackInteriorColour,
  TelEdgeColour,
  TelEdgeType,
  TelEdgeWidth,
  TelEdgeFlag,
  TelInteriorStyleIndex,
  TelFaceDistinguishingMode,
  TelFaceCullingMode,
  TelInteriorReflectanceEquation,
  TelSurfaceAreaProperties,
  TelBackInteriorReflectanceEquation,
  TelBackSurfaceAreaProperties,
  TelPolymarkerColour,
  TelPolymarkerType,
  TelPolymarkerSize,
  TelTextFont,
  TelTextHeight,
  TelCharacterSpacing,
  TelCharacterExpansionFactor,
  TelTextColour,
  TelDepthCueIndex,
  TelPickId,
  TelPolygon,
  TelPolygonSet,
  TelPolygonHoles,
  TelPolygonIndices,
  TelQuadrangle,
  TelPolyline,
  TelMarker,
  TelMarkerSet,
  TelText,
  TelTriangleMesh,
  TelAddNameset,
  TelRemoveNameset,
  TelInteriorShadingMethod,
  TelLocalTran3,
  TelHighlightIndex,
  TelLightSrcState,
  TelClippingPlane,
  TelViewIndex,
  TelApplicationData,
  TelCurve,
  TelTextureId,
  TelDoTextureMap,
  TelTextStyle,
  TelTextDisplayType, 
  TelTextColourSubTitle, 
  TelDegenerationMode,
  TelTextZoomable,//Text Zoomable attributes
  TelTextAngle,//Text Angle attributes
  TelTextFontAspect,//Text Font Aspect attributes 
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  TelTextAlign,
  /*OCC7456 abd 14.12.2004 Text alingnment attributes  */
  TelParray,
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  TelPolygonOffset,
  /* OCC4895 SAN 22/03/04 High-level interface for controlling polygon offsets */
  /* OCC???? SZV 11/08/05 Implementation of callbacks */
  TelUserdraw,
  /* OCC???? SZV 11/08/05 Implementation of callbacks */
  TelLast
} TelType;

#endif
