// Created on: 1993-06-23
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Interference_HeaderFile
#define _TopOpeBRepDS_Interference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopOpeBRepDS_Transition.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Kind.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TopOpeBRepDS_Transition;
class TCollection_AsciiString;


class TopOpeBRepDS_Interference;
DEFINE_STANDARD_HANDLE(TopOpeBRepDS_Interference, Standard_Transient)

//! An interference    is   the   description  of  the
//! attachment of  a new  geometry on a  geometry. For
//! example an intersection point  on an Edge or on  a
//! Curve.
//!
//! The Interference contains the following data :
//!
//! - Transition :  How the interference  separates the
//! existing geometry in INSIDE and OUTSIDE.
//!
//! - SupportType : Type of  the object supporting the
//! interference. (FACE, EDGE, VERTEX, SURFACE, CURVE).
//!
//! - Support :  Index  in the data  structure  of the
//! object supporting the interference.
//!
//! - GeometryType  :   Type  of the  geometry of  the
//! interference (SURFACE, CURVE, POINT).
//!
//! - Geometry : Index  in the data structure  of the
//! geometry.
class TopOpeBRepDS_Interference : public Standard_Transient
{

public:

  
  Standard_EXPORT TopOpeBRepDS_Interference();
  
  Standard_EXPORT TopOpeBRepDS_Interference(const TopOpeBRepDS_Transition& Transition, const TopOpeBRepDS_Kind SupportType, const Standard_Integer Support, const TopOpeBRepDS_Kind GeometryType, const Standard_Integer Geometry);
  
  Standard_EXPORT TopOpeBRepDS_Interference(const Handle(TopOpeBRepDS_Interference)& I);
  
  Standard_EXPORT const TopOpeBRepDS_Transition& Transition() const;
  
  Standard_EXPORT TopOpeBRepDS_Transition& ChangeTransition();
  
  Standard_EXPORT void Transition (const TopOpeBRepDS_Transition& T);
  
  //! return GeometryType + Geometry + SupportType + Support
  Standard_EXPORT void GKGSKS (TopOpeBRepDS_Kind& GK, Standard_Integer& G, TopOpeBRepDS_Kind& SK, Standard_Integer& S) const;
  
  Standard_EXPORT TopOpeBRepDS_Kind SupportType() const;
  
  Standard_EXPORT Standard_Integer Support() const;
  
  Standard_EXPORT TopOpeBRepDS_Kind GeometryType() const;
  
  Standard_EXPORT Standard_Integer Geometry() const;
  
  Standard_EXPORT void SetGeometry (const Standard_Integer GI);
  
  Standard_EXPORT void SupportType (const TopOpeBRepDS_Kind ST);
  
  Standard_EXPORT void Support (const Standard_Integer S);
  
  Standard_EXPORT void GeometryType (const TopOpeBRepDS_Kind GT);
  
  Standard_EXPORT void Geometry (const Standard_Integer G);
  
  Standard_EXPORT Standard_Boolean HasSameSupport (const Handle(TopOpeBRepDS_Interference)& Other) const;
  
  Standard_EXPORT Standard_Boolean HasSameGeometry (const Handle(TopOpeBRepDS_Interference)& Other) const;





  DEFINE_STANDARD_RTTIEXT(TopOpeBRepDS_Interference,Standard_Transient)

protected:




private:


  TopOpeBRepDS_Transition myTransition;
  Standard_Integer mySupport;
  Standard_Integer myGeometry;
  TopOpeBRepDS_Kind mySupportType;
  TopOpeBRepDS_Kind myGeometryType;


};







#endif // _TopOpeBRepDS_Interference_HeaderFile
