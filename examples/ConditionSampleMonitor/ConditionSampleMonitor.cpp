#include "ConditionSampleMonitor.h"

ConditionSampleMonitor::ConditionSampleMonitor() {}
ConditionSampleMonitor::~ConditionSampleMonitor() {}

bool
ConditionSampleMonitor::getParam(std::string prefix, monitorParam* monitorParam)
{
    setPrefix(prefix);
    unsigned int hist_bin;
    unsigned int hist_min;
    unsigned int hist_max;
    unsigned int monitor_update_rate;

    if (find("hist_bin", &hist_bin)) {
        monitorParam->hist_bin = hist_bin;
    }
    else {
        std::cerr << prefix + " hist_bin not fould" << std::endl;
        return false;
    }

    if (find("hist_min", &hist_min)) {
        monitorParam->hist_min = hist_min;
    }
    else {
        std::cerr << prefix + " hist_min not fould" << std::endl;
        return false;
    }

    if (find("hist_max", &hist_max)) {
        monitorParam->hist_max = hist_max;
    }
    else {
        std::cerr << prefix + " hist_max not fould" << std::endl;
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

bool ConditionSampleMonitor::initialize(std::string filename)
{
    if (m_json2ConList.makeConList(filename, &m_conListSampleMonitor) == false) {
        std::cerr << "### ERROR: Fail to read the Condition file "
                  << filename << std::endl;
    }
    init(&m_conListSampleMonitor);
    return true;
}
