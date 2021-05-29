#include <gtest/gtest.h>
#include <antity/utility/Iterator.h>
#include <antity/utility/VectorSequence.h>

struct Integers
{
	using IteratorInt = Iterator<int>;
	
	IteratorInt begin() { return Iterator(&m_data[0]); }
	IteratorInt end() { return Iterator(&m_data[200]); }
	
	int m_data[200];
};

struct MultiVector
{

	using IteratorInt = Iterator<int>;

	using MultiIteratorInt = MultiIterator<IteratorInt>;

	MultiVector(std::vector<Integers*> data) : data(data)
	{
		for (auto vec : data)
		{
			b.push_back(vec->begin());
			e.push_back(vec->end());
		}
	}
	
	MultiIteratorInt begin() { return MultiIteratorInt{b.front(),&b,&e}; }
	MultiIteratorInt end() { return MultiIteratorInt{e.back(),&b,&e,1}; }

	std::vector<Integers*> data;
	std::vector<IteratorInt> b;
	std::vector<IteratorInt> e;
};


TEST(Iterators, Iterator)
{
	Integers integers;
	std::fill(integers.begin(), integers.end(), 3);
	int counter = 0;
	for (auto i : integers)
	{
		counter++;
		ASSERT_EQ(i,3);
	}
	ASSERT_EQ(counter, 200);
}

TEST(Iterators, MultiIterator)
{
	Integers integers1;
	int a=0;
	Integers integers2;
	std::fill(integers1.begin(), integers1.end(), 43);
	std::fill(integers2.begin(), integers2.end(), 3);
	MultiVector mult = MultiVector{ std::vector<Integers*>{&integers1,& integers2} };
	ASSERT_EQ(mult.b.size(), 2);
	int counter = 0;
	for (auto i = mult.begin();i != mult.end();i++)
	{
		counter++;

	}
	ASSERT_EQ(counter, 400);


}

/*
TEST(Iterators, VectorSequence)
{
	std::vector<int> a = { 1,2,3 };
	std::vector<int> b = { 4,5,6 };
	std::vector<int> c = { 7,8,9 };

	int counter = 0;
	for (auto i : ant::makeVectorSequence<int>(a,b,c))
	{
		counter++;

	}
	ASSERT_EQ(counter, 9);


}
*/
