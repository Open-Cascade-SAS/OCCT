// File:        PlotMgt_PlotterParameter.cxx
// Created:     Quebex 30 October 1998
// Author:      DCB
// Copyright:   MatraDatavision 1998

#include <PlotMgt.hxx>
#include <PlotMgt_PlotterParameter.ixx>
#include <PlotMgt_PlotterTools.hxx>

// Removes str at index <idx2> and puts it before <idx1>
#define SWAP_STR(idx1,idx2)                  \
{                                            \
  TCollection_AsciiString str;               \
  str = myDescription -> Value(idx2);        \
  myDescription -> Remove (idx2);            \
  myDescription -> InsertBefore (idx1, str); \
}

#define STRING_TRIM(aString) \
  aString.LeftAdjust();      \
  aString.RightAdjust();

#define FLAG_DIALOG   0x00000001
#define FLAG_MINVAL   0x00000002
#define FLAG_MAXVAL   0x00000004
#define FLAG_VALUES   0x00000008
#define FLAG_LENGTH   0x00000010
#define FLAG_DEFVAL   0x00000020
#define FLAG_NTYPE    0x00000040

//============================================================================
PlotMgt_PlotterParameter::PlotMgt_PlotterParameter (const TCollection_AsciiString& aName)
{
  myName        = aName;
  myOldName     = "";
  myType        = PlotMgt_TOPP_Undefined;
  myIndex       = -1;
  myState       = Standard_False;
  myConfigState = Standard_False;
  myIsModified  = Standard_False;
  //--------------------------------------------------------------
  myFlags       = 0;
  myDialog      = "";
  myMinValue    = "";
  myMaxValue    = "";
  myValues      = "";
  myDefValue    = "";
  myMapLength   = 0;
  myMap         = new TColStd_HSequenceOfAsciiString ();
  myDescription = new TColStd_HSequenceOfAsciiString ();
  //---- Find parameter's index in __PossibleParameters array ----
  Standard_Integer i = 0;
  while (__PossibleParameters[i]._new_name != NULL) {
    if ( myName == __PossibleParameters[i]._new_name ||
         (__PossibleParameters[i]._old_name && myName == __PossibleParameters[i]._old_name))
    {
      if (__PossibleParameters[i]._old_name)
        myOldName = __PossibleParameters[i]._old_name;
      myName  = __PossibleParameters[i]._new_name;
      myIndex = i;
      break;
    }
    i++;
  }
  if (myIndex == -1) {
    cout << "PlotMgt_PlotterParameter WARNING: Unknown parameter'"
         << myName << "'\n" << flush;
    return;
  }
}

//============================================================================
#define PUT_INFO(aSign,aValue) {                             \
  sprintf(aBuffer,"%s%s%s%s\n", myName.ToCString(), (aSign), \
          _DELIM_SIGN, aValue.ToCString());                  \
  aLine += aBuffer;            }

//============================================================================
Standard_Boolean PlotMgt_PlotterParameter::Save (OSD_File& aFile)
{
  char aBuffer [1024];
  TCollection_AsciiString aLine = "";
  if (NeedToBeSaved ()) {
    //-----------------------------------------------------
    if (!myOldName.IsEmpty ()) {
      sprintf (aBuffer, "! %s (%s) parameter\n", myName.ToCString(),
        myOldName.ToCString());
      aLine += aBuffer;
    }
    //------------ Write Type info ------------------------
    if (myFlags & FLAG_NTYPE )  PUT_INFO(_TYPE_SIGN, PlotMgt::StringFromType (myType));
    //------------ Write dialog info ----------------------
    if (myFlags & FLAG_DIALOG)  PUT_INFO(_DIALOG_SIGN, myDialog);
    //------------ Write minval info ----------------------
    if (myFlags & FLAG_MINVAL)  PUT_INFO(_MINVAL_SIGN, myMinValue);
    //------------ Write maxval info ----------------------
    if (myFlags & FLAG_MAXVAL)  PUT_INFO(_MAXVAL_SIGN, myMaxValue);
    //------------ Write values info ----------------------
    if (myFlags & FLAG_VALUES)  PUT_INFO(_VALUES_SIGN, myValues);
    //------------ Write defavl info ----------------------
    if (myFlags & FLAG_DEFVAL)  PUT_INFO("", myDefValue);
    //------------ Write map info -------------------------
    if (myFlags & FLAG_LENGTH && myMapLength && !myMap.IsNull()) {
      Standard_Integer i, n = myMap -> Length();
      aLine += myName; aLine += _LENGTH_SIGN;
      aLine += _DELIM_SIGN; aLine += TCollection_AsciiString (myMapLength);
      aLine += "\n";
      for (i = 1; i <= n; i++)
        PUT_INFO("", myMap -> Value (i));
    }
    //------------ Append one new line --------------------
    aLine += "\n";
    aFile.Write (aLine, aLine.Length ());
    return (!aFile.Failed ());
  }
  return Standard_True;
}

//============================================================================
void PlotMgt_PlotterParameter::SetType (const PlotMgt_TypeOfPlotterParameter aType)
{
  myType = aType;
}

//============================================================================
void PlotMgt_PlotterParameter::SetState (const Standard_Boolean aState)
{
  myState = aState;
}

//============================================================================
TCollection_AsciiString PlotMgt_PlotterParameter::Name () const
{
  return myName;
}

//============================================================================
TCollection_AsciiString PlotMgt_PlotterParameter::OldName () const
{
  return myOldName;
}

//============================================================================
Handle(TColStd_HSequenceOfAsciiString)& PlotMgt_PlotterParameter::Description ()
{
  return myDescription;
}

//============================================================================
Standard_Boolean PlotMgt_PlotterParameter::NeedToBeSaved () const
{
  return (myConfigState && (myType != _T_UNK) && (myIsModified || myState));
}

//============================================================================
void PlotMgt_PlotterParameter::Dump () const
{
  if (!myState || (myType == _T_UNK) ||
      !myConfigState || !myIsModified)
    return;

  cout << "!++++++++++++++++++++++++++++++++++++++++++++" << endl << flush;
  cout << "! " << myName << " ";
  if (!myOldName.IsEmpty())
    cout << "(" << myOldName << ") ";
  cout << "parameter" << endl << flush;
  cout << myName << _TYPE_SIGN << _DELIM_SIGN
       << PlotMgt::StringFromType (myType) << endl << flush;
  if (myFlags & FLAG_DIALOG)
    cout << myName << _DIALOG_SIGN << _DELIM_SIGN << myDialog << endl << flush;
  if (myFlags & FLAG_MINVAL)
    cout << myName << _MINVAL_SIGN << _DELIM_SIGN << myMinValue << endl << flush;
  if (myFlags & FLAG_MAXVAL)
    cout << myName << _MAXVAL_SIGN << _DELIM_SIGN << myMaxValue << endl << flush;
  if (myFlags & FLAG_VALUES && (myType == _T_LSTR))
    cout << myName << _VALUES_SIGN << _DELIM_SIGN << myValues << endl << flush;
  if (myFlags & FLAG_DEFVAL)
    cout << myName << _DELIM_SIGN << myDefValue << endl << flush;
  if (myFlags & FLAG_LENGTH && myMapLength) {
    cout << myName << _LENGTH_SIGN << _DELIM_SIGN << myMapLength << endl << flush;
    Standard_Integer i, n = myMap -> Length();
    for (i = 1; i <= n; i++)
      cout << myName << _DELIM_SIGN << myMap -> Value (i) << endl << flush;
  }
  cout << "!++++++++++++++++++++++++++++++++++++++++++++" << endl << flush;
  cout << endl << flush;
}

//============================================================================
void PlotMgt_PlotterParameter::PutCommandInfo (const Aspect_FStream& outStream) const
{
#ifdef WNT
  TCollection_AsciiString _setenv_  ("set Plot_");
  TCollection_AsciiString _comment_ ("REM ######### ");
  TCollection_AsciiString _delim_   (" = ");
#else
  TCollection_AsciiString _setenv_  ("setenv Plot_");
  TCollection_AsciiString _comment_ ("############# ");
  TCollection_AsciiString _delim_   (" ");
#endif // WNT
  *outStream << _comment_ << "Parameter '" << myName << "' of type '"
             << PlotMgt::StringFromType (myType) << "'" << endl;

  if (myFlags & FLAG_DEFVAL)
    *outStream << _setenv_ << myName << _delim_ << "'" << myDefValue << "'" << endl;

  if (myFlags & FLAG_LENGTH && myMapLength) {
    *outStream << _setenv_ << myName << "_Length" << _delim_ << myMapLength << endl;
    Standard_Integer i, n = myMap -> Length();
    for (i = 1; i <= n; i++)
      *outStream << _setenv_ << myName << "_" << i << _delim_ << myMap -> Value (i) << endl;
  }
}

//============================================================================
Standard_Boolean PlotMgt_PlotterParameter::CheckListValue ()
{
  if (!(myFlags & FLAG_VALUES)) {
    cout << "PlotMgt_PlotterParameter WARNING: Parameter '" << myName
         << "' has type 'list_string', but does not have [.Values] "
         << "descriptor" << endl << flush;
    return Standard_False;
  }
  TCollection_AsciiString aValues = myValues, aToken, newValues;
  Standard_Boolean commaFound, defValFound = Standard_False;
  Standard_Integer idx, res;
  STRING_TRIM(myDefValue);
  STRING_TRIM(myValues);
  if (myValues.IsEmpty()) {
    if (!myDefValue.IsEmpty()) {
      cout << "PlotMgt_PlotterParameter WARNING: Parameter '" << myName
           << "' has type 'list_string', but [.Values] descriptor "
           << "is empty. Defaulting to '" << myDefValue << "'" << endl << flush;
      myValues = myDefValue;
      return Standard_True;
    } else {
      cout << "PlotMgt_PlotterParameter WARNING: Parameter '" << myName
           << "' has type 'list_string', but [.Values] descriptor "
           << "and default values are empty." << endl << flush;
      return Standard_False;
    }
  }
  do {
    idx = aValues.Search (",");
    commaFound = (idx != -1);
    if (commaFound) {
      aToken = aValues.Token (",", 1);
      res = aValues.Search (aToken);
      if (res != 1)
        aValues.Remove (1, res - 1);
      aValues.Remove (1, aToken.Length() + 1);
    } else {
      aToken = aValues;
    }
    STRING_TRIM(aToken);
    // Still trying to find default value
    if (!defValFound)
      defValFound = (aToken == myDefValue);
    // Append new values with a token
    if (!aToken.IsEmpty())
      newValues += aToken;
    if (commaFound && !aToken.IsEmpty())
      newValues += ",";
  } while (commaFound);
  // Check the default value
  if (!defValFound) {
    cout << "PlotMgt_PlotterParameter WARNING: Parameter '" << myName
         << "' has type 'list_string'. ";
    if (myFlags & FLAG_DEFVAL)
      cout << "But the default value '" << myDefValue << "' is not found. ";
    else
      cout << "But does not have default value. ";
    cout << "Defaulting to the first from the list." << endl << flush;
    idx = newValues.Search (",");
    if (idx != -1) myDefValue = newValues.Token (",", 1);
    else           myDefValue = newValues;
    myFlags |= FLAG_DEFVAL;
  }
  // Use new values as default
  myValues = newValues;
  return Standard_True;
}

//============================================================================
void PlotMgt_PlotterParameter::ProcessParamVal (
                               const Standard_CString   aParamSign,
                               const Standard_Integer   aFlag,
                               TCollection_AsciiString& aValue)
{
  Standard_Integer i, n = myDescription -> Length ();
  Standard_Boolean fFound = Standard_False;
  TCollection_AsciiString searchStr = myName, aLine;
//JR/Hp
  searchStr += (Standard_CString ) (aParamSign ? aParamSign : "");
//  searchStr += (aParamSign ? aParamSign : "");
  searchStr += _DELIM_SIGN;

  aValue = "";
  for (i = n; i >= 1; i--) {
    aLine = myDescription -> Value(i);
    Standard_Integer res = aLine.Search(searchStr);
    if (res != -1) {
      aLine.Remove (1, searchStr.Length());
      STRING_TRIM(aLine);
      myFlags |= aFlag;
      aValue   = aLine;
      fFound   = Standard_True;
      // Remove all other <aParamSign> strings
      Standard_Integer j = 1;
      while (j <= n) {
        if (myDescription -> Value(j).Search(searchStr) != -1) {
          myDescription -> Remove (j);
          n--;
        } else j++;
      }
      break;
    }
  }
}

//============================================================================
void PlotMgt_PlotterParameter::Normalize ()
{
  Standard_Integer n, i;
  TCollection_AsciiString aMapLen;
  // Quit if the parameter is not found in __PossibleParameters
  if (myIndex == -1)
    goto _CLEAR_AND_EXIT;
  // Check parameter's type in __PossibleParameters array
  if (myType == _T_UNK || myType != __PossibleParameters[myIndex]._type) {
    cout << "PlotMgt_PlotterParameter WARNING: Incorrect type of '"
         << myName << "' parameter : '"
         << PlotMgt::StringFromType (myType) << "' instead of '"
         << PlotMgt::StringFromType (__PossibleParameters[myIndex]._type)
         << "'. Using right TYPE.\n" << flush;
    myType = __PossibleParameters[myIndex]._type;
    myFlags |= FLAG_NTYPE;
  }
  // Change old names to new ones (if any)
  if (!myOldName.IsEmpty ()) {
    n = myDescription -> Length ();
    TCollection_AsciiString aLine;
    for (i = 1; i <= n; i++) {
      aLine = myDescription -> Value (i);
      Standard_Integer res = aLine.Search (myOldName);
      if (res != -1) {
        aLine.Remove (1, myOldName.Length());
        aLine.Insert (1, myName);
        myDescription -> SetValue (i, aLine);
      }
    }
  }
  // Normalize parameter (build all necessary values)
  ProcessParamVal (_DIALOG_SIGN, FLAG_DIALOG, myDialog);
  ProcessParamVal (_MINVAL_SIGN, FLAG_MINVAL, myMinValue);
  ProcessParamVal (_MAXVAL_SIGN, FLAG_MAXVAL, myMaxValue);
  ProcessParamVal (_VALUES_SIGN, FLAG_VALUES, myValues);
  ProcessParamVal (_LENGTH_SIGN, FLAG_LENGTH, aMapLen);
  if ((myFlags & FLAG_LENGTH) && aMapLen.IsIntegerValue())
    myMapLength = aMapLen.IntegerValue();
  switch (myType) {
    case _T_INT:
    case _T_REAL:
    case _T_BOOL:
    case _T_STR: {
        if (!__PossibleParameters[myIndex]._ismap) {
          ProcessParamVal (NULL, FLAG_DEFVAL, myDefValue);
        } else {
          n = myDescription -> Length ();
          // There must me at least <myMapLen> of default values
          if (myMapLength > n || myMapLength == 0) {
            cout << "PlotMgt_PlotterParameter WARNING: Bad Map description: present "
                 << "only " << n << " instead of " << myMapLength << " values in '"
                 << myName << "'" << endl;
            goto _CLEAR_AND_EXIT;
          }
          // Put lines from the end of description
          for (i = n - myMapLength + 1; i <= n; i++)
            myMap -> Append (myDescription -> Value (i));
          myFlags &= ~FLAG_DEFVAL;
          // Leave only map values
          TCollection_AsciiString remStr = (myName + _DELIM_SIGN), aLine;
          n = myMap -> Length ();
          for (i = 1; i <= n; i++) {
            aLine = myMap -> Value (i);
            if (aLine.Search(remStr) != -1) {
              aLine.Remove (1, remStr.Length());
              myMap -> SetValue (i, aLine);
            }
          }
        }
      } break;
    case _T_LSTR: {
        ProcessParamVal (NULL, FLAG_DEFVAL, myDefValue);
      } break;
#ifndef DEB
    default:
      break ;
#endif
  }
  // Check the configuration
  if (myFlags & FLAG_DEFVAL && myDefValue.IsEmpty())
    myFlags &= ~FLAG_DEFVAL;
  if (myFlags & FLAG_DEFVAL && (myType == _T_INT)) {
    if (!myDefValue.IsIntegerValue()) {
      myDefValue = "";
      myFlags &= ~FLAG_DEFVAL;
    }
  }
  if (myFlags & FLAG_DEFVAL && (myType == _T_REAL)) {
    if (!myDefValue.IsRealValue()) {
      myDefValue = "";
      myFlags &= ~FLAG_DEFVAL;
    }
  }
  if (myFlags & FLAG_DEFVAL && (myType == _T_BOOL)) {
    STRING_TRIM(myDefValue);
    myDefValue.LowerCase ();
    if (myDefValue.IsEmpty ())
      myFlags &= ~FLAG_DEFVAL;
  }
  if (myType == _T_LSTR && !CheckListValue ())
    goto _CLEAR_AND_EXIT;
  // Indicate that parameter is configured properly
  myConfigState = Standard_True;
_CLEAR_AND_EXIT:
  //Dump ();
  // We do not need it anymore
  myDescription->Clear ();
  myDescription.Nullify();
}

//============================================================================
//============================================================================
//============================================================================
#define GET_BAD_TYPE_INFO(aGetType)                              \
  cout << "PlotMgt_PlotterParameter ---> WARNING : '" << myName  \
       << "' of type '" << PlotMgt::StringFromType (myType)      \
       << "' requested about " << aGetType << " value"           \
       << endl << flush;

#define GET_EMPTY_VALUE_INFO(aValue)                             \
  cout << "PlotMgt_PlotterParameter ---> WARNING : '" << myName  \
       << "' of type '" << PlotMgt::StringFromType (myType)      \
       << "' has no default value."                              \
       << "Defaulting to " << aValue << "." << endl << flush;

//============================================================================
void PlotMgt_PlotterParameter::SValue (TCollection_AsciiString& aValue) const
{
  aValue = "";
  if ((myType == _T_STR) || (myType == _T_LSTR)) {
    if (myFlags & FLAG_DEFVAL && !myDefValue.IsEmpty()) {
      aValue = myDefValue;
    } else
      GET_EMPTY_VALUE_INFO("empty string");
  } else
    GET_BAD_TYPE_INFO("STRING");
}

//============================================================================
Standard_Boolean PlotMgt_PlotterParameter::BValue () const
{
  if (myType == _T_BOOL) {
    if (myFlags & FLAG_DEFVAL && !myDefValue.IsEmpty()) {
      if (myDefValue.IsEqual("true") ||
            myDefValue.IsIntegerValue() && myDefValue.IntegerValue())
        return Standard_True;
    } else
      GET_EMPTY_VALUE_INFO("FALSE");
  } else
    GET_BAD_TYPE_INFO("BOOLEAN");
  return Standard_False;
}

//============================================================================
Standard_Integer PlotMgt_PlotterParameter::IValue () const
{
  if (myType == _T_INT) {
    if (myFlags & FLAG_DEFVAL) {
      return myDefValue.IntegerValue ();
    } else
      GET_EMPTY_VALUE_INFO("0");
  } else
    GET_BAD_TYPE_INFO("INTEGER");
  return 0;
}

//============================================================================
Standard_Real PlotMgt_PlotterParameter::RValue () const
{
  if (myType == _T_REAL) {
    if (myFlags & FLAG_DEFVAL) {
      return myDefValue.RealValue ();
    } else
      GET_EMPTY_VALUE_INFO("0.0");
  } else
    GET_BAD_TYPE_INFO("REAL");
  return 0.0;
}

//============================================================================
void PlotMgt_PlotterParameter::LValues (Handle(TColStd_HSequenceOfAsciiString)& aList) const
{
  if (!aList.IsNull ()) {
    aList->Clear  ();
    aList.Nullify ();
  }
  aList = new TColStd_HSequenceOfAsciiString ();
  if (myType != _T_LSTR) {
    GET_BAD_TYPE_INFO("LIST_STRING");
    return;
  }
  TCollection_AsciiString aToken, aValues = myValues;
  Standard_Boolean commaFound;
  Standard_Integer idx;
  do {
    idx = aValues.Search (",");
    commaFound = (idx != -1);
    if (commaFound) {
      aToken = aValues.Token (",", 1);
      aValues.Remove (1, aToken.Length() + 1);
    } else {
      aToken = aValues;
    }
    aList -> Append (aToken);
  } while (commaFound);
}

//============================================================================
Handle(TColStd_HSequenceOfAsciiString) PlotMgt_PlotterParameter::MValue () const
{
  if (!(myFlags & FLAG_LENGTH && myMapLength))
    GET_BAD_TYPE_INFO("MAP_VALUE");
  return myMap;
}

//============================================================================
//============================================================================
//============================================================================
#define SET_VALUE()              \
  myDefValue    = aValue;        \
  myFlags       = FLAG_DEFVAL;   \
  myIsModified  = Standard_True;
//  myFlags      != FLAG_DEFVAL;

#define SET_BAD_TYPE_INFO(aSetType)                              \
{                                                                \
  cout << "PlotMgt_PlotterParameter ---> WARNING : '" << myName  \
       << "' of type '" << PlotMgt::StringFromType (myType)      \
       << "' requested to set " << aSetType << " value"          \
       << endl << flush;                                         \
  return;                                                        \
}

//============================================================================
void PlotMgt_PlotterParameter::SetSValue (const TCollection_AsciiString& aValue)
{
  if (myType != _T_STR && myType != _T_LSTR)
    SET_BAD_TYPE_INFO("STRING");
  SET_VALUE();
}

//============================================================================
void PlotMgt_PlotterParameter::SetBValue (const Standard_Boolean aValue)
{
  if (myType != _T_BOOL)
    SET_BAD_TYPE_INFO("BOOL");
//JR/Hp
  myDefValue    = (Standard_CString ) (aValue ? "true" : "false");
//  myDefValue    = (aValue ? "true" : "false");
//  myFlags      != FLAG_DEFVAL;
  myFlags       = FLAG_DEFVAL;
  myIsModified  = Standard_True;
}

//============================================================================
void PlotMgt_PlotterParameter::SetIValue (const Standard_Integer aValue)
{
  if (myType != _T_INT)
    SET_BAD_TYPE_INFO("INTEGER");
  SET_VALUE();
}

//============================================================================
void PlotMgt_PlotterParameter::SetRValue (const Standard_Real aValue)
{
  if (myType != _T_REAL)
    SET_BAD_TYPE_INFO("REAL");
  SET_VALUE();
}

//============================================================================
void PlotMgt_PlotterParameter::SetMValue (const Handle(TColStd_HSequenceOfAsciiString)& aMap)
{
  if (!(myFlags & FLAG_LENGTH && myMapLength))
    SET_BAD_TYPE_INFO("MAP_VALUE");
  myMap        = aMap;
  myMapLength  = myMap -> Length ();
  myIsModified = Standard_True;
}
