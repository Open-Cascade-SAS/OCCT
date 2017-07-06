// Created on: 1991-03-16
// Created by: LPA
// Copyright (c) 1991-1999 Matra Datavision
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


#include <AppDef_MultiPointConstraint.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_Transient.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColgp_HArray1OfVec.hxx>
#include <TColgp_HArray1OfVec2d.hxx>

AppDef_MultiPointConstraint::AppDef_MultiPointConstraint() {}



AppDef_MultiPointConstraint::
       AppDef_MultiPointConstraint(const Standard_Integer NbPoles,
				   const Standard_Integer NbPoles2d):
                                   AppParCurves_MultiPoint(NbPoles, NbPoles2d)
{
}

AppDef_MultiPointConstraint::
       AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP):
                                  AppParCurves_MultiPoint(tabP)
{
}

AppDef_MultiPointConstraint::
       AppDef_MultiPointConstraint(const TColgp_Array1OfPnt2d& tabP2d):
                                  AppParCurves_MultiPoint(tabP2d)
{
}




AppDef_MultiPointConstraint::
       AppDef_MultiPointConstraint(const TColgp_Array1OfPnt& tabP,
				   const TColgp_Array1OfPnt2d& tabP2d):
                                   AppParCurves_MultiPoint(tabP, tabP2d)
{
}


AppDef_MultiPointConstraint::AppDef_MultiPointConstraint 
                             (const TColgp_Array1OfPnt& tabP, 
			      const TColgp_Array1OfPnt2d& tabP2d, 
			      const TColgp_Array1OfVec& tabVec, 
			      const TColgp_Array1OfVec2d& tabVec2d,
			      const TColgp_Array1OfVec& tabCur,
			      const TColgp_Array1OfVec2d& tabCur2d):
                              AppParCurves_MultiPoint(tabP, tabP2d) {

  if ((tabP.Length() != tabVec.Length()) ||
      (tabP2d.Length() != tabVec2d.Length()) ||
      (tabCur.Length() != tabP.Length()) ||
      (tabCur2d.Length() != tabP2d.Length())) {
    throw Standard_ConstructionError();
  }

  tabTang = new TColgp_HArray1OfVec(1, tabVec.Length());
  tabTang2d = new TColgp_HArray1OfVec2d(1, tabVec2d.Length());

  Standard_Integer i, Lower = tabVec.Lower();
  for (i = 1; i <= tabVec.Length(); i++) {
    tabTang->SetValue(i, tabVec.Value(Lower+i-1));
  }
  Lower = tabVec2d.Lower();
  for (i = 1; i <= tabVec2d.Length(); i++) {
    tabTang2d->SetValue(i, tabVec2d.Value(Lower+i-1));
  }

  tabCurv = new TColgp_HArray1OfVec(1, tabCur.Length());
  tabCurv2d = new TColgp_HArray1OfVec2d(1, tabCur2d.Length());

  Lower = tabCur.Lower();
  for (i = 1; i <= tabVec.Length(); i++) {
    tabCurv->SetValue(i, tabCur.Value(Lower+i-1));
  }
  Lower = tabCur2d.Lower();
  for (i = 1; i <= tabCur2d.Length(); i++) {
    tabCurv2d->SetValue(i, tabCur2d.Value(Lower+i-1));
  }

}

AppDef_MultiPointConstraint::AppDef_MultiPointConstraint
			     (const TColgp_Array1OfPnt& tabP, 
			      const TColgp_Array1OfPnt2d& tabP2d, 
			      const TColgp_Array1OfVec& tabVec, 
			      const TColgp_Array1OfVec2d& tabVec2d):
                              AppParCurves_MultiPoint(tabP, tabP2d)
{

  if ((tabP.Length() != tabVec.Length()) ||
      (tabP2d.Length() != tabVec2d.Length())) {
    throw Standard_ConstructionError();
  }

  tabTang = new TColgp_HArray1OfVec(1, tabVec.Length());
  tabTang2d = new TColgp_HArray1OfVec2d(1, tabVec2d.Length());

  Standard_Integer i, Lower = tabVec.Lower();
  for (i = 1; i <= tabVec.Length(); i++) {
    tabTang->SetValue(i, tabVec.Value(Lower+i-1));
  }
  Lower = tabVec2d.Lower();
  for (i = 1; i <= tabVec2d.Length(); i++) {
    tabTang2d->SetValue(i, tabVec2d.Value(Lower+i-1));
  }
}



AppDef_MultiPointConstraint::AppDef_MultiPointConstraint (
                             const TColgp_Array1OfPnt& tabP, 
			     const TColgp_Array1OfVec& tabVec):
                                      AppParCurves_MultiPoint(tabP) {
  
  if (tabP.Length() != tabVec.Length()) {
    throw Standard_ConstructionError();
  }

  tabTang = new TColgp_HArray1OfVec(1, tabVec.Length());

  Standard_Integer i, Lower = tabVec.Lower();
  for (i = 1; i <= tabVec.Length(); i++) {
    tabTang->SetValue(i, tabVec.Value(Lower+i-1));
  }
}


AppDef_MultiPointConstraint::AppDef_MultiPointConstraint 
                             (const TColgp_Array1OfPnt& tabP, 
			      const TColgp_Array1OfVec& tabVec,
			      const TColgp_Array1OfVec& tabCur):
                                      AppParCurves_MultiPoint(tabP) {

  if ((tabP.Length() != tabVec.Length()) ||
      (tabP.Length() != tabCur.Length())) {
    throw Standard_ConstructionError();
  }

  tabTang = new TColgp_HArray1OfVec(1, tabVec.Length());
  Standard_Integer i, Lower = tabVec.Lower();
  for (i = 1; i <= tabVec.Length(); i++) {
    tabTang->SetValue(i, tabVec.Value(Lower+i-1));
  }

  tabCurv = new TColgp_HArray1OfVec(1, tabCur.Length());
  Lower = tabCur.Lower();
  for (i = 1; i <= tabCur.Length(); i++) {
    tabCurv->SetValue(i, tabCur.Value(Lower+i-1));
  }
}



AppDef_MultiPointConstraint::AppDef_MultiPointConstraint 
                             (const TColgp_Array1OfPnt2d& tabP2d, 
			      const TColgp_Array1OfVec2d& tabVec2d):

                                      AppParCurves_MultiPoint(tabP2d) {

  if (tabP2d.Length() != tabVec2d.Length()) {
    throw Standard_ConstructionError();
  }

  tabTang2d = new TColgp_HArray1OfVec2d(1, tabVec2d.Length());
  Standard_Integer i, Lower = tabVec2d.Lower();
  for (i = 1; i <= tabVec2d.Length(); i++) {
    tabTang2d->SetValue(i, tabVec2d.Value(Lower+i-1));
  }

}



AppDef_MultiPointConstraint::AppDef_MultiPointConstraint 
                             (const TColgp_Array1OfPnt2d& tabP2d, 
			      const TColgp_Array1OfVec2d& tabVec2d,
			      const TColgp_Array1OfVec2d& tabCur2d):
                                      AppParCurves_MultiPoint(tabP2d) {

  if ((tabP2d.Length() != tabVec2d.Length()) ||
      (tabCur2d.Length() != tabP2d.Length()))  {
    throw Standard_ConstructionError();
  }
  
  tabTang2d = new TColgp_HArray1OfVec2d(1, tabVec2d.Length());
  Standard_Integer i, Lower = tabVec2d.Lower();
  for (i = 1; i <= tabVec2d.Length(); i++) {
    tabTang2d->SetValue(i, tabVec2d.Value(Lower+i-1));
  }

  tabCurv2d = new TColgp_HArray1OfVec2d(1, tabCur2d.Length());
  Lower = tabCur2d.Lower();
  for (i = 1; i <= tabCur2d.Length(); i++) {
    tabCurv2d->SetValue(i, tabCur2d.Value(Lower+i-1));
  }
}


  
void AppDef_MultiPointConstraint::SetTang (const Standard_Integer Index, 
					   const gp_Vec& Tang) {
  if (tabTang.IsNull())
    tabTang = new TColgp_HArray1OfVec (1, nbP);
  if ((Index <= 0) || (Index > nbP)) {
    throw Standard_OutOfRange();
  }
  tabTang->SetValue(Index, Tang);
}
  

gp_Vec AppDef_MultiPointConstraint::Tang (const Standard_Integer Index) const {
  if ((Index <= 0) || (Index > nbP)) {
    throw Standard_OutOfRange();
  }
  return tabTang->Value(Index);
}
  


void AppDef_MultiPointConstraint::SetTang2d (const Standard_Integer Index, 
				   const gp_Vec2d& Tang2d){
  if (tabTang2d.IsNull()) 
    tabTang2d = new TColgp_HArray1OfVec2d (1, nbP2d);
    
  if ((Index <= nbP) || 
      (Index > nbP+nbP2d)) {
    throw Standard_OutOfRange();
  }
  tabTang2d->SetValue(Index-nbP, Tang2d);
}


gp_Vec2d AppDef_MultiPointConstraint::Tang2d (const Standard_Integer Index) const {
  if ((Index <= nbP) || 
      (Index > nbP+nbP2d)) {
    throw Standard_OutOfRange();
  }
  return tabTang2d->Value(Index-nbP);
}


void AppDef_MultiPointConstraint::SetCurv (const Standard_Integer Index, const gp_Vec& Curv) {
  if (tabCurv.IsNull())
    tabCurv = new TColgp_HArray1OfVec (1, nbP);
  if ((Index <= 0) || (Index > nbP)) {
    throw Standard_OutOfRange();
  }
  tabCurv->SetValue(Index, Curv);
}
  

gp_Vec AppDef_MultiPointConstraint::Curv (const Standard_Integer Index) const {
  if ((Index <= 0) || (Index > nbP)) {
    throw Standard_OutOfRange();
  }
  return tabCurv->Value(Index);
}
  


void AppDef_MultiPointConstraint::SetCurv2d (const Standard_Integer Index, 
					     const gp_Vec2d& Curv2d){
  if (tabCurv2d.IsNull()) 
    tabCurv2d = new TColgp_HArray1OfVec2d (1, nbP2d);
  if ((Index <= nbP) || 
      (Index > nbP+nbP2d)) {
    throw Standard_OutOfRange();
  }
  tabCurv2d->SetValue(Index- nbP, Curv2d);
}



gp_Vec2d AppDef_MultiPointConstraint::Curv2d (const Standard_Integer Index) const {
  if ((Index <= nbP) ||
      (Index > nbP+nbP2d)) {
    throw Standard_OutOfRange();
  }
  return tabCurv2d->Value(Index - nbP);
}


Standard_Boolean AppDef_MultiPointConstraint::IsTangencyPoint() const
{
  return !(tabTang.IsNull() && tabTang2d.IsNull());
}

Standard_Boolean AppDef_MultiPointConstraint::IsCurvaturePoint() const
{
  return !(tabCurv.IsNull() && tabCurv2d.IsNull());
}




void AppDef_MultiPointConstraint::Dump(Standard_OStream& o) const
{
  o << "AppDef_MultiPointConstraint dump:" << endl;
}
