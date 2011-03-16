// File:	StepFile_Read.hxx
// Created:	Fri Aug 30 11:31:31 1991
// Author:	Christian CAILLET
//		<cky@phobox>

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
