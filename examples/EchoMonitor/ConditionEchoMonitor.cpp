#include "ConditionEchoMonitor.h"

ConditionEchoMonitor::ConditionEchoMonitor() {}
ConditionEchoMonitor::~ConditionEchoMonitor() {}

bool
ConditionEchoMonitor::getParam(std::string prefix, monitorParam* monitorParam)
{
	setPrefix(prefix);
	unsigned int histogram_n_bin;
	unsigned int histogram_min;
	unsigned int histogram_max;
	unsigned int monitor_update_rate;

	if (find("histogram_n_bin", &histogram_n_bin)) {
		monitorParam->histogram_n_bin = histogram_n_bin;
	}
	else {
		std::cerr << prefix + " histogram_n_bin not fould" << std::endl;
		return false;
	}

	if (find("histogram_min", &histogram_min)) {
		monitorParam->histogram_min = histogram_min;
	}
	else {
		std::cerr << prefix + " histogram_min not fould" << std::endl;
		return false;
	}

	if (find("histogram_max", &histogram_max)) {
		monitorParam->histogram_max = histogram_max;
	}
	else {
		std::cerr << prefix + " histogram_max not fould" << std::endl;
		return false;
	}


	if (find("monitor_update_rate", &monitor_update_rate)) {
		monitorParam->monitor_update_rate = monitor_update_rate;
	}
	else {
		std::cerr << prefix + " monitor_update_rate not fould" << std::endl;
		return false;
	}


	return true;
}

bool ConditionEchoMonitor::initialize(std::string filename)
{
	if (m_json2ConList.makeConList(filename, &m_conListEchoMonitor) == false) {
		std::cerr << "### ERROR: Fail to read the Condition file "
				  << filename << std::endl;
	}
	init(&m_conListEchoMonitor);
	return true;
}
