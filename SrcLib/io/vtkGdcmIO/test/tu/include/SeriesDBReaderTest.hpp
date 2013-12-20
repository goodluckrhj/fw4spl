/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKGDCMIO_TEST_TU_SERIESDBREADERTEST_HPP__
#define __FWVTKGDCMIO_TEST_TU_SERIESDBREADERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace fwMedData
{
    class SeriesDB;
}

namespace vtkGdcmIO
{
namespace ut
{

class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SeriesDBReaderTest );
    CPPUNIT_TEST( readSeriesDBTest );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void readSeriesDBTest();

private:
    ::fwMedData::SeriesDB::sptr m_seriesDB;

};

} // namespace ut
} // namespace vtkGdcmIO

#endif // __FWVTKGDCMIO_TEST_TU_SERIESDBREADERTEST_HPP__

