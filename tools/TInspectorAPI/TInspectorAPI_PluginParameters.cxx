// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/TInspectorAPI_PluginParameters.hxx>

IMPLEMENT_STANDARD_RTTIEXT (TInspectorAPI_PluginParameters, Standard_Transient)

// =======================================================================
// function : SetParameters
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetParameters (const TCollection_AsciiString& thePluginName,
                                                    const NCollection_List<Handle(Standard_Transient)>& theParameters,
                                                    const Standard_Boolean&)
{
  if (theParameters.Size() > 0)
    myParameters.Bind (thePluginName, theParameters);
  else
    myParameters.UnBind (thePluginName);
}

// =======================================================================
// function : AddFileName
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::AddFileName (const TCollection_AsciiString& thePluginName,
                                                  const TCollection_AsciiString& theFileName)
{
  if (myFileNames.IsBound (thePluginName))
    myFileNames.ChangeFind (thePluginName).Append (theFileName);
  else
  {
    NCollection_List<TCollection_AsciiString> aNames;
    aNames.Append (theFileName);
    myFileNames.Bind (thePluginName, aNames);
  }
}

// =======================================================================
// function : SetFileNames
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetFileNames (const TCollection_AsciiString& thePluginName,
                                                   const NCollection_List<TCollection_AsciiString>& theFileNames)
{
  if (theFileNames.Size() > 0)
    myFileNames.Bind (thePluginName, theFileNames);
  else
    myFileNames.UnBind (thePluginName);
}

// =======================================================================
// function : SetSelectedNames
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetSelectedNames (const TCollection_AsciiString& thePluginName,
                                                       const NCollection_List<TCollection_AsciiString>& theItemNames)
{
  mySelectedItemNames.Bind (thePluginName, theItemNames);
}

// =======================================================================
// function : SetSelected
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::SetSelected (const TCollection_AsciiString& thePluginName,
                                                  const NCollection_List<Handle(Standard_Transient)>& theObjects)
{
  if (theObjects.Size() > 0)
    mySelectedObjects.Bind (thePluginName, theObjects);
  else
    mySelectedObjects.UnBind (thePluginName);
}

// =======================================================================
// function : FindParameters
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindParameters (const TCollection_AsciiString& thePluginName)
{
  return myParameters.IsBound (thePluginName);
}

// =======================================================================
// function : Parameters
// purpose :
// =======================================================================
const NCollection_List<Handle(Standard_Transient)>& TInspectorAPI_PluginParameters::Parameters
                                                     (const TCollection_AsciiString& thePluginName)
{
  return myParameters.Find (thePluginName);
}

// =======================================================================
// function : FindFileNames
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindFileNames (const TCollection_AsciiString& thePluginName)
{
  return myFileNames.IsBound (thePluginName);
}

// =======================================================================
// function : FileNames
// purpose :
// =======================================================================
const NCollection_List<TCollection_AsciiString>& TInspectorAPI_PluginParameters::FileNames
                                                       (const TCollection_AsciiString& thePluginName)
{
  return myFileNames.Find (thePluginName);
}

// =======================================================================
// function : FindSelectedNames
// purpose :
// =======================================================================
bool TInspectorAPI_PluginParameters::FindSelectedNames (const TCollection_AsciiString& thePluginName)
{
  return mySelectedItemNames.IsBound (thePluginName);
}

// =======================================================================
// function : GetSelectedNames
// purpose :
// =======================================================================
const NCollection_List<TCollection_AsciiString>& TInspectorAPI_PluginParameters::GetSelectedNames
                                                       (const TCollection_AsciiString& thePluginName)
{
  return mySelectedItemNames.Find (thePluginName);
}

// =======================================================================
// function : GetSelectedObjects
// purpose :
// =======================================================================
Standard_Boolean TInspectorAPI_PluginParameters::GetSelectedObjects (const TCollection_AsciiString& thePluginName,
                                                       NCollection_List<Handle(Standard_Transient)>& theObjects)
{
  return mySelectedObjects.Find (thePluginName, theObjects);
}

// =======================================================================
// function : toString
// purpose :
// =======================================================================
TCollection_AsciiString toString (const TopLoc_Location& theLocation)
{
  TCollection_AsciiString anInfo;
  gp_Trsf aTrsf = theLocation.Transformation();
  for (int aRowId = 1; aRowId <= 3; aRowId++)
  {
    if (!anInfo.IsEmpty())
        anInfo += " ";
    for (int aColumnId = 1; aColumnId <= 4; aColumnId++)
    {
      if (aColumnId > 1)
        anInfo += ",";
      anInfo += TCollection_AsciiString (aTrsf.Value (aRowId, aColumnId));
    }
  }
  return anInfo;
}

// =======================================================================
// function : ParametersToString
// purpose :
// =======================================================================
TCollection_AsciiString TInspectorAPI_PluginParameters::ParametersToString (const TopoDS_Shape& theShape)
{
  const TopLoc_Location& aLocation = theShape.Location();
  TCollection_AsciiString aLocationStr = toString (aLocation);

  TopAbs_Orientation anOrientation = theShape.Orientation();
  Standard_SStream aSStream;
  TopAbs::Print (anOrientation, aSStream);
  return TCollection_AsciiString (aSStream.str().c_str()) + ":" + aLocationStr;
}

// =======================================================================
// function : fromString
// purpose :
// =======================================================================
TopLoc_Location fromString (const TCollection_AsciiString& theValue)
{
  NCollection_Mat4<Standard_Real> aValues;

  TCollection_AsciiString aCurrentString = theValue;
  Standard_Integer aPosition = aCurrentString.Search (" ");
  if (aPosition < 0)
    return TopLoc_Location();
  TCollection_AsciiString aTailString = aCurrentString.Split (aPosition);
  Standard_Integer aRow = 0;
  while (!aCurrentString.IsEmpty())
  {
    TCollection_AsciiString aValueString = aCurrentString;
    aPosition = aValueString.Search (",");
    if (aPosition < 0 )
      break;
    aCurrentString = aValueString.Split (aPosition);
    Standard_Integer aColumn = 0;
    while (!aValueString.IsEmpty())
    {
      aPosition = aCurrentString.Search (" ");
      if (aPosition > 0)
       aValueString.Split (aValueString.Length() - 1);

      aValues.SetValue (aRow, aColumn, aValueString.RealValue());
      aColumn++;
      if (aCurrentString.IsEmpty())
        break;
      aValueString = aCurrentString;
      aPosition = aValueString.Search (",");
      if (aPosition < 0 )
      {
        aValueString = aCurrentString;
        aCurrentString = TCollection_AsciiString();
      }
      else
        aCurrentString = aValueString.Split (aPosition);
    }
    if (aTailString.IsEmpty())
      break;
    aCurrentString = aTailString;
    aPosition = aCurrentString.Search (" ");
    if (aPosition < 0 )
    {
      aCurrentString = aTailString;
      aTailString = TCollection_AsciiString();
    }
    else
      aTailString = aCurrentString.Split (aPosition);
    aRow++;
  }

  //if (aValues.Rows() != 3 || aValues.Cols() != 4)
  //  return TopLoc_Location();

  gp_Trsf aTrsf;
  aTrsf.SetValues (aValues.GetValue (0, 0), aValues.GetValue (0, 1), aValues.GetValue (0, 2), aValues.GetValue (0, 3),
                   aValues.GetValue (1, 0), aValues.GetValue (1, 1), aValues.GetValue (1, 2), aValues.GetValue (1, 3),
                   aValues.GetValue (2, 0), aValues.GetValue (2, 1), aValues.GetValue (2, 2), aValues.GetValue (2, 3));
  return TopLoc_Location (aTrsf);
}

// =======================================================================
// function : ParametersToShape
// purpose :
// =======================================================================
void TInspectorAPI_PluginParameters::ParametersToShape (const TCollection_AsciiString& theValue,
                                                        TopoDS_Shape& theShape)
{
  int aSeparatorPos = theValue.Search (":");
  TCollection_AsciiString anOrientationStr = theValue;
  TCollection_AsciiString aLocationStr = anOrientationStr.Split (aSeparatorPos);
  // orientation
  if (anOrientationStr.Length() < 2)
    return;
  anOrientationStr.Split (anOrientationStr.Length() - 1);

  TopAbs_Orientation anOrientation;
  if (!TopAbs::ShapeOrientationFromString (anOrientationStr.ToCString(), anOrientation))
    return;
  // location
  TopLoc_Location aLocation = fromString (aLocationStr);

  theShape.Location (aLocation);
  theShape.Orientation (anOrientation);
}
