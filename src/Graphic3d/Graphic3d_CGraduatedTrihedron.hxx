// File:      Graphic3d_CGraduatedTrihedron.hxx
// Created:   6 March 2011
// Author:    Sergey ZERCHANINOV
// Copyright: OPEN CASCADE SA 2011

#ifndef _Graphic3d_CGraduatedTrihedron_HeaderFile
#define _Graphic3d_CGraduatedTrihedron_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Type.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <OSD_FontAspect.hxx>

typedef void (*minMaxValuesCallback)(void*);
class Graphic3d_CGraduatedTrihedron
{
  public:
    /* Default constructor. Nullifies the view pointer */
    Graphic3d_CGraduatedTrihedron () : ptrVisual3dView(NULL) {}

    /* Names of axes */
    TCollection_ExtendedString xname, yname, zname;
    /* Draw names */
    Standard_Boolean xdrawname, ydrawname, zdrawname;
    /* Draw values */
    Standard_Boolean xdrawvalues, ydrawvalues, zdrawvalues;
    /* Draw grid */
    Standard_Boolean drawgrid;
    /* Draw axes */
    Standard_Boolean drawaxes;
    /* Number of splits along axes */
    Standard_Integer nbx, nby, nbz;
    /* Offset for drawing values */
    Standard_Integer xoffset, yoffset, zoffset;
    /* Offset for drawing names of axes */
    Standard_Integer xaxisoffset, yaxisoffset, zaxisoffset;
    /* Draw tickmarks */
    Standard_Boolean xdrawtickmarks, ydrawtickmarks, zdrawtickmarks;
    /* Length of tickmarks */
    Standard_Integer xtickmarklength, ytickmarklength, ztickmarklength;
    /* Grid color */
    Quantity_Color gridcolor;
    /* Colors of axis names */
    Quantity_Color xnamecolor, ynamecolor, znamecolor;
    /* Colors of axis and values */
    Quantity_Color xcolor, ycolor, zcolor;
    /* Font name of names of axes: Courier, Arial, ... */
    TCollection_AsciiString fontOfNames;
    /* Style of names of axes: OSD_FA_Regular, OSD_FA_Bold, ... */
    OSD_FontAspect styleOfNames;
    /* Size of names of axes: 8, 10, 12, 14, ... */
    Standard_Integer sizeOfNames;
    /* Font name of values: Courier, Arial, ... */
    TCollection_AsciiString fontOfValues;
    /* Style of values: OSD_FA_Regular, OSD_FA_Bold, ... */
    OSD_FontAspect styleOfValues;
    /* Size of values: 8, 10, 12, 14, ... */
    Standard_Integer sizeOfValues;

    /* Callback function to define boundary box of displayed objects */
    minMaxValuesCallback cbCubicAxes;
    void* ptrVisual3dView;
};

const Handle(Standard_Type)& TYPE(Graphic3d_CGraduatedTrihedron);

#endif /*Graphic3d_CGraduatedTrihedron_HeaderFile*/
