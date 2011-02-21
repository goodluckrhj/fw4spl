/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/lexical_cast.hpp>

#include <fwTools/TypeInfo.hpp>
#include <fwTools/fwID.hpp>
#include <fwTools/Factory.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>

#include "fwServices/Base.hpp"
#include "fwServices/IXMLParser.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/op/Add.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ServiceConfig.hpp"
#include "fwServices/registry/ServiceFactory.hpp"

namespace fwServices
{

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceType )
{
    OSLM_ASSERT("Unable to add service " <<serviceType<< " on a null object", obj);
    std::string defaultImplementation =
            ::fwServices::registry::ServiceFactory::getDefault()->getDefaultImplementationIdFromObjectAndType( obj->getClassname(), serviceType );
    return ::fwServices::add( obj , serviceType , defaultImplementation ) ;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceType , std::string _implementationId )
{
    IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( serviceType, _implementationId );
    ::fwServices::ObjectServiceRegistry::getDefault()->registerService( obj , srv );
    return srv;
}

//------------------------------------------------------------------------------

::fwServices::IService::sptr add( ::fwTools::Object::sptr obj , std::string serviceType , std::string _implementationId , std::string uid)
{
    IService::sptr srv = ::fwServices::add( obj , serviceType , _implementationId ) ;
    OSLM_ASSERT( "Try to set ID: "<<uid<<" but already has an ID: "<<srv->getID(), !srv->hasID() );
    srv->setID( uid ) ;
    return srv ;
}

//------------------------------------------------------------------------------

}

