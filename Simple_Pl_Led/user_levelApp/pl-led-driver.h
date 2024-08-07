/* Abushaqra - September - 03 - 2021 - created */
/* This Kernel Level driver header File is used with its corresponding PL Led Control Driver */
/* Any kind of distribution of this Kernel Level Driver without the Permission of the Author is FORBIDDEN */

// --------------------- IOCTL Strings ----------------------------------- //

#define IOCTL_DEVICE_DRIVER                                 'r'
#define IOCTL_LEDS_ON								    _IOWR (IOCTL_DEVICE_DRIVER, 0, int)
#define IOCTL_LEDS_OFF          					    _IOWR (IOCTL_DEVICE_DRIVER, 1, int)

// ---------------------------------------------------------------------- //
