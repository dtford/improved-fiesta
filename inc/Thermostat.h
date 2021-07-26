#ifndef __THERMOSTAT_H__
#define __THERMOSTAT_H__

#include <string>
#include <map>

#include "ThermostatConfig.h"

class Thermostat
{
    public:

    Thermostat( const ThermostatConfig config ) : config(config) {}

    void run();


    private:

    const ThermostatConfig config;
    double currentTemperature;
    bool heaterOn;

    bool getTemperature();

    double getSetpoint();

    void updateHeaterStatus();
    void updateHeaterFile();
    void updateHeaterServer();

    void updateTemperatureServer();

};

#endif
