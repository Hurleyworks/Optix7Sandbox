#pragma once

#include <unordered_map>
#include <unordered_set>
#include <array>
#include <queue>
#include <stack>
#include <fstream>
#include <set>
#include <vector>  
#include <sstream> 
#include <iomanip> // byteStr()
#include <random>
#include <chrono>
#include <thread>
#include <ctime>
#include <string>
#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <limits>
#include <algorithm>
#include <functional>
#include <stdint.h>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include<Eigen/StdVector>

// copper spice signal/slots
#include <cs_signal.h>
#include <cs_slot.h>

// juce
#include "../juce_core/juce_core.h"

// taskflow
#include <taskflow/taskflow.hpp>

// Poco
#include <Poco/Any.h>
#include "Poco/BinaryWriter.h"
#include "Poco/BinaryReader.h" 
#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"

// g3log
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

// json
#include <json.hpp>

// some useful stuff
#include "excludeFromBuild/basics//Alignment.h"
#include "excludeFromBuild/basics/Util.h"
#include "excludeFromBuild/basics/StopWatch.h"
#include "excludeFromBuild/basics/AnyValue.h"
#include "excludeFromBuild//basics/LockFreeQueue.h"

// genetic 
#include "excludeFromBuild/tools/openga.h"

using juce::numElementsInArray;
using juce::Random;
using juce::jmin;
using juce::jmax;
using json = nlohmann::json;

using Poco::BinaryWriter;
using Poco::BinaryReader;

namespace mace
{
	// tools
	#include "excludeFromBuild/tools/FileTools.h"
	#include "excludeFromBuild/tools/Pcg32.h"
	#include "excludeFromBuild/tools/pcg32_8.h"
	#include "excludeFromBuild/tools/Prime.h"
	#include "excludeFromBuild/tools/RSA.h"
	#include "excludeFromBuild/tools/LWSN.h"

	// basics
	#include "excludeFromBuild//basics/Buffer.h"
	#include "excludeFromBuild/basics/CPU.h"
	#include "excludeFromBuild/basics/StringUtil.h"
	#include "excludeFromBuild/basics/ThreadSafeContainers.h"
}

	

