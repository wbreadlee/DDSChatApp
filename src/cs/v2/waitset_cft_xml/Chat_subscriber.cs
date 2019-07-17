
/* ChatObject_subscriber.cs

A subscription example

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C# -example <arch> Chat.idl

Example subscription of type ChatObject automatically generated by 
'rtiddsgen'. To test them, follow these steps:

(1) Compile this file and the example publication.

(2) Start the subscription with the command
objs\<arch>${constructMap.nativeFQNameInModule}_subscriber <domain_id> <sample_count>

(3) Start the publication with the command
objs\<arch>${constructMap.nativeFQNameInModule}_publisher <domain_id> <sample_count>

(4) [Optional] Specify the list of discovery initial peers and 
multicast receive addresses via an environment variable or a file 
(in the current working directory) called NDDS_DISCOVERY_PEERS. 

You can run any number of publishers and subscribers programs, and can 
add and remove them dynamically from the domain.

Example:

To run the example application on domain <domain_id>:

bin\<Debug|Release>\ChatObject_publisher <domain_id> <sample_count>  
bin\<Debug|Release>\ChatObject_subscriber <domain_id> <sample_count>
*/

using System;
using System.Collections.Generic;
using System.Text;

namespace My{

    public class ChatObjectSubscriber {

        public class ChatObjectListener : DDS.DataReaderListener {

            public override void on_requested_deadline_missed(
                DDS.DataReader reader,
                ref DDS.RequestedDeadlineMissedStatus status) {}

            public override void on_requested_incompatible_qos(
                DDS.DataReader reader,
                DDS.RequestedIncompatibleQosStatus status) {}

            public override void on_sample_rejected(
                DDS.DataReader reader,
                ref DDS.SampleRejectedStatus status) {}

            public override void on_liveliness_changed(
                DDS.DataReader reader,
                ref DDS.LivelinessChangedStatus status) {}

            public override void on_sample_lost(
                DDS.DataReader reader,
                ref DDS.SampleLostStatus status) {}

            public override void on_subscription_matched(
                DDS.DataReader reader,
                ref DDS.SubscriptionMatchedStatus status) {}

            public override void on_data_available(DDS.DataReader reader) {
                ChatObjectDataReader ChatObject_reader =
                (ChatObjectDataReader)reader;

                try {
                    ChatObject_reader.read( /*>>><<<*/
                        data_seq,
                        info_seq,
                        DDS.ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
                        DDS.SampleStateKind.ANY_SAMPLE_STATE,
                        DDS.ViewStateKind.ANY_VIEW_STATE,
                        DDS.InstanceStateKind.ANY_INSTANCE_STATE);
                }
                catch(DDS.Retcode_NoData) {
                    return;
                }
                catch(DDS.Exception e) {
                    Console.WriteLine("take/read error {0}", e); /*>>><<<*/
                    return;
                }

                System.Int32 data_length = data_seq.length;
                for (int i = 0; i < data_length; ++i) {
                    if (info_seq.get_at(i).valid_data) {
                        ChatObjectTypeSupport.print_data(data_seq.get_at(i));
                    }
                }

                try {
                    ChatObject_reader.return_loan(data_seq, info_seq);
                }
                catch(DDS.Exception e) {
                    Console.WriteLine("return loan error {0}", e);
                }
            }

            public ChatObjectListener() {
                data_seq = new ChatObjectSeq();
                info_seq = new DDS.SampleInfoSeq();
            }

            private ChatObjectSeq data_seq;
            private DDS.SampleInfoSeq info_seq;
        };

        public static void Main(string[] args) {

            // --- Get domain ID --- //
            int domain_id = 0;
            if (args.Length >= 1) {
                domain_id = Int32.Parse(args[0]);
            }

            // --- Get max loop count; 0 means infinite loop  --- //
            int sample_count = 0;
            if (args.Length >= 2) {
                sample_count = Int32.Parse(args[1]);
            }

            /* Uncomment this to turn on additional logging
            NDDS.ConfigLogger.get_instance().set_verbosity_by_category(
                NDDS.LogCategory.NDDS_CONFIG_LOG_CATEGORY_API, 
                NDDS.LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
            */

            // --- Run --- //
            try {
                ChatObjectSubscriber.subscribe(
                    domain_id, sample_count);
            }
            catch(DDS.Exception) {
                Console.WriteLine("error in subscriber");
            }
        }

        static void subscribe(int domain_id, int sample_count) {

            // --- Register userGenerated datatype ---
            DDS.DomainParticipantFactory.get_instance().
                    register_type_support(
                    ChatObjectTypeSupport.get_instance(),
                    "My::Type::Chat::Obj");

            // --- Create participant --- //

            /* To customize the participant QoS, use 
            the configuration file USER_QOS_PROFILES.xml */
            DDS.DomainParticipant participant = 
                DDS.DomainParticipantFactory.get_instance().
                        create_participant_from_config(
                              "MyParticipant_Library::MySubscriptionParticipant");
            if (participant == null) {
                shutdown(participant);
                throw new ApplicationException("create_participant error");
            }

            /* Create a data reader listener */
            ChatObjectListener reader_listener =
            new ChatObjectListener();

            // --- Lookup reader --- //

            /* To customize the data reader QoS, use 
            the configuration file USER_QOS_PROFILES.xml */
            DDS.DataReader reader = participant.lookup_datareader_by_name(
                                             "MySubscriber::ChatObjectReader");
            if (reader == null) {
                shutdown(participant);
                reader_listener = null;
                throw new ApplicationException("lookup_datareader error");
            }

            /* >>> Setup StatusCondition */
            DDS.StatusCondition status_condition = reader.get_statuscondition();
            if (status_condition.Equals(null))
            {
                Console.WriteLine("get_statuscondition error\n");
                shutdown(participant);
                throw new ApplicationException("get_statuscondition error");
            }
            try {
                status_condition.set_enabled_statuses((DDS.StatusMask)DDS.StatusKind.DATA_AVAILABLE_STATUS);
            }
            catch {
                Console.WriteLine("set_enabled_statuses error\n");
                shutdown(participant);
                throw new ApplicationException("set_enabled_statuses error");
            }
            /* <<< */

            /* >>> Setup WaitSet */
            DDS.WaitSet waitset = new DDS.WaitSet();
            try {
                waitset.attach_condition(status_condition);
            }
            catch {
                // ... error
                waitset.Dispose(); waitset = null;
                shutdown(participant);
                reader_listener.Dispose(); reader_listener = null;
                return;
            }

            // holder for active conditions
            DDS.ConditionSeq active_conditions = new DDS.ConditionSeq();
            /* <<< */

            // --- Wait for data --- //

            /* Main loop */
            const System.Int32 receive_period = 4000; // milliseconds
            for (int count=0;
            (sample_count == 0) || (count < sample_count);
            ++count) {
                Console.WriteLine(
                    "ChatObject subscriber sleeping ...",
                receive_period / 1000);


                /* >>> Wait for condition to trigger */
                try {
                    waitset.wait(active_conditions, DDS.Duration_t.DURATION_INFINITE);
                    reader_listener.on_data_available(reader);
                }
                catch {
                }
                /* <<< */

                // System.Threading.Thread.Sleep(receive_period); /*>>><<<*/
            }

            // --- Shutdown --- //

            /* Delete all entities */
            waitset.Dispose(); waitset = null; /*>>><<<*/
            shutdown(participant);
            reader_listener = null;
        }

        static void shutdown(
            DDS.DomainParticipant participant) {

            /* Delete all entities */

            if (participant != null) {
                participant.delete_contained_entities();
                DDS.DomainParticipantFactory.get_instance().delete_participant(
                    ref participant);
            }

            /* RTI Connext provides finalize_instance() method on
            domain participant factory for users who want to release memory
            used by the participant factory. Uncomment the following block of
            code for clean destruction of the singleton. */
            /*
            try {
                DDS.DomainParticipantFactory.finalize_instance();
            }
            catch(DDS.Exception e) {
                Console.WriteLine("finalize_instance error {0}", e);
                throw e;
            }
            */
        }
    }

}
