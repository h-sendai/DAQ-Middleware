#ifndef _CONDITION_SAMPLEMONITOR_H
#define _CONDITION_SAMPLEMONITOR_H 1

#include <string>
#include "Condition.h"

struct monitorParam {
    unsigned int hist_bin;
    unsigned int hist_min;
    unsigned int hist_max;
    unsigned int monitor_update_rate;
};

typedef struct monitorParam monitorParam;

class ConditionSampleMonitor : public Condition {
public:
    ConditionSampleMonitor();
    virtual ~ConditionSampleMonitor();
    bool initialize(std::string filename);
    bool getParam(std::string prefix,  monitorParam* monitorParam);
private:
    Json2ConList m_json2ConList;
    conList      m_conListSampleMonitor;
};

#endif
