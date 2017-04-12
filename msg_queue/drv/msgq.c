#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#include "../msgq.h"

char *test_string = "Queue up and running";
static void msgq_encap_func(struct work_struct *work);

msgq_list_t msgq_struct[MSGQ_MAX_ID];

int test_func(char *str, int *q_num)
{
    printk(KERN_INFO "%s: %d: %s\n", __func__, *q_num, str);
    kfree(q_num);
    return 0;
}

void msgq_dump_stats(void)
{
    int count;
    
    printk(" %3s %12s %12s %18s %12s\n", "Qid", "Name",
            "Enqueued", "Dequeued", "Dropped");

    for(count = 0; count < MSGQ_MAX_ID; count++) {
        printk(" %3u %12s %12lu %18lu %12lu\n", count, msgq_struct[count].name,
                msgq_struct[count].enqueued,
                msgq_struct[count].dequeued,
                msgq_struct[count].dropped);
    }

    return;
}

static void msgq_encap_func(struct work_struct *work)
{
    msgq_t *sched_task = (msgq_t *)work;
    sched_task->func(sched_task->p1, sched_task->p2, sched_task->p3);
   
    msgq_struct[sched_task->qid].dequeued++;
    
	if (msgq_struct[sched_task->qid].enqueued > 0)
        msgq_struct[sched_task->qid].enqueued--;
    
    kfree(sched_task);
	return;
}

int init_work_queue(char *q_name, int qid, int thread_type, int nthreads,
        int qlen, unsigned int mask)
{ 
    char buf[MSGQ_NAME_MAX_LEN];
    int *q_num = kmalloc(sizeof(int), GFP_ATOMIC);
    *q_num = qid;

	strncpy(msgq_struct[qid].name, q_name, MSGQ_NAME_MAX_LEN);
    
    memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "%s_%s",
            MSGQ_STR_PREFIX, msgq_struct[qid].name); 

    msgq_struct[qid].msg_queue = create_workqueue(buf);

    if (!msgq_struct[qid].msg_queue) {
        printk(KERN_ERR "%s: Unable to create kernel thread\n", __func__);
        return -1;
    }
    add_work_to_queue(qid, (void *)test_func, test_string, q_num, 0);

    return 0;
}
EXPORT_SYMBOL(init_work_queue);

int add_work_to_queue(int qid, void (*func_ptr)(void *, void *, void*),
        void *p1, void *p2, void *p3)
{
    msgq_t *work;
   
	if (msgq_struct[qid].enqueued >= MSGQ_MAX_QLEN) {
		printk(KERN_ERR "Queue:%d full. Hence task drop\n", qid);
		msgq_struct[qid].dropped++;
		return -1;
	}

	if (!msgq_struct[qid].msg_queue) {
		printk(KERN_ERR "%s: Queue %d not initialized\n", __func__, qid);
		return -1;
	}
    work = kzalloc(sizeof(msgq_t), GFP_ATOMIC);

    //printk(KERN_INFO "%s: Adding task\n", __func__);

    INIT_WORK((struct work_struct *)work, msgq_encap_func);
    work->qid = qid;
    work->func = func_ptr;
    work->p1 = p1;
    work->p2 = p2;
    work->p3 = p3;

    queue_work(msgq_struct[qid].msg_queue, (struct work_struct *)work);
    msgq_struct[qid].enqueued++;

    return 0;
}
EXPORT_SYMBOL(add_work_to_queue);

static int msgq_init(void)
{
    printk(KERN_INFO "%s: Init\n", __func__);
    return 0;
}

static void msgq_exit(void)
{
    int count;

    printk(KERN_INFO "%s: Exit\n", __func__);
    for(count = 0; count < MSGQ_MAX_ID; count++) {
        flush_workqueue(msgq_struct[count].msg_queue);
        destroy_workqueue(msgq_struct[count].msg_queue);
        printk(KERN_INFO "%s: Flush and Destroy MSGQ_%s\n",
                __func__, msgq_struct[count].name);
    }
    return;
}

module_init(msgq_init);
module_exit(msgq_exit);

MODULE_LICENSE("GPL");
