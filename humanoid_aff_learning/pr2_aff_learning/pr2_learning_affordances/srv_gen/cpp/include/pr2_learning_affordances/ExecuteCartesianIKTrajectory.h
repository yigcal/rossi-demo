/* Auto-generated by genmsg_cpp for file /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/pr2_aff_learning/pr2_learning_affordances/srv/ExecuteCartesianIKTrajectory.srv */
#ifndef PR2_LEARNING_AFFORDANCES_SERVICE_EXECUTECARTESIANIKTRAJECTORY_H
#define PR2_LEARNING_AFFORDANCES_SERVICE_EXECUTECARTESIANIKTRAJECTORY_H
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/time.h"

#include "ros/macros.h"

#include "ros/assert.h"

#include "ros/service_traits.h"

#include "std_msgs/Header.h"
#include "geometry_msgs/Pose.h"



namespace pr2_learning_affordances
{
template <class ContainerAllocator>
struct ExecuteCartesianIKTrajectoryRequest_ {
  typedef ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> Type;

  ExecuteCartesianIKTrajectoryRequest_()
  : header()
  , poses()
  {
  }

  ExecuteCartesianIKTrajectoryRequest_(const ContainerAllocator& _alloc)
  : header(_alloc)
  , poses(_alloc)
  {
  }

  typedef  ::std_msgs::Header_<ContainerAllocator>  _header_type;
   ::std_msgs::Header_<ContainerAllocator>  header;

  typedef std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  _poses_type;
  std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other >  poses;


  ROS_DEPRECATED uint32_t get_poses_size() const { return (uint32_t)poses.size(); }
  ROS_DEPRECATED void set_poses_size(uint32_t size) { poses.resize((size_t)size); }
  ROS_DEPRECATED void get_poses_vec(std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other > & vec) const { vec = this->poses; }
  ROS_DEPRECATED void set_poses_vec(const std::vector< ::geometry_msgs::Pose_<ContainerAllocator> , typename ContainerAllocator::template rebind< ::geometry_msgs::Pose_<ContainerAllocator> >::other > & vec) { this->poses = vec; }
private:
  static const char* __s_getDataType_() { return "pr2_learning_affordances/ExecuteCartesianIKTrajectoryRequest"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "916c28c5764443f268b296bb671b9d97"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "d904d75e10f01066c74bf87962ffff22"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "Header header\n\
geometry_msgs/Pose[] poses\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.secs: seconds (stamp_secs) since epoch\n\
# * stamp.nsecs: nanoseconds since stamp_secs\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, header);
    ros::serialization::serialize(stream, poses);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, header);
    ros::serialization::deserialize(stream, poses);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(header);
    size += ros::serialization::serializationLength(poses);
    return size;
  }

  typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct ExecuteCartesianIKTrajectoryRequest
typedef  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<std::allocator<void> > ExecuteCartesianIKTrajectoryRequest;

typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest> ExecuteCartesianIKTrajectoryRequestPtr;
typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest const> ExecuteCartesianIKTrajectoryRequestConstPtr;


template <class ContainerAllocator>
struct ExecuteCartesianIKTrajectoryResponse_ {
  typedef ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> Type;

  ExecuteCartesianIKTrajectoryResponse_()
  : success(0)
  {
  }

  ExecuteCartesianIKTrajectoryResponse_(const ContainerAllocator& _alloc)
  : success(0)
  {
  }

  typedef uint32_t _success_type;
  uint32_t success;


private:
  static const char* __s_getDataType_() { return "pr2_learning_affordances/ExecuteCartesianIKTrajectoryResponse"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "952fc3b34491dbd8ce5170ced8b373fa"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "d904d75e10f01066c74bf87962ffff22"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "uint32 success\n\
\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, success);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, success);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(success);
    return size;
  }

  typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct ExecuteCartesianIKTrajectoryResponse
typedef  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<std::allocator<void> > ExecuteCartesianIKTrajectoryResponse;

typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse> ExecuteCartesianIKTrajectoryResponsePtr;
typedef boost::shared_ptr< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse const> ExecuteCartesianIKTrajectoryResponseConstPtr;

struct ExecuteCartesianIKTrajectory
{

typedef ExecuteCartesianIKTrajectoryRequest Request;
typedef ExecuteCartesianIKTrajectoryResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;
}; // struct ExecuteCartesianIKTrajectory
} // namespace pr2_learning_affordances

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "916c28c5764443f268b296bb671b9d97";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x916c28c5764443f2ULL;
  static const uint64_t static_value2 = 0x68b296bb671b9d97ULL;
};

template<class ContainerAllocator>
struct DataType< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "pr2_learning_affordances/ExecuteCartesianIKTrajectoryRequest";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "Header header\n\
geometry_msgs/Pose[] poses\n\
\n\
================================================================================\n\
MSG: std_msgs/Header\n\
# Standard metadata for higher-level stamped data types.\n\
# This is generally used to communicate timestamped data \n\
# in a particular coordinate frame.\n\
# \n\
# sequence ID: consecutively increasing ID \n\
uint32 seq\n\
#Two-integer timestamp that is expressed as:\n\
# * stamp.secs: seconds (stamp_secs) since epoch\n\
# * stamp.nsecs: nanoseconds since stamp_secs\n\
# time-handling sugar is provided by the client library\n\
time stamp\n\
#Frame this data is associated with\n\
# 0: no frame\n\
# 1: global frame\n\
string frame_id\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Pose\n\
# A representation of pose in free space, composed of postion and orientation. \n\
Point position\n\
Quaternion orientation\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Point\n\
# This contains the position of a point in free space\n\
float64 x\n\
float64 y\n\
float64 z\n\
\n\
================================================================================\n\
MSG: geometry_msgs/Quaternion\n\
# This represents an orientation in free space in quaternion form.\n\
\n\
float64 x\n\
float64 y\n\
float64 z\n\
float64 w\n\
\n\
";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct HasHeader< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct HasHeader< const ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros


namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "952fc3b34491dbd8ce5170ced8b373fa";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x952fc3b34491dbd8ULL;
  static const uint64_t static_value2 = 0xce5170ced8b373faULL;
};

template<class ContainerAllocator>
struct DataType< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "pr2_learning_affordances/ExecuteCartesianIKTrajectoryResponse";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "uint32 success\n\
\n\
\n\
";
  }

  static const char* value(const  ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct IsFixedSize< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.header);
    stream.next(m.poses);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ExecuteCartesianIKTrajectoryRequest_
} // namespace serialization
} // namespace ros


namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.success);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ExecuteCartesianIKTrajectoryResponse_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace service_traits
{
template<>
struct MD5Sum<pr2_learning_affordances::ExecuteCartesianIKTrajectory> {
  static const char* value() 
  {
    return "d904d75e10f01066c74bf87962ffff22";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectory&) { return value(); } 
};

template<>
struct DataType<pr2_learning_affordances::ExecuteCartesianIKTrajectory> {
  static const char* value() 
  {
    return "pr2_learning_affordances/ExecuteCartesianIKTrajectory";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectory&) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "d904d75e10f01066c74bf87962ffff22";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "pr2_learning_affordances/ExecuteCartesianIKTrajectory";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectoryRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "d904d75e10f01066c74bf87962ffff22";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "pr2_learning_affordances/ExecuteCartesianIKTrajectory";
  }

  static const char* value(const pr2_learning_affordances::ExecuteCartesianIKTrajectoryResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace service_traits
} // namespace ros

#endif // PR2_LEARNING_AFFORDANCES_SERVICE_EXECUTECARTESIANIKTRAJECTORY_H

