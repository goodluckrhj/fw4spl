/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwData/Composite.hpp>
#include <fwDataCamp/Version.hpp>

#include <fwActivities/ActivitySeries.hpp>

#include "DataCampHelper.hpp"
#include "fwActivitiesCamp/ActivitySeriesTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivitiesCamp::ut::ActivitySeriesTest );

namespace fwActivitiesCamp
{
namespace ut
{

void ActivitySeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void ActivitySeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActivitySeriesTest::propertiesTest()
{
    const ::fwActivities::ActivitySeries::ConfigIdType activity_config_id = "Visu2D";

    const ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                       ("patient")
                                                                       ("study")
                                                                       ("equipment")
                                                                       ("instance_uid")
                                                                       ("modality")
                                                                       ("date")
                                                                       ("time")
                                                                       ("performing_physicians_name")
                                                                       ("description")
                                                                       ("activity_config_id")
                                                                       ("data");

    ::fwData::Composite::sptr data = ::fwData::Composite::New();

    ::fwActivities::ActivitySeries::sptr obj = ::fwActivities::ActivitySeries::New();
    obj->setActivityConfigId(activity_config_id);
    obj->setData(data);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@activity_config_id", activity_config_id);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@data", obj->getData());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@data", data);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivitiesCamp
