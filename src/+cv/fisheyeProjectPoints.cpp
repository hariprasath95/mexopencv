/**
 * @file fisheyeProjectPoints.cpp
 * @brief mex interface for cv::fisheye::projectPoints
 * @ingroup calib3d
 * @author Amro
 * @date 2017
 */
#include "mexopencv.hpp"
#include "opencv2/calib3d.hpp"
using namespace std;
using namespace cv;

/**
 * Main entry called from Matlab
 * @param nlhs number of left-hand-side arguments
 * @param plhs pointers to mxArrays in the left-hand-side
 * @param nrhs number of right-hand-side arguments
 * @param prhs pointers to mxArrays in the right-hand-side
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check the number of arguments
    nargchk(nrhs>=4 && (nrhs%2)==0 && nlhs<=2);

    // Argument vector
    vector<MxArray> rhs(prhs, prhs+nrhs);

    // Option processing
    Mat D;
    double alpha = 0;
    for (int i=4; i<nrhs; i+=2) {
        string key(rhs[i].toString());
        if (key == "DistCoeffs")
            D = rhs[i+1].toMat(CV_64F);
        else if (key == "Alpha")
            alpha = rhs[i+1].toDouble();
        else
            mexErrMsgIdAndTxt("mexopencv:error",
                "Unrecognized option %s", key.c_str());
    }

    // Process
    Mat rvec(rhs[1].toMat(CV_64F)),
        tvec(rhs[2].toMat(CV_64F)),
        K(rhs[3].toMat(CV_64F)),
        jacobian;
    if (rhs[0].isNumeric()) {
        Mat objectPoints(rhs[0].toMat(CV_64F)),
            imagePoints;
        fisheye::projectPoints(objectPoints.reshape(3,0), imagePoints,
            rvec, tvec, K, D, alpha, (nlhs>1 ? jacobian : noArray()));
        if (objectPoints.channels() == 1 && objectPoints.cols == 3)
            imagePoints = imagePoints.reshape(1,0);  // Nx2
        plhs[0] = MxArray(imagePoints);
    }
    else if (rhs[0].isCell()) {
        vector<Point3d> objectPoints(rhs[0].toVector<Point3d>());
        vector<Point2d> imagePoints;
        fisheye::projectPoints(objectPoints, imagePoints,
            rvec, tvec, K, D, alpha, (nlhs>1 ? jacobian : noArray()));
        plhs[0] = MxArray(imagePoints);
    }
    else
        mexErrMsgIdAndTxt("mexopencv:error", "Invalid points argument");
    if (nlhs>1)
        plhs[1] = MxArray(jacobian);
}
