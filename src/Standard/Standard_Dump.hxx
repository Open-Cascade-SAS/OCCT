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

#include <Standard_SStream.hxx>
#include <TCollection_AsciiString.hxx>

class Standard_DumpSentry;

//! The file contains interface to prepare dump output for OCCT objects. Format of the dump is JSON.
//! To prepare this output, implement method DumpJson in the object and use macro functions from this file.
//! Macros have one parameter for both, key and the value. It is a field of the current class. Macro has internal analyzer that
//! uses the variable name to generate key. If the parameter has prefix symbols "&", "*" and "my", it is cut.
//!
//! - OCCT_DUMP_FIELD_VALUE_NUMERICAL. Use it for fields of numerical C++ types, like int, float, double. It creates a pair "key", "value",
//! - OCCT_DUMP_FIELD_VALUE_STRING. Use it for char* type. It creates a pair "key", "value",
//! - OCCT_DUMP_FIELD_VALUE_POINTER. Use it for pointer fields. It creates a pair "key", "value", where the value is the pointer address,
//! - OCCT_DUMP_FIELD_VALUES_DUMPED. Use it for fields that has own Dump implementation. It expects the pointer to the class instance.
//!     It creates "key": { result of dump of the field }
//! - OCCT_DUMP_FIELD_VALUES_NUMERICAL. Use it for unlimited list of fields of C++ double type.
//!     It creates massive of values [value_1, value_2, ...]
//! - OCCT_DUMP_FIELD_VALUES_STRING. Use it for unlimited list of fields of TCollection_AsciiString types.
//!     It creates massive of values ["value_1", "value_2", ...]
//! - OCCT_DUMP_BASE_CLASS. Use if Dump implementation of the class is virtual, to perform ClassName::Dump() of the parent class,
//!     expected parameter is the parent class name.
//!     It creates "key": { result of dump of the field }
//! - OCCT_DUMP_VECTOR_CLASS. Use it as a single row in some object dump to have one row in output.
//!     It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call.
//!     It creates massive of values [value_1, value_2, ...]
//!
//! The Dump result prepared by these macros is an output stream, it is not arranged with spaces and line feed.
//! To have output in a more readable way, use ConvertToAlignedString method for obtained stream.

//! Converts the class type into a string value
#define OCCT_CLASS_NAME(theClass) #theClass

//! @def OCCT_DUMP_CLASS_BEGIN
//! Creates an instance of Sentry to cover the current Dump implementation with keys of start and end.
//! This row should be inserted before other macros. The end key will be added by the sentry remove,
//! (exit of the method).
#define OCCT_DUMP_CLASS_BEGIN(theOStream, theName) \
  Standard_DumpSentry aSentry (theOStream, OCCT_CLASS_NAME(theName));

//! @def OCCT_DUMP_FIELD_VALUE_NUMERICAL
//! Append into output value: "Name": Field
#define OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, theField) \
{ \
  const char* aName = Standard_Dump::DumpFieldToName (#theField); \
  Standard_Dump::AddValuesSeparator (theOStream); \
  theOStream << "\"" << aName << "\": " << theField; \
}

//! @def OCCT_DUMP_FIELD_VALUE_STRING
//! Append into output value: "Name": "Field"
#define OCCT_DUMP_FIELD_VALUE_STRING(theOStream, theField) \
{ \
  const char* aName = Standard_Dump::DumpFieldToName (#theField); \
  Standard_Dump::AddValuesSeparator (theOStream); \
  theOStream << "\"" << aName << "\": \"" << theField << "\""; \
}

//! @def OCCT_DUMP_FIELD_VALUE_POINTER
//! Append into output value: "Name": "address of the pointer"
#define OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, theField) \
{ \
  const char* aName = Standard_Dump::DumpFieldToName (#theField); \
  Standard_Dump::AddValuesSeparator (theOStream); \
  theOStream << "\"" << aName << "\": \"" << Standard_Dump::GetPointerInfo (theField) << "\""; \
}

//! @def OCCT_DUMP_FIELD_VALUES_DUMPED
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the fields. The expected field is a pointer.
//! Use this macro for fields of the dumped class which has own Dump implementation.
//! The macros is recursive. Recursion is stopped when the depth value becomes equal to zero.
//! Depth = -1 is the default value, dump here is unlimited.
#define OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, theField) \
{ \
  if (theDepth != 0) \
  { \
    Standard_SStream aFieldStream; \
    if ((theField) != NULL) \
      (theField)->DumpJson (aFieldStream, theDepth - 1); \
    const char* aName = Standard_Dump::DumpFieldToName (#theField); \
    Standard_Dump::DumpKeyToClass (theOStream, aName, Standard_Dump::Text (aFieldStream)); \
  } \
}

//! @def OCCT_DUMP_FIELD_VALUES_NUMERICAL
//! Append real values into output values in an order: [value_1, value_2, ...]
//! It computes Dump of the parent. The expected field is a parent class name to call ClassName::Dump.
#define OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, theName, theCount, ...) \
{ \
  Standard_Dump::AddValuesSeparator (theOStream); \
  theOStream << "\"" << theName << "\": ["; \
  Standard_Dump::DumpRealValues (theOStream, theCount, __VA_ARGS__);\
  theOStream << "]"; \
}

//! @def OCCT_DUMP_FIELD_VALUES_STRING
//! Append real values into output values in an order: ["value_1", "value_2", ...]
//! It computes Dump of the parent. The expected field is a parent class name to call ClassName::Dump.
#define OCCT_DUMP_FIELD_VALUES_STRING(theOStream, theName, theCount, ...) \
{ \
  Standard_Dump::AddValuesSeparator (theOStream); \
  theOStream << "\"" << theName << "\": ["; \
  Standard_Dump::DumpCharacterValues (theOStream, theCount, __VA_ARGS__);\
  theOStream << "]"; \
}

//! @def OCCT_DUMP_BASE_CLASS
//! Append into output value: "Name": { field dumped values }
//! It computes Dump of the parent. The expected field is a parent class name to call ClassName::Dump.
//! Use this macro for parent of the current class.
//! The macros is recursive. Recursive is stoped when the depth value becomes equal to zero.
//! Depth = -1 is the default value, dump here is unlimited.
#define OCCT_DUMP_BASE_CLASS(theOStream, theDepth, theField) \
{ \
  if (theDepth != 0) \
  { \
    Standard_SStream aFieldStream; \
    theField::DumpJson (aFieldStream, theDepth - 1); \
    const char* aName = Standard_Dump::DumpFieldToName (#theField); \
    Standard_Dump::DumpKeyToClass (theOStream, aName, Standard_Dump::Text (aFieldStream)); \
  } \
}

//! @def OCCT_DUMP_VECTOR_CLASS
//! Append vector values into output value: "Name": [value_1, value_2, ...]
//! This macro is intended to have only one row for dumped object in Json.
//! It's possible to use it without necessity of OCCT_DUMP_CLASS_BEGIN call, but pay attention that it should be only one row in the object dump.
#define OCCT_DUMP_VECTOR_CLASS(theOStream, theName, theCount, ...) \
{ \
  theOStream << "\"" << OCCT_CLASS_NAME(theName) << "\": ["; \
  Standard_Dump::DumpRealValues (theOStream, theCount, __VA_ARGS__);\
  theOStream << "]"; \
}

//! @brief Simple sentry class providing convenient interface to dump.
//! Appends start and last rows in dump with class name key.
//! An example of the using: for ClassName, the result is: "ClassName" { ... }
//! Create instance of that class in the first row of Dump.
class Standard_DumpSentry
{
public:
  //! Constructor - add parameters of start class name definition in the stream
  Standard_EXPORT Standard_DumpSentry (Standard_OStream& theOStream, const char* theClassName);

  //! Destructor - add parameters of stop class name definition in the stream
  Standard_EXPORT ~Standard_DumpSentry();

private:
  Standard_OStream* myOStream; //!< modified stream
};

//! This interface has some tool methods for stream (in JSON format) processing.
class Standard_Dump
{
public:
  //! Converts stream value to string value. The result is original stream value.
  //! @param theStream source value
  //! @return text presentation
  Standard_EXPORT static TCollection_AsciiString Text (const Standard_SStream& theStream);

  //! Converts stream value to string value. Improves the text presentation with the following cases:
  //! - for '{' append after '\n' and indent to the next value, increment current indent value
  //! - for '}' append '\n' and current indent before it, decrement indent value
  //! - for ',' append after '\n' and indent to the next value. If the current symbol is in massive container [], do nothing
  //! @param theStream source value
  //! @param theIndent count of ' ' symbols to apply hierarchical indent of the text values
  //! @return text presentation
  Standard_EXPORT static TCollection_AsciiString FormatJson (const Standard_SStream& theStream, const Standard_Integer theIndent = 3);

  //! Add Json values separator if the stream last symbol is not an open brace.
  //! @param theStream source value
  static Standard_EXPORT void AddValuesSeparator (Standard_OStream& theOStream);

  //! Returns default prefix added for each pointer info string if short presentation of pointer used
  Standard_EXPORT static TCollection_AsciiString GetPointerPrefix() { return "0x"; }

  //! Convert handle pointer to address of the pointer. If the handle is NULL, the result is an empty string.
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                                 const bool isShortInfo = true);

  //! Convert pointer to address of the pointer. If the handle is NULL, the result is an empty string.
  //! @param thePointer a pointer
  //! @param isShortInfo if true, all '0' symbols in the beginning of the pointer are skipped
  //! @return the string value
  Standard_EXPORT static TCollection_AsciiString GetPointerInfo (const void* thePointer,
                                                                 const bool isShortInfo = true);

  //! Append into output value: "Name": { Field }
  //! @param theOStream [out] stream to be fill with values
  //! @param theKey a source value
  //! @param theField stream value
  Standard_EXPORT static void DumpKeyToClass (Standard_OStream& theOStream,
                                              const char* theKey,
                                              const TCollection_AsciiString& theField);

  //! Unite values in one value using template: "value_1", "value_2", ..., "value_n"
  //! @param theOStream [out] stream to be fill with values
  //! @param theCount numer of values
  Standard_EXPORT static void DumpCharacterValues (Standard_OStream& theOStream, int theCount, ...);

  //! Unite values in one value using template: value_1, value_2, ..., value_n
  //! @param theOStream [out] stream to be fill with values
  //! @param theCount numer of values
  Standard_EXPORT static void DumpRealValues (Standard_OStream& theOStream, int theCount, ...);

  //! Convert field name into dump text value, removes "&" and "my" prefixes
  //! An example, for field myValue, theName is Value, for &myCLass, the name is Class
  //! @param theField a source value 
  //! @param theName [out] an updated name 
  Standard_EXPORT static const char* DumpFieldToName (const char* theField);
};

#endif // _Standard_Dump_HeaderFile
