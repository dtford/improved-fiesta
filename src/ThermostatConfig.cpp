#include "ThermostatConfig.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <syslog.h>

ThermostatConfig::ThermostatConfig( const std::string configFile )
{
    std::ifstream configStream;
    configStream.open(configFile);
    std::string line;
    while( std::getline( configStream, line ) )
    {
        std::istringstream is_line( line );
        std::string key;
        if( std::getline( is_line, key, '=' ) )
        {
            std::string value;
            if( std::getline( is_line, value ) )
            {
                store_line( key, value );
            }
        }
    }
}

void ThermostatConfig::store_line( const std::string key, const std::string value )
{
    config_map.insert( std::make_pair( key, value ) );
}

std::string ThermostatConfig::get_URL() const
{
    return getConfigValue("URL");
}

std::string ThermostatConfig::getUsername() const
{
    return getConfigValue("Username");
}

std::string ThermostatConfig::getPassword() const
{
    return getConfigValue("Password");
}

std::string ThermostatConfig::getTemperatureFile() const
{
    return getConfigValue("TemperatureFile");
}

std::string ThermostatConfig::getHeaterFile() const
{
    return getConfigValue("HeaterFile");
}

std::string ThermostatConfig::getConfigValue( const std::string key ) const
{
    try
    {
        std::string value = config_map.at( key );
        return value;
    }
    catch( const std::out_of_range& e )
    {
        std::cerr << e.what() << " while getting " << key << " config.\n";
    }
    return std::string();
}
