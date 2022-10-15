#include"ui.h"
using namespace cv;
std::vector<vector<int>> position;
//�ڶ�Ӧ�ĸ����л�ͼ
void plot(Mat image, std::string name, int number, showtype type, bool isBin)
{
	Size dsize;
	switch (type)
	{
	case showtype::not_change:
		dsize = image.size();
		break;
	case showtype::keepRatio:
		if (image.size().height > image.size().width)
			dsize.width = GRID_H / image.size().height * image.size().width;
		else
			dsize.height = GRID_W / image.size().width * image.size().height;
		break;
	case showtype::whole_fill:
		dsize= Size(GRID_W, GRID_H);
		break;
	default:
		break;
	}
	if (isBin)
		image = image * 255;
	Mat back;
	resize(image, back, dsize);
	imshow(name, back);
	moveWindow(name, position[number][0], position[number][1]);
}

void plot(Mat image, std::string name, int number)
{
	plot(image, name, number, showtype::whole_fill, false);
}

void plot(Mat image, std::string name, int number, bool isBin)
{
	plot(image, name, number, showtype::whole_fill, isBin);
}
//ʵ������Matlab�е�subplot�Ĺ���
void subplot(int y, int x)
{
	position.clear();
	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			vector<int> tmp = { GRID_W * j ,GRID_H * i + 30 * i };
			position.push_back(tmp);
		}
	}
}
//��ά����תMat��ʽ
Mat array2mat(uint8* arr, uint8 h, uint8 w)
{
	Mat a = Mat(h, w, CV_8UC1, (uint8*)arr);
	Mat b;
	a.copyTo(b);
	return b;
}
//Mat��ʽת��ά����
void mat2array(Mat img)
{
	int h = img.size().height;
	int w = img.size().width;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			mt9v30x_image[i][j] = img.at<uint8>(i, j);
		}
	}
}
const Vec3b WHITE = Vec3b(255, 255, 255);
const Vec3b BLACK = Vec3b(0, 0, 0);
const Vec3b RED = Vec3b(0, 0, 255);
const Vec3b GREEN = Vec3b(0, 255, 0);
const Vec3b BLUE = Vec3b(255, 0, 0);
const Vec3b YELLOW = Vec3b(0, 255, 255);
const Vec3b CYAN = Vec3b(255, 255, 0);
const Vec3b DARKYELLOW = Vec3b(0, 128, 128);
const Vec3b GRAY = Vec3b(128, 128, 128);
//����ȡ����ͼ����Ϣ��ע��ͼƬ��
Mat paintLine(Mat imgBin)
{	
	Mat imgGray;
	imgBin.copyTo(imgGray);
	imgGray = imgGray * 255;
	Mat imgColor;
	cvtColor(imgGray, imgColor, COLOR_GRAY2BGR);
	//���߼����ұ���
	for (int i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		imgColor.at<Vec3b>(i, RLB[i]) = RED;
		imgColor.at<Vec3b>(i, LLB[i]) = RED;
		imgColor.at<Vec3b>(i, MLB[i]) = GREEN;

	}
	//��Ч������
	for (int j = 0; j < COLUMN; j++)
	{
		imgColor.at<Vec3b>(imgInfo.bottom, j) = YELLOW;
		imgColor.at<Vec3b>(imgInfo.top, j) = YELLOW;
	}
	String guide="";
	if (imgInfo.differ > 8)
		guide = ">";
	if (imgInfo.differ > 16)
		guide = ">>";
	if (imgInfo.differ > 24)
		guide = ">>>";
	if (imgInfo.differ < -8)
		guide = "<";
	if (imgInfo.differ < -16)
		guide = "<<";
	if (imgInfo.differ < -24)
		guide = "<<<";
	if(imgInfo.differ>0)
		cvui::printf(imgColor,138,40,0.4, 0xdc143c, guide.c_str());
	else
		cvui::printf(imgColor,50,40,0.4, 0xdc143c, guide.c_str());
	return imgColor;
}
//��ȡ�Ҷ�ֱ��ͼ
Mat getHistImg(Mat img, int th)
{
	//��ͼ��ָ������ͨ��BRG

	//������256���������ֱ��ͼ
	//ֵ�Ŀ�������Ϊ��0.255��
	int numbins = 256;

	//���÷�Χ��BGR�����һ��ֵ������
	float range[] = { 0, 256 };
	const float* histRange = { range };

	Mat hist;

	calcHist(&img, 1, 0, Mat(), hist, 1, &numbins, &histRange);

	//����ֱ��ͼ
	//��Ϊÿ��ͼ��ͨ������
	int width = IMAGE_W;
	int height = IMAGE_H;

	//�Ի�ɫΪ���״���ͼ��
	Mat histImage(height, width, CV_8UC3, Scalar(255, 255, 255));

	//��0��ͼ��ĸ߶ȹ�һ��ֱ��ͼ
	normalize(hist, hist, 0, height, NORM_MINMAX);

	int binStep = cvRound((float)width / (float)numbins);
	for (int i = 1; i < width; i++)
	{
		if ((i - 1) * numbins / width == th)
			line(histImage,
				Point(i - 1, height),
				Point(i - 1, 0),
				Scalar(0, 0, 255));
		else
			line(histImage,
				Point(i - 1, height),
				Point(i - 1, height - cvRound(hist.at<float>((i - 1) * numbins / width))),
				Scalar(128, 128, 128));
	}
	return histImage;
}
//��ͼƬ�ĻҶ�ֵ��ʾ����
Mat getTextImg(Mat img)
{
	int scalex = 3;
	int scaley = 9;
	Mat tmp = Mat(60 * scaley, 188 * scalex, CV_8UC1);
	tmp = cv::Scalar(0);
	for (int i = 0; i < 60; i += 2)
	{
		for (int j = 0; j < 188; j += 8)
		{
			cvui::printf(tmp, j * scalex, i * scaley, 0.4, 2 * img.at<uint8>(i,j), "%3d", img.at<uint8>(i, j));
		}
	}
	return tmp;
}
//ˢ��UI��ܲ���ȡ�û�����
void UI_refresh(Mat& frame,int &direction,bool &autoplay)
{
	frame = cv::Scalar(49, 52, 49);
	int line = 0;
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xff0000, "imgNum:%d", number_image);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xff0000, "differ: %d ", imgInfo.differ);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xff0000, "threshold: %d ", threshold_image);
	if (!autoplay)direction = 0;
	if (cvui::button(frame, 10, 20, 80, 50, "Previous"))
		direction = -1;
	if (cvui::button(frame, 100, 20, 80, 50, "Next"))
		direction = 1;
	cvui::checkbox(frame, 200, 40, "Auto Play", &autoplay, 0x00ff7f);
	if (cvui::button(frame, 60, 80, 80, 50, "End"))
		exit(0);
	cvui::update();
	cv::imshow("UI_block", frame);
}

