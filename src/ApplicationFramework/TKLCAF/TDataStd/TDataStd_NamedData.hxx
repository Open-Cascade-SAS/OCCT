// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _TDataStd_NamedData_HeaderFile
#define _TDataStd_NamedData_HeaderFile

#include <TDF_Attribute.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>

class TDataStd_HDataMapOfStringInteger;
class TDataStd_HDataMapOfStringReal;
class TDataStd_HDataMapOfStringString;
class TDataStd_HDataMapOfStringByte;
class TDataStd_HDataMapOfStringHArray1OfInteger;
class TDataStd_HDataMapOfStringHArray1OfReal;
class TCollection_ExtendedString;

//! Contains a named data.
class TDataStd_NamedData : public TDF_Attribute
{
public:
  //! Returns the ID of the named data attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates a named data attribute.
  Standard_EXPORT static occ::handle<TDataStd_NamedData> Set(const TDF_Label& label);

public:
  //! Empty constructor.
  Standard_EXPORT TDataStd_NamedData();

  //! Returns true if at least one named integer value is kept in the attribute.
  Standard_EXPORT bool HasIntegers() const;

  //! Returns true if the attribute contains specified by Name
  //! integer value.
  Standard_EXPORT bool HasInteger(const TCollection_ExtendedString& theName) const;

  //! Returns the integer value specified by the Name.
  //! It returns 0 if internal map doesn't contain the specified
  //! integer (use HasInteger() to check before).
  Standard_EXPORT int GetInteger(const TCollection_ExtendedString& theName);

  //! Defines a named integer.
  //! If the integer already exists, it changes its value to <theInteger>.
  Standard_EXPORT void SetInteger(const TCollection_ExtendedString& theName, const int theInteger);

  //! Returns the internal container of named integers.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString, int>&
                        GetIntegersContainer();

  //! Replace the container content by new content of the <theIntegers>.
  Standard_EXPORT void ChangeIntegers(
    const NCollection_DataMap<TCollection_ExtendedString, int>& theIntegers);

  //! Returns true if at least one named real value is kept in the attribute.
  Standard_EXPORT bool HasReals() const;

  //! Returns true if the attribute contains a real specified by Name.
  Standard_EXPORT bool HasReal(const TCollection_ExtendedString& theName) const;

  //! Returns the named real.
  //! It returns 0.0 if there is no such a named real
  //! (use HasReal()).
  Standard_EXPORT double GetReal(const TCollection_ExtendedString& theName);

  //! Defines a named real.
  //! If the real already exists, it changes its value to <theReal>.
  Standard_EXPORT void SetReal(const TCollection_ExtendedString& theName, const double theReal);

  //! Returns the internal container of named reals.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString, double>&
                        GetRealsContainer();

  //! Replace the container content by new content of the <theReals>.
  Standard_EXPORT void ChangeReals(
    const NCollection_DataMap<TCollection_ExtendedString, double>& theReals);

  //! Returns true if there are some named strings in the attribute.
  Standard_EXPORT bool HasStrings() const;

  //! Returns true if the attribute contains this named string.
  Standard_EXPORT bool HasString(const TCollection_ExtendedString& theName) const;

  //! Returns the named string.
  //! It returns an empty string if there is no such a named string
  //! (use HasString()).
  Standard_EXPORT const TCollection_ExtendedString& GetString(
    const TCollection_ExtendedString& theName);

  //! Defines a named string.
  //! If the string already exists, it changes its value to <theString>.
  Standard_EXPORT void SetString(const TCollection_ExtendedString& theName,
                                 const TCollection_ExtendedString& theString);

  //! Returns the internal container of named strings.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>&
                        GetStringsContainer();

  //! Replace the container content by new content of the <theStrings>.
  Standard_EXPORT void ChangeStrings(
    const NCollection_DataMap<TCollection_ExtendedString, TCollection_ExtendedString>& theStrings);

  //! Returns true if there are some named bytes in the attribute.
  Standard_EXPORT bool HasBytes() const;

  //! Returns true if the attribute contains this named byte.
  Standard_EXPORT bool HasByte(const TCollection_ExtendedString& theName) const;

  //! Returns the named byte.
  //! It returns 0 if there is no such a named byte
  //! (use HasByte()).
  Standard_EXPORT uint8_t GetByte(const TCollection_ExtendedString& theName);

  //! Defines a named byte.
  //! If the byte already exists, it changes its value to <theByte>.
  Standard_EXPORT void SetByte(const TCollection_ExtendedString& theName, const uint8_t theByte);

  //! Returns the internal container of named bytes.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString, uint8_t>&
                        GetBytesContainer();

  //! Replace the container content by new content of the <theBytes>.
  Standard_EXPORT void ChangeBytes(
    const NCollection_DataMap<TCollection_ExtendedString, uint8_t>& theBytes);

  //! Returns true if there are some named arrays of integer values in the attribute.
  Standard_EXPORT bool HasArraysOfIntegers() const;

  //! Returns true if the attribute contains this named array of integer values.
  Standard_EXPORT bool HasArrayOfIntegers(const TCollection_ExtendedString& theName) const;

  //! Returns the named array of integer values.
  //! It returns a NULL Handle if there is no such a named array of integers
  //! (use HasArrayOfIntegers()).
  Standard_EXPORT const occ::handle<NCollection_HArray1<int>>& GetArrayOfIntegers(
    const TCollection_ExtendedString& theName);

  //! Defines a named array of integer values.
  //! @param[in] theName  key
  //! @param[in] theArrayOfIntegers  new value, overrides existing (passed array will be copied by
  //! value!)
  void SetArrayOfIntegers(const TCollection_ExtendedString&            theName,
                          const occ::handle<NCollection_HArray1<int>>& theArrayOfIntegers)
  {
    Backup();
    setArrayOfIntegers(theName, theArrayOfIntegers);
  }

  //! Returns the internal container of named arrays of integer values.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString,
                                            occ::handle<NCollection_HArray1<int>>>&
                        GetArraysOfIntegersContainer();

  //! Replace the container content by new content of the <theArraysOfIntegers>.
  Standard_EXPORT void ChangeArraysOfIntegers(
    const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<int>>>&
      theArraysOfIntegers);

  //! Returns true if there are some named arrays of real values in the attribute.
  Standard_EXPORT bool HasArraysOfReals() const;

  //! Returns true if the attribute contains this named array of real values.
  Standard_EXPORT bool HasArrayOfReals(const TCollection_ExtendedString& theName) const;

  //! Returns the named array of real values.
  //! It returns a NULL Handle if there is no such a named array of reals
  //! (use HasArrayOfReals()).
  Standard_EXPORT const occ::handle<NCollection_HArray1<double>>& GetArrayOfReals(
    const TCollection_ExtendedString& theName);

  //! Defines a named array of real values.
  //! @param[in] theName key
  //! @param[in] theArrayOfReals new value, overrides existing (passed array will be copied by
  //! value!)
  void SetArrayOfReals(const TCollection_ExtendedString&               theName,
                       const occ::handle<NCollection_HArray1<double>>& theArrayOfReals)
  {
    Backup();
    setArrayOfReals(theName, theArrayOfReals);
  }

  //! Returns the internal container of named arrays of real values.
  Standard_EXPORT const NCollection_DataMap<TCollection_ExtendedString,
                                            occ::handle<NCollection_HArray1<double>>>&
                        GetArraysOfRealsContainer();

  //! Replace the container content by new content of the <theArraysOfReals>.
  Standard_EXPORT void ChangeArraysOfReals(
    const NCollection_DataMap<TCollection_ExtendedString, occ::handle<NCollection_HArray1<double>>>&
      theArraysOfReals);

  //! Clear data.
  void Clear()
  {
    Backup();
    clear();
  }

public: //! @name late-load deferred data interface
  //! Returns TRUE if some data is not loaded from deferred storage and can be loaded using
  //! LoadDeferredData().
  //!
  //! Late-load interface allows to avoid loading auxiliary data into memory until it is needed by
  //! application and also speed up reader by skipping data chunks in file. This feature requires
  //! file format having special structure, and usually implies read-only access, therefore default
  //! implementation will return FALSE here.
  //!
  //! Late-load elements require special attention to ensure data consistency,
  //! as such elements are created in undefined state (no data) and Undo/Redo mechanism will not
  //! work until deferred data being loaded.
  //!
  //! Usage scenarios:
  //! - Application displays model in read-only way.
  //!   Late-load elements are loaded temporarily on demand and immediately unloaded.
  //!     theNamedData->LoadDeferredData (true);
  //!     TCollection_AsciiString aValue = theNamedData->GetString (theKey);
  //!     theNamedData->UnloadDeferredData();
  //! - Application saves the model into another format.
  //!   All late-load elements should be loaded (at least temporary during operation).
  //! - Application modifies the model.
  //!   Late-load element should be loaded with removed link to deferred storage,
  //!   so that Undo()/Redo() will work as expected since loading.
  //!     theNamedData->LoadDeferredData (false);
  //!     theNamedData->SetString (theKey, theNewValue);
  virtual bool HasDeferredData() const { return false; }

  //! Load data from deferred storage, without calling Backup().
  //! As result, the content of the object will be overridden by data from deferred storage (which
  //! is normally read-only).
  //! @param[in] theToKeepDeferred  when TRUE, the link to deferred storage will be preserved
  //!                               so that it will be possible calling UnloadDeferredData()
  //!                               afterwards for releasing memory
  //! @return FALSE if deferred storage is unavailable or deferred data has been already loaded
  virtual bool LoadDeferredData(bool theToKeepDeferred = false)
  {
    (void)theToKeepDeferred;
    return false;
  }

  //! Releases data if object has connected deferred storage, without calling Backup().
  //! WARNING! This operation does not unload modifications to deferred storage (normally it is
  //! read-only), so that modifications will be discarded (if any).
  //! @return FALSE if object has no deferred data
  virtual bool UnloadDeferredData() { return false; }

public:
  //! Clear data without calling Backup().
  Standard_EXPORT void clear();

  //! Defines a named integer (without calling Backup).
  Standard_EXPORT void setInteger(const TCollection_ExtendedString& theName, const int theInteger);

  //! Defines a named real (without calling Backup).
  Standard_EXPORT void setReal(const TCollection_ExtendedString& theName, const double theReal);

  //! Defines a named string (without calling Backup).
  Standard_EXPORT void setString(const TCollection_ExtendedString& theName,
                                 const TCollection_ExtendedString& theString);

  //! Defines a named byte (without calling Backup).
  Standard_EXPORT void setByte(const TCollection_ExtendedString& theName, const uint8_t theByte);

  //! Defines a named array of integer values (without calling Backup).
  Standard_EXPORT void setArrayOfIntegers(
    const TCollection_ExtendedString&            theName,
    const occ::handle<NCollection_HArray1<int>>& theArrayOfIntegers);

  //! Defines a named array of real values (without calling Backup).
  Standard_EXPORT void setArrayOfReals(
    const TCollection_ExtendedString&               theName,
    const occ::handle<NCollection_HArray1<double>>& theArrayOfReals);

public: //! @name TDF_Attribute interface
  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_NamedData, TDF_Attribute)

protected:
  occ::handle<TDataStd_HDataMapOfStringInteger>          myIntegers;
  occ::handle<TDataStd_HDataMapOfStringReal>             myReals;
  occ::handle<TDataStd_HDataMapOfStringString>           myStrings;
  occ::handle<TDataStd_HDataMapOfStringByte>             myBytes;
  occ::handle<TDataStd_HDataMapOfStringHArray1OfInteger> myArraysOfIntegers;
  occ::handle<TDataStd_HDataMapOfStringHArray1OfReal>    myArraysOfReals;
};

#endif // _TDataStd_NamedData_HeaderFile
