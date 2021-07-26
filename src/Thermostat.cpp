#include "Thermostat.h"

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

    while( true )
    {
    
    bool heaterChanged = false;
    bool temperatureChanged = false;

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
    double setpoint = 70;

    // compare temperature and setpoint
    // set status as:
    // * if heater is off and temperature is at least one degree below setpoint, turn it on
    // * if heater is on and temperature is at least one degree above setpoint, turn it off
    if( !heaterOn && ( currentTemperature < ( setpoint - 1.0) ) )
    {
        heaterOn = true;
        heaterChanged = true;
    }
    else if( heaterOn && ( currentTemperature > ( setpoint + 1 ) ) )
    {
        heaterOn = false;
        heaterChanged = true;
    }

    // update heater status file
    if( heaterChanged )
    {
        updateHeaterStatus();
    }

    if( temperatureChanged )
    {
        // TODO: updateTemperatureServer();
    }

    sleep( 10 );

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
    }
    
    // DEBUG
    std::cout << currentTemperature << std::endl;
    return temperatureChanged;

}

void Thermostat::updateHeaterStatus()
{

    updateHeaterFile();
    // TODO: updateHeaterServer();
}

void Thermostat::updateHeaterFile()
{
    std::ofstream heaterFile( config.getHeaterFile() );
    time_t now = time(NULL);
    
    heaterFile << ( heaterOn ? "ON" : "OFF" ) << " " << ctime( &now ) << std::endl;
}

