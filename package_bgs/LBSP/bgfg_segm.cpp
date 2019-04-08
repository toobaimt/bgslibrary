
// minimalistic foreground-background segmentation sample, based off OpenCV's bgfg_segm sample

#include "BackgroundSubtractorPAWCS.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <dirent.h>

typedef std::chrono::high_resolution_clock Clock;

static void help() {
    printf("\nMinimalistic example of foreground-background segmentation in a video sequence using\n"
            "OpenCV's BackgroundSubtractor interface; will analyze frames from the default camera\n"
            "or from a specified file.\n\n"
            "Usage: \n"
            "  ./bgfg_segm [--camera]=<use camera, true/false>, [--file]=<path to file> \n\n");
}

const char* keys = {
    "{c  |camera   |true     | use camera or not}"
    "{f  |file     |tree.avi | movie file path  }"
};

std::vector<std::string> open(std::string path = ".") {
    DIR*    dir;
    dirent* pdir;
    std::vector<std::string> files;
    dir = opendir(path.c_str());
    while (pdir = readdir(dir)) {
        files.push_back(pdir->d_name);
    }
    return files;
}

int main(int argc, const char** argv) {
    // For Video
    // help();
    // cv::CommandLineParser parser(argc, argv, keys);
    // const bool bUseDefaultCamera = parser.get<bool>("camera");
    // const std::string sVideoFilePath = parser.get<std::string>("file");
    // const bool bUseDefaultCamera = 0;
    // const std::string sVideoFilePath = "/home/tooba/Downloads/pawcs/MVI_0797.MOV;
    // cv::VideoCapture oVideoInput;

/*    if(bUseDefaultCamera) {
        oVideoInput.open(0);
        oVideoInput >> oCurrInputFrame;
    }
    else {
        oVideoInput.open(sVideoFilePath);
        oVideoInput >> oCurrInputFrame;
        oVideoInput.set(cv::CAP_PROP_POS_FRAMES,0);
    }
    parser.printMessage();
    if(!oVideoInput.isOpened() || oCurrInputFrame.empty()) {
        if(bUseDefaultCamera)
            printf("Could not open default camera.\n");
        else
            printf("Could not open video file at '%s'.\n",sVideoFilePath.c_str());
        return -1;
    }*/

////    For Images
    const std::string sFilePath = "/home/tooba/Downloads/pawcs/Surv_Cam_Cmpr_5pm_W8";
    std::vector<std::string> f;
    f = open(sFilePath); // or pass which dir to open
    std::sort(f.begin(), f.end());

    cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
    oCurrInputFrame = cv::imread((sFilePath + "/" + f[2]).c_str(), cv::IMREAD_COLOR);

////   For both images and video

    oCurrSegmMask.create(oCurrInputFrame.size(),CV_8UC1);
    oCurrReconstrBGImg.create(oCurrInputFrame.size(),oCurrInputFrame.type());

////  For optimal results, pass a constrained ROI to the algorithm (ex: for CDnet, use ROI.bmp)
//    cv::Mat oSequenceROI = cv::imread("ROI.bmp", cv::IMREAD_COLOR);
    cv::Mat oSequenceROI(oCurrInputFrame.size(),CV_8UC1,cv::Scalar_<uchar>(255));
    BackgroundSubtractorPAWCS oBGSAlg;
    oBGSAlg.initialize(oCurrInputFrame,oSequenceROI);

    for(int k=2; k<f.size(); k++) {
////    For video
//        oVideoInput >> oCurrInputFrame;

////    For Images
        std::ostringstream oss;
        oss << std::setfill('0') << std::setw(6) << k-2;
        auto t1 = Clock::now();
        oCurrInputFrame = cv::imread((sFilePath + "/" + f[k]).c_str(), cv::IMREAD_COLOR);

        //Common
        if(oCurrInputFrame.empty())
            break;
        oBGSAlg.apply(oCurrInputFrame, oCurrSegmMask, double(k<=100)); // lower rate in the early frames helps bootstrap the model when foreground is present
        oBGSAlg.getBackgroundImage(oCurrReconstrBGImg);
        auto t2 = Clock::now();
//        imwrite("ip/ip-"+oss.str()+".jpg",oCurrInputFrame);
        imwrite("../segm/mask-"+oss.str()+".jpg",oCurrSegmMask);
        imwrite("../bg/bg-"+oss.str()+".jpg",oCurrReconstrBGImg);
        printf("\nFrame %6d processed in %ld ms", k, std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() );
        if(cv::waitKey(1)==27)
            break;
    }
    return 0;
}

