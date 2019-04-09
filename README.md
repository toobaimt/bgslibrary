## BGSLibrary
A Background Subtraction Library


Requirements
---------

    OpenCV (tested with 3.4.5)


Compiling
---------

    cd package_bgs/LBSP
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make
    ./bgfg_segm


Usage
---------

    ./bgfg_segm [--v]=<process video, true/false>, [--im]=<process images, true/false>, [--file]=<path to folder/video>, [--ROI]=<use ROI, true/false>, [--pR]=<path to ROI bmp file>
