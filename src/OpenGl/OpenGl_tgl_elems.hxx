/***********************************************************************

FONCTION :
----------
File OpenGl_tgl_elems :


REMARQUES:
----------       

HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
17-07-96 : FMN ; Suppression de TelHLHSRMode
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
et TelBackInteriorShadingMethod

************************************************************************/

#ifndef OPENGL_TGL_ELEMS_H
#define OPENGL_TGL_ELEMS_H

#include <OpenGl_tsm.hxx>

/* Element types */
#define CALL_PHIGS_ELEM_NIL                   TelNil
#define CALL_PHIGS_ELEM_POLYMARKER3           TelMarkerSet
#define CALL_PHIGS_ELEM_ANNO_TEXT_REL3        TelText
#define CALL_PHIGS_ELEM_LINETYPE              TelPolylineType
#define CALL_PHIGS_ELEM_LINEWIDTH             TelPolylineWidth
#define CALL_PHIGS_ELEM_MARKER_TYPE           TelPolymarkerType
#define CALL_PHIGS_ELEM_MARKER_SIZE           TelPolymarkerSize
#define CALL_PHIGS_ELEM_TEXT_FONT             TelTextFont
#define CALL_PHIGS_ELEM_CHAR_EXPAN            TelCharacterExpansionFactor
#define CALL_PHIGS_ELEM_CHAR_SPACE            TelCharacterSpacing
#define CALL_PHIGS_ELEM_ANNO_CHAR_HT          TelTextHeight
#define CALL_PHIGS_ELEM_INT_STYLE             TelInteriorStyle
#define CALL_PHIGS_ELEM_INT_STYLE_IND         TelInteriorStyleIndex
#define CALL_PHIGS_ELEM_EDGE_FLAG             TelEdgeFlag
#define CALL_PHIGS_ELEM_EDGETYPE              TelEdgeType
#define CALL_PHIGS_ELEM_EDGEWIDTH             TelEdgeWidth
#define CALL_PHIGS_ELEM_ADD_NAMES_SET         TelAddNameset
#define CALL_PHIGS_ELEM_REMOVE_NAMES_SET      TelRemoveNameset
#define CALL_PHIGS_ELEM_LOCAL_MODEL_TRAN3     TelLocalTran3
#define CALL_PHIGS_ELEM_VIEW_IND              TelViewIndex
#define CALL_PHIGS_ELEM_EXEC_STRUCT           TelExecuteStructure
#define CALL_PHIGS_ELEM_LABEL                 TelLabel
#define CALL_PHIGS_ELEM_APPL_DATA             TelApplicationData
#define CALL_PHIGS_ELEM_PICK_ID               TelPickId
#define CALL_PHIGS_ELEM_POLYLINE_SET3_DATA    TelPolyline
#define CALL_PHIGS_ELEM_TRI_STRIP3_DATA       TelTriangleMesh
#define CALL_PHIGS_ELEM_QUAD_MESH3_DATA       TelQuadrangle
#define CALL_PHIGS_ELEM_TEXT_COLR             TelTextColour
#define CALL_PHIGS_ELEM_MARKER_COLR           TelPolymarkerColour
#define CALL_PHIGS_ELEM_EDGE_COLR             TelEdgeColour
#define CALL_PHIGS_ELEM_LINE_COLR             TelPolylineColour
#define CALL_PHIGS_ELEM_INT_COLR              TelInteriorColour
#define CALL_PHIGS_ELEM_BACK_INT_COLR         TelBackInteriorColour
#define CALL_PHIGS_ELEM_REFL_PROPS            TelSurfaceAreaProperties
#define CALL_PHIGS_ELEM_BACK_REFL_PROPS       TelBackSurfaceAreaProperties
#define CALL_PHIGS_ELEM_INT_SHAD_METH         TelInteriorShadingMethod
#define CALL_PHIGS_ELEM_INT_REFL_EQN          TelInteriorReflectanceEquation
#define CALL_PHIGS_ELEM_BACK_INT_REFL_EQN     TelBackInteriorReflectanceEquation
#define CALL_PHIGS_ELEM_FACE_DISTING_MODE     TelFaceDistinguishingMode
#define CALL_PHIGS_ELEM_FACE_CULL_MODE        TelFaceCullingMode
#define CALL_PHIGS_ELEM_LIGHT_SRC_STATE       TelLightSrcState
#define CALL_PHIGS_ELEM_DCUE_IND              TelDepthCueIndex
#define CALL_PHIGS_ELEM_HIGHLIGHT_INDEX       TelHighlightIndex
#define CALL_PHIGS_ELEM_FILL_AREA3            TelPolygon
#define CALL_PHIGS_ELEM_NDXPOLYGONS3_DATA     TelPolygonIndices
#define CALL_PHIGS_ELEM_FILL_AREA_SET3        TelPolygonHoles
#endif
