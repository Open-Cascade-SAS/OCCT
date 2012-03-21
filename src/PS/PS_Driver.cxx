// Copyright (c) 1995-1999 Matra Datavision
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

#define PRO7689 //GG_300597
//              La hauteur des caracteres avec CapsHeight TRUE doit etre
//              relative a la hauteur de la lettre H et non pas
//              la hauteur max de la police.

#define MFT     //GG_MFT Etude G1343
//              Utilisation du FontManager MFT permettant de traiter
//              les chaines de caracteres EUCLID3 orientees et slantees.

#define PRO8709 //GG_170697
//              Les textes encadres ou caches ne fonctionnent pas en mode
//              NOIR & BLANC

#define CTS17946//GG_030797/GG_300698
//              En mode NOIR et BLANC,ne pas remplir les polygones.
//              dans la couleur du fond.

#define CTS18150//GG_210797
//              Parametrer le format papier de maniere a reduire
//              automatiquement l'echelle lorsque le format du dessin
//              est plus grand.

#define PRO11339        //GG_260398
//              Ne pas arrondir l'epaisseur du trait

#define JAP60166        //GG_180698
//              Les polygones sont transparents alors qu'il devraient 
//              etres remplis.
//              Traitement des niveaux de gris.

#define S3602 //SYL_170898
//              Constructeur avec Format du papier en DX,DY ou FOSP
//              Prise en compte de la marge papier
//#define PRO15090 //SYL_040998 annulle le 170998 suite a debug de GG (PRO15231) 
//              Utiliser toujours les polices MFT pour etre sur d'etre wysywig en ettendant de maitriser 
//              l'echelle 
#define PRO15119 //SYL_040998
//              En monochrome, on force les couleurs au noir tout en permettant le 
//              trace avec le fond blanc => le cache fonctionne

#define PRO15784 // SYL 19101998 les textes cachants ne doivent pas toujours etre encadres
#define PRO15786 // SYL 19101998 erreur setdash sur osf seulement !!

#define BUC60766    //  GG 300399
//              Ascendante compatibillity using OLD driver constructor
//              must generates directly a file.
//		Re Enable to draw PostScript string when it's requested

#define BUC60772	//GG_061100 Round off vectors intersection
//                      angle during MFT drawing with PostScript driver.

#define CSR862	// GG_101100
//		Enable to draw full drawing after rotation and auto scaling.

#define MOVETO "M "
#define RMOVETO "RM "
#define LINETO "L "
#define CURVETO "CT "
#define RLINETO "RL "
#define ARC "A "
#define ARCN "AN "
#define STROKE "ST "
#define FILL "F "
#define SHOW "SH "
#define GSAVE "GS "
#define GRESTORE "GR "
#define ROTATE "R "
#define SCALE "SC "
#define CHARPATH "CHP "
#define CLOSEPATH "CLP "
#define NEWPATH "NP "
#define SHOWHIDINGTEXT "ShowHidingText "
#define SHOWFRAMEDTEXT "ShowFramedText "
#define SHOWUNDERLINEDTEXT "ShowUnderlinedText "
#define FILLRECTANGLE "FillRectangle "
#define DRAWRECTANGLE "DrawRectangle "
#define UNDERLINE "UnderLine "
#define SCALEDRAWING "ScaleDrawing "
#define SCALEFONT "ScaleFont "
static const char* MONTHS [] = {
  "January", "February", "March",
  "April", "May", "June", "July",
  "August", "September", "October",
  "November", "December"
};

#include <PS_Driver.ixx>
#include <MFT_FontManager.hxx>
#include <PlotMgt_TextManager.hxx>
#include <PlotMgt_HListOfMFTFonts.hxx>
#include <TShort_HArray1OfShortReal.hxx>
#include <Aspect.hxx>
#include <Aspect_RGBPixel.hxx>
#include <Quantity_Date.hxx>
#include <Aspect_Units.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <OSD_Process.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <AlienImage.hxx>
#include <Image_Image.hxx>
#include <stdio.h>
static Handle(Image_Image) myImage;
#ifdef WNT
#pragma warning (disable : 4244 4018 4101)
#endif

#define MAXPOINT        1024
#define DRAD            (3.1415927/180.)
#define PPI             72./(0.0254005 METER)
#define IMAGEBUFFERSIZE 48
#define DEFPLOTTER      "DIRECT_PS"

#define PLOT_PCOLOR(aCol)                       \
  if (myColorIndex != (aCol)) {                 \
    myColorIndex = (aCol);                      \
    if (myColorIndex > 0)                       \
      (*Cout()) << " C" << myColorIndex << " "; \
    else                                        \
      (*Cout()) << " CB ";                      \
  }

//==========================================================================
PS_Driver::PS_Driver(const Handle(PlotMgt_Plotter)& aPlotter,
                     const Standard_CString aName,
                     const Quantity_Length aPaperX,
                     const Quantity_Length aPaperY,
                     const Aspect_TypeOfColorSpace aTypeOfColorSpace)
                   : PlotMgt_PlotterDriver (aPlotter, aName, Standard_True)
{
  BeginFile( aPlotter,aName,aPaperX,aPaperY,aTypeOfColorSpace,aPaperX,aPaperY);
}

//==========================================================================
PS_Driver::PS_Driver(const Standard_CString aName,
                     const Quantity_Length aDX,
                     const Quantity_Length aDY,
                     const Aspect_TypeOfColorSpace aTypeOfColorSpace,
                     const Aspect_FormatOfSheetPaper aSheetFormat)
                   : PlotMgt_PlotterDriver (aName, Standard_True)
{
  Quantity_Length thePaperWidth,thePaperHeight;
  Aspect::ValuesOfFOSP(aSheetFormat,thePaperWidth,thePaperHeight);
#ifndef BUC60766
  Handle(PlotMgt_Plotter) thePlotter = new PlotMgt_Plotter(TCollection_AsciiString(DEFPLOTTER));
#else
  Handle(PlotMgt_Plotter) thePlotter = new PlotMgt_Plotter(TCollection_AsciiString(DEFPLOTTER),Standard_True);
#endif
  SetPlotter (thePlotter);
  BeginFile (thePlotter,aName,aDX,aDY,aTypeOfColorSpace,thePaperWidth,thePaperHeight);
}

//==========================================================================
void PS_Driver::BeginFile(const Handle(PlotMgt_Plotter)& aPlotter,
                          const Standard_CString aName,
                          const Quantity_Length aDX,
                          const Quantity_Length aDY,
                          const Aspect_TypeOfColorSpace aTypeOfColorSpace,
                          const Quantity_Length thePaperWidth,
                          const Quantity_Length thePaperHeight)
{
  OSD_Process   P;
  Quantity_Date Date = P.SystemDate();

  myTypeOfColorSpace = aTypeOfColorSpace;
  
  // define the real workspace, usefull for mapping by the user of the driver
  // before sending the objects to plot (V2d_View)
  myWidth  = Standard_ShortReal(aDX - 2*myPaperMargin);
  myHeight = Standard_ShortReal(aDY - 2*myPaperMargin);

  (*Cout()) << "%!PS-Adobe-" << endl;
  (*Cout()) << "%%Title: " << aName << endl;
  (*Cout()) << "%%Creator: " << P.UserName() << endl;
  (*Cout()) << "%%CreationDate: " << MONTHS[Date.Month()-1] << " "
            << Date.Day() << " " << Date.Year() << " " << Date.Hour()
            << ":" << Date.Minute() << ":" << Date.Second() << endl;
  (*Cout()) << "%%BeginProlog" << endl;
  (*Cout()) << "%%EndProlog" << endl;
  (*Cout()) << "%%BeginSetup" << endl;
  (*Cout()) << "/Centimeter {" << PPI CENTIMETER << " mul} def" << endl;
  (*Cout()) << "/Inch {72 mul} def" << endl;
  (*Cout()) << "/PaperWidth  " << TOCENTIMETER(thePaperHeight) << " Centimeter def" << endl;
  (*Cout()) << "/PaperHeight " << TOCENTIMETER(thePaperWidth ) << " Centimeter def" << endl;
  (*Cout()) << "/PaperMargin " << TOCENTIMETER(myPaperMargin ) << " Centimeter def" << endl;
  (*Cout()) << "/PaperRotateIfPossible true def" << endl;
  (*Cout()) << "/BD {bind def} bind def" << endl;
  (*Cout()) << "/IB " << IMAGEBUFFERSIZE << " string def" << endl;
  (*Cout()) << "/" << MOVETO << "{moveto} BD" << endl;
  (*Cout()) << "/" << RMOVETO << "{rmoveto} BD" << endl;
  (*Cout()) << "/" << LINETO << "{lineto} BD" << endl;
  (*Cout()) << "/" << RLINETO << "{rlineto} BD" << endl;
  (*Cout()) << "/" << CURVETO << "{curveto} BD" << endl;
  (*Cout()) << "/" << ARC << "{arc} BD" << endl;
  (*Cout()) << "/" << ARCN << "{arcn} BD" << endl;
  (*Cout()) << "/" << STROKE << "{stroke} BD" << endl;
  (*Cout()) << "/" << FILL << "{fill} BD" << endl;
  (*Cout()) << "/" << SHOW   << "{show} BD" << endl;
  (*Cout()) << "/" << ROTATE << "{rotate} BD" << endl;
  (*Cout()) << "/" << SCALE << "{scale} BD" << endl;
  (*Cout()) << "/" << GSAVE << "{gsave} BD" << endl;
  (*Cout()) << "/" << GRESTORE << "{grestore} BD" << endl;
  (*Cout()) << "/" << CHARPATH << "{charpath} BD" << endl;
  (*Cout()) << "/" << CLOSEPATH << "{closepath} BD" << endl;
  (*Cout()) << "/" << NEWPATH << "{newpath} BD" << endl;
  (*Cout()) << "/" << UNDERLINE << "{" << endl;
  (*Cout()) << "% Compute underline position depending of the baseline position" << endl;
  (*Cout()) << "  /size exch def" << endl;
  (*Cout()) << "  currentfont /FontBBox known {" << endl;
  (*Cout()) << "    currentfont /FontBBox get dup" << endl;
  (*Cout()) << "    1 get /ymin exch def" << endl;
  (*Cout()) << "    3 get /ymax exch def" << endl;
  (*Cout()) << "    ymin ymax ymin sub div 0.6 mul size mul" << endl;
  (*Cout()) << "  } { " << endl;
  (*Cout()) << "    ymax ymin sub 8 div size mul neg" << endl;
  (*Cout()) << "  } ifelse" << endl;
  (*Cout()) << "} bind def" << endl;
  (*Cout()) << "/" << FILLRECTANGLE << "{" << endl;
  (*Cout()) << "  /rxmin exch def               % lower-left corner" << endl;
  (*Cout()) << "  /rymin exch def               % lower-left corner" << endl;
  (*Cout()) << "  /rwidth exch def              % rect width" << endl;
  (*Cout()) << "  /rheight exch def             % rect height" << endl;
  (*Cout()) << "  /orientation exch def         % text angle" << endl;
  (*Cout()) << "  /framedattrib exch def        % framed attrib" << endl;
  (*Cout()) << "  /hidingattrib exch def        % hiding background attrib" << endl;
  (*Cout()) << "  gsave                         % get text bounding box" << endl;
  (*Cout()) << "  currentpoint /y exch def /x exch def  % get current position" << endl;
  (*Cout()) << "  orientation rotate             % rotate the text" << endl;
  (*Cout()) << "  hidingattrib cvx exec        % draw hiding bounding box" << endl;
  (*Cout()) << "  rxmin rymin rmoveto 0 rheight rlineto rwidth 0 rlineto" << endl;
  (*Cout()) << "  0 rheight neg rlineto rwidth neg 0 rlineto closepath fill" << endl;
  (*Cout()) << "  x y moveto framedattrib cvx exec  % draw framed bounding box" << endl;
  (*Cout()) << "  rxmin rymin rmoveto 0 rheight rlineto rwidth 0 rlineto" << endl;
  (*Cout()) << "  0 rheight neg rlineto rwidth neg 0 rlineto stroke" << endl;
  (*Cout()) << "  grestore" << endl;
  (*Cout()) << "} bind def" << endl;
  (*Cout()) << "/" << DRAWRECTANGLE << "{" << endl;
  (*Cout()) << "  /rxmin exch def               % lower-left corner" << endl;
  (*Cout()) << "  /rymin exch def               % lower-left corner" << endl;
  (*Cout()) << "  /rwidth exch def              % rect width" << endl;
  (*Cout()) << "  /rheight exch def             % rect height" << endl;
  (*Cout()) << "  /orientation exch def         % text angle" << endl;
  (*Cout()) << "  /framedattrib exch def        % framed attrib" << endl;
  (*Cout()) << "  gsave                         % get text bounding box" << endl;
  (*Cout()) << "  orientation rotate             % rotate the text" << endl;
  (*Cout()) << "  framedattrib cvx exec  % draw framed bounding box" << endl;
  (*Cout()) << "  rxmin rymin rmoveto 0 rheight rlineto rwidth 0 rlineto" << endl;
  (*Cout()) << "  0 rheight neg rlineto rwidth neg 0 rlineto stroke" << endl;
  (*Cout()) << "  grestore" << endl;
  (*Cout()) << "} bind def" << endl;
  (*Cout()) << "/" << SHOWHIDINGTEXT << "{" << endl;
  (*Cout()) << "  /textstring exch def         % text to edit" << endl;
  (*Cout()) << "  /isunderlined exch def       % underline text flag" << endl;
  (*Cout()) << "  /textangle exch def          % text angle" << endl;
  (*Cout()) << "  /textmargin exch def         % text margin" << endl;
  (*Cout()) << "  /textattrib exch def         % text attrib" << endl;
  (*Cout()) << "  /fontsize exch def           % font size" << endl;
  (*Cout()) << "  /hidingattrib exch def       % hiding background attrib" << endl;
  (*Cout()) << "  /framedattrib exch def       % framed attrib" << endl;
  (*Cout()) << "  gsave" << endl;
  (*Cout()) << "  gsave                        % get text bounding box" << endl;
  (*Cout()) << "  0 0 moveto" << endl;
  (*Cout()) << "  textstring false charpath pathbbox" << endl;
  (*Cout()) << "  /tymax exch def /txmax exch def /tymin exch def /txmin exch def" << endl;
  (*Cout()) << "  grestore" << endl;
  
  (*Cout()) << "  /twidth txmax txmin sub def   % set text width" << endl;
  (*Cout()) << "  /theight tymax tymin sub def   % set text height" << endl;
  (*Cout()) << "  isunderlined {       % adjust underline space" << endl;
  (*Cout()) << "    /umargin fontsize UnderLine def" << endl;
  (*Cout()) << "    umargin tymin lt {" << endl;
  (*Cout()) << "      /tymin umargin def" << endl;
  (*Cout()) << "      /theight tymax tymin sub def" << endl;
  (*Cout()) << "    } if" << endl;
  (*Cout()) << "  } if" << endl;
  
  (*Cout()) << "  /hmargin textmargin theight mul def" << endl;
  (*Cout()) << "  /hxmin txmin hmargin sub def" << endl;
  (*Cout()) << "  /hymin tymin hmargin sub def" << endl;
  (*Cout()) << "  /hxmax txmax hmargin add def" << endl;
  (*Cout()) << "  /hymax tymax hmargin add def" << endl;
  (*Cout()) << "  /hwidth hxmax hxmin sub def  % set hiding width" << endl;
  (*Cout()) << "  /hheight hymax hymin sub def % set hiding height" << endl;

  (*Cout()) << "  hidingattrib framedattrib textangle hheight hwidth hymin hxmin FillRectangle" << endl;
  (*Cout()) << "  textangle rotate             % rotate the text" << endl;
  (*Cout()) << "  currentpoint /ytext exch def /xtext exch def  % get text position" << endl;
  (*Cout()) << "  textstring textattrib cvx exec % draw text" << endl;
  (*Cout()) << "  isunderlined {" << endl;
  (*Cout()) << "    theight 32 div setlinewidth" << endl;
  (*Cout()) << "    xtext ytext moveto 0 umargin rmoveto twidth 0 rlineto stroke % draw underline" << endl;
  (*Cout()) << "  } if" << endl;
  
  (*Cout()) << "  grestore" << endl;
  (*Cout()) << "} bind def" << endl;

  (*Cout()) << "/" << SHOWFRAMEDTEXT << "{" << endl;
  (*Cout()) << "  /textstring exch def         % text to edit" << endl;
  (*Cout()) << "  /isunderlined exch def       % underline text flag" << endl;
  (*Cout()) << "  /textangle exch def          % text angle" << endl;
  (*Cout()) << "  /textmargin exch def         % text margin" << endl;
  (*Cout()) << "  /textattrib exch def         % text attrib" << endl;
  (*Cout()) << "  /fontsize exch def           % font size" << endl;
  (*Cout()) << "  /framedattrib exch def       % framed attrib" << endl;
  (*Cout()) << "  gsave" << endl;
  (*Cout()) << "  gsave                        % get text bounding box" << endl;
  (*Cout()) << "  0 0 moveto" << endl;
  (*Cout()) << "  textstring false charpath pathbbox" << endl;
  (*Cout()) << "  /tymax exch def /txmax exch def /tymin exch def /txmin exch def" << endl;
  (*Cout()) << "  grestore" << endl;
  
  (*Cout()) << "  /twidth txmax txmin sub def   % set text width" << endl;
  (*Cout()) << "  /theight tymax tymin sub def   % set text height" << endl;
  (*Cout()) << "  isunderlined {       % adjust underline space" << endl;
  (*Cout()) << "    /umargin fontsize UnderLine def" << endl;
  (*Cout()) << "    umargin tymin lt {" << endl;
  (*Cout()) << "      /tymin umargin def" << endl;
  (*Cout()) << "      /theight tymax tymin sub def" << endl;
  (*Cout()) << "    } if" << endl;
  (*Cout()) << "  } if" << endl;
  
  (*Cout()) << "  /hmargin textmargin theight mul def" << endl;
  (*Cout()) << "  /hxmin txmin hmargin sub def" << endl;
  (*Cout()) << "  /hymin tymin hmargin sub def" << endl;
  (*Cout()) << "  /hxmax txmax hmargin add def" << endl;
  (*Cout()) << "  /hymax tymax hmargin add def" << endl;
  (*Cout()) << "  /hwidth hxmax hxmin sub def  % set framed width" << endl;
  (*Cout()) << "  /hheight hymax hymin sub def % set framed height" << endl;

  (*Cout()) << "  framedattrib textangle hheight hwidth hymin hxmin DrawRectangle" << endl;
  (*Cout()) << "  currentpoint /ytext exch def /xtext exch def  % get text position" << endl;
  (*Cout()) << "  textangle rotate             % rotate the text" << endl;
  (*Cout()) << "  textstring textattrib cvx exec % draw text" << endl;
  (*Cout()) << "  isunderlined {" << endl;
  (*Cout()) << "    theight 32 div setlinewidth" << endl;
  (*Cout()) << "    xtext ytext moveto 0 umargin rmoveto twidth 0 rlineto stroke % draw underline" << endl;
  (*Cout()) << "  } if" << endl;
  
  (*Cout()) << "  grestore" << endl;
  (*Cout()) << "} bind def" << endl;

  (*Cout()) << "/" << SHOWUNDERLINEDTEXT << "{" << endl;
  (*Cout()) << "  /textstring exch def         % text to edit" << endl;
  (*Cout()) << "  /textangle exch def          % text angle" << endl;
  (*Cout()) << "  /textattrib exch def         % text attrib" << endl;
  (*Cout()) << "  /fontsize exch def           % font size" << endl;
  (*Cout()) << "  gsave" << endl;
  (*Cout()) << "  gsave                        % get text bounding box" << endl;
  (*Cout()) << "  0 0 moveto" << endl;
  (*Cout()) << "  textstring false charpath pathbbox" << endl;
  (*Cout()) << "  /tymax exch def /txmax exch def /tymin exch def /txmin exch def" << endl;
  (*Cout()) << "  grestore" << endl;
  
  (*Cout()) << "  /twidth txmax txmin sub def   % set text width" << endl;
  (*Cout()) << "  /theight tymax tymin sub def   % set text height" << endl;
  (*Cout()) << "  /umargin fontsize UnderLine def" << endl;
  
  (*Cout()) << "  textangle rotate             % rotate the text" << endl;
  (*Cout()) << "  currentpoint /ytext exch def /xtext exch def  % get text position" << endl;
  (*Cout()) << "  theight 32 div setlinewidth" << endl;
  (*Cout()) << "  textstring textattrib cvx exec % draw text" << endl;
  (*Cout()) << "  xtext ytext moveto 0 umargin rmoveto twidth 0 rlineto stroke % draw underline" << endl;

  (*Cout()) << "  grestore" << endl;
  (*Cout()) << "} bind def" << endl;
  (*Cout()) << "/" << SCALEFONT << "{" << endl;
  (*Cout()) << "% Adjust font scale depending of the baseline position" << endl;
  (*Cout()) << "  /size exch def" << endl;
  (*Cout()) << "  /ratio 1. def" << endl;
  (*Cout()) << "  currentfont /FontBBox known {" << endl;
  (*Cout()) << "    currentfont /FontBBox get dup" << endl;
  (*Cout()) << "    1 get /ymin exch def" << endl;
  (*Cout()) << "    3 get /ymax exch def" << endl;
  (*Cout()) << "    ymax ymin sub ymax div /ratio exch def" << endl;

  (*Cout()) << "    0 0 moveto" << endl;
  (*Cout()) << "    (H) false charpath pathbbox /hmax exch def" << endl;
  (*Cout()) << "    0 0 moveto" << endl;
  (*Cout()) << "    ($) false charpath pathbbox /dmax exch def" << endl;
  (*Cout()) << "    ratio hmax mul dmax div /ratio exch def" << endl;

  (*Cout()) << "  } if" << endl;
  (*Cout()) << "  currentfont ratio size mul scalefont setfont" << endl;
  (*Cout()) << "} bind def" << endl;
  (*Cout()) << "/" << SCALEDRAWING << "{" << endl;
  (*Cout()) << "% Adjust drawing size depending of paper size" << endl;
  (*Cout()) << "  /DrawHeight exch Centimeter def" << endl;
  (*Cout()) << "  /DrawWidth exch Centimeter def" << endl;
  (*Cout()) << "  /DrawScale 1 def" << endl;
  (*Cout()) << "  /NewPaperMargin PaperMargin def" << endl;
#ifdef CSR862
  (*Cout()) << "  /NewDrawWidth DrawWidth def" << endl;
  (*Cout()) << "  /NewDrawHeight DrawHeight def" << endl;
#else
  (*Cout()) << "  /NewDrawWidth DrawHeight def" << endl;
  (*Cout()) << "  /NewDrawHeight DrawWidth def" << endl;
#endif
  (*Cout()) << "  /NewPaperWidth PaperWidth 2 PaperMargin mul sub def" << endl;
  (*Cout()) << "  /NewPaperHeight PaperHeight 2 PaperMargin mul sub def" << endl;
  (*Cout()) << " /Helvetica-BoldOblique findfont 10 scalefont setfont" << endl ;
  (*Cout()) << " /Tampon 20 string def" << endl;
  (*Cout()) << "  DrawWidth DrawHeight gt PaperRotateIfPossible and {" << endl;
  (*Cout()) << "    NewDrawWidth NewPaperWidth gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperWidth NewDrawWidth div def" << endl; 
  (*Cout()) << "      /NewDrawWidth NewDrawWidth DrawScale mul def" << endl; 
  (*Cout()) << "      /NewDrawHeight NewDrawHeight DrawScale mul def" << endl; 
  (*Cout()) << "    } if" << endl;
  (*Cout()) << "    NewDrawHeight NewPaperHeight gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperHeight NewDrawHeight div DrawScale mul def" << endl; 
  (*Cout()) << "      /NewDrawWidth NewDrawWidth DrawScale mul def" << endl; 
  (*Cout()) << "      /NewDrawHeight NewDrawHeight DrawScale mul def" << endl; 
  (*Cout()) << "    } if" << endl;
  (*Cout()) << "    DrawScale 1. lt {" << endl;
  (*Cout()) << "      NewPaperMargin 0 M (Scale : ) show DrawScale Tampon cvs show" << endl;
  (*Cout()) << "    } if" << endl;
#ifdef CSR862
  (*Cout()) << "    NewDrawWidth 2 div NewDrawHeight 2 div translate" << endl;
#else
  (*Cout()) << "    {" << endl;
  (*Cout()) << "      /NewPaperMargin PaperMargin def" << endl;
  (*Cout()) << "      /NewPaperWidth PaperWidth def" << endl;
  (*Cout()) << "      /NewPaperHeight PaperHeight def" << endl;
  (*Cout()) << "    } ifelse" << endl;
#endif
  (*Cout()) << "    90 rotate" << endl;
#ifdef CSR862
  (*Cout()) << "    NewDrawHeight neg 2 div NewPaperMargin add NewDrawWidth 2 div NewPaperHeight sub NewPaperMargin sub translate" << endl;
#else
  (*Cout()) << "    PaperMargin NewDrawWidth PaperMargin add neg translate" << endl;
#endif
  (*Cout()) << "  }{" << endl;
#ifdef CSR862
  (*Cout()) << "    NewDrawWidth NewPaperHeight gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperHeight NewDrawWidth div def" << endl; 
#else
  (*Cout()) << "    NewDrawHeight NewPaperHeight gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperHeight NewDrawHeight div def" << endl; 
#endif
  (*Cout()) << "      /NewDrawWidth NewDrawWidth DrawScale mul def" << endl; 
  (*Cout()) << "      /NewDrawHeight NewDrawHeight DrawScale mul def" << endl; 
  (*Cout()) << "    } if" << endl;
#ifdef CSR862
  (*Cout()) << "    NewDrawHeight NewPaperWidth gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperWidth NewDrawHeight div DrawScale mul def" << endl; 
#else
  (*Cout()) << "    NewDrawWidth NewPaperWidth gt {" << endl;
  (*Cout()) << "      /DrawScale NewPaperWidth NewDrawWidth div DrawScale mul def" << endl; 
#endif
  (*Cout()) << "      /NewDrawWidth NewDrawWidth DrawScale mul def" << endl; 
  (*Cout()) << "      /NewDrawHeight NewDrawHeight DrawScale mul def" << endl; 
  (*Cout()) << "    } if" << endl;
  (*Cout()) << "    DrawScale 1. lt {" << endl;
  (*Cout()) << "      NewPaperMargin 0 M (Scale : ) show DrawScale Tampon cvs show" << endl;
  (*Cout()) << "    } {" << endl;
  (*Cout()) << "      /NewPaperMargin PaperMargin def" << endl;
  (*Cout()) << "      /NewPaperWidth PaperWidth def" << endl;
  (*Cout()) << "      /NewPaperHeight PaperHeight def" << endl;
  (*Cout()) << "    } ifelse" << endl;
  (*Cout()) << "    NewPaperMargin NewPaperMargin translate" << endl; 
  (*Cout()) << "  } ifelse" << endl;
  (*Cout()) << "% set scale" << endl;
  (*Cout()) << " DrawScale DrawScale scale" << endl; 
  (*Cout()) << "% Draw frame and set clip path" << endl;
  (*Cout()) << " 0 0 M 0 DrawHeight L DrawWidth DrawHeight L DrawWidth 0 L 0 0 L ST" << endl;
  (*Cout()) << " NP 0 0 M 0 DrawHeight L DrawWidth DrawHeight L DrawWidth 0 L 0 0 L CLP clip NP" << endl;
#ifdef BUC60772
  (*Cout()) << " 2 setlinejoin" << endl;
#endif
  (*Cout()) << "} bind def" << endl;

  myCurrentPage = 0;
} 

//==========================================================================
void PS_Driver::BeginDraw ()
{ 
  // Text management
  myTextManager = new PlotMgt_TextManager(this);
  if (myCurrentPage <= 0) {
    (*Cout()) << "%%BeginSetup" << endl;
    myCurrentPage = 1;
  }
  (*Cout()) << "%%Page: " << myCurrentPage << endl;
  (*Cout()) << "GS " << TOCENTIMETER(myWidth) << " "
            << TOCENTIMETER(myHeight)  << " " << SCALEDRAWING << endl;
}

//==========================================================================
void PS_Driver::EndDraw (const Standard_Boolean dontFlush)
{
  (*Cout ()) << " showpage" << endl;
  (*Cout ()) << " GR" << endl;
  if (!dontFlush)
    (*Cout ()) << flush;
  myCurrentPage++;
  myImage.Nullify ();
}

//==========================================================================
void PS_Driver::InitializeColorMap (const Handle(Aspect_ColorMap)& aColorMap)
{ 
  Standard_Real r,g,b;
  Standard_Integer index,Size = aColorMap->Size();
  Aspect_ColorMapEntry entry ;
  Quantity_Color color ;

  if (myTypeOfColorSpace == Aspect_TOCS_GreyScale)
    (*Cout()) << "/CB {1 setgray} BD" << endl;
  else
    (*Cout()) << "/CB {1 1 1 setrgbcolor} BD" << endl;

  for (Standard_Integer i=1; i<= Size; i++) {
    entry = aColorMap->Entry(i);
    index = entry.Index();
    color = entry.Color();
    color.Values (r, g, b, Quantity_TOC_RGB);
    if (myTypeOfColorSpace == Aspect_TOCS_GreyScale)
      (*Cout()) << "/C" << index << " {" << (r + g + b)/3. << " setgray} BD" << endl;
    else if (myTypeOfColorSpace == Aspect_TOCS_BlackAndWhite)
      (*Cout()) << "/C" << index << " {0 0 0 setrgbcolor} BD" << endl;
    else
      (*Cout()) << "/C" << index << " {" << r << " " << g << " "
                << b << " setrgbcolor} BD" << endl;
  }
  if (myTypeOfColorSpace == Aspect_TOCS_BlackAndWhite)
    myTypeOfColorSpace = Aspect_TOCS_RGB;
}

//==========================================================================
void PS_Driver::InitializeTypeMap (const Handle(Aspect_TypeMap)& aTypeMap)
{ 
  Standard_Integer Size = aTypeMap->Size();
  Aspect_LineStyle aStyle;
  for (Standard_Integer i=1; i<= Size; i++) {
    (*Cout()) << "/D" << aTypeMap->Entry(i).Index() << " {[";
    aStyle =  aTypeMap->Entry(i).Type();
    for (Standard_Integer j = aStyle.Values().Lower(); j < aStyle.Values().Upper(); j += 2)
      (*Cout()) << Standard_Real(Convert(aStyle.Values().Value(j  ))) << " "
                << Standard_Real(Convert(aStyle.Values().Value(j+1))) << " ";
    (*Cout()) << "] 0 setdash} BD" << endl;
  }
}

//==========================================================================
void PS_Driver::InitializeWidthMap (const Handle(Aspect_WidthMap)& aWidthMap) 
{
  Standard_Integer Size = aWidthMap->Size();
  for (Standard_Integer i=1; i<= Size; i++) {
    Standard_Real w = aWidthMap->Entry(i).Width() / myPixelSize;
    (*Cout()) << "/W" << aWidthMap->Entry(i).Index() << " {"
              << w << " setlinewidth} BD" << endl;
  }
}

//==========================================================================
void PS_Driver::InitializeFontMap (const Handle(Aspect_FontMap)& aFontMap)
{
#ifdef BUC60766
    PlotMgt_PlotterDriver::InitializeFontMap (aFontMap);
#else
  if (UseMFT()) {
    PlotMgt_PlotterDriver::InitializeFontMap (aFontMap);
  } else
#endif
  {
    Aspect_FontMapEntry entry;
    Standard_Integer index, iindex;

    TCollection_AsciiString aname;
    Aspect_FontStyle style;
    Standard_Boolean theCapsHeight;
    Quantity_PlaneAngle theFontSlant;
    Quantity_Length theFontSize;
    Standard_ShortReal fsize;

    for (index = 1; index <= aFontMap->Size(); index++) {
      entry = aFontMap->Entry(index);
      iindex = entry.Index();
      style = entry.Type();
      theFontSize = TOMILLIMETER(style.Size());
      theFontSlant = style.Slant();
      aname = style.AliasName();
      theCapsHeight = style.CapsHeight();
      fsize = theFontSize;
      if (fsize > 0.0001)
        fsize = Convert(fsize);
      else fsize = 1.;
    
      (*Cout()) << "/F" << iindex;
      (*Cout()) << " {/scf exch def /mat exch def" << endl;

      if (!aname.Length() || (aname == "Default") || (aname == "Defaultfont"))
        aname = "Courier";

      (*Cout()) << "/" << aname << " findfont mat makefont ";
      if (theCapsHeight) (*Cout()) << "setfont scf " << SCALEFONT << "} BD " << endl;
      else               (*Cout()) << "scf scalefont setfont} BD " << endl;
      (*Cout()) << "/FSZ" << iindex << " " << Standard_Real(fsize) << " def" << endl;
    }
  }
}

//==========================================================================
void PS_Driver::InitializeMarkMap (const Handle(Aspect_MarkMap)& aMarkMap)
{
  Standard_Integer i,j,n,sl,np,Size = aMarkMap->Size();
  Standard_Real dx,dy,x,y;
  Standard_Boolean fl;

  for (i = 1; i <= Size; i++) {
    const TShort_Array1OfShortReal& amx = aMarkMap->Entry(i).Style().XValues();
    const TShort_Array1OfShortReal& amy = aMarkMap->Entry(i).Style().YValues();
    const TColStd_Array1OfBoolean& ams = aMarkMap->Entry(i).Style().SValues();
    (*Cout()) << "/MRK" << aMarkMap->Entry(i).Index() << " {" << endl;
    x = y = 0.;
    sl = amx.Lower();
    np = 0;
    fl = Standard_True;
    for (n=1, j=sl; j <= amx.Upper(); j++, n++) {
      dx = amx(j) - x;
      dy = amy(j) - y;
      x = amx(j);
      y = amy(j);
      (*Cout()) << dx << " " << dy << " ";
      if (ams(j)) {
        if (fl) np++;
        (*Cout()) << RLINETO;
      } else {
        if (np) fl = Standard_False;
        else sl = j;
        (*Cout()) << RMOVETO;
      }
      if (!(n % 4)) (*Cout()) << endl;
    }
    (*Cout()) << "} BD" << endl;
    (*Cout()) << "/FRMK" << aMarkMap->Entry(i).Index() << " {" << endl;
    if (np > 1) {
      x = y = 0.;
      for (n=1, j=sl; j <= sl+np; j++, n++) {
        dx = amx(j) - x;
        dy = amy(j) - y;
        x = amx(j);
        y = amy(j);
        (*Cout()) << dx << " " << dy << " ";
        if( ams(j) ) {
          (*Cout()) << RLINETO;
        } else {
          (*Cout()) << RMOVETO;
        }
        if (!(n % 4))
          (*Cout()) << endl;
      }
      (*Cout()) << CLOSEPATH;
    }
    (*Cout()) << "} BD " << endl;
  }
}

//==========================================================================
// Category: Methods to set the attributes
//==========================================================================
void PS_Driver::SetTextAttrib (const Standard_Integer ColorIndex,
                               const Standard_Integer FontIndex)
{
  PlotMgt_PlotterDriver::SetTextAttrib (ColorIndex, FontIndex);
#ifndef BUC60766
  if (!UseMFT())
#endif
    (*Cout()) << " [1 0 0 1 0 0 ] FSZ" << FontIndex << " F" << FontIndex << " ";
}

//==========================================================================
void PS_Driver::SetTextAttrib (const Standard_Integer ColorIndex,
                               const Standard_Integer FontIndex,
                               const Quantity_PlaneAngle aSlant,
                               const Quantity_Factor aHScale,
                               const Standard_Real aWScale,
                               const Standard_Boolean isUnderlined)
{
  PlotMgt_PlotterDriver::SetTextAttrib (
    ColorIndex, FontIndex, aSlant, aHScale, aWScale, isUnderlined);
#ifndef BUC60766
  if (!UseMFT())
#endif
    (*Cout()) << " [" << myTextWScale << " 0 " << Sin(aSlant)*myTextHScale
              << " " << Cos(aSlant)*myTextHScale << " 0 0] FSZ" << FontIndex
              << " F" << FontIndex << " ";
}

//==========================================================================
// Image methods
//==========================================================================
Standard_Boolean PS_Driver::SizeOfImageFile(const Standard_CString anImageFile,
                                            Standard_Integer &aWidth,
                                            Standard_Integer &aHeight) const
{
  return AlienImage::LoadImageFile (anImageFile, myImage, aWidth, aHeight);
}
 
//==========================================================================
Standard_Boolean PS_Driver::PlotImage (const Standard_ShortReal aX,
                                       const Standard_ShortReal aY,
                                       const Standard_ShortReal aWidth,
                                       const Standard_ShortReal aHeight,
                                       const Standard_ShortReal aScale,
                                       const Standard_CString anImageFile,
                                       const Standard_Address anArrayOfPixels,
                                       const Standard_Integer aLineIndex)
{
  Standard_Integer LowX = 0, LowY = 0;
  Standard_Integer width  = Standard_Integer(aWidth),
                   height = Standard_Integer(aHeight);
  Standard_Boolean status;
  Aspect_RGBPixel* g2dp = (Aspect_RGBPixel*) anArrayOfPixels;
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
  // Draw the image
  if (status) {
    Quantity_Color color;
    Standard_Real r, g, b;
    Standard_Integer n=0,x,y,red,green,blue;
    Standard_ShortReal wscale, hscale;
    wscale = hscale = aScale * myPixelSize * PPI;
    if (aLineIndex == -1) {
      (*Cout()) << " " << GSAVE << MapX(aX) - wscale*width/2. << " "
                << MapY(aY) - hscale*height/2.;
    } else {
      (*Cout()) << " " << GSAVE << MapX(aX) - wscale*width/2. << " "
                << MapY(aY) + hscale*(height/2. - aLineIndex);
      height = 1;
    }
    (*Cout()) << " translate " << wscale*width << " " << hscale*height << " scale ";
    (*Cout()) << width << " " << height << " 8 [" << width
              << " 0 0 " << -height << " 0 " << height
              << "] {currentfile IB readhexstring pop} false 3 colorimage " << endl;
    Standard_Integer r1, r2, g1, g2, b1, b2;
    char hstring[7];
    for (y=0; y < height; y++) {
      for (x=n=0; x < width; x++) {
        if (anImageFile) {
          color = myImage -> PixelColor (x + LowX, y + LowY);
          color.Values (r, g, b, Quantity_TOC_RGB);
        } else {
          r = g2dp -> red;
          g = g2dp -> green;
          b = g2dp -> blue;
          g2dp++;
        }
        red   = (int)(r * 255.);
        green = (int)(g * 255.);
        blue  = (int)(b * 255.);

        if (anImageFile) {
          r1 = (red >> 4) & 0xF;   r2 = red & 0xF;
          g1 = (green >> 4) & 0xF; g2 = green & 0xF;
          b1 = (blue >> 4) & 0xF;  b2 = blue & 0xF;
        } else {
          r1 = red >> 4;   r2 = red & 0xF;
          g1 = green >> 4; g2 = green & 0xF;
          b1 = blue >> 4;  b2 = blue & 0xF;
        }
        sprintf(hstring, "%X%X%X%X%X%X", r1, r2, g1, g2, b1, b2);
        (*Cout()) << hstring;
        n += 3;
        if (n >= IMAGEBUFFERSIZE) {
          n = 0;
          (*Cout()) << endl;
        }
      }
    }
    if (n > 0) {
      while (n < IMAGEBUFFERSIZE) {
        (*Cout()) << "00"; n++;
      }
    }
    (*Cout()) << endl;
    (*Cout()) << " " << GRESTORE << endl;
    return Standard_True;
  } 
  return Standard_False;
}

//=============================================================
// Category: Methods to write attributes
//=============================================================
void PS_Driver::PlotLineAttrib (const Standard_Integer ColorIndex,
                                const Standard_Integer TypeIndex,
                                const Standard_Integer WidthIndex)
{
  if ((myColorIndex != ColorIndex) && 
      (myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite)) {
    myColorIndex = ColorIndex;
    (*Cout()) << " C" << myColorIndex << " ";
  }
  if (myTypeIndex != TypeIndex) {
    myTypeIndex = TypeIndex;
    (*Cout()) << " D" << myTypeIndex << " ";
  }
  if (myWidthIndex != WidthIndex) {
    myWidthIndex = WidthIndex;
    (*Cout()) << " W" << myWidthIndex << " ";
  }
}

//=============================================================
// Category: Methods to draw primitives
//=============================================================

//=============================================================
Standard_Boolean PS_Driver::PlotPoint (const Standard_ShortReal X,
                                       const Standard_ShortReal Y)
{
  (*Cout()) << X << " " << Y << " " << MOVETO << " "
            << X << " " << Y << " " << LINETO << " "
            << STROKE << endl;
  return Standard_True;
}

//=============================================================
Standard_Boolean PS_Driver::PlotSegment (const Standard_ShortReal X1,
                                         const Standard_ShortReal Y1,
                                         const Standard_ShortReal X2,
                                         const Standard_ShortReal Y2)
{
  (*Cout()) << X1 << " " << Y1 << " " << MOVETO << " "
            << X2 << " " << Y2 << " " << LINETO << " "
            << STROKE << endl;
  return Standard_True;
}

//=============================================================
#define _XP(idx) ((float*)xArray)[(idx)]
#define _YP(idx) ((float*)yArray)[(idx)]
//=============================================================
Standard_Boolean PS_Driver::PlotPolyline (const Standard_Address xArray,
                                          const Standard_Address yArray,
                                          const Standard_Address nPts,
                                          const Standard_Integer nParts)
{
  // Multipart drawing can only be used for text drawing
  // (called from PlotMgt_TextManager::EndChar ()
  int N, cpN, i, j;
  for (N = 0, i = 0; i < nParts; i++) {
    cpN = ((int*)nPts)[i];
    (*Cout()) << _XP(N) << " " << _YP(N) << " " << MOVETO;
    for (j = N + 1; j < N + cpN; j++)
      (*Cout()) << _XP(j) << " " << _YP(j) << " " << LINETO;
    (*Cout()) << " " << STROKE << endl;
    N += cpN;
  }
  return Standard_True;
}

//=============================================================
Standard_Boolean PS_Driver::PlotPolygon (const Standard_Address xArray,
                                         const Standard_Address yArray,
                                         const Standard_Address nPts,
                                         const Standard_Integer nParts)
{
  Standard_Integer i;
  if (nParts == 1) {
    if (myPolyTileIndex >= 0) {
      PLOT_PCOLOR (myPolyColorIndex);
      (*Cout()) << _XP(0) << " " << _YP(0) << " " << MOVETO;
      for (i = 1; i < ((int*)nPts)[0]; i++)
        (*Cout()) << _XP(i) << " " << _YP(i) << " " << LINETO;
      (*Cout()) << CLOSEPATH << FILL << endl;

      if (myPolyEdgeFlag) {
POLYLN1:
        PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
        PlotPolyline   (xArray, yArray, nPts, nParts);
      }
    } else
      goto POLYLN1;
  } else {
    // This can only be used for text drawing
    // (called from PlotMgt_TextManager::EndChar ()
    PLOT_PCOLOR (myPolyColorIndex);
    int N, cpN, j;
    for (N = 0, i = 0; i < nParts; i++) {
      cpN = ((int*)nPts)[i];
      (*Cout()) << _XP(N) << " " << _YP(N) << " " << MOVETO;
      for (j = N + 1; j < N + cpN; j++)
        (*Cout()) << _XP(j) << " " << _YP(j) << " " << LINETO;
      (*Cout()) << CLOSEPATH << endl;
      N += cpN;
    }
    if (myPolyTileIndex == -1) (*Cout()) << STROKE << endl;
    else                       (*Cout()) << FILL << endl;
  }
  return Standard_True;
}
//=============================================================
#undef _XP
#undef _YP

//=============================================================
Standard_Boolean PS_Driver::PlotArc (const Standard_ShortReal Xpos,
                                     const Standard_ShortReal Ypos,
                                     const Standard_ShortReal aXradius,
                                     const Standard_ShortReal aYradius,
                                     const Standard_ShortReal sAngle,
                                     const Standard_ShortReal oAngle)
{
  Standard_Real san = sAngle;
  Standard_Real fan = sAngle + oAngle;

  if (Abs(aXradius-aYradius) <= 1.) {
    (*Cout()) << Xpos + aXradius*Cos(san) << " "
              << Ypos + aYradius*Sin(san) << " " << MOVETO;
    (*Cout()) << GSAVE << Xpos << " " << Ypos << " " << aXradius << " ";

    if (Abs(fan)-Abs(san) >= 2.*M_PI) {
      (*Cout()) << " 0 360 " << ARC << STROKE << GRESTORE << endl;
    } else {
      Standard_Real a1 = san/DRAD;
      Standard_Real a2 = fan/DRAD;
      (*Cout()) << a1 << " " << a2 << " " << ARC << STROKE << GRESTORE << endl;
    }
    return Standard_True;
  }
  return Standard_False;
}

//=============================================================
Standard_Boolean PS_Driver::PlotPolyArc (const Standard_ShortReal Xpos,
                                         const Standard_ShortReal Ypos,
                                         const Standard_ShortReal aXradius,
                                         const Standard_ShortReal aYradius,
                                         const Standard_ShortReal sAngle,
                                         const Standard_ShortReal oAngle)
{
  Standard_Real san = sAngle;
  Standard_Real fan = sAngle + oAngle;

  if (Abs(aXradius-aYradius) <= 1.) {
    PLOT_PCOLOR (myPolyColorIndex);
    if (Abs(oAngle) < 2.*M_PI)
      (*Cout()) << Xpos << " " << Ypos << " " << MOVETO;
    else
      (*Cout()) << Xpos + aXradius*Cos(san) << " "
                << Ypos + aXradius*Sin(san) << " " << MOVETO;
    (*Cout())  << GSAVE << Xpos << " " << Ypos << " " << aXradius << " ";

    if (Abs(fan) - Abs(san) >= 2.*M_PI) {
      (*Cout()) << " 0 360 " << ARC << FILL << GRESTORE << endl;
    } else {
      Standard_Real a1 = san/DRAD;
      Standard_Real a2 = fan/DRAD;
      (*Cout()) << a1 << " " << a2 << " " << ARC << FILL << GRESTORE << endl;
    }
    if (myPolyEdgeFlag) {
      PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
      return PlotArc (Xpos, Ypos, aXradius, aYradius, sAngle, oAngle);
    }
    return Standard_True;
  }
  return Standard_False;
}

//=============================================================
Standard_Boolean PS_Driver::PlotMarker (const Standard_Integer aMarker,
                                        const Standard_ShortReal Xpos,
                                        const Standard_ShortReal Ypos,
                                        const Standard_ShortReal Width,
                                        const Standard_ShortReal Height,
                                        const Standard_ShortReal Angle)
{
  Standard_Real w = Convert(Width/2.);
  Standard_Real h = Convert(Height/2.);
  Standard_Real a = Angle/DRAD;

  if (aMarker >= 0 && w > 0. && h > 0.) {
    while (a > 360.)  a -= 360.;
    while (a < -360.) a += 360.;
    if (myMarkerFillFlag) {
      PlotLineAttrib (myPolyColorIndex, myLineTypeIndex, myLineWidthIndex);
      (*Cout()) << GSAVE << Xpos << " " << Ypos << " " << MOVETO;
      if (Angle != 0.)
        (*Cout()) << a << " " << ROTATE;
      (*Cout()) << w << " " << h << " " << SCALE << "FRMK"
                << aMarker << " " << FILL << GRESTORE << endl;
    }

    PlotLineAttrib (myMarkerColorIndex, 0, myMarkerWidthIndex);
    (*Cout()) << GSAVE << Xpos << " " << Ypos << " " << MOVETO;
    if (Angle != 0.)
      (*Cout()) << a << " " << ROTATE;
    (*Cout()) << w << " " << h << " " << SCALE << " MRK" << aMarker << " "
              << 1./w << " " << 1./h << " " << SCALE << STROKE << GRESTORE << endl;
  } else {
    DrawPoint (Xpos,Ypos);
  }
  return Standard_True;
}

//=============================================================
Standard_Boolean PS_Driver::PlotText (const TCollection_ExtendedString& aText,
                                      const Standard_ShortReal Xpos,
                                      const Standard_ShortReal Ypos,
                                      const Standard_ShortReal Angle,
                                      const Aspect_TypeOfText aType)
{
  if( aText.IsAscii() ) {
    TCollection_AsciiString atext(aText,'?');
    PlotText(atext.ToCString(),Xpos,Ypos,Angle,aType);
  } else {
#ifdef BUC60766
    return Standard_False;	// Use MFT fonts
#else
    Standard_CString ptext = Aspect::ToCString(aText);
    Standard_Real a = Angle/DRAD;
    Standard_Integer i,h1,h2;
    char hstring[3];
    while (a >  360.) a -= 360.;
    while (a < -360.) a += 360.;
    PLOT_PCOLOR (myTextColorIndex);
    if (myTextIsUnderlined) {
      (*Cout())  << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
      (*Cout()) << "FSZ" << myFontIndex << " " << myTextHScale << " mul (";
      (*Cout()) << "C" << myTextColorIndex;
      if (aType == Aspect_TOT_OUTLINE) (*Cout()) << " true " << CHARPATH << STROKE << ") ";
      else                             (*Cout()) << " " << SHOW << ") ";
      (*Cout()) << a << "<";
      for (i=0; i<strlen(ptext); i++) {
        h1 = (ptext[i] >> 4) & 0xF; h2 = ptext[i] & 0xF;
        sprintf(hstring,"%X%X",h1,h2);
        (*Cout()) << hstring;
      }
      (*Cout()) << "> " << SHOWUNDERLINEDTEXT << endl;
    } else {
      if (Angle == 0.) (*Cout()) << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
      else             (*Cout()) << GSAVE << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO
                                 << a << " " << ROTATE;
      (*Cout()) << "<";
      for (i=0; i<strlen(ptext); i++) {
        h1 = (ptext[i] >> 4) & 0xF; h2 = ptext[i] & 0xF;
        sprintf(hstring,"%X%X",h1,h2);
        (*Cout()) << hstring;
      }
      (*Cout()) << "> ";
      if (aType == Aspect_TOT_OUTLINE) (*Cout()) << "true " << CHARPATH << STROKE;
      else                             (*Cout()) << SHOW;
      if (Angle == 0.) (*Cout()) << endl;
      else             (*Cout()) << GRESTORE << endl;
    }
    return Standard_True;
#endif
  }
  return Standard_False;
}

//=============================================================
Standard_Boolean PS_Driver::PlotText (const Standard_CString aText,
                                      const Standard_ShortReal Xpos,
                                      const Standard_ShortReal Ypos,
                                      const Standard_ShortReal Angle,
                                      const Aspect_TypeOfText aType)
{
#ifdef BUC60766
  {
    Standard_Real a = Angle/DRAD;
    while (a >  360.) a -= 360.;
    while (a < -360.) a += 360.;
    PLOT_PCOLOR (myTextColorIndex);
    if (myTextIsUnderlined) {
      (*Cout())  << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
      (*Cout()) << "FSZ" << myFontIndex << " " << myTextHScale << " mul (";
      (*Cout()) << "C" << myTextColorIndex;
      if (aType == Aspect_TOT_OUTLINE) (*Cout()) << " true " << CHARPATH << STROKE << ") ";
      else                             (*Cout()) << " " << SHOW << ") ";
      (*Cout()) << a << " (" << aText << ") " << SHOWUNDERLINEDTEXT << endl;
    } else {
      if (Angle == 0.) (*Cout()) << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
      else             (*Cout()) << GSAVE << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO
                                 << a << " " << ROTATE;
      (*Cout()) << "(" << aText << ") ";
      if (aType == Aspect_TOT_OUTLINE) (*Cout()) << "true " << CHARPATH << STROKE;
      else                             (*Cout()) << SHOW;
      if (Angle == 0.) (*Cout()) << endl;
      else             (*Cout()) << GRESTORE << endl;
    }
    return Standard_True;
  }
#else
  return Standard_False;
#endif
}

//=============================================================
Standard_Boolean PS_Driver::PlotPolyText (const TCollection_ExtendedString& aText,
                                          const Standard_ShortReal Xpos,
                                          const Standard_ShortReal Ypos,
                                          const Quantity_Factor aMargin,
                                          const Standard_ShortReal Angle,
                                          const Aspect_TypeOfText aType)
{
  if( aText.IsAscii() ) {
    TCollection_AsciiString atext(aText,'?');
    PlotPolyText(atext.ToCString(),Xpos,Ypos,aMargin,Angle,aType);
  } else {
#ifdef BUC60766
    return Standard_False;
#else
    Standard_Real a = Angle/DRAD;
    while( a > 360. ) a -= 360.;
    while( a < -360. ) a += 360.;
    (*Cout())  << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
    {
      Standard_CString ptext = Aspect::ToCString(aText);
      Standard_Integer i,h1,h2;
      char hstring[3];
      if( myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite ) {
        if( myLineColorIndex > 0 && myPolyEdgeFlag) {
          (*Cout()) << "(C" << myLineColorIndex;
        } else {
          (*Cout()) << "(CB";
        }
        (*Cout()) << " W" << myLineWidthIndex << ")";
        if( myPolyTileIndex >= 0 ) {
          if( myPolyColorIndex > 0 ) {
            (*Cout()) << "(C" << myPolyColorIndex << ")";
          } else {
            (*Cout()) << "(CB)";
          }
        }
        (*Cout()) << " FSZ" << myFontIndex << " " << myTextHScale << " mul ";
        if( myTextColorIndex > 0 ) {
          (*Cout()) << "(C" << myTextColorIndex;
        } else {
          (*Cout()) << "(CB";
        }
      } else {
        if( myPolyTileIndex >= 0 ) {
          (*Cout()) << "(W" << myLineWidthIndex << ") ()";
        } else {
          (*Cout()) << "(W" << myLineWidthIndex << ")";
        }
        (*Cout()) << " FSZ" << myFontIndex << " " << myTextHScale << " mul (";
      }
      if( aType == Aspect_TOT_OUTLINE ) {
        (*Cout()) << " true " << CHARPATH << STROKE << ") ";
      } else {
        (*Cout()) << " " << SHOW << ") ";
      }
      (*Cout()) << aMargin << " " << a;
      if( myTextIsUnderlined ) (*Cout()) << " true <";
      else (*Cout()) << " false <";
      for( i=0 ; i<strlen(ptext) ; i++ ) {
        h1 = (ptext[i] >> 4) & 0xF; h2 = ptext[i] & 0xF;
        sprintf(hstring,"%X%X",h1,h2);
        (*Cout()) << hstring;
      }
      if( myPolyTileIndex >= 0 && myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite ) {
        (*Cout()) << "> " << SHOWHIDINGTEXT << endl;
      } else {
        (*Cout()) << "> " << SHOWFRAMEDTEXT << endl;
      }
    }
    return Standard_True;
#endif
  }
  return Standard_False;
}

//=============================================================
Standard_Boolean PS_Driver::PlotPolyText (const Standard_CString aText,
                                          const Standard_ShortReal Xpos,
                                          const Standard_ShortReal Ypos,
                                          const Quantity_Factor aMargin,
                                          const Standard_ShortReal Angle,
                                          const Aspect_TypeOfText aType)
{
#ifdef BUC60766
  Standard_Real a = Angle/DRAD;
  while( a > 360. ) a -= 360.;
  while( a < -360. ) a += 360.;
  (*Cout())  << MapX(Xpos) << " " << MapY(Ypos) << " " << MOVETO;
  {
    if( myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite ) {
      if( myLineColorIndex > 0 && myPolyEdgeFlag) {
          (*Cout()) << "(C" << myLineColorIndex;
        } else {
          (*Cout()) << "(CB";
        }
        (*Cout()) << " W" << myLineWidthIndex << ")";
        if( myPolyTileIndex >= 0 ) {
          if( myPolyColorIndex > 0 ) {
            (*Cout()) << "(C" << myPolyColorIndex << ")";
          } else {
            (*Cout()) << "(CB)";
          }
        }
        (*Cout()) << " FSZ" << myFontIndex << " " << myTextHScale << " mul ";
        if( myTextColorIndex > 0 ) {
          (*Cout()) << "(C" << myTextColorIndex;
        } else {
          (*Cout()) << "(CB";
        }
    } else {
      if( myPolyTileIndex >= 0 ) {
        (*Cout()) << "(W" << myLineWidthIndex << ") ()";
      } else {
        (*Cout()) << "(W" << myLineWidthIndex << ")";
      }
      (*Cout()) << " FSZ" << myFontIndex << " " << myTextHScale << " mul (";
    }
    if( aType == Aspect_TOT_OUTLINE ) {
      (*Cout()) << " true " << CHARPATH << STROKE << ") ";
    } else {
      (*Cout()) << " " << SHOW << ") ";
    }
    (*Cout()) << aMargin << " " << a;
    if( myTextIsUnderlined ) (*Cout()) << " true";
    else (*Cout()) << " false";
    if( myPolyTileIndex >= 0 && myTypeOfColorSpace != Aspect_TOCS_BlackAndWhite ) {
      (*Cout()) << " (" << aText << ") " << SHOWHIDINGTEXT << endl;
    } else {
      (*Cout()) << " (" << aText << ") " << SHOWFRAMEDTEXT << endl;
    }
  }
  return Standard_True;
#else
  return Standard_False;
#endif
}

//=============================================================
Standard_ShortReal PS_Driver::Convert(const Standard_ShortReal aShortreal) const 
{
  return aShortreal * PPI;
}

//===============================================================================
Standard_Boolean PS_Driver::DrawCurveCapable () const
{
  return Standard_False;
}

//=============================================================
Standard_ShortReal PS_Driver::MapX (const Standard_ShortReal aShortreal) const
{
  return aShortreal * PPI;
}

//=============================================================
Standard_ShortReal PS_Driver::MapY (const Standard_ShortReal aShortreal) const
{
  return aShortreal * PPI;
}
