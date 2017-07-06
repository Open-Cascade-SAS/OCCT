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

IMPLEMENT_STANDARD_RTTIEXT(MAT_Bisector,Standard_Transient)

MAT_Bisector::MAT_Bisector()
{
  thebisectornumber = -1;
  thefirstparameter  = Precision::Infinite();
  thesecondparameter = Precision::Infinite();
  thelistofbisectors = new MAT_ListOfBisector();
}

void MAT_Bisector::AddBisector(const Handle(MAT_Bisector)& abisector) const
{
  thelistofbisectors->BackAdd(abisector);
}

Handle(MAT_ListOfBisector) MAT_Bisector::List() const
{
  return thelistofbisectors;
}

Handle(MAT_Bisector) MAT_Bisector::FirstBisector() const
{
  return thelistofbisectors->FirstItem();
}

Handle(MAT_Bisector) MAT_Bisector::LastBisector() const
{
  return thelistofbisectors->LastItem();
}

void MAT_Bisector::BisectorNumber(const Standard_Integer anumber)
{
  thebisectornumber = anumber;
}
    
void MAT_Bisector::IndexNumber(const Standard_Integer anumber)
{
  theindexnumber = anumber;
}
    
void MAT_Bisector::FirstEdge(const Handle(MAT_Edge)& anedge)
{
  thefirstedge = anedge;
}
    
void MAT_Bisector::SecondEdge(const Handle(MAT_Edge)& anedge)
{
  thesecondedge = anedge;
}
    
void MAT_Bisector::IssuePoint(const Standard_Integer apoint)
{
  theissuepoint = apoint;
}
    
void MAT_Bisector::EndPoint(const Standard_Integer apoint)
{
  theendpoint = apoint;
}

void MAT_Bisector::DistIssuePoint(const Standard_Real areal)
{
  distissuepoint = areal;
}
    
void MAT_Bisector::FirstVector(const Standard_Integer avector)
{
  thefirstvector = avector;
}
    
void MAT_Bisector::SecondVector(const Standard_Integer avector)
{
  thesecondvector = avector;
}
    
void MAT_Bisector::Sense(const Standard_Real asense)
{
  thesense = asense;
}
    
void MAT_Bisector::FirstParameter(const Standard_Real aparameter)
{
  thefirstparameter = aparameter;
}
    
void MAT_Bisector::SecondParameter(const Standard_Real aparameter)
{
  thesecondparameter = aparameter;
}
    
Standard_Integer MAT_Bisector::BisectorNumber() const
{
  return thebisectornumber;
}
    
Standard_Integer MAT_Bisector::IndexNumber() const
{
  return theindexnumber;
}
    
Handle(MAT_Edge) MAT_Bisector::FirstEdge() const
{
  return thefirstedge;
}
    
Handle(MAT_Edge) MAT_Bisector::SecondEdge() const
{
  return thesecondedge;
}
    
Standard_Integer MAT_Bisector::IssuePoint() const
{
  return theissuepoint;
}
    
Standard_Integer MAT_Bisector::EndPoint() const
{
  return theendpoint;
}

Standard_Real MAT_Bisector::DistIssuePoint() const
{
  return distissuepoint;
}    
Standard_Integer MAT_Bisector::FirstVector() const
{
  return thefirstvector;
}
    
Standard_Integer MAT_Bisector::SecondVector() const
{
  return thesecondvector;
}

Standard_Real MAT_Bisector::Sense() const
{
  return thesense;
}
    
Standard_Real MAT_Bisector::FirstParameter() const
{
  return thefirstparameter;
}
    
Standard_Real MAT_Bisector::SecondParameter() const
{
  return thesecondparameter;
}

void MAT_Bisector::Dump(const Standard_Integer ashift,
			 const Standard_Integer alevel) const
{
  Standard_Integer i;

  for(i=0; i<ashift; i++)cout<<"  ";
  cout<<" BISECTOR : "<<thebisectornumber<<endl;
  for(i=0; i<ashift; i++)cout<<"  ";
  cout<<"   First edge     : "<<thefirstedge->EdgeNumber()<<endl;
  for(i=0; i<ashift; i++)cout<<"  ";
  cout<<"   Second edge    : "<<thesecondedge->EdgeNumber()<<endl;
  for(i=0; i<ashift; i++)cout<<"  ";
  if(alevel)
    {
      if(!thelistofbisectors->More())
	{
	  cout<<"   Bisectors List : "<<endl;
	  thelistofbisectors->Dump(ashift+1,1);
	}
    }
  cout<<endl;
}
