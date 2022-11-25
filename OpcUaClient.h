#ifndef OpcUaClientH
#define OpcUaClientH
#include <open62541/client_highlevel_async.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_config_default.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include <string>
#include <stdlib.h>
#pragma comment(lib, "Ws2_32.lib")

class OpcUaClient
{
public:
	OpcUaClient();
	virtual ~OpcUaClient();
<<<<<<< HEAD
	void subLoop();
	void stopSession();
private:
	UA_Client* client;
=======
	void subLoop(std::string address);
	void stopSession();
private:
>>>>>>> 4df2694 (init project)
	static void handlerNodeChanged(UA_Client* client, UA_UInt32 subId, void* subContext,
		UA_UInt32 monId, void* monContext, 
		UA_DataValue* value);
	static void deleteSubscriptionCallback(UA_Client* client, UA_UInt32 subscriptionId, void* subscriptionContext);
	static void subscriptionInactivityCallback(UA_Client* client, UA_UInt32 subId, void* subContext);
	static void stateCallback(UA_Client* client, UA_SecureChannelState channelState,
		UA_SessionState sessionState, UA_StatusCode recoveryStatus);
<<<<<<< HEAD
=======
	static void readValueAttributeCallback(UA_Client* client, void* userdata,
		UA_UInt32 requestId, UA_StatusCode status, 
		UA_DataValue* var);
>>>>>>> 4df2694 (init project)

};
#endif
