#include "mmo/AuthTicketAuthority.h"
#include "mmo/WorldServerIntegrationAdapter.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <vector>

using namespace mmo::portable;
#define CHECK(c) do { if (!(c)) { std::fprintf(stderr, "line %d: %s\n", __LINE__, #c); return 1; } } while (false)

// Offline *server-side* new-client handoff test, not a real socket/client.
// The accountId, connectionId, sessionId and AOI entity identity originate
// from trusted server state, not user-controlled packet fields.
int main() {
    AuthTicketAuthority auth;
    auto firstTicket = auth.issue(501, 7);
    auto secondTicket = auth.issue(502, 7);
    CHECK(firstTicket.status == AuthTicketStatus::Ok);
    CHECK(secondTicket.status == AuthTicketStatus::Ok);

    const auto first = auth.consume(firstTicket.token, 7, 1001);
    const auto second = auth.consume(secondTicket.token, 7, 1002);
    CHECK(first.status == AuthTicketStatus::Ok);
    CHECK(second.status == AuthTicketStatus::Ok);
    CHECK(auth.valid(first.session));
    CHECK(auth.valid(second.session));
    CHECK(auth.consume(firstTicket.token, 7, 1003).status == AuthTicketStatus::NotFound);

    WorldServerIntegrationAdapter world({16, 16, 8, 16, 64},
                                        {8, 4096, 4096, 8, 1}, true);
    CHECK(world.bindOwnerToCurrentThread());
    const EntityKey charA{1501, 1, 7, 1};
    const EntityKey charB{1502, 1, 7, 1};
    TrustedWorldContext contextA{first.session.connectionId, charA,
                                  first.session.generation, 1};
    TrustedWorldContext contextB{second.session.connectionId, charB,
                                  second.session.generation, 1};
    CHECK(world.attach(contextA, {256, 3, 1000000000}, 32));
    CHECK(world.attach(contextB, {256, 3, 1000000000}, 32));
    CHECK(world.upsert({charA, 0, 0, 1, true}) == AoiStatus::Ok);
    CHECK(world.upsert({charB, 5, 7, 1, true}) == AoiStatus::Ok);
    auto visibleA = world.prepare(contextA, 0, 0, 100, 1);
    CHECK(visibleA.replication.status == ReplicationStatus::Prepared);
    CHECK(!visibleA.replication.batch.events.empty());
    const auto seq = visibleA.replication.batch.sequence;
    CHECK(seq != 0);

    // Synthetic *encoded frame placeholder*; this test does not implement
    // the new-client wire codec or send bytes on the network.
    CHECK(world.admit(contextA, seq, {0x41, 0x01}, 2).delivery ==
          AoiDeliveryStatus::Accepted);
    CHECK(world.beginTick(1) == AoiPoolStatus::Ok);
    const auto send = world.nextSend(3);
    CHECK(send.status == AoiPoolStatus::Ok);
    CHECK(send.sessionId == contextA.sessionId);
    CHECK(send.sequence == seq);
    CHECK(world.postAck(contextA, seq) == WorldCommandQueue::PostStatus::Accepted);
    CHECK(world.drain().failed == 0);
    CHECK(world.metrics().acceptedAcks == 1);

    // AuthService's second successful login revokes the previous session.
    auto replacementTicket = auth.issue(501, 7);
    CHECK(replacementTicket.status == AuthTicketStatus::Ok);
    CHECK(!auth.valid(first.session));
    CHECK(!auth.close(first.session));
    CHECK(world.detach(contextA));
    CHECK(world.postAck(contextA, seq) == WorldCommandQueue::PostStatus::Accepted);
    world.drain();
    CHECK(world.metrics().acceptedAcks == 1);

    const auto replacement = auth.consume(replacementTicket.token, 7, 2001);
    CHECK(replacement.status == AuthTicketStatus::Ok);
    CHECK(replacement.session.generation > first.session.generation);
    CHECK(auth.valid(replacement.session));
    TrustedWorldContext current{replacement.session.connectionId, charA,
                                replacement.session.generation, 2};
    CHECK(world.attach(current, {256, 3, 1000000000}, 32));
    CHECK(world.prepare(contextA, 0, 0, 100, 4).status == AoiPoolStatus::Invalid);
    auto newPlan = world.prepare(current, 0, 0, 100, 4);
    CHECK(newPlan.replication.status == ReplicationStatus::Prepared);
    CHECK(!newPlan.replication.batch.events.empty());
    CHECK(newPlan.replication.batch.events.front().kind == AoiDelta::Kind::Reset);

    // Realm mismatch and replay never enter the WorldServer.
    const auto bad = auth.issue(503, 7);
    CHECK(bad.status == AuthTicketStatus::Ok);
    CHECK(auth.consume(bad.token, 8, 9000).status == AuthTicketStatus::WrongRealm);
    CHECK(auth.revokeAccount(503));
    CHECK(auth.consume(bad.token, 7, 9000).status == AuthTicketStatus::NotFound);
    CHECK(world.detach(current));
    CHECK(world.detach(contextB));
    CHECK(auth.close(replacement.session));
    CHECK(auth.close(second.session));
    std::puts("NEW_CLIENT_AUTH_TO_AOI_OWNER_HARNESS_PASS");
    return 0;
}
