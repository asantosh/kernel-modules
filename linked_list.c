#include <linux/module.h>       /* for module_init and exit function */
#include <linux/list.h>         /* to include the struct list_head */
#include <linux/slab.h>         /* to include kmalloc */

/* macro to enable printk to print the argument along with additional info
   like the function name, the line number */
#define print(fmt, ...) \
	printk("LIST: %s: %d: "fmt"\n", __func__, __LINE__, ##__VA_ARGS__)

struct node {
	int data;
	struct list_head list;
};

LIST_HEAD(mylist);
/* or you could have declared as struct node mylist;
   followed by INIT_LIST_HEAD(&mylist.list); which will initialize the list */
	
int list_init(void)
{
	struct node *ptr;
	int i = 0;

	print("List");
	
	for (i = 0; i < 5; i++) {
		ptr =  (struct node *) kmalloc(sizeof(struct node), GFP_ATOMIC);
		ptr->data = i + 1;
		list_add_tail(&(ptr->list), &mylist);
		/* list_add() adds a new entry after the specified head.
		   This is good for implementing stacks.
		   list_add_tail() adds a new entry before the specified head.
		   This is good for implementing queues. */
	}

	print("traverse the list");

	list_for_each_entry(ptr, &mylist, list)
		print("ptr = %p data = %d", ptr, ptr->data );
	/*list_for_each_entry is used to traverse the loop, where ptr is the loop pointer, 
	  mylist being the head node and list is the name of list_struct with struct*/

	return 0;
}

void list_exit(void)
{
	struct node *ptr, *tmp;

	print("Deleting the list\n");
	
	list_for_each_entry_safe(ptr, tmp, &mylist, list) {
		print("Freeing Node : %d", ptr->data);
		list_del(&(ptr->list));
		kfree(ptr);
	}

	print("List end");

}

module_init(list_init);
module_exit(list_exit);

MODULE_AUTHOR("Linux");
MODULE_DESCRIPTION("TEST");
MODULE_LICENSE("GPL");
