#ifndef __THERMOSTAT_H__
#define __THERMOSTAT_H__

#include <string>
#include <map>

#include "ThermostatConfig.h"
#include "ThermostatNetwork.h"

class Thermostat
{
    public:

    Thermostat( const ThermostatConfig config ) : config(config), network(config.get_URL(), config.getUsername(), config.getPassword() ) {
                
    }

    void run();


    private:

    const ThermostatConfig config;
    double currentTemperature = 0;
    bool heaterOn = false;
    ThermostatNetwork network;

    bool getTemperature();

    double getSetpoint();

    void updateHeaterStatus();
    void updateHeaterFile();
    void updateHeaterServer();

    void updateTemperatureServer();

};

#endif
