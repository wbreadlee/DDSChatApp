/* Chat_publisher.cxx

A publication of data of type My::Type::Chat::Obj

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C++11 -example <arch> Chat.idl

Example publication of type My::Type::Chat::Obj automatically generated by 
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example subscription.

(2) Start the subscription on the same domain used for RTI Data Distribution
Service with the command
objs/<arch>/Chat_subscriber <domain_id> <sample_count>

(3) Start the publication on the same domain used for RTI Data Distribution
Service with the command
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

#include <iostream>

#include <dds/core/QosProvider.hpp>
#include <dds/pub/ddspub.hpp>
#include <rti/util/util.hpp> // for sleep()

#include "Chat.hpp"

void publisher_main(int domain_id, int sample_count)
{
	// Register types
	rti::domain::register_type<My::Type::Chat::Obj>(My::Topic::Chat::TYPE);

    // Create a DomainParticipant from the named configuration
	dds::domain::DomainParticipant participant =
	        dds::core::QosProvider::Default()->create_participant_from_config(
	        		My::If::PUB);

    // Lookup DataWriter
    dds::pub::DataWriter<My::Type::Chat::Obj> writer =
			rti::pub::find_datawriter_by_name<
				dds::pub::DataWriter<My::Type::Chat::Obj> >(
					participant, My::Topic::Chat::WRITER);

    // Lookup Publisher
    dds::pub::Publisher publisher = writer.publisher();
            /*rti::pub::find_publisher(participant, My::Pub::Default);*/

    // Set the PARTITION QosPolicy on the publisher
    std::vector< std::string > partitions = { "starwars", "startrek", "funny" };
    dds::pub::qos::PublisherQos publisher_qos = publisher.qos();
    publisher_qos << dds::core::policy::Partition(partitions);
    publisher.qos(publisher_qos);


    My::Type::Chat::Obj sample;
    sample.id("Rajive");
    for (int count = 0; count < sample_count || sample_count == 0; count++) {
        // Modify the data to be written here
    	sample.content("Hello from Rajive " + std::to_string(count));

        std::cout << "Writing My::Type::Chat::Obj, count " << count << std::endl;
        writer.write(sample);

        rti::util::sleep(dds::core::Duration(4));
    }
}

int main(int argc, char *argv[])
{

    int domain_id = 0;
    int sample_count = 0; // infinite loop

    if (argc >= 2) {
        domain_id = atoi(argv[1]);
    }
    if (argc >= 3) {
        sample_count = atoi(argv[2]);
    }

    // To turn on additional logging, include <rti/config/Logger.hpp> and
    // uncomment the following line:
    // rti::config::Logger::instance().verbosity(rti::config::Verbosity::STATUS_ALL);

    try {
        publisher_main(domain_id, sample_count);
    } catch (const std::exception& ex) {
        // This will catch DDS exceptions
        std::cerr << "Exception in publisher_main(): " << ex.what() << std::endl;
        return -1;
    }

    // RTI Connext provides a finalize_participant_factory() method
    // if you want to release memory used by the participant factory singleton.
    // Uncomment the following line to release the singleton:
    //
    // dds::domain::DomainParticipant::finalize_participant_factory();

    return 0;
}

