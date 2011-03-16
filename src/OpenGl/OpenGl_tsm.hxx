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

#define G003  /* EUG 21-09-99 Degeneration management
*/

#define OCC1188 /* SAV 23/12/02 Added structure to control background texture
+ enum to control texture fill method
*/


#include <OpenGl_cmn_varargs.hxx>

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
#ifdef G003
  TelDegenerationMode,
#endif  /* G003 */
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

typedef  union  TSM_ELEM_DATA_UNION
{
  void      *pdata;
  Tint       ldata;
} TSM_ELEM_DATA, *tsm_elem_data;

typedef  struct  TSM_ELEM_STRUCT
{
  TelType        el;
  TSM_ELEM_DATA  data;
  IMPLEMENT_MEMORY_OPERATORS
} TSM_ELEM, *tsm_elem;

/* A node containing an elem when structure is in the form of a list */
struct  TSM_NODE_STRUCT
{
  struct  TSM_NODE_STRUCT  *next;
  struct  TSM_NODE_STRUCT  *prev;
  TSM_ELEM   elem;
  IMPLEMENT_MEMORY_OPERATORS
};
typedef TSM_NODE_STRUCT  TSM_NODE;
typedef TSM_NODE_STRUCT* tsm_node;

#ifdef OCC1188
/* background texture properties */
typedef enum
{
  TSM_FS_CENTER,
  TSM_FS_TILE,
  TSM_FS_STRETCH
} TSM_FillStyle;

typedef struct 
{
  Tuint           texId;
  Tint            width;
  Tint            height;
  TSM_FillStyle   style;
  IMPLEMENT_MEMORY_OPERATORS
} TSM_BG_TEXTURE, *tsm_bg_texture;
#endif /* OCC1188 */

typedef enum
{
  TSM_GT_NONE,
  TSM_GT_HOR,
  TSM_GT_VER,
  TSM_GT_DIAG1,
  TSM_GT_DIAG2,
  TSM_GT_CORNER1,
  TSM_GT_CORNER2,
  TSM_GT_CORNER3,
  TSM_GT_CORNER4
} TSM_GradientType;

typedef struct 
{
  TEL_COLOUR     color1;
  TEL_COLOUR     color2;
  TSM_GradientType   type;
  IMPLEMENT_MEMORY_OPERATORS
} TSM_BG_GRADIENT, *tsm_bg_gradient;


typedef  enum
{
  PickTraverse=0,
  DisplayTraverse,
  Add,
  Delete,
  Print,
  Inquire
} TMsgType;

typedef  enum
{
  TEditInsert = 1,
  TEditReplace
} TEditMode;

extern  Tint  TglActiveWs;      /* currently defined in tsm/tsm.c */

typedef  TStatus (**MtblPtr)( TSM_ELEM_DATA, Tint, cmn_key* );

extern  void      TsmInitAllClasses( MtblPtr  (**tbl)(TelType*), Tint size );

extern  TEditMode TsmSetEditMode( TEditMode );
extern  TStatus   TsmSendMessage( TelType, TMsgType, TSM_ELEM_DATA, Tint, ... );
extern  TStatus   TsmOpenStructure( Tint );
extern  TStatus   TsmCloseStructure();
extern  TStatus   TsmDisplayStructure( Tint, Tint );
extern  TStatus   TsmPrintStructure( Tint );
extern  TStatus   TsmAddToStructure( TelType, Tint, ... );
extern  TStatus   TsmDeleteStructure( Tint );
extern  TStatus   TsmDeleteElement();
extern  TStatus   TsmDeleteElementsBetweenLabels( Tint, Tint );
extern  TStatus   TsmDeleteElementRange( Tint, Tint );
extern  TStatus   TsmSetElementPointer( Tint );
extern  TStatus   TsmSetElementPointerAtLabel( Tint );
extern  TStatus   TsmOffsetElementPointer( Tint );
extern  TStatus   TsmGetStructure( Tint, Tint*, tsm_node * );
extern  TStatus   TsmGetStructureDepth( Tint, Tint* );
extern  TStatus   TsmGetCurElem( TSM_ELEM * );
extern  TStatus   TsmGetCurElemPtr( Tint * );

#endif
