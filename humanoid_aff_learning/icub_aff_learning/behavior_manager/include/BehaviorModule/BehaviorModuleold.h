#ifndef BEHAVIORMODULE_H_
#define BEHAVIORMODULE_H_

#include <yarp/os/Network.h>
#include <yarp/os/RFModule.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/sig/Vector.h>
#include <yarp/os/Time.h>
#include <yarp/os/Semaphore.h>
#include <yarp/math/Math.h>
#include <yarp/dev/Drivers.h>
#include <yarp/dev/GazeControl.h>

#include <iCub/perception/models.h>
#include <iCub/action/actionPrimitives.h>
#include <iCub/ctrl/math.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <deque>
#include <pthread.h>
#include <math.h>

//ros includes
#include "ros/ros.h"
#include "aff_msgs/ExperimentState.h"
#include "aff_msgs/Features.h"
#include "behavior_manager/Action.h"

/* Data logging functionalities */

#include "DataLogger.h"

#define PI 3.14159265

#define HYSTERESIS_THRES 1
#define CONTACT_THRES 4

YARP_DECLARE_DEVICES(icubmod)

using namespace std;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;
using namespace yarp::math;
using namespace iCub::perception;
using namespace iCub::action;

const int REACH_X_LIMIT = -0.45;

class BehaviorModule: public RFModule {
protected:
	ActionPrimitivesLayer2 *action_left;
	ActionPrimitivesLayer2 *action_right;

	Property options_left;
	Property options_right;
	Property options_torso;
	Property options_gaze;
	Property options_head;

	PolyDriver driver_right;
	PolyDriver driver_left;
	PolyDriver driver_head;
	PolyDriver driver_torso;
	PolyDriver driver_gaze;

	IPositionControl* pos_ctrl_left;
	IPositionControl* pos_ctrl_right;
	IPositionControl* pos_ctrl_head;
	IPositionControl* pos_ctrl_torso;
	IGazeControl* igaze;

	IEncoders* encoders_right;
	IEncoders* encoders_left;
	IEncoders *encoders_head;
	IEncoders *encoders_torso;

	Vector positions_left_cmd;
	Vector positions_left_enc;
	Vector positions_right_cmd;
	Vector positions_right_enc;
	Vector positions_head_enc;
	Vector positions_head_cmd;
	Vector positions_torso_enc;
	Vector positions_torso_cmd;

	Vector graspOrien;
	Vector graspDisp;
	Vector dOffs;
	Vector dLift;
	Vector home_x;

	bool openPorts;
	bool firstRun;
	bool sim;
	string robotName;

	/* Vector torques_left; */
	/* Vector torques_right; */

	string chosen_arm;
	BufferedPort<Bottle> port_grasp_comm_right;
	BufferedPort<Bottle> port_grasp_comm_left;

	ros::NodeHandle nh;
	ros::ServiceServer srv_action;

	/* BufferedPort<Bottle> port_sim_rpc_out; */
	/* BufferedPort<Bottle> port_simon_in; */
	/* BufferedPort<Bottle> port_grasp_out; */
	/* BufferedPort<Bottle> tactileReader_in; */


	Port rpcPort;
	/* yarp::os::Semaphore menu_mutex; */


	//ITorqueControl *trq_ctrl_left, *trq_ctrl_right;
	//IControlMode* ictrl;


	aff_msgs::ExperimentState exp_state_;

	ros::Publisher pub_internal_features_;

	std::vector<float> bb_center_;
	std::vector<float> bb_dims_;

	bool actionCallback(behavior_manager::Action::Request& request,
			behavior_manager::Action::Response& response);

	// ++Onur

	/* DataLogger* armLogger, *headLogger, *tactileLogger; */
	/* FeatureTuple armPoseInfo; */
	/* FeatureTuple headPoseInfo; */
	/* FeatureTuple tactileInfo; */
	/* int actionTag; */

	/* int hysteresisCounter; */
	/* bool reflexiveHandClosed; */
	// --Onur

public:
	BehaviorModule(ros::NodeHandle& n);
	virtual ~BehaviorModule();

	void
	getArmDependentOptions(Bottle &b, Vector &_gOrien, Vector &_gDisp,
			Vector &_dOffs, Vector &_dLift, Vector &_home_x);

	virtual bool
	configure(ResourceFinder &rf);

	virtual bool
	close();

	virtual double
	getPeriod();

	void
	init();

	Vector
	vectorAngle2Normal(Vector vec_angle_rep);

	Vector
	normal2VectorAngle(const Vector& hand_normal);

	Vector
	angleXZToVectorAngle(const double x_ang, const double z_ang);

	Vector
	angleXYZToVectorAngle(const double x_ang, const double y_ang,
			const double z_ang);

	//bb_pos and bb_dims specifies the bounding box properties of the object of interest
	//push_dir is the angle(in radians). 0 -> right, PI/2 -> forward, PI -> left, 3*PI/2 -> backward
	/* void */
	/* push(const Vector& bb_center, const Vector& bb_dims, double push_dir_angle, */
	/* 		const double poi_shift, bool spin = false); */

	/* void */
	/* push2(const Vector& bb_center, const Vector& bb_dims, */
	/* 		double push_dir_angle, const double poi_shift, bool is_left_arm, */
	/* 		bool spin = false); */

	/* void */
	/* lift(const double poi_shift); */

	/* void */
	/* hide(Vector bb_center, Vector bb_dims); */

	void pointTo(Vector point);

	void release(Vector point, bool palm_upward = true);

	/* void releaseUpward(Vector point); */

	/* void releaseDownward(Vector point); */

	void choseArm(double y_position);
	/* void */
	/* drop(Vector point); */

	/* void */
	/* grasp(Vector bb_center); */

	/* void */
	/* graspLiftAndRelease(Vector bb_center, Vector bb_target_center, */
	/* 		const double shift, bool palm_upward = false); */

	/* void doReflexiveGrasp(); */

	void home(bool is_left_arm = false);

	void tuckArms();

	void lookAtPoint(Vector bb_center);

	//void lookAtRegion(uint region_id);

	void lookAtFace();

	/* void take(); */
	/* void give(); */
	/* void giveAfterTake(); */
	void reach(Vector bb_center, Vector bb_dims); 
	void push_right(Vector bb_center, Vector bb_dims, bool isUpper);
	void push_left(Vector bb_center, Vector bb_dims, bool isUpper);
	void cover(Vector bb_center, Vector bb_dims);


	/* int */
	/* voiceCommand(Vector bb_center, Vector bb_dims); */

	/* void */
	/* manualCommand(Vector bb_center, Vector bb_dims); */

	//from top or sides (left, right, back)
	/* void */
	/* fetch(Vector bb_center, Vector bb_dims, Vector hand_orient); */

	/* void */
	/* shake(const Vector dir, const double shake_off); */

	/* void */
	/* basket(Vector bb_center, Vector bb_dims); */

	void testHandSequences();

	/* void */
	/* createObject(int op_type, Vector& bb_center); */

	/* void */
	/* deleteObject(); */

	/* void grasp2(Vector bb_upward); */


	//++Onur

	void openHand();
	// we don't need a thread since the actions library already
	// incapsulates one inside dealing with all the tight time constraints
	virtual bool updateModule();

	bool interruptModule();

	/* // ++Onur */
	/* void getEEPosition(); // hand end effector position */
	/* void logPosData(uint8_t task_id); // */
	/* void getHeadJoints(); */
	/* void logTactileData(); */

	//	void obtainAction();
	// --Onur

};

#endif /* BEHAVIORMODULE_H_ */
