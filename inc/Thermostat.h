#ifndef __THERMOSTAT_H__
#define __THERMOSTAT_H__

#include <string>
#include <map>

#include "ThermostatConfig.h"

class Thermostat
{
    public:

    Thermostat( ThermostatConfig config ) : config(config) {}

    void run();


    private:

    ThermostatConfig config;

};

#endif
