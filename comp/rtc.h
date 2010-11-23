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
 * Component header file.
 */


#ifndef RTC_H__
#define RTC_H__

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/InPort.h>
#include <rtm/OutPort.h>
#include <rtm/idl/BasicDataType.hh>
#include <rtm/idl/ExtendedDataTypes.hh>
#include <rtm/idl/InterfaceDataTypes.hh>
#include <libfreenect.h>
#include <stdexcept>

using namespace RTC;


// Base exception
class BaseRTCError : public std::runtime_error
{
    public:
        BaseRTCError(const std::string &arg)
            : std::runtime_error(std::string("Base error ") + arg)
        {}
};


class RTCKinect
: public RTC::DataFlowComponentBase
{
    public:
        RTCKinect(RTC::Manager* manager);
        ~RTCKinect();

        virtual RTC::ReturnCode_t onInitialize();
        virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);
        virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);
        virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

    private:
        RTC::TimedFloat tilt_;
        RTC::InPort<RTC::TimedFloat> tilt_port_;
        RTC::CameraImage image_;
        RTC::OutPort<RTC::CameraImage> image_port_;
        RTC::CameraImage depth_;
        RTC::OutPort<RTC::CameraImage> depth_port_;
        RTC::TimedAcceleration3D raw_accel_;
        RTC::OutPort<RTC::TimedAcceleration3D> raw_accel_port_;
        RTC::TimedAcceleration3D mks_accel_;
        RTC::OutPort<RTC::TimedAcceleration3D> mks_accel_port_;

        unsigned int dev_num_;
        freenect_context* cxt_;
        freenect_device* dev_;

        void process_imu();
};


void image_cb(freenect_device *dev, freenect_pixel *image, uint32_t timestamp);
void depth_cb(freenect_device *dev, freenect_depth *depth_data,
        uint32_t timestamp);

extern "C"
{
    DLL_EXPORT void rtc_init(RTC::Manager* manager);
};

#endif // RTC_H__

