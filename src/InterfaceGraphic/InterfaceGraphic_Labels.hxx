/*
 * 24/01/00 EUG : G003 add Structure_LABDegenerateModel value
*/

#ifndef LABELS_HXX_INCLUDED
#define LABELS_HXX_INCLUDED

/* gestion des labels */

/* structure, begin */
#define Structure_LABBegin	0

/* structure, transformation */
#define Structure_LABTransformation	1

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
/*#define Structure_LABTransPersistenceFlag  2*/
#define Structure_LABTransPersistence  2
/*#define Structure_LABTransPersistencePoint 3*/
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

/* structure, attributes */
#define Structure_LABContextLine	11
#define Structure_LABContextFillArea	12
#define Structure_LABContextMarker	13
#define Structure_LABContextText	14

/* structure, highlight, visibility, pick */
#define Structure_LABHighlight	21
#define Structure_LABVisibility	22
#define Structure_LABPick	23
#define Structure_LABNameSet	31

/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */
/*#define Structure_LABTransPersistence	32*/
/* ABD 29/10/04  Transform Persistence of Presentation( pan, zoom, rotate ) */

/* structure, hierarchy */
#define Structure_LABConnect	33

/* structure degenerate model */
#define Structure_LABDegenerateModel 34

/* view, index */
#define View_LABViewIndex	10

/* view, context */
#define View_LABViewContext	15

/* view, hlhsr */
#define View_LABHlhsr		20

/* view, light source management */
#define View_LABLight		25

/* view, clip plane management */
#define View_LABPlane		30

/* view, aliasing management */
#define View_LABAliasing	35

/* view, depth cueing management */
#define View_LABDepthCueing	40

/* view, priority management */
#define View_LABPriority10	100
#define View_LABPriority09	99
#define View_LABPriority08	98
#define View_LABPriority07	97
#define View_LABPriority06	96
#define View_LABPriority05	95
#define View_LABPriority04	94
#define View_LABPriority03	93
#define View_LABPriority02	92
#define View_LABPriority01	91
#define View_LABPriority00	90

/* view, end */
#define View_LABEnd		200

/* view, transient emulation */
#define View_LABImmediat1	301
#define View_LABImmediat2	302

#endif
