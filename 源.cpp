#include "opencv2/opencv.hpp" 
#include<iostream>
#include"opencv2/xfeatures2d.hpp"
#include<opencv2/core/core.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv/cv.h>
#include<stdio.h>

using namespace std;
using namespace cv;

int Find_Can(Mat srcImage, Mat srcScene) {
	//�����ز�ͼ  
	// srcImage = imread("C:\\Users\\wy\\Desktop\\ʶ��\\sww.jpg", 1);
	// srcScene = imread("C:\\Users\\wy\\Desktop\\ʶ��\\lehu.jpg", 1);
	//imshow("Image", srcImage);
	//imshow("Scene", srcScene);


	//ʹ��SURF���Ӽ��ؼ���  
	//int minHessian = 200;
	Ptr<Feature2D> surf = xfeatures2d::SURF::create(200);
	vector<KeyPoint> ImgPoints, ScePoints;

	//����detect��������SURF�����ؼ��㣬������vector������
	surf->detect(srcImage, ImgPoints);
	surf->detect(srcScene, ScePoints);

	//����������������������
	//SurfDescriptorExtractor extractor;
	Mat descriptors1, descriptors2;
	surf->compute(srcImage, ImgPoints, descriptors1);
	surf->compute(srcScene, ScePoints, descriptors2);

	//ʹ��FlannBased����ƥ�� 
	FlannBasedMatcher matcher;
	vector< DMatch > matches;
	matcher.match(descriptors1, descriptors2, matches);

	// ����ռ�
	int ptCount = (int)matches.size();
	Mat p1(ptCount, 2, CV_32F);
	Mat p2(ptCount, 2, CV_32F);

	// ��Keypointת��ΪMat
	Point2f pt;
	for (int i = 0; i<ptCount; i++)
	{
		pt = ImgPoints[matches[i].queryIdx].pt;
		p1.at<float>(i, 0) = pt.x;
		p1.at<float>(i, 1) = pt.y;

		pt = ScePoints[matches[i].trainIdx].pt;
		p2.at<float>(i, 0) = pt.x;
		p2.at<float>(i, 1) = pt.y;
	}

	// ��RANSAC��������F
	Mat m_Fundamental;
	vector<uchar> RANSACStatus;
	m_Fundamental = findFundamentalMat(p1, p2, RANSACStatus, FM_RANSAC);

	// ����Ұ�����
	int OutlinerCount = 0;
	for (int i = 0; i<ptCount; i++)
	{
		if (RANSACStatus[i] == 0) // ״̬Ϊ0��ʾҰ��
		{
			OutlinerCount++;
		}
	}

	// �����ڵ�
	vector<Point2f> ImgPointInlier;
	vector<Point2f> ScePointInlier;
	vector<DMatch> InlierMatches;
	// ���������������ڱ����ڵ��ƥ���ϵ

	int InlinerCount = ptCount - OutlinerCount;
	cout << InlinerCount << endl;

	InlierMatches.resize(InlinerCount);
	ImgPointInlier.resize(InlinerCount);
	ScePointInlier.resize(InlinerCount);
	InlinerCount = 0;
	for (int i = 0; i<ptCount; i++)
	{
		if (RANSACStatus[i] != 0)
		{
			ImgPointInlier[InlinerCount].x = p1.at<float>(i, 0);
			ImgPointInlier[InlinerCount].y = p1.at<float>(i, 1);
			ScePointInlier[InlinerCount].x = p2.at<float>(i, 0);
			ScePointInlier[InlinerCount].y = p2.at<float>(i, 1);
			InlierMatches[InlinerCount].queryIdx = InlinerCount;
			InlierMatches[InlinerCount].trainIdx = InlinerCount;
			InlinerCount++;
		}
	}
	/*
	// ���ڵ�ת��ΪdrawMatches����ʹ�õĸ�ʽ
	vector<KeyPoint> key1(InlinerCount);
	vector<KeyPoint> key2(InlinerCount);
	KeyPoint::convert(ImgPointInlier, key1);
	KeyPoint::convert(ScePointInlier, key2);

	// ��ʾ����F������ڵ�ƥ��

	Mat imgMatches;
	drawMatches(srcImage, key1, srcScene, key2, InlierMatches, imgMatches);//���л���
	imshow("ƥ��ͼ", imgMatches);

	waitKey(0);
	destroyWindow("ƥ��ͼ");
	*/
	printf("%d\n", InlinerCount);
	//cout << InlinerCount << endl;
	return InlinerCount;
}
int Judge_Can(Mat Scene, int thr) //sceneΪ�Ҷ�ͼthrΪ��ֵ
{
	//�ҳ����ֵ
	int Best_match[10] = { 0 };
	Mat Can = imread("C:\\Users\\wy\\Desktop\\ʶ��\\sww.jpg", 1);//imread�в���1Ϊԭͼ��0Ϊ�Ҷ�ͼ
	Best_match[1] = Find_Can(Can, Scene);
	Can = imread("C:\\Users\\wy\\Desktop\\ʶ��\\whh.jpg", 1);
	Best_match[2] = Find_Can(Can, Scene);
	Can = imread("C:\\Users\\wy\\Desktop\\ʶ��\\mf.jpg", 1);
	Best_match[5] = Find_Can(Can, Scene);
	Can = imread("C:\\Users\\wy\\Desktop\\ʶ��\\xh.jpg", 1);
	Best_match[8] = Find_Can(Can, Scene);
	Can = imread("C:\\Users\\wy\\Desktop\\ʶ��\\lehu.jpg", 1);
	Best_match[9] = Find_Can(Can, Scene);
	
	int Max_count = Best_match[0];
	int temp;
	int i;
	int k = 0;
	for (i = 1; i < 10; i++) {
		if (Max_count < Best_match[i]) {
			temp = Max_count;
			Max_count = Best_match[i];
			k = i;
			Best_match[i] = temp;
		}
	}

	if (Max_count > thr) {
		return k;
	}
	else return 0;
}


int Judge_Red(Mat src)//srcΪHSVģ��
{
	int minh, maxh, mins, maxs, minv, maxv;
	minh = 0;
	maxh = 10;
	mins = 43;
	maxs = 255;
	minv = 46;
	maxv = 255;

	Mat special;
	inRange(src, Scalar(minh, mins, minv), Scalar(maxh, maxs, maxv), special);
	imshow("red", special);
	//�������ص����
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = special.begin<uchar>();
	Mat_<uchar>::iterator itend = special.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//��ֵ�������ص���0����255
	}
	printf("red%d\n",counter);
	return counter;
}
int Judge_Green(Mat src)//srcΪHSVģ��
{
	int minh, maxh, mins, maxs, minv, maxv;
	minh = 35;
	maxh = 77;
	mins = 43;
	maxs = 255;
	minv = 46;
	maxv = 255;

	Mat special;
	inRange(src, Scalar(minh, mins, minv), Scalar(maxh, maxs, maxv), special);
	imshow("green",special);
	//�������ص����
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = special.begin<uchar>();
	Mat_<uchar>::iterator itend = special.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//��ֵ�������ص���0����255
	}
	printf("green%d\n", counter);
	return counter;
}
int Judge_Yellow(Mat src)//srcΪHSVģ��
{
	int minh, maxh, mins, maxs, minv, maxv;
	minh = 16;
	maxh = 34;
	mins = 43;
	maxs = 255;
	minv = 46;
	maxv = 255;

	Mat special;
	inRange(src, Scalar(minh, mins, minv), Scalar(maxh, maxs, maxv), special);
	imshow("yellow", special);
	//�������ص����
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = special.begin<uchar>();
	Mat_<uchar>::iterator itend = special.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//��ֵ�������ص���0����255
	}
	printf("yellow%d\n", counter);
	return counter;
}
int Judge_Blue(Mat src)//srcΪHSVģ��
{
	int minh, maxh, mins, maxs, minv, maxv;
	minh = 80;
	maxh = 130;
	mins = 43;
	maxs = 255;
	minv = 46;
	maxv = 255;

	Mat special;
	inRange(src, Scalar(minh, mins, minv), Scalar(maxh, maxs, maxv), special);
	imshow("blue", special);
	//�������ص����
	int counter = 0;
	//�������������ص�
	Mat_<uchar>::iterator it = special.begin<uchar>();
	Mat_<uchar>::iterator itend = special.end<uchar>();
	for (; it != itend; ++it)
	{
		if ((*it)>0) counter += 1;//��ֵ�������ص���0����255
	}
	printf("blue%d\n", counter);
	return counter;
}

int main()
{
	VideoCapture cap(0);
	Mat frame;
	Mat frame_find;
	Mat frame_color;

	while (1) {
		cap >> frame;
		imshow("Camera", frame);
		frame_find = frame;
		//cvtColor(frame, frame_find, CV_RGB2GRAY);		//ת��Ϊ�Ҷ�ͼ	
		cvtColor(frame, frame_color, COLOR_BGR2HSV);	//ת��ΪHSVģ��
		//imshow("grey", frame_find);
		//imshow("HSV", frame_color);
		int p = waitKey(10);
		//if (p == 'o') break;
		if (p == 'c') {
			//�õ�hsv��ROI
			int cols_c = frame_color.cols, rows_c = frame.rows;
			//cout << cols << endl;
			//cout << rows << endl;
			int width_c = 300, height_c = 130;
			Rect rect_c(0.5*(cols_c - width_c), height_c, width_c, rows_c - height_c);
			Mat frame_CROI = frame_color(rect_c);
			imshow("ROI", frame_CROI);
			int flag = 0;
			int red = 0;
			int green = 0;
			int yellow = 0;
			int blue = 0;
			int max = 13000;//��ţ��ɫ���ޣ�ľ�鵥һ��ɫ����
			int min_mk = 9000;//����ɫ������
			int min = 100;//ľ����ɫ����
			int judge = 0;
			char output = '0';

			red = Judge_Red(frame_CROI);
			green = Judge_Green(frame_CROI);
			blue = Judge_Blue(frame_CROI);				
			yellow = Judge_Yellow(frame_CROI);

			if (green >= max && red <= min) { output = '8'; flag++; }//xh
			if (yellow >= max && (red >= 2000 && red <= 6000) && green <= min) { output = '7'; flag++; }//redbull
			if (yellow >= max && green >= 2000 && red <= min) { output = '3'; flag++; }//tennis
			if ((red >= 4000 && red <= 6000) && green <= min && yellow <= min && blue <= min) { output = '4'; flag++; }//yld
			if ((yellow >= 1000 && yellow <= 4000) && green <= min && red <= min+500 && blue <= min) { output = '6'; flag++; }//tls
			if ((red >= max + 10000) && yellow <= min && green <= min && blue <= min) { output = 'A'; flag++; }//RED
			if ((green >= max + 10000) && yellow <= min && red <= min && blue <= min) { output = 'B'; flag++; }//GREEN
			if ((blue >= max + 10000) && yellow <= min && green <= min && red <= min) { output = 'C'; flag++; }//BLUE
			
			if (flag == 0) {
				//�õ��Ҷ�ͼ��ROI
				int cols = frame_find.cols, rows = frame.rows;
				//cout << cols << endl;
				//cout << rows << endl;
				int width = 300, height = 100;
				Rect rect(0.5*(cols - width), height, width, rows - height);
				Mat frame_FROI = frame_find(rect);
				imshow("ROI", frame_FROI);
				output = 48 + Judge_Can(frame_FROI, 40);				
			}

			putchar(output);
			
		}
	}


	return 0;
}

