/**
 * This program takes a set of corresponding 2D and 3D points and finds the transformation matrix
 * that best brings the 3D points to their corresponding 2D points.
 */
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace cv;

vector<Point2f> Generate2DPoints(vector<vector<float>> vec);
vector<Point3f> Generate3DPoints(vector<vector<float>> vec);

int main(int argc, char* argv[])
{
    // Read points

    vector<vector<float>> markers_screen;
    markers_screen = {{0, 34.0, 56.0}, {2, 9.0, 57.0}, {3, 29.0, 67.0}, {4, 30.0, 21.0}};

    vector<vector<float>> markers_floor;
    markers_floor = {{0, 56.0, 23.0}, {2, 37.0, 47.0}, {3, 13.0, 32.0}, {4, 34, 46}};


    vector<vector<float>> targets_screen;
    targets_screen = {{0, 1.0, 2.0}, {2, 1.0, 2.0}, {3, 1.0, 2.0}};

    vector<vector<float>> targets_floor;

    vector<vector<float>> robot_screen;
    robot_screen = {{0, 1.0, 2.0}};

    vector<vector<float>> robot_floor;


    vector<Point2f> imagePoints = Generate2DPoints(markers_screen);
    vector<Point3f> objectPoints = Generate3DPoints(markers_floor);

    cout << "There are " << imagePoints.size() << " imagePoints and " << objectPoints.size() << " objectPoints." << endl;
    Mat cameraMatrix(3, 3, DataType<double>::type);
    setIdentity(cameraMatrix);

    cout << "Initial cameraMatrix:\n " << cameraMatrix << endl;

    Mat distCoeffs(4, 1, DataType<double>::type);
    distCoeffs.at<double>(0) = 0;
    distCoeffs.at<double>(1) = 0;
    distCoeffs.at<double>(2) = 0;
    distCoeffs.at<double>(3) = 0;

    Mat rvec(3, 1, DataType<double>::type);
    Mat tvec(3, 1, DataType<double>::type);

    vector<Point2f> projectedPoints;
    vector<Point2f> currentProjectedPoints;

    for (int count = 1; count <= imagePoints.size(); count++) {


        solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec, tvec, false, SOLVEPNP_P3P);

        cout << "rvec:\n " << rvec << std::endl;
        cout << "tvec:\n " << tvec << std::endl;

        projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, currentProjectedPoints);
        
        if (count > 2 and count < imagePoints.size()) {
            projectedPoints[count] = currentProjectedPoints[0];
        }
        else {
            projectedPoints = currentProjectedPoints;
        }

        count += 2;
        if(count<imagePoints.size())
        imagePoints[0] = Point2f(imagePoints[count]);
    }

    for (unsigned int i = 0; i < projectedPoints.size(); ++i)
    {
        std::cout << "Image point: " << imagePoints[i] << " Projected to " << projectedPoints[i] << endl;
    }

    return 0;
}


vector<Point2f> Generate2DPoints(vector<vector<float>> vec)
{
    vector<Point2f> points;

    for (vector<float> interVec : vec) {
        
        points.push_back(Point2f(interVec[1], interVec[2]));
    }
    
    if (vec.size() < 3 and vec.size()>0) {
      vector<float> backVec = vec.back();
      int iter = 3 - vec.size();
      for (int i = 0; i < iter; ++i) {
          points.push_back(Point2f(backVec[1], backVec[2]));
      }
    }

    

    for (unsigned int i = 0; i < points.size(); ++i)
    {
        std::cout << points[i] << endl;
    }
    cout << endl;
    return points;
}


vector<Point3f> Generate3DPoints(vector<vector<float>> vec)
{
    vector<Point3f> points;

    for (vector<float> interVec : vec) {

        points.push_back(Point3f(interVec[1], interVec[2],0));
    }

    if (vec.size() < 3 and vec.size() > 0) {
        vector<float> backVec = vec.back();
        int iter = 3 - vec.size();
        for (int i = 0; i < iter; ++i) {
            points.push_back(Point3f(backVec[1], backVec[2], 0));
        }
    }

    for (unsigned int i = 0; i < points.size(); ++i)
    {
        std::cout << points[i] << endl;
    }
    cout << endl;
    return points;
}