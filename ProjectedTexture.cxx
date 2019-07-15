#include <vtkSmartPointer.h>

#include <vtkProjectedTexture.h>
#include <vtkCamera.h>
#include <vtkFrustumSource.h>
#include <vtkPlanes.h>
#include <vtkMath.h>


#include <vtkWindowToImageFilter.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkTexture.h>
#include <vtkPointData.h>
#include <vtkImageData.h>


#include <vtkPNGWriter.h>
#include <vtkBYUReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>
#include <vtkSTLReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkSphereSource.h>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>

#include <vtkNamedColors.h>

#include <vtksys/SystemTools.hxx>

namespace
{
vtkSmartPointer<vtkPolyData> ReadPolyData(const char *fileName);
}

int main (int argc, char *argv[])
{
  if (argc < 4)
  {
    std::cout << "Usage: " << argv[0] << " polydata.plt texture.jpg output.png" << std::endl;
    return EXIT_FAILURE;
  }

  /* Read ply file */
  vtkSmartPointer<vtkPolyData> polyData = ReadPolyData(argc > 1 ? argv[1] : "");;

  /* Read texture file */
  vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
  vtkSmartPointer<vtkImageReader2> textureFile;
  textureFile.TakeReference( readerFactory->CreateImageReader2(argv[2]));
  textureFile->SetFileName(argv[2]);
  textureFile->Update();

  vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

  /* define camera */
  vtkSmartPointer<vtkCamera> camera =   vtkSmartPointer<vtkCamera>::New();
  //camera->Dolly(1.4);
 // camera->SetFocalPoint(0.0, 0.0, 0.0);
  camera->Azimuth(30);
 // camera->Elevation(15);
  //camera->SetClippingRange(.5, .6);


  vtkSmartPointer<vtkProjectedTexture>  projectedTexture = vtkSmartPointer<vtkProjectedTexture>::New();

  projectedTexture->SetInputData( polyData); 
  projectedTexture->Update();

  vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();
  texture->SetInputData(textureFile->GetOutput());

  vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(polyData);

  /* create actor to which ply and texture can be shown */
  vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->SetTexture(texture);

  vtkSmartPointer<vtkRenderer> renderer =  vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderWindow> renderWindow =  vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->SetOffScreenRendering(1);
  renderer->AddActor(actor);
//  renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());
  renderer->SetActiveCamera(camera);
  renderer->ResetCameraClippingRange();
  renderWindow->AddRenderer(renderer);

  /* set background to white */
  renderer->SetBackground(1, 1, 1);

  renderer->ResetCameraClippingRange();
  float magnification = 1.0;
  float size = 300*magnification;
  renderWindow->SetSize(size, size);
  renderWindow->Render();


  /* send renderWinsow to window to image filter */
  vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
  windowToImageFilter->SetInput(renderWindow);
  windowToImageFilter->Update();

  /*write the png file */
  std::string outputImageName = argv[3];
  vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
  writer->SetFileName(outputImageName.c_str());
  writer->SetInputConnection(windowToImageFilter->GetOutputPort());
  writer->Write();

  return EXIT_SUCCESS;
}

namespace
{
vtkSmartPointer<vtkPolyData> ReadPolyData(const char *fileName)
{
  vtkSmartPointer<vtkPolyData> polyData;
  std::string extension = vtksys::SystemTools::GetFilenameExtension(std::string(fileName));
  if (extension == ".ply")
  {
    vtkSmartPointer<vtkPLYReader> reader =
      vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtp")
  {
    vtkSmartPointer<vtkXMLPolyDataReader> reader =
      vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".obj")
  {
    vtkSmartPointer<vtkOBJReader> reader =
      vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".stl")
  {
    vtkSmartPointer<vtkSTLReader> reader =
      vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".vtk")
  {
    vtkSmartPointer<vtkPolyDataReader> reader =
      vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else if (extension == ".g")
  {
    vtkSmartPointer<vtkBYUReader> reader =
      vtkSmartPointer<vtkBYUReader>::New();
    reader->SetGeometryFileName (fileName);
    reader->Update();
    polyData = reader->GetOutput();
  }
  else
  {
    vtkSmartPointer<vtkSphereSource> source =
      vtkSmartPointer<vtkSphereSource>::New();
    source->Update();
    polyData = source->GetOutput();
  }
  return polyData;
}
}
