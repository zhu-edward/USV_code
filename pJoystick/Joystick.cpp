#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <string>
#include "Joystick.h"


Joystick::Joystick() :
    initialized (false),
    opened(false),
    name(""),
    devPath("")
{
    // List of device names to try
    devList.push_back("/dev/input/js0");
    devList.push_back("/dev/input/js1");
    devList.push_back("/dev/input/js2");
    devList.push_back("/dev/js0");
    devList.push_back("/dev/js1");
    devList.push_back("/dev/js2");
}


Joystick::~Joystick() {
    closefd();
}

bool Joystick::open (const std::string device="")
{
    joy_fd=-1;
    if (!device.empty()) {
        joy_fd = ::open (device.c_str(), O_RDONLY | O_NONBLOCK);
        devPath=device;
    } else {
        // No device specified, use the first successfully opened joystick from the
        // hard coded list
        bool foundJS=false;       
        for (unsigned int i=0; i<devList.size() && joy_fd==-1; i++) {
            joy_fd = ::open (devList[i].c_str(), O_RDONLY | O_NONBLOCK);
            devPath=devList[i];
        }
    }

    if (joy_fd == -1) {
        return false;
    }
    else {
        
        std::cerr << "Successfully opened joystick device: " << devPath << std::endl;

        char d_buttons;
        char d_axes;
        char d_name[128];


        // get device name
        if (ioctl(joy_fd, JSIOCGNAME (sizeof(d_name)), d_name) < 0) 
            name = "Unknown";
        else
            name = d_name;

        
        // get the version
        ioctl(joy_fd, JSIOCGVERSION, &version);
    
        // get number of buttons
        ioctl (joy_fd, JSIOCGBUTTONS, &d_buttons);
        numButtons = d_buttons;
        
        // get number of axes
        ioctl (joy_fd, JSIOCGAXES, &d_axes);
        numAxes = d_axes;

        // Reserve space for the buttons and axes
        buttons.resize(numButtons);
        axes.resize(numAxes);
        

    }
    
    opened=true;
    return true;
}



bool Joystick::getJoystickEvent() {
    int bytes = read(joy_fd,&joy_event,sizeof(struct js_event));
    
    if (bytes<0)
        return false;

    // Mask out the init events that happen at start 
    joy_event.type &= ~JS_EVENT_INIT;

    updateTime = joy_event.time;
    
    if (joy_event.type & JS_EVENT_BUTTON)  {
        //std::cerr << "Setting button " << (int)joy_event.number << " to " << joy_event.value << std::endl;
        buttons[(int)joy_event.number] = joy_event.value;
    }    

    if (joy_event.type & JS_EVENT_AXIS) {
        //std::cerr << "Setting axis " << (int)joy_event.number << " to " << joy_event.value << std::endl;
        axes[(int)joy_event.number] = joy_event.value;
    }

    return true;
    
}


bool Joystick::closefd() {
    buttons.clear();
    axes.clear();
    numButtons=0;
    numAxes=0;
    if (opened) {
        return close(joy_fd);
    }
}
