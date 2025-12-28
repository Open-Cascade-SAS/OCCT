// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_PatternStd_HeaderFile
#define _TDataXtd_PatternStd_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <TDataXtd_Pattern.hxx>
#include <gp_Trsf.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_OStream.hxx>
class TNaming_NamedShape;
class TDataStd_Real;
class TDataStd_Integer;
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;

//! to create a PatternStd
//! (LinearPattern, CircularPattern, RectangularPattern,
//! RadialCircularPattern, MirrorPattern)
class TDataXtd_PatternStd : public TDataXtd_Pattern
{

public:
  Standard_EXPORT static const Standard_GUID& GetPatternID();

  //! Find, or create, a PatternStd attribute
  Standard_EXPORT static occ::handle<TDataXtd_PatternStd> Set(const TDF_Label& label);

  Standard_EXPORT TDataXtd_PatternStd();

  Standard_EXPORT void Signature(const int signature);

  Standard_EXPORT void Axis1(const occ::handle<TNaming_NamedShape>& Axis1);

  Standard_EXPORT void Axis2(const occ::handle<TNaming_NamedShape>& Axis2);

  Standard_EXPORT void Axis1Reversed(const bool Axis1Reversed);

  Standard_EXPORT void Axis2Reversed(const bool Axis2Reversed);

  Standard_EXPORT void Value1(const occ::handle<TDataStd_Real>& value);

  Standard_EXPORT void Value2(const occ::handle<TDataStd_Real>& value);

  Standard_EXPORT void NbInstances1(const occ::handle<TDataStd_Integer>& NbInstances1);

  Standard_EXPORT void NbInstances2(const occ::handle<TDataStd_Integer>& NbInstances2);

  Standard_EXPORT void Mirror(const occ::handle<TNaming_NamedShape>& plane);

  int Signature() const;

  occ::handle<TNaming_NamedShape> Axis1() const;

  occ::handle<TNaming_NamedShape> Axis2() const;

  bool Axis1Reversed() const;

  bool Axis2Reversed() const;

  occ::handle<TDataStd_Real> Value1() const;

  occ::handle<TDataStd_Real> Value2() const;

  occ::handle<TDataStd_Integer> NbInstances1() const;

  occ::handle<TDataStd_Integer> NbInstances2() const;

  occ::handle<TNaming_NamedShape> Mirror() const;

  Standard_EXPORT int NbTrsfs() const override;

  Standard_EXPORT void ComputeTrsfs(NCollection_Array1<gp_Trsf>& Trsfs) const override;

  Standard_EXPORT const Standard_GUID& PatternID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT virtual void References(const occ::handle<TDF_DataSet>& aDataSet) const override;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const override;

  DEFINE_STANDARD_RTTIEXT(TDataXtd_PatternStd, TDataXtd_Pattern)

private:
  int                             mySignature;
  bool                            myAxis1Reversed;
  bool                            myAxis2Reversed;
  occ::handle<TNaming_NamedShape> myAxis1;
  occ::handle<TNaming_NamedShape> myAxis2;
  occ::handle<TDataStd_Real>      myValue1;
  occ::handle<TDataStd_Real>      myValue2;
  occ::handle<TDataStd_Integer>   myNb1;
  occ::handle<TDataStd_Integer>   myNb2;
  occ::handle<TNaming_NamedShape> myMirror;
};

#include <TDataXtd_PatternStd.lxx>

#endif // _TDataXtd_PatternStd_HeaderFile
