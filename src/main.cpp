/* To compile:
 * $(CXX) -o thermo *.cpp -lcurlpp -uClibc
 *
 * This code was written by Dallen Ford
 * for the final project of ECE 531 at UNM
 * */

#include <iostream>
#include <string>

#include <signal.h>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

#include "thermo_errors.h"
#include "Thermostat.h"

#define DAEMON_NAME "thermod"

static const std::string DEFAULT_CONFIG_FILE = "config.txt";

static const std::string DEFAULT_TEMPERATURE_FILE = "/var/log/temperature";

static const std::string DEFAULT_STATUS_FILE = "/var/log/heater";

static const std::string HELP_STRING = "Usage: thermo [-h] [-c configfile]\n";

static const std::string WORKING_DIR = "/";

static void _exit_process( const thermo_error_t error )
{
    syslog(LOG_INFO, "%s", thermo_error_to_msg( error ).c_str()  );
    closelog();
    exit( error );
}

static void _signal_handler( const int signal )
{
    switch(signal)
    {
        case SIGHUP:
        {
            break;
        }
        case SIGTERM:
        {
            _exit_process(RECV_SIGTERM);
            break;
        }
        default:
        {
            syslog(LOG_INFO, "received unhandled signal");
        }
    }
}

static void _handle_fork( const pid_t pid )
{
    if(pid < 0)
    {
        _exit_process(NO_FORK);
    }
    
    if(pid > 0)
    {
        exit(OK);
    }
}

static void _daemonize(void)
{
	// fork
	pid_t pid = fork();

	// open syslog
	openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON );

	// handle fork results
	_handle_fork (pid);

	// become session leader
	if (setsid() < -1 )
	{
		_exit_process( NO_SETSID );
	}

	// set signal handling
	signal( SIGTERM, _signal_handler );
	signal (SIGHUP, _signal_handler );

	// permissions
	umask( S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
	umask( 666 );

	// change dir
	chdir( WORKING_DIR.c_str() );

	// close stdin, stdout, etc.
	for( long x = sysconf(_SC_OPEN_MAX); x >= 0; --x )
	{
		close(x);
	}

}

static std::string _readopts( int argc, char **argv )
{
	// parse options
	int c;
	std::string configFile = DEFAULT_CONFIG_FILE;
	while( ( c = getopt( argc, argv, "hc:" ) ) != -1 )
	{
		switch( c )
		{
			case( 'h' ):
			{
				std::cout << HELP_STRING;
				_exit_process( OK );
				break;
			}
			case( 'c' ):
			{
				configFile = optarg;
				break;
			}
			case '?':
			{
				if (optopt == 'c')
				{
					//fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if (isprint (optopt))
				{
					std::cout << HELP_STRING;
				//	std::cerr << "Unknown option '-" << static_cast<char>(optopt) << "'.\n";
				//	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				}
				else
				{
					fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
				}
				_exit_process( OPT_ERROR );
			}
			default:
			{
				abort ();
			}
		}
	}

#ifdef DEBUG
	std::cout << "Config File = " << configFile << std::endl;
#endif
    return configFile;
}

int main( int argc, char **argv )
{

// we will eventually daemonize this
//	_daemonize(); 

    std::string configFile = _readopts( argc, argv );

    ThermostatConfig thermostatConfig( configFile );

    Thermostat thermostat( thermostatConfig );

    thermostat.run();	
}
