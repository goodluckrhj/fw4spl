/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwThread/Timer.hpp"
#include "fwThread/Worker.hpp"

#include <fwCore/TimeStamp.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>

namespace fwThread
{

//------------------------------------------------------------------------------

std::size_t WorkerThread( ::boost::asio::io_service& io_service )
{
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Start");
    std::size_t res = io_service.run();
    OSLM_TRACE("Thread " << getCurrentThreadId() <<" Finish");
    return res;
}

/**
 * @brief Private implementation of fwThread::Worker using boost::asio.
 */
class WorkerAsio : public ::fwThread::Worker
{
public:
    typedef ::boost::asio::io_service IOServiceType;
    typedef ::boost::asio::io_service::work WorkType;
    typedef std::shared_ptr< WorkType > WorkPtrType;
    typedef std::thread ThreadType;

    WorkerAsio();

    virtual ~WorkerAsio();

    void stop();

    void post(TaskType handler);

    ThreadIdType getThreadId() const;

    SPTR(::fwThread::Timer) createTimer();

    virtual void processTasks();

    virtual void processTasks(PeriodType maxtime);

protected:

    /// Copy constructor forbidden
    WorkerAsio( const WorkerAsio& );

    /// Copy operator forbidden
    WorkerAsio& operator=( const WorkerAsio& );

    /// Class provides functionality to manipulate asynchronous tasks.
    IOServiceType m_ioService;

    /// Class to inform the io_service when it has work to do.
    WorkPtrType m_work;

    /// Thread created and managed by the worker.
    SPTR(ThreadType) m_thread;
};

//------------------------------------------------------------------------------

/**
 * @brief Private Timer implementation using boost::asio.
 */
class TimerAsio : public ::fwThread::Timer
{
public:
    /**
     * @brief Constructs a TimerAsio from given io_service.
     */
    TimerAsio(::boost::asio::io_service& ioSrv);

    ~TimerAsio();

    /// Starts or restarts the timer.
    void start();

    /// Stops the timer and cancel all pending operations.
    void stop();

    /// Sets time duration.
    void setDuration(TimeDurationType duration);

    /// Returns if the timer mode is 'one shot'.
    bool isOneShot() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_oneShot;
    }

    /// Sets timer mode.
    void setOneShot(bool oneShot)
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        m_oneShot = oneShot;
    }

    /// Returns true if the timer is currently running.
    bool isRunning() const
    {
        ::fwCore::mt::ScopedLock lock(m_mutex);
        return m_running;
    }

protected:

    void cancelNoLock();
    void rearmNoLock(TimeDurationType duration);

    void call(const ::boost::system::error_code& code);

    /// Copy constructor forbidden.
    TimerAsio( const TimerAsio& );

    /// Copy operator forbidden.
    TimerAsio& operator=( const TimerAsio& );

    /// Timer object.
    ::boost::asio::deadline_timer m_timer;

    /// Time to wait until timer's expiration.
    TimeDurationType m_duration;

    /// Timer's mode.
    bool m_oneShot;

    /// Timer's state.
    bool m_running;
};

//------------------------------------------------------------------------------

// ---------- WorkerAsio private implementation ----------

WorkerAsio::WorkerAsio() :
    m_ioService(),
    m_work( std::make_shared< WorkType >(std::ref(m_ioService)) )
{
    std::packaged_task< ::fwThread::Worker::ExitReturnType() > task( std::bind(&WorkerThread, std::ref(m_ioService)) );
    std::future< ::fwThread::Worker::ExitReturnType > ufuture = task.get_future();

    m_thread = std::make_shared< ThreadType >( std::move( task ) );

    m_future = std::move(ufuture);
}

WorkerAsio::~WorkerAsio()
{
    if(!m_ioService.stopped())
    {
        this->stop();
    }
}

//------------------------------------------------------------------------------

void WorkerAsio::stop()
{
    m_work.reset();
    m_thread->join();
}

SPTR(::fwThread::Timer) WorkerAsio::createTimer()
{
    return std::make_shared< TimerAsio >(std::ref(m_ioService));
}

//------------------------------------------------------------------------------

void WorkerAsio::post(TaskType handler)
{
    m_ioService.post(handler);
}

//------------------------------------------------------------------------------

ThreadIdType WorkerAsio::getThreadId() const
{
    return m_thread->get_id();
}

//------------------------------------------------------------------------------

void WorkerAsio::processTasks()
{
    m_ioService.poll();
}

//------------------------------------------------------------------------------

void WorkerAsio::processTasks(PeriodType maxtime)
{
    ::fwCore::TimeStamp timeStamp;
    timeStamp.setLifePeriod(maxtime);
    timeStamp.modified();
    while(timeStamp.periodExpired())
    {
        m_ioService.poll_one();
    }
}

// ---------- Worker ----------
SPTR(Worker) Worker::defaultFactory()
{
    return std::make_shared< WorkerAsio >();
}

// ---------- Timer private implementation ----------

TimerAsio::TimerAsio(::boost::asio::io_service& ioSrv) :
    m_timer(ioSrv),
    m_duration(std::chrono::seconds(1)),
    m_oneShot(false),
    m_running(false)
{
}

TimerAsio::~TimerAsio()
{
}

//------------------------------------------------------------------------------

void TimerAsio::setDuration(TimeDurationType duration)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    m_duration = duration;
}

//------------------------------------------------------------------------------

void TimerAsio::start()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    this->rearmNoLock(m_duration);
    m_running = true;
}

//------------------------------------------------------------------------------

void TimerAsio::stop()
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    if (m_running )
    {
        m_running = false;
        this->cancelNoLock();
    }
}

//------------------------------------------------------------------------------

void TimerAsio::rearmNoLock(TimeDurationType duration)
{
    this->cancelNoLock();
    ::boost::posix_time::time_duration d =
        ::boost::posix_time::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
    m_timer.expires_from_now( d );
    m_timer.async_wait( std::bind(&TimerAsio::call, this, std::placeholders::_1));
}

//------------------------------------------------------------------------------

void TimerAsio::call(const ::boost::system::error_code& error)
{
    if(!error)
    {
        // We keep a reference to prevent a deletion of the Timer before the call back is over
        // This means the timer may delete itself, this is not awesome but that seems to be enough for now
        TimerAsio::sptr deleteLater = std::dynamic_pointer_cast<TimerAsio>(shared_from_this());

        TimeDurationType duration;
        bool oneShot;
        {
            ::fwCore::mt::ScopedLock lock(m_mutex);
            oneShot  = m_oneShot;
            duration = m_duration;
        }

        if (!oneShot)
        {
            this->rearmNoLock(duration);
            m_function();
        }
        else
        {
            m_function();
            ::fwCore::mt::ScopedLock lock(m_mutex);
            m_running = false;
        }
    }

}

//------------------------------------------------------------------------------

void TimerAsio::cancelNoLock()
{
    m_timer.cancel();
}

} //namespace fwThread

