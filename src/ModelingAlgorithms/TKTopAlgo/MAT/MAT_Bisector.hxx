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

class MAT_Bisector;
DEFINE_STANDARD_HANDLE(MAT_Bisector, Standard_Transient)

class MAT_Bisector : public Standard_Transient
{

public:
  Standard_EXPORT MAT_Bisector();

  Standard_EXPORT void AddBisector(const Handle(MAT_Bisector)& abisector) const;

  Standard_EXPORT Handle(MAT_ListOfBisector) List() const;

  Standard_EXPORT Handle(MAT_Bisector) FirstBisector() const;

  Standard_EXPORT Handle(MAT_Bisector) LastBisector() const;

  Standard_EXPORT void BisectorNumber(const Standard_Integer anumber);

  Standard_EXPORT void IndexNumber(const Standard_Integer anumber);

  Standard_EXPORT void FirstEdge(const Handle(MAT_Edge)& anedge);

  Standard_EXPORT void SecondEdge(const Handle(MAT_Edge)& anedge);

  Standard_EXPORT void IssuePoint(const Standard_Integer apoint);

  Standard_EXPORT void EndPoint(const Standard_Integer apoint);

  Standard_EXPORT void DistIssuePoint(const Standard_Real areal);

  Standard_EXPORT void FirstVector(const Standard_Integer avector);

  Standard_EXPORT void SecondVector(const Standard_Integer avector);

  Standard_EXPORT void Sense(const Standard_Real asense);

  Standard_EXPORT void FirstParameter(const Standard_Real aparameter);

  Standard_EXPORT void SecondParameter(const Standard_Real aparameter);

  Standard_EXPORT Standard_Integer BisectorNumber() const;

  Standard_EXPORT Standard_Integer IndexNumber() const;

  Standard_EXPORT Handle(MAT_Edge) FirstEdge() const;

  Standard_EXPORT Handle(MAT_Edge) SecondEdge() const;

  Standard_EXPORT Standard_Integer IssuePoint() const;

  Standard_EXPORT Standard_Integer EndPoint() const;

  Standard_EXPORT Standard_Real DistIssuePoint() const;

  Standard_EXPORT Standard_Integer FirstVector() const;

  Standard_EXPORT Standard_Integer SecondVector() const;

  Standard_EXPORT Standard_Real Sense() const;

  Standard_EXPORT Standard_Real FirstParameter() const;

  Standard_EXPORT Standard_Real SecondParameter() const;

  Standard_EXPORT void Dump(const Standard_Integer ashift, const Standard_Integer alevel) const;

  DEFINE_STANDARD_RTTIEXT(MAT_Bisector, Standard_Transient)

protected:
private:
  Standard_Integer           thebisectornumber;
  Standard_Integer           theindexnumber;
  Handle(MAT_Edge)           thefirstedge;
  Handle(MAT_Edge)           thesecondedge;
  Handle(MAT_ListOfBisector) thelistofbisectors;
  Standard_Integer           theissuepoint;
  Standard_Integer           theendpoint;
  Standard_Integer           thefirstvector;
  Standard_Integer           thesecondvector;
  Standard_Real              thesense;
  Standard_Real              thefirstparameter;
  Standard_Real              thesecondparameter;
  Standard_Real              distissuepoint;
};

#endif // _MAT_Bisector_HeaderFile
