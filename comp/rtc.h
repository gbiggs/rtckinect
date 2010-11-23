/* RTC:Kinect
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
 * Component header file.
 */


#ifndef RTC_H__
#define RTC_H__

#include <rtm/Manager.h>
#include <rtm/DataFlowComponentBase.h>
#include <rtm/InPort.h>
#include <rtm/OutPort.h>
//#include <rtm/CorbaPort.h>
//#include "service_impl.h"
#include <libfreenect.h>

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
: public RTCKinect::DataFlowComponentBase
{
    public:
        RTCKinect(RTC::Manager* manager);
        ~RTCKinect();

        virtual RTC::ReturnCode_t onInitialize();
        //virtual RTC::ReturnCode_t onFinalize();
        virtual RTC::ReturnCode_t onActivated(RTC::UniqueId ec_id);
        virtual RTC::ReturnCode_t onDeactivated(RTC::UniqueId ec_id);
        virtual RTC::ReturnCode_t onExecute(RTC::UniqueId ec_id);

    private:
        RTC::TimedFloat tilt_;
        RTC::InPort<RTC::> tilt_port_;
        RTC::CameraImage image_;
        RTC::OutPort<RTC::CameraImage> image_port_;
        RTC::CameraImage depth_;
        RTC::OutPort<RTC::CameraImage> depth_port_;
        //ServiceProvider svc_prov_;
        //RTC::CorbaPort svc_port_;

        unsigned int dev_num_;
        freenect_context* cxt_;
        freenect_device* dev_;

        void image_cb(freenect_device *dev, freenect_pixel *image,
                uint32_t timestamp);
        void depth_cb(freenect_device *dev, freenect_depth *depth_data,
                uint32_t timestamp);
};


extern "C"
{
    DLL_EXPORT void rtc_init(RTC::Manager* manager);
};

#endif // RTC_H__

