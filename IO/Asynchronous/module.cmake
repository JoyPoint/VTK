vtk_module(vtkIOAsynchronous
  GROUPS
    StandAlone
  TEST_DEPENDS
    vtkTestingCore
  KIT
    vtkIO
  DEPENDS
    vtkCommonCore
    vtkCommonExecutionModel
    vtkIOCore
    vtkIOImage
    vtkIOXML
  PRIVATE_DEPENDS
    vtkCommonDataModel
    vtkCommonMath
    vtkCommonMisc
    vtkCommonSystem
    vtkjpeg
    vtkpng
    vtksys
    vtktiff
    vtkzlib
  )
