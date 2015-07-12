// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#ifndef _BOPAlgo_WireSplitter_HeaderFile
#define _BOPAlgo_WireSplitter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_PWireEdgeSet.hxx>
#include <BOPTools_ListOfConnexityBlock.hxx>
#include <BOPAlgo_Algo.hxx>
#include <BOPCol_BaseAllocator.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPTools_ConnexityBlock.hxx>
class BOPAlgo_WireEdgeSet;
class TopoDS_Wire;
class TopoDS_Face;



class BOPAlgo_WireSplitter  : public BOPAlgo_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BOPAlgo_WireSplitter();
Standard_EXPORT virtual ~BOPAlgo_WireSplitter();
  
  Standard_EXPORT BOPAlgo_WireSplitter(const BOPCol_BaseAllocator& theAllocator);
  
  Standard_EXPORT void SetWES (const BOPAlgo_WireEdgeSet& theWES);
  
  Standard_EXPORT BOPAlgo_WireEdgeSet& WES();
  
  Standard_EXPORT virtual void Perform() Standard_OVERRIDE;
  
    static void MakeWire (BOPCol_ListOfShape& theLE, TopoDS_Wire& theW);
  
  Standard_EXPORT static void SplitBlock (const TopoDS_Face& theF, BOPTools_ConnexityBlock& theCB);




protected:

  
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;
  
  Standard_EXPORT void MakeConnexityBlocks();
  
  Standard_EXPORT void MakeWires();


  BOPAlgo_PWireEdgeSet myWES;
  BOPTools_ListOfConnexityBlock myLCB;


private:





};


#include <BOPAlgo_WireSplitter.lxx>





#endif // _BOPAlgo_WireSplitter_HeaderFile
