/* -*- Mode: C++; tab-width: 8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#ifndef RR_TRACE_FRAME_H_
#define RR_TRACE_FRAME_H_

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "Event.h"
#include "ExtraRegisters.h"
#include "PerfCounters.h"
#include "Registers.h"
#include "Ticks.h"

class TraceReader;
class TraceWriter;

/**
 * A trace_frame is one "trace event" from a complete trace.  During
 * recording, a trace_frame is recorded upon each significant event,
 * for example a context-switch or syscall.  During replay, a
 * trace_frame represents a "next state" that needs to be transitioned
 * into, and the information recorded in the frame dictates the nature
 * of the transition.
 */
class TraceFrame {
public:
  typedef uint32_t Time;

  TraceFrame(Time global_time, pid_t tid, const Event& event, Ticks tick_count)
      : global_time(global_time),
        tid_(tid),
        ev(event.encode()),
        ticks_(tick_count) {}
  TraceFrame(Time global_time, pid_t tid, const EncodedEvent& event,
             Ticks tick_count)
      : global_time(global_time), tid_(tid), ev(event), ticks_(tick_count) {}
  TraceFrame() : global_time(0), tid_(0), ticks_(0) { ev.encoded = 0; }

  void set_exec_info(const Registers& regs,
                     const PerfCounters::Extra* extra_perf_values,
                     const ExtraRegisters* extra_regs);

  Time time() const { return global_time; }
  pid_t tid() const { return tid_; }
  EncodedEvent event() const { return ev; }
  Ticks ticks() const { return ticks_; }

  const Registers& regs() const { return exec_info.recorded_regs; }
  const ExtraRegisters& extra_regs() const { return recorded_extra_regs; }

  /**
   * Log a human-readable representation of this to |out|
   * (defaulting to stdout), including a newline character.
   * A human-friendly format is used. Does not emit a trailing '}'
   * (so the caller can add more fields to the record).
   */
  void dump(FILE* out = nullptr) const;
  /**
   * Log a human-readable representation of this to |out|
   * (defaulting to stdout), including a newline character.  An
   * easily machine-parseable format is dumped.
   */
  void dump_raw(FILE* out = nullptr) const;

private:
  friend class TraceReader;
  friend class TraceWriter;

  Time global_time;
  pid_t tid_;
  EncodedEvent ev;
  Ticks ticks_;

  struct {
    PerfCounters::Extra extra_perf_values;
    Registers recorded_regs;
  } exec_info;

  // Only used when has_exec_info, but variable length (and usually not
  // present) so we don't want to stuff it into exec_info
  ExtraRegisters recorded_extra_regs;
};

#endif /* RR_TRACE_FRAME_H_ */
