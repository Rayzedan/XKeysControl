#include "OpcUaClient.h"
#include "SetupDevice.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>

struct signalNode
{
	int indexButton;
	UA_NodeId node;
	bool isSignalGood;
};
std::map<int, std::pair<std::string, int>> signalMap{};
std::map<int, signalNode> subcribeMap{};
std::vector<std::string> config{};
DWORD timeout = 30;
SetupDevice* device = new SetupDevice();



OpcUaClient::OpcUaClient()
{
	running = false;
}

OpcUaClient::~OpcUaClient()
{
}

void OpcUaClient::handlerNodeChanged(UA_Client* client, UA_UInt32 subId, void* subContext, UA_UInt32 monId, void* monContext, UA_DataValue* value)
{
	if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
		unsigned int status = value->status;
		int key = subId;
		int index = subcribeMap[key].indexButton;
		if (status != 2150760448 || status == 0)
			subcribeMap[key].isSignalGood = true;
		else
			subcribeMap[key].isSignalGood = false;

		bool isEnable = subcribeMap[key].isSignalGood;
		UA_Boolean error = *(UA_Boolean*)value->value.data;
		if (error)
			device->setLED(index, 2, isEnable);
		else
			device->setLED(index, 1, isEnable);

	}
}

void OpcUaClient::deleteSubscriptionCallback(UA_Client* client, UA_UInt32 subscriptionId, void* subscriptionContext)
{
	/*UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
		"Subscription Id %u was deleted", subscriptionId);*/
}

void OpcUaClient::subscriptionInactivityCallback(UA_Client* client, UA_UInt32 subId, void* subContext)
{
	//UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Inactivity for subscription %u", subId);
}

void OpcUaClient::stateCallback(UA_Client* client, UA_SecureChannelState channelState,
	UA_SessionState sessionState, UA_StatusCode recoveryStatus)
{
	switch (channelState) {
	case UA_SECURECHANNELSTATE_CLOSED:		
		device->setAllRed();
		break;
	default:
		break;
	}
	switch (sessionState) {
	case UA_SESSIONSTATE_ACTIVATED: {
		device->setAllBlue();
		/* A new session was created. We need to create the subscription. */
		/* Create a subscription */
		for (const auto& item : signalMap) {
			/* Add a MonitoredItem */
			if (item.second.second) {
				UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
				UA_CreateSubscriptionResponse response =
					UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);
				if (response.responseHeader.serviceResult != UA_STATUSCODE_GOOD)
					return;

				UA_NodeId currentNode =
					UA_NODEID_STRING(1, _strdup(item.second.first.c_str()));

				UA_MonitoredItemCreateRequest monRequest =
					UA_MonitoredItemCreateRequest_default(currentNode);
				UA_MonitoredItemCreateResult monResponse =
					UA_Client_MonitoredItems_createDataChange(client, response.subscriptionId,
						UA_TIMESTAMPSTORETURN_BOTH, monRequest,
						NULL, handlerNodeChanged, NULL);
				if (monResponse.statusCode == UA_STATUSCODE_GOOD) {
					UA_Variant status;
					UA_Variant_init(&status);
					UA_StatusCode retval;
					retval = UA_Client_readValueAttribute(client, currentNode, &status);
					int key = response.subscriptionId;
					int value = item.first;
					bool isEnable = UA_StatusCode_isGood(retval);
					subcribeMap[key].indexButton = value;
					subcribeMap[key].isSignalGood = isEnable;
					subcribeMap[key].node = currentNode;
				}
			}
		}
	}
								  break;
	case UA_SESSIONSTATE_CLOSED:
		device->setAllRed();
		break;
	default:
		break;
	}
}

void OpcUaClient::initialRequest()
{
	m_file = new ParseXml();
	if (m_file->getConfigFile() != -1) {
		running = true;
		m_file->getSignalMap(signalMap);
		m_file->getConfigList(config);
		timeout = atoi(config[2].c_str());
		device->setTimeoutDevice(timeout);
		device->installDevice();
		m_requestClientTime = atoi(config[1].c_str());
		m_client = UA_Client_new();
		UA_ClientConfig* cc = UA_Client_getConfig(m_client);
		UA_ClientConfig_setDefault(cc);
		/* Set stateCallback */
		cc->stateCallback = stateCallback;
		cc->subscriptionInactivityCallback = subscriptionInactivityCallback;
		if (!infiniteRequest())
			m_threadState = 2;
	}
	else {
		m_threadState = -1;
	}
	/* Clean up */
	/* Disconnects the client internally */
	UA_Client_disconnect(m_client);
	m_threadState = 1;
	running = false;
}

int OpcUaClient::getCurrentState()
{
	return m_threadState;
}

void OpcUaClient::stopSession()
{
	running = false;
	subcribeMap.clear();
	signalMap.clear();
	config.clear();
	device->setAllRed();
	if (m_client != nullptr)
		UA_Client_delete(m_client);
	if (device != nullptr)
		delete device;
	if (m_file != nullptr)
		delete m_file;
}

bool OpcUaClient::infiniteRequest()
{
	/* Endless loop runAsync */
	while (!device->getCurrentState()) {
		running = false;
	}
	if (device->getCurrentState()) {
		running = true;
	}

	while (running) {
		/* if already connected, this will return GOOD and do nothing */
		/* if the connection is closed/errored, the connection will be reset and then reconnected */
		/* Alternatively you can also use UA_Client_getState to get the current state */
		bool deviceIndicator = device->getCurrentState();
		UA_StatusCode retval = UA_STATUSCODE_BAD;
		if (deviceIndicator)
			retval = UA_Client_connect(m_client, config[0].c_str());
		else {
			if (retval == UA_STATUSCODE_GOOD)
				UA_Client_disconnect(m_client);
			continue;
		}

		if (retval != UA_STATUSCODE_GOOD) {
			UA_sleep_ms(m_requestClientTime * 1000);
			continue;
		}
		if (deviceIndicator && retval == UA_STATUSCODE_GOOD)
			UA_Client_run_iterate(m_client, m_requestClientTime * 1000);
	}
	m_threadState = 0;
	return false;
}
