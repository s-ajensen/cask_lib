#include <catch2/catch_all.hpp>
#include <cask/event/event_queue.hpp>

struct DamageEvent {
    int amount;
};

SCENARIO("event queue poll returns nothing before any swap", "[event_queue]") {
    GIVEN("a fresh queue with emitted events") {
        EventQueue<DamageEvent> queue;
        queue.emit(DamageEvent{10});

        WHEN("poll is called without swap") {
            auto events = queue.poll();

            THEN("no events are visible") {
                REQUIRE(events.size() == 0);
            }
        }
    }
}

SCENARIO("event queue accumulates multiple emits", "[event_queue]") {
    GIVEN("a queue with several emitted events") {
        EventQueue<DamageEvent> queue;
        queue.emit(DamageEvent{10});
        queue.emit(DamageEvent{20});
        queue.emit(DamageEvent{30});

        WHEN("swap is called and events are polled") {
            queue.swap();
            auto events = queue.poll();

            THEN("all events are returned in emission order") {
                REQUIRE(events.size() == 3);
                REQUIRE(events[0].amount == 10);
                REQUIRE(events[1].amount == 20);
                REQUIRE(events[2].amount == 30);
            }
        }
    }
}

SCENARIO("event queue swap clears previous events", "[event_queue]") {
    GIVEN("a queue that has been swapped once with events") {
        EventQueue<DamageEvent> queue;
        queue.emit(DamageEvent{10});
        queue.swap();

        WHEN("new events are emitted and swapped again") {
            queue.emit(DamageEvent{50});
            queue.swap();
            auto events = queue.poll();

            THEN("only the second batch is visible") {
                REQUIRE(events.size() == 1);
                REQUIRE(events[0].amount == 50);
            }
        }
    }
}

SCENARIO("event queue poll returns nothing when nothing was emitted", "[event_queue]") {
    GIVEN("a fresh queue with no emitted events") {
        EventQueue<DamageEvent> queue;

        WHEN("swap is called and events are polled") {
            queue.swap();
            auto events = queue.poll();

            THEN("no events are returned") {
                REQUIRE(events.size() == 0);
            }
        }
    }
}

SCENARIO("event queue delivers events after swap", "[event_queue]") {
    GIVEN("an event queue with an emitted event") {
        EventQueue<DamageEvent> queue;
        queue.emit(DamageEvent{25});

        WHEN("swap is called") {
            queue.swap();
            auto events = queue.poll();

            THEN("the event is visible with correct data") {
                REQUIRE(events.size() == 1);
                REQUIRE(events[0].amount == 25);
            }
        }
    }
}
