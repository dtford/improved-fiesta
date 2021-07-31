#ifndef __THERMOSTAT_NETWORK_H__
#define __THERMOSTAT_NETWORK_H__

#include <string>
#include <list>

#include <curlpp/Easy.hpp>

#include "ThermostatConfig.h"

class ThermostatNetwork
{
    public:
        ThermostatNetwork( std::string url, std::string username, std::string password );

        void login();
        void updateHeaterStatus( bool heaterOn );
        void updateTemperature( double temperature );
        void getTime();

        double getCurrentSetpoint();

    private:
        
        std::string url;
        std::string username;
        std::string password;

        std::string session_cookie;

        std::map<std::string, double> parse_json( std::stringstream& resp_strm );

        std::unique_ptr<curlpp::Easy> getCurlHandle( const std::string endpoint ) const;

        bool is_between( const time_t timer, const std::string time0, const std::string time1 ) const;
        struct tm str_to_tm( const std::string str ) const;
        void strip_tm( struct tm &tim ) const;

};

#endif
