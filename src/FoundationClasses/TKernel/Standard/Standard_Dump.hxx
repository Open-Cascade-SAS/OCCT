// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Standard_Dump_HeaderFile
#define _Standard_Dump_HeaderFile

#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_List.hxx>
#include <Standard_OStream.hxx>
#include <Standard_SStream.hxx>
#include <TCollection_AsciiString.hxx>

//!@file
//! The file contains interface to prepare dump output for OCCT objects. Format of the dump is JSON.
//!
//! To prepare this output, implement method DumpJson in the object and use macro functions from
//! this file. Macros have one parameter for both, key and the value. It is a field of the current
//! class. Macro has internal analyzer that uses the variable name to generate key. If the parameter
//! has prefix symbols "&", "*" and "my", it is cut.
//!
//! - OCCT_DUMP_FIELD_VALUE_NUMERICAL. Use it for fields of numerical C++ types, like int, float,
//! double. It creates a pair "key", "value",
//! - OCCT_DUMP_FIELD_VALUE_NUMERICAL_INC. Use it for fields of numerical C++ types, like int,
//! float, double.
//!     It creates a pair "key_inc", "value",
//! - OCCT_DUMP_FIELD_VALUE_STRING. Use it for char* type. It creates a pair "key", "value",
//! - OCCT_DUMP_FIELD_VALUE_POINTER. Use it for pointer fields. It creates a pair "key", "value",
//! where the value is the pointer address,
//! - OCCT_DUMP_FIELD_VALUES_DUMPED. Use it for fields that has own Dump implementation. It expects
//! the pointer to the class instance.
//!     It creates "key": { result of dump of the field }
//! - OCCT_DUMP_FIELD_VALUES_DUMPED_INC. Use it for fields that has own Dump implementation. It
//! expects the pointer to the class instance.
//!     It creates "key_inc": { result of dump of the field }
//! - OCCT_DUMP_STREAM_VALUE_DUMPED. Use it for Standard_SStream. It creates "key": { text of stream
//! }
//! - OCCT_DUMP_FIELD_VALUES_NUMERICAL. Use it for unlimited list of fields of C++ double type.
//!     It creates massive of values [value_1, value_2, ...]
//! - OCCT_DUMP_FIELD_VALUES_STRING. Use it for unlimited list of fields of TCollection_AsciiString
//! types.
//!     It creates massive of values ["value_1", "value_2", ...]
//! - OCCT_DUMP_BASE_CLASS. Use if Dump implementation of the class is virtual, to perform
//! ClassName::Dump() of the parent class,
//!     expected parameter is the parent class name.
//!     It creates "key": { result of dump of the field }
//! - OCCT_DUMP_VECTOR_CLASS. Use it as a single row in some object dump to have one row in output.
//!     It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call.
//!     It creates massive of values [value_1, value_2, ...]
//!
//! The Dump result prepared by these macros is an output stream, it is not arranged with spaces and
//! line feed. To have output in a more readable way, use ConvertToAlignedString method for obtained
//! stream.

//! Converts the class type into a string value
#define OCCT_CLASS_NAME(theClass) #theClass

//! @def OCCT_DUMP_CLASS_BEGIN
//! Creates an instance of Sentry to cover the current Dump implementation with keys of start and
//! end. This row should be inserted before other macros. The end key will be added by the sentry
//! remove, (exit of the method).
#define OCCT_DUMP_CLASS_BEGIN(theOStream, theField)                                                \
  {                                                                                                \
    const char* className = OCCT_CLASS_NAME(theField);                                             \
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, className)                                            \
  }

//! @def OCCT_DUMP_TRANSIENT_CLASS_BEGIN
//! Creates an instance of Sentry to cover the current Dump implementation with keys of start and
//! end. This row should be inserted before other macros. The end key will be added by the sentry
//! remove, (exit of the method).
#define OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)                                                \
  {                                                                                                \
    const char* className = get_type_name();                                                       \
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, className)                                            \
  }

//! @def OCCT_DUMP_FIELD_VALUE_NUMERICAL
//! Append into output value: "Name": Field
#define OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, theField)                                      \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                     \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << aName << "\": " << theField;                                             \
  }

//! @def OCCT_DUMP_FIELD_VALUE_NUMERICAL
//! Append into output value: "Name": Field
//! Inc name value added to the key to provide unique keys
#define OCCT_DUMP_FIELD_VALUE_NUMERICAL_INC(theOStream, theField, theIncName)                      \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField) + theIncName;        \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << aName << "\": " << theField;                                             \
  }

//! @def OCCT_INIT_FIELD_VALUE_REAL
//! Append vector values into output value: "Name": [value_1, value_2, ...]
//! This macro is intended to have only one row for dumped object in Json.
//! It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call, but pay attention that
//! it should be only one row in the object dump.
#define OCCT_INIT_FIELD_VALUE_REAL(theOStream, theStreamPos, theField)                             \
  {                                                                                                \
    Standard_Integer aStreamPos = theStreamPos;                                                    \
    if (!Standard_Dump::ProcessFieldName(theOStream, #theField, aStreamPos))                       \
      return Standard_False;                                                                       \
    TCollection_AsciiString aValueText;                                                            \
    if (!Standard_Dump::InitValue(theOStream, aStreamPos, aValueText)                              \
        || !aValueText.IsRealValue())                                                              \
      return Standard_False;                                                                       \
    theField     = aValueText.RealValue();                                                         \
    theStreamPos = aStreamPos;                                                                     \
  }

//! @def OCCT_INIT_FIELD_VALUE_NUMERICAL
//! Append vector values into output value: "Name": [value_1, value_2, ...]
//! This macro is intended to have only one row for dumped object in Json.
//! It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call, but pay attention that
//! it should be only one row in the object dump.
#define OCCT_INIT_FIELD_VALUE_INTEGER(theOStream, theStreamPos, theField)                          \
  {                                                                                                \
    Standard_Integer aStreamPos = theStreamPos;                                                    \
    if (!Standard_Dump::ProcessFieldName(theOStream, #theField, aStreamPos))                       \
      return Standard_False;                                                                       \
    TCollection_AsciiString aValueText;                                                            \
    if (!Standard_Dump::InitValue(theOStream, aStreamPos, aValueText)                              \
        || !aValueText.IsIntegerValue())                                                           \
      return Standard_False;                                                                       \
    theField     = aValueText.IntegerValue();                                                      \
    theStreamPos = aStreamPos;                                                                     \
  }

//! @def OCCT_DUMP_FIELD_VALUE_STRING
//! Append into output value: "Name": "Field"
#define OCCT_DUMP_FIELD_VALUE_STRING(theOStream, theField)                                         \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                     \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << aName << "\": \"" << theField << "\"";                                   \
  }

//! @def OCCT_DUMP_FIELD_VALUE_POINTER
//! Append into output value: "Name": "address of the pointer"
#define OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, theField)                                        \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                     \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << aName << "\": \"" << Standard_Dump::GetPointerInfo(theField) << "\"";    \
  }

//! @def OCCT_DUMP_FIELD_VALUE_STRING
//! Append into output value: "Name": "Field"
#define OCCT_DUMP_FIELD_VALUE_GUID(theOStream, theField)                                           \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                     \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    char aStr[Standard_GUID_SIZE_ALLOC];                                                           \
    theField.ToCString(aStr);                                                                      \
    theOStream << "\"" << aName << "\": \"" << aStr << "\"";                                       \
  }

//! @def OCCT_DUMP_FIELD_VALUES_DUMPED
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the fields. The expected field is a pointer.
//! Use this macro for fields of the dumped class which has own Dump implementation.
//! The macros is recursive. Recursion is stopped when the depth value becomes equal to zero.
//! Depth = -1 is the default value, dump here is unlimited.
#define OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, theField)                              \
  {                                                                                                \
    if (theDepth != 0 && (void*)(theField) != NULL)                                                \
    {                                                                                              \
      Standard_SStream aFieldStream;                                                               \
      (theField)->DumpJson(aFieldStream, theDepth - 1);                                            \
      TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                   \
      Standard_Dump::DumpKeyToClass(theOStream, aName, Standard_Dump::Text(aFieldStream));         \
    }                                                                                              \
  }

//! @def OCCT_DUMP_FIELD_VALUES_DUMPED_INC
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the fields. The expected field is a pointer.
//! Use this macro for fields of the dumped class which has own Dump implementation.
//! The macros is recursive. Recursion is stopped when the depth value becomes equal to zero.
//! Depth = -1 is the default value, dump here is unlimited.
//! Inc name value added to the key to provide unique keys
#define OCCT_DUMP_FIELD_VALUES_DUMPED_INC(theOStream, theDepth, theField, theIncName)              \
  {                                                                                                \
    if (theDepth != 0 && (void*)(theField) != NULL)                                                \
    {                                                                                              \
      Standard_SStream aFieldStream;                                                               \
      (theField)->DumpJson(aFieldStream, theDepth - 1);                                            \
      TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField) + theIncName;      \
      Standard_Dump::DumpKeyToClass(theOStream, aName, Standard_Dump::Text(aFieldStream));         \
    }                                                                                              \
  }

//! @def OCCT_INIT_FIELD_VALUES_DUMPED
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the fields. The expected field is a pointer.
//! Use this macro for fields of the dumped class which has own Dump implementation.
//! The macros is recursive. Recursion is stopped when the depth value becomes equal to zero.
//! Depth = -1 is the default value, dump here is unlimited.
#define OCCT_INIT_FIELD_VALUES_DUMPED(theSStream, theStreamPos, theField)                          \
  {                                                                                                \
    if ((theField) == NULL || !(theField)->InitFromJson(theSStream, theStreamPos))                 \
      return Standard_False;                                                                       \
  }

//! @def OCCT_DUMP_STREAM_VALUE_DUMPED
//! Append into output value: "Name": { stream text }
//! It computes Dump of the fields. The expected field is a pointer.
//! Use this macro for Standard_SStream field.
#define OCCT_DUMP_STREAM_VALUE_DUMPED(theOStream, theField)                                        \
  {                                                                                                \
    TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(#theField);                     \
    Standard_Dump::DumpKeyToClass(theOStream, aName, Standard_Dump::Text(theField));               \
  }

//! @def OCCT_DUMP_FIELD_VALUES_NUMERICAL
//! Append real values into output values in an order: [value_1, value_2, ...]
//! It computes Dump of the parent. The expected field is a parent class name to call
//! ClassName::Dump.
#define OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, theName, theCount, ...)                       \
  {                                                                                                \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << theName << "\": [";                                                      \
    Standard_Dump::DumpRealValues(theOStream, theCount, __VA_ARGS__);                              \
    theOStream << "]";                                                                             \
  }

//! @def OCCT_DUMP_FIELD_VALUES_STRING
//! Append real values into output values in an order: ["value_1", "value_2", ...]
//! It computes Dump of the parent. The expected field is a parent class name to call
//! ClassName::Dump.
#define OCCT_DUMP_FIELD_VALUES_STRING(theOStream, theName, theCount, ...)                          \
  {                                                                                                \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << theName << "\": [";                                                      \
    Standard_Dump::DumpCharacterValues(theOStream, theCount, __VA_ARGS__);                         \
    theOStream << "]";                                                                             \
  }

//! @def OCCT_DUMP_BASE_CLASS
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the parent. The expected field is a parent class name to call
//! ClassName::Dump. Use this macro for parent of the current class. The macros is recursive.
//! Recursive is stopped when the depth value becomes equal to zero. Depth = -1 is the default
//! value, dump here is unlimited.
#define OCCT_DUMP_BASE_CLASS(theOStream, theDepth, theField)                                       \
  {                                                                                                \
    if (theDepth != 0)                                                                             \
    {                                                                                              \
      Standard_Dump::AddValuesSeparator(theOStream);                                               \
      theField::DumpJson(theOStream, theDepth - 1);                                                \
    }                                                                                              \
  }

//! @def OCCT_DUMP_VECTOR_CLASS
//! Append vector values into output value: "Name": [value_1, value_2, ...]
//! This macro is intended to have only one row for dumped object in Json.
//! It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call, but pay attention that
//! it should be only one row in the object dump.
#define OCCT_DUMP_VECTOR_CLASS(theOStream, theName, theCount, ...)                                 \
  {                                                                                                \
    Standard_Dump::AddValuesSeparator(theOStream);                                                 \
    theOStream << "\"" << theName << "\": [";                                                      \
    Standard_Dump::DumpRealValues(theOStream, theCount, __VA_ARGS__);                              \
    theOStream << "]";                                                                             \
  }

//! @def OCCT_INIT_VECTOR_CLASS
//! Append vector values into output value: "Name": [value_1, value_2, ...]
//! This macro is intended to have only one row for dumped object in Json.
//! It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call, but pay attention that
//! it should be only one row in the object dump.
#define OCCT_INIT_VECTOR_CLASS(theOStream, theName, theStreamPos, theCount, ...)                   \
  {                                                                                                \
    Standard_Integer aStreamPos = theStreamPos;                                                    \
    if (!Standard_Dump::ProcessStreamName(theOStream, theName, aStreamPos))                        \
      return Standard_False;                                                                       \
    if (!Standard_Dump::InitRealValues(theOStream, aStreamPos, theCount, __VA_ARGS__))             \
      return Standard_False;                                                                       \
    theStreamPos = aStreamPos;                                                                     \
  }

//! Kind of key in Json string
enum Standard_JsonKey
{
  Standard_JsonKey_None,                 //!< no key
  Standard_JsonKey_OpenChild,            //!< "{"
  Standard_JsonKey_CloseChild,           //!< "}"
  Standard_JsonKey_OpenContainer,        //!< "["
  Standard_JsonKey_CloseContainer,       //!< "]"
  Standard_JsonKey_Quote,                //!< "\""
  Standard_JsonKey_SeparatorKeyToValue,  //!< ": "
  Standard_JsonKey_SeparatorValueToValue //!< ", "
};

//! Type for storing a dump value with the stream position
struct Standard_DumpValue
{
  Standard_DumpValue()
      : myStartPosition(0)
  {
  }

  Standard_DumpValue(const TCollection_AsciiString& theValue, const Standard_Integer theStartPos)
      : myValue(theValue),
        myStartPosition(theStartPos)
  {
  }

  TCollection_AsciiString myValue;         //!< current string value
  Standard_Integer        myStartPosition; //!< position of the value first char in the whole stream
};

//! This interface has some tool methods for stream (in JSON format) processing.
class Standard_Dump
{
public:
  //! Converts stream value to string value. The result is original stream value.
  //! @param theStream source value
  //! @return text presentation
  Standard_EXPORT static TCollection_AsciiString Text(const Standard_SStream& theStream);

  //! Converts stream value to string value. Improves the text presentation with the following
  //! cases:
  //! - for '{' append after '\n' and indent to the next value, increment current indent value
  //! - for '}' append '\n' and current indent before it, decrement indent value
  //! - for ',' append after '\n' and indent to the next value. If the current symbol is in massive
  //! container [], do nothing Covers result with opened and closed brackets on the top level, if it
  //! has no symbols there.
  //! @param theStream source value
  //! @param theIndent count of ' ' symbols to apply hierarchical indent of the text values
  //! @return text presentation
  Standard_EXPORT static TCollection_AsciiString FormatJson(const Standard_SStream& theStream,
                                                            const Standard_Integer  theIndent = 3);

  //! Converts stream into map of values.
  //!
  //! The one level stream example: 'key_1: value_1, key_2: value_2'
  //! In output: values contain 'key_1: value_1' and 'key_2: value_2'.
  //!
  //! The two level stream example: 'key_1: value_1, key_2: value_2, key_3: {sublevel_key_1:
  //! sublevel_value_1}, key_4: value_4' In output values contain 'key_1: value_1', 'key_2:
  //! value_2', 'key_3: {sublevel_key_1: sublevel_value_1}' and 'key_4: value_4'. The sublevel value
  //! might be processed later using the same method.
  //!
  //! @param theStreamStr stream value
  //! @param[out] theKeyToValues  container of split values. It contains key to value and position
  //! of the value in the stream text
  Standard_EXPORT static Standard_Boolean SplitJson(
    const TCollection_AsciiString&                                           theStreamStr,
    NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theKeyToValues);

  //! Returns container of indices in values, that has hierarchical value
  Standard_EXPORT static NCollection_List<Standard_Integer> HierarchicalValueIndices(
    const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues);

  //! Returns true if the value has bracket key
  Standard_EXPORT static Standard_Boolean HasChildKey(
    const TCollection_AsciiString& theSourceValue);

  //! Returns key value for enum type
  Standard_EXPORT static Standard_CString JsonKeyToString(const Standard_JsonKey theKey);

  //! Returns length value for enum type
  Standard_EXPORT static Standard_Integer JsonKeyLength(const Standard_JsonKey theKey);

  //! @param theOStream source value
  static Standard_EXPORT void AddValuesSeparator(Standard_OStream& theOStream);

  //! Returns default prefix added for each pointer info string if short presentation of pointer
  //! used
  static TCollection_AsciiString GetPointerPrefix() { return "0x"; }

  //! Convert handle pointer to address of the pointer. If the handle is NULL, the result is an
  //! empty string.
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo(
    const Handle(Standard_Transient)& thePointer,
    const bool                        isShortInfo = true);

  //! Convert pointer to address of the pointer. If the handle is NULL, the result is an empty
  //! string.
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo(const void* thePointer,
                                                                const bool  isShortInfo = true);

  //! Append into output value: "Name": { Field }
  //! @param[out] theOStream  stream to be fill with values
  //! @param theKey a source value
  //! @param theField stream value
  Standard_EXPORT static void DumpKeyToClass(Standard_OStream&              theOStream,
                                             const TCollection_AsciiString& theKey,
                                             const TCollection_AsciiString& theField);

  //! Unite values in one value using template: "value_1", "value_2", ..., "value_n"
  //! @param[out] theOStream  stream to be fill with values
  //! @param[in] theCount     number of values
  Standard_EXPORT static void DumpCharacterValues(Standard_OStream& theOStream, int theCount, ...);

  //! Unite values in one value using template: value_1, value_2, ..., value_n
  //! @param[out] theOStream  stream to be fill with values
  //! @param[in] theCount     number of values
  Standard_EXPORT static void DumpRealValues(Standard_OStream& theOStream, int theCount, ...);

  //! Check whether the parameter name is equal to the name in the stream at position
  //! @param[in]  theStreamStr stream with values
  //! @param[in]  theName      stream key value
  //! @param[out] theStreamPos current position in the stream
  Standard_EXPORT static Standard_Boolean ProcessStreamName(
    const TCollection_AsciiString& theStreamStr,
    const TCollection_AsciiString& theName,
    Standard_Integer&              theStreamPos);

  //! Check whether the field name is equal to the name in the stream at position
  //! @param[in]  theStreamStr stream with values
  //! @param[in]  theName      stream key field value
  //! @param[out] theStreamPos current position in the stream
  Standard_EXPORT static Standard_Boolean ProcessFieldName(
    const TCollection_AsciiString& theStreamStr,
    const TCollection_AsciiString& theName,
    Standard_Integer&              theStreamPos);

  //! Unite values in one value using template: value_1, value_2, ..., value_n
  //! @param[in]  theStreamStr stream with values
  //! @param[out] theStreamPos current position in the stream
  //! @param[in]  theCount     number of values
  Standard_EXPORT static Standard_Boolean InitRealValues(
    const TCollection_AsciiString& theStreamStr,
    Standard_Integer&              theStreamPos,
    int                            theCount,
    ...);

  //! Returns real value
  //! @param[in]  theStreamStr stream with values
  //! @param[out] theStreamPos current position in the stream
  //! @param[out] theValue     stream value
  Standard_EXPORT static Standard_Boolean InitValue(const TCollection_AsciiString& theStreamStr,
                                                    Standard_Integer&              theStreamPos,
                                                    TCollection_AsciiString&       theValue);

  //! Convert field name into dump text value, removes "&" and "my" prefixes
  //! An example, for field myValue, theName is Value, for &myCLass, the name is Class
  //! @param theField a source value
  Standard_EXPORT static TCollection_AsciiString DumpFieldToName(
    const TCollection_AsciiString& theField);

private:
  //! Extracts from the string value a pair (key, value), add it into output container, update index
  //! value Example: stream string starting the index position contains: ..."key": <value>... a pair
  //! key, value will be added into theValues at beginning theIndex is the position of the quota
  //! before <key>, after the index is the next position after the value splitDumped(aString) gives
  //! theSplitValue = "abc", theTailValue = "defg", theKey = "key"
  Standard_EXPORT static Standard_Boolean splitKeyToValue(
    const TCollection_AsciiString&                                           theStreamStr,
    Standard_Integer                                                         theStartIndex,
    Standard_Integer&                                                        theNextIndex,
    NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theValues);

  //! Returns key of json in the index position. Increment the index position to the next symbol in
  //! the row
  Standard_EXPORT static Standard_Boolean jsonKey(const TCollection_AsciiString& theStreamStr,
                                                  Standard_Integer               theStartIndex,
                                                  Standard_Integer&              theNextIndex,
                                                  Standard_JsonKey&              theKey);

  //! Find position in the source string of the symbol close after the start position.
  //! Ignore combination <symbol open> ... <symbol close> between the close symbol.
  //! Example, for case ... { ... { ... } ...} ... } it returns the position of the forth brace
  Standard_EXPORT static Standard_Integer nextClosePosition(
    const TCollection_AsciiString& theSourceValue,
    const Standard_Integer         theStartPosition,
    const Standard_JsonKey         theCloseKey,
    const Standard_JsonKey         theOpenKey);
};

#endif // _Standard_Dump_HeaderFile
