/* Abushaqra - August - 01 - 2024 - created */
/* This Kernel Level driver enables the control of the pwm fan ip core in Vivado */
/* Any kind of distribution of this Kernel Level Driver without the Permission of the Author is FORBIDDEN */

#define DRIVER_NAME "PWM_FAN_CONTROLLER"

// --------------------- IOCTL Strings ----------------------------------- //
#define IOCTL_DEVICE_DRIVER		'r'
#define IOCTL_PWM_SPEED_FULLSPEED	_IOWR (IOCTL_DEVICE_DRIVER, 0, int)
#define IOCTL_PWM_SPEED_MIDDLE		_IOWR (IOCTL_DEVICE_DRIVER, 1, int)
#define IOCTL_PWM_SPEED_QUIET		_IOWR (IOCTL_DEVICE_DRIVER, 2, int)
#define IOCTL_PWM_SPEED_OFF			_IOWR (IOCTL_DEVICE_DRIVER, 4, int)
#define IOCTL_PWM_SPEED_CUSTOM		_IOWR (IOCTL_DEVICE_DRIVER, 5, int)
