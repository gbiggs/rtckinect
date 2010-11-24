/* RTC:Kinect
 *
 * Copyright (C) 2009-2010
 *     Geoffrey Biggs, Yuki Suga and contributors
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


// This is awful, but because the freenect API is callback-based...
coil::Mutex mutex;
bool new_image;
RTC::CameraImage image_data;
bool new_depth;
RTC::CameraImage depth_data;


void image_cb(freenect_device *dev, freenect_pixel *image, uint32_t timestamp)
{
    coil::Guard<coil::Mutex> guard(mutex);

    image_data.tm.sec = timestamp / 1000000000;
    image_data.tm.nsec = timestamp % 1000000000;
    for (unsigned int ii = 0; ii < FREENECT_RGB_SIZE; ii++)
    {
        image_data.pixels[ii] = image[ii];
    }
    new_image = true;
}


void depth_cb(freenect_device *dev, void *raw_depth, uint32_t timestamp)
{
    coil::Guard<coil::Mutex> guard(mutex);
    freenect_depth* depth(reinterpret_cast<freenect_depth*>(raw_depth));

    depth_data.tm.sec = timestamp / 1000000000;
    depth_data.tm.nsec = timestamp % 1000000000;
    for (unsigned int ii = 0; ii < FREENECT_DEPTH_SIZE; ii++)
    {
        depth_data.pixels[ii] = depth[ii];
    }
    new_depth = true;
}


RTCKinect::RTCKinect(RTC::Manager* manager)
    : RTC::DataFlowComponentBase(manager),
    tilt_port_("tilt", tilt_),
    led_port_("led", led_),
    image_port_("image", image_),
    depth_port_("depth", depth_),
    raw_accel_port_("raw_accel", raw_accel_),
    mks_accel_port_("mks_accel", mks_accel_),
    enable_camera_(true),
    enable_depth_(true),
    dev_num_(0),
    cxt_(NULL),
    dev_(NULL)
{
    tilt_.data = 0.0;
}


RTCKinect::~RTCKinect()
{
}


RTC::ReturnCode_t RTCKinect::onInitialize()
{
    bindParameter("enable_camera", enable_camera_, "1");
    bindParameter("enable_depth", enable_depth_, "1");
    bindParameter("dev_num", dev_num_, "0");

    addInPort(tilt_port_.getName(), tilt_port_);
    addOutPort(image_port_.getName(), image_port_);
    addOutPort(depth_port_.getName(), depth_port_);
    addOutPort(raw_accel_port_.getName(), raw_accel_port_);
    addOutPort(mks_accel_port_.getName(), mks_accel_port_);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t RTCKinect::onActivated(RTC::UniqueId ec_id)
{
    if (freenect_init(&cxt_, NULL) < 0)
    {
        std::cerr << "Kinect initialisation failed.\n";
        return RTC::RTC_ERROR;
    }
    freenect_set_log_level(cxt_, FREENECT_LOG_DEBUG);
    int num_devs = freenect_num_devices(cxt_);
    if (num_devs < 1)
    {
        std::cerr << "No Kinect devices found.\n";
        return RTC::RTC_ERROR;
    }
    else if (num_devs > 1)
    {
        std::cerr << num_devs << " Kinect devices found.\n";
    }

    if (freenect_open_device(cxt_, &dev_, dev_num_) < 0)
    {
        std::cerr << "Failed to open Kinect device " << dev_num_ << ".\n";
        return RTC::RTC_ERROR;
    }

    freenect_set_tilt_degs(dev_, tilt_.data);
    freenect_set_led(dev_, LED_GREEN);

    if (enable_camera_)
    {
        image_data.width = 640;
        image_data.width = 480;
        image_data.bpp = 32;
        image_data.format = "bitmap";
        image_data.fDiv = 1.0;
        image_data.pixels.length(FREENECT_RGB_SIZE);
        new_image = false;
        freenect_set_rgb_callback(dev_, image_cb);
        freenect_set_rgb_format(dev_, FREENECT_FORMAT_RGB);
        freenect_start_rgb(dev_);
    }

    if (enable_depth_)
    {
        depth_data.width = 640;
        depth_data.width = 480;
        depth_data.bpp = 11;
        depth_data.format = "bitmap";
        depth_data.fDiv = 1.0;
        depth_data.pixels.length(FREENECT_DEPTH_SIZE);
        new_depth = false;
        freenect_set_depth_callback(dev_, depth_cb);
        freenect_set_depth_format(dev_, FREENECT_FORMAT_11_BIT);
        freenect_start_depth(dev_);
    }

    return RTC::RTC_OK;
}


RTC::ReturnCode_t RTCKinect::onDeactivated(RTC::UniqueId ec_id)
{
    freenect_stop_rgb(dev_);
    freenect_stop_depth(dev_);
    return RTC::RTC_OK;
}


RTC::ReturnCode_t RTCKinect::onExecute(RTC::UniqueId ec_id)
{
    process_imu();

    if (tilt_port_.isNew())
    {
        tilt_port_.read();
        freenect_set_tilt_degs(dev_, tilt_.data);
    }

    if (freenect_process_events(cxt_) < 0)
    {
        std::cerr << "Error in Kinect event processing.\n";
        return RTC::RTC_ERROR;
    }
    coil::Guard<coil::Mutex> guard(mutex);
    if (new_image)
    {
        image_port_.write(image_data);
        new_image = false;
    }
    if (new_depth)
    {
        depth_port_.write(depth_data);
        new_depth = false;
    }

    if (led_port_.isNew())
    {
        led_port_.read();
        switch(led_.colour)
        {
            case RTCKinectTypes::LED_GREEN:
                freenect_set_led(dev_, LED_GREEN);
                break;
            case RTCKinectTypes::LED_RED:
                freenect_set_led(dev_, LED_RED);
                break;
            case RTCKinectTypes::LED_YELLOW:
                freenect_set_led(dev_, LED_YELLOW);
                break;
            case RTCKinectTypes::LED_BLINK_YELLOW:
                freenect_set_led(dev_, LED_BLINK_YELLOW);
                break;
            case RTCKinectTypes::LED_BLINK_GREEN:
                freenect_set_led(dev_, LED_BLINK_GREEN);
                break;
            case RTCKinectTypes::LED_BLINK_RED_YELLOW:
                freenect_set_led(dev_, LED_BLINK_RED_YELLOW);
                break;
            case RTCKinectTypes::LED_OFF:
            default:
                freenect_set_led(dev_, LED_OFF);
                break;
        }
    }

    return RTC::RTC_OK;
}


void RTCKinect::process_imu()
{
    setTimestamp(raw_accel_);
    int16_t ax, ay, az;
    freenect_get_raw_accel(dev_, &ax, &ay, &az);
    raw_accel_.data.ax = ax;
    raw_accel_.data.ay = ay;
    raw_accel_.data.az = az;
    raw_accel_port_.write();

    setTimestamp(mks_accel_);
    freenect_get_mks_accel(dev_, &mks_accel_.data.ax, &mks_accel_.data.ay,
            &mks_accel_.data.az);
    mks_accel_port_.write();
}


static const char* spec[] =
{
    "implementation_id", "RTCKinect",
    "type_name",         "RTCKinect",
    "description",       "RT-Component for the Kinect sensor.",
    "version",           "1.0",
    "vendor",            "Geoffrey Biggs, AIST",
    "category",          "Sensor",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.enable_camera", "1",
    "conf.default.enable_depth", "1",
    "conf.default.dev_num", "0",
    // Widget
    //"conf.__widget__.", "text",
    "conf.__widget__.enable_camera", "spin",
    "conf.__widget__.enable_depth", "spin",
    "conf.__widget__.dev_num", "spin",
    // Constraints
    "conf.__constraints__.enable_camera", "0<=x<=1",
    "conf.__constraints__.enable_depth", "0<=x<=1",
    "conf.__constraints__.dev_num", "0<=x",
    ""
};

extern "C"
{
    void rtc_init(RTC::Manager* manager)
    {
        coil::Properties profile(spec);
        manager->registerFactory(profile, RTC::Create<RTCKinect>,
                RTC::Delete<RTCKinect>);
    }
};

