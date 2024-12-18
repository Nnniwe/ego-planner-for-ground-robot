//
// Created by Hyungtae Lim on 6/23/21.
//

// For disable PCL complile lib, to use PointXYZILID
#define PCL_NO_PRECOMPILE
#include "patchwork/patchwork.hpp"
#include <cstdlib>


using PointType = pcl::PointXYZ;
using namespace std;

ros::Publisher CloudPublisher;
ros::Publisher PositivePublisher;
ros::Publisher NegativePublisher;

pcl::PointCloud<PointType> pc_curr;
pcl::PointCloud<PointType> pc_ground;
pcl::PointCloud<PointType> pc_non_ground;
static double time_taken;

boost::shared_ptr<PatchWork<PointType> > PatchworkGroundSeg;

std::string output_filename;
std::string acc_filename, pcd_savepath;
string      algorithm;
string      mode;
string      seq;
bool        save_flag;

template<typename T>
pcl::PointCloud<T> cloudmsg2cloud(sensor_msgs::PointCloud2 cloudmsg)
{
    pcl::PointCloud<T> cloudresult;
    pcl::fromROSMsg(cloudmsg,cloudresult);
    return cloudresult;
}

template<typename T>
sensor_msgs::PointCloud2 cloud2msg(pcl::PointCloud<T> cloud, std::string frame_id = "world")
{
    sensor_msgs::PointCloud2 cloud_ROS;
    pcl::toROSMsg(cloud, cloud_ROS);
    cloud_ROS.header.frame_id = frame_id;
    return cloud_ROS;
}

void callbackNode(const sensor_msgs::PointCloud2ConstPtr &msg) {
    pcl::fromROSMsg(*msg,pc_curr);
    PatchworkGroundSeg->estimate_ground(pc_curr, pc_ground, pc_non_ground, time_taken);
    CloudPublisher.publish(cloud2msg(pc_curr));
    PositivePublisher.publish(cloud2msg(pc_ground));
    NegativePublisher.publish(cloud2msg(pc_non_ground));
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "Benchmark");
    ros::NodeHandle nh;
    nh.param<string>("/algorithm", algorithm, "patchwork");

    ros::Subscriber NodeSubscriber = nh.subscribe<sensor_msgs::PointCloud2>("/pointcloud_scan", 5000, callbackNode);

    ros::Rate loop_rate(10);

    PatchworkGroundSeg.reset(new PatchWork<PointType>(&nh));

    CloudPublisher  = nh.advertise<sensor_msgs::PointCloud2>("/benchmark/cloud", 100);
    PositivePublisher     = nh.advertise<sensor_msgs::PointCloud2>("/benchmark/P", 100);
    NegativePublisher     = nh.advertise<sensor_msgs::PointCloud2>("/benchmark/N", 100);

    string filename = "/home/dango/dango_file2/graduation_project/catkin_ws/005269.pcd";

    // An example for Loading own data
    //pcl::PointCloud<PointType> pc_curr;
    //if (pcl::io::loadPCDFile<PointType> (filename, pc_curr) == -1) //* load the file
    //{
    //    PCL_ERROR ("Couldn't read file test_pcd.pcd \n");
    //    return (-1);
    //}

    
    cout << "Operating patchwork..." << endl;

    while (ros::ok()){
        
        ros::spinOnce();
        loop_rate.sleep();
    }






    return 0;
}
