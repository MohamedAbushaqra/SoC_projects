/* Abushaqra - August - 03 - 2024 - created */
/* This is a user level application to utilize the kernel driver and create a user cli to control the system fan */
/* Any kind of distribution of this application without the Permission of the Author is FORBIDDEN */

#include <iostream>
#include <cstring>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/ioctl.h>

#include "pwm-fan-controller.h"

struct option long_options[] =
{
    {"help", no_argument, 0, 'h'},
    {"fanconfig", required_argument, 0, 'c'},
	{"speed", required_argument, 0, 's'},
    {0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
    int opt;
	int percentage=0;
	int tmp=0;
	bool percentageFlag = false;
    std::string speedConfig;

    const char* deviceNode = "/dev/PWM_FAN_CONTROLLER";
    int deviceHandle = open(deviceNode, O_RDWR);
    if (deviceHandle < 0) {
        std::cerr << "user_level_app: could not open file handle to device node '" << deviceNode
                  << "'. Are all nodes present?\n";
        std::cerr << "Error: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt_long(argc, argv, "hc:s:", long_options, nullptr)) != -1)
	{
        switch (opt)
		{
            case 'h':
                std::cout << "- Set a preconfigured configuration speed using: -c <FULL,MIDDLE,QUIET,OFF> , --fanconfig=<FULL,MIDDLE,QUIET,OFF>\n";
				std::cout << "- Set a custom speed as percentage: -s <percentage> , --speed=<percentage>\n";
			return 0;

            case 'c':
				if (strcmp(optarg, "FULL") != 0 && strcmp(optarg, "MIDDLE") != 0 && strcmp(optarg, "QUIET") != 0 && strcmp(optarg, "OFF") != 0)
				{
					std::cout << "- The speed configuration is not valid, please choose between FULL, MIDDLE, QUIET or OFF!\n";
					return 1;
				}
				else
					speedConfig = optarg;
			break;

            case 's':
				percentage = std::stoi(optarg);
				if (percentage >= 0 && percentage <=100)
					percentageFlag = true;
				else
				{
					std::cout << "The provided value is out of range, choose between 0 and 100\n";
					percentageFlag = false;
					return 1;
				}
			break;

            default:
                std::cerr << "Bad usage, use --help to know more about using this command line\n";
			return 1;
        }
    }

    if (!speedConfig.empty())
	{
        std::cout << "Setting the fan config to: " << speedConfig << "\n";
		if(speedConfig == "FULL")
			ioctl(deviceHandle, IOCTL_PWM_SPEED_FULLSPEED);
		else if (speedConfig == "MIDDLE")
			ioctl(deviceHandle, IOCTL_PWM_SPEED_MIDDLE);
		else if (speedConfig == "QUIET")
			ioctl(deviceHandle, IOCTL_PWM_SPEED_QUIET);
		else if (speedConfig == "OFF")
			ioctl(deviceHandle, IOCTL_PWM_SPEED_OFF);
    }

	if (percentageFlag)
	{
        std::cout << "Setting fan speed to: " << percentage << "\n";
		tmp = (percentage/100.0)*255;
		ioctl(deviceHandle, IOCTL_PWM_SPEED_CUSTOM, tmp);
    }


    return 0;
}
