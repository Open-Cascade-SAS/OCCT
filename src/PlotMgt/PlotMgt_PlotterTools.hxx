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

#ifndef PLOTMGT_PLOTTERTOOLS
#define PLOTMGT_PLOTTERTOOLS

#include <Standard_Stream.hxx>

#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_FileIterator.hxx>
#include <PlotMgt_TypeOfPlotterParameter.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <stdio.h>
#ifndef WNT
# include <strings.h>
# include <unistd.h>
#else
# define STRICT
# include <windows.h>
# pragma comment (lib,"GDI32.LIB")
#endif // WNT

#define _T_UNK   PlotMgt_TOPP_Undefined
#define _T_INT   PlotMgt_TOPP_Integer
#define _T_REAL  PlotMgt_TOPP_Real
#define _T_BOOL  PlotMgt_TOPP_Boolean
#define _T_STR   PlotMgt_TOPP_String
#define _T_LSTR  PlotMgt_TOPP_ListString

#define _TYPE_SIGN    ".Type"
#define _DIALOG_SIGN  ".Dialog"
#define _MINVAL_SIGN  ".Min"
#define _MAXVAL_SIGN  ".Max"
#define _VALUES_SIGN  ".Values"
#define _LENGTH_SIGN  ".Length"
#define _DELIM_SIGN   " : "

typedef struct _ParameterDesc {
  Standard_CString               _new_name;
  Standard_CString               _old_name;
  PlotMgt_TypeOfPlotterParameter _type;
  Standard_Integer               _ismap;
} ParameterDesc;

extern ParameterDesc __PossibleParameters[];

//==============================================================================
#define PLOT_CONFIGVER       "ConfigurationVersion"
#define PLOT_DIALOGUNIT      "DialogUnit"
#define PLOT_MODEL           "Model"
#define PLOT_OUTFORMAT       "OutputFormat"
#define PLOT_IMGFORMAT       "ImageFormat"
#define PLOT_SETBKWHITE      "SetBackgroundInWhite"
#define PLOT_TITLE           "Title"
#define PLOT_DRVTYPE         "DriverType"
#define PLOT_EXTENSION       "Extension"
#define PLOT_PAPERFORMAT     "PaperFormat"
#define PLOT_ORIENTATION     "PlottingOrientation"
#define PLOT_MIRRORDRIVEN    "MirroringDriven"
#define PLOT_MIRRORING       "PlottingMirroring"
#define PLOT_PAPERWIDTH      "PaperWidth"
#define PLOT_PAPERLENGTH     "PaperLength"
#define PLOT_QUALITYDRIVEN   "QualityDriven"
#define PLOT_QUALITY         "Quality"
#define PLOT_COPYDRIVEN      "CopyDriven"
#define PLOT_FEEDDRIVEN      "PaperFeedDriven"
#define PLOT_PAPERFEED       "PaperFeed"
#define PLOT_CUTDRIVEN       "PaperCutDriven"
#define PLOT_ORIGIN          "Origin"
#define PLOT_MINLEFTMARGIN   "MinLeftMargin"
#define PLOT_MINBOTTOMMARGIN "MinBottomMargin"
#define PLOT_MINTOPMARGIN    "MinTopMargin"
#define PLOT_MINRIGHTMARGIN  "MinRightMargin"
#define PLOT_LEFTMARGIN      "LeftMargin"
#define PLOT_BOTTOMMARGIN    "BottomMargin"
#define PLOT_TOPMARGIN       "TopMargin"
#define PLOT_RIGHTMARGIN     "RightMargin"
#define PLOT_PLOTTINGGAP     "PlottingGap"
#define PLOT_PLOTTERCOLOR    "PlotterColor"
#define PLOT_MAXCOLORS       "MaxColors"
#define PLOT_PLOTTINGCOLOR   "PlottingColor"
#define PLOT_COLORMAPPING    "ColorMapping"
#define PLOT_MAXPATTERMS     "MaxPatterns"
#define PLOT_COLORMAPRGB     "ColorMapRGB"
#define PLOT_COLORMAXGAP     "ColorMaxGap"
#define PLOT_WIDTHMAP        "WidthMap"
#define PLOT_WIDTHMAXGAP     "WidthMaxGap"
#define PLOT_LINETYPEMAP     "LineTypeMap"
#define PLOT_LINETYPEMAXGAP  "LineTypeMaxGap"
#define PLOT_RESOLUTION      "Resolution"
#define PLOT_COLORRESOLUTION "ColorResolution"
#define PLOT_CIRCLEDRIVEN    "CircleDriven"
#define PLOT_ELLIPSEDRIVEN   "EllipseDriven"
#define PLOT_CURVEDRIVEN     "CurveDriven"
#define PLOT_TEXTDRIVEN      "TextDriven"
#define PLOT_MARKERDRIVEN    "MarkerDriven"
#define PLOT_RECTANGLEDRIVEN "RectangleDriven"
#define PLOT_FILLDRIVEN      "FillDriven"
#define PLOT_FONTMAP         "FontMap"
#define PLOT_PLOTTINGTYPE    "PlottingType"
#define PLOT_STYLUSNUMBER    "StylusNumber"
#define PLOT_DENSITY         "LinearDensity"
#define PLOT_MAXPENS         "MaxPens"
#define PLOT_MOUNTEDPENS     "MountedPens"
#define PLOT_PENSINDEXWIDTH  "PensIndexWidth"
#define PLOT_PENSINDEXCOLOR  "PensIndexColor"
#define PLOT_PENSINDEXTYPE   "PensIndexLineType"
#define PLOT_CHECKSUMDRIVEN  "ChecksumDriven"
#define PLOT_BEFORECMD       "CommandBeforeDriver"
#define PLOT_AFTERCMD        "CommandAfterDriver"
#define PLOT_PRINTCMD        "CommandToPrint"
#define PLOT_MEDIASAVDRIVEN  "MediaSaverDriven"
#define PLOT_MEDIASAVTIME    "MediaSaverTimeOut"
#define PLOT_ENDOFPLOTDRIVEN "EndOfPlotTimerDriven"
#define PLOT_ENDOFPLOTTIMER  "EndOfPlotTimer"
#define PLOT_DRYTIMEDRIVEN   "DryTimeDriven"
#define PLOT_DRYTIME         "DryTime"
#define PLOT_PHYSICALSTEP    "PhysicalStep"
#define PLOT_MINWIDTHOFLINE  "MinWidthOfLine"
#define PLOT_COPIES          "PlottingCopie"
#define PLOT_ROTATEDRIVEN    "RotationDriven"
#define PLOT_ROTATION        "PlottingRotation"
#define PLOT_FRAME           "PlottingFrame"
#define PLOT_FRAMEAROUNDIMG  "FrameAroundImage"
#define PLOT_MULTISHEET      "PlottingMultiSheet"
#define PLOT_PLOTCOMMENT     "PlottingComment"
#define PLOT_COMMENTS        "Comment"
#define PLOT_PLOTTITLE       "PlottingTitle"
#define PLOT_PLOTBACKDRAW    "PlottingBackDraw"
#define PLOT_BACKDRAW        "BackgroundDrawing"
#define PLOT_WIDTHOFFSET     "WidthOffset"
#define PLOT_WIDTHSCALE      "WidthScale"
#define PLOT_LENGTHOFFSET    "LengthOffset"
#define PLOT_LENGTHSCALE     "LengthScale"
#define PLOT_FILLWIDTH       "FillWidth"

//==============================================================================
#ifdef WNT
  #define GET_WNT_DEVICE_SIZE()                                        \
    HDC hdc;                                                           \
    if (!lstrcmpi (myName.ToCString(), "DISPLAY"))                     \
      hdc = CreateDC ("DISPLAY", NULL, NULL, NULL);                    \
    else                                                               \
      hdc = CreateDC ("WINSPOOL", myName.ToCString(), NULL, NULL);     \
    if (hdc != NULL) {                                                 \
      myWidth  = ((Standard_Real)GetDeviceCaps (hdc, HORZSIZE) / 10.); \
      myLength = ((Standard_Real)GetDeviceCaps (hdc, VERTSIZE) / 10.); \
      DeleteDC (hdc);                                                  \
    }
#else
  #define GET_WNT_DEVICE_SIZE() { myWidth = 0.0; myLength = 0.0; }
#endif

//==============================================================================
#define TRIM_RIGHT(aBuffer)                \
  {                                        \
    int aPos = strlen (aBuffer);           \
    while ( ((aBuffer[aPos-1] == '\n') ||  \
             (aBuffer[aPos-1] == ' ' )) && \
            aPos )                         \
      aPos--;                              \
    aBuffer [aPos] = '\0';                 \
  }


//==============================================================================
#define BAD_VALUE1(theVal)          (theVal <= 0)

#define BAD_VALUE2(theVal)          ((theVal != 0) && (theVal != -1))

#define BAD_PLOTTER_MSG()                                      \
  cout << "Plotter ignored : BAD CONFIG ('" << aModel << "','" \
       << aOutFormat << "'," << anImgFormat << ")\n" << flush;

//==============================================================================
Standard_CString PARAM_BAD_VALUE1   (const Standard_CString aMethodName);
Standard_CString PARAM_BAD_VALUE2   (const Standard_CString aMethodName);
char*            trim               (char* buffer);
Standard_Integer ItemIndex          (Handle(TColStd_HSequenceOfAsciiString)&,
                                     const TCollection_AsciiString&,
                                     const Standard_Integer nStartIndex = 1);
void             FillListByIterator (Handle(TColStd_HSequenceOfAsciiString)& aList,
                                     OSD_FileIterator& anIterator,
                                     Standard_CString aNeedExtension,
                                     // If plotter already exist in the list do not use it.
                                     const Standard_Boolean aCheckExisted = Standard_False);

#endif // PLOTMGT_PLOTTERTOOLS
