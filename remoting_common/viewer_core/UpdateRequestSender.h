// #ifndef _UPDATE_REQUEST_SENDER_
// #define _UPDATE_REQUEST_SENDER_


#pragma once

#include "remoting/remoting_common/thread/Thread.h"
#include "remoting/remoting_common/rfb/FrameBuffer.h"
//// #include aaa_<log_writer/LogWriter.h>
#include "remoting/remoting_common/network/RfbOutputGate.h"

class CLASS_DECL_REMOTING_COMMON UpdateRequestSender : public task
{
public:
	UpdateRequestSender(lockable* m_fb_lock, FrameBuffer* m_frame_buffer, LogWriter* m_log_writer);

	~UpdateRequestSender();

	void setWasUpdated();
	void setTimeout(int miliseconds);
	void setIsIncremental(bool isIncremental);
	void setOutput(RfbOutputGate* output);

	int getTimeout();

protected:
	virtual void execute() override;


private:
	void sendFbUpdateRequest();

	bool isUpdated();
	bool isIncremental();
	RfbOutputGate* getOutput();

	bool m_wasUpdateRecieved;
	critical_section m_wasUpdatedLock;

	int m_timeOut;
	critical_section m_timeOutLock;

	bool m_isIncrimental;
	critical_section m_isIncrimentalLock;

	lockable *m_fbLock;
	FrameBuffer *m_frameBuffer;

	LogWriter *m_plogwriter;
	
	RfbOutputGate *m_output;
	critical_section m_outputLock;
};

//_UPDATE_REQUEST_SENDER_

