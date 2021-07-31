#include "Thermostat.h"

#include "ThermostatNetwork.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <syslog.h>
#include <unistd.h>

void Thermostat::run()
{
//    syslog(LOG_INFO, "beginning thermostat simulation");

    //login (create function, so I can re-login later)

    // get config vals so that I don't have to get them every time through the loop

    std::string url = config.get_URL();
    std::string username = config.getUsername();
    std::string password = config.getPassword();
    std::string heaterFileName = config.getHeaterFile();

    network.getTime();
    network.login();

    double setpoint = getSetpoint();

    // make sure that we update these statuses the first loop
    bool heaterChanged = true;
    bool temperatureChanged = true;
    while( true )
    {
    
    /* TODO: Each time through the loop, I should:
    
    * Get current temperature from file  <---- DONE ---->
    * Get current setpoints from server (ONLY NEEDED ONCE PER MINUTE)
    * Change heater status file if needed <---- DONE ---->
    * Update temperature on server, if changed
    * Update status on server, if changed

    Error checking:
    * each server access should attempt to relogin if a "not authenticated" response is received.

*/

// Get current temperature from file
    temperatureChanged = getTemperature();

        // temporary setpoint - TODO: should be read by server
        // TODO: double setpoint = getCurrentSetpoint();
    time_t now = time(NULL);
    // get setpoint once each minute
    if( ( now % 60 ) == 0 )
    {
        setpoint = getSetpoint();
    }

    // compare temperature and setpoint
    // set status as:
    // * if heater is off and temperature is at least one degree below setpoint, turn it on
    // * if heater is on and temperature is at least one degree above setpoint, turn it off
    if( !heaterOn && ( currentTemperature <= ( setpoint - 1.0) ) )
    {
        heaterOn = true;
        heaterChanged = true;
    }
    else if( heaterOn && ( currentTemperature >= ( setpoint + 1 ) ) )
    {
        heaterOn = false;
        heaterChanged = true;
    }

    // update heater status file
    if( heaterChanged )
    {
        updateHeaterStatus();
        heaterChanged = false;
    }

    if( temperatureChanged )
    {
        updateTemperatureServer();
        temperatureChanged = false;
    }

    sleep( 1 );

    }
}

bool Thermostat::getTemperature()
{
    bool temperatureChanged = false;
    double oldTemperature = currentTemperature;
    std::ifstream temperatureFile( config.getTemperatureFile() );
    temperatureFile >> currentTemperature;

    if(oldTemperature != currentTemperature)
    {
        temperatureChanged = true;
       syslog(LOG_INFO, "Detected new temperature %f\n", currentTemperature );
    }

    return temperatureChanged;

}

void Thermostat::updateHeaterStatus()
{

    updateHeaterFile();
    updateHeaterServer();
}

void Thermostat::updateHeaterFile()
{
    std::ofstream heaterFile( config.getHeaterFile() );
    time_t now = time(NULL);
    
    heaterFile << ( heaterOn ? "ON" : "OFF" ) << " " << ctime( &now ) << std::endl;
    syslog(LOG_INFO, "Updating %s to %s\n", config.getHeaterFile(), ( heaterOn ? "ON" : "OFF" ) );
}

void Thermostat::updateHeaterServer()
{
    network.updateHeaterStatus( heaterOn );
    syslog(LOG_INFO, "Updated Server to %s\n", ( heaterOn ? "ON" : "OFF" ) );
}

void Thermostat::updateTemperatureServer()
{
    network.updateTemperature( currentTemperature );
}

double Thermostat::getSetpoint()
{
    double setpoint = network.getCurrentSetpoint();

    return setpoint;
}
