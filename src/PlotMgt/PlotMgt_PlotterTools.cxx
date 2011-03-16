#include <Standard_Type.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <PlotMgt_PlotterTools.hxx>

//==============================================================================
ParameterDesc __PossibleParameters [] = {
  { PLOT_CONFIGVER,        NULL,      _T_STR,   0 },
  { PLOT_MODEL,            "PLOTPAR", _T_STR,   0 },
  { PLOT_TITLE,            "PLOTTIT", _T_STR,   0 },
  { PLOT_EXTENSION,        "PLOTUNI", _T_STR,   0 },
  { PLOT_BACKDRAW,         NULL,      _T_STR,   0 },
  { PLOT_COMMENTS,         NULL,      _T_STR,   0 },
  { PLOT_COLORMAPRGB,      NULL,      _T_STR,   1 },
  { PLOT_LINETYPEMAP,      NULL,      _T_STR,   1 },
  { PLOT_FONTMAP,          NULL,      _T_STR,   1 },
  { PLOT_BEFORECMD,        NULL,      _T_STR,   0 },
  { PLOT_AFTERCMD,         NULL,      _T_STR,   0 },
  { PLOT_PRINTCMD,         NULL,      _T_STR,   0 },

  { PLOT_DIALOGUNIT,       NULL,      _T_LSTR,  0 },
  { PLOT_OUTFORMAT,        "PLOTFMT", _T_LSTR,  0 },
  { PLOT_IMGFORMAT,        "IMAGFMT", _T_LSTR,  0 },
  { PLOT_DRVTYPE,          "PLOTDRV", _T_LSTR,  0 },
  { PLOT_PAPERFORMAT,      NULL,      _T_LSTR,  0 },
  { PLOT_ORIENTATION,      NULL,      _T_LSTR,  0 },
  { PLOT_QUALITY,          NULL,      _T_LSTR,  0 },
  { PLOT_PAPERFEED,        NULL,      _T_LSTR,  0 },
  { PLOT_ORIGIN,           "PLOTORI", _T_LSTR,  0 },
  { PLOT_COLORMAPPING,     NULL,      _T_LSTR,  0 },
  { PLOT_PLOTTINGTYPE,     "PLOTTYP", _T_LSTR,  0 },
  { PLOT_ROTATION,         NULL,      _T_LSTR,  0 },

  { PLOT_SETBKWHITE,       NULL,      _T_BOOL,  0 },
  { PLOT_MIRRORDRIVEN,     NULL,      _T_BOOL,  0 },
  { PLOT_MIRRORING,        NULL,      _T_BOOL,  0 },
  { PLOT_QUALITYDRIVEN,    NULL,      _T_BOOL,  0 },
  { PLOT_COPYDRIVEN,       NULL,      _T_BOOL,  0 },
  { PLOT_FEEDDRIVEN,       "PLOTAUT", _T_BOOL,  0 },
  { PLOT_CUTDRIVEN,        "PLOTCUT", _T_BOOL,  0 },
  { PLOT_PLOTTINGCOLOR,    NULL,      _T_BOOL,  0 },
  { PLOT_PLOTTERCOLOR,     "PLOTCOL", _T_BOOL,  0 },
  { PLOT_CIRCLEDRIVEN,     "PLOTCIR", _T_BOOL,  0 },
  { PLOT_ELLIPSEDRIVEN,    "PLOTELL", _T_BOOL,  0 },
  { PLOT_CURVEDRIVEN,      "PLOTCUR", _T_BOOL,  0 },
  { PLOT_TEXTDRIVEN,       "PLOTTEX", _T_BOOL,  0 },
  { PLOT_MARKERDRIVEN,     NULL,      _T_BOOL,  0 },
  { PLOT_RECTANGLEDRIVEN,  NULL,      _T_BOOL,  0 },
  { PLOT_FILLDRIVEN,       NULL,      _T_BOOL,  0 },
  { PLOT_CHECKSUMDRIVEN,   "PLOTCHK", _T_BOOL,  0 },
  { PLOT_MEDIASAVDRIVEN,   NULL,      _T_BOOL,  0 },
  { PLOT_ENDOFPLOTDRIVEN,  NULL,      _T_BOOL,  0 },
  { PLOT_DRYTIMEDRIVEN,    NULL,      _T_BOOL,  0 },
  { PLOT_ROTATEDRIVEN,     NULL,      _T_BOOL,  0 },
  { PLOT_FRAME,            NULL,      _T_BOOL,  0 },
  { PLOT_FRAMEAROUNDIMG,   NULL,      _T_BOOL,  0 },
  { PLOT_MULTISHEET,       NULL,      _T_BOOL,  0 },
  { PLOT_PLOTCOMMENT,      NULL,      _T_BOOL,  0 },
  { PLOT_PLOTTITLE,        NULL,      _T_BOOL,  0 },
  { PLOT_PLOTBACKDRAW,     NULL,      _T_BOOL,  0 },

  { PLOT_MINLEFTMARGIN,    NULL,      _T_REAL,  0 },
  { PLOT_MINBOTTOMMARGIN,  NULL,      _T_REAL,  0 },
  { PLOT_MINTOPMARGIN,     NULL,      _T_REAL,  0 },
  { PLOT_MINRIGHTMARGIN,   NULL,      _T_REAL,  0 },
  { PLOT_LEFTMARGIN,       "PLOTDEX", _T_REAL,  0 },
  { PLOT_BOTTOMMARGIN,     "PLOTDEY", _T_REAL,  0 },
  { PLOT_TOPMARGIN,        NULL,      _T_REAL,  0 },
  { PLOT_RIGHTMARGIN,      NULL,      _T_REAL,  0 },
  { PLOT_WIDTHOFFSET,      NULL,      _T_REAL,  0 },
  { PLOT_WIDTHSCALE,       NULL,      _T_REAL,  0 },
  { PLOT_LENGTHOFFSET,     NULL,      _T_REAL,  0 },
  { PLOT_LENGTHSCALE,      NULL,      _T_REAL,  0 },
  { PLOT_PLOTTINGGAP,      "PLOTDIF", _T_REAL,  0 },
  { PLOT_WIDTHMAP,         NULL,      _T_REAL,  1 },
  { PLOT_WIDTHMAXGAP,      NULL,      _T_REAL,  0 },
  { PLOT_LINETYPEMAXGAP,   NULL,      _T_REAL,  0 },
  { PLOT_RESOLUTION,       "PLOTRES", _T_REAL,  0 },
  { PLOT_COLORRESOLUTION,  NULL,      _T_REAL,  0 },
  { PLOT_PHYSICALSTEP,     NULL,      _T_REAL,  0 },
  { PLOT_MINWIDTHOFLINE,   NULL,      _T_REAL,  0 },
  { PLOT_PAPERWIDTH,       "PLOTWID", _T_REAL,  0 },
  { PLOT_PAPERLENGTH,      "PLOTLEN", _T_REAL,  0 },
  { PLOT_FILLWIDTH,        NULL,      _T_REAL,  0 },

  { PLOT_MAXCOLORS,        NULL,      _T_INT,   0 },
  { PLOT_MAXPATTERMS,      NULL,      _T_INT,   0 },
  { PLOT_COLORMAXGAP,      NULL,      _T_INT,   0 },
  { PLOT_STYLUSNUMBER,     "PLOTSTY", _T_INT,   0 },
  { PLOT_DENSITY,          "PLOTNIB", _T_INT,   0 },
  { PLOT_MAXPENS,          "PLOTMXP", _T_INT,   0 },
  { PLOT_MOUNTEDPENS,      "PLOTNBP", _T_INT,   0 },
  { PLOT_PENSINDEXWIDTH,   NULL,      _T_INT,   1 },
  { PLOT_PENSINDEXCOLOR,   NULL,      _T_INT,   1 },
  { PLOT_PENSINDEXTYPE,    NULL,      _T_INT,   1 },
  { PLOT_MEDIASAVTIME,     NULL,      _T_INT,   0 },
  { PLOT_ENDOFPLOTTIMER,   NULL,      _T_INT,   0 },
  { PLOT_DRYTIME,          NULL,      _T_INT,   0 },
  { PLOT_COPIES,           NULL,      _T_INT,   0 },

  { NULL,                  NULL,      _T_UNK,   0 }
};

//==============================================================================
static TCollection_AsciiString anErrorMsg;

//==============================================================================
Standard_CString PARAM_BAD_VALUE1 (const Standard_CString aMethodName) {
  anErrorMsg  = "PlotMgt_Plotter::SetParameter (must be > 0) from ";
  anErrorMsg += aMethodName;
  anErrorMsg += " ()";
  return anErrorMsg.ToCString ();
}

//==============================================================================
Standard_CString PARAM_BAD_VALUE2 (const Standard_CString aMethodName) {
  anErrorMsg  = "PlotMgt_Plotter::SetParameter (must be -1 or 0) from ";
  anErrorMsg += aMethodName;
  anErrorMsg += " ()";
  return anErrorMsg.ToCString ();
}

//==============================================================================
char* trim (char* buffer)
{
  char *first=NULL, *last=NULL;
  first = strchr ( buffer, ':' ) + 1;
  while ( *first == ' ' ) first++;
  last  = first + strlen(first) - 1;
  if ( *last == '\n' ) { last--; *(last+1) = '\0'; }
  while ( *last  == ' ' ) { last--; *(last+1) = '\0'; }
  return first;
}

//==============================================================================
// Finds item <anItem> in the sequence <aList> and returns <index> if item
// exists in the list. Otherwise returns 0.
//==============================================================================
Standard_Integer ItemIndex (Handle(TColStd_HSequenceOfAsciiString)& aList,
                            const TCollection_AsciiString& anItem,
                            const Standard_Integer nStartIndex)
{
  if (aList.IsNull())
    return 0;
  Standard_Integer i, theLength = aList->Length ();
  if ((nStartIndex < 1) || (nStartIndex > theLength))
    return 0;
  for (i = nStartIndex; i <= theLength; i++)
    if ( anItem.IsEqual(aList->Value(i).ToCString()) )
      return i;
  return 0;
}

//==============================================================================
// Fill the list <aList> by using file iterator <anIterator>
//==============================================================================
#define SORT_LIST
void FillListByIterator (Handle(TColStd_HSequenceOfAsciiString)& aList,
                         OSD_FileIterator& anIterator,
                         Standard_CString aNeedExtension,
                         // If plotter already exist in the list do not use it.
                         const Standard_Boolean aCheckExisted)
{
  Standard_Boolean        anAddInList;
  OSD_File                aFile;
  OSD_Path                aPath;
  TCollection_AsciiString aName;
  while (anIterator.More ()) {
    aFile = anIterator.Values ();
    aFile.Path (aPath);
    aName  = aPath.Name ();
    anAddInList = ( !aCheckExisted ||
                    (aCheckExisted && !ItemIndex(aList, aName)) );
    if (aNeedExtension!=NULL)
      anAddInList = (anAddInList && (aPath.Extension() == aNeedExtension));
    anAddInList = (anAddInList && !aName.IsEmpty());
    if (anAddInList)
#ifdef SORT_LIST
    {
      Standard_Integer i = 1, n = aList->Length();
      if (n != 0) {
        while ( i < n && (aList->Value(i) < aName) ) i++;
        if (i == n) aList->Append(aName);
        else        aList->InsertBefore(i, aName);
      } else        aList->Append(aName);
    }
#else
      aList->Append(aName);
#endif
    anIterator.Next();
  }
}
#undef SORT_LIST
