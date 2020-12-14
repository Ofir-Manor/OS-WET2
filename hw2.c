#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/list.h>

long total_weight_of_task(struct task_struct* task);
long heaviest_task(struct task_struct* task);

asmlinkage long sys_hello(void) {
	printk("Hello, World!\n");
	return 0;
}

asmlinkage long sys_set_weight(int weight){
	if(weight<0) return -EINVAL;
	current->weight = weight;
	return 0;
}

asmlinkage long sys_get_total_weight(void){
	long w = total_weight_of_task(current);	
	printk("The weight is: %ld\n", w);
	return w;
	
}

asmlinkage long sys_get_heaviest_child(void){
	long heaviest_child_pid = heaviest_task(current);
	if(heaviest_child_pid==0) return -ECHILD;
	return heaviest_child_pid;
}	

/************************************************************
*   Help functions for searching in tree          *                                         *
*************************************************************/

long total_weight_of_task(struct task_struct* task){
	
	struct task_struct* curr_child;
	struct list_head* pos;
	long sum_weight = task->weight;
	list_for_each(pos, &(task->children)){
		curr_child = list_entry(pos, struct task_struct, sibling);
		sum_weight += total_weight_of_task(curr_child);
	}
	printk("The weight is: %ld\n", sum_weight);
	return sum_weight;	
}

long heaviest_task(struct task_struct* task){
	struct task_struct* curr_child;
	struct list_head* pos;
	long max_weight = 0;
	long max_weight_pid = 0;
	long curr_child_weight;

	list_for_each(pos, &(task->children)){
		curr_child = list_entry(pos, struct task_struct, sibling);
		curr_child_weight = total_weight_of_task(curr_child);
		if(curr_child_weight > max_weight){
			max_weight = curr_child_weight;
			max_weight_pid = curr_child->pid;
		}
	}
	
	return max_weight_pid;
}