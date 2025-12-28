// Created on: 1992-09-30
// Created by: Gilles DEBARBOUILLE
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

#ifndef _MAT_Bisector_HeaderFile
#define _MAT_Bisector_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class MAT_Edge;
class MAT_ListOfBisector;

class MAT_Bisector : public Standard_Transient
{

public:
  Standard_EXPORT MAT_Bisector();

  Standard_EXPORT void AddBisector(const occ::handle<MAT_Bisector>& abisector) const;

  Standard_EXPORT occ::handle<MAT_ListOfBisector> List() const;

  Standard_EXPORT occ::handle<MAT_Bisector> FirstBisector() const;

  Standard_EXPORT occ::handle<MAT_Bisector> LastBisector() const;

  Standard_EXPORT void BisectorNumber(const int anumber);

  Standard_EXPORT void IndexNumber(const int anumber);

  Standard_EXPORT void FirstEdge(const occ::handle<MAT_Edge>& anedge);

  Standard_EXPORT void SecondEdge(const occ::handle<MAT_Edge>& anedge);

  Standard_EXPORT void IssuePoint(const int apoint);

  Standard_EXPORT void EndPoint(const int apoint);

  Standard_EXPORT void DistIssuePoint(const double areal);

  Standard_EXPORT void FirstVector(const int avector);

  Standard_EXPORT void SecondVector(const int avector);

  Standard_EXPORT void Sense(const double asense);

  Standard_EXPORT void FirstParameter(const double aparameter);

  Standard_EXPORT void SecondParameter(const double aparameter);

  Standard_EXPORT int BisectorNumber() const;

  Standard_EXPORT int IndexNumber() const;

  Standard_EXPORT occ::handle<MAT_Edge> FirstEdge() const;

  Standard_EXPORT occ::handle<MAT_Edge> SecondEdge() const;

  Standard_EXPORT int IssuePoint() const;

  Standard_EXPORT int EndPoint() const;

  Standard_EXPORT double DistIssuePoint() const;

  Standard_EXPORT int FirstVector() const;

  Standard_EXPORT int SecondVector() const;

  Standard_EXPORT double Sense() const;

  Standard_EXPORT double FirstParameter() const;

  Standard_EXPORT double SecondParameter() const;

  Standard_EXPORT void Dump(const int ashift, const int alevel) const;

  DEFINE_STANDARD_RTTIEXT(MAT_Bisector, Standard_Transient)

private:
  int                             thebisectornumber;
  int                             theindexnumber;
  occ::handle<MAT_Edge>           thefirstedge;
  occ::handle<MAT_Edge>           thesecondedge;
  occ::handle<MAT_ListOfBisector> thelistofbisectors;
  int                             theissuepoint;
  int                             theendpoint;
  int                             thefirstvector;
  int                             thesecondvector;
  double                          thesense;
  double                          thefirstparameter;
  double                          thesecondparameter;
  double                          distissuepoint;
};

#endif // _MAT_Bisector_HeaderFile
