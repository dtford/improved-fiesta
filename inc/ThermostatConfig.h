#ifndef __THERMOSTAT_CONFIG_H__
#define __THERMOSTAT_CONFIG_H__

#include <string>
#include <map>

class ThermostatConfig
{
    public:

    ThermostatConfig( const std::string configFile );

    std::string get_URL() const;
    std::string getUsername() const;
    std::string getPassword() const;
    std::string getTemperatureFile() const;
    std::string getHeaterFile() const;

    private:

    std::map<std::string, std::string> config_map;

    void store_line( const std::string key, const std::string value );
    std::string getConfigValue ( const std::string key ) const;
};

#endif
