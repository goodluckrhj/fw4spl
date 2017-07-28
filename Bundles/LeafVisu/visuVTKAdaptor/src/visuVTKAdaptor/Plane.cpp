/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/Plane.hpp"

#include "visuVTKAdaptor/SPoint.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Plane.hpp>

#include <fwMath/IntrasecTypes.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>
#include <fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::Plane, ::fwData::Plane );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINTS_SLOT     = "updatePoints";
static const ::fwCom::Slots::SlotKeyType s_START_INTERACTION_SLOT = "startInteraction";
static const ::fwCom::Slots::SlotKeyType s_SELECT_PLANE_SLOT      = "selectPlane";

const ::fwCom::Signals::SignalKeyType Plane::s_INTERACTION_STARTED_SIG = "interactionStarted";

//------------------------------------------------------------------------------

Plane::Plane() noexcept :
    m_vtkPlane(nullptr),
    m_actorPlan(nullptr),
    m_vtkImplicitPlane(nullptr),
    m_vtkPlaneCollection(nullptr)
{
    newSlot(s_UPDATE_POINTS_SLOT, &Plane::updatePoints, this);
    newSlot(s_START_INTERACTION_SLOT, &Plane::startInteraction, this);
    newSlot(s_SELECT_PLANE_SLOT, &Plane::selectPlane, this);

    newSignal< InteractionStartedSignalType >(s_INTERACTION_STARTED_SIG);
}

//------------------------------------------------------------------------------

Plane::~Plane() noexcept
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Plane::doConfigure()
{
}

//------------------------------------------------------------------------------

void Plane::doStart()
{
    SLM_TRACE_FUNC();

    m_pPlane = this->getObject< ::fwData::Plane >();

    for( ::fwData::Point::sptr point :  m_pPlane.lock()->getPoints() )
    {
        ::fwRenderVTK::IAdaptor::sptr servicePoint =
            ::fwServices::add< ::fwRenderVTK::IAdaptor >
                ( point, "::visuVTKAdaptor::SPoint" );
        SLM_ASSERT("servicePoint not instanced", servicePoint);

        servicePoint->setRenderService(this->getRenderService());
        servicePoint->setRendererId( this->getRendererId() );
        servicePoint->setPickerId( this->getPickerId() );
        servicePoint->setAutoRender( this->getAutoRender() );
        servicePoint->start();

        this->registerService(servicePoint);

        m_connections.connect(point, ::fwData::Object::s_MODIFIED_SIG,
                              this->getSptr(), s_UPDATE_POINTS_SLOT);
        m_connections.connect(servicePoint, ::visuVTKAdaptor::SPoint::s_INTERACTION_STARTED_SIG,
                              this->getSptr(), s_START_INTERACTION_SLOT);
    }

    if (m_vtkPlaneCollection)
    {
        m_vtkImplicitPlane = vtkPlane::New();
        m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
    }

    setVtkPipelineModified();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Plane::doSwap()
{
    SLM_TRACE("SWAPPING Plane");
    m_pPlane = this->getObject< ::fwData::Plane >();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Plane::doUpdate()
{
    assert(!m_pPlane.expired());
    ::fwData::Plane::sptr plane = m_pPlane.lock();

    ::fwData::Point::sptr pt0 = plane->getPoints()[0];
    ::fwData::Point::sptr pt1 = plane->getPoints()[1];
    ::fwData::Point::sptr pt2 = plane->getPoints()[2];

    fwPlane planeDesc;
    ::fwMath::setValues(planeDesc, pt0->getCoord(), pt1->getCoord(), pt2->getCoord());

    if(m_vtkImplicitPlane)
    {
        fwVec3d normal = ::fwMath::getNormal(planeDesc);

        m_vtkImplicitPlane->SetOrigin(pt0->getCoord()[0], pt0->getCoord()[1], pt0->getCoord()[2]);
        m_vtkImplicitPlane->SetNormal(normal[0], normal[1], normal[2]);
        m_vtkImplicitPlane->Modified();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Plane::updatePoints()
{
    this->updating();

    auto sig = m_pPlane.lock()->signal< ::fwData::Object::ModifiedSignalType >(
        ::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void Plane::startInteraction()
{
    auto sig = m_pPlane.lock()->signal< InteractionStartedSignalType >( s_INTERACTION_STARTED_SIG);
    sig->asyncEmit( m_pPlane.lock() );
}

//------------------------------------------------------------------------------

void Plane::doStop()
{
    if (m_vtkPlaneCollection && m_vtkImplicitPlane)
    {
        m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
        m_vtkImplicitPlane->Delete();
        m_vtkImplicitPlane = 0;
    }

    m_connections.disconnect();

    this->unregisterServices();
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void Plane::setVtkPlaneCollection( vtkObject* col )
{
    if (m_vtkPlaneCollection != col)
    {
        if (m_vtkPlaneCollection)
        {
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
            }
            m_vtkPlaneCollection = 0;
        }

        if ( col )
        {
            m_vtkPlaneCollection = vtkPlaneCollection::SafeDownCast(col);
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
            }
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Plane::selectPlane(bool select)
{
    for( ServiceVector::value_type service :  m_subServices )
    {
        if(!service.expired())
        {
            ::visuVTKAdaptor::SPoint::sptr servicePoint = ::visuVTKAdaptor::SPoint::dynamicCast(service.lock());
            if(select)
            {
                servicePoint->setColor(1., 0., 0.);
            }
            else
            {
                servicePoint->setColor(1., 1., 1.);
            }
        }
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Plane::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Plane::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Plane::s_SELECTED_SIG, s_SELECT_PLANE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif //ANDROID
