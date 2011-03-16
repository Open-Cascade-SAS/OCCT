// File:      PlotMgt_PlotterDriver.cxx
// Created:   DEC-98
// Author:    SYL & DCB
// Copyright: Matra Datavision 1993

#define BUC60766    //GG 041000
//              The plotter filename must be preserved when it contains
//		a directory definition for compatibillity.

#include <PlotMgt_PlotterDriver.ixx>
#include <PlotMgt_Plotter.hxx>
#include <PlotMgt_PlotterParameter.hxx>
#include <PlotMgt_PlotterTools.hxx>
#include <Aspect_Units.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_MarkMapEntry.hxx>
#include <Aspect_DriverError.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <AlienImage.hxx>
#include <OSD_Environment.hxx>

#define xTRACE        1
//#define DEBUG_KEEP 
#define MAXPOINT         1024
#define PPI              72./(0.0254005 METER)
#define ERROR_NO_PLOTTER "PlotMgt_PlotterDriver::Plotter() --> No plotter defined."

#ifdef WNT
# pragma warning (disable : 4244 4305)
# ifdef DrawText
#  undef DrawText
# endif
#endif

//===============================================================================
static OSD_Environment& _DIRPLOTDEF() {
  static OSD_Environment DIRPLOTDEF("DIRPLOTDEF");
  return DIRPLOTDEF;
}
#define DIRPLOTDEF _DIRPLOTDEF()

//===============================================================================
#define SET_SPOOL_NAME(aFileName) {                               \
  TCollection_AsciiString FileName = DIRPLOTDEF.Value();          \
  myFileName  = aFileName;                                        \
  if (!FileName.IsEmpty()) {                                      \
    myDrawingName = aFileName;                                    \
    Standard_Character aChar = FileName.Value(FileName.Length()); \
    if ( (aChar != '\\') && (aChar != '/') ) FileName += "/";     \
    FileName   += myFileName;                                     \
    myFileName  = FileName;                                       \
  } else {                                                        \
    myFileName  = "./";                                           \
    myFileName += aFileName;                                      \
  }                                                               \
								}
//===============================================================================
#define INIT_GRAPHIC_ATTRIB()            \
  myLineColorIndex    = 0;               \
  myLineTypeIndex     = 0;               \
  myLineWidthIndex    = 0;               \
  myPolyColorIndex    = 0;               \
  myPolyTileIndex     = 0;               \
  myPolyEdgeFlag      = Standard_False;  \
  myMarkerColorIndex  = 0;               \
  myMarkerWidthIndex  = 0;               \
  myMarkerFillFlag    = Standard_False;  \
  myTextColorIndex    = 0;               \
  myTextFontIndex     = 0;               \
  myTextSlant         = 0.0F;            \
  myTextHScale        = 1.0;             \
  myTextWScale        = 1.0;             \
  myCapsHeight        = Standard_False;  \
  myTextIsUnderlined  = Standard_False;  \
  myColorIndex        = -1;              \
  myTypeIndex         = -1;              \
  myWidthIndex        = -1;              \
  myFontIndex         = -1;              \
  myPaperMargin       = 8.;

//===============================================================================
PlotMgt_PlotterDriver::PlotMgt_PlotterDriver (const Handle(PlotMgt_Plotter)& aPlotter,
                                              const Standard_CString aFileName,
                                              const Standard_Boolean fCreateFile)
                                            : myPixelSize        (0.325 METER / 1152),
                                              myPrimitiveType    (Aspect_TOP_UNKNOWN),
                                              myPrimitiveLength  (0),
                                              myPrimitiveX       (1, MAXPOINT),
                                              myPrimitiveY       (1, MAXPOINT),
                                              myPlotterIsDefined (Standard_True)
{
#ifdef BUC60766
  myFileName = aFileName;
  myDrawingName = aFileName;
  if( myFileName.Search("/") <= 0 && myFileName.Search("\\") <= 0 ) 
#endif
  	SET_SPOOL_NAME (aFileName);
  INIT_GRAPHIC_ATTRIB ();
  if (fCreateFile) {
    myFStream = new ofstream;
    myFStream->open(myFileName.ToCString(),ios::out);
#ifdef BUC60766
    if( !myFStream->rdbuf()->is_open() ) {
      cout << "*PlotMgt_PlotterDriver Failed on opening the file '" << myFileName << "'" <<endl;
    }
#endif
  } else {
    myFStream = NULL;
  }
#if TRACE > 0
  cout << "PlotMgt_PlotterDriver::PlotMgt_PlotterDriver('"
       << aPlotter->Name() << "','" << aFileName << "') : myFileName '" << myFileName << "'" << endl;     
#endif
  myPlotter = aPlotter;
}

//===============================================================================
PlotMgt_PlotterDriver::PlotMgt_PlotterDriver (const Standard_CString aFileName,
                                              const Standard_Boolean fCreateFile)
                                            : myPixelSize        (0.325 METER / 1152),
                                              myPrimitiveType    (Aspect_TOP_UNKNOWN),
                                              myPrimitiveLength  (0),
                                              myPrimitiveX       (1, MAXPOINT),
                                              myPrimitiveY       (1, MAXPOINT),
                                              myPlotterIsDefined (Standard_False)
{
#ifdef BUC60766
  myFileName = aFileName;
  myDrawingName = aFileName;
  if( myFileName.Search("/") <= 0 && myFileName.Search("\\") <= 0 ) 
#endif
    SET_SPOOL_NAME (aFileName);
  INIT_GRAPHIC_ATTRIB ();
  if (fCreateFile) {
    myFStream = new ofstream;
    myFStream->open(myFileName.ToCString(),ios::out);
#ifdef BUC60766
    if( !myFStream->rdbuf()->is_open() ) {
      cout << "*PlotMgt_PlotterDriver Failed on opening the file '" << myFileName << "'" <<endl;
    }
#endif
  } else {
    myFStream = NULL;
  }
#if TRACE > 0
  cout << "PlotMgt_PlotterDriver::PlotMgt_PlotterDriver('" << aFileName
       << "') : myFileName '" << myFileName << "'" << endl;
#endif
  myPlotter.Nullify ();
}

//===============================================================================
void PlotMgt_PlotterDriver::Close ()
{
  if (myFStream)
    myFStream->close();
#ifdef BUC60766
  myFStream = NULL;
#endif
#if TRACE > 0
  cout << "Aspect_PlotterDriver::Close()" << endl << flush;
#endif
}

//===============================================================================
void PlotMgt_PlotterDriver::SetPixelSize (const Standard_Real aPixelSize)
{
  myPixelSize = aPixelSize;
}

//===============================================================================
void PlotMgt_PlotterDriver::SetPlotter (const Handle(PlotMgt_Plotter)& aPlotter)
{
  if (myPlotterIsDefined) {
    if (myPlotter->Name() != aPlotter->Name()) {
      Aspect_DriverError_Raise_if(Standard_True, "PlotMgt_PlotterDriver::SetPlotter -> Plotter already defined.");
    }
  }
  myPlotterIsDefined = Standard_True;
  myPlotter          = aPlotter;
}

//===============================================================================
Aspect_FStream PlotMgt_PlotterDriver::Cout () const
{
  return myFStream;
}

//===============================================================================
TCollection_AsciiString PlotMgt_PlotterDriver::PlotFileName () const
{
  return myFileName;
}

//===============================================================================
TCollection_AsciiString PlotMgt_PlotterDriver::DrawingName () const
{
  return myDrawingName;
}
//===============================================================================
TCollection_AsciiString PlotMgt_PlotterDriver::SpoolDirectory () const
{
  TCollection_AsciiString aRes = DIRPLOTDEF.Value();
  if (!aRes.IsEmpty()) {
    Standard_Character aChar = aRes.Value(aRes.Length());
    if ( (aChar != '\\') && (aChar != '/') )
      aRes += "/";
  } else {
    aRes = "./";
  }
  return aRes;
}

//===============================================================================
Handle(PlotMgt_Plotter) PlotMgt_PlotterDriver::Plotter () const
{
  Aspect_DriverError_Raise_if(!myPlotterIsDefined, ERROR_NO_PLOTTER);
  return myPlotter;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::Spool (const Aspect_PlotMode aPlotMode,
                                               const Standard_CString /*aReserved1*/,
                                               const Standard_Boolean /*aReserved2*/)
{
  Standard_Boolean Done = Standard_True;
  Close();
  TCollection_AsciiString beforeCommand = Plotter() -> BeforePrintCommand ();
  TCollection_AsciiString afterCommand  = Plotter() -> AfterPrintCommand ();
  TCollection_AsciiString printCommand  = Plotter() -> PrintCommand ();

  if (!afterCommand.IsEmpty() || 
      (aPlotMode != Aspect_PM_FILEONLY && !printCommand.IsEmpty())) {
    TCollection_AsciiString theCmdFileName;
    OSD_Path aCmdPath (myFileName);
#ifdef WNT
    aCmdPath.SetExtension (".cmd");
#else
    aCmdPath.SetExtension (".csh");
#endif // WNT
    aCmdPath.SystemName (theCmdFileName);
    Aspect_FStream cmdFStream = new ofstream;
    cmdFStream -> open (theCmdFileName.ToCString(), ios::out);
#ifdef WNT
    *cmdFStream << "set DESSNOM=" << myFileName << endl;
    *cmdFStream << "set Plot_FileName=" << myFileName << endl;
    *cmdFStream << "set Plot_PlotterName=" << myPlotter->Name() << endl;
#else   
    *cmdFStream << "#!/bin/csh" << endl;
    *cmdFStream << "setenv DESSNOM " << myFileName << endl;
    *cmdFStream << "setenv Plot_FileName " << myFileName << endl;
    *cmdFStream << "setenv Plot_PlotterName " << myPlotter->Name() << endl;
#endif // WNT

    Standard_Integer theNumber = myPlotter->NumberOfParameters();
    Standard_Integer i;
    TCollection_AsciiString theName;
//    Standard_Integer theType;
    TCollection_AsciiString theValue;
//    Standard_Boolean theStatusOfGet;
    *cmdFStream << endl << flush;
    for (i = 1; i <= theNumber; i++) {
      Handle(PlotMgt_PlotterParameter) aParameter = Plotter() -> Parameter (i);
      aParameter -> PutCommandInfo (cmdFStream);
    }   
    *cmdFStream << endl << flush;

    if (!afterCommand.IsEmpty())
      *cmdFStream << endl << afterCommand << endl;
    if (aPlotMode != Aspect_PM_FILEONLY) { 
      if (!printCommand.IsEmpty())
        *cmdFStream << endl << printCommand << endl;
    }
#ifndef WNT    
    *cmdFStream << "rm " << theCmdFileName.ToCString() << endl;
#else
    *cmdFStream << "del " << theCmdFileName.ToCString() << endl;
#endif // WNT
    *cmdFStream << "exit" << endl << flush;
    cmdFStream -> close();

    TCollection_AsciiString theCmd;
#ifdef WNT   
    Standard_Integer theStatus = 0;
#else
    theCmd = "chmod 755 ";
    theCmd += theCmdFileName;
    Standard_Integer theStatus = system (theCmd.ToCString());
#endif // WNT
    if (theStatus == 0) {
      theCmd = theCmdFileName;
//      theStatus = system (theCmd.ToCString());
      if (theStatus != 0)
        Done = Standard_False;
    } else
      Done = Standard_False;
  }
  return Done;
}
        
//===============================================================================
void PlotMgt_PlotterDriver::CreateDriverColorMap ()
{
}

//===============================================================================
void PlotMgt_PlotterDriver::CreateDriverTypeMap ()
{
}

//===============================================================================
void PlotMgt_PlotterDriver::CreateDriverWidthMap ()
{
}

//===============================================================================
void PlotMgt_PlotterDriver::CreateDriverMarkMap ()
{
}

//===============================================================================
void PlotMgt_PlotterDriver::InitializeColorMap (const Handle(Aspect_ColorMap)& /*aColorMap*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::InitializeTypeMap (const Handle(Aspect_TypeMap)& /*aTypeMap*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::InitializeWidthMap (const Handle(Aspect_WidthMap)& /*aWidthMap*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::InitializeFontMap (const Handle(Aspect_FontMap)& aFontMap)
{
  if (!UseMFT())
    return;
  Aspect_FontMapEntry entry;
  Standard_Integer index,iindex,minindex=IntegerLast(),maxindex= -minindex;

  for( index=1; index<=aFontMap->Size(); index++ ) {
    entry = aFontMap->Entry(index);
    maxindex = Max(maxindex, entry.Index());
    minindex = Min(minindex, entry.Index());
  }
  Handle(MFT_FontManager) dummy;
  dummy.Nullify ();
  myMFTFonts = new PlotMgt_HListOfMFTFonts(minindex, maxindex, dummy);
  myMFTSizes = new TShort_HArray1OfShortReal(minindex, maxindex, 0.);

  TCollection_AsciiString aname;
  Aspect_FontStyle style;
  Standard_Boolean theCapsHeight;
  Quantity_PlaneAngle theFontSlant;
  Quantity_Length theFontSize;
  Standard_Real fsize;

  for (index=1; index <= aFontMap->Size(); index++) {
    entry = aFontMap->Entry(index);
    iindex = entry.Index();
    style = entry.Type();
    theFontSize = TOMILLIMETER(style.Size());
    theFontSlant = style.Slant();
    aname = style.AliasName();
    theCapsHeight = style.CapsHeight();
    fsize = theFontSize; 
    if (fsize > 0.0001) fsize = Convert(fsize); 
    else                fsize = 1.; 
    Standard_Boolean found = MFT_FontManager::IsKnown(aname.ToCString());
    if (found) {
      found = Standard_False;
      Handle(MFT_FontManager) theFontManager;
      Aspect_FontStyle fstyle = MFT_FontManager::Font(aname.ToCString());
      for (Standard_Integer i = minindex; i <= maxindex; i++) {
        theFontManager = myMFTFonts->Value(i);
        if (!theFontManager.IsNull()) {
          if (fstyle == theFontManager->Font()) {
            found = Standard_True;
            break;
          }
        }
      }
      if (!found) {
        theFontManager = new MFT_FontManager(aname.ToCString());
      }
      theFontManager->SetFontAttribs (theFontSize,theFontSize,
                                      theFontSlant,0.,theCapsHeight);
      myMFTFonts->SetValue (iindex, theFontManager);
      if (theCapsHeight)
        myMFTSizes->SetValue (iindex, Standard_ShortReal(-theFontSize));
      else
        myMFTSizes->SetValue (iindex, Standard_ShortReal(theFontSize));
    } else {
      Handle(MFT_FontManager) theFontManager;
      if (MFT_FontManager::IsKnown("Defaultfont")) {
        theFontManager = new MFT_FontManager ("Defaultfont");
      } else {
        for (int j=1; j <= aFontMap->Size(); j++) {
          if (MFT_FontManager::IsKnown(aFontMap->Entry(j).Type().AliasName())) {
            cout << "--->>> PlotterDriver: First found font is : '"
                 << aFontMap->Entry(j).Type().AliasName() << "'" << endl << flush;
            theFontManager = new MFT_FontManager (aFontMap->Entry(j).Type().AliasName());
            break;
          }
        }
      }
      if (!theFontManager.IsNull()) {
        myMFTFonts->SetValue (iindex, theFontManager);
        if (theCapsHeight)
          myMFTSizes->SetValue (iindex, Standard_ShortReal(-theFontSize));
        else
          myMFTSizes->SetValue (iindex, Standard_ShortReal(theFontSize));
      }
    }
  }
}

//===============================================================================
void PlotMgt_PlotterDriver::InitializeMarkMap (const Handle(Aspect_MarkMap)& /*aMarkMap*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::SetLineAttrib (const Standard_Integer ColorIndex,
                                           const Standard_Integer TypeIndex,
                                           const Standard_Integer WidthIndex)
{
  myLineColorIndex = ColorIndex;
  myLineTypeIndex  = TypeIndex;
  myLineWidthIndex = WidthIndex;
}

//===============================================================================
void PlotMgt_PlotterDriver::SetTextAttrib (const Standard_Integer ColorIndex,
                                           const Standard_Integer FontIndex)
{
  SetTextAttrib (ColorIndex, FontIndex, 0., 1., 1.);
}

//===============================================================================
void PlotMgt_PlotterDriver::SetTextAttrib (const Standard_Integer ColorIndex,
                                           const Standard_Integer FontIndex,
                                           const Quantity_PlaneAngle aSlant,
                                           const Quantity_Factor aHScale,
                                           const Quantity_Factor aWScale,
                                           const Standard_Boolean isUnderlined)
{
  myTextColorIndex   = ColorIndex;
  myTextFontIndex    = FontIndex;
  myTextIsUnderlined = isUnderlined;
  if (FontIndex != myFontIndex || aSlant != myTextSlant || 
      aHScale != myTextHScale || aWScale != myTextWScale) {
    myFontIndex  = FontIndex;
    myTextSlant  = aSlant;
    myTextHScale = aHScale;
    myTextWScale = (aWScale > 0.) ? aWScale : aHScale;
    if (UseMFT()) {
      Handle(MFT_FontManager) theFontManager
        = myMFTFonts->Value(myFontIndex);
      if (!theFontManager.IsNull()) {
        Quantity_Length theTextSize      = myMFTSizes->Value(myFontIndex);
        Quantity_Length theTextWidth     = myTextWScale * Abs(theTextSize);
        Quantity_Length theTextHeight    = myTextHScale * Abs(theTextSize);
        Quantity_PlaneAngle theTextSlant = myTextSlant;
        Standard_Boolean theCapsHeight   =
          (theTextSize > 0.) ? Standard_False : Standard_True;
        theFontManager->SetFontAttribs (theTextWidth, theTextHeight,
                                        theTextSlant, 0., theCapsHeight);
      }
    }
  }
}

//===============================================================================
void PlotMgt_PlotterDriver::SetPolyAttrib (const Standard_Integer ColorIndex,
                                           const Standard_Integer TileIndex,
                                           const Standard_Boolean DrawEdge)
{
  myPolyColorIndex = ColorIndex;
  myPolyTileIndex  = TileIndex;
  myPolyEdgeFlag   = DrawEdge;
}

//===============================================================================
void PlotMgt_PlotterDriver::SetMarkerAttrib (const Standard_Integer ColorIndex,
                                             const Standard_Integer WidthIndex,
                                             const Standard_Boolean FillMarker)
{
  myMarkerColorIndex = ColorIndex;
  myMarkerWidthIndex = WidthIndex;
  myMarkerFillFlag   = FillMarker;
}

//===============================================================================
void PlotMgt_PlotterDriver::PlotLineAttrib (const Standard_Integer /*ColorIndex*/,
                                            const Standard_Integer /*TypeIndex*/,
                                            const Standard_Integer /*WidthIndex*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::PlotPolyAttrib (const Standard_Integer /*ColorIndex*/,
                                            const Standard_Integer /*TileIndex*/,
                                            const Standard_Boolean /*DrawEdge*/)
{
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================

//===============================================================================
void PlotMgt_PlotterDriver::DrawPoint (const Standard_ShortReal X,
                                       const Standard_ShortReal Y) 
{
  switch (myPrimitiveType) {
    case Aspect_TOP_POLYLINE:
    case Aspect_TOP_POLYGON:
      if (myPrimitiveLength < MAXPOINT) {
        myPrimitiveLength++;
        myPrimitiveX(myPrimitiveLength) = X;
        myPrimitiveY(myPrimitiveLength) = Y;
      }
      break;
    default:
      PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
      PlotPoint      (MapX(X), MapY(Y));
      break;
  }
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawSegment (const Standard_ShortReal X1,
                                         const Standard_ShortReal Y1,
                                         const Standard_ShortReal X2,
                                         const Standard_ShortReal Y2)
{
  PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
  PlotSegment    (MapX(X1), MapY(Y1), MapX(X2), MapY(Y2));
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawRectangle (const Standard_ShortReal aX,
                                           const Standard_ShortReal aY,
                                           const Standard_ShortReal aDX,
                                           const Standard_ShortReal aDY)
{
  Standard_Boolean Done = Standard_False;
  if (myPlotter->RectangleDriven()) {
    PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
    Done = PlotRectangle (MapX(aX), MapY(aY), MapX(aDX), MapY(aDY), 0.0);
  }

  if (!Done) {
    BeginPolyline (5);
    DrawPoint (aX,       aY);
    DrawPoint (aX,       aY + aDY);
    DrawPoint (aX + aDX, aY + aDY);
    DrawPoint (aX + aDX, aY);
    DrawPoint (aX,       aY);
    ClosePrimitive ();
  }
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawPolyline (const TShort_Array1OfShortReal& aListX,
                                          const TShort_Array1OfShortReal& aListY)
{
  static float     xarray[MAXPOINT], yarray[MAXPOINT];
  Standard_Integer u = aListX.Upper(), l = aListX.Lower(), n = aListX.Length();
  if (n != aListY.Length ())
    Aspect_DriverError::Raise ("PlotMgt_PlotterDriver::DrawPolyline() -> 2 different lengths.");
  if (n > MAXPOINT)
    Aspect_DriverError::Raise ("PlotMgt_PlotterDriver::DrawPolyline() -> Array is too long.");
  if (n > 1) {
    for (Standard_Integer i=l; i<=u; i++) {
      xarray[i-l] = MapX(aListX(i));
      yarray[i-l] = MapY(aListY(i));
    }
    PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
    PlotPolyline   (xarray, yarray, &n, 1);
  }
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawPolygon (const TShort_Array1OfShortReal& aListX,
                                         const TShort_Array1OfShortReal& aListY)
{
  static float     xarray[MAXPOINT+1], yarray[MAXPOINT+1];
  Standard_Integer u = aListX.Upper(), l = aListX.Lower(), n = aListX.Length();
  if (n != aListY.Length ())
    Aspect_DriverError::Raise ("PlotMgt_PlotterDriver::DrawPolygon() -> 2 different lengths.");
  if (n > MAXPOINT)
    Aspect_DriverError::Raise ("PlotMgt_PlotterDriver::DrawPolygon() -> Array is too long.");
  if (n > 1) {
    for (Standard_Integer i=l; i<=u; i++) {
      xarray[i-l] = MapX(aListX(i));
      yarray[i-l] = MapY(aListY(i));
    }
    if ((xarray[l] != xarray[u]) || (yarray[l] != yarray[u])) {
      xarray[n] = xarray[0]; yarray[n] = yarray[0]; n++;
    }
    PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
    PlotPolyAttrib (myPolyColorIndex, myPolyTileIndex, myPolyEdgeFlag);
    PlotPolygon    (xarray, yarray, &n, 1);
  }
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::DrawArc (const Standard_ShortReal X,
                                                 const Standard_ShortReal Y,
                                                 const Standard_ShortReal anXradius,
                                                 const Standard_ShortReal anYradius,
                                                 const Standard_ShortReal aStartAngle,
                                                 const Standard_ShortReal anOpenAngle)
{
  Standard_Boolean Done = Standard_False;
  float san, oan;
  if (anOpenAngle == 0.F || anOpenAngle > 2*PI) {
    san = 0.F;  oan = 2.*PI;
  } else if (anOpenAngle < 0.F) {
    san = 2.*PI + aStartAngle + anOpenAngle;
    oan = -anOpenAngle;
  } else {
    san = aStartAngle; oan = anOpenAngle;
  }

  if ( ((anXradius == anYradius) && myPlotter->CircleDriven ()) ||
       ((anXradius != anYradius) && myPlotter->EllipseDriven())   ) {
    PlotLineAttrib (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
    Done = PlotArc (MapX(X), MapX(Y), MapX(anXradius), MapY(anYradius), san, oan);
  }

  if (!Done) {
    Standard_ShortReal precis = FROMCENTIMETER(myPlotter->Resolution());
    Standard_ShortReal value;
    Standard_ShortReal aRadius = Max (anXradius, anYradius);
    if (aRadius > precis)
      value = Max (0.0044, Min (0.7854, 2. * ACos(1.-precis/aRadius)));
    else
      value = PI/4.;
    Standard_Integer nbpoints = Min(MAXPOINT,
                        Standard_Integer(Abs(oan)/value)+2);

    nbpoints = 10;
    float pX, pY, alpha;
    BeginPolyline (nbpoints);
    for (int i=0; i <= nbpoints; i++) {
      alpha = san + (oan*i)/nbpoints;
      pX = float(X + anXradius*Cos(alpha));
      pY = float(Y + anYradius*Sin(alpha));
      DrawPoint (pX, pY);
    }
    ClosePrimitive ();
    Done = Standard_True;
  }    
  return Done;
}                                                       

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::DrawPolyArc (const Standard_ShortReal X,
                                                     const Standard_ShortReal Y,
                                                     const Standard_ShortReal anXradius,
                                                     const Standard_ShortReal anYradius,
                                                     const Standard_ShortReal aStartAngle,
                                                     const Standard_ShortReal anOpenAngle)
{
  Standard_Boolean Done = Standard_False;
  float san, oan;
  if (anOpenAngle == 0.F || anOpenAngle > 2*PI) {
    san = 0.F;  oan = 2.*PI;
  } else if (anOpenAngle < 0.F) {
    san = 2.*PI + aStartAngle + anOpenAngle;
    oan = -anOpenAngle;
  } else {
    san = aStartAngle; oan = anOpenAngle;
  }

  if ( ((anXradius == anYradius) && myPlotter->CircleDriven ()) ||
       ((anXradius != anYradius) && myPlotter->EllipseDriven())   ) {
    PlotLineAttrib     (myLineColorIndex, myLineTypeIndex, myLineWidthIndex);
    PlotPolyAttrib     (myPolyColorIndex, myPolyTileIndex, myPolyEdgeFlag);
    Done = PlotPolyArc (MapX(X), MapY(Y), MapX(anXradius), MapY(anYradius), san, oan);
  }

  if (!Done) {
    Standard_ShortReal precis = FROMCENTIMETER(myPlotter->Resolution());
    Standard_ShortReal value;
    Standard_ShortReal aRadius = Max (anXradius, anYradius);
    if (aRadius > precis)
      value = Max (0.0044, Min (0.7854, 2. * ACos(1.-precis/aRadius)));
    else
      value = PI/4.;
    Standard_Integer nbpoints = Min(MAXPOINT,
                        Standard_Integer(Abs(oan)/value)+2);

    nbpoints = 10;
    float pX, pY, alpha;
    BeginPolygon (nbpoints+1);
    DrawPoint (X, Y);
    for (int i=0; i <= nbpoints; i++) {
      alpha = san + (oan*i)/nbpoints;
      pX = float(X + anXradius*Cos(alpha));
      pY = float(Y + anYradius*Sin(alpha));
      DrawPoint (pX, pY);
    }
    ClosePrimitive ();
    Done = Standard_True;
  }
  return Done;
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawMarker (const Standard_Integer aMarker,
                                        const Standard_ShortReal Xpos,
                                        const Standard_ShortReal Ypos,
                                        const Standard_ShortReal Width,
                                        const Standard_ShortReal Height,
                                        const Standard_ShortReal Angle)
{  
  Standard_Boolean Done = Standard_False;
  if (myPlotter->MarkerDriven())
    Done = PlotMarker (aMarker, MapX(Xpos), MapY(Ypos), Width, Height, Angle);

  if (!Done) {
    if (aMarker >= 0 && Width > 0. && Height > 0.) {
      // Declare the arrays to put in the marker points.

      const TShort_Array1OfShortReal& amx =
        MarkMap ()->Entry(aMarker+1).Style().XValues();
      const TShort_Array1OfShortReal& amy =
        MarkMap ()->Entry(aMarker+1).Style().YValues();
      const TColStd_Array1OfBoolean&  ams =
        MarkMap ()->Entry(aMarker+1).Style().SValues();
      Standard_Integer i, sl, su;
      Standard_ShortReal Ca = (float)Cos((3*Standard_PI)/2+Angle),
                         Sa = (float)Sin((3*Standard_PI)/2+Angle);
      Standard_ShortReal dx, dy, Cx, Cy;

      // Remember the old attributes
      Standard_Integer lci, lti, lwi, pci, pti;
      Standard_Boolean pef;
      LineAttrib (lci, lti, lwi);
      PolyAttrib (pci, pti, pef);
      // Set new line params
      SetLineAttrib (myMarkerColorIndex, 0, myMarkerWidthIndex);
      SetPolyAttrib (myPolyColorIndex, 0, Standard_True);

      // Actual drawing
      sl = amx.Lower();  su = amx.Upper();
      for (i=sl; i <= su; i++) {
        Cx = amx(i)*Width/2.F;
        Cy = amy(i)*Height/2.F;
        if (!ams(i)) {
          dx = Xpos+((Ca*Cx)+(Sa*Cy));
          dy = Ypos+((Sa*Cx)-(Ca*Cy));
          ClosePrimitive ();
          if (myMarkerFillFlag) BeginPolygon  (su);
          else                  BeginPolyline (su);
          DrawPoint (dx, dy);
        } else {
          dx = Xpos+((Ca*Cx)+(Sa*Cy));
          dy = Ypos+((Sa*Cx)-(Ca*Cy));
          DrawPoint (dx, dy);
        }
      }
      ClosePrimitive ();
      // Restore old attributes
      SetLineAttrib (lci, lti, lwi);
      SetPolyAttrib (pci, pti, pef);
    } else {
      DrawPoint (Xpos, Ypos);
    }
  }
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
#define TRANSFORM(X,Y,rX,rY)     \
{                                \
  Standard_Real x = X,y = Y;     \
  X = (float)(rX + x*Ca - y*Sa); \
  Y = (float)(rY + x*Sa + y*Ca); \
}

#define DRAW_TEXT_FRAME(aText)                                       \
  Standard_Real underlinePosition =                                  \
    (myTextIsUnderlined) ? theFontManager->UnderlinePosition() : 0.; \
  Quantity_Length theWidth = 0.,theAscent = 0.,                      \
                  theLbearing = 0.,theDescent = 0.;                  \
  theFontManager->TextSize (aText, theWidth, theAscent,              \
                            theLbearing, theDescent);                \
  theDescent = Max(theDescent, underlinePosition);                   \
  TShort_Array1OfShortReal Xp (1,4), Yp (1,4);                       \
  Standard_ShortReal rwidth  = theWidth;                             \
  Standard_ShortReal rheight = theAscent + theDescent;               \
  Standard_ShortReal rxmin   = theLbearing;                          \
  Standard_ShortReal rymin   = -theDescent;                          \
  Standard_ShortReal marge   = rheight * aMargin;                    \
  rwidth += 2.F*marge; rheight += 2.F*marge;                         \
  rxmin -= marge; rymin -= marge;                                    \
  Standard_ShortReal X1, Y1, X2, Y2, X3, Y3, X4, Y4;                 \
  X1 = rxmin;          Y1 = rymin;                                   \
  X2 = rxmin + rwidth; Y2 = rymin;                                   \
  X3 = rxmin + rwidth; Y3 = rymin + rheight;                         \
  X4 = rxmin;          Y4 = rymin + rheight;                         \
  TRANSFORM (X1, Y1, Xpos, Ypos);                                    \
  TRANSFORM (X2, Y2, Xpos, Ypos);                                    \
  TRANSFORM (X3, Y3, Xpos, Ypos);                                    \
  TRANSFORM (X4, Y4, Xpos, Ypos);                                    \
  Xp.SetValue (1, X1); Yp.SetValue (1, Y1);                          \
  Xp.SetValue (2, X2); Yp.SetValue (2, Y2);                          \
  Xp.SetValue (3, X3); Yp.SetValue (3, Y3);                          \
  Xp.SetValue (4, X4); Yp.SetValue (4, Y4);                          \
  DrawPolygon (Xp, Yp);

//===============================================================================
void PlotMgt_PlotterDriver::DrawText (const TCollection_ExtendedString& aText,
                                      const Standard_ShortReal Xpos,
                                      const Standard_ShortReal Ypos,
                                      const Standard_ShortReal anAngle,
                                      const Aspect_TypeOfText aType)
{
  if (aText.Length() > 0) {
    if (aText.IsAscii()) {
      TCollection_AsciiString atext(aText,'?');
      DrawText (atext.ToCString(), Xpos, Ypos, anAngle, aType);
    } else {
      Standard_Boolean Done = Standard_False;
#ifdef BUC60766
      if (myPlotter->TextDriven())
#else
      if (myPlotter->TextDriven() == (unsigned int ) -1)
#endif
        Done = PlotText (aText, Xpos, Ypos, anAngle, aType);

      if (!Done && UseMFT()) {
        Handle(MFT_FontManager) theFontManager = myMFTFonts->Value(myFontIndex);
        if (!theFontManager.IsNull()) {
          if (!aText.IsAscii() && !theFontManager->IsComposite()) {
            cout << "***ERROR***PlotMgt_PlotterDriver::DrawExtText.UNABLE to draw"
                 << " an extended text with an ANSI font" << endl << flush;
            return;
          }
          Standard_Real underlinePosition =
            (myTextIsUnderlined) ? theFontManager->UnderlinePosition() : 0.;
          myTextManager->SetTextAttribs (myTextColorIndex, aType, underlinePosition);
          theFontManager->DrawText (myTextManager, aText.ToExtString(),
                                    Quantity_Length(Xpos), Quantity_Length(Ypos),
                                    Quantity_PlaneAngle(anAngle));
        }
      } // MFT
    } // Ascii
  } // Length > 0
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawText (const Standard_CString aText,
                                      const Standard_ShortReal Xpos,
                                      const Standard_ShortReal Ypos,
                                      const Standard_ShortReal anAngle,
                                      const Aspect_TypeOfText aType)
{
  if (strlen(aText) > 0) {
    Standard_Boolean Done = Standard_False;
#ifdef BUC60766
    if (myPlotter->TextDriven())
#else
    if (myPlotter->TextDriven() == (unsigned int ) -1)
#endif
      Done = PlotText (aText, Xpos, Ypos, anAngle, aType);

    if (!Done && UseMFT()) {
      Handle(MFT_FontManager) theFontManager = myMFTFonts->Value(myFontIndex);
      if (!theFontManager.IsNull()) {
        Standard_Real underlinePosition =
          (myTextIsUnderlined) ? theFontManager->UnderlinePosition() : 0.;
        myTextManager->SetTextAttribs (myTextColorIndex, aType, underlinePosition);
        theFontManager->DrawText (myTextManager, aText,
                                  Quantity_Length(Xpos), Quantity_Length(Ypos),
                                  Quantity_PlaneAngle(anAngle));
      }
    } // MFT
  } // Length > 0
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawPolyText (const TCollection_ExtendedString& aText,
                                          const Standard_ShortReal Xpos,
                                          const Standard_ShortReal Ypos,
                                          const Quantity_Ratio aMargin,
                                          const Standard_ShortReal anAngle,
                                          const Aspect_TypeOfText aType)
{
  if (aText.Length() > 0) {
    if (aText.IsAscii()) {
      TCollection_AsciiString atext(aText,'?');
      DrawPolyText (atext.ToCString(), Xpos, Ypos, aMargin, anAngle, aType);
    } else {
      Standard_Boolean Done = Standard_False;
#ifdef BUC60766
      if (myPlotter->TextDriven())
#else
      if (myPlotter->TextDriven() == (unsigned int ) -1)
#endif
      {
        PlotPolyAttrib (myPolyColorIndex, myPolyTileIndex, myPolyEdgeFlag);
        Done = PlotPolyText(aText, Xpos, Ypos, aMargin, anAngle, aType);
      }

      if (!Done && UseMFT()) {
        Standard_ShortReal  Ca = Cos(anAngle), Sa = Sin(anAngle);
//        Standard_ShortReal Tsl = Tan(myTextSlant);
        Handle(MFT_FontManager) theFontManager = myMFTFonts->Value(myFontIndex);
        if (!theFontManager.IsNull()) {
          if (!theFontManager->IsComposite()) {
            cout << "***PlotMgt_PlotterDriver::DrawPolyExtText.UNABLE to draw"
                 << " an extended text with an ANSI font" << endl << flush;
            return;
          }
          DRAW_TEXT_FRAME (aText.ToExtString());
          myTextManager->SetTextAttribs (myTextColorIndex, aType, underlinePosition);
          theFontManager->DrawText (myTextManager, aText.ToExtString(),
                                    Quantity_Length(Xpos), Quantity_Length(Ypos),
                                    Quantity_PlaneAngle(anAngle));
        }
      } // MFT
    } // Ascii
  } // aText.Length() > 0
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawPolyText (const Standard_CString aText,
                                          const Standard_ShortReal Xpos,
                                          const Standard_ShortReal Ypos,
                                          const Quantity_Ratio aMargin,
                                          const Standard_ShortReal anAngle,
                                          const Aspect_TypeOfText aType)
{
  if (strlen(aText) > 0) {
    Standard_Boolean Done = Standard_False;
#ifdef BUC60766
    if (myPlotter->TextDriven())
#else
    if (myPlotter->TextDriven() == (unsigned int ) -1)
#endif
    {
      PlotPolyAttrib (myPolyColorIndex, myPolyTileIndex, myPolyEdgeFlag);
      Done = PlotPolyText(aText, Xpos, Ypos, aMargin, anAngle, aType);
    }

    if (!Done && UseMFT()) {
      Standard_ShortReal Ca = Cos(anAngle),
                         Sa = Sin(anAngle);
      Handle(MFT_FontManager) theFontManager = myMFTFonts->Value(myFontIndex);
      if (!theFontManager.IsNull()) {
        DRAW_TEXT_FRAME (aText);
        myTextManager->SetTextAttribs (myTextColorIndex, aType, underlinePosition);
        theFontManager->DrawText (myTextManager, aText,
                                  Quantity_Length(Xpos), Quantity_Length(Ypos),
                                  Quantity_PlaneAngle(anAngle));
      }
    } // MFT
  } // strlen(aText) > 0
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPoint (const Standard_ShortReal /*X*/,
                                                   const Standard_ShortReal /*Y*/) 
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotSegment (const Standard_ShortReal /*X1*/,
                                                     const Standard_ShortReal /*Y1*/,
                                                     const Standard_ShortReal /*X2*/,
                                                     const Standard_ShortReal /*Y2*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotRectangle (const Standard_ShortReal /*aX*/,
                                                       const Standard_ShortReal /*aY*/,
                                                       const Standard_ShortReal /*aDX*/,
                                                       const Standard_ShortReal /*aDY*/,
                                                       const Standard_ShortReal /*aAngle*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPolyline (const Standard_Address /*xArray*/,
                                                      const Standard_Address /*yArray*/,
                                                      const Standard_Address /*nPts*/,
                                                      const Standard_Integer /*nParts*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPolygon (const Standard_Address /*xArray*/,
                                                     const Standard_Address /*yArray*/,
                                                     const Standard_Address /*nPts*/,
                                                     const Standard_Integer /*nParts*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotArc (const Standard_ShortReal /*X*/,
                                                 const Standard_ShortReal /*Y*/,
                                                 const Standard_ShortReal /*anXradius*/,
                                                 const Standard_ShortReal /*anYradius*/,
                                                 const Standard_ShortReal /*sAngle*/,
                                                 const Standard_ShortReal /*oAngle*/)
{
  return Standard_False;
}               

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPolyArc (const Standard_ShortReal /*X*/,
                                                     const Standard_ShortReal /*Y*/,
                                                     const Standard_ShortReal /*anXradius*/,
                                                     const Standard_ShortReal /*anYradius*/,
                                                     const Standard_ShortReal /*sAngle*/,
                                                     const Standard_ShortReal /*oAngle*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotMarker (const Standard_Integer /*aMarker*/,
                                                    const Standard_ShortReal /*Xpos*/,
                                                    const Standard_ShortReal /*Ypos*/,
                                                    const Standard_ShortReal /*Width*/,
                                                    const Standard_ShortReal /*Height*/,
                                                    const Standard_ShortReal /*Angle*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotText (const TCollection_ExtendedString& /*aText*/,
                                                  const Standard_ShortReal /*Xpos*/,
                                                  const Standard_ShortReal /*Ypos*/,
                                                  const Standard_ShortReal /*anAngle*/,
                                                  const Aspect_TypeOfText /*aType*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotText (const Standard_CString /*aText*/,
                                                  const Standard_ShortReal /*Xpos*/,
                                                  const Standard_ShortReal /*Ypos*/,
                                                  const Standard_ShortReal /*anAngle*/,
                                                  const Aspect_TypeOfText /*aType*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPolyText (const TCollection_ExtendedString& /*aText*/,
                                                      const Standard_ShortReal /*Xpos*/,
                                                      const Standard_ShortReal /*Ypos*/,
                                                      const Quantity_Ratio /*aMargin*/,
                                                      const Standard_ShortReal /*anAngle*/,
                                                      const Aspect_TypeOfText /*aType*/)
{
  return Standard_False;
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotPolyText (const Standard_CString /*aText*/,
                                                      const Standard_ShortReal /*Xpos*/,
                                                      const Standard_ShortReal /*Ypos*/,
                                                      const Quantity_Ratio /*aMargin*/,
                                                      const Standard_ShortReal /*anAngle*/,
                                                      const Aspect_TypeOfText /*aType*/)
{
  return Standard_False;
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
void PlotMgt_PlotterDriver::BeginPolyline (const Standard_Integer aNumber)
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive ();
  if (aNumber <= 0) return;
  myPrimitiveType = Aspect_TOP_POLYLINE;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginPolygon (const Standard_Integer aNumber)
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  if (aNumber <= 0) return;
  myPrimitiveType = Aspect_TOP_POLYGON;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginSegments ()
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  myPrimitiveType = Aspect_TOP_SEGMENTS;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginArcs ()
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  myPrimitiveType = Aspect_TOP_ARCS;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginPolyArcs ()
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  myPrimitiveType = Aspect_TOP_POLYARCS;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginMarkers ()
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  myPrimitiveType = Aspect_TOP_MARKERS;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::BeginPoints ()
{
  if (myPrimitiveType != Aspect_TOP_UNKNOWN) ClosePrimitive();
  myPrimitiveType = Aspect_TOP_POINTS;
  myPrimitiveLength = 0;
}

//===============================================================================
void PlotMgt_PlotterDriver::ClosePrimitive ()
{
  switch (myPrimitiveType) {
    case Aspect_TOP_POLYLINE:
      if (myPrimitiveLength > 0) {
        TShort_Array1OfShortReal Xpoint(myPrimitiveX(1),1,myPrimitiveLength);
        TShort_Array1OfShortReal Ypoint(myPrimitiveY(1),1,myPrimitiveLength);
        myPrimitiveLength = 0;
        DrawPolyline (Xpoint,Ypoint);
      }
      break;
    case Aspect_TOP_POLYGON:
      if (myPrimitiveLength > 0) {
        TShort_Array1OfShortReal Xpoint(myPrimitiveX(1),1,myPrimitiveLength);
        TShort_Array1OfShortReal Ypoint(myPrimitiveY(1),1,myPrimitiveLength);
        myPrimitiveLength = 0;
        DrawPolygon (Xpoint,Ypoint);
      }
      break;
    case Aspect_TOP_SEGMENTS:
    case Aspect_TOP_ARCS:
    case Aspect_TOP_POLYARCS:
    case Aspect_TOP_MARKERS:
    case Aspect_TOP_POINTS:
      break;
    default:
      myPrimitiveType = Aspect_TOP_UNKNOWN;
  }
  myPrimitiveType = Aspect_TOP_UNKNOWN;
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::IsKnownImage (const Handle(Standard_Transient)& /*anImage*/)
{
  return Standard_False;
}

//===============================================================================
void PlotMgt_PlotterDriver::ClearImage (const Handle(Standard_Transient)& /*anImage*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::ClearImageFile (const Standard_CString /*anImageFile*/)
{
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawImage (const Handle(Standard_Transient)& /*anImage*/,
                                       const Standard_ShortReal /*aX*/,
                                       const Standard_ShortReal /*aY*/)
{
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::SizeOfImageFile (const Standard_CString /*anImageFile*/,
                                                         Standard_Integer &aWidth,
                                                         Standard_Integer &aHeight) const
{
  aWidth = aHeight = 0;
  return Standard_False;
}

//===============================================================================
void PlotMgt_PlotterDriver::DrawImageFile (const Standard_CString anImageFile,
                                           const Standard_ShortReal aX,
                                           const Standard_ShortReal aY,
                                           const Quantity_Factor aScale)
{
  PlotImage (aX, aY, 0.0F, 0.0F, (float)aScale, anImageFile, NULL);
}

//===============================================================================
void PlotMgt_PlotterDriver::FillAndDrawImage (const Handle(Standard_Transient)& /*anImage*/,
                                              const Standard_ShortReal aX,
                                              const Standard_ShortReal aY,
                                              const Standard_Integer aWidth,
                                              const Standard_Integer aHeight,
                                              const Standard_Address anArrayOfPixels) 
{
  PlotImage (aX, aY, (float)aWidth, (float)aHeight, 1.0F,
             NULL, anArrayOfPixels);
}

//===============================================================================
void PlotMgt_PlotterDriver::FillAndDrawImage (const Handle(Standard_Transient)& /*anImage*/,
                                              const Standard_ShortReal aX,
                                              const Standard_ShortReal aY,
                                              const Standard_Integer anIndexOfLine,
                                              const Standard_Integer aWidth,
                                              const Standard_Integer aHeight,
                                              const Standard_Address anArrayOfPixels)
{
  PlotImage (aX, aY, (float)aWidth, (float)aHeight, 1.0F,
             NULL, anArrayOfPixels, anIndexOfLine);
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::PlotImage (const Standard_ShortReal /*aX*/,
                                                   const Standard_ShortReal /*aY*/,
                                                   const Standard_ShortReal /*aWidth*/,
                                                   const Standard_ShortReal /*aHeight*/,
                                                   const Standard_ShortReal /*aScale*/,
                                                   const Standard_CString /*anImageFile*/,
                                                   const Standard_Address /*anArrayOfPixels*/,
                                                   const Standard_Integer /*aLineIndex*/)
{
  return Standard_False;
}

//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================
//===============================================================================

//===============================================================================
void PlotMgt_PlotterDriver::LineAttrib (Standard_Integer& ColorIndex,
                                        Standard_Integer& TypeIndex,
                                        Standard_Integer& WidthIndex) const
{
  ColorIndex = myLineColorIndex;
  TypeIndex  = myLineTypeIndex;
  WidthIndex = myLineWidthIndex;
}

//===============================================================================
void PlotMgt_PlotterDriver::PolyAttrib (Standard_Integer& ColorIndex,
                                        Standard_Integer& TileIndex,
                                        Standard_Boolean& EdgeFlag) const
{
  ColorIndex = myPolyColorIndex;
  TileIndex  = myPolyTileIndex;
  EdgeFlag   = myPolyEdgeFlag;
}

//===============================================================================
void PlotMgt_PlotterDriver::WorkSpace (Quantity_Length& Width,
                                       Quantity_Length& Height) const
{
  Width  = Quantity_Length (myWidth);
  Height = Quantity_Length (myHeight);
}

//===============================================================================
Standard_Boolean PlotMgt_PlotterDriver::DrawCurveCapable () const
{
  return Standard_False;
}

//===============================================================================
Standard_ShortReal PlotMgt_PlotterDriver::MapX (const Standard_ShortReal aShortreal) const
{
  return aShortreal;
}

//===============================================================================
Standard_ShortReal PlotMgt_PlotterDriver::MapY (const Standard_ShortReal aShortreal) const 
{
  return aShortreal;
}

//===============================================================================
Quantity_Length PlotMgt_PlotterDriver::Convert (const Standard_Integer PV) const
{
  return Quantity_Length (PV * myPixelSize);
}

//===============================================================================
Standard_Integer PlotMgt_PlotterDriver::Convert (const Quantity_Length DV) const
{
  return Standard_Integer (DV/myPixelSize);
}

//===============================================================================
void PlotMgt_PlotterDriver::Convert (const Standard_Integer PX,
                                     const Standard_Integer PY,
                                     Quantity_Length& DX,
                                     Quantity_Length& DY) const
{
  DX = PX * myPixelSize;
  DY = PY * myPixelSize;
}

//===============================================================================
void PlotMgt_PlotterDriver::Convert (const Quantity_Length DX,
                                     const Quantity_Length DY,
                                     Standard_Integer& PX,
                                     Standard_Integer& PY) const
{
  PX = (Standard_Integer )( DX / myPixelSize);
  PY = (Standard_Integer )( DY / myPixelSize);
}
