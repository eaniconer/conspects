#include "list.h"
#include <iostream>

bool test_stack_interface() {

	basic::List<int> stack;

	bool test_result = true;

	stack.Push(1);
	stack.Push(2);
	test_result &= (stack.Count() == 2);
	test_result &= (stack.Top() == 2);
	int poped = stack.Pop();
	test_result &= (poped == 2);
	test_result &= (stack.Count() == 1);
	poped = stack.Pop();
	test_result &= (stack.Count() == 0);
	test_result &= (poped = 1);

	return test_result;
}

bool test_remove_all() {
	basic::List<int> lst;
	for(int i = 0; i < 100; i++) lst.Append(i);

	bool test_result = true;
	lst.RemoveAll();
	test_result &= (lst.Count() == 0);
	return test_result;
}

bool test_remove_last() {
	basic::List<int> list;
	list.Append(1);
	list.Prepend(2);
	list.RemoveLast();
	bool test_result = true;
	test_result &= (list.Count() == 1);
	test_result &= (list.Get(0) == 2);
	return test_result;
}

bool test_remove_first() {
	basic::List<int> list;
	list.Append(1);
	list.Prepend(2);
	list.RemoveFirst();
	bool test_result = true;
	test_result &= (list.Count() == 1);
	test_result &= (list.Get(0) == 1);
	return test_result;
}

bool test_remove_existing_item() {
	basic::List<int> list;
	list.Append(2);
	list.Append(3);
	list.Prepend(8);
	list.Remove(2);

	bool test_result = true;
	test_result &= (list.Count() == 2);
	test_result &= (list.Get(0) == 8);
	test_result &= (list.Get(1) == 3);
	return test_result;
}

bool test_remove_non_existing_item() {
	basic::List<int> list;
	list.Append(2);
	list.Append(3);
	list.Prepend(8);
	list.Remove(321);

	bool test_result = true;
	test_result &= (list.Count() == 3);
	test_result &= (list.Get(0) == 8);
	test_result &= (list.Get(1) == 2);
	test_result &= (list.Get(2) == 3);
	return test_result;
}

bool test_size_after_append() 
{
	basic::List<int> list;
	list.Append(5);

	bool test_result = true;
	test_result &= (list.Count() == 1L);

	list.Append(6);
	test_result &= (list.Count() == 2L);

	return test_result;
}

bool test_size_after_prepend() {
	basic::List<int> list;
	list.Prepend(5);

	bool test_result = true;
	test_result &= (list.Count() == 1L);

	list.Prepend(6);
	test_result &= (list.Count() == 2L);

	return test_result;
}

bool test_includes() 
{
	basic::List<int> list;
	for(int i = 0; i < 10; ++i) list.Append(i);
	bool test_result = true;
	
	for(int i = 0; i < 10; ++i) 
	{
		test_result &= list.Includes(i);
		if (test_result == false) {
			std::cout << "\tList does not contain the item " << i << ", but the item was appended earlier" << std::endl;
			return false;
		}
	}
	
	for(int i = 10; i < 20; ++i) 
	{
		test_result &= !list.Includes(i);
		if (test_result == false) {
			std::cout << "\tList contains the item " << i << ", but the item was never appended" << std::endl;
			return false;
		}
	}

	return test_result;	
}

bool test_get_after_append() {
	int array[] = {9, 2, 18, 1, 28, 90, 91};
	basic::List<int> lst;
	for(int val : array) 
	{
		lst.Append(val);
	}
	bool test_result = true;
	for(int i = 0; i < 7; ++i) {
		test_result &= (array[i] == lst.Get(i));
	}
	return test_result;
}
bool test_get_after_prepend() {
	int array[] = {9, 2, 18, 1, 28, 90, 91};
	basic::List<int> lst;
	for(int val : array) 
	{
		lst.Prepend(val);
	}
	bool test_result = true;
	for(int i = 0; i < 7; ++i) {
		test_result &= (array[6-i] == lst.Get(i));
	}
	return test_result;
}

bool test_first() 
{
	basic::List<int> lst;

	bool test_result = true;
	int val = 0;
	lst.Append(val); 	test_result &= (val == lst.First());
	lst.Append(9);		test_result &= (val == lst.First());

	val = 9;
	lst.Prepend(val);	test_result &= (val == lst.First());
	return test_result;
}

bool test_last() 
{
	basic::List<int> lst;

	bool test_result = true;
	int val = 0;
	lst.Append(val); 		test_result &= (val == lst.Last());
	val = 9;
	lst.Append(val);		test_result &= (val == lst.Last());
	lst.Prepend(12);		test_result &= (val == lst.Last());
	return test_result;
}

bool test_ctor_by_default() {
	basic::List<int> lst;
	for(int i = 0; i < 100; i++) {
		lst.Append(i);
	}

	bool test_result = true;
	basic::List<int> c_lst(lst);

	test_result &= (lst.Count() == c_lst.Count());
	if (test_result == false) 
	{
		std::cout << "Wrong count: " <<  std::endl;
		return test_result;
	}

	for(int i = 0; i < lst.Count(); ++i) {
		int a = lst.Get(i);
		int b = c_lst.Get(i);
		test_result &= (a == b);
		if (test_result == false) {
			std::cout << "Wrong value on index " << i << std::endl;
			return false;
		}
	}

	return test_result;
}

void test() {
	std::string res_map[2] = {"NOT PASSED", "Passed"};

	std::cout << "Testing: " << std::endl;
	std::cout << res_map[(int)test_size_after_append()] << " - test_size_after_append" << std::endl;
	std::cout << res_map[(int)test_size_after_prepend()] << " - test_size_after_prepend" << std::endl;
	std::cout << res_map[(int)test_includes()] << " - test_includes" << std::endl;
	std::cout << res_map[(int)test_get_after_append()] << " - test_get_after_append" << std::endl;
	std::cout << res_map[(int)test_get_after_prepend()] << " - test_get_after_prepend" << std::endl;
	std::cout << res_map[(int)test_first()] << " - test_first" << std::endl;
	std::cout << res_map[(int)test_last()] << " - test_last" << std::endl;
	std::cout << res_map[(int)test_remove_existing_item()] << " - test_remove_existing_item" << std::endl;
	std::cout << res_map[(int)test_remove_non_existing_item()] << " - test_remove_non_existing_item" << std::endl;
	std::cout << res_map[(int)test_remove_first()] << " - test_remove_first" << std::endl;
	std::cout << res_map[(int)test_remove_last()] << " - test_remove_last" << std::endl;
	std::cout << res_map[(int)test_remove_all()] << " - test_remove_all" << std::endl;
	std::cout << res_map[(int)test_stack_interface()] << " - test_stack_interface" << std::endl;	
	std::cout << res_map[(int)test_ctor_by_default()] << " - test_ctor_by_default" << std::endl;
}

int main() {
	test();
	return 0;
}