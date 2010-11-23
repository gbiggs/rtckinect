/* RTC:
 *
 * Copyright (C) 2009-2010
 *     Geoffrey Biggs
 *     RT-Synthesis Research Group
 *     Intelligent Systems Research Institute,
 *     National Institute of Advanced Industrial Science and Technology (AIST),
 *     Japan
 *     All rights reserved.
 * Licensed under the Eclipse Public License -v 1.0 (EPL)
 * http://www.opensource.org/licenses/eclipse-1.0.txt
 *
 * Source file for launching the component in stand-alone mode.
 */


#include <iostream>
#include <rtm/Manager.h>
#include <string>
#include <stdlib.h>

#include "rtc.h"

void ModuleInit(RTC::Manager* manager)
{
    rtc_init(manager);
    RTC::RtcBase* comp;
    comp = manager->createComponent("RTCKinect");

    if (comp == NULL)
    {
        std::cerr << "RTC:Kinect component creation failed." << std::endl;
        abort();
    }
}


int main(int argc, char** argv)
{
    RTC::Manager *manager;
    manager = RTC::Manager::init(argc, argv);
    manager->init(argc, argv);
    manager->setModuleInitProc(ModuleInit);
    manager->activateManager();
    manager->runManager();

    return 0;
}

