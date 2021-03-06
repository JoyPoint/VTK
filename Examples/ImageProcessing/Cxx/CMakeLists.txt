cmake_minimum_required(VERSION 3.3...3.12 FATAL_ERROR)


PROJECT(ImageProcessing)
find_package(VTK REQUIRED)
vtk_module_config(VTK
  vtkCommonCore
  vtkFiltersCore
  vtkFiltersModeling
  vtkFiltersSources
  vtkIOImage
  vtkImagingCore
  vtkImagingGeneral
  vtkImagingStatistics
  vtkImagingStencil
  vtkInteractionStyle
  vtkRenderingOpenGL2
  vtkRenderingCore
  vtkTestingCore
  vtkTestingRendering
)
include(${VTK_USE_FILE})

add_executable(ImageSlicing MACOSX_BUNDLE ImageSlicing.cxx)
target_link_libraries(ImageSlicing ${VTK_LIBRARIES})

add_executable(ImageBenchmark ImageBenchmark.cxx)
target_link_libraries(ImageBenchmark ${VTK_LIBRARIES})

add_executable(ImageBenchmarkDriver ImageBenchmarkDriver.cxx)
target_link_libraries(ImageBenchmarkDriver ${VTK_LIBRARIES})

if(BUILD_TESTING)
  if(vtkTestingRendering_LOADED)
   ######## Regression Testing ########
   set(vtk-example ImageProcessingExamples)
   set(TestImageSlicing_ARGS ${VTK_TEST_DATA_DIR}/Data/headsq/quarter)
   set(TestImageBenchmark_ARGS
       "--threads" "2" "--filter" "resize:kernelsize=4")
   vtk_add_test_cxx(${vtk-example}CxxTests tests
     TestImageBenchmark.cxx,NO_VALID,NO_DATA,NO_OUTPUT
     TestImageSlicing.cxx
     )
   vtk_test_cxx_executable(${vtk-example}CxxTests tests
     RENDERING_FACTORY
     )
  endif()
endif()
