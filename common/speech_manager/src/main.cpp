/*
 * main.cpp
 *
 *  Created on: Oct 26, 2011
 *      Author: kadir
 */

#include "iostream"
#include "string"

#include "ros/ros.h"
#include "rosconsole/macros_generated.h"
#include "aff_msgs/Speech.h"
#include "std_msgs/String.h"

const std::string speech_topic_name = "/icub/output";

std::string speech_msg_;
aff_msgs::Speech speech_cmd_;
bool speech_msg_rcvd_ = false;
bool new_speech_msg_rcvd_ = false;

void
speechCallback (std_msgs::String::ConstPtr msg)
{
  if (!speech_msg_rcvd_)
    speech_msg_rcvd_ = true;

  new_speech_msg_rcvd_ = true;
  speech_msg_ = msg->data;
}

int8_t
enumerateSpeechCommand (const std::string& speech_msg)
{
  int8_t cmd = -1;

  if (!speech_msg.compare ("icub push right"))
  {
    cmd = aff_msgs::Speech::PUSH_RIGHT;
    ROS_INFO("icub push right");
  }
  else if (!speech_msg.compare ("icub push left"))
  {
    cmd = aff_msgs::Speech::PUSH_LEFT;
    ROS_INFO("icub push left");
  }
  else if (!speech_msg.compare ("icub push forward"))
  {
    cmd = aff_msgs::Speech::PUSH_FORWARD;
    ROS_INFO("icub push forward");
  }
  else if (!speech_msg.compare ("icub push backward"))
  {
    cmd = aff_msgs::Speech::PUSH_BACKWARD;
    ROS_INFO("icub push backward");
  }
  else if (!speech_msg.compare ("icub push reach"))
  {
    cmd = aff_msgs::Speech::REACH;
    ROS_INFO("icub reach");
  }
  else if (!speech_msg.compare ("icub home"))
  {
    cmd = aff_msgs::Speech::HOME;
    ROS_INFO("icub home");
  }
  else if (!speech_msg.compare ("icub point"))
  {
    cmd = aff_msgs::Speech::POINT;
    ROS_INFO("icub point");
  }
  else if (!speech_msg.compare ("icub hide"))
  {
    cmd = aff_msgs::Speech::HIDE;
    ROS_INFO("icub hide");
  }
  else if (!speech_msg.compare ("icub stop"))
  {
    cmd = aff_msgs::Speech::STOP;
    ROS_INFO("icub stop");
  }
  else if (!speech_msg.compare ("icub cancel"))
  {
    cmd = aff_msgs::Speech::CANCEL;
    ROS_INFO("icub cancel");
  }
  else if (!speech_msg.compare ("effect disappeared"))
  {
    cmd = aff_msgs::Speech::DISAPPEARED;
    ROS_INFO("effect disappeared");
  }
  else if (!speech_msg.compare ("effect pushed left"))
  {
    cmd = aff_msgs::Speech::PUSHED_LEFT;
    ROS_INFO("effect pushed left");
  }
  else if (!speech_msg.compare ("effect pushed right"))
  {
    cmd = aff_msgs::Speech::PUSHED_LEFT;
    ROS_INFO("effect pushed right");
  }
  else if (!speech_msg.compare ("effect pushed forward"))
  {
    cmd = aff_msgs::Speech::PUSHED_FORWARD;
    ROS_INFO("effect pushed forward");
  }
  else if (!speech_msg.compare ("effect pushed backward"))
  {
    cmd = aff_msgs::Speech::PUSHED_BACKWARD;
    ROS_INFO("effect pushed backward");
  }
  else if (!speech_msg.compare ("effect don't care"))
  {
    cmd = aff_msgs::Speech::DONTCARE;
    ROS_INFO("effect don't care");
  }
  else if (!speech_msg.compare ("no change"))
  {
    cmd = aff_msgs::Speech::NO_CHANGE;
    ROS_INFO("effect no change");
  }

  return cmd;
}

int8_t
enumerateSpeechCommand3 (const std::string& speech_msg, int8_t& cmd_arg)
{
  int8_t cmd = -1;
  cmd_arg = -1;
  //TODO action argument passing (e.g. object one, two; box, sphere etc.)

  if ((int)speech_msg.find ("icub") == 0) //icub is addressed
  {
    if ((int)speech_msg.find ("push right") == 5 || (int)speech_msg.find ("pushed right") == 5)//action
    {
      cmd = aff_msgs::Speech::PUSH_RIGHT;
      ROS_INFO("icub push right");
    }
    else if ((int)speech_msg.find ("push left") == 5 || (int)speech_msg.find ("pushed left") == 5)
    {
      cmd = aff_msgs::Speech::PUSH_LEFT;
      ROS_INFO("icub push LEFT");
    }
    else if ((int)speech_msg.find ("push forward") == 5 || (int)speech_msg.find ("pushed forward") == 5)
    {
      cmd = aff_msgs::Speech::PUSH_FORWARD;
      ROS_INFO("icub push FORWARD");
    }
    else if ((int)speech_msg.find ("push backward") == 5 || (int)speech_msg.find ("pushed backward") == 5)
    {
      cmd = aff_msgs::Speech::PUSH_BACKWARD;
      ROS_INFO("icub push BACKWARD");
    }
    else if ((int)speech_msg.find ("show") == 5)
    {
      cmd = aff_msgs::Speech::SHOW;
      ROS_INFO("icub show");
    }
    else if ((int)speech_msg.find ("home") == 5)
    {
      cmd = aff_msgs::Speech::HOME;
      ROS_INFO("icub home");
      return cmd;//home doesn't take any argument
    }
    else if((int)speech_msg.find ("grasp") == 5)
    {
      cmd = aff_msgs::Speech::GRASP;
      ROS_INFO("icub grasp");
    }
    else if((int)speech_msg.find ("reach") == 5)
    {
      cmd = aff_msgs::Speech::REACH;
      ROS_INFO("icub reach");
    }
    else if((int)speech_msg.find ("take") == 5)
    {
      cmd = aff_msgs::Speech::TAKE;
      ROS_INFO("icub take");
    }
    else if((int)speech_msg.find ("give") == 5)
    {
      cmd = aff_msgs::Speech::GIVE;
      ROS_INFO("icub give");
    }
    else if((int)speech_msg.find ("release") == 5)
    {
      cmd = aff_msgs::Speech::RELEASE;
      ROS_INFO("icub release");
    }
    else if((int)speech_msg.find ("continue") == 5)
    {
      cmd = aff_msgs::Speech::CONTINUE;
      ROS_INFO("icub continue");
    }
    else
      return -1;//invalid action

    size_t arg_found = speech_msg.find ("object");
    if (arg_found != std::string::npos)
    {
      if ((int)speech_msg.find ("one") == (int)arg_found + 7)
      {
        cmd_arg = 1;
        ROS_INFO("object one");
      }
      else if ((int)speech_msg.find ("zero") == (int)arg_found + 7)
      {
        cmd_arg = 0;
        ROS_INFO("object zero");
      }
      else if ((int)speech_msg.find ("two") == (int)arg_found + 7)
      {
        cmd_arg = 2;
        ROS_INFO("object two");
      }
      else if ((int)speech_msg.find ("three") == (int)arg_found + 7)
      {
        cmd_arg = 3;
        ROS_INFO("object three");
      }
      else if ((int)speech_msg.find ("four") == (int)arg_found + 7)
      {
        cmd_arg = 4;
        ROS_INFO("object four");
      }
      else if ((int)speech_msg.find ("five") == (int)arg_found + 7)
      {
        cmd_arg = 5;
        ROS_INFO("object five");
      }
      else if ((int)speech_msg.find ("six") == (int)arg_found + 7)
      {
        cmd_arg = 6;
        ROS_INFO("object six");
      }
      else if ((int)speech_msg.find ("seven") == (int)arg_found + 7)
      {
        cmd_arg = 7;
        ROS_INFO("object seven");
      }
      else if ((int)speech_msg.find ("eight") == (int)arg_found + 7)
      {
        cmd_arg = 8;
        ROS_INFO("object eight");
      }
      else if ((int)speech_msg.find ("nine") == (int)arg_found + 7)
      {
        cmd_arg = 9;
        ROS_INFO("object nine");
      }
      else if ((int)speech_msg.find ("home") == (int)arg_found + 7)
      {
        cmd_arg = 1;
        ROS_INFO("said home, interpreted as one");
      }
      //this is necessary since "object n" corresponds to "object n-1"
      cmd_arg--;
      if (cmd_arg < 0) // necessary precaution to avoid wrong outputs ('down', 'dont', etc.)
      {
        ROS_WARN("An invalid argument was tried to be passed to experiment manager");
        cmd = -1;
      }
      return cmd;
    }
  }
  else if ((int)speech_msg.find ("effect") == 0) // an effect is being issued
  {

    if (!speech_msg.compare ("effect disappeared"))
    {
      cmd = aff_msgs::Speech::DISAPPEARED;
      ROS_INFO("effect disappeared");
      return cmd;
    }
    else if (!speech_msg.compare ("effect pushed left"))
    {
      cmd = aff_msgs::Speech::PUSHED_LEFT;
      ROS_INFO("effect pushed left");
      return cmd;
    }
    else if (!speech_msg.compare ("effect pushed right"))
    {
      cmd = aff_msgs::Speech::PUSHED_LEFT;
      ROS_INFO("effect pushed right");
      return cmd;
    }
    else if (!speech_msg.compare ("effect pushed forward"))
    {
      cmd = aff_msgs::Speech::PUSHED_FORWARD;
      ROS_INFO("effect pushed forward");
      return cmd;
    }
    else if (!speech_msg.compare ("effect pushed backward"))
    {
      cmd = aff_msgs::Speech::PUSHED_BACKWARD;
      ROS_INFO("effect pushed backward");
      return cmd;
    }
    else if(!speech_msg.compare ("effect don't care"))
    {
      cmd = aff_msgs::Speech::DONTCARE;
      ROS_INFO("effect don't care");

      return cmd;
    }
    else if(!speech_msg.compare ("effect reached"))
    {
      cmd = aff_msgs::Speech::REACHED;
      ROS_INFO("effect reached");

      return cmd;
    }
    else if(!speech_msg.compare ("effect taken"))
    {
      cmd = aff_msgs::Speech::TAKEN;
      ROS_INFO("effect taken");

      return cmd;
    }
    else if(!speech_msg.compare ("effect given"))
    {
      cmd = aff_msgs::Speech::GIVEN;
      ROS_INFO("effect given");

      return cmd;
    }
    else if(!speech_msg.compare ("effect no change"))
    {
      cmd = aff_msgs::Speech::NO_CHANGE;
      ROS_INFO("effect no change");

      return cmd;
    }

  }

  return -1;
}

int8_t
enumerateSpeechCommand2 (const std::string& speech_msg, int8_t& cmd_arg)
{
  int8_t cmd = -1;
  cmd_arg = -1;
  //TODO action argument passing (e.g. object one, two; box, sphere etc.)

  size_t found = speech_msg.find ("icub push right");
  if (found != std::string::npos)
  {
    cmd = aff_msgs::Speech::PUSH_RIGHT;
    ROS_INFO("icub push right");
  }
  else
  {
    found = speech_msg.find ("icub push left");
    if (found != std::string::npos)
    {
      cmd = aff_msgs::Speech::PUSH_RIGHT;
      ROS_INFO("icub push LEFT");
    }
    else
    {
      found = speech_msg.find ("icub show");
      if (found != std::string::npos)
      {
        cmd = aff_msgs::Speech::SHOW;
        ROS_INFO("icub show");
      }
      else
      {
        found = speech_msg.find ("icub home");
        if (found != std::string::npos)
        {
          cmd = aff_msgs::Speech::HOME;
          ROS_INFO("icub home");
          return cmd;
        }
        else
          return cmd;
      }
    }
  }

  size_t arg_found = speech_msg.find ("object zero");
  if (arg_found != std::string::npos)
  {
    cmd_arg = 0;
    ROS_INFO("object zero");
    return cmd;
  }
  else
  {
    arg_found = speech_msg.find ("object one");
    if (arg_found != std::string::npos)
    {
      cmd_arg = 1;
      ROS_INFO("object one");
      return cmd;
    }
    else
    {
      arg_found = speech_msg.find ("object two");
      if (arg_found != std::string::npos)
      {
        cmd_arg = 2;
        ROS_INFO("object two");
        return cmd;
      }
      else
        return cmd;
    }
  }
}

int
main (int argc, char** argv)
{
  ros::init (argc, argv, "speech_manager");
  ros::NodeHandle nh;
  ros::Subscriber sub;
  ros::Publisher pub;
  sub = nh.subscribe (speech_topic_name, 2, &speechCallback);
  pub = nh.advertise<aff_msgs::Speech> ("/speech_command", 1);

  ROS_INFO("waiting for a speech command to be received!");
  while (nh.ok () && !speech_msg_rcvd_)
  {
    ros::spinOnce ();
  }

  ROS_INFO("first speech command received!");

  while (nh.ok ())
  {
    if (new_speech_msg_rcvd_)
    {
      new_speech_msg_rcvd_ = false;
      int8_t arg = -1;
      //      speech_cmd_.speech_cmd = enumerateSpeechCommand (speech_msg_);
      speech_cmd_.speech_cmd = enumerateSpeechCommand3 (speech_msg_, arg);
      speech_cmd_.speech_arg = arg;
      std::cout<<(int)speech_cmd_.speech_cmd<<" - "<<(int)speech_cmd_.speech_arg<<std::endl;
      if (speech_cmd_.speech_cmd != -1 && pub.getNumSubscribers () > 0)
      {
        pub.publish (speech_cmd_);
      }
    }
    ros::spinOnce ();
  }

  return 0;
}
