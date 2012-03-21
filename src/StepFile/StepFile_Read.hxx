// Created on: 1991-08-30
// Created by: Christian CAILLET
// Copyright (c) 1991-1999 Matra Datavision
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


//   StepFile_Read.hxx

//   routine assurant l'enchainement des operations de lecture d'un fichier
//   STEP dans un StepModel, en fonction d'une cle de reconnaissance
//   Retour de la fonction :
//     0 si OK  (le StepModel a ete charge)
//    -1 si abandon car fichier pas pu etre ouvert
//     1 si erreur en cours de lecture


#ifndef StepFile_Read_HeaderFile
#define StepFile_Read_HeaderFile

//# include <stepread.h>  : sauf recfile_modeprint, declare ici
# include <StepData_StepModel.hxx>
# include <StepData_FileRecognizer.hxx>
# include <StepData_Protocol.hxx>


Standard_EXPORT void StepFile_ReadTrace (const Standard_Integer mode);
// Modal : 0 pas de trace, 1 trace LoadModel, 2 & 3 + trace interne lex-yac

Standard_EXPORT Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_FileRecognizer)& recoheader,   // Reconnait le Header
  const Handle(StepData_FileRecognizer)& recodata);    // Entites du Data

Standard_EXPORT Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_FileRecognizer)& recoheader,   // Reconnait le Header
  const Handle(StepData_Protocol)& protocol);          // Entites du Data

Standard_EXPORT Standard_Integer StepFile_Read
 (char* nomfic,
  const Handle(StepData_StepModel)& stepmodel,
  const Handle(StepData_Protocol)& protocol);          // Header & Data

#endif
