#ifndef _tools_timed_hierarchic_log_hpp_INCLUDED_
#define _tools_timed_hierarchic_log_hpp_INCLUDED_

#include "timed_log.hpp"
#include "hierarchic_log.hpp"


namespace tools{


	struct timed_hierarchic_log: timed_log_base, hierarchic_log_base, log_base{
		using hierarchic_log_base::first;

		void prefix(std::ostringstream& os)const{
			timed_log_base::extended_prefix(log_base::has_body, os);
			hierarchic_log_base::prefix(os);
		}
	};


}


#endif
