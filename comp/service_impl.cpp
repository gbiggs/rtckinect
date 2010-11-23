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
 * Service implementation class.
 */


#include "comp/service_impl.h"

using namespace Services;

//////////////////////////////////////////////////////////////////////////////
// Service provider class

<INTERFACE>Provider::<INTERFACE>Provider()
{
}


<INTERFACE>Provider::~<INTERFACE>Provider()
{
}


RTC::<RETURN_TYPE> <INTERFACE>Provider::<METHOD>()
    throw(CORBA::SystemException)
{
    RTC::<RETURN_TYPE> result;
    return result;
}

