# Source files for Storage package
set(OCCT_Storage_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_Storage_FILES
  Storage.cxx
  Storage.hxx

  Storage_ArrayOfSchema.hxx
  Storage_BaseDriver.cxx
  Storage_BaseDriver.hxx
  Storage_BucketOfPersistent.hxx
  Storage_CallBack.cxx
  Storage_CallBack.hxx
  Storage_Data.cxx
  Storage_Data.hxx

  Storage_DefaultCallBack.cxx
  Storage_DefaultCallBack.hxx
  Storage_Error.hxx

  Storage_HeaderData.cxx
  Storage_HeaderData.hxx

  Storage_InternalData.cxx
  Storage_InternalData.hxx
  Storage_Macros.hxx

  Storage_OpenMode.hxx

  Storage_Position.hxx

  Storage_Root.cxx
  Storage_Root.hxx
  Storage_RootData.cxx
  Storage_RootData.hxx
  Storage_Schema.cxx
  Storage_Schema.hxx

  Storage_SolveMode.hxx
  Storage_StreamExtCharParityError.hxx
  Storage_StreamFormatError.hxx
  Storage_StreamModeError.hxx
  Storage_StreamReadError.hxx
  Storage_StreamTypeMismatchError.hxx
  Storage_StreamUnknownTypeError.hxx
  Storage_StreamWriteError.hxx
  Storage_TypeData.cxx
  Storage_TypeData.hxx
  Storage_TypedCallBack.cxx
  Storage_TypedCallBack.hxx
)
