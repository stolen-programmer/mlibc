
#pragma once

#include <stddef.h>

#include <primitive/bool.h>
#include <recycle/recycle.h>

typedef struct _scope *Scope;
typedef struct _scope_node *ScopeNode;

typedef struct _scope {

    Scope parent;
    Scope children;

    ScopeNode head;
    ScopeNode tail;

    /**
     * 分配空间
     * @param scope 域
     * @param size 大小
     * @param r 回收接口
     *
     * @return 分配的空间
     */
    void *(*alloc)(Scope scope, size_t size, void (*r)(void *ptr));

    /**
     * 释放
     * @param mem
     * @return
     */
    bool (*free)(Scope, void *mem);

    /**
     *
     * @param level +1 to parent -1 to child
     * @return
     */
    Scope (*switchScope)(Scope, int level);

    bool (*move)(Scope, Scope target, void *mem);

    void (*recycle)(Scope);
} scope;

typedef struct _scope_node {
    ScopeNode prev;
    ScopeNode next;

    Recycle recycle;
    void* mem;
    size_t size;

} scope_node;


extern Scope scope_global, scope_current;

/**
 * 初始化全局作用域  记得释放 SRecycle
 */
void scope_init_global();

/**
 * 初始化局部作用域 记得释放
 */
void scope_init_locale();

