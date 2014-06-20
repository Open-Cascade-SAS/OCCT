// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef InterfaceGraphic_Graphic3dHeader
#define InterfaceGraphic_Graphic3dHeader

#include <InterfaceGraphic_telem.hxx>
#include <Graphic3d_BndBox4f.hxx>
#include <Standard_Transient.hxx>

/* COULEUR */

typedef struct {

        float r, g, b;

} CALL_DEF_COLOR;

/* POINT */

typedef struct {

        float x, y, z;

} CALL_DEF_POINT;

/* MATERIAL */

typedef struct {

        float Ambient;
        int IsAmbient;

        float Diffuse;
        int IsDiffuse;

        float Specular;
        int IsSpecular;

        float Emission;
        int IsEmission;

        float Shininess;
        float Transparency;
        float RefractionIndex;

        float EnvReflexion;

        int IsPhysic;

        /* Attribut couleur eclairage */
        CALL_DEF_COLOR ColorAmb, ColorDif, ColorSpec, ColorEms, Color;


} CALL_DEF_MATERIAL;

/* Transform persistence struct */
typedef struct
{
        int            IsSet;
	int            IsDef;
	int	       Flag;
        CALL_DEF_POINT Point;
} CALL_DEF_TRANSFORM_PERSISTENCE;

/* USERDRAW DATA */

typedef struct {

        void            *Data;
        Graphic3d_BndBox4f  *Bounds;

} CALL_DEF_USERDRAW;

#endif /* InterfaceGraphic_Graphic3dHeader */
