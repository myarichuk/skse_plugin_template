#include "../include/event_aggregator.hpp"
#include <string>
#include <catch.hpp>

struct TestEvent {
    explicit TestEvent(const int val) : val(val) {}
    int val;
};

class AnotherTestEvent {
public:
    explicit AnotherTestEvent(std::string val) : val(std::move(val)) {}
    std::string val;
};

TEST_CASE("EventAggregator Tests", "[event]") {
    event_aggregator& ea = event_aggregator::Instance();

    SECTION("Multiple subscribers, one disconnects") {
        int received1 = 0;
        int received2 = 0;
        auto connection1 = ea.Subscribe<TestEvent>([&received1](const TestEvent event) {
            received1 = event.val;
        });
        auto connection2 = ea.Subscribe<TestEvent>([&received2](const TestEvent event) {
            received2 = event.val;
        });

        ea.Publish(TestEvent(100));
        REQUIRE(received1 == 100);
        REQUIRE(received2 == 100);

        // Disconnect first subscriber
        connection1.disconnect();

        ea.Publish(TestEvent(200));
        REQUIRE(received1 == 100);  // Shouldn't change, as it's no longer subscribed
        REQUIRE(received2 == 200);
    }

    SECTION("Test Publish without Subscriber") {
        REQUIRE_NOTHROW(ea.Publish(TestEvent(5)));  // There's no subscriber for this event yet.
    }

    SECTION("Test Subscribe and Publish") {
        int received = 0;
        ea.Subscribe<TestEvent>([&received](const TestEvent event) {
            received = event.val;
        });

        ea.Publish(TestEvent(42));
        REQUIRE(received == 42);
    }

    SECTION("Multiple subscribers for same event") {
        int received1 = 0;
        int received2 = 0;
        ea.Subscribe<TestEvent>([&received1](const TestEvent event) {
            received1 = event.val;
        });
        ea.Subscribe<TestEvent>([&received2](const TestEvent event) {
            received2 = event.val;
        });

        ea.Publish(TestEvent(77));
        REQUIRE(received1 == 77);
        REQUIRE(received2 == 77);
    }

    SECTION("Different events have different subscribers") {
        int intReceived = 0;
        std::string stringReceived;

        ea.Subscribe<TestEvent>([&intReceived](const TestEvent event) {
            intReceived = event.val;
        });
        ea.Subscribe<AnotherTestEvent>([&stringReceived](const AnotherTestEvent& event) {
            stringReceived = event.val;
        });

        ea.Publish(TestEvent(101));
        REQUIRE(intReceived == 101);

        ea.Publish(AnotherTestEvent("EventString"));
        REQUIRE(stringReceived == "EventString");
    }

    SECTION("Disconnect from event") {
        int received = 0;

        auto connection = ea.Subscribe<TestEvent>([&received](const TestEvent event) {
            received = event.val;
        });

        connection.disconnect();  // Unsubscribe
        ea.Publish(TestEvent(300));
        REQUIRE(received == 0);  // Handler shouldn't have been invoked.
    }
}