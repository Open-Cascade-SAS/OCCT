// Copyright (c) 1995-1999 Matra Datavision
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

/***********************************************************************

FONCTION :
----------
File OpenGl_telem_util :


REMARQUES:
---------- 


HISTORIQUE DES MODIFICATIONS   :
--------------------------------
xx-xx-xx : xxx ; Creation.
07-02-96 : FMN ; - Ajout trace
- Suppression code inutile
08-03-96 : FMN ; - Ajout include manquant 
01-04-96 : CAL ; Integration MINSK portage WNT
15-04-96 : CAL ; Integration travail PIXMAP de Jim ROTH
22-04-96 : FMN ; Ajout TelReadImage TelDrawImage
10-05-96 : CAL ; Ajout d'un nouveau delta dans les copies
de pixels (voir CALL_DEF_DELTA)
25-06-96 : FMN ; Suppression utilisation de glScissor.
02-07-96 : FMN ; Suppression WSWSHeight et WSWSWidth
Suppression glViewport inutile.
18-07-96 : FMN ; Suppression TelFlush inutile.
08-07-96 : FMN ; Suppression de OPENGL_DEBUG inutile avec la nouvelle
version de ogldebug.
24-10-96 : CAL ; Portage WNT
23-01-97 : CAL ; Suppression de TelClearViews dans TelCopyBuffers
30-01-97 : FMN ; Ajout commentaires + WNT.
12-02-97 : FMN ; Suppression TelEnquireFacilities()
22-04-97 : FMN ; Ajout affichage du cadre pour la copie de buffer
30-06-97 : FMN ; Suppression OpenGl_telem_light.h
18-07-97 : FMN ; Utilisation de la toolkit sur les lights
07-10-97 : FMN ; Simplification WNT + correction Transient
05-12-97 : FMN ; PRO11168: Suppression TglActiveWs pour project/unproject
23-12-97 : FMN ; Suppression TelSetFrontFaceAttri et TelSetBackFaceAttri 
30-12-97 : FMN ; CTS18312: Correction back material
04-05-98 : CAL ; Contournement bug SGI octane bavure de pixels (PRO12899)
30-09-98 : CAL ; Optimisation pour eviter de charger inutilement
les matrices de la vue.
19-10-98 : FMN ; Suppression de glPixelTransferi dans TelEnable() car cela
rentre en conflit avec l'utilisation d'une image de fond.
02.14.100 : JR : Warnings on WNT truncations from double to float
08-03-01 : GG  ; BUC60823 Avoid crash in the normal computation method
when a face has confused or aligned points.

************************************************************************/

#include <stdio.h>

#include <OpenGl_GlCore11.hxx>

#include <OpenGl_telem_util.hxx>
#include <InterfaceGraphic_Graphic3d.hxx>
#include <InterfaceGraphic_Visual3d.hxx>

#define GPRECIS 0.000001
Tint TelGetPolygonNormal(tel_point pnts, Tint* indexs, Tint npnt, Tfloat *norm ) {
  Tint status=0;

  norm[0] = norm[1] = norm[2] = 0.;
  if( npnt > 2 ) { 
    Tfloat a[3], b[3], c[3];
    Tint i,j,i0,ii=0,jj;

    i0 = 0; if( indexs ) i0 = indexs[0];
    for( i=1 ; i<npnt ; i++ ) {
      ii = i; if( indexs ) ii = indexs[i];
      vecsub( a, pnts[ii].xyz, pnts[i0].xyz );
      if( vecmg2(a) > GPRECIS ) break;
    }
    if( i < npnt-1 ) {
      for( j=i+1 ; j<npnt ; j++ ) {
        jj = j; if( indexs ) jj = indexs[j];
        vecsub( b, pnts[jj].xyz, pnts[i0].xyz );
        vecsub( c, pnts[jj].xyz, pnts[ii].xyz );
        if( (vecmg2(b) > GPRECIS) && (vecmg2(c) > GPRECIS) ) break;
      }
      if( j < npnt ) {
        Tfloat d;
        veccrs( norm, a, b );
        d = vecnrmd( norm, d );
        status = (d > 0.) ? 1 : 0;
      }
    }
  }
#ifdef OCCT_DEBUG
  if( !status )
    printf(" *** OpenGl_TelGetPolygonNormal.has found confused or aligned points\n");
#endif

  return status;
}

Tint TelGetNormal(Tfloat *data1, Tfloat *data2, Tfloat *data3, Tfloat *norm ) {
  Tfloat a[3], b[3];
  Tint status=0;

  norm[0] = norm[1] = norm[2] = 0.;
  vecsub( a, data2, data1 );
  vecsub( b, data3, data2 );
  if( (vecmg2(a) > GPRECIS) && (vecmg2(b) > GPRECIS) ) {
    Tfloat d;
    veccrs( norm, a, b );
    d = vecnrmd( norm, d );
    status = (d > 0.) ? 1 : 0;
  }
#ifdef OCCT_DEBUG
  if( !status )
    printf(" *** OpenGl_TelGetNormal.has found confused or aligned points\n");
#endif

  return status;
}

void TelMultiplymat3 (Tmatrix3 c, Tmatrix3 a, Tmatrix3 b)
{
  Tint row, col, i;
  Tmatrix3 res;
  Tint dim = 4;

  /* on multiplie d'abord les 2 matrices dim x dim */
  for (row = 0; row < dim; row++) {
    for (col = 0; col < dim; col++) {
      Tfloat sum = ( float )0.0;
      for (i = 0; i < dim; i++)
        sum += a[row][i] * b[i][col];
      res[row][col] = sum;
    }
  }

  /* on copie ensuite le resultat */
  matcpy (c, res);

  return;
}
