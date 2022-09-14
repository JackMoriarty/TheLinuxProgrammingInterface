/**
 * @file mymalloc_myfree.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief 自己实现的 malloc 和 free, 只供单线程使用
 * @version 0.1
 * @date 2018-12-31
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MEM_INC_SIZE 1024

/**
 * @brief 空闲块首部信息
 * 
 */
struct free_block_msg {
    size_t block_size;
    struct free_block_msg * prev;
    struct free_block_msg * next;
};

/**
 * @brief 空闲块列表头
 * 
 */
static struct free_block_msg free_block_list_head ={
    .block_size = 0,
    .prev = &free_block_list_head,
    .next = &free_block_list_head
};
struct free_block_msg * free_block_list_head_p = &free_block_list_head;

/**
 * @brief 自己实现的malloc函数, 只供单线程使用
 * 
 * @param size 申请内存块大小
 * @return void* 申请得到的内存块指针
 */
void *my_malloc(size_t size)
{
    struct free_block_msg *ptr = NULL;

    // 在空闲块列表中查找
    ptr = free_block_list_head_p->next;
    for (; ptr != free_block_list_head_p; ptr = ptr->next) {
        if (ptr->block_size >= size) {
            break;
        }
    }

    // 没找到, 调用sbrk申请更大的堆空间
    if (ptr == free_block_list_head_p) {
        size_t alloc_num = size / MEM_INC_SIZE;
        void *temp_p = sbrk(MEM_INC_SIZE * (alloc_num + 1));

        if (temp_p == (void *)(-1)) {
            perror("No MORE MEM\n");
            exit(EXIT_FAILURE);
        }

        // 信息填充, 并加入到空闲列表, 尝试与最后一个块合并
        if (free_block_list_head_p->prev + free_block_list_head_p->block_size + 
                sizeof(size_t) == temp_p) {
            free_block_list_head_p->prev->block_size += MEM_INC_SIZE * (alloc_num + 1);
        } else {
            // 无法合并
            ((struct free_block_msg *)temp_p)->block_size = 
                    MEM_INC_SIZE * (alloc_num + 1) - sizeof(size_t);
            ((struct free_block_msg *)temp_p)->prev = free_block_list_head_p->prev;
            ((struct free_block_msg *)temp_p)->next = free_block_list_head_p;
            free_block_list_head_p->prev->next = temp_p;
            free_block_list_head_p->prev = temp_p;
        }
        
        ptr = free_block_list_head_p->prev;
    }

    // 选定的内存空间比申请的大, 在保证分割剩下的内存足够保存相关信息前提下,进行分割
    if (ptr->block_size - size >= sizeof(struct free_block_msg)) {
        void *temp_p = (void *)ptr + size + sizeof(size_t);

        // 填充两者相关头部信息, 将分割的块添加到空闲块链表中
        ((struct free_block_msg *)temp_p)->next = ptr->next;
        ((struct free_block_msg *)temp_p)->prev = ptr;
        ptr->next->prev = temp_p;
        ptr->next = temp_p;
        ((struct free_block_msg *)temp_p)->block_size = 
            ptr->block_size - size - sizeof(size_t);
        ptr->block_size = size;
        
    }

    // 从空闲列表中取出内存块
    ptr->prev->next = ptr->next;
    ptr->next->prev = ptr->prev;

    return (void *)ptr;
}

/**
 * @brief 自己实现的free函数, 仅供单线程使用
 * 
 * @param ptr 待释放的内存指针
 */
void my_free(void *ptr)
{
    // 寻找应当插入的位置
    struct free_block_msg *tmp_p = free_block_list_head_p->next;
    for (;tmp_p != free_block_list_head_p; tmp_p = tmp_p->next) {
        if (ptr < (void *)tmp_p) {
            break;
        }
    }

    // 尝试合并相邻块
    struct free_block_msg *tmp_front_p = tmp_p->prev;
    //尝试与后面的块(如果相邻)合并
    if(tmp_p != free_block_list_head_p && 
            ptr + ((struct free_block_msg *)ptr)->block_size + sizeof(size_t) == tmp_p) {
        ((struct free_block_msg *)ptr)->next = tmp_p->next;
        ((struct free_block_msg *)ptr)->block_size += tmp_p->block_size + sizeof(size_t);
        tmp_p->next->prev = ptr;
    } else {
        // 不连续
        ((struct free_block_msg *)ptr)->next = tmp_p;
        tmp_p->prev = ptr;
    }

    // 尝试与前面的块(如果相邻)合并
    if(tmp_front_p != free_block_list_head_p &&
            (void *)tmp_front_p + tmp_front_p->block_size + sizeof(size_t) == ptr) {
        tmp_front_p->block_size += ((struct free_block_msg *)ptr)->block_size + sizeof(size_t);
        tmp_front_p->next = ((struct free_block_msg *)ptr)->next;
        ((struct free_block_msg *)ptr)->next->prev = tmp_front_p;
    } else {
        // 不连续
        ((struct free_block_msg *)ptr)->prev = tmp_front_p;
        tmp_front_p->next = ptr;
    }
    
    // 尝试缩小program break
    tmp_p = free_block_list_head_p->prev;
    if (sbrk(0) == (void *)tmp_p + tmp_p->block_size + sizeof(size_t)) {
        size_t free_num = tmp_p->block_size / MEM_INC_SIZE;
        sbrk(-1 * free_num * MEM_INC_SIZE);
    }
}

int main(int argc, char *argv[])
{
    void *tmp = NULL;
    printf("Initial program break:\t%10p\n", sbrk(0));
    tmp = my_malloc(10240);

    printf("Program break is now:\t%10p\n", sbrk(0));
    my_free(tmp);

    printf("After free(), program break is:\t%10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}