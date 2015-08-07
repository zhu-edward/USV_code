
Open Loop Control of Torqeedo Motor

---------------------------------------------

Mission file:
Variable Name   Format            Notes
----------------------------------------------------------------------
Device          string            The /dev/* device to use
Name            string            The human readable label for this motor.  
                                  Such as "Starboard" or "Port".
                                  NOTE: The publications and subscriptions will use this name.
ControlRate     double            The driver will stop the motor unless it gets command messages
                                  at this rate (in Hz) or faster.  Set to zero to disable this
                                  feature.


Subscriptions:
Variable Name      Format      Notes
------------------------------------------------------------------------------
<name>_CMD         Double      Range: (-100, 100)


Publications:
None.  This is open loop control.  There is some info that can be read from the prop, but
it is not clear if this is voltage, RPM, current, etc.  
