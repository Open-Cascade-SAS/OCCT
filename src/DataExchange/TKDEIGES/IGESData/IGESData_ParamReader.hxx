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

#ifndef _IGESData_ParamReader_HeaderFile
#define _IGESData_ParamReader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_ReadStage.hxx>
#include <Interface_ParamType.hxx>
#include <Standard_CString.hxx>
#include <IGESData_Status.hxx>
#include <IGESData_ParamCursor.hxx>
#include <Standard_Type.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_HAsciiString.hxx>
#include <IGESData_IGESEntity.hxx>

class Interface_ParamList;
class Interface_Check;
class IGESData_IGESEntity;
class IGESData_IGESReaderData;
class Message_Msg;
class gp_XY;
class gp_XYZ;
class TCollection_HAsciiString;
class Interface_EntityList;

//! access to a list of parameters, with management of read stage
//! (owned parameters, properties, associativities) and current
//! parameter number, read errors (which feed a Check), plus
//! convenient facilities to read parameters, in particular :
//! - first parameter is ignored (it repeats entity type), hence
//! number 1 gives 2nd parameter, etc...
//! - lists are not explicit, list-reading methods are provided
//! which manage a current param. number
//! - interpretation is made as possible (texts, reals, entities ...)
//! (in particular, Reading a Real accepts an Integer)
class IGESData_ParamReader
{
public:
  DEFINE_STANDARD_ALLOC

  //! Prepares a ParamReader, stage "Own", current param = 1
  //! It considers a part of the list, from <base> (excluded) for
  //! <nbpar> parameters; <nbpar> = 0 commands to take list length.
  //! Default is (1 to skip type)
  Standard_EXPORT IGESData_ParamReader(const occ::handle<Interface_ParamList>& list,
                                       const occ::handle<Interface_Check>&     ach,
                                       const int                               base  = 1,
                                       const int                               nbpar = 0,
                                       const int                               num   = 0);

  //! Returns the entity number in the file
  Standard_EXPORT int EntityNumber() const;

  //! resets state (stage, current param number, check with no fail)
  Standard_EXPORT void Clear();

  //! returns the current parameter number
  //! This notion is involved by the organisation of an IGES list of
  //! parameters: it can be ended by two lists (Associativities and
  //! Properties), which can be empty, or even absent. Hence, it is
  //! necessary to know, at the end of specific reading, how many
  //! parameters have been read : the optional lists follow
  Standard_EXPORT int CurrentNumber() const;

  //! sets current parameter number to a new value
  //! must be done at end of each step : set on first parameter
  //! following last read one; is done by some Read... methods
  //! (must be done directly if these method are not used)
  //! num greater than NbParams means that following lists are empty
  //! If current num is not managed, it remains at 1, which probably
  //! will cause error when successive steps of reading are made
  Standard_EXPORT void SetCurrentNumber(const int num);

  //! gives current stage (Own-Props-Assocs-End, begins at Own)
  Standard_EXPORT IGESData_ReadStage Stage() const;

  //! passes to next stage (must be linked with setting Current)
  Standard_EXPORT void NextStage();

  //! passes directly to the end of reading process
  Standard_EXPORT void EndAll();

  //! returns number of parameters (minus the first one)
  //! following method skip the first parameter (1 gives the 2nd)
  Standard_EXPORT int NbParams() const;

  //! returns type of parameter; note that "Ident" or "Sub" cannot
  //! be encountered, they correspond to "Integer", see also below
  Standard_EXPORT Interface_ParamType ParamType(const int num) const;

  //! returns literal value of a parameter, as it was in file
  Standard_EXPORT const char* ParamValue(const int num) const;

  //! says if a parameter is defined (not void)
  //! See also DefinedElseSkip
  Standard_EXPORT bool IsParamDefined(const int num) const;

  //! says if a parameter can be regarded as an entity reference
  //! (see Prepare from IGESReaderData for more explanation)
  //! Note that such a parameter can seen as be a plain Integer too
  Standard_EXPORT bool IsParamEntity(const int num) const;

  //! returns entity number corresponding to a parameter if there is
  //! otherwise zero (according criterium IsParamEntity)
  Standard_EXPORT int ParamNumber(const int num) const;

  //! directly returns entity referenced by a parameter
  Standard_EXPORT occ::handle<IGESData_IGESEntity> ParamEntity(
    const occ::handle<IGESData_IGESReaderData>& IR,
    const int                                   num);

  //! Creates a ParamCursor from the Current Number, to read one
  //! parameter, and to advance Current Number after reading
  Standard_EXPORT IGESData_ParamCursor Current() const;

  //! Creates a ParamCursor from the Current Number, to read a list
  //! of "nb" items, and to advance Current Number after reading
  //! By default, each item is made of one parameter
  //! If size is given, it precises the number of params per item
  Standard_EXPORT IGESData_ParamCursor CurrentList(const int nb, const int size = 1) const;

  //! Allows to simply process a parameter which can be defaulted.
  //! Waits on the Current Number a defined parameter or skips it :
  //! If the parameter <num> is defined, changes nothing and returns True
  //! Hence, the next reading with current cursor will concern <num>
  //! If it is void, advances Current Position by one, and returns False
  //! The next reading will concern <num+1> (except if <num> = NbParams)
  //!
  //! This allows to process Default values as follows (C++) :
  //! if (PR.DefinedElseSkip()) {
  //! .. PR.Read... (current parameter);
  //! } else {
  //! <current parameter> = default value
  //! .. nothing else to do with ParamReader
  //! }
  //! For Message
  Standard_EXPORT bool DefinedElseSkip();

  Standard_EXPORT bool ReadInteger(const IGESData_ParamCursor& PC, int& val);

  //! Reads an Integer value designated by PC
  //! The method Current designates the current parameter and
  //! advances the Current Number by one after reading
  //! Note that if a count (not 1) is given, it is ignored
  //! If it is not an Integer, fills Check with a Fail (using mess)
  //! and returns False
  Standard_EXPORT bool ReadInteger(const IGESData_ParamCursor& PC,
                                   const char* const           mess,
                                   int&                        val);

  Standard_EXPORT bool ReadBoolean(const IGESData_ParamCursor& PC,
                                   const Message_Msg&          amsg,
                                   bool&                       val,
                                   const bool                  exact = true);

  //! Reads a Boolean value from parameter "num"
  //! A Boolean is given as an Integer value 0 (False) or 1 (True)
  //! Anyway, an Integer is demanded (else, Check is filled)
  //! If exact is given True, those precise values are demanded
  //! Else, Correction is done, as False for 0 or <0, True for >0
  //! (with a Warning error message, and return is True)
  //! In case of error (not an Integer, or not 0/1 and exact True),
  //! Check is filled with a Fail (using mess) and return is False
  Standard_EXPORT bool ReadBoolean(const IGESData_ParamCursor& PC,
                                   const char* const           mess,
                                   bool&                       val,
                                   const bool                  exact = true);

  Standard_EXPORT bool ReadReal(const IGESData_ParamCursor& PC, double& val);

  //! Reads a Real value from parameter "num"
  //! An Integer is accepted (Check is filled with a Warning
  //! message) and causes return to be True (as normal case)
  //! In other cases, Check is filled with a Fail and return is False
  Standard_EXPORT bool ReadReal(const IGESData_ParamCursor& PC,
                                const char* const           mess,
                                double&                     val);

  Standard_EXPORT bool ReadXY(const IGESData_ParamCursor& PC, Message_Msg& amsg, gp_XY& val);

  //! Reads a couple of Real values (X,Y) from parameter "num"
  //! Integers are accepted (Check is filled with a Warning
  //! message) and cause return to be True (as normal case)
  //! In other cases, Check is filled with a Fail and return is False
  Standard_EXPORT bool ReadXY(const IGESData_ParamCursor& PC, const char* const mess, gp_XY& val);

  Standard_EXPORT bool ReadXYZ(const IGESData_ParamCursor& PC, Message_Msg& amsg, gp_XYZ& val);

  //! Reads a triplet of Real values (X,Y,Z) from parameter "num"
  //! Integers are accepted (Check is filled with a Warning
  //! message) and cause return to be True (as normal case)
  //! In other cases, Check is filled with a Fail and return is False
  //! For Message
  Standard_EXPORT bool ReadXYZ(const IGESData_ParamCursor& PC, const char* const mess, gp_XYZ& val);

  Standard_EXPORT bool ReadText(const IGESData_ParamCursor&            thePC,
                                const Message_Msg&                     theMsg,
                                occ::handle<TCollection_HAsciiString>& theVal);

  //! Reads a Text value from parameter "num", as a String from
  //! Collection, that is, Hollerith text without leading "nnnH"
  //! If it is not a String, fills Check with a Fail (using mess)
  //! and returns False
  Standard_EXPORT bool ReadText(const IGESData_ParamCursor&            PC,
                                const char* const                      mess,
                                occ::handle<TCollection_HAsciiString>& val);

  Standard_EXPORT bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                  const IGESData_ParamCursor&                 PC,
                                  IGESData_Status&                            aStatus,
                                  occ::handle<IGESData_IGESEntity>&           val,
                                  const bool                                  canbenul = false);

  //! Reads an IGES entity from parameter "num"
  //! An Entity is known by its reference, which has the form of an
  //! odd Integer Value (a number in the Directory)
  //! If <canbenul> is given True, a Reference can also be Null :
  //! in this case, the result is a Null Handle with no Error
  //! If <canbenul> is False, a Null Reference causes an Error
  //! If the parameter cannot refer to an entity (or null), fills
  //! Check with a Fail (using mess) and returns False
  Standard_EXPORT bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                  const IGESData_ParamCursor&                 PC,
                                  const char* const                           mess,
                                  occ::handle<IGESData_IGESEntity>&           val,
                                  const bool                                  canbenul = false);

  Standard_EXPORT bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                  const IGESData_ParamCursor&                 PC,
                                  IGESData_Status&                            aStatus,
                                  const occ::handle<Standard_Type>&           type,
                                  occ::handle<IGESData_IGESEntity>&           val,
                                  const bool                                  canbenul = false);

  //! Safe variant for arbitrary type of argument
  template <class T>
  bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                  const IGESData_ParamCursor&                 PC,
                  IGESData_Status&                            aStatus,
                  const occ::handle<Standard_Type>&           type,
                  occ::handle<T>&                             val,
                  const bool                                  canbenul = false)
  {
    occ::handle<IGESData_IGESEntity> aVal = val;
    bool                             aRes = ReadEntity(IR, PC, aStatus, type, aVal, canbenul);
    val                                   = occ::down_cast<T>(aVal);
    return aRes && (canbenul || !val.IsNull());
  }

  //! Works as ReadEntity without Type, but in addition checks the
  //! Type of the Entity, which must be "kind of" a given <type>
  //! Then, gives the same fail cases as ReadEntity without Type,
  //! plus the case "Incorrect Type"
  //! (in such a case, returns False and givel <val> = Null)
  Standard_EXPORT bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                                  const IGESData_ParamCursor&                 PC,
                                  const char* const                           mess,
                                  const occ::handle<Standard_Type>&           type,
                                  occ::handle<IGESData_IGESEntity>&           val,
                                  const bool                                  canbenul = false);

  //! Safe variant for arbitrary type of argument
  template <class T>
  bool ReadEntity(const occ::handle<IGESData_IGESReaderData>& IR,
                  const IGESData_ParamCursor&                 PC,
                  const char* const                           mess,
                  const occ::handle<Standard_Type>&           type,
                  occ::handle<T>&                             val,
                  const bool                                  canbenul = false)
  {
    occ::handle<IGESData_IGESEntity> aVal = val;
    bool                             aRes = ReadEntity(IR, PC, mess, type, aVal, canbenul);
    val                                   = occ::down_cast<T>(aVal);
    return aRes && (canbenul || !val.IsNull());
  }

  Standard_EXPORT bool ReadInts(const IGESData_ParamCursor&            PC,
                                const Message_Msg&                     amsg,
                                occ::handle<NCollection_HArray1<int>>& val,
                                const int                              index = 1);

  //! Reads a list of Integer values, defined by PC (with a count of
  //! parameters). PC can start from Current Number and command it
  //! to advance after reading (use method CurrentList to do this)
  //! The list is given as a HArray1, numered from "index"
  //! If all params are not Integer, Check is filled (using mess)
  //! and return value is False
  Standard_EXPORT bool ReadInts(const IGESData_ParamCursor&            PC,
                                const char* const                      mess,
                                occ::handle<NCollection_HArray1<int>>& val,
                                const int                              index = 1);

  Standard_EXPORT bool ReadReals(const IGESData_ParamCursor&               PC,
                                 Message_Msg&                              amsg,
                                 occ::handle<NCollection_HArray1<double>>& val,
                                 const int                                 index = 1);

  //! Reads a list of Real values defined by PC
  //! Same conditions as for ReadInts, for PC and index
  //! An Integer parameter is accepted, if at least one parameter is
  //! Integer, Check is filled with a "Warning" message
  //! If all params are neither Real nor Integer, Check is filled
  //! (using mess) and return value is False
  Standard_EXPORT bool ReadReals(const IGESData_ParamCursor&               PC,
                                 const char* const                         mess,
                                 occ::handle<NCollection_HArray1<double>>& val,
                                 const int                                 index = 1);

  Standard_EXPORT bool ReadTexts(
    const IGESData_ParamCursor&                                              PC,
    const Message_Msg&                                                       amsg,
    occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& val,
    const int                                                                index = 1);

  //! Reads a list of Hollerith Texts, defined by PC
  //! Texts are read as Hollerith texts without leading "nnnH"
  //! Same conditions as for ReadInts, for PC and index
  //! If all params are not Text, Check is filled (using mess)
  //! and return value is False
  Standard_EXPORT bool ReadTexts(
    const IGESData_ParamCursor&                                              PC,
    const char* const                                                        mess,
    occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>>& val,
    const int                                                                index = 1);

  Standard_EXPORT bool ReadEnts(
    const occ::handle<IGESData_IGESReaderData>&                         IR,
    const IGESData_ParamCursor&                                         PC,
    const Message_Msg&                                                  amsg,
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& val,
    const int                                                           index = 1);

  //! Reads a list of Entities defined by PC
  //! Same conditions as for ReadInts, for PC and index
  //! The list is given as a HArray1, numered from "index"
  //! If all params cannot be read as Entities, Check is filled
  //! (using mess) and return value is False
  //! Remark : Null references are accepted, they are ignored
  //! (negative pointers too : they provoke a Warning message)
  //! If the caller wants to check them, a loop on ReadEntity should
  //! be used
  Standard_EXPORT bool ReadEnts(
    const occ::handle<IGESData_IGESReaderData>&                         IR,
    const IGESData_ParamCursor&                                         PC,
    const char* const                                                   mess,
    occ::handle<NCollection_HArray1<occ::handle<IGESData_IGESEntity>>>& val,
    const int                                                           index = 1);

  Standard_EXPORT bool ReadEntList(const occ::handle<IGESData_IGESReaderData>& IR,
                                   const IGESData_ParamCursor&                 PC,
                                   Message_Msg&                                amsg,
                                   Interface_EntityList&                       val,
                                   const bool                                  ord = true);

  //! Reads a list of Entities defined by PC
  //! Same conditions as for ReadEnts, for PC
  //! The list is given as an EntityList
  //! (index has no meaning; the EntityList starts from clear)
  //! If "ord" is given True (default), entities will be added to
  //! the list in their original order
  //! Remark : Negative or Null Pointers are ignored
  //! Else ("ord" False), order is not guaranteed (faster mode)
  //! If all params cannot be read as Entities, same as above
  //! Warning: Give "ord" to False ONLY if order is not significant
  Standard_EXPORT bool ReadEntList(const occ::handle<IGESData_IGESReaderData>& IR,
                                   const IGESData_ParamCursor&                 PC,
                                   const char* const                           mess,
                                   Interface_EntityList&                       val,
                                   const bool                                  ord = true);

  Standard_EXPORT bool ReadingReal(const int num, double& val);

  //! Routine which reads a Real parameter, given its number
  //! Same conditions as ReadReal for mess, val, and return value
  Standard_EXPORT bool ReadingReal(const int num, const char* const mess, double& val);

  Standard_EXPORT bool ReadingEntityNumber(const int num, int& val);

  //! Routine which reads an Entity Number (which allows to read the
  //! Entity in the IGESReaderData by BoundEntity), given its number
  //! in the list of Parameters
  //! Same conditions as ReadEntity for mess, val, and return value
  //! In particular, returns True and val to zero means Null Entity,
  //! and val not zero means Entity read by BoundEntity
  Standard_EXPORT bool ReadingEntityNumber(const int num, const char* const mess, int& val);

  Standard_EXPORT void SendFail(const Message_Msg& amsg);

  Standard_EXPORT void SendWarning(const Message_Msg& amsg);

  Standard_EXPORT void AddFail(const char* const afail, const char* const bfail = "");

  //! feeds the Check with a new fail (as a String or as a CString)
  Standard_EXPORT void AddFail(const occ::handle<TCollection_HAsciiString>& af,
                               const occ::handle<TCollection_HAsciiString>& bf);

  Standard_EXPORT void AddWarning(const char* const awarn, const char* const bwarn = "");

  //! feeds the Check with a new Warning message
  Standard_EXPORT void AddWarning(const occ::handle<TCollection_HAsciiString>& aw,
                                  const occ::handle<TCollection_HAsciiString>& bw);

  Standard_EXPORT void Mend(const char* const pref = "");

  //! says if fails have been recorded into the Check
  Standard_EXPORT bool HasFailed() const;

  //! returns the Check
  //! Note that any error signaled above is also recorded into it
  Standard_EXPORT const occ::handle<Interface_Check>& Check() const;

  //! returns the check in a way which allows to work on it directly
  //! (i.e. messages added to the Check are added to ParamReader too)
  Standard_EXPORT occ::handle<Interface_Check>& CCheck();

  //! Returns True if the Check is Empty
  //! Else, it has to be recorded with the Read Entity
  Standard_EXPORT bool IsCheckEmpty() const;

private:
  Standard_EXPORT bool PrepareRead(const IGESData_ParamCursor& PC,
                                   const bool                  several,
                                   const int                   size = 1);

  //! Prepares work for Read... methods which call it to begin
  //! The required count of parameters must not overpass NbParams.
  //! If several is given False, PC count must be one.
  //! If size is given, the TermSize from ParmCursor must be a
  //! multiple count of this size.
  //! If one of above condition is not satisfied, a Fail Message is
  //! recorded into Check, using the root "mess" and return is False
  Standard_EXPORT bool PrepareRead(const IGESData_ParamCursor& PC,
                                   const char* const           mess,
                                   const bool                  several,
                                   const int                   size = 1);

  //! Gets the first parameter number to be read, determined from
  //! ParamCursor data read by PrepareRead (Start + Offset)
  //! Then commands to skip 1 parameter (default) or nb if given
  Standard_EXPORT int FirstRead(const int nb = 1);

  //! Gets the next parameter number to be read. Skips to next Item
  //! if TermSize has been read.
  //! Then commands to skip 1 parameter (default) or nb if given
  Standard_EXPORT int NextRead(const int nb = 1);

  //! internal method which builds a Fail message from an
  //! identification "idm" and a diagnostic ("afail")
  //! Also feeds LastReadStatus
  //! <af> for final message, bf (can be different) for original
  Standard_EXPORT void AddFail(const char* const                            idm,
                               const occ::handle<TCollection_HAsciiString>& af,
                               const occ::handle<TCollection_HAsciiString>& bf);

  //! Same as above but with CString
  //! <bf> empty means = <af>
  Standard_EXPORT void AddFail(const char* const idm,
                               const char* const afail,
                               const char* const bfail);

  //! internal method which builds a Warning message from an
  //! identification "idm" and a diagnostic
  //! <aw> is final message, bw is original (can be different)
  //! Also feeds LastReadStatus
  Standard_EXPORT void AddWarning(const char* const                            idm,
                                  const occ::handle<TCollection_HAsciiString>& aw,
                                  const occ::handle<TCollection_HAsciiString>& bw);

  //! Same as above but with CString
  //! <bw> empty means = <aw>
  Standard_EXPORT void AddWarning(const char* const idm,
                                  const char* const aw,
                                  const char* const bw);

  occ::handle<Interface_ParamList> theparams;
  occ::handle<Interface_Check>     thecheck;
  int                              thebase;
  int                              thenbpar;
  int                              thecurr;
  IGESData_ReadStage               thestage;
  bool                             thelast;
  int                              theindex;
  int                              thenbitem;
  int                              theitemsz;
  int                              theoffset;
  int                              thetermsz;
  int                              themaxind;
  int                              thenbterm;
  int                              pbrealint;
  int                              pbrealform;
  int                              thenum;
};

#endif // _IGESData_ParamReader_HeaderFile
