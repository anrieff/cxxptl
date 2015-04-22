/***************************************************************************
 *   Copyright (C) 2006, 2014 by Veselin Georgiev                          *
 *   anrieff@mxgail.com (change to gmail)                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "cxxptl.h"
#include <math.h>
#include <stdio.h>
#include <time.h>

ThreadPool pool;

const int MAX_THREADS_TO_RUN = 8;

struct DoWork: public Parallel {
	Barrier barrier;

	double data[MAX_THREADS_TO_RUN];

	DoWork(int numWorkerThreads) : barrier(numWorkerThreads)
	{}

	// this method will be executed on up to 8 threads, depending on the
	// second parameter to pool.run(). In the case of 8 threads, each of
	// them will be invoked with a different threadIdx (ranging 0..7), while
	// numWorkerThreads will be 8.
	void entry(int threadIdx, int numWorkerThreads)
	{
		double partialSum = 0;
		printf("DoWork::entry(%d, %d)\n", threadIdx, numWorkerThreads); fflush(stdout);
		int i;
		for (i = threadIdx; i < 500000000; i += numWorkerThreads) {
			if (!i) continue;
			double d = i;
			partialSum += 1.0 / (d * d);
		}
		printf("Thread %d: waiting for others...\n", threadIdx); fflush(stdout);
		barrier.checkout();
		printf("Thread %d: continuing\n", threadIdx); fflush(stdout);
		for (; i < 1000000000; i += numWorkerThreads) {
			double d = i;
			partialSum += 1.0 / (d*d);
		}
		data[threadIdx] = partialSum;
	}
};

int main(void)
{
	pool.preload_threads(8); // reserve 8 threads for work
	printf("This PC has %d processors\n", get_processor_count());
	for (int numThreads = 1; numThreads <= MAX_THREADS_TO_RUN; numThreads *= 2) {
		printf("Using %d processors for internal calculations.\n", numThreads);
		DoWork dowork(numThreads);
		time_t xt = time(NULL);
		pool.run(&dowork, numThreads);
		xt = time(NULL) - xt;
		
		double sum = 0;
		for (int i = 0; i < numThreads; i++) {
			printf("Processor %d gave %.5lf as result\n", i, dowork.data[i]);
			sum += dowork.data[i];
		}
		printf("pi = %.9lf\n", sqrt(sum * 6.0));
		printf("Result produced in %u sec\n", (unsigned)xt);
	}
	return 0;
}
