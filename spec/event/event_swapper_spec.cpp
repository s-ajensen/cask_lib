#include <catch2/catch_all.hpp>
#include <cask/event/event_queue.hpp>
#include <cask/event/event_swapper.hpp>

struct DamageEvent {
    int amount;
};

struct HealEvent {
    int amount;
};

SCENARIO("swap_all swaps a registered queue", "[event_swapper]") {
    GIVEN("a swapper with one registered queue") {
        EventQueue<DamageEvent> damage_queue;
        EventSwapper swapper;
        swapper.add(&damage_queue, swap_queue<DamageEvent>);

        damage_queue.emit(DamageEvent{42});

        WHEN("swap_all is called") {
            swapper.swap_all();

            THEN("the queue's events are available via poll") {
                auto events = damage_queue.poll();
                REQUIRE(events.size() == 1);
                REQUIRE(events[0].amount == 42);
            }
        }
    }
}

SCENARIO("swap_all swaps multiple registered queues", "[event_swapper]") {
    GIVEN("a swapper with two registered queues of different types") {
        EventQueue<DamageEvent> damage_queue;
        EventQueue<HealEvent> heal_queue;
        EventSwapper swapper;
        swapper.add(&damage_queue, swap_queue<DamageEvent>);
        swapper.add(&heal_queue, swap_queue<HealEvent>);

        damage_queue.emit(DamageEvent{30});
        heal_queue.emit(HealEvent{15});

        WHEN("swap_all is called") {
            swapper.swap_all();

            THEN("both queues have their events available via poll") {
                auto damage_events = damage_queue.poll();
                REQUIRE(damage_events.size() == 1);
                REQUIRE(damage_events[0].amount == 30);

                auto heal_events = heal_queue.poll();
                REQUIRE(heal_events.size() == 1);
                REQUIRE(heal_events[0].amount == 15);
            }
        }
    }
}
