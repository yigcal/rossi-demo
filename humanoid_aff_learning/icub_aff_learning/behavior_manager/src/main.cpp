#include "BehaviorModule/BehaviorModule.h"
#include <iostream>

YARP_DECLARE_DEVICES(icubmod);
int main(int argc, char *argv[]) {
	ros::init(argc, argv, "action_manager");
	ros::NodeHandle n;
	std::cout << "init" << endl;
	Network yarp;

	if (!yarp.checkNetwork())
		return -1;

	YARP_REGISTER_DEVICES(icubmod)
	std::cout << "after yarp"  << endl;
	ResourceFinder rf;
	rf.setVerbose(true);
	rf.setDefaultContext("actionPrimitivesExample/conf");
	rf.setDefaultConfigFile("config.ini");
	rf.setDefault("part", "left_arm"); // "right_arm"
	rf.setDefault("grasp_model_type", "tactile");
	rf.setDefault("grasp_model_file", "grasp_model.ini");
	rf.setDefault("hand_sequences_file", "hand_sequences.ini");
	rf.setDefault("name", "actionPrimitivesMod");
	rf.setDefault("sim", "off"); // off
	rf.setDefault("robot", "icub");
	rf.configure("ICUB_ROOT", argc, argv);
	std::cout << "after ros" << endl;
	BehaviorModule mod(n);

	return mod.runModule(rf);
}
