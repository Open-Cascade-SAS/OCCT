// Created by: DCB
// Copyright (c) 1998-1999 Matra Datavision
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

// Contains Load()/Save() and all concerned methods

#define BUC60766	//GG_140699/041000
//		Enable to use Direct drivers when required 
//		using or not the plotter parameters definition
//		sets by DIRPLOT symbol for compatibillty only.

#define TRACE            0
#define MAX_LINE_LEN  1024

#include <PlotMgt.hxx>
#include <PlotMgt_Plotter.ixx>
#include <PlotMgt_PlotterParameter.hxx>
#include <PlotMgt_PlotterDefinitionError.hxx>
#include <PlotMgt_PlotterTools.hxx>
#include <OSD_Environment.hxx>
#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_OpenMode.hxx>
#include <OSD_Protection.hxx>

//=============== Static definitions ====================
static OSD_Environment& _DIRPLOT() {
  static OSD_Environment DIRPLOT("DIRPLOT");
  return DIRPLOT;
}
#define DIRPLOT _DIRPLOT()

static OSD_Environment& _DIRPARPLO() {
  static OSD_Environment DIRPARPLO("DIRPARPLO");
  return DIRPARPLO;
}
#define DIRPARPLO _DIRPARPLO()

#define INIT_VALUES()               \
  myConfigVer          = "";        \
  myModel              = "";        \
  myTitle              = "";        \
  myExtension          = "";        \
  myBeforeCommand      = "";        \
  myAfterCommand       = "";        \
  myPrintCommand       = "";        \
  myComments           = "";        \
  myBackDraw           = "";        \
  myColorMapping       = "";        \
  myOutputFormat       = "";        \
  myDriverType         = "";        \
  myListOfPaperFormats.Nullify ();  \
  myListOfOrigins.Nullify ();       \
  myListOfQualities.Nullify ();     \
  myListOfImageFormats.Nullify ();  \
  myListOfColorMappings.Nullify (); \
  myListOfOutputFormats.Nullify (); \
  myListOfDriverTypes.Nullify ();   \
  myPenColorIndex.Nullify ();       \
  myPenWidthIndex.Nullify ();       \
  myPenTypeIndex.Nullify ();        \
  myColorMap.Nullify();             \
  myWidthMap.Nullify();             \
  myTypeMap.Nullify();              \
  myFontMap.Nullify();              \
  myAllFonts.Nullify();

#define RAISE_FILE_EXIST_ERROR(fExist,aName)                           \
if (!(fExist)) {                                                       \
  anErrorString  = "PlotMgt_Plotter::PlotMgt_Plotter ERROR => File '"; \
  anErrorString += aName;                                              \
  anErrorString += "' not found in DIRPLOT='";                         \
  anErrorString += DIRPLOT.Value ();                                   \
  anErrorString += "' and in DIRPARPLO='";                             \
  anErrorString += DIRPARPLO.Value ();                                 \
  anErrorString += "'.";                                               \
  cout << anErrorString.ToCString() << endl;				\
}
//GG  PlotMgt_PlotterDefinitionError::Raise (anErrorString.ToCString()); 

#define STRING_TRIM(aString) \
  aString.LeftAdjust();      \
  aString.RightAdjust();

static TCollection_AsciiString aReturnValue;
static TCollection_AsciiString anEmptyString;
static TCollection_AsciiString anErrorString;

//==============================================================================
static Standard_Boolean FILE_EXISTS (const TCollection_AsciiString& aName,
                              Standard_CString anExt,
                              TCollection_AsciiString& aFullName)
{
  OSD_Path aFullPath;
  OSD_File aFile1 = OSD_File ( OSD_Path ( DIRPLOT.Value()   + "/" + aName + anExt ) );
  OSD_File aFile2 = OSD_File ( OSD_Path ( DIRPARPLO.Value() + "/" + aName + anExt ) );
  if (aFile1.Exists ()) {
    aFile1.Path (aFullPath);
    aFullPath.SystemName (aFullName);
    return Standard_True;
  }
  if (aFile2.Exists ()) {
    aFile2.Path (aFullPath);
    aFullPath.SystemName (aFullName);
    return Standard_True;
  }
  aFullName = "";
  return Standard_False;
}

//==============================================================================
// PlotMgt_Plotter::PlotMgt_Plotter
//==============================================================================
PlotMgt_Plotter::PlotMgt_Plotter (const TCollection_AsciiString& theName,
                                  const Standard_Boolean fDirectDevice)
{
  TCollection_AsciiString aName = theName, aModel = "NEW_MODEL";
  aName.UpperCase ();
  aModel.UpperCase ();
#ifdef BUC60766
  myDirectDevice = fDirectDevice;
#else
#ifndef WNT
  myDirectDevice = Standard_False;
#else
  myDirectDevice = fDirectDevice;
#endif
#endif //BUC60766
  myName      = aName;
  myModelName = aModel;
#ifndef BUC60766
  if (!myDirectDevice) 
#endif
  {
    Standard_Boolean isModelExists = FILE_EXISTS(myModelName,".pmc",myModelSystemName);
    Standard_Boolean isFileExists  = FILE_EXISTS(myName,".plc",mySystemName) ||
                                     FILE_EXISTS(myName,".plc_off",mySystemName);
    RAISE_FILE_EXIST_ERROR(isModelExists,myModelName);
    RAISE_FILE_EXIST_ERROR(isFileExists,myName);
  }
  INIT_VALUES();
  myParameters = new PlotMgt_HListOfPlotterParameter ();
  myIsLoaded = Load ();
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::Load ()
{
  Standard_Boolean fRetVal1 = Standard_True,
                   fRetVal2 = Standard_True;
#ifndef BUC60766
  if (!myDirectDevice)
#endif
  {
    myParameters -> Clear ();
    // Read parameters from file
    fRetVal1 = ReadParametersFromFile (myModelSystemName, Standard_False);
    if (fRetVal1) {
      // PAR file contains all possible FontMap items
      // We have to remember them in myAllFonts fontmap
//      myAllFonts = this -> FontMap ();
//      myFontMap.Nullify ();
      // Further, we read PLO file
      fRetVal2 = ReadParametersFromFile (mySystemName, Standard_True);
    }
  }
  return (fRetVal1 && fRetVal2);
}

//============================================================================
Standard_Boolean PlotMgt_Plotter::NeedToBeSaved () const
{
#ifndef BUC60766
  if (!myDirectDevice)
#endif
  {
    Standard_Integer i, n  = myParameters -> Length ();
//    Standard_Boolean fSave = Standard_False;;
    // Check if we need to save
    for (i = 1; i <= n; i++) {
      if (myParameters -> Value(i) -> NeedToBeSaved())
        return Standard_True;
    }
  }
  return Standard_False;
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::SavePlotter ()
{
  Standard_Integer i, n  = myParameters -> Length ();
  // Create new file and save plotter to it
  TCollection_AsciiString aLine;
  OSD_File aFile = OSD_File (mySystemName);
  OSD_Protection aProtection;
  Standard_Boolean fSave;
  // Create new file (or empty old one)
  aFile.Build (OSD_WriteOnly, aProtection);
  // Build and put starting comments
  aLine  = "! This is a '"; aLine += myName; aLine += "' plotter file\n";
  aFile.Write (aLine, aLine.Length ());
  fSave = !aFile.Failed ();
  // Write all parameters
  for (i = 1; i <= n && fSave; i++)
    fSave = myParameters -> Value(i) -> Save (aFile);
  // Build and put comments at the end
  if (fSave) {
    aLine  = "! End of '"; aLine += myName; aLine += "' plotter file\n";
    aFile.Write (aLine, aLine.Length ());
  }
  // Close file
  aFile.Close ();
  return (fSave && !aFile.Failed ());
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::Save ()
{
  // Save if necessary
  if (NeedToBeSaved ()) {
    // Rename old file
    OSD_Path anOldPath (mySystemName);
    anOldPath.SetExtension (".plc_old");
    OSD_File anOldFile = OSD_File (mySystemName);
    anOldFile.Move (anOldPath);
    // Save plotter to the <mySystemName> name.
    return SavePlotter ();
  }
  return Standard_False;
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::SaveAs (const TCollection_AsciiString& aNewName)
{
  // Save if necessary
  if (NeedToBeSaved ()) {
    // Set new name (system name) for the plotter
    myName = aNewName;
    myName.UpperCase ();
    mySystemName = DIRPARPLO.Value () + "/" + myName + ".plc";
    // Save plotter to the new <mySystemName> name.
    return SavePlotter ();
  }
  return Standard_False;
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::CheckConfig (const Standard_Boolean /*theViewIs2D*/)
{
/*
  if (!myIsLoaded)
    return Standard_False;

  TCollection_AsciiString aModel      = Model ();
  TCollection_AsciiString aOutFormat  = OutputFormat ();
  PlotMgt_ImageFormat     anImgFormat = ImageFormat ();
  if (aModel.IsEqual("direct_hpgl2")) { // HPGL2_Driver
    // direct_hpgl2, dhpgl2, {ENABLE | DISABLE(2D)}
    return Standard_True;
  } else if (aModel.IsEqual("direct_ps")) { // PS_Driver
    // direct_ps, DPS, {ENABLE | DISABLE(2D)}
    if (!aOutFormat.IsEqual("DPS") ||
        (anImgFormat != PlotMgt_IF_DISABLE && anImgFormat != PlotMgt_IF_ENABLE))
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
    if (anImgFormat == PlotMgt_IF_DISABLE && !theViewIs2D)
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
  } else if (aModel.IsEqual("direct_cgm")) { // CGM_Driver
    // direct_cgm, DCGM, {ENABLE | DISABLE(2D)}
    if (!aOutFormat.IsEqual("DCGM") ||
        (anImgFormat != PlotMgt_IF_DISABLE && anImgFormat != PlotMgt_IF_ENABLE))
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
    if (anImgFormat == PlotMgt_IF_DISABLE && !theViewIs2D)
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
  } else if (aModel.IsEqual("hp")) { // E3Plot_Driver
    // hp, hpgl, DISABLE(2D)
    if (!aOutFormat.IsEqual("hpgl") || anImgFormat != PlotMgt_IF_DISABLE ||
        !theViewIs2D)
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
  } else if (aModel.IsEqual("direct_img")) { // ScreenCopy_Driver
    // direct_img, DISABLE, {XWD | BMP | GIF}
    if (!aOutFormat.IsEqual("DISABLE") ||
        (anImgFormat != PlotMgt_IF_XWD && anImgFormat != PlotMgt_IF_BMP &&
         anImgFormat != PlotMgt_IF_GIF))
    {
      BAD_PLOTTER_MSG();
      return Standard_False;
    }
  }
  // Default
  if (!theViewIs2D && anImgFormat == PlotMgt_IF_DISABLE)
    return Standard_False;
  return Standard_True;
*/
  return Standard_True;
}

//==============================================================================
// List all plotters (.plc and ,plc_off files ) in DIRPARPLO and DIRPLOT
// directories (if specified).
//==============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_Plotter::ListPlotters (
                                 const Standard_Boolean anActiveOnly)
{
  Handle(TColStd_HSequenceOfAsciiString) aListOfPlotters =
    new TColStd_HSequenceOfAsciiString ();
  OSD_Path aDirPlotPath   = DIRPLOT.Value (),
           aDirParPloPath = DIRPARPLO.Value ();
  OSD_FileIterator actDirPlot     (aDirPlotPath,   "*.plc"    ),
                   actDirParPlo   (aDirParPloPath, "*.plc"    ),
                   inactDirPlot   (aDirPlotPath,   "*.plc_off"),
                   inactDirParPlo (aDirParPloPath, "*.plc_off");
  // Firstly list plotters in DIRPARPLO
  FillListByIterator (aListOfPlotters, actDirParPlo, ".plc");
  if (!anActiveOnly)
    FillListByIterator (aListOfPlotters, inactDirParPlo, ".plc_off");
  // Second step : list plotters in DIRPLOT, skip plotters found in DIRPARPLO
  FillListByIterator (aListOfPlotters, actDirPlot, ".plc", Standard_True);
  if (!anActiveOnly)
    FillListByIterator (aListOfPlotters, inactDirPlot, ".plc_off", Standard_True);
  return aListOfPlotters;
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::FindParameter (const TCollection_AsciiString& aName)
{
  Standard_Integer i, n = NumberOfParameters ();
  for (i = 1; i <= n; i++) {
    if (myParameters -> Value(i) -> Name().IsEqual(aName) ||
        myParameters -> Value(i) -> OldName().IsEqual(aName))
      return i;
  }
  return 0;
}

//==============================================================================
void PlotMgt_Plotter::Dump ()
{
  cout << "Plotter '" << Name() << "' dump:\n" << flush;
  cout << "=================================================\n" << flush;
  Standard_Integer n = NumberOfParameters ();
  for (Standard_Integer i = 1; i <= n; i++)
    myParameters -> Value (i) -> Dump ();
  cout << "=================================================\n" << flush;
  cout << "End of dump plotter '" << Name() << "'\n" << flush;
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::ReadParametersFromFile (
                 const TCollection_AsciiString& aFileName,
                 const Standard_Boolean fChangeState)
{
  OSD_File                         aFile = OSD_File (OSD_Path (aFileName));
  OSD_Protection                   aProtection (OSD_R, OSD_R, OSD_R, OSD_R);
  Standard_Integer                 nBytes, idx, n, i;
//  Standard_CString                 aSeparator = ":";
//  Standard_CString                 aDelimiter = ".";
  TCollection_AsciiString          aLine;
  TCollection_AsciiString          aParam;
  TCollection_AsciiString          aParamName;
  TCollection_AsciiString          aParamDef;
  TCollection_AsciiString          aValue;
  TCollection_AsciiString          aDescLine;
  Handle(PlotMgt_PlotterParameter) aParameter;
  aParameter.Nullify ();
  if (!aFile.Exists ()) {
//    cout << "PlotMgt_Plotter::ReadParametersFromFile() -> '" << aFileName
//         << "' could not be found." << endl << flush;
    return Standard_False;
  }
  aFile.Open (OSD_ReadOnly, aProtection);
  while (!aFile.IsAtEnd ()) {
    aFile.ReadLine (aLine, MAX_LINE_LEN, nBytes);
    // Remove all spaces at the begin and end of line
    STRING_TRIM(aLine);
    if (aLine.IsEmpty ())
      continue;
    // If the line is a comment, skip it
    if (aLine.Value(1) == '!')
      continue;
    // Parse the line to <aParam>:<aValue>
    if (aLine.Search (":") == -1) {
      cout << "See line '" << aLine << "'. There is error in it (':' not found)." << endl << flush;
      continue;
    }
    aParam = aLine.Token (":", 1);
    aLine.Remove (1, aParam.Length() + 1); // ':' also
    aValue = aLine;
    STRING_TRIM(aParam);
    STRING_TRIM(aValue);
    idx = aParam.Search (".");
    if (idx == -1) {
      // <aParam> contains only parameter name
      aParamName = aParam;
      aParamDef  = "";
    } else {
      // <aParam> is <aName>.<aQualifier>, where <aQualifier> is one of the:
      // "Type", "Values", "Dialog", "Length".
      aParamName = aParam.Token (".", 1);
      aParamDef  = aParam.Token (".", 2);
      STRING_TRIM(aParamName);
      STRING_TRIM(aParamDef);
      aParamDef.Prepend (TCollection_AsciiString ("."));
    }
    if ((idx = FindParameter(aParamName))) {
      aParameter = myParameters -> Value (idx);
    } else {
      aParameter = new PlotMgt_PlotterParameter (aParamName);
      myParameters -> Append (aParameter);
    }
    // <fChangeState> is false, if we read PAR file and is true if
    // we read PLO file.
    aParameter -> SetState (fChangeState);
    // Check <aValue> if it's a '.Type'
    if (aParamDef.IsEqual (_TYPE_SIGN)) {
      aParameter -> SetType (PlotMgt::TypeFromString(aValue));
    } else {
      aLine = aParamName;
      if (aParamDef != ".")
        aLine += aParamDef;
      aLine += _DELIM_SIGN;
      aLine += aValue;
      aParameter -> Description () -> Append (aLine);
    }
  }
  aFile.Close ();
  // Normalize parameters
  if (fChangeState) {
    n = NumberOfParameters ();
    for (i = 1; i <= n; i++)
      myParameters -> Value (i) -> Normalize ();
  }
  return Standard_True;
}

//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::GetStringValue (const Standard_CString aParam,
                                      TCollection_AsciiString& aValue)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ())
    myParameters -> Value (anIndex) -> SValue (aValue);
}

//==============================================================================
Standard_Boolean PlotMgt_Plotter::GetBooleanValue (const Standard_CString aParam)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ())
    return myParameters -> Value (anIndex) -> BValue ();
  return Standard_False;
}

//==============================================================================
Standard_Integer PlotMgt_Plotter::GetIntegerValue (const Standard_CString aParam)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ())
    return myParameters -> Value (anIndex) -> IValue ();
  return 0;
}

//==============================================================================
Standard_Real PlotMgt_Plotter::GetRealValue (const Standard_CString aParam)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ())
    return myParameters -> Value (anIndex) -> RValue ();
  return 0.0;
}

//==============================================================================
void PlotMgt_Plotter::GetListValue (const Standard_CString aParam,
                                    Handle(TColStd_HSequenceOfAsciiString)& aList)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ())
    myParameters -> Value (anIndex) -> LValues (aList);
}

//==============================================================================
void PlotMgt_Plotter::GetIntTable (const Standard_CString aParam,
                                   Handle(TColStd_HSequenceOfInteger)& aTable)
{
  TCollection_AsciiString aLine;
  Handle(TColStd_HSequenceOfAsciiString) aMap;
  aMap.Nullify ();
  aTable = new TColStd_HSequenceOfInteger ();
  Standard_Integer i = FindParameter (aParam), n;
  if (i >= 1 && i <= NumberOfParameters ())
    aMap = myParameters -> Value (i) -> MValue ();
  if (!aMap.IsNull()) {
    n = aMap -> Length ();
    for (i = 1; i <= n; i++) {
      aLine = aMap -> Value (i);
      aLine.RemoveAll (' ');
      aTable -> Append (
        (!aLine.IsEmpty () && aLine.IsIntegerValue ()) ?
        aLine.IntegerValue () : 1);
    }
  }
}

//==============================================================================
//==============================================================================
//==============================================================================
void PlotMgt_Plotter::SetStringValue (const Standard_CString aParam,
                                      TCollection_AsciiString& anOldValue,
                                      const TCollection_AsciiString& aNewValue)
{
  if (anOldValue != aNewValue) {
    anOldValue = aNewValue;
    Standard_Integer anIndex = FindParameter (aParam);
    if (anIndex >= 1 && anIndex <= NumberOfParameters ())
      myParameters -> Value (anIndex) -> SetSValue (aNewValue);
  }
}

//==============================================================================
void PlotMgt_Plotter::SetBooleanValue (const Standard_CString aParam,
                                       const Standard_Boolean aValue)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ()) {
    if (myParameters -> Value (anIndex) -> BValue () != aValue)
      myParameters -> Value (anIndex) -> SetBValue (aValue);
  }
}

//==============================================================================
void PlotMgt_Plotter::SetIntegerValue (const Standard_CString aParam,
                                       const Standard_Integer aValue)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ()) {
    if (myParameters -> Value (anIndex) -> IValue () != aValue)
      myParameters -> Value (anIndex) -> SetIValue (aValue);
  }
}

//==============================================================================
void PlotMgt_Plotter::SetRealValue (const Standard_CString aParam,
                                    const Standard_Real aValue)
{
  Standard_Integer anIndex = FindParameter (aParam);
  if (anIndex >= 1 && anIndex <= NumberOfParameters ()) {
    if (myParameters -> Value (anIndex) -> RValue () != aValue)
      myParameters -> Value (anIndex) -> SetRValue (aValue);
  }
}

//==============================================================================
void PlotMgt_Plotter::SetIntTable (const Standard_CString aParam,
                                   const Handle(TColStd_HSequenceOfInteger)& aTable)
{
  Standard_Integer anIndex = 0;
  Standard_Integer i, n ;
  anIndex = FindParameter (aParam) ;
  n = aTable -> Length ();
  if (anIndex >= 1 && anIndex <= NumberOfParameters ()) {
    Handle(TColStd_HSequenceOfAsciiString) aMap = new TColStd_HSequenceOfAsciiString ();
    for (i = 1; i <= n; i++)
      aMap -> Append (aTable -> Value (i));
    myParameters -> Value (anIndex) -> SetMValue (aMap);
  }
}
