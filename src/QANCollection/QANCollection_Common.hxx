// File:        QANCollection_Common.hxx
// Created:     Wed May 15 12:39:54 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>


#ifndef QANCollection_Common_HeaderFile
#define QANCollection_Common_HeaderFile

#include <gp_Pnt.hxx>

// ===================== Methods for accessing items/keys =====================

// To print other type of items define PrintItem for it

Standard_EXPORT void PrintItem(const gp_Pnt&       thePnt);
Standard_EXPORT void PrintItem(const Standard_Real theDbl);

// So do for the pseudo-random generation

Standard_EXPORT void Random (Standard_Real& theValue);
Standard_EXPORT void Random (Standard_Integer& theValue,
                             const Standard_Integer theMax=RAND_MAX);
Standard_EXPORT void Random (gp_Pnt& thePnt);

#endif
