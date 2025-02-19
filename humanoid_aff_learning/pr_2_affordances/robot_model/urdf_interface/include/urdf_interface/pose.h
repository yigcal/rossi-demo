/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Wim Meeussen */

#ifndef URDF_INTERFACE_POSE_H
#define URDF_INTERFACE_POSE_H

#include <string>
#include <vector>
#include <math.h>
#include <ros/ros.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <tinyxml.h> // FIXME: remove parser from here
#include <ros/ros.h>

namespace urdf{

class Vector3
{
public:
  Vector3(double _x,double _y, double _z) {this->x=_x;this->y=_y;this->z=_z;};
  Vector3() {this->clear();};
  double x;
  double y;
  double z;

  void clear() {this->x=this->y=this->z=0.0;};
  bool init(const std::string &vector_str)
  {
    this->clear();
    std::vector<std::string> pieces;
    std::vector<double> xyz;
    boost::split( pieces, vector_str, boost::is_any_of(" "));
    for (unsigned int i = 0; i < pieces.size(); ++i){
      if (pieces[i] != ""){
        try
        {
          xyz.push_back(boost::lexical_cast<double>(pieces[i].c_str()));
        }
        catch (boost::bad_lexical_cast &e)
        {
          ROS_ERROR("Vector3 xyz element (%s) is not a valid float",pieces[i].c_str());
          return false;
        }
      }
    }

    if (xyz.size() != 3) {
	ROS_ERROR("Vector contains %i elements instead of 3 elements", (int)xyz.size()); 
      return false;
    }

    this->x = xyz[0];
    this->y = xyz[1];
    this->z = xyz[2];

    return true;
  };
  Vector3 operator+(Vector3 vec)
  {
    return Vector3(this->x+vec.x,this->y+vec.y,this->z+vec.z);
  };
};

class Rotation
{
public:
  Rotation(double _x,double _y, double _z, double _w) {this->x=_x;this->y=_y;this->z=_z;this->w=_w;};
  Rotation() {this->clear();};
  void getQuaternion(double &quat_x,double &quat_y,double &quat_z, double &quat_w) const
  {
    quat_x = this->x;
    quat_y = this->y;
    quat_z = this->z;
    quat_w = this->w;
  };
  void getRPY(double &roll,double &pitch,double &yaw) const
  {
    double sqw;
    double sqx;
    double sqy;
    double sqz;

    sqx = this->x * this->x;
    sqy = this->y * this->y;
    sqz = this->z * this->z;
    sqw = this->w * this->w;

    roll  = atan2(2 * (this->y*this->z + this->w*this->x), sqw - sqx - sqy + sqz);
    double sarg = -2 * (this->x*this->z - this->w*this->y);
    pitch = sarg <= -1.0 ? -0.5*M_PI : (sarg >= 1.0 ? 0.5*M_PI : asin(sarg));
    yaw   = atan2(2 * (this->x*this->y + this->w*this->z), sqw + sqx - sqy - sqz);

  };
  void setFromQuaternion(double quat_x,double quat_y,double quat_z,double quat_w)
  {
    this->x = quat_x;
    this->y = quat_y;
    this->z = quat_z;
    this->w = quat_w;
    this->normalize();
  };
  void setFromRPY(double roll, double pitch, double yaw)
  {
    double phi, the, psi;

    phi = roll / 2.0;
    the = pitch / 2.0;
    psi = yaw / 2.0;

    this->x = sin(phi) * cos(the) * cos(psi) - cos(phi) * sin(the) * sin(psi);
    this->y = cos(phi) * sin(the) * cos(psi) + sin(phi) * cos(the) * sin(psi);
    this->z = cos(phi) * cos(the) * sin(psi) - sin(phi) * sin(the) * cos(psi);
    this->w = cos(phi) * cos(the) * cos(psi) + sin(phi) * sin(the) * sin(psi);

    this->normalize();
  };

  double x,y,z,w;

  bool init(const std::string &rotation_str)
  {
    this->clear();

    Vector3 rpy;
    
    if (!rpy.init(rotation_str))
      return false;
    else
    {
      this->setFromRPY(rpy.x,rpy.y,rpy.z);
      return true;
    }
      
  };

  void clear() { this->x=this->y=this->z=0.0;this->w=1.0; }

  void normalize()
  {
    double s = sqrt(this->x * this->x +
                    this->y * this->y +
                    this->z * this->z +
                    this->w * this->w);
    if (s == 0.0)
    {
      this->x = 0.0;
      this->y = 0.0;
      this->z = 0.0;
      this->w = 1.0;
    }
    else
    {
      this->x /= s;
      this->y /= s;
      this->z /= s;
      this->w /= s;
    }
  };

  // Multiplication operator (copied from gazebo)
  Rotation operator*( const Rotation &qt ) const
  {
    Rotation c;

    c.x = this->w * qt.x + this->x * qt.w + this->y * qt.z - this->z * qt.y;
    c.y = this->w * qt.y - this->x * qt.z + this->y * qt.w + this->z * qt.x;
    c.z = this->w * qt.z + this->x * qt.y - this->y * qt.x + this->z * qt.w;
    c.w = this->w * qt.w - this->x * qt.x - this->y * qt.y - this->z * qt.z;

    return c;
  };
  /// Rotate a vector using the quaternion
  Vector3 operator*(Vector3 vec) const
  {
    Rotation tmp;
    Vector3 result;

    tmp.w = 0.0;
    tmp.x = vec.x;
    tmp.y = vec.y;
    tmp.z = vec.z;

    tmp = (*this) * (tmp * this->GetInverse());

    result.x = tmp.x;
    result.y = tmp.y;
    result.z = tmp.z;

    return result;
  };
  // Get the inverse of this quaternion
  Rotation GetInverse() const 
  {
    Rotation q;

    double norm = this->w*this->w+this->x*this->x+this->y*this->y+this->z*this->z;

    if (norm > 0.0)
    {
      q.w = this->w / norm;
      q.x = -this->x / norm;
      q.y = -this->y / norm;
      q.z = -this->z / norm;
    }

    return q;
  };


};

class Pose
{
public:
  Pose() { this->clear(); };

  Vector3  position;
  Rotation rotation;

  void clear()
  {
    this->position.clear();
    this->rotation.clear();
  };
  bool initXml(TiXmlElement* xml)
  {
    this->clear();
    if (!xml)
    {
      ROS_DEBUG("parsing pose: xml empty");
      return false;
    }
    else
    {
      const char* xyz_str = xml->Attribute("xyz");
      if (xyz_str == NULL)
      {
        ROS_DEBUG("parsing pose: no xyz, using default values.");
        return true;
      }
      else
      {
        if (!this->position.init(xyz_str))
        {
          ROS_ERROR("malformed xyz");
          this->position.clear();
          return false;
        }
      }

      const char* rpy_str = xml->Attribute("rpy");
      if (rpy_str == NULL)
      {
        ROS_DEBUG("parsing pose: no rpy, using default values.");
        return true;
      }
      else
      {
        if (!this->rotation.init(rpy_str))
        {
          ROS_ERROR("malformed rpy");
          return false;
          this->rotation.clear();
        }
      }

      return true;
    }
  };
};

}

#endif
