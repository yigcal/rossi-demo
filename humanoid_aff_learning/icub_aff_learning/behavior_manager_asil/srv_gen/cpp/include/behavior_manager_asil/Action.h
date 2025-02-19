/* Auto-generated by genmsg_cpp for file /home/kadir/interconnection/workspace/metu-ros-pkg/stacks/aff_learning/humanoid_aff_learning/icub_aff_learning/behavior_manager_asil/srv/Action.srv */
#ifndef BEHAVIOR_MANAGER_ASIL_SERVICE_ACTION_H
#define BEHAVIOR_MANAGER_ASIL_SERVICE_ACTION_H
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




namespace behavior_manager_asil
{
template <class ContainerAllocator>
struct ActionRequest_ {
  typedef ActionRequest_<ContainerAllocator> Type;

  ActionRequest_()
  : task(0)
  , pushable_object_center()
  , pushable_object_size()
  {
  }

  ActionRequest_(const ContainerAllocator& _alloc)
  : task(0)
  , pushable_object_center(_alloc)
  , pushable_object_size(_alloc)
  {
  }

  typedef int8_t _task_type;
  int8_t task;

  typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _pushable_object_center_type;
  std::vector<float, typename ContainerAllocator::template rebind<float>::other >  pushable_object_center;

  typedef std::vector<float, typename ContainerAllocator::template rebind<float>::other >  _pushable_object_size_type;
  std::vector<float, typename ContainerAllocator::template rebind<float>::other >  pushable_object_size;

  enum { DONT_ACT = -1 };
  enum { PUSH_RIGHT = 0 };
  enum { PUSH_LEFT = 1 };
  enum { PUSH_FORWARD = 2 };
  enum { PUSH_BACKWARD = 3 };
  enum { LIFT = 4 };
  enum { GRASP = 5 };
  enum { HOME = 6 };
  enum { POINT = 7 };
  enum { HIDE = 8 };
  enum { CANCEL = 9 };
  enum { STOP = 10 };
  enum { SHOW = 11 };
  enum { TUCK_ARMS = 12 };

  ROS_DEPRECATED uint32_t get_pushable_object_center_size() const { return (uint32_t)pushable_object_center.size(); }
  ROS_DEPRECATED void set_pushable_object_center_size(uint32_t size) { pushable_object_center.resize((size_t)size); }
  ROS_DEPRECATED void get_pushable_object_center_vec(std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) const { vec = this->pushable_object_center; }
  ROS_DEPRECATED void set_pushable_object_center_vec(const std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) { this->pushable_object_center = vec; }
  ROS_DEPRECATED uint32_t get_pushable_object_size_size() const { return (uint32_t)pushable_object_size.size(); }
  ROS_DEPRECATED void set_pushable_object_size_size(uint32_t size) { pushable_object_size.resize((size_t)size); }
  ROS_DEPRECATED void get_pushable_object_size_vec(std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) const { vec = this->pushable_object_size; }
  ROS_DEPRECATED void set_pushable_object_size_vec(const std::vector<float, typename ContainerAllocator::template rebind<float>::other > & vec) { this->pushable_object_size = vec; }
private:
  static const char* __s_getDataType_() { return "behavior_manager_asil/ActionRequest"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "6f28010420559760ce532a04644ecab7"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "9900281ecfdf116827776adaa867e6d8"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "int8 DONT_ACT = -1\n\
int8 PUSH_RIGHT = 0\n\
int8 PUSH_LEFT =1\n\
int8 PUSH_FORWARD =2\n\
int8 PUSH_BACKWARD=3\n\
int8 LIFT=4\n\
int8 GRASP=5\n\
int8 HOME=6\n\
int8 POINT=7\n\
int8 HIDE=8\n\
int8 CANCEL=9\n\
int8 STOP=10\n\
int8 SHOW=11\n\
int8 TUCK_ARMS=12\n\
int8 task\n\
float32[] pushable_object_center\n\
float32[] pushable_object_size\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, task);
    ros::serialization::serialize(stream, pushable_object_center);
    ros::serialization::serialize(stream, pushable_object_size);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, task);
    ros::serialization::deserialize(stream, pushable_object_center);
    ros::serialization::deserialize(stream, pushable_object_size);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(task);
    size += ros::serialization::serializationLength(pushable_object_center);
    size += ros::serialization::serializationLength(pushable_object_size);
    return size;
  }

  typedef boost::shared_ptr< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::behavior_manager_asil::ActionRequest_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct ActionRequest
typedef  ::behavior_manager_asil::ActionRequest_<std::allocator<void> > ActionRequest;

typedef boost::shared_ptr< ::behavior_manager_asil::ActionRequest> ActionRequestPtr;
typedef boost::shared_ptr< ::behavior_manager_asil::ActionRequest const> ActionRequestConstPtr;


template <class ContainerAllocator>
struct ActionResponse_ {
  typedef ActionResponse_<ContainerAllocator> Type;

  ActionResponse_()
  : feedback(0)
  {
  }

  ActionResponse_(const ContainerAllocator& _alloc)
  : feedback(0)
  {
  }

  typedef uint8_t _feedback_type;
  uint8_t feedback;

  enum { DONE = 1 };

private:
  static const char* __s_getDataType_() { return "behavior_manager_asil/ActionResponse"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "0c33925582a4cf8db9f48c73fab8a9c4"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "9900281ecfdf116827776adaa867e6d8"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "\n\
uint8 DONE = 1\n\
uint8 feedback\n\
\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, feedback);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, feedback);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(feedback);
    return size;
  }

  typedef boost::shared_ptr< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::behavior_manager_asil::ActionResponse_<ContainerAllocator>  const> ConstPtr;
  boost::shared_ptr<std::map<std::string, std::string> > __connection_header;
}; // struct ActionResponse
typedef  ::behavior_manager_asil::ActionResponse_<std::allocator<void> > ActionResponse;

typedef boost::shared_ptr< ::behavior_manager_asil::ActionResponse> ActionResponsePtr;
typedef boost::shared_ptr< ::behavior_manager_asil::ActionResponse const> ActionResponseConstPtr;

struct Action
{

typedef ActionRequest Request;
typedef ActionResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;
}; // struct Action
} // namespace behavior_manager_asil

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::behavior_manager_asil::ActionRequest_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "6f28010420559760ce532a04644ecab7";
  }

  static const char* value(const  ::behavior_manager_asil::ActionRequest_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x6f28010420559760ULL;
  static const uint64_t static_value2 = 0xce532a04644ecab7ULL;
};

template<class ContainerAllocator>
struct DataType< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "behavior_manager_asil/ActionRequest";
  }

  static const char* value(const  ::behavior_manager_asil::ActionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "int8 DONT_ACT = -1\n\
int8 PUSH_RIGHT = 0\n\
int8 PUSH_LEFT =1\n\
int8 PUSH_FORWARD =2\n\
int8 PUSH_BACKWARD=3\n\
int8 LIFT=4\n\
int8 GRASP=5\n\
int8 HOME=6\n\
int8 POINT=7\n\
int8 HIDE=8\n\
int8 CANCEL=9\n\
int8 STOP=10\n\
int8 SHOW=11\n\
int8 TUCK_ARMS=12\n\
int8 task\n\
float32[] pushable_object_center\n\
float32[] pushable_object_size\n\
\n\
";
  }

  static const char* value(const  ::behavior_manager_asil::ActionRequest_<ContainerAllocator> &) { return value(); } 
};

} // namespace message_traits
} // namespace ros


namespace ros
{
namespace message_traits
{
template<class ContainerAllocator> struct IsMessage< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > : public TrueType {};
template<class ContainerAllocator> struct IsMessage< ::behavior_manager_asil::ActionResponse_<ContainerAllocator>  const> : public TrueType {};
template<class ContainerAllocator>
struct MD5Sum< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "0c33925582a4cf8db9f48c73fab8a9c4";
  }

  static const char* value(const  ::behavior_manager_asil::ActionResponse_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x0c33925582a4cf8dULL;
  static const uint64_t static_value2 = 0xb9f48c73fab8a9c4ULL;
};

template<class ContainerAllocator>
struct DataType< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "behavior_manager_asil/ActionResponse";
  }

  static const char* value(const  ::behavior_manager_asil::ActionResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "\n\
uint8 DONE = 1\n\
uint8 feedback\n\
\n\
\n\
";
  }

  static const char* value(const  ::behavior_manager_asil::ActionResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator> struct IsFixedSize< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> > : public TrueType {};
} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::behavior_manager_asil::ActionRequest_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.task);
    stream.next(m.pushable_object_center);
    stream.next(m.pushable_object_size);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ActionRequest_
} // namespace serialization
} // namespace ros


namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::behavior_manager_asil::ActionResponse_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.feedback);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ActionResponse_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace service_traits
{
template<>
struct MD5Sum<behavior_manager_asil::Action> {
  static const char* value() 
  {
    return "9900281ecfdf116827776adaa867e6d8";
  }

  static const char* value(const behavior_manager_asil::Action&) { return value(); } 
};

template<>
struct DataType<behavior_manager_asil::Action> {
  static const char* value() 
  {
    return "behavior_manager_asil/Action";
  }

  static const char* value(const behavior_manager_asil::Action&) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<behavior_manager_asil::ActionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "9900281ecfdf116827776adaa867e6d8";
  }

  static const char* value(const behavior_manager_asil::ActionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<behavior_manager_asil::ActionRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "behavior_manager_asil/Action";
  }

  static const char* value(const behavior_manager_asil::ActionRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<behavior_manager_asil::ActionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "9900281ecfdf116827776adaa867e6d8";
  }

  static const char* value(const behavior_manager_asil::ActionResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<behavior_manager_asil::ActionResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "behavior_manager_asil/Action";
  }

  static const char* value(const behavior_manager_asil::ActionResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace service_traits
} // namespace ros

#endif // BEHAVIOR_MANAGER_ASIL_SERVICE_ACTION_H

