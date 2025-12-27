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

#include <MAT_Bisector.hxx>
#include <MAT_Edge.hxx>
#include <MAT_ListOfBisector.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MAT_Bisector, Standard_Transient)

MAT_Bisector::MAT_Bisector()
{
  thebisectornumber  = -1;
  thefirstparameter  = Precision::Infinite();
  thesecondparameter = Precision::Infinite();
  thelistofbisectors = new MAT_ListOfBisector();
}

void MAT_Bisector::AddBisector(const occ::handle<MAT_Bisector>& abisector) const
{
  thelistofbisectors->BackAdd(abisector);
}

occ::handle<MAT_ListOfBisector> MAT_Bisector::List() const
{
  return thelistofbisectors;
}

occ::handle<MAT_Bisector> MAT_Bisector::FirstBisector() const
{
  return thelistofbisectors->FirstItem();
}

occ::handle<MAT_Bisector> MAT_Bisector::LastBisector() const
{
  return thelistofbisectors->LastItem();
}

void MAT_Bisector::BisectorNumber(const int anumber)
{
  thebisectornumber = anumber;
}

void MAT_Bisector::IndexNumber(const int anumber)
{
  theindexnumber = anumber;
}

void MAT_Bisector::FirstEdge(const occ::handle<MAT_Edge>& anedge)
{
  thefirstedge = anedge;
}

void MAT_Bisector::SecondEdge(const occ::handle<MAT_Edge>& anedge)
{
  thesecondedge = anedge;
}

void MAT_Bisector::IssuePoint(const int apoint)
{
  theissuepoint = apoint;
}

void MAT_Bisector::EndPoint(const int apoint)
{
  theendpoint = apoint;
}

void MAT_Bisector::DistIssuePoint(const double areal)
{
  distissuepoint = areal;
}

void MAT_Bisector::FirstVector(const int avector)
{
  thefirstvector = avector;
}

void MAT_Bisector::SecondVector(const int avector)
{
  thesecondvector = avector;
}

void MAT_Bisector::Sense(const double asense)
{
  thesense = asense;
}

void MAT_Bisector::FirstParameter(const double aparameter)
{
  thefirstparameter = aparameter;
}

void MAT_Bisector::SecondParameter(const double aparameter)
{
  thesecondparameter = aparameter;
}

int MAT_Bisector::BisectorNumber() const
{
  return thebisectornumber;
}

int MAT_Bisector::IndexNumber() const
{
  return theindexnumber;
}

occ::handle<MAT_Edge> MAT_Bisector::FirstEdge() const
{
  return thefirstedge;
}

occ::handle<MAT_Edge> MAT_Bisector::SecondEdge() const
{
  return thesecondedge;
}

int MAT_Bisector::IssuePoint() const
{
  return theissuepoint;
}

int MAT_Bisector::EndPoint() const
{
  return theendpoint;
}

double MAT_Bisector::DistIssuePoint() const
{
  return distissuepoint;
}

int MAT_Bisector::FirstVector() const
{
  return thefirstvector;
}

int MAT_Bisector::SecondVector() const
{
  return thesecondvector;
}

double MAT_Bisector::Sense() const
{
  return thesense;
}

double MAT_Bisector::FirstParameter() const
{
  return thefirstparameter;
}

double MAT_Bisector::SecondParameter() const
{
  return thesecondparameter;
}

void MAT_Bisector::Dump(const int ashift, const int alevel) const
{
  int i;

  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << " BISECTOR : " << thebisectornumber << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "   First edge     : " << thefirstedge->EdgeNumber() << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  std::cout << "   Second edge    : " << thesecondedge->EdgeNumber() << std::endl;
  for (i = 0; i < ashift; i++)
    std::cout << "  ";
  if (alevel)
  {
    if (!thelistofbisectors->More())
    {
      std::cout << "   Bisectors List : " << std::endl;
      thelistofbisectors->Dump(ashift + 1, 1);
    }
  }
  std::cout << std::endl;
}
