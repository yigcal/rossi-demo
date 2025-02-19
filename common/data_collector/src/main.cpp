/*
 * main.cpp
 *
 *  Created on: Mar 31, 2011
 *      Author: kadir
 */

// ROS include
#include "ros/ros.h"
#include <pcl/ros/conversions.h>
#include <image_transport/image_transport.h>

//pcl include
#include "pcl/io/pcd_io.h"
#include "pcl/point_types.h"

#include "cv_bridge/CvBridge.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

//std include
#include "iostream"
#include "queue"
#include "string"
#include "sstream"

//keyboard event
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define ENTER_KEY 10
#define SPACE 32

char 	my_kbhit (void){
	  struct timeval tv;
	  fd_set rdfs;

	  tv.tv_sec = 0;
	  tv.tv_usec = 0;

	  FD_ZERO(&rdfs);
	  FD_SET (STDIN_FILENO, &rdfs);

	  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	  return (char) FD_ISSET(STDIN_FILENO, &rdfs);
}
void	change_mode(int dir){
	static struct termios oldt, newt;
	if ( dir == 1 )
	{
	  tcgetattr( STDIN_FILENO, &oldt);
	  newt = oldt;
	  newt.c_lflag &= ~( ICANON | ECHO );
	  tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	}
	else
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
void 	quit(){
	change_mode(0); // VERY IMPORTANT, KEEPS SHELL FROM BECOMING A MASS
}

void 	nullDeleter(void*) {}

class DataCollector{
private:
	ros::NodeHandle n;
	image_transport::ImageTransport n_it_img_d, n_it_img_a, n_it_img_c, n_it_img_d_16;
	image_transport::Subscriber sub_img_d;
	image_transport::Subscriber sub_img_a;
	image_transport::Subscriber sub_img_c;
	image_transport::Subscriber sub_img_d_16;
	ros::Subscriber sub_pc2;

	sensor_msgs::Image img_d, img_a, img_c, img_d_16;
	sensor_msgs::PointCloud2 pc2;
	std::queue<sensor_msgs::Image> img_d_buffer;
	std::queue<sensor_msgs::Image> img_a_buffer;
	std::queue<sensor_msgs::Image> img_c_buffer;
	std::queue<sensor_msgs::Image> img_d_16_buffer;
	std::queue<sensor_msgs::PointCloud2> pc2_buffer;

	pcl::PointCloud<pcl::PointXYZ>::Ptr pc2_data;
	sensor_msgs::CvBridge bridge;
	IplImage* dist_data;
	IplImage* conf_data;
	IplImage* amp_data;
	IplImage* dist_16_data;

	unsigned int data_cnt;
	std::string pc2_file_base_name;
	std::string dist_file_base_name;
	std::string dist_16_file_base_name;
	std::string conf_file_base_name;
	std::string ampl_file_base_name;

	void pc2RcvdCallback(const sensor_msgs::PointCloud2::ConstPtr& pc_2 ){
		pc2 = *pc_2;
		pcl::fromROSMsg(pc2,*pc2_data);
		pc2_buffer.push(pc2);
	}
	void distanceRcvdCallback(const sensor_msgs::ImageConstPtr& image_d){
		img_d = *image_d;
		img_d_buffer.push(img_d);
	}
	void amplitudeRcvdCallback(const sensor_msgs::ImageConstPtr& image_a){
		img_a = *image_a;
		img_a_buffer.push(img_a);
	}
	void confidenceRcvdCallback(const sensor_msgs::ImageConstPtr& image_c){
		img_c = *image_c;
		img_c_buffer.push(img_c);
	}
	void distance16RcvdCallback(const sensor_msgs::ImageConstPtr& image_d_16){
		img_d_16 = *image_d_16;
		img_d_16_buffer.push(img_d_16);
	}

public:
	DataCollector(const ros::NodeHandle& nh_) : n(nh_), n_it_img_d(n), n_it_img_a(n), n_it_img_c(n), n_it_img_d_16(n){

		pc2_data = (pcl::PointCloud<pcl::PointXYZ>::Ptr)new pcl::PointCloud<pcl::PointXYZ> ();

		sub_pc2 	=
				n.subscribe
				("/swissranger/pointcloud2_raw", 10, &DataCollector::pc2RcvdCallback,this);

		sub_img_d 	=
				n_it_img_d.subscribe
				("/swissranger/distance/image_raw",	10,	&DataCollector::distanceRcvdCallback, this);
		sub_img_c 	=
				n_it_img_c.subscribe
				("/swissranger/confidence/image_raw", 10, &DataCollector::confidenceRcvdCallback, this);
		sub_img_a 	=
				n_it_img_a.subscribe
				("/swissranger/intensity/image_raw", 10, &DataCollector::amplitudeRcvdCallback, this);
		//sub_img_d_16 	=
		//		n_it_img_d_16.subscribe
		//		("/swissranger/distance/image_raw16",	10,	&DataCollector::distance16RcvdCallback, this);

		dist_data = cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
		conf_data = cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
		amp_data  = cvCreateImage(cvSize(176,144),IPL_DEPTH_8U,1);
		//dist_16_data= cvCreateImage(cvSize(176,144),IPL_DEPTH_16U,1);

		data_cnt = 0;
		pc2_file_base_name.assign("pc2_");
		dist_file_base_name.assign("dist_");
		conf_file_base_name.assign("conf_");
		ampl_file_base_name.assign("ampl_");
		dist_16_file_base_name.assign("dist_16_");
/*
		cvNamedWindow("distance",1);
		cvNamedWindow("confidence",1);
		cvNamedWindow("amplitude",1);
*/
	}
	~DataCollector(){
		quit();
/*
		cvDestroyWindow("distance");
		cvDestroyWindow("confidence");
		cvDestroyWindow("amplitude");
*/
	}

	void saveData(){

			std::string file_name;
			std::stringstream s;
			s<<data_cnt;

			file_name.assign(pc2_file_base_name);
			file_name.append(s.str());
			file_name.append(".pcd");
			pcl::io::savePCDFile(file_name,*pc2_data);

			file_name.clear();
			file_name.assign(dist_file_base_name);
			file_name.append(s.str());
			file_name.append(".jpg");
			cvSaveImage(file_name.c_str(),dist_data);

			file_name.clear();
			file_name.assign(conf_file_base_name);
			file_name.append(s.str());
			file_name.append(".jpg");
			cvSaveImage(file_name.c_str(),conf_data);

			file_name.clear();
			file_name.assign(ampl_file_base_name);
			file_name.append(s.str());
			file_name.append(".jpg");
			cvSaveImage(file_name.c_str(),amp_data);
//what is the point of stroing 16bit depth image, you already have the point cloud
			//file_name.clear();
			//file_name.assign(dist_16_file_base_name);
			//file_name.append(s.str());
			//file_name.append(".jpg");
			//cvSaveImage(file_name.c_str(),dist_16_data);
	}

	bool getData(){
		if(	img_d_buffer.size() > 0 &&
			img_a_buffer.size() > 0 &&
			img_c_buffer.size() > 0 &&
			//img_d_16_buffer.size() > 0 &&
			pc2_buffer.size() > 0){

			sensor_msgs::PointCloud2 _pc2 = pc2_buffer.front();
			sensor_msgs::Image _img_d = img_d_buffer.front();
			sensor_msgs::Image _img_a = img_a_buffer.front();
			sensor_msgs::Image _img_c = img_c_buffer.front();
			//sensor_msgs::Image _img_d_16 = img_d_16_buffer.front();

			pcl::fromROSMsg(_pc2, *pc2_data);
			IplImage* tmp = cvCreateImage( cvSize(176,144), IPL_DEPTH_8U, 1 );
			boost::shared_ptr<sensor_msgs::Image> d_ptr(&_img_d, nullDeleter);
			tmp = bridge.imgMsgToCv(d_ptr, "mono8");
			cvCopy(tmp,dist_data);

			boost::shared_ptr<sensor_msgs::Image> a_ptr(&_img_a, nullDeleter);
			tmp = bridge.imgMsgToCv(a_ptr, "mono8");
			cvCopy(tmp,amp_data);

			boost::shared_ptr<sensor_msgs::Image> c_ptr(&_img_c, nullDeleter);
			tmp = bridge.imgMsgToCv(c_ptr, "mono8");
			cvCopy(tmp,conf_data);

			//boost::shared_ptr<sensor_msgs::Image> d_16_ptr(&_img_d_16, nullDeleter);
			//tmp = bridge.imgMsgToCv(d_16_ptr, "mono8");
			//cvCopy(tmp,dist_16_data);

			if(pc2_buffer.size()>0)
				pc2_buffer.pop();
			if(img_d_buffer.size()>0)
				img_d_buffer.pop();
			if(img_a_buffer.size()>0)
				img_a_buffer.pop();
			if(img_c_buffer.size()>0)
				img_c_buffer.pop();
			//if(img_d_16_buffer.size()>0)
				//img_d_16_buffer.pop();
			std::cout<<data_cnt<<std::endl;
			data_cnt++;
			//cvReleaseImage(&tmp);
			return true;
		}
		else
			return false;
	}

	void showData(){
		cvShowImage("distance",dist_data);
		cvShowImage("confidence",conf_data);
		cvShowImage("amplitude",amp_data);
		cvWaitKey(5);
	}

	bool run(){

		char key=0;
		change_mode(1); //used for keyboard interrupt
		std::cout<<"entering the loop"<<std::endl;

		while(n.ok())
		{
			if(getData())
			{
				showData();
				if(my_kbhit()){
					key=getchar();
					if(key == SPACE)
						saveData();
				}
			}
			ros::spinOnce();
		}
		return true;
	}
};

int main(int argc, char** argv){
	ros::init(argc, argv, "data_collector");
	ros::NodeHandle nh;
	DataCollector dc(nh);
	dc.run();
	return 0;
}
