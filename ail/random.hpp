#pragma once

#include <ail/exception.hpp>
#include <ail/types.hpp>

#include <boost/random.hpp>

namespace ail
{
	typedef boost::mt19937 random_algorithm_type;

	template <typename integer_type>
		integer_type random_integer(integer_type minimum, integer_type maximum)
	{
		seed_check();

		typedef boost::uniform_int<integer_type> distribution_type;
		
		distribution_type distribution(minimum, maximum);
		boost::variate_generator<random_algorithm_type &, distribution_type> generator(prng_algorithm, distribution);

		return generator();
	}

	template <class element_type, std::size_t array_size>
		element_type & random_pick(element_type (&array)[array_size])
	{
		std::size_t index = random_integer<std::size_t>(0, array_size - 1);
		return array[index];
	}

	template <class type>
		class random_scale
	{
	public:
		random_scale():
			total_weight(0)
		{
		}

		void add(type object, ulong weight)
		{
			if(weight == 0)
				throw exception("Element weight can't be zero");
			elements.push_back(element(object, total_weight, total_weight + weight - 1));
			total_weight += weight;
		}

		type random()
		{
			return make_choice()->object;
		}

	private:

		struct element
		{
			type object;

			ulong minimum;
			ulong maximum;

			element()
			{
			}

			element(type object, ulong minimum, ulong maximum):
				object(object),
				minimum(minimum),
				maximum(maximum)
			{
			}

			bool operator==(ulong input) const
			{
				return input >= minimum && input <= maximum;
			}
		};

		std::vector<element> elements;
		ulong total_weight;

		typename std::vector<element>::iterator make_choice()
		{
			if(total_weight == 0)
				throw exception("Random scale object doesn't contain any elements yet");
			ulong choice = random::uint(0, total_weight - 1);
			typename std::vector<element>::iterator iterator = std::find(elements.begin(), elements.end(), choice);
			if(iterator == elements.end())
				throw exception("Unable to match weight, code must be broken");
			return iterator;
		}
	};

	void seed_check();

	extern random_algorithm_type prng_algorithm;
	extern bool prng_has_been_seeded;
}
