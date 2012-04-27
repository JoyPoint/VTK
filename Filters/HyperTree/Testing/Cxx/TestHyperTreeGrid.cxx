/*=========================================================================

Copyright (c) Kitware Inc.
All rights reserved.

=========================================================================*/
// .SECTION Thanks
// This test was written by Charles Law and Philippe Pebay, Kitware 2012

#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridAxisCut.h"
#include "vtkHyperTreeGridSource.h"
#include "vtkHyperTreeGridGeometry.h"

#include "vtkCellData.h"
#include "vtkContourFilter.h"
#include "vtkCutter.h"
#include "vtkDataSetWriter.h"
#include "vtkNew.h"
#include "vtkPlane.h"
#include "vtkPolyDataWriter.h"
#include "vtkRenderer.h"
#include "vtkShrinkFilter.h"
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkXMLUnstructuredGridWriter.h"

#include "vtksys/CommandLineArguments.hxx"

int TestHyperTreeGrid( int argc, char* argv[] )
{
  // Default parameters and options
  int dim = 3;
  int branch = 3;
  int max = 3;
  int nX = 3;
  int nY = 4;
  int nZ = 2;
  bool skipAxisCut = false;
  bool skipContour = false;
  bool skipCut = false;
  bool skipGeometry = false;
  bool skipShrink = false;

  // Initialize command line argument parser
  vtksys::CommandLineArguments clArgs;
  clArgs.Initialize( argc, argv );
  clArgs.StoreUnusedArguments( false );

  // Parse command line parameters and options
  clArgs.AddArgument( "--dimension",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &dim, "Dimension of hyper tree grid" );

  clArgs.AddArgument( "--branch-factor",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &branch, "Branching factor of hyper tree grid" );

  clArgs.AddArgument( "--max-level",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &max, "Maximum depth of hyper tree grid" );

  clArgs.AddArgument( "--grid-size-X",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &nX, "Size of hyper tree grid in X direction" );

  clArgs.AddArgument( "--grid-size-Y",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &nY, "Size of hyper tree grid in Y direction" );

  clArgs.AddArgument( "--grid-size-Z",
                      vtksys::CommandLineArguments::SPACE_ARGUMENT,
                      &nZ, "Size of hyper tree grid in Z direction" );

  clArgs.AddArgument( "--skip-Axis-Cut",
                      vtksys::CommandLineArguments::NO_ARGUMENT,
                      &skipAxisCut, "Skip axis cut filter" );

  clArgs.AddArgument( "--skip-Contour",
                      vtksys::CommandLineArguments::NO_ARGUMENT,
                      &skipAxisCut, "Skip contour filter" );

  clArgs.AddArgument( "--skip-Cut",
                      vtksys::CommandLineArguments::NO_ARGUMENT,
                      &skipCut, "Skip cut filter" );

  clArgs.AddArgument( "--skip-Geometry",
                      vtksys::CommandLineArguments::NO_ARGUMENT,
                      &skipGeometry, "Skip geometry filter" );

  clArgs.AddArgument( "--skip-Shrink",
                      vtksys::CommandLineArguments::NO_ARGUMENT,
                      &skipShrink, "Skip shrink filter" );

  // If incorrect arguments were provided, provide some help and terminate in error.
  if ( ! clArgs.Parse() )
    {
    cerr << "Usage: "
         << clArgs.GetHelp()
         << "\n";
    }

  // Ensure that parsed dimensionality makes sense
  if ( dim > 3 )
    {
    dim = 3;
    }
  else if ( dim < 1 )
    {
    dim = 1;
    }
    
  // Ensure that parsed branch factor makes sense
  if ( branch > 3 )
    {
    branch = 3;
    }
  else if ( branch < 2 )
    {
    branch = 2;
    }
    
  // Ensure that parsed maximum level makes sense
  if ( max < 1 )
    {
    max = 1;
    }
    
  // Ensure that parsed grid sizes make sense
  if ( nX < 1 )
    {
    nX = 1;
    }
  if ( nY < 1 )
    {
    nY = 1;
    }
  if ( nZ < 1 )
    {
    nZ = 1;
    }

  // Ensure that parsed grid sizes are consistent with dimensionality
  if ( dim < 3 )
    {
    nZ = 1;
    if ( dim < 2 )
      {
      nY = 1;
      }
    }
 
  // Initialize return value of test
  int testIntValue = 0;

  // Create hyper tree grid source
  vtkNew<vtkHyperTreeGridSource> fractal;
  fractal->SetMaximumLevel( max );
  fractal->DualOn();
  if ( dim == 3 )
    {
    fractal->SetGridSize( nX, nY, nZ );
    }
  fractal->SetGridSize( nX, nY, nZ );
  fractal->SetDimension( dim );
  fractal->SetAxisBranchFactor( branch );
  fractal->Update();
  vtkHyperTreeGrid* htGrid = fractal->GetOutput();

  if ( ! skipGeometry )
    {
    cerr << "# Geometry" << endl;
    vtkNew<vtkHyperTreeGridGeometry> geometry;
    geometry->SetInputConnection( fractal->GetOutputPort() );
    vtkNew<vtkPolyDataWriter> writer4;
    writer4->SetFileName( "./hyperTreeGridGeometry.vtk" );
    writer4->SetInputConnection( geometry->GetOutputPort() );
    writer4->Write();
    }

  if ( ! skipContour )
    {
    cerr << "# Contour" << endl;
    vtkNew<vtkContourFilter> contour;
    contour->SetInputData( htGrid );
    contour->SetNumberOfContours( 2 );
    contour->SetValue( 0, 4. );
    contour->SetValue( 1, 18. );
    contour->SetInputArrayToProcess( 0, 0, 0,
                                     vtkDataObject::FIELD_ASSOCIATION_POINTS,
                                     "Cell Value" );
    vtkNew<vtkPolyDataWriter> writer0;
    writer0->SetFileName( "./hyperTreeGridContour.vtk" );
    writer0->SetInputConnection( contour->GetOutputPort() );
    writer0->Write();
    }

  if ( ! skipShrink )
    {
    cerr << "# Shrink" << endl;
    vtkNew<vtkShrinkFilter> shrink;
    shrink->SetInputData( htGrid );
    shrink->SetShrinkFactor( 1. );
    vtkNew<vtkUnstructuredGridWriter> writer1;
    writer1->SetFileName( "./hyperTreeGridShrink.vtk" );
    writer1->SetInputConnection( shrink->GetOutputPort() );
    writer1->Write();
    }

  if ( ! skipAxisCut )
    {
    // Axis-aligned cut works only in 3D for now
    if ( dim == 3 )
      {
      cerr << "# HyperTreeGridAxisCut" << endl;
      vtkNew<vtkHyperTreeGridAxisCut> axisCut;
      axisCut->SetInputConnection( fractal->GetOutputPort() );
      axisCut->SetPlaneNormalAxis( 2 );
      axisCut->SetPlanePosition( .1 );
      vtkNew<vtkPolyDataWriter> writer2;
      writer2->SetFileName( "./hyperTreeGridAxisCut.vtk" );
      writer2->SetInputConnection( axisCut->GetOutputPort() );
      writer2->Write();
      }
    }

  if ( ! skipCut )
    {
    cerr << "# Cut" << endl;
    vtkNew<vtkCutter> cut;
    vtkNew<vtkPlane> plane;
    plane->SetOrigin( .5, .5, .15 );
    plane->SetNormal( 0, 0, 1 );
    cut->SetInputData( htGrid );
    cut->SetCutFunction( plane.GetPointer() );
    vtkNew<vtkPolyDataWriter> writer3;
    writer3->SetFileName( "./hyperTreeGridCut.vtk" );
    writer3->SetInputConnection( cut->GetOutputPort() );
    writer3->Write();
    }

  return testIntValue;
}
