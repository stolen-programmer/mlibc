#include <log/log.h>
#include <scope/node.h>
#include <scope/scope.h>

#include <malloc.h>

/**
 * 非托管内存分配
 * @param scope 域
 * @param size 大小
 * @return
 */
void *scope_mem_alloc(size_t size);

/**
 * 直接内存释放
 * @param mem
 * @return
 */
bool scope_mem_free(void *mem);

/**
 * 构造新节点
 * @param scope 域
 * @param size 大小
 * @return
 */
ScopeNode scope_node_new(Scope s, size_t size, void (*r)(void *)) {

    check_null(s,  NULL)
    check_log(size > 0, "size <= 0", NULL)

    ScopeNode node = scope_mem_alloc(sizeof(scope_node));
    void *mem = scope_mem_alloc(sizeof(SRecycle));
    void *ptr = scope_mem_alloc(size);

    Recycle recycle = recycle_init_from(mem, ptr);
    recycle->recycle = r;

    node->prev = node->next = NULL;
    node->mem = ptr;
    node->recycle = recycle;
    node->size = size;

    return node;
}


/**
 * 连接两个节点 target <-> node
 * @param node 当前节点
 * @param target 目标节点
 */
void scope_node_link(ScopeNode node, ScopeNode target) {

    check_null(node,)
    check_null(target,)

    target->next = node;
    node->prev = target;

}


/**
 * 断开节点的连接
 * @param node
 */
void scope_node_unlink(ScopeNode node) {
    check_null(node,)

    node->next = node->prev = NULL;

}

/**
 * 单节点回收
 * @param node
 * @return
 */
bool scope_node_cycle(ScopeNode node) {

    check_null(node, false)

    if (node->recycle->recycle != NULL)
        node->recycle->recycle(node->recycle);

    scope_mem_free(node->mem);
    scope_mem_free(node->recycle);
    scope_mem_free(node);

    return true;

}
