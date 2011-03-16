#include <OpenGl_tgl_all.hxx>
#include <OpenGl_graduatedtrihedron.hxx>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Aspect.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#include <OpenGl_tgl_funcs.hxx>
#include <OpenGl_tgl_subrs.hxx>
#include <OpenGl_TextRender.hxx>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* Graduated trihedron data */
static int  nbWsIds = 0;                       /* Number of the views (size of the arrays below */
static int* WsIds = 0;                         /* The array contans indices of graduated trihedron data corresponding to the view ID */
static CALL_DEF_GRADUATEDTRIHEDRON** graduatedTrihedronData = 0; /* The array contains graduated trihedron data of all views */

/* Bounding box */
float xmin = 0.0f, ymin = 0.0f, zmin = 0.0f, xmax = 100.0f, ymax = 100.0f, zmax = 100.0f;

static void copyData(CALL_DEF_GRADUATEDTRIHEDRON* fromData, CALL_DEF_GRADUATEDTRIHEDRON* toData)
{
    int len;

    /* Names of axes */
    /* X-name */
    if (fromData->xname)
        len = strlen(fromData->xname) + 1;
    else 
        len = 1;
    toData->xname = (char*) malloc(len * sizeof(char));
    if (fromData->xname)
        strcpy(toData->xname, fromData->xname);
    else
        toData->xname[0] = '\0';
    /* Y-name */
    if (fromData->yname)
        len = strlen(fromData->yname) + 1;
    else 
        len = 1;
    toData->yname = (char*) malloc(len * sizeof(char));
    if (fromData->yname)
        strcpy(toData->yname, fromData->yname);
    else
        toData->yname[0] = '\0';
    /* Z-name */
    if (fromData->zname)
        len = strlen(fromData->zname) + 1;
    else 
        len = 1;
    toData->zname = (char*) malloc(len * sizeof(char));
    if (fromData->zname)
        strcpy(toData->zname, fromData->zname);
    else
        toData->zname[0] = '\0';
    /* Draw names */
    toData->xdrawname = fromData->xdrawname; 
    toData->ydrawname = fromData->ydrawname; 
    toData->zdrawname = fromData->zdrawname;
    /* Draw values */
    toData->xdrawvalues = fromData->xdrawvalues; 
    toData->ydrawvalues = fromData->ydrawvalues; 
    toData->zdrawvalues = fromData->zdrawvalues;
    /* Draw grid */
    toData->drawgrid = fromData->drawgrid;
    /* Draw axes */
    toData->drawaxes = fromData->drawaxes;
    /* Number of splits along axes */
    toData->nbx = fromData->nbx; 
    toData->nby = fromData->nby; 
    toData->nbz = fromData->nbz;
    /* Offset for drawing values */
    toData->xoffset = fromData->xoffset; 
    toData->yoffset = fromData->yoffset; 
    toData->zoffset = fromData->zoffset;
    /* Offset for drawing names of axes */
    toData->xaxisoffset = fromData->xaxisoffset; 
    toData->yaxisoffset = fromData->yaxisoffset; 
    toData->zaxisoffset = fromData->zaxisoffset;
    /* Draw tickmarks */
    toData->xdrawtickmarks = fromData->xdrawtickmarks; 
    toData->ydrawtickmarks = fromData->ydrawtickmarks; 
    toData->zdrawtickmarks = fromData->zdrawtickmarks;
    /* Length of tickmarks */
    toData->xtickmarklength = fromData->xtickmarklength; 
    toData->ytickmarklength = fromData->ytickmarklength; 
    toData->ztickmarklength = fromData->ztickmarklength;
    /* Grid color */
    memcpy(toData->gridcolor, fromData->gridcolor, 3 * sizeof(float));
    /* X name color */
    memcpy(toData->xnamecolor, fromData->xnamecolor, 3 * sizeof(float));
    /* Y name color */
    memcpy(toData->ynamecolor, fromData->ynamecolor, 3 * sizeof(float));
    /* Z name color */
    memcpy(toData->znamecolor, fromData->znamecolor, 3 * sizeof(float));
    /* X color of axis and values */
    memcpy(toData->xcolor, fromData->xcolor, 3 * sizeof(float));
    /* Y color of axis and values */
    memcpy(toData->ycolor, fromData->ycolor, 3 * sizeof(float));
    /* Z color of axis and values */
    memcpy(toData->zcolor, fromData->zcolor, 3 * sizeof(float));
    /* Font name of names of axes: Courier, Arial, ... */
    if (fromData->fontOfNames)
        len = strlen(fromData->fontOfNames) + 1;
    else 
        len = 1;
    toData->fontOfNames = (char*) malloc(len * sizeof(char));
    if (fromData->fontOfNames)
        strcpy(toData->fontOfNames, fromData->fontOfNames);
    else
        toData->fontOfNames[0] = '\0';
    /* Style of names of axes: OSD_FA_Regular, OSD_FA_Bold, ... */
    toData->styleOfNames = fromData->styleOfNames;
    /* Size of names of axes: 8, 10, 12, 14, ... */
    toData->sizeOfNames = fromData->sizeOfNames;
    /* Font name of values: Courier, Arial, ... */
    if (fromData->fontOfValues)
        len = strlen(fromData->fontOfValues) + 1;
    else 
        len = 1;
    toData->fontOfValues = (char*) malloc(len * sizeof(char));
    if (fromData->fontOfValues)
        strcpy(toData->fontOfValues, fromData->fontOfValues);
    else
        toData->fontOfValues[0] = '\0';
    /* Style of values: OSD_FA_Regular, OSD_FA_Bold, ... */
    toData->styleOfValues = fromData->styleOfValues;
    /* Size of values: 8, 10, 12, 14, ... */
    toData->sizeOfValues = fromData->sizeOfValues;
    /* Callback - updater of boundary box */
    toData->cbCubicAxes = fromData->cbCubicAxes;
    toData->ptrVisual3dView = fromData->ptrVisual3dView;
}

static void freeGraduatedTrihedronData(CALL_DEF_GRADUATEDTRIHEDRON* data)
{
    /* Names of axes */
    if (data->xname)
        free(data->xname); 
    if (data->yname)
        free(data->yname); 
    if (data->zname)
        free(data->zname); 

    /* Fonts */
    if (data->fontOfNames)
        free(data->fontOfNames); 
    if (data->fontOfValues)
        free(data->fontOfValues); 

    free(data);
}

static void freeData()
{
    int i;

    if (WsIds)
    {
        free(WsIds);
        for (i = 0; i < nbWsIds; i++)
        {
            freeGraduatedTrihedronData(graduatedTrihedronData[i]);
        }
        free(graduatedTrihedronData);

        WsIds = 0;
        graduatedTrihedronData = 0;
    }
}

static int getGraduatedTrihedronDataIndex(int WsId)
{
    int i = 0;
    
    if (!nbWsIds)
        return -1;
    
    for (; i < nbWsIds; i++)
    {
        if (WsIds[i] == WsId)
            return i;
    }

    return -1;
}

static unsigned char initView(int WsId)
{
    int i;
    int* newWsIds;
    CALL_DEF_GRADUATEDTRIHEDRON** newGraduatedTrihedronData;

    /* Extend arrays for +1 */
    nbWsIds++;
    newWsIds = (int*) calloc(nbWsIds, sizeof(int));
    newGraduatedTrihedronData = (CALL_DEF_GRADUATEDTRIHEDRON**) calloc(nbWsIds, sizeof(CALL_DEF_GRADUATEDTRIHEDRON*));
    for (i = 0; i < nbWsIds; i++)
    {
        newGraduatedTrihedronData[i] = (CALL_DEF_GRADUATEDTRIHEDRON*) calloc(1, sizeof(CALL_DEF_GRADUATEDTRIHEDRON));
    }

    /* Copy data from current arrays to the newly created */
    if (nbWsIds > 1)
    {
        for (i = 0; i < nbWsIds - 1; i++)
        {
            newWsIds[i] = WsIds[i];
            copyData(graduatedTrihedronData[i], newGraduatedTrihedronData[i]);
        }
    }

    /* Delete the previous used arrays */
    nbWsIds--; /* Don't delete just created graduated trihedron data */
    freeData();
    nbWsIds++; /* Return the counter back */

    /* Switch to new arrays */
    WsIds = newWsIds;
    graduatedTrihedronData = newGraduatedTrihedronData;

    /* Set view ID */
    WsIds[nbWsIds - 1] = WsId;

    return TSuccess;
}

/* Erases the trihedron from the view */
static TStatus removeView(int WsId)
{
    int index, i, j;
    int* newWsIds;
    CALL_DEF_GRADUATEDTRIHEDRON** newGraduatedTrihedronData;

    index = getGraduatedTrihedronDataIndex(WsId);
    if (index == -1)
        return TSuccess; /* Nothing to remove */

    /* If trihedron is displayed only in one view,
       just free the arrays and set nbWsIds equal to 0. */
    if (nbWsIds == 1)
    {
        freeData();
        nbWsIds = 0;
        return TSuccess;
    }

    /* create new arrays with nbWsIds - 1 length. */
    nbWsIds--;
    newWsIds = (int*) calloc(nbWsIds, sizeof(int));
    newGraduatedTrihedronData = (CALL_DEF_GRADUATEDTRIHEDRON**) calloc(nbWsIds, sizeof(CALL_DEF_GRADUATEDTRIHEDRON*));
    for (i = 0; i < nbWsIds; i++)
    {
        newGraduatedTrihedronData[i] = (CALL_DEF_GRADUATEDTRIHEDRON*) calloc(1, sizeof(CALL_DEF_GRADUATEDTRIHEDRON));
    }

    /* Copy data from current arrays to the newly created */
    for (i = 0, j = 0; j <= nbWsIds; j++)
    {
        if (j != index)
        {
            newWsIds[i] = WsIds[j];
            copyData(graduatedTrihedronData[j], newGraduatedTrihedronData[i]);
            i++;
        }
    }

    return TSuccess;
}

/* Normal of the view (not normalized!) */
static float getNormal(float* normal) 
{
	GLint viewport[4];
	GLdouble model_matrix[16], proj_matrix[16];
	double x1, y1, z1, x2, y2, z2, x3, y3, z3;
    double dx1, dy1, dz1, dx2, dy2, dz2, width;

	glGetDoublev(GL_MODELVIEW_MATRIX,  model_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, proj_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluUnProject(viewport[0], viewport[1], 0., model_matrix, proj_matrix, viewport, &x1, &y1, &z1);
	gluUnProject(viewport[0] + viewport[2], viewport[1], 0., model_matrix, proj_matrix, viewport, &x2, &y2, &z2);
	gluUnProject(viewport[0], viewport[1] + viewport[3], 0., model_matrix, proj_matrix, viewport, &x3, &y3, &z3);

    /* Normal out of user is p1p3^p1p2 */
    dx1 = x3 - x1; dy1 = y3 - y1; dz1 = z3 - z1;
    dx2 = x2 - x1; dy2 = y2 - y1; dz2 = z2 - z1;
    normal[0] = (float) (dy1 * dz2 - dz1 * dy2);
	normal[1] = (float) (dz1 * dx2 - dx1 * dz2);
	normal[2] = (float) (dx1 * dy2 - dy1 * dx2);

    /* Distance corresponding to 1 pixel */
    width = sqrt(dx2 * dx2 + dy2 * dy2 + dz2 * dz2);
    return (float) width / (float) viewport[2];
}

static float getDistance2Corner(float* normal, float* center, float x, float y, float z)
{
    return normal[0] * (x - center[0]) + normal[1] * (y - center[1]) + normal[2] * (z - center[2]);
}

static char getFarestCorner(float d000, float d100, float d010, float d001, 
                            float d110, float d101, float d011, float d111)
{
    if (d000 > 0.0f && 
        d000 > d100 && d000 > d010 && d000 > d001 && d000 > d110 && 
        d000 > d101 && d000 > d011 && d000 > d111)
    {
        return 1;
    }
    else if (d100 > 0.0f &&
             d100 > d000 && d100 > d010 && d100 > d001 && d100 > d110 && 
             d100 > d101 && d100 > d011 && d100 > d111)
    {
        return 2;
    }
    else if (d010 > 0.0f &&
             d010 > d000 && d010 > d100 && d010 > d001 && d010 > d110 && 
             d010 > d101 && d010 > d011 && d010 > d111)
    {
        return 3;
    }
    else if (d001 > 0.0f &&
             d001 > d000 && d001 > d100 && d001 > d010 && d001 > d110 && 
             d001 > d101 && d001 > d011 && d001 > d111)
    {
        return 4;
    }
    else if (d110 > 0.0f &&
             d110 > d000 && d110 > d100 && d110 > d010 && d110 > d001 && 
             d110 > d101 && d110 > d011 && d110 > d111)
    {
        return 5;
    }
    else if (d101 > 0.0f &&
             d101 > d000 && d101 > d100 && d101 > d010 && d101 > d001 && 
             d101 > d110 && d101 > d011 && d101 > d111)
    {
        return 6;
    }
    else if (d011 > 0.0f &&
             d011 > d000 && d011 > d100 && d011 > d010 && d011 > d001 && 
             d011 > d110 && d011 > d101 && d011 > d111)
    {
        return 7;
    }
    return 8; /* d111 */
}

static void drawText(char* text, char* font, OSD_FontAspect style, int size, float x, float y, float z)
{
    GLuint fontBase = 0;
    OpenGl_TextRender* textRenderer = OpenGl_TextRender::instance();
    fontBase = textRenderer->FindFont((Tchar*) font, style, (float) size);
    textRenderer->RenderText(text, fontBase, 0, x, y, z);

/* 4 OCC 6.3.1 and older:
    GLuint fontBase;

#ifndef WNT
     fontBase = tXfmsetfont (1.0F, 1.0F);
#else
     fontBase = WNTSetFont (1.0F, 1.0F);
#endif

#ifndef WNT
     tXfmprstr(text, fontBase, x, y, z);
#else
     WNTPuts(text, fontBase, 0, x, y, z);
#endif
*/
}

static void drawArrow(float x1, float y1, float z1, 
                      float x2, float y2, float z2,
                      float xn, float yn, float zn)
{
    float h, r;
    float xa, ya, za;
    float x0, y0, z0;
    float xr, yr, zr;
    float xa1, ya1, za1, xa2, ya2, za2;

    /* Start of arrow: at 10% from the end */
    x0 = x1 + 0.9f * (x2 - x1); y0 = y1 + 0.9f * (y2 - y1); z0 = z1 + 0.9f * (z2 - z1);

    /* Base of the arrow */
    xa = (x2 - x0); ya = (y2 - y0); za = (z2 - z0);

    /* Height of the arrow */
    h = sqrtf(xa * xa + ya * ya + za * za);
    if (h <= 0.0f)
        return;
    xa = xa / h; ya = ya / h; za = za / h;

    /* Radial direction to the arrow */
    xr = ya * zn - za * yn;
    yr = za * xn - xa * zn;
    zr = xa * yn - ya * xn;

    /* Normalize the radial vector */
    r = sqrtf(xr * xr + yr * yr + zr * zr);
    if (r <= 0.0f)
        return;
    xr = xr / r; yr = yr / r; zr = zr / r;

    /* First point of the base of the arrow */
    r = 0.2f * h;
    xr = r * xr; yr = r * yr; zr = r * zr;
    xa1 = x0 + xr; ya1 = y0 + yr; za1 = z0 + zr;

    /* Second point of the base of the arrow */
    xa2 = x0 - xr; ya2 = y0 - yr; za2 = z0 - zr;

    /* Draw a line to the arrow */
    glBegin(GL_LINES);
        glVertex3f(x1, y1, z1);
        glVertex3f(x0, y0, z0);
    glEnd();

    /* Draw a triangle of the arrow */
    glBegin(GL_LINE_LOOP);
        glVertex3f(xa1, ya1, za1);
        glVertex3f(xa2, ya2, za2);
        glVertex3f(x2,  y2,  z2);
    glEnd();
}

TStatus call_graduatedtrihedron_get(int WsId, CALL_DEF_GRADUATEDTRIHEDRON* data)
{
    int index;

    /* Get index of the view */
    index = getGraduatedTrihedronDataIndex(WsId);
    if (index == -1)
    {
        /* Default values */
        data->xname = "X\0";
        data->yname = "Y\0"; 
        data->zname = "Z\0";
        data->xdrawname = 1;
        data->ydrawname = 1; 
        data->zdrawname = 1;
        data->xdrawvalues = 1;
        data->ydrawvalues = 1; 
        data->zdrawvalues = 1;
        data->drawgrid = 1;
        data->drawaxes = 1;
        data->nbx = 3;
        data->nby = 3; 
        data->nbz = 3;
        data->xoffset = 10;
        data->yoffset = 10; 
        data->zoffset = 10;
        data->xaxisoffset = 30;
        data->yaxisoffset = 30; 
        data->zaxisoffset = 30;
        data->xdrawtickmarks = 1;
        data->ydrawtickmarks = 1; 
        data->zdrawtickmarks = 1;
        data->xtickmarklength = 10;
        data->ytickmarklength = 10; 
        data->ztickmarklength = 10;
        /*Quantity_NOC_WHITE*/;
        data->gridcolor[0] = 1.0f;
        data->gridcolor[1] = 1.0f;
        data->gridcolor[2] = 1.0f;
        /* Quantity_NOC_RED */
        data->xnamecolor[0] = 1.0f;
        data->xnamecolor[1] = 0.0f;
        data->xnamecolor[2] = 0.0f;
        /* Quantity_NOC_GREEN */
        data->ynamecolor[0] = 0.0f;
        data->ynamecolor[1] = 1.0f;
        data->ynamecolor[2] = 0.0f;
        /* Quantity_NOC_BLUE1 */
        data->znamecolor[0] = 0.0f;
        data->znamecolor[1] = 0.0f;
        data->znamecolor[2] = 1.0f;
        /* Quantity_NOC_RED */
        data->xcolor[0] = 1.0f;
        data->xcolor[1] = 0.0f;
        data->xcolor[2] = 0.0f;
        /* Quantity_NOC_GREEN */
        data->ycolor[0] = 0.0f;
        data->ycolor[1] = 1.0f;
        data->ycolor[2] = 0.0f;
        /* Quantity_NOC_BLUE1 */
        data->zcolor[0] = 0.0f;
        data->zcolor[1] = 0.0f;
        data->zcolor[2] = 1.0f;
    }
    else
    {
        copyData(graduatedTrihedronData[index], data);
    }
    return TSuccess;
}

TStatus call_graduatedtrihedron_display(int WsId, CALL_DEF_GRADUATEDTRIHEDRON* data)
{
    int index;

    /* Initialize data for a new view */
    index = getGraduatedTrihedronDataIndex(WsId);
    if (index == -1)
    {
        initView(WsId);
        index = getGraduatedTrihedronDataIndex(WsId);
    }
    copyData(data, graduatedTrihedronData[index]);
    return call_graduatedtrihedron_redraw(WsId);
}

TStatus call_graduatedtrihedron_erase(int WsId)
{
    return removeView(WsId);
}

TStatus call_graduatedtrihedron_redraw(int WsId)
{
    GLboolean light;
    unsigned int i, offset;
    unsigned char farestCorner;
    float normal[3], center[3];
    CALL_DEF_GRADUATEDTRIHEDRON* data;
    float d000, d100, d010, d001, d110, d101, d011, d111; /* 0 - min, 1 - max */
    float LX1[6], LX2[6], LX3[6]; /* Lines along X direction */
    float LY1[6], LY2[6], LY3[6]; /* Lines along Y direction */
    float LZ1[6], LZ2[6], LZ3[6]; /* Lines along Z direction */
    unsigned char LX1draw, LX2draw, LX3draw; /* Allows drawing of X-line (000 - 100 is forbidden) */
    unsigned char LY1draw, LY2draw, LY3draw; /* Allows drawing of Y-line (000 - 010 is forbidden) */
    unsigned char LZ1draw, LZ2draw, LZ3draw; /* Allows drawing of Z-line (000 - 001 is forbidden) */
    float m1[3], m2[3];
    float step, d, dpix, dx, dy, dz;
    char textValue[128];
    int index;

    /* Get index of the trihedron data */
    index = getGraduatedTrihedronDataIndex(WsId);
    if (index == -1)
        return TFailure;
    data = graduatedTrihedronData[index];

    /* Update boundary box */
    if (data->cbCubicAxes)
        data->cbCubicAxes(data->ptrVisual3dView);

    /* Disable lighting for lines */
    light = glIsEnabled(GL_LIGHTING);
    if (light)
        glDisable(GL_LIGHTING);
    
    /* Find the farest point of bounding box */

    /* Get normal of the view out of user. */
    /* Also, the method return distance corresponding to 1 pixel */
    dpix = getNormal(normal);
    
    /* Normalize normal */
    d = sqrtf(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    normal[0] = normal[0] / d;
    normal[1] = normal[1] / d;
    normal[2] = normal[2] / d;

    /* Get central point of bounding box */
    center[0] = 0.5f * (xmin + xmax);
    center[1] = 0.5f * (ymin + ymax);
    center[2] = 0.5f * (zmin + zmax);

    /* Check distance to corners of bounding box along the normal*/
    d000 = getDistance2Corner(normal, center, xmin, ymin, zmin);
    d100 = getDistance2Corner(normal, center, xmax, ymin, zmin);
    d010 = getDistance2Corner(normal, center, xmin, ymax, zmin);
    d001 = getDistance2Corner(normal, center, xmin, ymin, zmax);
    d110 = getDistance2Corner(normal, center, xmax, ymax, zmin);
    d101 = getDistance2Corner(normal, center, xmax, ymin, zmax);
    d011 = getDistance2Corner(normal, center, xmin, ymax, zmax);
    d111 = getDistance2Corner(normal, center, xmax, ymax, zmax);
    farestCorner = getFarestCorner(d000, d100, d010, d001, d110, d101, d011, d111);

    /* Choose axes for the grid. */
    /* The first axis will be used for drawing the text and the values. */
    switch (farestCorner)
    {
        case 1: /* d000 */
        {
            /* 001 - 101 */
            LX1draw = 1;
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
            /* 000 - 100 */
            LX2draw = 0; /* forbidden! */
            LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmin;
            /* 010 - 110 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

            /* 100 - 110 */
            LY1draw = 1;
            LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
            /* 000 - 010 */
            LY2draw = 0; /* forbidden! */
            LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmin;
            /* 001 - 011 */
            LY3draw = 1;
            LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

            /* 100 - 101 */
            LZ1draw = 1;
            LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 000 - 001 */
            LZ2draw = 0; /* forbidden! */
            LZ2[0] = xmin; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymin; LZ2[5] = zmax;
            /* 010 - 011 */
            LZ3draw = 1;
            LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 2: /* d100 */
        {
            /* 001 - 101 */
            LX1draw = 1;
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
            /* 000 - 100 */
            LX2draw = 0; /* forbidden! */
            LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmin;
            /* 010 - 110 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

            /* 000 - 010 */
            LY1draw = 0; /* forbidden! */
            LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
            /* 100 - 110 */
            LY2draw = 1;
            LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmin;
            /* 101 - 111 */
            LY3draw = 1;
            LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

            /* 000 - 001 */
            LZ1draw = 0; /* forbidden! */
            LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 100 - 101 */
            LZ2draw = 1;
            LZ2[0] = xmax; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymin; LZ2[5] = zmax;
            /* 110 - 111 */
            LZ3draw = 1;
            LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 3: /* d010 */
        {
            /* 000 - 100 */
            LX1draw = 0; /* forbidden */
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
            /* 010 - 110 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmin;
            /* 011 - 111 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

            /* 100 - 110 */
            LY1draw = 1;
            LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
            /* 000 - 010 */
            LY2draw = 0; /* forbidden */
            LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmin;
            /* 001 - 011 */
            LY3draw = 1;
            LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

            /* 110 - 111 */
            LZ1draw = 1;
            LZ1[0] = xmax; LZ1[1] = ymax; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymax; LZ1[5] = zmax;
            /* 010 - 011 */
            LZ2draw = 1;
            LZ2[0] = xmin; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymax; LZ2[5] = zmax;
            /* 000 - 001 */
            LZ3draw = 0; /* forbidden */
            LZ3[0] = xmin; LZ3[1] = ymin; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymin; LZ3[5] = zmax;

            break;
        }
        case 4: /* d001 */
        {
            /* 000 - 100 */
            LX1draw = 0; /* forbidden */
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
            /* 001 - 101 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmax;
            /* 011 - 111 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

            /* 000 - 010 */
            LY1draw = 0; /* forbidden */
            LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
            /* 001 - 011 */
            LY2draw = 1;
            LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmax;
            /* 101 - 111 */
            LY3draw = 1;
            LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

            /* 100 - 101 */
            LZ1draw = 1;
            LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 000 - 001 */
            LZ2draw = 0; /* forbidden */
            LZ2[0] = xmin; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymin; LZ2[5] = zmax;
            /* 010 - 011 */
            LZ3draw = 1;
            LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 5: /* d110 */
        {
            /* 000 - 100 */
            LX1draw = 0; /* forbidden */
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
            /* 010 - 110 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmin; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmin;
            /* 011 - 111 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

            /* 000 - 010 */
            LY1draw = 0; /* forbidden */
            LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
            /* 100 - 110 */
            LY2draw = 1;
            LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmin; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmin;
            /* 101 - 111 */
            LY3draw = 1;
            LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

            /* 100 - 101 */
            LZ1draw = 1;
            LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 110 - 111 */
            LZ2draw = 1;
            LZ2[0] = xmax; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymax; LZ2[5] = zmax;
            /* 010 - 011 */
            LZ3draw = 1;
            LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 6: /* d101 */
        {
            /* 000 - 100 */
            LX1draw = 0; /* forbidden */
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmin;
            /* 001 - 101 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymin; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymin; LX2[5] = zmax;
            /* 011 - 111 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmax;

            /* 100 - 110 */
            LY1draw = 1;
            LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
            /* 101 - 111 */
            LY2draw = 1;
            LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmax;
            /* 001 - 011 */
            LY3draw = 1;
            LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

            /* 000 - 001 */
            LZ1draw = 0; /* forbidden */
            LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 100 - 101 */
            LZ2draw = 1;
            LZ2[0] = xmax; LZ2[1] = ymin; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymin; LZ2[5] = zmax;
            /* 110 - 111 */
            LZ3draw = 1;
            LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 7: /* d011 */
        {
            /* 001 - 101 */
            LX1draw = 1;
            LX1[0] = xmin; LX1[1] = ymin; LX1[2] = zmax; LX1[3] = xmax; LX1[4] = ymin; LX1[5] = zmax;
            /* 011 - 111 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmax;
            /* 010 - 110 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymax; LX3[2] = zmin; LX3[3] = xmax; LX3[4] = ymax; LX3[5] = zmin;

            /* 000 - 010 */
            LY1draw = 0; /* forbidden */
            LY1[0] = xmin; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmin; LY1[4] = ymax; LY1[5] = zmin;
            /* 001 - 011 */
            LY2draw = 1;
            LY2[0] = xmin; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmin; LY2[4] = ymax; LY2[5] = zmax;
            /* 101 - 111 */
            LY3draw = 1;
            LY3[0] = xmax; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmax; LY3[4] = ymax; LY3[5] = zmax;

            /* 000 - 001 */
            LZ1draw = 0; /* forbidden */
            LZ1[0] = xmin; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmin; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 010 - 011 */
            LZ2draw = 1;
            LZ2[0] = xmin; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmin; LZ2[4] = ymax; LZ2[5] = zmax;
            /* 110 - 111 */
            LZ3draw = 1;
            LZ3[0] = xmax; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmax; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
        case 8: /* d111 */
        {
            /* 010 - 110 */
            LX1draw = 1;
            LX1[0] = xmin; LX1[1] = ymax; LX1[2] = zmin; LX1[3] = xmax; LX1[4] = ymax; LX1[5] = zmin;
            /* 011 - 111 */
            LX2draw = 1;
            LX2[0] = xmin; LX2[1] = ymax; LX2[2] = zmax; LX2[3] = xmax; LX2[4] = ymax; LX2[5] = zmax;
            /* 001 - 101 */
            LX3draw = 1;
            LX3[0] = xmin; LX3[1] = ymin; LX3[2] = zmax; LX3[3] = xmax; LX3[4] = ymin; LX3[5] = zmax;

            /* 100 - 110 */
            LY1draw = 1;
            LY1[0] = xmax; LY1[1] = ymin; LY1[2] = zmin; LY1[3] = xmax; LY1[4] = ymax; LY1[5] = zmin;
            /* 101 - 111 */
            LY2draw = 1;
            LY2[0] = xmax; LY2[1] = ymin; LY2[2] = zmax; LY2[3] = xmax; LY2[4] = ymax; LY2[5] = zmax;
            /* 001 - 011 */
            LY3draw = 1;
            LY3[0] = xmin; LY3[1] = ymin; LY3[2] = zmax; LY3[3] = xmin; LY3[4] = ymax; LY3[5] = zmax;

            /* 100 - 101 */
            LZ1draw = 1;
            LZ1[0] = xmax; LZ1[1] = ymin; LZ1[2] = zmin; LZ1[3] = xmax; LZ1[4] = ymin; LZ1[5] = zmax;
            /* 110 - 111 */
            LZ2draw = 1;
            LZ2[0] = xmax; LZ2[1] = ymax; LZ2[2] = zmin; LZ2[3] = xmax; LZ2[4] = ymax; LZ2[5] = zmax;
            /* 010 - 011 */
            LZ3draw = 1;
            LZ3[0] = xmin; LZ3[1] = ymax; LZ3[2] = zmin; LZ3[3] = xmin; LZ3[4] = ymax; LZ3[5] = zmax;

            break;
        }
    }

    /* Draw the graduated trihedron */

    /* Grid */
    if (data->drawgrid)
    {
        glColor3fv(data->gridcolor);
        glBegin(GL_LINES);
            /* Boundary grid-lines */
            if (LX1draw == 1)
            {
                glVertex3fv(&(LX1[0]));
                glVertex3fv(&(LX1[3]));
            }
            if (LX2draw == 1)
            {
                glVertex3fv(&(LX2[0]));
                glVertex3fv(&(LX2[3]));
            }
            if (LX3draw == 1)
            {
                glVertex3fv(&(LX3[0]));
                glVertex3fv(&(LX3[3]));
            }
            if (LY1draw == 1)
            {
                glVertex3fv(&(LY1[0]));
                glVertex3fv(&(LY1[3]));
            }
            if (LY2draw == 1)
            {
                glVertex3fv(&(LY2[0]));
                glVertex3fv(&(LY2[3]));
            }
            if (LY3draw == 1)
            {
                glVertex3fv(&(LY3[0]));
                glVertex3fv(&(LY3[3]));
            }
            if (LZ1draw == 1)
            {
                glVertex3fv(&(LZ1[0]));
                glVertex3fv(&(LZ1[3]));
            }
            if (LZ2draw == 1)
            {
                glVertex3fv(&(LZ2[0]));
                glVertex3fv(&(LZ2[3]));
            }
            if (LZ3draw == 1)
            {
                glVertex3fv(&(LZ3[0]));
                glVertex3fv(&(LZ3[3]));
            }
        glEnd();

        /* Intermediate grid-lines */
        /* X-Grid lines */
        if (data->nbx > 0)
        {
            i = data->drawaxes ? 1 : 0;
            step = fabsf(LX1[3] - LX1[0]) / (float) data->nbx;
            while (i < data->nbx)
            {
                glBegin(GL_LINE_STRIP);
                    glVertex3f(LX1[0] + i * step, LX1[1], LX1[2]);
                    glVertex3f(LX2[0] + i * step, LX2[1], LX2[2]);
                    glVertex3f(LX3[0] + i * step, LX3[1], LX3[2]);
                glEnd();
                i = i + 1;
            }
        }
        /* Y-Grid lines */
        if (data->nby > 0)
        {
            i = data->drawaxes ? 1 : 0;
            step = fabsf(LY1[4] - LY1[1]) / (float) data->nby;
            while (i < data->nby)
            {
                glBegin(GL_LINE_STRIP);
                    glVertex3f(LY1[0], LY1[1] + i * step, LY1[2]);
                    glVertex3f(LY2[0], LY2[1] + i * step, LY2[2]);
                    glVertex3f(LY3[0], LY3[1] + i * step, LY3[2]);
                glEnd();
                i = i + 1;
            }
        }
        /* Z-Grid lines */
        if (data->nbz > 0)
        {
            i = data->drawaxes ? 1 : 0;
            step = fabsf(LZ1[5] - LZ1[2]) / (float) data->nbz;
            while (i < data->nbz)
            {
                glBegin(GL_LINE_STRIP);
                    glVertex3f(LZ1[0], LZ1[1], LZ1[2] + i * step);
                    glVertex3f(LZ2[0], LZ2[1], LZ2[2] + i * step);
                    glVertex3f(LZ3[0], LZ3[1], LZ3[2] + i * step);
                glEnd();
                i = i + 1;
            }
        }
    }

    /* Axes (arrows) */
    if (data->drawaxes)
    {
        /* X-axis */
        glColor3fv(data->xcolor);
        drawArrow(xmin, ymin, zmin, xmax, ymin, zmin, normal[0], normal[1], normal[2]);
    
        /* Y-axis */
        glColor3fv(data->ycolor);
        drawArrow(xmin, ymin, zmin, xmin, ymax, zmin, normal[0], normal[1], normal[2]);
    
        /* Z-axis */
        glColor3fv(data->zcolor);
        drawArrow(xmin, ymin, zmin, xmin, ymin, zmax, normal[0], normal[1], normal[2]);
    }
    /* Names of axes & values*/

    if (data->xdrawname || data->xdrawvalues)
    {
        /* Middle point of the first X-axis */
        m1[0] = 0.5f * (LX1[0] + LX1[3]);
        m1[1] = 0.5f * (LX1[1] + LX1[4]);
        m1[2] = 0.5f * (LX1[2] + LX1[5]);

        /* Middle point of the second X-axis */
        m2[0] = 0.5f * (LX2[0] + LX2[3]);
        m2[1] = 0.5f * (LX2[1] + LX2[4]);
        m2[2] = 0.5f * (LX2[2] + LX2[5]);

        /* Apply offset to m1 */
        dy = m1[1] - m2[1];
        if (fabsf(dy) > 1.e-7f)
        {
            if (dy > 0.0f)
                dy = 1.0f;
            else 
                dy = -1.0f;
        }
        dz = m1[2] - m2[2];
        if (fabsf(dz) > 1.e-7f)
        {
            if (dz > 0.0f)
                dz = 1.0f;
            else
                dz = -1.0f;
        }

        m2[1] = dpix * dy;
        m2[2] = dpix * dz;

        /* Name of X-axis */
        if (data->xdrawname)
        {
            glColor3fv(data->xnamecolor);
            offset = data->xaxisoffset + data->xtickmarklength;
            drawText(data->xname, data->fontOfNames, data->styleOfNames, data->sizeOfNames, 
                     m1[0], m1[1] + offset * m2[1], m1[2] + offset * m2[2]);
        }

        /* X-values */
        if (data->xdrawvalues && data->nbx > 0)
        {
            glColor3fv(data->xcolor);

            i = 0;
            step = fabsf(LX1[3] - LX1[0]) / (float) data->nbx;
            offset = data->xoffset + data->xtickmarklength;
            while (i <= data->nbx)
            {
                sprintf(textValue, "%g", LX1[0] + i * step);
                drawText(textValue, data->fontOfValues, data->styleOfValues, data->sizeOfValues, 
                         LX1[0] + i * step, m1[1] + offset * m2[1], m1[2] + offset * m2[2]);
                i = i + 1;
            }
        }

        /* X-tickmark */
        if (data->xdrawtickmarks && data->nbx > 0)
        {
            glColor3fv(data->gridcolor);

            i = 0;
            step = fabsf(LX1[3] - LX1[0]) / (float) data->nbx;
            while (i <= data->nbx)
            {
                glBegin(GL_LINES);
                    glVertex3f(LX1[0] + i * step, m1[1],                                 m1[2]);
                    glVertex3f(LX1[0] + i * step, m1[1] + data->xtickmarklength * m2[1], m1[2] + data->xtickmarklength * m2[2]);
                glEnd();
                i = i + 1;
            }
        }
    }

    if (data->ydrawname || data->ydrawvalues)
    {
        /* Middle point of the first Y-axis */
        m1[0] = 0.5f * (LY1[0] + LY1[3]);
        m1[1] = 0.5f * (LY1[1] + LY1[4]);
        m1[2] = 0.5f * (LY1[2] + LY1[5]);

        /* Middle point of the second Y-axis */
        m2[0] = 0.5f * (LY2[0] + LY2[3]);
        m2[1] = 0.5f * (LY2[1] + LY2[4]);
        m2[2] = 0.5f * (LY2[2] + LY2[5]);

        /* Apply offset to m1 */
        dx = m1[0] - m2[0];
        if (fabsf(dx) > 1.e-7f)
        {
            if (dx > 0.0f)
                dx = 1.0f;
            else
                dx = -1.0f;
        }
        dz = m1[2] - m2[2];
        if (fabsf(dz) > 1.e-7f)
        {
            if (dz > 0.0f)
                dz = 1.0f;
            else
                dz = -1.0f;
        }

        m2[0] = dpix * dx;
        m2[2] = dpix * dz;

        /* Name of Y-axis */
        if (data->ydrawname)
        {
            glColor3fv(data->ynamecolor);
            offset = data->yaxisoffset + data->ytickmarklength;
            drawText(data->yname, data->fontOfNames, data->styleOfNames, data->sizeOfNames, 
                     m1[0] + offset * m2[0], m1[1], m1[2] + offset * m2[2]);
        }

        /* Y-values */
        if (data->ydrawvalues && data->nby > 0)
        {
            glColor3fv(data->ycolor);

            i = 0;
            step = fabsf(LY1[4] - LY1[1]) / (float) data->nby;
            offset = data->yoffset + data->ytickmarklength;
            while (i <= data->nby)
            {
                sprintf(textValue, "%g", LY1[1] + i * step);
                drawText(textValue, data->fontOfValues, data->styleOfValues, data->sizeOfValues, 
                         m1[0] + offset * m2[0], LY1[1] + i * step, m1[2] + offset * m2[2]);
                i = i + 1;
            }
        }

        /* Y-tickmark */
        if (data->ydrawtickmarks && data->nby > 0)
        {
            glColor3fv(data->gridcolor);

            i = 0;
            step = fabsf(LY1[4] - LY1[1]) / (float) data->nby;
            while (i <= data->nby)
            {
                glBegin(GL_LINES);
                    glVertex3f(m1[0],                                 LY1[1] + i * step, m1[2]);
                    glVertex3f(m1[0] + data->ytickmarklength * m2[0], LY1[1] + i * step, m1[2] + data->ytickmarklength * m2[2]);
                glEnd();
                i = i + 1;
            }
        }
    }

    if (data->zdrawname || data->zdrawvalues)
    {
        /* Middle point of the first Z-axis */
        m1[0] = 0.5f * (LZ1[0] + LZ1[3]);
        m1[1] = 0.5f * (LZ1[1] + LZ1[4]);
        m1[2] = 0.5f * (LZ1[2] + LZ1[5]);

        /* Middle point of the second Z-axis */
        m2[0] = 0.5f * (LZ2[0] + LZ2[3]);
        m2[1] = 0.5f * (LZ2[1] + LZ2[4]);
        m2[2] = 0.5f * (LZ2[2] + LZ2[5]);

        /* Apply offset to m1 */
        dx = m1[0] - m2[0];
        if (fabsf(dx) > 1.e-7f)
        {
            if (dx > 0.0f)
                dx = 1.0f;
            else
                dx = -1.0f;
        }
        dy = m1[1] - m2[1];
        if (fabsf(dy) > 1.e-7f)
        {
            if (dy > 0.0f)
                dy = 1.0f;
            else
                dy = -1.0f;
        }

        m2[0] = dpix * dx;
        m2[1] = dpix * dy;

        /* Name of Z-axis */
        if (data->zdrawname)
        {
            glColor3fv(data->znamecolor);
            offset = data->zaxisoffset + data->ztickmarklength;
            drawText(data->zname, data->fontOfNames, data->styleOfNames, data->sizeOfNames, 
                     m1[0] + offset * m2[0], m1[1] + offset * m2[1], m1[2]);
        }

        /* Z-values */
        if (data->zdrawvalues && data->nbz > 0)
        {
            glColor3fv(data->zcolor);

            i = 0;
            step = fabsf(LZ1[5] - LZ1[2]) / (float) data->nbz;
            offset = data->zoffset + data->ztickmarklength;
            while (i <= data->nbz)
            {
                sprintf(textValue, "%g", LZ1[2] + i * step);
                drawText(textValue, data->fontOfValues, data->styleOfValues, data->sizeOfValues, 
                         m1[0] + offset * m2[0], m1[1] + offset * m2[1], LZ1[2] + i * step);
                i = i + 1;
            }
        }

        /* Z-tickmark */
        if (data->zdrawtickmarks && data->nbz > 0)
        {
            glColor3fv(data->gridcolor);

            i = 0;
            step = fabsf(LZ1[5] - LZ1[2]) / (float) data->nbz;
            while (i <= data->nbz)
            {
                glBegin(GL_LINES);
                    glVertex3f(m1[0],                                 m1[1],                                LZ1[2] + i * step);
                    glVertex3f(m1[0] + data->ztickmarklength * m2[0], m1[1] + data->ztickmarklength * m2[1], LZ1[2] + i * step);
                glEnd();
                i = i + 1;
            }
        }
    }

    /* Activate the lighting if it was turned off by this method call */
    if (light)
        glEnable(GL_LIGHTING);
    
    return TSuccess;
}

TStatus call_graduatedtrihedron_minmaxvalues(const float xMin,
                                             const float yMin,
                                             const float zMin,
                                             const float xMax,
                                             const float yMax,
                                             const float zMax)
{
    xmin = xMin;
    ymin = yMin;
    zmin = zMin;
    xmax = xMax;
    ymax = yMax;
    zmax = zMax;
    return TSuccess;
}