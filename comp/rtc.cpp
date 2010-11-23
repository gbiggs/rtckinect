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
 * Component source file.
 */


#include "rtc.h"

#include <iostream>


RTC::RTC(RTC::Manager* manager)
    : RTC::DataFlowComponentBase(manager),
    tilt_port_("tilt", tilt_),
    image_port_("image", image_),
    depth_port_("tilt", depth_),
    //svc_prov_(),
    //svc_port_("<PORT_NAME>")
    cxt_(NULL),
    dev_(NULL)
{
}


RTC::~RTC()
{
}


RTC::ReturnCode_t RTC::onInitialize()
{
    bindParameter("cxt_num", cxt_num_, "0");
    /*std::string active_set =
        m_properties.getProperty("configuration.active_config", "default");
    m_configsets.update(active_set.c_str());*/

    comp.addInPort(tilt_port_.getName(), tilt_port_);
    comp.addOutPort(image_port_.getName(), image_port_);
    comp.addOutPort(depth_port_.getName(), depth_port_);
    //svc_port_.registerProvider("<INSTANCE_NAME>", "<TYPE_NAME>", svc_prov_);
    //comp.addPort(svc_port_);

    return RTC::RTC_OK;
}


/*RTC::ReturnCode_t RTC::onFinalize()
{
    return RTC::RTC_OK;
}*/


RTC::ReturnCode_t RTC::onActivated(RTC::UniqueId ec_id)
{
    if (freenect_init(&cxt_, NULL) < 0)
    {
        std::cerr << "Kinect initialisation failed.\n";
        return RTC::RTC_ERROR;
    }
    freenect_set_log_level(cxt_, FREENET_LOG_DEBUG);
    int num_devs = freenect_num_devices(cxt_);
    if (num_devs < 1)
    {
        std::cerr << "No Kinect devices found.\n";
        return RTC::RTC_ERROR;
    }
    elif (num_devs > 1)
    {
        std::cerr << num_devs << " Kinect devices found.\n";
    }

    if (freenect_open_device(cxt_, &dev_, dev_num) < 0)
    {
        std::cerr << "Failed to open Kinect device " << dev_num << ".\n";
        return RTC::RTC_ERROR;
    }

    freenect_set_tilt_degs(dev_, 0);
    freenect_set_led(dev_, LED_RED);

    freenect_set_depth_callback(dev_, depth_cb);
    freenect_set_depth_format(dev_, FREENECT_FORMAT_11_BIT);
    freenect_start_depth(dev_);

    freenect_set_rgb_callback(dev_ image_cb);
    freenect_set_rgb_format(dev_, FREENECT_FORMAT_RGB);
    freenect_start_rgb(dev_);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t RTC::onDeactivated(RTC::UniqueId ec_id)
{
    return RTC::RTC_OK;
}


RTC::ReturnCode_t RTC::onExecute(RTC::UniqueId ec_id)
{

    return RTC::RTC_OK;
}


static const char* spec[] =
{
    "implementation_id", "RTCKinect",
    "type_name",         "RTCKinect",
    "description",       "RT-Component for the Kinect sensor.",
    "version",           "1.0",
    "vendor",            "Geoffrey Biggs, AIST",
    "category",          "",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.cxt_num", "0",
    // Widget
    //"conf.__widget__.", "text",
    "conf.__widget__.cxt_num", "spin",
    // Constraints
    "conf.__constraints__.cxt_num", "0<=x",
    ""
};

extern "C"
{
    void rtc_init(RTC::Manager* manager)
    {
        coil::Properties profile(spec);
        manager->registerFactory(profile, RTC::Create<RTC>,
                RTC::Delete<RTC>);
    }
};

