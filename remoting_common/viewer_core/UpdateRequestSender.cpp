#include "framework.h"
#include "UpdateRequestSender.h"
// #include aaa_<thread/critical_section_lock.h>
#include "RfbFramebufferUpdateRequestClientMessage.h"

UpdateRequestSender::UpdateRequestSender(lockable* m_fb_lock, ::subsystem::FrameBuffer* m_frame_buffer, ::subsystem::LogWriter* m_log_writer):
	m_wasUpdateRecieved(false),
	m_timeOut(0),
	m_isIncrimental(true),
    m_fbLock(m_fb_lock),
    m_frameBuffer(m_frame_buffer),
    m_plogwriter(m_log_writer),
    m_output(0)
{
	
}

UpdateRequestSender::~UpdateRequestSender()
{
	try
	{
		terminate();
		wait();
	}
	catch (...)
	{
	}
}

void UpdateRequestSender::setWasUpdated()
{
	critical_section_lock al(&m_wasUpdatedLock);
	m_wasUpdateRecieved = true;
}

void UpdateRequestSender::setTimeout(int miliseconds)
{
	critical_section_lock al(&m_timeOutLock);
	m_timeOut = miliseconds;
}

void UpdateRequestSender::setIsIncremental(bool isIncremental)
{
	critical_section_lock al(&m_isIncrimentalLock);
	m_isIncrimental = isIncremental;
}

void UpdateRequestSender::setOutput(RfbOutputGate* output)
{
	{
		critical_section_lock al(&m_outputLock);
		m_output = output;
	}

	// Start thread.
	resume();
	m_plogwriter->debug("UpdateRequestServer is started");
}

void UpdateRequestSender::execute()
{
	try
	{
		while(!isTerminating())
		{
			if (isUpdated())
			{
				sendFbUpdateRequest();
			}

			int timeout = getTimeout();
			timeout = timeout > 0 ? timeout : 500;
			//todo: check if it is best approoach?
			Thread::sleep(timeout);
		}
	}
	catch(const ::exception &ex)
	{
		m_plogwriter->debug("UpdateRequestSender. ::subsystem::Exception: {}", ex.get_message());
	}
	catch(...)
	{
		m_plogwriter->error("UpdateRequestSender. Unknow error has occured.");
	}
}

void UpdateRequestSender::sendFbUpdateRequest()
{
	RfbOutputGate* output = getOutput();

	if(output == 0)
		return;

	::int_rectangle updateRect;
	{
		critical_section_lock al(m_fbLock);
		updateRect = m_frameBuffer->getDimension();
	}

	bool isIncremental = this->isIncremental();

	if (isIncremental)
	{
		m_plogwriter->debug("Sending frame buffer incremental update request [%dx{}]...",
                      updateRect.width(), updateRect.height());
	}
	else
	{
		m_plogwriter->debug("Sending frame buffer full update request [%dx{}]...",
                      updateRect.width(), updateRect.height());
	}

	RfbFramebufferUpdateRequestClientMessage fbUpdReq(isIncremental, updateRect);
	fbUpdReq.send(m_output);
	m_plogwriter->debug("Frame buffer update request is sent");
}

bool UpdateRequestSender::isUpdated()
{
	critical_section_lock al(&m_wasUpdatedLock);
	bool result = m_wasUpdateRecieved;
	m_wasUpdateRecieved = false;
	return result;
}

int UpdateRequestSender::getTimeout()
{
	critical_section_lock al(&m_timeOutLock);
	return m_timeOut;
}

bool UpdateRequestSender::isIncremental()
{
	critical_section_lock al(&m_isIncrimentalLock);
	return m_isIncrimental;
}

RfbOutputGate* UpdateRequestSender::getOutput()
{
	critical_section_lock al(&m_outputLock);
	return m_output;
}