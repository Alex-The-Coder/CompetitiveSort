#ifndef SORTINGALGORITHM_H_
#define SORTINGALGORITHM_H_

class SortingAlgorithm
{
private:
	char*** buckets;
public:
	static const unsigned int MAX_STRING_LENGTH = 30;
	static const unsigned int TOTAL_ACCEPTABLE_CHARS = 62;

	SortingAlgorithm(char*, char*);
};

#endif /* SORTINGALGORITHM_H_ */
