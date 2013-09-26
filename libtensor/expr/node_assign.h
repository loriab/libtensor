#ifndef LIBTENSOR_EXPR_NODE_ASSIGN_H
#define LIBTENSOR_EXPR_NODE_ASSIGN_H

#include "node.h"

namespace libtensor {
namespace expr {


/** \brief Assignment node of the expression tree

    \ingroup libtensor_expr
 **/
class node_assign : public node {
private:
    unsigned m_tid; //!< Tensor ID
    const node &m_rhs; //!< Operation to be assigned

public:
    /** \brief Creates an assignment node
     **/
    node_assign(unsigned tid, const node &rhs) :
        node("assign"), m_tid(tid), m_rhs(rhs)
    { }

    /** \brief Virtual destructor
     **/
    virtual ~node_assign() { }

    /** \brief Creates a copy of the node via new
     **/
    virtual node *clone() const {
        return new node_assign(*this);
    }

    /** \brief Returns the right-hand side of the assignment
     **/
    const node &get_rhs() const {
        return m_rhs;
    }

};


} // namespace expr
} // namespace libtensor

#endif // LIBTENSOR_EXPR_NODE_ASSIGN_H
