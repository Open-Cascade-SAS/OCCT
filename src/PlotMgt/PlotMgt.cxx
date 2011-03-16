// File:      PlotMgt.cxx
// Created:   22-JAN-99
// Author:    DCB
// Copyright: MatraDatavision 1999

#ifdef WNT
# include <windows.h>
# pragma comment(lib, "winspool.lib")
# pragma comment(lib, "user32.lib"  )
# include <Standard_ProgramError.hxx>
#endif // WNT

#include <stdio.h>

#include <PlotMgt.ixx>

//==============================================================================
static int __NumberOfTypes         = PlotMgt_TOPP_ListString + 1;
static int __NumberOfOrigins       = PlotMgt_TOO_TOPRIGHT + 1;
static int __NumberOfQualities     = PlotMgt_TOQ_BEST + 1;
static int __NumberOfImageFormats  = PlotMgt_IF_GIF + 1;
static int __NumberOfPlottingTypes = PlotMgt_PT_PENEMULATOR + 1;
static int __NumberOfPaperFormats  = PlotMgt_PF_MONARCH + 1;

static TCollection_AsciiString aTypeStr;
static TCollection_AsciiString anOriginStr;
static TCollection_AsciiString aQualityStr;
static TCollection_AsciiString anImageFormatStr;
static TCollection_AsciiString aPlottingTypeStr;
static TCollection_AsciiString aPaperFormatStr;

static const char* __TypeOfParameter[] = {
  "undefined",  "boolean",
  "integer",    "real",
  "string",     "list_string"
};

static const char* __TypeOfOrigin [] = {
  "Center",       "Bottom left",
  "Bottom right", "Top left",
  "Top right"
};

static const char* __TypeOfQuality [] = {
  "Draft", "Normal",
  "High",  "Best"
};

static const char* __ImageFormat [] = {
  "DISABLE", "ENABLE",
  "XWD",     "BMP",
  "GIF"
};

static const char* __PlottingType [] = {
  "Raster", "PenEmulator"
};

static const char* __PaperFormats [] = {
  "UserDefined", "A0",    "A1",        "A2",       "A3",
  "A4",          "A5",    "Letter",    "Legal",    "Statement",
  "Executive",   "Folio", "Quarto",    "Envelope", "Monarch"
};

//==============================================================================
#ifdef WNT
static BOOL IsWindows95 () {
  OSVERSIONINFO os;
  os.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );
  GetVersionEx (&os);
  return (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);
}
#endif

//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt::DeviceList ()
{
  Handle(TColStd_HSequenceOfAsciiString) aList = new TColStd_HSequenceOfAsciiString ();
#ifdef WNT
  char                    buff[ MAX_PATH ];
  DWORD                   dwLen, dwLenRet;
  TCollection_AsciiString dev, defDev;
  buff[ dwLen =
    GetProfileString ( "Windows", "Device", ",,,", buff, MAX_PATH ) ] = '\x00';
  defDev = TCollection_AsciiString ( buff );
  defDev = defDev.Token ( ",", 1 );
  dwLen  = 0;
  if (IsWindows95()) {
    ///////////////////////// W95 ////////////////////////////
    PRINTER_INFO_5* ppi = NULL;
    if (!EnumPrinters (PRINTER_ENUM_LOCAL, NULL, 5, NULL, 0, &dwLen, &dwLenRet)) {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
        ppi = ( PRINTER_INFO_5* )HeapAlloc (GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, dwLen);
        EnumPrinters (PRINTER_ENUM_LOCAL, NULL, 5, ( LPBYTE )ppi, dwLen, &dwLen, &dwLenRet);
      } else {
        wsprintf (buff, "PlotMgt::DeviceList -> Could not obtain device list (%d)", GetLastError ());
        Standard_ProgramError :: Raise ( buff );
        dwLenRet = 0;
      }  // end else
    }
    for (dwLen = 0; dwLen < dwLenRet; ++ dwLen) {
      dev = TCollection_AsciiString ( ppi[ dwLen ].pPrinterName );
      if (dev.IsEqual (defDev)) aList -> Prepend ( dev );
      else                      aList -> Append ( dev );
    }  // end for
    if (ppi != NULL)
      HeapFree (GetProcessHeap (), 0, ( LPVOID )ppi);
  } else {
    ///////////////////////// WNT ////////////////////////////
    PRINTER_INFO_4* ppi = NULL;
    if (!EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
                       NULL, 4, NULL, 0, &dwLen, &dwLenRet)) {
      if (GetLastError () == ERROR_INSUFFICIENT_BUFFER) {
        ppi = ( PRINTER_INFO_4* )HeapAlloc (GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS, dwLen);
        EnumPrinters (PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
                      NULL, 4, ( LPBYTE )ppi, dwLen, &dwLen, &dwLenRet);
      } else {
        wsprintf (buff, "PlotMgt::DeviceList -> Could not obtain device list (%d)", GetLastError () );
        Standard_ProgramError :: Raise ( buff );
        dwLenRet = 0;
      }  // end else
    }
    for (dwLen = 0; dwLen < dwLenRet; ++ dwLen) {
      dev = TCollection_AsciiString ( ppi[ dwLen ].pPrinterName );
      if (dev.IsEqual (defDev)) aList -> Prepend ( dev );
      else                      aList -> Append ( dev );
    }  // end for
    if (ppi != NULL)
      HeapFree (GetProcessHeap (), 0, ( LPVOID )ppi);
  } // WINNT
#else
  const char *cmd = "lpstat -v";
  char buf[BUFSIZ];
  FILE *ptr;
  if ((ptr = popen(cmd, "r")) != NULL) {
    while (fgets(buf, BUFSIZ, ptr) != NULL)
      aList -> Append (buf);
    pclose (ptr);
  }
#endif // WNT
  return aList;
}

//==============================================================================
PlotMgt_TypeOfPlotterParameter PlotMgt::TypeFromString (
                               TCollection_AsciiString& aTypeString)
{
  aTypeString.LowerCase ();
  aTypeString.LeftAdjust ();
  aTypeString.RightAdjust ();
  for (int i = 0; i < __NumberOfTypes; i++)
    if (aTypeString.IsEqual(__TypeOfParameter[i]))
      return PlotMgt_TypeOfPlotterParameter(i);
  return PlotMgt_TOPP_Undefined;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromType (
                        const PlotMgt_TypeOfPlotterParameter aType)
{
  aTypeStr = TCollection_AsciiString (__TypeOfParameter [(int)aType]);
  return aTypeStr;
}

//==============================================================================
PlotMgt_TypeOfOrigin PlotMgt::OriginFromString (
                     TCollection_AsciiString& anOriginString)
{
  anOriginString.LeftAdjust ();
  anOriginString.RightAdjust ();
  for (int i = 0; i < __NumberOfOrigins; i++)
    if (anOriginString.IsEqual(__TypeOfOrigin[i]))
      return PlotMgt_TypeOfOrigin(i);
  return PlotMgt_TOO_CENTER;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromOrigin (
                        const PlotMgt_TypeOfOrigin anOrigin)
{
  anOriginStr = TCollection_AsciiString (__TypeOfOrigin [(int)anOrigin]);
  return anOriginStr;
}

//==============================================================================
PlotMgt_TypeOfQuality PlotMgt::QualityFromString (
                      TCollection_AsciiString& aQualityString)
{
  aQualityString.LeftAdjust ();
  aQualityString.RightAdjust ();
  for (int i = 0; i < __NumberOfQualities; i++)
    if (aQualityString.IsEqual(__TypeOfQuality[i]))
      return PlotMgt_TypeOfQuality(i);
  return PlotMgt_TOQ_DRAFT;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromQuality (
                        const PlotMgt_TypeOfQuality aQuality)
{
  aQualityStr = TCollection_AsciiString (__TypeOfQuality [(int)aQuality]);
  return aQualityStr;
}

//==============================================================================
PlotMgt_ImageFormat PlotMgt::ImageFormatFromString (
                             TCollection_AsciiString& anImageFormatString)
{
  anImageFormatString.LeftAdjust ();
  anImageFormatString.RightAdjust ();
  for (int i = 0; i < __NumberOfImageFormats; i++)
    if (anImageFormatString.IsEqual(__ImageFormat[i]))
      return PlotMgt_ImageFormat(i);
  return PlotMgt_IF_DISABLE;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromImageFormat (
                        const PlotMgt_ImageFormat anImageFormat)
{
  anImageFormatStr = TCollection_AsciiString (__ImageFormat [(int)anImageFormat]);
  return anImageFormatStr;
}

//==============================================================================
PlotMgt_PlottingType PlotMgt::PlottingTypeFromString (
                              TCollection_AsciiString& aPlottingTypeString)
{
  aPlottingTypeString.LeftAdjust ();
  aPlottingTypeString.RightAdjust ();
  for (int i = 0; i < __NumberOfPlottingTypes; i++)
    if (aPlottingTypeString.IsEqual(__PlottingType[i]))
      return PlotMgt_PlottingType(i);
  return PlotMgt_PT_RASTER;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromPlottingType (
                        const PlotMgt_PlottingType aPlottingType)
{
  aPlottingTypeStr = TCollection_AsciiString (__PlottingType [(int)aPlottingType]);
  return aPlottingTypeStr;
}

//==============================================================================
//==============================================================================
//==============================================================================
static Standard_Real aPaperWidths [] = {
  // UserDefined
  00.00,
  // A0, A1, A2, A3, A4, A5
  84.10,  59.40,  42.00,  29.70,  21.00,  14.80,
  // Letter, Legal, Statement, Executive, Folio, Quarto
  21.59,  21.59,  13.97,  18.41,  21.59,  21.50,
  // Envelope, Monarch
  11.00,  09.84
};

static Standard_Real aPaperLengths [] = {
  // UserDefined
  00.00,
  // A0, A1, A2, A3, A4, A5
  118.9,  84.10,  59.40,  42.00,  29.70,  21.00,
  // Letter, Legal, Statement, Executive, Folio, Quarto
  27.94,  35.56,  21.59,  26.67,  33.02,  27.50,
  // Envelope, Monarch
  23.00,  19.04
};

//==============================================================================
PlotMgt_PaperFormat PlotMgt::PaperFormatFromString (
                             TCollection_AsciiString& aPaperFormatString)
{
  aPaperFormatString.LeftAdjust ();
  aPaperFormatString.RightAdjust ();
  for (int i = 0; i < __NumberOfPaperFormats; i++)
    if (aPaperFormatString.IsEqual(__PaperFormats[i]))
      return PlotMgt_PaperFormat(i);
  return PlotMgt_PF_A4;
}

//==============================================================================
TCollection_AsciiString PlotMgt::StringFromPaperFormat (
                        const PlotMgt_PaperFormat aPaperFormat)
{
  aPaperFormatStr = TCollection_AsciiString (__PaperFormats [(int)aPaperFormat]);
  return aPaperFormatStr;
}

//==============================================================================
void PlotMgt::PaperSize (TCollection_AsciiString& aFormat,
                         Standard_Real& aWidth,
                         Standard_Real& aLength)
{
  PlotMgt_PaperFormat aFmt = PlotMgt::PaperFormatFromString (aFormat);
  aWidth  = aPaperWidths  [(int)aFmt];
  aLength = aPaperLengths [(int)aFmt];
}
