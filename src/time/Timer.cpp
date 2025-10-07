//
// Created by Brandon on 3/25/25.
//

#include "Timer.h"
#include "Assertions.h"
#include "Clock.h"

namespace rgb {

Timer::Timer() {
  INFO("Initializing Timers");
  for (int i = 0; i < TIMER_COUNT; ++i) {
    auto& current = nodes[i];
    current.id = i;
    if (i > 0) {
      current.prev = &nodes[i - 1];
    }
    else {
      current.prev = nullptr;
    }
    if (i < TIMER_COUNT - 1) {
      current.next = &nodes[i + 1];
    }
    else {
      current.next = nullptr;
    }
  }
  for (int i = 0; i < TIMER_COUNT; ++i) {
    ASSERT(nodes[i].id == i, "Id invalid");
    if (i != 0) {
      ASSERT(nodes[i].prev == &nodes[i - 1], "Prev Mismatch");
    }
    if (i != TIMER_COUNT - 1) {
      ASSERT(nodes[i].next == &nodes[i + 1], "Next Mismatch");
    }
  }
  unusedHead = &nodes[0];
  ASSERT(unusedHead != nullptr, "Failed to initialize Timers");
  ASSERT(nodes[0].prev == nullptr, "Head invalid");
  ASSERT(nodes[TIMER_COUNT - 1].next == nullptr, "Tail invalid");
}

auto Timer::SetTimeout(Duration duration, const Runnable& function) -> TimerHandle {
  return Instance().setTimeout(duration, [=](const auto& unused){ function(); });
}

auto Timer::ContinuouslyFor(Duration duration, const Runnable& function) -> TimerHandle {
  return Instance().continuouslyFor(duration, [=](const auto& unused){ function(); });
}


auto Timer::ContinuouslyFor(Duration duration, const TimerFunction& function) -> TimerHandle {
  return Instance().continuouslyFor(duration, function);
}

auto Timer::SetTimeout(Duration duration, const TimerFunction& function) -> TimerHandle {
  return Instance().setTimeout(duration, function);
}

auto Timer::SetImmediateTimeout(const Runnable& function) -> TimerHandle {
  return Instance().setTimeout(Duration {0}, [=](const auto& unused){ function(); });
}

auto Timer::SetImmediateTimeout(const TimerFunction& function) -> TimerHandle {
  return Instance().setTimeout(Duration {0}, function);
}

auto Timer::setTimeout(Duration duration, const TimerFunction& function) -> TimerHandle {
  TRACE("SetTimeout()");
  auto now = Clock::Now();
  auto timer = nextTimerNode();

  timer->clean();
  timer->timerFunction = function;
  timer->cancelFunction = DoNothing;
  timer->executeAt = now + duration;
  timer->handleId = nextHandleId++;
  timer->startedAt = now;

  TRACE("Assigning Timer '%i'", timer->id);
  ASSERT(timer->next == nullptr, "Timer.Next is not nullptr");
  ASSERT(timer->prev == nullptr, "Timer.Prev is not nullptr");

  TimerNode::InsertFront(toAddHead, timer);

  return TimerHandle { timer };
}

auto Timer::continuouslyFor(Duration duration, const TimerFunction& function) -> TimerHandle {
  TRACE("ContinuouslyFor()");
  auto now = Clock::Now();
  auto timer = nextTimerNode();

  timer->clean();
  timer->timerFunction = function;
  timer->cancelFunction = DoNothing;
  timer->finishAt = now + duration;
  timer->executeAt = now;
  timer->startedAt = now;
  timer->handleId = nextHandleId++;

  TRACE("Assigning Timer '%i'. startedAt=%lu, finishAt=%llu", timer->id, timer->startedAt.value, timer->finishAt.value);
  ASSERT(timer->next == nullptr, "Timer.Next is not nullptr");
  ASSERT(timer->prev == nullptr, "Timer.Prev is not nullptr");

  TimerNode::InsertFront(toAddHead, timer);

  return TimerHandle { timer };
}

auto Timer::Cancel(TimerNode* node) -> void {
  Instance().cancel(node);
}

auto Timer::cancel(TimerNode* node) -> void {
  if (node != nullptr) {
    TRACE("Cancelling Timer '%i'", node->id);
    node->cancelled = true;
  }
}

auto Timer::enqueueForAdding(TimerNode* node) -> void {
  ASSERT(node->next == nullptr, "Timer.Next is not nullptr");
  ASSERT(node->prev == nullptr, "Timer.Prev is not nullptr");
  TimerNode::InsertFront(toAddHead, node);
}

auto Timer::ProcessTimers() -> void {
  Instance().processTimers();
}

auto Timer::processTimers() -> void {
  processAdditions();

  auto now = Clock::Now();
  while (activeHead != nullptr && activeHead->executeAt <= now) {
    auto timer = TimerNode::Pop(activeHead);
    if (timer->cancelled) {
      timer->cancelFunction();
      recycle(timer);
      continue;
    }
    else {
      executeTimer(timer, now);
    }
  }
}

auto Timer::executeTimer(TimerNode* timer, Timestamp now) -> void {
  auto shouldRecycle = false;
  if (timer->isContinuous()) {
    shouldRecycle = executeContinuousTimer(timer, now);
  }
  else {
    shouldRecycle = executeRegularTimer(timer, now);
  }

  if (shouldRecycle) {
    recycle(timer);
  }
}

auto Timer::processAdditions() -> void {
  while (toAddHead != nullptr) {
    auto nodeToInsert = toAddHead;
    toAddHead = toAddHead->next;

    nodeToInsert->prev = nullptr;
    nodeToInsert->next = nullptr;

    if (activeHead == nullptr || nodeToInsert->executeAt < activeHead->executeAt) {
      TimerNode::InsertFront(activeHead, nodeToInsert);
      continue;
    }

    auto current = activeHead;
    while (current->next != nullptr && current->next->executeAt < nodeToInsert->executeAt) {
      current = current->next;
    }

    // todo current->next->executeAt >= nodeToInsert->executeAt
    ASSERT(nodeToInsert->executeAt >= current->executeAt, "Node in wrong position");

    nodeToInsert->prev = current;
    nodeToInsert->next = current->next;
    if (current->next != nullptr) {
      current->next->prev = nodeToInsert;
    }
    current->next = nodeToInsert;
  }
}

auto Timer::nextTimerNode() -> TimerNode* {
  if (unusedHead == nullptr) {
    reclaimNodes();
    ASSERT_C(unusedHead != nullptr, "No more timer nodes available", Color::WHITE());
  }
  auto next = TimerNode::Pop(unusedHead);
  ASSERT(next->next == nullptr, "Next is not a nullptr");
  ASSERT(next->prev == nullptr, "Prev is not a nullptr");
  return next;
}

auto Timer::reclaimNodes() -> void {
  INFO("Reclaiming Timer Nodes");
  for (auto& node : nodes) {
    if (node.cancelled) {
      TimerNode::Remove(activeHead, &node);
      node.cancelled = false;
      TimerNode::InsertFront(unusedHead, &node);
    }
  }
}

auto Timer::Instance() -> Timer& {
  static Timer timer;
  return timer;
}

auto Timer::count() {
  auto num = 0;
  auto current = activeHead;
  while (current != nullptr) {
    if (!current->cancelled) {
      ++num;
    }
    current = current->next;
  }
  return num;
}

auto Timer::Count() {
  return Instance().count();
}

auto Timer::recycle(TimerNode* timer) -> void {
  TRACE("Recycling Timer '%i'", timer->id);
  timer->clean();
  TimerNode::InsertFront(unusedHead, timer);
}

auto Timer::executeRegularTimer(TimerNode* timer, Timestamp now) -> bool {
  auto context = TimerContext{};
  TRACE("Running Timer '%i'", timer->id);
  timer->timerFunction(context);
  if (context.repeatIn) {
    TRACE("Repeating Timer '%i'", timer->id);
    timer->repeat(now + context.repeatIn.value());
    enqueueForAdding(timer);
    return false;
  }
  else {
    return true;
  }
}

auto Timer::executeContinuousTimer(TimerNode* timer, Timestamp now) -> bool {
  auto context = TimerContext{};
  context.percentComplete = std::min(1.0f, (now - timer->startedAt).percentOf(timer->finishAt - timer->startedAt));
  timer->timerFunction(context);
  if (now < timer->finishAt) {
    timer->repeat(now);
    enqueueForAdding(timer);
    return false;
  }
  else {
    return true;
  }
}

}
