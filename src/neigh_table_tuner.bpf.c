/* SPDX-License-Identifier: (LGPL-2.1 OR BSD-2-Clause) */
/* Copyright (c) 2022, Oracle and/or its affiliates. */

#include "bpftune.bpf.h"
#include "neigh_table_tuner.h"

struct {
	__uint(type, BPF_MAP_TYPE_HASH);
	__uint(max_entries, 1024);
	__type(key, __u64);
	__type(value, struct tbl_stats);
} tbl_map SEC(".maps");

static __always_inline struct tbl_stats *get_tbl_stats(struct neigh_table *tbl, struct net_device *dev)
{
	struct tbl_stats *tbl_stats = bpf_map_lookup_elem(&tbl_map, &tbl);
	if (!tbl_stats) {
		struct tbl_stats new_tbl_stats = {};

		new_tbl_stats.family = tbl->family;
		new_tbl_stats.entries = tbl->entries.counter;
		new_tbl_stats.max = tbl->gc_thresh3;
		if (dev) {
			__builtin_memcpy(&new_tbl_stats.dev, dev->name, sizeof(new_tbl_stats.dev));
			new_tbl_stats.ifindex = dev->ifindex;
		}
		bpf_map_update_elem(&tbl_map, &tbl, &new_tbl_stats, BPF_ANY);
		tbl_stats = bpf_map_lookup_elem(&tbl_map, &tbl);
		if (!tbl_stats)
			return 0;
	}
	tbl_stats->entries = tbl->entries.counter;
	tbl_stats->gc_entries = tbl->gc_entries.counter;

	tbl_stats->max = tbl->gc_thresh3;

	return tbl_stats;
}

SEC("tp_btf/neigh_create")
int BPF_PROG(bpftune_neigh_create, struct neigh_table *tbl,
	     struct net_device *dev, const void *pkey,
	     struct neighbour *n, bool exempt_from_gc)
{
	struct tbl_stats *tbl_stats, new_tbl_stats = {};
	struct bpftune_event event = {};

	tbl_stats = get_tbl_stats(tbl, dev);
	if (!tbl_stats)
		return 0;

	/* exempt from gc entries are not subject to space constraints, but
 	 * do take up table entries.
 	 */

	__bpf_printk("neigh_create: tbl entries %d (%d gc) %d max\n",
		     tbl_stats->entries, tbl_stats->gc_entries, tbl->gc_thresh3);
	//if (NEARLY_FULL(tbl_stats->entries, tbl_stats->max)) {
	{	
		event.tuner_id = tuner_id;
		__builtin_memcpy(&event.raw_data, tbl_stats, sizeof(*tbl_stats));
		bpf_ringbuf_output(&ringbuf_map, &event, sizeof(event), 0);
	}
	return 0;
}

char _license[] SEC("license") = "GPL";
