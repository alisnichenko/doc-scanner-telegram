#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <string.h>

using namespace cv;

// Image class declaration
// Lookup the special constructor
// class Image {
// public:
// 	std::string Name() : const {};
// 	~Image();
// };

// links:
// https://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
// https://google.github.io/styleguide/cppguide.html#Variable_Names
// https://www.pyimagesearch.com/2014/09/01/build-kick-ass-mobile-document-scanner-just-5-minutes/

// Prototypes
bool CompareContourAreas(const std::vector<Point> &contour1, const std::vector<Point> &contour2);

int main(int argc, char** argv) {
	// Names of the input and output files
	const char *kImageName = "media/test-receipt.jpg";
	const char *kResultName = "media/test-receipt-edge.jpg";

	// Image objects
	Mat gray_image, original_image, image, result;
	Mat contour_image, contour_output;
	// Contour image full of zeros, basically a matrix with the same size as the source image
	Mat contour_image_draw = Mat::zeros(original_image.rows, original_image.cols, CV_8UC3);

	// Vector to store arrays of arrays of contours
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	// Read an image specified in the beginning
	// The image should be located in the same dir w/ the program
	original_image = imread(kImageName, 1);

	// This is a check if an image has been loaded properly
	if (!original_image.data) {
		printf("No image data \n");
		return -1;
	}

	// Copying the image
	image = original_image.clone();
	cvtColor(image, gray_image, COLOR_BGR2GRAY);

	// Blurring the image using 3,3 kernel
	GaussianBlur(gray_image, gray_image, Size(3, 3), 0);
	// Finding the contours of the image
	Canny(gray_image, contour_output, 75, 200);
	// Actually finding the contours now
	findContours(gray_image, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	// Sort the contours using additional comparison function
	std::sort(contours.begin(), contours.end(), CompareContourAreas);
	// Loop over contours and draw the contours with random colors
	
	// I don't know how the loop works though. I just took it from the docs
	/*for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
		Scalar color(rand()&255, rand()&255, rand()&255);
		drawContours(contour_image_draw, contours, idx, color, FILLED, 8, hierarchy);
	}*/
	// Canny(gray_image, contours, 75, 200);
	// Actually finding the contours now
	// findContours(contours, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	// Writing to disk a new gray image
	imwrite(kResultName, gray_image);

	// Opening two windows to display the results
	namedWindow(kImageName, WINDOW_NORMAL);
	namedWindow(kResultName, WINDOW_NORMAL);

	// For testing purposes, assign to current result image
	result = contour_output;

	// Showing the images
	imshow(kImageName, image);
	imshow(kResultName, result);

	waitKey(0);
	return 0;
}

// Comparison function object using passed by references arrays of contours
bool CompareContourAreas(const std::vector<Point> &contour1, const std::vector<Point> &contour2) {
	double i = fabs(contourArea(cv::Mat(contour1)));
	double j = fabs(contourArea(cv::Mat(contour2)));
	return (i < j);
}