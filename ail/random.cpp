#include <ail/random.hpp>
#include <ail/time.hpp>

namespace ail
{
	random_algorithm_type prng_algorithm;
	bool prng_has_been_seeded = false;

	void seed_check()
	{
		if(!prng_has_been_seeded)
		{
			prng_algorithm.seed(static_cast<boost::uint32_t>(milliseconds()));
			prng_has_been_seeded = true;
		}
	}
}
