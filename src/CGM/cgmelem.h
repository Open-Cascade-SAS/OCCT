/*
 Copyright (c) 1999-2012 OPEN CASCADE SAS

 The content of this file is subject to the Open CASCADE Technology Public
 License Version 6.5 (the "License"). You may not use the content of this file
 except in compliance with the License. Please obtain a copy of the License
 at http://www.opencascade.org and read it completely before using this file.

 The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
 main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.

 The Original Code and all software distributed under the License is
 distributed on an "AS IS" basis, without warranty of any kind, and the
 Initial Developer hereby disclaims all such warranties, including without
 limitation, any warranties of merchantability, fitness for a particular
 purpose or non-infringement. Please see the License for the specific terms
 and conditions governing the rights and limitations under the License.

*/

#ifndef CGMELEM_H
#define CGMELEM_H

/*  Include the files on which this is dependent.  */

#include "cgmtypes.h"

#define ASFS          18

/*  Non-Op  */

#define NONOP            (Code) 0

/*  Character CGM hex values  */

#define BEGMF            (Code) 0x3020
#define ENDMF            (Code) 0x3021
#define BEGPIC           (Code) 0x3022
#define BEGPICBODY       (Code) 0x3023
#define ENDPIC           (Code) 0x3024

/*  Metafile Descriptor elements  */

#define MFVERSION        (Code) 0x3120
#define MFDESC           (Code) 0x3121
#define VDCTYPE          (Code) 0x3122
#define INTEGERPREC      (Code) 0x3123
#define REALPREC         (Code) 0x3124
#define INDEXPREC        (Code) 0x3125
#define COLRPREC         (Code) 0x3126
#define COLRINDEXPREC    (Code) 0x3127
#define MAXCOLRINDEX     (Code) 0x3128
#define COLRVALUEEXT     (Code) 0x3129
#define MFELEMLIST       (Code) 0x312A
#define BEGMFDEFAULTS    (Code) 0x312B
#define ENDMFDEFAULTS    (Code) 0x312C
#define FONTLIST         (Code) 0x312D
#define CHARSETLIST      (Code) 0x312E
#define CHARCODING       (Code) 0x312F

/*  Picture descriptor elements  */

#define SCALEMODE        (Code) 0x3220
#define COLRMODE         (Code) 0x3221
#define LINEWIDTHMODE    (Code) 0x3222
#define MARKERSIZEMODE   (Code) 0x3223
#define EDGEWIDTHMODE    (Code) 0x3224
#define VDCEXT           (Code) 0x3225
#define BACKCOLR         (Code) 0x3226

/*  Control elements   */

#define VDCINTEGERPREC   (Code) 0x3320
#define VDCREALPREC      (Code) 0x3321
#define AUXCOLR          (Code) 0x3322
#define TRANSPARENCY     (Code) 0x3323
#define CLIPRECT         (Code) 0x3324
#define CLIP             (Code) 0x3325

/*  Graphics primitives  */

#define LINE             (Code) 0x20
#define DISJTLINE        (Code) 0x21
#define MARKER           (Code) 0x22
#define TEXT             (Code) 0x23
#define RESTRTEXT        (Code) 0x24
#define APNDTEXT         (Code) 0x25
#define POLYGON          (Code) 0x26
#define POLYGONSET       (Code) 0x27
#define CELLARRAY        (Code) 0x28
#define GDP              (Code) 0x29
#define RECT             (Code) 0x2A

#define CIRCLE           (Code) 0x3420
#define ARC3PT           (Code) 0x3421
#define ARC3PTCLOSE      (Code) 0x3422
#define ARCCTR           (Code) 0x3423
#define ARCCTRCLOSE      (Code) 0x3424
#define ELLIPSE          (Code) 0x3425
#define ELLIPARC         (Code) 0x3426
#define ELLIPARCCLOSE    (Code) 0x3427

/*  Attribute elements  */

#define LINEINDEX        (Code) 0x3520
#define LINETYPE         (Code) 0x3521
#define LINEWIDTH        (Code) 0x3522
#define LINECOLR         (Code) 0x3523
#define MARKERINDEX      (Code) 0x3524
#define MARKERTYPE       (Code) 0x3525
#define MARKERSIZE       (Code) 0x3526
#define MARKERCOLR       (Code) 0x3527

#define TEXTINDEX        (Code) 0x3530
#define TEXTFONTINDEX    (Code) 0x3531
#define TEXTPREC         (Code) 0x3532
#define CHAREXPAN        (Code) 0x3533
#define CHARSPACE        (Code) 0x3534
#define TEXTCOLR         (Code) 0x3535
#define CHARHEIGHT       (Code) 0x3536
#define CHARORI          (Code) 0x3537
#define TEXTPATH         (Code) 0x3538
#define TEXTALIGN        (Code) 0x3539
#define CHARSETINDEX     (Code) 0x353A
#define ALTCHARSETINDEX  (Code) 0x353B

#define FILLINDEX        (Code) 0x3620
#define INTSTYLE         (Code) 0x3621
#define FILLCOLR         (Code) 0x3622
#define HATCHINDEX       (Code) 0x3623
#define PATINDEX         (Code) 0x3624
#define EDGEINDEX        (Code) 0x3625
#define EDGETYPE         (Code) 0x3626
#define EDGEWIDTH        (Code) 0x3627
#define EDGECOLR         (Code) 0x3628
#define EDGEVIS          (Code) 0x3629
#define FILLREFPT        (Code) 0x362A
#define PATTABLE         (Code) 0x362B
#define PATSIZE          (Code) 0x362C

#define COLRTABLE        (Code) 0x3630
#define ASF              (Code) 0x3631

/*  Escape Elements  */

#define ESCAPE           (Code) 0x3720
#define MESSAGE          (Code) 0x3721
#define APPLDATA         (Code) 0x3722

#define DOMAINRING       (Code) 0x3730

/*  Version 2 Elements - Addendum 1  */

#if CGMVERSION > 1

#define BEGSEG           (Code) 0x3025
#define ENDSEG           (Code) 0x3026
#define BEGFIG           (Code) 0x3027
#define ENDFIG           (Code) 0x3028

#define NAMEPREC         (Code) 0x3130
#define MAXVDCEXT        (Code) 0x3131
#define SEGPRIEXT        (Code) 0x3132

#define DEVVP            (Code) 0x3227
#define DEVVPMODE        (Code) 0x3228
#define DEVVPMAP         (Code) 0x3229
#define LINEREP          (Code) 0x322A
#define MARKERREP        (Code) 0x322B
#define TEXTREP          (Code) 0x322C
#define FILLREP          (Code) 0x322D
#define EDGEREP          (Code) 0x322E

#define LINECLIPMODE     (Code) 0x3326

#define MARKERCLIPMODE   (Code) 0x3327

#define EDGECLIPMODE     (Code) 0x3328

#define NEWREGION        (Code) 0x3329
#define SAVEPRIMCONT     (Code) 0x332A

#define RESPRIMCONT      (Code) 0x332B

#define ARCCTRREV        (Code) 0x3428
#define CONNEDGE         (Code) 0x3429

#define PICKID           (Code) 0x3632

#define COPYSEG          (Code) 0x3820
#define INHFILTER        (Code) 0x3821
#define CLIPINH          (Code) 0x3822
#define SEGTRANS         (Code) 0x3823
#define SEGHIGHLIGHT     (Code) 0x3824

#define SEGDISPPRI       (Code) 0x3825
#define SEGPICKPRI       (Code) 0x3826

/*  GKS Annex elements */

#define BEGGKSSESSIONMF  (Code) 0x3029
#define BEGGKSSESSION    (Code) 0x302A
#define ENDGKSSESSION    (Code) 0x302B

#define WSVP             (Code) 0x332C
#define CLEAR            (Code) 0x332D
#define UPD              (Code) 0x332E
#define DEFERST          (Code) 0x332F

#define DELSEG           (Code) 0x3827
#define RENAMESEG        (Code) 0x3828
#define REDRAWALLSEG     (Code) 0x3829
#define SEGVIS           (Code) 0x382A
#define SEGDET           (Code) 0x382B

/*  Version 3 Elements - Addendum 3  */

#if CGMVERSION > 2

#define BEGPROTREGION       (Code) 0x302C
#define ENDPROTREGION       (Code) 0x302D
#define BEGCOMPOLINE        (Code) 0x302E
#define ENDCOMPOLINE        (Code) 0x302F
#define BEGCOMPOTEXTPATH    (Code) 0x3030
#define ENDCOMPOTEXTPATH    (Code) 0x3031
#define BEGTILEARRAY        (Code) 0x3032
#define ENDTILEARRAY        (Code) 0x3033

#define COLRMODEL           (Code) 0x3133
#define COLRCALIB           (Code) 0x3134
#define FONTPROP            (Code) 0x3135
#define GLYPHMAP            (Code) 0x3136
#define SYMBOLLIBLIST       (Code) 0x3137

#define INTSTYLEMODE        (Code) 0x322F
#define LINEEDGETYPEDEF     (Code) 0x3230
#define HATCHSTYLEDEF       (Code) 0x3231
#define GEOPATDEF           (Code) 0x3232

#define PROTREGION          (Code) 0x3330
#define GENTEXTPATHMODE     (Code) 0x3331
#define MITRELLIMIT         (Code) 0x3332
#define TRANSPCELLCOLR      (Code) 0x3333

#define HYPERBARC           (Code) 0x342A
#define PARABARC            (Code) 0x342B
#define NUB                 (Code) 0x342C
#define NURB                (Code) 0x342D
#define POLYBEZIER          (Code) 0x342E
#define SYMBOL              (Code) 0x342F
#define INCRSYMBOL          (Code) 0x342F
#define BITONALTILE         (Code) 0x3430
#define TILE                (Code) 0x3431

#define LINECAP             (Code) 0x3528
#define LINEJOIN            (Code) 0x3529
#define LINETYPECONT        (Code) 0x352A
#define LINETYPEINITOFFSET  (Code) 0x352B
#define TEXTSCORETYPE       (Code) 0x352C
#define RESTTEXTTYPE        (Code) 0x352D
#define INTERPINT           (Code) 0x352E
#define EDGECAP             (Code) 0x352F
#define EDGEJOIN            (Code) 0x362D
#define EDGETYPECONT        (Code) 0x362E
#define EDGETYPEINITOFFSET  (Code) 0x362F
#define SYMBOLLIBINDEX      (Code) 0x3633
#define SYMBOLCOLR          (Code) 0x3634
#define SYMBOLSIZE          (Code) 0x3635
#define SYMBOLORI           (Code) 0x3636

#endif /* Version 3 */

#endif  /* Version 2 */

#endif  /*  end of cgmelem.h */
