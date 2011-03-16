/***********************************************************************

FONCTION :
----------
Include OpenGl_context :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
05-08-97 : PCT ; support texture mapping
augmentation de la taille de CONTEXTFILLAREA_SIZE
car ajout de champs pour le texture mapping
23-12-97 : FMN ; Suppression TelBackInteriorStyle, TelBackInteriorStyleIndex
30-11-98 : FMN ; S3819 : Textes toujours visibles
22-01-04 : SAN ; OCC2934 Texture mapped fonts on WNT 
22-03-04 : SAN ; OCC4895 High-level interface for controlling polygon offsets

************************************************************************/

/*----------------------------------------------------------------------*/

#ifndef  OPENGL_CONTEXT_H
#define  OPENGL_CONTEXT_H

#define  CALL_DEF_CONTEXTLINE_SIZE    3

/* 22-03-04 : SAN ; OCC4895 TelPolygonOffset element added to fill area context (size++) */
#define  CALL_DEF_CONTEXTFILLAREA_SIZE    17
/* 22-03-04 : SAN ; OCC4895 TelPolygonOffset element added to fill area context (size++) */

#define  CALL_DEF_CONTEXTMARKER_SIZE    3


/* TelTextZoomable, TelTextAngle, TelTextAspect element added to text context (size++) */
#define  CALL_DEF_CONTEXTTEXT_SIZE    10

#endif
