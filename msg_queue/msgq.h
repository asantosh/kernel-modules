#ifndef _MSGQ_H_
#define _MSGQ_H_

#include <linux/workqueue.h>

#define MSGQ_MAX_ID			 32
#define MSGQ_NAME_MAX_LEN    32
#define MSGQ_STR_PREFIX      "MQ"
#define MSGQ_MAX_QLEN        8192

typedef struct _msgq_list_t {
    char name[MSGQ_NAME_MAX_LEN];
	struct workqueue_struct *msg_queue;
    unsigned long enqueued;
    unsigned long dequeued;
    unsigned long dropped;
} msgq_list_t;

typedef struct _msgq_t {
    struct work_struct my_work;
    unsigned int qid;
    void (*func)(void *, void *, void *);
    void *p1;
    void *p2;
    void *p3;
} msgq_t;

extern int add_work_to_queue(int qid, void (*func_ptr)(void *, void *, void*),
        void *p1, void *p2, void *p3);

extern int init_work_queue(char *q_name, int qid, int thread_type, int nthreads,
        int qlen, unsigned int mask);

#endif /* _MSGQ_H_ */
