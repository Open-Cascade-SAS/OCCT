// Created on: 1992-04-06
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _IGESData_GlobalSection_HeaderFile
#define _IGESData_GlobalSection_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class TCollection_HAsciiString;
class Interface_ParamSet;
class Interface_Check;
class gp_XYZ;

//! Description of a global section (corresponds to file header)
//! used as well in IGESModel, IGESReader and IGESWriter
//! Warning : From IGES-5.1, a parameter is added : LastChangeDate (concerns
//! transferred set of data, not the file itself)
//! Of course, it can be absent if read from earlier versions
//! (a default is then to be set to current date)
//! From 5.3, one more : ApplicationProtocol (optional)
class IGESData_GlobalSection
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates an empty GlobalSection, ready to be filled,
  //! Warning : No default value is provided
  Standard_EXPORT IGESData_GlobalSection();

  //! Fills GlobalSection from a ParamSet (i.e. taken from file)
  //! undefined parameters do not change default values when defined
  //! Fills Check about Corrections or Fails
  Standard_EXPORT void Init(const occ::handle<Interface_ParamSet>& params, occ::handle<Interface_Check>& ach);

  //! Copies data referenced by Handle (that is, Strings)
  //! useful to "isolate" a GlobalSection after copy by "="
  //! (from a Model to another Model for instance)
  Standard_EXPORT void CopyRefs();

  //! Returns all contained data in the form of a ParamSet
  //! Remark : Strings are given under Hollerith form
  Standard_EXPORT occ::handle<Interface_ParamSet> Params() const;

  //! Returns a string withpout its Hollerith marks (nnnH ahead).
  //! Remark : all strings stored in GlobalSection are expurged
  //! from Hollerith information (without nnnH)
  //! If <astr> is not Hollerith form, it is simply copied
  Standard_EXPORT occ::handle<TCollection_HAsciiString> TranslatedFromHollerith(
    const occ::handle<TCollection_HAsciiString>& astr) const;

  //! Returns the parameter delimiter character.
  Standard_EXPORT char Separator() const;

  //! Returns the record delimiter character.
  Standard_EXPORT char EndMark() const;

  //! Returns the name of the sending system.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> SendName() const;

  //! Returns the name of the IGES file.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> FileName() const;

  //! Returns the Native System ID of the system that created the IGES file.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> SystemId() const;

  //! Returns the name of the pre-processor used to write the IGES file.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> InterfaceVersion() const;

  //! Returns the number of binary bits for integer representations.
  Standard_EXPORT int IntegerBits() const;

  //! Returns the maximum power of a decimal representation of a
  //! single-precision floating point number in the sending system.
  Standard_EXPORT int MaxPower10Single() const;

  Standard_EXPORT int MaxDigitsSingle() const;

  //! Returns the maximum power of a decimal representation of a
  //! double-precision floating point number in the sending system.
  Standard_EXPORT int MaxPower10Double() const;

  Standard_EXPORT int MaxDigitsDouble() const;

  //! Returns the name of the receiving system.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> ReceiveName() const;

  //! Returns the scale used in the IGES file.
  Standard_EXPORT double Scale() const;

  //! Returns the system length unit
  Standard_EXPORT double CascadeUnit() const;

  //! Returns the unit flag that was used to write the IGES file.
  Standard_EXPORT int UnitFlag() const;

  //! Returns the name of the unit the IGES file was written in.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> UnitName() const;

  //! Returns the maximum number of line weight gradations.
  Standard_EXPORT int LineWeightGrad() const;

  //! Returns the of maximum line weight width in IGES file units.
  Standard_EXPORT double MaxLineWeight() const;

  //! Returns the IGES file creation date.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Date() const;

  //! Returns the resolution used in the IGES file.
  Standard_EXPORT double Resolution() const;

  //! Returns the approximate maximum coordinate value found in the model.
  Standard_EXPORT double MaxCoord() const;

  //! Returns True if the approximate maximum coordinate value found in
  //! the model is greater than 0.
  Standard_EXPORT bool HasMaxCoord() const;

  //! Returns the name of the IGES file author.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> AuthorName() const;

  //! Returns the name of the company where the IGES file was written.
  Standard_EXPORT occ::handle<TCollection_HAsciiString> CompanyName() const;

  //! Returns the IGES version that the IGES file was written in.
  Standard_EXPORT int IGESVersion() const;

  Standard_EXPORT int DraftingStandard() const;

  //! Returns the date and time when the model was created or last
  //! modified (for IGES 5.1 and later).
  Standard_EXPORT occ::handle<TCollection_HAsciiString> LastChangeDate() const;

  //! Returns True if the date and time when the model was created or
  //! last modified are specified, i.e. not defaulted to NULL.
  Standard_EXPORT bool HasLastChangeDate() const;

  Standard_EXPORT void SetLastChangeDate();

  Standard_EXPORT occ::handle<TCollection_HAsciiString> ApplicationProtocol() const;

  Standard_EXPORT bool HasApplicationProtocol() const;

  //! Returns a string built from year,
  //! month, day, hour, minute and second values. The form of the
  //! resulting string is defined as follows:
  //! -      -1: YYMMDD.HHNNSS,
  //! -       0: YYYYMMDD.HHNNSS,
  //! -       1: YYYY-MM-DD:HH-NN-SS, where:
  //! - YYYY or YY is 4 or 2 digit year,
  //! - HH is hour (00-23),
  //! - MM is month (01-12),
  //! - NN is minute (00-59)
  //! - DD is day (01-31),
  //! - SS is second (00-59).
  Standard_EXPORT static occ::handle<TCollection_HAsciiString> NewDateString(
    const int year,
    const int month,
    const int day,
    const int hour,
    const int minut,
    const int second,
    const int mode = -1);

  //! Converts the string given in the
  //! form YYMMDD.HHNNSS or YYYYMMDD.HHNNSS to either
  //! YYMMDD.HHNNSS, YYYYMMDD.HHNNSS or YYYY-MM-DD:HH-NN-SS.
  Standard_EXPORT static occ::handle<TCollection_HAsciiString> NewDateString(
    const occ::handle<TCollection_HAsciiString>& date,
    const int                  mode = 1);

  //! Returns the unit value (in
  //! meters) that the IGES file was written in.
  Standard_EXPORT double UnitValue() const;

  Standard_EXPORT void SetSeparator(const char val);

  Standard_EXPORT void SetEndMark(const char val);

  Standard_EXPORT void SetSendName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetFileName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetSystemId(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetInterfaceVersion(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetIntegerBits(const int val);

  Standard_EXPORT void SetMaxPower10Single(const int val);

  Standard_EXPORT void SetMaxDigitsSingle(const int val);

  Standard_EXPORT void SetMaxPower10Double(const int val);

  Standard_EXPORT void SetMaxDigitsDouble(const int val);

  Standard_EXPORT void SetReceiveName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetCascadeUnit(const double theUnit);

  Standard_EXPORT void SetScale(const double val);

  Standard_EXPORT void SetUnitFlag(const int val);

  Standard_EXPORT void SetUnitName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetLineWeightGrad(const int val);

  Standard_EXPORT void SetMaxLineWeight(const double val);

  Standard_EXPORT void SetDate(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetResolution(const double val);

  Standard_EXPORT void SetMaxCoord(const double val = 0.0);

  Standard_EXPORT void MaxMaxCoord(const double val = 0.0);

  Standard_EXPORT void MaxMaxCoords(const gp_XYZ& xyz);

  Standard_EXPORT void SetAuthorName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetCompanyName(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetIGESVersion(const int val);

  Standard_EXPORT void SetDraftingStandard(const int val);

  Standard_EXPORT void SetLastChangeDate(const occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT void SetApplicationProtocol(const occ::handle<TCollection_HAsciiString>& val);

private:
  char               theSeparator;
  char               theEndMark;
  occ::handle<TCollection_HAsciiString> theSendName;
  occ::handle<TCollection_HAsciiString> theFileName;
  occ::handle<TCollection_HAsciiString> theSystemId;
  occ::handle<TCollection_HAsciiString> theInterfaceVersion;
  int                 theIntegerBits;
  int                 theMaxPower10Single;
  int                 theMaxDigitsSingle;
  int                 theMaxPower10Double;
  int                 theMaxDigitsDouble;
  occ::handle<TCollection_HAsciiString> theReceiveName;
  double                    theScale;
  double                    theCascadeUnit;
  int                 theUnitFlag;
  occ::handle<TCollection_HAsciiString> theUnitName;
  int                 theLineWeightGrad;
  double                    theMaxLineWeight;
  occ::handle<TCollection_HAsciiString> theDate;
  double                    theResolution;
  double                    theMaxCoord;
  bool                 hasMaxCoord;
  occ::handle<TCollection_HAsciiString> theAuthorName;
  occ::handle<TCollection_HAsciiString> theCompanyName;
  int                 theIGESVersion;
  int                 theDraftingStandard;
  occ::handle<TCollection_HAsciiString> theLastChangeDate;
  occ::handle<TCollection_HAsciiString> theAppliProtocol;
};

#endif // _IGESData_GlobalSection_HeaderFile
