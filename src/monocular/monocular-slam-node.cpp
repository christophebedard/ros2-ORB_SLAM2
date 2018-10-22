#include "monocular-slam-node.hpp"

#include<opencv2/core/core.hpp>

#include"MapPoint.h"

using ImageMsg = sensor_msgs::msg::Image;

using namespace std;

using std::placeholders::_1;

MonocularSlamNode::MonocularSlamNode(ORB_SLAM2::System* pSLAM, const string &strVocFile, const string &strSettingsFile)
:   Node("orbslam")
    ,m_SLAM(pSLAM)
{

    m_image_subscriber = this->create_subscription<ImageMsg>("camera", std::bind(&MonocularSlamNode::GrabImage, this, std::placeholders::_1));

}


MonocularSlamNode::~MonocularSlamNode()
{
    // Stop all threads
    m_SLAM->Shutdown();

    // Save camera trajectory
    m_SLAM->SaveKeyFrameTrajectoryTUM("KeyFrameTrajectory.txt");

}


void MonocularSlamNode::GrabImage(const ImageMsg::SharedPtr msg)
{

    // Copy the ros image message to cv::Mat.
    try
    {
        m_cvImPtr = cv_bridge::toCvCopy(msg);
    }
    catch (cv_bridge::Exception& e)
    {
        RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
        return;
    }
    
    cv::Mat Tcw = m_SLAM->TrackMonocular(m_cvImPtr->image, msg->header.stamp.sec);

}
