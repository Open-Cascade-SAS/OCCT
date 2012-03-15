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
