!!!!!!!!!! ----- Messages file for TObj packages --------- !!!!!!!!!
!  Syntax for keywords: NameOfPackage_NameOfClass_NameOfMessage
!                 where NameOfClass is optional or abbreviated
!!!!!!!!!! --------------------------------------------------- !!!!!!!!!

.TObj_DL_VoidItemId
  Void ID attribute in XML file when loading the library %s

.TObj_DL_ZeroId
  Non-positive or irrelevant (%d) ID attribute in XML file when loading the library %s

.TObj_DL_MissItemData
  Missing LibraryData element with ID %d in XML file when loading the library %s

.TObj_DL_NoLibName
  Cannot find the library "%s" with the type "%s"

.TObj_DL_TraceReadLib
  Loading the library %s / %s, version %s, date %s

.TObj_DL_NoDocument
  The document %s does not contain data libraries

.TObj_DL_AlrReadLib
  The library "%s" for data type "%s" has already been loaded

.TObj_DL_AlrAddedReader
  TObj_DataLibrary::AddReader: Warning: this Reader already added to a different DataLibrary

.TObj_DL_InvalidLibHeader
  Invalid library header encountered in file %s

.TObj_DL_NotSupported
  The data type "%s" not supported

.TObj_DL_EmptyDL
  DataLibrary is empty, the file is not created

.TObj_DL_CannotCreateFile
  Can't create the file for data library

.TObj_DL_ErrorClosingFile
  Error closing the file for data library

.TObj_DL_DoubledTag
  Duplicate tag %d found for element "%s" in library "%s", type "%s"

!!! ---------------- TObj_Application ----------------

.TObj_Appl_SDriverFailure
  Error saving document %s : driver failure. Check presence of resource files.

.TObj_Appl_SWriteFailure
  Error saving document %s : write failure
  
.TObj_Appl_SFailure
  Error saving document %s : general failure of persistence driver
  
.TObj_Appl_SDocIsNull
  Error saving document %s : No document to save
  
.TObj_Appl_SNoObj
 Error saving document %s : No objects written
 
.TObj_Appl_SInfoSectionError
 Error saving document %s : Write info section failure

.TObj_Appl_SUnknownFailure
  Error saving document %s : unknown failure

.TObj_Appl_RUnknownDocument
  Error loading document %s : unknown document

.TObj_Appl_RAlreadyRetrieved
  Error loading document %s : already retrieved

.TObj_Appl_RAlreadyRetrievedAndModified
  Error loading document %s : already retrieved and modified

.TObj_Appl_RNoDriver
  Error loading document %s : no appropriate driver was found

.TObj_Appl_ROpenError
  Error loading document %s : cannot open file

.TObj_Appl_RNoVersion
  Error loading document %s : no version

.TObj_Appl_RNoModel
  Error loading document %s : no model

.TObj_Appl_RNoDocument
  Error loading document %s : no document

.TObj_Appl_RFormatFailure
  Error loading document %s : format failure

.TObj_Appl_RTypeNotFound
  Error loading document %s : type not found in schema

.TObj_Appl_RBadFileFormat
  Error loading document %s : unrecognized file format

.TObj_Appl_RMakeFailure
  Error loading document %s : failure making document

.TObj_Appl_RPermissionDenied
  Error loading document %s : permission denied

.TObj_Appl_RDriverFailure
  Error loading document %s : driver failure

.TObj_Appl_RUnknownFail
  Error loading document %s : unknown failure

.TObj_Appl_RException
  Error loading document %s : the file is probably corrupted

.TObj_Appl_Exception
  An exception was caught: %s

!!! ---------------- TObj_Checker ----------------

.TObj_Ch_RefToNullObj
  Referencing to NULL object: %s

.TObj_Ch_BackRefToNullObj
  Back Referencing to NULL object: %s

.TObj_Ch_BackRefToDelObj
  Back Referencing to deleted object: %s

.TObj_Ch_BackRefError
  Back Reference Error: %s

.TObj_Ch_RefWithoutBack
  Reference without back one: %s

.TObj_Ch_BackRefWithoutDirect
  Back Reference without direct one: %s from: %s

.TObj_Ch_NoAttr
  Has no attribute %s at label

.TObj_Ch_NotOcafObj
  Object is not OCAF Object: %s

.TObj_Ch_ObjWithoutName
  Object without name: %s entry %s

.TObj_Ch_NameNotUnique
  Name: %s is not unique in the model %s

.TObj_Ch_NameNotRegistered
  Name: %s is not registered in names map %s

.TObj_Ch_RegistrationError
  Registration Error: name %s, target entry %s, entry %s

.TObj_Ch_ExtraName
  Map of model contains the extra name : %s

.TObj_M_LoadDocument
  +++++ Load document %s

.TObj_M_SaveDocument
  +++++ Save document %s

.TObj_M_TimeLoading
  +++++ Fin loading                : 

.TObj_M_TimeInit
  +++++ Fin initializing new model : 

.TObj_M_TimeSaving
  +++++ Fin saving : 

.TObj_M_WrongFile
  File %s has wrong content

.TObj_M_NoWriteAccess
  Cannot write the file %s, check permissions

.TObj_Any_FailCreateDir
  Error creating the directory %s

.TObj_Any_NoFormatVersion
  File %s has no format version, probably it is obsolete

.TObj_Any_WrongFormatVersion
  File %s has unsupported format version; model is discarded

.TObj_Any_OldFormatVersion
  Warning: Model version is updated; it will not be readable by previous versions of application
