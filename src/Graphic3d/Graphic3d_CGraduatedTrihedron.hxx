// Created on: 2011-03-06
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _Graphic3d_CGraduatedTrihedron_HeaderFile
#define _Graphic3d_CGraduatedTrihedron_HeaderFile

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Quantity_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Font_FontAspect.hxx>

class Visual3d_View;

//! Defines the C structure of a graduated trihedron.
class Graphic3d_CGraduatedTrihedron
{
public:
  typedef void (*minMaxValuesCallback)(Visual3d_View*);
public:
    /* Default constructor. Nullifies the view pointer */
    Graphic3d_CGraduatedTrihedron () : xdrawname ( Standard_False ),
    ydrawname ( Standard_False ),
    zdrawname ( Standard_False ),
    xdrawvalues ( Standard_False ),
    ydrawvalues ( Standard_False ),
    zdrawvalues ( Standard_False ),
    drawgrid ( Standard_False ),
    drawaxes ( Standard_False ),
    nbx ( 0 ),nby ( 0 ),nbz ( 0 ),
    xoffset ( 0 ),yoffset ( 0 ),zoffset ( 0 ),
    xaxisoffset ( 0 ),yaxisoffset ( 0 ),zaxisoffset ( 0 ),
    xdrawtickmarks ( Standard_False ),
    ydrawtickmarks ( Standard_False ),
    zdrawtickmarks ( Standard_False ),
    xtickmarklength ( 0 ), ytickmarklength ( 0 ), ztickmarklength ( 0 ),
    styleOfNames ( Font_FA_Undefined ),
    sizeOfNames ( 0 ),
    styleOfValues ( Font_FA_Undefined ),
    sizeOfValues ( 0 ),
    cbCubicAxes ( NULL ),
    ptrVisual3dView(NULL)
	{
		//
	}

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
    Font_FontAspect styleOfNames;
    /* Size of names of axes: 8, 10, 12, 14, ... */
    Standard_Integer sizeOfNames;
    /* Font name of values: Courier, Arial, ... */
    TCollection_AsciiString fontOfValues;
    /* Style of values: OSD_FA_Regular, OSD_FA_Bold, ... */
    Font_FontAspect styleOfValues;
    /* Size of values: 8, 10, 12, 14, ... */
    Standard_Integer sizeOfValues;

    /* Callback function to define boundary box of displayed objects */
    minMaxValuesCallback cbCubicAxes;
    Visual3d_View* ptrVisual3dView;
};

#endif /*Graphic3d_CGraduatedTrihedron_HeaderFile*/
