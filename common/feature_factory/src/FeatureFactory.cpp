/*  Copyright (C) 2011, Kadir Firat Uyanik
 *    Middle East Technical University, Kovan Research Lab
 *    kadir@ceng.metu.edu.tr
 *
 *    http://kovan.ceng.metu.edu.tr/~kadir
 *
 *  FeatureFactory.cpp is part of feature_factory.
 *
 *  feature_factory is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  feature_factory is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with feature_factory. If not, see <http://www.gnu.org/licenses/>.
 */

#include "feature_factory/FeatureFactory.h"

namespace feature_factory
{
  FeatureFactory::FeatureFactory (const ros::NodeHandle& nh, int const& grid_edge_size_) :
    n (nh), n_it_img_d (n), n_it_img_a (n), n_it_img_c (n)
  {

    n_features_ = 0;

    grid_edge_size = grid_edge_size_;
    grid_data.resize (grid_edge_size * grid_edge_size);
    pc2_data.reset (new pcl::PointCloud<pcl::PointXYZ>);
    pc2_normals.reset (new pcl::PointCloud<pcl::Normal>);

    ne.setNormalEstimationMethod (ne.AVERAGE_DEPTH_CHANGE);
    ne.setMaxDepthChangeFactor (0.02f);
    ne.setNormalSmoothingSize (10.0f);

    n.getParam ("/use_sim_time", is_sim_);

    cvNamedWindow ("distance");

    if (is_sim_ == false)
    {
      cvNamedWindow ("confidence");
      cvNamedWindow ("amplitude");

      sub_pc2 = n.subscribe ("/swissranger/pointcloud2_raw", 10, &FeatureFactory::pc2RcvdCallBack, this);

      sub_img_d = n_it_img_d.subscribe ("/swissranger/distance/image_raw", 10, &FeatureFactory::distanceRcvdCallback,
                                        this);
      sub_img_c = n_it_img_c.subscribe ("/swissranger/confidence/image_raw", 10,
                                        &FeatureFactory::confidenceRcvdCallback, this);
      sub_img_a = n_it_img_a.subscribe ("/swissranger/intensity/image_raw", 10, &FeatureFactory::amplitudeRcvdCallback,
                                        this);
    }
    else
    {
      sub_pc = n.subscribe ("/swissranger/scan", 10, &FeatureFactory::pcRcvdCallBack, this);

      pub_pc2 = n.advertise<sensor_msgs::PointCloud2> ("/swissranger/pointcloud2_raw", 10);
    }

    pub_features = n.advertise<sensor_msgs::PointCloud2> ("/distances", 10);
  }

  FeatureFactory::~FeatureFactory ()
  {
    // TODO Auto-generated destructor stub
  }

  bool
  FeatureFactory::calcFeaturesCallBack (FeatureCalculation::Request& request, FeatureCalculation::Response& response)
  {
    return true;
  }
  void
  FeatureFactory::pc2RcvdCallBack (const sensor_msgs::PointCloud2::ConstPtr& pc_2)
  {
    pc2_ = *pc_2;
    pc2_buffer.push (pc2_);
  }
  void
  FeatureFactory::pcRcvdCallBack (const sensor_msgs::PointCloud::ConstPtr& pc)
  {
    // NOTE: This didn't work

    //    sensor_msgs::convertPointCloudToPointCloud2 (*pc, pc2_);
    //
    //    sensor_msgs::PointCloud2 p;
    //    p.data = pc2_.data;
    //    p.fields.resize (3);
    //    p.fields[0] = pc2_.fields[0];
    //    p.fields[1] = pc2_.fields[1];
    //    p.fields[2] = pc2_.fields[2];
    //
    //    p.header = pc2_.header;
    //    p.height = pc2_.width;
    //    p.width = pc2_.height;
    //    p.point_step = pc2_.point_step*3/4;
    //    p.row_step = pc2_.row_step*3/4;
    //    p.is_bigendian = pc2_.is_bigendian;

    pc2_data->header = pc->header;
    pc2_data->height = RANGE_IMAGE_HEIGHT;
    pc2_data->width = RANGE_IMAGE_WIDTH;
    pc2_data->is_dense = true; //there are no invalid points

    pc2_data->points.resize (pc->points.size ());
    for (uint i = 0; i < pc->points.size (); i++)
    {
      //      pc2_data->points[i].x = pc->points[i].x;
      pc2_data->points[i].x = pc->points[pc->points.size () - 1 - i].x;
      pc2_data->points[i].y = pc->points[pc->points.size () - 1 - i].y;
      pc2_data->points[i].z = pc->points[pc->points.size () - 1 - i].z;
    }

    pcl::toROSMsg (*pc2_data, pc2_);
    pc2_buffer.push (pc2_);

    if (pub_pc2.getNumSubscribers () > 0)
      pub_pc2.publish (pc2_);

  }

  void
  FeatureFactory::distanceRcvdCallback (const sensor_msgs::ImageConstPtr& image_d)
  {
    img_d = *image_d;
    img_d_buffer.push (img_d);
  }

  void
  FeatureFactory::amplitudeRcvdCallback (const sensor_msgs::ImageConstPtr& image_a)
  {
    img_a = *image_a;
    img_a_buffer.push (img_a);
  }

  void
  FeatureFactory::confidenceRcvdCallback (const sensor_msgs::ImageConstPtr& image_c)
  {
    img_c = *image_c;
    img_c_buffer.push (img_c);
  }

  void
  FeatureFactory::extractSurfaceNFeatures (pcl::PointCloud<pcl::Normal>::Ptr pc2_normals, pcl::PointCloud<
      feature_factory::FeaturePoint<SURFACE_FEATURES_HISTOGRAM> > &surface_zen_features, pcl::PointCloud<
      feature_factory::FeaturePoint<SURFACE_FEATURES_HISTOGRAM> > &surface_azi_features)
  {
    cv::Mat normals_zen = cv::Mat (RANGE_IMAGE_HEIGHT, RANGE_IMAGE_WIDTH, CV_32F);
    cv::Mat normals_azi = cv::Mat (RANGE_IMAGE_HEIGHT, RANGE_IMAGE_WIDTH, CV_32F);

    for (uint16_t ri = 0; ri < RANGE_IMAGE_HEIGHT; ri++)
      for (uint16_t ci = 0; ci < RANGE_IMAGE_WIDTH; ci++)
      {
        double n_x = pc2_normals->points[ri * RANGE_IMAGE_WIDTH + ci].normal_x;
        double n_y = pc2_normals->points[ri * RANGE_IMAGE_WIDTH + ci].normal_y;
        double n_z = pc2_normals->points[ri * RANGE_IMAGE_WIDTH + ci].normal_z;

        //always return [0, PI]
        normals_zen.at<float> (ri, ci) = atan2 (sqrt (n_x * n_x + n_y * n_y), n_z);

        //may return [-PI, PI]
        normals_azi.at<float> (ri, ci) = atan2 (n_y, n_x);
        if (normals_azi.at<float> (ri, ci) < 0)
          normals_azi.at<float> (ri, ci) += 2 * PI;
      }

    const uint16_t n_grids_per_row = RANGE_IMAGE_WIDTH / grid_edge_size;
    const uint16_t n_grids_per_col = RANGE_IMAGE_HEIGHT / grid_edge_size;
    const uint16_t n_grids = n_grids_per_col * n_grids_per_row;

    surface_azi_features.resize (n_grids);
    surface_zen_features.resize (n_grids);
    for (uint16_t gri = 0; gri < n_grids_per_col; gri++)
      for (uint16_t gci = 0; gci < n_grids_per_row; gci++)
      {
        cv::Rect roi = cv::Rect (gci * grid_edge_size, gri * grid_edge_size, grid_edge_size, grid_edge_size);
        surface_azi_features[gri * n_grids_per_row + gci]
            = feature_factory::FeaturePoint<SURFACE_FEATURES_HISTOGRAM>::calculate (normals_azi, roi, 0, 2 * PI);
        surface_zen_features[gri * n_grids_per_row + gci]
            = feature_factory::FeaturePoint<SURFACE_FEATURES_HISTOGRAM>::calculate (normals_zen, roi, 0, PI);
      }
  }

  void
  FeatureFactory::extractRangeFeature (cv_bridge::CvImagePtr cv_ptr, pcl::PointCloud<feature_factory::FeaturePoint<
      RANGE_FEATURES_HISTOGRAM> > &range_features, FeatureType feature_type)
  {
    double max_feature_val;
    if (feature_type == DEPTH)
      max_feature_val = MAX_DEPTH_FEATURE_VAL;
    else
      max_feature_val = MAX_CONF_AMP_FEATURE_VAL;

    const uint16_t n_grids_per_row = RANGE_IMAGE_WIDTH / grid_edge_size;
    const uint16_t n_grids_per_col = RANGE_IMAGE_HEIGHT / grid_edge_size;
    const uint16_t n_grids = n_grids_per_col * n_grids_per_row;

    range_features.resize (n_grids);
    for (uint16_t gri = 0; gri < n_grids_per_col; gri++)
      for (uint16_t gci = 0; gci < n_grids_per_row; gci++)
      {
        // Hint: Rect(top_left_x, top_left_y, width, height)
        range_features[gri * n_grids_per_row + gci]
            = feature_factory::FeaturePoint<RANGE_FEATURES_HISTOGRAM>::calculate (cv_ptr->image, cv::Rect (gci
                * grid_edge_size, gri * grid_edge_size, grid_edge_size, grid_edge_size), 0, max_feature_val);
      }
  }

  bool
  FeatureFactory::convertPointCloud2ToDistImg (const sensor_msgs::PointCloud2& pc_2, cv::Mat& dist_img)
  {
    pcl::PointCloud<pcl::PointXYZ> cloud;
    pcl::fromROSMsg (pc_2, cloud);
    dist_img = cv::Mat (RANGE_IMAGE_HEIGHT, RANGE_IMAGE_WIDTH, CV_16U);

    const double norm_to_16 = (double)MAX_DEPTH_FEATURE_VAL / MAX_RANGE;
    //    const double norm_to_8 = (double)MAX_CONF_AMP_FEATURE_VAL / MAX_DEPTH_FEATURE_VAL;

    for (uint i = 0; i < RANGE_IMAGE_HEIGHT; i++)
    {
      for (uint j = 0; j < RANGE_IMAGE_WIDTH; j++)
      {
        float x = cloud.points[i * RANGE_IMAGE_WIDTH + j].x;
        float y = cloud.points[i * RANGE_IMAGE_WIDTH + j].y;
        float z = cloud.points[i * RANGE_IMAGE_WIDTH + j].z;

        float dist = sqrt (x * x + y * y + z * z);
        dist *= norm_to_16; //re-scale for 16bit image format
        dist_img.at<uint16_t> (i, j) = (uint16_t)dist;
      }
    }
    return true;
  }

  bool
  FeatureFactory::extractFeatures ()
  {
    // check if a new point cloud received, or waiting for to be processed
    // you may handle windowing process here as well checking if the buffer size is larger than
    // the window size
    if (pc2_buffer.size () > 0)
    {
      if (is_sim_)
      {
        //        std::cout << "asd" << std::endl;
        cleanFeatures ();

        //pop the latest point cloud
        sensor_msgs::PointCloud2* cloud = &pc2_buffer.front ();
        // extract range features (just the distance)
        // but first obtain the distance image from the point cloud since it is not provided by
        // the gazebo.
        cv_bridge::CvImagePtr cv_ptr (new cv_bridge::CvImage);
        convertPointCloud2ToDistImg (*cloud, cv_ptr->image);
        cv::imshow ("distance", cv_ptr->image);
        cv::waitKey (2);

        extractRangeFeature (cv_ptr, distances, (FeatureType)DEPTH);

        pcl::fromROSMsg (pc2_buffer.front (), *pc2_data);
        ne.setInputCloud (pc2_data);
        ne.compute (*pc2_normals);

        // extract surface features
        extractSurfaceNFeatures (pc2_normals, normals_zen, normals_azi);

        n_features_++;
        pc2_buffer.pop ();
        //        std::cout << n_features_ << std::endl;
      }
      else
      {
        //wait for depth, amplitude and confidence buffers to be filled
        if (img_d_buffer.size () > 0 && img_a_buffer.size () > 0 && img_c_buffer.size ())
        {
          //first clear previously obtained features
          cleanFeatures ();

          //pop the latest point cloud
          pcl::fromROSMsg (pc2_buffer.front (), *pc2_data);
          ne.setInputCloud (pc2_data);
          ne.compute (*pc2_normals);
          pc2_buffer.pop ();

          // extract surface features
          extractSurfaceNFeatures (pc2_normals, normals_zen, normals_azi);

          // TODO: use toCvShared instead of toCvCopy here. But you should change the
          // image input argument correspondingly
          cv_bridge::CvImagePtr cv_ptr = cv_bridge::toCvCopy (img_d_buffer.front (),
                                                              sensor_msgs::image_encodings::MONO16);
          img_d_buffer.pop ();
          extractRangeFeature (cv_ptr, distances, (FeatureType)DEPTH);

          cv_ptr = cv_bridge::toCvCopy (img_a_buffer.front (), sensor_msgs::image_encodings::MONO8);
          img_a_buffer.pop ();
          cv::imshow ("amplitude", cv_ptr->image);
          cv::waitKey (2);
          extractRangeFeature (cv_ptr, amplitudes, (FeatureType)AMP);

          cv_ptr = cv_bridge::toCvCopy (img_c_buffer.front (), sensor_msgs::image_encodings::MONO8);
          img_c_buffer.pop ();
          cv::imshow ("confidence", cv_ptr->image);
          cv::waitKey (2);
          extractRangeFeature (cv_ptr, confidences, (FeatureType)CONF);

          n_features_++;
        }
      }
    }
    else
      return false;

    return true;
  }

  void
  FeatureFactory::saveFeatures ()
  {
    //save feature vector
    std::stringstream s;
    s << n_features_;
    pcl::io::savePCDFile ("dist_" + s.str () + ".pcd", distances);
    pcl::io::savePCDFile ("n_azi_" + s.str () + ".pcd", normals_azi);
    pcl::io::savePCDFile ("n_zen_" + s.str () + ".pcd", normals_zen);
  }

  void
  FeatureFactory::sendFeatures ()
  {
    sensor_msgs::PointCloud2 msg;
    pcl::toROSMsg (distances, msg);
    if (pub_features.getNumSubscribers () > 0)
      pub_features.publish (msg);
  }

  void
  FeatureFactory::visFeatures()
  {

//   vis_histogram.addFeatureHistogram()
  }

  void
  FeatureFactory::cleanFeatures ()
  {
    distances.clear ();
    confidences.clear ();
    amplitudes.clear ();

    normals_azi.clear ();
    normals_zen.clear ();
  }

  bool
  FeatureFactory::run ()
  {
    while (n.ok ())
    {
      /*
      if (extractFeatures ())
      {
        saveFeatures ();
      }
      */
      ros::spinOnce ();
    }
    return false;
  }
}
