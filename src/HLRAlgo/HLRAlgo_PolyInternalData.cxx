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

#define Tri1Node1 ((Standard_Integer*)Tri1Indices)[0]
#define Tri1Node2 ((Standard_Integer*)Tri1Indices)[1]
#define Tri1Node3 ((Standard_Integer*)Tri1Indices)[2]
#define Tri1Flags ((Standard_Boolean*)Tri1Indices)[3]

#define Tri2Node1 ((Standard_Integer*)Tri2Indices)[0]
#define Tri2Node2 ((Standard_Integer*)Tri2Indices)[1]
#define Tri2Node3 ((Standard_Integer*)Tri2Indices)[2]
#define Tri2Flags ((Standard_Boolean*)Tri2Indices)[3]

#define Seg1LstSg1 ((Standard_Integer*)Seg1Indices)[0]
#define Seg1LstSg2 ((Standard_Integer*)Seg1Indices)[1]
#define Seg1NxtSg1 ((Standard_Integer*)Seg1Indices)[2]
#define Seg1NxtSg2 ((Standard_Integer*)Seg1Indices)[3]
#define Seg1Conex1 ((Standard_Integer*)Seg1Indices)[4]
#define Seg1Conex2 ((Standard_Integer*)Seg1Indices)[5]

#define Seg2LstSg1 ((Standard_Integer*)Seg2Indices)[0]
#define Seg2LstSg2 ((Standard_Integer*)Seg2Indices)[1]
#define Seg2NxtSg1 ((Standard_Integer*)Seg2Indices)[2]
#define Seg2NxtSg2 ((Standard_Integer*)Seg2Indices)[3]
#define Seg2Conex1 ((Standard_Integer*)Seg2Indices)[4]
#define Seg2Conex2 ((Standard_Integer*)Seg2Indices)[5]

#define Nod1NdSg ((Standard_Integer*)Nod1Indices)[0]
#define Nod1Flag ((Standard_Boolean*)Nod1Indices)[1]
#define Nod1Edg1 ((Standard_Boolean*)Nod1Indices)[2]
#define Nod1Edg2 ((Standard_Boolean*)Nod1Indices)[3]

#define Nod1PntX ((Standard_Real*)Nod1RValues)[ 0]
#define Nod1PntY ((Standard_Real*)Nod1RValues)[ 1]
#define Nod1PntZ ((Standard_Real*)Nod1RValues)[ 2]
#define Nod1PntU ((Standard_Real*)Nod1RValues)[ 3]
#define Nod1PntV ((Standard_Real*)Nod1RValues)[ 4]
#define Nod1NrmX ((Standard_Real*)Nod1RValues)[ 5]
#define Nod1NrmY ((Standard_Real*)Nod1RValues)[ 6]
#define Nod1NrmZ ((Standard_Real*)Nod1RValues)[ 7]
#define Nod1PCu1 ((Standard_Real*)Nod1RValues)[ 8]
#define Nod1PCu2 ((Standard_Real*)Nod1RValues)[ 9]
#define Nod1Scal ((Standard_Real*)Nod1RValues)[10]

#define Nod2NdSg ((Standard_Integer*)Nod2Indices)[0]
#define Nod2Flag ((Standard_Boolean*)Nod2Indices)[1]
#define Nod2Edg1 ((Standard_Boolean*)Nod2Indices)[2]
#define Nod2Edg2 ((Standard_Boolean*)Nod2Indices)[3]

#define Nod2PntX ((Standard_Real*)Nod2RValues)[ 0]
#define Nod2PntY ((Standard_Real*)Nod2RValues)[ 1]
#define Nod2PntZ ((Standard_Real*)Nod2RValues)[ 2]
#define Nod2PntU ((Standard_Real*)Nod2RValues)[ 3]
#define Nod2PntV ((Standard_Real*)Nod2RValues)[ 4]
#define Nod2NrmX ((Standard_Real*)Nod2RValues)[ 5]
#define Nod2NrmY ((Standard_Real*)Nod2RValues)[ 6]
#define Nod2NrmZ ((Standard_Real*)Nod2RValues)[ 7]
#define Nod2PCu1 ((Standard_Real*)Nod2RValues)[ 8]
#define Nod2PCu2 ((Standard_Real*)Nod2RValues)[ 9]
#define Nod2Scal ((Standard_Real*)Nod2RValues)[10]

#define Nod3NdSg ((Standard_Integer*)Nod3Indices)[0]
#define Nod3Flag ((Standard_Boolean*)Nod3Indices)[1]
#define Nod3Edg1 ((Standard_Boolean*)Nod3Indices)[2]
#define Nod3Edg2 ((Standard_Boolean*)Nod3Indices)[3]

#define Nod3PntX ((Standard_Real*)Nod3RValues)[ 0]
#define Nod3PntY ((Standard_Real*)Nod3RValues)[ 1]
#define Nod3PntZ ((Standard_Real*)Nod3RValues)[ 2]
#define Nod3PntU ((Standard_Real*)Nod3RValues)[ 3]
#define Nod3PntV ((Standard_Real*)Nod3RValues)[ 4]
#define Nod3NrmX ((Standard_Real*)Nod3RValues)[ 5]
#define Nod3NrmY ((Standard_Real*)Nod3RValues)[ 6]
#define Nod3NrmZ ((Standard_Real*)Nod3RValues)[ 7]
#define Nod3PCu1 ((Standard_Real*)Nod3RValues)[ 8]
#define Nod3PCu2 ((Standard_Real*)Nod3RValues)[ 9]
#define Nod3Scal ((Standard_Real*)Nod3RValues)[10]

#define Nod4NdSg ((Standard_Integer*)Nod4Indices)[0]
#define Nod4Flag ((Standard_Boolean*)Nod4Indices)[1]
#define Nod4Edg1 ((Standard_Boolean*)Nod4Indices)[2]
#define Nod4Edg2 ((Standard_Boolean*)Nod4Indices)[3]

#define Nod4PntX ((Standard_Real*)Nod4RValues)[ 0]
#define Nod4PntY ((Standard_Real*)Nod4RValues)[ 1]
#define Nod4PntZ ((Standard_Real*)Nod4RValues)[ 2]
#define Nod4PntU ((Standard_Real*)Nod4RValues)[ 3]
#define Nod4PntV ((Standard_Real*)Nod4RValues)[ 4]
#define Nod4NrmX ((Standard_Real*)Nod4RValues)[ 5]
#define Nod4NrmY ((Standard_Real*)Nod4RValues)[ 6]
#define Nod4NrmZ ((Standard_Real*)Nod4RValues)[ 7]
#define Nod4PCu1 ((Standard_Real*)Nod4RValues)[ 8]
#define Nod4PCu2 ((Standard_Real*)Nod4RValues)[ 9]
#define Nod4Scal ((Standard_Real*)Nod4RValues)[10]
#ifdef OCCT_DEBUG
static Standard_Integer TRACE = Standard_False;
static Standard_Integer ERROR = Standard_False;
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
//  Standard_Address Nod1Indices;
  
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
(Standard_Address& TData,
 Standard_Address& PISeg,
 Standard_Address& PINod)
{
  Standard_Integer i,n1,n2;
  Standard_Integer find,iiii,icsv = 0;
  Standard_Address A1,A2,A3,Nod1Indices,Nod2Indices,Seg2Indices = NULL,Tri2Indices;
  Standard_Boolean newSeg = Standard_False;
  HLRAlgo_TriangleData* TD =
    &(((HLRAlgo_Array1OfTData*)TData)->ChangeValue(1));
  
  for (i = 1; i <= myNbTData; i++) {
    Tri2Indices = TD->Indices();
    A1 = ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node1)->Indices();
    A2 = ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node2)->Indices();
    A3 = ((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(Tri2Node3)->Indices();
    n1 = Tri2Node1;
    n2 = Tri2Node2;
    Nod1Indices = A1;
    Nod2Indices = A2;
    newSeg = Standard_False;
    if (Nod1NdSg == 0 && Nod2NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      Nod1NdSg = myNbPISeg;
      Nod2NdSg = myNbPISeg;
    }
    else {
      iiii = Nod1NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
	  Seg2Indices =
	    ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	  if (Seg2LstSg1 == n1) {
	    if (Seg2LstSg2 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (Seg2LstSg1 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Seg2Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	Nod1NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = Nod2NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n2) { icsv = 1; iiii = Seg2NxtSg1; }
	    else                  { icsv = 2; iiii = Seg2NxtSg2; }
	  }
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Nod2NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      Seg2Indices =
	((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(myNbPISeg).Indices();
      Seg2LstSg1 = n1;
      Seg2LstSg2 = n2;
      Seg2Conex1 = i;
      Seg2Conex2 = 0;
      Seg2NxtSg1 = 0;
      Seg2NxtSg2 = 0;
    }
    
    n1 = Tri2Node2;
    n2 = Tri2Node3;
    Nod1Indices = A2;
    Nod2Indices = A3;
    newSeg = Standard_False;
    if (Nod1NdSg == 0 && Nod2NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      Nod1NdSg = myNbPISeg;
      Nod2NdSg = myNbPISeg;
    }
    else {
      iiii = Nod1NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
	  Seg2Indices =
	    ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	  if (Seg2LstSg1 == n1) {
	    if (Seg2LstSg2 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (Seg2LstSg1 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Seg2Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	Nod1NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = Nod2NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n2) { icsv = 1; iiii = Seg2NxtSg1; }
	    else                  { icsv = 2; iiii = Seg2NxtSg2; }
	  }
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Nod2NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      Seg2Indices =
	((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(myNbPISeg).Indices();
      Seg2LstSg1 = n1;
      Seg2LstSg2 = n2;
      Seg2Conex1 = i;
      Seg2Conex2 = 0;
      Seg2NxtSg1 = 0;
      Seg2NxtSg2 = 0;
    }
    
    n1 = Tri2Node3;
    n2 = Tri2Node1;
    Nod1Indices = A3;
    Nod2Indices = A1;
    newSeg = Standard_False;
    if (Nod1NdSg == 0 && Nod2NdSg == 0) {
      newSeg = Standard_True;
      myNbPISeg++;
      Nod1NdSg = myNbPISeg;
      Nod2NdSg = myNbPISeg;
    }
    else {
      iiii = Nod1NdSg;
      if (iiii != 0) {
	find = 0;
	
	while (iiii != 0 && find == 0) {
	  Seg2Indices =
	    ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	  if (Seg2LstSg1 == n1) {
	    if (Seg2LstSg2 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg1;
	    icsv = 1;
	  }
	  else {
	    if (Seg2LstSg1 == n2) find = iiii;
	    else                  iiii = Seg2NxtSg2;
	    icsv = 2;
	  }
	}
	if (find == 0) {
	  newSeg = Standard_True;
	  myNbPISeg++;
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Seg2Conex2 = i;
      }
      else {
	newSeg = Standard_True;
	myNbPISeg++;
	Nod1NdSg = myNbPISeg;
      }
      if (newSeg) {
	iiii = Nod2NdSg;
	if (iiii != 0) {
	  
	  while (iiii != 0) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n2) { icsv = 1; iiii = Seg2NxtSg1; }
	    else                  { icsv = 2; iiii = Seg2NxtSg2; }
	  }
	  if (icsv == 1) Seg2NxtSg1 = myNbPISeg;
	  else           Seg2NxtSg2 = myNbPISeg;
	}
	else Nod2NdSg = myNbPISeg;
      }
    }
    if (newSeg) {
      Seg2Indices =
	((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(myNbPISeg).Indices();
      Seg2LstSg1 = n1;
      Seg2LstSg2 = n2;
      Seg2Conex1 = i;
      Seg2Conex2 = 0;
      Seg2NxtSg1 = 0;
      Seg2NxtSg2 = 0;
    }
    TD++;
  }
}

//=======================================================================
//function : AddNode
//purpose  : 
//=======================================================================

Standard_Integer
HLRAlgo_PolyInternalData::AddNode (const Standard_Address Nod1RValues,
				   const Standard_Address Nod2RValues,
				   Standard_Address& PINod1,
				   Standard_Address& PINod2,
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
  const Standard_Address Nod3RValues = (*pip3)->RValues();
  const Standard_Address Nod3Indices = (*pip3)->Indices();
  Nod3NdSg = 0;
  Nod3Flag = 0;
  Nod3PntX = X3;
  Nod3PntY = Y3;
  Nod3PntZ = Z3;
  Nod3PntU = Nod1PntU * coef2 + Nod2PntU * coef1;
  Nod3PntV = Nod1PntV * coef2 + Nod2PntV * coef1; 
  Nod3Scal = Nod1Scal * coef2 + Nod2Scal * coef1;
  Standard_Real x = Nod1NrmX * coef2 + Nod2NrmX * coef1;
  Standard_Real y = Nod1NrmY * coef2 + Nod2NrmY * coef1;
  Standard_Real z = Nod1NrmZ * coef2 + Nod2NrmZ * coef1;
  Standard_Real d = sqrt(x * x + y * y + z * z);

  if (d > 0) {
    Nod3NrmX = x / d;
    Nod3NrmY = y / d;
    Nod3NrmZ = z / d;
  }
  else {
    Nod3NrmX = 1;
    Nod3NrmY = 0;
    Nod3NrmZ = 0;
#ifdef OCCT_DEBUG
    if (ERROR)
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
				       Standard_Address& TData1,
				       Standard_Address& TData2,
				       Standard_Address& PISeg1,
				       Standard_Address& PISeg2,
				       Standard_Address& PINod1,
				       Standard_Address& )
{
  Standard_Integer find,iiii,iisv,icsv,iip2 =0,cnx1 =0,cnx2 =0;
  Standard_Address Seg1Indices,Seg2Indices = NULL;
  Seg1Indices = 0;
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
  Standard_Address Nod1Indices =
    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip1)->Indices();
  Standard_Address Nod2Indices =
    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip2)->Indices();
  Standard_Address Nod3Indices =
    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip3)->Indices();
  iiii = Nod2NdSg;
  
  while (iiii != 0 && find == 0) {
    Seg2Indices =
      ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(iiii).Indices();
    if (Seg2LstSg1 == ip2) {
      if (Seg2LstSg2 == ip1) {
	find = iiii;
	cnx1 = Seg2Conex1;
	cnx2 = Seg2Conex2;
	Seg2LstSg1 = ip3;
	iip2 = Seg2NxtSg1;
	Seg2NxtSg1 = myNbPISeg;
	if      (iisv == 0) Nod2NdSg   = myNbPISeg;
	else if (icsv == 1) Seg1NxtSg1 = myNbPISeg;
	else                Seg1NxtSg2 = myNbPISeg;
      }
      else { 
	iisv = iiii;
	icsv = 1;
      }
      iiii = Seg2NxtSg1;
    }
    else {
      if (Seg2LstSg1 == ip1) {
	find = iiii;
	cnx1 = Seg2Conex1;
	cnx2 = Seg2Conex2;
	Seg2LstSg2 = ip3;
	iip2 = Seg2NxtSg2;
	Seg2NxtSg2 = myNbPISeg;
	if      (iisv == 0) Nod2NdSg   = myNbPISeg;
	else if (icsv == 1) Seg1NxtSg1 = myNbPISeg;
	else                Seg1NxtSg2 = myNbPISeg;
      }
      else { 
	iisv = iiii;
	icsv = 2;
      }
      iiii = Seg2NxtSg2;
    }
    Seg1Indices = Seg2Indices;
  }
  if (find == 0) {
    myNbPISeg--;
#ifdef OCCT_DEBUG
    if (ERROR) {
      cout << "HLRAlgo_PolyInternalData::UpdateLinks : segment error";
      cout << endl;
    }
#endif
  }
  else {
    Seg2Indices =
      ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(myNbPISeg).Indices();
    Seg2NxtSg1 = 0;
    Seg2NxtSg2 = iip2;
    Seg2LstSg1 = ip3;
    Seg2LstSg2 = ip2;
    Seg2Conex1 = cnx1;
    Seg2Conex2 = cnx2;
    Nod3NdSg   = find;

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
	Standard_Address Tri1Indices =
	  ((HLRAlgo_Array1OfTData*)TData1)->ChangeValue(iOld).Indices();
	Standard_Address Tri2Indices =
	  ((HLRAlgo_Array1OfTData*)TData1)->ChangeValue(iNew).Indices();
	n1 = Tri1Node1;
	n2 = Tri1Node2;
	n3 = Tri1Node3;
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
	  Standard_Address Nod4Indices =
	    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(ip4)->Indices();
	  Tri1Node1 = nOld[0];
	  Tri1Node2 = nOld[1];
	  Tri1Node3 = nOld[2];
	  Tri2Node1 = nNew[0];
	  Tri2Node2 = nNew[1];
	  Tri2Node3 = nNew[2];
	  Tri2Flags = Tri1Flags;
	  myNbPISeg++;
	  Seg2Indices =
	    ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(myNbPISeg).Indices();
	  Seg2LstSg1 = ip3;
	  Seg2LstSg2 = ip4;
	  Seg2NxtSg1 = Nod3NdSg;
	  Seg2NxtSg2 = Nod4NdSg;
	  Seg2Conex1 = iOld;
	  Seg2Conex2 = iNew;
	  Nod3NdSg   = myNbPISeg;
	  Nod4NdSg   = myNbPISeg;
	}
#ifdef OCCT_DEBUG
	else if (ERROR) {
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
	Standard_Address Tri2Indices =
	  ((HLRAlgo_Array1OfTData*)TData1)->ChangeValue(iNew).Indices();
	n1 = Tri2Node1;
	n2 = Tri2Node2;
	n3 = Tri2Node3;	
	
	if (!((n1 == ip3 && n2 == ip4) ||
	      (n2 == ip3 && n1 == ip4))) {
	  Standard_Boolean found = Standard_False;
	  Nod1Indices =
	    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(n1)->Indices();
	  iiii = Nod1NdSg;
	  
	  while (iiii != 0 && !found) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n1) {
	      if (Seg2LstSg2 == n2) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg1;
	    }
	    else {
	      if (Seg2LstSg1 == n2) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg2;
	    }
	  }
	}
	
	if (!((n2 == ip3 && n3 == ip4) ||
	      (n3 == ip3 && n2 == ip4))) {
	  Standard_Boolean found = Standard_False;
	  Nod1Indices =
	    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(n2)->Indices();
	  iiii = Nod1NdSg;
	  
	  while (iiii != 0 && !found) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n2) {
	      if (Seg2LstSg2 == n3) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg1;
	    }
	    else {
	      if (Seg2LstSg1 == n3) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg2;
	    }
	  }
	}

	if (!((n3 == ip3 && n1 == ip4) ||
	      (n1 == ip3 && n3 == ip4))) {
	  Standard_Boolean found = Standard_False;
	  Nod1Indices =
	    ((HLRAlgo_Array1OfPINod*)PINod1)->ChangeValue(n3)->Indices();
	  iiii = Nod1NdSg;
	  
	  while (iiii != 0 && !found) {
	    Seg2Indices =
	      ((HLRAlgo_Array1OfPISeg*)PISeg1)->ChangeValue(iiii).Indices();
	    if (Seg2LstSg1 == n3) {
	      if (Seg2LstSg2 == n1) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg1;
	    }
	    else {
	      if (Seg2LstSg1 == n1) {
		found = Standard_True;
		if      (Seg2Conex1 == iOld) Seg2Conex1 = iNew;
		else if (Seg2Conex2 == iOld) Seg2Conex2 = iNew;
	      }
	      else iiii = Seg2NxtSg2;
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
  Standard_Address Seg2Indices;
  Standard_Address TData = &myTData->ChangeArray1();
  Standard_Address PISeg = &myPISeg->ChangeArray1();
  Standard_Address PINod = &myPINod->ChangeArray1();
  
  for (i = 1; i <= myNbPINod; i++) {
    const Handle(HLRAlgo_PolyInternalNode)* pi =
      &(((HLRAlgo_Array1OfPINod*)PINod)->ChangeValue(i));
    Standard_Address Nod1Indices = (*pi)->Indices();
    Standard_Address Nod1RValues = (*pi)->RValues();
    cout << "Node " << setw(6) << i << " : ";
    cout << setw(6) << Nod1NdSg;
    cout << setw(20)<< Nod1PntX;
    cout << setw(20)<< Nod1PntY;
    cout << setw(20)<< Nod1PntZ;
    cout << endl;
  }

  for (i = 1; i <= myNbPISeg; i++) {
    Seg2Indices =
      ((HLRAlgo_Array1OfPISeg*)PISeg)->ChangeValue(i).Indices();
    cout << "Segment " << setw(6) << i << " : ";
    cout << setw(6) << Seg2LstSg1;
    cout << setw(6) << Seg2LstSg2;
    cout << setw(6) << Seg2NxtSg1;
    cout << setw(6) << Seg2NxtSg2;
    cout << setw(6) << Seg2Conex1;
    cout << setw(6) << Seg2Conex2;
    cout << endl;
  }

  for (i = 1; i <= myNbTData; i++) {
    Standard_Address Tri2Indices =
      ((HLRAlgo_Array1OfTData*)TData)->ChangeValue(i).Indices();
    cout << "Triangle " << setw(6) << i << " : ";
    cout << setw(6) << Tri2Node1;
    cout << setw(6) << Tri2Node2;
    cout << setw(6) << Tri2Node3;
    cout << endl;
  }
}

//=======================================================================
//function : IncTData
//purpose  : 
//=======================================================================

void HLRAlgo_PolyInternalData::
IncTData (Standard_Address& TData1,
	  Standard_Address& TData2)
{
  if (myNbTData >= myMxTData) {
#ifdef OCCT_DEBUG
    if (TRACE) 
      cout << "HLRAlgo_PolyInternalData::IncTData : " << myMxTData << endl;
#endif
    Standard_Integer i,j,k;
    j = myMxTData;
    k = 2 * j;

    Handle(HLRAlgo_HArray1OfTData) NwTData =
      new HLRAlgo_HArray1OfTData(0,k);
    HLRAlgo_Array1OfTData& oTData = myTData->ChangeArray1();
    HLRAlgo_Array1OfTData& nTData = NwTData->ChangeArray1();
    HLRAlgo_TriangleData* OT = &(oTData.ChangeValue(1));
    HLRAlgo_TriangleData* NT = &(nTData.ChangeValue(1));
    Standard_Address Tri1Indices,Tri2Indices;

    for (i = 1; i <= j; i++) {
      Tri1Indices = OT->Indices();
      Tri2Indices = NT->Indices();
      Tri2Node1 = Tri1Node1;
      Tri2Node2 = Tri1Node2;
      Tri2Node3 = Tri1Node3;
      Tri2Flags = Tri1Flags;
      OT++;
      NT++;
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

void HLRAlgo_PolyInternalData::
IncPISeg (Standard_Address& PISeg1,
	  Standard_Address& PISeg2)
{ 
  if (myNbPISeg >= myMxPISeg) {
#ifdef OCCT_DEBUG
    if (TRACE) 
      cout << "HLRAlgo_PolyInternalData::IncPISeg : " << myMxPISeg << endl;
#endif
    Standard_Integer i,j,k;
    j = myMxPISeg;
    k = 2 * j;
    Handle(HLRAlgo_HArray1OfPISeg) NwPISeg =
      new HLRAlgo_HArray1OfPISeg(0,k);
    HLRAlgo_Array1OfPISeg& oPISeg = myPISeg->ChangeArray1();
    HLRAlgo_Array1OfPISeg& nPISeg = NwPISeg->ChangeArray1();
    HLRAlgo_PolyInternalSegment* OS = &(oPISeg.ChangeValue(1));
    HLRAlgo_PolyInternalSegment* NS = &(nPISeg.ChangeValue(1));
    Standard_Address Seg1Indices,Seg2Indices;

    for (i = 1; i <= j; i++) {
      Seg1Indices = OS->Indices();
      Seg2Indices = NS->Indices();
      Seg2LstSg1 = Seg1LstSg1;
      Seg2LstSg2 = Seg1LstSg2;
      Seg2NxtSg1 = Seg1NxtSg1;
      Seg2NxtSg2 = Seg1NxtSg2;
      Seg2Conex1 = Seg1Conex1;
      Seg2Conex2 = Seg1Conex2;
      OS++;
      NS++;
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

void HLRAlgo_PolyInternalData::
IncPINod (Standard_Address& PINod1,
	  Standard_Address& PINod2)
{
  if (myNbPINod >= myMxPINod) {
#ifdef OCCT_DEBUG
    if (TRACE) 
      cout << "HLRAlgo_PolyInternalData::IncPINod : " << myMxPINod << endl;
#endif
    Standard_Integer i,j,k;
//    Standard_Address Nod1Indices;
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
