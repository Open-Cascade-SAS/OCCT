// Copyright (c) 2022 OPEN CASCADE SAS
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

#ifndef _DE_ConfigurationContext_HeaderFile
#define _DE_ConfigurationContext_HeaderFile

#include <NCollection_DataMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_List.hxx>

//! Provides convenient interface to resource file
//! Allows loading of the resource file and getting attributes'
//! values starting from some scope, for example
//! if scope is defined as "ToV4" and requested parameter
//! is "exec.op", value of "ToV4.exec.op" parameter from
//! the resource file will be returned
class DE_ConfigurationContext : public Standard_Transient
{
public:
  DEFINE_STANDARD_RTTIEXT(DE_ConfigurationContext, Standard_Transient)

  //! Creates an empty tool
  Standard_EXPORT DE_ConfigurationContext();

  //! Import the custom configuration
  //! Save all parameters with their values.
  //! @param[in] theConfiguration path to configuration file or string value
  //! @return true in case of success, false otherwise
  Standard_EXPORT bool Load(const TCollection_AsciiString& theConfiguration);

  //! Import the resource file.
  //! Save all parameters with their values.
  //! @param[in] theFile path to the resource file
  //! @return true in case of success, false otherwise
  Standard_EXPORT bool LoadFile(const TCollection_AsciiString& theFile);

  //! Import the resource string.
  //! Save all parameters with their values.
  //! @param[in] theResource string with resource content
  //! @return true in case of success, false otherwise
  Standard_EXPORT bool LoadStr(const TCollection_AsciiString& theResource);

  //! Checks for existing the parameter name
  //! @param[in] theParam complex parameter name
  //! @param[in] theScope base parameter name
  //! @return true if parameter is defined in the resource file
  Standard_EXPORT bool IsParamSet(const TCollection_AsciiString& theParam,
                                  const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[out] theValue value to get by parameter
  //! @param[in] theScope base parameter name
  //! @return false if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetReal(const TCollection_AsciiString& theParam,
                               double&                        theValue,
                               const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[out] theValue value to get by parameter
  //! @param[in] theScope base parameter name
  //! @return false if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetInteger(const TCollection_AsciiString& theParam,
                                  int&                           theValue,
                                  const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[out] theValue value to get by parameter
  //! @param[in] theScope base parameter name
  //! @return false if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetBoolean(const TCollection_AsciiString& theParam,
                                  bool&                          theValue,
                                  const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[out] theValue value to get by parameter
  //! @param[in] theScope base parameter name
  //! @return false if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetString(const TCollection_AsciiString& theParam,
                                 TCollection_AsciiString&       theValue,
                                 const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[out] theValue value to get by parameter
  //! @param[in] theScope base parameter name
  //! @return false if parameter is not defined or has a wrong type
  Standard_EXPORT bool GetStringSeq(const TCollection_AsciiString&             theParam,
                                    NCollection_List<TCollection_AsciiString>& theValue,
                                    const TCollection_AsciiString&             theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[in] theDefValue value by default if param is not found or has wrong type
  //! @param[in] theScope base parameter name
  //! @return specific type value
  Standard_EXPORT double RealVal(const TCollection_AsciiString& theParam,
                                 const double                   theDefValue,
                                 const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[in] theDefValue value by default if param is not found or has wrong type
  //! @param[in] theScope base parameter name
  //! @return specific type value
  Standard_EXPORT int IntegerVal(const TCollection_AsciiString& theParam,
                                 const int                      theDefValue,
                                 const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[in] theDefValue value by default if param is not found or has wrong type
  //! @param[in] theScope base parameter name
  //! @return specific type value
  Standard_EXPORT bool BooleanVal(const TCollection_AsciiString& theParam,
                                  const bool                     theDefValue,
                                  const TCollection_AsciiString& theScope = "") const;

  //! Gets value of parameter as being of specific type
  //! @param[in] theParam complex parameter name
  //! @param[in] theDefValue value by default if param is not found or has wrong type
  //! @param[in] theScope base parameter name
  //! @return specific type value
  Standard_EXPORT TCollection_AsciiString
    StringVal(const TCollection_AsciiString& theParam,
              const TCollection_AsciiString& theDefValue,
              const TCollection_AsciiString& theScope = "") const;

  //! Gets internal resource map
  //! @return map with resource value
  Standard_EXPORT const NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>&
                        GetInternalMap() const
  {
    return myResource;
  }

protected:
  //! Update the resource with param value from the line
  //! @paramp[in] theResourceLine line contains the parameter
  //! @return true if theResourceLine has loaded correctly
  bool load(const TCollection_AsciiString& theResourceLine);

private:
  NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>
    myResource; //!< Internal parameters map
};

#endif // _DE_ConfigurationContext_HeaderFile
