// Created on: 1995-07-11
// Created by: Christophe MARION
// Copyright (c) 1995-1999 Matra Datavision
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


#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_PolyInternalData.hxx>
#include <Standard_Stream.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyInternalData,Standard_Transient)

#ifdef OCCT_DEBUG
static Standard_Integer HLRAlgo_PolyInternalData_TRACE = Standard_False;
static Standard_Integer HLRAlgo_PolyInternalData_ERROR = Standard_False;
#endif
//=======================================================================
//function : PolyInternalData
//purpose  : 
//=======================================================================

HLRAlgo_PolyInternalData::HLRAlgo_PolyInternalData
(const Standard_Integer nbNod,
 const Standard_Integer nbTri) :
 myNbTData(nbTri),
 myNbPISeg(0),
 myNbPINod(nbNod),
 myMxTData(nbTri),
 myMxPINod(nbNod),
 myIntOutL(Standard_False),
 myPlanar(Standard_False)
{
  Standard_Integer i;
  myMxPISeg = 2 + (3 * nbTri + nbNod) / 2;
  myTData = new HLRAlgo_HArray1OfTData(0,myMxTData);
  myPISeg = new HLRAlgo_HArray1OfPISeg(0,myMxPISeg);
  myPINod = new HLRAlgo_HArray1OfPINod(0,myMxPINod);
  
  HLRAlgo_Array1OfPINod& PINod = myPINod->ChangeArray1();
  Handle(HLRAlgo_PolyInternalNode)* NN = &(PINod.ChangeValue(1));

  for (i = 1; i <= myMxPINod; i++) {
    *NN = new HLRAlgo_PolyInternalNode();
    NN++;
  }
}

//=======================================================================
//function : UpdateLinks
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::UpdateLinks
(HLRAlgo_Array1OfTData*& TData,
 HLRAlgo_Array1OfPISeg*& PISeg,
 HLRAlgo_Array1OfPINod*& PINod)
{
  Standard_Integer i,n1,n2;
  Standard_Integer find,iiii,icsv = 0;
  HLRAlgo_PolyInternalSegment* aSegIndices = NULL;

  Standard_Boolean newSeg = Standard_False;
  HLRAlgo_TriangleData* TD =
    &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));
  
  for (i = 1; i <= myNbTData; i++) {
    HLRAlgo_PolyInternalNode::NodeIndices& A1 =
      PINod->ChangeValue(TD->Node1)->Indices();
    HLRAlgo_PolyInternalNode::NodeIndices& A2 =
      PINod->ChangeValue(TD->Node2)->Indices();
    HLRAlgo_PolyInternalNode::NodeIndices& A3 =
      PINod->ChangeValue(TD->Node3)->Indices();

    {
    n1 = TD->Node1;
    n2 = TD->Node2;
    newSeg = Standard_False;
    if (A1.NdSg == 0 && A2.NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      A1.NdSg = myNbPISeg;
      A2.NdSg = myNbPISeg;
    }
    else {
      iiii = A1.NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
	  aSegIndices = &PISeg->ChangeValue(iiii);
	  if (aSegIndices->LstSg1 == n1) {
	    if (aSegIndices->LstSg2 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (aSegIndices->LstSg1 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else aSegIndices->Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	A1.NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = A2.NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
	    aSegIndices = &PISeg->ChangeValue(iiii);
	    if (aSegIndices->LstSg1 == n2) { icsv = 1; iiii = aSegIndices->NxtSg1; }
	    else                  { icsv = 2; iiii = aSegIndices->NxtSg2; }
	  }
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else A2.NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      aSegIndices = &PISeg->ChangeValue(myNbPISeg);
      aSegIndices->LstSg1 = n1;
      aSegIndices->LstSg2 = n2;
      aSegIndices->Conex1 = i;
      aSegIndices->Conex2 = 0;
      aSegIndices->NxtSg1 = 0;
      aSegIndices->NxtSg2 = 0;
    }
    }

    {
    n1 = TD->Node2;
    n2 = TD->Node3;
    newSeg = Standard_False;
    if (A2.NdSg == 0 && A3.NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      A2.NdSg = myNbPISeg;
      A3.NdSg = myNbPISeg;
    }
    else {
      iiii = A2.NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
    aSegIndices = &PISeg->ChangeValue(iiii);
	  if (aSegIndices->LstSg1 == n1) {
	    if (aSegIndices->LstSg2 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (aSegIndices->LstSg1 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else aSegIndices->Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	A2.NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = A3.NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
      aSegIndices = &PISeg->ChangeValue(iiii);
	    if (aSegIndices->LstSg1 == n2) { icsv = 1; iiii = aSegIndices->NxtSg1; }
	    else                  { icsv = 2; iiii = aSegIndices->NxtSg2; }
	  }
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else A3.NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      aSegIndices = &PISeg->ChangeValue(myNbPISeg);
      aSegIndices->LstSg1 = n1;
      aSegIndices->LstSg2 = n2;
      aSegIndices->Conex1 = i;
      aSegIndices->Conex2 = 0;
      aSegIndices->NxtSg1 = 0;
      aSegIndices->NxtSg2 = 0;
    }
    }

    {
    n1 = TD->Node3;
    n2 = TD->Node1;
    newSeg = Standard_False;
    if (A3.NdSg == 0 && A1.NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      A3.NdSg = myNbPISeg;
      A1.NdSg = myNbPISeg;
    }
    else {
      iiii = A3.NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
    aSegIndices = &PISeg->ChangeValue(iiii);
	  if (aSegIndices->LstSg1 == n1) {
	    if (aSegIndices->LstSg2 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (aSegIndices->LstSg1 == n2) find = iiii;
	    else                  iiii = aSegIndices->NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else aSegIndices->Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	A3.NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = A1.NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
      aSegIndices = &PISeg->ChangeValue(iiii);
	    if (aSegIndices->LstSg1 == n2) { icsv = 1; iiii = aSegIndices->NxtSg1; }
	    else                  { icsv = 2; iiii = aSegIndices->NxtSg2; }
	  }
	  if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	  else           aSegIndices->NxtSg2 = myNbPISeg;
	}
	else A1.NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      aSegIndices = &PISeg->ChangeValue(myNbPISeg);
      aSegIndices->LstSg1 = n1;
      aSegIndices->LstSg2 = n2;
      aSegIndices->Conex1 = i;
      aSegIndices->Conex2 = 0;
      aSegIndices->NxtSg1 = 0;
      aSegIndices->NxtSg2 = 0;
    }
    }
    TD++;
  }
}

//=======================================================================
//function : AddNode
//purpose  : 
//=======================================================================

Standard_Integer
HLRAlgo_PolyInternalData::AddNode (
  HLRAlgo_PolyInternalNode::NodeData& Nod1RValues,
  HLRAlgo_PolyInternalNode::NodeData& Nod2RValues,
  HLRAlgo_Array1OfPINod*& PINod1,
  HLRAlgo_Array1OfPINod*& PINod2,
  const Standard_Real coef1,
  const Standard_Real X3,
  const Standard_Real Y3,
  const Standard_Real Z3)
{
  Standard_Real coef2 = 1 - coef1;
  IncPINod(PINod1,PINod2);
  Standard_Integer ip3 = myNbPINod;
  Handle(HLRAlgo_PolyInternalNode)* pip3 = 
    &(((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip3));
  *pip3 = new HLRAlgo_PolyInternalNode();
  HLRAlgo_PolyInternalNode::NodeData& Nod3RValues = (*pip3)->Data();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodeIndices = (*pip3)->Indices();
  aNodeIndices.NdSg = 0;
  aNodeIndices.Flag = 0;
  Nod3RValues.Point = gp_XYZ(X3, Y3, Z3);
  Nod3RValues.UV = coef2 * Nod1RValues.UV + coef1 * Nod2RValues.UV;
  Nod3RValues.Scal = Nod1RValues.Scal * coef2 + Nod2RValues.Scal * coef1;
  const gp_XYZ aXYZ = coef2 * Nod1RValues.Normal + coef1 * Nod2RValues.Normal;
  const Standard_Real aNorm = aXYZ.Modulus();

  if (aNorm > 0) {
    Nod3RValues.Normal = (1 / aNorm) * aXYZ;
  }
  else {
    Nod3RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_ERROR)
      cout << "HLRAlgo_PolyInternalData::AddNode" << endl;
#endif
  }
  return ip3;
}

//=======================================================================
//function : UpdateLinks
//purpose  : 
//=======================================================================

void
HLRAlgo_PolyInternalData::UpdateLinks (const Standard_Integer ip1,
				       const Standard_Integer ip2,
				       const Standard_Integer ip3,
				       HLRAlgo_Array1OfTData*& TData1,
				       HLRAlgo_Array1OfTData*& TData2,
				       HLRAlgo_Array1OfPISeg*& PISeg1,
				       HLRAlgo_Array1OfPISeg*& PISeg2,
				       HLRAlgo_Array1OfPINod*& PINod1,
				       HLRAlgo_Array1OfPINod*& )
{
  Standard_Integer find,iiii,iisv,icsv,iip2 =0,cnx1 =0,cnx2 =0;
  HLRAlgo_PolyInternalSegment* aSegIndices = NULL;
  HLRAlgo_PolyInternalSegment* aSegIndices2 = NULL;
  find = 0;
  iisv = 0;
  icsv = 0;
  IncPISeg(PISeg1,PISeg2);
  IncPISeg(PISeg1,PISeg2);
  IncPISeg(PISeg1,PISeg2);
  myNbPISeg--;
  myNbPISeg--;
  IncTData(TData1,TData2);
  IncTData(TData1,TData2);
  myNbTData--;
  myNbTData--;
  HLRAlgo_PolyInternalNode::NodeIndices* aNodIndices1 =
    &PINod1->ChangeValue(ip1)->Indices();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices2 =
    PINod1->ChangeValue(ip2)->Indices();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices3 =
    PINod1->ChangeValue(ip3)->Indices();
  iiii = aNodIndices2.NdSg;
  
  while (iiii != 0 && find == 0) {
    aSegIndices2 = &PISeg1->ChangeValue(iiii);
    if (aSegIndices2->LstSg1 == ip2) {
      if (aSegIndices2->LstSg2 == ip1) {
	find = iiii;
	cnx1 = aSegIndices2->Conex1;
	cnx2 = aSegIndices2->Conex2;
	aSegIndices2->LstSg1 = ip3;
	iip2 = aSegIndices2->NxtSg1;
	aSegIndices2->NxtSg1 = myNbPISeg;
	if      (iisv == 0) aNodIndices2.NdSg   = myNbPISeg;
	else if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	else                aSegIndices->NxtSg2 = myNbPISeg;
      }
      else { 
	iisv = iiii;
	icsv = 1;
      }
      iiii = aSegIndices2->NxtSg1;
    }
    else {
      if (aSegIndices2->LstSg1 == ip1) {
	find = iiii;
	cnx1 = aSegIndices2->Conex1;
	cnx2 = aSegIndices2->Conex2;
	aSegIndices2->LstSg2 = ip3;
	iip2 = aSegIndices2->NxtSg2;
	aSegIndices2->NxtSg2 = myNbPISeg;
	if      (iisv == 0) aNodIndices2.NdSg   = myNbPISeg;
	else if (icsv == 1) aSegIndices->NxtSg1 = myNbPISeg;
	else                aSegIndices->NxtSg2 = myNbPISeg;
      }
      else { 
	iisv = iiii;
	icsv = 2;
      }
      iiii = aSegIndices2->NxtSg2;
    }
    aSegIndices = aSegIndices2;
  }
  if (find == 0) {
    myNbPISeg--;
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_ERROR) {
      cout << "HLRAlgo_PolyInternalData::UpdateLinks : segment error";
      cout << endl;
    }
#endif
  }
  else {
    aSegIndices2 = &PISeg1->ChangeValue(myNbPISeg);
    aSegIndices2->NxtSg1 = 0;
    aSegIndices2->NxtSg2 = iip2;
    aSegIndices2->LstSg1 = ip3;
    aSegIndices2->LstSg2 = ip2;
    aSegIndices2->Conex1 = cnx1;
    aSegIndices2->Conex2 = cnx2;
    aNodIndices3.NdSg   = find;

    Standard_Integer iOld,iNew,iTr,skip,ip4,itpk[2];
    Standard_Integer n1,n2,n3,nOld[3],nNew[3],New[4];
    New[0] = cnx1;
    New[2] = myNbTData + 1;
    if (cnx2 == 0) {
      New[1] = 0;
      New[3] = 0;
    }
    else {
      New[1] = cnx2;
      New[3] = myNbTData + 2;
    }
    
    for (skip = 0; skip <= 1; skip++) {
      iOld = New[skip];
      iNew = New[skip + 2];
      if (iOld != 0) {
  HLRAlgo_TriangleData& aTriangle = TData1->ChangeValue(iOld);
  HLRAlgo_TriangleData& aTriangle2 = TData1->ChangeValue(iNew);
	n1 = aTriangle.Node1;
	n2 = aTriangle.Node2;
	n3 = aTriangle.Node3;
	nOld[0] = n1;
        nOld[1] = n2;
        nOld[2] = n3;
	nNew[0] = n1;
        nNew[1] = n2;
        nNew[2] = n3;
	Standard_Boolean found = Standard_False;
	if      (n1 == ip1 && n2 == ip2) {
	  found = Standard_True;
	  nOld[1] = ip3;
	  nNew[0] = ip3;
	  itpk[skip] = n3;
	}
	else if (n1 == ip2 && n2 == ip1) {
	  found = Standard_True;
	  nOld[0] = ip3;
	  nNew[1] = ip3;
	  itpk[skip] = n3;
	}
	else if (n2 == ip1 && n3 == ip2) {
	  found = Standard_True;
	  nOld[2] = ip3;
	  nNew[1] = ip3;
	  itpk[skip] = n1;
	}
	else if (n2 == ip2 && n3 == ip1) {
	  found = Standard_True;
	  nOld[1] = ip3;
	  nNew[2] = ip3;
	  itpk[skip] = n1;
	}
	else if (n3 == ip1 && n1 == ip2) {
	  found = Standard_True;
	  nOld[0] = ip3;
	  nNew[2] = ip3;
	  itpk[skip] = n2;
	}
	else if (n3 == ip2 && n1 == ip1) {
	  found = Standard_True;
	  nOld[2] = ip3;
	  nNew[0] = ip3;
	  itpk[skip] = n2;
	}
	if (found) {
	  myNbTData++;
	  ip4 = itpk[skip];
	  HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices4 =
      PINod1->ChangeValue(ip4)->Indices();
	  aTriangle.Node1 = nOld[0];
	  aTriangle.Node2 = nOld[1];
	  aTriangle.Node3 = nOld[2];
	  aTriangle2.Node1 = nNew[0];
	  aTriangle2.Node2 = nNew[1];
	  aTriangle2.Node3 = nNew[2];
	  aTriangle2.Flags = aTriangle.Flags;
	  myNbPISeg++;
    aSegIndices2 = &PISeg1->ChangeValue(myNbPISeg);
	  aSegIndices2->LstSg1 = ip3;
	  aSegIndices2->LstSg2 = ip4;
	  aSegIndices2->NxtSg1 = aNodIndices3.NdSg;
	  aSegIndices2->NxtSg2 = aNodIndices4.NdSg;
	  aSegIndices2->Conex1 = iOld;
	  aSegIndices2->Conex2 = iNew;
	  aNodIndices3.NdSg   = myNbPISeg;
	  aNodIndices4.NdSg   = myNbPISeg;
	}
#ifdef OCCT_DEBUG
	else if (HLRAlgo_PolyInternalData_ERROR) {
	  cout << "HLRAlgo_PolyInternalData::UpdateLinks : triangle error ";
	  cout << endl;
	}
#endif
      }
    }
    
    for (iTr = 0; iTr <= 3; iTr++) {
      iNew = New [iTr];
      if (iTr < 2) skip = iTr;
      else         skip = iTr - 2;
      iOld = New [skip];
      ip4  = itpk[skip];
      if (iNew != 0) {
  HLRAlgo_TriangleData& aTriangle2 = TData1->ChangeValue(iNew);
	n1 = aTriangle2.Node1;
	n2 = aTriangle2.Node2;
	n3 = aTriangle2.Node3;
	
	if (!((n1 == ip3 && n2 == ip4) ||
	      (n2 == ip3 && n1 == ip4))) {
	  Standard_Boolean found = Standard_False;
    aNodIndices1 = &PINod1->ChangeValue(n1)->Indices();
	  iiii = aNodIndices1->NdSg;
	  
	  while (iiii != 0 && !found) {
      aSegIndices2 = &PISeg1->ChangeValue(iiii);
	    if (aSegIndices2->LstSg1 == n1) {
	      if (aSegIndices2->LstSg2 == n2) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg1;
	    }
	    else {
	      if (aSegIndices2->LstSg1 == n2) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg2;
	    }
	  }
	}
	
	if (!((n2 == ip3 && n3 == ip4) ||
	      (n3 == ip3 && n2 == ip4))) {
	  Standard_Boolean found = Standard_False;
    aNodIndices1 = &PINod1->ChangeValue(n2)->Indices();
	  iiii = aNodIndices1->NdSg;
	  
	  while (iiii != 0 && !found) {
      aSegIndices2 = &PISeg1->ChangeValue(iiii);
	    if (aSegIndices2->LstSg1 == n2) {
	      if (aSegIndices2->LstSg2 == n3) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg1;
	    }
	    else {
	      if (aSegIndices2->LstSg1 == n3) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg2;
	    }
	  }
	}

	if (!((n3 == ip3 && n1 == ip4) ||
	      (n1 == ip3 && n3 == ip4))) {
	  Standard_Boolean found = Standard_False;
    aNodIndices1 = &PINod1->ChangeValue(n3)->Indices();
	  iiii = aNodIndices1->NdSg;
	  
	  while (iiii != 0 && !found) {
      aSegIndices2 = &PISeg1->ChangeValue(iiii);
	    if (aSegIndices2->LstSg1 == n3) {
	      if (aSegIndices2->LstSg2 == n1) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg1;
	    }
	    else {
	      if (aSegIndices2->LstSg1 == n1) {
		found = Standard_True;
		if      (aSegIndices2->Conex1 == iOld) aSegIndices2->Conex1 = iNew;
		else if (aSegIndices2->Conex2 == iOld) aSegIndices2->Conex2 = iNew;
	      }
	      else iiii = aSegIndices2->NxtSg2;
	    }
	  }
	}
      }
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::Dump () const
{
  Standard_Integer i;//,i1,i2,i3;
  HLRAlgo_Array1OfTData* TData = &myTData->ChangeArray1();
  HLRAlgo_Array1OfPISeg* PISeg = &myPISeg->ChangeArray1();
  HLRAlgo_Array1OfPINod* PINod = &myPINod->ChangeArray1();
  
  for (i = 1; i <= myNbPINod; i++) {
    const Handle(HLRAlgo_PolyInternalNode)* pi = &PINod->ChangeValue(i);
    HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices1 = (*pi)->Indices();
    HLRAlgo_PolyInternalNode::NodeData& Nod1RValues = (*pi)->Data();
    cout << "Node " << setw(6) << i << " : ";
    cout << setw(6) << aNodIndices1.NdSg;
    cout << setw(20)<< Nod1RValues.Point.X();
    cout << setw(20)<< Nod1RValues.Point.Y();
    cout << setw(20)<< Nod1RValues.Point.Z();
    cout << endl;
  }

  for (i = 1; i <= myNbPISeg; i++) {
    HLRAlgo_PolyInternalSegment* aSegIndices = &PISeg->ChangeValue(i);
    cout << "Segment " << setw(6) << i << " : ";
    cout << setw(6) << aSegIndices->LstSg1;
    cout << setw(6) << aSegIndices->LstSg2;
    cout << setw(6) << aSegIndices->NxtSg1;
    cout << setw(6) << aSegIndices->NxtSg2;
    cout << setw(6) << aSegIndices->Conex1;
    cout << setw(6) << aSegIndices->Conex2;
    cout << endl;
  }

  for (i = 1; i <= myNbTData; i++) {
    HLRAlgo_TriangleData& aTriangle = TData->ChangeValue(i);
    cout << "Triangle " << setw(6) << i << " : ";
    cout << setw(6) << aTriangle.Node1;
    cout << setw(6) << aTriangle.Node2;
    cout << setw(6) << aTriangle.Node3;
    cout << endl;
  }
}

//=======================================================================
//function : IncTData
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::IncTData(
  HLRAlgo_Array1OfTData*& TData1, HLRAlgo_Array1OfTData*& TData2)
{
  if (myNbTData >= myMxTData) {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      cout << "HLRAlgo_PolyInternalData::IncTData : " << myMxTData << endl;
#endif
    Standard_Integer i,j,k;
    j = myMxTData;
    k = 2 * j;

    Handle(HLRAlgo_HArray1OfTData) NwTData =
      new HLRAlgo_HArray1OfTData(0,k);
    HLRAlgo_Array1OfTData& oTData = myTData->ChangeArray1();
    HLRAlgo_Array1OfTData& nTData = NwTData->ChangeArray1();

    for (i = 1; i <= j; i++)
    {
      nTData.ChangeValue(i) = oTData.Value(i);
    }
    myMxTData = k;
    myTData  = NwTData;
    if (TData1 == TData2) {
      TData1 = &nTData;
      TData2 = TData1;
    }
    else {
      TData1 = &nTData;
    }
  }
  myNbTData++;
}

//=======================================================================
//function : IncPISeg
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::IncPISeg(
  HLRAlgo_Array1OfPISeg*& PISeg1, HLRAlgo_Array1OfPISeg*& PISeg2)
{ 
  if (myNbPISeg >= myMxPISeg) {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      cout << "HLRAlgo_PolyInternalData::IncPISeg : " << myMxPISeg << endl;
#endif
    Standard_Integer i,j,k;
    j = myMxPISeg;
    k = 2 * j;
    Handle(HLRAlgo_HArray1OfPISeg) NwPISeg =
      new HLRAlgo_HArray1OfPISeg(0,k);
    HLRAlgo_Array1OfPISeg& oPISeg = myPISeg->ChangeArray1();
    HLRAlgo_Array1OfPISeg& nPISeg = NwPISeg->ChangeArray1();

    for (i = 1; i <= j; i++)
    {
      nPISeg.ChangeValue(i) = oPISeg.Value(i);
    }
    myMxPISeg = k;
    myPISeg = NwPISeg;
    if (PISeg1 == PISeg2) {
      PISeg1 = &nPISeg;
      PISeg2 = PISeg1;
    }
    else {
      PISeg1 = &nPISeg;
    }
  }
  myNbPISeg++;
}

//=======================================================================
//function : IncPINod
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::IncPINod(
  HLRAlgo_Array1OfPINod*& PINod1, HLRAlgo_Array1OfPINod*& PINod2)
{
  if (myNbPINod >= myMxPINod) {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      cout << "HLRAlgo_PolyInternalData::IncPINod : " << myMxPINod << endl;
#endif
    Standard_Integer i,j,k;
    j = myMxPINod;
    k = 2 * j;
    Handle(HLRAlgo_HArray1OfPINod) NwPINod =
      new HLRAlgo_HArray1OfPINod(0,k);
    HLRAlgo_Array1OfPINod& oPINod = myPINod->ChangeArray1();
    HLRAlgo_Array1OfPINod& nPINod = NwPINod->ChangeArray1();
    Handle(HLRAlgo_PolyInternalNode)* ON = &(oPINod.ChangeValue(1));
    Handle(HLRAlgo_PolyInternalNode)* NN = &(nPINod.ChangeValue(1));

    for (i = 1; i <= j; i++) {
      *NN = *ON;
      ON++;
      NN++;
    }
    myMxPINod = k;
    myPINod = NwPINod;
    if (PINod1 == PINod2) {
      PINod1 = &nPINod;
      PINod2 = PINod1;
    }
    else {
      PINod1 = &nPINod;
    }
  }
  myNbPINod++;
}
