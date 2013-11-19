#ifndef BLOCK_KERNELS_TEST_H
#define BLOCK_KERNELS_TEST_H

#include <libtest/unit_test.h>

namespace libtensor {

class block_kernels_test : public libtest::unit_test { 
public:
    virtual void perform() throw(libtest::test_exception);
private: 

    /*
     * block_printer tests
     */
    void test_block_print_kernel_2d() throw(libtest::test_exception);
    void test_block_print_kernel_3d() throw(libtest::test_exception);

    /*
     * block_permute_kernel tests
     */
    void test_block_permute_kernel_2d() throw(libtest::test_exception);
    void test_block_permute_kernel_3d_120() throw(libtest::test_exception);
    void test_block_permute_kernel_3d_021() throw(libtest::test_exception);

    /*
     * block_contract_kernel tests
     */
    //Constructor tests
    void test_block_contract2_kernel_2d_not_enough_loops() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_not_enough_bispaces() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_C_missing_idx() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_C_extra_idx() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_no_contracted_inds() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_strided_output() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_A_ikj() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_A_jik() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_A_trans_kjli() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_A_trans_klji() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_B_jlk() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_B_jkml() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_B_trans_kjl() throw(libtest::test_exception);
    void test_block_contract2_kernel_non_matmul_B_trans_lkj() throw(libtest::test_exception);

    //Running tests
    void test_block_contract2_kernel_2d_not_enough_dims_and_ptrs() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_invalid_dims() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_incompatible_dims() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_ik_kj() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_ik_jk() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_ki_kj() throw(libtest::test_exception);
    void test_block_contract2_kernel_2d_ki_jk() throw(libtest::test_exception);

    void test_block_contract2_kernel_3d_2d() throw(libtest::test_exception);
    void test_block_contract2_kernel_3d_3d_multi_index() throw(libtest::test_exception);

    /*
     * block_subtract_kernel tests
     */
    void test_block_subtract_kernel_2d_2d() throw(libtest::test_exception);

    /*
     * direct_kernel tests
     */
    void test_direct_block_subtract_kernel_2d_2d() throw(libtest::test_exception);
};


} // namespace libtensor

#endif /* BLOCK_KERNELS_TEST_H */
