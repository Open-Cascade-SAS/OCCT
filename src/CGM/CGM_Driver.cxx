// Created on: 1996-09-13
// Created by: DCB
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#define xBUG    //  GG 140699
//              Ascendante compatibillity using OLD driver constructor
//              must generates directly a file.

#define IMP020701	//GG
// 		Don't creates an external reference named error please ...
//		Thanks to SAMTECH

#include <CGM_Driver.ixx>
#include <MFT_FontManager.hxx>
#include <PlotMgt_TextManager.hxx>
#include <PlotMgt_HListOfMFTFonts.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Aspect_Units.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_RGBPixel.hxx>
#include <Aspect_DriverError.hxx>
#include <TColStd_DataMapOfIntegerInteger.hxx>
#include <OSD_Environment.hxx>
#include <cgmlib.hxx>

#include <AlienImage.hxx>
#include <Image_Image.hxx>
static Handle(Image_Image) myImage;

/////////////////////////////////////////////////////////////////////////*/
#define TRACE 0
#define PPI (float)(72.F/(0.0254005F METER))
#define MAXPARAM 1024
#define TRANSFORMCOLOR(c) \
  { if (TypeOfCgm == CgmCharEncoding) c = (((c+1) << 2) - 1); }

#define DRAD       (M_PI/180.)
#define DEFPLOTTER "DIRECT_CGM"

//-----------------------------------------------------------------
// Standard CGM settings.
//-----------------------------------------------------------------
#define CGMVER             1
#define INTBITS           32
#define MAXREAL   32767.0000F // Clear text
#define MINREAL  -32768.0000F
#define REALBITS          15 // Character encoding
#define REALPLACES        -5
#define REALDEFEXP         0
#define REALEXP            0
#define REALFORM           0 // Binary encoding
#define REALWHOLE         15
#define REALFRACTION      20

#define INDEXBITS          8
#define CLRBITS           16
#define CLRINDBITS         8
#define SCALEMOD    ABSTRACT
#define SCALFACT           1
#define IMCOLRPREC     65536
#define BACKRED          255
#define BACKGREEN        255
#define BACKBLUE         255

//-----------------------------------------------------------------
// CGM states.
//-----------------------------------------------------------------
typedef enum {
  CgmBinary,
  CgmCharEncoding,
  CgmClearText
} CgmType;

typedef enum {
  mfDesc,
  picDesc,
  picBody,
  imgDraw
} CGMstate;
static CGMstate myState;

static TColStd_DataMapOfIntegerInteger aTypeIndex;
static CgmType                         TypeOfCgm;

//Definition of the C handle who handles the CGM metafile
FILE* cgmo;

//Those C arrays are the parameters used by the CGM library.
long  ptablong[MAXPARAM];
float ptabreal[MAXPARAM];
char  ptabchar[MAXPARAM];

//Declare the array in wich WIDTHS will be put (an internal widthmap).
float* WIDTHMAP = NULL;

// Error string
#ifndef IMP020701	
char error[1024];
#endif

//=============================================================
CGM_Driver::CGM_Driver (const Handle(PlotMgt_Plotter)& aPlotter,
                        const Standard_CString aName,
                        const Quantity_Length aDX,
                        const Quantity_Length aDY,
                        const Aspect_TypeOfColorSpace aTypeOfColorSpace)
                      : PlotMgt_PlotterDriver (aPlotter, aName)
{
  BeginFile (aPlotter, aDX, aDY, aTypeOfColorSpace);
}

//=============================================================
CGM_Driver::CGM_Driver (const Standard_CString aName,
                        const Quantity_Length aDX,
                        const Quantity_Length aDY,
                        const Aspect_TypeOfColorSpace aTypeOfColorSpace)
                      : PlotMgt_PlotterDriver (aName)
{
  Handle(PlotMgt_Plotter) thePlotter =
#ifdef BUG
    new PlotMgt_Plotter(TCollection_AsciiString(DEFPLOTTER));
#else
    new PlotMgt_Plotter(TCollection_AsciiString(DEFPLOTTER),Standard_True);
#endif
  BeginFile (thePlotter, aDX, aDY, aTypeOfColorSpace);
}

//=============================================================
void CGM_Driver::BeginFile (const Handle(PlotMgt_Plotter)& aPlotter,
                            const Quantity_Length aDX,
                            const Quantity_Length aDY,
                            const Aspect_TypeOfColorSpace aTypeOfColorSpace)
{
  SetPlotter (aPlotter);
  // Initialization of buffers
  memset (ptablong, 0, sizeof(ptablong));
  memset (ptabreal, 0, sizeof(ptabreal));
  memset (ptabchar, 0, sizeof(ptabchar));

  myWidth            = Standard_ShortReal(aDX - 2*myPaperMargin);
  myHeight           = Standard_ShortReal(aDY - 2*myPaperMargin);
  myTypeOfColorSpace = aTypeOfColorSpace;

  // Set the type of CGM file by reading the environment variable "CSF_CGM_Type".
  OSD_Environment cgmType = OSD_Environment ( "CSF_CGM_Type" );
  if (cgmType.Value().IsEmpty()) TypeOfCgm = CgmClearText;
  else {
    if (cgmType.Value().IsEqual("BIN")) {
      TypeOfCgm = CgmBinary;
    } else if (cgmType.Value().IsEqual("CAR")) {
      TypeOfCgm = CgmCharEncoding;
    } else if (cgmType.Value().IsEqual("TEX")) {
      TypeOfCgm = CgmClearText;
    } else TypeOfCgm = CgmClearText;
  }

  // Open the CGM metafile
  cgmo = OPEN_FILE ((Standard_PCharacter)myFileName.ToCString(), TypeOfCgm + 1);
  if (!cgmo) {
#ifdef IMP020701
    char error[1024];
#endif
    sprintf (error, "****** CGM_Driver ERROR, unable to open file '%s'\n",
             myFileName.ToCString());
    Aspect_DriverError::Raise(error);
  }
  // Write the metafile descriptor    
  myFileIsOpened = Standard_True;
  myState = mfDesc;
  InitializeDriver (myFileName.ToCString());
}

//=============================================================
void CGM_Driver::Close()
{
  if (myFileIsOpened) {
    // Declare the end of the CGM metafile
    WriteData (ENDMF, ptablong, ptabreal, ptabchar);
    // Free memory allocated for WIDTHS
    if (WIDTHMAP)
      delete[] WIDTHMAP;
    // Close the CGM metafile
    CLOSE_FILE ();
    myFileIsOpened = Standard_False;
  }
  aTypeIndex.Clear ();
}

//=============================================================
void CGM_Driver::BeginDraw ()
{
  // Create TextManager
  myTextManager = new PlotMgt_TextManager(this);
  //============================================
  if (myCurrentPage <= 0) myCurrentPage = 1;
  else                    myCurrentPage++;
  // Write the picture descriptor in the CGM metafile
  myState = picDesc;
  // Declare the new image
  ptablong[0] = myCurrentPage;
  WriteData (BEGPIC, ptablong, ptabreal, ptabchar);
  // Set the color selection mode (always index)
  old.color_mode = DIRECT;
  cur.color_mode = INDEXED;
  WriteData (COLRMODE, ptablong, ptabreal, ptabchar);
  // Set the scale mode
  cur.scale_mode = SCALEMOD;
  cur.scale_factor = (float)SCALFACT;
  WriteData (SCALEMODE, ptablong, ptabreal, ptabchar);
  // Set the method to compute line width
  old.linewidth_mode = SCALED;
  cur.linewidth_mode = ABS;
  WriteData (LINEWIDTHMODE, ptablong, ptabreal, ptabchar);
  //Set the method to compute edge width
  old.edgewidth_mode = SCALED;
  cur.edgewidth_mode = ABS;
  WriteData (EDGEWIDTHMODE, ptablong, ptabreal, ptabchar);
  // Declare the window in which the picture will be drawn
  cur.vdc_extent.a.x.real = (float)0;
  cur.vdc_extent.a.y.real = (float)0;
  cur.vdc_extent.b.x.real = myWidth;
  cur.vdc_extent.b.y.real = myHeight;
  WriteData (VDCEXT, ptablong, ptabreal, ptabchar);
  // set the background color
  cur.back.red   = BACKRED;    TRANSFORMCOLOR (cur.back.red);
  cur.back.green = BACKGREEN;  TRANSFORMCOLOR (cur.back.green);
  cur.back.blue  = BACKBLUE;   TRANSFORMCOLOR (cur.back.blue);
  WriteData (BACKCOLR, ptablong, ptabreal, ptabchar);
  // Declare the body of the picture
  WriteData (BEGPICBODY, ptablong, ptabreal, ptabchar);
  myState = picBody;
  // Write color map in the picture body
  InitializeColorMap(ColorMap ());
  // Set clipping rectangle
  cur.clip_rect.a.x.real = (float)0;
  cur.clip_rect.a.y.real = (float)0;
  cur.clip_rect.b.x.real = myWidth;
  cur.clip_rect.b.y.real = myHeight;
  WriteData (CLIPRECT, ptablong, ptabreal, ptabchar);
  // Set clip indicator to ON
  cur.clip_ind = ON;
  WriteData (CLIP, ptablong, ptabreal, ptabchar);
  // Reset default values for line & fill color and type of line
  oldatt.line.index  = -1;
  oldatt.fill.index  = -1;
  oldatt.edge.index  = -1;
  oldatt.line_type   = -1;
  oldatt.edge_type   = -1;
}

//=============================================================
void CGM_Driver::EndDraw (const Standard_Boolean )//dontFlush)
{
  // Close the current picture
  WriteData (ENDPIC, ptablong, ptabreal, ptabchar);
  myImage.Nullify ();
}

//=============================================================
void CGM_Driver::InitializeColorMap (const Handle(Aspect_ColorMap)& aColorMap) 
{ 
  // CGM version 1 allows the COLOR MAP setting only in the body of a picture.
  if (myState == picBody || myState == imgDraw) {
    Standard_Real r,g,b;
    Standard_Integer index,cpt,tab=2,Size = aColorMap->Size();
    Aspect_ColorMapEntry entry;
    Quantity_Color color;
    ptablong[0] = Size;
    ptablong[1] = cpt = aColorMap->Entry(1).Index();
    for (Standard_Integer i=1; i<= Size; i++) {
      entry = aColorMap->Entry(i);
      index = entry.Index();
      color = entry.Color();
      color.Values(r, g, b, Quantity_TOC_RGB);
      if (myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite) {
        ptablong[tab  ] = Standard_Integer(255*r); TRANSFORMCOLOR(ptablong[tab  ]);
        ptablong[tab+1] = Standard_Integer(255*g); TRANSFORMCOLOR(ptablong[tab+1]);
        ptablong[tab+2] = Standard_Integer(255*b); TRANSFORMCOLOR(ptablong[tab+2]);
      } else {
        ptablong[tab  ] = 0; TRANSFORMCOLOR(ptablong[tab  ]);
        ptablong[tab+1] = 0; TRANSFORMCOLOR(ptablong[tab+1]);
        ptablong[tab+2] = 0; TRANSFORMCOLOR(ptablong[tab+2]);
      }
      tab += 3;
      if (cpt != index)
        break;
      cpt++;
    }
    // Put backgrount color at the end of ColorMap
    ptablong[0]++;
    ptablong[tab  ] = BACKRED;   TRANSFORMCOLOR(ptablong[tab  ]);
    ptablong[tab+1] = BACKGREEN; TRANSFORMCOLOR(ptablong[tab+1]);
    ptablong[tab+2] = BACKBLUE;  TRANSFORMCOLOR(ptablong[tab+2]);
    myBKIndex = cpt;
    // Write ColorMap to the CGM file
    WriteData (COLRTABLE, ptablong, ptabreal, ptabchar);
    if (myTypeOfColorSpace == Aspect_TOCS_BlackAndWhite)
      myTypeOfColorSpace = Aspect_TOCS_RGB;
  }
}

//=============================================================
void CGM_Driver::InitializeTypeMap (const Handle(Aspect_TypeMap)& aTypeMap) 
{ 
/*
In CGM File:
   LINEEDGETYPEDEF  - code
   LONG [0]         - linetype (negative)
   LING [1]         - repeat length
   for (i=0; i < LONG[1]; i++)
     REAL [i]       - line description element
     it's necessary to put all line description elements as integers
*/
/*
  if (CGMstates == picdes) {
    Standard_Integer Size = aTypeMap->Size();
    switch (TypeOfCgm) {
      case (CgmBinary):
        Aspect_DriverError::Raise("CGM BINARY ENCODING not implemented yet");
        break;
      case (CgmCharEncoding):
        Aspect_DriverError::Raise("CGM CHARACTER ENCODING not implemented yet");
        break;
      case (CgmClearText):
        for (Standard_Integer i=1; i<= Size; i++) {
          fprintf(cgmo, "LINEEDGETYPEDEF"); 
          const TColQuantity_Array1OfLength& asr = aTypeMap->Entry(i).Type().Values();
          fprintf (cgmo," (%ld), ", -aTypeMap->Entry(i).Index());
          fprintf (cgmo,"%ld", asr.Upper()-asr.Lower()+1);
          if ( (asr.Upper()-asr.Lower()+1) > 1 ) {
            fprintf(cgmo,",");
          }
          for (Standard_Integer j = asr.Lower(); j < asr.Upper(); j += 2) {
            fprintf(cgmo, "%.*f, %.*f", 5, asr(j+1), 5, asr(j));
            if (j+2 < asr.Upper()) {
              fprintf(cgmo,",");
            }
          }
          fprintf(cgmo,";\n");
        }
        break;
    }
  } else {
    CGMTypeMap = aTypeMap;
  }
*/
  Standard_Integer Size = aTypeMap->Size(), i;
  aTypeIndex.Clear();
  for (i = 1; i <= Size; i++) {
    Standard_Integer index = aTypeMap->Entry(i).Index();
    if (aTypeMap->Entry(i).Type().Style() == Aspect_TOL_USERDEFINED)
      aTypeIndex.Bind (index, 1);
    else
      aTypeIndex.Bind (index, aTypeMap->Entry(i).Type().Style()+1);
  }
}

//=============================================================
void CGM_Driver::InitializeWidthMap (const Handle(Aspect_WidthMap)& aWidthMap) 
{
  Standard_Integer Size = aWidthMap->Size();
  if (WIDTHMAP)
    delete[] WIDTHMAP;
  WIDTHMAP = new float[Size];
  memset (WIDTHMAP, 0, sizeof(WIDTHMAP));
  for (Standard_Integer i=1; i<= Size; i++) {
    Standard_Real w = aWidthMap->Entry(i).Width();
    WIDTHMAP[aWidthMap->Entry(i).Index()] = (float)w;
  }
}

//=================================================================================
Standard_Boolean CGM_Driver::SizeOfImageFile (const Standard_CString anImageFile,
                                              Standard_Integer& aWidth,
                                              Standard_Integer& aHeight) const
{
  return AlienImage::LoadImageFile (anImageFile, myImage, aWidth, aHeight);
}

//=============================================================
// Category: Methods to write attributes
//=============================================================
void CGM_Driver::PlotLineAttrib (const Standard_Integer ColorIndex,
                                 const Standard_Integer TypeIndex,
                                 const Standard_Integer WidthIndex)
{
  // Set color of the line
  if (myColorIndex != ColorIndex) {
    curatt.line.index = myColorIndex = ColorIndex;
    WriteData (LINECOLR, ptablong, ptabreal, ptabchar);
  }
  // Set type of the line
  if (myTypeIndex != TypeIndex) {
    curatt.line_type = myTypeIndex = TypeIndex;
    WriteData (LINETYPE, ptablong, ptabreal, ptabchar);
  }
  // Set width of the line
  if (myWidthIndex != WidthIndex) {
    curatt.line_width.real = WIDTHMAP[(myWidthIndex = WidthIndex)];
    WriteData (LINEWIDTH, ptablong, ptabreal, ptabchar);
  }
  // Set the color of the edge of polygon
  if (myEdgeColor != ColorIndex) {
    curatt.edge.index = myEdgeColor = ColorIndex;
    WriteData (EDGECOLR, ptablong, ptabreal, ptabchar);
  }
  // Set the type of the edge of polygon
  if (myEdgeType != TypeIndex) {
    curatt.edge_type = myEdgeType = TypeIndex;
    WriteData (EDGETYPE, ptablong, ptabreal, ptabchar);
  }
  // Set the width of the edge of polygon
  if (myEdgeWidth != WidthIndex) {
    curatt.edge_width.real = WIDTHMAP[(myEdgeWidth = WidthIndex)];
    WriteData (EDGEWIDTH, ptablong, ptabreal, ptabchar);
  }
}

//===============================================================================
void CGM_Driver::PlotPolyAttrib (const Standard_Integer ColorIndex,
                                 const Standard_Integer TileIndex,
                                 const Standard_Boolean DrawEdge)
{
  // Set the edge visibility to the corresponding value
  if (myEdgeVisibility != (DrawEdge ? ON : OFF)) {
    curatt.edge_vis = myEdgeVisibility = (DrawEdge ? ON : OFF);
    WriteData (EDGEVIS, ptablong, ptabreal, ptabchar);
  }
  // Set the interior style to fill by solid
  if (myInteriorStyle != (TileIndex < 0 ? EMPTY : SOLID)) {
    curatt.int_style = myInteriorStyle = (TileIndex < 0 ? EMPTY : SOLID);
    WriteData (INTSTYLE, ptablong, ptabreal, ptabchar);
  }
  // Set the fill color for drawing
  if (myFillIndex != ColorIndex) {
    if (ColorIndex > 0) curatt.fill.index = myFillIndex = ColorIndex;
    else                curatt.fill.index = myFillIndex = myBKIndex;
    WriteData (FILLCOLR, ptablong, ptabreal, ptabchar);
  }
}

//=============================================================
// Category: Methods to draw primitives
//=============================================================

//=============================================================
Standard_Boolean CGM_Driver::PlotPoint (const Standard_ShortReal X,
                                        const Standard_ShortReal Y)
{
  ptablong[0] = 2;
  ptabreal[0] = X;        ptabreal[1] = Y;
  ptabreal[2] = X + 0.1F; ptabreal[3] = Y + 0.1F;
  WriteData (LINE, ptablong, ptabreal, ptabchar);
  return Standard_True;
}

//=============================================================
Standard_Boolean CGM_Driver::PlotSegment (const Standard_ShortReal X1,
                                          const Standard_ShortReal Y1,
                                          const Standard_ShortReal X2,
                                          const Standard_ShortReal Y2)
{
  ptablong[0] = 2;
  ptabreal[0] = X1; ptabreal[1] = Y1;
  ptabreal[2] = X2; ptabreal[3] = Y2;
  WriteData (LINE, ptablong, ptabreal, ptabchar);
  return Standard_True;
}

//=============================================================
#define _XP(idx) ((float*)xArray)[(idx)]
#define _YP(idx) ((float*)yArray)[(idx)]
//=============================================================
Standard_Boolean CGM_Driver::PlotPolyline (const Standard_Address xArray,
                                           const Standard_Address yArray,
                                           const Standard_Address nPts,
                                           const Standard_Integer nParts)
{
  // Multipart drawing can only be used for text drawing
  // (called from PlotMgt_TextManager::EndChar()
  int Tab, N = 0;
  for (int i = 0; i < nParts; i++) {
    Tab = 0;
    ptablong[0] = ((int*)nPts)[i];
    for (int j = N; j < N + ptablong[0]; j++) {
      ptabreal[Tab++] = _XP(j); ptabreal[Tab++] = _YP(j);
    }
    WriteData (LINE, ptablong, ptabreal, ptabchar);
    N += ptablong[0];
  }
  return Standard_True;
}

//=============================================================
Standard_Boolean CGM_Driver::PlotPolygon (const Standard_Address xArray,
                                          const Standard_Address yArray,
                                          const Standard_Address nPts,
                                          const Standard_Integer nParts)
{
  if (nParts == 1) {
    int Tab = 0;
    ptablong[0] = ((int*)nPts)[0];
    for (int i=0; i < ptablong[0]; i++) {
      ptabreal[Tab++] = _XP(i); ptabreal[Tab++] = _YP(i);
    }
    WriteData (POLYGON, ptablong, ptabreal, ptabchar);
  } else {
    // This can only be used for text drawing
    // (called from PlotMgt_TextManager::EndChar()
    int Tab = 0, Vis = 1, N = 0, cpN;
    ptablong[0] = 0;
    for (int i = 0; i < nParts; i++) {
      cpN = ((int*)nPts)[i];
      for (int j = N; j < N + cpN - 1; j++) {
        ptabreal[Tab++] = _XP(j); ptabreal[Tab++] = _YP(j);
        ptablong[Vis++] = VIS; ptablong[0]++;
      }
      N += cpN;
      ptabreal[Tab++] = _XP(N-1); ptabreal[Tab++] = _YP(N-1);
      ptablong[Vis++] = CLOSEVIS; ptablong[0]++;
    }
    WriteData (POLYGONSET, ptablong, ptabreal, ptabchar);
  }
  return Standard_True;
}
//=============================================================
#undef _XP
#undef _YP

//=============================================================
Standard_Boolean CGM_Driver::PlotArc (const Standard_ShortReal Xpos,
                                      const Standard_ShortReal Ypos,
                                      const Standard_ShortReal aXradius,
                                      const Standard_ShortReal aYradius,
                                      const Standard_ShortReal sAngle,
                                      const Standard_ShortReal oAngle)
{
  Standard_ShortReal san = sAngle;
  Standard_ShortReal fan = sAngle + oAngle;
  if (oAngle >= 2*M_PI) {
    PlotPolyAttrib (myLineColorIndex, -1, Standard_True);
    if (aXradius == aYradius) {
      ptabreal[0] = (float)Xpos; ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)aXradius;
      WriteData (CIRCLE, ptablong, ptabreal, ptabchar);
    } else {
      ptabreal[0] = (float)Xpos;              ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)(Xpos + aXradius); ptabreal[3] = (float)Ypos;
      ptabreal[4] = (float)Xpos;              ptabreal[5] = (float)(Ypos + aYradius);
      WriteData (ELLIPSE, ptablong, ptabreal, ptabchar);
    }
  } else {
    if (aXradius == aYradius) {
      ptabreal[0] = (float)Xpos;     ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)cos(san); ptabreal[3] = (float)sin(san);
      ptabreal[4] = (float)cos(fan); ptabreal[5] = (float)sin(fan);
      ptabreal[6] = (float)aXradius;
      WriteData (ARCCTR, ptablong, ptabreal, ptabchar);
    } else {
      ptabreal[0] = (float)Xpos;              ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)(Xpos + aXradius); ptabreal[3] = (float)Ypos;
      ptabreal[4] = (float)Xpos;              ptabreal[5] = (float)(Ypos + aYradius);
      ptabreal[6] = (float)cos(san);          ptabreal[7] = (float)sin(san);
      ptabreal[8] = (float)cos(fan);          ptabreal[9] = (float)sin(fan);
      WriteData (ELLIPARC, ptablong,  ptabreal, ptabchar);
    }
  }
  return Standard_True;
}

//=============================================================
Standard_Boolean CGM_Driver::PlotPolyArc (const Standard_ShortReal Xpos,
                                          const Standard_ShortReal Ypos,
                                          const Standard_ShortReal aXradius,
                                          const Standard_ShortReal aYradius,
                                          const Standard_ShortReal sAngle,
                                          const Standard_ShortReal oAngle)
{
  Standard_ShortReal san = sAngle;
  Standard_ShortReal fan = sAngle + oAngle;
  if (oAngle >= 2.*M_PI) {
    if (aXradius == aYradius) {
      ptabreal[0] = (float)Xpos; ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)aXradius;
      WriteData (CIRCLE, ptablong, ptabreal, ptabchar);
    } else {
      ptabreal[0] = (float)Xpos;              ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)(Xpos + aXradius); ptabreal[3] = (float)aYradius;
      ptabreal[4] = (float)Xpos;              ptabreal[5] = (float)(Ypos + aYradius);
      WriteData (ELLIPSE, ptablong, ptabreal, ptabchar);
    }
  } else {
    if (aXradius == aYradius) {
      ptabreal[0] = (float)Xpos;     ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)cos(san); ptabreal[3] = (float)sin(san);
      ptabreal[4] = (float)cos(fan); ptabreal[5] = (float)sin(fan);
      ptabreal[6] = (float)aXradius;       ptablong[8] = 0;
      WriteData (ARCCTRCLOSE, ptablong, ptabreal, ptabchar);
    } else {
      ptabreal[0] = (float)Xpos;                ptabreal[1] = (float)Ypos;
      ptabreal[2] = (float)(Xpos + aXradius);   ptabreal[3] = (float)Ypos;
      ptabreal[4] = (float)Xpos;                ptabreal[5] = (float)(Ypos + aYradius);
      ptabreal[6] = (float)cos(san);            ptabreal[7] = (float)sin(san);
      ptabreal[8] = (float)(aXradius*cos(fan)); ptabreal[9] = (float)(aYradius*sin(fan));
      ptablong[11] = 0;
      WriteData (ELLIPARCCLOSE, ptablong, ptabreal, ptabchar);
    }
  }
  return Standard_True;
}

//=============================================================
// Private methods
//=============================================================
void CGM_Driver::InitializeDriver (const Standard_CString aName)
{
  //declare the Metafile
  strcpy(ptabchar,aName);
  WriteData (BEGMF, ptablong, ptabreal, ptabchar);
  //Set the CGM's version
  ptablong[0] = CGMVER;
  WriteData (MFVERSION, ptablong, ptabreal, ptabchar);
  //Put in the metafile a string whose content is free
  strcpy(ptabchar,"Generated with CGM_Driver from MATRA-DATAVISION");
  WriteData (MFDESC, ptablong, ptabreal, ptabchar);
  //Set the element list (always 'DRAWINGPLUS')
  ptablong[1]=1;
  WriteData (MFELEMLIST, ptablong, ptabreal, ptabchar);
  //Set the type of value (integer or real)
  cur.vdc_type = REAL;
  WriteData (VDCTYPE, ptablong, ptabreal, ptabchar);
  //Set the integer precision 
  cur.int_bits = INTBITS;
  WriteData (INTEGERPREC, ptablong, ptabreal, ptabchar);
  //Set the real precision
  cur.max_real = MAXREAL;     // Clear text
  cur.min_real = MINREAL;
  cur.real_bits = REALBITS;   // Character encoding
  cur.real_places = REALPLACES;
  cur.real_defexp = REALDEFEXP;
  cur.real_expald = REALEXP;
  curbin.real_type = REALFORM;    // Binary encoding
  curbin.real_whole = REALWHOLE;
  curbin.real_fraction = REALFRACTION;
  WriteData (REALPREC, ptablong, ptabreal, ptabchar);
  //Set the index precision
  cur.index_bits = INDEXBITS;
  WriteData (INDEXPREC, ptablong, ptabreal, ptabchar);
  //Set the color precision
  cur.col_bits = CLRBITS;
  WriteData (COLRPREC, ptablong, ptabreal, ptabchar);
  //Set the Color Index precision
  cur.colind_bits = CLRINDBITS;
  WriteData (COLRINDEXPREC, ptablong, ptabreal, ptabchar);
  //set the max color index
  cur.max_colind = 255;
  WriteData (MAXCOLRINDEX, ptablong, ptabreal, ptabchar);
  //Set the minimum color and the maximum color
  cur.min_rgb.red = cur.min_rgb.green = cur.min_rgb.blue = 0;
  cur.max_rgb.red = cur.max_rgb.green = cur.max_rgb.blue = 255;
  TRANSFORMCOLOR(cur.max_rgb.red);
  TRANSFORMCOLOR(cur.max_rgb.green);
  TRANSFORMCOLOR(cur.max_rgb.blue);
  WriteData (COLRVALUEEXT, ptablong, ptabreal,  ptabchar);
  //Set the internal variables
  myBKIndex           = 0;
  myCurrentPage       = 0;
  myFillIndex         = -9;
  myEdgeColor         = -9;
  myEdgeType          = -9;
  myEdgeWidth         = -9;
  myInteriorStyle     = -9;
  myEdgeVisibility    = -9;
  myImage.Nullify ();
}

//=============================================================
void CGM_Driver::WriteData (const Standard_Integer aCode,
                            const Standard_Address pLongData,
                            const Standard_Address pFloatData,
                            const Standard_Address pCharData)
{
  short  code   = (short) aCode;
  long*  aLong  = (long*) pLongData;
  float* aFloat = (float*)pFloatData;
  char*  aChar  = (char*) pCharData;
  if (code == LINETYPE) {
    curatt.line_type = aTypeIndex.Find(curatt.line_type);
  } else if (code == EDGETYPE) {
    curatt.edge_type = aTypeIndex.Find(curatt.edge_type);
  }
  switch (TypeOfCgm) {
    case (CgmBinary):
      CGMObin  (cgmo, code, aLong, aFloat, aChar);
      break;
    case (CgmCharEncoding):
      CGMOchar (cgmo, code, aLong, aFloat, aChar);
      break;
    case (CgmClearText):
      CGMOtext (cgmo, code, aLong, aFloat, aChar);
      break;
  }
}

//=============================================================
Standard_Boolean CGM_Driver::PlotImage (const Standard_ShortReal aX,
                                        const Standard_ShortReal aY,
                                        const Standard_ShortReal aWidth,
                                        const Standard_ShortReal aHeight,
                                        const Standard_ShortReal aScale,
                                        const Standard_CString anImageFile,
                                        const Standard_Address anArrayOfPixels,
                                        const Standard_Integer aLineIndex)
{
  Aspect_ColorMapEntry anEntry;
  Handle(Aspect_GenericColorMap) aColorMap = new Aspect_GenericColorMap;
  // We'll create new colormap here and will set it
  Standard_Boolean status;
  Standard_Integer idx = 0, found = 0, fidx = 0, LowX = 0, LowY = 0;
  Standard_Integer width  = Standard_Integer(aWidth),
                   height = Standard_Integer(aHeight);
  Standard_Integer x, y;
  Standard_Real    r, g, b;
  Aspect_RGBPixel* g2dp = (Aspect_RGBPixel*) anArrayOfPixels;
  Quantity_Color   color;
  // Load image if necessary
  if (anImageFile) {
    status = SizeOfImageFile (anImageFile, width, height);
    if (status) {
      LowX = myImage -> LowerX ();
      LowY = myImage -> LowerY ();
    }
  } else {
    status = Standard_True;
  }

  // Put image colormap and pixels to the file
  if (status) {
    Standard_ShortReal wscale, hscale;
    wscale = hscale = (float)(aScale * myPixelSize * PPI / 2.F);
    // Put image in the file
    float a2X = (float)(aX - wscale * width/2.F);
    float a2Y = (float)(aY + hscale * (height/2.F - (aLineIndex == -1 ? 0:aLineIndex)));
    if (aLineIndex != -1) height = 1;
    int cellsSize = sizeof(long)*(width*height + 10);

    Aspect_RGBPixel* pixels =
      (Aspect_RGBPixel*) malloc (IMCOLRPREC*sizeof(Aspect_RGBPixel));
    if (pixels == NULL)  return Standard_False;
    long* cells = (long*) malloc (cellsSize);
    if (cells == NULL) {  free (pixels); return Standard_False;  }

    //Initialize the array
    memset (pixels, 0, IMCOLRPREC*sizeof(Aspect_RGBPixel));
    int pixidx = 10; // Pixel description begins with index 10
    // Put cellarray entry data
    ptabreal[0] = a2X;                ptabreal[1] = a2Y; // P
    ptabreal[2] = a2X+(width*wscale); ptabreal[3] = a2Y-(height*hscale); // Q
    ptabreal[4] = a2X+(width*wscale); ptabreal[5] = a2Y; // R
    cells[0]    = width*height;       cells[7] = width;
    cells[8]    = height;
    // For all pixels of an Image
    for (y=0; y < height; y++) {
      for (x=0; x < width; x++) {
        // Get pixel of image at (x,y) position
        if (anImageFile) {
          color = myImage -> PixelColor (x + LowX, y + LowY);
          color.Values (r, g, b, Quantity_TOC_RGB);
        } else {
          r = g2dp -> red;
          g = g2dp -> green;
          b = g2dp -> blue;
          g2dp++;
        }
        // Try to find the pixel
        for (int c = 0; c < idx; c++) {
          if (pixels[c].red   == r && pixels[c].green == g && pixels[c].blue  == b) {
            found = 1;
            fidx  = c;
          }
          if (found) break;
        }
        if (!found) {
          // It's necessary to add new color to colormap
          pixels[idx].red   = (float)r;
          pixels[idx].green = (float)g;
          pixels[idx].blue  = (float)b;
          color.SetValues (r, g, b, Quantity_TOC_RGB);
          anEntry = Aspect_ColorMapEntry (idx, color);
          anEntry.SetIndex (idx);
          aColorMap->AddEntry (anEntry);
          cells[pixidx++] = idx;
          idx++;
        } else {
          cells[pixidx++] = fidx;
        }
        if (idx > IMCOLRPREC) {
          free (pixels);
          free (cells);
          return Standard_False;
        }
        found = 0;
      } // for (x=0; x < width; x++)
    } // for (y=0; y < height; y++)
    free (pixels);
    // Calculate image color precision
    int imcolprec = 0, tmp = 1L;
    while (tmp < aColorMap->Size()) { tmp <<= 1L; imcolprec++; }
    cells[9] = imcolprec;
    // Write image data to file (colormap and image data).
    myState = imgDraw;
    InitializeColorMap (aColorMap);
    WriteData          (CELLARRAY, cells, ptabreal, ptabchar);
    // Restore old colormap
    myState = picBody;
    InitializeColorMap (ColorMap ());
    free (cells);
    return Standard_True;
  } // IF STATUS
  return Standard_False;
}
