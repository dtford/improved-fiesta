#ifndef __THERMOSTAT_CONFIG_H__
#define __THERMOSTAT_CONFIG_H__

#include <string>
#include <map>

class ThermostatConfig
{
    public:

    ThermostatConfig( const std::string configFile );

    std::string get_URL();
    std::string getUsername();
    std::string getPassword();
    std::string getTemperatureFile();
    std::string getHeaterFile();

    private:

    std::map<std::string, std::string> config_map;

    void store_line( const std::string key, const std::string value );
    std::string getConfigValue ( const std::string key );
};

#endif
