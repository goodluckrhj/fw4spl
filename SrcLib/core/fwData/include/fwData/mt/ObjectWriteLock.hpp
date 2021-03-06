/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_MT_OBJECTWRITELOCK_HPP__
#define __FWDATA_MT_OBJECTWRITELOCK_HPP__

#include "fwData/Object.hpp"
#include "fwData/config.hpp"

#include <fwCore/mt/types.hpp>

namespace fwData
{
namespace mt
{

/**
 * @brief  A helper to lock object on exclusive mode.
 */
class FWDATA_CLASS_API ObjectWriteLock
{

public:

    /**
     * @brief Constructor : owns an exclusive lock on object mutex.
     * If adopt_lock==false : the mutex is not locked (call lock() to lock mutex)
     */
    FWDATA_API ObjectWriteLock( ::fwData::Object::sptr obj, bool adopt_lock = true );

    /// Destructor. Does nothing
    FWDATA_API ~ObjectWriteLock();

    /// Adds exclusive lock on object mutex
    FWDATA_API void lock();

    /// Releases lock on object mutex
    FWDATA_API void unlock();

private:

    /// lock on object mutex
    ::fwCore::mt::WriteLock m_lock;
};

} // mt
} // fwData

#endif // __FWDATA_MT_OBJECTWRITELOCK_HPP__
