#include <stdlib.h>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include "SortingAlgorithm.h"

using namespace std;

inline unsigned int charGroupOf(char* text, unsigned int position)
{
	return (text[position] > 57 ? (text[position] > 90 ? text[position] - 13 : text[position] - 7) : text[position]) - 48;
}

inline void swap(char** bucket, unsigned int left, unsigned int right)
{
	char* hold = bucket[left];
	bucket[left] = bucket[right];
	bucket[right] = hold;
}

inline unsigned int findSmallestIndex(char** bucket, unsigned int charPosition, unsigned int start, unsigned int end)
{
	unsigned int cur = start;

	for (unsigned int i = start; i <= end; i++)
	{
		if (bucket[i][charPosition] < bucket[cur][charPosition])
		{
			cur = i;
		}
	}

	return cur;
}

inline void insertionSort(char** bucket, unsigned int charPosition, unsigned int start, unsigned int end)
{
	swap(bucket, start, findSmallestIndex(bucket, charPosition, start, end));

	for (unsigned int i = start; i <= end; i++)
	{
		unsigned int cur = i;

		for (unsigned int j = i + 1; j <= end; j++)
		{
			if (bucket[i][charPosition] < bucket[cur][charPosition])
			{
				cur = i;
			}
		}

		if (cur != i)
		{
			swap(bucket, i, cur);
		}
	}
}

inline void radixSort(char*** buckets, unsigned int strLen, unsigned int start, unsigned int end, unsigned int charPosition)
{
	if (start > end)
	{
		return;
	}

	unsigned int total = end - start + 1;
	char** bucket = buckets[strLen - 1];

	if (total == 2)
	{
		if (bucket[start][charPosition] > bucket[end][charPosition])
		{
			swap(bucket, start, end);
		}

		return;
	}
	else if (total < 2)
	{
		return;
	}
	else if (total < 30)
	{
		insertionSort(bucket, charPosition, start, end);

		return;
	}

	char** temp = (char**) malloc(total * sizeof(char*));

	unsigned int readIndices[SortingAlgorithm::TOTAL_ACCEPTABLE_CHARS + 1] = { 0 };
	unsigned int writeIndices[SortingAlgorithm::TOTAL_ACCEPTABLE_CHARS + 1] = { 0 };

	for (unsigned int i = start; i < (total + start); i++)
	{
		unsigned int charGroup = charGroupOf(bucket[i], charPosition);

		readIndices[charGroup + 1]++;
		writeIndices[charGroup + 1]++;
	}

	for (unsigned int i = 1; i <= SortingAlgorithm::TOTAL_ACCEPTABLE_CHARS; i++)
	{
		readIndices[i] += readIndices[i - 1];
		writeIndices[i] += writeIndices[i - 1];
	}

	for (unsigned int i = start; i < (total + start); i++)
	{
		unsigned int charGroup = charGroupOf(bucket[i], charPosition);

		temp[writeIndices[charGroup]++] = bucket[i];
	}

	memcpy(&(bucket[start]), &(temp[0]), total * sizeof(char*));
	free(temp);

	if (++charPosition < strLen)
	{
		for (unsigned int i = 0; i < SortingAlgorithm::TOTAL_ACCEPTABLE_CHARS; i++)
		{
			if (readIndices[i] == readIndices[i + 1])
			{
				continue;
			}

			if ((start + readIndices[i + 1] - 1) > end)
			{
				readIndices[i + 1] = end - start;
			}

			radixSort(buckets, strLen, start + readIndices[i], start + readIndices[i + 1] - 1, charPosition);
		}
	}
}

SortingAlgorithm::SortingAlgorithm(char* inputFile, char* outputFile)
{
	buckets = (char***) malloc(MAX_STRING_LENGTH * sizeof(char**));
	unsigned int bucketCounters[MAX_STRING_LENGTH] = { 0 };

	unsigned int totalStrings = 0;
	unsigned int toSort = 0;

	char* inputBuf = nullptr;

	FILE* input = fopen(inputFile, "rb");
	long fileLength = 0;

	if (input)
	{
		fseek(input, 0, SEEK_END);
		fileLength = ftell(input);

		fseek(input, 0, SEEK_SET);
		inputBuf = (char*) malloc(fileLength * sizeof(char));
		fread(inputBuf, sizeof(char), fileLength, input);
	}

	fclose(input);

	unsigned int ptrStart = 0;
	unsigned int curIndex = 0;
	unsigned int processed = 0;

	{
		bool total = false;
		bool sort = false;

		while (!total || !sort)
		{
			if (inputBuf[curIndex] == '\n' || inputBuf[curIndex] == '\0')
			{
				if (!total)
				{
					total = true;
					totalStrings = strtoul((inputBuf + ptrStart), nullptr, 10);
				}
				else if (!sort)
				{
					sort = true;
					toSort = strtoul((inputBuf + ptrStart), nullptr, 10);
				}
                
				curIndex++;
				ptrStart = curIndex;
                
				continue;
			}
			else
			{
				curIndex++;
			}
		}
	}

	while (processed < totalStrings)
	{
		if (inputBuf[curIndex] == '\n' || inputBuf[curIndex] == '\0')
		{
			char at = inputBuf[curIndex];
			unsigned int strLen = curIndex - ptrStart;

			if (strLen > 0)
			{
				unsigned int& bucketCount = bucketCounters[strLen - 1];

				if (bucketCount == 0)
				{
					buckets[strLen - 1] = (char**) malloc(totalStrings * sizeof(char*));
				}

				buckets[strLen - 1][bucketCount] = &inputBuf[ptrStart];
				buckets[strLen - 1][bucketCount++][strLen] = '\0';
				processed++;
			}

			curIndex++;
			ptrStart = curIndex;

			if (at == '\0')
			{
				break;
			}
		}
		else
		{
			curIndex++;
		}
	}

	unsigned int bytes = 0;
	char* outputBuf = (char*) malloc(fileLength * sizeof(char));

	unsigned int sorted = 0;

	for (unsigned int bucketSort = 0; bucketSort < MAX_STRING_LENGTH; bucketSort++)
	{
		if (bucketCounters[bucketSort] > 0 && sorted < toSort)
		{
			radixSort(buckets, bucketSort + 1, 0, bucketCounters[bucketSort] - 1, 0);

			for (unsigned int inside = 0; inside < bucketCounters[bucketSort]; inside++)
			{
				memcpy(&outputBuf[bytes], &(buckets[bucketSort][inside][0]), bucketSort + 2);
				bytes += (bucketSort + 2);
				outputBuf[bytes - 1] = '\n';
			}

			sorted += bucketCounters[bucketSort];
		}

		free(buckets[bucketSort]);
	}

	free(buckets);
	free(inputBuf);

	FILE* output = fopen(outputFile, "wb");
	fwrite(&outputBuf[0], 1, bytes - 1, output);
	fclose(output);
	free(outputBuf);
}
