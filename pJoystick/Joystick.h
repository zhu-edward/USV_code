#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <linux/joystick.h>
#include <string>
#include <vector>

class Joystick
{
 public:
    Joystick();
    virtual ~Joystick();


    /* Open a joystick device.
     * @param device A device (e.g. /dev/input/jsX).
     */
    bool open (const std::string device);
    
    /* Close the joystick device.
     */
    bool closefd();  

    /* Get the next joystick event.
     */
    bool getJoystickEvent();

    
    // The buttons and their state
    std::vector<int> buttons;

    // The axes and their state
    std::vector<int> axes;
    
    // Time of the update
    int32_t updateTime;

    // The device name
    std::string name;

    // The device path
    std::string devPath;


    // List of potential joystick device names if one is not supplied
    std::vector<std::string> devList;

    // The device version
    unsigned int version;

 private:
    int joy_fd;// file descriptor for the opened device
    bool opened;
    bool initialized;

    struct js_event joy_event;


    int numButtons;
    int numAxes;


    
};
#endif // JOYSTICK_H
