
/* Chat_publisher.cxx

A publication of data of type My::Type::Chat::Obj

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C++ -example <arch> Chat.idl

Example publication of type My::Type::Chat::Obj automatically generated by 
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example subscription.

(2) Start the subscription with the command
objs/<arch>/Chat_subscriber <domain_id> <sample_count>

(3) Start the publication with the command
objs/<arch>/Chat_publisher <domain_id> <sample_count>

(4) [Optional] Specify the list of discovery initial peers and 
multicast receive addresses via an environment variable or a file 
(in the current working directory) called NDDS_DISCOVERY_PEERS. 

You can run any number of publishers and subscribers programs, and can 
add and remove them dynamically from the domain.

Example:

To run the example application on domain <domain_id>:

On Unix: 

objs/<arch>/Chat_publisher <domain_id> o
objs/<arch>/Chat_subscriber <domain_id> 

On Windows:

objs\<arch>\Chat_publisher <domain_id>  
objs\<arch>\Chat_subscriber <domain_id>    

*/

#include <stdio.h>
#include <stdlib.h>
#include "Chat.h"
#include "ChatSupport.h"
#include "ndds/ndds_cpp.h"

#include "ndds/ndds_namespace_cpp.h"

using namespace DDS;

/* Delete all entities */
static int publisher_shutdown(
    DomainParticipant *participant)
{
    ReturnCode_t retcode;
    int status = 0;

    if (participant != NULL) {
        retcode = participant->delete_contained_entities();
        if (retcode != RETCODE_OK) {
            printf("delete_contained_entities error %d\n", retcode);
            status = -1;
        }

        retcode = TheParticipantFactory->delete_participant(participant);
        if (retcode != RETCODE_OK) {
            printf("delete_participant error %d\n", retcode);
            status = -1;
        }
    }

    /* RTI Connext provides finalize_instance() method on
    domain participant factory for people who want to release memory used
    by the participant factory. Uncomment the following block of code for
    clean destruction of the singleton. */
    retcode = DomainParticipantFactory::finalize_instance();
    if (retcode != RETCODE_OK) {
        printf("finalize_instance error %d\n", retcode);
        status = -1;
    }

    return status;
}

extern "C" int publisher_main(int domainId, int sample_count)
{
    DomainParticipant *participant = NULL;
    Publisher *publisher = NULL;
    Topic *topic = NULL;
    DataWriter *writer = NULL;
    My::Type::Chat::ObjDataWriter * ChatObject_writer = NULL;
    My::Type::Chat::Obj *instance = NULL;
    ReturnCode_t retcode;
    InstanceHandle_t instance_handle = HANDLE_NIL;
    const char *type_name = NULL;
    int count = 0;  
    Duration_t send_period = {4,0};

    /* To customize participant QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    participant = TheParticipantFactory->create_participant(
        domainId, PARTICIPANT_QOS_DEFAULT, 
        NULL /* listener */, STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize publisher QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    publisher = participant->create_publisher(
        PUBLISHER_QOS_DEFAULT, NULL /* listener */, STATUS_MASK_NONE);
    if (publisher == NULL) {
        printf("create_publisher error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Register type before creating topic */
    type_name = My::Topic::Chat::TYPE.c_str();
    retcode = My::Type::Chat::ObjTypeSupport::register_type(
        participant, type_name);
    if (retcode != RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize topic QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
        My::Topic::Chat::TOPIC.c_str(), /*>>><<<*/
        type_name, TOPIC_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* To customize data writer QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    writer = publisher->create_datawriter(
        topic, DATAWRITER_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (writer == NULL) {
        printf("create_datawriter error\n");
        publisher_shutdown(participant);
        return -1;
    }
    ChatObject_writer = My::Type::Chat::ObjDataWriter::narrow(writer);
    if (ChatObject_writer == NULL) {
        printf("DataWriter narrow error\n");
        publisher_shutdown(participant);
        return -1;
    }

    /* Create data sample for writing */
    instance = new My::Type::Chat::Obj();
    if (instance == NULL) {
        printf("new My::Type::Chat::Obj() error\n");
       	publisher_shutdown(participant);
        return -1;
    }

    /* For a data type that has a key, if the same instance is going to be
    written multiple times, initialize the key here
    and register the keyed instance prior to writing */
    /*
    instance_handle = ChatObject_writer->register_instance(*instance);
    */

    /* Main loop */
    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        printf("Writing My::Type::Chat::Obj, count %d\n", count);

        /* Modify the data to be sent here */
        /* >>> */
        instance->content = "Hello from Rajive " + std::to_string(count);
        /* <<< */

        retcode = ChatObject_writer->write(*instance, instance_handle);
        if (retcode != RETCODE_OK) {
            printf("write error %d\n", retcode);
        }

        NDDSUtility::sleep(send_period);
    }

    /*
    retcode = ChatObject_writer->unregister_instance(
        *instance, instance_handle);
    if (retcode != RETCODE_OK) {
        printf("unregister instance error %d\n", retcode);
    }
    */

    /* Delete data sample */
  	delete instance;

    /* Delete all entities */
    return publisher_shutdown(participant);
}

int main(int argc, char *argv[])
{
    int domainId = 0;
    int sample_count = 0; /* infinite loop */

    if (argc >= 2) {
        domainId = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    /* Uncomment this to turn on additional logging
    NDDSConfigLogger::get_instance()->
    set_verbosity_by_category(NDDS_CONFIG_LOG_CATEGORY_API, 
    NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
    */

    return publisher_main(domainId, sample_count);
}

