#ifndef LIBTENSOR_EVAL_PLAN_TEST_H
#define LIBTENSOR_EVAL_PLAN_TEST_H

#include <libtest/unit_test.h>

namespace libtensor {


/** \brief Tests the libtensor::eval_plan class

    \ingroup libtensor_tests_expr
**/
class eval_plan_test : public libtest::unit_test {
public:
    virtual void perform() throw(libtest::test_exception);

private:
    void test_1();

};


} // namespace libtensor

#endif // LIBTENSOR_EVAL_PLAN_TEST_H

