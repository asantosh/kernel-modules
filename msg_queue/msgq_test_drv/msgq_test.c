#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#include "../msgq.h"

int msgq_test_func(int *x, int *y)
{
    printk("%s: Q->%d V:%d\n", __func__, *x, *y);
    kfree(x);
    kfree(y);
    return 0;
}

static int msgq_test_init(void)
{
    int count, val;
    int *q, *n;
    printk("%s: Init\n", __func__);

    for(count = 0; count < MSGQ_MAX_ID; count++) {

        if (init_work_queue("test_q", count, 0, 0, 0, 0)) {
            printk("Unable to Create the Workqueue\n");
            return -1;
        }

        for(val = 0; val < MSGQ_MAX_QLEN; val++) {

            q = kmalloc(sizeof(*q), GFP_KERNEL);
            n = kmalloc(sizeof(*n), GFP_KERNEL);

            if (!q || !n) {
                printk("Unable to allocate memory\n");
                return -1;
            }

            *q = count;
            *n = val;

            add_work_to_queue(count, (void *)msgq_test_func, q, n, 0);
        }
    }
    return 0;
}

static void msgq_test_exit(void)
{
    printk("%s: Exit\n", __func__);
    return;
}

module_init(msgq_test_init);
module_exit(msgq_test_exit);
