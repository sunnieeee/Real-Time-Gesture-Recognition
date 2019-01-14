/*=================================================================================
 *	                    Copyleft! 2019 Sunny Cao
 *          Some rights reserved：CC(creativecommons.org)BY-NC-SA
 *                      Copyleft! 2019 Sunny Cao
 *      版权部分所有，遵循CC(creativecommons.org)BY-NC-SA协议授权方式使用
 *
 * Filename                : main.cpp
 * Description             : 基于Opencv库的手势识别
 * Reference               : 
 * Programmer(s)           : Sunny Cao, sunniecyq@163.com
 * 							 
 * Company                 : HUST, Thinkfly
 * Modification History	   : ver1.0, 2017.12.26, Sunny Cao 
 *                           
=================================================================================*/


#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	Mat s,g;                                    //s为输入图像，g为输出图像
//	VideoCapture capture(0);                    //调用摄像头采集图像，当capture内为文件名时可为调用视频文件
	VideoCapture capture(argv[1]);
	vector<vector<Point> > g_Contours;          //数组，由点至一条轮廓至轮廓集
	vector<Vec4i> g_Hier;                       //开辟存放轮廓坐标的层级空间
	bool verify(vector<Point>input, int min, int max);

/****读取视频图像****/
	while (1)                                   
	{
		Mat frame;                              
		Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));      //定义待处理的图像元素的形状、大小（椭圆/圆/矩形），参数由调试确定
		capture >> frame;                       

		namedWindow("1",WINDOW_AUTOSIZE);
		imshow("1",frame);                            


/****图像预处理****/
		//转化为灰度图
		cvtColor(frame, s, COLOR_BGR2GRAY);          
		//通过滤波、开运算（腐蚀膨胀）对图像进行降噪
		blur(s, s, Size(15, 15));
		erode(s,s, element);
		dilate(s, s, element);


/****转化为二值图****/
/*      adaptiveThreshold(s,g,255 , ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV ,5, 0); */      //自适应阈值函数，效果不佳，不及OTSU算法
		threshold(s, g, 0, 255, CV_THRESH_OTSU);
		
		namedWindow("cyq", WINDOW_AUTOSIZE);
		imshow("cyq", g);



/****轮廓处理****/
        //获取轮廓
		findContours(g,g_Contours,g_Hier,RETR_TREE,CHAIN_APPROX_SIMPLE,Point());

    	//轮廓筛选

		//设置轮廓周长的极大值和极小值，通过调试而得
		int cmax = 360, cmin = 160;                 
		//优化？用轮廓周长和面积的比值？？可减少手距摄像头距离影响                   
		
		vector<vector<Point> >::iterator itc = g_Contours.begin();
		while (itc != g_Contours.end())
		{
			//擦去不在区间内的轮廓
			double area = contourArea(*itc);
      	/*  double perimeter= arcLength(*itc, true); */
			
			if (itc->size() > cmin && itc->size() < cmax && area<16000 &&area>8000)
			    ++itc;                                                      //循环至达到.end() 的数目跳出
			else
				itc = g_Contours.erase(itc);
			
			
		}




/****计算图像的矩的中心	****/
    //图像的矩
        /* int k = 0;*/
		vector<Moments> ma(g_Contours.size());
		for (int i = 0; i < g_Contours.size(); i++)
		{

			ma[i] = moments(g_Contours[i], false);
        /*	k++;    */                                                      //计数器，显示每次循环绘制的中心数便于调试
		}
        /*cout << k << endl;    */
	

	//二维浮点数坐标的质心
		vector<Point2f> mc(g_Contours.size());        
		for (int i = 0; i < g_Contours.size(); i++)
		{
			mc[i] = Point2f(static_cast<float>(ma[i].m10 / ma[i].m00), static_cast<float>(ma[i].m01 / ma[i].m00));
		}


/****绘制符合条件的轮廓及中心点****/
		Mat drawing = Mat::zeros(g.size(), CV_8UC3);                        //定义画板
		
	   //循环控制绘制轮廓
		for (int i = 0; i < g_Contours.size(); i++)          
		{
			double area = contourArea(g_Contours[i]);
			double premiter = arcLength(g_Contours[i], true);
	     /* cout << area << "  " << premiter << endl;
         */   
				Scalar color = Scalar(255);                             
				drawContours(drawing, g_Contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());   
				circle(drawing, mc[i], 4, color, -1, 8, 0);                 //绘制质心，半径为4，厚度为负数即实心圆
	      /*	cout << mc[i] << endl;             */                       //输出质心坐标
		

/****手势匹配识别****/
		   //可优化？
			if (premiter > 800)                                             //根据不同手势的轮廓周长分类
				cout << "五指巴掌" << endl;
			else if (premiter < 500)
				cout << "拳头" << endl;
			else
				cout << "其他" << endl;
		}
		

		namedWindow("2", WINDOW_AUTOSIZE);
		imshow("2",drawing);                                                 //显示画板窗口


		if ((char)waitKey(30) == 27)break;                                   //等待用户按键，27是ESC对应的ASCII码值
	}
	return 0;
	
}

