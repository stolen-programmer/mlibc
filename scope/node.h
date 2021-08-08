
#pragma once

#include <scope/scope.h>

/**
 * 单节点回收
 * @param node
 * @return
 */
bool scope_node_cycle(ScopeNode node);

/**
 * 单节点回收
 * @param node
 * @return
 */
bool scope_node_cycle(ScopeNode node);

/**
 * 断开节点的连接
 * @param node
 */
void scope_node_unlink(ScopeNode node);


/**
 * 构造新节点
 * @param s 域
 * @param size 大小
 * @return
 */
ScopeNode scope_node_new(Scope s, size_t size, void (*pFunction)(void *));

/**
 * 连接两个节点 target <-> node
 * @param node 当前节点
 * @param target 目标节点
 */
void scope_node_link(ScopeNode node, ScopeNode target);

