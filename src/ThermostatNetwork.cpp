#include "ThermostatNetwork.h"

#include "json/json.h"

#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <sstream>


ThermostatNetwork::ThermostatNetwork( std::string url, std::string username, std::string password ) : url(url), username(username), password(password)
{
//    curlHandle.setOpt(curlpp::Options::Url(url));
//    curlHandle.setOpt(curlpp::Options::WriteFunction(write_callback));
//    curlHandle.setOpt(curlpp::Options::WriteData( &buffer));a
    
}

void ThermostatNetwork::getTime()
{
    std::stringstream response;
    auto curlHandle = getCurlHandle( std::string( "/time" ) );
    curlHandle->setOpt( new curlpp::options::WriteStream( &response ) );

    curlHandle->perform();

    Json::Value root;
    response >> root;

    long seconds = root["time"].asLargestInt();
    std::ostringstream syscall;
    syscall << "date -s @" << seconds;
    system(syscall.str().c_str() );
    
}

void ThermostatNetwork::login()
{
    std::stringstream response;
    session_cookie = std::string();
    auto curlHandle = getCurlHandle( std::string( "/login" ) );
   
    Json::Value root;
    root["username"] = username;
    root["password"] = password;
    std::ostringstream postStream;
    postStream << root;
    curlHandle->setOpt(new curlpp::options::PostFields(postStream.str()));
    curlHandle->setOpt(new curlpp::options::PostFieldSize(postStream.str().size()));
    curlHandle->setOpt( new curlpp::options::WriteStream( &response ) );

    curlHandle->perform();
    
    std::list<std::string> cookieList;
    curlpp::infos::CookieList::get(*curlHandle, cookieList);
    session_cookie = cookieList.front();
}

void ThermostatNetwork::updateHeaterStatus( bool heaterOn )
{
    auto curlHandle = getCurlHandle( "/setstatus" );

    Json::Value json_root;
    json_root["heaterOn"] = heaterOn;
    std::ostringstream postStream;
    postStream << json_root;
    curlHandle->setOpt( new curlpp::options::PostFields( postStream.str() ) );
    curlHandle->setOpt( new curlpp::options::PostFieldSize( postStream.str().size() ) );
    std::stringstream response;
    curlHandle->setOpt( new curlpp::options::WriteStream( &response ) );

    curlHandle->perform();
}

void ThermostatNetwork::updateTemperature( double temperature )
{
    std::stringstream response;
    bool retry;
    do {
        retry = false;
        response.str("");       
        auto curlHandle = getCurlHandle( "/settemp" );
    
        Json::Value json_root;
        json_root["temperature"] = temperature;
        std::ostringstream postStream;
        postStream << json_root;

        curlHandle->setOpt( new curlpp::options::PostFields( postStream.str() ) );
        curlHandle->setOpt( new curlpp::options::PostFieldSize( postStream.str().size() ) );
        curlHandle->setOpt( new curlpp::options::WriteStream( &response ) );
 
        curlHandle->perform();
        if( response.str().find( "Not Authenticated" ) != std::string::npos )
        {
            retry = true;
            login();
        }
    }
    while( retry );
}

double ThermostatNetwork::getCurrentSetpoint()
{
    std::stringstream response;
    bool retry = false;
    do {
        retry = false;
        response.str(""); // clear response string
        auto curlHandle = getCurlHandle( "/setpoints" );

        curlHandle->setOpt( new curlpp::options::WriteStream( &response ) );

        curlHandle->perform();

        if( response.str().find( "Not Authenticated" ) != std::string::npos )
        {
            retry = true;
            login();
        }
    }
    while( retry );
    
    std::map<std::string, double> setpoints = parse_json( response );

    time_t now = time(NULL);

    for( auto setpoint_it = setpoints.cbegin();
            setpoint_it != setpoints.cend();
       )
    {
        auto point0 = *setpoint_it;
        std::pair<std::string, double> point1;
        setpoint_it++;
        if( setpoint_it == setpoints.cend() )
            point1 = *(setpoints.cbegin());
        else
            point1 = *setpoint_it;

        if( is_between( now, point0.first, point1.first ) )
        {
            return point0.second;
        }
    }
    
    return 0;
}

std::unique_ptr<curlpp::Easy> ThermostatNetwork::getCurlHandle( const std::string endpoint ) const
{
    auto curlHandle = std::make_unique<curlpp::Easy>();
    curlHandle->setOpt( new curlpp::options::Url(url + endpoint ) );
    std::list<std::string> header;
    header.push_back("Content-type: application/json");
    header.push_back("Accept: application/json" );
    curlHandle->setOpt( new curlpp::options::HttpHeader( header ) );
    curlHandle->setOpt( curlpp::options::CookieList( session_cookie ) );

    return curlHandle;
}

std::map<std::string, double> ThermostatNetwork::parse_json( std::stringstream& resp_strm )
{
    std::map<std::string, double> setpointsMap;
    
    Json::Value json_root;
    resp_strm >> json_root;

    const Json::Value setpoints_json = json_root["setpoints"];

    for(int index = 0; index < setpoints_json.size(); ++index )
    {
        auto setpoint = setpoints_json[index];
        std::string beginTime = setpoint["beginTime"].asString();
        double temperature = setpoint["temperature"].asDouble();

        setpointsMap.insert( std::make_pair( beginTime, temperature ) );
    }

    return setpointsMap;
}

bool ThermostatNetwork::is_between( const time_t timer, const std::string time0, const std::string time1 ) const
{
    struct tm tm0 = str_to_tm( time0 );
    struct tm tm1 = str_to_tm( time1 );

    struct tm tm2 = tm1;

    struct tm tmr = *( localtime( &timer ) );
    strip_tm( tmr );

    // get integer values
    time_t t0 = mktime( &tm0 );
    time_t t1 = mktime( &tm1 );
    time_t tmrt = mktime( &tmr );

    bool ret = ( ( t0 <= tmrt ) && ( tmrt < t1 ) ) // normal check
        || ( ( t0 <= tmrt ) && ( t1 < tmrt ) && ( t1 < t0 ) ) // first half of wrap-around check
        || ( ( t0 > tmrt ) && ( t0 > t1 ) && (tmrt < t1 ) );

    return ret;
}

struct tm ThermostatNetwork::str_to_tm( const std::string str ) const
{
    struct tm tim;
    std::string::size_type cpos = str.find(":");
    tim.tm_hour = stoi( str.substr( 0, str.size() - cpos ) );
    tim.tm_min = stoi( str.substr( cpos + 1 ) );

    // verify that all the other fields are 0
    strip_tm( tim );

    return tim;
}

void ThermostatNetwork::strip_tm( struct tm &tim ) const
{
    tim.tm_sec = 0;
    tim.tm_mday = 1; // must be 1-31
    tim.tm_mon = 0;
    tim.tm_year = 70;
    tim.tm_wday = 0;
    tim.tm_yday = 0;
    tim.tm_isdst = 0;
}
