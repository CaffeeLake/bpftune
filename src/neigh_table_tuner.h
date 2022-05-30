#include <bpftune.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif

enum neigh_table_tunables {
	NEIGH_TABLE_IPV4_GC_INTERVAL,
	NEIGH_TABLE_IPV4_GC_STALE_TIME,
	NEIGH_TABLE_IPV4_GC_THRESH1,
	NEIGH_TABLE_IPV4_GC_THRESH2,
	NEIGH_TABLE_IPV4_GC_THRESH3,
	NEIGH_TABLE_IPV6_GC_INTERVAL,
	NEIGH_TABLE_IPV6_GC_STALE_TIME,
	NEIGH_TABLE_IPV6_GC_THRESH1,
	NEIGH_TABLE_IPV6_GC_THRESH2,
	NEIGH_TABLE_IPV6_GC_THRESH3,
	NEIGH_TABLE_NUM_TUNABLES
};

struct tbl_stats {
	int family;
	int entries;
	int gc_entries;
	int max;
	int ifindex;
	char dev[IFNAMSIZ];
};
