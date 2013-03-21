/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwAtoms/registry/macros.hpp"
#include "fwAtoms/Map.hpp"

fwCampImplementMacro((fwAtoms)(Map))
{
    typedef ::fwAtoms::Map::MapType& (::fwAtoms::Map::* AccessorType) ();
    AccessorType get = &::fwAtoms::Map::getValue;

    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwAtoms")
        .base< ::fwAtoms::Base>()
        .property("value", get);
}

fwAtomsRegisterMacro( ::fwAtoms::Map );

namespace fwAtoms
{

void Map::insert(const Base::sptr key, Base::sptr value)
{
    m_value[key] = value;
}

//------------------------------------------------------------------------------

size_t Map::getSize() const
{
    return m_value.size();
}

//------------------------------------------------------------------------------

Map::IteratorType Map::begin()
{
    return m_value.begin();
}

//------------------------------------------------------------------------------

Map::ConstIteratorType Map::begin() const
{
    return m_value.begin();
}

//------------------------------------------------------------------------------

Map::IteratorType Map::end()
{
    return m_value.end();
}

//------------------------------------------------------------------------------

Map::ConstIteratorType Map::end() const
{
    return m_value.end();
}

//------------------------------------------------------------------------------

const Map::MapType& Map::getValue() const
{
    return m_value;
}

//------------------------------------------------------------------------------

Map::MapType& Map::getValue()
{
    return m_value;
}

//------------------------------------------------------------------------------

bool Map::isEmpty() const
{
    return m_value.empty();
}

//------------------------------------------------------------------------------

Base::sptr Map::operator[](std::string index)
{
    MapType::const_iterator cIt = m_value.begin();
    Base::sptr result;

    for(;cIt != m_value.end(); ++cIt)
    {
        if(cIt->first->getString() == index)
        {
            result = cIt->second;
            break;
        }
    }
    return result;
}

//------------------------------------------------------------------------------

Base::sptr Map::clone() const
{
    Map::sptr cloneMap = Map::New();
    MapType& valueMap = cloneMap->getValue();
    BOOST_FOREACH(const ValueType &elem, m_value)
    {
        valueMap.insert( ValueType(elem.first, elem.second->clone() ) );
    }
    return cloneMap;
}

//------------------------------------------------------------------------------

Map::MapType::const_iterator Map::find(Base::sptr key) const
{
    return m_value.find(key);
}

//------------------------------------------------------------------------------

size_t Map::size()
{
    return m_value.size();
}


}
