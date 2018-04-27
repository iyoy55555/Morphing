#include <opencv2/opencv.hpp>
#include <utility>

using namespace std;
using namespace cv;

void mouseCallback1(int event, int x, int y, int flags, void *param);
void mouseCallback2(int event, int x, int y, int flags, void *param);

vector<pair<Point, Point>> pic1_lines;
vector<pair<Point, Point>> pic2_lines;
vector<pair<Point, Point>> result_lines;
Mat morphing(Mat img1, Mat img2, float t);
Point Transform(Point p,pair<Point,Point> L1, pair<Point,Point> L2);

Point p1;
Point p2;
Point p3;
Point p4;

int main() {
	Mat img1 = imread("cheetah.jpg");
	Mat img2 = imread("women.jpg");
	Mat backup1 = imread("cheetah.jpg");
	Mat backup2 = imread("women.jpg");
	namedWindow("picture1");
	namedWindow("picture2");
	setMouseCallback("picture1", mouseCallback1);
	setMouseCallback("picture2", mouseCallback2);


	while (true) {
		if (p1.x == -1 && p2.x == -1) {
			imshow("picture1", img1);
		}
		if (p3.x == -1 && p4.x == -1) {
			imshow("picture2", img2);
		}
		if (p1.x != -1 && p2.x != -1) {
			line(img1, p1, p2, Scalar(255, 0, 0), 1);
			p1.x = -1;
			p1.y = -1;
			p2.x = -1;
			p2.y = -1;
			imshow("picture1", img1);
		}
		if (p3.x != -1 && p4.x != -1) {
			line(img2, p3, p4, Scalar(255, 0, 0), 1);
			p3.x = -1;
			p3.y = -1;
			p4.x = -1;
			p4.y = -1;
			imshow("picture2", img2);
		}
		if (cvWaitKey(33) == 27) {		//esc
			return 0;
		}
		if (cvWaitKey(33) == 13) {		//enter
			break;
		}
	}
	Mat result = morphing(backup1, backup2, 0.5);
	imshow("result", result);
	waitKey();
	return 0;
}

void mouseCallback1(int event, int x, int y, int flags, void *param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		cout << x << " " << y << endl;
		p1.x = x;
		p1.y = y;
		pair<Point, Point> line;
		line.first = Point(x, y);
		pic1_lines.push_back(line);
	}
	if (event == CV_EVENT_LBUTTONUP) {
		p2.x = x;
		p2.y = y;
		pic1_lines.back().second = Point(x, y);
	}
}

void mouseCallback2(int event, int x, int y, int flags, void *param) {
	if (event == CV_EVENT_LBUTTONDOWN) {
		cout << x << " " << y << endl;
		p3.x = x;
		p3.y = y;
		pair<Point, Point> line;
		line.first = Point(x, y);
		pic2_lines.push_back(line);
	}
	if (event == CV_EVENT_LBUTTONUP) {
		p4.x = x;
		p4.y = y;
		pic2_lines.back().second = Point(x, y);
	}
}

Mat morphing(Mat img1, Mat img2, float t) {
	
	for (int i = 0; i < min(pic1_lines.size(), pic2_lines.size()); ++i) {
		Point p1 = pic1_lines.at(i).first * t + pic2_lines.at(i).first * (1 - t);
		Point p2 = pic1_lines.at(i).second * t + pic2_lines.at(i).second * (1 - t);
		pair<Point, Point> line;
		line.first = p1;
		line.second = p2;
		result_lines.push_back(line);
	}
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv	pic1
	
	int height = img1.rows;
	int width = img1.cols;
	int ch = img1.channels();
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			for (int k = 0; k < ch; ++k) {
				Point psum(0, 0);
				float wsum = 0;
				for (int l = 0; l < result_lines.size(); ++l) {
					Point p_before = Transform(Point(i,j),pic1_lines[l],result_lines[l]);
					float length = sqrt(pow(result_lines.at(l).first.x - result_lines.at(l).second.x, 2) + pow(result_lines.at(l).first.y - result_lines.at(l).second.y, 2));
					float dist;
					float weight = pow(pow(length, 2) / 1 + dist, 2);
					//img1.at(height, width, ch);
				}
			}
		}
	}
	
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^	pic1
	//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv	pic2

	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^				pic2

	Mat result = img1 / 2 + img2 / 2;
	return result;
}
Point Transform(Point p, pair<Point, Point> L1, pair<Point, Point> L2) {
	//calculate u
	pair<int, int> XP;
	pair<int, int> QP;
	QP.first = L1.second.x - L1.first.x;
	QP.second = L1.second.y - L1.first.y;
	XP.first = p.x - L1.first.x;
	XP.second = p.y - L1.first.y;
	float distQP= sqrt(pow(QP.first, 2) + pow(QP.second, 2));
	float u = (XP.first*QP.first + XP.second*QP.second) / pow(distQP, 2);
	//calculate v
	pair<int, int> Per_QP;
	Per_QP.first = QP.second;
	Per_QP.second = -QP.first;
	if (Per_QP.first*XP.first + Per_QP.second*XP.second < 0) {
		Per_QP.first = -Per_QP.first;
		Per_QP.second= -Per_QP.second;
	}
	float v= (XP.first*Per_QP.first + XP.second*Per_QP.second) /distQP;

	//calculate p'
	pair<int, int> QP_result;
	QP_result.first = L2.second.x - L2.first.x;
	QP_result.second= L2.second.y - L2.first.y;
	float distQP_result = sqrt(pow(QP_result.first, 2) + pow(QP_result.second, 2));
	Point P_result;
	P_result.x = L2.first.x + u*QP_result.first+v*QP_result.first/distQP_result;
	P_result.y = L2.first.y + u*QP_result.second + v*QP_result.second/distQP_result;
	return P_result;
}

