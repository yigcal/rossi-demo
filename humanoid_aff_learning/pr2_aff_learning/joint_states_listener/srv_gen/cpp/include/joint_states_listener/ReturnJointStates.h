/* Auto-generated by genmsg_cpp for file /home/kadir/interconnection/workspace/to_do/Dropbox/kovan/pr_2_affordances/joint_states_listener/srv/ReturnJointStates.srv */
#ifndef JOINT_STATES_LISTENER_SERVICE_RETURNJOINTSTATES_H
#define JOINT_STATES_LISTENER_SERVICE_RETURNJOINTSTATES_H
#include <string>
#include <vector>
#include <ostream>
#include "ros/serialization.h"
#include "ros/builtin_message_traits.h"
#include "ros/message_operations.h"
#include "ros/message.h"
#include "ros/time.h"

#include "ros/service_traits.h"




namespace joint_states_listener
{
template <class ContainerAllocator>
struct ReturnJointStatesRequest_ : public ros::Message
{
  typedef ReturnJointStatesRequest_<ContainerAllocator> Type;

  ReturnJointStatesRequest_()
  : name()
  {
  }

  ReturnJointStatesRequest_(const ContainerAllocator& _alloc)
  : name(_alloc)
  {
  }

  typedef std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  _name_type;
  std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other >  name;


  ROS_DEPRECATED uint32_t get_name_size() const { return (uint32_t)name.size(); }
  ROS_DEPRECATED void set_name_size(uint32_t size) { name.resize((size_t)size); }
  ROS_DEPRECATED void get_name_vec(std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other > & vec) const { vec = this->name; }
  ROS_DEPRECATED void set_name_vec(const std::vector<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > , typename ContainerAllocator::template rebind<std::basic_string<char, std::char_traits<char>, typename ContainerAllocator::template rebind<char>::other > >::other > & vec) { this->name = vec; }
private:
  static const char* __s_getDataType_() { return "joint_states_listener/ReturnJointStatesRequest"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "3f2d21c30868b92dc41a0431bacd47b2"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "ce9bd2b56c904b190a782a08482fb4e9"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "string[] name\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, name);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, name);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(name);
    return size;
  }

  typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator>  const> ConstPtr;
}; // struct ReturnJointStatesRequest
typedef  ::joint_states_listener::ReturnJointStatesRequest_<std::allocator<void> > ReturnJointStatesRequest;

typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesRequest> ReturnJointStatesRequestPtr;
typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesRequest const> ReturnJointStatesRequestConstPtr;


template <class ContainerAllocator>
struct ReturnJointStatesResponse_ : public ros::Message
{
  typedef ReturnJointStatesResponse_<ContainerAllocator> Type;

  ReturnJointStatesResponse_()
  : found()
  , position()
  , velocity()
  , effort()
  {
  }

  ReturnJointStatesResponse_(const ContainerAllocator& _alloc)
  : found(_alloc)
  , position(_alloc)
  , velocity(_alloc)
  , effort(_alloc)
  {
  }

  typedef std::vector<uint32_t, typename ContainerAllocator::template rebind<uint32_t>::other >  _found_type;
  std::vector<uint32_t, typename ContainerAllocator::template rebind<uint32_t>::other >  found;

  typedef std::vector<double, typename ContainerAllocator::template rebind<double>::other >  _position_type;
  std::vector<double, typename ContainerAllocator::template rebind<double>::other >  position;

  typedef std::vector<double, typename ContainerAllocator::template rebind<double>::other >  _velocity_type;
  std::vector<double, typename ContainerAllocator::template rebind<double>::other >  velocity;

  typedef std::vector<double, typename ContainerAllocator::template rebind<double>::other >  _effort_type;
  std::vector<double, typename ContainerAllocator::template rebind<double>::other >  effort;


  ROS_DEPRECATED uint32_t get_found_size() const { return (uint32_t)found.size(); }
  ROS_DEPRECATED void set_found_size(uint32_t size) { found.resize((size_t)size); }
  ROS_DEPRECATED void get_found_vec(std::vector<uint32_t, typename ContainerAllocator::template rebind<uint32_t>::other > & vec) const { vec = this->found; }
  ROS_DEPRECATED void set_found_vec(const std::vector<uint32_t, typename ContainerAllocator::template rebind<uint32_t>::other > & vec) { this->found = vec; }
  ROS_DEPRECATED uint32_t get_position_size() const { return (uint32_t)position.size(); }
  ROS_DEPRECATED void set_position_size(uint32_t size) { position.resize((size_t)size); }
  ROS_DEPRECATED void get_position_vec(std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) const { vec = this->position; }
  ROS_DEPRECATED void set_position_vec(const std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) { this->position = vec; }
  ROS_DEPRECATED uint32_t get_velocity_size() const { return (uint32_t)velocity.size(); }
  ROS_DEPRECATED void set_velocity_size(uint32_t size) { velocity.resize((size_t)size); }
  ROS_DEPRECATED void get_velocity_vec(std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) const { vec = this->velocity; }
  ROS_DEPRECATED void set_velocity_vec(const std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) { this->velocity = vec; }
  ROS_DEPRECATED uint32_t get_effort_size() const { return (uint32_t)effort.size(); }
  ROS_DEPRECATED void set_effort_size(uint32_t size) { effort.resize((size_t)size); }
  ROS_DEPRECATED void get_effort_vec(std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) const { vec = this->effort; }
  ROS_DEPRECATED void set_effort_vec(const std::vector<double, typename ContainerAllocator::template rebind<double>::other > & vec) { this->effort = vec; }
private:
  static const char* __s_getDataType_() { return "joint_states_listener/ReturnJointStatesResponse"; }
public:
  ROS_DEPRECATED static const std::string __s_getDataType() { return __s_getDataType_(); }

  ROS_DEPRECATED const std::string __getDataType() const { return __s_getDataType_(); }

private:
  static const char* __s_getMD5Sum_() { return "3a36649f5b1439b638a41d18af93e9a4"; }
public:
  ROS_DEPRECATED static const std::string __s_getMD5Sum() { return __s_getMD5Sum_(); }

  ROS_DEPRECATED const std::string __getMD5Sum() const { return __s_getMD5Sum_(); }

private:
  static const char* __s_getServerMD5Sum_() { return "ce9bd2b56c904b190a782a08482fb4e9"; }
public:
  ROS_DEPRECATED static const std::string __s_getServerMD5Sum() { return __s_getServerMD5Sum_(); }

  ROS_DEPRECATED const std::string __getServerMD5Sum() const { return __s_getServerMD5Sum_(); }

private:
  static const char* __s_getMessageDefinition_() { return "uint32[] found\n\
float64[] position\n\
float64[] velocity\n\
float64[] effort\n\
\n\
\n\
"; }
public:
  ROS_DEPRECATED static const std::string __s_getMessageDefinition() { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED const std::string __getMessageDefinition() const { return __s_getMessageDefinition_(); }

  ROS_DEPRECATED virtual uint8_t *serialize(uint8_t *write_ptr, uint32_t seq) const
  {
    ros::serialization::OStream stream(write_ptr, 1000000000);
    ros::serialization::serialize(stream, found);
    ros::serialization::serialize(stream, position);
    ros::serialization::serialize(stream, velocity);
    ros::serialization::serialize(stream, effort);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint8_t *deserialize(uint8_t *read_ptr)
  {
    ros::serialization::IStream stream(read_ptr, 1000000000);
    ros::serialization::deserialize(stream, found);
    ros::serialization::deserialize(stream, position);
    ros::serialization::deserialize(stream, velocity);
    ros::serialization::deserialize(stream, effort);
    return stream.getData();
  }

  ROS_DEPRECATED virtual uint32_t serializationLength() const
  {
    uint32_t size = 0;
    size += ros::serialization::serializationLength(found);
    size += ros::serialization::serializationLength(position);
    size += ros::serialization::serializationLength(velocity);
    size += ros::serialization::serializationLength(effort);
    return size;
  }

  typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > Ptr;
  typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator>  const> ConstPtr;
}; // struct ReturnJointStatesResponse
typedef  ::joint_states_listener::ReturnJointStatesResponse_<std::allocator<void> > ReturnJointStatesResponse;

typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesResponse> ReturnJointStatesResponsePtr;
typedef boost::shared_ptr< ::joint_states_listener::ReturnJointStatesResponse const> ReturnJointStatesResponseConstPtr;

struct ReturnJointStates
{

typedef ReturnJointStatesRequest Request;
typedef ReturnJointStatesResponse Response;
Request request;
Response response;

typedef Request RequestType;
typedef Response ResponseType;
}; // struct ReturnJointStates
} // namespace joint_states_listener

namespace ros
{
namespace message_traits
{
template<class ContainerAllocator>
struct MD5Sum< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "3f2d21c30868b92dc41a0431bacd47b2";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x3f2d21c30868b92dULL;
  static const uint64_t static_value2 = 0xc41a0431bacd47b2ULL;
};

template<class ContainerAllocator>
struct DataType< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "joint_states_listener/ReturnJointStatesRequest";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "string[] name\n\
\n\
";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> &) { return value(); } 
};

} // namespace message_traits
} // namespace ros


namespace ros
{
namespace message_traits
{
template<class ContainerAllocator>
struct MD5Sum< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "3a36649f5b1439b638a41d18af93e9a4";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> &) { return value(); } 
  static const uint64_t static_value1 = 0x3a36649f5b1439b6ULL;
  static const uint64_t static_value2 = 0x38a41d18af93e9a4ULL;
};

template<class ContainerAllocator>
struct DataType< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "joint_states_listener/ReturnJointStatesResponse";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct Definition< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "uint32[] found\n\
float64[] position\n\
float64[] velocity\n\
float64[] effort\n\
\n\
\n\
";
  }

  static const char* value(const  ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace message_traits
} // namespace ros

namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.name);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ReturnJointStatesRequest_
} // namespace serialization
} // namespace ros


namespace ros
{
namespace serialization
{

template<class ContainerAllocator> struct Serializer< ::joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> >
{
  template<typename Stream, typename T> inline static void allInOne(Stream& stream, T m)
  {
    stream.next(m.found);
    stream.next(m.position);
    stream.next(m.velocity);
    stream.next(m.effort);
  }

  ROS_DECLARE_ALLINONE_SERIALIZER;
}; // struct ReturnJointStatesResponse_
} // namespace serialization
} // namespace ros

namespace ros
{
namespace service_traits
{
template<>
struct MD5Sum<joint_states_listener::ReturnJointStates> {
  static const char* value() 
  {
    return "ce9bd2b56c904b190a782a08482fb4e9";
  }

  static const char* value(const joint_states_listener::ReturnJointStates&) { return value(); } 
};

template<>
struct DataType<joint_states_listener::ReturnJointStates> {
  static const char* value() 
  {
    return "joint_states_listener/ReturnJointStates";
  }

  static const char* value(const joint_states_listener::ReturnJointStates&) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "ce9bd2b56c904b190a782a08482fb4e9";
  }

  static const char* value(const joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> > {
  static const char* value() 
  {
    return "joint_states_listener/ReturnJointStates";
  }

  static const char* value(const joint_states_listener::ReturnJointStatesRequest_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct MD5Sum<joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "ce9bd2b56c904b190a782a08482fb4e9";
  }

  static const char* value(const joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> &) { return value(); } 
};

template<class ContainerAllocator>
struct DataType<joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> > {
  static const char* value() 
  {
    return "joint_states_listener/ReturnJointStates";
  }

  static const char* value(const joint_states_listener::ReturnJointStatesResponse_<ContainerAllocator> &) { return value(); } 
};

} // namespace service_traits
} // namespace ros

#endif // JOINT_STATES_LISTENER_SERVICE_RETURNJOINTSTATES_H

