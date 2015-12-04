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
#include <Standard_Boolean.hxx>
#include <TDataXtd_Pattern.hxx>
#include <TDataXtd_Array1OfTrsf.hxx>
#include <Standard_OStream.hxx>
class TNaming_NamedShape;
class TDataStd_Real;
class TDataStd_Integer;
class Standard_GUID;
class TDF_Label;
class TDF_Attribute;
class TDF_RelocationTable;
class TDF_DataSet;


class TDataXtd_PatternStd;
DEFINE_STANDARD_HANDLE(TDataXtd_PatternStd, TDataXtd_Pattern)

//! to create a PatternStd
//! (LinearPattern, CircularPattern, RectangularPattern,
//! RadialCircularPattern, MirrorPattern)
class TDataXtd_PatternStd : public TDataXtd_Pattern
{

public:

  
  Standard_EXPORT static const Standard_GUID& GetPatternID();
  
  //! Find, or  create,  a PatternStd  attribute
  Standard_EXPORT static Handle(TDataXtd_PatternStd) Set (const TDF_Label& label);
  
  Standard_EXPORT TDataXtd_PatternStd();
  
  Standard_EXPORT void Signature (const Standard_Integer signature);
  
  Standard_EXPORT void Axis1 (const Handle(TNaming_NamedShape)& Axis1);
  
  Standard_EXPORT void Axis2 (const Handle(TNaming_NamedShape)& Axis2);
  
  Standard_EXPORT void Axis1Reversed (const Standard_Boolean Axis1Reversed);
  
  Standard_EXPORT void Axis2Reversed (const Standard_Boolean Axis2Reversed);
  
  Standard_EXPORT void Value1 (const Handle(TDataStd_Real)& value);
  
  Standard_EXPORT void Value2 (const Handle(TDataStd_Real)& value);
  
  Standard_EXPORT void NbInstances1 (const Handle(TDataStd_Integer)& NbInstances1);
  
  Standard_EXPORT void NbInstances2 (const Handle(TDataStd_Integer)& NbInstances2);
  
  Standard_EXPORT void Mirror (const Handle(TNaming_NamedShape)& plane);
  
    Standard_Integer Signature() const;
  
    Handle(TNaming_NamedShape) Axis1() const;
  
    Handle(TNaming_NamedShape) Axis2() const;
  
    Standard_Boolean Axis1Reversed() const;
  
    Standard_Boolean Axis2Reversed() const;
  
    Handle(TDataStd_Real) Value1() const;
  
    Handle(TDataStd_Real) Value2() const;
  
    Handle(TDataStd_Integer) NbInstances1() const;
  
    Handle(TDataStd_Integer) NbInstances2() const;
  
    Handle(TNaming_NamedShape) Mirror() const;
  
  Standard_EXPORT Standard_Integer NbTrsfs() const Standard_OVERRIDE;
  
  Standard_EXPORT void ComputeTrsfs (TDataXtd_Array1OfTrsf& Trsfs) const Standard_OVERRIDE;
  
  Standard_EXPORT const Standard_GUID& PatternID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& With) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& Into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void References (const Handle(TDF_DataSet)& aDataSet) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataXtd_PatternStd,TDataXtd_Pattern)

protected:




private:


  Standard_Integer mySignature;
  Standard_Boolean myAxis1Reversed;
  Standard_Boolean myAxis2Reversed;
  Handle(TNaming_NamedShape) myAxis1;
  Handle(TNaming_NamedShape) myAxis2;
  Handle(TDataStd_Real) myValue1;
  Handle(TDataStd_Real) myValue2;
  Handle(TDataStd_Integer) myNb1;
  Handle(TDataStd_Integer) myNb2;
  Handle(TNaming_NamedShape) myMirror;


};


#include <TDataXtd_PatternStd.lxx>





#endif // _TDataXtd_PatternStd_HeaderFile
