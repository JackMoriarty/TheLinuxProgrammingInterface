/**
 * @file binary_tree.c
 * @author Bangduo Chen (chenbangduo@qq.com)
 * @brief 多线程下的二叉树
 * @version 0.1
 * @date 2019-04-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

typedef struct bt_node {
    int key;
    int value;
    struct bt_node *parent;
    struct bt_node *l_child;
    struct bt_node *r_child;
    pthread_mutex_t lock;
} bt_node;

/**
 * @brief 初始化树的根
 * 
 * @param tree 作为树根的结点
 * @return int 0 时成功, 非 0 表示失败, 且该值为错误码
 * 此部分仅在主线程中执行
 */
int initialize(bt_node *tree)
{
    int s;

    tree->parent = NULL;
    tree->l_child = NULL;
    tree->r_child = NULL;
    tree->key = 100;    // 随机指定的值
    tree->value = 0;
    s = pthread_mutex_init(&(tree->lock), NULL);
    if (s != 0) {
        return s;
    }

    return 0;
}

/**
 * @brief 增加结点
 * 
 * @param tree 树的根
 * @param key 键值
 * @param value 值
 * @return int 0 表示成功, 非 0 表示失败, 且此值为错误码
 */
int add(bt_node *tree, int key, int value)
{
    int retval = 0;
    bt_node *tmp;

    // 对根结点加锁
    if (tree->parent == NULL) {
        retval = pthread_mutex_lock(&(tree->lock));
        if (retval != 0) {
            return retval;
        }
    }


    // 对父节点解锁
    if (tree->parent != NULL) {
        retval = pthread_mutex_lock(&(tree->parent->lock));
        if (retval != 0) {
            // 忽略解锁错误
            pthread_mutex_unlock(&(tree->lock));
            return retval;
        }
    }

    // NOTICE: 此时本节点已加锁, 加锁位置为父结点函数调用, 若为根节点则在开头处加锁
    if (tree->key > key) {
        if (tree->l_child == NULL) {
            tmp = (bt_node *)malloc(sizeof(bt_node));
            tmp->key = key;
            tmp->value = value;
            tmp->parent = tree;
            tmp->l_child = NULL;
            tmp->r_child = NULL;
            retval = pthread_mutex_init(&(tmp->lock), NULL);
            if (retval != 0) {
                free(tmp);
                // 忽略解锁时出现的错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }

            tree->l_child = tmp;
        } else {
            // 当前结点并非叶子结点, 对子节点加锁
            retval = pthread_mutex_lock(&(tree->l_child->lock));
            if (retval != 0) {
                // 忽略解锁时出现的错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }

            retval = add(tree->l_child, key, value);
            return retval;
        }
    } else if (tree->key == key) {
        tree->value = value;
    } else if (tree->key < key) {
        if (tree->r_child == NULL) {
            tmp = (bt_node *)malloc(sizeof(bt_node));
            tmp->key = key;
            tmp->value = value;
            tmp->parent = tree;
            tmp->l_child = NULL;
            tmp->r_child = NULL;
            retval = pthread_mutex_init(&(tmp->lock), NULL);
            if (retval != 0) {
                free(tmp);
                // 忽略释放锁出现的错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }

            tree->r_child = tmp;
        } else {
            // 当前结点并非叶子结点, 对子结点加锁
            retval = pthread_mutex_lock(&(tree->r_child->lock));
            if (retval != 0) {
                // 忽略解锁时出现的错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
            retval = add(tree->r_child, key, value);
            return retval;            
        }
    }

    // 对本节点解锁
    retval = pthread_mutex_unlock(&(tree->lock));
    return retval;
}

/**
 * @brief 删除结点
 * 
 * @param tree 根节点
 * @param key 待删除结点的键值
 * @return int 0 表示成功, 非 0 表示失败, 且此值为错误码
 */
int delete(bt_node *tree, int key)
{
    int retval;
    bt_node *tmp;

    // 对根结点加锁
    if (tree->parent == NULL) {
        retval = pthread_mutex_lock(&(tree->lock));
        if (retval != 0) {
            return retval;
        }
    }
    
    // 给父节点的父节点解锁
    if (tree->parent != NULL && tree->parent->parent != NULL) {
        retval = pthread_mutex_lock(&(tree->parent->parent->lock));
        if (retval != 0) {
            // 忽略解锁错误
            pthread_mutex_unlock(&(tree->parent->lock));
            pthread_mutex_unlock(&(tree->lock));
            return retval;
        }
    }

    if (key < tree->key) {
        if (tree->l_child != NULL) {
            // 给左子结点加锁
            retval = pthread_mutex_lock(&(tree->l_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->parent->lock));
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
            
            retval = delete(tree->l_child, key);
        }

        return 0;

    } else if (key == tree->key) {
        // 给左子结点和右子结点加锁
        if (tree->l_child != NULL) {
            retval = pthread_mutex_lock(&(tree->l_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->parent->lock));
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
        }

        if (tree->r_child != NULL) {
            retval = pthread_mutex_lock(&(tree->r_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->parent->lock));
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
        }

        // 左子树, 右子树均不为空时
        // 删除当前结点, 其右子树接到其左子树的右子树上
        if (tree->l_child != NULL && tree->r_child != NULL) {
            tmp = tree->l_child;
            int r_key = tree->r_child->key;
            while (1) {
                if (r_key < tmp->key) {
                    if (tmp->l_child != NULL) {
                        // 左结点加锁
                        retval = pthread_mutex_lock(&(tmp->l_child->lock));
                        if (retval != 0) {
                            // 忽略解锁错误
                            pthread_mutex_unlock(&(tree->parent->lock));
                            pthread_mutex_unlock(&(tree->lock));
                            pthread_mutex_unlock(&(tree->l_child->lock));
                            pthread_mutex_unlock(&(tree->r_child->lock));
                            if (tmp != tree->l_child && tmp != tree->r_child) {
                                pthread_mutex_unlock(&(tmp->lock));
                            }
                            return retval;
                        }

                        tmp = tmp->l_child;
                    } else {
                        tmp->l_child = tree->r_child;
                        tree->r_child->parent = tmp;
                        break;
                    }
                } else {
                    
                    if (tmp->r_child != NULL){
                        // 给右结点加锁
                        retval = pthread_mutex_lock(&(tmp->r_child->lock));
                        if (retval != 0) {
                            // 忽略解锁错误
                            pthread_mutex_unlock(&(tree->parent->lock));
                            pthread_mutex_unlock(&(tree->lock));
                            pthread_mutex_unlock(&(tree->l_child->lock));
                            pthread_mutex_unlock(&(tree->r_child->lock));
                            if (tmp != tree->l_child && tmp != tree->r_child) {
                                pthread_mutex_unlock(&(tmp->lock));
                            }
                            return retval;
                        }

                        tmp = tmp->r_child;   
                    } else {
                        tmp->r_child = tree->r_child;
                        tree->r_child = tmp;
                        break;
                    }
                }

                // 给当前的父节点解锁
                retval = pthread_mutex_unlock(&(tmp->parent->lock));
                if (retval != 0) {
                    // 忽略解锁错误
                    pthread_mutex_unlock(&(tree->parent->lock));
                    pthread_mutex_unlock(&(tree->lock));
                    if (tmp->parent != tree->l_child)
                        pthread_mutex_unlock(&(tree->l_child->lock));
                    if (tmp->parent != tree->r_child)
                        pthread_mutex_unlock(&(tree->r_child->lock));
                    pthread_mutex_unlock(&(tmp->lock));
                    
                }
            }
        } else if (tree->l_child != NULL) {
            if (tree->parent->l_child == tree) {
                tree->parent->l_child = tree->l_child;
            } else {
                tree->parent->r_child = tree->l_child;
            }
        } else {
            if (tree->parent->l_child == tree) {
                tree->parent->l_child = tree->r_child;
            } else {
                tree->parent->r_child = tree->r_child;
            }
        }

        // 删除结点
        tree->l_child->parent = tree->parent;
        if (tree->parent->l_child == tree)
            tree->parent->l_child = tree->l_child;
        else 
            tree->parent->r_child = tree->l_child;
        
        // 释放锁
        retval = pthread_mutex_unlock(&tree->parent->lock);
        if (retval != 0) {
            // 忽略解锁错误
            pthread_mutex_unlock(&(tree->l_child->lock));
            pthread_mutex_unlock(&(tree->r_child->lock));
            return retval;
        }
        if (tree->l_child != NULL) {
            retval = pthread_mutex_unlock(&(tree->l_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->r_child->lock));
                return retval;
            }
        }

        if (tree->r_child != NULL) {
            retval = pthread_mutex_unlock(&(tree->r_child->lock));
            if (retval != 0)
                return retval;
        }
        
        retval = pthread_mutex_destroy(&(tree->lock));
        if (retval != 0)
            return retval;
        
        free(tree);
    } else {
        // 给右子结点加锁
        if (tree->r_child != NULL) {
            retval = pthread_mutex_lock(&(tree->r_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->parent->lock));
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
            
            retval = delete(tree->r_child, key);
        }

        return 0;
    }

    return retval;
}

/**
 * @brief 查找指定键值指定数值的结点是否存在
 * 
 * @param key 键值
 * @param value 数值
 * @return int 0 表示成功, -1 表示没找到, 正值表示错误码
 */
int lookup(bt_node *tree, int key, int value)
{
    int retval;

    // 根节点默认加锁
    if (tree->parent == NULL) {
        retval = pthread_mutex_lock(&(tree->lock));
        if (retval != 0)
            return retval;
    }

    // 给父节点解锁
    if (tree->parent != NULL) {
        retval = pthread_mutex_unlock(&(tree->lock));
        if (retval != 0) {
            // 忽略解锁错误
            pthread_mutex_unlock(&(tree->lock));
            return retval;
        }
    }

    if (key < tree->key) {
        if (tree->l_child != NULL) {
            // 左子节点加锁
            retval = pthread_mutex_lock(&(tree->l_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
            retval = lookup(tree->l_child, key, value);
            return retval;
        }
        return -1;
    } else if (key > tree->key) {
        if (tree->r_child != NULL) {
            // 右子结点加锁
            retval = pthread_mutex_lock(&(tree->r_child->lock));
            if (retval != 0) {
                // 忽略解锁错误
                pthread_mutex_unlock(&(tree->lock));
                return retval;
            }
            retval = lookup(tree->r_child, key, value);
            return retval;
        }
        return -1;
    } else {
        // 释放自己的所
        retval = pthread_mutex_unlock(&(tree->lock));
        if (retval != 0) {
            return retval;
        }
        return 0;
    }

    return -1;
}