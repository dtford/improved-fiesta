#include "Thermostat.h"

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
    
    while( true )
    {
    
    std::cout << "Running thermostat simulation\n";

    /* TODO: Each time through the loop, I should:
    
    * Get current temperature from file
    * Get current setpoints from server (ONLY NEEDED ONCE PER MINUTE)
    * Change heater status file if needed (TODO: determine "if needed")
    * Update temperature on server, if changed
    * Update status on server, if changed

    Error checking:
    * each server access should attempt to relogin if a "not authenticated" response is received.

*/

// DEBUG ONLY!!!!!
    std::cout << config.get_URL() << std::endl;
    std::cout << config.getUsername() << std::endl;
    std::cout << config.getPassword() << std::endl;
    std::cout << config.getTemperatureFile() << std::endl;
    std::cout << config.getHeaterFile() << std::endl;

    sleep( 10 );

    }
}
