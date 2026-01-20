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
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyInternalData, Standard_Transient)

#ifdef OCCT_DEBUG
static int HLRAlgo_PolyInternalData_TRACE = false;
static int HLRAlgo_PolyInternalData_ERROR = false;
#endif
//=================================================================================================

HLRAlgo_PolyInternalData::HLRAlgo_PolyInternalData(const int nbNod,
                                                   const int nbTri)
    : myNbTData(nbTri),
      myNbPISeg(0),
      myNbPINod(nbNod),
      myMxTData(nbTri),
      myMxPINod(nbNod),
      myIntOutL(false),
      myPlanar(false)
{
  int i;
  myMxPISeg = 2 + (3 * nbTri + nbNod) / 2;
  myTData   = new NCollection_HArray1<HLRAlgo_TriangleData>(0, myMxTData);
  myPISeg   = new NCollection_HArray1<HLRAlgo_PolyInternalSegment>(0, myMxPISeg);
  myPINod   = new NCollection_HArray1<occ::handle<HLRAlgo_PolyInternalNode>>(0, myMxPINod);

  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&            PINod = myPINod->ChangeArray1();
  occ::handle<HLRAlgo_PolyInternalNode>* NN    = &(PINod.ChangeValue(1));

  for (i = 1; i <= myMxPINod; i++)
  {
    *NN = new HLRAlgo_PolyInternalNode();
    NN++;
  }
}

//=================================================================================================

void HLRAlgo_PolyInternalData::UpdateLinks(NCollection_Array1<HLRAlgo_TriangleData>& theTData,
                                           NCollection_Array1<HLRAlgo_PolyInternalSegment>& thePISeg,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>& thePINod)
{
  int             n1, n2;
  int             find, iiii, icsv = 0;
  HLRAlgo_PolyInternalSegment* aSegIndices = NULL;

  bool newSeg = false;
  for (int i = 1; i <= myNbTData; i++)
  {
    HLRAlgo_TriangleData* TD = &theTData.ChangeValue(i);

    HLRAlgo_PolyInternalNode::NodeIndices& A1 = thePINod.ChangeValue(TD->Node1)->Indices();
    HLRAlgo_PolyInternalNode::NodeIndices& A2 = thePINod.ChangeValue(TD->Node2)->Indices();
    HLRAlgo_PolyInternalNode::NodeIndices& A3 = thePINod.ChangeValue(TD->Node3)->Indices();

    {
      n1     = TD->Node1;
      n2     = TD->Node2;
      newSeg = false;
      if (A1.NdSg == 0 && A2.NdSg == 0)
      {
        newSeg = true;
        myNbPISeg++;
        A1.NdSg = myNbPISeg;
        A2.NdSg = myNbPISeg;
      }
      else
      {
        iiii = A1.NdSg;
        if (iiii != 0)
        {
          find = 0;

          while (iiii != 0 && find == 0)
          {
            aSegIndices = &thePISeg.ChangeValue(iiii);
            if (aSegIndices->LstSg1 == n1)
            {
              if (aSegIndices->LstSg2 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg1;
              icsv = 1;
            }
            else
            {
              if (aSegIndices->LstSg1 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg2;
              icsv = 2;
            }
          }
          if (find == 0)
          {
            newSeg = true;
            myNbPISeg++;
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            aSegIndices->Conex2 = i;
        }
        else
        {
          newSeg = true;
          myNbPISeg++;
          A1.NdSg = myNbPISeg;
        }
        if (newSeg)
        {
          iiii = A2.NdSg;
          if (iiii != 0)
          {

            while (iiii != 0)
            {
              aSegIndices = &thePISeg.ChangeValue(iiii);
              if (aSegIndices->LstSg1 == n2)
              {
                icsv = 1;
                iiii = aSegIndices->NxtSg1;
              }
              else
              {
                icsv = 2;
                iiii = aSegIndices->NxtSg2;
              }
            }
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            A2.NdSg = myNbPISeg;
        }
      }
      if (newSeg)
      {
        aSegIndices         = &thePISeg.ChangeValue(myNbPISeg);
        aSegIndices->LstSg1 = n1;
        aSegIndices->LstSg2 = n2;
        aSegIndices->Conex1 = i;
        aSegIndices->Conex2 = 0;
        aSegIndices->NxtSg1 = 0;
        aSegIndices->NxtSg2 = 0;
      }
    }

    {
      n1     = TD->Node2;
      n2     = TD->Node3;
      newSeg = false;
      if (A2.NdSg == 0 && A3.NdSg == 0)
      {
        newSeg = true;
        myNbPISeg++;
        A2.NdSg = myNbPISeg;
        A3.NdSg = myNbPISeg;
      }
      else
      {
        iiii = A2.NdSg;
        if (iiii != 0)
        {
          find = 0;

          while (iiii != 0 && find == 0)
          {
            aSegIndices = &thePISeg.ChangeValue(iiii);
            if (aSegIndices->LstSg1 == n1)
            {
              if (aSegIndices->LstSg2 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg1;
              icsv = 1;
            }
            else
            {
              if (aSegIndices->LstSg1 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg2;
              icsv = 2;
            }
          }
          if (find == 0)
          {
            newSeg = true;
            myNbPISeg++;
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            aSegIndices->Conex2 = i;
        }
        else
        {
          newSeg = true;
          myNbPISeg++;
          A2.NdSg = myNbPISeg;
        }
        if (newSeg)
        {
          iiii = A3.NdSg;
          if (iiii != 0)
          {

            while (iiii != 0)
            {
              aSegIndices = &thePISeg.ChangeValue(iiii);
              if (aSegIndices->LstSg1 == n2)
              {
                icsv = 1;
                iiii = aSegIndices->NxtSg1;
              }
              else
              {
                icsv = 2;
                iiii = aSegIndices->NxtSg2;
              }
            }
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            A3.NdSg = myNbPISeg;
        }
      }
      if (newSeg)
      {
        aSegIndices         = &thePISeg.ChangeValue(myNbPISeg);
        aSegIndices->LstSg1 = n1;
        aSegIndices->LstSg2 = n2;
        aSegIndices->Conex1 = i;
        aSegIndices->Conex2 = 0;
        aSegIndices->NxtSg1 = 0;
        aSegIndices->NxtSg2 = 0;
      }
    }

    {
      n1     = TD->Node3;
      n2     = TD->Node1;
      newSeg = false;
      if (A3.NdSg == 0 && A1.NdSg == 0)
      {
        newSeg = true;
        myNbPISeg++;
        A3.NdSg = myNbPISeg;
        A1.NdSg = myNbPISeg;
      }
      else
      {
        iiii = A3.NdSg;
        if (iiii != 0)
        {
          find = 0;

          while (iiii != 0 && find == 0)
          {
            aSegIndices = &thePISeg.ChangeValue(iiii);
            if (aSegIndices->LstSg1 == n1)
            {
              if (aSegIndices->LstSg2 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg1;
              icsv = 1;
            }
            else
            {
              if (aSegIndices->LstSg1 == n2)
                find = iiii;
              else
                iiii = aSegIndices->NxtSg2;
              icsv = 2;
            }
          }
          if (find == 0)
          {
            newSeg = true;
            myNbPISeg++;
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            aSegIndices->Conex2 = i;
        }
        else
        {
          newSeg = true;
          myNbPISeg++;
          A3.NdSg = myNbPISeg;
        }
        if (newSeg)
        {
          iiii = A1.NdSg;
          if (iiii != 0)
          {

            while (iiii != 0)
            {
              aSegIndices = &thePISeg.ChangeValue(iiii);
              if (aSegIndices->LstSg1 == n2)
              {
                icsv = 1;
                iiii = aSegIndices->NxtSg1;
              }
              else
              {
                icsv = 2;
                iiii = aSegIndices->NxtSg2;
              }
            }
            if (icsv == 1)
              aSegIndices->NxtSg1 = myNbPISeg;
            else
              aSegIndices->NxtSg2 = myNbPISeg;
          }
          else
            A1.NdSg = myNbPISeg;
        }
      }
      if (newSeg)
      {
        aSegIndices         = &thePISeg.ChangeValue(myNbPISeg);
        aSegIndices->LstSg1 = n1;
        aSegIndices->LstSg2 = n2;
        aSegIndices->Conex1 = i;
        aSegIndices->Conex2 = 0;
        aSegIndices->NxtSg1 = 0;
        aSegIndices->NxtSg2 = 0;
      }
    }
  }
}

//=================================================================================================

int HLRAlgo_PolyInternalData::AddNode(
  HLRAlgo_PolyInternalNode::NodeData& theNod1RValues,
  HLRAlgo_PolyInternalNode::NodeData& theNod2RValues,
  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&             thePINod1,
  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&             thePINod2,
  const double                 theCoef1,
  const double                 theX3,
  const double                 theY3,
  const double                 theZ3)
{
  double coef2 = 1 - theCoef1;
  IncPINod(thePINod1, thePINod2);
  int                  ip3               = myNbPINod;
  occ::handle<HLRAlgo_PolyInternalNode>& pip3              = thePINod1->ChangeValue(ip3);
  pip3                                                = new HLRAlgo_PolyInternalNode();
  HLRAlgo_PolyInternalNode::NodeData&    Nod3RValues  = pip3->Data();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodeIndices = pip3->Indices();
  aNodeIndices.NdSg                                   = 0;
  aNodeIndices.Flag                                   = 0;
  Nod3RValues.Point                                   = gp_XYZ(theX3, theY3, theZ3);
  Nod3RValues.UV            = coef2 * theNod1RValues.UV + theCoef1 * theNod2RValues.UV;
  Nod3RValues.Scal          = theNod1RValues.Scal * coef2 + theNod2RValues.Scal * theCoef1;
  const gp_XYZ        aXYZ  = coef2 * theNod1RValues.Normal + theCoef1 * theNod2RValues.Normal;
  const double aNorm = aXYZ.Modulus();

  if (aNorm > 0)
  {
    Nod3RValues.Normal = (1 / aNorm) * aXYZ;
  }
  else
  {
    Nod3RValues.Normal = gp_XYZ(1., 0., 0.);
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_ERROR)
      std::cout << "HLRAlgo_PolyInternalData::AddNode" << std::endl;
#endif
  }
  return ip3;
}

//=================================================================================================

void HLRAlgo_PolyInternalData::UpdateLinks(const int  ip1,
                                           const int  ip2,
                                           const int  ip3,
                                           NCollection_Array1<HLRAlgo_TriangleData>*& TData1,
                                           NCollection_Array1<HLRAlgo_TriangleData>*& TData2,
                                           NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg1,
                                           NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg2,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& PINod1,
                                           NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*&)
{
  int             find, iiii, iisv, icsv, iip2 = 0, cnx1 = 0, cnx2 = 0;
  HLRAlgo_PolyInternalSegment* aSegIndices  = NULL;
  HLRAlgo_PolyInternalSegment* aSegIndices2 = NULL;
  find                                      = 0;
  iisv                                      = 0;
  icsv                                      = 0;
  IncPISeg(PISeg1, PISeg2);
  IncPISeg(PISeg1, PISeg2);
  IncPISeg(PISeg1, PISeg2);
  myNbPISeg--;
  myNbPISeg--;
  IncTData(TData1, TData2);
  IncTData(TData1, TData2);
  myNbTData--;
  myNbTData--;
  HLRAlgo_PolyInternalNode::NodeIndices* aNodIndices1 = &PINod1->ChangeValue(ip1)->Indices();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices2 = PINod1->ChangeValue(ip2)->Indices();
  HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices3 = PINod1->ChangeValue(ip3)->Indices();
  iiii                                                = aNodIndices2.NdSg;

  while (iiii != 0 && find == 0)
  {
    aSegIndices2 = &PISeg1->ChangeValue(iiii);
    if (aSegIndices2->LstSg1 == ip2)
    {
      if (aSegIndices2->LstSg2 == ip1)
      {
        find                 = iiii;
        cnx1                 = aSegIndices2->Conex1;
        cnx2                 = aSegIndices2->Conex2;
        aSegIndices2->LstSg1 = ip3;
        iip2                 = aSegIndices2->NxtSg1;
        aSegIndices2->NxtSg1 = myNbPISeg;
        if (iisv == 0)
          aNodIndices2.NdSg = myNbPISeg;
        else if (icsv == 1)
          aSegIndices->NxtSg1 = myNbPISeg;
        else
          aSegIndices->NxtSg2 = myNbPISeg;
      }
      else
      {
        iisv = iiii;
        icsv = 1;
      }
      iiii = aSegIndices2->NxtSg1;
    }
    else
    {
      if (aSegIndices2->LstSg1 == ip1)
      {
        find                 = iiii;
        cnx1                 = aSegIndices2->Conex1;
        cnx2                 = aSegIndices2->Conex2;
        aSegIndices2->LstSg2 = ip3;
        iip2                 = aSegIndices2->NxtSg2;
        aSegIndices2->NxtSg2 = myNbPISeg;
        if (iisv == 0)
          aNodIndices2.NdSg = myNbPISeg;
        else if (icsv == 1)
          aSegIndices->NxtSg1 = myNbPISeg;
        else
          aSegIndices->NxtSg2 = myNbPISeg;
      }
      else
      {
        iisv = iiii;
        icsv = 2;
      }
      iiii = aSegIndices2->NxtSg2;
    }
    aSegIndices = aSegIndices2;
  }
  if (find == 0)
  {
    myNbPISeg--;
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_ERROR)
    {
      std::cout << "HLRAlgo_PolyInternalData::UpdateLinks : segment error";
      std::cout << std::endl;
    }
#endif
  }
  else
  {
    aSegIndices2         = &PISeg1->ChangeValue(myNbPISeg);
    aSegIndices2->NxtSg1 = 0;
    aSegIndices2->NxtSg2 = iip2;
    aSegIndices2->LstSg1 = ip3;
    aSegIndices2->LstSg2 = ip2;
    aSegIndices2->Conex1 = cnx1;
    aSegIndices2->Conex2 = cnx2;
    aNodIndices3.NdSg    = find;

    int iOld, iNew, iTr, skip, ip4, itpk[2];
    int n1, n2, n3, nOld[3], nNew[3], New[4];
    New[0] = cnx1;
    New[2] = myNbTData + 1;
    if (cnx2 == 0)
    {
      New[1] = 0;
      New[3] = 0;
    }
    else
    {
      New[1] = cnx2;
      New[3] = myNbTData + 2;
    }

    for (skip = 0; skip <= 1; skip++)
    {
      iOld = New[skip];
      iNew = New[skip + 2];
      if (iOld != 0)
      {
        HLRAlgo_TriangleData& aTriangle  = TData1->ChangeValue(iOld);
        HLRAlgo_TriangleData& aTriangle2 = TData1->ChangeValue(iNew);
        n1                               = aTriangle.Node1;
        n2                               = aTriangle.Node2;
        n3                               = aTriangle.Node3;
        nOld[0]                          = n1;
        nOld[1]                          = n2;
        nOld[2]                          = n3;
        nNew[0]                          = n1;
        nNew[1]                          = n2;
        nNew[2]                          = n3;
        bool found           = false;
        if (n1 == ip1 && n2 == ip2)
        {
          found      = true;
          nOld[1]    = ip3;
          nNew[0]    = ip3;
          itpk[skip] = n3;
        }
        else if (n1 == ip2 && n2 == ip1)
        {
          found      = true;
          nOld[0]    = ip3;
          nNew[1]    = ip3;
          itpk[skip] = n3;
        }
        else if (n2 == ip1 && n3 == ip2)
        {
          found      = true;
          nOld[2]    = ip3;
          nNew[1]    = ip3;
          itpk[skip] = n1;
        }
        else if (n2 == ip2 && n3 == ip1)
        {
          found      = true;
          nOld[1]    = ip3;
          nNew[2]    = ip3;
          itpk[skip] = n1;
        }
        else if (n3 == ip1 && n1 == ip2)
        {
          found      = true;
          nOld[0]    = ip3;
          nNew[2]    = ip3;
          itpk[skip] = n2;
        }
        else if (n3 == ip2 && n1 == ip1)
        {
          found      = true;
          nOld[2]    = ip3;
          nNew[0]    = ip3;
          itpk[skip] = n2;
        }
        if (found)
        {
          myNbTData++;
          ip4                                                 = itpk[skip];
          HLRAlgo_PolyInternalNode::NodeIndices& aNodIndices4 = PINod1->ChangeValue(ip4)->Indices();
          aTriangle.Node1                                     = nOld[0];
          aTriangle.Node2                                     = nOld[1];
          aTriangle.Node3                                     = nOld[2];
          aTriangle2.Node1                                    = nNew[0];
          aTriangle2.Node2                                    = nNew[1];
          aTriangle2.Node3                                    = nNew[2];
          aTriangle2.Flags                                    = aTriangle.Flags;
          myNbPISeg++;
          aSegIndices2         = &PISeg1->ChangeValue(myNbPISeg);
          aSegIndices2->LstSg1 = ip3;
          aSegIndices2->LstSg2 = ip4;
          aSegIndices2->NxtSg1 = aNodIndices3.NdSg;
          aSegIndices2->NxtSg2 = aNodIndices4.NdSg;
          aSegIndices2->Conex1 = iOld;
          aSegIndices2->Conex2 = iNew;
          aNodIndices3.NdSg    = myNbPISeg;
          aNodIndices4.NdSg    = myNbPISeg;
        }
#ifdef OCCT_DEBUG
        else if (HLRAlgo_PolyInternalData_ERROR)
        {
          std::cout << "HLRAlgo_PolyInternalData::UpdateLinks : triangle error ";
          std::cout << std::endl;
        }
#endif
      }
    }

    for (iTr = 0; iTr <= 3; iTr++)
    {
      iNew = New[iTr];
      if (iTr < 2)
        skip = iTr;
      else
        skip = iTr - 2;
      iOld = New[skip];
      ip4  = itpk[skip];
      if (iNew != 0)
      {
        HLRAlgo_TriangleData& aTriangle2 = TData1->ChangeValue(iNew);
        n1                               = aTriangle2.Node1;
        n2                               = aTriangle2.Node2;
        n3                               = aTriangle2.Node3;

        if (!((n1 == ip3 && n2 == ip4) || (n2 == ip3 && n1 == ip4)))
        {
          bool found = false;
          aNodIndices1           = &PINod1->ChangeValue(n1)->Indices();
          iiii                   = aNodIndices1->NdSg;

          while (iiii != 0 && !found)
          {
            aSegIndices2 = &PISeg1->ChangeValue(iiii);
            if (aSegIndices2->LstSg1 == n1)
            {
              if (aSegIndices2->LstSg2 == n2)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg1;
            }
            else
            {
              if (aSegIndices2->LstSg1 == n2)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg2;
            }
          }
        }

        if (!((n2 == ip3 && n3 == ip4) || (n3 == ip3 && n2 == ip4)))
        {
          bool found = false;
          aNodIndices1           = &PINod1->ChangeValue(n2)->Indices();
          iiii                   = aNodIndices1->NdSg;

          while (iiii != 0 && !found)
          {
            aSegIndices2 = &PISeg1->ChangeValue(iiii);
            if (aSegIndices2->LstSg1 == n2)
            {
              if (aSegIndices2->LstSg2 == n3)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg1;
            }
            else
            {
              if (aSegIndices2->LstSg1 == n3)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg2;
            }
          }
        }

        if (!((n3 == ip3 && n1 == ip4) || (n1 == ip3 && n3 == ip4)))
        {
          bool found = false;
          aNodIndices1           = &PINod1->ChangeValue(n3)->Indices();
          iiii                   = aNodIndices1->NdSg;

          while (iiii != 0 && !found)
          {
            aSegIndices2 = &PISeg1->ChangeValue(iiii);
            if (aSegIndices2->LstSg1 == n3)
            {
              if (aSegIndices2->LstSg2 == n1)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg1;
            }
            else
            {
              if (aSegIndices2->LstSg1 == n1)
              {
                found = true;
                if (aSegIndices2->Conex1 == iOld)
                  aSegIndices2->Conex1 = iNew;
                else if (aSegIndices2->Conex2 == iOld)
                  aSegIndices2->Conex2 = iNew;
              }
              else
                iiii = aSegIndices2->NxtSg2;
            }
          }
        }
      }
    }
  }
}

//=================================================================================================

void HLRAlgo_PolyInternalData::Dump() const
{
  int       i; //,i1,i2,i3;
  NCollection_Array1<HLRAlgo_TriangleData>* TData = &myTData->ChangeArray1();
  NCollection_Array1<HLRAlgo_PolyInternalSegment>* PISeg = &myPISeg->ChangeArray1();
  NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>* PINod = &myPINod->ChangeArray1();

  for (i = 1; i <= myNbPINod; i++)
  {
    const occ::handle<HLRAlgo_PolyInternalNode>* pi           = &PINod->ChangeValue(i);
    HLRAlgo_PolyInternalNode::NodeIndices&  aNodIndices1 = (*pi)->Indices();
    HLRAlgo_PolyInternalNode::NodeData&     Nod1RValues  = (*pi)->Data();
    std::cout << "Node " << std::setw(6) << i << " : ";
    std::cout << std::setw(6) << aNodIndices1.NdSg;
    std::cout << std::setw(20) << Nod1RValues.Point.X();
    std::cout << std::setw(20) << Nod1RValues.Point.Y();
    std::cout << std::setw(20) << Nod1RValues.Point.Z();
    std::cout << std::endl;
  }

  for (i = 1; i <= myNbPISeg; i++)
  {
    HLRAlgo_PolyInternalSegment* aSegIndices = &PISeg->ChangeValue(i);
    std::cout << "Segment " << std::setw(6) << i << " : ";
    std::cout << std::setw(6) << aSegIndices->LstSg1;
    std::cout << std::setw(6) << aSegIndices->LstSg2;
    std::cout << std::setw(6) << aSegIndices->NxtSg1;
    std::cout << std::setw(6) << aSegIndices->NxtSg2;
    std::cout << std::setw(6) << aSegIndices->Conex1;
    std::cout << std::setw(6) << aSegIndices->Conex2;
    std::cout << std::endl;
  }

  for (i = 1; i <= myNbTData; i++)
  {
    HLRAlgo_TriangleData& aTriangle = TData->ChangeValue(i);
    std::cout << "Triangle " << std::setw(6) << i << " : ";
    std::cout << std::setw(6) << aTriangle.Node1;
    std::cout << std::setw(6) << aTriangle.Node2;
    std::cout << std::setw(6) << aTriangle.Node3;
    std::cout << std::endl;
  }
}

//=================================================================================================

void HLRAlgo_PolyInternalData::IncTData(NCollection_Array1<HLRAlgo_TriangleData>*& TData1,
                                        NCollection_Array1<HLRAlgo_TriangleData>*& TData2)
{
  if (myNbTData >= myMxTData)
  {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      std::cout << "HLRAlgo_PolyInternalData::IncTData : " << myMxTData << std::endl;
#endif
    int i, j, k;
    j = myMxTData;
    k = 2 * j;

    occ::handle<NCollection_HArray1<HLRAlgo_TriangleData>> NwTData = new NCollection_HArray1<HLRAlgo_TriangleData>(0, k);
    NCollection_Array1<HLRAlgo_TriangleData>&         oTData  = myTData->ChangeArray1();
    NCollection_Array1<HLRAlgo_TriangleData>&         nTData  = NwTData->ChangeArray1();

    for (i = 1; i <= j; i++)
    {
      nTData.ChangeValue(i) = oTData.Value(i);
    }
    myMxTData = k;
    myTData   = NwTData;
    if (TData1 == TData2)
    {
      TData1 = &nTData;
      TData2 = TData1;
    }
    else
    {
      TData1 = &nTData;
    }
  }
  myNbTData++;
}

//=================================================================================================

void HLRAlgo_PolyInternalData::IncPISeg(NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg1,
                                        NCollection_Array1<HLRAlgo_PolyInternalSegment>*& PISeg2)
{
  if (myNbPISeg >= myMxPISeg)
  {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      std::cout << "HLRAlgo_PolyInternalData::IncPISeg : " << myMxPISeg << std::endl;
#endif
    int i, j, k;
    j                                      = myMxPISeg;
    k                                      = 2 * j;
    occ::handle<NCollection_HArray1<HLRAlgo_PolyInternalSegment>> NwPISeg = new NCollection_HArray1<HLRAlgo_PolyInternalSegment>(0, k);
    NCollection_Array1<HLRAlgo_PolyInternalSegment>&         oPISeg  = myPISeg->ChangeArray1();
    NCollection_Array1<HLRAlgo_PolyInternalSegment>&         nPISeg  = NwPISeg->ChangeArray1();

    for (i = 1; i <= j; i++)
    {
      nPISeg.ChangeValue(i) = oPISeg.Value(i);
    }
    myMxPISeg = k;
    myPISeg   = NwPISeg;
    if (PISeg1 == PISeg2)
    {
      PISeg1 = &nPISeg;
      PISeg2 = PISeg1;
    }
    else
    {
      PISeg1 = &nPISeg;
    }
  }
  myNbPISeg++;
}

//=================================================================================================

void HLRAlgo_PolyInternalData::IncPINod(NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& PINod1,
                                        NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>*& PINod2)
{
  if (myNbPINod >= myMxPINod)
  {
#ifdef OCCT_DEBUG
    if (HLRAlgo_PolyInternalData_TRACE)
      std::cout << "HLRAlgo_PolyInternalData::IncPINod : " << myMxPINod << std::endl;
#endif
    int i, j, k;
    j                                         = myMxPINod;
    k                                         = 2 * j;
    occ::handle<NCollection_HArray1<occ::handle<HLRAlgo_PolyInternalNode>>>    NwPINod = new NCollection_HArray1<occ::handle<HLRAlgo_PolyInternalNode>>(0, k);
    NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&            oPINod  = myPINod->ChangeArray1();
    NCollection_Array1<occ::handle<HLRAlgo_PolyInternalNode>>&            nPINod  = NwPINod->ChangeArray1();
    occ::handle<HLRAlgo_PolyInternalNode>* ON      = &(oPINod.ChangeValue(1));
    occ::handle<HLRAlgo_PolyInternalNode>* NN      = &(nPINod.ChangeValue(1));

    for (i = 1; i <= j; i++)
    {
      *NN = *ON;
      ON++;
      NN++;
    }
    myMxPINod = k;
    myPINod   = NwPINod;
    if (PINod1 == PINod2)
    {
      PINod1 = &nPINod;
      PINod2 = PINod1;
    }
    else
    {
      PINod1 = &nPINod;
    }
  }
  myNbPINod++;
}
