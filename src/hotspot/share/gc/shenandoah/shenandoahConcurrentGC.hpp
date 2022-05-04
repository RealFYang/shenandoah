/*
 * Copyright (c) 2021, Red Hat, Inc. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_GC_SHENANDOAH_SHENANDOAHCONCURRENTGC_HPP
#define SHARE_GC_SHENANDOAH_SHENANDOAHCONCURRENTGC_HPP

#include "gc/shared/gcCause.hpp"
#include "gc/shenandoah/shenandoahConcurrentMark.hpp"
#include "gc/shenandoah/shenandoahGC.hpp"
#include "gc/shenandoah/shenandoahHeap.hpp"

class ShenandoahGeneration;

class VM_ShenandoahInitMark;
class VM_ShenandoahFinalMarkStartEvac;
class VM_ShenandoahInitUpdateRefs;
class VM_ShenandoahFinalUpdateRefs;

class ShenandoahConcurrentGC : public ShenandoahGC {
  friend class VM_ShenandoahInitMark;
  friend class VM_ShenandoahFinalMarkStartEvac;
  friend class VM_ShenandoahInitUpdateRefs;
  friend class VM_ShenandoahFinalUpdateRefs;
  friend class VM_ShenandoahFinalRoots;

protected:
  ShenandoahConcurrentMark    _mark;

private:
  ShenandoahDegenPoint        _degen_point;
  bool                        _abbreviated;
  const bool                  _do_old_gc_bootstrap;

protected:
  ShenandoahGeneration* const _generation;

public:
  ShenandoahConcurrentGC(ShenandoahGeneration* generation, bool do_old_gc_bootstrap);
  bool collect(GCCause::Cause cause);
  ShenandoahDegenPoint degen_point() const;
  bool abbreviated() const { return _abbreviated; }

private:
  // Entry points to STW GC operations, these cause a related safepoint, that then
  // call the entry method below
  void vmop_entry_init_mark();

protected:
  void vmop_entry_final_mark();
  void vmop_entry_final_roots(bool incr_region_ages);

private:
  void vmop_entry_init_updaterefs();
  void vmop_entry_final_updaterefs();

  // Entry methods to normally STW GC operations. These set up logging, monitoring
  // and workers for net VM operation
  void entry_init_mark();
  void entry_final_mark();
  void entry_init_updaterefs();
  void entry_final_updaterefs();
  void entry_final_roots();

  // Entry methods to normally concurrent GC operations. These set up logging, monitoring
  // for concurrent operation.
  void entry_reset();
  void entry_mark_roots();

protected:
  void entry_mark();
  void entry_thread_roots();
  void entry_weak_refs();
  void entry_weak_roots();
  void entry_class_unloading();
  void entry_strong_roots();
  void entry_cleanup_early();
  virtual void op_final_mark();

private:
  void entry_evacuate();
  void entry_update_thread_roots();
  void entry_updaterefs();
  void entry_cleanup_complete();
  void entry_global_coalesce_and_fill();

  // Actual work for the phases
  void op_reset();
  void op_init_mark();
  void op_mark_roots();
  void op_mark();
  void op_thread_roots();
  void op_weak_refs();
  void op_weak_roots();
  void op_class_unloading();
  void op_strong_roots();
  void op_cleanup_early();
  void op_evacuate();
  void op_init_updaterefs();
  void op_updaterefs();
  void op_update_thread_roots();
  void op_final_updaterefs();
  void op_final_roots();
  void op_cleanup_complete();
  void op_global_coalesce_and_fill();

  // Messages for GC trace events, they have to be immortal for
  // passing around the logging/tracing systems
  void init_mark_event_message(char* buf, size_t len) const;
  void final_mark_event_message(char* buf, size_t len) const;
  void conc_mark_event_message(char* buf, size_t len) const;

protected:
  // Check GC cancellation and abort concurrent GC
  bool check_cancellation_and_abort(ShenandoahDegenPoint point);
};

#endif // SHARE_GC_SHENANDOAH_SHENANDOAHCONCURRENTGC_HPP
