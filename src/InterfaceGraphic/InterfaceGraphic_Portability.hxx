
#ifndef InterfaceGraphic_PortabilityHeader
#define InterfaceGraphic_PortabilityHeader

/* 
  
                     Copyright (C) 1991,1992,1993 by
  
                      MATRA DATAVISION, FRANCE
  
This software is furnished in accordance with the terms and conditions
of the contract and with the inclusion of the above copyright notice.
This software or any other copy thereof may not be provided or otherwise
be made available to any other person. No title to an ownership of the
software is hereby transferred.
  
At the termination of the contract, the software and all copies of this
software must be deleted.
  
Facility : CAS-CADE V1
  
*/ 

/* structures pour portabilite des differents Phigs */
/* SUNPHIGS, DECPHIGS, TGSPHIGS, G5GPHIGS, PEX */

/***********************************************************************/
/******************************** CONST ********************************/
/***********************************************************************/

#define CALL_DEF_ENABLE_MODE_PAINTERS	0
#define CALL_DEF_ENABLE_MODE_ZBUFFER	1
#define CALL_DEF_DISABLE_MODE_ZBUFFER	-1
#define CALL_DEF_ENABLE_ZBUFFER		2
#define CALL_DEF_DISABLE_ZBUFFER	-2
#define CALL_DEF_TEST_CALL	-99999

/***********************************************************************/
/********************************* DATA ********************************/
/***********************************************************************/

typedef struct {
	int size;
	char *data;
} CALL_DEF_DATA;

/***********************************************************************/
/********************************** 2D *********************************/
/***********************************************************************/

typedef struct {
	float x;
	float y;
} CALL_DEF_POINT2;

typedef struct {
	float x;
	float y;
} CALL_DEF_VECTOR2;

/***********************************************************************/
/********************************** 3D *********************************/
/***********************************************************************/

typedef struct {
	float x;
	float y;
	float z;
} CALL_DEF_POINT3;

typedef struct {
	float x;
	float y;
	float z;
} CALL_DEF_VECTOR3;

/***********************************************************************/
/********************************* VIEW ********************************/
/***********************************************************************/

typedef float CALL_DEF_MATRIX3[4][4];

typedef struct {
	float x_min;
	float x_max;
	float y_min;
	float y_max;
} CALL_DEF_LIMIT;

typedef struct {
	float x_min;
	float x_max;
	float y_min;
	float y_max;
	float z_min;
	float z_max;
} CALL_DEF_LIMIT3;

typedef struct {
	CALL_DEF_MATRIX3 ori_matrix;
	CALL_DEF_MATRIX3 map_matrix;
	CALL_DEF_LIMIT3 clip_limit;
	int xy_clip;
	int back_clip;
	int front_clip;

#ifdef TGSPHIGS
	int opacity;
	int back_colr;
	int outline;
	int outline_colr;
	int temporary;
	int active;
#endif
} CALL_DEF_VIEWREP3;

typedef struct {
	CALL_DEF_LIMIT win;
	CALL_DEF_LIMIT3 proj_vp;
	int proj_type;
	CALL_DEF_POINT3 proj_ref_point;
	float view_plane;

#if defined (SUNPHIGS) || defined (TGSPHIGS) || defined (G5GPHIGS) || defined (HPPHIGS) || defined (PEX5)
	float back_plane;
	float front_plane;
#endif
#ifdef DECPHIGS
	float front_plane;
	float back_plane;
#endif
} CALL_DEF_VIEWMAP3;

/***********************************************************************/
/********************************* LIGHT *******************************/
/***********************************************************************/

typedef struct {
	int num_ints;
	int *ints;
} CALL_DEF_INTLIST;

typedef struct {
	float r;
	float g;
	float b;
} CALL_DEF_RGB;

typedef struct {
	int colr_type;
	CALL_DEF_RGB colr;
} CALL_DEF_AMBLIGHTSRCREC;

typedef struct {
	int colr_type;
	CALL_DEF_RGB colr;
	CALL_DEF_VECTOR3 dir;
} CALL_DEF_DIRLIGHTSRCREC;

typedef struct {
	int colr_type;
	CALL_DEF_RGB colr;
	CALL_DEF_POINT3 pos;
	float coef[2];
} CALL_DEF_POSLIGHTSRCREC;

typedef struct {
	int colr_type;
	CALL_DEF_RGB colr;
	CALL_DEF_POINT3 pos;
	CALL_DEF_VECTOR3 dir;
	float exp;
	float coef[2];
	float angle;
} CALL_DEF_SPOTLIGHTSRCREC;

typedef union {
	CALL_DEF_AMBLIGHTSRCREC ambient;
	CALL_DEF_DIRLIGHTSRCREC directional;
	CALL_DEF_POSLIGHTSRCREC positional;
	CALL_DEF_SPOTLIGHTSRCREC spot;
} CALL_DEF_LIGHTSRCREC;

typedef struct {
	int type;
	CALL_DEF_LIGHTSRCREC rec;
} CALL_DEF_LIGHTSRCBUNDLE;

/***********************************************************************/
/**************************** CLIPPING PLANE ***************************/
/***********************************************************************/

typedef struct {
	float a;
	float b;
	float c;
	float d;
	CALL_DEF_POINT3 point;
	CALL_DEF_VECTOR3 norm;
} CALL_DEF_HALF_SPACE;

typedef struct {
	int num_half_spaces;
	CALL_DEF_HALF_SPACE *half_spaces;
} CALL_DEF_HALF_SPACE_LIST;

typedef struct {
	int op;
	CALL_DEF_HALF_SPACE_LIST half_spaces;
} CALL_DEF_PLANEBUNDLE;

/***********************************************************************/
/******************************** SCREEN *******************************/
/***********************************************************************/

typedef struct {
	int size_x;
	int size_y;
} CALL_DEF_INT_SIZE;

typedef struct {
	float size_x;
	float size_y;
} CALL_DEF_FLOAT_SIZE;

typedef struct {
	int dc_units;
	CALL_DEF_FLOAT_SIZE size_dc;
	CALL_DEF_INT_SIZE size_raster;
} CALL_DEF_DISP_SPACE_SIZE;

/***********************************************************************/
/********************************* TEXT ********************************/
/***********************************************************************/

typedef struct {
	int horizontal;
	int vertical;
} CALL_DEF_TEXT_ALIGN;

#endif /* InterfaceGraphic_PortabilityHeader */
