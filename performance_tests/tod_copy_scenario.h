#ifndef TOD_COPY_SCENARIO_H
#define TOD_COPY_SCENARIO_H

#include <libtest.h>
#include "tod_copy_performance.h"
#include "performance_test_scenario_i.h"

using libtest::unit_test_factory;

namespace libtensor {

/**	\brief Performance test scenario for the libtensor::tod_add class
 
 	\param N dimensions of the tensors to be added
 	\param X size of the tensors 

	The size of the tensors by function dimA() of the X object.

 	\ingroup libtensor_performance_tests
**/
template<size_t Repeats,size_t N, typename X>  
class tod_copy_scenario
	: public performance_test_scenario_i 
{	
	unit_test_factory<tod_copy_ref<Repeats,X> > m_ref;
	unit_test_factory<tod_copy_p1<Repeats,N,X> > m_pt1;
	unit_test_factory<tod_copy_p2<Repeats,N,X> > m_pt2;
	
public:
	tod_copy_scenario();
	virtual ~tod_copy_scenario() {}
};


template<size_t Repeats,size_t N, typename X>  
tod_copy_scenario<Repeats,N,X>::tod_copy_scenario()
{  
	add_test("reference",m_ref); 
	add_test("test 1",m_pt1);
	add_test("test 2",m_pt2);
}

} // namespace libtensor

#endif // TOD_COPY_SCENARIO_H

