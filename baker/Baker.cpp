#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
#include "../includes/box.h"
using namespace std;

mutex m;
Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

void Baker::bake_and_box(ORDER &anOrder) {
	int numDonuts = anOrder.number_donuts;
	Box box;
	DONUT donut;
	for (int i = 0; i < numDonuts; i++){
		if (box.size() < DOZEN){
			box.addDonut(donut);
		}
		if (box.size() == DOZEN){
			anOrder.boxes.push_back(box);
			box.clear();
		}
	}
	if (box.size() > 0){
		anOrder.boxes.push_back(box);
	}
}

void Baker::beBaker() {
	{
		unique_lock<mutex> lck(mutex_order_outQ);
		while (!b_WaiterIsFinished){
			cv_order_inQ.wait(lck);
		}
	}
	while (true){
		unique_lock<mutex> lck(mutex_order_outQ);
		if (b_WaiterIsFinished){
			break;
		}
		if (order_in_Q.empty() && !b_WaiterIsFinished){
			cv_order_inQ.wait(lck);
		}
		if (order_in_Q.size() > 0){
			ORDER order = order_in_Q.front();
			bake_and_box(order);
			order_out_Vector.push_back(order);
			order_in_Q.pop();

		}
	}
}
