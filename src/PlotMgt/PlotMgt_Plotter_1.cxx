// Created by: DCB
// Copyright (c) 1999 Matra Datavision
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

// Contains all plotter's Inquire methods

#include <PlotMgt.hxx>
#include <PlotMgt_Plotter.hxx>
#include <PlotMgt_HListOfPlotterParameter.hxx>
#include <PlotMgt_PlotterParameter.hxx>
#include <PlotMgt_PlotterTools.hxx>
#include <PlotMgt_PlotterAccessError.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <Quantity_Color.hxx>
#include <Aspect_GenericColorMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>

//==============================================================================
Standard_Integer PlotMgt_Plotter::NumberOfParameters ()
{
  return myParameters -> Length ();
}

//==============================================================================
const Handle(PlotMgt_PlotterParameter)& PlotMgt_Plotter::Parameter (const Standard_Integer anIndex)
{
  PlotMgt_PlotterAccessError_Raise_if ((anIndex < 1) || (anIndex > myParameters -> Length ()),
    "PlotMgt_Plotter::Parameter() : index is out of range.");
  return myParameters -> Value (anIndex);
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::Name ()
{
  return myName;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::Model ()
{
//  if (myModel.IsEmpty()) GetStringValue (PLOT_MODEL, myModel);
  if (myModel.IsEmpty()) GetStringValue (PLOT_DRVTYPE, myModel);
  return myModel;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::Title ()
{
  if (myTitle.IsEmpty()) GetStringValue (PLOT_TITLE, myTitle);
  return myTitle;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::FileExtension ()
{
  if (myExtension.IsEmpty()) GetStringValue (PLOT_EXTENSION, myExtension);
  return myExtension;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::PrintCommand ()
{
  if (myPrintCommand.IsEmpty()) GetStringValue (PLOT_PRINTCMD, myPrintCommand);
  return myPrintCommand;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::BeforePrintCommand ()
{
  if (myBeforeCommand.IsEmpty()) GetStringValue (PLOT_BEFORECMD, myBeforeCommand);
  return myBeforeCommand;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::AfterPrintCommand ()
{
  if (myAfterCommand.IsEmpty()) GetStringValue (PLOT_AFTERCMD, myAfterCommand);
  return myAfterCommand;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::Comments ()
{
  if (myComments.IsEmpty()) GetStringValue (PLOT_COMMENTS, myComments);
  return myComments;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::BackDraw ()
{
  if (myBackDraw.IsEmpty()) GetStringValue (PLOT_BACKDRAW, myBackDraw);
  return myBackDraw;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::ColorMapping ()
{
  if (myColorMapping.IsEmpty()) GetStringValue (PLOT_COLORMAPPING, myColorMapping);
  return myColorMapping;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::OutputFormat ()
{
  if (myOutputFormat.IsEmpty()) GetStringValue (PLOT_OUTFORMAT, myOutputFormat);
  return myOutputFormat;
}

//==============================================================================
TCollection_AsciiString PlotMgt_Plotter::DriverType ()
{
  if (myDriverType.IsEmpty()) GetStringValue (PLOT_DRVTYPE, myDriverType);
  return myDriverType;
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfPaperFormats ()
{
  if (myListOfPaperFormats.IsNull ())
    GetListValue (PLOT_PAPERFORMAT, myListOfPaperFormats);
  return myListOfPaperFormats;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfOrigins ()
{
  if (myListOfOrigins.IsNull ())
    GetListValue (PLOT_ORIGIN, myListOfOrigins);
  return myListOfOrigins;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfQualities ()
{
  if (myListOfQualities.IsNull ())
    GetListValue (PLOT_QUALITY, myListOfQualities);
  return myListOfQualities;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfImageFormats ()
{
  if (myListOfImageFormats.IsNull ())
    GetListValue (PLOT_IMGFORMAT, myListOfImageFormats);
  return myListOfImageFormats;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfColorMappings ()
{
  if (myListOfColorMappings.IsNull ())
    GetListValue (PLOT_COLORMAPPING, myListOfColorMappings);
  return myListOfColorMappings;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfOutputFormats ()
{
  if (myListOfOutputFormats.IsNull ())
    GetListValue (PLOT_OUTFORMAT, myListOfOutputFormats);
  return myListOfOutputFormats;
}

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListOfDriverTypes ()
{
  if (myListOfDriverTypes.IsNull ())
    GetListValue (PLOT_DRVTYPE, myListOfDriverTypes);
  return myListOfDriverTypes;
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
Standard_Boolean PlotMgt_Plotter::MirrorPlot ()
{
  return GetBooleanValue (PLOT_MIRRORING);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::MultiSheet ()
{
  return GetBooleanValue (PLOT_MULTISHEET);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::FramePlot ()
{
  return GetBooleanValue (PLOT_FRAME);
}



//==============================================================================
Standard_Boolean PlotMgt_Plotter::PlotTitle ()
{
  return GetBooleanValue (PLOT_PLOTTITLE);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::PlotComments ()
{
  return GetBooleanValue (PLOT_PLOTCOMMENT);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::PlotBackDraw ()
{
  return GetBooleanValue (PLOT_PLOTBACKDRAW);
}



//==============================================================================
Standard_Boolean PlotMgt_Plotter::PlotterColor ()
{
  return GetBooleanValue (PLOT_PLOTTERCOLOR);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::EndOfPlotDriven ()
{
  return GetBooleanValue (PLOT_ENDOFPLOTDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::DryTimeDriven ()
{
  return GetBooleanValue (PLOT_DRYTIMEDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::PaperFeedDriven ()
{
  return GetBooleanValue (PLOT_FEEDDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::MirrorDriven ()
{
  return GetBooleanValue (PLOT_MIRRORDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::CheckSumDriven ()
{
  return GetBooleanValue (PLOT_CHECKSUMDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::MediaSaverDriven ()
{
  return GetBooleanValue (PLOT_MEDIASAVDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::QualityDriven ()
{
  return GetBooleanValue (PLOT_QUALITYDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::CopyDriven ()
{
  return GetBooleanValue (PLOT_COPYDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::PaperCutDriven ()
{
  return GetBooleanValue (PLOT_CUTDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::RotateDriven ()
{
  return GetBooleanValue (PLOT_ROTATEDRIVEN);
}



//==============================================================================
Standard_Boolean PlotMgt_Plotter::CircleDriven ()
{
  return GetBooleanValue (PLOT_CIRCLEDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::EllipseDriven ()
{
  return GetBooleanValue (PLOT_ELLIPSEDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::CurveDriven ()
{
  return GetBooleanValue (PLOT_CURVEDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::TextDriven ()
{
  return GetBooleanValue (PLOT_TEXTDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::MarkerDriven ()
{
  return GetBooleanValue (PLOT_MARKERDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::RectangleDriven ()
{
  return GetBooleanValue (PLOT_RECTANGLEDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::FillDriven ()
{
  return GetBooleanValue (PLOT_FILLDRIVEN);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::PlottingColor ()
{
  return GetBooleanValue (PLOT_PLOTTINGCOLOR);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
Standard_Real PlotMgt_Plotter::PaperWidth ()
{
  return GetRealValue (PLOT_PAPERWIDTH);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::PaperLength ()
{
  return GetRealValue (PLOT_PAPERLENGTH);
}

//==============================================================================
void PlotMgt_Plotter::PaperSize (Standard_Real& aWidth,
                                 Standard_Real& aLength)
{
  aWidth  = PaperWidth  ();
  aLength = PaperLength ();
}

//==============================================================================
void PlotMgt_Plotter::HardMargins (Standard_Real& aLeft,
                                   Standard_Real& aTop,
                                   Standard_Real& aRight,
                                   Standard_Real& aBottom)
{
  aTop    = GetRealValue (PLOT_MINTOPMARGIN   );
  aLeft   = GetRealValue (PLOT_MINLEFTMARGIN  );
  aRight  = GetRealValue (PLOT_MINRIGHTMARGIN );
  aBottom = GetRealValue (PLOT_MINBOTTOMMARGIN);
}

//==============================================================================
void PlotMgt_Plotter::Margins (Standard_Real& aLeft,
                               Standard_Real& aTop,
                               Standard_Real& aRight,
                               Standard_Real& aBottom)
{
  aTop    = GetRealValue (PLOT_TOPMARGIN   );
  aLeft   = GetRealValue (PLOT_LEFTMARGIN  );
  aRight  = GetRealValue (PLOT_RIGHTMARGIN );
  aBottom = GetRealValue (PLOT_BOTTOMMARGIN);
}

//==============================================================================
void PlotMgt_Plotter::HardMappings (Standard_Real& aWOffset,
                                    Standard_Real& aWScale,
                                    Standard_Real& anLOffset,
                                    Standard_Real& anLScale)
{
  aWOffset  = GetRealValue (PLOT_WIDTHOFFSET );
  aWScale   = GetRealValue (PLOT_WIDTHSCALE  );
  anLOffset = GetRealValue (PLOT_LENGTHOFFSET);
  anLScale  = GetRealValue (PLOT_LENGTHSCALE );
}

//==============================================================================
Standard_Real PlotMgt_Plotter::SpaceDrawing ()
{
  return GetRealValue (PLOT_PLOTTINGGAP);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::PhysicalStep ()
{
  return GetRealValue (PLOT_PHYSICALSTEP);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::MinWidthOfLine ()
{
  return GetRealValue (PLOT_MINWIDTHOFLINE);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::WidthMaxGap ()
{
  return GetRealValue (PLOT_WIDTHMAXGAP);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::LineTypeMaxGap ()
{
  return GetRealValue (PLOT_LINETYPEMAXGAP);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::Resolution ()
{
  return GetRealValue (PLOT_RESOLUTION);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::ColorResolution ()
{
  return GetRealValue (PLOT_COLORRESOLUTION);
}

//==============================================================================
Standard_Real PlotMgt_Plotter::FillWidth ()
{
  return GetRealValue (PLOT_FILLWIDTH);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
Standard_Integer PlotMgt_Plotter::MediaSaverDelay ()
{
  return GetIntegerValue (PLOT_MEDIASAVTIME);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::NumberOfCopies ()
{
  return GetIntegerValue (PLOT_COPIES);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::DryTime ()
{
  return GetIntegerValue (PLOT_DRYTIME);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::EndOfPlotTimer ()
{
  return GetIntegerValue (PLOT_ENDOFPLOTTIMER);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::MaxColors ()
{
  return GetIntegerValue (PLOT_MAXCOLORS);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::MaxPatterns ()
{
  return GetIntegerValue (PLOT_MAXPATTERMS);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::ColorMaxGap ()
{
  return GetIntegerValue (PLOT_COLORMAXGAP);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::LinearDensity ()
{
  return GetIntegerValue (PLOT_DENSITY);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::StylusNumber ()
{
  return GetIntegerValue (PLOT_STYLUSNUMBER);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::PenMaxNumber ()
{
  return GetIntegerValue (PLOT_MAXPENS);
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::NumberInCarrousel ()
{
  return GetIntegerValue (PLOT_MOUNTEDPENS);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
PlotMgt_TypeOfOrigin PlotMgt_Plotter::Origin ()
{
  TCollection_AsciiString anOrigin;
  GetStringValue (PLOT_ORIGIN, anOrigin);
  return PlotMgt::OriginFromString (anOrigin);
}

//==============================================================================
PlotMgt_TypeOfQuality PlotMgt_Plotter::Quality ()
{
  TCollection_AsciiString aQuality;
  GetStringValue (PLOT_QUALITY, aQuality);
  return PlotMgt::QualityFromString (aQuality);
}

//==============================================================================
PlotMgt_ImageFormat PlotMgt_Plotter::ImageFormat ()
{
  TCollection_AsciiString anImageFormat;
  GetStringValue (PLOT_IMGFORMAT, anImageFormat);
  return PlotMgt::ImageFormatFromString (anImageFormat);
}

//==============================================================================
PlotMgt_PlottingType PlotMgt_Plotter::PlottingType ()
{
  TCollection_AsciiString aPlottingType;
  GetStringValue (PLOT_PLOTTINGTYPE, aPlottingType);
  return PlotMgt::PlottingTypeFromString (aPlottingType);
}

//==============================================================================
PlotMgt_PaperFormat PlotMgt_Plotter::PaperFormat ()
{
  TCollection_AsciiString aPaperFormat;
  GetStringValue (PLOT_PAPERFORMAT, aPaperFormat);
  return PlotMgt::PaperFormatFromString (aPaperFormat);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================

//==============================================================================
Handle(TColStd_HSequenceOfInteger) PlotMgt_Plotter::PenColorIndex ()
{
  if (myPenColorIndex.IsNull ())
    GetIntTable (PLOT_PENSINDEXCOLOR, myPenColorIndex);
  return myPenColorIndex;
}

//==============================================================================
Handle(TColStd_HSequenceOfInteger) PlotMgt_Plotter::PenWidthIndex ()
{
  if (myPenWidthIndex.IsNull ())
    GetIntTable (PLOT_PENSINDEXWIDTH, myPenWidthIndex);
  return myPenWidthIndex;
}

//==============================================================================
Handle(TColStd_HSequenceOfInteger) PlotMgt_Plotter::PenTypeIndex ()
{
  if (myPenTypeIndex.IsNull ())
    GetIntTable (PLOT_PENSINDEXTYPE, myPenTypeIndex);
  return myPenTypeIndex;
}

//==============================================================================
Handle(Aspect_ColorMap) PlotMgt_Plotter::ColorMap ()
{
  Handle(TColStd_HSequenceOfAsciiString) aMap;
  aMap.Nullify ();
  if (myColorMap.IsNull ()) {
    TCollection_AsciiString aLine, R, G, B;
    Quantity_Color aColor;
    Aspect_ColorMapEntry anEntry;
    Standard_Integer i = FindParameter (PLOT_COLORMAPRGB), n;
    if (i >= 1 && i <= NumberOfParameters ())
      aMap = myParameters -> Value (i) -> MValue ();
    myColorMap = new Aspect_GenericColorMap ();
    if (!aMap.IsNull()) {
      n = aMap -> Length ();
      for (i = 1; i <= n; i++) {
        aLine = aMap -> Value (i);
        aLine.RemoveAll ('\"');
        R = aLine.Token (" ", 1);
        G = aLine.Token (" ", 2);
        B = aLine.Token (" ", 3);
        if (R.IsEmpty() || !R.IsRealValue()) R = TCollection_AsciiString (0.0);
        if (G.IsEmpty() || !G.IsRealValue()) G = TCollection_AsciiString (0.0);
        if (B.IsEmpty() || !B.IsRealValue()) B = TCollection_AsciiString (0.0);
        aColor = Quantity_Color (R.RealValue(), G.RealValue(), B.RealValue(), Quantity_TOC_RGB);
        anEntry.SetValue (i-1, aColor);
        myColorMap -> AddEntry (anEntry);
      }
    }
  }
  return myColorMap;
}

//==============================================================================
Handle(Aspect_WidthMap) PlotMgt_Plotter::WidthMap ()
{
  Handle(TColStd_HSequenceOfAsciiString) aMap;
  aMap.Nullify ();
  if (myWidthMap.IsNull ()) {
    TCollection_AsciiString aW;
    Aspect_WidthMapEntry anEntry;
    myWidthMap = new Aspect_WidthMap ();
    Standard_Integer i = FindParameter (PLOT_WIDTHMAP), n;
    if (i >= 1 && i <= NumberOfParameters ())
      aMap = myParameters -> Value (i) -> MValue ();
    if (!aMap.IsNull()) {
      n = aMap -> Length ();
      for (i = 1; i <= n; i++) {
        aW = aMap -> Value (i);
        aW.RemoveAll (' ');
        if (!aW.IsEmpty () && aW.IsRealValue()) {
          anEntry.SetValue (i-1, aW.RealValue () * 10.); // To MM
          myWidthMap -> AddEntry (anEntry);
        }
      }
    }
  }
  return myWidthMap;
}

//==============================================================================
Handle(Aspect_TypeMap) PlotMgt_Plotter::TypeMap ()
{
  Handle(TColStd_HSequenceOfAsciiString) aMap;
  aMap.Nullify ();
  if (myTypeMap.IsNull ()) {
    TCollection_AsciiString aLine, aValue;
    Aspect_LineStyle aStyle;
    Aspect_TypeMapEntry anEntry;
    myTypeMap = new Aspect_TypeMap ();
    Standard_Integer i = FindParameter (PLOT_LINETYPEMAP), n, p, j;
    if (i >= 1 && i <= NumberOfParameters ())
      aMap = myParameters -> Value (i) -> MValue ();
    if (!aMap.IsNull()) {
      n = aMap -> Length ();
      for (i = 1; i <= n; i++) {
        aLine = aMap -> Value (i);
        aLine.RemoveAll ('\"');
        // Count of elements in line style
        p = 0;
        do {
          aValue = aLine.Token (" ", p+1);
          if (!aValue.IsEmpty ())
            p++;
        } while (!aValue.IsEmpty ());
        // If there are some elements
        if (p > 1) {
          TColQuantity_Array1OfLength aValues (1, p);
          for (j = 1; j <= p; j++) {
            aValue = aLine.Token (" ", j);
            aValues.SetValue (j, aValue.RealValue ());
          }
          aStyle.SetValues (aValues);
        } else { // else add solid line
          aStyle = Aspect_LineStyle (Aspect_TOL_SOLID);
        }
        anEntry.SetValue (i-1, aStyle);
        myTypeMap -> AddEntry (anEntry);
      }
    }
  }
  return myTypeMap;
}

//==============================================================================
Handle(Aspect_FontMap) PlotMgt_Plotter::FontMap ()
{
  Handle(TColStd_HSequenceOfAsciiString) aMap;
  aMap.Nullify ();
  if (myFontMap.IsNull ()) {
    TCollection_AsciiString aLine;
    Aspect_FontStyle aStyle;
    Aspect_FontMapEntry anEntry;
    myFontMap = new Aspect_FontMap ();
    Standard_Integer i = FindParameter (PLOT_FONTMAP), n;
    if (i >= 1 && i <= NumberOfParameters ())
      aMap = myParameters -> Value (i) -> MValue ();
    if (!aMap.IsNull()) {
      n = aMap -> Length ();
      for (i = 1; i <= n; i++) {
        aLine = aMap -> Value (i);
        aStyle.SetValues (aLine.ToCString ());
        anEntry.SetValue (i-1, aStyle);
        myFontMap -> AddEntry (anEntry);
      }
    }
  }
  return myFontMap;
}

//==============================================================================
Handle(Aspect_FontMap) PlotMgt_Plotter::AllFonts ()
{
  if (myAllFonts.IsNull ())
    myAllFonts = new Aspect_FontMap ();
  return myAllFonts;
}
