#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <string.h>

using namespace cv;

// links:
// https://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
// https://google.github.io/styleguide/cppguide.html#Variable_Names
// https://www.pyimagesearch.com/2014/09/01/build-kick-ass-mobile-document-scanner-just-5-minutes/

// Headers / Prototypes
bool CompareContourAreas(const std::vector<Point>& contour1, const std::vector<Point>& contour2);
std::vector<std::vector<Point>> FindRectangeContour(const std::vector<std::vector<Point>>& contours);

int main(int argc, char** argv) {
	// Names of the input and output files
	const char *kImageName = "media/test-receipt.jpg";
	const char *kResultName = "media/test-receipt-edge.jpg";
	RNG rng(12345);
	// Image objects
	Mat original_image, copy_image, result;
	Mat edge_output, contour_drawing;
	std::vector<std::vector<Point>> contours;
	std::vector<std::vector<Point>> approx_contours;
	std::vector<std::vector<Point>> single_contour{ 0, };
	std::vector<Vec4i> hierarchy;

	// Read the image
	original_image = imread(kImageName, 1);

	// This is a check if an image has been loaded properly
	if (!original_image.data) {
		printf("No image data \n");
		return -1;
	}

	// Copying the image
	copy_image = original_image.clone();

	// Gray scaling the image
	cvtColor(copy_image, copy_image, COLOR_BGR2GRAY);
	// Blurring the image using 3,3 kernel
	GaussianBlur(copy_image, copy_image, Size(3, 3), 0);
	// Finding the contrasts of the image
	Canny(copy_image, edge_output, 75, 200);
	// Finding contours
	findContours(edge_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	// Drawing the contours
	contour_drawing = Mat::zeros(edge_output.size(), CV_8UC3);
	//for (auto i = 0; i < contours.size(); ++i) {
	//	Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
	//	// Taken from openCV documentation https://docs.opencv.org/2.4/doc/tutorials/imgproc/shapedescriptors/find_contours/find_contours.html
	//	drawContours(contour_drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
	//}

	// Finding the piece of paper
	std::vector<std::vector<Point>> object_contour = FindRectangeContour(contours);
	single_contour = object_contour;
	drawContours(contour_drawing, single_contour, -1, (0, 255, 0), 2);

	// Opening two windows to display the results
	namedWindow(kImageName, WINDOW_NORMAL);
	namedWindow(kResultName, WINDOW_NORMAL);

	// For testing purposes, assign to current result image
	result = contour_drawing;

	// Saving the image to the disk
	imwrite(kResultName, result);
	// Showing the images
	imshow(kImageName, original_image);
	imshow(kResultName, result);

	waitKey(0);
	return 0;
}

// Comparison function object
bool CompareContourAreas(const std::vector<cv::Point>& contour1, const std::vector<cv::Point>& contour2) {
	double i = fabs(contourArea(cv::Mat(contour1)));
	double j = fabs(contourArea(cv::Mat(contour2)));
	return (i > j);
}

// Find the document in the image
std::vector<std::vector<Point>> FindRectangeContour(const std::vector<std::vector<Point>>& contours) {
	std::vector<std::vector<Point>> approx_contours;
	// Sorting contours by using a custom comp function
	std::sort(contours.begin(), contours.end(), CompareContourAreas);
	
	for (auto& contour : contours) {
		// Approximate the contour
		double arc_length = arcLength(contour, true);
		approxPolyDP(contour, approx_contours, 0.02 * arc_length, true);

		// Checking if it's a rectangle
		if (approx_contours.size() == 4) {
			return approx_contours;
		}
	}
	// In case of a failure, return the biggest one
	return approx_contours;
}