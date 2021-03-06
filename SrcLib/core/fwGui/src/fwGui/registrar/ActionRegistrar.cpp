/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>

#include <fwTools/fwID.hpp>
#include <fwServices/macros.hpp>

#include "fwGui/GuiRegistry.hpp"
#include "fwGui/registrar/ActionRegistrar.hpp"

namespace fwGui
{
namespace registrar
{

//-----------------------------------------------------------------------------

ActionRegistrar::ActionRegistrar(const std::string &sid) : m_sid(sid)
{
}

//-----------------------------------------------------------------------------

ActionRegistrar::~ActionRegistrar()
{
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStopping()
{
    ::fwGui::GuiRegistry::actionServiceStopping(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceStarting()
{
    ::fwGui::GuiRegistry::actionServiceStarting(m_sid);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetActive(bool isActive)
{
    ::fwGui::GuiRegistry::actionServiceSetActive(m_sid, isActive);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetExecutable(bool isExecutable)
{
    ::fwGui::GuiRegistry::actionServiceSetExecutable(m_sid, isExecutable);
}

//-----------------------------------------------------------------------------

void ActionRegistrar::actionServiceSetVisible(bool isVisible)
{
    ::fwGui::GuiRegistry::actionServiceSetVisible(m_sid, isVisible);
}

//-----------------------------------------------------------------------------

} // namespace registrar
} //namespace fwGui
