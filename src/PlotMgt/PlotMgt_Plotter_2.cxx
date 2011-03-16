// File:        PlotMgt_Plotter_2.cxx
// Created:     Quebex 14-JAN-99
// Author:      DCB
// Copyright:   MatraDatavision 1999
// Contains all plotter's SetXXX methods

#include <PlotMgt.hxx>
#include <PlotMgt_Plotter.hxx>
#include <PlotMgt_PlotterTools.hxx>
#include <PlotMgt_PlotterParameter.hxx>
#include <PlotMgt_HListOfPlotterParameter.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_TypeOfColor.hxx>
#include <Aspect_ColorMap.hxx>
#include <Aspect_WidthMap.hxx>
#include <Aspect_TypeMap.hxx>
#include <Aspect_FontMap.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_FontMapEntry.hxx>

//==============================================================================
void PlotMgt_Plotter::SetModel (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_DRVTYPE, myModel, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetTitle (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_TITLE, myTitle, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetFileExtension (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_EXTENSION, myExtension, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPrintCommand (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_PRINTCMD, myPrintCommand, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetBeforePrintCommand (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_BEFORECMD, myBeforeCommand, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetAfterPrintCommand (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_AFTERCMD, myAfterCommand, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetComments (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_COMMENTS, myComments, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetBackDraw (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_BACKDRAW, myBackDraw, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetColorMapping (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_COLORMAPPING, myColorMapping, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetOutputFormat (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_OUTFORMAT, myOutputFormat, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetDriverType (const TCollection_AsciiString& aValue)
{
  SetStringValue (PLOT_DRVTYPE, myDriverType, aValue);
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
void PlotMgt_Plotter::SetMirrorPlot (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_MIRRORING, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMultiSheet (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_MULTISHEET, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetFramePlot (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_FRAME, aValue);
}



//==============================================================================
void PlotMgt_Plotter::SetPlotTitle (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_PLOTTITLE, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPlotComments (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_PLOTCOMMENT, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPlotBackDraw (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_PLOTBACKDRAW, aValue);
}



//==============================================================================
void PlotMgt_Plotter::SetPlotterColor (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_PLOTTERCOLOR, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetEndOfPlotDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_ENDOFPLOTDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetDryTimeDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_DRYTIMEDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPaperFeedDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_FEEDDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMirrorDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_MIRRORDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetCheckSumDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_CHECKSUMDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMediaSaverDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_MEDIASAVDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetQualityDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_QUALITYDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetCopyDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_COPYDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPaperCutDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_CUTDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetRotateDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_ROTATEDRIVEN, aValue);
}



//==============================================================================
void PlotMgt_Plotter::SetCircleDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_CIRCLEDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetEllipseDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_ELLIPSEDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetCurveDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_CURVEDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetTextDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_TEXTDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMarkerDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_MARKERDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetRectangleDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_RECTANGLEDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetFillDriven (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_FILLDRIVEN, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPlottingColor (const Standard_Boolean aValue)
{
  SetBooleanValue (PLOT_PLOTTINGCOLOR, aValue);
}


//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::SetPaperWidth (const Standard_Real aValue)
{
  SetRealValue (PLOT_PAPERWIDTH, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPaperLength (const Standard_Real aValue)
{
  SetRealValue (PLOT_PAPERLENGTH, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPaperSize (const Standard_Real aWidth,
                                    const Standard_Real aLength)
{
  SetPaperWidth  (aWidth);
  SetPaperLength (aLength);
}

//==============================================================================
void PlotMgt_Plotter::SetHardMargins (const Standard_Real aLeft,
                                      const Standard_Real aTop,
                                      const Standard_Real aRight,
                                      const Standard_Real aBottom)
{
  SetRealValue (PLOT_MINTOPMARGIN,    aTop);
  SetRealValue (PLOT_MINLEFTMARGIN,   aLeft);
  SetRealValue (PLOT_MINRIGHTMARGIN,  aRight);
  SetRealValue (PLOT_MINBOTTOMMARGIN, aBottom);
}

//==============================================================================
void PlotMgt_Plotter::SetMargins (const Standard_Real aLeft,
                                  const Standard_Real aTop,
                                  const Standard_Real aRight,
                                  const Standard_Real aBottom)
{
  SetRealValue (PLOT_TOPMARGIN,    aTop);
  SetRealValue (PLOT_LEFTMARGIN,   aLeft);
  SetRealValue (PLOT_RIGHTMARGIN,  aRight);
  SetRealValue (PLOT_BOTTOMMARGIN, aBottom);
}

//==============================================================================
void PlotMgt_Plotter::SetHardMappings (const Standard_Real aWOffset,
                                       const Standard_Real aWScale,
                                       const Standard_Real anLOffset,
                                       const Standard_Real anLScale)
{
  SetRealValue (PLOT_WIDTHOFFSET,  aWOffset);
  SetRealValue (PLOT_WIDTHSCALE,   aWScale);
  SetRealValue (PLOT_LENGTHOFFSET, anLOffset);
  SetRealValue (PLOT_LENGTHSCALE,  anLScale);
}

//==============================================================================
void PlotMgt_Plotter::SetSpaceDrawing (const Standard_Real aValue)
{
  SetRealValue (PLOT_PLOTTINGGAP, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPhysicalStep (const Standard_Real aValue)
{
  SetRealValue (PLOT_PHYSICALSTEP, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMinWidthOfLine (const Standard_Real aValue)
{
  SetRealValue (PLOT_MINWIDTHOFLINE, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetWidthMaxGap (const Standard_Real aValue)
{
  SetRealValue (PLOT_WIDTHMAXGAP, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetLineTypeMaxGap (const Standard_Real aValue)
{
  SetRealValue (PLOT_LINETYPEMAXGAP, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetResolution (const Standard_Real aValue)
{
  SetRealValue (PLOT_RESOLUTION, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetColorResolution (const Standard_Real aValue)
{
  SetRealValue (PLOT_COLORRESOLUTION, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetFillWidth (const Standard_Real aValue)
{
  SetRealValue (PLOT_FILLWIDTH, aValue);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::SetMediaSaverDelay (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_MEDIASAVTIME, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetNumberOfCopies (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_COPIES, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetDryTime (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_DRYTIME, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetEndOfPlotTimer (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_ENDOFPLOTTIMER, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMaxColors (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_MAXCOLORS, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetMaxPatterns (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_MAXPATTERMS, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetColorMaxGap (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_COLORMAXGAP, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetLinearDensity (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_DENSITY, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetStylusNumber (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_STYLUSNUMBER, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetPenMaxNumber (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_MAXPENS, aValue);
}

//==============================================================================
void PlotMgt_Plotter::SetNumberInCarrousel (const Standard_Integer aValue)
{
  SetIntegerValue (PLOT_MOUNTEDPENS, aValue);
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::SetOrigin (const PlotMgt_TypeOfOrigin anOrigin)
{
  TCollection_AsciiString oldVal;
  GetStringValue (PLOT_ORIGIN, oldVal);
  SetStringValue (PLOT_ORIGIN, oldVal, PlotMgt::StringFromOrigin (anOrigin));
}

//==============================================================================
void PlotMgt_Plotter::SetQuality (const PlotMgt_TypeOfQuality aQuality)
{
  TCollection_AsciiString oldVal;
  GetStringValue (PLOT_QUALITY, oldVal);
  SetStringValue (PLOT_QUALITY, oldVal, PlotMgt::StringFromQuality (aQuality));
}

//==============================================================================
void PlotMgt_Plotter::SetImageFormat (const PlotMgt_ImageFormat anImageFormat)
{
  TCollection_AsciiString oldVal;
  GetStringValue (PLOT_IMGFORMAT, oldVal);
  SetStringValue (PLOT_IMGFORMAT, oldVal, PlotMgt::StringFromImageFormat (anImageFormat));
}

//==============================================================================
void PlotMgt_Plotter::SetPlottingType (const PlotMgt_PlottingType aPlottingType)
{
  TCollection_AsciiString oldVal;
  GetStringValue (PLOT_PLOTTINGTYPE, oldVal);
  SetStringValue (PLOT_PLOTTINGTYPE, oldVal, PlotMgt::StringFromPlottingType (aPlottingType));
}

//==============================================================================
void PlotMgt_Plotter::SetPaperFormat (const PlotMgt_PaperFormat aPaperFormat)
{
  TCollection_AsciiString oldVal;
  GetStringValue (PLOT_PAPERFORMAT, oldVal);
  SetStringValue (PLOT_PAPERFORMAT, oldVal, PlotMgt::StringFromPaperFormat (aPaperFormat));
}

//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::SetPenColorIndex (const Handle(TColStd_HSequenceOfInteger)& aTable)
{
  SetIntTable (PLOT_PENSINDEXCOLOR, aTable);
  myPenColorIndex = aTable -> ShallowCopy ();
}

//==============================================================================
void PlotMgt_Plotter::SetPenWidthIndex (const Handle(TColStd_HSequenceOfInteger)& aTable)
{
  SetIntTable (PLOT_PENSINDEXWIDTH, aTable);
  myPenWidthIndex = aTable -> ShallowCopy ();
}

//==============================================================================
void PlotMgt_Plotter::SetPenTypeIndex (const Handle(TColStd_HSequenceOfInteger)& aTable)
{
  SetIntTable (PLOT_PENSINDEXTYPE, aTable);
  myPenTypeIndex = aTable -> ShallowCopy ();
}

//==============================================================================
void PlotMgt_Plotter::SetColorMap (const Handle(Aspect_ColorMap)& aColorMap)
{
  Handle(TColStd_HSequenceOfAsciiString) aMap = new TColStd_HSequenceOfAsciiString ();
  Standard_Integer j, i = FindParameter (PLOT_COLORMAPRGB), n = aColorMap -> Size ();
  if (i >= 1 && i <= NumberOfParameters ()) {
    TCollection_AsciiString aLine;
    Quantity_Color aColor;
    Standard_Real r, g, b;
    for (j = 1; j <= n; j++) {
      aColor = aColorMap -> Entry (j).Color ();
      aColor.Values (r, g, b, Quantity_TOC_RGB);
      aLine  = "\"";
      aLine += TCollection_AsciiString (r); aLine += " ";
      aLine += TCollection_AsciiString (g); aLine += " ";
      aLine += TCollection_AsciiString (b); aLine += "\"";
      aMap -> Append (aLine);
    }
    myParameters -> Value (i) -> SetMValue (aMap);
    myColorMap = Handle(Aspect_GenericColorMap)::DownCast(aColorMap);
  }
}

//==============================================================================
void PlotMgt_Plotter::SetWidthMap (const Handle(Aspect_WidthMap)& aWidthMap)
{
  Handle(TColStd_HSequenceOfAsciiString) aMap = new TColStd_HSequenceOfAsciiString ();
  Standard_Integer j, i = FindParameter (PLOT_WIDTHMAP), n = aWidthMap -> Size ();
  if (i >= 1 && i <= NumberOfParameters ()) {
    TCollection_AsciiString aLine;
    for (j = 1; j <= n; j++) {
      aLine = TCollection_AsciiString (aWidthMap -> Entry(j).Width());
      aMap -> Append (aLine);
    }
    myParameters -> Value (i) -> SetMValue (aMap);
    myWidthMap = aWidthMap;
  }
}

//==============================================================================
void PlotMgt_Plotter::SetTypeMap (const Handle(Aspect_TypeMap)& aTypeMap)
{
  Handle(TColStd_HSequenceOfAsciiString) aMap = new TColStd_HSequenceOfAsciiString ();
  Standard_Integer j, i = FindParameter (PLOT_LINETYPEMAP), n = aTypeMap -> Size ();
  if (i >= 1 && i <= NumberOfParameters ()) {
    TCollection_AsciiString aLine;
    for (j = 1; j <= n; j++) {
      Aspect_LineStyle aStyle = aTypeMap -> Entry(j).Type();
      Standard_Integer low = aStyle.Values().Lower (), up = aStyle.Values().Upper ();
      aLine  = "\"";
      for (i = low; i <= up; i++) {
        aLine += TCollection_AsciiString (aStyle.Values().Value (i));
        if (i != up) aLine += " ";
        else         aLine += "\"";
      }
      aMap -> Append (aLine);
    }
    myParameters -> Value (i) -> SetMValue (aMap);
    myTypeMap = aTypeMap;
  }
}

//==============================================================================
void PlotMgt_Plotter::SetFontMap (const Handle(Aspect_FontMap)& aFontMap)
{
  Handle(TColStd_HSequenceOfAsciiString) aMap = new TColStd_HSequenceOfAsciiString ();
  Standard_Integer j, i = FindParameter (PLOT_FONTMAP), n = aFontMap -> Size ();
  if (i >= 1 && i <= NumberOfParameters ()) {
    TCollection_AsciiString aLine;
    for (j = 1; j <= n; j++) {
      aLine = aFontMap -> Entry(j).Type().AliasName();
      aMap -> Append (aLine);
    }
    myParameters -> Value (i) -> SetMValue (aMap);
    myFontMap = aFontMap;
  }
}
