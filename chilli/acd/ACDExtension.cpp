#include "ACDExtension.h"
#include <log4cplus/loggingmacros.h>
#include <json/json.h>

namespace chilli{
namespace ACD{


ACDExtension::ACDExtension(const std::string &ext, const std::string &smFileName, fsm::SMInstance * smInstance) :Extension(), 
SendInterface("this"), m_ExtNumber(ext), m_smInstance(smInstance), m_SM(nullptr)
{
	log = log4cplus::Logger::getInstance(m_ExtNumber);
	m_SM = new fsm::StateMachine(smFileName, log);
	LOG4CPLUS_DEBUG(log,"new a ACD extension object.");
}
ACDExtension::~ACDExtension(){
	delete m_SM;
	LOG4CPLUS_DEBUG(log,"destruction a ACD extension object.");
}


const std::string & ACDExtension::getExtensionNumber() const
{
	return m_ExtNumber;
}

bool ACDExtension::isIdle()
{
	return m_SM->getCurrentStateID() == "Idle";
}

void ACDExtension::setSessionId(const std::string & sessinId)
{
	this->m_SessionId = sessinId;
}

const std::string & ACDExtension::getSessionId()
{
	return this->m_SessionId;
}

void ACDExtension::go()
{
	m_SM->setSessionID(m_SessionId);
	m_SM->setscInstance(m_smInstance);
	m_SM->addSendImplement(this);
	m_SM->go();
}

void ACDExtension::run()
{
	m_SM->mainEventLoop();
}

void ACDExtension::termination()
{
	m_SM->termination();
}

int ACDExtension::Answer()
{
	LOG4CPLUS_WARN(log, "not implement.");
	return 0;
}

int ACDExtension::PlayFile(const std::string &fileName)
{
	LOG4CPLUS_WARN(log, "not implement.");
	return 0;
}

int ACDExtension::HangUp()
{
	LOG4CPLUS_WARN(log, "not implement.");
	return 0;
}

void ACDExtension::fireSend(const std::string & strContent,const void * param)
{
	LOG4CPLUS_TRACE(log," recive a Send event from stateMachine:" << strContent);
}

int ACDExtension::pushEvent(const std::string& strEvent)
{
	Json::Value jsonEvent;
	Json::Reader jsonReader;
	if (jsonReader.parse(strEvent, jsonEvent)){
		std::string eventName;
		std::string _from;
		if (jsonEvent["event"].isString()){
			eventName = jsonEvent["event"].asString();
		}

		if (jsonEvent["from"].isString()){
			_from = jsonEvent["from"].asString();
		}


		fsm::TriggerEvent evt(eventName, _from);
		LOG4CPLUS_INFO(log, " Recived a event," << "from " << _from << "event=" << evt.ToString());
		m_SM->pushEvent(evt);

	}
	else{
		LOG4CPLUS_ERROR(log, __FUNCTION__ ",event:" << strEvent << " not json data.");
	}

	return 0;
}

}
}