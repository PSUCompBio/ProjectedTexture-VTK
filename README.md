# ProjectedTexture-VTK
VTK project that puts Avatar 3D SDK textures onto Avatar 3D SDK ply files and outputs png file (all from command line).

## To build
cd ProjectedTexture-VTK
mkdir build 
cd build 
cmake .. -DVTK_DIR:PATH=/path/to/vtk_build
make -j 8

## To test (on linux)
xvfb-run ./ProjectedTexture ../model.ply ../model.jpg  test.png
