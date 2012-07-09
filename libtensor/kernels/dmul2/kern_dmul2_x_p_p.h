#ifndef LIBTENSOR_KERN_DMUL2_X_P_P_H
#define LIBTENSOR_KERN_DMUL2_X_P_P_H

#include "../kern_dmul2.h"

namespace libtensor {


/** \brief Specialized kernel for \f$ c = c + a_p b_p \f$

	\ingroup libtensor_kernels
 **/
class kern_dmul2_x_p_p : public kernel_base<2, 1> {
	friend class kern_mul_i_ip_p;
	friend class kern_mul_i_p_ip;
	friend class kern_mul_x_pq_qp;

public:
	static const char *k_clazz; //!< Kernel name

private:
	double m_d;
	size_t m_np;
	size_t m_spa, m_spb;

public:
	virtual ~kern_dmul2_x_p_p() { }

	virtual const char *get_name() const {
		return k_clazz;
	}

	virtual void run(const loop_registers<2, 1> &r);

	static kernel_base<2, 1> *match(const kern_dmul2 &z,
		list_t &in, list_t &out);

};


} // namespace libtensor

#endif // LIBTENSOR_KERN_DMUL2_X_P_P_H
