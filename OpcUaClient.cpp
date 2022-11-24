#include "OpcUaClient.h"
#include "ParseXml.h"
#include "SetupDevice.h"
#include <iostream>
UA_Boolean running;
SetupDevice* device = new SetupDevice();
ParseXml* file = new ParseXml();
std::map<int, std::pair<std::string, int>> signalMap;
std::map<int, int> subcribeMap;

OpcUaClient::OpcUaClient() 
{
    running = true;
}

OpcUaClient::~OpcUaClient() 
{
    std::cout << "DELETE OPC UA CLIENT\n";
    stopSession();
    delete device;
    delete file;
}

void OpcUaClient::handlerNodeChanged(UA_Client* client, UA_UInt32 subId, void* subContext, UA_UInt32 monId, void* monContext, UA_DataValue* value) 
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "current node changed");
    //UA_sleep_ms(1000);   
    if (UA_Variant_hasScalarType(&value->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
        UA_Boolean error = *(UA_Boolean*)value->value.data;
        int temp = subId;
        //std::cout << "MON ID - " << temp << std::endl;
        std::cout << "SUB ID - " << temp << std::endl;
        int index = subcribeMap[temp];
        for (const auto& item : subcribeMap) {
            std::cout << "KEY - " << item.first << " VALUE - " << item.second << std::endl;
        }
        if (error) {
            std::cout << "index - " << index << std::endl;
            device->setLED(index, 2);
        }
        else {
            std::cout << "index - " << index << std::endl;
            device->setLED(index, 1);
        }   
       /* printf("---%-40s%-8i\n",
            "Reading the value of node (1, \"Object.error\"):", error);*/     
    }
}

void OpcUaClient::deleteSubscriptionCallback(UA_Client* client, UA_UInt32 subscriptionId, void* subscriptionContext) 
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
        "Subscription Id %u was deleted", subscriptionId);
}

void OpcUaClient::subscriptionInactivityCallback(UA_Client* client, UA_UInt32 subId, void* subContext) 
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Inactivity for subscription %u", subId);
}

void OpcUaClient::stateCallback(UA_Client* client, UA_SecureChannelState channelState,
    UA_SessionState sessionState, UA_StatusCode recoveryStatus) 
{
    switch (channelState) {
    case UA_SECURECHANNELSTATE_FRESH:
    case UA_SECURECHANNELSTATE_CLOSED:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "The client is disconnected");
        break;
    case UA_SECURECHANNELSTATE_HEL_SENT:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Waiting for ack");
        break;
    case UA_SECURECHANNELSTATE_OPN_SENT:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Waiting for OPN Response");
        break;
    case UA_SECURECHANNELSTATE_OPEN:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A SecureChannel to the server is open");
        break;
    default:
        break;
    }

    switch (sessionState) {
    case UA_SESSIONSTATE_ACTIVATED: {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "A session with the server is activated");
        /* A new session was created. We need to create the subscription. */
        /* Create a subscription */
        for (const auto& item : signalMap) {
            std::cout <<  "item second - " << item.second.second << std::endl;
            /* Add a MonitoredItem */
            if (item.second.second) {
                UA_CreateSubscriptionRequest request = UA_CreateSubscriptionRequest_default();
                UA_CreateSubscriptionResponse response =
                    UA_Client_Subscriptions_create(client, request, NULL, NULL, deleteSubscriptionCallback);
                if (response.responseHeader.serviceResult == UA_STATUSCODE_GOOD) {
                    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Create subscription succeeded, id %u",
                        response.subscriptionId);
                }
                else
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
                    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                        "Monitoring, id %u",
                        monResponse.monitoredItemId);
                    int key = response.subscriptionId;
                    int value = item.first;
                    subcribeMap[key] = value;
                    std::cout << "KEY - " << key << " INDEX BUTTON - " << value << std::endl;
                }
            }                       
        }
    }
        break;
    case UA_SESSIONSTATE_CLOSED:
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Session disconnected");
        break;
    default:
        break;
    }
}

void OpcUaClient::readValueAttributeCallback(UA_Client* client, void* userdata,
    UA_UInt32 requestId, UA_StatusCode status,
    UA_DataValue* var) 
{
    UA_sleep_ms(1000);
    if (UA_Variant_hasScalarType(&var->value, &UA_TYPES[UA_TYPES_BOOLEAN])) {
        UA_Boolean error = *(UA_Boolean*)var->value.data;
        if (error)
            device->setLED(0, 2);
        else
            device->setLED(0, 1);
        printf("---%-40s%-8i\n",
            "Reading the value of node (1, \"Object.error\"):", error);
    }
}

void OpcUaClient::subLoop(std::string address) 
{
    file->getSignalMap(signalMap);
    UA_Boolean value = 0;
    UA_UInt32 reqId = 0;
    UA_Client* client = UA_Client_new();
    UA_ClientConfig* cc = UA_Client_getConfig(client);
    UA_ClientConfig_setDefault(cc);
    /* Set stateCallback */
    cc->stateCallback = stateCallback;
    cc->subscriptionInactivityCallback = subscriptionInactivityCallback;
    /* Endless loop runAsync */
    while (running) {
        /* if already connected, this will return GOOD and do nothing */
        /* if the connection is closed/errored, the connection will be reset and then reconnected */
        /* Alternatively you can also use UA_Client_getState to get the current state */
        UA_StatusCode retval = UA_Client_connect(client, address.c_str());
        if (retval != UA_STATUSCODE_GOOD) {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                "Not connected. Retrying to connect in 5 second");           
            /* The connect may timeout after 1 second (see above) or it may fail immediately on network errors */
            /* E.g. name resolution errors or unreachable network. Thus there should be a small sleep here */
            UA_sleep_ms(5000);
            continue;
        }
       //UA_Client_readValueAttribute_async(client, UA_NODEID_STRING(1, _strdup("Object.error")), readValueAttributeCallback,NULL, &reqId);
   

        UA_Client_run_iterate(client, 5000);
    };

    /* Clean up */
    UA_Client_delete(client); /* Disconnects the client internally */
}

void OpcUaClient::stopSession()
{
    running = false;
    delete device;
    delete file;
}