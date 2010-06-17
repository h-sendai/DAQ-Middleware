#ifndef _CONDITION_ECHOMONITOR_H
#define _CONDITION_ECHOMONITOR_H 1

#include <string>
#include "Condition.h"

struct monitorParam {
	unsigned int histogram_n_bin;
	unsigned int histogram_min;
	unsigned int histogram_max;
	unsigned int monitor_update_rate;
};

typedef struct monitorParam monitorParam;

class ConditionEchoMonitor : public Condition {
public:
	ConditionEchoMonitor();
	virtual ~ConditionEchoMonitor();
	bool initialize(std::string filename);
	bool getParam(std::string prefix,  monitorParam* monitorParam);
private:
	Json2ConList m_json2ConList;
	conList      m_conListEchoMonitor;
};

#endif
