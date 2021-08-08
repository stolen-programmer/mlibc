
#include <log/log.h>
#include <scope/node.h>
#include <scope/scope.h>

#include <assert.h>
#include <malloc.h>

Scope scope_global = NULL, scope_current = NULL;

/**
 * 和parent解除连接
 * @param s 当前域
 */
void scope_parent_unlink(Scope s) {
    if (s->parent == NULL)
        return;

    s->parent->children = NULL;
    s->parent = NULL;
}

bool scope_is_empty(Scope s) {
    if (s->head == NULL && s->tail == NULL)
        return true;

    return false;

}

/**
 * 在域内追加节点
 * @param s 域
 * @param node 节点
 */
void scope_append_node(Scope s, ScopeNode node) {
    check_null(s,)
    check_null(node,)

    if (scope_is_empty(s)) {
        s->head = s->tail = node;
    } else {
        scope_node_link(node, s->tail);
        s->tail = node;
    }
}

/**
 * 非托管内存分配
 * @param scope 域
 * @param size 大小
 * @return
 */
void *scope_mem_alloc(size_t size) {

    check_log(size > 0, "size <= 0", NULL)

    void *mem = malloc(size);
    assert(mem != NULL);

#ifdef SCOPE_DEBUG
    printf("%p to mem alloc\n", mem);
#endif

    return mem;

}

/**
 * 托管分配
 * @param size 大小
 * @return
 */
void *scope_alloc(Scope s, size_t size, void (*r)(void *)) {

    check_null(s, NULL)
    check_log(size > 0, "size <= 0", NULL)

    ScopeNode node = scope_node_new(s, size, r);

    scope_append_node(s, node);

    return node->mem;
}

/**
 * 直接内存释放
 * @param mem
 * @return
 */
bool scope_mem_free(void *mem) {

    check_null(mem, false)

#ifdef SCOPE_DEBUG
    printf("%p to mem free\n", mem);
#endif

    return true;

}

/**
 * 在域内查找内存块
 * @param s 域
 * @param mem 内存块
 * @return 成功返回对应节点 失败返回NULL
 */
ScopeNode scope_find(Scope s, void *mem) {

    ScopeNode node = NULL;

    for (node = s->head; node != NULL; node = node->next) {

        if (node->mem == mem) {
            break;
        }
    }

    return node;
}

/**
 * 从当前域内断开节点的连接
 * @param s 域
 * @param node 节点
 */
void scope_unlink_node(Scope s, ScopeNode node) {
    check_null(s,)
    check_null(node,)

    if (node->prev != NULL)
        node->prev->next = node->next;
    else // first
        s->head = node->next;

    if (node->next != NULL)
        node->next->prev = node->prev;
    else // last
        s->tail = node->prev;

    scope_node_unlink(node);

}

/**
 * 从域释放内存
 * @param s 域
 * @param mem 内存块
 * @return
 */
bool scope_free(Scope s, void *mem) {
    // find
    ScopeNode node = scope_find(s, mem);
    // unlink
    scope_unlink_node(s, node);
    // SRecycle
    return scope_node_cycle(node);
}

/**
 * 根据正负选择域
 * @param s 当前域
 * @param index 正负值
 * @return + to parent - to child
 */
Scope scope_choose(Scope s, int index) {
    if (s == NULL)
        return NULL;

    if (index == 0)
        return NULL;

    if (index > 0)
        return s->parent;

    if (index < 0)
        return s->children;

    return NULL;
}

/**
 * 切换域
 * @param s 域
 * @param level 层级 + to parent - to child
 * @return 目标域
 */
Scope scope_switch_scope(Scope s, int level) {
    if (level == 0)
        return s;

    int index = level;

    for (Scope result = s; result != NULL;) {

        if (index == 0)
            return result;

        result = scope_choose(result, index);
        index = (index > 0) ? index - 1 : index + 1;
    }

    return NULL;
}

/**
 * 节点从本域移动到目标域
 * @param self 本域
 * @param target 目标域
 * @param mem 内存块
 * @return
 */
bool scope_move(Scope self, Scope target, void *mem) {
    ScopeNode node = scope_find(self, mem);
    if (node == NULL)
        return false;
    scope_unlink_node(self, node);

    scope_append_node(target, node);

    return true;
}

/**
 * 切换到parent 没有parent 返回global
 * @param s 当前域
 * @return parent or global
 */
Scope scope_switch_to_parent(Scope s) {
    Scope current = s->switchScope(s, 1);
    return current == NULL ? scope_global : current;
}

/**
 * 回收当前域的子域  如果有
 * @param s 当前域
 */
void scope_recycle_children(Scope s) {
    if (s->children != NULL) {
        s->children->recycle(s->children);
    }
}

/**
 * 回收当前域内节点
 * @param s 域
 * @param root 根节点
 */
void scope_recycle_node(Scope s, ScopeNode root) {

    check_null(s,)

    if (root == NULL)
        return;

    // SRecycle next root
    scope_recycle_node(s, root->next);

    // root SRecycle
    scope_node_cycle(root);

}

/**
 * 回收当前域
 * @param s 当前域
 */
void scope_recycle(Scope s) {
    scope_current = scope_switch_to_parent(s);

    scope_parent_unlink(s);

    scope_recycle_children(s);

    scope_recycle_node(s, s->head);

    scope_mem_free(s);
}

/**
 * 初始化函数指针
 * @param target 待初始化域
 */
void initialize_func(Scope target) {
    target->alloc = scope_alloc;
    target->free = scope_free;
    target->switchScope = scope_switch_scope;
    target->move = scope_move;
    target->recycle = scope_recycle;
}

void scope_init_global() {

    check_log(scope_global == NULL, "global scope multi initialize",)

    Scope result = malloc(sizeof(scope));
#ifdef SCOPE_DEBUG
    printf("%p to alloc\n", result);
#endif

    result->parent = result->children = NULL;
    result->head = result->tail = NULL;

    initialize_func(result);

    scope_global = scope_current = result;

}

void scope_init_locale() {
    Scope current = scope_current;
    scope_current = malloc(sizeof(scope));
#ifdef SCOPE_DEBUG
    printf("%p to alloc\n", scope_current);
#endif

    scope_current->parent = current;
    scope_current->parent->children = scope_current;
    scope_current->head = scope_current->tail = NULL;
    scope_current->children = NULL;

    initialize_func(scope_current);

}
