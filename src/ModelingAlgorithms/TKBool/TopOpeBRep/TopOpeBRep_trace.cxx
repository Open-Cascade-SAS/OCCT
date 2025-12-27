// Created on: 1996-01-29
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifdef OCCT_DEBUG

  #include <Standard_Type.hxx>
// #include <Standard_OStream.hxx>
// #include <stdio.h>

static bool TopOpeBRep_traceFITOL = false;

Standard_EXPORT void TopOpeBRep_SettraceFITOL(const bool b)
{
  TopOpeBRep_traceFITOL = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceFITOL()
{
  return TopOpeBRep_traceFITOL;
}

static bool TopOpeBRep_traceFI = false;

Standard_EXPORT void TopOpeBRep_SettraceFI(const bool b)
{
  TopOpeBRep_traceFI = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceFI()
{
  return TopOpeBRep_traceFI;
}

static bool TopOpeBRep_traceSAVFF = false;

Standard_EXPORT void TopOpeBRep_SettraceSAVFF(const bool b)
{
  TopOpeBRep_traceSAVFF = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceSAVFF()
{
  return TopOpeBRep_traceSAVFF;
}

static bool TopOpeBRep_tracePROEDG = false;

Standard_EXPORT void TopOpeBRep_SettracePROEDG(const bool b)
{
  TopOpeBRep_tracePROEDG = b;
}

Standard_EXPORT bool TopOpeBRep_GettracePROEDG()
{
  return TopOpeBRep_tracePROEDG;
}

static bool TopOpeBRep_contextNOPROEDG = false;

Standard_EXPORT void TopOpeBRep_SetcontextNOPROEDG(const bool b)
{
  TopOpeBRep_contextNOPROEDG = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNOPROEDG()
{
  return TopOpeBRep_contextNOPROEDG;
}

static bool TopOpeBRep_contextFFOR = false;

Standard_EXPORT void TopOpeBRep_SetcontextFFOR(const bool b)
{
  TopOpeBRep_contextFFOR = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextFFOR()
{
  return TopOpeBRep_contextFFOR;
}

static bool TopOpeBRep_contextNOPUNK = false;

Standard_EXPORT void TopOpeBRep_SetcontextNOPUNK(const bool b)
{
  TopOpeBRep_contextNOPUNK = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNOPUNK()
{
  return TopOpeBRep_contextNOPUNK;
}

static bool TopOpeBRep_contextNOFEI = false;

Standard_EXPORT void TopOpeBRep_SetcontextNOFEI(const bool b)
{
  TopOpeBRep_contextNOFEI = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNOFEI()
{
  return TopOpeBRep_contextNOFEI;
}

static bool TopOpeBRep_traceSI = false;

Standard_EXPORT void TopOpeBRep_SettraceSI(const bool b)
{
  TopOpeBRep_traceSI = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceSI()
{
  return TopOpeBRep_traceSI;
}

static bool TopOpeBRep_traceBIPS = false;

Standard_EXPORT void TopOpeBRep_SettraceBIPS(const bool b)
{
  TopOpeBRep_traceBIPS = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceBIPS()
{
  return TopOpeBRep_traceBIPS;
}

static bool TopOpeBRep_traceCONIC = false;

Standard_EXPORT void TopOpeBRep_SettraceCONIC(const bool b)
{
  TopOpeBRep_traceCONIC = b;
}

Standard_EXPORT bool TopOpeBRep_GettraceCONIC()
{
  return TopOpeBRep_traceCONIC;
}

static bool TopOpeBRep_contextREST1 = false;

Standard_EXPORT void TopOpeBRep_SetcontextREST1(const bool b)
{
  TopOpeBRep_contextREST1 = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextREST1(char* str)
{
  if (TopOpeBRep_contextREST1)
  {
    std::cout << "context REST1 actif";
    if (str != NULL)
      std::cout << str;
    std::cout << std::endl;
  }
  return TopOpeBRep_contextREST1;
}

Standard_EXPORT bool TopOpeBRep_GetcontextREST1()
{
  if (TopOpeBRep_contextREST1)
    std::cout << "context REST1 actif" << std::endl;
  return TopOpeBRep_contextREST1;
}

static bool TopOpeBRep_contextREST2 = false;

Standard_EXPORT void TopOpeBRep_SetcontextREST2(const bool b)
{
  TopOpeBRep_contextREST2 = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextREST2(char* str)
{
  if (TopOpeBRep_contextREST2)
  {
    std::cout << "context REST2 actif";
    if (str != NULL)
      std::cout << str;
    std::cout << std::endl;
  }
  return TopOpeBRep_contextREST2;
}

Standard_EXPORT bool TopOpeBRep_GetcontextREST2()
{
  if (TopOpeBRep_contextREST2)
    std::cout << "context REST2 actif" << std::endl;
  return TopOpeBRep_contextREST2;
}

static bool TopOpeBRep_contextINL = false;

Standard_EXPORT void TopOpeBRep_SetcontextINL(const bool b)
{
  TopOpeBRep_contextINL = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextINL(char* str)
{
  if (TopOpeBRep_contextINL)
  {
    std::cout << "context INL actif";
    if (str != NULL)
      std::cout << str;
    std::cout << std::endl;
  }
  return TopOpeBRep_contextINL;
}

Standard_EXPORT bool TopOpeBRep_GetcontextINL()
{
  if (TopOpeBRep_contextINL)
    std::cout << "context INL actif" << std::endl;
  return TopOpeBRep_contextINL;
}

static bool TopOpeBRep_contextNEWKP = false;

Standard_EXPORT void TopOpeBRep_SetcontextNEWKP(const bool b)
{
  TopOpeBRep_contextNEWKP = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNEWKP(char* str)
{
  if (TopOpeBRep_contextNEWKP)
  {
    std::cout << "context NEWKP actif";
    if (str != NULL)
      std::cout << str;
    std::cout << std::endl;
  }
  return TopOpeBRep_contextNEWKP;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNEWKP()
{
  if (TopOpeBRep_contextNEWKP)
    std::cout << "context NEWKP actif" << std::endl;
  return TopOpeBRep_contextNEWKP;
}

static bool TopOpeBRep_contextTOL0 = false;

Standard_EXPORT void TopOpeBRep_SetcontextTOL0(const bool b)
{
  TopOpeBRep_contextTOL0 = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextTOL0(char* /*str*/)
{
  if (TopOpeBRep_contextTOL0)
  {
  }
  return TopOpeBRep_contextTOL0;
}

Standard_EXPORT bool TopOpeBRep_GetcontextTOL0()
{
  if (TopOpeBRep_contextTOL0)
    std::cout << "context TOL0 actif" << std::endl;
  return TopOpeBRep_contextTOL0;
}

static bool TopOpeBRep_contextNONOG = false;

Standard_EXPORT void TopOpeBRep_SetcontextNONOG(const bool b)
{
  TopOpeBRep_contextNONOG = b;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNONOG(char* str)
{
  if (TopOpeBRep_contextNONOG)
  {
    std::cout << "context NONOG actif";
    if (str != NULL)
      std::cout << str;
    std::cout << std::endl;
  }
  return TopOpeBRep_contextNONOG;
}

Standard_EXPORT bool TopOpeBRep_GetcontextNONOG()
{
  if (TopOpeBRep_contextNONOG)
    std::cout << "context NONOG actif" << std::endl;
  return TopOpeBRep_contextNONOG;
}

// #ifdef OCCT_DEBUG
#endif
