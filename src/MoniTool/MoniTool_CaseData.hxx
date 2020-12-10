// Created on: 1998-04-01
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _MoniTool_CaseData_HeaderFile
#define _MoniTool_CaseData_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>
class Standard_Transient;
class TopoDS_Shape;
class gp_XYZ;
class gp_XY;
class TCollection_AsciiString;
class Message_Msg;


class MoniTool_CaseData;
DEFINE_STANDARD_HANDLE(MoniTool_CaseData, Standard_Transient)

//! This class is intended to record data attached to a case to be
//! exploited.
//! Cases can be :
//! * internal, i.e. for immediate debug
//! for instance, on an abnormal exception, fill a CaseData
//! in a DB (see class DB) then look at its content by XSDRAW
//! * to record abnormal situation, which cause a warning or fail
//! message, for instance during a transfer
//! This will allow, firstly to build a more comprehensive
//! message (with associated data), secondly to help seeing
//! "what happened"
//! * to record data in order to fix a problem
//! If a CASE is well defined and its fix is well known too,
//! recording a CaseData which identifies the CASE will allow
//! to furstherly call the appropriate fix routine
//!
//! A CaseData is defined by
//! * an optional CASE identifier
//! If it is defined, this will allow systematic exploitation
//! such as calling a fix routine
//! * an optional Check Status, Warning or Fail, else it is Info
//! * a NAME : it just allows to identify where this CaseData was
//! created (help to debug)
//! * a LIST OF DATA
//!
//! Each Data has a type (integer, real etc...) and can have a name
//! Hence, each data may be identified by :
//! * its absolute rank (from 1 to NbData)
//! * its name if it has one (exact matching)
//! * else, an interpreted identifier, which gives the type and
//! the rank in the type (for instance, first integer; etc)
//! (See NameRank)
class MoniTool_CaseData : public Standard_Transient
{

public:

  
  //! Creates a CaseData with a CaseId and a Name
  //! (by default not defined)
  Standard_EXPORT MoniTool_CaseData(const Standard_CString caseid = "", const Standard_CString name = "");
  
  //! Sets a CaseId
  Standard_EXPORT void SetCaseId (const Standard_CString caseid);
  
  //! Sets a Name
  Standard_EXPORT void SetName (const Standard_CString name);
  
  //! Returns the CaseId
  Standard_EXPORT Standard_CString CaseId() const;
  
  //! Returns the Name
  Standard_EXPORT Standard_CString Name() const;
  
  //! Tells if <me> is Check (Warning or Fail), else it is Info
  Standard_EXPORT Standard_Boolean IsCheck() const;
  
  //! Tells if <me> is Warning
  Standard_EXPORT Standard_Boolean IsWarning() const;
  
  //! Tells if <me> is Fail
  Standard_EXPORT Standard_Boolean IsFail() const;
  
  //! Resets Check Status, i.e. sets <me> as Info
  Standard_EXPORT void ResetCheck();
  
  //! Sets <me> as Warning
  Standard_EXPORT void SetWarning();
  
  //! Sets <me> as Fail
  Standard_EXPORT void SetFail();
  
  //! Sets the next Add... not to add but to change the data item
  //! designated by its name.
  //! If next Add... is not called with a name, SetChange is ignored
  //! Reset by next Add... , whatever <num> is correct or not
  Standard_EXPORT void SetChange();
  
  //! Sets the next Add... not to add but to replace the data item
  //! <num>, if <num> is between 1 and NbData.
  //! Reset by next Add... , whatever <num> is correct or not
  Standard_EXPORT void SetReplace (const Standard_Integer num);
  
  //! Unitary adding a data; rather internal
  Standard_EXPORT void AddData (const Handle(Standard_Transient)& val, const Standard_Integer kind, const Standard_CString name = "");
  
  //! Adds the currently caught exception
  Standard_EXPORT void AddRaised (const Handle(Standard_Failure)& theException, const Standard_CString name = "");
  
  //! Adds a Shape (recorded as a HShape)
  Standard_EXPORT void AddShape (const TopoDS_Shape& sh, const Standard_CString name = "");
  
  //! Adds a XYZ
  Standard_EXPORT void AddXYZ (const gp_XYZ& aXYZ, const Standard_CString name = "");
  
  //! Adds a XY
  Standard_EXPORT void AddXY (const gp_XY& aXY, const Standard_CString name = "");
  
  //! Adds a Real
  Standard_EXPORT void AddReal (const Standard_Real val, const Standard_CString name = "");
  
  //! Adds two reals (for instance, two parameters)
  Standard_EXPORT void AddReals (const Standard_Real v1, const Standard_Real v2, const Standard_CString name = "");
  
  //! Adds the CPU time between lastCPU and now
  //! if <curCPU> is given, the CPU amount is  curCPU-lastCPU
  //! else it is currently measured CPU - lastCPU
  //! lastCPU has been read by call to GetCPU
  //! See GetCPU to get amount, and LargeCPU to test large amount
  Standard_EXPORT void AddCPU (const Standard_Real lastCPU, const Standard_Real curCPU = 0, const Standard_CString name = "");
  
  //! Returns the current amount of CPU
  //! This allows to laterly test and record CPU amount
  //! Its value has to be given to LargeCPU and AddCPU
  Standard_EXPORT Standard_Real GetCPU() const;
  
  //! Tells if a CPU time amount is large
  //! <maxCPU>  gives the amount over which an amount is large
  //! <lastCPU> gives the start CPU amount
  //! if <curCPU> is given, the tested CPU amount is curCPU-lastCPU
  //! else it is currently measured CPU - lastCPU
  Standard_EXPORT Standard_Boolean LargeCPU (const Standard_Real maxCPU, const Standard_Real lastCPU, const Standard_Real curCPU = 0) const;
  
  //! Adds a Geometric as a Transient (Curve, Surface ...)
  Standard_EXPORT void AddGeom (const Handle(Standard_Transient)& geom, const Standard_CString name = "");
  
  //! Adds a Transient, as an Entity from an InterfaceModel for
  //! instance : it will then be printed with the help of a DBPE
  Standard_EXPORT void AddEntity (const Handle(Standard_Transient)& ent, const Standard_CString name = "");
  
  //! Adds a Text (as HAsciiString)
  Standard_EXPORT void AddText (const Standard_CString text, const Standard_CString name = "");
  
  //! Adds an Integer
  Standard_EXPORT void AddInteger (const Standard_Integer val, const Standard_CString name = "");
  
  //! Adds a Transient, with no more meaning
  Standard_EXPORT void AddAny (const Handle(Standard_Transient)& val, const Standard_CString name = "");
  
  //! Removes a Data from its rank. Does nothing if out of range
  Standard_EXPORT void RemoveData (const Standard_Integer num);
  
  //! Returns the count of data recorded to a set
  Standard_EXPORT Standard_Integer NbData() const;
  
  //! Returns a data item (n0 <nd> in the set <num>)
  Standard_EXPORT Handle(Standard_Transient) Data (const Standard_Integer nd) const;
  
  //! Returns a data item, under control of a Type
  //! If the data item is kind of this type, it is returned in <val>
  //! and the returned value is True
  //! Else, <val> is unchanged and the returned value is False
  Standard_EXPORT Standard_Boolean GetData (const Standard_Integer nd, const Handle(Standard_Type)& type, Handle(Standard_Transient)& val) const;
  
  //! Returns the kind of a data :
  //! KIND TYPE      MEANING
  //! 0  ANY       any (not one of the following)
  //! 1  EX        raised exception
  //! 2  EN        entity
  //! 3  G         geom
  //! 4  SH        shape
  //! 5  XYZ       XYZ
  //! 6  XY or UV  XY
  //! 7  RR        2 reals
  //! 8  R         1 real
  //! 9  CPU       CPU (1 real)
  //! 10 T         text
  //! 11 I         integer
  //!
  //! For NameNum, these codes for TYPE must be given exact
  //! i.e. SH for a Shape, not S nor SHAPE nor SOLID etc
  Standard_EXPORT Standard_Integer Kind (const Standard_Integer nd) const;
  
  //! Returns the name of a data. If it has no name, the string is
  //! empty (length = 0)
  Standard_EXPORT const TCollection_AsciiString& Name (const Standard_Integer nd) const;
  
  //! Returns the first suitable data rank for a given name
  //! Exact matching (exact case, no completion) is required
  //! Firstly checks the recorded names
  //! If not found, considers the name as follows :
  //! Name = "TYPE" : search for the first item with this TYPE
  //! Name = "TYPE:nn" : search for the nn.th item with this TYPE
  //! See allowed values in method Kind
  Standard_EXPORT Standard_Integer NameNum (const Standard_CString name) const;
  
  //! Returns a data as a shape, Null if not a shape
  Standard_EXPORT TopoDS_Shape Shape (const Standard_Integer nd) const;
  
  //! Returns a data as a XYZ (i.e. Geom_CartesianPoint)
  //! Returns False if not the good type
  Standard_EXPORT Standard_Boolean XYZ (const Standard_Integer nd, gp_XYZ& val) const;
  
  //! Returns a data as a XY  (i.e. Geom2d_CartesianPoint)
  //! Returns False if not the good type
  Standard_EXPORT Standard_Boolean XY (const Standard_Integer nd, gp_XY& val) const;
  
  //! Returns a couple of reals  (stored in Geom2d_CartesianPoint)
  Standard_EXPORT Standard_Boolean Reals (const Standard_Integer nd, Standard_Real& v1, Standard_Real& v2) const;
  
  //! Returns a real or CPU amount (stored in Geom2d_CartesianPoint)
  //! (allows an Integer converted to a Real)
  Standard_EXPORT Standard_Boolean Real (const Standard_Integer nd, Standard_Real& val) const;
  
  //! Returns a text (stored in TCollection_HAsciiString)
  Standard_EXPORT Standard_Boolean Text (const Standard_Integer nd, Standard_CString& text) const;
  
  //! Returns an Integer
  Standard_EXPORT Standard_Boolean Integer (const Standard_Integer nd, Standard_Integer& val) const;
  
  //! Returns a Msg from a CaseData : it is build from DefMsg, which
  //! gives the message code plus the designation of items of the
  //! CaseData to be added to the Msg
  //! Empty if no message attached
  //!
  //! Remains to be implemented
  Standard_EXPORT Message_Msg Msg() const;
  
  //! Sets a Code to give a Warning
  Standard_EXPORT static void SetDefWarning (const Standard_CString acode);
  
  //! Sets a Code to give a Fail
  Standard_EXPORT static void SetDefFail (const Standard_CString acode);
  
  //! Returns Check Status for a Code : 0 non/info (default),
  //! 1 warning, 2 fail
  //!
  //! Remark : DefCheck is used to set the check status of a
  //! CaseData when it is attached to a case code, it can be changed
  //! later (by SetFail, SetWarning, ResetCheck)
  Standard_EXPORT static Standard_Integer DefCheck (const Standard_CString acode);
  
  //! Attaches a message definition to a case code
  //! This definition includes the message code plus designation of
  //! items of the CaseData to be added to the message (this part
  //! not yet implemented)
  Standard_EXPORT static void SetDefMsg (const Standard_CString casecode, const Standard_CString mesdef);
  
  //! Returns the message definition for a case code
  //! Empty if no message attached
  Standard_EXPORT static Standard_CString DefMsg (const Standard_CString casecode);




  DEFINE_STANDARD_RTTIEXT(MoniTool_CaseData,Standard_Transient)

protected:




private:


  Standard_Integer thecheck;
  Standard_Integer thesubst;
  TCollection_AsciiString thecase;
  TCollection_AsciiString thename;
  TColStd_SequenceOfTransient thedata;
  TColStd_SequenceOfInteger thekind;
  TColStd_SequenceOfAsciiString thednam;


};







#endif // _MoniTool_CaseData_HeaderFile
