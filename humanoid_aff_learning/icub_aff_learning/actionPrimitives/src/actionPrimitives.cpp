/* 
 * Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Ugo Pattacini
 * email:  ugo.pattacini@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
 */

#include <yarp/os/Network.h>
#include <yarp/os/Time.h>
#include <yarp/sig/Matrix.h>
#include <yarp/math/Math.h>
#include <yarp/math/Rand.h>

#include <gsl/gsl_math.h>

#include <iCub/ctrl/math.h>
#include <iCub/perception/springyFingers.h>
#include <iCub/perception/tactileFingers.h>
#include <iCub/action/actionPrimitives.h>

#include <stdio.h>
#include <stdarg.h>
#include <string>

#define RES_WAVER(x)                                (dynamic_cast<ArmWavingMonitor*>(x))

#define ACTIONPRIM_DEFAULT_PER                      50      // [ms]
#define ACTIONPRIM_DEFAULT_EXECTIME                 3.0     // [s]
#define ACTIONPRIM_DEFAULT_REACHTOL                 0.005   // [m]
#define ACTIONPRIM_DUMP_PERIOD                      1.0     // [s]
#define ACTIONPRIM_DEFAULT_EXT_FORCE_THRES          1e9     // [N, Nm]
#define ACTIONPRIM_DEFAULT_PART                     "right_arm"
#define ACTIONPRIM_DEFAULT_TRACKINGMODE             "off"
#define ACTIONPRIM_DEFAULT_VERBOSITY                "off"
//++Kadir
#define ACTIONPRIM_DEFAULT_SIM		                "on"
//--Kadir

// defines for balancing the arm when in home position
#define ACTIONPRIM_BALANCEARM_PERIOD                2.0     // [s]
#define ACTIONPRIM_BALANCEARM_LENGTH                0.04    // [m]
using namespace std;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;
using namespace iCub::ctrl;
using namespace iCub::iDyn;
using namespace iCub::perception;
using namespace iCub::action;

// This class handles the automatic arm-waving
class ArmWavingMonitor: public RateThread {
	ICartesianControl *cartCtrl;
	Vector restPos;
	Vector q0, w0;
	double L;

public:
	/************************************************************************/
	ArmWavingMonitor(ICartesianControl *_cartCtrl) :
		RateThread((int) (1000 * ACTIONPRIM_BALANCEARM_PERIOD)) {
		cartCtrl = _cartCtrl;
		L = ACTIONPRIM_BALANCEARM_LENGTH;

		restPos.resize(1, 0.0);

		Rand::init();
	}

	/************************************************************************/
	void setRestPosition(const Vector &_restPos) {
		restPos = _restPos;
	}

	/************************************************************************/
	virtual void afterStart(bool s) {
		cartCtrl->getRestPos(q0);
		cartCtrl->getRestWeights(w0);

		// start in suspended mode
		disable();
	}

	/************************************************************************/
	virtual bool enable() {
		if (isSuspended()) {
			cartCtrl->getRestPos(q0);
			cartCtrl->getRestWeights(w0);

			// impose further constraints as third task
			// since we reach only in position and not in orientation:
			// keep the wrist aligned with the forearm
			Vector q = q0;
			Vector w = w0;
			q[3 + 5] = 0.0;
			w[3 + 5] = 1.0;

			cartCtrl->setRestPos(q, q);
			cartCtrl->setRestWeights(w, w);

			resume();

			return true;
		} else
			return false;
	}

	/************************************************************************/
	virtual bool disable() {
		if (!isSuspended()) {
			cartCtrl->setRestPos(q0, q0);
			cartCtrl->setRestWeights(w0, w0);

			suspend();

			return true;
		} else
			return false;
	}

	/************************************************************************/
	virtual void run() {
		int len = restPos.length();

		if ((cartCtrl != NULL) && (len >= 3)) {
			Vector halves(len);
			halves = 0.5;
			Vector randOffs = L * (Rand::vector(len) - halves);

			cartCtrl->goToPosition(restPos + randOffs);
		}
	}

	/************************************************************************/
	virtual ~ArmWavingMonitor() {
		// safety check: make sure to stop
		// the interface when closing
		cartCtrl->stopControl();
	}
};

/************************************************************************/
ActionPrimitives::ActionPrimitives() :
	RateThread(ACTIONPRIM_DEFAULT_PER) {
	init();
}

/************************************************************************/
ActionPrimitives::ActionPrimitives(Property &opt) :
	RateThread(ACTIONPRIM_DEFAULT_PER) {
	init();
	open(opt);
}

/************************************************************************/
void ActionPrimitives::init() {
	polyHand = polyCart = NULL;

	armWaver = NULL;
	actionClb = NULL;

	graspModel = NULL;

	armMoveDone = latchArmMoveDone = true;
	handMoveDone = latchHandMoveDone = true;
	configured = closed = false;
	checkEnabled = true;
	torsoActive = true;
	handSeqTerminator = false;
	fingersInPosition = true;
	reachTmoEnabled = false;
	locked = false;

	latchTimerWait = waitTmo = 0.0;
	latchTimerReach = reachTmo = 0.0;
	latchTimerHand = curHandTmo = 0.0;
	latchTimerReachLog = 0.0;
}

/************************************************************************/
bool ActionPrimitives::isValid() const {
	return configured;
}

/************************************************************************/
string ActionPrimitives::toCompactString(const Vector &v) {
	char buf[255];
	string ret;
	int i;

	for (i = 0; i < v.length() - 1; i++) {
		sprintf(buf, "%g ", v[i]);
		ret += buf;
	}

	sprintf(buf, "%g", v[i]);
	ret += buf;

	return ret;
}

/************************************************************************/
int ActionPrimitives::printMessage(const char *format, ...) {
	if (verbose) {
		fprintf(stdout, "*** %s: ", (local + "/" + part).c_str());

		va_list ap;
		va_start(ap, format);
		int ret = vfprintf(stdout, format, ap);
		va_end(ap);

		return ret;
	} else
		return -1;
}

/************************************************************************/
bool ActionPrimitives::handleTorsoDOF(Property &opt, const string &key,
		const int j) {
	if (opt.check(key.c_str())) {
		bool sw = opt.find(key.c_str()).asString() == "on" ? true : false;

		Vector newDof, dummyRet;
		newDof.resize(3, 2.0);
		newDof[j] = sw ? 1.0 : 0.0;

		printMessage("%s %s\n", key.c_str(), sw ? "enabled" : "disabled");
		cartCtrl->setDOF(newDof, dummyRet);

		if (sw) {
			string minKey = key + "_min";
			string maxKey = key + "_max";
			double min, max;

			cartCtrl->getLimits(j, &min, &max);

			min = opt.check(minKey.c_str(), Value(min)).asDouble();
			max = opt.check(maxKey.c_str(), Value(max)).asDouble();

			cartCtrl->setLimits(j, min, max);
			cartCtrl->getLimits(j, &min, &max);

			Vector weights;
			if (cartCtrl->getRestWeights(weights)) {
				// enforce the torso rest position
				weights[j] = 4.0;
				cartCtrl->setRestWeights(weights, weights);
			}

			printMessage("%s limits: [%g,%g] deg; weight=%g\n", key.c_str(),
					min, max, weights[j]);
		}

		return true;
	}

	return false;
}

/************************************************************************/
bool ActionPrimitives::configHandSeq(Property &opt) {
	if (opt.check("hand_sequences_file")) {
		string handSeqFile = opt.find("hand_sequences_file").asString().c_str();

		printMessage("Processing %s file\n", handSeqFile.c_str());
		Property handSeqProp;
		handSeqProp.fromConfigFile(handSeqFile.c_str());

		// GENERAL group
		Bottle &bGeneral = handSeqProp.findGroup("GENERAL");
		if (bGeneral.isNull()) {
			printMessage("WARNING: \"GENERAL\" group is missing\n");
			return false;
		}

		if (!bGeneral.check("numSequences")) {
			printMessage("WARNING: \"numSequences\" option is missing\n");
			return false;
		}

		int numSequences = bGeneral.find("numSequences").asInt();

		// SEQUENCE groups
		for (int i = 0; i < numSequences; i++) {
			char seq[255];
			sprintf(seq, "SEQ_%d", i);

			Bottle &bSeq = handSeqProp.findGroup(seq);
			if (bSeq.isNull()) {
				printMessage("WARNING: \"%s\" group is missing\n", seq);
				return false;
			}

			if (!bSeq.check("key")) {
				printMessage("WARNING: \"key\" option is missing\n");
				return false;
			}

			string key = bSeq.find("key").asString().c_str();

			if (isValidHandSeq(key)) {
				printMessage(
						"WARNING: the sequence \"%s\" is already defined: skipping ...\n",
						key.c_str());
				continue;
			}

			addHandSequence(key, bSeq);
		}

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::configGraspModel(Property &opt) {
	if (opt.check("grasp_model_type")) {
		string modelType = opt.find("grasp_model_type").asString().c_str();
		if (modelType != "none") {
			//++Kadir
			if (modelType == "springy" && sim) {
				printMessage(
						"ERROR: springy grasp model has no use in the simulator, EXITING!\n");
				return false;
			}
			//--Kadir

			if (opt.check("grasp_model_file")) {
				if (modelType == "springy")
					graspModel = new SpringyFingersModel;
				else if (modelType == "tactile")
					graspModel = new TactileFingersModel;
				else {
					printMessage("WARNING: unknown grasp model type %s!\n",
							modelType.c_str());
					return false;
				}

				string modelFile =
						opt.find("grasp_model_file").asString().c_str();
				printMessage("Retrieving grasp model data from %s file\n",
						modelFile.c_str());
				Property modelProp;
				modelProp.fromConfigFile(modelFile.c_str());

				// consistency check between the model and the part
				if (modelProp.check("type")) {
					string type = modelProp.find("type").asString().c_str();
					type += "_arm";
					if (type != part) {
						printMessage(
								"WARNING: attempt to instantiate a grasp model of type %s while part is %s\n",
								type.c_str(), part.c_str());
						return false;
					}
				}

				return graspModel->fromProperty(modelProp);
			} else
				printMessage(
						"WARNING: unable to find \"grasp_model_file\" option!\n");
		}
	}

	return false;
}

/************************************************************************/
bool ActionPrimitives::open(Property &opt) {
	if (configured) {
		printMessage("WARNING: already configured\n");
		return true;
	}

	if (!opt.check("local")) {
		printMessage("ERROR: option \"local\" is missing\n");
		return false;
	}
	sim
			= opt.check("sim", Value(ACTIONPRIM_DEFAULT_SIM)).asString()
					== "on" ? true : false;
	robot = opt.check("robot", Value("icub")).asString().c_str();
	local = opt.find("local").asString().c_str();
	part = opt.check("part", Value(ACTIONPRIM_DEFAULT_PART)).asString().c_str();
	default_exec_time = opt.check("default_exec_time", Value(
			ACTIONPRIM_DEFAULT_EXECTIME)).asDouble();
	tracking_mode = opt.check("tracking_mode", Value(
			ACTIONPRIM_DEFAULT_TRACKINGMODE)).asString() == "on" ? true : false;
	verbose
			= opt.check("verbosity", Value(ACTIONPRIM_DEFAULT_VERBOSITY)).asString()
					== "on" ? true : false;

	int period =
			opt.check("thread_period", Value(ACTIONPRIM_DEFAULT_PER)).asInt();
	double reach_tol = opt.check("reach_tol",
			Value(ACTIONPRIM_DEFAULT_REACHTOL)).asDouble();

	// create the model for grasp detection (if any)
	configGraspModel(opt);

	// get hand sequence motions (if any)
	configHandSeq(opt);

	// open the position client
	Property optPolyHand("(device remote_controlboard)");
	optPolyHand.put("remote", ("/" + robot + "/" + part).c_str());
	optPolyHand.put("local", ("/" + local + "/" + part + "/position").c_str());

	polyHand = new PolyDriver;
	if (!polyHand->open(optPolyHand)) {
		close();
		return false;
	}

	// open the cartesian client
	Property optPolyCart("(device cartesiancontrollerclient)");
	optPolyCart.put("remote",
			("/" + robot + "/cartesianController/" + part).c_str());
	optPolyCart.put("local", ("/" + local + "/" + part + "/cartesian").c_str());

	polyCart = new PolyDriver;
	if (!polyCart->open(optPolyCart)) {
		close();
		return false;
	}

	// open views
	polyHand->view(encCtrl);
	polyHand->view(posCtrl);
	polyCart->view(cartCtrl);

	// latch the controller context
	cartCtrl->storeContext(&startup_context_id);

	// set tolerance
	cartCtrl->setInTargetTol(reach_tol);

	// set tracking mode
	setTrackingMode(tracking_mode);

	// handle torso DOF's
	handleTorsoDOF(opt, "torso_pitch", 0);
	handleTorsoDOF(opt, "torso_roll", 1);
	handleTorsoDOF(opt, "torso_yaw", 2);

	Vector curDof;
	cartCtrl->getDOF(curDof);

	enableTorsoSw.resize(3, 0.0);
	disableTorsoSw.resize(3, 0.0);
	for (int i = 0; i < 3; i++)
		enableTorsoSw[i] = curDof[i];

	// start with torso disabled
	disableTorsoDof();

	jHandMin = 7; // hand first joint
	posCtrl->getAxes(&jHandMax); // hand last joint

	// hand joints set
	for (int j = jHandMin; j < jHandMax; j++)
		fingersJntsSet.insert(j);

	// map from hand joints to fingers
	fingers2JntsMap.insert(pair<int, int> (0, 8));
	fingers2JntsMap.insert(pair<int, int> (0, 9));
	fingers2JntsMap.insert(pair<int, int> (0, 10));
	fingers2JntsMap.insert(pair<int, int> (1, 11));
	fingers2JntsMap.insert(pair<int, int> (1, 12));
	fingers2JntsMap.insert(pair<int, int> (2, 13));
	fingers2JntsMap.insert(pair<int, int> (2, 14));
	fingers2JntsMap.insert(pair<int, int> (3, 15));
	fingers2JntsMap.insert(pair<int, int> (4, 15));

	// start the thread with the specified period
	Time::turboBoost();
	setRate(period);
	start();

	// start the balancer thread
	armWaver = new ArmWavingMonitor(cartCtrl);
	armWaver->start();

	return configured = true;
}

/************************************************************************/
void ActionPrimitives::close() {
	if (closed)
		return;

	if (armWaver != NULL) {
		printMessage("stopping balancer thread ...\n");
		stop();

		delete armWaver;
	}

	if (isRunning()) {
		printMessage("stopping main thread ...\n");
		stop();
	}

	if ((polyHand != NULL) && (polyCart != NULL)) {
		if (polyHand->isValid() && polyCart->isValid())
			stopControl();
	}

	if (polyHand != NULL) {
		printMessage("closing hand driver ...\n");
		delete polyHand;
	}

	if (polyCart != NULL) {
		printMessage("closing cartesian driver ...\n");
		cartCtrl->restoreContext(startup_context_id);
		delete polyCart;
	}

	if (graspModel != NULL)
		delete graspModel;

	closed = true;
}

/************************************************************************/
//TODO: Study on this method
bool ActionPrimitives::isHandSeqEnded() {
	// latch the current moving fingers set
	set<int> tmpSet = fingersMovingJntsSet;

	for (set<int>::iterator i = fingersMovingJntsSet.begin(); i
			!= fingersMovingJntsSet.end(); ++i) {
		if (handCheckMotionDone(*i))
			tmpSet.erase(*i);
	}

	// get data from the grasp model
	if (graspModel != NULL) {
		Value out;
		graspModel->getOutput(out);
		Bottle *pB = out.asList();

		// span over fingers
		for (int fng = 0; fng < 5; fng++) {
			double val = pB->get(fng).asDouble();
			double thres = curGraspDetectionThres[fng];

			// detect contact on the finger
			if (val > thres) {
				fingersInPosition = false;

				// take joints belonging to the finger
				pair<multimap<int, int>::iterator, multimap<int, int>::iterator>
						i = fingers2JntsMap.equal_range(fng);

				for (multimap<int, int>::iterator j = i.first; j != i.second; ++j) {
					int jnt = j->second;

					// stop and remove if not done yet
					if (tmpSet.find(jnt) != tmpSet.end()) {
						printMessage(
								"contact detected on finger %d: (%g>%g) => stopping joint %d\n",
								fng, val, thres, jnt);

						stopJntTraj(jnt);
						tmpSet.erase(jnt);
					}
				}
			}
		}
	}

	// handle hand timeout
	if ((Time::now() - latchTimerHand) > curHandTmo) {
		printMessage("timeout (%g) expired on hand WP\n", curHandTmo);

		for (set<int>::iterator i = fingersMovingJntsSet.begin(); i
				!= fingersMovingJntsSet.end(); ++i) {
			stopJntTraj(*i);
			tmpSet.erase(*i);
		}
	}

	// update the moving fingers set
	fingersMovingJntsSet = tmpSet;

	if (fingersMovingJntsSet.size())
		return false;
	else
		return true;
}

/************************************************************************/
void ActionPrimitives::postReachCallback() {
	latchArmMoveDone = armMoveDone = false;
}

/************************************************************************/
bool ActionPrimitives::clearActionsQueue() {
	if (configured) {
		mutex.wait();
		actionsQueue.clear();
		mutex.post();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::lockActions() {
	if (configured) {
		locked = true;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::unlockActions() {
	if (configured) {
		locked = false;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getActionsLockStatus() const {
	return locked;
}

/************************************************************************/
bool ActionPrimitives::_pushAction(const bool execArm, const Vector &x,
		const Vector &o, const double execTime, const bool oEnabled,
		const bool execHand, const HandWayPoint &handWP,
		const bool handSeqTerminator, ActionPrimitivesCallback *clb) {
	if (configured && !locked) {
		mutex.wait();
		Action action;

		action.waitState = false;
		action.execArm = execArm;
		action.x = x;
		action.o = o;
		action.execTime = execTime;
		action.oEnabled = oEnabled;
		action.execHand = execHand;
		action.handWP = handWP;
		action.handSeqTerminator = handSeqTerminator;
		action.clb = clb;

		actionsQueue.push_back(action);
		mutex.post();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::_pushAction(const Vector &x, const Vector &o,
		const string &handSeqKey, const double execTime,
		ActionPrimitivesCallback *clb, const bool oEnabled) {
	if (configured) {
		map<string, deque<HandWayPoint> >::iterator itr = handSeqMap.find(
				handSeqKey);
		if (itr != handSeqMap.end()) {
			deque<HandWayPoint> &q = itr->second;

			if (q.size()) {
				Vector vectDummy(1);

				// combined action
				_pushAction(true, x, o, execTime, oEnabled, true, q[0],
						q.size() == 1, q.size() == 1 ? clb : NULL);

				if (q.size() > 1) {
					unsigned int i;

					// decompose hand action in sum of fingers sequences
					for (i = 1; i < q.size() - 1; i++)
						_pushAction(false, vectDummy, vectDummy,
								ACTIONPRIM_DISABLE_EXECTIME, false, true, q[i],
								false, NULL);

					// reserve the callback whenever the last hand WP is achieved
					if (i < q.size())
						_pushAction(false, vectDummy, vectDummy,
								ACTIONPRIM_DISABLE_EXECTIME, false, true, q[i],
								true, clb);
				}
			}

			return true;
		} else {
			printMessage("WARNING: \"%s\" hand sequence key not found\n",
					handSeqKey.c_str());

			return false;
		}
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::pushAction(const Vector &x, const Vector &o,
		const string &handSeqKey, const double execTime,
		ActionPrimitivesCallback *clb) {
	return _pushAction(x, o, handSeqKey, execTime, clb, true);
}

/************************************************************************/
bool ActionPrimitives::pushAction(const Vector &x, const string &handSeqKey,
		const double execTime, ActionPrimitivesCallback *clb) {
	Vector vectDummy(1);

	return _pushAction(x, vectDummy, handSeqKey, execTime, clb, false);
}

/************************************************************************/
bool ActionPrimitives::pushAction(const Vector &x, const Vector &o,
		const double execTime, ActionPrimitivesCallback *clb) {
	if (configured) {
		HandWayPoint handDummy;

		_pushAction(true, x, o, execTime, true, false, handDummy, false, clb);

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::pushAction(const Vector &x, const double execTime,
		ActionPrimitivesCallback *clb) {
	if (configured) {
		HandWayPoint handDummy;
		Vector vectDummy(1);

		_pushAction(true, x, vectDummy, execTime, false, false, handDummy,
				false, clb);

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::pushAction(const string &handSeqKey,
		ActionPrimitivesCallback *clb) {
	if (configured) {
		map<string, deque<HandWayPoint> >::iterator itr = handSeqMap.find(
				handSeqKey);
		if (itr != handSeqMap.end()) {
			deque<HandWayPoint> &q = itr->second;
			Vector vectDummy(1);
			unsigned int i;

			// decompose hand action in sum of fingers sequences
			for (i = 0; i < q.size() - 1; i++)
				_pushAction(false, vectDummy, vectDummy,
						ACTIONPRIM_DISABLE_EXECTIME, false, true, q[i], false,
						NULL);

			// reserve the callback whenever the last hand WP is achieved
			if (i < q.size())
				_pushAction(false, vectDummy, vectDummy,
						ACTIONPRIM_DISABLE_EXECTIME, false, true, q[i], true,
						clb);

			return true;
		} else {
			printMessage("WARNING: \"%s\" hand sequence key not found\n",
					handSeqKey.c_str());

			return false;
		}
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::pushWaitState(const double tmo,
		ActionPrimitivesCallback *clb) {
	if (configured && !locked) {
		mutex.wait();
		Action action;

		action.waitState = true;
		action.tmo = tmo;
		action.execArm = false;
		action.execHand = false;
		action.handSeqTerminator = false;
		action.clb = clb;

		actionsQueue.push_back(action);
		mutex.post();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::reachPose(const Vector &x, const Vector &o,
		const double execTime) {
	if (configured && !locked) {
		disableArmWaving();

		const double t = execTime > 0.0 ? execTime : default_exec_time;

		enableTorsoDof();

		cartCtrl->goToPose(x, o, t);

		printMessage("reach at %g [s] for [%s], [%s]\n", t,
				toCompactString(x).c_str(), toCompactString(o).c_str());

		postReachCallback();

		latchTimerReachLog = latchTimerReach = Time::now();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::reachPosition(const Vector &x, const double execTime) {
	if (configured && !locked) {
		disableArmWaving();

		const double t = execTime > 0.0 ? execTime : default_exec_time;

		enableTorsoDof();

		cartCtrl->goToPosition(x, t);

		printMessage("reach at %g [s] for [%s]\n", t,
				toCompactString(x).c_str());

		postReachCallback();

		latchTimerReachLog = latchTimerReach = Time::now();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::execQueuedAction() {
	bool exec = false;
	Action action;

	mutex.wait();
	if (actionsQueue.size()) {
		action = actionsQueue.front();
		actionsQueue.pop_front();
		exec = true;
	}
	mutex.post();

	if (exec) {
		if (action.waitState)
			wait(action);

		if (action.execArm)
			cmdArm(action);

		if (action.execHand)
			cmdHand(action);

		actionClb = action.clb;
	}

	return exec;
}

/************************************************************************/
bool ActionPrimitives::execPendingHandSequences() {
	bool exec = false;
	Action action;

	mutex.wait();
	if (actionsQueue.size()) {
		// polling on the first action in the queue
		action = actionsQueue.front();

		// if it is an hand-action then execute and update queue
		if (action.execHand && !action.execArm && !action.waitState) {
			actionsQueue.pop_front();
			cmdHand(action);
			exec = true;
		}
	}
	mutex.post();

	return exec;
}

/************************************************************************/
void ActionPrimitives::run() {
	const double t = Time::now();

	if (!armMoveDone) {
		Vector x, o, xdhat, odhat, qdhat;
		cartCtrl->getPose(x, o);
		cartCtrl->getDesired(xdhat, odhat, qdhat);

		if ((t - latchTimerReachLog) > ACTIONPRIM_DUMP_PERIOD) {
			printMessage("reaching... xdhat=[%s] |e|=%.3f [m]\n",
					toCompactString(xdhat).c_str(), norm(xdhat - x));

			latchTimerReachLog = t;
		}

		cartCtrl->checkMotionDone(&armMoveDone);

		// check if timeout has expired
		if (reachTmoEnabled && !armMoveDone) {
			if ((t - latchTimerReach) > reachTmo) {
				printMessage("timeout (%g) expired while reaching\n", reachTmo);
				armMoveDone = true;
			}
		}

		if (armMoveDone) {
			printMessage("reaching complete\n");
			disableTorsoDof();
		}
	}

	if (!handMoveDone) {
		// check whether all the remaining active joints have come
		// to a complete stop
		if (handMoveDone = isHandSeqEnded()) {
			printMessage("hand WP reached\n");

			if (!handSeqTerminator)
				if (execPendingHandSequences()) // here handMoveDone may switch false again
					motionStartEvent.signal();
		}
	}

	latchArmMoveDone = armMoveDone;
	latchHandMoveDone = handMoveDone;

	if (latchArmMoveDone && latchHandMoveDone && (t - latchTimerWait > waitTmo)) {
		// execute action-end callback
		if (actionClb) {
			printMessage("executing action-end callback ...\n");
			actionClb->exec();
			printMessage("... action-end callback executed\n");

			actionClb = NULL;
		}

		if (execQueuedAction())
			motionStartEvent.signal();
		else
			motionDoneEvent.signal();
	}
}

/************************************************************************/
ActionPrimitives::~ActionPrimitives() {
	close();
}

/************************************************************************/
bool ActionPrimitives::stopJntTraj(const int jnt) {
	double fb;

	if (encCtrl->getEncoder(jnt, &fb))
		return posCtrl->positionMove(jnt, fb);
	else
		return false;
}

/************************************************************************/
bool ActionPrimitives::handCheckMotionDone(const int jnt) {
	double fb;

	if (encCtrl->getEncoder(jnt, &fb)) {
		if (fabs(curHandFinalPoss[jnt - jHandMin] - fb) < curHandTols[jnt
				- jHandMin])
			return true;
		else
			return false;
	} else
		return false;
}

/************************************************************************/
void ActionPrimitives::enableTorsoDof() {
	// enable torso joints, if any
	if (!torsoActive && norm(enableTorsoSw)) {
		Vector dummyRet;
		cartCtrl->setDOF(enableTorsoSw, dummyRet);
		torsoActive = true;
	}
}

/************************************************************************/
void ActionPrimitives::disableTorsoDof() {
	// disable torso joints, if any
	if (torsoActive && norm(enableTorsoSw)) {
		Vector dummyRet;
		cartCtrl->setDOF(disableTorsoSw, dummyRet);
		torsoActive = false;
	}
}

/************************************************************************/
bool ActionPrimitives::wait(const Action &action) {
	if (configured) {
		printMessage("wait for %g seconds\n", action.tmo);
		waitTmo = action.tmo;
		latchTimerWait = Time::now();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::cmdArm(const Action &action) {
	if (configured) {
		disableArmWaving();

		const Vector &x = action.x;
		const Vector &o = action.o;
		const bool oEnabled = action.oEnabled;
		const double t = action.execTime > 0.0 ? action.execTime
				: default_exec_time;

		enableTorsoDof();

		if (oEnabled) {
			if (!cartCtrl->goToPoseSync(x, o, t)) {
				printMessage("reach error\n");
				return false;
			}

			printMessage("reach at %g [s] for [%s], [%s]\n", t,
					toCompactString(x).c_str(), toCompactString(o).c_str());
		} else {
			if (!cartCtrl->goToPositionSync(x, t)) {
				printMessage("reach error\n");
				return false;
			}

			printMessage("reach at %g [s] for [%s]\n", t,
					toCompactString(x).c_str());
		}

		postReachCallback();

		latchTimerReachLog = latchTimerReach = Time::now();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::cmdHand(const Action &action) {
	if (configured) {
		disableArmWaving();

		const string &tag = action.handWP.tag;
		const Vector &poss = action.handWP.poss;
		const Vector &vels = action.handWP.vels;
		const Vector &tols = action.handWP.tols;
		const Vector &thres = action.handWP.thres;
		const double &tmo = action.handWP.tmo;

		fingersMovingJntsSet = fingersJntsSet;
		curHandFinalPoss = poss;
		curHandTols = tols;
		curGraspDetectionThres = thres;
		curHandTmo = tmo;

		for (set<int>::iterator itr = fingersJntsSet.begin(); itr
				!= fingersJntsSet.end(); ++itr) {
			int j = *itr - jHandMin;

			if ((j >= poss.length()) || (j >= vels.length()))
				break;

			posCtrl->setRefSpeed(*itr, vels[j]);
			posCtrl->positionMove(*itr, poss[j]);
		}

		latchHandMoveDone = handMoveDone = false;
		handSeqTerminator = action.handSeqTerminator;
		fingersInPosition = true;
		printMessage("\"%s\" WP: [%s] (thres = [%s]) (tmo = %g)\n",
				tag.c_str(), toCompactString(poss).c_str(), toCompactString(
						thres).c_str(), tmo);

		latchTimerHand = Time::now();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::addHandSeqWP(const string &handSeqKey,
		const Vector &poss, const Vector &vels, const Vector &tols,
		const Vector &thres, const double tmo) {
	if ((poss.length() == 9) && (vels.length() == 9) && (tols.length() == 9)
			&& (thres.length() == 5)) {
		HandWayPoint handWP;

		handWP.tag = handSeqKey;
		handWP.poss = poss;
		handWP.vels = vels;
		handWP.tols = tols;
		handWP.thres = thres;
		handWP.tmo = tmo;

		handSeqMap[handSeqKey].push_back(handWP);

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::addHandSequence(const string &handSeqKey,
		const Bottle &sequence) {
	Bottle &bSeq = const_cast<Bottle&> (sequence);

	if (!bSeq.check("numWayPoints")) {
		printMessage("WARNING: \"numWayPoints\" option is missing\n");
		return false;
	}

	int numWayPoints = bSeq.find("numWayPoints").asInt();
	bool ret = false;

	for (int j = 0; j < numWayPoints; j++) {
		char wp[255];
		sprintf(wp, "wp_%d", j);

		Bottle &bWP = bSeq.findGroup(wp);
		if (bWP.isNull()) {
			printMessage("WARNING: \"%s\" entry is missing\n", wp);
			return false;
		}

		if (!bWP.check("poss")) {
			printMessage("WARNING: \"poss\" option is missing\n");
			return false;
		}

		if (!bWP.check("vels")) {
			printMessage("WARNING: \"vels\" option is missing\n");
			return false;
		}

		if (!bWP.check("tols")) {
			printMessage("WARNING: \"tols\" option is missing\n");
			return false;
		}

		if (!bWP.check("thres")) {
			printMessage("WARNING: \"thres\" option is missing\n");
			return false;
		}

		if (!bWP.check("tmo")) {
			printMessage("WARNING: \"tmo\" option is missing\n");
			return false;
		}

		Bottle *bPoss = bWP.find("poss").asList();
		Vector poss(bPoss->size());

		for (int k = 0; k < poss.length(); k++)
			poss[k] = bPoss->get(k).asDouble();

		Bottle *bVels = bWP.find("vels").asList();
		Vector vels(bVels->size());

		for (int k = 0; k < vels.length(); k++)
			vels[k] = bVels->get(k).asDouble();

		Bottle *bTols = bWP.find("tols").asList();
		Vector tols(bTols->size());

		for (int k = 0; k < tols.length(); k++)
			tols[k] = bTols->get(k).asDouble();

		Bottle *bThres = bWP.find("thres").asList();
		Vector thres(bThres->size());

		for (int k = 0; k < thres.length(); k++)
			thres[k] = bThres->get(k).asDouble();

		double tmo = bWP.find("tmo").asDouble();

		if (addHandSeqWP(handSeqKey, poss, vels, tols, thres, tmo))
			ret = true; // at least one WP has been added
		else
			printMessage(
					"WARNING: \"%s\" entry is invalid, not added to \"%s\"\n",
					wp, handSeqKey.c_str());
	}

	return ret;
}

/************************************************************************/
bool ActionPrimitives::isValidHandSeq(const string &handSeqKey) {
	map<string, deque<HandWayPoint> >::iterator itr = handSeqMap.find(
			handSeqKey);

	if (itr != handSeqMap.end())
		return true;
	else
		return false;
}

/************************************************************************/
bool ActionPrimitives::removeHandSeq(const string &handSeqKey) {
	map<string, deque<HandWayPoint> >::iterator itr = handSeqMap.find(
			handSeqKey);

	if (itr != handSeqMap.end()) {
		handSeqMap[handSeqKey].clear();
		return true;
	} else
		return false;
}

/************************************************************************/
deque<string> ActionPrimitives::getHandSeqList() {
	map<string, deque<HandWayPoint> >::iterator itr;
	deque<string> q;

	for (itr = handSeqMap.begin(); itr != handSeqMap.end(); ++itr)
		q.push_back(itr->first);

	return q;
}

/************************************************************************/
bool ActionPrimitives::getHandSequence(const string &handSeqKey,
		Bottle &sequence) {
	if (isValidHandSeq(handSeqKey)) {
		deque<HandWayPoint> &handWP = handSeqMap[handSeqKey];
		sequence.clear();

		// numWayPoints part
		Bottle &bNum = sequence.addList();
		bNum.addString("numWayPoints");
		bNum.addInt(handWP.size());

		// wayPoints parts
		for (unsigned int i = 0; i < handWP.size(); i++) {
			char wp[255];
			sprintf(wp, "wp_%d", i);

			Bottle &bWP = sequence.addList();
			bWP.addString(wp);

			// poss part
			Bottle &bPoss = bWP.addList();
			bPoss.addString("poss");
			Bottle &bPossVects = bPoss.addList();
			for (int j = 0; j < handWP[i].poss.length(); j++)
				bPossVects.addDouble(handWP[i].poss[j]);

			// vels part
			Bottle &bVels = bWP.addList();
			bVels.addString("vels");
			Bottle &bVelsVects = bVels.addList();
			for (int j = 0; j < handWP[i].vels.length(); j++)
				bVelsVects.addDouble(handWP[i].vels[j]);

			// tols part
			Bottle &bTols = bWP.addList();
			bTols.addString("tols");
			Bottle &bTolsVects = bTols.addList();
			for (int j = 0; j < handWP[i].tols.length(); j++)
				bTolsVects.addDouble(handWP[i].tols[j]);

			// thres part
			Bottle &bThres = bWP.addList();
			bThres.addString("thres");
			Bottle &bThresVects = bThres.addList();
			for (int j = 0; j < handWP[i].thres.length(); j++)
				bThresVects.addDouble(handWP[i].thres[j]);

			// tmo part
			Bottle &bTmo = bWP.addList();
			bTmo.addString("tmo");
			bTmo.addDouble(handWP[i].tmo);
		}

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::areFingersMoving(bool &f) const {
	if (configured) {
		f = latchHandMoveDone;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::areFingersInPosition(bool &f) const {
	if (configured) {
		f = fingersInPosition;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getGraspModel(Model *&model) const {
	if (configured) {
		model = static_cast<Model*> (graspModel);
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getCartesianIF(ICartesianControl *&ctrl) const {
	if (configured) {
		ctrl = cartCtrl;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getTorsoJoints(yarp::sig::Vector &torso) {
	if (configured) {
		torso = enableTorsoSw;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::setTorsoJoints(const yarp::sig::Vector &torso) {
	if (configured) {
		int len = torso.length() > 3 ? 3 : torso.length();
		for (int i = 0; i < len; i++)
			enableTorsoSw[i] = torso[i];

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getPose(Vector &x, Vector &o) const {
	if (configured) {
		cartCtrl->getPose(x, o);
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::stopControl() {
	if (configured) {
		suspend();

		clearActionsQueue();

		cartCtrl->stopControl();

		for (set<int>::iterator itr = fingersJntsSet.begin(); itr
				!= fingersJntsSet.end(); ++itr)
			stopJntTraj(*itr);

		armMoveDone = latchArmMoveDone = true;
		handMoveDone = latchHandMoveDone = true;

		resume();

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::setTrackingMode(const bool f) {
	if (configured) {
		if (cartCtrl->setTrackingMode(f)) {
			tracking_mode = f;
			return true;
		} else
			return false;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::enableArmWaving(const Vector &restPos) {
	if (configured) {
		RES_WAVER(armWaver)->setRestPosition(restPos);
		printMessage("setting waving position to %s\n",
				toCompactString(restPos).c_str());

		if (RES_WAVER(armWaver)->enable())
			printMessage("arm waving enabled\n");

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::disableArmWaving() {
	if (configured) {
		if (RES_WAVER(armWaver)->disable())
			printMessage("arm waving disabled\n");

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::enableReachingTimeout(const double tmo) {
	if (configured) {
		reachTmo = tmo;
		reachTmoEnabled = true;

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::disableReachingTimeout() {
	if (configured) {
		reachTmoEnabled = false;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::getTrackingMode() const {
	return tracking_mode;
}

/************************************************************************/
bool ActionPrimitives::checkActionsDone(bool &f, const bool sync) {
	if (configured) {
		if (sync && checkEnabled) {
			motionDoneEvent.reset();
			motionDoneEvent.wait();
		}

		f = latchArmMoveDone && latchHandMoveDone;

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::checkActionOnGoing(bool &f, const bool sync) {
	if (configured) {
		if (sync && checkEnabled) {
			motionStartEvent.reset();
			motionStartEvent.wait();
		}

		f = !latchArmMoveDone || !latchHandMoveDone;

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::syncCheckInterrupt(const bool disable) {
	if (configured) {
		motionDoneEvent.signal();

		if (disable)
			checkEnabled = false;

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitives::syncCheckReinstate() {
	if (configured) {
		checkEnabled = true;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer1::grasp(const Vector &x, const Vector &o,
		const Vector &d) {
	if (configured) {
		printMessage("start grasping\n");

		pushAction(x + d, o, "open_hand");
		pushAction(x, o);
		pushAction("close_hand");

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer1::touch(const Vector &x, const Vector &o,
		const Vector &d) {
	if (configured) {
		printMessage("start touching\n");

		pushAction(x + d, o, "karate_hand");
		pushAction(x, o);

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer1::tap(const Vector &x1, const Vector &o1,
		const Vector &x2, const Vector &o2, const double execTime) {
	if (configured) {
		printMessage("start tapping\n");

		pushAction(x1, o1, "karate_hand");
		pushAction(x2, o2, execTime);
		pushAction(x1, o1);

		return true;
	} else
		return false;
}

/************************************************************************/
ActionPrimitivesLayer1::~ActionPrimitivesLayer1() {
	close();
}

/************************************************************************/
//TODO: study on this callback
void liftAndGraspCallback::exec() {
	// lift up the hand iff contact detected
	if (action->contactDetected) {
		Vector x, o;
		action->cartCtrl->getPose(x, o);
		action->printMessage("logged 3-d pos: [%s]\n", action->toCompactString(
				x).c_str());

		action->pushAction(x + action->grasp_d2, action->grasp_o);
	}

	action->disableContactDetection();
	action->pushAction("close_hand");
}

/************************************************************************/
void touchCallback::exec() {
	// just disable the contact detection
	action->disableContactDetection();
}

/************************************************************************/
ActionPrimitivesLayer2::ActionPrimitivesLayer2() :
	ActionPrimitivesLayer1() {
	init();
}

/************************************************************************/
ActionPrimitivesLayer2::ActionPrimitivesLayer2(Property &opt) :
	ActionPrimitivesLayer1(opt) {
	init();
	skipFatherPart = true;
	open(opt);
}

/************************************************************************/
void ActionPrimitivesLayer2::init() {
	skipFatherPart = false;
	configuredLayer2 = false;
	contactDetectionOn = false;
	contactDetected = false;

	polyTorso = NULL;
	encTorso = NULL;
	velEst = NULL;
	accEst = NULL;
	dynArm = NULL;
	dynSensor = NULL;
	dynTransformer = NULL;
	execLiftAndGrasp = NULL;
	execTouch = NULL;
	ftPortIn = NULL;

	//++Kadir
	port_palm_contact_in = NULL;
	//--Kadir
}

/************************************************************************/
void ActionPrimitivesLayer2::postReachCallback() {
	// init the contact variable
	contactDetected = false;

	// call the main postReachCallback()
	ActionPrimitivesLayer1::postReachCallback();
}

/************************************************************************/
//TODO: this is going to be modified a lot
void ActionPrimitivesLayer2::run() {
	// skip until this layer is configured
	if (!configuredLayer2)
		return;

	// ++ Kadir
	///*
	const double t = Time::now();

	// estimation of internal dynamic model
	encTorso->getEncoders(encDataTorso.data());
	encCtrl->getEncoders(encDataArm.data());

	for (int i = 0; i < 3; i++)
		q[i] = encDataTorso[2 - i];

	for (int i = 3; i < q.length(); i++)
		q[i] = encDataArm[i - 3];

	AWPolyElement el;
	el.data = q;
	el.time = t;

	dq = velEst->estimate(el);
	d2q = accEst->estimate(el);

	dynArm->setAng(CTRL_DEG2RAD * q);
	dynArm->setDAng(CTRL_DEG2RAD * dq);
	dynArm->setD2Ang(CTRL_DEG2RAD * d2q);
	dynArm->computeNewtonEuler();

	dynSensor->computeSensorForceMoment();
	wrenchModel = dynSensor->getSensorForceMoment();

	// get the input from the sensor
	if (Vector *ftMeasured=ftPortIn->read(false))
		wrenchMeasured = *ftMeasured;

	// estimation of external forces/torques acting on the end-effector
	wrenchExternal = dynTransformer->getEndEffWrenchAsBase((wrenchMeasured
			- wrenchOffset) + wrenchModel);

	Vector forceExternal(3);
	forceExternal[0] = wrenchExternal[0];
	forceExternal[1] = wrenchExternal[1];
	forceExternal[2] = wrenchExternal[2];
	const double forceExternalAbs = norm(forceExternal);

	// stop the arm iff contact detected while reaching
	if (!armMoveDone && contactDetectionOn && (forceExternalAbs
			> ext_force_thres)) {
		cartCtrl->stopControl();

		printMessage(
				"contact detected on arm: external force [%s], (%g>%g) => stopping arm\n",
				toCompactString(forceExternal).c_str(), forceExternalAbs,
				ext_force_thres);

		disableTorsoDof();

		armMoveDone = true;
		contactDetected = true;
	}

	/*
	 Vector forceExternal(3);
	 double forceExternalAbs;

	 if (!sim) {
	 const double t = Time::now();

	 // estimation of internal dynamic model
	 encTorso->getEncoders(encDataTorso.data());
	 encCtrl->getEncoders(encDataArm.data());

	 for (int i = 0; i < 3; i++)
	 q[i] = encDataTorso[2 - i];

	 for (int i = 3; i < q.length(); i++)
	 q[i] = encDataArm[i - 3];

	 AWPolyElement el;
	 el.data = q;
	 el.time = t;

	 dq = velEst->estimate(el);
	 d2q = accEst->estimate(el);

	 dynArm->setAng(CTRL_DEG2RAD * q);
	 dynArm->setDAng(CTRL_DEG2RAD * dq);
	 dynArm->setD2Ang(CTRL_DEG2RAD * d2q);
	 dynArm->computeNewtonEuler();

	 dynSensor->computeSensorForceMoment();
	 wrenchModel = dynSensor->getSensorForceMoment();

	 // get the input from the sensor
	 if (Vector *ftMeasured=ftPortIn->read(false))
	 wrenchMeasured = *ftMeasured;

	 // estimation of external forces/torques acting on the end-effector
	 wrenchExternal = dynTransformer->getEndEffWrenchAsBase((wrenchMeasured
	 - wrenchOffset) + wrenchModel);

	 forceExternal[0] = wrenchExternal[0];
	 forceExternal[1] = wrenchExternal[1];
	 forceExternal[2] = wrenchExternal[2];
	 forceExternalAbs = norm(forceExternal);

	 if ((forceExternalAbs > ext_force_thres))
	 contactDetected = true;
	 } else {
	 //TODO handle palm contact detection here
	 if (Vector* contact_detection=port_palm_contact_in->read(false)) {
	 if((*contact_detection)[5] > 0.01)
	 {
	 printMessage("************************\n");
	 printMessage("palm contact: %f\n", (*contact_detection)[5]);
	 printMessage("------------------------\n");
	 contactDetected = true;
	 }
	 }
	 }

	 // stop the arm iff contact detected while reaching
	 if (!armMoveDone && contactDetectionOn && contactDetected) {
	 cartCtrl->stopControl();

	 if (!sim)
	 printMessage(
	 "contact detected on arm: external force [%s], (%g>%g) => stopping arm\n",
	 toCompactString(forceExternal).c_str(), forceExternalAbs,
	 ext_force_thres);
	 else {
	 //TODO:
	 printMessage("***********************\n");
	 printMessage("contact detected on arm\n");
	 printMessage("-----------------------\n");
	 }

	 disableTorsoDof();

	 armMoveDone = true;
	 }
	 // -- Kadir
	 */

	// call the main run()
	// the order does matter
	ActionPrimitivesLayer1::run();
}

/************************************************************************/
bool ActionPrimitivesLayer2::open(Property &opt) {
	if (!skipFatherPart)
		ActionPrimitivesLayer1::open(opt);

	if (configuredLayer2) {
		printMessage("WARNING: already configured\n");
		return true;
	}

	if (configured) {
		ext_force_thres = opt.check("ext_force_thres", Value(
				ACTIONPRIM_DEFAULT_EXT_FORCE_THRES)).asDouble();

		// open motor interfaces
		Property optPolyTorso("(device remote_controlboard)");
		optPolyTorso.put("remote", ("/" + robot + "/torso").c_str());
		optPolyTorso.put("local",
				("/" + local + "/" + part + "/torso/position").c_str());

		polyTorso = new PolyDriver;
		if (!polyTorso->open(optPolyTorso)) {
			close();
			return false;
		}

		polyTorso->view(encTorso);

		int nTorso;
		int nArm;

		encTorso->getAxes(&nTorso);
		encCtrl->getAxes(&nArm);

		encDataTorso.resize(nTorso, 0.0);
		encDataArm.resize(nArm, 0.0);
		// ++ Kadir
		///*
		// open port to get FT input
		ftPortIn = new BufferedPort<Vector> ;
		string ftPortInName = "/" + local + "/" + part + "/ft:i";
		string ftServerPortName = "/" + robot + "/" + part + "/analog:o";
		ftPortIn->open(ftPortInName.c_str());

		// connect automatically to FT sensor
		if (!Network::connect(ftServerPortName.c_str(), ftPortInName.c_str())) {
			printMessage("ERROR: unable to connect to port %s\n",
					ftServerPortName.c_str());

			//close();
			//return false;
		}

		// create callbacks
		execLiftAndGrasp = new liftAndGraspCallback(this);
		execTouch = new touchCallback(this);

		// create estimators
		velEst = new AWLinEstimator(16, 1.0);
		accEst = new AWQuadEstimator(25, 1.0);

		// create dynamics
		string armType = (part == "right_arm" ? "right" : "left");
		dynArm = new iCubArmDyn(armType);

		// free up torso links
		dynArm->releaseLink(0);
		dynArm->releaseLink(1);
		dynArm->releaseLink(2);

		dynSensor = new iDynInvSensorArm(dynArm, DYNAMIC);
		dynTransformer = new iFTransformation(dynSensor);

		// configure dynamics
		alignJointsBounds();

		Vector zeros(3), accFrame0(3);
		zeros = accFrame0 = 0.0;
		accFrame0[2] = 9.81; // along z-component

		wrenchModel.resize(6, 0.0);
		wrenchOffset.resize(6, 0.0);
		wrenchMeasured.resize(6, 0.0);
		wrenchExternal.resize(6, 0.0);

		q.resize(dynArm->getN(), 0.0);
		dq.resize(dynArm->getN(), 0.0);
		d2q.resize(dynArm->getN(), 0.0);

		dynArm->setAng(q);
		dynArm->setDAng(dq);
		dynArm->setD2Ang(d2q);
		dynArm->prepareNewtonEuler(DYNAMIC);
		dynArm->initNewtonEuler(zeros, zeros, accFrame0, zeros, zeros);
		//*/
		/*
		 if (!sim) {
		 // open port to get FT input
		 ftPortIn = new BufferedPort<Vector> ;
		 string ftPortInName = "/" + local + "/" + part + "/ft:i";
		 string ftServerPortName = "/" + robot + "/" + part + "/analog:o";
		 ftPortIn->open(ftPortInName.c_str());

		 // connect automatically to FT sensor
		 if (!Network::connect(ftServerPortName.c_str(),
		 ftPortInName.c_str())) {
		 printMessage("ERROR: unable to connect to port %s\n",
		 ftServerPortName.c_str());

		 close();
		 return false;
		 }

		 // create estimators
		 velEst = new AWLinEstimator(16, 1.0);
		 accEst = new AWQuadEstimator(25, 1.0);

		 // create dynamics
		 string armType = (part == "right_arm" ? "right" : "left");
		 dynArm = new iCubArmDyn(armType);

		 // free up torso links
		 dynArm->releaseLink(0);
		 dynArm->releaseLink(1);
		 dynArm->releaseLink(2);

		 dynSensor = new iDynInvSensorArm(dynArm, DYNAMIC);
		 dynTransformer = new iFTransformation(dynSensor);

		 // configure dynamics
		 alignJointsBounds();

		 Vector zeros(3), accFrame0(3);
		 zeros = accFrame0 = 0.0;
		 accFrame0[2] = 9.81; // along z-component

		 wrenchModel.resize(6, 0.0);
		 wrenchOffset.resize(6, 0.0);
		 wrenchMeasured.resize(6, 0.0);
		 wrenchExternal.resize(6, 0.0);

		 q.resize(dynArm->getN(), 0.0);
		 dq.resize(dynArm->getN(), 0.0);
		 d2q.resize(dynArm->getN(), 0.0);

		 dynArm->setAng(q);
		 dynArm->setDAng(dq);
		 dynArm->setD2Ang(d2q);
		 dynArm->prepareNewtonEuler(DYNAMIC);
		 dynArm->initNewtonEuler(zeros, zeros, accFrame0, zeros, zeros);
		 } else {

		 // open port to get palm contact feedback
		 port_palm_contact_in = new BufferedPort<Vector> ;
		 string port_palm_contact_in_name = "/" + local + "/" + part
		 + "/palm_contact:i";
		 string armType = (part == "right_arm" ? "right" : "left");
		 string port_skin_server_name = "/" + robot + "/skin/" + armType
		 + "_hand";
		 port_palm_contact_in->open(port_palm_contact_in_name.c_str());

		 if (!Network::connect(port_skin_server_name.c_str(),
		 port_palm_contact_in_name.c_str()))
		 {
		 printMessage("ERROR: unable to connect to port %s\n",
		 port_skin_server_name.c_str());

		 close();
		 return false;
		 }
		 }

		 // create callbacks
		 execLiftAndGrasp = new liftAndGraspCallback(this);
		 execTouch = new touchCallback(this);
		 // --Kadir
		 */
		return configuredLayer2 = true;
	} else
		return false;
}

/************************************************************************/
void ActionPrimitivesLayer2::alignJointsBounds() {
	IControlLimits *limTorso;
	IControlLimits *limHand;

	polyTorso->view(limTorso);
	polyHand->view(limHand);

	deque<IControlLimits*> lim;
	lim.push_back(limTorso);
	lim.push_back(limHand);

	dynArm->alignJointsBounds(lim);
}

/************************************************************************/
bool ActionPrimitivesLayer2::isValid() const {
	return (ActionPrimitivesLayer1::isValid() && configuredLayer2);
}

/************************************************************************/
void ActionPrimitivesLayer2::close() {
	if (closed)
		return;

	// call the main close()
	// the order does matter
	ActionPrimitivesLayer1::close();

	// ++Kadir
	if (port_palm_contact_in != NULL) {
		port_palm_contact_in->interrupt();
		port_palm_contact_in->close();
		delete port_palm_contact_in;
	}
	// --Kadir

	if (ftPortIn != NULL) {
		ftPortIn->interrupt();
		ftPortIn->close();
		delete ftPortIn;
	}

	if (dynTransformer != NULL)
		delete dynTransformer;

	if (dynSensor != NULL)
		delete dynSensor;

	if (dynArm != NULL)
		delete dynArm;

	if (velEst != NULL)
		delete velEst;

	if (accEst != NULL)
		delete accEst;

	if (execLiftAndGrasp != NULL)
		delete execLiftAndGrasp;

	if (execTouch != NULL)
		delete execTouch;

	if (polyTorso != NULL) {
		printMessage("closing torso driver ...\n");
		delete polyTorso;
	}
}

/************************************************************************/
bool ActionPrimitivesLayer2::grasp(const Vector &x, const Vector &o,
		const Vector &d1, const Vector &d2) {
	if (configured) {
		printMessage("start grasping\n");

		// ++Kadir
		///*
		// latch the offset
		latchWrenchOffset();
		//*/
		/*
		 if (!sim) {
		 // latch the offset
		 latchWrenchOffset();
		 }
		 // --Kadir
		 */

		enableContactDetection();

		pushAction(x + d1, o, "open_hand");
		pushAction(x, o, ACTIONPRIM_DISABLE_EXECTIME, execLiftAndGrasp);
		// the remaining part is done in the callback

		//TODO: isn't this to be above the pushAction that inputs a callback?
		// save data
		grasp_d2 = d2;
		grasp_o = o;

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::grasp(const Vector &x, const Vector &o,
		const Vector &d) {
	return ActionPrimitivesLayer1::grasp(x, o, d);
}

/************************************************************************/
bool ActionPrimitivesLayer2::touch(const Vector &x, const Vector &o,
		const Vector &d) {
	if (configured) {
		printMessage("start touching\n");

		// ++Kadir
		/*
		 // latch the offset
		 latchWrenchOffset();
		 */
		if (!sim) {
			// latch the offset
			latchWrenchOffset();
		}
		// --Kadir

		enableContactDetection();

		pushAction(x + d, o, "karate_hand");
		pushAction(x, o, ACTIONPRIM_DISABLE_EXECTIME, execTouch);

		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::latchWrenchOffset() {
	if (configured) {
		wrenchOffset = wrenchMeasured + wrenchModel;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::getExtWrench(Vector &wrench) const {
	if (configured) {
		wrench = wrenchExternal;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::getExtForceThres(double &thres) const {
	if (configured) {
		thres = ext_force_thres;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::setExtForceThres(const double thres) {
	if (configured) {
		ext_force_thres = thres;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::enableContactDetection() {
	if (configured) {
		contactDetectionOn = true;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::disableContactDetection() {
	if (configured) {
		contactDetectionOn = false;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::isContactDetectionEnabled(bool &f) const {
	if (configured) {
		f = contactDetectionOn;
		return true;
	} else
		return false;
}

/************************************************************************/
bool ActionPrimitivesLayer2::checkContact(bool &f) const {
	if (configured) {
		f = contactDetected;
		return true;
	} else
		return false;
}

/************************************************************************/
ActionPrimitivesLayer2::~ActionPrimitivesLayer2() {
	close();
}

