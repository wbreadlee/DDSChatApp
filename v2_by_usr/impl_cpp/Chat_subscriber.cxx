
/* Chat_subscriber.cxx

A subscription example

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C++ -example <arch> Chat.idl

Example subscription of type My::ChatObject automatically generated by 
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example publication.

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

objs/<arch>/Chat_publisher <domain_id> 
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

namespace My {

    class ChatObjectListener : public DataReaderListener {

      public:

        virtual void on_requested_deadline_missed(
            DataReader* /*reader*/,
            const RequestedDeadlineMissedStatus& /*status*/) {}

        virtual void on_requested_incompatible_qos(
            DataReader* /*reader*/,
            const RequestedIncompatibleQosStatus& /*status*/) {}

        virtual void on_sample_rejected(
            DataReader* /*reader*/,
            const SampleRejectedStatus& /*status*/) {}

        virtual void on_liveliness_changed(
            DataReader* /*reader*/,
            const LivelinessChangedStatus& /*status*/) {}

        virtual void on_sample_lost(
            DataReader* /*reader*/,
            const SampleLostStatus& /*status*/) {}

        virtual void on_subscription_matched(
            DataReader* /*reader*/,
            const SubscriptionMatchedStatus& /*status*/) {}

        virtual void on_data_available(DataReader* reader);
    };

    void ChatObjectListener::on_data_available(DataReader* reader)
    {
        My::ChatObjectDataReader *ChatObject_reader = NULL;
        My::ChatObjectSeq data_seq;
        SampleInfoSeq info_seq;
        ReturnCode_t retcode;
        int i;

        ChatObject_reader = My::ChatObjectDataReader::narrow(reader);
        if (ChatObject_reader == NULL) {
            printf("DataReader narrow error\n");
            return;
        }

        retcode = ChatObject_reader->take(
            data_seq, info_seq, LENGTH_UNLIMITED,
            ANY_SAMPLE_STATE, ANY_VIEW_STATE, ANY_INSTANCE_STATE);

        if (retcode == RETCODE_NO_DATA) {
            return;
        } else if (retcode != RETCODE_OK) {
            printf("take error %d\n", retcode);
            return;
        }

        for (i = 0; i < data_seq.length(); ++i) {
            if (info_seq[i].valid_data) {
                printf("Received data\n");
                My::ChatObjectTypeSupport::print_data(&data_seq[i]);
            }
        }

        retcode = ChatObject_reader->return_loan(data_seq, info_seq);
        if (retcode != RETCODE_OK) {
            printf("return loan error %d\n", retcode);
        }
    }

}

/* Delete all entities */
static int subscriber_shutdown(
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

    /* RTI Connext provides the finalize_instance() method on
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

extern "C" int subscriber_main(int domainId, int sample_count)
{
    DomainParticipant *participant = NULL;
    Subscriber *subscriber = NULL;
    Topic *topic = NULL;
    My::ChatObjectListener *reader_listener = NULL; 
    DataReader *reader = NULL;
    ReturnCode_t retcode;
    const char *type_name = NULL;
    int count = 0;
    Duration_t receive_period = {4,0};
    int status = 0;

    /* To customize the participant QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    participant = TheParticipantFactory->create_participant(
        domainId, PARTICIPANT_QOS_DEFAULT, 
        NULL /* listener */, STATUS_MASK_NONE);
    if (participant == NULL) {
        printf("create_participant error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the subscriber QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    subscriber = participant->create_subscriber(
        SUBSCRIBER_QOS_DEFAULT, NULL /* listener */, STATUS_MASK_NONE);
    if (subscriber == NULL) {
        printf("create_subscriber error\n");
        subscriber_shutdown(participant);
        return -1;
    }

    /* Register the type before creating the topic */
    type_name = My::ChatObjectTypeSupport::get_type_name();
    retcode = My::ChatObjectTypeSupport::register_type(
        participant, type_name);
    if (retcode != RETCODE_OK) {
        printf("register_type error %d\n", retcode);
        subscriber_shutdown(participant);
        return -1;
    }

    /* To customize the topic QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    topic = participant->create_topic(
        My::CHAT_TOPIC_NAME, /*>>><<<*/
        type_name, TOPIC_QOS_DEFAULT, NULL /* listener */,
        STATUS_MASK_NONE);
    if (topic == NULL) {
        printf("create_topic error\n");
        subscriber_shutdown(participant);
        return -1;
    }


    /* Create a data reader listener */
    reader_listener = new My::ChatObjectListener();

    /* To customize the data reader QoS, use 
    the configuration file USER_QOS_PROFILES.xml */
    reader = subscriber->create_datareader(
    	topic, DATAREADER_QOS_DEFAULT, NULL /*>>><<<*/,
        STATUS_MASK_NONE /*>>><<<*/);
    if (reader == NULL) {
        printf("create_datareader error\n");
        subscriber_shutdown(participant);
        delete reader_listener;
        return -1;
    }

    /* >>> Setup StatusCondition */
    StatusCondition* status_condition = reader->get_statuscondition();
    if (status_condition == NULL) {
        printf("get_statuscondition error\n");
        subscriber_shutdown(participant);
        return -1;
    }
    retcode = status_condition->set_enabled_statuses(DDS_DATA_AVAILABLE_STATUS);
    if (retcode != DDS_RETCODE_OK) {
        printf("set_enabled_statuses error\n");
        subscriber_shutdown(participant);
        return -1;
    }
    /* <<< */

    /* >>> Setup WaitSet */
    WaitSet* waitset = new WaitSet();
    retcode = waitset->attach_condition(status_condition);
    if (retcode != DDS_RETCODE_OK) {
        // ... error
        delete waitset;
        subscriber_shutdown(participant);
        return -1;
    }
    ConditionSeq active_conditions; // holder for active conditions
    /* <<< */


    /* Main loop */

    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {

        printf("My::ChatObject subscriber waiting for %d sec...\n",
        receive_period.sec);

        /* >>> Wait */
        retcode = waitset->wait(active_conditions, receive_period);
        if (retcode == DDS_RETCODE_OK) {
        	reader_listener->on_data_available(reader);
        }
        /* <<< */
    }

    /* Delete all entities */
    delete waitset; /*>>><<<*/
    status = subscriber_shutdown(participant);
    delete reader_listener;

    return status;
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

    return subscriber_main(domainId, sample_count);
}

