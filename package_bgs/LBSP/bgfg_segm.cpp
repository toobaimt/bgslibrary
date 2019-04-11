
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
#include <sys/stat.h>

typedef std::chrono::high_resolution_clock Clock;

static void help() {
    printf("\nMinimalistic example of foreground-background segmentation in a video sequence using\n"
            "OpenCV's BackgroundSubtractor interface; will analyze frames from the default camera\n"
            "or from a specified file.\n\n"
            "Usage: \n"
            "  ./bgfg_segm [--v]=<process video, true/false>, [--im]=<process images, true/false>, [--file]=<path to folder/video>, [--ROI]=<use ROI, true/false>, [--pR]=<path to ROI bmp file>\n\n");
}

const char* keys = {
    "{v     |false     | process video}"
    "{im    |true    | process images}"
    "{file  |/home/tooba/Downloads/bgslibrary/package_bgs/LBSP/MVI_0797/ip | movie file path  }"
    "{ROI   |false    | use ROI}"
    "{pR    |ROI.bmp  | path to ROI}"
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
////    For Video
    help();
    cv::CommandLineParser parser(argc, argv, keys);
    const bool readImages = parser.get<bool>("im");
    const bool readVideo = parser.get<bool>("v");
    const bool ROI = parser.get<bool>("ROI");
    std::string sVideoFilePath = parser.get<std::string>("file");

    cv::Mat oCurrInputFrame, oCurrSegmMask, oCurrReconstrBGImg;
    std::vector<std::string> f;
    cv::VideoCapture oVideoInput;

    if(readImages){
        ////    For Images
        f = open(sVideoFilePath); // or pass which dir to open
        std::sort(f.begin(), f.end());
        oCurrInputFrame = cv::imread((sVideoFilePath + "/" + f[2]).c_str(), cv::IMREAD_COLOR);
    }
    else if(readVideo){
        ////    For Video
        oVideoInput.open(sVideoFilePath);
        sVideoFilePath = sVideoFilePath.substr(0,sVideoFilePath.length()-4);
        oVideoInput >> oCurrInputFrame;
        oVideoInput.set(cv::CAP_PROP_POS_FRAMES,0);
        std::string vidPath = sVideoFilePath;
        mkdir(vidPath.c_str(), ACCESSPERMS);
        std::string ipPath = sVideoFilePath+"/ip";
        mkdir(ipPath.c_str(), ACCESSPERMS);
    }

    if(oCurrInputFrame.empty()) {
        printf("Could not open video file at '%s'.\n",sVideoFilePath.c_str());
        return -1;
    }

////   For both images and video
    oCurrSegmMask.create(oCurrInputFrame.size(),CV_8UC1);
    oCurrReconstrBGImg.create(oCurrInputFrame.size(),oCurrInputFrame.type());

////  For optimal results, pass a constrained ROI to the algorithm (ex: for CDnet, use ROI.bmp)

    cv::Mat oSequenceROI(oCurrInputFrame.size(),CV_8UC1,cv::Scalar_<uchar>(255));
    if(ROI)
        oSequenceROI = cv::imread("ROI.bmp", cv::IMREAD_COLOR);

    BackgroundSubtractorPAWCS oBGSAlg;
    oBGSAlg.initialize(oCurrInputFrame,oSequenceROI);

    std::string bgPath = sVideoFilePath+"/bg";
    std::string segPath = sVideoFilePath+"/segm";
    mkdir(bgPath.c_str(), ACCESSPERMS);
    mkdir(segPath.c_str(), ACCESSPERMS);

    if(readImages){
        for(int k=2; k<f.size(); k++) {
        ////    For Images
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(6) << k-2;
            auto t1 = Clock::now();
            oCurrInputFrame = cv::imread((sVideoFilePath + "/" + f[k]).c_str(), cv::IMREAD_COLOR);

            //Common
            if(oCurrInputFrame.empty())
                break;
            oBGSAlg.apply(oCurrInputFrame, oCurrSegmMask, double(k<=100)); // lower rate in the early frames helps bootstrap the model when foreground is present
            oBGSAlg.getBackgroundImage(oCurrReconstrBGImg);
            auto t2 = Clock::now();
            //imwrite("..ip/ip-"+oss.str()+".jpg",oCurrInputFrame);
            imwrite(sVideoFilePath+"/segm/mask-"+oss.str()+".jpg",oCurrSegmMask);
            imwrite(sVideoFilePath+"/bg/bg-"+oss.str()+".jpg",oCurrReconstrBGImg);
            printf("\nFrame %6d processed in %ld ms", k, std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() );
            if(cv::waitKey(1)==27)
                break;
        }
    }
    else if(readVideo) {
        for (int k = 0; ; k++) {
            ////    For video
            oVideoInput >> oCurrInputFrame;
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(6) << k;
            auto t1 = Clock::now();

            //Common
            if (oCurrInputFrame.empty())
                break;
            oBGSAlg.apply(oCurrInputFrame, oCurrSegmMask, double(k <=
                                                                 100)); // lower rate in the early frames helps bootstrap the model when foreground is present
            oBGSAlg.getBackgroundImage(oCurrReconstrBGImg);
            auto t2 = Clock::now();
            imwrite(sVideoFilePath+"/ip/ip-"+oss.str()+".jpg",oCurrInputFrame);
            imwrite(sVideoFilePath+"/segm/mask-" + oss.str() + ".jpg", oCurrSegmMask);
            imwrite(sVideoFilePath+"/bg/bg-" + oss.str() + ".jpg", oCurrReconstrBGImg);
            printf("\nFrame %6d processed in %ld ms", k,
                   std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
            if (cv::waitKey(1) == 27)
                break;
        }
    }

    return 0;
}

